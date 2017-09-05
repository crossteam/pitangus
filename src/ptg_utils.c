#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

const char *ptg_tamanho(uint64_t size) {
    if (size == 0)
        return "B";
    const char *sizes[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB"};
    return sizes[(uint8_t) floor(log(size) / log(1000))];
}

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