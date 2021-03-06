/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of Pitangus.
 *
 * Pitangus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pitangus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Pitangus.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "tool_sefaz_response.h"
#include "tool_window.h"
#include "tool_db_interface.h"
#include "tool_pitangus.h"
#include "tool_prefs.h"
#include "tool_crypto_interface.h"
#include <pitangus/sped.h>
#include <pitangus/libsped.h>
#include <pitangus/sefaz.h>
#include <gtk/gtk.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>


struct _SefazResponseClass {
	GtkDialogClass parent_class;
};

typedef struct _SefazResponsePrivate SefazResponsePrivate;

struct _SefazResponsePrivate{
	GtkSpinner *spinner;
	GtkLabel *resposta;
	GtkButton *ok_btn;
	GtkOverlay *overlay;
};

G_DEFINE_TYPE_WITH_PRIVATE(SefazResponse, sefaz_response, GTK_TYPE_DIALOG)

static void *sefaz_thread(void *arg){
	SefazResponse *sr = SEFAZ_RESPONSE(arg);
	SefazResponsePrivate *priv;
	priv = sefaz_response_get_instance_private(sr);
	X509 *cert = NULL;
	EVP_PKEY *pKey = NULL;
	PREFS *prefs = get_prefs();
	EMITENTE *e = get_emitente(1);
	int cuf = e->endereco->municipio->uf->cUF;
	free_emitente(e);
	char *msg;

	if((prefs->cert_type == CERT_TYPE_A1 && strcmp("", prefs->cert_file))
		|| (prefs->cert_type == CERT_TYPE_A3 && 
		strcmp("", prefs->card_reader_lib))){
		int rc = get_private_key(&pKey, &cert, sr->password);

		if(rc == 0){
			msg = malloc(sizeof(char) * 255);
			int ambiente = prefs->ambiente;
			URLS *urls = prefs->urls;
			if(sr->lote){
				send_lote(sr->lote, urls->nfeautorizacao, prefs->ambiente, 
					cuf, pKey, cert, msg);
				cons_lote(sr->lote, urls->nferetautorizacao, ambiente, 
					cuf, pKey, cert, msg);
				db_save_lote(sr->lote);

			} else if(sr->lote_evento){
				send_lote_evento(sr->lote_evento, urls->nfeconsultaprotocolo, 
					ambiente, cuf, pKey, cert, msg);
				db_save_lote_evento(sr->lote_evento);
			} else {
				int res = get_status_servico(ambiente, urls->nfestatusservico, cuf, 
					pKey, cert, msg);
				if(res < 0){
					msg = strcpy(msg, "Erro ao consultar status de serviço");
				}
			}
			EVP_PKEY_free(pKey);
			X509_free(cert);
		} else {
			msg = strdup("Erro ao obter certificado e chave");
		}
	} else {
		msg = strdup("Configure o certificado");
	}

	gtk_spinner_stop(priv->spinner);
	gtk_label_set_text(priv->resposta, msg);

	gint width;
	gint height;
	gtk_widget_get_preferred_height(GTK_WIDGET(priv->resposta), NULL, &height);
	gtk_widget_get_preferred_width(GTK_WIDGET(priv->resposta), NULL, &width);
	gtk_widget_set_size_request(GTK_WIDGET(&(sr->parent)), width, height);
	gtk_window_set_position(GTK_WINDOW(&(sr->parent)), GTK_WIN_POS_CENTER_ALWAYS);
#if GTK_CHECK_VERSION(3,18,0)
	gtk_overlay_reorder_overlay(priv->overlay, GTK_WIDGET(priv->resposta),
		-1);
#else
	gtk_widget_destroy(GTK_WIDGET(priv->spinner));
#endif
	gtk_widget_set_visible(GTK_WIDGET(priv->ok_btn), TRUE);
	free(msg);
	free_prefs(prefs);
	return NULL;
}

static void get_response(SefazResponse *sr){
	SefazResponsePrivate *priv;
	pthread_t tid;

	priv = sefaz_response_get_instance_private(sr);
	gtk_spinner_start(priv->spinner);
	pthread_create(&tid, NULL, &sefaz_thread, sr);
}

static void ok_click(GtkButton *b, SefazResponse *sr){
	gtk_widget_destroy(GTK_WIDGET(sr));
}

static void sefaz_response_dispose(GObject *object){
	G_OBJECT_CLASS(sefaz_response_parent_class)->dispose(object);
}


static void sefaz_response_init(SefazResponse *sr){
	SefazResponsePrivate *priv;

	priv = sefaz_response_get_instance_private(sr);
	gtk_widget_init_template(GTK_WIDGET(sr));
	g_signal_connect(sr, "show", G_CALLBACK(get_response),
			NULL);
	g_signal_connect(priv->ok_btn, "clicked", G_CALLBACK(ok_click),
			sr);

}



static void sefaz_response_class_init(SefazResponseClass *class){
	G_OBJECT_CLASS (class)->dispose = sefaz_response_dispose;

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                               "/br/com/crossteam/pitangus/tool_sefaz_response.ui");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		SefazResponse, spinner);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		SefazResponse, resposta);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		SefazResponse, ok_btn);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		SefazResponse, overlay);
}

SefazResponse *sefaz_response_new(LivrenfeWindow *win){
	return g_object_new (SEFAZ_RESPONSE_TYPE, "transient-for", win, 
		"use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
