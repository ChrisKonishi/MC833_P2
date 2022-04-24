#include <stdio.h>

#include "movie_db/movie_db.h"

int main() {
  char all_data[1000];
  get_all_movie_data(all_data, 1000);
  printf("%s", all_data);
}