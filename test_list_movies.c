#include <stdio.h>

#include "movie_db/movie_db.h"
int main() {
  char buffer[1000];
  list_movies(buffer, 1000);
  printf("%s", buffer);
}