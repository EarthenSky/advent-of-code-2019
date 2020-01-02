#include <iostream>
#include <fstream>  // for file stream (file writing)
#include <vector>
using namespace std;

#include "intcode.hpp"

//#include <boost/multiprecision/cpp_int.hpp>  // uses boost's bigint
//namespace mp = boost::multiprecision;

int main() {
    cout << "running:\n";
    
    Robot robot;
    robot.RunRobot("resc/input.txt");

    robot.PaintToFile("paint_out.txt");  // make file

    cout << "ending program\n";
    return 0;
}

// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //