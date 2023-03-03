//
// Created by Ben Snellgrove on 14/12/2022.
//

#ifndef GARDEN_PARADISE_UTILS_H
#define GARDEN_PARADISE_UTILS_H



#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "buildinfo.h"

#include "global_enums.h"


// Coord_ll struct
typedef struct COORD_LL {

    int values[2];
    bool ignore;

    struct COORD_LL * next;

} coord_ll;

// Coord_ll functions
coord_ll * coord_ll_init(void);
coord_ll * coord_ll_make(const int values[2]);
coord_ll * coord_ll_get(coord_ll * head, int index);
void coord_ll_push(coord_ll * head, const int values[2]);
bool coord_ll_remove(coord_ll * head, int index);
void coord_ll_free(coord_ll * head);
int coord_ll_size(coord_ll * head);



// Random functions
int dice_roll(int sides);
bool event_roll(double probability);
int pick_random_bit(int options);


// Misc
void clear_output(void);

void change_pos(int start_pos[2], DIRECTION dir);

bool file_exists(const char * filename);


#endif //GARDEN_PARADISE_UTILS_H
