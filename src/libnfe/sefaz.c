/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 * 
 * LivreNFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LivreNFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with LivreNFE.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <libnfe/sefaz.h>
#include <libnfe/libnfe.h>
#include "send.h"
#include "xml.h"
#include <libnfe/gen_xml.h>
#include <libnfe/errno.h>
#include <libxml/parser.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <string.h>

int get_status_servico(int ambiente, char *URL, int cuf, 
		EVP_PKEY *key, X509 *cert, char *msg){ 
	char *response, *status;
	int cStat;
	xmlDocPtr doc;
	char *xml = gen_cons_status(ambiente, cuf);
	response = send_sefaz(SEFAZ_NFE_STATUS_SERVICO, URL, ambiente, cuf, 
		xml, key, cert);
	if(response == NULL){
		strcpy(msg, "Sem resposta do SEFAZ, tente novamente");
		return -ESEFAZ;
	}
	doc = xmlReadMemory(response, strlen(response), "noname.xml", NULL, 0);
	status = get_xml_element(doc, "nfe:cStat");
	if(status == NULL){
		return -ESEFAZ;	
	}
	cStat = atoi(status);
	char *motivo = get_xml_element(doc, "nfe:xMotivo");
	if(motivo == NULL){
		return -ESEFAZ;	
	}
	strcpy(msg, motivo);
	xmlFree(motivo);
	xmlFree(status);
	return cStat;
}

static int sefaz_response_protocolos(LOTE *lote, xmlDocPtr doc, char *msg){
	int i;
	LOTE_ITEM *it = lote->nfes;
	for (i = 0; i < lote->qtd; i++){
		char *status, *motivo;
		int cStat;
		NFE *n = it->nfe;
		char *xp = malloc(sizeof(char) * 100);
		sprintf(xp, "nfe:protNFe/nfe:infProt[nfe:chNFe='%s']/nfe:cStat", 
			n->idnfe->chave);
		status = get_xml_element(doc, xp);
		if(status == NULL){
			xmlFree(status);
			free(xp);
			return -ESEFAZ;	
		}
		cStat = atoi(status);
		xmlFree(status);
		n->protocolo->cod_status = cStat;

		sprintf(xp, "nfe:protNFe/nfe:infProt[nfe:chNFe='%s']/nfe:xMotivo", 
			n->idnfe->chave);
		motivo = get_xml_element(doc, xp);
		n->protocolo->xmot = strdup(motivo);
		strcat(msg, "\n");
		strcat(msg, n->idnfe->chave);
		strcat(msg, ": ");
		strcat(msg, motivo);
		xmlFree(motivo);
		if(cStat == 100){
			char *nProt;
			sprintf(xp, "nfe:protNFe/nfe:infProt[nfe:chNFe='%s']/nfe:nProt", 
				n->idnfe->chave);
			nProt = get_xml_element(doc, xp);
			n->protocolo->numero = strdup(nProt);
			sprintf(xp, "//nfe:protNFe/nfe:infProt[nfe:chNFe='%s']/..", 
				n->idnfe->chave);
			char *xml_prot = get_xml_subtree(doc, xp);
			n->protocolo->xml = strdup(xml_prot);
			xmlFree(nProt);
			xmlFree(xml_prot);
		}
		it = it->next;
		free(xp);
	}
	return 0;
}

static int sefaz_response_eventos(LOTE_EVENTO *lote, xmlDocPtr doc, char *msg){
	int i;
	LOTE_EVENTO_ITEM *it = lote->eventos;
	for (i = 0; i < lote->qtd; i++){
		char *status, *motivo;
		int cStat;
		EVENTO *e = it->evento;
		NFE *n = e->nfe;
		char *xp = malloc(sizeof(char) * 100);
		sprintf(xp, "nfe:retEvento/nfe:infEvento[nfe:chNFe='%s']/nfe:cStat", 
			n->idnfe->chave);
		status = get_xml_element(doc, xp);
		if(status == NULL){
			xmlFree(status);
			free(xp);
			return -ESEFAZ;	
		}
		cStat = atoi(status);
		xmlFree(status);
		e->cStat = cStat;

		sprintf(xp, "nfe:retEvento/nfe:infEvento[nfe:chNFe='%s']/nfe:xMotivo", 
			n->idnfe->chave);
		motivo = get_xml_element(doc, xp);
		strcat(msg, "\n");
		strcat(msg, n->idnfe->chave);
		strcat(msg, ": ");
		strcat(msg, motivo);
		e->xmot = strdup(motivo);
		xmlFree(motivo);
		if(cStat == 135 || cStat == 136){
			char *nProt;
			sprintf(xp, "nfe:retEvento/nfe:infEvento[nfe:chNFe='%s']/nfe:nProt", 
				n->idnfe->chave);
			nProt = get_xml_element(doc, xp);
			e->protocolo = strdup(nProt);
			sprintf(xp, "//nfe:retEvento/nfe:infEvento[nfe:chNFe='%s']/..", 
				n->idnfe->chave);
			char *xml_prot = get_xml_subtree(doc, xp);
			e->xml_response = strdup(xml_prot);
			n->canceled = 1;
			xmlFree(nProt);
			xmlFree(xml_prot);
		}
		it = it->next;
		free(xp);
	}
	return 0;
}

int send_lote(LOTE *lote, char *URL, int ambiente, int cuf, EVP_PKEY *key, 
		X509 *cert, char *msg){
	char *response, *status;
	int cStat, rc;
	xmlDocPtr doc;
	char *xml = gen_lote_xml(lote, key, cert);
	response = send_sefaz(SEFAZ_NFE_AUTORIZACAO, URL, ambiente, cuf, 
		xml, key, cert);
	if(response == NULL){
		strcpy(msg, "Sem resposta do SEFAZ, tente novamente");
		return -ESEFAZ;
	}
	doc = xmlReadMemory(response, strlen(response), "noname.xml", NULL, 0);
	status = get_xml_element(doc, "nfe:cStat");
	if(status == NULL){
		return -ESEFAZ;	
	}
	cStat = atoi(status);
	char *motivo = get_xml_element(doc, "nfe:xMotivo");
	if(motivo == NULL){
		return -ESEFAZ;	
	}
	strcpy(msg, motivo);
	char *nRec = get_xml_element(doc, "nfe:nRec");
	fprintf(stdout, "Lote: %s\n", nRec);
	lote->recibo = strdup(nRec);
	lote->xml_response = strdup(response);
	xmlFree(motivo);
	xmlFree(status);
	xmlFree(nRec);

	if(rc){
		sprintf(msg, "Erro ao salvar lote\nNúmero de recibo: %s",
			nRec);
		return -ESQL;
	}
	return cStat;
}

int send_lote_evento(LOTE_EVENTO *lote, char *URL, int ambiente, int cuf, 
		EVP_PKEY *key, X509 *cert, char *msg){
	char *response, *status;
	int cStat, rc = 0;
	xmlDocPtr doc;
	char *xml = gen_lote_evento_xml(lote, key, cert);
	response = send_sefaz(SEFAZ_RECEPCAO_EVENTO, URL, ambiente, cuf, 
		xml, key, cert);
	if(response == NULL){
		strcpy(msg, "Sem resposta do SEFAZ, tente novamente");
		return -ESEFAZ;
	}
	lote->xml_response = response;
	doc = xmlReadMemory(response, strlen(response), "noname.xml", NULL, 0);
	status = get_xml_element(doc, "nfe:cStat");
	if(status == NULL){
		return -ESEFAZ;	
	}
	cStat = atoi(status);
	char *motivo = get_xml_element(doc, "nfe:xMotivo");
	if(motivo == NULL){
		return -ESEFAZ;	
	}
	strcpy(msg, motivo);

	if(cStat == 128){
		rc = sefaz_response_eventos(lote, doc, msg);
	}
	xmlFree(motivo);
	xmlFree(status);

	if(rc){
		strcpy(msg, "Erro ao enviar eventos");
		return -ESQL;
	}
	return cStat;
}

int cons_lote(LOTE *lote, char *URL, int ambiente, int cuf, EVP_PKEY *key, 
		X509 *cert, char *msg){
	char *response, *status;
	int cStat;
	xmlDocPtr doc;
	char *xml = gen_cons_nfe(lote, ambiente);
	response = send_sefaz(SEFAZ_NFE_RET_AUTORIZACAO, URL, ambiente, cuf, 
		xml, key, cert);
	if(response == NULL){
		strcpy(msg, "Sem resposta do SEFAZ, tente novamente");
		return -ESEFAZ;
	}
	doc = xmlReadMemory(response, strlen(response), "noname.xml", NULL, 0);
	status = get_xml_element(doc, "nfe:cStat");
	if(status == NULL){
		return -ESEFAZ;	
	}
	cStat = atoi(status);
	char *motivo = get_xml_element(doc, "nfe:xMotivo");
	if(motivo == NULL){
		return -ESEFAZ;	
	}
	msg = strcpy(msg, motivo);
	if(cStat == 104)
		sefaz_response_protocolos(lote, doc, msg);
	xmlFree(motivo);
	xmlFree(status);

	return cStat;
}
