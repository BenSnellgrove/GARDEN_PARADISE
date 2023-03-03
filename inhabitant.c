//
// Created by Ben Snellgrove on 14/12/2022.
//

#include "inhabitant.h"



/**
 * Create an inhabitant struct and return a pointer to it
 * @param i_type The type of the inhabitant
 * @param initial_age Its initial age
 * @param lastMove It's last move, only relevant for SLUG
 * @return The pointer
 */
inhabitant * create_inhabitant(INHABITANT_TYPE i_type, int initial_age, DIRECTION lastMove) {

    if (i_type == EMPTY) return NULL;

    inhabitant * out = malloc(sizeof(inhabitant));
    out->inhabitant_type = i_type;
    out->next_move = lastMove;
    out->age = initial_age;
    out->hunger = 0;
    out->actioned_this_round = false;

    return out;

}


/**
 * Free an inhabitant
 * @param i The inhabitant
 */
void free_inhabitant(inhabitant * i) {
    free(i);
}


/**
 * Attempt to perform an action on an inhabitant
 * Order of potential actions:
 * - Die
 * - Eat
 * - Reproduce
 * - Move
 * - Do nothing
 * Then age is incremented
 * @param sim The simulation this is performed in
 * @param coord The coordinates of the inhabitant
 */
void inhabitant_action(simulation * sim, const int coord[2]) {

    inhabitant * i = sim_get(sim, coord);

    // Skip if the inhabitant has already moved this round
    if (i->actioned_this_round) {
        update_action_message(sim,
                              i, coord, TIRED,
                              NULL, NULL);
        return;
    }


#ifdef DEBUG
    printf("Position %d %d has age %d\n", coord[0], coord[1], i->age);
#endif
    // Just created, ignore this round
    if (i->age >= 0) {

        switch (i->inhabitant_type) {

            case LETTUCE:
                if (event_roll(sim->config->LETTUCE_GROW_PROB) && !i->actioned_this_round) {
                    i->actioned_this_round = breed(sim, coord);
                }
                break;

            case SLUG:
                // Die
                if (i->age > sim->config->SLUG_LIFESPAN) {
                    die(sim, coord);
                    return;
                }

                // Eat
                if (!i->actioned_this_round) {
                    i->actioned_this_round = eat(sim, coord);
                }

                // Reproduce
                if (event_roll(sim->config->SLUG_REPRODUCE_PROB) && !i->actioned_this_round) {
                    i->actioned_this_round = breed(sim, coord);
                }

                // Move
                if (!i->actioned_this_round) {
                    i->actioned_this_round = move_s(sim, coord);
                }

                break;

            case FROG:

                // Die
                if (i->age > sim->config->FROG_LIFESPAN) {
                    die(sim, coord);
                    return;
                }

                // Eat
                if (!i->actioned_this_round) {
                    i->actioned_this_round = eat(sim, coord);
                }

                // Reproduce
                if (event_roll(sim->config->FROG_REPRODUCE_PROB) && !i->actioned_this_round) {
                    i->actioned_this_round = breed(sim, coord);
                    if (i->actioned_this_round)
                        i->hunger = 0;
                }

                // Move
                if (!i->actioned_this_round) {
                    if (i->hunger >= sim->config->FROG_HUNGRY) {
                        i->actioned_this_round = move_f(sim, coord);
                    }
                }

                break;

            default:
                // Should never happen, EMPTY exists as a placeholder
                break;

        }

        if (!i->actioned_this_round)
            update_action_message(sim,
                                  sim_get(sim, coord), coord, NOTHING,
                                  NULL, NULL);

    }

    i->hunger++;
    i->age++;
}



/**
 * Eat function for all inhabitant types, frees the inhabitant that is eaten
 * @param sim The simulation this happens in
 * @param hungry The coordinates of the hungry inhabitant
 * @return If the move was completed
 */
bool eat(simulation * sim, const int hungry[2]) {

    // Input sanitisation
    if (!in_bounds(sim, hungry)) return false;
    if (is_null(sim, hungry)) return false;

    // Check valid typing
    INHABITANT_TYPE type = sim_get(sim, hungry)->inhabitant_type;
    if (type != SLUG && type != FROG) return false;


    // Some setup
    int size;
    int target[2];

    int vision_dist;
    if (type == SLUG) vision_dist = 1;
    if (type == FROG) vision_dist = sim->config->FROG_VISION_DISTANCE;


    // Get all food
    coord_ll * visible_food = find_all(sim, type + 1, hungry, vision_dist);


    size = coord_ll_size(visible_food);
#ifdef DEBUG
    printf("%d is visible_food size\n", size);
#endif

    // If there is no food, don't eat any
    if (size == 0) {
        coord_ll_free(visible_food);
        return false;
    }


    // Pick random food from the list
    coord_ll * chosen = coord_ll_get(visible_food, dice_roll(coord_ll_size(visible_food)));
    target[0] = chosen->values[0];
    target[1] = chosen->values[1];

    // Free the linked list as it is no longer needed
    coord_ll_free(visible_food);

    update_action_message(sim,
                          sim_get(sim, hungry), hungry, EAT,
                          sim_get(sim, target), target);


    // Moving frees the old value, so don't need to update that
    free_inhabitant(sim_get(sim, target));
    sim_set(sim, target, sim_get(sim, hungry));
    sim_set(sim, hungry, NULL);

    return true;

}


/**
 * Create a new creature in a random free space around the given coordinate
 * If required, a check is performed for a valid parent
 * @param sim The simulation to perform the action in
 * @param parent The coordinate {x, y} of the parent
 * @return Success
 */
bool breed(simulation * sim, const int parent[2]) {

    // Input sanitisation
    if (!in_bounds(sim, parent)) return false;

    inhabitant * i = sim_get(sim, parent);
    if (i == NULL) return false;

    // Some setup
    INHABITANT_TYPE type = i->inhabitant_type;
    int size;
    int target[2];
    bool found_mate = false;

    // Lettuce don't need a mate to breed
    if (type == LETTUCE) found_mate = true;

    // Instead of lettuce check, check found mate
    // This somewhat "future-proofs" the code as any other vegetables
    // will not need manual checks here
    if (!found_mate) {

        // Some setup for Slugs/Frogs
        int vision_dist;
        if (type == SLUG) vision_dist = 1;
        if (type == FROG) vision_dist = sim->config->FROG_VISION_DISTANCE;

        int mature_age;
        if (type == SLUG) mature_age = sim->config->SLUG_MATURE_AGE;
        if (type == FROG) mature_age = sim->config->FROG_MATURE_AGE;

        // Haven't created anything that needs freeing at this point
        if (i->age < mature_age) return false;



        // Find all visible of same type as parent
        coord_ll * potential_mates = find_all(sim, type, parent, vision_dist);

        // Check if they can breed
        for (int j = 0; j < coord_ll_size(potential_mates); j++) {
            if (sim_get(sim, coord_ll_get(potential_mates, j)->values)->age >= mature_age)
                found_mate = true;
        }

        coord_ll_free(potential_mates);

    }
    if (!found_mate) return false;


    // Find all free spaces within 1
    coord_ll * visible_spaces = find_all(sim, EMPTY, parent, 1);


    size = coord_ll_size(visible_spaces);
#ifdef DEBUG
    printf("%d is visible_spaces size\n", size);
#endif

    // There is no space to create another lettuce
    if (size == 0) {
        coord_ll_free(visible_spaces);
        return false;
    }


    // Pick a space to move to
    coord_ll * chosen = coord_ll_get(visible_spaces, dice_roll(size));
    target[0] = chosen->values[0];
    target[1] = chosen->values[1];

    coord_ll_free(visible_spaces);

#ifdef DEBUG
    printf("Got here\n");
    printf("%d %d  is new lettuce\n", target[0], target[1]);
#endif

    // Perform the move
    free_inhabitant(sim_get(sim, target));
    sim_set(sim, target, create_inhabitant(type, 0, STATIONARY));

    /// I have decided that an entity should not do anything in the round it is created in
    sim_get(sim, target)->actioned_this_round = true;

    // Action message
    update_action_message(sim,
                          sim_get(sim, parent), parent, REPRODUCE,
                          sim_get(sim, target), target);

    return true;

}


/**
 * Move a frog to a random free location which it can see
 * @param sim The simulation the frog is in
 * @param frog The coordinates of the frog
 * @return Success
 */
bool move_f(simulation * sim, const int frog[2]) {

    // Input sanitisation
    if (!in_bounds(sim, frog)) return false;
    if (is_null(sim, frog)) return false;
    if (sim_get(sim, frog)->inhabitant_type != FROG) return false;

    coord_ll * visible_spaces = find_all(sim, EMPTY, frog, sim->config->FROG_VISION_DISTANCE);

    int size = coord_ll_size(visible_spaces);
#ifdef DEBUG
    printf("%d is visible_spaces size\n", size);
#endif

    // There is no space to move too
    if (size == 0) {
        coord_ll_free(visible_spaces);
        return false;
    }

    int target[2];
    coord_ll * chosen = coord_ll_get(visible_spaces, dice_roll(size));
    target[0] = chosen->values[0];
    target[1] = chosen->values[1];

    coord_ll_free(visible_spaces);

    free_inhabitant(sim_get(sim, target));
    sim_set(sim, target, sim_get(sim, frog));
    sim_set(sim, frog, NULL);

    update_action_message(sim,
                          sim_get(sim, target), frog, MOVE,
                          NULL, target);

    return true;

}


/**
 * Move a slug in the direction it was already travelling
 * If this isn't possible, perform one recursive call with a new random legal direction
 * If none are found return false
 * @param sim The simulation the slug is in
 * @param slug The coordinates of the slug
 * @return Success
 */
bool move_s(simulation * sim, const int slug[2]) {

    // Input sanitisation
    if (!in_bounds(sim, slug)) return false;
    if (is_null(sim, slug)) return false;
    if (sim_get(sim, slug)->inhabitant_type != SLUG) return false;

    int _coord[2];
    inhabitant * i = sim_get(sim, slug);

    _coord[0] = slug[0];
    _coord[1] = slug[1];

    if (i->next_move != STATIONARY) {

        change_pos(_coord, i->next_move);
        if (in_bounds(sim, (_coord))) {
            if (is_null(sim, _coord)) {

                free_inhabitant(sim_get(sim, _coord));
                sim_set(sim, _coord, sim_get(sim, slug));
                sim_set(sim, slug, NULL);

#ifdef DEBUG
                printf("MOVE TO %d, %d\nDIrection is %d\n", _coord[0], _coord[1], i->next_move);
#endif

                update_action_message(sim,
                                      sim_get(sim, _coord), slug, MOVE,
                                      NULL, _coord);

                return true;

            }
            // Position is occupied
        }
        // Position is out of bounds
    }

    // Next move was not legal OR next_move was 'stationary'
    i->next_move = STATIONARY;
    int legal_moves = 15;

    // I am going to do bit operations.
    for (int b = 8; b > 0; b >>= 1) {

        _coord[0] = slug[0];
        _coord[1] = slug[1];

        // If this is a legal move
        if (legal_moves & b) {

            // Check if that direction is a legal potential move
            change_pos(_coord, b);
            if (in_bounds(sim, _coord))
                if (is_null(sim, _coord))
                    continue;

            legal_moves -= b;
        }
    }

#ifdef DEBUG
    printf("LEGAL MOVES IS %d\n", legal_moves);
#endif

    // No adjacent free squares
    if (!legal_moves) {
        return false;
    }


    i->next_move = pick_random_bit(legal_moves);
#ifdef DEBUG
    printf("NEXT MOVE IS %d\n", i->next_move);
#endif
    return move_s(sim, slug);

}


/**
 * Function called when an inhabitant dies
 * @param sim The simulation the inhabitant is in
 * @param coord Coordinates of the inhabitant
 */
void die(simulation * sim, const int coord[2]) {

    update_action_message(sim,
                          sim_get(sim, coord), coord, DIED,
                          NULL, NULL);

    free_inhabitant(sim_get(sim, coord));
    sim_set(sim, coord, NULL);

}


