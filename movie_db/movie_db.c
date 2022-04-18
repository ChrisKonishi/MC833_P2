
#include "movie_db.h"
#include "../shared_structs.h"

int register_movie(char *name, int genre_count, char **genres, char *director,
                   int release_year) {
  movie_struct movie; /* temp struct to serialize */
  movie.name = name;
  movie.genres = genres;
  movie.director = director;
  movie.release_year = release_year;

  for (int i = 0; i < genre_count; i++) {
    strcpy(movie.genres[i], genres[i]);
  }
  movie.genre_count = genre_count;
  movie.id = _get_next_id();

  char buffer[MAX_SERIALIZED_SIZE];
  if (serialize_movie(movie, buffer, 1000)) {
    fprintf(stderr, "Error on serializing movie\n");
    return -1;
  }

  char filename[MAX_FILENAME_SIZE];
  snprintf(filename, MAX_FILENAME_SIZE, "%s/%d_%s", DB_FOLDER, movie.id,
           movie.name);

  /* write to file */
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Couldn't create file %s\n", filename);
    return -1;
  }
  fprintf(file, "%s", buffer);
  fclose(file);
}

int _get_next_id() { /* return an unused id */
  movie_struct movie_list[MAX_MOVIES];
  int movie_count = _list_id_name(movie_list);
  int max_id = -1;
  for (int i = 0; i < movie_count; i++) {
    max_id = (movie_list[i].id > max_id) ? movie_list[i].id : max_id;
  }
  return max_id + 1;
}

int _list_id_name(movie_struct *id_name_list) {
  /* Fill id and name fields on movie struct with data of all movies,
     return number of movies */
  char buffer[MAX_FILENAME_SIZE];

  struct dirent *de;
  DIR *dir = opendir(DB_FOLDER);
  if (dir == NULL) {
    fprintf(stderr, "Could not open directory");
    return 0;
  }

  /* http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html */
  char *token;
  char sep[2] = "_";
  int i = 0;
  while ((de = readdir(dir)) != NULL) {
    token = strtok(de->d_name, sep);
    id_name_list[i].id = atoi(token);
    token = strtok(NULL, sep);
    strcpy(id_name_list[i].name, token);
    i++;
  }
  closedir(dir);
  return i;
}

int test() {
  movie_struct m;
  m.id = 6;
  printf("%d\n", m.id);
  return 1;
}