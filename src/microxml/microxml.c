/*
 * libmicroxml - Micro cross-platform library for XML validation and signature.
 *
 * Copyright (c) 2017 Silvio Clecio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CONFIG_H
#define CONFIG_H
#include <config.h>
#endif
#include <string.h>
#include <xmlsec/base64.h>
#include <xmlsec/crypto.h>
#include <libxml/xmlschemas.h>
#include <libxml/xmlsave.h>
#include <xmlsec/templates.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <microutils.h>
#include <microxml.h>

#define _(String) (String)

#ifndef _MXML_THROW
#define _MXML_THROW(cb, cls, ...) \
do { \
    char *err; \
    char msg[MXML_ERR_SIZE]; \
    if (cb) { \
        err = mu_fmt(__VA_ARGS__); \
        strcpy(msg, err); \
        free(err); \
        cb(cls, msg); \
    } else \
        fprintf(stderr, __VA_ARGS__); \
} while (0)
#endif

struct mxml_xml_cfg {
    xmlSchemaParserCtxtPtr parser;
    xmlSchema *schema;
    xmlSchemaValidCtxt *ctxt;
    mxml_err_cb error_cb;
    mxml_err_cb warn_cb;
    void *error_warn_cls;
    bool ok;
};

const char *mxml_version() {
    return VERSION;
}

void _mxml_xml_ensure_id(xmlNodePtr node, const xmlChar *name, xmlChar **id) {
    xmlAttrPtr id_attr;
    *id = xmlGetProp(node, name);
    id_attr = xmlGetID(node->doc, *id);
    if (!id_attr) {
        for (id_attr = node->properties; id_attr; id_attr = id_attr->next)
            if (xmlStrEqual(id_attr->name, name)) {
                free(*id);
                break;
            }
        if (id_attr) {
            *id = xmlNodeListGetString(node->doc, id_attr->children, 1);
            xmlAddID(NULL, node->doc, *id, id_attr);
        }
    }
}

void _mxml_xml_remove_blank_nodes(xmlNodePtr node) {
    xmlNodePtr cur;
    xmlNodePtr next;
    if (!node)
        return;
    cur = node->children;
    while (cur) {
        next = cur->next;
        if (xmlIsBlankNode(cur)) {
            xmlUnlinkNode(cur);
            xmlFreeNode(cur);
        } else
            _mxml_xml_remove_blank_nodes(cur);
        cur = next;
    }
}

static xmlNodePtr _mxml_xml_dom_get_dom1_attribute(xmlNodePtr elem, xmlChar *name) {
    int len;
    const xmlChar *nqname;
    xmlNsPtr ns;
    xmlChar *prefix;
    nqname = xmlSplitQName3(name, &len);
    if (nqname) {
        prefix = xmlStrndup(name, len);
        if (prefix && xmlStrEqual(prefix, (xmlChar *) "xmlns")) {
            ns = elem->nsDef;
            while (ns && !xmlStrEqual(ns->prefix, nqname))
                ns = ns->next;
            xmlFree(prefix);
            return (xmlNodePtr) ns;
        }
        ns = xmlSearchNs(elem->doc, elem, prefix);
        if (prefix)
            xmlFree(prefix);
        if (ns)
            return (xmlNodePtr) xmlHasNsProp(elem, nqname, ns->href);
    } else if (xmlStrEqual(name, (xmlChar *) "xmlns")) {
        xmlNsPtr nsPtr = elem->nsDef;
        while (nsPtr) {
            if (!nsPtr->prefix)
                return (xmlNodePtr) nsPtr;
            nsPtr = nsPtr->next;
        }
        return NULL;
    }
    return (xmlNodePtr) xmlHasNsProp(elem, name, NULL);
}

static void _mxml_xml_err_cb(void *cls, const char *fmt, ...) {
    struct mxml_xml_cfg *cfg = cls;
    char *err;
    char msg[MXML_ERR_SIZE];
    if (cfg->error_cb) {
        err = NULL;
        va_list va;
        va_start(va, fmt);
        if (vasprintf(&err, fmt, va) == -1)
            memset(msg, 0, sizeof(msg));
        else
            memcpy(msg, err, sizeof(msg));
        free(err);
        va_end(va);
        cfg->error_cb(cfg->error_warn_cls, msg);
    }
}

static void _mxml_xml_warn_cb(void *cls, const char *fmt, ...) {
    struct mxml_xml_cfg *cfg = cls;
    char *warn;
    char msg[MXML_ERR_SIZE];
    if (cfg->warn_cb) {
        warn = NULL;
        va_list va;
        va_start(va, fmt);
        if (vasprintf(&warn, fmt, va) == -1)
            memset(msg, 0, sizeof(msg));
        else
            strcpy(msg, warn);
        free(warn);
        va_end(va);
        cfg->warn_cb(cfg->error_warn_cls, msg);
    }
}

void *_mxml_xml_node_dom_get_ns(void *nodep, const char *uri, int *errorcode, const char *prefix) {
    xmlNsPtr nsptr = NULL;
    *errorcode = 0;
#define _DOM_XMLNS_NAMESPACE "http://www.w3.org/2000/xmlns/"
    if (!((prefix && !strcmp(prefix, "xml") && strcmp(uri, (char *) XML_XML_NAMESPACE)) ||
          (prefix && !strcmp(prefix, "xmlns") && strcmp(uri, _DOM_XMLNS_NAMESPACE)) ||
          (prefix && !strcmp(uri, _DOM_XMLNS_NAMESPACE) && strcmp(prefix, "xmlns")))) {
        nsptr = xmlNewNs(nodep, (xmlChar *) uri, (xmlChar *) prefix);
    }
#undef _DOM_XMLNS_NAMESPACE
    if (!nsptr)
        *errorcode = 14; /* NAMESPACE_ERR */
    return nsptr;
}

// XML

void mxml_xml_init() {
    xmlInitParser();
}

void mxml_xml_finit() {
    xmlCleanupParser();
    xmlMemoryDump();
}

struct mxml_xml_cfg *mxml_xml_cfg_new(const char *xsd_uri,
                                      mxml_err_cb error_cb, mxml_err_cb warn_cb, void *error_warn_cls) {
    struct mxml_xml_cfg *cfg;
    if (!(cfg = calloc(1, sizeof(struct mxml_xml_cfg))))
        return NULL;
    cfg->error_cb = error_cb;
    cfg->warn_cb = warn_cb;
    cfg->error_warn_cls = error_warn_cls;
    xmlSetGenericErrorFunc(cfg, &_mxml_xml_err_cb);
    cfg->parser = xmlSchemaNewParserCtxt(xsd_uri);
    if (!cfg->parser) {
        free(cfg);
        _MXML_THROW(error_cb, error_warn_cls, _("Failed to create XSD parser\n"));
        return NULL;
    }
    xmlSchemaSetParserErrors(cfg->parser, &_mxml_xml_err_cb, &_mxml_xml_warn_cb, cfg);
    cfg->schema = xmlSchemaParse(cfg->parser);
    if (!cfg->schema) {
        mxml_xml_cfg_free(cfg);
        _MXML_THROW(error_cb, error_warn_cls, _("Failed to parse XSD schema\n"));
        return NULL;
    }
    cfg->ctxt = xmlSchemaNewValidCtxt(cfg->schema);
    if (!cfg->ctxt) {
        mxml_xml_cfg_free(cfg);
        _MXML_THROW(error_cb, error_warn_cls, "%s", _("Failed to validade XSD schema\n"));
        return NULL;
    }
    xmlSchemaSetValidErrors(cfg->ctxt, &_mxml_xml_err_cb, &_mxml_xml_warn_cb, cfg);
    cfg->ok = true;
    return cfg;
}

void mxml_xml_cfg_free(struct mxml_xml_cfg *cfg) {
    if (cfg) {
        if (cfg->ctxt)
            xmlSchemaFreeValidCtxt(cfg->ctxt);
        if (cfg->schema)
            xmlSchemaFree(cfg->schema);
        if (cfg->parser)
            xmlSchemaFreeParserCtxt(cfg->parser);
        free(cfg);
    }
}

bool mxml_xml_cfg_ok(struct mxml_xml_cfg *cfg) {
    return cfg && cfg->ok;
}

bool mxml_xml_validate_file(struct mxml_xml_cfg *cfg, const char *filename, mxml_err_cb error_cb, void *error_cls) {
    if (!cfg) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), "cfg");
        return false;
    }
    if (!cfg->ok) {
        _MXML_THROW(error_cb, error_cls, _("Invalid configuration\n"));
        return false;
    }
    if (!filename) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), "filename");
        return false;
    }
    if (!mu_exists(filename)) {
        _MXML_THROW(error_cb, error_cls, _("File not found: %s\n"), filename);
        return false;
    }
    return xmlSchemaValidateFile(cfg->ctxt, filename, 0) == 0;
}

// XML utils

void *mxml_xml_doc_create_element(void *doc, const char *node_name, const char *node_value) {
    xmlChar *name;
    if (!doc || !node_name)
        return NULL;
    name = BAD_CAST node_name;
    if (xmlValidateName(name, 0) != 0)
        return NULL;
    if (mu_is_empty(node_value))
        return xmlNewDocRawNode(doc, NULL, name, BAD_CAST "");
    else
        return xmlNewDocNode(doc, NULL, name, BAD_CAST node_value);
}

void *mxml_xml_doc_create_empty_element(void *doc, const char *node_name) {
    return mxml_xml_doc_create_element(doc, node_name, NULL);
}

void *mxml_xml_doc_create_attribute(void *doc, const char *attr_name) {
    xmlChar *name;
    if (!doc || !attr_name)
        return NULL;
    name = BAD_CAST attr_name;
    if (xmlValidateName(name, 0) != 0)
        return NULL;
    return xmlNewDocProp(doc, name, NULL);
}

void *mxml_xml_doc_create_text_node(void *doc, const char *content) {
    if (!doc)
        return NULL;
    return xmlNewDocText(doc, BAD_CAST content);
}

void *mxml_xml_node_append_child(void *parent_node, void *new_node) {
    if (!new_node)
        return NULL;
    if (!parent_node) {
        xmlFreeNode(new_node);
        return NULL;
    }
    return xmlAddChild(parent_node, new_node);
}

void *mxml_xml_doc_create_cdata_section(void *doc, const char *value, int value_length) {
    int len;
    if (!doc || mu_is_empty(value))
        return NULL;
    if (value_length < 1)
        len = (int) strlen(value);
    else
        len = value_length;
    if (len < 1)
        return NULL;
    return xmlNewCDataBlock(doc, BAD_CAST value, len);
}

bool mxml_xml_node_children_valid(void *node) {
    if (!node)
        return false;
    switch (((xmlNodePtr) node)->type) {
        case XML_DOCUMENT_TYPE_NODE:
        case XML_DTD_NODE:
        case XML_PI_NODE:
        case XML_COMMENT_NODE:
        case XML_TEXT_NODE:
        case XML_CDATA_SECTION_NODE:
        case XML_NOTATION_NODE:
            return false;
        default:
            return true;
    }
}

bool mxml_xml_node_is_read_only(void *node) {
    xmlNodePtr nd;
    if (!node)
        return false;
    nd = node;
    switch (nd->type) {
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
        case XML_NAMESPACE_DECL:
            return true;
        default:
            return !nd->doc;
    }
}

void *mxml_xml_node_find_node_by_name(void *root_node, const char *node_name) {
    xmlNodePtr node;
    xmlNodePtr int_node;
    if (!root_node || !node_name)
        return NULL;
    node = root_node;
    while (node) {
        if (!xmlStrcmp(node->name, BAD_CAST node_name))
            return node;
        else if (node->children && (int_node = mxml_xml_node_find_node_by_name(node->children, node_name)))
            return int_node;
        node = node->next;
    }
    return NULL;
}

void *mxml_xml_doc_find_node_by_name(void *doc, const char *node_name) {
    return mxml_xml_node_find_node_by_name(xmlDocGetRootElement(doc), node_name);
}

bool mxml_xml_node_try_get_value(void *node, char **value) {
    return !value && (*value = (char *) xmlNodeGetContent(node)) != NULL;
}

char *mxml_xml_node_get_value_str(void *root_node, const char *node_name) {
    char *res;
    if (!mxml_xml_node_try_get_value(mxml_xml_node_find_node_by_name(root_node, node_name), &res))
        return malloc(0);
    return res;
}

char *mxml_xml_doc_get_value_str(void *doc, const char *node_name) {
    char *res;
    if (!mxml_xml_node_try_get_value(mxml_xml_doc_find_node_by_name(doc, node_name), &res))
        return malloc(0);
    return res;
}

bool mxml_xml_node_try_get_attr(void *node, const char *attr_name, char **value) {
    return !value && (*value = (char *) xmlGetProp(node, BAD_CAST attr_name)) != NULL;
}

char *mxml_xml_node_get_attr_str(void *root_node, const char *node_name, const char *attr_name) {
    char *res;
    if (!mxml_xml_node_try_get_attr(mxml_xml_node_find_node_by_name(root_node, node_name), attr_name, &res))
        return malloc(0);
    return res;
}

char *mxml_xml_doc_get_attr_str(void *doc, const char *node_name, const char *attr_name) {
    char *res;
    if (!mxml_xml_node_try_get_attr(mxml_xml_doc_find_node_by_name(doc, node_name), attr_name, &res))
        return malloc(0);
    return res;
}

void *mxml_xml_node_insert_before(void *ref_node, void *new_node) {
    if (!new_node)
        return NULL;
    if (!ref_node) {
        xmlFreeNode(new_node);
        return NULL;
    }
    if (((xmlNodePtr) ref_node)->parent &&
        (!mxml_xml_node_children_valid(ref_node) || mxml_xml_node_is_read_only(ref_node))) {
        xmlFreeNode(new_node);
        return NULL;
    }
    return xmlAddPrevSibling(ref_node, new_node);
}

void *mxml_xml_node_child_new(void *parent_node, const char *node_name, const char *node_value) {
    if (!node_name || xmlValidateName(BAD_CAST node_name, 0) != 0)
        return NULL;
    if (parent_node && (!mxml_xml_node_children_valid(parent_node) || mxml_xml_node_is_read_only(parent_node)))
        return NULL;
    if (mu_is_empty(node_value))
        return xmlNewTextChild(parent_node, NULL, BAD_CAST node_name, BAD_CAST "");
    else
        return xmlNewChild(parent_node, NULL, BAD_CAST node_name, BAD_CAST node_value);
}

void *mxml_xml_node_child_empty_new(void *parent_node, const char *node_name) {
    return mxml_xml_node_child_new(parent_node, node_name, NULL);
}

void *mxml_xml_doc_import_node(void *doc, void *imported_node, bool deep) {
    xmlNodePtr nodep;
    xmlNodePtr retnodep;
    xmlNsPtr nsptr;
    xmlNodePtr root;
    int recursive;
    int errorcode;
    if (!doc || !imported_node)
        return NULL;
    nodep = imported_node;
    if (nodep->type == XML_HTML_DOCUMENT_NODE || nodep->type == XML_DOCUMENT_NODE
        || nodep->type == XML_DOCUMENT_TYPE_NODE)
        return NULL;
    if (nodep->doc == doc)
        retnodep = nodep;
    else {
        recursive = deep ? 1 : 0;
        if (recursive == 0 && nodep->type == XML_ELEMENT_NODE)
            recursive = 2;
        retnodep = xmlDocCopyNode(nodep, doc, recursive);
        if (!retnodep)
            return NULL;
        if (retnodep->type == XML_ATTRIBUTE_NODE && nodep->ns != NULL) {
            root = xmlDocGetRootElement(doc);
            nsptr = xmlSearchNsByHref(nodep->doc, root, nodep->ns->href);
            if (!nsptr)
                nsptr = _mxml_xml_node_dom_get_ns(root, (char *) nodep->ns->href, &errorcode,
                                                  (char *) nodep->ns->prefix);
            xmlSetNs(retnodep, nsptr);
        }
    }
    return retnodep;
}

char *mxml_xml_node_dom_element_get_attribute(void *node, const char *name) {
    xmlChar *value;
    xmlNodePtr attr;
    if (!node || mu_is_empty(name))
        return NULL;
    value = NULL;
    attr = _mxml_xml_dom_get_dom1_attribute(node, (xmlChar *) name);
    if (attr) {
        switch (attr->type) {
            case XML_ATTRIBUTE_NODE:
                value = xmlNodeListGetString(attr->doc, attr->children, 1);
                break;
            case XML_NAMESPACE_DECL:
                value = xmlStrdup(((xmlNsPtr) attr)->href);
                break;
            default:
                value = xmlStrdup(((xmlAttributePtr) attr)->defaultValue);
        }
    }
    return (char *) value;
}

// XML sec

struct mxml_xmlsec_cfg {
    bool ok;
    const char *pfx_filename;
    const char *pfx_pwd;
};

void mxml_xmlsec_init(bool columned_base64, mxml_err_cb error_cb, void *error_cls) {
    mxml_xml_init();
    LIBXML_TEST_VERSION
    xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefault(1);
    if (!columned_base64)
        xmlSecBase64SetDefaultLineSize(0);
    if (xmlSecInit() < 0)
        _MXML_THROW(error_cb, error_cls, _("xmlSecInit(): initialization failed\n"));
    else if (xmlSecCheckVersion() != 1)
        _MXML_THROW(error_cb, error_cls, _("xmlSecCheckVersion(): incompatible version\n"));
    else if (xmlSecCryptoAppInit(NULL) < 0)
        _MXML_THROW(error_cb, error_cls, _("xmlSecCryptoAppInit(): initialization failed\n"));
    else if (xmlSecCryptoInit() < 0)
        _MXML_THROW(error_cb, error_cls, _("xmlSecCryptoInit(): initialization failed\n"));
}

void mxml_xmlsec_finit() {
    xmlSecCryptoShutdown();
    xmlSecCryptoAppShutdown();
    xmlSecShutdown();
    mxml_xml_finit();
}

struct mxml_xmlsec_cfg *mxml_xmlsec_cfg_pfx_file_new(const char *pfx_filename, const char *pfx_pwd,
                                                     mxml_err_cb error_cb, void *error_cls) {
    struct mxml_xmlsec_cfg *cfg;
    if (!pfx_filename) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), "pfx_filename");
        return NULL;
    }
    if (!pfx_pwd) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), "pfx_pwd");
        return NULL;
    }
    if (!mu_exists(pfx_filename)) {
        _MXML_THROW(error_cb, error_cls, _("File not found: %s\n"), pfx_filename);
        return NULL;
    }
    if (!(cfg = calloc(1, sizeof(struct mxml_xmlsec_cfg))))
        return NULL;
    cfg->pfx_filename = pfx_filename;
    cfg->pfx_pwd = pfx_pwd;
    cfg->ok = true;
    return cfg;
}

void mxml_xmlsec_cfg_free(struct mxml_xmlsec_cfg *cfg) {
    free(cfg);
}

bool mxml_xmlsec_cfg_ok(struct mxml_xmlsec_cfg *cfg) {
    return cfg && cfg->ok;
}

bool mxml_xmlsec_sign(struct mxml_xmlsec_cfg *cfg, bool is_file, const char *data, char **signed_data,
                      const char *param_name, const char *signed_param_name, const char *encoding, const char *id_attr,
                      const char *id_name, bool declares_attlist, bool formatted, bool removes_blanks,
                      bool skip_xml_declaration, int compression, bool auto_uri, bool single_cert,
                      mxml_sec_cb sec_cb, void *sec_cls, mxml_err_cb error_cb, void *error_cls) {
    xmlDocPtr doc;
    xmlChar *id;
    xmlChar *uri;
    xmlNodePtr id_node;
    xmlNodePtr root_node;
    xmlNodePtr sign_node;
    xmlNodePtr ref_node;
    xmlNodePtr key_info_node;
    xmlNodePtr x509_data_node;
    xmlNodePtr last_cert_node;
    xmlNodePtr prev_cert_node;
    xmlSecDSigCtxPtr d_sig_ctx;
    xmlSaveCtxtPtr save_ctxt;
    xmlBufferPtr buffer;
    bool res;
    char *attlist;
    int options;
    if (!cfg) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), "cfg");
        return false;
    }
    if (!cfg->ok) {
        _MXML_THROW(error_cb, error_cls, _("Invalid configuration\n"));
        return false;
    }
    if (!data) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), param_name);
        return false;
    }
    if (!signed_data) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), signed_param_name);
        return false;
    }
    if (is_file && !mu_exists(data)) {
        _MXML_THROW(error_cb, error_cls, _("File not found: %s\n"), data);
        return false;
    }
    res = false;
    id = NULL;
    uri = NULL;
    d_sig_ctx = NULL;
    if (is_file)
        doc = xmlParseFile(data);
    else
        /*doc = xmlParseMemory(data, xmlStrlen(BAD_CAST data));*/
        doc = xmlParseDoc(BAD_CAST data);
    if (!doc || !(root_node = xmlDocGetRootElement(doc))) {
        if (is_file)
            _MXML_THROW(error_cb, error_cls, _("Unable to parse file: %s\n"), data);
        else
            _MXML_THROW(error_cb, error_cls, _("Unable to parse string\n"));
        goto done;
    }
    if (declares_attlist && (id_attr && id_name &&
                             asprintf(&attlist, "test [<!ATTLIST %s %s ID #IMPLIED>]", id_name, id_attr) != -1 &&
                             attlist)) {
        xmlCreateIntSubset(doc, BAD_CAST attlist, NULL, NULL);
        free(attlist);
    }
    sign_node = xmlSecFindNode(root_node, xmlSecNodeSignature, xmlSecDSigNs);
    if (sec_cb && sign_node && !sec_cb(sec_cls, doc, MXML_SEC_CB_TYPE_SIGNING))
        goto done;
    if (sign_node) {
        _MXML_THROW(error_cb, error_cls, _("Signature node already exists\n"));
        goto done;
    }
    sign_node = xmlSecTmplSignatureCreate(doc, xmlSecTransformInclC14NId, xmlSecTransformRsaSha1Id, NULL);
    xmlAddChild(root_node, sign_node);
    if (!sign_node) {
        _MXML_THROW(error_cb, error_cls, _("Failed to create signature template\n"));
        goto done;
    }
    if (auto_uri && id_attr) {
        id_node = xmlFirstElementChild(root_node);
        _mxml_xml_ensure_id(id_node, BAD_CAST id_attr, &id);
        if (id)
            uri = xmlStrncatNew(BAD_CAST "#", id, -1);
    }
    ref_node = xmlSecTmplSignatureAddReference(sign_node, xmlSecTransformSha1Id, NULL, uri, NULL);
    if (!ref_node) {
        _MXML_THROW(error_cb, error_cls, _("Failed to add reference to signature template\n"));
        goto done;
    }
    if (!xmlSecTmplReferenceAddTransform(ref_node, xmlSecTransformEnvelopedId)) {
        _MXML_THROW(error_cb, error_cls, _("Failed to add enveloped transform to reference\n"));
        goto done;
    }
    if (!xmlSecTmplReferenceAddTransform(ref_node, xmlSecTransformInclC14NId)) {
        _MXML_THROW(error_cb, error_cls, _("Failed to add C14N transform to reference\n"));
        goto done;
    }
    key_info_node = xmlSecTmplSignatureEnsureKeyInfo(sign_node, NULL);
    if (!key_info_node) {
        _MXML_THROW(error_cb, error_cls, _("Failed to add key info\n"));
        goto done;
    }
    x509_data_node = xmlSecTmplKeyInfoAddX509Data(key_info_node);
    if (!x509_data_node) {
        _MXML_THROW(error_cb, error_cls, _("Failed to add X509Data node\n"));
        goto done;
    }
    if (!xmlSecTmplX509DataAddCertificate(x509_data_node)) {
        _MXML_THROW(error_cb, error_cls, _("Failed to add X509Certificate node\n"));
        goto done;
    }
    if (removes_blanks)
        _mxml_xml_remove_blank_nodes(root_node);
    if (sec_cb && !sec_cb(sec_cls, doc, MXML_SEC_CB_TYPE_BEFORE))
        goto done;
    d_sig_ctx = xmlSecDSigCtxCreate(NULL);
    if (!d_sig_ctx) {
        _MXML_THROW(error_cb, error_cls, _("Failed to create signature context\n"));
        goto done;
    }
    d_sig_ctx->signKey = xmlSecCryptoAppPkcs12Load(cfg->pfx_filename, cfg->pfx_pwd, NULL, NULL);
    if (!d_sig_ctx->signKey) {
        _MXML_THROW(error_cb, error_cls, _("Failed to load PFX file: %s\n"), cfg->pfx_filename);
        goto done;
    }
    if (xmlSecDSigCtxSign(d_sig_ctx, sign_node) < 0) {
        _MXML_THROW(error_cb, error_cls, _("Signature failed\n"));
        goto done;
    }
    if (removes_blanks)
        _mxml_xml_remove_blank_nodes(sign_node);
    /*TODO: remove the implementation below sending a patch to the xmlsec project, decaring something like "skip_other_certs". */
    if (single_cert) {
        last_cert_node = xmlLastElementChild(x509_data_node);
        while ((prev_cert_node = xmlPreviousElementSibling(last_cert_node))) {
            xmlUnlinkNode(prev_cert_node);
            xmlFreeNode(prev_cert_node);
        }
    }
    if (sec_cb && !sec_cb(sec_cls, doc, MXML_SEC_CB_TYPE_AFTER))
        goto done;
    xmlSetDocCompressMode(doc, compression);
    options = XML_SAVE_AS_XML;
    if (formatted)
        options |= XML_SAVE_FORMAT;
    if (skip_xml_declaration)
        options |= XML_SAVE_NO_DECL;
    if (is_file) {
        save_ctxt = xmlSaveToFilename(*signed_data, encoding, options);
        buffer = NULL;
    } else {
        buffer = xmlBufferCreate();
        if (!buffer)
            goto done;
        save_ctxt = xmlSaveToBuffer(buffer, encoding, options);
    }
    if (!save_ctxt) {
        _MXML_THROW(error_cb, error_cls, _("Failed to create save context\n"));
        goto done;
    }
    if (xmlSaveDoc(save_ctxt, doc) < 0) {
        if (is_file)
            _MXML_THROW(error_cb, error_cls, _("Failed to save file: %s\n"), data);
        else
            _MXML_THROW(error_cb, error_cls, _("Failed to save XML data\n"));
        goto done;
    }
    xmlSaveClose(save_ctxt);
    if (!is_file) {
        *signed_data = strdup((char *) xmlBufferContent(buffer));
        xmlBufferFree(buffer);
    }
    res = true;
done:
    free(uri);
    free(id);
    if (d_sig_ctx)
        xmlSecDSigCtxDestroy(d_sig_ctx);
    if (doc)
        xmlFreeDoc(doc);
    return res;
}

bool mxml_xmlsec_sign_string(struct mxml_xmlsec_cfg *cfg, const char *string, char **signed_string,
                             const char *encoding, const char *id_attr, const char *id_name, bool declares_attlist,
                             bool formatted, bool removes_blanks, bool skip_xml_declaration, int compression,
                             bool auto_uri, bool single_cert, mxml_sec_cb sec_cb, void *sec_cls,
                             mxml_err_cb error_cb, void *error_cls) {
    return mxml_xmlsec_sign(cfg, false, string, signed_string, "string", "signed_string", encoding, id_attr, id_name,
                            declares_attlist, formatted, removes_blanks, skip_xml_declaration, compression, auto_uri,
                            single_cert, sec_cb, sec_cls, error_cb, error_cls);
}

bool mxml_xmlsec_easy_sign_string(struct mxml_xmlsec_cfg *cfg, const char *string, char **signed_string,
                                  const char *id_attr, const char *id_name, bool declares_attlist,
                                  mxml_sec_cb sec_cb, void *sec_cls, mxml_err_cb error_cb, void *error_cls) {
    return mxml_xmlsec_sign_string(cfg, string, signed_string, NULL, id_name, id_attr, declares_attlist, false, true,
                                   true, 0, true, true, sec_cb, sec_cls, error_cb, error_cls);
}

bool mxml_xmlsec_sign_file(struct mxml_xmlsec_cfg *cfg, const char *filename, const char *signed_filename,
                           const char *encoding, const char *id_attr, const char *id_name, bool declares_attlist,
                           bool formatted, bool removes_blanks, bool skip_xml_declaration, int compression,
                           bool auto_uri, bool single_cert, mxml_sec_cb sec_cb, void *sec_cls,
                           mxml_err_cb error_cb, void *error_cls) {
    return mxml_xmlsec_sign(cfg, true, filename, (void *) &signed_filename, "filename", "signed_filename",
                            encoding, id_attr, id_name, declares_attlist, formatted, removes_blanks,
                            skip_xml_declaration, compression, auto_uri, single_cert,
                            sec_cb, sec_cls, error_cb, error_cls);
}

bool mxml_xmlsec_easy_sign_file(struct mxml_xmlsec_cfg *cfg, const char *filename, const char *signed_filename,
                                const char *id_attr, const char *id_name, bool declares_attlist,
                                mxml_sec_cb sec_cb, void *sec_cls, mxml_err_cb error_cb, void *error_cls) {
    return mxml_xmlsec_sign_file(cfg, filename, signed_filename, NULL, id_name, id_attr, declares_attlist, false, true,
                                 true, 0, true, true, sec_cb, sec_cls, error_cb, error_cls);
}

bool mxml_xmlsec_verify_file(struct mxml_xmlsec_cfg *cfg, const char *filename, bool removes_dtd,
                             mxml_sec_cb sec_cb, void *sec_cls, mxml_err_cb error_cb, void *error_cls) {
    xmlDocPtr doc;
    xmlNodePtr root_node;
    xmlNodePtr sign_node;
    xmlSecDSigCtxPtr d_sig_ctx;
    bool res;
    if (!cfg) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), "cfg");
        return false;
    }
    if (!cfg->ok) {
        _MXML_THROW(error_cb, error_cls, _("Invalid configuration\n"));
        return false;
    }
    if (!filename) {
        _MXML_THROW(error_cb, error_cls, _("Required argument: %s\n"), "filename");
        return false;
    }
    if (!mu_exists(filename)) {
        _MXML_THROW(error_cb, error_cls, _("File not found: %s\n"), filename);
        return false;
    }
    res = false;
    d_sig_ctx = NULL;
    doc = xmlParseFile(filename);
    if (!doc || !(root_node = xmlDocGetRootElement(doc))) {
        _MXML_THROW(error_cb, error_cls, _("Unable to parse file: %s\n"), filename);
        goto done;
    }
    sign_node = xmlSecFindNode(root_node, xmlSecNodeSignature, xmlSecDSigNs);
    if (!sign_node) {
        _MXML_THROW(error_cb, error_cls, _("Signature node not found\n"));
        goto done;
    }
    if (sec_cb && !sec_cb(sec_cls, doc, MXML_SEC_CB_TYPE_BEFORE))
        goto done;
    d_sig_ctx = xmlSecDSigCtxCreate(NULL);
    if (!d_sig_ctx) {
        _MXML_THROW(error_cb, error_cls, _("Failed to create signature context\n"));
        goto done;
    }
    d_sig_ctx->signKey = xmlSecCryptoAppPkcs12Load(cfg->pfx_filename, cfg->pfx_pwd, NULL, NULL);
    if (!d_sig_ctx->signKey) {
        _MXML_THROW(error_cb, error_cls, _("Failed to load PFX file: %s\n"), cfg->pfx_filename);
        goto done;
    }
    if (xmlSecDSigCtxVerify(d_sig_ctx, sign_node) < 0) {
        _MXML_THROW(error_cb, error_cls, _("Signature verification failed\n"));
        goto done;
    }
    res = xmlSecDSigStatusSucceeded == d_sig_ctx->status;
    if (res && removes_dtd) {
        xmlDtdPtr dtd = xmlGetIntSubset(doc);
        if (dtd) {
            xmlUnlinkNode((xmlNodePtr) dtd);
            xmlFreeDtd(dtd);
            xmlSaveFile(filename, doc);
        }
    }
    if (sec_cb && !sec_cb(sec_cls, doc, MXML_SEC_CB_TYPE_AFTER)) {
        res = false;
        goto done;
    }
done:
    if (d_sig_ctx)
        xmlSecDSigCtxDestroy(d_sig_ctx);
    if (doc)
        xmlFreeDoc(doc);
    return res;
}

bool mxml_xmlsec_easy_verify_file(struct mxml_xmlsec_cfg *cfg, const char *filename, mxml_sec_cb sec_cb, void *sec_cls,
                                  mxml_err_cb error_cb, void *error_cls) {
    return mxml_xmlsec_verify_file(cfg, filename, false, sec_cb, sec_cls, error_cb, error_cls);
}

#undef _MXML_THROW