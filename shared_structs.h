#ifndef SHARED_STRUCTS_H_
#define SHARED_STRUCTS_H_

#define PORT_NUMBER "3490"
#define MAXDATASIZE 100

#define MAX_SERIALIZED_SIZE 10000

typedef struct {
  int id;
  char name[50];
  char genres[50][20];
  int genre_count;
  char director[50];
  int release_year;
  /* char img[50]; */
} movie_struct;

int serialize_movie(movie_struct movie, char *dst, int len);
/* serializes a movie struct to a JSON string (dst), returns 0 on success, 1
   on error, such as insufficient string size */

int deserialize_movie(char *movie_data, movie_struct *dst);

#include "shared_structs.c"

#endif