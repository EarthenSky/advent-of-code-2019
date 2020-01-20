#include <stdio.h>
#include <string.h>

#include "reaction.h"
#include "alg.h"

#include "../res/vec.h"  // this is for vectors in c.

int get_ore_to_fuel_ratio(reaction_t* r[], int rs_len);

int main() {
    printf("part1: \n");
    int rs_len = 0;
    reaction_t** reactions = read_reactions_from_file("test_input", &rs_len);
    int ore_to_fuel_ratio = ore_fuel_ratio(reactions, rs_len); //get_ore_to_fuel_ratio(reactions, rs_len);
    printf("%d ore -> 1 fuel\n", ore_to_fuel_ratio);


    printf("part2: \n");


    printf("done\n");
    return 0;
}


// ------------------------------------------------------------------------- //

// how much ore is needed to get the current output chemical.
int recursive_get_value_in_ore(reaction_t* fuel, reaction_t* rs[], int rs_len) {
    // recursively get sum of all inputs of a reaction in rounded up "ORE" cost
    int sum = 0;
    for(int i = 0; i < fuel->input_len; i++) {
        //printf("here2.5 + %d + %d\n", i, fuel->input_len);  // todo: later
        //printf("-- %s --\n", fuel->input_chemicals[i]->name );
        if( strcmp(fuel->input_chemicals[i]->name, "ORE") == 0 ) {
            printf("ore\n");
            sum += fuel->input_chemicals[i]->value;  // base case.
        } else {
            printf("recurse\n");
            // do this again but with the next reaction
            char* src_name = fuel->input_chemicals[i]->name;
            reaction_t* next_reaction = find_reaction(src_name, rs, rs_len);
            printf("next reac: %s\n", next_reaction->output_chemicals[0]->name);
            sum += round_up_to_multiple(recursive_get_value_in_ore(next_reaction, rs, rs_len), next_reaction->output_chemicals[0]->value);
        }
    }

    /*
    int multiple = fuel->input_chemicals[0]->value;
    if(fuel->input_len > 1) {  // case: there are at least two input chemicals
        for(int i = 1; i < fuel->input_len; i++) {
            multiple = lcm(fuel->input_chemicals[i]->value, multiple);
        }
    }*/

    // is this correct math? --> NO WTF --> maybe now?
    printf("sum= %d, value= %d, total=%d\n", sum, fuel->output_chemicals[0]->value, ceil_div(sum, fuel->output_chemicals[0]->value));

    // divide by my coefficient (rounds up [b/c ceil])
    // rounding up is an important part of the problem.
    return /*ceil_div(*/sum;/*, fuel->output_chemicals[0]->value);*/
    
}

// this is the solution to the problem
int get_ore_to_fuel_ratio(reaction_t* rs[], int rs_len) {
    reaction_t* root_reaction = find_reaction("FUEL", rs, rs_len);

    // recursively find the amount of ore that makes up the fuel reaction.
    return recursive_get_value_in_ore(root_reaction, rs, rs_len);
}

// Note:
// - my recursive approach can have extra unused chemicals.
// - instead, do a recursive "expand search" to determine how many of all 
//   "base checmicals" are needed. After that, do the ore conversion.
// - 


// for storing chemical vectors.
typedef vec_t(chemical_t*) vec_chemical_t;

// This function converts the input chemicals of a reaction into the input values
// from other equations that create that chemical.
int layered_get_sub_chemicals(reaction_t* fuel, reaction_t* rs[], int rs_len) {
    vec_chemical_t chemicals, extras;
    vec_init(&chemicals); 
    vec_init(&extras); 

    printf("test\n");
    
    // fill data
    for(int i = 0; i < fuel->input_len; i++) {
        chemical_t* chem = malloc(sizeof(chemical_t*));  // need to free this.
        chem->name = fuel->input_chemicals[i]->name;
        chem->value = fuel->input_chemicals[i]->value;

        printf("push\n");
        vec_push(&chemicals, chem);
    }

    printf("test2\n");
    
    // Convert a chemical into its components.
    while(chemicals.length > 1 || strcmp(chemicals.data[0]->name, "ORE") == 0) {
        printf("loop\n");
        int i = 0;  // will be the index of the not ore item.
        while(strcmp(chemicals.data[i]->name, "ORE")) { i++; }  // THIS IS THE PROBLEM : TODO: THIS.
        printf("i=%d\n", i);
        chemical_t* chemical_to_convert = chemicals.data[i];

        printf("not ore index\n");

        // find reaction to make the selected item.
        int iChem = 0;
        for(iChem = 0; iChem < rs_len; iChem++) {
            if (chemical_to_convert->name == rs[iChem]->output_chemicals[0]->name)
                break;
        }

        printf("before foreach\n");

        // How many recycled chemicals can be used.
        int recycled_index; chemical_t* recycled_chemical;
        vec_foreach(&extras, recycled_chemical, recycled_index) {
            if(recycled_chemical->name == chemical_to_convert->name) // case: there are chemicals to be recycled.
                break;
        }
        printf("loop2\n");

        // the recycled chemical will never reach 0 without being "used up" first.
        int recycled_chemical_value = (recycled_chemical == NULL) ? 0 : recycled_chemical->value;

        int chemical_needed = chemical_to_convert->value;    
        int output_chemical_yield = rs[iChem]->output_chemicals[0]->value;

        // calculate recycled chemical component and remove from recycled chemical list if needed.
        if(recycled_chemical_value >= chemical_needed) {
            chemical_needed = 0;
            recycled_chemical->value -= chemical_needed;
            printf("overkill\n");

        } else {
            chemical_needed -= recycled_chemical->value;

            printf("good2\n");

            // case: the recycled chemical has been used up.
            if(recycled_chemical_value != 0) {
                // Remove the recycled chemical from the list.
                chemical_t* tmp = extras.data[recycled_index];
                vec_splice(&extras, recycled_index, 1);  //TODO: check this.
                free(tmp); 
            }

            // ---------------------------------------------------------------------- //

            // how many times the reaction has to be run.
            int reaction_iterations = round_up_to_multiple(chemical_needed, output_chemical_yield) / output_chemical_yield;

            // add leftover chemicals to the extras list.
            int extras_chemical_yield = output_chemical_yield - chemical_needed * reaction_iterations;

            // ---------------------------------------------------------------------- //

            // Check if the leftover chemical already exists.
            int chemical_exists = 0; int j; chemical_t* tmp;
            vec_foreach(&extras, tmp, j) {
                if(tmp->name == chemical_to_convert->name) { // case: there are chemicals to be recycled.
                    chemical_exists = 1;
                    break;
                }   
            }

            // create new chemical or incrememnt existing chemical value.
            if(chemical_exists) {
                extras.data[j]->value += extras_chemical_yield;
            } else {
                chemical_t* chem = malloc(sizeof(chemical_t*));  // need to free this.
                chem->name = chemical_to_convert->name;  // TODO: is this correct.
                chem->value = extras_chemical_yield;
                vec_push(&extras, chem);
            }

            // ---------------------------------------------------------------------- //

            // add chemicals to the main vector / fill the existing parts.
            for(int j = 0; j < rs[iChem]->input_len; j++) {
                chemical_t* chem = malloc(sizeof(chemical_t*));  // need to free this.
                chem->name = fuel->input_chemicals[j]->name;
                chem->value = fuel->input_chemicals[j]->value * reaction_iterations;  // This is how many times the reaction was run.

                vec_push(&chemicals, chem);
            } 

            // ---------------------------------------------------------------------- //

            // remove the chemical from the list.
            tmp = chemicals.data[i];
            vec_splice(&chemicals, i, 1);  //TODO: check this.
            free(tmp);

        }
    }

    printf("done\n");

    // the last chemical left will be "ORE" (since its never an output chemical)
    int out = chemicals.data[0]->value;

    vec_deinit(&chemicals);
    vec_deinit(&extras);

    return out;
}

int ore_fuel_ratio(reaction_t* rs[], int rs_len) {
    reaction_t* root_reaction = find_reaction("FUEL", rs, rs_len);

    // uses the "layered" approach to getting the sub chemicals, until it is left with only ore.
    // Works non-recursively.
    return layered_get_sub_chemicals(root_reaction, rs, rs_len);
}