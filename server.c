#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "shared_structs.h"
#include "movie_db/movie_db.h"

#define MAX_QUEUE 10

int process_request();
void read_param(char* token, char param[][100], char delim[2]);

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

    intialize_movie_db();

    printf("Aguardando conexões...\n");

    for ( ; ; ) {
        addr_size = sizeof client_addr;
        if ( (new_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size)) < 0) {
            printf("Erro ao aceitar conexão\n");
            exit(1);
        }
        printf("Cliente conectado\n");
        if ( (child_pid = fork()) == 0) { /* child process */
            close(socket_fd); /* close listening socket */
            int open = 1;
            // loop para processar multiplas requisicoes do cliente
            // requisicao especifica para encerrar a conexao
            while (open) {
                int a = process_request(new_fd);
                if (a == Close)
                    open = 0;
            }
            exit(0);
        }

        close(new_fd);
    }

    return 0;
}

int process_request(int socket_fd) {
    int operation, id;
    char buf[MAX_MSG_SIZE], *token;
    char answer[MAX_MSG_SIZE];
    char delim[2] = ",";

    // Recebe a operação a ser realizada
    recv_msg(socket_fd, buf);
    token = strtok(buf, delim);
    operation = (int) strtol(token, NULL, 10);
    
    char param[MAX_PARAM_COUNT][100];
    // Lê os parâmetros
    switch (operation)
    {
    case RegisterMovie:
        read_param(token, param, delim);

        // converts values to int
        int release_year = (int) strtol(param[2], NULL, 10);
        int genre_count = (int) strtol(param[3], NULL, 10);

        // fills array with genres names
        char genres[MAX_GENRE_COUNT][MAX_GENRE_STRING_LENGTH];
        for (int i = 0; i < genre_count; i++) {
            strcpy(genres[i], param[i + 4]);
        }

        return register_movie(param[0], param[1], release_year, genre_count, genres);

    case AddGenreToMovie:
        read_param(token, param, delim);
        int movie_id = (int) strtol(param[0], NULL, 10);
        return add_genre_to_movie(movie_id, param[1]);
    
    case ListMovies:
        list_movies(answer, MAX_MSG_SIZE);
        return send_msg(socket_fd, answer, strlen(answer) + 1);

    case ListInfoGenre:
        read_param(token, param, delim);
        get_movie_per_genre(answer, MAX_MSG_SIZE, param[0]);
        return send_msg(socket_fd, answer, strlen(answer));

    case ListAll:
        get_all_movie_data(answer, MAX_MSG_SIZE);  
        return send_msg(socket_fd, answer, strlen(answer));

    case ListFromID:
        read_param(token, param, delim);
        id = (int) strtol(param[0], NULL, 10);
        get_movie_data(id, answer, MAX_MSG_SIZE);
        return send_msg(socket_fd, answer, strlen(answer));

    case RmMovie:
        read_param(token, param, delim);
        id = (int) strtol(param[0], NULL, 10);
        return rm_movie(id);

    case Close:
        return -2;

    default:
        return -1;
    }
    //RegisterMovie = 1, AddGenreToMovie, ListMovies, ListInfoGenre, ListAll, ListID, RmMovie
}

void read_param(char* token, char param[][100], char delim[2]) {
    int i = 0;
    while( token != NULL ) {
        token = strtok(NULL, delim);
        if (token) {
            strcpy(param[i++], token);
        }
            
    }
}

