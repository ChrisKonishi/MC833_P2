#include <stdio.h>

#include "shared_structs.h"

int serialize_movie(movie_struct movie, char *dst, int len) {
  char genres_buffer[len];
  int i = 0, length = 0, l_write;

  /* prepare genre line */
  while (i < movie.genre_count) {
    l_write =
        snprintf(genres_buffer + length, len - length, "%s,", movie.genres[i]);
    if (l_write <= 0 || l_write >= len - length)
      return 1;
    length += l_write;
    i ++;
  }
  if (length >= 1) { /* rm last comma */
    genres_buffer[length - 1] = '\0';
  }

  length = snprintf(dst, len, "%d\n%s\n%s\n%s\n%d\n", movie.id, movie.name,
                    genres_buffer, movie.director, movie.release_year);
  if (length <= 0 || length >= len)
    return 1;

  return 0;
}