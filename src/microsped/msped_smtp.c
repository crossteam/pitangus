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

#ifndef CONFIG_H
#define CONFIG_H
# include "config.h"
#endif
#include <openssl/rand.h>
#include "msped_macros.h"
#include "msped_smtp.h"
#include <microutils.h>

char *msped_smtp_str_append(char **existente, const char *nova) {
    char *p;
    size_t len = (*existente ? strlen(*existente) : 0);
    if ((p = (char *) realloc(*existente, len + strlen(nova) + 1)) == NULL) {
        free(p);
        return NULL;
    }
    *existente = p;
    strcpy(*existente + len, nova);
    return *existente;
}

void msped_smtp_bound_gerar(char *bound) {
    int i;
    char c;
    RAND_pseudo_bytes((unsigned char *) bound, 32);
    for (i = 0; i < 32; i++) {
        c = (char) (bound[i] & 0xf);
        if (c < 10) c += '0';
        else c += 'a' - 10;
        bound[i] = c;
    }
    bound[32] = 0;
}

char *msped_smtp_quoted_printable_encode(const char *str, const char *term) {
    char *res;
    size_t line_out_s;
    char CRLF[] = "\r\n";
    const char *line_s;
    const char *line_e;
    const char *p;
    char *op;
    char *pp;
    char paragraph[100];
    size_t pp_remain;
    size_t out_remain;
    const char *in_data_limit = str + strlen(str);
    size_t cur_line_len = 0;
    line_out_s = strlen(str) * 3 + 1;
    res = malloc(line_out_s * sizeof(char));
    if (NULL == res)
        return NULL;
    if (NULL == term)
        term = CRLF;
    line_s = NULL;
    line_e = str;
    op = res;
    out_remain = line_out_s;
    do {
        char charout[4];
        int charout_size = 0;
        if (line_e != '\0') {
            if (line_s == NULL)
                line_s = str;
            else
                line_s = line_e;
            line_e = strstr(line_s, term);
            if (line_e == NULL)
                line_e = str + strlen(str);
            else
                line_e += strlen(term);
        }
        paragraph[0] = '\0';
        pp = paragraph;
        pp_remain = sizeof(paragraph);
        cur_line_len = 0;
        p = line_s;
        while ((p < line_e)) {
            if (*p < ' ' || *p == '=' || *p > 126) {
                snprintf(charout, sizeof(charout), "=%02X", (unsigned char) *p);
                charout_size = 3;
            } else {
                snprintf(charout, sizeof(charout), "%c", *p);
                charout_size = 1;
            }
            if (cur_line_len + charout_size >= 76) {
                snprintf(op, out_remain, "%s=%s", paragraph, term);
                op += strlen(paragraph);
                out_remain -= (strlen(paragraph));
                paragraph[0] = '\0';
                pp_remain = sizeof(paragraph);
                pp = paragraph;
                cur_line_len = (size_t) -1;
            }
            snprintf(pp, pp_remain, "%s", charout);
            pp += charout_size;
            pp_remain -= charout_size;
            p++;
            cur_line_len += charout_size;
        }
        snprintf(op, out_remain, "%s%s", paragraph, term);
        op += (strlen(paragraph) + 2);
        out_remain -= (strlen(paragraph) + 2);
    } while ((line_e < in_data_limit) && (*line_e != '\0'));
    return res;
}

unsigned char *msped_smtp_base64_from_file(const char *filename) {
    unsigned char *res;
    unsigned char *buf;
    FILE *fp;
    size_t sz;
    size_t len;
    fp = fopen(filename, "rb");
    if (NULL == fp)
        return NULL;
    fseek(fp, 0, SEEK_END);
    sz = (size_t) ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buf = malloc(sz);
    if (NULL == buf) {
        fclose(fp);
        return NULL;
    }
    size_t rd = fread(buf, 1, sz, fp);
    fclose(fp);
    if (rd < 1) {
        free(buf);
        return NULL;
    }
    res = mu_b64enc(buf, sz, &len);
    free(buf);
    return res;
}

char *msped_smtp_multipart_msg_formatar(const char *bound, const char *msg_ct, const char *msg) {
    char *res;
    char *mqp;
    mqp = msped_smtp_quoted_printable_encode(msg, NULL);
#define _SMTP_MULTIPART_MSG \
    "--%s\r\n" \
    "Content-Type: %s\r\n" \
    "Content-Transfer-Encoding: quoted-printable\r\n" \
    "\r\n" \
    "%s"
    res = mu_fmt(_SMTP_MULTIPART_MSG, bound, msg_ct, mqp);
#undef _SMTP_MULTIPART_MSG
    free(mqp);
    return res;
}

char *msped_smtp_multipart_anexo_formatar(const char *bound, const char *nome, const char *ct,
                                          const unsigned char *base64) {
#define SMTP_MULTIPART_ANEXOS \
    "--%s\r\n" \
    "Content-Type: %s; Name=\"%s\"\r\n" \
    "Content-Disposition: attachment; filename=\"%s\"\r\n" \
    "Content-Transfer-Encoding: base64\r\n" \
    "\r\n" \
    "%s\r" /* a função de base64 adiciona o \n */
    return mu_fmt(SMTP_MULTIPART_ANEXOS, bound, ct, nome, nome, base64);
#undef SMTP_MULTIPART_ANEXOS
}

static bool msped_smtp_multipart_anexos_add_split_str_callback(void *cls, const char *step, const char *token) {
    struct msped_smtp_multipart_record *record = cls;
    unsigned char *b64;
    char *fn;
    char *nm;
    char *ct;
    char *buf;
    if (NULL != token) {
        fn = mu_trim(token);
        nm = mu_filename(fn);
        ct = strdup(mu_fntomime(nm));
        b64 = msped_smtp_base64_from_file(fn);
        buf = msped_smtp_multipart_anexo_formatar(record->boundary, nm, ct, b64);
        free(fn);
        free(nm);
        free(ct);
        free(b64);
        if (NULL != buf) {
            record->payload = msped_smtp_str_append(&record->payload, buf);
            free(buf);
        }
    }
    return true;
}

void msped_smtp_multipart_anexos_add(void *cls, const char *anexos) {
    mu_split(anexos, ",", &msped_smtp_multipart_anexos_add_split_str_callback, cls);
}

char *msped_smtp_multipart_ct_formatar(const char *bound) {
    return mu_fmt("multipart/mixed; boundary=%s", bound);
}

char *msped_smtp_multipart_fim(const char *bound) {
    return mu_fmt("--%s--", bound);
}

char *msped_smtp_ct_texto_formatar(enum MSPED_SMTP_TIPO_CONTEUDO tipo) {
    return mu_fmt("text/%s; charset=UTF-8", MSPED_SMTP_TIPO_CONT_TEXTO == tipo ? "plain" : "html");
}

char *msped_smtp_payload_formatar(enum MSPED_SMTP_TIPO_CONTEUDO tipo, const char *titulo, const char *de,
                                  const char *para, const char *cc, const char *msg, const char *anexos) {
    char *res;
    char *data;
    char *cco;
    char *mct;
    char *ct;
    char *fim;
    struct msped_smtp_multipart_record record;
    data = mu_now_rfc2822();
    if (mu_is_empty(cc))
        cco = strdup("");
    else
        cco = mu_fmt("Cc: %s\r\n", cc);
    mct = msped_smtp_ct_texto_formatar(tipo);
    if (NULL != anexos) {
        msped_smtp_bound_gerar(record.boundary);
        ct = msped_smtp_multipart_ct_formatar(record.boundary);
        record.payload = msped_smtp_multipart_msg_formatar(record.boundary, mct, msg);
        if (NULL != record.payload)
            msped_smtp_multipart_anexos_add(&record, anexos);
        fim = msped_smtp_multipart_fim(record.boundary);
        record.payload = msped_smtp_str_append(&record.payload, fim);
        free(fim);
    } else {
        ct = mct;
        mct = NULL;
        record.payload = strdup(msg);
    }
#define SMTP_PAYLOAD_FMT \
    "MIME-Version: 1.0\r\n" \
    "Date: %s\r\n" \
    "Subject: %s\r\n" \
    "From: %s\r\n" \
    "To: %s\r\n" \
    "%s" \
    "Content-Type: %s\r\n" \
    "User-Agent: " PACKAGE_STRING "\r\n" \
    "\r\n" \
    "%s"
    res = mu_fmt(SMTP_PAYLOAD_FMT, data, titulo, de, para, cco, ct, record.payload);
#undef SMTP_PAYLOAD_FMT
    free(data);
    free(cco);
    free(mct);
    free(ct);
    free(record.payload);
    return res;
}

static size_t msped_cur_read_data_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct msped_smtp_payload *payload = userp;
    memcpy(ptr, payload->buf, payload->len);
    return payload->len;
}

static bool msped_string_para_lista_split_str_callback(void *cls, const char *step, const char *token) {
    struct curl_slist **lista;
    char *email;
    char *tmp;
    if (NULL != token) {
        lista = cls;
        email = mu_ibetween(token, "<", ">");
        if (NULL != email) {
            tmp = email;
            email = mu_trim(email);
            free(tmp);
        } else
            email = mu_trim(token);
        if (NULL != *lista)
            *lista = curl_slist_append(*lista, email);
        else
            *lista = curl_slist_append(NULL, email);
        free(email);
    }
    return true;
}


void msped_string_para_lista(struct curl_slist **lista, const char *para) {
    mu_split(para, ",", &msped_string_para_lista_split_str_callback, lista);
}

void msped_smtp_inicializar() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void msped_smtp_finalizar() {
    curl_global_cleanup();
}

struct msped_smtp *msped_smtp_novo(const char *servidor, long porta, bool ssl, const char *usuario,
                                   const char *senha, long timeout, bool depuravel,
                                   msped_erro_callback erro_cb, void *erro_cls) {
    struct msped_smtp *smtp;
    long tm;
    if (mu_is_empty(servidor)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "servidor");
        return NULL;
    }
    if (mu_is_empty(usuario)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "usuario");
        return NULL;
    }
    if (mu_is_empty(senha)) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "senha");
        return NULL;
    }
    if (NULL == (smtp = malloc(sizeof(struct msped_smtp)))) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "smtp");
        return NULL;
    }
    memset(smtp, 0, sizeof(struct msped_smtp));
    smtp->depuravel = depuravel;
    smtp->curl = curl_easy_init();
    if (NULL == smtp->curl) {
        free(smtp);
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "curl");
        return NULL;
    }
    tm = timeout > 0 ? timeout : MSPED_SMTP_TIMEOUT_PADRAO;
    curl_easy_setopt(smtp->curl, CURLOPT_VERBOSE, smtp->depuravel);
    curl_easy_setopt(smtp->curl, CURLOPT_URL, servidor);
    if (porta > 0)
        curl_easy_setopt(smtp->curl, CURLOPT_PORT, porta);
    if (ssl) {
        curl_easy_setopt(smtp->curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(smtp->curl, CURLOPT_SSL_VERIFYPEER, 0L);
    }
    curl_easy_setopt(smtp->curl, CURLOPT_USERNAME, usuario);
    curl_easy_setopt(smtp->curl, CURLOPT_PASSWORD, senha);
    curl_easy_setopt(smtp->curl, CURLOPT_CONNECTTIMEOUT_MS, tm);
    curl_easy_setopt(smtp->curl, CURLOPT_TIMEOUT_MS, tm * 6);
    curl_easy_setopt(smtp->curl, CURLOPT_READFUNCTION, &msped_cur_read_data_callback);
    curl_easy_setopt(smtp->curl, CURLOPT_UPLOAD, true);
    smtp->ok = true;
    return smtp;
}

void msped_smtp_liberar(struct msped_smtp *smtp) {
    if (NULL != smtp) {
        if (NULL != smtp->curl)
            curl_easy_cleanup(smtp->curl);
        free(smtp);
    }
}

bool msped_smtp_ok(struct msped_smtp *smtp) {
    return (NULL != smtp && smtp->ok);
}

bool msped_smtp_enviar(struct msped_smtp *smtp, enum MSPED_SMTP_TIPO_CONTEUDO tipo_conteudo, const char *de,
                       const char *para, const char *cc, const char *cco, const char *titulo, const char *msg,
                       const char *anexos, msped_erro_callback erro_cb, void *erro_cls) {
    bool ret;
    CURLcode code;
    struct msped_smtp_payload *payload;
    struct curl_slist *para_lista;
    char *mail_from;
    char *tmp;
    if (NULL == smtp) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "smtp");
        return false;
    }
    if (MSPED_SMTP_TIPO_CONT_NENHUM == tipo_conteudo) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "tipo_conteudo");
        return false;
    }
    if (!smtp->ok) {
        _MSPED_ERR(erro_cb, erro_cls, "%s", S_MSPED_SMTP_CFG_INV);
        return false;
    }
    if (NULL == de) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "de");
        return false;
    }
    if (NULL == para) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "para");
        return false;
    }
    if (NULL == titulo) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "titulo");
        return false;
    }
    if (NULL == msg) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_ARG_REQ, "msg");
        return false;
    }
    payload = malloc(sizeof(struct msped_smtp_payload));
    if (NULL == payload) {
        _MSPED_ERR(erro_cb, erro_cls, S_MSPED_OBJ_ERR, "payload");
        return false;
    }
    memset(payload, 0, sizeof(struct msped_smtp_payload));
    mail_from = mu_ibetween(de, "<", ">");
    if (NULL != mail_from) {
        tmp = mail_from;
        mail_from = mu_trim(mail_from);
        free(tmp);
    } else
        mail_from = mu_trim(de);
    curl_easy_setopt(smtp->curl, CURLOPT_MAIL_FROM, mail_from);
    payload->buf = msped_smtp_payload_formatar(tipo_conteudo, titulo, de, para, cc, msg, anexos);
    payload->len = strlen(payload->buf);
    curl_easy_setopt(smtp->curl, CURLOPT_INFILESIZE, payload->len);
    curl_easy_setopt(smtp->curl, CURLOPT_READDATA, payload);
    para_lista = NULL;
    msped_string_para_lista(&para_lista, para);
    if (!mu_is_empty(cc))
        msped_string_para_lista(&para_lista, cc);
    if (!mu_is_empty(cco))
        msped_string_para_lista(&para_lista, cco);
    curl_easy_setopt(smtp->curl, CURLOPT_MAIL_RCPT, para_lista);
    if (smtp->depuravel) {
        fprintf(stdout, "*** início dados SMTP libmicrosped ***\n");
        fprintf(stdout, "%s\n", payload->buf);
        fprintf(stdout, "*** fim dados SMTP libmicrosped ***\n");
        fflush(stdout);
    }
    code = curl_easy_perform(smtp->curl);
    free(mail_from);
    free(payload->buf);
    free(payload);
    if (NULL != para_lista)
        curl_slist_free_all(para_lista);
    ret = code == CURLE_OK;
    if (!ret && NULL != erro_cb)
        erro_cb(erro_cls, curl_easy_strerror(code));
    return ret;
}