#ifndef DRONE_CONTROL_SYSTEM
#define DRONE_CONTROL_SYSTEM

#include <vector>
#include <string>
using namespace std;

#include <stdlib.h>

#include "intcode.hpp"

// kinds of output
enum DeployReport {
    Stationary  = 0,
    Pulled      = 1,
    NoReport    = 2
};

class DroneControlSystem {
private:
    bool _isLoaded;
    IntcodeComputer _ic;

public:
    DroneControlSystem();

    void LoadControlProgram(string filename);
    DeployReport SendDroneToCoord(int x, int y);
};

#endif //!DRONE_CONTROL_SYSTEM