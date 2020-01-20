#ifndef REACTION
#define REACTION

// ------------------------------------------------------------------------- //

// represents a chemical in a reaction
struct _chemical {
    char* name;
    int value;  // yield
};
typedef struct _chemical chemical_t;

// stores data for how a certain reaction works. 
struct _reaction {
    int input_len;
    int output_len;
    chemical_t** input_chemicals;
    chemical_t** output_chemicals;
};
typedef struct _reaction reaction_t;

// ------------------------------------------------------------------------- //

// functions needed
void free_chemical(chemical_t* c); 
void free_chemicals(chemical_t** cs, int len); 
void free_reaction(reaction_t* r);  // needed to free malloced strings.

void free_reactions(reaction_t** rs, int len);
reaction_t** read_reactions_from_file(const char* file_location, int* len);

reaction_t* find_reaction(const char* src_name, reaction_t* rs[], int rs_len);

// ------------------------------------------------------------------------- //

#endif //!REACTION