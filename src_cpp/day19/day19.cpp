#include <iostream>
#include <vector>
using namespace std;

#include "util.hpp"
#include "droneControlSystem.hpp"

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
    cout << "-- Part 1 --\n";

    DroneControlSystem dcs;
    dcs.LoadControlProgram("resc/input.txt");

    int width = 50, height = 50;
    vector<char> grid;
    
    cout << "got grid\n";

    int tractorBeamAffectedPoints = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            DeployReport report = dcs.SendDroneToCoord(x, y);
            if (report == Stationary) {
                grid.push_back('.');
            } else if (report == Pulled) {
                grid.push_back('#');
                tractorBeamAffectedPoints += 1;
            } else {
                grid.push_back('E');
            }
        }
    }

    cout << "filled grid\n";

    DisplayVecAsMat(grid, width);
    cout << "tractorBeamAffectedPoints = " << tractorBeamAffectedPoints << endl;
}

void part2() {
    cout << "-- Part 2 --\n";
}