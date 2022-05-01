#ifndef MOVIE_DB_H_
#define MOVIE_DB_H_

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILENAME_SIZE 100
#define MAX_MOVIES 200

#define DB_FOLDER "./db"

#include "../shared_structs.h"

/* Public functions */

int intialize_movie_db();
/* Call it before using other db functions, returns 0 if nothing goes wrong */

int register_movie(char *name, char *director, int release_year,
                   int genre_count, char genres[][MAX_GENRE_STRING_LENGTH]
                   ); /* add more stuff here */
/* returns -1 on error, registered movie id otherwise */

int list_movies(char *buffer, int buffer_size);
/* returns "id: title" */

int get_movie_data(int id, char *buffer, int buffer_size);

int add_genre_to_movie(int id, char *genre);

int get_all_movie_data(char *buffer, int buffer_size);

int get_movie_per_genre(char *buffer, int buffer_size, char *genre);

int rm_movie(int id);
/* 1 on error, -1 if not found, 0 if everything goes right */

int add_genre_to_movie(int id, char *genre);
/* -1 if id not found, 1 on error, 0 on success */

/* Private functions */
int _get_movie_filename_from_id(int id, char *filename);
int _get_pretty_movie_str(movie_struct movie, char *buffer, int buffer_size,
                          char *genre);
int _read_movie_str(movie_struct movie, char *buffer);
int test();
int _get_next_id();
int _list_id_name(movie_struct *id_name_list);
int _cmp_movie_id(const void *a, const void *b);
int _write_str_to_file(char *filename, char *buffer);

#include "movie_db.c"

#endif
