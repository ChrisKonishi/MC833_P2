#ifndef MOVIE_DB_H_
#define MOVIE_DB_H_

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILENAME_SIZE 40
#define MAX_MOVIES 200

#define DB_FOLDER "./db"
mkdir(DB_FOLDER, 0777);

#include "../shared_structs.h"

int register_movie(char *name, int genre_count, char **genres, char *director,
                   int release_year); /* add more stuff here */
/* returns -1 on error, registered movie id otherwise */

char ***list_movies();
/* returns [[<id>, <name>], [<id>, <name>]] */

movie_struct *get_movie_data(movie_struct *movie);

int add_genre_to_movie(int id, char *genre);

movie_struct **get_all_movie_data();

int rm_movie(int id);

int test();

#include "movie_db.c"

#endif
