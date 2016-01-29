#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *parse_url(const char *header)
{
    // Get url from 'GET */* HTTP/1.1\r\n...'
    char *beg = strstr(header, " ") + 1;
    char *end = strstr(beg, " ");

    // Abort '?*' in url
    char *sep = strstr(beg, "?");
    if (sep != NULL && sep < end) {
        end = sep;
    }

    size_t len = end - beg;
    char *url = malloc(len + 1);
    strncpy(url, beg, len);
    url[len] = '\0';

    return url;
}

