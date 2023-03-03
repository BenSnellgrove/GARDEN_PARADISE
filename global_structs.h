//
// Created by Ben Snellgrove on 03/01/23.
//

#ifndef GARDEN_PARADISE_GLOBAL_STRUCTS_H
#define GARDEN_PARADISE_GLOBAL_STRUCTS_H


#include <stdbool.h>

#include "global_enums.h"



typedef struct {

    char name[256];

    double LETTUCE_GROW_PROB;

    int SLUG_LIFESPAN;
    int SLUG_MATURE_AGE;
    double SLUG_REPRODUCE_PROB;

    int FROG_HUNGRY;
    int FROG_LIFESPAN;
    int FROG_MATURE_AGE;
    double FROG_REPRODUCE_PROB;
    int FROG_VISION_DISTANCE;

} CONFIG;



typedef struct {

    INHABITANT_TYPE inhabitant_type;

    DIRECTION next_move;

    int age;
    int hunger;

    bool actioned_this_round;

} inhabitant;





typedef struct {

    int x;
    int y;
    inhabitant *** garden;

    int round;

    CONFIG * config;

    char ** last_action_message;

} simulation;



#endif //GARDEN_PARADISE_GLOBAL_STRUCTS_H
