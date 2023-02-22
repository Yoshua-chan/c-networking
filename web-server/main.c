#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 2137
#define BUFSIZE 2048

int main(int argc, char** argv) {
    int listenfd, connfd; // Socket file descriptor
    int n;
    struct sockaddr_in serv_addr;
    char buff[BUFSIZE];
    char recvline[BUFSIZE];
    int sendbytes;
        
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Couldn't create a socket\n");
        fflush(stderr);
    }

    bzero(&serv_addr, sizeof(serv_addr)); // Zero the address
    serv_addr.sin_family = AF_INET; // I don't know what it does
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen for connections from any address
    serv_addr.sin_port        = htons(SERVER_PORT); // htons converts the port to standard endianness

    if(bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
        fprintf(stderr, "Couldn't bind\n");
        fflush(stderr);
    }

    if(listen(listenfd, 10) < 0) {
        fprintf(stderr, "Couldn't listen\n");
        fflush(stderr);
    }

    while(true) {
        struct sockaddr_in addr;
        socklen_t addr_len;

        char* html = "<head><title>Tytul stronki</title></head>\
        <body><h1>Witaj na mojej stronie</h1>Test HTML\
        </body>";
    
        printf("Waiting for a connection on port %d\n", SERVER_PORT);
        fflush(stdout);
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);

        memset(recvline, 0, BUFSIZE);

        while( (n = read(connfd, recvline, BUFSIZE - 1)) > 0) {
            fprintf(stdout, "\n%s\n", recvline);
            if(recvline[n-1] == '\n') {
                break;
            }
            memset(recvline, 0, BUFSIZE);
        }
        if(n < 0) {
            fprintf(stderr, "Read error\n");
        }

        snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\n%s", html);

        write(connfd, (char*)buff, strlen((char*)buff));
        close(connfd);
        
    }
    
    exit(0);
}