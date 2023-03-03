//
// Created by Ben Snellgrove on 14/12/2022.
//



#ifndef GARDEN_PARADISE_SIMULATION_H
#define GARDEN_PARADISE_SIMULATION_H


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "buildinfo.h"
#include "config.h"
#include "utils.h"

#include "global_enums.h"
#include "global_structs.h"


// Creation
simulation * create_simulation(int x, int y);
bool free_simulation(simulation * sim);

// Check functions
bool in_bounds(simulation * sim, const int coordinate[2]);
bool is_null(simulation * sim, const int coordinate[2]);

// Encapsulated get and set
inhabitant * sim_get(simulation * sim, const int coordinate[2]);
bool sim_set(simulation * sim, const int coordinate[2], inhabitant * i);

// coord_ll functions requiring sim
coord_ll * find_all(simulation * sim, INHABITANT_TYPE type, const int pos[2], int radius);
coord_ll * find_adjacent(simulation * sim, INHABITANT_TYPE type, const int pos[2]);

// Printing functions
void pretty_print(simulation * sim);

void print_last_action_message(simulation * sim);
void update_action_message(simulation * sim,
                           inhabitant * i1, const int coord[2], ACTION action,
                           inhabitant * i2, const int target[2]);


#endif //GARDEN_PARADISE_SIMULATION_H
