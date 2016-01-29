#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>

enum HttpMethod {
    GET,
};

struct HttpHeader {
    enum HttpMethod method;
    char *requested_file;
};

struct HttpHeader *parse_httpheader(const char *text);

void free_httpheader(struct HttpHeader *hdr);

size_t response_header(char *dst, int status_code, char *body, size_t body_size);

#endif // HTTP_H
