#include <stdio.h>

#include "movie_db.h"
#include "../shared_structs.h"

int test() {
    movie_struct m;
    m.id = 6;
    printf("%d\n", m.id);
    return 1;
}