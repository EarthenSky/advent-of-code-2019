#include <iostream>

#include "day9.h"

using namespace std;

int main() { 
    cout << "running:\n";
    
    IntcodeComputer ic = new IntcodeComputer(true);
    ic.LoadProgram("resc/input.txt");

    ic.EnqueInput(2);
    ic.RunProgram();

    cout << "BOOST keycode: \n";
    ic.PrintOutput();

    cout << "ending program\n";
    return 0;
}
