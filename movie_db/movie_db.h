#ifndef MOVIE_DB_H_
#define MOVIE_DB_H_

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILENAME_SIZE 100
#define MAX_MOVIES 200

#define DB_FOLDER "./db"

#include "../shared_structs.h"

int intialize_movie_db();
/* Call it before using other db functions, returns 0 if nothing goes wrong */

int register_movie(char *name, int genre_count,
                   char genres[][MAX_GENRE_STRING_LENGTH], char *director,
                   int release_year); /* add more stuff here */
/* returns -1 on error, registered movie id otherwise */

char ***list_movies();
/* returns [[<id>, <name>], [<id>, <name>]] */

movie_struct *get_movie_data(movie_struct *movie);

int add_genre_to_movie(int id, char *genre);

movie_struct **get_all_movie_data();

int rm_movie(int id);

/* Not public functions */

int test();
int _get_next_id();
int _list_id_name(movie_struct *id_name_list);

#include "movie_db.c"

#endif
