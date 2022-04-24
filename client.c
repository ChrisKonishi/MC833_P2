#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "shared_structs.h"

void make_request(int socket_fd, int operation, char *param);

int main(int argc, char **argv) {

    int status;
    int socket_fd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints)); // make sure the struct isty
    hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockets

    // OBS: o primeiro parâmetro é o endereço IP do servidor
    if (( status = getaddrinfo("192.168.1.120", PORT_NUMBER, &hints, &res)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    if ( (socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol) ) < 0) {
        printf("Erro ao criar o socket\n");
        exit(1);
    }

    if (connect(socket_fd, res->ai_addr, res->ai_addrlen) < 0) {
        printf("Não foi possível conectar ao servidor\n");
        exit(1);
    }

    printf("Conectado\n");

    make_request(socket_fd, 1, "teste,picanha");

    freeaddrinfo(res);

    return 0;
}

void make_request(int socket_fd, int operation, char *param) {
    char buf[1002];
    int msg_size;

    // Adiciona o tamanho da mensagem e o número da operação no início, nessa ordem
    msg_size = strlen(param) + OPERATION_SIZE + MAX_MSG_SIZE_DIGITS;
    snprintf(buf, 8, "%04d%02d,", msg_size, operation);
    strcat(buf, param);

    // Envia a operação requisitada
    send_msg(socket_fd, buf, msg_size);
}
