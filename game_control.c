//
// Created by Ben Snellgrove on 03/01/23.
//

#include "game_control.h"



/**
 * Iterate over every inhabitant in a sim and action them
 * @param sim The simulation
 */
void garden_round(simulation * sim) {
    int t[2];

    for (int x = 0; x < sim->x; x++) {
        for (int y = 0; y < sim->y; y++) {
            t[0] = x; t[1] = y;

            if (sim_get(sim, t) != NULL) {
#ifdef DEBUG
                printf("\nAction %d %d:", x,y);
                if (sim->garden[x][y]->inhabitant_type == FROG) {
                    printf("\n%d rounds old", sim->garden[x][y]->age);
                }
#endif
                inhabitant_action(sim, t);
#ifdef DEBUG
                if (sim->garden[x][y] != NULL)
                if (sim->garden[x][y]->inhabitant_type == FROG) {
                    printf("\n%d rounds old post action\n", sim->garden[x][y]->age);
                };
#endif

            }
        }
    }

    reset_actions(sim);

    sim->round++;

}


/**
 * Set the 'actioned_this_round' value of every inhabitant struct in a simulation to false
 * @param sim The simulation
 */
void reset_actions(simulation * sim) {
    int t[2];

    // Reset the actions for next round
    for (int x = 0; x < sim->x; x++) {
        for (int y = 0; y < sim->y; y++) {

            t[0] = x; t[1] = y;

            if (sim_get(sim, t) != NULL) {
#ifdef DEBUG
                printf("\nDe actioned  %d %d", x,y);
#endif
                sim_get(sim, t)->actioned_this_round = false;
            }
        }
    }
}


/**
 * Display a formatted terminal header
 */
void terminal_header(void) {

    printf("Running Garden Paradise (");

    print_version();

    printf(")\n\n");

}


/**
 * Print the version of the project as defined in buildinfo.h
 */
void print_version(void) {
#ifdef VERSION
    printf("%s", VERSION);
#endif

#ifndef VERSION
    printf("v0.0");
#endif
}


/**
 * Prompt the user to either continue or quit the game loop
 * @return Whether or not to quit
 */
bool quit_sim_loop(void) {

    printf("<enter> to continue or 'q' to quit: ");

    // Q is 01010000
    // q is 01110000
    int c = getchar();
    // Detect both cases
    if (!((c | 32) ^ 'q')) return true;
    // Clear buffer
    while (c != '\n') {
        c = getchar();
    }

    return false;
}
