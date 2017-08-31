/*
 * libmicrosped - Micro biblioteca multiplataforma para comunicação com web services
 * de SPED da SEFAZ.
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

#include <stdio.h>
#include "msped_strs.h"
#include "msped_macros.h"
#include "msped_ws_defs.h"
#include "msped_http.h"
#include "msped_xpath.h"

#define MSPED_QRCODE_SEQ_FMT \
    "chNFe=%s" \
    "&nVersao=%s" \
    "&tpAmb=%s" \
    "%s%s" \
    "&dhEmi=%s" \
    "&vNF=%s" \
    "&vICMS=%s" \
    "&digVal=%s" \
    "&cIdToken=%s"

#define MSPED_QRCODE_URL_FMT \
    "%s%s" \
    "%s" \
    "&cHashQRCode=%s"

struct msped_cfg {
    xmlDocPtr modelo_nf;
    long http_timeout;
    bool ok;
    bool depuravel;
    enum MSPED_TIPO tipo;
    enum MSPED_CUF cuf;
    enum MSPED_AMBIENTE ambiente;
    enum MSPED_SV sv;
    enum MSPED_NF_MODELO modelo;
    char *pfx;
    char *pfx_senha;
    char *portal;
    char *n_versao;
    char *token;
    char *id_token;
};

struct msped_lote {
    const char *itens[MSPED_LOTE_QTD_MAX_ITENS];
    uint8_t quantidade;
};

static inline bool msped_validar_cfg(struct msped_cfg *cfg, msped_erro_callback erro_cb, void *erro_cls) {
    if (NULL == cfg) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "cfg");
        return false;
    }
    if (!cfg->ok) {
        _MSPED_ERR(erro_cb, erro_cls, "%s", S_MSPED_CFG_INV);
        return false;
    }
    return true;
}

bool msped_ws_info(xmlDocPtr modelo_nf, enum MSPED_NF_MODELO modelo, enum MSPED_TIPO tipo, enum MSPED_AMBIENTE ambiente,
                   enum MSPED_CUF cuf, enum MSPED_SV sv, const char *servico,
                   char **metodo, char **operacao, char **versao, char **sv_str, char **url,
                   msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    xmlXPathContextPtr xpath_ctx;
    res = false;
    *metodo = NULL;
    *operacao = NULL;
    *url = NULL;
    if (mu_is_empty(servico)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "servico");
        return false;
    }
    xpath_ctx = xmlXPathNewContext(modelo_nf);
    if (NULL == xpath_ctx) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "xpath_ctx");
        goto feito;
    }
    *sv_str = (char *) msped_obter_sv_str(tipo, modelo, cuf, sv);
    if (NULL == *sv_str) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_VAR_ERR, "sv_str");
        goto feito;
    }
    if (!msped_xpath_parse_node(xpath_ctx, ambiente, *sv_str, servico,
                                metodo, operacao, versao, url, erro_cb, erro_cls))
        goto feito;
    if (NULL != sv_str && (0 == strcmp(*sv_str, "SVAN") || 0 == strcmp(*sv_str, "SVRS")))
        msped_xpath_parse_node(xpath_ctx, ambiente, (char *) msped_cuf_para_uf(cuf),
                               servico, metodo, operacao, versao, url, erro_cb, erro_cls);
    res = (NULL != *url);
feito:
    xmlXPathFreeContext(xpath_ctx);
    return res;
}

bool msped_enviar_envelope_soap12(struct msped_cfg *cfg, const char *url, const char *operacao,
                                  const char *dados_msg, const char *versao,
                                  msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                  msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                  msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    CURL *curl;
    char *namespace;
    char *soap_action;
    const char *headers[3];
    char *body;
    size_t tam_lote;
    curl = msped_curl_novo(cfg->pfx, cfg->pfx_senha, cfg->http_timeout, cfg->depuravel,
                           http_escrita_cb, http_escrita_cls);
    if (NULL == curl) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "curl");
        return NULL;
    }
    namespace = msped_montar_namespace(cfg->tipo, operacao);
    soap_action = msped_montar_header_soap_action(namespace);
    headers[0] = MSPED_HEADER_SOAP_CONTENT_TYPE;
    headers[1] = soap_action;
    headers[2] = NULL;
    body = msped_montar_envelope_soap12(cfg->tipo, namespace, cfg->cuf, versao, dados_msg);
    tam_lote = strlen(body);
    if (tam_lote > MSPED_LOTE_TAMANHO_MAX) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_LIM_TAM_LOTE_ERR, tam_lote, mu_size_unit(tam_lote));
        res = false;
    } else
        res = msped_curl_executar(curl, url, headers, body, cfg->depuravel,
                                  http_tentativa_cb, http_tentativa_cls, erro_cb, erro_cls);
    msped_curl_liberar(curl);
    free(soap_action);
    free(namespace);
    free(body);
    return res;
}

const char *msped_qrcode_param_url(const char *valor) {
    return (mu_is_empty(valor) ? "" : valor);
}

char *msped_qrcode_url_criar(const char *url, const char *ch_nfe, const char *versao, const char *tp_amb,
                             const char *c_dest, const char *dh_emi, const char *v_nf, const char *v_icms,
                             const char *dig_val, const char *id_token, const char *token,
                             msped_erro_callback erro_cb, void *erro_cls) {
    char *res;
    char *seq;
    const char *p_url;
    const char *p_c_dest;
    char *p_dh_emi;
    char *p_dig_val;
    char *partes;
    char *hash;
    if (mu_is_empty(url)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "url");
        return NULL;
    }
    if (mu_is_empty(ch_nfe)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "ch_nfe");
        return NULL;
    }
    if (mu_is_empty(versao)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "versao");
        return NULL;
    }
    if (mu_is_empty(tp_amb)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "tp_amb");
        return NULL;
    }
    if (mu_is_empty(dh_emi)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "dh_emi");
        return NULL;
    }
    if (mu_is_empty(v_nf)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "v_nf");
        return NULL;
    }
    if (mu_is_empty(v_icms)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "v_icms");
        return NULL;
    }
    if (mu_is_empty(dig_val)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "dig_val");
        return NULL;
    }
    if (mu_is_empty(id_token)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "id_token");
        return NULL;
    }
    if (mu_is_empty(token)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "token");
        return NULL;
    }
    p_url = msped_qrcode_param_url(url);
    p_c_dest = msped_qrcode_param_url(c_dest);
    p_dh_emi = mu_stoh(msped_qrcode_param_url(dh_emi));
    p_dig_val = mu_stoh(msped_qrcode_param_url(dig_val));
    seq = mu_fmt(MSPED_QRCODE_SEQ_FMT,
                            msped_qrcode_param_url(ch_nfe),
                            msped_qrcode_param_url(versao),
                            msped_qrcode_param_url(tp_amb),
                            (strcmp("", p_c_dest) == 0 ? "" : "&cDest="), (strcmp("", p_c_dest) == 0 ? "" : p_c_dest),
                            p_dh_emi,
                            msped_qrcode_param_url(v_nf),
                            msped_qrcode_param_url(v_icms),
                            p_dig_val,
                            msped_qrcode_param_url(id_token));
    free(p_dh_emi);
    free(p_dig_val);
    partes = mu_fmt("%s%s", seq, token);
    partes[strlen(partes)] = '\0';
    hash = msped_sha1_criar_hash(partes);
    free(partes);
    res = mu_fmt(MSPED_QRCODE_URL_FMT,
                            p_url, (NULL == strchr(p_url, '?') ? "?" : ""),
                            seq,
                            hash);
    free(hash);
    free(seq);
    return res;
}

void msped_inicializar() {
    mxml_xml_init();
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void msped_finalizar() {
    curl_global_cleanup();
    mxml_xml_finit();
}

struct msped_cfg *msped_cfg_arquivo_novo(enum MSPED_TIPO tipo, enum MSPED_CUF cuf, enum MSPED_AMBIENTE ambiente,
                                         enum MSPED_SV sv, enum MSPED_NF_MODELO modelo, const char *arquivo_modelo,
                                         const char *pfx, const char *pfx_senha,
                                         const char *n_versao, const char *token, const char *id_token,
                                         long http_timeout, bool depuravel,
                                         msped_erro_callback erro_cb, void *erro_cls) {
    struct msped_cfg *cfg;
    if (!msped_validar_tipo(tipo)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_INV, "tipo");
        return NULL;
    }
    if (!msped_validar_cuf(cuf)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_INV, "cuf");
        return NULL;
    }
    if (!msped_validar_ambiente(ambiente)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_INV, "ambiente");
        return NULL;
    }
    if (NULL == arquivo_modelo) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "arquivo_modelo");
        return NULL;
    }
    if (NULL == pfx) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "pfx");
        return false;
    }
    if (NULL == pfx_senha) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "pfx_senha");
        return false;
    }
    if (!mu_exists(arquivo_modelo)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARQ_NAO_ENC, arquivo_modelo);
        return false;
    }
    if (NULL != pfx && !mu_exists(pfx)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARQ_NAO_ENC, pfx);
        return false;
    }
    if (NULL == (cfg = malloc(sizeof(struct msped_cfg))))
        return NULL;
    memset(cfg, 0, sizeof(struct msped_cfg));
    cfg->modelo_nf = xmlParseFile(arquivo_modelo);
    if (NULL == cfg->modelo_nf) {
        free(cfg);
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "modelo_nf");
        return NULL;
    }
    cfg->depuravel = depuravel;
    cfg->pfx = (mu_is_empty(pfx) ? NULL : strdup(pfx));
    cfg->pfx_senha = (mu_is_empty(pfx_senha) ? NULL : strdup(pfx_senha));
    cfg->http_timeout = http_timeout;
    cfg->portal = msped_montar_portal(tipo);
    cfg->n_versao = (mu_is_empty(n_versao) ? NULL : strdup(n_versao));
    cfg->token = (mu_is_empty(token) ? NULL : strdup(token));
    cfg->id_token = (mu_is_empty(id_token) ? NULL : strdup(id_token));
    cfg->tipo = tipo;
    cfg->cuf = cuf;
    cfg->ambiente = ambiente;
    cfg->sv = sv;
    cfg->modelo = modelo;
    cfg->ok = true;
    return cfg;
}

void msped_cfg_liberar(struct msped_cfg *cfg) {
    if (NULL != cfg) {
        xmlFreeDoc(cfg->modelo_nf);
        free(cfg->portal);
        free(cfg->pfx);
        free(cfg->pfx_senha);
        free(cfg->token);
        free(cfg->id_token);
        free(cfg->n_versao);
        free(cfg);
    }
}

bool msped_cfg_ok(struct msped_cfg *cfg) {
    return (NULL != cfg && cfg->ok);
}

bool msped_cfg_ws_info(struct msped_cfg *cfg, const char *servico,
                       char **metodo, char **operacao, char **versao, char **sv_str, char **url,
                       msped_erro_callback erro_cb, void *erro_cls) {
    *metodo = NULL;
    *operacao = NULL;
    *url = NULL;
    if (NULL == cfg) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "cfg");
        return false;
    }
    if (NULL == cfg->modelo_nf) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "modelo_nf");
        return false;
    }
    return msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cfg->cuf, cfg->sv, servico,
                         metodo, operacao, versao, sv_str, url, erro_cb, erro_cls);
}

bool msped_consultar_disponibilidade(struct msped_cfg *cfg, const char *servico,
                                     msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    CURL *curl;
    char *metodo;
    char *operacao;
    char *versao;
    char *sv_str;
    char *url;
    if (!msped_validar_cfg(cfg, erro_cb, erro_cls))
        return false;
    if (!msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cfg->cuf, cfg->sv, servico,
                       &metodo, &operacao, &versao, &sv_str, &url, erro_cb, erro_cls)) {
        if (MSPED_NF_NENHUM != cfg->modelo)
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_MOD_IND, msped_modelo_para_str(cfg->modelo),
                        msped_cuf_para_uf(cfg->cuf), servico);
        else
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_IND, msped_cuf_para_uf(cfg->cuf), servico);
        return false;
    }
    curl = msped_curl_novo(cfg->pfx, cfg->pfx_senha, cfg->http_timeout, cfg->depuravel, NULL, NULL);
    if (NULL == curl) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "curl");
        return NULL;
    }
    res = msped_curl_executar(curl, url, NULL, NULL, cfg->depuravel, NULL, NULL, erro_cb, erro_cls);
    msped_curl_liberar(curl);
    return res;
}

bool msped_consultar_situacao(struct msped_cfg *cfg, const char *chave,
                              msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                              msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                              msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    char *cons_sit;
    const char *servico;
    enum MSPED_CUF cuf;
    enum MSPED_NF_MODELO modelo;
    char *metodo;
    char *operacao;
    char *versao;
    char *sv_str;
    char *url;
    if (!msped_validar_cfg(cfg, erro_cb, erro_cls))
        return false;
    if (cfg->depuravel) // força escrita pendente na `stdout`
        fflush(stdout);
    if (mu_is_empty(chave)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "chave");
        return false;
    }
    if (!msped_validar_chave(chave)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_CHV_INV, chave);
        return false;
    }
    servico = "NfeConsultaProtocolo";
    if (!msped_extrair_cuf_modelo(chave, &cuf, &modelo))
        return false;
    if (!msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cuf, cfg->sv, servico,
                       &metodo, &operacao, &versao, &sv_str, &url, erro_cb, erro_cls)) {
        if (MSPED_NF_NENHUM != modelo)
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_MOD_IND, msped_modelo_para_str(modelo), msped_cuf_para_uf(cuf),
                        servico);
        else
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_IND, msped_cuf_para_uf(cuf), servico);
        return false;
    }
    cons_sit = msped_montar_cons_sit(cfg->tipo, cfg->portal, versao, cfg->ambiente, chave);
    res = msped_enviar_envelope_soap12(cfg, url, operacao, cons_sit, versao, http_escrita_cb, http_escrita_cls,
                                       http_tentativa_cb, http_tentativa_cls, erro_cb, erro_cls);
    free(cons_sit);
    return res;
}

bool msped_consultar_status_servico(struct msped_cfg *cfg,
                                    msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                                    msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                                    msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    char *cons_stat_serv;
    const char *servico;
    char *metodo;
    char *operacao;
    char *versao;
    char *sv_str;
    char *url;
    if (!msped_validar_cfg(cfg, erro_cb, erro_cls))
        return false;
    servico = "NfeStatusServico";
    if (!msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cfg->cuf, cfg->sv, servico,
                       &metodo, &operacao, &versao, &sv_str, &url, erro_cb, erro_cls)) {
        if (MSPED_NF_NENHUM != cfg->modelo)
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_MOD_IND, msped_modelo_para_str(cfg->modelo),
                        msped_cuf_para_uf(cfg->cuf), servico);
        else
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_IND, msped_cuf_para_uf(cfg->cuf), servico);
        return false;
    }
    cons_stat_serv = msped_montar_cons_stat_serv(cfg->tipo, cfg->portal, versao, cfg->ambiente, cfg->cuf);
    res = msped_enviar_envelope_soap12(cfg, url, operacao, cons_stat_serv, versao, http_escrita_cb, http_escrita_cls,
                                       http_tentativa_cb, http_tentativa_cls, erro_cb, erro_cls);
    free(cons_stat_serv);
    return res;
}

bool msped_consultar_cadastro(struct msped_cfg *cfg, enum MSPED_DOC_TIPO tipo_doc, const char *doc,
                              msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                              msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                              msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    char *cons_cad;
    const char *servico;
    char *metodo;
    char *operacao;
    char *versao;
    char *sv_str;
    char *url;
    if (!msped_validar_cfg(cfg, erro_cb, erro_cls))
        return false;
    servico = "NfeConsultaCadastro";
    if (!msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cfg->cuf, cfg->sv, servico,
                       &metodo, &operacao, &versao, &sv_str, &url, erro_cb, erro_cls)) {
        if (MSPED_NF_NENHUM != cfg->modelo)
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_MOD_IND, msped_modelo_para_str(cfg->modelo),
                        msped_cuf_para_uf(cfg->cuf), servico);
        else
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_IND, msped_cuf_para_uf(cfg->cuf), servico);
        return false;
    }
    cons_cad = msped_montar_cons_cad(cfg->portal, versao, cfg->cuf, tipo_doc, doc);
    res = msped_enviar_envelope_soap12(cfg, url, operacao, cons_cad, versao, http_escrita_cb, http_escrita_cls,
                                       http_tentativa_cb, http_tentativa_cls, erro_cb, erro_cls);
    free(cons_cad);
    return res;
}

struct msped_lote *msped_lote_novo(msped_erro_callback erro_cb, void *erro_cls) {
    struct msped_lote *lote;
    lote = malloc(sizeof(struct msped_lote));
    if (NULL == lote) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "lote");
        return NULL;
    }
    memset(lote, 0, sizeof(struct msped_lote));
    return lote;
}

void msped_lote_liberar(struct msped_lote *lote) {
    if (NULL != lote)
        free(lote);
}

bool msped_lote_ok(struct msped_lote *lote) {
    return (NULL != lote && lote->quantidade > 0);
}

bool msped_lote_arquivo_adicionar(struct msped_lote *lote, const char *arquivo_xml,
                                  msped_erro_callback erro_cb, void *erro_cls) {
    if (NULL == lote) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "lote");
        return false;
    }
    if (NULL == arquivo_xml) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "arquivo_xml");
        return false;
    }
    if (!mu_exists(arquivo_xml)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARQ_NAO_ENC, arquivo_xml);
        return false;
    }
    if (lote->quantidade > MSPED_LOTE_QTD_MAX_ITENS) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_LOTE_MAX_ERR, MSPED_LOTE_QTD_MAX_ITENS);
        return false;
    }
    lote->itens[lote->quantidade++] = arquivo_xml;
    return true;
}

const char *msped_lote_arquivo_obter_por_indice(struct msped_lote *lote, uint8_t indice,
                                                msped_erro_callback erro_cb, void *erro_cls) {
    if (NULL == lote) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "lote");
        return false;
    }
    if (indice < 0 || indice > lote->quantidade)
        return NULL;
    return lote->itens[indice];
}

int8_t msped_lote_contar(struct msped_lote *lote) {
    if (NULL == lote)
        return -1;
    return lote->quantidade;
}

char *msped_doc_obter_chave(void *doc, const char *tag) {
    char *res;
    char *ch;
    if (NULL == doc)
        return NULL;
    ch = mxml_xml_doc_get_attr_str(doc, (mu_is_empty(tag) ? "infNFe" : tag), "Id");
    res = mu_nums(ch);
    free(ch);
    return res;
}

bool msped_qrcode_acrescentar(struct msped_cfg *cfg, void *xml_doc, msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    xmlDocPtr doc;
    xmlNodePtr root;
    xmlNodePtr nfe;
    xmlNodePtr ide;
    xmlNodePtr dest;
    xmlNodePtr icmsTot;
    xmlNodePtr signedInfo;
    xmlNodePtr infNFeSupl;
    xmlNodePtr nodeqr;
    xmlNodePtr signature;
    char *chNFe;
    char *cUF;
    char *tpAmb;
    char *dhEmi;
    char *cDest;
    char *vNF;
    char *vICMS;
    char *digVal;
    const char *token;
    const char *idToken;
    const char *versao;
    char *servico;
    char *sv_metodo;
    char *sv_operacao;
    char *sv_versao;
    char *sv_str;
    char *url;
    char *qrcode;
    if (NULL == cfg) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "cfg");
        return false;
    }
    if (NULL == xml_doc) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "xml_doc");
        return false;
    }
    res = false;
    doc = xml_doc;
    root = xmlDocGetRootElement(doc);
    nfe = mxml_xml_node_find_node_by_name(root, "NFe");
    ide = mxml_xml_node_find_node_by_name(root, "ide");
    dest = mxml_xml_node_find_node_by_name(root, "dest");
    icmsTot = mxml_xml_node_find_node_by_name(root, "ICMSTot");
    signedInfo = mxml_xml_node_find_node_by_name(root, "SignedInfo");
    chNFe = msped_doc_obter_chave(doc, "infNFe");
    cUF = mxml_xml_node_get_value_str(ide, "cUF");
    tpAmb = mxml_xml_node_get_value_str(ide, "tpAmb");
    dhEmi = mxml_xml_node_get_value_str(ide, "dhEmi");
    cDest = NULL;
    if (NULL != dest) {
        cDest = mxml_xml_node_get_value_str(dest, "CNPJ");
        if (strcmp("", cDest) == 0) {
            cDest = mxml_xml_node_get_value_str(dest, "CPF");
            if (strcmp("", cDest) == 0)
                cDest = mxml_xml_node_get_value_str(dest, "idEstrangeiro");
        }
    }
    vNF = mxml_xml_node_get_value_str(icmsTot, "vNF");
    vICMS = mxml_xml_node_get_value_str(icmsTot, "vICMS");
    digVal = mxml_xml_node_get_value_str(signedInfo, "DigestValue");
    token = cfg->token;
    idToken = (mu_is_empty(cfg->id_token), "000001", cfg->id_token);
    versao = (mu_is_empty(cfg->n_versao), "100", cfg->n_versao);
    servico = "NfeConsultaQR";
    if (!msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cfg->cuf, cfg->sv, servico,
                       &sv_metodo, &sv_operacao, &sv_versao, &sv_str, &url, erro_cb, erro_cls)) {
        if (MSPED_NF_NENHUM != cfg->modelo)
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_MOD_IND, msped_modelo_para_str(cfg->modelo),
                        msped_cuf_para_uf(cfg->cuf), servico);
        else
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_IND, msped_cuf_para_uf(cfg->cuf), servico);
        goto feito;
    }
    qrcode = msped_qrcode_url_criar(url, chNFe, versao, tpAmb, cDest, dhEmi, vNF, vICMS, digVal, idToken, token,
                                    erro_cb, erro_cls);
    if (mu_is_empty(qrcode)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_QRCODE_ERR, chNFe);
        goto feito;
    }
    infNFeSupl = mxml_xml_doc_create_empty_element(doc, "infNFeSupl");
    nodeqr = mxml_xml_node_append_child(infNFeSupl, mxml_xml_doc_create_empty_element(doc, "qrCode"));
    mxml_xml_node_append_child(nodeqr, mxml_xml_doc_create_cdata_section(doc, qrcode, 0));
    signature = mxml_xml_node_find_node_by_name(nfe, "Signature");
    if (NULL == signature) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_XML_ELEM_ERR, "Signature");
        goto feito;
    }
    mxml_xml_node_insert_before(signature, infNFeSupl);
    res = true;
    free(qrcode);
feito:
    free(chNFe);
    free(cUF);
    free(tpAmb);
    free(dhEmi);
    free(cDest);
    free(vNF);
    free(vICMS);
    free(digVal);
    if (res && cfg->depuravel)
        xmlDocFormatDump(stdout, xml_doc, 1);
    return res;
}

bool msped_enviar_lote(struct msped_cfg *cfg, struct msped_lote *lote, enum MSPED_PROCESSO_TIPO processo,
                       msped_lote_geraracao_id_callback lote_gera_id_cb, void *lote_gera_id_cls,
                       msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                       msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                       msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    char *envi_lote;
    const char *servico;
    char *metodo;
    char *operacao;
    char *versao;
    char *sv_str;
    char *url;
    uint64_t id_lote;
    uint8_t ind_sinc;
    const char *item;
    char *nfe;
    char *tmp_str;
    size_t nfe_len;
    char *xmls;
    if (NULL == cfg) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "cfg");
        return false;
    }
    if (NULL == lote) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "lote");
        return false;
    }
    if (MSPED_PROC_TIPO_NENHUM == processo) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "processo");
        return false;
    }
    if (!msped_validar_cfg(cfg, erro_cb, erro_cls))
        return false;
    servico = "NfeAutorizacao";
    if (!msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cfg->cuf, cfg->sv, servico,
                       &metodo, &operacao, &versao, &sv_str, &url, erro_cb, erro_cls)) {
        if (MSPED_NF_NENHUM != cfg->modelo)
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_MOD_IND, msped_modelo_para_str(cfg->modelo),
                        msped_cuf_para_uf(cfg->cuf), servico);
        else
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_IND, msped_cuf_para_uf(cfg->cuf), servico);
        return false;
    }
    if (NULL != lote_gera_id_cb)
        id_lote = lote_gera_id_cb(lote_gera_id_cls);
    else
        id_lote = 0;
    if (processo == MSPED_PROC_TIPO_SINCRONO)
        ind_sinc = 1;
    else
        ind_sinc = 0;
    if (lote->quantidade < 1) {
        _MSPED_ERR(erro_cb, erro_cls, "%s", S_MSPED_SEM_NFE_ERR);
        return false;
    }
    xmls = NULL;
    for (int i = 0; i < lote->quantidade; i++)
        if (NULL != (item = lote->itens[i])) {
            if (!mu_exists(item)) {
                _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARQ_NAO_ENC, item);
                free(xmls);
                return false;
            }
            nfe = mu_ftos(item);
            if (NULL == nfe) {
                _MSPED_ERR(erro_cb, erro_cls, S_MSPED_VAR_ERR, "xmls");
                free(xmls);
                return false;
            }
            tmp_str = mu_rtrim(nfe);
            free(nfe);
            nfe = tmp_str;
            nfe_len = strlen(nfe);
            if (nfe_len > 256) { // tamanho mínimo de uma nota
                if (NULL == xmls)
                    xmls = nfe;
                else {
                    tmp_str = realloc(xmls, strlen(nfe) + 1);
                    xmls = tmp_str;
                    free(tmp_str);
                    strcat(xmls, nfe);
                    free(nfe);
                }
            } else
                free(nfe);
        }
    if (NULL == xmls) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_VAR_ERR, "xmls");
        free(xmls);
        return false;
    }
    xmls[strlen(xmls)] = '\0';
    envi_lote = msped_montar_envi_lote(cfg->tipo, cfg->portal, versao, id_lote, ind_sinc, xmls);
    res = msped_enviar_envelope_soap12(cfg, url, operacao, envi_lote, versao, http_escrita_cb, http_escrita_cls,
                                       http_tentativa_cb, http_tentativa_cls, erro_cb, erro_cls);
    free(xmls);
    free(envi_lote);
    return res;
}

bool msped_consultar_recibo(struct msped_cfg *cfg, uint64_t recibo,
                            msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                            msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                            msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    char *cons_reci;
    const char *servico;
    char *metodo;
    char *operacao;
    char *versao;
    char *sv_str;
    char *url;
    if (!msped_validar_cfg(cfg, erro_cb, erro_cls))
        return false;
    if (recibo < 1) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "recibo");
        return false;
    }
    servico = "NfeRetAutorizacao";
    if (!msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cfg->cuf, cfg->sv, servico,
                       &metodo, &operacao, &versao, &sv_str, &url, erro_cb, erro_cls)) {
        if (MSPED_NF_NENHUM != cfg->modelo)
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_MOD_IND, msped_modelo_para_str(cfg->modelo),
                        msped_cuf_para_uf(cfg->cuf), servico);
        else
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_IND, msped_cuf_para_uf(cfg->cuf), servico);
        return false;
    }
    cons_reci = msped_montar_cons_reci(cfg->tipo, cfg->portal, versao, cfg->ambiente, recibo);
    res = msped_enviar_envelope_soap12(cfg, url, operacao, cons_reci, versao, http_escrita_cb, http_escrita_cls,
                                       http_tentativa_cb, http_tentativa_cls, erro_cb, erro_cls);
    free(cons_reci);
    return res;
}

bool msped_enviar_evento(struct msped_cfg *cfg, enum MSPED_DOC_TIPO tipo_doc, const char *doc, const char *chave,
                         const char *tp_evento, uint8_t n_seq_evento, const char *tag_adic,
                         msped_evento_assinatura_msg_callback assin_msg_cb, void *assin_msg_cls,
                         msped_lote_geraracao_id_callback lote_gera_id_cb, void *lote_gera_id_cls,
                         msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                         msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                         msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    const char *signed_msg;
    uint64_t num_lote;
    char *mensagem;
    char *env_evento;
    const char *servico;
    enum MSPED_CUF cuf;
    enum MSPED_NF_MODELO modelo;
    char *metodo;
    char *operacao;
    char *versao;
    char *sv_str;
    char *url;
    if (!msped_validar_cfg(cfg, erro_cb, erro_cls))
        return false;
    if (cfg->depuravel) // força escrita pendente na `stdout`
        fflush(stdout);
    if (mu_is_empty(chave)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "chave");
        return false;
    }
    if (!msped_validar_chave(chave)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_CHV_INV, chave);
        return false;
    }
    if (NULL == assin_msg_cb) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "assin_msg_cb");
        return false;
    }
    if (NULL == lote_gera_id_cb) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "lote_gera_id_cb");
        return false;
    }
    if (0 == n_seq_evento) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "n_seq_evento");
        return false;
    }
    if (n_seq_evento < 1 || n_seq_evento > 20) {
        _MSPED_ERR(erro_cb, erro_cls, "%s", S_MSPED_EV_NUM_SEQ_ERR);
        return false;
    }
    servico = "RecepcaoEvento";
    if (!msped_extrair_cuf_modelo(chave, &cuf, &modelo))
        return false;
    if (!msped_ws_info(cfg->modelo_nf, cfg->modelo, cfg->tipo, cfg->ambiente, cuf, cfg->sv, servico,
                       &metodo, &operacao, &versao, &sv_str, &url, erro_cb, erro_cls)) {
        if (MSPED_NF_NENHUM != modelo)
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_MOD_IND, msped_modelo_para_str(modelo), msped_cuf_para_uf(cuf),
                        servico);
        else
            _MSPED_ERR(erro_cb, erro_cls, S_MSPED_SRV_IND, msped_cuf_para_uf(cuf), servico);
        return false;
    }
    mensagem = msped_montar_evento(cfg->tipo, cfg->portal, versao, cfg->ambiente, cfg->cuf, tipo_doc, doc, chave,
                                   tp_evento, n_seq_evento, tag_adic);
    if (!assin_msg_cb(assin_msg_cls, mensagem, "infEvento", &signed_msg)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_EV_ASS_MSG_ERR, mensagem);
        free(mensagem);
        return false;
    }
    free(mensagem);
    num_lote = lote_gera_id_cb(lote_gera_id_cls);
    env_evento = msped_montar_env_evento(cfg->portal, versao, num_lote, signed_msg);
    res = msped_enviar_envelope_soap12(cfg, url, operacao, env_evento, versao, http_escrita_cb, http_escrita_cls,
                                       http_tentativa_cb, http_tentativa_cls, erro_cb, erro_cls);
    free(env_evento);
    return res;
}

char *msped_atribuir_prot_msg(const char *tagproc, const char *tagmsg, const char *xmlmsg, const char *tagretorno,
                              const char *xmlretorno, bool *atribuido,
                              msped_atribuir_prot_msg_callback atrib_cb, void *atrib_cls,
                              msped_erro_callback erro_cb, void *erro_cls) {
    xmlDocPtr doc;
    xmlDocPtr proc;
    xmlNodePtr nodedoc;
    char *procver;
    char *procns;
    xmlDocPtr doc1;
    xmlNodePtr nodedoc1;
    xmlNodePtr proc_node;
    xmlAttrPtr proc_node_att1;
    xmlAttrPtr proc_node_att2;
    xmlNodePtr node;
    char *proc_xml;
    int proc_xml_len;
    char *res;
    doc = xmlParseDoc(BAD_CAST xmlmsg);
    if (NULL == doc) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "doc");
        return NULL;
    }
    res = NULL;
    doc1 = NULL;
    procns = NULL;
    proc = NULL;
    nodedoc = mxml_xml_doc_find_node_by_name(doc, tagmsg);
    procver = mxml_xml_node_dom_element_get_attribute(nodedoc, "versao");
    if (mu_is_empty(procver)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_XML_ATRI_ERR, "versao");
        goto feito;
    }
    procns = mxml_xml_node_dom_element_get_attribute(nodedoc, "xmlns");
    if (mu_is_empty(procns)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_XML_ATRI_ERR, "xmlns");
        goto feito;
    }
    doc1 = xmlParseDoc(BAD_CAST xmlretorno);
    if (NULL == doc1) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "doc1");
        goto feito;
    }
    nodedoc1 = mxml_xml_doc_find_node_by_name(doc1, tagretorno);
    proc = xmlNewDoc(BAD_CAST "1.0");
    proc_node = mxml_xml_doc_create_empty_element(proc, tagproc);
    mxml_xml_node_append_child(proc, proc_node);
    proc_node_att1 = mxml_xml_node_append_child(proc_node, mxml_xml_doc_create_attribute(proc, "versao"));
    mxml_xml_node_append_child(proc_node_att1, mxml_xml_doc_create_text_node(proc, procver));
    proc_node_att2 = mxml_xml_node_append_child(proc_node, mxml_xml_doc_create_attribute(proc, "xmlns"));
    mxml_xml_node_append_child(proc_node_att2, mxml_xml_doc_create_text_node(proc, procns));
    node = mxml_xml_doc_import_node(proc, nodedoc, true);
    mxml_xml_node_append_child(proc_node, node);
    node = mxml_xml_doc_import_node(proc, nodedoc1, true);
    mxml_xml_node_append_child(proc_node, node);
    xmlDocDumpFormatMemoryEnc(proc, (xmlChar **) &proc_xml, &proc_xml_len, "UTF-8", 0);
    res = msped_limpar_prot(proc_xml);
    free(proc_xml);
    if (NULL != atrib_cb)
        *atribuido = atrib_cb(atrib_cls, doc1);
feito:
    free(procver);
    free(procns);
    if (NULL != doc)
        xmlFreeDoc(doc);
    if (NULL != doc1)
        xmlFreeDoc(doc1);
    if (NULL != proc)
        xmlFreeDoc(proc);
    return res;
}

bool msped_enviar_evento_cce(struct msped_cfg *cfg, enum MSPED_DOC_TIPO tipo_doc, const char *doc, const char *chave,
                             const char *x_correcao, uint8_t n_seq_evento,
                             msped_evento_assinatura_msg_callback assin_msg_cb, void *assin_msg_cls,
                             msped_lote_geraracao_id_callback lote_gera_id_cb, void *lote_gera_id_cls,
                             msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                             msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                             msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    const char *tp_evento;
    const char *x_cond_uso;
    char *tag_adic;
    size_t x_correcao_len;
    if (mu_is_empty(x_correcao)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "x_correcao");
        return false;
    }
    x_correcao_len = strlen(x_correcao);
    if (x_correcao_len < 15 || x_correcao_len > 1000) {
        _MSPED_ERR(erro_cb, erro_cls, "%s", S_MSPED_EV_COR_ERR);
        return false;
    }
    tp_evento = "110110";
    x_cond_uso = "A Carta de Correcao e disciplinada pelo paragrafo "
            "1o-A do art. 7o do Convenio S/N, de 15 de dezembro de 1970 "
            "e pode ser utilizada para regularizacao de erro ocorrido "
            "na emissao de documento fiscal, desde que o erro nao esteja "
            "relacionado com: I - as variaveis que determinam o valor "
            "do imposto tais como: base de calculo, aliquota, diferenca "
            "de preco, quantidade, valor da operacao ou da prestacao; "
            "II - a correcao de dados cadastrais que implique mudanca "
            "do remetente ou do destinatario; "
            "III - a data de emissao ou de saida.";
    tag_adic = mu_fmt("<xCorrecao>%s</xCorrecao><xCondUso>%s</xCondUso>", x_correcao, x_cond_uso);
    res = msped_enviar_evento(cfg, tipo_doc, doc, chave, tp_evento, n_seq_evento, tag_adic,
                              assin_msg_cb, assin_msg_cls, lote_gera_id_cb, lote_gera_id_cls,
                              http_escrita_cb, http_escrita_cls, http_tentativa_cb, http_tentativa_cls,
                              erro_cb, erro_cls);
    free(tag_adic);
    return res;
}

bool msped_enviar_evento_canc(struct msped_cfg *cfg, enum MSPED_DOC_TIPO tipo_doc, const char *doc, const char *chave,
                              const char *x_just, const char *n_prot,
                              msped_evento_assinatura_msg_callback assin_msg_cb, void *assin_msg_cls,
                              msped_lote_geraracao_id_callback lote_gera_id_cb, void *lote_gera_id_cls,
                              msped_http_escrita_callback http_escrita_cb, void *http_escrita_cls,
                              msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                              msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    const char *tp_evento;
    char *tag_adic;
    uint8_t n_seq_evento;
    size_t x_just_len;
    if (mu_is_empty(x_just)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "x_just");
        return false;
    }
    if (mu_is_empty(n_prot)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "n_prot");
        return false;
    }
    x_just_len = strlen(x_just);
    if (x_just_len < 15 || x_just_len > 255) {
        _MSPED_ERR(erro_cb, erro_cls, "%s", S_MSPED_EV_JUS_ERR);
        return false;
    }
    tp_evento = "110111";
    n_seq_evento = 1;
    tag_adic = mu_fmt("<nProt>%s</nProt><xJust>%s</xJust>", n_prot, x_just);
    res = msped_enviar_evento(cfg, tipo_doc, doc, chave, tp_evento, n_seq_evento, tag_adic,
                              assin_msg_cb, assin_msg_cls, lote_gera_id_cb, lote_gera_id_cls,
                              http_escrita_cb, http_escrita_cls, http_tentativa_cb, http_tentativa_cls,
                              erro_cb, erro_cls);
    free(tag_adic);
    return res;
}