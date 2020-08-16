#include "droidUtils.hpp"

char inverse(char action) {
    switch (action) {
        case 'u': return 'd';
        case 'r': return 'l';
        case 'd': return 'u';
        case 'l': return 'r';
        default: {
            cout << "inverse direction conversion error.\n";
            return '\0';
        }
    }
}

int directionToInt(char action) {
    switch (action) {
        case 'u': return 1;
        case 'r': return 4;
        case 'd': return 2;
        case 'l': return 3;
        default: {
            cout << "invalid direction character #: " << (int) action << "\n";
            return -1;
        }
    }
}

Point2D directionToMove(char action) {
    Point2D p = Point2D(0, 0);
    switch (action) {
        case 'u': {
            p.x = 0;
            p.y = 1;
            break;
        }
        case 'r': {
            p.x = 1;
            p.y = 0;
            break;
        }
        case 'd': {
            p.x = 0;
            p.y = -1;
            break;
        }
        case 'l': {
            p.x = -1;
            p.y = 0;
            break;
        }
        default: {
            cout << "invalid direction.\n";
        }
    }
    return p; 
}