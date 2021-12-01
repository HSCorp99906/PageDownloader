#include "../include/tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

int sockfd;
FILE* fp;


void close_and_die() {
    fclose(fp);
}

void write_chunks(unsigned int chunksize) {
    char chunk[chunksize];

    fp = fopen("page.html", "a");

    char readbuf[chunksize];
    unsigned short writechunk = 0;

    fputs("<!--", fp);

    while (read(sockfd, chunk, chunksize - 1) > 0) {
        if (!(writechunk)) {
            for (int i = 0; i < strlen(chunk); ++i) {
                if (chunk[i] == '<') {
                    writechunk = 1;
                    fputs("-->", fp);
                }
            }
        }

        if (writechunk) {
            fputs(chunk, fp);
        } else {
            continue;
        }
    }

    fclose(fp);
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage %s <ip> <port>\n", argv[0]);
        exit(0);
    }

    const char* const ip = argv[1];
    const char* const port = argv[2];

    size_t bufsize = 5;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        printf("Failed to create socket.\n");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(atoi(port));

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Failed to connect to the host.\n");
        exit(1);
    }

    printf("Connected to the host!");

    char messageBuf[30] = "GET / HTTP/1.1\r\n\r\n";

    if (send(sockfd, messageBuf, sizeof(messageBuf), 0) < 0) {
        printf("Failed to send get request.\n");
        close(sockfd);
        exit(1);
    }

    signal(SIGINT, close_and_die);
    write_chunks(20);
    
    close(sockfd);
    return 0;


}
