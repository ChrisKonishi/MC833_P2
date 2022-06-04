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
int expects_answer(int op);
void run_test_op(int socket_fd);


int connect_to_server(char *ip_address) {

    int status;
    int socket_fd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP stream sockets

    // OBS: o primeiro parâmetro é o endereço IP do servidor
    if (( status = getaddrinfo(ip_address, PORT_NUMBER, &hints, &res)) != 0) {
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
    char buf[MAX_MSG_SIZE] = {'\0'};
    char answer[MAX_MSG_SIZE];
    int msg_size;

    // Adiciona o o número da operação aos parâmetros
    // Somamos 1 no tamanho da mensagem para contar a vírgula adicionada
    if (param) {
        msg_size = strlen(param) + OPERATION_SIZE + 1;
        snprintf(buf, 4, "%02d,", op);
        strcat(buf, param);
    }
    else {
        msg_size = OPERATION_SIZE + 1;
        snprintf(buf, 4, "%02d,", op);
    }

    // Envia a operação requisitada
    send_msg(socket_fd, buf, msg_size);

    // Algumas operacoes esperam uma resposta
    if (expects_answer(op)) {
        recv_msg(socket_fd, answer);
        printf("%s\n", answer);
    }
}

int expects_answer(int op) {
    if (op == ListMovies || op == ListInfoGenre || op == ListAll || op == ListFromID)
        return 1;
    return 0;
}

void terminal_interaction(int socket_fd) {
    char buf[MAX_MSG_SIZE];
    char param[100] = "";

    while (1) {
        print_operations();
        memset(param, 0, 100);
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
            printf("Digite o ID do filme\n");
            concat_param(buf, param);
            printf("Digite o genero a ser adicionado\n");
            concat_param(buf, param);
            make_request(socket_fd, AddGenreToMovie, param);
            break;

        case ListMovies:
            make_request(socket_fd, ListMovies, NULL);
            break;

        case ListInfoGenre:
            printf("Digite o genero\n");
            concat_param(buf, param);
            make_request(socket_fd, ListInfoGenre, param);
            break;

        case ListAll:
            make_request(socket_fd, ListAll, NULL);
            break;

        case ListFromID:
            printf("Digite o ID\n");
            concat_param(buf, param);
            make_request(socket_fd, ListFromID, param);
            break;
            
        case RmMovie:
            printf("Digite o ID\n");
            concat_param(buf, param);
            make_request(socket_fd, RmMovie, param);
            break;

        default:
            printf("Operação inválida\n");
            break;
        }
    }
}

// Monta a string de parametros separados por virgula para enviar ao servidor 
void concat_param(char* buf, char* param) {
    fgets(buf, 100, stdin);
    buf[strcspn(buf, "\n")] = 0;
    strcat(param, buf);
    strcat(param, ",");
}

void print_operations() {
    printf("\n");
    printf("**************************************\n");
    printf("Selecione uma operação:\n");
    printf("    %d: Registrar filme\n", RegisterMovie);
    printf("    %d: Adicionar um gênero ao filme\n", AddGenreToMovie);
    printf("    %d: Listar títulos e seus identificadores\n", ListMovies);
    printf("    %d: Listar filmes de um gênero\n", ListInfoGenre);
    printf("    %d: Listar todos os filmes\n", ListAll);
    printf("    %d: Listar informações de um filme\n", ListFromID);
    printf("    %d: Remover filme\n", RmMovie);
    printf("    %d: Sair\n", 0);
}

void run_test_op(int socket_fd) {
    struct timeval start, end;
    
    make_request(socket_fd, RegisterMovie, "Avengers: Endgame,Anthony Russo,2019,4,Fantasy, Adventure,Sci-Fi,Action");
    make_request(socket_fd, RegisterMovie, "The Irishman,Martin Scorsese,2019,2,Drama,Crime");
    make_request(socket_fd, RegisterMovie, "Inside Out,Pete Docter,2015,3,Comedy,Fantasy,Animation");
    make_request(socket_fd, RegisterMovie, "Gravity,Alfonso Cuarón, 2013,3,Drama,Mystery & Thriller,Sci-Fi");
    make_request(socket_fd, RegisterMovie, "1917,Sam Mendes,2020,3,Drama,War,History");
    make_request(socket_fd, AddGenreToMovie, "0,Superheroes");

    printf("*********************************\n");
    printf("Informações de todos os filmes:\n\n");
    gettimeofday(&start, NULL);
    make_request(socket_fd, ListAll, NULL);
    gettimeofday(&end, NULL);
    printf("Tempo de resposta: %f\n", time_diff(&start, &end));

    printf("*********************************\n");
    printf("Informações do filme de ID 2:\n\n");
    gettimeofday(&start, NULL);
    make_request(socket_fd, ListFromID, "2");
    gettimeofday(&end, NULL);
    printf("Tempo de resposta: %f\n", time_diff(&start, &end));

    printf("*********************************\n");
    printf("Informações dos filmes do gênero sci-fi:\n\n");
    gettimeofday(&start, NULL);
    make_request(socket_fd, ListInfoGenre, "Sci-Fi");
    gettimeofday(&end, NULL);
    printf("Tempo de resposta: %f\n", time_diff(&start, &end));

    make_request(socket_fd, RmMovie, "3");
    printf("*********************************\n");
    printf("Listagem dos filmes após a remoção do filme de ID 3:\n\n");
    gettimeofday(&start, NULL);
    make_request(socket_fd, ListMovies, NULL);
    gettimeofday(&end, NULL);
    printf("Tempo de resposta: %f\n", time_diff(&start, &end));

    make_request(socket_fd, RmMovie, "0");
    make_request(socket_fd, RmMovie, "1");
    make_request(socket_fd, RegisterMovie, "Alien,Ridley Scott,1979,2,Sci-Fi,Horror");
    printf("*********************************\n");
    printf("Listagem dos filmes após a remoção dos IDs 0 e 1 e adição do Alien:\n\n");
    gettimeofday(&start, NULL);
    make_request(socket_fd, ListMovies, NULL);
    gettimeofday(&end, NULL);
    printf("Tempo de resposta: %f\n", time_diff(&start, &end));

    // cleans database
    make_request(socket_fd, RmMovie, "2");
    make_request(socket_fd, RmMovie, "3");
    make_request(socket_fd, RmMovie, "4");
    make_request(socket_fd, RmMovie, "5");

    // closes connection
    make_request(socket_fd, Close, NULL);
}

int main(int argc, char **argv) {
    char *ip_address;
    char buf[10];

    if (argc == 1){
        printf("Endereço de IP não informado (./<programa> <endereço de IP>), utilizando %s\n", IP_ADDRESS);
        ip_address = IP_ADDRESS;
    }
    else if (argc == 2){
        ip_address = argv[1];
    }
    else{
        printf("Uso: ./<programa> <endereço de IP (opcional)>\nSe o endereço for omitido, utiliza-se %s\n", IP_ADDRESS);
        exit(1);
    }

    int socket_fd = connect_to_server(ip_address);

    printf("Selecione o modo de operação:\n");
    printf("1: Modo interativo\n");
    printf("2: Rodar teste\n");

    fgets(buf, 10, stdin);
    int op = (int) strtol(buf, NULL, 10);

    if (op == 1)
        terminal_interaction(socket_fd);
    else {
        run_test_op(socket_fd);
    }

    return 0;
}