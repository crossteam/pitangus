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

#include <string.h>
#include "msped_ws_defs.h"
#include "msped_strs.h"
#include "msped_macros.h"

const char *msped_obter_sv_str(enum MSPED_TIPO tipo, enum MSPED_NF_MODELO modelo, enum MSPED_CUF cuf,
                               enum MSPED_SV sv) {
    if (MSPED_TIPO_MDFE == tipo)
        return "RS";
    switch (sv) {
        case MSPED_SV_NENHUM:
            break;
        case MSPED_SVAN:
            return "SVAN";
        case MSPED_SVRS:
            return "SVRS";
        case MSPED_SVCAN:
            return "SVCAN";
        case MSPED_SVCRS:
            return "SVCRS";
        default:
            break;
    }
    switch (modelo) {
        case MSPED_NF_NENHUM:
            return S_MSPED_NENHUM;
        case MSPED_NF_MODELO_55:
            switch (cuf) {
                case MSPED_CUF_NENHUM:
                    return S_MSPED_NENHUM;
                case MSPED_CUF_AC:
                    return "SVRS";
                case MSPED_CUF_AL:
                    return "SVRS";
                case MSPED_CUF_AM:
                    return "AM";
                case MSPED_CUF_AN:
                    return "AN";
                case MSPED_CUF_AP:
                    return "SVRS";
                case MSPED_CUF_BA:
                    return "BA";
                case MSPED_CUF_CE:
                    return "CE";
                case MSPED_CUF_DF:
                    return "SVRS";
                case MSPED_CUF_ES:
                    return "SVRS";
                case MSPED_CUF_GO:
                    return "GO";
                case MSPED_CUF_MA:
                    return "SVAN";
                case MSPED_CUF_MG:
                    return "MG";
                case MSPED_CUF_MS:
                    return "MS";
                case MSPED_CUF_MT:
                    return "MT";
                case MSPED_CUF_PA:
                    return "SVAN";
                case MSPED_CUF_PB:
                    return "SVRS";
                case MSPED_CUF_PE:
                    return "PE";
                case MSPED_CUF_PI:
                    return "SVAN";
                case MSPED_CUF_PR:
                    return "PR";
                case MSPED_CUF_RJ:
                    return "SVRS";
                case MSPED_CUF_RN:
                    return "SVRS";
                case MSPED_CUF_RO:
                    return "SVRS";
                case MSPED_CUF_RR:
                    return "SVRS";
                case MSPED_CUF_RS:
                    return "RS";
                case MSPED_CUF_SC:
                    return "SVRS";
                case MSPED_CUF_SE:
                    return "SVRS";
                case MSPED_CUF_SP:
                    return "SP";
                case MSPED_CUF_TO:
                    return "SVRS";

            }
        case MSPED_NF_MODELO_65:
            switch (cuf) {
                case MSPED_CUF_NENHUM:
                    return S_MSPED_NENHUM;
                case MSPED_CUF_AC:
                    return "SVRS";
                case MSPED_CUF_AL:
                    return "SVRS";
                case MSPED_CUF_AM:
                    return "AM";
                case MSPED_CUF_AN:
                    return "AN";
                case MSPED_CUF_AP:
                    return "SVRS";
                case MSPED_CUF_BA:
                    return "SVRS";
                case MSPED_CUF_CE:
                    return "CE";
                case MSPED_CUF_DF:
                    return "SVRS";
                case MSPED_CUF_ES:
                    return "SVRS";
                case MSPED_CUF_GO:
                    return "GO";
                case MSPED_CUF_MA:
                    return "SVRS";
                case MSPED_CUF_MG:
                    return "MG";
                case MSPED_CUF_MS:
                    return "MS";
                case MSPED_CUF_MT:
                    return "MT";
                case MSPED_CUF_PA:
                    return "SVRS";
                case MSPED_CUF_PB:
                    return "SVRS";
                case MSPED_CUF_PE:
                    return "PE";
                case MSPED_CUF_PI:
                    return "SVRS";
                case MSPED_CUF_PR:
                    return "PR";
                case MSPED_CUF_RJ:
                    return "SVRS";
                case MSPED_CUF_RN:
                    return "SVRS";
                case MSPED_CUF_RO:
                    return "SVRS";
                case MSPED_CUF_RR:
                    return "SVRS";
                case MSPED_CUF_RS:
                    return "RS";
                case MSPED_CUF_SC:
                    return "SVRS";
                case MSPED_CUF_SE:
                    return "SVRS";
                case MSPED_CUF_SP:
                    return "SP";
                case MSPED_CUF_TO:
                    return "SVRS";
            }
        default:
            return NULL;
    }
}

char *msped_montar_header_soap_action(const char *namespace) {
    return mu_fmt(MSPED_HEADER_SOAP_ACTION, namespace);
}

const char *msped_montar_tipo_minusculo(enum MSPED_TIPO tipo) {
    switch (tipo) {
        case MSPED_TIPO_NFE:
            return "nfe";
        case MSPED_TIPO_CTE:
            return "cte";
        case MSPED_TIPO_MDFE:
            return "mdfe";
        default:
            return "nfe";
    }
}

const char *msped_montar_tipo_normal(enum MSPED_TIPO tipo, bool permite_vazio) {
    switch (tipo) {
        case MSPED_TIPO_NFE:
            if (permite_vazio)
                return "";
            else
                return "NFe";
        case MSPED_TIPO_CTE:
            return "CTe";
        case MSPED_TIPO_MDFE:
            return "MDFe";
        default:
            return "NFe";
    }
}

const char *msped_montar_tipo_doc(enum MSPED_DOC_TIPO tipo_doc) {
    switch (tipo_doc) {
        case MSPED_DOC_TIPO_NENHUM:
            return S_MSPED_NENHUM;
        case MSPED_DOC_TIPO_CPF:
            return "CPF";
        case MSPED_DOC_TIPO_CNPJ:
            return "CNPJ";
        case MSPED_DOC_TIPO_IE:
            return "IE";
        default:
            return "CNPJ";
    }
}

char *msped_montar_portal(enum MSPED_TIPO tipo) {
    return mu_fmt(MSPED_PORTAL_FMT, msped_montar_tipo_minusculo(tipo));
}

char *msped_montar_namespace(enum MSPED_TIPO tipo, const char *operacao) {
    return mu_fmt(MSPED_NAMESPACE_FMT, msped_montar_tipo_minusculo(tipo), operacao);
}

char *msped_montar_cabec_msg(enum MSPED_TIPO tipo, const char *namespace, enum MSPED_CUF cuf, const char *versao) {
    const char *tp = msped_montar_tipo_minusculo(tipo);
    return mu_fmt(MSPED_CABEC_MSG_FMT, tp, namespace, cuf, versao, tp);
}

char *msped_montar_dados_msg(enum MSPED_TIPO tipo, const char *namespace, const char *dados) {
    const char *tp = msped_montar_tipo_minusculo(tipo);
    return mu_fmt(MSPED_DADOS_MSG_FMT, tp, namespace, dados, tp);
}

char *msped_montar_cons_sit(enum MSPED_TIPO tipo, const char *portal, const char *versao, enum MSPED_AMBIENTE ambiente,
                            const char *ch) {
    const char *tp = msped_montar_tipo_normal(tipo, false);
    return mu_fmt(MSPED_CONS_SIT_FMT, tp, portal, versao, ambiente, tp, ch, tp, tp);
}

char *msped_montar_cons_stat_serv(enum MSPED_TIPO tipo, const char *portal, const char *versao,
                                  enum MSPED_AMBIENTE ambiente, enum MSPED_CUF cuf) {
    const char *tp = msped_montar_tipo_normal(tipo, true);
    return mu_fmt(MSPED_CONS_STAT_SERV_FMT, tp, portal, versao, ambiente, cuf, tp);
}

char *msped_montar_cons_cad(const char *portal, const char *versao, enum MSPED_CUF cuf, enum MSPED_DOC_TIPO tipo_doc,
                            const char *doc) {
    const char *tpd = msped_montar_tipo_doc(tipo_doc);
    return mu_fmt(MSPED_CONS_CAD_FMT, portal, versao, msped_cuf_para_uf(cuf), tpd, doc, tpd);
}

char *msped_montar_envi_lote(enum MSPED_TIPO tipo, const char *portal, const char *versao, uint64_t id_lote,
                             uint8_t ind_sinc, const char *nfe) {
    const char *tp = msped_montar_tipo_normal(tipo, false);
    return mu_fmt(MSPED_ENV_LOTE_FMT, tp, portal, versao, id_lote, ind_sinc, nfe, tp);
}

char *msped_montar_cons_reci(enum MSPED_TIPO tipo, const char *portal, const char *versao, enum MSPED_AMBIENTE ambiente,
                             uint64_t recibo) {
    const char *tp = msped_montar_tipo_normal(tipo, false);
    return mu_fmt(MSPED_CONS_RECI_FMT, tp, portal, versao, ambiente, recibo, tp);
}

bool msped_extrair_info_evento(const char *tp_evento, const char **alias_evento, const char **desc_evento) {
    if (0 == strcmp(tp_evento, "110110")) {
        //CCe
        *alias_evento = "CCe";
        *desc_evento = "Carta de Correcao";
        return true;
    }
    if (0 == strcmp(tp_evento, "110111")) {
        //cancelamento
        *alias_evento = "CancNFe";
        *desc_evento = "Cancelamento";
        return true;
    }
    if (0 == strcmp(tp_evento, "110140")) {
        //EPEC
        //emissão em contingência EPEC
        *alias_evento = "EPEC";
        *desc_evento = "EPEC";
        return true;
    }
    if (0 == strcmp(tp_evento, "111500"))
        return true;
    if (0 == strcmp(tp_evento, "111501")) {
        //EPP
        //Pedido de prorrogação
        *alias_evento = "EPP";
        *desc_evento = "Pedido de Prorrogacao";
        return true;
    }
    if (0 == strcmp(tp_evento, "111502"))
        return true;
    if (0 == strcmp(tp_evento, "111503")) {
        //ECPP
        //Cancelamento do Pedido de prorrogação
        *alias_evento = "ECPP";
        *desc_evento = "Cancelamento de Pedido de Prorrogacao";
        return true;
    }
    if (0 == strcmp(tp_evento, "210200")) {
        //Confirmacao da Operacao
        *alias_evento = "EvConfirma";
        *desc_evento = "Confirmacao da Operacao";
        return true;
    }
    if (0 == strcmp(tp_evento, "210210")) {
        //Ciencia da Operacao
        *alias_evento = "EvCiencia";
        *desc_evento = "Ciencia da Operacao";
        return true;
    }
    if (0 == strcmp(tp_evento, "210220")) {
        //Desconhecimento da Operacao
        *alias_evento = "EvDesconh";
        *desc_evento = "Desconhecimento da Operacao";
        return true;
    }
    if (0 == strcmp(tp_evento, "210240")) {
        //Operacao não Realizada
        *alias_evento = "EvNaoRealizada";
        *desc_evento = "Operacao nao Realizada";
        return true;
    };
    return false;
}

char *msped_montar_evento(enum MSPED_TIPO tipo, const char *portal, const char *versao, enum MSPED_AMBIENTE tp_amb,
                          enum MSPED_CUF c_orgao, enum MSPED_DOC_TIPO tipo_doc, const char *doc, const char *ch,
                          const char *tp_evento, uint64_t n_seq_evento, const char *tag_adic) {
    const char *tp;
    const char *tpd;
    const char *alias_evento;
    const char *desc_evento;
    char *dh_evento;
    char *s_seq_evento;
    char *id_evento;
    char *mensagem;
    msped_extrair_info_evento(tp_evento, &alias_evento, &desc_evento);
    dh_evento = msped_data_hora_utc();
    s_seq_evento = mu_fmt("%02"PRIu64, n_seq_evento);
    id_evento = mu_fmt("ID%s%s%s", tp_evento, ch, s_seq_evento);
    tp = msped_montar_tipo_normal(tipo, false);
    tpd = msped_montar_tipo_doc(tipo_doc);
    mensagem = mu_fmt(MSPED_EVENTO_FMT, portal, versao, id_evento, c_orgao, tp_amb, tpd, doc, tpd, tp,
                                 ch, tp, dh_evento, tp_evento, n_seq_evento, versao, versao, desc_evento, tag_adic);
    free(dh_evento);
    free(s_seq_evento);
    free(id_evento);
    return mensagem;
}

char *msped_montar_env_evento(const char *portal, const char *versao, uint64_t id_lote, const char *signed_msg) {
    return mu_fmt(MSPED_ENV_EVENTO_FMT, portal, versao, id_lote, signed_msg);
}

char *msped_montar_envelope_soap12(enum MSPED_TIPO tipo, const char *namespace, enum MSPED_CUF cuf, const char *versao,
                                   const char *dados) {
    char *res;
    char *cabec_msg;
    char *dados_msg;
    cabec_msg = msped_montar_cabec_msg(tipo, namespace, cuf, versao);
    dados_msg = msped_montar_dados_msg(tipo, namespace, dados);
    res = mu_fmt(MSPED_ENVELOPE_SOAP12, cabec_msg, dados_msg);
    free(cabec_msg);
    free(dados_msg);
    return res;
}