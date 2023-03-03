//
// Created by Ben Snellgrove on 03/01/23.
//

#ifndef GARDEN_PARADISE_GLOBAL_ENUMS_H
#define GARDEN_PARADISE_GLOBAL_ENUMS_H



typedef enum {
    STATIONARY = 0,
    NORTH = 1,
    EAST = 2,
    SOUTH = 4,
    WEST = 8
} DIRECTION;


// ORDER HERE IS IMPORTANT
// Higher up in the enum indicates higher rank in the food chain
typedef enum {
    EMPTY = -1,
    FROG = 0,
    SLUG = 1,
    LETTUCE = 2
} INHABITANT_TYPE;


typedef enum {
    NOTHING,
    DIED,
    EAT,
    REPRODUCE,
    MOVE,
    TIRED
} ACTION;


#endif //GARDEN_PARADISE_GLOBAL_ENUMS_H
