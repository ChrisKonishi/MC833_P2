#include <stdio.h>

#include "movie_db/movie_db.h"

int main() {
  int ret = add_genre_to_movie(0, "Suspense");
  /* Check db file */
  printf("%d\n", ret);
  return 0;
}