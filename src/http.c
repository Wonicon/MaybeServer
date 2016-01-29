#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

size_t tok(char *dst, size_t dst_size, const char *src, const char **next, const char *delim)
{
    if (*src == '\0') {
        return 0;
    }

    char *delim_pos = strstr(src, delim);
    size_t tok_size = (delim_pos == NULL) ? strlen(src) : (delim_pos - src);
    size_t size = min(dst_size - 1, tok_size);

    memset(dst, '\0', dst_size);
    strncpy(dst, src, size);

    if (next != NULL) {
        if (delim_pos == NULL) {
            *next = src + strlen(src);
        }
        else {
            *next = delim_pos + strlen(delim);
        }
    }

    return size;
}

struct HttpHeader *parse_httpheader(const char *text)
{

    char token[256];
    char line[1024];
    const char *tail = line;

    struct HttpHeader *hdr = malloc(sizeof(*hdr));

    tok(line, sizeof(line), text, &text, "\r\n");

    // Get method
    tok(token, sizeof(token), tail, &tail, " ");

    if (!strcmp(token, "GET")) {
        hdr->method = GET;
    }

    // Get filename
    tok(token, sizeof(token), tail, &tail, " ");

    // Dup the string
    hdr->requested_file = malloc(strlen(token) + 1);
    strcpy(hdr->requested_file, token);

    return hdr;
}

void free_httpheader(struct HttpHeader *hdr)
{
    free(hdr->requested_file);
    free(hdr);
}

size_t response_header(char *dst, int status_code, char *body, size_t body_size)
{
    extern const char *serv_name;
    size_t n = sprintf(dst, "HTTP/1.1 %d\nServer:%s\n\n", status_code, serv_name);
    memcpy(dst + n, body, body_size);
    return n + body_size;
}

