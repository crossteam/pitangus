/*
 * Pitangus - Biblioteca multiplataforma para geração e envio de documentos SPED da SEFAZ.
 *
 * Copyright (c) 2017 Pablo G. Gallardo, Silvio Clecio, Waldir Paim
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
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* TODO: testar no Windows e Android. */
const char *ptg_tamanho(uint64_t size) {
    if (size == 0)
        return "B";
    const char *sizes[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB"};
    return sizes[(uint8_t) floor(log(size) / log(1000))];
}

/* TODO: testar no Windows e Android. */
char *ptg_agora_rfc2822() {
    struct tm *tm;
    char *ret, buf[64];
    size_t len;
    time_t t;
    t = time(NULL);
    if (!(tm = localtime(&t)))
        return NULL;
    /* Ver man: https://linux.die.net/man/3/strftime */
    if (strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %z", tm) == 0)
        return NULL;
    len = strlen(buf);
    if (!(ret = malloc(len + sizeof(char))))
        return NULL;
    if (!memcpy(ret, buf, len))
        return NULL;
    ret[len] = '\0';
    return ret;
}