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
#include "msped_http.h"

//TODO: MSPED_HTTP_MAX_TENTATIVAS
#define MSPED_CURL_MAX_TENTATIVAS 10

CURL *msped_curl_novo(const char *pfx, const char *pfx_senha, long timeout, bool depuravel,
                      msped_http_escrita_callback escrita_cb, void *escrita_cls) {
    long tm;
    CURL *curl;
    curl = curl_easy_init();
    if (NULL == curl)
        return NULL;
    tm = timeout > 0 ? timeout : MSPED_HTTP_TIMEOUT_PADRAO;
    curl_easy_setopt(curl, CURLOPT_VERBOSE, depuravel);
    //TODO: curl_easy_setopt(curl, CURLOPT_PROXY, proxy_ip);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, tm);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, tm * 6);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "P12");
    curl_easy_setopt(curl, CURLOPT_SSLCERT, pfx);
    curl_easy_setopt(curl, CURLOPT_SSLCERTPASSWD, pfx_senha);
    if (NULL != escrita_cb) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escrita_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, escrita_cls);
    }
    return curl;
}

void msped_curl_liberar(CURL *curl) {
    if (NULL != curl)
        curl_easy_cleanup(curl);
}

bool msped_curl_executar(CURL *curl, const char *url, const char *headers[], const char *data, bool depuravel,
                         msped_http_tentativa_callback http_tentativa_cb, void *http_tentativa_cls,
                         msped_erro_callback erro_cb, void *erro_cls) {
    bool ret;
    bool nobody;
    CURLcode code;
    const char **header;
    struct curl_slist *chunk;
    uint8_t tentativas;
    if (NULL == curl)
        return false;
    nobody = (NULL == data);
    chunk = NULL;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    if (nobody)
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    else {
        if (depuravel) {
            fprintf(stdout, "*** início dados HTTP libmicrosped ***\n");
            fprintf(stdout, "POST %s\n", url);
        }
        header = headers;
        while (NULL != *header) {
            if (depuravel)
                fprintf(stdout, "> %s\n", *header);
            chunk = curl_slist_append(chunk, *header);
            header++;
        }
        if (depuravel) {
            fprintf(stdout, "\n%s\n", data);
            fprintf(stdout, "*** fim dados HTTP libmicrosped ***\n");
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(data));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    }
    if (NULL != http_tentativa_cb) {
        tentativas = 0;
        while (tentativas < MSPED_CURL_MAX_TENTATIVAS) {
            code = curl_easy_perform(curl);
            if (code == CURLE_OK)
                break;
            if (!http_tentativa_cb(http_tentativa_cls, curl, url)) {
                code = CURLE_ABORTED_BY_CALLBACK;
                break;
            }
            tentativas++;
        }
    } else
        code = curl_easy_perform(curl);
    ret = code == CURLE_OK;
    if (!nobody) {
        curl_slist_free_all(chunk);
        if (!ret && NULL != erro_cb)
            erro_cb(erro_cls, curl_easy_strerror(code));
    }
    return ret;
}