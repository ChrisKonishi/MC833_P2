#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "shared_structs.h"

void make_request(int socket_fd, int op, char *param);
void print_operations();
void terminal_interaction();
void concat_param(char* buf, char* param);

int connect_to_server() {

    int status;
    int socket_fd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockets

    // OBS: o primeiro parâmetro é o endereço IP do servidor
    if (( status = getaddrinfo("127.0.1.1", PORT_NUMBER, &hints, &res)) != 0) {
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

    freeaddrinfo(res);

    return socket_fd;
}

void make_request(int socket_fd, int op, char *param) {
    char buf[1000] = {'\0'};
    int msg_size;

    // Adiciona o tamanho da mensagem e o número da operação no início, nessa ordem
    // Somamos 2 no tamanho da mensagem para contar a vírgula adicionada e o \0
    msg_size = strlen(param) + OPERATION_SIZE + MAX_MSG_SIZE_DIGITS + 2;
    snprintf(buf, 8, "%04d%02d,", msg_size, op);
    strcat(buf, param);

    // Envia a operação requisitada
    send_msg(socket_fd, buf, msg_size);

    if (expects_answer(op)) {
        recv_msg(socket_fd, buf);
        printf("%s", buf);
    }
}

int expects_answer(int op) {
    if (op == ListMovies || op == ListInfoGenre || op == ListAll || op == ListFromID)
        return 1;
    return 0;
}

void terminal_interaction(int socket_fd) {
    char buf[100];
    char param[100] = "";
    while (1) {
        print_operations();
        fgets(buf, 100, stdin);
        int op = (int) strtol(buf, NULL, 10);
        switch (op)
        {
        case 0:
            return;
        case RegisterMovie:
            printf("Digite o nome do filme\n");
            concat_param(buf, param);
            printf("Digite o nome do diretor\n");
            concat_param(buf, param);
            printf("Digite o ano de lancamento\n");
            concat_param(buf, param);
            printf("Digite o numero de generos\n");
            concat_param(buf, param);
            printf("Digite os generos\n");
            concat_param(buf, param);
            make_request(socket_fd, RegisterMovie, param);
            break;

        case AddGenreToMovie:


        default:
            printf("Operação inválida\n");
        }
    }
}
//enum operations {RegisterMovie = 1, AddGenreToMovie, ListMovies, ListInfoGenre, ListAll, ListFromID, RmMovie, Close = -2};
void concat_param(char* buf, char* param) {
    fgets(buf, 100, stdin);
    buf[strcspn(buf, "\n")] = 0;
    strcat(param, buf);
    strcat(param, ",");
}

void print_operations() {
    printf("\n");
    printf("Selecione uma operação:\n");
    printf("    %d: Register movie\n", RegisterMovie);
    printf("    %d: Add genre to movie\n", AddGenreToMovie);
    printf("    %d: List movie titles\n", ListMovies);
    printf("    %d: List movies info from genre\n", ListInfoGenre);
    printf("    %d: List all info\n", ListAll);
    printf("    %d: Get movie info\n", ListFromID);
    printf("    %d: Remove movie\n", RmMovie);
    printf("    %d: Exit\n", 0);
}

int main() {
    int socket_fd = connect_to_server();
    int interactive_mode = 0;
    char buf[MAX_MSG_SIZE];

    if (interactive_mode)
        terminal_interaction(socket_fd);
    else {
        make_request(socket_fd, RegisterMovie, "nomee,dirr,1993,3,acao,terror,sobrenatural");
        // make_request(socket_fd, RegisterMovie, "nomee2,dirr2,2311,2,acao,terror");
        // make_request(socket_fd, AddGenreToMovie, "0,old");
        make_request(socket_fd, ListAll, "");
        make_request(socket_fd, ListFromID, "1");
        make_request(socket_fd, RmMovie, "3");
        make_request(socket_fd, RmMovie, "4");
        make_request(socket_fd, Close, "");
    }

    

    return 0;
}