#include "droneControlSystem.hpp"

DroneControlSystem::DroneControlSystem() {
    _isLoaded = false;
}

void DroneControlSystem::LoadControlProgram(string filename) {
    _ic.LoadProgram(filename);
    _isLoaded = true;
}

DeployReport DroneControlSystem::SendDroneToCoord(int x, int y) {
    if (_isLoaded == false) {
        cout << "ERROR: program must first be loaded" << endl;
        exit(1);
    }

    _ic.ResetProgram();
    _ic.EnqueInput(x);
    _ic.EnqueInput(y);
    _ic.RunProgram();

    int out = (int) _ic.DequeueOutput();
    switch (out) {
        case 0:
            return Stationary;
        case 1:
            return Pulled;
        default:
            return NoReport;
    }
}