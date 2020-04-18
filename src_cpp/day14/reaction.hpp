#ifndef REACTION
#define REACTION

#include <vector>
#include <string>
using namespace std;


// This represents a chemical variable in an equation.
struct Chemical {
    long yield;
    string name;
    Chemical(long yield, string name) 
        : yield(yield), name(name) {}
    
    Chemical(Chemical* chemical) 
        : yield(chemical->yield), name(chemical->name) {}
};


struct Reaction {
    vector<Chemical*> inputs;
    Chemical* output;

    Reaction(string reactionData);
    ~Reaction();
};

#endif //!REACTION