/*
 * libmicroutils - Micro cross-platform C library for handling strings, files,
 * regex, hashs, among others.
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

#ifndef CONFIG_H
#define CONFIG_H
#include <config.h>
#endif
#include <microutils.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <regex.h>
#include <libgen.h>
#include <assert.h>
#include <zlib.h>

#define _(String) (String)

#ifdef MINGW

static const char *strcasestr(const char *s1, const char *s2) {
    if (!s1 || !s2)
        return NULL;
    size_t n = strlen(s2);
    while (*s1)
        if (!strnicmp(s1++, s2, n))
            return s1 - 1;
    return 0;
}

static char *strsep(char **stringp, const char *delim) {
    if (!*stringp)
        return NULL;
    char *token_start = *stringp;
    *stringp = strpbrk(token_start, delim);
    if (*stringp) {
        **stringp = '\0';
        (*stringp)++;
    }
    return token_start;
}

#endif

const char *mu_ver() {
    return VERSION;
}

/* system */

int mu_errno() {
    return errno;
}

char *mu_err(int errnum) {
    char buf[256];
    char *ret;
    int saved_errno = errno;
#if defined(__GLIBC__) && !((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE)
    ret = strerror_r(errnum, buf, sizeof(buf));
#elif defined(WIN32)
    strerror_s(buf, sizeof(buf), errnum);
    ret = buf;
#endif
    errno = saved_errno;
    return strdup(ret);
}

void *mu_alloc(size_t size) {
    return calloc(1, size);
}

void mu_free(void *ptr) {
    free(ptr);
}

bool mu_exec2(const char *dir, const char *name, const char **args, bool waited, bool showed, bool term, int timeout) {
#ifdef WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char cmd[MAX_PATH];
    int pos, ret;
    if (mu_is_empty(name))
        return EINVAL;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = (WORD) (showed ? SW_SHOWNORMAL : SW_HIDE);
    ZeroMemory(&pi, sizeof(pi));
    pos = snprintf(cmd, sizeof(cmd), "%s", name);
    if (args)
        while (*args) {
            pos += snprintf(cmd + pos, sizeof(cmd), " %s", *args);
            args++;
        }
    if (!CreateProcessA(NULL, (LPSTR) cmd, NULL, NULL, FALSE,
                        term ? CREATE_NEW_CONSOLE : NORMAL_PRIORITY_CLASS, NULL, dir, &si, &pi))
        return false;
    if (waited) {
        WaitForSingleObject(pi.hProcess, (timeout == -1) ? INFINITE : (unsigned int) timeout);
        GetExitCodeProcess(pi.hProcess, (LPDWORD) &ret);
        if (ret != 0)
            errno = ret;
    } else
        ret = 0;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return ret == 0;
#else
    return false;
#endif
}

bool mu_exec(const char *name) {
    return mu_exec2(NULL, name, NULL, false, false, false, 0);
}

/* strings */

char *mu_replace(const char *str, const char *substr, const char *rep) {
    char *tmp;
    char *newstr;
    char *oldstr;
    size_t oldstr_len;
    size_t substr_len;
    size_t rplcmt_len;
    if (!str || !substr || !rep)
        return NULL;
    newstr = strdup(str);
    if (strcmp(str, rep) == 0)
        return newstr;
    substr_len = strlen(substr);
    rplcmt_len = strlen(rep);
    while ((tmp = strstr(newstr, substr))) {
        oldstr = newstr;
        oldstr_len = strlen(oldstr);
        newstr = malloc(sizeof(char) * (oldstr_len - substr_len + rplcmt_len + sizeof(char)));
        if (!newstr) {
            free(oldstr);
            return NULL;
        }
        memcpy(newstr, oldstr, tmp - oldstr);
        memcpy(newstr + (tmp - oldstr), rep, rplcmt_len);
        memcpy(newstr + (tmp - oldstr) + rplcmt_len, tmp + substr_len, oldstr_len - substr_len - (tmp - oldstr));
        memset(newstr + oldstr_len - substr_len + rplcmt_len, 0, sizeof(char));
        free(oldstr);
    }
    return newstr;
}

char *mu_mreplace(const char *str, const char *rep, ...) {
    char *ret;
    char *tmp;
    va_list va;
    if (!str || !rep)
        return NULL;
    ret = strdup(str);
    if (!ret)
        return NULL;
    va_start(va, rep);
    while ((tmp = va_arg(va, char *))) {
        tmp = mu_replace(ret, tmp, rep);
        if (!tmp)
            continue;
        free(ret);
        ret = tmp;
    }
    va_end(va);
    return ret;
}

bool mu_split(const char *str, const char *delim, mu_split_cb cb, void *cls) {
    bool ret;
    char *step;
    char *tmp;
    char *token;
    if (!str || !delim)
        return false;
    if (cb && !cb(cls, str, NULL))
        return false;
    ret = false;
    step = strdup(str);
    tmp = step;
    while ((token = strsep(&step, delim))) {
        ret = true;
        if (cb && !cb(cls, step, token))
            break;
    }
    free(tmp);
    return ret;
}

char *mu_trim(const char *str) {
    char *ret;
    size_t ofs;
    size_t len;
    if (!str)
        return NULL;
    len = strlen(str);
    while (len > 0 && str[len] <= ' ')
        len--;
    len++;
    ofs = 0;
    while (ofs < len && str[ofs] <= ' ')
        ofs++;
    len -= ofs;
    ret = malloc(len + sizeof(char));
    memcpy(ret, str + ofs, len);
    ret[len] = '\0';
    return ret;
}

char *mu_ltrim(const char *str) {
    char *ret;
    size_t ofs;
    size_t len;
    if (!str)
        return NULL;
    len = strlen(str);
    ofs = 0;
    while (ofs < len && str[ofs] <= ' ')
        ofs++;
    len = len - ofs;
    ret = malloc(len + sizeof(char));
    memcpy(ret, str + ofs, len);
    ret[len] = '\0';
    return ret;
}

char *mu_rtrim(const char *str) {
    char *ret;
    size_t len;
    if (!str)
        return NULL;
    len = strlen(str);
    while (len > 0 && str[len] <= ' ')
        len--;
    if (len > 0)
        len++;
    ret = malloc(len + sizeof(char));
    memcpy(ret, str, len);
    ret[len] = '\0';
    return ret;
}

char *mu_nums(const char *str) {
    char *ret;
    size_t dl;
    size_t sl;
    if (!str)
        return NULL;
    sl = strlen(str);
    ret = malloc(sl + sizeof(char));
    if (!ret)
        return NULL;
    dl = 0;
    while (*str) {
        if (isdigit(*str)) {
            ret[dl] = *str;
            dl++;
        }
        str++;
    }
    ret = realloc(ret, dl);
    ret[dl] = '\0';
    return ret;
}

char *mu_between(const char *str, const char *p1, const char *p2) {
    const char *i1 = strstr(str, p1);
    if (i1 && p2) {
        const size_t pl1 = strlen(p1);
        const char *i2 = strstr(i1 + pl1, p2);
        const size_t mlen = i2 - (i1 + pl1);
        char *ret = malloc(mlen + sizeof(char));
        if (ret) {
            memcpy(ret, i1 + pl1, mlen);
            ret[mlen] = '\0';
            return ret;
        }
    }
    return NULL;
}

char *mu_ibetween(const char *str, const char *p1, const char *p2) {
    const char *i1 = strcasestr(str, p1);
    if (i1 && p2) {
        const size_t pl1 = strlen(p1);
        const char *i2 = strcasestr(i1 + pl1, p2);
        const size_t mlen = i2 - (i1 + pl1);
        char *ret = malloc(mlen + sizeof(char));
        if (ret) {
            memcpy(ret, i1 + pl1, mlen);
            ret[mlen] = '\0';
            return ret;
        }
    }
    return NULL;
}

char *mu_fmt(const char *fmt, ...) {
    va_list va;
    char *ret;
    int nb;
    va_start(va, fmt);
    nb = vasprintf(&ret, fmt, va);
    va_end(va);
    return nb != -1 ? ret : NULL;
}

bool mu_is_empty(const char *str) {
    return !str || *str == 0;
}

void mu_uitoh(uint8_t val, char *hex) {
    int8_t len;
    for (len = 1; len >= 0; val >>= 4, len--)
        hex[len] = MU_HEX_CHARS[val & 0xf];
}

void mu_uitouh(uint8_t val, char *hex) {
    int8_t len;
    for (len = 1; len >= 0; val >>= 4, len--)
        hex[len] = MU_HEX_UCHARS[val & 0xf];
}

char *mu_stoh(const char *str) {
    uint32_t i;
    size_t len;
    char *dest;
    len = strlen(str);
    dest = malloc((len * 2) + sizeof(char));
    if (!dest)
        return NULL;
    for (i = 0; i < len; i++)
        mu_uitoh((uint8_t) str[i], dest + i * 2);
    *(dest + i * 2) = '\0';
    return dest;
}

char *mu_stouh(const char *str) {
    uint32_t i;
    size_t len;
    char *dest;
    len = strlen(str);
    dest = malloc((len * 2) + sizeof(char));
    if (!dest)
        return NULL;
    for (i = 0; i < len; i++)
        mu_uitouh((uint8_t) str[i], dest + i * 2);
    *(dest + i * 2) = '\0';
    return dest;
}

const char *mu_size_unit(uint64_t size) {
    if (size == 0)
        return "B";
    const char *sizes[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB"};
    return sizes[(uint8_t) floor(log(size) / log(1000))];
}

char *mu_now_rfc2822() {
    char *ret;
    char buf[64];
    size_t len;
    time_t t;
    struct tm *tm;
    t = time(NULL);
    tm = localtime(&t);
    if (!tm)
        return NULL;
    /* see man: https://linux.die.net/man/3/strftime */
    if (strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %z", tm) == 0)
        return NULL;
    len = strlen(buf);
    ret = malloc(len + sizeof(char));
    if (!ret)
        return NULL;
    memcpy(ret, buf, len);
    ret[len] = '\0';
    return ret;
}

/* file */

struct mu_mime {
    const char *extension;
    const char *type;
};

const struct mu_mime mu_mimes[] = {
        {".gif",  "image/gif"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".txt",  "text/plain"},
        {".html", "text/html"},
        {".xml",  "application/xml"},
        {".pdf",  "application/pdf"}
};

bool mu_remove(const char *path) {
    return remove(path) == 0;
}

bool mu_rename(const char *old_path, const char *new_path) {
    return rename(old_path, new_path) == 0;
}

bool mu_exists(const char *path) {
    return access(path, F_OK) != -1;
}

char *mu_change_ext(const char *filename, const char *ext) {
    char *ret;
    size_t i;
    size_t el;
    if (!filename || !ext)
        return NULL;
    el = strlen(ext);
    char *dot = strrchr(filename, '.');
    if (!dot) {
        i = strlen(filename);
        ret = malloc(i + el + sizeof(char));
        if (!ret)
            return NULL;
        strcpy(ret, filename);
        strncat(ret, ext, el);
        ret[i + el] = '\0';
        return ret;
    }
    i = dot - filename;
    ret = malloc((i + el) + sizeof(char));
    if (!ret)
        return NULL;
    memcpy(ret, filename, i);
    memcpy(ret + i, ext, el);
    ret[i + el] = '\0';
    return ret;
}

char *mu_ext(const char *filename) {
    char *ret;
    size_t len;
    size_t pos;
    if (!filename)
        return NULL;
    len = strlen(filename);
    if (len == 0)
        return NULL;
    pos = len;
    while (pos > 0) {
        if (*(filename + pos) == '.')
            break;
        pos--;
    }
    if (pos < 1)
        return NULL;
    len -= pos;
    ret = malloc(len + sizeof(char));
    if (!ret)
        return NULL;
    memcpy(ret, filename + pos, len);
    ret[len] = 0;
    return ret;
}

char *mu_filename(const char *filename) {
    char *ret;
    char *tmp;
    if (!filename)
        return NULL;
    tmp = strdup(filename);
    ret = strdup(basename(tmp));
    free(tmp);
    return ret;
}

char *mu_path(const char *filename) {
    char *ret;
    char *tmp;
    if (!filename)
        return NULL;
    tmp = strdup(filename);
    ret = strdup(dirname(tmp));
    free(tmp);
    return ret;
}

char *mu_ftos(const char *filename) {
    FILE *fp;
    char *ret;
    size_t len;
    fp = fopen(filename, "rb");
    if (!fp)
        return NULL;
    fseek(fp, 0, SEEK_END);
    len = (size_t) ftell(fp);
    fseek(fp, 0, SEEK_SET);
    ret = malloc(len + sizeof(char));
    if (ret && fread(ret, sizeof(char), len, fp))
        ret[len] = '\0';
    fclose(fp);
    return ret;
}

bool mu_stofa(const char *str, const char *filename, bool append) {
    FILE *fp;
    size_t len;
    if (!str || mu_is_empty(filename))
        return false;
    fp = fopen(filename, append ? "ab+" : "wb");
    if (!fp)
        return false;
    len = strlen(str);
    if (fwrite(str, sizeof(char), len, fp) != len) {
        fclose(fp);
        return false;
    }
    return fclose(fp) == 0;
}

bool mu_stof(const char *str, const char *filename) {
    return mu_stofa(str, filename, false);
}

const char *mu_etomime(const char *ext) {
    unsigned int i;
    struct mu_mime mime;
    if (!ext)
        return NULL;
    if (strlen(ext) == 0)
        return NULL;
    for (i = 0; i < MU_ARRAY_SIZE(mu_mimes); i++) {
        mime = mu_mimes[i];
        if (strcasecmp(ext, mime.extension) == 0)
            return mime.type;
    }
    return MU_DEF_MIME;
}

const char *mu_fntomime(const char *filename) {
    size_t fl;
    size_t el;
    unsigned int i;
    struct mu_mime mime;
    if (!filename)
        return NULL;
    fl = strlen(filename);
    if (fl == 0)
        return NULL;
    for (i = 0; i < MU_ARRAY_SIZE(mu_mimes); i++) {
        mime = mu_mimes[i];
        el = strlen(mime.extension);
        if (fl >= el && strcasecmp(filename + fl - el, mime.extension) == 0)
            return mime.type;
    }
    return MU_DEF_MIME;
}

/* regex */

bool mu_mmatch(const char *pattern, const char *str, bool ncase, mu_mmatch_cb cb, void *cls) {
    bool ret;
    int flags;
    size_t offset;
    size_t length;
    regex_t regex;
    regmatch_t regmatch;
    flags = REG_EXTENDED;
    if (ncase)
        flags |= REG_ICASE;
    if (regcomp(&regex, pattern, flags) == REG_NOMATCH)
        return false;
    ret = false;
    flags = 0;
    offset = 0;
    length = strlen(str);
    while (regexec(&regex, str + offset, 1, &regmatch, flags) != REG_NOMATCH) {
        ret = true;
        flags = REG_NOTBOL;
        if (cb && !cb(cls, str, offset + regmatch.rm_so, offset + regmatch.rm_eo))
            break;
        offset += regmatch.rm_eo;
        if (regmatch.rm_so == regmatch.rm_eo)
            offset += 1;
        if (offset > length)
            break;
    }
    regfree(&regex);
    return ret;
}

bool mu_match(const char *pattern, const char *str) {
    return mu_match2(pattern, str, false);
}

bool mu_match2(const char *pattern, const char *str, bool ncase) {
    regex_t re;
    int ret;
    int flags;
    flags = REG_EXTENDED | REG_NOSUB;
    if (ncase)
        flags |= REG_ICASE;
    if (regcomp(&re, pattern, flags) != 0)
        return false;
    ret = regexec(&re, str, 0, NULL, 0);
    regfree(&re);
    return REG_NOMATCH != ret;
}

bool mu_imatch(const char *pattern, const char *str) {
    return mu_match2(pattern, str, true);
}

struct _mu_extract_holder {
    char *buf;
    bool recur;
};

static bool _mu_extract_cb(void *cls, const char *str, size_t start, size_t end) {
    struct _mu_extract_holder *holder = cls;
    char *part;
    if ((end - start) > 0) {
        part = malloc((size_t) (start + end + 1));
        if (part) {
            memcpy(part, str + start, end - start);
            part[end - start] = 0;
            if (!holder->buf)
                holder->buf = part;
            else {
                holder->buf = strcat(holder->buf, part);
                free(part);
            }
            if (!holder->recur)
                return false;
        }
    }
    return true;
}

char *mu_mextract(const char *pattern, const char *str, bool ncase, bool recur) {
    struct _mu_extract_holder holder;
    holder.buf = NULL;
    holder.recur = recur;
    if (!mu_mmatch(pattern, str, ncase, &_mu_extract_cb, &holder))
        return NULL;
    return holder.buf;
}

/* deflate/inflate */

int mu_zdeflate(mu_zstream_data_cb read_cb, void *read_cls, mu_zstream_data_cb write_cb, void *write_cls,
                mu_zstream_eof_cb eof_cb, void *eof_cls, int level) {
    z_stream strm;
    int ret, flush;
    unsigned have;
    unsigned char in[MU_ZBUF_CHUNK], out[MU_ZBUF_CHUNK];
    if (!read_cb || !read_cls || !write_cb || !write_cls || !eof_cb || !eof_cls) {
        errno = EINVAL;
        return Z_ERRNO;
    }
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;
    do {
        strm.avail_in = (uInt) read_cb(read_cls, in, MU_ZBUF_CHUNK);
        if (strm.avail_in == -1) {
            deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = eof_cb(eof_cls) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        do {
            strm.avail_out = MU_ZBUF_CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            assert(ret != Z_STREAM_ERROR);
            have = MU_ZBUF_CHUNK - strm.avail_out;
            if (write_cb(write_cls, out, have) == -1) {
                deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);
    deflateEnd(&strm);
    return Z_OK;
}

static size_t _mu_zfstream_read_cb(void *cls, void *buf, size_t len) {
    size_t ret;
    ret = fread(buf, 1, len, cls);
    if (ferror(cls))
        return (size_t) -1;
    return ret;
}

static size_t _mu_zfstream_write_cb(void *cls, void *buf, size_t len) {
    size_t ret;
    if ((ret = fwrite(buf, 1, len, cls)) != len || ferror(cls))
        return (size_t) -1;
    return ret;
}

static bool _mu_zfstream_eof_cb(void *cls) {
    return (bool) feof(cls);
}

int mu_zfdeflate(const char *filename_in, const char *filename_out, int level) {
    FILE *fin, *fout;
    int ret;
    if (mu_is_empty(filename_in) || mu_is_empty(filename_out)) {
        errno = EINVAL;
        return Z_ERRNO;
    }
    fin = fopen(filename_in, "rb");
    if (!fin)
        return Z_ERRNO;
    fout = fopen(filename_out, "wb");
    if (!fout) {
        fclose(fin);
        errno = EINVAL;
        return Z_ERRNO;
    }
    ret = mu_zdeflate(&_mu_zfstream_read_cb, fin, &_mu_zfstream_write_cb, fout, &_mu_zfstream_eof_cb, fin, level);
    fclose(fin);
    fclose(fout);
    return ret;
}

int mu_zinflate(mu_zstream_data_cb read_cb, void *read_cls, mu_zstream_data_cb write_cb, void *write_cls) {
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[MU_ZBUF_CHUNK];
    unsigned char out[MU_ZBUF_CHUNK];
    if (!read_cb || !read_cls || !write_cb || !write_cls) {
        errno = EINVAL;
        return Z_ERRNO;
    }
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;
    do {
        strm.avail_in = (uInt) read_cb(read_cls, in, MU_ZBUF_CHUNK);
        if (strm.avail_in == -1) {
            inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;
        do {
            strm.avail_out = MU_ZBUF_CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);
            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    inflateEnd(&strm);
                    return ret;
                default:;
            }
            have = MU_ZBUF_CHUNK - strm.avail_out;
            if (write_cb(write_cls, out, have) == -1) {
                inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
    } while (ret != Z_STREAM_END);
    inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int mu_zfinflate(const char *filename_in, const char *filename_out) {
    FILE *fin, *fout;
    int ret;
    if (mu_is_empty(filename_in) || mu_is_empty(filename_out)) {
        errno = EINVAL;
        return Z_ERRNO;
    }
    fin = fopen(filename_in, "rb");
    if (!fin)
        return Z_ERRNO;
    fout = fopen(filename_out, "wb");
    if (!fout) {
        fclose(fin);
        errno = EINVAL;
        return Z_ERRNO;
    }
    ret = mu_zinflate(&_mu_zfstream_read_cb, fin, &_mu_zfstream_write_cb, fout);
    fclose(fin);
    fclose(fout);
    return ret;
}

const char *mu_zerr(int zerrnum) {
    switch (zerrnum) {
        case Z_STREAM_ERROR:
            return _("Invalid compression level");
        case Z_DATA_ERROR:
            return _("Invalid or incomplete deflate data");
        case Z_MEM_ERROR:
            return _("Out of memory");
        case Z_BUF_ERROR:
            return _("Buffer error");
        case Z_VERSION_ERROR:
            return _("ZLib version mismatch");
        default:
            NULL;
    }
    return NULL;
}

/* hash */

/* Base64 encoding/decoding (RFC1341) functions from Jouni Malinen under BSD license. */

static const unsigned char _mu_b64enc_tbl[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char *mu_b64enc(const unsigned char *src, size_t len, size_t *out_len) {
    unsigned char *out, *pos;
    const unsigned char *end, *in;
    size_t olen;
    int line_len;
    olen = len * 4 / 3 + 4;
    olen += olen / 76;
    olen++;
    if (olen < len)
        return NULL;
    out = malloc(olen);
    if (!out)
        return NULL;
    end = src + len;
    in = src;
    pos = out;
    line_len = 0;
    while (end - in >= 3) {
        *pos++ = _mu_b64enc_tbl[in[0] >> 2];
        *pos++ = _mu_b64enc_tbl[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = _mu_b64enc_tbl[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = _mu_b64enc_tbl[in[2] & 0x3f];
        in += 3;
        line_len += 4;
        if (line_len >= 76) {
            *pos++ = '\n';
            line_len = 0;
        }
    }
    if (end - in) {
        *pos++ = _mu_b64enc_tbl[in[0] >> 2];
        if (end - in == 1) {
            *pos++ = _mu_b64enc_tbl[(in[0] & 0x03) << 4];
            *pos++ = '=';
        } else {
            *pos++ = _mu_b64enc_tbl[((in[0] & 0x03) << 4) | (in[1] >> 4)];
            *pos++ = _mu_b64enc_tbl[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
        line_len += 4;
    }
    if (line_len)
        *pos++ = '\n';
    *pos = '\0';
    if (out_len)
        *out_len = (size_t) (pos - out);
    return out;
}

static const unsigned char _mu_b64dec_tbl[256] = {
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x3e, 0x80, 0x80, 0x80, 0x3f,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x80, 0x80,
        0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
        0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
        0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
        0x31, 0x32, 0x33, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80
};

unsigned char *mu_b64dec(const unsigned char *src, size_t len, size_t *out_len) {
    unsigned char *out, *pos, block[4];
    size_t i, count, olen;
    int pad = 0;
    count = 0;
    for (i = 0; i < len; i++) {
        if (_mu_b64dec_tbl[src[i]] != 0x80)
            count++;
    }
    if (count == 0 || count % 4)
        return NULL;
    olen = (count / 4 * 3) + 1;
    pos = out = malloc(olen);
    if (!out)
        return NULL;
    count = 0;
    for (i = 0; i < len; i++) {
        unsigned char tmp = _mu_b64dec_tbl[src[i]];
        if (tmp == 0x80)
            continue;
        if (src[i] == '=')
            pad++;
        block[count] = tmp;
        count++;
        if (count == 4) {
            *pos++ = ((block[0] << 2) | (block[1] >> 4));
            *pos++ = ((block[1] << 4) | (block[2] >> 2));
            *pos++ = ((block[2] << 6) | block[3]);
            count = 0;
            if (pad) {
                if (pad == 1)
                    pos--;
                else if (pad == 2)
                    pos -= 2;
                else {
                    free(out);
                    return NULL;
                }
                break;
            }
        }
    }
    *pos = '\0';
    *out_len = (size_t) (pos - out);
    return out;
}