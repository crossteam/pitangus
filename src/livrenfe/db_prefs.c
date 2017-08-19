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

#include "db_interface.h"
#include "db.h"
#include "livrenfe.h"
#include <libnfe/errno.h>
#include <libnfe/libnfe.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

char *get_ws_url(char *service, int ambiente, char **url_header, 
		char **url_body){
	sqlite3_stmt *stmt;
	char *err, *url = NULL , *s;
	err = malloc(sizeof(char) * 200);
	int rc;
	s = ambiente == 1? "url_prod" : "url_cert";

	char *sql = sqlite3_mprintf("SELECT %s, url_header, url_body FROM urls \
		WHERE service = %Q", s, service);
	if(db_select(sql, &err, &stmt)){
	fprintf(stdout, "SQL: %s\n", err);
		return NULL;
	}
	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW){
		const char *aux = NULL;
		aux = (char*)sqlite3_column_text(stmt, 0);
		url = aux == NULL? NULL:strdup(aux);
		aux = (char*)sqlite3_column_text(stmt, 1);
		*url_header = aux == NULL? NULL:strdup(aux);
		aux = (char*)sqlite3_column_text(stmt, 2);
		*url_body = aux == NULL? NULL:strdup(aux);
	}
	return url;
}

int get_url_id(char *service){
	sqlite3_stmt *stmt;
	char *err;
	err = malloc(sizeof(char) * 200);
	int rc, id = 0;
	char *sql = sqlite3_mprintf("SELECT id_url  FROM urls \
		WHERE service = %Q", service);
	if(db_select(sql, &err, &stmt)){
	fprintf(stdout, "SQL: %s\n", err);
		return -EIDNFOUND;
	}
	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW){
		id = sqlite3_column_int(stmt, 0);
	}
	return id;
}

int set_prefs_urls(PREFS_URLS *urls){
	char *sql, *err;
	err = NULL;
	if(urls){
		sql = sqlite3_mprintf("UPDATE urls SET url_prod = %Q,\
			url_cert = %Q WHERE id_url = %d;",
			urls->recepcaoevento_prod, 
			urls->recepcaoevento_cert, SEFAZ_RECEPCAO_EVENTO);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error: %s", err);
			return -ESQL;
		}
		sql = sqlite3_mprintf("UPDATE urls SET url_prod = %Q,\
			url_cert = %Q WHERE id_url = %d;",
			urls->nfeconsultacadastro_prod, 
			urls->nfeconsultacadastro_cert, SEFAZ_NFE_CONSULTA_CADASTRO);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error: %s", err);
			return -ESQL;
		}
		sql = sqlite3_mprintf("UPDATE urls SET url_prod = %Q,\
			url_cert = %Q WHERE id_url = %d;",
			urls->nfeinutilizacao_prod, 
			urls->nfeinutilizacao_cert, SEFAZ_NFE_INUTILIZACAO);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error: %s", err);
			return -ESQL;
		}
		sql = sqlite3_mprintf("UPDATE urls SET url_prod = %Q,\
			url_cert = %Q WHERE id_url = %d;",
			urls->nfeconsultaprotocolo_prod, 
			urls->nfeconsultaprotocolo_cert, SEFAZ_NFE_CONSULTA_PROTOCOLO);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error: %s", err);
			return -ESQL;
		}
		sql = sqlite3_mprintf("UPDATE urls SET url_prod = %Q,\
			url_cert = %Q WHERE id_url = %d;",
			urls->nfestatusservico_prod, 
			urls->nfestatusservico_cert, SEFAZ_NFE_STATUS_SERVICO);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error: %s", err);
			return -ESQL;
		}
		sql = sqlite3_mprintf("UPDATE urls SET url_prod = %Q,\
			url_cert = %Q WHERE id_url = %d;",
			urls->nfeautorizacao_prod, 
			urls->nfeautorizacao_cert, SEFAZ_NFE_AUTORIZACAO);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error: %s", err);
			return -ESQL;
		}
		sql = sqlite3_mprintf("UPDATE urls SET url_prod = %Q,\
			url_cert = %Q WHERE id_url = %d;",
			urls->nferetautorizacao_prod, 
			urls->nferetautorizacao_cert, SEFAZ_NFE_RET_AUTORIZACAO);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error: %s", err);
			return -ESQL;
		}
	}
	return 0;
}

int set_prefs(PREFS *prefs){
	char *sql, *err;
	err = NULL;
	if(prefs){
		sql = sqlite3_mprintf("REPLACE INTO prefs (id, ambiente,\
			cert_type, a1_cert_file, a3_library)\
			VALUES  (%d, %d, %d, %Q, %Q);",
			1, prefs->ambiente, prefs->cert_type,
			prefs->cert_file, prefs->card_reader_lib);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error: %s", err);
			return -ESQL;
		}
	}
	return 0;
}

PREFS_URLS *get_prefs_urls(){
	PREFS_URLS *p = malloc(sizeof(PREFS_URLS));
	sqlite3_stmt *stmt;
	char *err;
	int rc;
	err = malloc(sizeof(char) * 200);
	
	char *sql = sqlite3_mprintf("SELECT id_url, url_prod, url_cert \
		FROM urls");
	if(db_select(sql, &err, &stmt)){
		free(p);
		return NULL;
	}
	
	do{
		rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW){
			int id_url = sqlite3_column_int(stmt, 0);
			const char *prod = (char*)sqlite3_column_text(stmt, 1);
			const char *cert = (char*)sqlite3_column_text(stmt, 2);
			switch(id_url){
				case SEFAZ_RECEPCAO_EVENTO: //RecepcaoEvento
					p->recepcaoevento_prod = strdup(prod);
					p->recepcaoevento_cert = strdup(cert);
					break;
				case SEFAZ_NFE_CONSULTA_CADASTRO: //NfeConsultaCadastro
					p->nfeconsultacadastro_prod = strdup(prod);
					p->nfeconsultacadastro_cert = strdup(cert);
					break;
				case SEFAZ_NFE_INUTILIZACAO: //NfeInutilizacao
					p->nfeinutilizacao_prod = strdup(prod);
					p->nfeinutilizacao_cert = strdup(cert);
					break;
				case SEFAZ_NFE_CONSULTA_PROTOCOLO: //NfeConsultaProtocolo
					p->nfeconsultaprotocolo_prod = strdup(prod);
					p->nfeconsultaprotocolo_cert = strdup(cert);
					break;
				case SEFAZ_NFE_STATUS_SERVICO: //NfeStatusServico
					p->nfestatusservico_prod = strdup(prod);
					p->nfestatusservico_cert = strdup(cert);
					break;
				case SEFAZ_NFE_AUTORIZACAO: //NfeAutorizacao
					p->nfeautorizacao_prod = strdup(prod);
					p->nfeautorizacao_cert = strdup(cert);
					break;
				case SEFAZ_NFE_RET_AUTORIZACAO: //NFeRetAutorizacao
					p->nferetautorizacao_prod = strdup(prod);
					p->nferetautorizacao_cert = strdup(cert);
					break;
			}
		} else if(rc == SQLITE_DONE){
			break;
		} else {
			free(p);
			return NULL;
		}
	} while(rc == SQLITE_ROW);

	return p;
}

static URLS *get_ambiente_urls(int ambiente){
	URLS *u = malloc(sizeof(URLS));
	sqlite3_stmt *stmt;
	char *err;
	int rc;
	err = malloc(sizeof(char) * 200);
	
	char *sql = sqlite3_mprintf("SELECT id_url, url_prod, url_cert \
		FROM urls");
	if(db_select(sql, &err, &stmt)){
		free(u);
		return NULL;
	}
	do{
		rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW){
			int id_url = sqlite3_column_int(stmt, 0);
			const char *link = (char*)sqlite3_column_text(stmt, ambiente);
			switch(id_url){
				case SEFAZ_RECEPCAO_EVENTO: //RecepcaoEvento
					u->recepcaoevento = strdup(link);
					break;
				case SEFAZ_NFE_CONSULTA_CADASTRO: //NfeConsultaCadastro
					u->nfeconsultacadastro = strdup(link);
					break;
				case SEFAZ_NFE_INUTILIZACAO: //NfeInutilizacao
					u->nfeinutilizacao = strdup(link);
					break;
				case SEFAZ_NFE_CONSULTA_PROTOCOLO: //NfeConsultaProtocolo
					u->nfeconsultaprotocolo = strdup(link);
					break;
				case SEFAZ_NFE_STATUS_SERVICO: //NfeStatusServico
					u->nfestatusservico = strdup(link);
					break;
				case SEFAZ_NFE_AUTORIZACAO: //NfeAutorizacao
					u->nfeautorizacao = strdup(link);
					break;
				case SEFAZ_NFE_RET_AUTORIZACAO: //NFeRetAutorizacao
					u->nferetautorizacao = strdup(link);
					break;
			}
		} else if(rc == SQLITE_DONE){
			break;
		} else {
			free(u);
			return NULL;
		}
	} while(rc == SQLITE_ROW);
	return u;
}

PREFS *get_prefs(){
	PREFS *p = malloc(sizeof(PREFS));
	sqlite3_stmt *stmt;
	char *err;
	int rc;
	err = malloc(sizeof(char) * 200);
	
	char *sql = sqlite3_mprintf("SELECT ambiente, cert_type, a1_cert_file,\
		a3_library\
		FROM prefs");
	if(db_select(sql, &err, &stmt)){
		free(p);
		return NULL;
	}
	do{
		rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW){
			int ambiente = sqlite3_column_int(stmt, 0);
			int cert_type = sqlite3_column_int(stmt, 1);
			const char *a1_cert = (char*)sqlite3_column_text(stmt, 2);
			const char *a3_lib = (char*)sqlite3_column_text(stmt, 3);

			p->cert_file = a1_cert? strdup(a1_cert):strdup("");
			p->card_reader_lib = a3_lib? strdup(a3_lib):strdup("");
			p->ambiente = ambiente;
			p->cert_type = cert_type;
			p->urls = get_ambiente_urls(ambiente);
			break;
		} else if(rc == SQLITE_DONE){
			p->ambiente = DEFAULT_AMBIENTE;
			p->cert_type = DEFAULT_CERT_TYPE;
			p->urls = get_ambiente_urls(DEFAULT_AMBIENTE);
			p->cert_file = strdup("");
			p->card_reader_lib = strdup("");
			break;
		} else {
			free(p);
			return NULL;
		}
	} while(rc == SQLITE_ROW);
	return p;
}
