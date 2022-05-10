#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>

#include "shared_structs.h"

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

int send_msg(int socket_fd, char *msg, int msg_size) {
  int total, bytes_sent, bytes_left;
  char buf[MAX_MSG_SIZE] = {'\0'};

  msg_size += 7;
  snprintf(buf, 7, "%06d", (short) msg_size);
  strcat(buf, msg);
  total = 0;
  bytes_left = msg_size;

  do {
    if ( ( bytes_sent = send(socket_fd, buf + total, bytes_left, 0) ) == -1) {
      return -1;
    }
    bytes_left -= bytes_sent;
    total += bytes_sent;
  } while (bytes_left > 0);

  return 0;
}

int recv_msg(int socket_fd, char* buf) {
  int total, bytes_received, bytes_left, msg_size;
  char temp[30] = {'\0'};

  // Read msg_size
  bytes_left = MAX_MSG_SIZE_DIGITS;
  total = 0;
  do {
    if ( ( bytes_received = recv(socket_fd, temp + total, bytes_left, 0) ) == -1) {
      return -1;
    }
    bytes_left -= bytes_received;
    total += bytes_received;
  } while (bytes_left > 0);

  msg_size = (int) strtol(temp, NULL, 10);

  // Read rest of the msg
  bytes_left = msg_size - MAX_MSG_SIZE_DIGITS;
  total = 0;
  do {
    if ( ( bytes_received = recv(socket_fd, buf + total, bytes_left, 0) ) == -1) {
      return -1;
    }
    bytes_left -= bytes_received;
    total += bytes_received;
  } while (bytes_left > 0);

  return 0;
}

float time_diff(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) + 1e-6*(end->tv_usec - start->tv_usec);
}