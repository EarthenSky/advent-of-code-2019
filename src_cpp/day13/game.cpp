#include "game.hpp"
#include "intcode.hpp"

#include <iostream>
#include <fstream>  // for file stream (file writing)
#include <vector>
#include <string>
using namespace std;

#include <unistd.h>  // for usleep for visual appeal.


void Game::Reset() {
    _intcomp.ResetProgram();

    _programLoaded = false;
    score = 0;

    // free data
    for(DataPoint2D* dp : _tiles) delete dp;
    _tiles.clear();

    _simulatedActions.clear();
}

// this function will also update highscore variable.
void Game::UpdateScreen() {
    // load the screen state after program is run.
    int limit = _intcomp.GetOutputSize();
    for(int i = 0; i < limit; i += 3) {
        int x = (int) _intcomp.DequeueOutput();
        int y = (int) _intcomp.DequeueOutput();
        int val = (int) _intcomp.DequeueOutput();

        if(x == -1 && y == 0 && val > TILE_BALL) {  // case: update highscore.
            score = val;
        } else {
            //todo: this is likely very slow.
            bool occupied = false;
            for(DataPoint2D* dp : _tiles) {
                if(dp->x == x && dp->y == y) {
                    occupied = true;
                    dp->val = val;
                }
            }

            if(!occupied) _tiles.push_back( new DataPoint2D(x, y, val) );
        }
    }
}

void Game::WriteStateToFile(string filename) {
    int height, width;
    int** map = CreateMap(&height, &width);

    // Write map to a file
    ofstream outFile;
    outFile.open(filename);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(map[x][y] == TILE_EMPTY)
                outFile << ".";
            else if(map[x][y] == TILE_WALL)
                outFile << "W";
            else if(map[x][y] == TILE_BLOCK)
                outFile << "#";
            else if(map[x][y] == TILE_H_PADDLE)
                outFile << "P";
            else if(map[x][y] == TILE_BALL)
                outFile << "B";
            else 
                outFile << "?";
        }
        outFile << "\n";
    }

    outFile << "current score = " << score << "\n";
    outFile.close();

    DeleteMap(map, width);
}

// dynamically allocates _tiles as a "map" of tiles.
int** Game::CreateMap(int* h, int* w) {
    int maxX = -1000;
    for(DataPoint2D* dp : _tiles) {
        if(dp->x > maxX)
            maxX = dp->x;   
    }

    int minX = 1000;
    for(DataPoint2D* dp : _tiles) {
        if(dp->x < minX)
            minX = dp->x;   
    }

    int maxY = -1000;
    for(DataPoint2D* dp : _tiles) {
        if(dp->y > maxY)
            maxY = dp->y;   
    }

    int minY = 1000;
    for(DataPoint2D* dp : _tiles) {
        if(dp->y < minY)
            minY = dp->y;   
    }

    // I'm too lazy to fix this.
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

    int** map = new int*[width];
    for(int i = 0; i < width; ++i) {
        map[i] = new int[height];

        // zero out array
        for(int j = 0; j < height; ++j)
            map[i][j] = 0;

    }

    // fill zeroed out array
    for(DataPoint2D* dp : _tiles) {
        map[dp->x - minX][dp->y - minY] = dp->val;  
    }

    *h = height;
    *w = width; 

    return map;
}

// frees allocated map memory
void Game::DeleteMap(int** map, int width) {
    for(int i = 0; i < width; ++i)
        delete[] map[i];

    delete[] map;
}

// Similar affect to Run(false);  // part of the ai.
void Game::LoadScreen() {
    _intcomp.RunProgram();
    UpdateScreen();
    _intcomp.ResetProgram();
}

// -------------------------------------------------------------------------- //

Game::Game() {
    Reset();
}

void Game::LoadProgram(string fileLocation) {
    _intcomp.LoadProgram(fileLocation);
    _programLoaded = true;
}

void Game::Run(bool insertQuarters/*=false*/, bool activateAI/*=true*/) {
    if (!_programLoaded) {
        cout << "error: no program loaded.\n";
        return;
    }

    // Check parameters
    if(activateAI && !_aiSimulated)
        SimulateAi();

    if(insertQuarters)
        _intcomp.SetMemory(0, 2, false);

    // Each loop supplies one input to the intcode computer, and updates the screen once.
    // Thnk of this like an "update()" game loop.
    int frameNum = 0, maxFrames = _simulatedActions.size();
    int exitCode = C_CONTINUE;
    while(exitCode != C_END) {
        exitCode = _intcomp.RunProgram();

        // Check for the intcode computer failing.
        if(_intcomp.GetOutputSize() % 3 != 0) 
            cout << "Something is wrong.\n";

        // Updates the screen and overwrites the frame file (emulates a monitor)
        UpdateScreen();

        WriteStateToFile("out/frame.txt");

        int joystickState;
        if(activateAI) {  // Ask ai for joystick state.
            joystickState = _simulatedActions[frameNum];
            usleep(100 * 100 * 10);
            //cout << joystickState;
        } else { // Ask user for joystick state.
            cout << "Enter joystick position (2, 0, 1): ";
            cin >> joystickState;
            if(joystickState == 2) joystickState = -1; //shorthand for easier gameplay
        }

        _intcomp.EnqueInput(joystickState);
        
        frameNum++;
        if(activateAI && frameNum > maxFrames) cout << "aww, shit, simulation failed.\n";
    }

}

int Game::GetTileCount(int tile) {
    int counter = 0;
    for(DataPoint2D* dp : _tiles) {
        if(dp->val == tile) counter++;
    }

    return counter;
}

// Simulates the actions the ai must do.
void Game::SimulateAi() {
    if (!_programLoaded) {
        cout << "error: no program loaded.\n";
        return;
    }

    LoadScreen();  // this loads the initial map data (like part 1)
    int height, width;
    int** map = CreateMap(&height, &width);
    map[24][24] = TILE_EMPTY;  // paddle gets in the way of the simulations

    // debug this print map
    /*
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(map[x][y] == TILE_EMPTY)
                cout << ".";
            else if(map[x][y] == TILE_WALL)
                cout << "W";
            else if(map[x][y] == TILE_BLOCK)
                cout << "#";
            else if(map[x][y] == TILE_H_PADDLE)
                cout << "P";
            else if(map[x][y] == TILE_BALL)
                cout << "B";
            else 
                cout << "?";
        }
        cout << "\n";
    }
    */

    int ballDirection = 1;  // 1 is down-right
    int ballX = 22, ballY = 21;

    vector<int> _paddlePositions;

    // Simulate what would happen in the game by assuming that the paddle allways hits the ball.
    // I'm sick so I'm gonna do this the lazy way. --> it does work though.
    int maxFrames = (100 * 62);
    int frame = 0;
    while(frame < maxFrames) {
        bool freeSpaceForward = false;
        while(!freeSpaceForward) {
            if(ballDirection == 1) {  // downward direction
                if(map[ballX + 1][ballY] != TILE_EMPTY) {
                    ballDirection = 2;
                    if(map[ballX + 1][ballY] == TILE_BLOCK) map[ballX + 1][ballY] = TILE_EMPTY;
                } else if(map[ballX][ballY + 1] != TILE_EMPTY) {
                    ballDirection = 4;
                    if(map[ballX][ballY + 1] == TILE_BLOCK) map[ballX][ballY + 1] = TILE_EMPTY;
                } else if(map[ballX + 1][ballY + 1] != TILE_EMPTY) {
                    ballDirection = 3;
                    if(map[ballX + 1][ballY + 1] == TILE_BLOCK) map[ballX + 1][ballY + 1] = TILE_EMPTY;
                } else if(ballY + 1 == 24) {
                    ballDirection = 4;  // special case: assuming paddle hit
                    _paddlePositions.push_back(frame);  // frame # to be there at
                    _paddlePositions.push_back(ballX);  // this is telling the ai that it needs to be here.
                } else {
                    freeSpaceForward = true;
                }

            } else if(ballDirection == 2) {  // downward direction
                if(map[ballX - 1][ballY] != TILE_EMPTY) {
                    ballDirection = 1;
                    if(map[ballX - 1][ballY] == TILE_BLOCK) map[ballX - 1][ballY] = TILE_EMPTY;
                } else if(map[ballX][ballY + 1] != TILE_EMPTY) {
                    ballDirection = 3;
                    if(map[ballX][ballY + 1] == TILE_BLOCK) map[ballX][ballY + 1] = TILE_EMPTY;
                } else if(map[ballX - 1][ballY + 1] != TILE_EMPTY) {
                    ballDirection = 4;
                    if(map[ballX - 1][ballY + 1] == TILE_BLOCK) map[ballX - 1][ballY + 1] = TILE_EMPTY;
                } else if(ballY + 1 == 24) {
                    ballDirection = 3;  // special case: assuming paddle hit
                    _paddlePositions.push_back(frame);  // frame # to be there at
                    _paddlePositions.push_back(ballX); // this is telling the ai that it needs to be here.
                } else {
                    freeSpaceForward = true;
                }

            } else if(ballDirection == 3) {
                if(map[ballX - 1][ballY] != TILE_EMPTY) {
                    ballDirection = 4;
                    if(map[ballX - 1][ballY] == TILE_BLOCK) map[ballX - 1][ballY] = TILE_EMPTY;
                } else if(map[ballX][ballY - 1] != TILE_EMPTY) {
                    ballDirection = 2;
                    if(map[ballX][ballY - 1] == TILE_BLOCK) map[ballX][ballY - 1] = TILE_EMPTY;
                } else if(map[ballX - 1][ballY - 1] != TILE_EMPTY) {
                    ballDirection = 1;
                    if(map[ballX - 1][ballY - 1] == TILE_BLOCK) map[ballX - 1][ballY - 1] = TILE_EMPTY;
                } else {
                    freeSpaceForward = true;
                }

            } else if(ballDirection == 4) {
                if(map[ballX + 1][ballY] != TILE_EMPTY) {
                    ballDirection = 3;
                    if(map[ballX + 1][ballY] == TILE_BLOCK) map[ballX + 1][ballY] = TILE_EMPTY;
                } else if(map[ballX][ballY - 1] != TILE_EMPTY) {
                    ballDirection = 1;
                    if(map[ballX][ballY - 1] == TILE_BLOCK) map[ballX][ballY - 1] = TILE_EMPTY;
                } else if(map[ballX + 1][ballY - 1] != TILE_EMPTY) {
                    ballDirection = 2;
                    if(map[ballX + 1][ballY - 1] == TILE_BLOCK) map[ballX + 1][ballY - 1] = TILE_EMPTY;
                } else {
                    freeSpaceForward = true;
                }

            }
        }

        // move the ball forwards
        map[ballX][ballY] = TILE_EMPTY;
        if(ballDirection == 1) {
            ballX++; ballY++;
        } else if(ballDirection == 2) {
            ballX--; ballY++;
        } else if(ballDirection == 3) {
            ballX--; ballY--;
        } else if(ballDirection == 4) {
            ballX++; ballY--;
        }

        map[ballX][ballY] = TILE_BALL;

        frame++;
    }

    // Then make actions to string together each point that the ball hits the paddle line, and make sure that the
    // paddle moves to that position before the ball gets there.
    frame = 0;
    int i = 0, savedFrame = 0, xPos = 24, targetX;
    while(frame < maxFrames) {
        // Update Ai Target position.
        if(frame > savedFrame) {
            if (i > (int) _paddlePositions.size()) {
                savedFrame = -1;
            } else {
                //cout << "# " << _paddlePositions[i] << "\n";
                savedFrame = _paddlePositions[i];
                targetX = _paddlePositions[i + 1];
                i += 2;
            }
        }

        // move towards target & determine joystick movement
        int joyState = 0;
        if(xPos > targetX) {
            joyState = -1;
        } else if(xPos < targetX) {
            joyState = 1;
        }        

        // Fill the _simulatedActions;
        _simulatedActions.push_back(joyState);
        xPos += joyState;

        frame++;
    }
    
    _paddlePositions.clear();

    DeleteMap(map, width);
    _aiSimulated = true;
}