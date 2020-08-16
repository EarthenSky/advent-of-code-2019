#include "droidUtils.hpp"
#include "repairDroid.hpp"
#include "intcode.hpp"

#include <iostream>
#include <vector>
#include <string>
using namespace std;


// ************************************************************************** //

void RepairDroid::Reset() {
    _intcomp.ResetProgram();
    _programLoaded = false;

    // free data
    for(PathPoint2D* dp : _frontier) 
        delete dp;
    _frontier.clear();
    _marked.clear();
}

// Only adds a node if it has not yet been marked.
void RepairDroid::AddNode(int x, int y, char action) {
    if (_marked.find(Point2D(x, y)) == _marked.end()) {
        _frontier.push_back( new PathPoint2D(x, y, action) );
        _marked[Point2D(x, y)] = inverse(action);
    }
}
void RepairDroid::AddNode(int x, int y, const vector<char> &path, char action) {
    if (_marked.find(Point2D(x, y)) == _marked.end()) {
        _frontier.push_back( new PathPoint2D(x, y, path, action) );
        _marked[Point2D(x, y)] = inverse(action);
    }
}  

// ************************************************************************** //

RepairDroid::RepairDroid() : _goal(Point2D(0, 0)) {
    Reset();
}

void RepairDroid::LoadProgram(string fileLocation) {
    _intcomp.LoadProgram(fileLocation);
    _programLoaded = true;
}

// This pathfinding is pretty much djikstra's algorithm I think.
void RepairDroid::Activate() {
    if (!_programLoaded) {
        cout << "error: no program loaded.\n";
        return;
    }

    // add root node
    _marked[Point2D(0, 0)] = 'c';  // c for center

    // add first four nodes to frontier & mark them
    AddNode(0, 1, 'u');
    AddNode(1, 0, 'r');
    AddNode(0, -1, 'd');
    AddNode(-1, 0, 'l');

    int x = 0;
    int y = 0;
    bool isReturning = false;
    list<char> currentPath;
    PathPoint2D* target = nullptr;

    while(_frontier.size() != 0 || isReturning == false) {
        char cur;
        if (currentPath.size() == 0) { // case: reached target node & going backwards
            char direction = _marked[Point2D(x, y)];
            if (direction == 'c') {
                // assign new path to node.
                if (target != nullptr) 
                    delete target;
                target = _frontier.front();
                _frontier.pop_front();

                currentPath.assign(target->path.begin(), target->path.end());
                cur = currentPath.front();
                currentPath.pop_front();
                isReturning = false; 
            } else {
                cur = direction;
            }
        } else { // case: going towards target node
            cur = currentPath.front();
            currentPath.pop_front();
        }
        
        // give program the input.
        _intcomp.EnqueInput( directionToInt(cur) );
        _intcomp.RunProgram();
        
        // get tile state if it is frontier.
        int out = (int) _intcomp.DequeueOutput();
        if (currentPath.size() == 0 && isReturning == false) {
            switch (out) {
                case TILE_WALL: { // case: failed to move last tile in map.
                    Point2D moved = directionToMove(cur);  // player moves
                    _values[Point2D(x + moved.x, y + moved.y)] = '#';
                    break; // player didn't move
                }
                case TILE_EMPTY: {
                    Point2D moved = directionToMove(cur);  // player moves
                    x += moved.x;
                    y += moved.y;
                    _values[Point2D(x, y)] = ' ';
                    break;
                }
                case TILE_GOAL: {
                    Point2D moved = directionToMove(cur);
                    _goal = Point2D(x + moved.x, y + moved.y);
                    _values[Point2D(x + moved.x, y + moved.y)] = 'G';
                    return;
                }
                default: {
                    cout << "Error: invalid program output.\n";
                    return;
                }
            }
        } else {
            Point2D moved = directionToMove(cur);
            x += moved.x;
            y += moved.y;
        }

        if (currentPath.size() == 0 && isReturning == false && out == TILE_EMPTY) {
            isReturning = true;
            AddNode(x, y + 1, target->path, 'u');
            AddNode(x + 1, y, target->path, 'r');
            AddNode(x, y - 1, target->path, 'd');
            AddNode(x - 1, y, target->path, 'l');
        }
    }

    cout << "checked_nodes\n";
    for (auto x : _marked) 
        cout << x.first.x << "," << x.first.y << " " << x.second << endl;
}

int RepairDroid::GetMinPathLen() {
    // print graphic:
    cout << "\npicture:\n";
    for(int y = 22; y > -22; y--) {
        for(int x = -30; x < 30; x++) {
            if (x == 0 && y == 0) {
                cout << "o";
            } else if (_values.find(Point2D(x, y)) == _marked.end()) {
                cout << ".";
            } else {
                cout << _values.find(Point2D(x, y))->second;
            } 
        }
        cout << "\n";
    }

    int pathSize = 0;
    Point2D currentPosition = _goal;
    while(true) {
        char action = _marked[currentPosition];

        if (action == 'c') {
            return pathSize;
        }
        Point2D offset = directionToMove(action);
        currentPosition.x += offset.x;
        currentPosition.y += offset.y;

        pathSize += 1;
    }
}

void RepairDroid::OxygenPush(list<Point2D> &front, Point2D pos, char value) {
    if ( _marked.find(pos) == _marked.end() && _values.find(pos) != _values.end() && 
         (_values.find(pos)->second == ' ' || _values.find(pos)->second == 'G') ) {
        _marked[pos] = value;
        front.push_back(pos);
    }
}

// Must activate droid before simulating oxygen
void RepairDroid::SimulateOxygen() {

    list<Point2D> oxygenFrontier;

    _marked.clear();
    OxygenPush(oxygenFrontier, _goal, 'O');

    _oxygenFillTime = 0;

    while (oxygenFrontier.size() != 0) {
        list<Point2D> newFrontier;
        for (Point2D pos : oxygenFrontier) {
            OxygenPush(newFrontier, Point2D(pos.x + 1, pos.y), 'O');
            OxygenPush(newFrontier, Point2D(pos.x, pos.y + 1), 'O');
            OxygenPush(newFrontier, Point2D(pos.x - 1, pos.y), 'O');
            OxygenPush(newFrontier, Point2D(pos.x, pos.y - 1), 'O');
        }

        // update true frontier with new frontier and forget all past nodes.
        oxygenFrontier.clear();
        oxygenFrontier = newFrontier;

        _oxygenFillTime += 1;
    }

    _oxygenFillTime -= 1;
}

int RepairDroid::GetFillTime() {
    return _oxygenFillTime;
}