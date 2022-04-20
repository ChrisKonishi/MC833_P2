#include <stdio.h>
#include <string.h>

#include "movie_db/movie_db.h"
#include "shared_structs.h"

int main() {
  if (intialize_movie_db()) {
    exit(1);
  }

  char genres[3][MAX_GENRE_STRING_LENGTH] = {"Romance", "Body Swap", "Meteor"};
  register_movie("Kimi no Na wa", 3, genres, "Makoto Shinkai", 2017);

  strcpy(genres[0], "Bullying");
  strcpy(genres[0], "Romance");
  register_movie("Koe no Katachi", 2, genres, "Naoko Yamada", 2016);

  return 0;
}