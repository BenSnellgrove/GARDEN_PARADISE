//
// Created by Ben Snellgrove on 14/12/2022.
//

#ifndef GARDEN_PARADISE_INHABITANT_H
#define GARDEN_PARADISE_INHABITANT_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "buildinfo.h"
#include "simulation.h"
#include "utils.h"

#include "global_enums.h"
#include "global_structs.h"




// Creation
inhabitant * create_inhabitant(INHABITANT_TYPE i_type, int initial_age, DIRECTION lastMove);
void free_inhabitant(inhabitant * i);

// Action 'controller'
void inhabitant_action(simulation * sim, const int coord[2]);

// Actions
bool eat(simulation * sim, const int hungry[2]);
bool breed(simulation * sim, const int parent[2]);
bool move_f(simulation * sim, const int frog[2]);
bool move_s(simulation * sim, const int slug[2]);
void die(simulation * sim, const int coord[2]);



#endif //GARDEN_PARADISE_INHABITANT_H
