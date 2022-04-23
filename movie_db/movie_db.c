
#include "movie_db.h"

#include "../shared_structs.h"

/* Public functions */

int intialize_movie_db() {
  if (mkdir(DB_FOLDER, 0777)) {
    if (errno == EEXIST)
      return 0;
    else {
      fprintf(stderr, "Error on creating DB folder: %d", errno);
    }
  }
  return 0;
}

int register_movie(char *name, int genre_count,
                   char genres[][MAX_GENRE_STRING_LENGTH], char *director,
                   int release_year) {
  movie_struct movie; /* temp struct to serialize */
  strcpy(movie.name, name);
  strcpy(movie.director, director);
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
  return movie.id;
}

int get_all_movie_data(char *buffer, int buffer_size) {
  movie_struct movie_list[MAX_MOVIES];
  int q = _list_id_name(movie_list);
  char serialized[MAX_SERIALIZED_SIZE];
  int read_status, total_length = 0;
  int genre_buffer_size = MAX_GENRE_STRING_LENGTH * (MAX_GENRE_COUNT + 2) + 1;
  char genres_buffer[genre_buffer_size];
  int j = 0, length = 0, l_write;

  for (int i = 0; i < q; i++) {
    read_status = _read_movie_str(movie_list[i], serialized);
    if (read_status)
      continue;
    deserialize_movie(serialized, &(movie_list[i]));

    j = 0;
    length = 0;
    /* prepare genre line */
    while (j < movie_list[i].genre_count) {
      l_write = snprintf(genres_buffer + length, genre_buffer_size - length,
                         "%s, ", movie_list[i].genres[j]);
      if (l_write <= 0 || l_write >= genre_buffer_size - length)
        continue;
      length += l_write;
      j++;
    }
    if (length >= 2) /* rm last comma */
      genres_buffer[length - 2] = '\0';

    l_write = snprintf(
        buffer + total_length, buffer_size - total_length,
        "ID: %d\nName: %s\nDirector: %s\nGenres: %s\nRelease year: %d\n\n",
        movie_list[i].id, movie_list[i].name, movie_list[i].director,
        genres_buffer, movie_list[i].release_year);
    if (l_write < 0 || l_write > buffer_size)
      return 1;

    total_length += l_write;
  }

  return 0;
}

/* Private functions */

int _read_movie_str(movie_struct movie, char *buffer) {
  char filename[MAX_FILENAME_SIZE];
  int error = 0;
  snprintf(filename, MAX_FILENAME_SIZE, "%s/%d_%s", DB_FOLDER, movie.id,
           movie.name);
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Couldn't open file %s\n", filename);
    return 1;
  }

  error = fseek(file, 0, SEEK_END); /* end of file */
  long fsize = ftell(file);         /* size of file */
  error = error || fsize == -1;
  error = error || fseek(file, 0, SEEK_SET); /* rewind file */
  error = error || fread(buffer, fsize, 1, file) != 1;

  fclose(file);
  if (error) {
    fprintf(stderr, "Couldn't read file %s\n", filename);
    return 1;
  }
  buffer[fsize] = '\0';
  return 0;
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
    if (de->d_type == DT_REG) {
      token = strtok(de->d_name, sep);
      id_name_list[i].id = atoi(token);
      token = strtok(NULL, sep);
      strcpy(id_name_list[i].name, token);
      i++;
    }
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