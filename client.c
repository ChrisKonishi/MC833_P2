#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include "shared_structs.h"

void make_request(int socket_fd);

int main(int argc, char **argv) {

    int status;
    int socket_fd, numbytes;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints)); // make sure the struct isty
    hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockets

    // OBS: o primeiro parâmetro é o endereço IP do servidor
    if (( status = getaddrinfo("192.168.0.210", PORT_NUMBER, &hints, &res)) != 0) {
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

    make_request(socket_fd);

    freeaddrinfo(res);

    return 0;
}

void make_request(int socket_fd) {
    char buf[MAXDATASIZE];
    int numbytes;

    if ( (numbytes = recv(socket_fd, buf, sizeof(buf), 0)) == -1) {
        perror("recv");
        exit(1);
    }

    printf("Numbytes: %d\n", numbytes);
    buf[numbytes] = '\0';

    printf("client: received '%s'\n", buf);
}
