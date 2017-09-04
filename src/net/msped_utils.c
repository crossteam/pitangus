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

#include <microsped.h>
#include <string.h>
#include <unistd.h>
#include "msped_strs.h"
#include "msped_macros.h"

char *msped_sha1_criar_hash(const char *str) {
    size_t l;
    char *res;
    unsigned char digest[SHA_DIGEST_LENGTH];
    if (NULL == str)
        return NULL;
    res = malloc((SHA_DIGEST_LENGTH * 2) + 1);
    if (NULL == res)
      return NULL;
    l = strlen(str);
    SHA1((const unsigned char *) str, l, digest);
    for (uint8_t i = 0; i < SHA_DIGEST_LENGTH; i++)
        mu_uitouh(digest[i], res + i * 2);
    res[SHA_DIGEST_LENGTH * 2] = '\0';
    return res;
}

enum MSPED_DOC_TIPO msped_extrair_tipo_doc(const char *doc) {
    size_t len;
    if (mu_is_empty(doc))
        return MSPED_DOC_TIPO_NENHUM;
    len = strlen(doc);
    if (len < 2)
        return MSPED_DOC_TIPO_NENHUM;
    switch (len) {
        case 11:
            return MSPED_DOC_TIPO_CPF;
        case 14:
            return MSPED_DOC_TIPO_CNPJ;
        default:
            return MSPED_DOC_TIPO_IE;
    }
}

bool msped_extrair_cuf_modelo(const char *chave, enum MSPED_CUF *cuf, enum MSPED_NF_MODELO *modelo) {
    char buf[2];
    if (!msped_validar_chave(chave))
        return false;
    assert(cuf);
    assert(modelo);
    memcpy(buf, chave, 2);
    *cuf = (enum MSPED_CUF) strtol(buf, NULL, 10);
    memcpy(buf, chave + 20, 2);
    *modelo = (enum MSPED_NF_MODELO) strtol(buf, NULL, 10);
    return (msped_validar_cuf(*cuf) && msped_validar_modelo_nf(*modelo));
}

const char *msped_cuf_para_uf(enum MSPED_CUF cuf) {
    switch (cuf) {
        case MSPED_CUF_NENHUM:
            return S_MSPED_NENHUM;
        case MSPED_CUF_AC:
            return "AC";
        case MSPED_CUF_AL:
            return "AL";
        case MSPED_CUF_AP:
            return "AP";
        case MSPED_CUF_AM:
            return "AM";
        case MSPED_CUF_BA:
            return "BA";
        case MSPED_CUF_CE:
            return "CE";
        case MSPED_CUF_DF:
            return "DF";
        case MSPED_CUF_ES:
            return "ES";
        case MSPED_CUF_GO:
            return "GO";
        case MSPED_CUF_MA:
            return "MA";
        case MSPED_CUF_MT:
            return "MT";
        case MSPED_CUF_MS:
            return "MS";
        case MSPED_CUF_MG:
            return "MG";
        case MSPED_CUF_PA:
            return "PA";
        case MSPED_CUF_PB:
            return "PB";
        case MSPED_CUF_PR:
            return "PR";
        case MSPED_CUF_PE:
            return "PE";
        case MSPED_CUF_PI:
            return "PI";
        case MSPED_CUF_RJ:
            return "RJ";
        case MSPED_CUF_RN:
            return "RN";
        case MSPED_CUF_RS:
            return "RS";
        case MSPED_CUF_RO:
            return "RO";
        case MSPED_CUF_RR:
            return "RR";
        case MSPED_CUF_SC:
            return "SC";
        case MSPED_CUF_SP:
            return "SP";
        case MSPED_CUF_SE:
            return "SE";
        case MSPED_CUF_TO:
            return "TO";
        default:
            return NULL;
    }
}

enum MSPED_CUF msped_uf_para_cuf(const char *uf) {
    if (mu_is_empty(uf))
        return MSPED_CUF_NENHUM;
    if (0 == strcmp(uf, "AC"))
        return MSPED_CUF_AC;
    else if (0 == strcmp(uf, "AL"))
        return MSPED_CUF_AL;
    else if (0 == strcmp(uf, "AP"))
        return MSPED_CUF_AP;
    else if (0 == strcmp(uf, "AM"))
        return MSPED_CUF_AM;
    else if (0 == strcmp(uf, "BA"))
        return MSPED_CUF_BA;
    else if (0 == strcmp(uf, "CE"))
        return MSPED_CUF_CE;
    else if (0 == strcmp(uf, "DF"))
        return MSPED_CUF_DF;
    else if (0 == strcmp(uf, "ES"))
        return MSPED_CUF_ES;
    else if (0 == strcmp(uf, "GO"))
        return MSPED_CUF_GO;
    else if (0 == strcmp(uf, "MA"))
        return MSPED_CUF_MA;
    else if (0 == strcmp(uf, "MT"))
        return MSPED_CUF_MT;
    else if (0 == strcmp(uf, "MS"))
        return MSPED_CUF_MS;
    else if (0 == strcmp(uf, "MG"))
        return MSPED_CUF_MG;
    else if (0 == strcmp(uf, "PA"))
        return MSPED_CUF_PA;
    else if (0 == strcmp(uf, "PB"))
        return MSPED_CUF_PB;
    else if (0 == strcmp(uf, "PR"))
        return MSPED_CUF_PR;
    else if (0 == strcmp(uf, "PE"))
        return MSPED_CUF_PE;
    else if (0 == strcmp(uf, "PI"))
        return MSPED_CUF_PI;
    else if (0 == strcmp(uf, "RJ"))
        return MSPED_CUF_RJ;
    else if (0 == strcmp(uf, "RN"))
        return MSPED_CUF_RN;
    else if (0 == strcmp(uf, "RS"))
        return MSPED_CUF_RS;
    else if (0 == strcmp(uf, "RO"))
        return MSPED_CUF_RO;
    else if (0 == strcmp(uf, "RR"))
        return MSPED_CUF_RR;
    else if (0 == strcmp(uf, "SC"))
        return MSPED_CUF_SC;
    else if (0 == strcmp(uf, "SP"))
        return MSPED_CUF_SP;
    else if (0 == strcmp(uf, "SE"))
        return MSPED_CUF_SE;
    else if (0 == strcmp(uf, "TO"))
        return MSPED_CUF_TO;
    return MSPED_CUF_NENHUM;
}

bool msped_validar_tipo(enum MSPED_TIPO tipo) {
    switch (tipo) {
        case MSPED_TIPO_NENHUM:
            return false;
        case MSPED_TIPO_NFE:
            return true;
        case MSPED_TIPO_CTE:
            return true;
        case MSPED_TIPO_MDFE:
            return true;
    }
    return false;
}

bool msped_validar_cuf(enum MSPED_CUF cuf) {
    switch (cuf) {
        case MSPED_CUF_NENHUM:
            return false;
        case MSPED_CUF_AC:
            return true;
        case MSPED_CUF_AL:
            return true;
        case MSPED_CUF_AP:
            return true;
        case MSPED_CUF_AM:
            return true;
        case MSPED_CUF_AN:
            return true;
        case MSPED_CUF_BA:
            return true;
        case MSPED_CUF_CE:
            return true;
        case MSPED_CUF_DF:
            return true;
        case MSPED_CUF_ES:
            return true;
        case MSPED_CUF_GO:
            return true;
        case MSPED_CUF_MA:
            return true;
        case MSPED_CUF_MT:
            return true;
        case MSPED_CUF_MS:
            return true;
        case MSPED_CUF_MG:
            return true;
        case MSPED_CUF_PA:
            return true;
        case MSPED_CUF_PB:
            return true;
        case MSPED_CUF_PR:
            return true;
        case MSPED_CUF_PE:
            return true;
        case MSPED_CUF_PI:
            return true;
        case MSPED_CUF_RJ:
            return true;
        case MSPED_CUF_RN:
            return true;
        case MSPED_CUF_RS:
            return true;
        case MSPED_CUF_RO:
            return true;
        case MSPED_CUF_RR:
            return true;
        case MSPED_CUF_SC:
            return true;
        case MSPED_CUF_SP:
            return true;
        case MSPED_CUF_SE:
            return true;
        case MSPED_CUF_TO:
            return true;
    }
    return false;
}

bool msped_validar_ambiente(enum MSPED_AMBIENTE ambiente) {
    switch (ambiente) {
        case MSPED_AMB_NENHUM:
            return false;
        case MSPED_AMB_PRODUCAO:
            return true;
        case MSPED_AMB_HOMOLOGACAO:
            return true;
    }
    return false;
}

const char *msped_ambiente_para_str(enum MSPED_AMBIENTE ambiente) {
    switch (ambiente) {
        case MSPED_AMB_NENHUM:
            return S_MSPED_NENHUM;
        case MSPED_AMB_PRODUCAO:
            return "producao";
        case MSPED_AMB_HOMOLOGACAO:
            return "homologacao";
        default:
            return NULL;
    }
}

bool msped_validar_modelo_nf(enum MSPED_NF_MODELO modelo) {
    switch (modelo) {
        case MSPED_NF_NENHUM:
            return false;
        case MSPED_NF_MODELO_55:
            return true;
        case MSPED_NF_MODELO_65:
            return true;
    }
    return false;
}

const char *msped_modelo_para_str(enum MSPED_NF_MODELO modelo) {
    switch (modelo) {
        case MSPED_NF_NENHUM:
            return S_MSPED_NENHUM;
        case MSPED_NF_MODELO_55:
            return "55";
        case MSPED_NF_MODELO_65:
            return "65";
        default:
            return NULL;
    }
}

enum MSPED_NF_MODELO msped_str_para_modelo(const char *str) {
    if (mu_is_empty(str))
        return MSPED_NF_NENHUM;
    if (0 == strcmp(str, "55"))
        return MSPED_NF_MODELO_55;
    else if (0 == strcmp(str, "65"))
        return MSPED_NF_MODELO_65;
    return MSPED_NF_NENHUM;
}

bool msped_validar_chave(const char *chave) {
    return !mu_is_empty(chave) && mu_match("^[0-9]{44}$", chave);
}

#ifdef _WIN32

struct tm *localtime_r(const time_t *timep, struct tm *result) {
    // localtime() na MSVCRT.DLL é thread-safe, mas não "reentrant"
    memcpy(result, localtime(timep), sizeof(struct tm));
    return result;
}

#endif

void msped_gerar_id_lote_sleep(char *id, useconds_t usegs) {
    struct tm lt;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &lt);
    snprintf(id, 16, "%d%02d%02d%02d%02d%02d%01ld", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min,
             lt.tm_sec, tv.tv_usec / 100000);
    if (usegs > 0)
        usleep(usegs);
}

void msped_gerar_id_lote(char *id) {
    msped_gerar_id_lote_sleep(id, 100000);
}

char *msped_data_hora_utc() {
#ifdef WIN32
#define TZ_FMT(tz) ((tz) > 0 ? "-%H:%M" : "+%H:%M")
    char *res;
    char buf[64];
    char zbuf[8];
    size_t len;
    size_t zlen;
    time_t t;
    struct tm *tm;
    tzset();
    t = abs((int) timezone);
    tm = gmtime(&t);
    if (NULL == tm)
        return NULL;
    if (0 == strftime(zbuf, sizeof(zbuf), TZ_FMT(timezone), tm))
        return NULL;
    t = time(NULL);
    tm = localtime(&t);
    if (NULL == tm)
        return NULL;
    if (0 == strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm))
        return NULL;
    len = strlen(buf);
    zlen = strlen(zbuf);
    res = malloc(len + zlen + 1);
    if (NULL == res)
        return NULL;
    memcpy(res, buf, len);
    memcpy(res + len, zbuf, zlen);
    res[len + zlen] = '\0';
    return res;
#else
    char *res;
    char buf[64];
    size_t len;
    time_t t;
    struct tm *tm;
    t = time(NULL);
    tm = localtime(&t);
    if (NULL == tm)
        return NULL;
    if (0 == strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", tm))
        return NULL;
    len = strlen(buf);
    res = malloc(len + 2);
    if (NULL == res)
        return NULL;
    memcpy(res, buf, len - 2);
    memset(res + (len - 2), ':', 1);
    memcpy(res + (len - 1), buf + (len - 2), 2);
    res[len + 1] = '\0';
    return res;
#endif
}

char *msped_limpar_mensagem(const char *msg) {
    char *nmsg;
    char *nnmsg;
    if (NULL == msg)
        return NULL;
    nmsg = mu_mreplace(msg, "", " standalone=\"no\"", "default:", ":default", "\n", "\r", "\t", NULL);
    nnmsg = mu_replace(nmsg, "> ", ">");
    free(nmsg);
    if (NULL == nnmsg)
        return NULL;
    if (strstr(nnmsg, "> ") != NULL) {
        nmsg = msped_limpar_mensagem(nnmsg);
        free(nnmsg);
        nnmsg = nmsg;
    }
    return nnmsg;
}

char *msped_limpar_prot(const char *proc_xml) {
    uint8_t i;
    char *res;
    char *tmp1;
    char *tmp2;
    char *tmp3;
    char *app;
    char *aapp[] = {"nfe", "cte", "mdfe"};
    if (NULL == proc_xml)
        return NULL;
    res = msped_limpar_mensagem(proc_xml);
    if (NULL == res)
        return NULL;
    for (i = 0; i < sizeof(aapp) / sizeof(aapp[0]); i++) {
        app = aapp[i];
        tmp1 = mu_fmt("xmlns=\"http://www.portalfiscal.inf.br/%s\" xmlns=\"http://www.w3.org/2000/09/xmldsig#\"", app);
        tmp2 = mu_fmt("xmlns=\"http://www.portalfiscal.inf.br/%s\"", app);
        tmp3 = mu_replace(res, tmp1, tmp2);
        free(tmp1);
        free(tmp2);
        free(res);
        res = tmp3;
    }
    return res;
}

char *msped_limpar_xml(const char *xml, bool rem_enc) {
    uint8_t i;
    char *ret_xml;
    char *tmp;
    char *afind[] = {
            "xmlns:default=\"http://www.w3.org/2000/09/xmldsig#\"",
            " standalone=\"no\"",
            "default:",
            ":default",
            "\n",
            "\r",
            "\t"
    };
    char *afind2[] = {
            "<?xml version=\"1.0\"?>",
            "<?xml version=\"1.0\" encoding=\"utf-8\"?>",
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>",
            "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>",
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
    };
    if (NULL == xml)
        return NULL;
    ret_xml = strdup(xml);
    if (NULL == ret_xml)
        return NULL;
    for (i = 0; i < sizeof(afind) / sizeof(afind[0]); i++) {
        tmp = mu_replace(ret_xml, afind[i], "");
        free(ret_xml);
        ret_xml = tmp;
    }
    if (rem_enc)
        for (i = 0; i < sizeof(afind2) / sizeof(afind2[0]); i++) {
            tmp = mu_replace(ret_xml, afind2[i], "");
            free(ret_xml);
            ret_xml = tmp;
        }
    return ret_xml;
}

long msped_static_str_to_long(const char *str) {
    long res;
    if (mu_is_empty(str))
        return 0;
    res = strtol(str, NULL, 10);
    return res;
}

long msped_str_to_long(char *str) {
    long res;
    if (mu_is_empty(str))
        return 0;
    res = strtol(str, NULL, 10);
    free(str);
    return res;
}