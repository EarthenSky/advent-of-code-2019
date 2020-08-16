#include <iostream>
#include <vector>
using namespace std;

#include "intcode.hpp"
#include "repairDroid.hpp"

void part1();
void part2();

int main() {
    cout << "## Running program ##\n";

    part1();
    //part2();

    cout << "## Ending program ##\n";
    return 0;
}

void part1() {
    // create repairDroid object.
    RepairDroid droid; 
    droid.LoadProgram("resc/input.txt");
    droid.Activate(); // turn on repairDroid.
    
    // get output from repairDroid on distance from start to oxygen system.
    cout << "Min move commands = " << droid.GetMinPathLen() << "\n";
}

void part2() {
    
}

