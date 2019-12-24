#include "day9.h"

#include <iostream>
#include <fstream>
#include <string>

#include <cmath>
//#include <ctype>

#include <vector>
#include <deque>
using namespace std;


#include <boost/multiprecision/cpp_int.hpp>  // uses boost's bigint
namespace mp = boost::multiprecision;


const int INSTRUCTION_END = -1;

// ---------------- Private Methods ---------------- //

// instruction % (10^2 * 10^n) // n > 0
// pos = 1 is 1st parameter's mode.
int IntcodeComputer::GetParamMode(int pos) {
    return (int)(_currentInstruction % (100 * (int)pow(10, pos))) / (10 * (int)pow(10, pos));
}

// This function supports parameter modes.
mp::int128_t IntcodeComputer::GetValueAt(int pos) {
    int index = _instructionPointer;  // index in the program

    int parameterMode = GetParamMode(pos);
    if(parameterMode == 0)  {
        return _program[ (int)_program[index + pos] ];  // position mode
    } else if(parameterMode == 1) {
        return _program[ index + pos ];  // immediate mode
    } else if(parameterMode == 2) {
        return _program[ _relativeBase + (int)_program[index + pos] ];  // relative mode
    } else {
        cout << "Error: invalid GetValueAt() parameter mode.\n";
        return -1;  // -1 just means error.
    }
}

// This function supports parameter modes.
void IntcodeComputer::SetValueAt(int pos, mp::int128_t value) {
    int index = _instructionPointer;  // index in the program
    
    int parameterMode = GetParamMode(pos);
    int address = -1;
    if(parameterMode == 0) {
        address = (int)_program[index + pos]; // position mode
    } else if(parameterMode == 1) {
        cout << "SetValueAt() parameter mode Error: not allowed setting parameter mode 1!\n";
        //int address = index + pos;  // immediate mode
    } else if(parameterMode == 2) {
        address = _relativeBase + (int)_program[index + pos]; // relative mode
    } else {
        cout << "Error: invalid SetValueAt() parameter mode.\n";
    }

    //if( address >  )
        _program[ address ] = value;
}

void IntcodeComputer::RunOpCode() {
    _currentInstruction = _program[(int)_instructionPointer];  // update current instruction
    
    int opcode = (int)(_currentInstruction % (mp::int128_t)100);  // gets last two digits of number
    switch (opcode) {
    case 1: {
        mp::int128_t value = GetValueAt(1) + GetValueAt(2);
        SetValueAt(3, value);
        _instructionPointer += 4;
        return;

    } case 2: {
        mp::int128_t value = GetValueAt(1) * GetValueAt(2);
        SetValueAt(3, value);
        _instructionPointer += 4;
        return;

    } case 3: { // in
        mp::int128_t value = _inputQueue.front();
        _inputQueue.pop_front();
        SetValueAt(1, value);
        _instructionPointer += 2;
        return;

    } case 4: { // out
        _outputQueue.push_back( GetValueAt(1) );
        _instructionPointer += 2;
        return;

    } case 5: { // jump if true
        if( GetValueAt(1) != 0) 
            _instructionPointer = (int)GetValueAt(2);
        else 
            _instructionPointer += 3;
        return;

    } case 6: { // jump if false
        if( GetValueAt(1) == 0) 
            _instructionPointer = (int) GetValueAt(2);
        else 
            _instructionPointer += 3;
        return;

    } case 7: { // lt
        mp::int128_t value = GetValueAt(1) < GetValueAt(2) ? 1 : 0;
        SetValueAt(3, value);
        _instructionPointer += 4;
        return;

    } case 8: {// equal
        mp::int128_t value = GetValueAt(1) == GetValueAt(2) ? 1 : 0;
        SetValueAt(3, value);
        _instructionPointer += 4;
        return;

    } case 9: { // adjust relative base
        _relativeBase += (int) GetValueAt(1);
        _instructionPointer += 2;
        return;

    } case 99: { // exit
        if(_isDebug) cout << "graceful exit\n";
        _instructionPointer = -1;
        return;

    } default: {
        cout << "something went wrong, _currentInstruction=" << _currentInstruction; 
        cout << ", _instructionPointer=" << _instructionPointer << "\n";
        _instructionPointer = -1;
        return;
    }}
}

// Private printers
void IntcodeComputer::PrintData(deque<mp::int128_t> const& input) {
    for(unsigned int i = 0; i < input.size(); i++) {
        cout << input[i] << ", ";
    }
    cout << "\n";
}

void IntcodeComputer::PrintData(vector<mp::int128_t> const& input) {
    for(unsigned int i = 0; i < input.size(); i++) {
        cout << input[i] << ", ";
    }
    cout << "\n";
}

// ---------------- Public Methods ---------------- //

IntcodeComputer::IntcodeComputer(bool isDebug) {
    _isDebug = isDebug;
    _isReset = false;
}

// Can be called to re-init the program. (Flushes input and output)
void IntcodeComputer::LoadProgram(string filename) {
    // clear the program backup.
    vector<mp::int128_t> empty;
    swap( _programBackup, empty );

    ifstream file(filename);
    
    // fill vector
    string str;
    while (getline(file, str, ',')) {
        _programBackup.push_back( stoi(str) );
    }

    ResetProgram(true);
}

void IntcodeComputer::RunProgram() {
    if(!_isReset) ResetProgram(); 

    // loop until program exits. (will stop when iterations overflows ~a few million operations)
    int32_t iter = 0; 
    while(_instructionPointer != INSTRUCTION_END && iter >= 0) {
        RunOpCode();
    }

    if(iter < 0) cout << "Error: Program counter overflowed; likely an infinite loop\n";

    _isReset = false;
}

void IntcodeComputer::FlushInput() {
    deque<mp::int128_t> empty;
    swap( _inputQueue, empty );
}

void IntcodeComputer::FlushOutput() {
    deque<mp::int128_t> empty;
    swap( _outputQueue, empty );
}

void IntcodeComputer::ResetProgram(bool flushQueues /*=true*/) {
    _program = _programBackup;

    _relativeBase = 0;  // move _relativeBase to start as well.
    _instructionPointer = 0;  // move instruction pointer to start.
    
    FlushInput();
    FlushOutput();

    _isReset = true;
}

void IntcodeComputer::EnqueInput(mp::int128_t val) { 
    _inputQueue.push_back(val); 
} 

mp::int128_t IntcodeComputer::DequeueOutput() { 
    mp::int128_t val = _inputQueue.front();
    _inputQueue.pop_front();
    return val; 
}


// ---------------- Printing Methods (Public) ---------------- //

void IntcodeComputer::PrintInput() {
    PrintData(_inputQueue);
}

void IntcodeComputer::PrintOutput() {
    PrintData(_outputQueue);
}

void IntcodeComputer::PrintProgram(bool isBackup /*=false*/) {
    if(!isBackup)
        PrintData(_program);
    else
        PrintData(_programBackup);
}
