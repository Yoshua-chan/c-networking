#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 2137
#define BUFSIZE 2048

int main(int argc, char** argv) {
    int sockfd; // Socket file descriptor
    int n;
    struct sockaddr_in addr;
    char sendline[BUFSIZE];
    char recvline[BUFSIZE];
    int sendbytes;

    if(argc != 2) {
        fprintf(stderr, "Invalid number of arguments\n");
        fflush(stderr);
    }
        
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Couldn't create a socket\n");
        fflush(stderr);
    }

    bzero(&addr, sizeof(addr)); // Zero the address
    addr.sin_family = AF_INET; // I don't know what it does
    addr.sin_port = htons(SERVER_PORT); // htons converts the port to standard endianness

    if( (inet_pton(AF_INET, argv[1], &addr.sin_addr)) < 0) { // Translate the ASCII IP to dedicated structure
        fprintf(stderr, "inet_pton error\n");
        fflush(stderr);
    }

    if( connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0 ) {
        fprintf(stderr, "Couldn't connect\n");
        fflush(stderr);
    }

    sprintf(sendline, "GET /say=asdasdasd&dupa=aklsdjalskda HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);

    if( (write(sockfd, sendline, sendbytes)) != sendbytes ) {
        fprintf(stderr, "Error writing to socket\n");
        fflush(stderr);
    }

    memset(recvline, 0, BUFSIZE);

    while( (n = read(sockfd, recvline, BUFSIZE - 1)) > 0) {
        printf("%s", recvline);
    }
    if(n < 0) {
        fprintf(stderr, "Error reading from socket\n");
        fflush(stderr);
    }
    
    exit(0);
}