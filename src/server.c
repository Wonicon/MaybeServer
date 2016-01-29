#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "debug.h"
#include "http.h"

const char *serv_name = NULL;
const char *root_path = NULL;

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: [prog] port-no server-name root-path\n");
        exit(1);
    }

    short port = (short)atoi(argv[1]);
    serv_name = argv[2];
    root_path = argv[3];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        panic("Cannot open socket");
    }

    struct sockaddr_in serv_addr = {};        // Init.
    serv_addr.sin_family = AF_INET;           // Internet address family.
    serv_addr.sin_addr.s_addr = INADDR_ANY;   // Get the default ip address.
    serv_addr.sin_port = htons(port);         // Set port number.

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        panic("Cannot bind");
    }

    listen(sockfd, 5);  // Indicate the willingness of receiving connections,
                        // which is limited to 5. This system call does not
                        // stall the process.

    int counter = 0;
    for (;;) {
        struct sockaddr_in cli_addr;
        unsigned int cli_len = sizeof(cli_addr);
        int connect_sock = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);

        if (fork() == 0) {
            if (connect_sock < 0) {
                panic("Cannot accept");
            }

            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));

            int n_read = read(connect_sock, buffer, sizeof(buffer));
            if (n_read < 0) {
                panic("Failed to read");
            }

            //fprintf(stdout, "[%d] Received %d bytes of message from %s:\n%s\n",
                    //counter, n_read, inet_ntoa(cli_addr.sin_addr), buffer);

            struct HttpHeader *phd = parse_httpheader(buffer);

            // Get the full path.
            memset(buffer, '\0', sizeof(buffer));
            strcat(buffer, root_path);
            strcat(buffer, phd->requested_file);

            char *response = NULL;
            size_t response_size = 0;

            FILE *fp = fopen(buffer, "rb");
            if (fp != NULL) {
                fseek(fp, 0L, SEEK_END);
                size_t file_size = ftell(fp);
                rewind(fp);

                char *file = malloc(file_size);
                fread(file, 1, file_size, fp);

                response = malloc(1024 + file_size);
                response_size = response_header(response, 200, file, file_size); 
            }
            else {
                char body[] = "<html>404</html>";
                response = malloc(1024 + sizeof(body));
                response_size = response_header(response, 404, body, sizeof(body));
            }

            int n_write = write(connect_sock, response, response_size);
            if (n_write < 0) {
                panic("Failed to write");
            }

            close(connect_sock);

            exit(0);
        }

        counter++;
    }

    close(sockfd);

    return 0;
}

