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

#include "msped_strs.h"
#include "msped_macros.h"
#include "msped_xpath.h"

char *msped_xpath_servico_expr(enum MSPED_AMBIENTE ambiente, const char *sv_str, const char *servico,
                               msped_erro_callback erro_cb, void *erro_cls) {
    if (!msped_validar_ambiente(ambiente)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "ambiente");
        return NULL;
    }
    if (NULL == sv_str) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "sv_str");
        return NULL;
    }
    if (NULL == servico) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "servico");
        return NULL;
    }
    return mu_fmt("/WS/UF[sigla='%s']/%s/%s", sv_str, msped_ambiente_para_str(ambiente), servico);
}

bool msped_xpath_parse_node(xmlXPathContextPtr xpath_ctx, enum MSPED_AMBIENTE ambiente, char *sv_str,
                            const char *servico, char **metodo, char **operacao, char **versao, char **url,
                            msped_erro_callback erro_cb, void *erro_cls) {
    bool res;
    xmlXPathObjectPtr xpath;
    xmlNodePtr node;
    struct _xmlAttr *props;
    char *xpath_expr;
    int i;
    res = false;
    xpath = NULL;
    xpath_expr = msped_xpath_servico_expr(ambiente, sv_str, servico, erro_cb, erro_cls);
    if (NULL == xpath_expr) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_VAR_ERR, "xpath_expr");
        goto feito;
    }
    xpath = xmlXPathEvalExpression(BAD_CAST xpath_expr, xpath_ctx);
    if (NULL == xpath) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "xpath");
        goto feito;
    }
    if (NULL != xpath->nodesetval) {
        for (i = 0; i < xpath->nodesetval->nodeNr; i++) {
            node = xpath->nodesetval->nodeTab[i];
            if (node->type == XML_ELEMENT_NODE) {
                props = node->properties;
                if (NULL != props) {
                    if (NULL != props->children) {
                        *metodo = (char *) props->children->content;
                        props = node->properties->next;
                        if (NULL != props && NULL != props->children) {
                            *operacao = (char *) props->children->content;
                            props = props->next;
                            if (NULL != props && NULL != props->children)
                                *versao = (char *) props->children->content;
                        }
                    }
                    if (NULL != node->children)
                        *url = (char *) node->children->content;
                }
            }
        }
    }
    res = true;
feito:
    free(xpath_expr);
    xmlXPathFreeObject(xpath);
    return res;
}