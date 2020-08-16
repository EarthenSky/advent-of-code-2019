#ifndef DROID_UTILS
#define DROID_UTILS

#include <iostream>
#include <vector>
#include <string>
using namespace std;

char inverse(char action);
int directionToInt(char action);

// ************************************************************************** //

// information for a point and a value at that point
struct PathPoint2D {
    int x, y;
    vector<char> path;
    char direction;

    PathPoint2D(int x, int y, const vector<char> &path, char action) {
        this->x = x;
        this->y = y;
        this->path.assign(path.begin(), path.end());
        this->path.push_back(action);
        this->direction = inverse(action);
    }

    PathPoint2D(int x, int y, char action) {
        this->x = x;
        this->y = y;
        this->path.push_back(action);
        this->direction = inverse(action);
    }
};

struct Point2D {
    int x, y;

    Point2D(int x, int y) {
        this->x = x;
        this->y = y;
    }
    
    bool operator==(const Point2D &p) const {
        return this->x == p.x && this->y == p.y;
    }
};

struct hash_fn {
    size_t operator() (const Point2D &p) const {
        size_t h1 = hash<int>()(p.x);
        size_t h2 = hash<int>()(p.y);
        return h1 ^ h2;
    }
};

// ************************************************************************** //

Point2D directionToMove(char action);

#endif //!DROID_UTILS