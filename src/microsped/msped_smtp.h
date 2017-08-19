#ifndef MSPED_SMTP_H
#define MSPED_SMTP_H

struct msped_smtp {
    CURL *curl;
    bool depuravel;
    bool ok;
};

struct msped_smtp_payload {
    char *buf;
    size_t len;
};

struct msped_smtp_multipart_record {
    char boundary[33];
    char *payload;
};

#endif