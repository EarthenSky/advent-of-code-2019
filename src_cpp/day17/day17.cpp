#include <iostream>
#include <vector>
using namespace std;

#include "intcode.hpp"

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
    
    IntcodeComputer intcomp;
    intcomp.LoadProgram("resc/input.txt");
    intcomp.RunProgram();

    vector<char> map;
    while (intcomp.GetOutputSize() != 0) {
        char out = (char) intcomp.DequeueOutput();
        map.push_back(out);
    }

    // calc width
    int width = 0;
    while (map[width] != '\n') { width++; }
    int trueWidth = width + 1;

    // get sum
    int alignmentParamSum = 0;
    for (int i = 0; i < (int) map.size(); i++) {
        int x = i % trueWidth;
        int y = i / trueWidth;
        
        // case: tile is on edge
        if (x == 0 || x == trueWidth-1 || y == 0 || y == ((int)map.size()/trueWidth)-1) {
            cout << map[y * trueWidth + x];
            continue;
        }

        // case: tile is intersection
        if(map[y * trueWidth + x] == '#' && 
           map[y * trueWidth + (x + 1)] == '#' && map[(y + 1) * trueWidth + x] == '#' &&
           map[y * trueWidth + (x - 1)] == '#' && map[(y - 1) * trueWidth + x] == '#') {
            alignmentParamSum += x * y;
            cout << 'O';
        } else {
            cout << map[y * trueWidth + x];
        }
    }

    cout << "Alignment Param Sum: " << alignmentParamSum << endl;

}

void part2() {
    cout << "-- Part 2 --\n";
}

