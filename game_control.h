//
// Created by Ben Snellgrove on 03/01/23.
//

#ifndef GARDEN_PARADISE_GAME_CONTROL_H
#define GARDEN_PARADISE_GAME_CONTROL_H



#include "inhabitant.h"
#include "simulation.h"
#include "utils.h"

#include "buildinfo.h"
#include "global_enums.h"
#include "global_structs.h"


void garden_round(simulation * sim);
void reset_actions(simulation * sim);


void terminal_header(void);
void print_version(void);

bool quit_sim_loop(void);



#endif //GARDEN_PARADISE_GAME_CONTROL_H
