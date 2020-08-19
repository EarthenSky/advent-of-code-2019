#include <iostream>
#include <vector>
using namespace std;

#include "intcode.hpp"

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

vector<char> getPathFromMap(vector<char> &map);

void part2() {
    cout << "-- Part 2 --\n";

    // load map into memory
    IntcodeComputer intcomp;
    intcomp.LoadProgram("resc/input.txt");
    intcomp.RunProgram();

    vector<char> map;
    while (intcomp.GetOutputSize() != 0) {
        char out = (char) intcomp.DequeueOutput();
        map.push_back(out);
    }

    // generate path
    vector<char> path = getPathFromMap(map);
    for (char ch : path) {
        cout << ch;
    }

    // I had this whole algorithm designed for solving the problem, but then i found the solution manually using 
    // vscode's variable duplicate checker. T-T

    string main = "A,B,A,C,A,B,C,B,C,B\n";
    string funcA = "L,10,R,8,L,6,R,6\n";
    string funcB = "L,8,L,8,R,8\n";
    string funcC = "R,8,L,6,L,10,L,10\n";
    string video = "n\n";
    
    // Run actual program.
    intcomp.LoadProgram("resc/wake_up_input.txt");
    
    // give input to program in stages.
    intcomp.RunProgram();
    while (intcomp.GetOutputSize() != 0) {
        cout << (char) intcomp.DequeueOutput();
    }
    for (char ch : main) {
        cout << ch;
        intcomp.EnqueInput((int) ch);
    }

    intcomp.RunProgram();
    while (intcomp.GetOutputSize() != 0) {
        cout << (char) intcomp.DequeueOutput();
    }
    for (char ch : funcA) {
        cout << ch;
        intcomp.EnqueInput((int) ch);
    }

    intcomp.RunProgram();
    while (intcomp.GetOutputSize() != 0) {
        cout << (char) intcomp.DequeueOutput();
    }
    for (char ch : funcB) {
        cout << ch;
        intcomp.EnqueInput((int) ch);
    }

    intcomp.RunProgram();
    while (intcomp.GetOutputSize() != 0) {
        cout << (char) intcomp.DequeueOutput();
    }
    for (char ch : funcC) {
        cout << ch;
        intcomp.EnqueInput((int) ch);
    }

    intcomp.RunProgram();
    while (intcomp.GetOutputSize() != 0) {
        cout << (char) intcomp.DequeueOutput();
    }
    for (char ch : video) {
        cout << ch;
        intcomp.EnqueInput((int) ch);
    }

    // get dust
    intcomp.RunProgram();
    while (intcomp.GetOutputSize() != 1) {
        cout << (char) intcomp.DequeueOutput();
    }
    cout << "dust number: " << intcomp.DequeueOutput() << endl;
}

// L,10,R,8,L,6,R,6,L,8,L,8,R,8,L,10,R,8,L,6,R,6,R,8,L,6,L,10,L,10,L,10,R,8,L,6,R,6,L,8,L,8,R,8,R,8,L,6,L,10,L,10,L,8,L,8,R,8,R,8,L,6,L,10,L,10,L,8,L,8,R,8

// This function assumes that map is well formed:
//   - starts from a position which is only adjacent to one scaffold.
//   - scaffolds only branch in one direction, and never touch on parallels.
// Also, the function assumes in the puzzle, the player is facing up and direction starts as 'E'.
vector<char> getPathFromMap(vector<char> &map) {
    // calc width & height
    int width = 0;
    while (map[width] != '\n') { width++; }
    int trueWidth = width + 1;  // includes newline
    int height = (int) map.size() / trueWidth;

    int robotIndex = 0;
    while (map[robotIndex] == '.' || map[robotIndex] == '#' || map[robotIndex] == '\n') { 
        robotIndex++;
    }

    int x = robotIndex % trueWidth;
    int y = robotIndex / trueWidth;

    // Determine starting direction
    char direction;
    if (x != trueWidth-1 && map[y * trueWidth + (x+1)] == '#') {
        direction = 'E';
    } else if (x != 0 && map[y * trueWidth + (x-1)] == '#') {
        direction = 'W';
    } else if (y != height-1 && map[(y+1) * trueWidth + x] == '#') {
        direction = 'S';
    } else if (y != 0 && map[(y-1) * trueWidth + x] == '#') {
        direction = 'N';
    }

    vector<char> path;

    // This is inferred manually from seeing the puzzle.
    path.push_back('L');
    path.push_back(',');

    int moveLen = 0;
    bool exit = false;
    while (!exit) {
        bool east = x != trueWidth-1 && map[y * trueWidth + (x+1)] == '#';
        bool west = x != 0 && map[y * trueWidth + (x-1)] == '#';
        bool south = y != height-1 && map[(y+1) * trueWidth + x] == '#';
        bool north = y != 0 && map[(y-1) * trueWidth + x] == '#';

        /*
        cout << direction << "\n";
        cout << x << "," << y << "\n";
        cout << east << west << south << north << "\n";
        */
        
        if((direction == 'E' && east) || (direction == 'W' && west) || 
           (direction == 'S' && south) || (direction == 'N' && north)) {
            moveLen += 1;
            switch (direction) { // move robot position
                case 'E': x += 1; break;
                case 'W': x -= 1; break;
                case 'S': y += 1; break;
                case 'N': y -= 1; break;
            }
        } else { // case: need to do a turn.
            // apply move.
            for (char ch : to_string(moveLen)) {
                path.push_back(ch);
            }
            path.push_back(',');
            moveLen = 0;

            // check rotate direction.
            if((direction == 'E' && south) || (direction == 'S' && west) || 
               (direction == 'W' && north) || (direction == 'N' && east)) {
                path.push_back('R');
                path.push_back(',');
                switch (direction) { // rotate robot direction
                    case 'E': direction = 'S'; break;
                    case 'W': direction = 'N'; break;
                    case 'S': direction = 'W'; break;
                    case 'N': direction = 'E'; break;
                }
            } else if((direction == 'E' && north) || (direction == 'S' && east) || 
                      (direction == 'W' && south) || (direction == 'N' && west)) {
                path.push_back('L');
                path.push_back(',');
                switch (direction) { // rotate robot direction
                    case 'E': direction = 'N'; break;
                    case 'W': direction = 'S'; break;
                    case 'S': direction = 'E'; break;
                    case 'N': direction = 'W'; break;
                }
            } else { // case: we hit the end
                exit = true;
            }
        }
    }
    
    path.pop_back();  // rem last comma
    path.push_back('\n');
    return path;
}
