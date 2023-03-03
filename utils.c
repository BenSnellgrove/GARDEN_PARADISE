//
// Created by Ben Snellgrove on 14/12/2022.
//


#include "utils.h"


/**
 * Create a header pointer for a coord linked list
 * @return The pointer
 */
coord_ll * coord_ll_init(void) {

    coord_ll * out = malloc(sizeof(coord_ll));
    out->values[0] = 0;
    out->values[1] = 0;
    out->ignore = true;
    out->next = NULL;

    return out;

}


/**
 * Create a new linked list struct
 * @param values The values to add
 * @return The pointer to the linked list struct
 */
coord_ll * coord_ll_make(const int values[2]) {

    coord_ll * out = malloc(sizeof(coord_ll));
    out->values[0] = values[0];
    out->values[1] = values[1];
    out->ignore = false;
    out->next = NULL;

    return out;

}


/**
 * Get the coord_ll struct at a given index in a chain
 * @param head The start of the coord_ll chain
 * @param index The index to get
 * @return Pointer to the requested struct if it exists
 */
coord_ll * coord_ll_get(coord_ll * head, const int index) {

    if (index < 0) return NULL;
    if (head == NULL) return NULL;

    // I wanted to put these in ternary operators so here they are
    // If we want to ignore the current value, recursive call with no change
    return (head->ignore) ?
           coord_ll_get(head->next, index) :
           // Otherwise
           // If index == 0, return this
           // else decrement index by 1 and recurse
           (!index) ?
                    head :
                    coord_ll_get(head->next, index - 1);

}


/**
 * Add a new value pair to the linked list struct chain
 * @param head Pointer to the start of the linked list
 * @param values The new values to be added
 */
void coord_ll_push(coord_ll * head, const int values[2]) {

    if (head->next == NULL) {

        coord_ll * new = coord_ll_make(values);
        head->next = new;
        return;

    }

    coord_ll_push(head->next, values);

}


/**
 * Remove a struct from a chain of coord_ll structs
 * @param head Start of the linked list chain
 * @param index The index to remove
 * @return Success
 */
bool coord_ll_remove(coord_ll * head, const int index) {

    if (index < 0) return false;
    if (head == NULL) return false;

    // Index == 0 but in a C way
    if (!index) {
        // These are nested to prevent NULL->NULL
        if (head->next != NULL) {
            if (head->next->next != NULL) {
                coord_ll * temp = head->next;
                head->next = head->next->next;
                free(temp);
                return true;

            }

            return false;
        }

        return false;

    }

    //                                      If we are ignoring values, don't decrement index
    return coord_ll_remove(head->next, (head->ignore) ? index : index - 1);

}


/**
 * Free a whole linked list chain
 * @param head Pointer to the start of the chain
 */
void coord_ll_free(coord_ll * head) {

    if (head == NULL) return;

    coord_ll_free(head->next);
    free(head);

}


/**
 * Find the size of a linked list chain
 * @param head The start of the chain
 * @return Size of the list
 */
int coord_ll_size(coord_ll * head) {

    if (head == NULL) return 0;

    // Here, the ternary operator seems unnecessary, but only the values of 'false' can be guaranteed
    //  Despite stdbool defining false as 0 and true as 1, code such as
    //  head->ignore = 3;
    //  Will still compile and cause logic errors here
    return ((head->ignore) ? 0 : 1) + coord_ll_size(head->next); // This feels very Haskell-eqsue

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// This section of code was from the assignment brief
/// I have modified it however the original was not mine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Clear the terminal output for whichever platform the project is compiled for
 * Supports:
 *  Mac
 *  Windows
 *  GNU/Linux
 */
void clear_output(void) {

#ifdef DEBUG
    return;
#endif

#ifdef __WIN32
    // Should find all Windows systems
    system("cls");
#endif

#ifdef __unix__
    // Should find all unix systems
    system("clear");
#endif

#ifdef __APPLE__
    // Should find all Apple machines
    system("clear");
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// End of stolen code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * Change the values of a coordinate pair by 1 in a given direction
 * @param start_pos The coordinate pair
 * @param dir The direction to change it in
 */
void change_pos(int start_pos[2], DIRECTION dir) {

    // In each case
    // Increment / Decrement the value
    // Compare to bounds of garden (0, sim->x/y)
    // Return false if unsafe to access that memory location

    switch (dir) {
        case NORTH:
            start_pos[0]--;
            return;

        case EAST:
            start_pos[1]++;
            return;

        case SOUTH:
            start_pos[0]++;
            return;

        case WEST:
            start_pos[1]--;
            return;

        default:
            return;

    }
}


/**
 * Roll a 'dice' with n sides, values 0 -> n - 1
 *
 * The original random code, WHICH HAS BEEN MODIFIED, was provided by Neal
 * @authors Neal Snooke, Ben Snellgrove
 * @param sides The amount of sides on the dice
 * @return The value of the dice
 */
int dice_roll(const int sides) {
    if (sides < 0) return 0;
    if (sides == 0) return 0;
    return ((rand() % sides));
}


/**
 * Flip a weighted coin to determine whether or not an event will happen
 *
 * The original random code, WHICH HAS BEEN MODIFIED, was provided by Neal
 * @authors Neal Snooke, Ben Snellgrove
 * @param probability the weight of the coin
 * @return true/false
 */
bool event_roll(double probability) {
    return probability > ((double) rand())/RAND_MAX;
}


/**
 * Select a random bit from any that are 1 in an int
 * @param options The bits to be selected from
 * @return An int with only that bit as 1
 */
int pick_random_bit(int options) {

#ifdef DEBUG
    printf("OPTIONS are %d, ", options);
#endif
    if (!options) return 0;

    int count = 0;

    for (int b = 1; b > 0; b <<= 1)
        if (options & b) count++;

#ifdef DEBUG
    printf("COUNT IS %d, ", count);
#endif
    if (count == 1) return options;

    int choose = dice_roll(count);

#ifdef DEBUG
    printf("choose is %d, ", choose);
#endif

    for (int b = 1; b > 0; b <<= 1) {
        if (options & b) {

#ifdef DEBUG
            if (!choose)
                printf("Choosing %d\n", b);
#endif

            if (!choose) return b;
            choose--;
        }
    }

    // Should never be reached
    return 0;

}


/**
 * Check if a file exists
 * @param filename The name of the file
 * @return If it exists
 */
bool file_exists(const char * filename) {

    return (fopen(filename, "r"));

}



