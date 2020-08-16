#include <iostream>
#include <vector>
using namespace std;

#include "intcode.hpp"
#include "repairDroid.hpp"

void part1();
void part2();

int main() {
    cout << "## Running program ##\n";

    //part1();
    part2();

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
    // create repairDroid object.
    RepairDroid droid; 
    droid.LoadProgram("resc/input.txt");

    // turn on repairDroid to scout the environment. Given that I rendered the maze,
    // I can personally assert that every space in the board, except for one has been
    // traversed, and that one space will not affect the validity of the calculation.
    droid.Activate(); 

    droid.SimulateOxygen();
    cout << "Time for oxygen to fill maze = " << droid.GetFillTime() << " mins \n";
}

