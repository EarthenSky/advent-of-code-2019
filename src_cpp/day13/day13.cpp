#include <iostream>
#include <fstream>  // for file stream (file writing)
#include <vector>
using namespace std;

#include "intcode.hpp"
#include "game.hpp"


void part1();
void part2();

int main() {
    cout << "running:\n";

    //part1();
    part2();

    cout << "ending program\n";
    return 0;
}

void part1() {
    Game game;
    game.LoadProgram("resc/input.txt");

    game.Run(false, false);
    
    cout << "Total block tiles = " << game.GetTileCount(TILE_BLOCK) << "\n";
}

// todo: give a user interface.
void part2() {
    Game game;
    game.LoadProgram("resc/input.txt");

    game.SimulateAi();
    game.Run(true, true);
    
    //Todo: user input loop so game plays.
}

