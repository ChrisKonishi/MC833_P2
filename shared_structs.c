#include "shared_structs.h"

#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

typedef struct {
  int operation_id;
  char param[1000];
} operation;

int serialize_movie(movie_struct movie, char *dst, int len) {
  char genres_buffer[len];
  int i = 0, length = 0, l_write;

  /* prepare genre line */
  while (i < movie.genre_count) {
    l_write =
        snprintf(genres_buffer + length, len - length, "%s,", movie.genres[i]);
    if (l_write <= 0 || l_write >= len - length) return 1;
    length += l_write;
    i++;
  }
  if (length >= 1) { /* rm last comma */
    genres_buffer[length - 1] = '\0';
  }

  length = snprintf(dst, len, "%d\n%s\n%s\n%s\n%d\n", movie.id, movie.name,
                    genres_buffer, movie.director, movie.release_year);
  if (length <= 0 || length >= len) return 1;

  return 0;
}

int deserialize_movie(char *movie_data, movie_struct *dst) {
  char main_sep[2] = "\n";
  char genres_buffer[MAX_SERIALIZED_SIZE];

  char *token = strtok(movie_data, main_sep);
  dst->id = atoi(token);
  token = strtok(NULL, main_sep);
  strcpy(dst->name, token);
  token = strtok(NULL, main_sep);
  strcpy(genres_buffer, token);
  token = strtok(NULL, main_sep);
  strcpy(dst->director, token);
  token = strtok(NULL, main_sep);
  dst->release_year = atoi(token);

  /* genres */
  int i = 0;
  dst->genre_count = 0;
  token = strtok(genres_buffer, ",");
  while (token != NULL) {
    strcpy(dst->genres[i], token);
    dst->genre_count++;
    token = strtok(NULL, ",");
    i++;
  }

  return 0;
}

int send_msg(int socket_fd, struct addrinfo *p, char *msg, int msg_size) {
  int sent = sendto(socket_fd, msg, msg_size, 0, p->ai_addr, p->ai_addrlen);
  if (sent == -1) return -1;
  return 0;
}

int send_msg_response(int socket_fd, struct sockaddr_in *p, char *msg, int msg_size) {
  int sent = sendto(socket_fd, msg, msg_size, 0, (struct sockaddr *) p, sizeof(*p));
  if (sent == -1) return -1;
  return 0;
}

int recv_msg(int socket_fd, char *buf, struct sockaddr_storage *client_addr) {
  socklen_t addr_len = sizeof(*client_addr);
  int received = recvfrom(socket_fd, buf, MAX_MSG_SIZE, 0,
                      (struct sockaddr *)client_addr, &addr_len);
  if (received == -1) {
    fprintf(stderr, "Error receiving message\n");
    return -1;
  }
  return 0;
}

int recv_msg_non_blocking(int socket_fd, char *buf,
                          struct sockaddr_storage *client_addr) {
  struct pollfd pfds[1];

  pfds[0].fd = socket_fd;
  pfds[0].events = POLLIN;

  int num_events = poll(pfds, 1, TIMEOUT);

  if (num_events == 0) {
    printf("Timeout\n");
    return 1;
  } else {
    if (pfds[0].revents & POLLIN) {
      return recv_msg(socket_fd, buf, client_addr);
    } else {
      fprintf(stderr, "Unexpected event occurred: %d\n", pfds[0].revents);
      return 1;
    }
  }
}

float time_diff(struct timeval *start, struct timeval *end) {
  return (end->tv_sec - start->tv_sec) + 1e-6 * (end->tv_usec - start->tv_usec);
}
