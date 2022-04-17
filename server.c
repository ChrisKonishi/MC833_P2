#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "shared_structs.h"

#define MAX_QUEUE 10

void process_request();

//void attach_header(char *msg, int size);

int main(int argc, char **argv) {

    int status, child_pid;
    int socket_fd, new_fd;
    struct addrinfo hints, *res;
    struct sockaddr_storage client_addr;
    socklen_t addr_size;

    memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    if (( status = getaddrinfo(NULL, PORT_NUMBER, &hints, &res)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    if ( (socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol) ) < 0) {
        printf("Erro ao criar o socket\n");
        exit(1);
    }

    if ( bind(socket_fd, res->ai_addr, res->ai_addrlen) < 0) {
        printf("Erro ao dar bind\n");
        exit(1);
    }

    if ( listen(socket_fd, MAX_QUEUE) < 0) {
        printf("Erro ao escutar\n");
        exit(1);
    }

    printf("Aguardando conexões...\n");

    for ( ; ; ) {
        addr_size = sizeof client_addr;
        if ( (new_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size)) < 0) {
            printf("Erro ao aceitar conexão\n");
            exit(1);
        }

        if ( (child_pid = fork()) == 0) { /* child process */
            close(socket_fd); /* close listening socket */
            process_request(new_fd); /* process the request */
            exit(0);
        }

        close(new_fd);
    }

    return 0;
}

void process_request(int socket_fd) {
    int bytes_received, bytes_sent;
    uint16_t operation, temp, size = 100;
    temp = htons(size);

    // Recebe a operação a ser realizada
    if ( ( bytes_received = recv(socket_fd, &operation, sizeof(uint16_t), 0) ) == -1 ) {
        printf("Erro ao enviar o tamanho da mensagem\n");
        exit(1);
    }
    printf("Processando operação %d\n", ntohs(operation));
    // Primeiro envia o tamanho da mensagem
    if ( ( bytes_sent = send(socket_fd, &temp, sizeof(uint16_t), 0) ) == -1) {
        printf("Erro ao enviar o tamanho da mensagem\n");
        exit(1);
    }
}