#ifndef MOVIE_DB_H_
#define MOVIE_DB_H_

#include "movie_db.c"

typedef struct {
    int id;
    char name[50];
    /* Add more stuff */
} movie_struct;

char ***list_movies();
/* returns [[<id>, <name>], [<id>, <name>]] */

movie_struct *get_movie_data(movie_struct *movie);

int register_movie(char *name); /* add more stuff here */

int test();

#endif
