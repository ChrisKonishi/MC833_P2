#include <stdio.h>

#include "movie_db/movie_db.h"

int main() {
  char buffer[1000];
  int ret = get_movie_per_genre(buffer, 1000, "meteor");
  printf("%s\n%d", buffer, ret);
  return 0;
}
