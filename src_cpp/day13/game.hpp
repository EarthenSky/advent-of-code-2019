#ifndef GAME
#define GAME

#include "intcode.hpp"

#include <vector>
#include <string>
using namespace std;

// tile constants
const int TILE_EMPTY = 0;
const int TILE_WALL = 1;
const int TILE_BLOCK = 2;
const int TILE_H_PADDLE = 3;
const int TILE_BALL = 4;

// information for a point and a value at that point
struct DataPoint2D {
    int x, y;
    int val;  // can be any tile constant

    DataPoint2D(int x, int y, int val) {
        this->x = x;
        this->y = y;
        this->val = val;
    }

};

class Game {
private:
    IntcodeComputer _intcomp;
    vector<DataPoint2D*> _tiles;
    vector<int> _simulatedActions;

    bool _programLoaded;
    bool _aiSimulated;
    
    void Reset();
    void UpdateScreen();
    void WriteStateToFile(string filename);

    int** CreateMap(int* h, int* w);
    void DeleteMap(int** map, int width);

    void LoadScreen();

public:
    int score;

    Game();

    void LoadProgram(string filename);
    void Run(bool insertQuarters=false, bool activateAI=true);
    int GetTileCount(int tile);

    void SimulateAi();

};

#endif //!INTCODE