#include "movie_db/movie_db.h"

int main() {
  int status = rm_movie(1000);
  printf("%d: Not found\n", status);

  status = rm_movie(1);
  printf("%d: Removed\n", status);
  return 0;
}