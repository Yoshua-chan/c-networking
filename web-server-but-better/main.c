#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 2048

// TODO: Add 
// FIXME: Server sends only a part of html when website is cached in browser
// FIX: SIema

char* file_to_string(char* filename) {
    const int initial_buf_size = 1024;
    int current_buf_size = initial_buf_size;
    int position = 0;
    char* buffer = (char*)malloc(current_buf_size * sizeof(char));

    FILE* file = fopen(filename, "r");

    int c;
    while( (c = getc(file)) != EOF && position < current_buf_size) {

        /* Reallocate bigger chunk of memory and copy */
        if(position >= current_buf_size - 1) {
            current_buf_size += initial_buf_size;
            buffer = (char*)realloc(buffer, current_buf_size);
        }

        if(buffer == NULL) {
            fprintf(stderr, "Error while reallocating the buffer\n");
            exit(EXIT_FAILURE);
        }

        buffer[position++] = c;
    }

    return buffer;
}

int main(int argc, char** argv) {
    int listenfd, connfd; // Socket file descriptor
    int n;
    struct sockaddr_in serv_addr;
    char buff[BUFSIZE];
    char recvline[BUFSIZE];
    int sendbytes;

    int _port_pos = 0;
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            _port_pos = i + 1;
            break;
        }
    }

    const int server_port = atoi(argv[_port_pos]);
        
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Couldn't create a socket\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    bzero(&serv_addr, sizeof(serv_addr)); // Zero the address
    serv_addr.sin_family = AF_INET; // I don't know what it does
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen for connections from any address
    serv_addr.sin_port        = htons(server_port); // htons converts the port to standard endianness

    if(bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
        fprintf(stderr, "Couldn't bind\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    if(listen(listenfd, 10) < 0) {
        fprintf(stderr, "Couldn't listen\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    while(true) {
        struct sockaddr_in addr;
        socklen_t addr_len;
        bool if_css = false;

        char* html;
        char* css;
    
        printf("Waiting for a connection on port %d\n", server_port);
        fflush(stdout);
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);

        memset(recvline, 0, BUFSIZE);


        while( (n = read(connfd, recvline, BUFSIZE - 1)) > 0) {
            if(strstr(recvline, "GET") == recvline) {
                printf("GET Request detected\n");
            }
            if(strstr(recvline, "style.css") != NULL) {
                if_css = true;
            }
            fprintf(stdout, "\n%s\n", recvline);
            if(recvline[n-1] == '\n') {
                break;
            }
            memset(recvline, 0, BUFSIZE);
        }
        if(n < 0) {
            fprintf(stderr, "Read error\n");
        }

        html = file_to_string("index.html");
        css = file_to_string("style.css");

        char* header_ok = "HTTP/1.0 200 OK\r\n\r\n%s";
        if(if_css) { // My god, this is ugly
            snprintf((char*)buff, strlen(header_ok) + strlen(css) + 1, css);
            printf("%s", css);
        } else {
            snprintf((char*)buff, strlen(header_ok) + strlen(html) + 1, "HTTP/1.0 200 OK\r\n\r\n%s", html);
            printf("%s", html);
        }

        write(connfd, (char*)buff, strlen((char*)buff));

        free(html);
        free(css);

        close(connfd);
    }
    
    exit(0); 
}