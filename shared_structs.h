#ifndef SHARED_STRUCTS_H_
#define SHARED_STRUCTS_H_

#define PORT_NUMBER "3290"
#define MAXDATASIZE 100

#define MAX_GENRE_STRING_LENGTH 20
#define MAX_SERIALIZED_SIZE 1000
#define MAX_GENRE_COUNT 10
#define OPERATION_SIZE 2
#define MAX_MSG_SIZE 1500
#define MAX_MSG_SIZE_DIGITS 4
#define MAX_PARAM_COUNT MAX_GENRE_STRING_LENGTH + 4

typedef struct {
  int id;
  char name[50];
  char genres[MAX_GENRE_COUNT][MAX_GENRE_STRING_LENGTH];
  int genre_count;
  char director[50];
  int release_year;
  /* char img[50]; */
} movie_struct;

enum operations {RegisterMovie = 1, AddGenreToMovie, ListMovies, ListInfoGenre, ListAll, ListFromID, RmMovie, Close = -2};

int serialize_movie(movie_struct movie, char *dst, int len);
/* serializes a movie struct to a JSON string (dst), returns 0 on success, 1
   on error, such as insufficient string size */

int deserialize_movie(char *movie_data, movie_struct *dst);

#include "shared_structs.c"

#endif