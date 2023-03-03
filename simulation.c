//
// Created by Ben Snellgrove on 14/12/2022.
//


#include "simulation.h"



/**
 * Create a simulation struct and return a pointer too it
 * Simulations are stored on the heap, so various functions are used to hide malloc/free -ing
 * @param x Dimension of the simulation
 * @param y Dimension of the simulation
 * @return A pointer to the simulation
 */
simulation * create_simulation(const int x, const int y) {

    simulation * out = malloc(sizeof(simulation));

    out->x = x;
    out->y = y;

    out->garden = malloc(x * sizeof(inhabitant **));
    for (int n = 0; n < x; n++) {
        out->garden[n] = malloc(y * sizeof(inhabitant *));
    }
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            out->garden[i][j] = NULL;
        }
    }


    out->round = 0;

    out->config = create_empty_config();

    // Setup pointer to hold char *
    out->last_action_message = malloc(sizeof (char *));

    // This is a pointer to the char *, so dereference when malloc-ing
    * (out->last_action_message) = malloc(sizeof (char) * 256);
    * (out->last_action_message)[0] = '\000';


    return out;

}


/**
 * Safely 'delete' a simulation struct
 * Frees all inhabitants, then cleans up and frees itself
 * @param sim The simulation to be freed
 * @return Success
 */
bool free_simulation(simulation * sim) {

    // Sim is free
    if (sim == NULL) return true;

    // Free everything within the sim
    for (int x = 0; x < sim->x; x++) {
        for (int y = 0; y < sim->y; y++) {
#ifdef DEBUG
            printf("checking to free %d,%d \n", x, y);
#endif
#ifdef DEBUG
            if (sim->garden[x][y] != NULL)
                printf("Freed %d at %d, %d\n", sim->garden[x][y]->inhabitant_type, x, y);
#endif
            free_inhabitant(sim->garden[x][y]);
        }
    }

    // Free the rows
    for (int n = 0; n < sim->x; n++)
        free(sim->garden[n]);
    // Free the columns
    free(sim->garden);


    // Free config
    free_config(sim->config);


    // Free action message
    free(*(sim->last_action_message));
    free(sim->last_action_message);


    // Free the sim
    free(sim);

#ifdef DEBUG
    printf("FREED THE SIMULATION!\n");
#endif
    return true;

}


/**
 * Checks if a given position is within the bounds of a simulation
 * @param sim The simulation
 * @param coordinate The position
 * @return if the position is within bounds
 */
bool in_bounds(simulation * sim, const int coordinate[2]) {

    // Checking x axis
    if (coordinate[0] < 0 || coordinate[0] >= sim-> x) return false;
    // Cheching y axis
    if (coordinate[1] < 0 || coordinate[1] >= sim-> y) return false;

    // Position is in-bounds
    return true;
}


/**
 * Checks if a position in a given simulation's "garden" is null
 * @param sim Simulation pointer
 * @param coordinate [X,Y] position
 * @return if that position == NULL
 */
bool is_null(simulation * sim, const int coordinate[2]) {

    if (!in_bounds(sim, coordinate)) return true;

    // If the new position is null return true
    return sim->garden[coordinate[0]][coordinate[1]] == NULL;

}


/**
 * Get the inhabitant in a sim at given coords
 * @param sim The simulation
 * @param coordinate The coordinates
 * @return Inhabitant * if one exists in bounds
 */
inhabitant * sim_get(simulation * sim, const int coordinate[2]) {

    if (!in_bounds(sim, coordinate)) return NULL;

    return sim->garden[coordinate[0]][coordinate[1]];


}


/**
 * Set the value of a pointer in the garden if in bounds
 * @param sim The simulation
 * @param coordinate The coordinates
 * @param i The new inhabitant value
 * @return Success
 */
bool sim_set(simulation * sim, const int coordinate[2], inhabitant * i) {

    if (!in_bounds(sim, coordinate)) return false;

    sim->garden[coordinate[0]][coordinate[1]] = i;
    return true;

}


/**
 * Finds all inhabitants of a given type in a sim within a radius.
 * Does not include inhabitant at 'pos'
 * @param sim The simulation
 * @param type Desired type
 * @param pos Initial position to search around
 * @param radius Distance around pos to look; Radius 0 will only consider pos
 * @return The linked list of relevant coordinates
 */
coord_ll * find_all(simulation * sim, INHABITANT_TYPE type, const int pos[2], int radius) {

    int _coord[2];
    inhabitant * i;

    // Initialise the linked list
    coord_ll * out = coord_ll_init();

    // Check all within radius
    for (int x = pos[0] - radius; x < pos[0] + radius + 1; x++) {
        for (int y = pos[1] - radius; y < pos[1] + radius + 1; y++) {

            if (x == pos[0] && y == pos[1]) continue;

            _coord[0] = x;
            _coord[1] = y;

            if (!in_bounds(sim, _coord)) continue;

            i = sim_get(sim, _coord);

            // If looking for free spaces
            if (i == NULL) {
                if (type == EMPTY)
                    coord_ll_push(out, _coord);
            } else {
                // Check if types match requested
                if (i->inhabitant_type == type)
                    coord_ll_push(out, _coord);
            }

        }
    }

    return out;

}


/**
 * Finds all inhabitants of a given type in a sim adjacent to a point.
 * Does not include inhabitant at 'pos'
 * @param sim The simulation
 * @param type Desired type
 * @param pos Initial position to search around
 * @return A linked list to all relevant coordinates
 */
coord_ll * find_adjacent(simulation * sim, INHABITANT_TYPE type, const int pos[2]) {

    int _coord[2];
    inhabitant * i;

    // Initialise the linked list
    coord_ll * out = coord_ll_init();

    // Check all within radius
    for (int x = pos[0] - 1; x < pos[0] + 2; x++) {
        for (int y = pos[1] - 1; y < pos[1] + 2; y++) {

            // Don't want corners
            if (x != pos[0] && y != pos[1]) continue;
            // Don't want the given pos
            if (x == pos[0] && y == pos[1]) continue;

            _coord[0] = x;
            _coord[1] = y;

            if (!in_bounds(sim, _coord)) continue;

            i = sim_get(sim, _coord);

            // If looking for free spaces
            if (i == NULL) {
                if (type == EMPTY)
                    coord_ll_push(out, _coord);
            } else {
                // Check if types match requested
                if (i->inhabitant_type == type)
                    coord_ll_push(out, _coord);
            }

        }
    }

    return out;

}


/**
 * Print a formatted version of a given simulation to the terminal
 * @param sim The simulation to be displayed
 */
void pretty_print(simulation * sim) {

    char * c_temp;
    inhabitant * i_temp;

    // Print the config name
    if (sim->config != NULL)
        printf("Garden '%s':\n", strcmp(sim->config->name, "") ? sim->config->name : "unknown");

    // Print the header line
    printf("    ");
    for (int i = 0; i < sim->y; i++) printf("%2d ", i);
    printf("\n");

    // Print the rows
    for (int x = 0; x < sim->x; x++) {
        // Row number
        printf("%3d ", x);
        for (int y = 0; y < sim->y; y++) {

            c_temp = " ";
            i_temp = sim_get(sim, (int[2]) {x,y});

            if (i_temp != NULL) {
                switch (i_temp->inhabitant_type) {
                    case LETTUCE:
                        c_temp = "0";
                        break;
                    case SLUG:
                        // Display as different characters depending on maturity
                        c_temp = i_temp->age < sim->config->SLUG_MATURE_AGE ? "s" : "S";
                        break;
                    case FROG:
                        // Same here
                        c_temp = i_temp->age < sim->config->FROG_MATURE_AGE ? "f" : "F";
                        break;
                    default:
                        // Something has gone fatally wrong
                        c_temp = "!";
                        break;
                }}

            // Space the characters out
            printf(" %s ", c_temp);
        }
        printf("\n");
    }
}


/**
 * Update the last action message of a simulation to reflect the last action
 * First 4 args must not be null
 * @param sim The simulation the action is performed in
 * @param i1 The inhabitant performing the action
 * @param coord The coordinates of the performer
 * @param action The action being performed
 * @param i2 The recipient of the action
 * @param target The coordinates of the recipient
 */
void update_action_message(simulation * sim, inhabitant * i1, const int coord[2], ACTION action, inhabitant * i2, const int target[2]) {

    if (sim == NULL) return;
    if (i1 == NULL) return;
    if (coord == NULL) return;

    free(* (sim->last_action_message));
    * (sim->last_action_message) = malloc(sizeof (char) * 256);

    // Max string length 255 + \000
    char *out = malloc(sizeof(char) * 256);
    // Used to store ints in string format, length 4 + \000
    char i_temp1[5];
    char i_temp2[5];

    char t1[8] = "";
    char t2[32] = "";
    if (i1 != NULL)
        switch (i1->inhabitant_type) {
            case LETTUCE:
                sprintf(t1, "LETTUCE");
                break;
            case FROG:
                sprintf(t1, "FROG");

                snprintf(i_temp1, 4, "%d", i1->age);
                snprintf(i_temp2, 4, "%d", i1->hunger);
                sprintf(t2, "[age %s, hunger %s]", i_temp1, i_temp2);
                break;
            case SLUG:
                sprintf(t1, "SLUG");

                snprintf(i_temp1, 4, "%d", i1->age);
                sprintf(t2, "[age %s]", i_temp1);
                break;
            default:
                sprintf(t1, "ERROR!");
        }

    //printf("%s\000", t2);

    char t3[16] = "";
    if (coord != NULL) {
        snprintf(i_temp1, 4, "%d", coord[0]);
        snprintf(i_temp2, 4, "%d", coord[1]);
        sprintf(t3, "at (%s, %s)", i_temp1, i_temp2);
    }


    char t4[40] = "";
    switch (action) {
        case NOTHING:
            sprintf(t4, "did nothing");
            break;
        case DIED:
            sprintf(t4, "died");
            break;
        case EAT:
            sprintf(t4, "ate ");
            break;
        case REPRODUCE:
            sprintf(t4, "produced ");
            break;
        case MOVE:
            sprintf(t4, "moved to");
            break;
        case TIRED:
            sprintf(t4, "has already done something this round");
            break;
        default:
            sprintf(t4, "has a memory leak");
            break;
    }

    char t5[16] = "";
    if (i2 != NULL)
        switch (i2->inhabitant_type) {
            case LETTUCE:
                sprintf(t5, "LETTUCE at");
                break;
            case FROG:
                sprintf(t5, "FROG at");
                break;
            case SLUG:
                sprintf(t5, "SLUG at");
                break;
            default:
                break;
        }

    char t6[16] = "";
    if (target != NULL) {
        snprintf(i_temp1, 4, "%d", target[0]);
        snprintf(i_temp2, 4, "%d", target[1]);
        sprintf(t6, "(%s, %s)", i_temp1, i_temp2);
    }

    snprintf(out, 256, "%s%s %s %s%s %s", t1, t2, t3, t4, t5, t6);
    // Copy to last message
    snprintf(*(sim->last_action_message), 256, "%s%s %s %s%s %s", t1, t2, t3, t4, t5, t6);

    //printf("---> %s <---\n", last_action_message);

}


/**
 * Print the last action message of a simulation to terminal
 * @param sim The simulation
 */
void print_last_action_message(simulation * sim) {

    if (sim->last_action_message == NULL) return;
    if (* (sim->last_action_message) == NULL) return;
    printf("%s\n\n", * (sim->last_action_message));
}
