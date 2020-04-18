#include "reaction.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

// Initialize this reaction.
Reaction::Reaction(string reactionData) {
    size_t middle = reactionData.find("=>");
    string input_string = reactionData.substr(0, middle);
    string output_string = reactionData.substr(middle + 2, string::npos);

    // split the list of chemicals
    stringstream ss(input_string);
    string token;
    while (getline(ss, token, ',')) {
        // split the single chemical in half
        stringstream css(token);

        // This skips any leading spaces
        string tmp;
        while((getline(css, tmp, ' ')), tmp == "") {}
        long yield = stoi(tmp);

        string name;
        getline(css, name, ' ');
        
        inputs.push_back( new Chemical(yield, name) );
    }

    // split the single chemical in half
    stringstream css(output_string);

    // This skips any leading spaces
    string tmp;
    while((getline(css, tmp, ' ')), tmp == "") {}
    long yield = stoi(tmp);

    string name;
    getline(css, name, ' ');
    
    output = new Chemical(yield, name);
}

// free all the data
Reaction::~Reaction() {
    for(Chemical* c : inputs) 
        delete c;
        
    delete output;
}
