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

int process_request(int socket_f);
void read_param(char* token, char param[][100], char delim[2]);

int main(int argc, char **argv) {

    int status, child_pid;
    int socket_fd, new_fd;
    struct addrinfo hints, *res, *p;
    socklen_t addr_size;

    memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; //UDP datagram sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    if (( status = getaddrinfo(NULL, PORT_NUMBER, &hints, &res)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    for(p = res; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            continue;
        }

        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 1;
    }
    freeaddrinfo(res);

    intialize_movie_db();

    printf("Aguardando mensagens...\n");

    for ( ; ; ) {
        int a = process_request(socket_fd);
    }
    close(socket_fd);
    return 0;
}

int process_request(int socket_fd) {
    int operation, id;
    char buf[MAX_MSG_SIZE], *token;
    char answer[MAX_MSG_SIZE];
    char delim[2] = ",";
    struct timeval start, end;
    struct sockaddr_storage client_addr;
    struct sockaddr_in *addr_sender;

    // Recebe a operação a ser realizada
    recv_msg(socket_fd, buf, &client_addr);
    addr_sender = (struct sockaddr_in *) &client_addr;

    gettimeofday(&start, NULL);
    
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
        gettimeofday(&end, NULL);
        printf("Tempo de processamento: %f\n", time_diff(&start, &end));
        return send_msg_response(socket_fd, addr_sender, answer, strlen(answer) + 1);

    case ListInfoGenre:
        read_param(token, param, delim);
        get_movie_per_genre(answer, MAX_MSG_SIZE, param[0]);
        gettimeofday(&end, NULL);
        printf("Tempo de processamento: %f\n", time_diff(&start, &end));
        return send_msg_response(socket_fd, addr_sender, answer, strlen(answer));

    case ListAll:
        get_all_movie_data(answer, MAX_MSG_SIZE);
        gettimeofday(&end, NULL);
        printf("Tempo de processamento: %f\n", time_diff(&start, &end));
        return send_msg_response(socket_fd, addr_sender, answer, strlen(answer));

    case ListFromID:
        read_param(token, param, delim);
        id = (int) strtol(param[0], NULL, 10);
        get_movie_data(id, answer, MAX_MSG_SIZE);
        gettimeofday(&end, NULL);
        printf("Tempo de processamento: %f\n", time_diff(&start, &end));
        return send_msg_response(socket_fd, addr_sender, answer, strlen(answer));

    case RmMovie:
        read_param(token, param, delim);
        id = (int) strtol(param[0], NULL, 10);
        return rm_movie(id);

    case Close:
        return -2;

    default:
        return -1;
    }

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

