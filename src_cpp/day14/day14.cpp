#include <iostream>
#include <fstream>  // for file stream (file writing)
#include <vector>
using namespace std;

#include "nanofactory.hpp"


void part1(bool test=false);
void part2(bool test=false);

int main() {
    cout << "running:\n";

    //part1(false);
    part2(false);

    cout << "ending program\n";
    return 0;
}

void part1(bool test/*=false*/) {
    NanoFactory factory;
    if(test == true)
        factory.LoadReactions("resc/test_input_1.txt");
    else
        factory.LoadReactions("resc/input.txt");

    long oreNeeded = factory.CalculateOreFuelRatio();
    
    cout << "ore needed to produce 1 fuel = " << oreNeeded << "\n";
}

void part2(bool test/*=false*/) {
    NanoFactory factory;
    if(test == true)
        factory.LoadReactions("resc/test_input_1.txt");
    else
        factory.LoadReactions("resc/input.txt");

    // using 1 trillion ore
    long fuelProduced = factory.ProduceFuel(1000000000000);  // 1000000000000

    cout << "fuel produced = " << fuelProduced << "\n";  
}

