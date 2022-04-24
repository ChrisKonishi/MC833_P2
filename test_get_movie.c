#include <stdio.h>

#include "movie_db/movie_db.h"
int main() {
  char buffer[100];
	int id = 0;
  get_movie_data(id, buffer, 100);
	printf("%s", buffer);
}