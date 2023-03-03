//
// Created by Ben Snellgrove on 07/11/2022.
//

#include "config.h"



/**
 * Create a config with all values as 0
 * @return The config
 */
CONFIG * create_empty_config(void) {
    return create_config(
            "",
            0.0F,
            0, 0, 0.0F,
            0, 0, 0, 0.0F, 0
    );
}


/**
 * Create a config with all the values given
 * @param name
 * @param lettuce_grow_prob
 * @param slug_lifespan
 * @param slug_mature_age
 * @param slug_reproduce_age
 * @param frog_hungry
 * @param frog_lifespan
 * @param frog_mature_age
 * @param frog_reproduce_prob
 * @param frog_vision_distance
 * @return The config
 */
CONFIG * create_config(const char * name,
                       const double lettuce_grow_prob,
                       const int slug_lifespan,
                       const int slug_mature_age,
                       const double slug_reproduce_age,
                       const int frog_hungry,
                       const int frog_lifespan,
                       const int frog_mature_age,
                       const double frog_reproduce_prob,
                       const int frog_vision_distance) {

    CONFIG * out = malloc(sizeof(CONFIG));

    strncpy(out->name, name, 255);
    out->name[255] = '\000';

    out->LETTUCE_GROW_PROB = lettuce_grow_prob;

    out->SLUG_LIFESPAN = slug_lifespan;
    out->SLUG_MATURE_AGE = slug_mature_age;
    out->SLUG_REPRODUCE_PROB = slug_reproduce_age;

    out->FROG_HUNGRY = frog_hungry;
    out->FROG_LIFESPAN = frog_lifespan;
    out->FROG_MATURE_AGE = frog_mature_age;
    out->FROG_REPRODUCE_PROB = frog_reproduce_prob;
    out->FROG_VISION_DISTANCE = frog_vision_distance;

    return out;

}


/**
 * Free a config
 * @param config
 */
void free_config(CONFIG * config) {

    // Config is free
    if (config == NULL) return;
#ifdef DEBUG
    printf("Freed a config\n");
#endif
    free(config);

}


/**
 * Create a new sim from a config file
 * @param filename The file name
 * @return pointer to the new simulation, NULL if the config file was invalid
 */
simulation * read_file(const char * filename) {

    FILE * configfile;
    configfile = fopen(filename, "r");

    CONFIG * cfg = create_empty_config();

    char temp[256];

    // Lettuce grow
    if (!read_config_line(configfile, "LETTUCE_GROW_PROB", temp)) return NULL;
    cfg->LETTUCE_GROW_PROB = strtof(temp, NULL);

    // Slug breed
    if (!read_config_line(configfile, "SLUG_REPRODUCE_PROB", temp)) return NULL;
    cfg->SLUG_REPRODUCE_PROB = strtof(temp, NULL);

    // Frog breed
    if (!read_config_line(configfile, "FROG_REPRODUCE_PROB", temp)) return NULL;
    cfg->FROG_REPRODUCE_PROB = strtof(temp, NULL);

    // Slug mature
    if (!read_config_line(configfile, "SLUG_MATURE_AGE", temp)) return NULL;
    cfg->SLUG_MATURE_AGE = (int) strtol(temp, NULL, 10);

    // Frog mature
    if (!read_config_line(configfile, "FROG_MATURE_AGE", temp)) return NULL;
    cfg->FROG_MATURE_AGE = (int) strtol(temp, NULL, 10);

    // Slug lifespan
    if (!read_config_line(configfile, "SLUG_LIFESPAN", temp)) return NULL;
    cfg->SLUG_LIFESPAN = (int) strtol(temp, NULL, 10);

    // Frog Lifespan
    if (!read_config_line(configfile, "FROG_LIFESPAN", temp)) return NULL;
    cfg->FROG_LIFESPAN = (int) strtol(temp, NULL, 10);

    // Frog hungry
    if (!read_config_line(configfile, "FROG_HUNGRY", temp)) return NULL;
    cfg->FROG_HUNGRY = (int) strtol(temp, NULL, 10);

    // Frog vision
    if (!read_config_line(configfile, "FROG_VISION_DISTANCE", temp)) return NULL;
    cfg->FROG_VISION_DISTANCE = (int) strtol(temp, NULL, 10);


    strncpy(cfg->name, filename, 255);
    cfg->name[255] = '\000';

    simulation * out = create_simulation(12, 12);
    free_config(out->config);
    out->config = cfg;

    // Find all inhabitants in file
    for(int i = 0; i < 10; i++) {
        if (!read_inhabitant_line(configfile, out)) break;
#ifdef DEBUG
        printf("found an inhabitant!\n");
#endif
    }

    // Close file and return
    fclose(configfile);
    return out;
}


/**
 * Read a single line from a config file
 * @param file The file
 * @param expected Expected text before a given value
 * @param store location of the output
 * @return True if the config line is valid, otherwise false
 */
bool read_config_line(FILE * file, const char * expected, char * store) {

    fscanf(file, " %255[_A-Za-z]", store);
    if (strcmp(store, expected)) return false;
    fscanf(file, " %255[-.0-9]", store);
    return true;

}


/**
 * Read an inhabitant line from a config file
 * @param file The file
 * @param sim The simulation to put the inhabitant in
 * @return Success, false if end of file has been reached
 */
bool read_inhabitant_line(FILE * file, simulation * sim) {

    char temp[16];
    int coord[2];
    INHABITANT_TYPE type;
    int init_age;
    int hunger;
    DIRECTION next_move;
    inhabitant * i;

    // Coordinate reading
    // Check for End Of File
    if (fscanf(file, " (%15[-.0-9],", temp) == EOF) return false;
    coord[0] = (int) strtol(temp, NULL, 10);
    fscanf(file, " %15[-.0-9])", temp);
    coord[1] = (int) strtol(temp, NULL, 10);

    // Type
    fscanf(file, " %15[A-Z]", temp);
    if (!strcmp(temp, "LETTUCE")) type = LETTUCE;
    if (!strcmp(temp, "SLUG")) type = SLUG;
    if (!strcmp(temp, "FROG")) type = FROG;

    // Age
    fscanf(file, " %15[0-9]", temp);
    init_age = (int) strtol(temp, NULL, 10);

    // Hungry
    fscanf(file, " %15[0-9]", temp);
    hunger = (int) strtol(temp, NULL, 10);

    // First move
    fscanf(file, " %15[A-Z]", temp);
    if (!strcmp(temp, "STATIONARY")) next_move = STATIONARY;
    if (!strcmp(temp, "NORTH")) next_move = NORTH;
    if (!strcmp(temp, "EAST")) next_move = EAST;
    if (!strcmp(temp, "SOUTH")) next_move = SOUTH;
    if (!strcmp(temp, "WEST")) next_move = WEST;

    // Create inhabitant
    i = create_inhabitant(type, init_age, next_move);
    i->hunger = hunger;

    // Prevent memory leak
    free_inhabitant(sim_get(sim, coord));
    sim_set(sim, coord, i);

    return true;
}

