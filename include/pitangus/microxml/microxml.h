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

#ifndef MICROXML_H
#define MICROXML_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MXML_EXTERN
# define MXML_EXTERN extern
#endif

#define MXML_ERR_SIZE 256

/**
 * \brief Returns the library version.
 * \return Library version.
 */
MXML_EXTERN const char *mxml_version();

/**
 * \brief Enum for identifying the signature phase.
 */
enum MXML_SEC_CB_TYPE {
    /// Before signing.
    MXML_SEC_CB_TYPE_BEFORE = 0,
    /// On signing.
    MXML_SEC_CB_TYPE_SIGNING = 1,
    /// After signing.
    MXML_SEC_CB_TYPE_AFTER = 2
};

/**
 * \brief Callback for handling the signature.
 * \param cls Callback class.
 * \param doc Reference to the DOM object.
 * \param cb_type Callback type.
 */
typedef bool (*mxml_sec_cb)(void *cls, void *doc, enum MXML_SEC_CB_TYPE cb_type);

/**
 * \brief Callback for handling errors.
 * \param cls Callback class.
 * \param msg Message error.
 */
typedef void (*mxml_err_cb)(void *cls, const char *msg);

/* XML */

/**
 * \brief The XML configuration object.
 */
struct mxml_xml_cfg;

/**
 * \brief Initializes the XML2 library.
 */
MXML_EXTERN void mxml_xml_init();

/**
 * \brief Finalizes the XML2 library.
 */
MXML_EXTERN void mxml_xml_finit();

/**
 * \brief Creates a new _mxml_xml_cfg_ instance.
 * \param xsd_uri XSD URL or filename.
 * \param error_cb Error callback.
 * \param warn_cb Warning callback.
 * \param error_warn_cls Error/warning callback class.
 * \return Instance of _mxml_xml_cfg_.
 */
MXML_EXTERN struct mxml_xml_cfg *mxml_xml_cfg_new(const char *xsd_uri, mxml_err_cb error_cb,
                                                  mxml_err_cb warn_cb, void *error_warn_cls);

/**
 * \brief Frees _mxml_xml_cfg_ instance.
 * \param cfg Instance to be freed.
 */
MXML_EXTERN void mxml_xml_cfg_free(struct mxml_xml_cfg *cfg);

/**
 * \brief Checks if a _mxml_xml_cfg_ is OK.
 * \param cfg Instance to be checked.
 * \return **true** if OK **false** otherwise.
 */
MXML_EXTERN bool mxml_xml_cfg_ok(struct mxml_xml_cfg *cfg);

/**
 * \brief Validates a XML file.
 * \param cfg _mxml_xml_cfg_ instance
 * \param filename XML file to be validated.
 * \param error_cb Error callback.
 * \param error_cls Error callback class.
 * \return **true** if valid **false** otherwise.
 */
MXML_EXTERN bool mxml_xml_validate_file(struct mxml_xml_cfg *cfg, const char *filename,
                                        mxml_err_cb error_cb, void *error_cls);

/* XML utils */

/**
 * \brief Creates a new element into the document passed as reference.
 * \param doc Document instance (DOM).
 * \param node_name Node name.
 * \param node_value Node value.
 * \return Element pointer.
 * \note If the **node_value** is **NULL** the function uses an empty string instead.
 */
MXML_EXTERN void *mxml_xml_doc_create_element(void *doc, const char *node_name, const char *node_value);

/**
 * \brief Creates a new empty element into the document passed as reference.
 * \param doc Document instance (DOM).
 * \param node_name Node name.
 * \return Element pointer.
 */
MXML_EXTERN void *mxml_xml_doc_create_empty_element(void *doc, const char *node_name);

/**
 * \brief Creates a new attribute in the document passed as reference.
 * \param doc Document instance (DOM).
 * \param attr_name Attribute name.
 * \return Attribute pointer.
 */
MXML_EXTERN void *mxml_xml_doc_create_attribute(void *doc, const char *attr_name);

/**
 * \brief creates a Text Node with the specified content in the document passed as reference.
 * \param doc Document instance (DOM).
 * \param content Content text.
 * \return Node pointer.
 */
MXML_EXTERN void *mxml_xml_doc_create_text_node(void *doc, const char *content);

/**
 * \brief Appends a node as the last child of a node.
 * \param parent_node Parent node.
 * \param new_node New node to be appended.
 * \return Node pointer.
 */
MXML_EXTERN void *mxml_xml_node_append_child(void *parent_node, void *new_node);

/**
 * \brief Creates a CDATASection node with the specified value.
 * \param doc Document instance (DOM).
 * \param value Text value.
 * \param value_length Value length.
 * \return Object pointer.
 */
MXML_EXTERN void *mxml_xml_doc_create_cdata_section(void *doc, const char *value, int value_length);

/**
 * \brief Checks if the specified node is a valid children.
 * \param node Node to be checked.
 * \return **true** if valid **false** otherwise.
 */
MXML_EXTERN bool mxml_xml_node_children_valid(void *node);

/**
 * \brief Checks if a node is read only.
 * \param node Node to be checked.
 * \return **true** if read only **false** otherwise.
 */
MXML_EXTERN bool mxml_xml_node_is_read_only(void *node);

/**
 * \brief Finds a node recursively inside another one.
 * \param root_node Base node.
 * \param node_name Node name.
 * \return Node value if found **NULL** otherwise.
 */
MXML_EXTERN void *mxml_xml_node_find_node_by_name(void *root_node, const char *node_name);

/**
 * \brief Finds a node recursively inside a document.
 * \param doc Document instance (DOM).
 * \param node_name Node name.
 * \return Node pointer if found **NULL** otherwise.
 */
MXML_EXTERN void *mxml_xml_doc_find_node_by_name(void *doc, const char *node_name);

/**
 * \brief Tries to get a node value.
 * \param node Given node.
 * \param value Node value.
 * \return **true** if got **false** otherwise.
 */
MXML_EXTERN bool mxml_xml_node_try_get_value(void *node, char **value);

/**
 * \brief Finds a node recursively inside another one and gets its value.
 * \param root_node Base node.
 * \param node_name Node name.
 * \return Node value if found or empty string otherwise.
 */
MXML_EXTERN char *mxml_xml_node_get_value_str(void *root_node, const char *node_name);

/**
 * \brief Finds a node recursively inside a document and gets its value.
 * \param doc Document instance (DOM).
 * \param node_name Node name.
 * \return Node value if found or empty string otherwise.
 */
MXML_EXTERN char *mxml_xml_doc_get_value_str(void *doc, const char *node_name);

/**
 * \brief Tries to get a node attribute value.
 * \param node Given node.
 * \param attr_name Attribute name.
 * \param value Attribute value.
 * \return **true** if got **false** otherwise.
 */
MXML_EXTERN bool mxml_xml_node_try_get_attr(void *node, const char *attr_name, char **value);

/**
 * \brief Finds a node recursively inside another one and gets a specified attribute value.
 * \param root_node Base node.
 * \param node_name Node name.
 * * \param attr_name Attribute name.
 * \return Attribute value if found or empty string otherwise.
 */
MXML_EXTERN char *mxml_xml_node_get_attr_str(void *root_node, const char *node_name, const char *attr_name);

/**
 * \brief Finds a node recursively inside a document and gets a specified attribute value.
 * \param root_node Base node.
 * \param node_name Node name.
 * * \param attr_name Attribute name.
 * \return Attribute value if found or empty string otherwise.
 */
MXML_EXTERN char *mxml_xml_doc_get_attr_str(void *doc, const char *node_name, const char *attr_name);

/**
 * \brief Inserts a node as a child right before an existing one.
 * \param ref_node Reference node.
 * \param new_node New node to be inserted.
 * \return Node reference.
 */
MXML_EXTERN void *mxml_xml_node_insert_before(void *ref_node, void *new_node);

/**
 * \brief Creates a child node inside another one.
 * \param parent_node Parent node.
 * \param node_name Node name.
 * \param node_value Node value.
 * \return Node reference.
 */
MXML_EXTERN void *mxml_xml_node_child_new(void *parent_node, const char *node_name, const char *node_value);

/**
 * \brief Creates an empty child node inside another one.
 * \param parent_node Parent node.
 * \param node_name Node name.
 * \param node_value Node value.
 * \return Node reference.
 */
MXML_EXTERN void *mxml_xml_node_child_empty_new(void *parent_node, const char *node_name);

/**
 * \brief Imports a node from another document.
 * \param doc Document instance (DOM).
 * \param imported_node **false** imports node itself **true** all child nodes.
 * \param deep
 * \return
 */
MXML_EXTERN void *mxml_xml_doc_import_node(void *doc, void *imported_node, bool deep);

/**
 * \brief Retrieves an attribute value by name.
 * \param node Specified node.
 * \param name Attribute name.
 * \return Attribute value.
 */
MXML_EXTERN char *mxml_xml_node_dom_element_get_attribute(void *node, const char *name);

/* XML sec */

/**
 * \brief The XMLSec configuration object.
 */
struct mxml_xmlsec_cfg;

/**
 * \brief Initializes the XMLSec library.
 * \param error_cb Error callback.
 * \param error_warn_cls Error callback class.
 */
MXML_EXTERN void mxml_xmlsec_init(bool columned_base64, mxml_err_cb error_cb, void *error_cls);

/**
 * \brief Finalizes the XMLSec library.
 */
MXML_EXTERN void mxml_xmlsec_finit();

/**
 * \brief Creates a new _mxml_xmlsec_cfg_ passing the PFX file name.
 * \param pfx_filename PFX file name.
 * \param pfx_pwd PFX password.
 * \param error_cb Error callback.
 * \param error_cls Error callback class.
 * \return Instance of _mxml_xmlsec_cfg_.
 */
MXML_EXTERN struct mxml_xmlsec_cfg *mxml_xmlsec_cfg_pfx_file_new(const char *pfx_filename, const char *pfx_pwd,
                                                                 mxml_err_cb error_cb, void *error_cls);

/**
 * \brief Frees _mxml_xmlsec_cfg_ instance.
 * \param cfg Instance to be freed.
 */
MXML_EXTERN void mxml_xmlsec_cfg_free(struct mxml_xmlsec_cfg *cfg);

/**
 * \brief Checks if a _mxml_xmlsec_cfg_ is OK.
 * \param cfg Instance to be checked.
 * \return **true** if OK **false** otherwise.
 */
MXML_EXTERN bool mxml_xmlsec_cfg_ok(struct mxml_xmlsec_cfg *cfg);

/**
 * \brief Signs a XML string.
 * \param cfg Configuration object.
 * \param string String to be signed.
 * \param signed_string Signed string.
 * \param encoding String encoding.
 * \param id_attr Attribute ID.
 * \param id_name ID name.
 * \param declares_attlist Declares _ATTLIST_.
 * \param formatted Formatted result.
 * \param removes_blanks Removes all blank elements.
 * \param skip_xml_declaration Skip the XML declaration.
 * \param compression Compression level (0 (uncompressed) to 9 (max compression)).
 * \param auto_uri **true** auto declares URI.
 * \param single_cert **true** removes the other certificates.
 * \param sec_cb Signature callback.
 * \param sec_cls Signature callback class.
 * \param error_cb Error callback.
 * \param error_cls Error callback class.
 * \return **true** if signed **false** otherwise.
 */
MXML_EXTERN bool mxml_xmlsec_sign_string(struct mxml_xmlsec_cfg *cfg, const char *string, char **signed_string,
                                         const char *encoding, const char *id_attr, const char *id_name,
                                         bool declares_attlist, bool formatted, bool removes_blanks,
                                         bool skip_xml_declaration, int compression, bool auto_uri, bool single_cert,
                                         mxml_sec_cb sec_cb, void *sec_cls, mxml_err_cb error_cb, void *error_cls);

/**
 * \brief Signs a XML string passing less parameters.
 * \param cfg Configuration object.
 * \param string String to be signed.
 * \param signed_string Signed string.
 * \param id_attr Attribute ID.
 * \param id_name ID name.
 * \param declares_attlist Declares _ATTLIST_.
 * \param sec_cb Signature callback.
 * \param sec_cls Signature callback class.
 * \param error_cb Error callback.
 * \param error_cls Error callback class.
 * \return **true** if signed **false** otherwise.
 */
MXML_EXTERN bool mxml_xmlsec_easy_sign_string(struct mxml_xmlsec_cfg *cfg, const char *string, char **signed_string,
                                              const char *id_attr, const char *id_name, bool declares_attlist,
                                              mxml_sec_cb sec_cb, void *sec_cls,
                                              mxml_err_cb error_cb, void *error_cls);

/**
 * \brief Signs a XML file.
 * \param cfg Configuration object.
 * \param filename File to be signed.
 * \param signed_filename Signed file.
 * \param encoding File encoding.
 * \param id_attr Attribute ID.
 * \param id_name ID name.
 * \param declares_attlist Declares _ATTLIST_.
 * \param formatted Formatted result.
 * \param removes_blanks Removes all blank elements.
 * \param skip_xml_declaration Skip the XML declaration.
 * \param compression Compression level (0 (uncompressed) to 9 (max compression)).
 * \param auto_uri **true** auto declares URI.
 * \param single_cert **true** removes the other certificates.
 * \param sec_cb Signature callback.
 * \param sec_cls Signature callback class.
 * \param error_cb Error callback.
 * \param error_cls Error callback class.
 * \return **true** if signed **false** otherwise.
 */
MXML_EXTERN bool mxml_xmlsec_sign_file(struct mxml_xmlsec_cfg *cfg, const char *filename, const char *signed_filename,
                                       const char *encoding, const char *id_attr, const char *id_name,
                                       bool declares_attlist, bool formatted, bool removes_blanks,
                                       bool skip_xml_declaration, int compression, bool auto_uri,
                                       bool single_cert, mxml_sec_cb sec_cb, void *sec_cls,
                                       mxml_err_cb error_cb, void *error_cls);

/**
 * \brief Signs a XML file passing less parameters.
 * \param cfg Configuration object.
 * \param filename File name.
 * \param signed_filename Signed file.
 * \param id_attr Attribute ID.
 * \param id_name ID name.
 * \param declares_attlist Declares _ATTLIST_.
 * \param sec_cb Signature callback.
 * \param sec_cls Signature callback class.
 * \param error_cb Error callback.
 * \param error_cls Error callback class.
 * \return **true** if signed **false** otherwise.
 */
MXML_EXTERN bool mxml_xmlsec_easy_sign_file(struct mxml_xmlsec_cfg *cfg, const char *filename,
                                            const char *signed_filename, const char *id_attr,
                                            const char *id_name, bool declares_attlist,
                                            mxml_sec_cb sec_cb, void *sec_cls,
                                            mxml_err_cb error_cb, void *error_cls);

/**
 * \brief Validates the signature of the specified file.
 * \param cfg Configuration object.
 * \param filename File name.
 * \param removes_dtd Removes DTD.
 * \param sec_cb Signature callback.
 * \param sec_cls Signature callback class.
 * \param error_cb Error callback.
 * \param error_cls Error callback class.
 * \return **true** if valid **false** otherwise.
 */
MXML_EXTERN bool mxml_xmlsec_verify_file(struct mxml_xmlsec_cfg *cfg, const char *filename, bool removes_dtd,
                                         mxml_sec_cb sec_cb, void *sec_cls,
                                         mxml_err_cb error_cb, void *error_cls);

/**
 * \brief Validates the signature of the specified file passing less parameters.
 * \param cfg Configuration object.
 * \param filename File name.
 * \param sec_cb Signature callback.
 * \param sec_cls Signature callback class.
 * \param error_cb Error callback.
 * \param error_cls Error callback class.
 * \return **true** if valid **false** otherwise.
 */
MXML_EXTERN bool mxml_xmlsec_easy_verify_file(struct mxml_xmlsec_cfg *cfg, const char *filename,
                                              mxml_sec_cb sec_cb, void *sec_cls,
                                              mxml_err_cb error_cb, void *error_cls);

#ifdef __cplusplus
}
#endif

#endif