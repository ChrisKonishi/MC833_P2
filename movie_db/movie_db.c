
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

int list_movies(char *buffer, int buffer_size) {
  movie_struct movie_list[MAX_MOVIES];
  int q = _list_id_name(movie_list);
  int length = 0, l_write;

  qsort(movie_list, q, sizeof(movie_struct), _cmp_movie_id);

  l_write =
      snprintf(buffer + length, buffer_size - length, "%s\n\n", "[ID]: [Name]");
  if (l_write <= 0 || l_write >= buffer_size - length)
    return 1;
  length += l_write;
  for (int movie_idx = 0; movie_idx < q; movie_idx++) {
    l_write = snprintf(buffer + length, buffer_size - length, "%d: %s\n",
                       movie_list[movie_idx].id, movie_list[movie_idx].name);
    if (l_write <= 0 || l_write >= buffer_size - length)
      return 1;
    length += l_write;
  }

  return 0;
}

int get_all_movie_data(char *buffer, int buffer_size) {
  movie_struct movie_list[MAX_MOVIES];
  int q = _list_id_name(movie_list);
  int l_write, offset = 0;

  qsort(movie_list, q, sizeof(movie_struct), _cmp_movie_id);

  for (int i = 0; i < q; i++) {
    l_write = _get_pretty_movie_str(movie_list[i], buffer + offset,
                                    buffer_size - offset, NULL);
    if (l_write == -1) {
      return 1;
    }
    offset += l_write;
  }

  return 0;
}

int get_movie_data(int id, char *buffer, int buffer_size) {
  movie_struct movie_list[MAX_MOVIES];
  int q = _list_id_name(movie_list), found = 0;
  int movie_idx = 0;
  for (movie_idx = 0; movie_idx < q; movie_idx++) {
    if (movie_list[movie_idx].id == id) {
      found = 1;
      break;
    }
  }

  if (!found) {
    snprintf(buffer, buffer_size, "%s", "Movie not found\n");
  }

  movie_struct *movie = &movie_list[movie_idx];
  if (_get_pretty_movie_str(*movie, buffer, buffer_size, NULL) == -1) {
    return 1;
  }
  return 0;
}

int get_movie_per_genre(char *buffer, int buffer_size, char *genre) {
  movie_struct movie_list[MAX_MOVIES];
  int q = _list_id_name(movie_list);
  int l_write, offset = 0;

  qsort(movie_list, q, sizeof(movie_struct), _cmp_movie_id);

  for (int i = 0; i < q; i++) {
    l_write = _get_pretty_movie_str(movie_list[i], buffer + offset,
                                    buffer_size - offset, genre);
    if (l_write == -1) {
      return 1;
    }
    offset += l_write;
  }

  return 0;
}

/* Private functions */

int _get_pretty_movie_str(movie_struct movie, char *buffer, int buffer_size,
                          char *genre) {
  /* genre = NULL : list all */
  char raw_movie_str[1000];
  int genre_buffer_size = MAX_GENRE_STRING_LENGTH * (MAX_GENRE_COUNT + 2) + 1;
  char genres_buffer[genre_buffer_size];
  int read_status = _read_movie_str(movie, raw_movie_str);
  if (read_status)
    return -1;
  deserialize_movie(raw_movie_str, &movie);

  /* Filter genre if necessary */
  if (genre != NULL) {
    int has_genre = 0;
    for (int genre_idx = 0; genre_idx < movie.genre_count; genre_idx++) {
      if (strcasecmp(movie.genres[genre_idx], genre) == 0) {
        has_genre = 1;
        break;
      }
    }
    if (!has_genre) {
      strcpy(buffer, "");
      return 0;
    }
  }

  int j = 0, length = 0, l_write;
  /* prepare genre line */
  while (j < movie.genre_count) {
    l_write = snprintf(genres_buffer + length, genre_buffer_size - length,
                       "%s, ", movie.genres[j]);
    if (l_write <= 0 || l_write >= genre_buffer_size - length)
      return -1;
    length += l_write;
    j++;
  }
  if (length >= 2) /* rm last comma */
    genres_buffer[length - 2] = '\0';

  l_write = snprintf(
      buffer, buffer_size,
      "ID: %d\nName: %s\nDirector: %s\nGenres: %s\nRelease year: %d\n\n",
      movie.id, movie.name, movie.director, genres_buffer, movie.release_year);
  if (l_write < 0 || l_write > buffer_size)
    return -1;
  return l_write;
}

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

int _cmp_movie_id(const void *a, const void *b) {
  movie_struct *ma = (movie_struct *)a;
  movie_struct *mb = (movie_struct *)b;

  return (ma->id - mb->id);
}

int test() {
  movie_struct m;
  m.id = 6;
  printf("%d\n", m.id);
  return 1;
}