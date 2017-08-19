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

#ifndef MSPED_SOAP_DEFS_H
#define MSPED_SOAP_DEFS_H

#include <microsped.h>

#define MSPED_HEADER_SOAP_CONTENT_TYPE "Content-Type: application/soap+xml; charset=utf-8"

#define MSPED_HEADER_SOAP_ACTION "SOAPAction: \"%s\""

#define MSPED_PORTAL_FMT "http://www.portalfiscal.inf.br/%s"

#define MSPED_NAMESPACE_FMT "http://www.portalfiscal.inf.br/%s/wsdl/%s"

#define MSPED_CABEC_MSG_FMT "<%sCabecMsg xmlns=\"%s\"><cUF>%d</cUF><versaoDados>%s</versaoDados></%sCabecMsg>"

#define MSPED_DADOS_MSG_FMT "<%sDadosMsg xmlns=\"%s\">%s</%sDadosMsg>"

#define MSPED_CONS_SIT_FMT "<consSit%s xmlns=\"%s\" versao=\"%s\"><tpAmb>%d</tpAmb><xServ>CONSULTAR</xServ><ch%s>%s</ch%s></consSit%s>"

#define MSPED_CONS_STAT_SERV_FMT "<consStatServ%s xmlns=\"%s\" versao=\"%s\"><tpAmb>%d</tpAmb><cUF>%d</cUF><xServ>STATUS</xServ></consStatServ%s>"

#define MSPED_CONS_CAD_FMT "<ConsCad xmlns=\"%s\" versao=\"%s\"><infCons><xServ>CONS-CAD</xServ><UF>%s</UF><%s>%s</%s></infCons></ConsCad>"

#define MSPED_ENV_LOTE_FMT "<envi%s xmlns=\"%s\" versao=\"%s\"><idLote>%"PRIu64"</idLote><indSinc>%d</indSinc>%s</envi%s>"

#define MSPED_CONS_RECI_FMT "<consReci%s xmlns=\"%s\" versao=\"%s\"><tpAmb>%d</tpAmb><nRec>%"PRIu64"</nRec></consReci%s>"

#define MSPED_EVENTO_FMT "<evento xmlns=\"%s\" versao=\"%s\"><infEvento Id=\"%s\"><cOrgao>%d</cOrgao><tpAmb>%d</tpAmb><%s>%s</%s><ch%s>%s</ch%s><dhEvento>%s</dhEvento><tpEvento>%s</tpEvento><nSeqEvento>%"PRIu64"</nSeqEvento><verEvento>%s</verEvento><detEvento versao=\"%s\"><descEvento>%s</descEvento>%s</detEvento></infEvento></evento>"

#define MSPED_ENV_EVENTO_FMT "<envEvento xmlns=\"%s\" versao=\"%s\"><idLote>%"PRIu64"</idLote>%s</envEvento>"

#define MSPED_ENVELOPE_SOAP12 "<?xml version=\"1.0\" encoding=\"UTF-8\"?><soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\"><soap12:Header>%s</soap12:Header><soap12:Body>%s</soap12:Body></soap12:Envelope>"

const char *msped_obter_sv_str(enum MSPED_TIPO tipo, enum MSPED_NF_MODELO modelo, enum MSPED_CUF cuf, enum MSPED_SV sv);

char *msped_montar_header_soap_action(const char *namespace);

const char *msped_montar_tipo_minusculo(enum MSPED_TIPO tipo);

const char *msped_montar_tipo_normal(enum MSPED_TIPO tipo, bool permite_vazio);

const char *msped_montar_tipo_doc(enum MSPED_DOC_TIPO tipo_doc);

char *msped_montar_portal(enum MSPED_TIPO tipo);

char *msped_montar_namespace(enum MSPED_TIPO tipo, const char *operacao);

char *msped_montar_cabec_msg(enum MSPED_TIPO tipo, const char *namespace, enum MSPED_CUF cuf, const char *versao);

char *msped_montar_dados_msg(enum MSPED_TIPO tipo, const char *namespace, const char *dados);

char *msped_montar_cons_sit(enum MSPED_TIPO tipo, const char *portal, const char *versao, enum MSPED_AMBIENTE ambiente,
                            const char *ch);

char *msped_montar_cons_stat_serv(enum MSPED_TIPO tipo, const char *portal, const char *versao,
                                  enum MSPED_AMBIENTE ambiente, enum MSPED_CUF cuf);

char *msped_montar_cons_cad(const char *portal, const char *versao, enum MSPED_CUF cuf, enum MSPED_DOC_TIPO tipo_doc,
                            const char *doc);

char *msped_montar_envi_lote(enum MSPED_TIPO tipo, const char *portal, const char *versao, uint64_t id_lote,
                             uint8_t ind_sinc, const char *nfe);

char *msped_montar_cons_reci(enum MSPED_TIPO tipo, const char *portal, const char *versao, enum MSPED_AMBIENTE ambiente,
                             uint64_t recibo);

bool msped_extrair_info_evento(const char *tp_evento, const char **alias_evento, const char **desc_evento);

char *msped_montar_evento(enum MSPED_TIPO tipo, const char *portal, const char *versao, enum MSPED_AMBIENTE tp_amb,
                          enum MSPED_CUF c_orgao, enum MSPED_DOC_TIPO tipo_doc, const char *doc, const char *ch,
                          const char *tp_evento, uint64_t n_seq_evento, const char *tag_adic);

char *msped_montar_env_evento(const char *portal, const char *versao, uint64_t id_lote, const char *signed_msg);

char *msped_montar_envelope_soap12(enum MSPED_TIPO tipo, const char *namespace, enum MSPED_CUF cuf, const char *versao,
                                   const char *dados);

#endif