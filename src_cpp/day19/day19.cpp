#include <iostream>
#include <vector>
using namespace std;

#include "util.hpp"
#include "droneControlSystem.hpp"

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
    cout << "-- Part 1 --\n";

    DroneControlSystem dcs;
    dcs.LoadControlProgram("resc/input.txt");

    int width = 50, height = 50;
    vector<char> grid;

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

    DisplayVecAsMat(grid, width);
    cout << "tractorBeamAffectedPoints = " << tractorBeamAffectedPoints << endl;
}

void doSliceCheck(DroneControlSystem &dcs, int &lastStartingX, int curX, int curY, bool &found100, int &resultX);
bool doVerticalCheck(DroneControlSystem &dcs, int curX, int curY);
bool doVerticalCheckFast(DroneControlSystem &dcs, int curX, int curY);

void part2() {
    cout << "-- Part 2 --\n";

    DroneControlSystem dcs;
    dcs.LoadControlProgram("resc/input.txt");

    int lastStartingX = 0;
    int curX = 0; 
    int curY = 200;  // Fixes the empty line problem which may occur at the beginning of rays.
                     // This can be tweaked as a form of 'manual optimization.'
    bool found100 = false;

    // do horizontal scans until a valid line is found.
    int resultX = -1;
    while (resultX == -1) {
        doSliceCheck(dcs, lastStartingX, curX, curY, found100, resultX);

        // sanity checks
        if (curY % 100 == 0) {
            cout << curX << ", " << curY << (found100 ? " -> found100" : "") << endl;
        }

        curY += 1;
        curX = lastStartingX;
    }
    curY -= 1;

    cout << "result code = " << 10000 * resultX + (curY - 99)  << endl;
    cout << "double check: ";
    cout << dcs.SendDroneToCoordRaw(resultX, curY);
    cout << dcs.SendDroneToCoordRaw(resultX + 99, curY);
    cout << dcs.SendDroneToCoordRaw(resultX, curY - 99);
    cout << dcs.SendDroneToCoordRaw(resultX + 99, curY - 99) << endl;
}

// do a single, optimized scanline pass through a horizontal slice of the ray.
void doSliceCheck(DroneControlSystem &dcs, int &lastStartingX, int curX, int curY, bool &found100, int &resultX) {
    char lastReport = '.';
    while (true) {
        char report = dcs.SendDroneToCoordRaw(curX, curY);
        if (lastReport == '.' && report == '#') {
            lastStartingX = curX;
            break;
        }
        
        curX += 1;
        lastReport = report;   
    }

    // after we find a 100 sized scanline, we can assume every next scaline is >= 100.
    if (found100 == true) {
        if (doVerticalCheckFast(dcs, curX + 99, curY)) {
            resultX = curX; // return value
        }
        return;
    } else if (dcs.SendDroneToCoordRaw(curX + 99, curY) == '#') {
        found100 = true;
        if (doVerticalCheckFast(dcs, curX + 99, curY)) {
            resultX = curX; // return value
        }
        return;
    }
}

// x,y is the right most position of the scanline.
// returns if 100 size vertical scanline was found.
bool doVerticalCheck(DroneControlSystem &dcs, int curX, int curY) {
    int curRayWidth = 0;
    while (true) {
        char report = dcs.SendDroneToCoordRaw(curX, curY);
        if (report == '#') {
            curRayWidth += 1;
        } else {
            return false;
        }

        if (curRayWidth == 100) {
            return true;
        }
        
        curY -= 1;   
    }
}

// x,y is the right most position of the scanline.
// returns if 100 size vertical scanline was found.
bool doVerticalCheckFast(DroneControlSystem &dcs, int curX, int curY) {
    return dcs.SendDroneToCoordRaw(curX, curY - 99) == '#';
}