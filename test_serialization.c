#include <string.h>

#include "shared_structs.h"

int main() {
    movie_struct a;
    strcpy(a.director, "Hayao Miyazaki");
    strcpy(a.name, "Mononoke Hime");
    a.release_year = 2000;
    a.id = 0;
    strcpy(a.genres[0], "comédia");
    strcpy(a.genres[1], "ação");
    strcpy(a.genres[2], "suspense");
    strcpy(a.genres[3], "romance");
    strcpy(a.genres[4], "ficção científica");
    a.genre_count = 5;
    char buffer[1000];
    serialize_movie(a, buffer, 1000);
    printf("%s", buffer);
    printf("\n\n");

    movie_struct b;
    deserialize_movie(buffer, &b);
    serialize_movie(b, buffer, 1000);
    printf("%s", buffer);
    return 0;
}
