#ifndef REPAIR_DROID
#define REPAIR_DROID

#include "droidUtils.hpp"
#include "intcode.hpp"

#include <vector>
#include <list>
#include <unordered_map>
#include <string>
using namespace std;

// tile constants
const int TILE_WALL = 0;
const int TILE_EMPTY = 1;
const int TILE_GOAL = 2;

// ************************************************************************** //

class RepairDroid {
private:
    IntcodeComputer _intcomp;
    list<PathPoint2D*> _frontier;
    unordered_map<Point2D, char, hash_fn> _marked;
    unordered_map<Point2D, char, hash_fn> _values;

    bool _programLoaded;
    Point2D _goal;
    int _oxygenFillTime;

    void Reset();

    void AddNode(int x, int y, char action);
    void AddNode(int x, int y, const vector<char> &path, char action);
    void OxygenPush(list<Point2D> &front, Point2D pos, char value);

public:
    RepairDroid();
    void LoadProgram(string filename);

    void Activate();
    int GetMinPathLen();

    void SimulateOxygen();
    int GetFillTime();
};

#endif //!REPAIR_DROID