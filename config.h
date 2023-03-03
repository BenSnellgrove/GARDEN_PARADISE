//
// Created by Ben Snellgrove on 07/11/2022.
//

#ifndef GARDEN_PARADISE_CONFIG_H
#define GARDEN_PARADISE_CONFIG_H


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "buildinfo.h"
#include "inhabitant.h"
#include "simulation.h"

#include "global_structs.h"



CONFIG * create_empty_config(void);
CONFIG * create_config(
        const char * name,
        double lettuce_grow_prob,
        int slug_lifespan,
        int slug_mature_age,
        double slug_reproduce_age,
        int frog_hungry,
        int frog_lifespan,
        int frog_mature_age,
        double frog_reproduce_prob,
        int frog_vision_distance
);
void free_config(CONFIG * config);


simulation * read_file(const char * filename);
bool read_config_line(FILE * file, const char * expected, char * store);
bool read_inhabitant_line(FILE * file, simulation * sim);

#endif //GARDEN_PARADISE_CONFIG_H
