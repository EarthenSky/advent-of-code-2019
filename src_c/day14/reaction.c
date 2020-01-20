#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reaction.h"

void free_chemical(chemical_t* c) {
    free(c->name);
    free(c);
}

void free_chemicals(chemical_t** cs, int len) {
    for(int i = 0; i < len; i++)
        free_chemical(cs[i]);
}

void free_reaction(reaction_t* r) {
    free_chemicals(r->input_chemicals, r->input_len);
    free_chemicals(r->output_chemicals, r->output_len);
    free(r);
}

void free_reactions(reaction_t** rs, int len) {
    for(int i = 0; i < len; i++)  
        free_reaction(rs[i]);
}

// len gets reset to the value;
const int MAX_LINE_SIZE = 256;
reaction_t** read_reactions_from_file(const char* file_location, int* len) {
    FILE* file = fopen(file_location, "r");
    char line[MAX_LINE_SIZE];
    char line_copy[MAX_LINE_SIZE];

    // find how many lines are in the file.
    *len = 1; // if there are no newlines, there can still be some text (1 line)
    char c;
    for (c = getc(file); c != EOF; c = getc(file)) 
        if (c == '\n') (*len)++;
    fseek(file, 0, SEEK_SET);  // go back to beginning

    // allocate memory for array of reactions.
    reaction_t** out = (reaction_t**) malloc( sizeof(reaction_t*) * (*len) );
    int i = 0;
    while (fgets(line, MAX_LINE_SIZE, file)) {
        strcpy(line_copy, line);

        out[i] = (reaction_t*) malloc(sizeof(reaction_t));
        out[i]->input_len = 0;
        out[i]->output_len = 1;  // allways 1

        // count the amount of commas in input (input chemicals).
        char* input = strtok(line, "=");
        char* output = strtok(NULL, "=") + 2;  // moves forwards 2 chars.

        input = strtok(input, ","); 
        while(input != NULL) {
            out[i]->input_len++;
            input = strtok(NULL, ",");  
        }

        out[i]->input_chemicals = (chemical_t**) malloc(sizeof(chemical_t*) * out[i]->input_len);
        out[i]->output_chemicals = (chemical_t**) malloc(sizeof(chemical_t*) * out[i]->output_len);

        // create chemicals from input.
        //printf("%s", line_copy);
        input = strtok(line_copy, "=");  // reseting the internal strtok thing  
        input = strtok(input, ", "); 
        int j = 0;
        while(input != NULL) {
            chemical_t* chem = (chemical_t*) malloc(sizeof(chemical_t));
            chem->value = atoi(input);

            input = strtok(NULL, ", "); 
            chem->name = (char*) malloc(strlen(input) * sizeof(char)); 
            strcpy(chem->name, input);

            out[i]->input_chemicals[j] = chem;
            j++;

            // this input is the number for the next chemical or null.
            input = strtok(NULL, ", ");
        }

        // create chemical from output
        output = strtok(output, ", \r\n");
        out[i]->output_chemicals[0] = (chemical_t*) malloc(sizeof(chemical_t));
        out[i]->output_chemicals[0]->value = atoi(output);

        output = strtok(NULL, ", \r\n");
        out[i]->output_chemicals[0]->name = (char*) malloc(strlen(output) * sizeof(char)); 
        strcpy(out[i]->output_chemicals[0]->name, output);

        i++;
    }

    return out;
}

// finds a reaction by its output chemical's name (lets see if inline does anything)
reaction_t* find_reaction(const char* src_name, reaction_t* rs[], int rs_len) {
    reaction_t* tmp = NULL;
    for(int i = 0; i < rs_len; i++) {
        //printf("%s == %s\n", rs[i]->output_chemicals[0]->name, src_name);
        if(strcmp(rs[i]->output_chemicals[0]->name, src_name) == 0) {
            tmp = rs[i];
            break;
        }
    }

    if (tmp == NULL) printf("invalid name: %s\n", src_name);
    return tmp;
}