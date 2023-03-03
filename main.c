/*
 * Created in C17 by Ben Snellgrove
 * Not licensed, just made by me
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "buildinfo.h" // Contains preprocessor 'DEBUG' switch
#include "config.h"
#include "game_control.h"
#include "inhabitant.h"
#include "simulation.h"
#include "utils.h"

#include "global_structs.h"




int main(int argc, char **argv) {

    // Thanks Neal for these 2 lines
    // Seed is program run time
    time_t t = time(0);
    srand(t);


    char configfile[256];
    bool run_3_mode = false; // for --3 mode

    if (argc > 1) {

        if (!strcmp(argv[1], "--3")) run_3_mode = true; // for --3 mode

        strncpy(configfile, argv[1], 255);
        configfile[255] = '\000';
    }


    simulation * sim1 = NULL;
    simulation * sim2 = NULL; // for --3 mode
    simulation * sim3 = NULL; // for --3 mode

    // If the config file is in the args
    if (argc > 1) {
        if (file_exists(argv[1]))
            sim1 = read_file(argv[1]);
    } else {
        sim1 = create_simulation(12, 12);
    }


    // --3 mode setup
    if (run_3_mode) {
        // Free each first JUST IN CASE
        free_simulation(sim1);
        if (file_exists("config1.txt")) sim1 = read_file("config1.txt");
        else sim1 = create_simulation(12, 12);

        free_simulation(sim2);
        if (file_exists("config2.txt")) sim2 = read_file("config2.txt");
        else sim2 = create_simulation(12, 12);

        free_simulation(sim3);
        if (file_exists("config3.txt")) sim3 = read_file("config3.txt");
        else sim3 = create_simulation(12, 12);
    }


    clear_output();

    printf("Welcome to Garden Paradise (");
    print_version();
    printf(")\n\nPlease select a game mode!\n'round' | 'move':\n");


    // Get the mode to run the game in
    char runMode[8];
    fgets(runMode, 8, stdin);
    // Delete newline character
    runMode[7] = '\000';
    runMode[strlen(runMode) - 1] = '\000';


    // for --3 mode
    if (run_3_mode && !strcmp(runMode, "round")) {
        pretty_print(sim2);
        pretty_print(sim3);
    }


    // Round mode
    if (!strcmp(runMode, "round")) {

        // Forever
        for (;;) {

            // Action the sim
            garden_round(sim1);

            // Print
#ifndef DEBUG
            clear_output();
#endif
            terminal_header();
            pretty_print(sim1);
            printf("End of round %d\n", sim1->round);

            // --3 mode code
            if (run_3_mode) {
                printf("\n");
                garden_round(sim2);
                pretty_print(sim2);
                printf("End of round %d\n\n", sim2->round);

                garden_round(sim3);
                pretty_print(sim3);
                printf("End of round %d\n", sim3->round);
            }

            // Ask for input, break if 'q' or 'Q' pressed
            if (quit_sim_loop()) break;
        }
    }


    // Move mode
    else if (!strcmp(runMode, "move")) {

        // Need a check for quitting as this uses nested for loops
        bool quit = false;

        int temp[2];

        // Until quit
        for (;;) {
            // For each coordinate pair
            for (int x = 0; x < sim1->x; x++) {
                if (quit) break;
                for (int y = 0; y < sim1->y; y++) {
                    temp[0] = x;
                    temp[1] = y;
                    // That isn't null
                    if (sim_get(sim1, temp) == NULL) continue;

                    // Do 1 action, print, wait for input
                    inhabitant_action(sim1, temp);
#ifndef DEBUG
                    clear_output();
#endif
                    terminal_header();
                    pretty_print(sim1);
                    // Formatted action message
                    print_last_action_message(sim1);

                    // Prompt for escape. If 'q' or 'Q', mark the loop for quitting and then break
                    if ((quit = quit_sim_loop())) break;

                }
            }

            // Reset the sim
            reset_actions(sim1);
            if (quit) break;


            // Check for empty grid
            coord_ll * tcl = find_all(sim1, EMPTY, (int[]) {-1, -1}, (sim1->x > sim1->y ? sim1->x : sim1->y) + 1 );
            // If the entire grid is empty:
            if (coord_ll_size(tcl) == sim1->x * sim1->y) {
#ifndef DEBUG
                clear_output();
#endif
                terminal_header();
                pretty_print(sim1);
                // Prompt for quit
                printf("\nSimulation is empty! Are you sure you wish to continue?\n");
                if ((quit = quit_sim_loop())) break;
            }
            coord_ll_free(tcl);
        }
    }

    // Reject all except valid modes
    else {
        printf("Sorry, that's not a valid mode!\n\n");
    }


    printf("Thank you for playing!\n");


    free_simulation(sim1);
    // Safe to free sim2 and sim3 as they are defined as NULL by default
    // Slight sacrifice of memory efficiency for VERY slight increase in performance
    free_simulation(sim2); // for --3 mode
    free_simulation(sim3); // for --3 mode


    return 0;
}
