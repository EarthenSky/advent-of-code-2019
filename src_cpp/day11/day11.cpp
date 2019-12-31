#include <iostream>
#include <fstream>  // for file stream (file writing)
#include <vector>
using namespace std;

#include "intcode.hpp"

#include <boost/multiprecision/cpp_int.hpp>  // uses boost's bigint
namespace mp = boost::multiprecision;

// ----------------- Classes ------------------------------------------------ //

// information for a point and a value at that point
struct DataPoint2D {
    int x, y;
    int val;  // can only be 0 or 1.

    DataPoint2D(int x, int y, int val) {
        this->x = x;
        this->y = y;
        this->val = val;
    }

};

// these were private members of the class but error?
const int UP = 0;
const int RIGHT = 1;
const int DOWN = 2;
const int LEFT = 3;

class Robot {
private:
    int _uniquePanels;
    int _direction;

    int _x, _y;

    vector<DataPoint2D*> _steppedPanels;  // Panels that have been stepped on.

    void ControlRobot(int paint, int turn);  // todo: make private
    int GetCameraValue();  // gets the colour the camera sees.  // todo: also private
    
public:
    Robot();

    void RunRobot(string programLocation);
    void ResetRobot();
    int GetUniquePanels();
    void PaintToFile(string name);

};

// -------------------------------------------------------------------------- //

int main() {

    cout << "running:\n";
    Robot robot;
    robot.RunRobot("resc/input.txt");

    //cout << "Panels Painted At Least Once: " << robot.GetUniquePanels() << "\n";
    robot.PaintToFile("paint_out.txt");  // make file

    robot.ResetRobot();

    cout << "ending program\n";
    return 0;
}

// -------------------------------------------------------------------------- //

int Robot::GetCameraValue() {
    for(DataPoint2D* dp : _steppedPanels) {
        if(dp->x == _x && dp->y == _y)
            return dp->val;
    }

    return (_x == 0 && _y == 0) ? 1 : 0;
}

// paint is colour, turn is direction to turn. Robot also steps forwards in this function.
void Robot::ControlRobot(int paint, int turn) {
    // Paint tile if already have been there
    bool tilePainted = false;
    for(DataPoint2D* dp : _steppedPanels) {
        if(dp->x == _x && dp->y == _y) {
            dp->val = paint;
            tilePainted = true;
            break;
        }
    }
    
    // create new tile and paint it
    if(!tilePainted) {
        cout << "vars: " << _x << " " << _y << " " << paint << "\n";
        cout << _steppedPanels.size() << "\n";
        DataPoint2D* dp = new DataPoint2D(_x, _y, paint);
        //cout << "vars: " << _x << " " << _y << " " << paint << "\n";
        _steppedPanels.push_back(dp);
    }

    // turn robot
    if(turn == 0) {  // left
        _direction = (_direction == UP) ? LEFT : _direction - 1; 
    } else if(turn == 1) {  // right
        _direction = (_direction == LEFT) ? UP : _direction + 1; 
    }

    // step robot forwards once.
    switch(_direction) {
        case UP : _y--; break;  // why?
        case RIGHT : _x++; break;
        case DOWN : _y++; break;
        case LEFT : _x--; break;
    }
}

// -------------------------------------------------------------------------- //

Robot::Robot() {
    ResetRobot();
}

void Robot::RunRobot(string programLocation) {
    // load intcode
    IntcodeComputer ic;
    ic.LoadProgram(programLocation);

    int steps = 0;

    cout << "s\n";

    // loop on
    int exitCode = C_CONTINUE;
    while(exitCode != C_END) {
        // fill input
        ic.EnqueInput( (mp::int128_t)GetCameraValue() ); 
        
        // run intcode & get output 
        exitCode = ic.RunProgram();
        
        // change robot state
        int paint = (int) ic.DequeueOutput();
        int turn = (int) ic.DequeueOutput();
        ControlRobot(paint, turn);
        steps++;
    }

    cout << "steps: " << steps << "\n";

    // amount of tiles that have been affected.
    _uniquePanels = _steppedPanels.size();
}

void Robot::ResetRobot() {
    _uniquePanels = -1;
    _direction = UP;

    _x = 0; _y = 0;

    // free data
    for(DataPoint2D* dp : _steppedPanels) delete dp;
    _steppedPanels.clear();

    // add initial white panel
    //DataPoint2D* dp = new DataPoint2D(0, 0, 1);
    //_steppedPanels.push_back(dp);
}

int Robot::GetUniquePanels() {
    return _uniquePanels;
}

void Robot::PaintToFile(string name) {
    int maxX = -100;
    for(DataPoint2D* dp : _steppedPanels) {
        if(dp->x > maxX)
            maxX = dp->x;   
    }

    int minX = 100;
    for(DataPoint2D* dp : _steppedPanels) {
        if(dp->x < minX)
            minX = dp->x;   
    }

    int maxY = -100;
    for(DataPoint2D* dp : _steppedPanels) {
        if(dp->y > maxY)
            maxY = dp->y;   
    }

    int minY = 100;
    for(DataPoint2D* dp : _steppedPanels) {
        if(dp->y < minY)
            minY = dp->y;   
    }
    
    minY--;
    minX--;
    maxX++;
    maxY++;

    minY--;
    minX--;
    maxX++;
    maxY++;
    
    // create a zeroed out array
    int height = maxY - minY;
    int width = maxX - minX;
    int map[width][height] = {};
    //int** a = new int*[rowCount];
    //for(int i = 0; i < rowCount; ++i)
    //    a[i] = new int[colCount];

    // fill zeroed out array
    for(DataPoint2D* dp : _steppedPanels) {
        map[dp->x - minX][dp->y - minY] = dp->val + 1;  
    }

    map[_x - minX][_y - minY] = 3;  

    // Write map to a file
    ofstream outFile;
    outFile.open(name);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(map[x][y] == 1)
                outFile << ".";
            else if(map[x][y] == 2)
                outFile << "#";
            else if(map[x][y] == 0)
                outFile << ".";
            else if(map[x][y] == 3)
                outFile << ((_direction == UP) ? "^" : ((_direction == LEFT) ? "<" : ((_direction == DOWN) ? "v" : ">")));
        }
        outFile << "\n";
    }

    outFile << _steppedPanels.size() << "\n";
    outFile.close();

    // done 
}

// -------------------------------------------------------------------------- //