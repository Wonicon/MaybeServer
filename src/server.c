#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "debug.h"
#include "http.h"

const char *serv_name = NULL;
const char *root_path = NULL;

int init_server(short portno); 
void response(int connect_sock);


int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: [prog] port-no server-name root-path\n");
        exit(1);
    }

    serv_name = argv[2];
    root_path = argv[3];

    int sockfd = init_server( (short) atoi(argv[1]) );

    struct sockaddr_in cli_addr;
    unsigned int cli_len = sizeof(cli_addr);

    for (;;) {
        int connect_sock = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
        if (fork() == 0) {
            response(connect_sock);
            shutdown(connect_sock, SHUT_RDWR);
            close(connect_sock);
            exit(0);
        }
    }

    close(sockfd);
    return 0;
}


int init_server(short portno)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        panic("Cannot open socket");
    }

    struct sockaddr_in serv_addr = {};        // Init.
    serv_addr.sin_family = AF_INET;           // Internet address family.
    serv_addr.sin_addr.s_addr = INADDR_ANY;   // Get the default ip address.
    serv_addr.sin_port = htons(portno);       // Set port number.

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        panic("Cannot bind");
    }

    listen(sockfd, 5);  // Indicate the willingness of receiving connections,
                        // This system call does not stall the process.
    return sockfd;
}


void response(int sockfd)
{
    char buffer[1024] = {};

    ssize_t n_read = read(sockfd, buffer, sizeof(buffer));
    if (n_read < 0) {
        panic("Failed to read");
    }

    fprintf(stdout, "%s\n", buffer);

    // Get the full path.
    char *url = parse_url(buffer);
    strcpy(buffer, root_path);
    strcat(buffer, url);
    free(url);

    int fd = open(buffer, O_RDONLY);
    if (fd != -1) {
        char header[] = "HTTP/1.1 200 OK\n\n";
        write(sockfd, header, sizeof(header) - 1);
        while ((n_read = read(fd, buffer, sizeof(buffer))) > 0) {
            write(sockfd, buffer, n_read);
        }
    }
    else {
        char header[] = "HTTP/1.1 404 Not Found\n\n";
        write(sockfd, header, sizeof(header) - 1);
    }
}

