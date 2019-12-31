#include "intcode.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <cmath>

#include <vector>
#include <deque>
using namespace std;


#include <boost/multiprecision/cpp_int.hpp>  // uses boost's bigint
namespace mp = boost::multiprecision;
#include <boost/lexical_cast.hpp>  // for lexical_cast (stoi() alternative)
using boost::lexical_cast;


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
    int address = -1;
    if(parameterMode == 0) {
        address = (int) _program[index + pos];  // position mode
    } else if(parameterMode == 1) {
        address = index + pos;  // immediate mode
    } else if(parameterMode == 2) {
        address = _relativeBase + (int) _program[index + pos];  // relative mode
    } else {
        cout << "Error: invalid GetValueAt() parameter mode.\n";
    }

    // Check address value.
    if( address < (int) _program.size() && address >= 0 ) {
        return _program[ address ];
    } else if (address >= 0 && _extraMemory.find( address ) != _extraMemory.end()) {  // outside of program bounds.
        return _extraMemory[ address ];
    } else {
        cout << "invalid addr: " << address << "\n";
        return -1;  // returns garbage
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

    // Check address value.
    if( address < (int) _program.size() && address >= 0 ) {
        _program[ address ] = value;
    } else if (address >= 0) {  // outside of program bounds.
        _extraMemory[ (int) address ] = (mp::int128_t) value;
    } else {
        cout << "invalid addr: " << address << "\n";
    }
    
}

int IntcodeComputer::RunOpCode() {
    _currentInstruction = _program[(int)_instructionPointer];  // update current instruction
    //if(_isDebug) cout << "instruction: " << _currentInstruction << "\n";

    int opcode = (int)(_currentInstruction % (mp::int128_t)100);  // gets last two digits of number
    switch (opcode) {
    case 1: { // add
        mp::int128_t value = GetValueAt(1) + GetValueAt(2);
        SetValueAt(3, value);
        _instructionPointer += 4;
        return C_CONTINUE;

    } case 2: {  // mul
        mp::int128_t value = GetValueAt(1) * GetValueAt(2);
        //if(GetValueAt(1) > 100 || GetValueAt(2) > 100)
            //cout << GetValueAt(1)  << " | " << GetValueAt(2) << " | " << value << "\n";

        SetValueAt(3, value);
        _instructionPointer += 4;
        return C_CONTINUE;

    } case 3: { // in
        if (_inputQueue.empty()) {
            //if(_isDebug) cout << "requesting more input\n";
            return C_PAUSE;
        } else {
            mp::int128_t value = _inputQueue.front();
            _inputQueue.pop_front();
            SetValueAt(1, value);
            _instructionPointer += 2;
            return C_CONTINUE;
        }

    } case 4: { // out
        _outputQueue.push_back( GetValueAt(1) );
        _instructionPointer += 2;
        return C_CONTINUE;

    } case 5: { // jump if true
        if( GetValueAt(1) != 0) 
            _instructionPointer = (int) GetValueAt(2);
        else 
            _instructionPointer += 3;
        return C_CONTINUE;

    } case 6: { // jump if false
        if( GetValueAt(1) == 0) 
            _instructionPointer = (int) GetValueAt(2);
        else 
            _instructionPointer += 3;
        return C_CONTINUE;

    } case 7: { // lt
        mp::int128_t value = GetValueAt(1) < GetValueAt(2) ? 1 : 0;
        SetValueAt(3, value);
        _instructionPointer += 4;
        return C_CONTINUE;

    } case 8: {// equal
        mp::int128_t value = GetValueAt(1) == GetValueAt(2) ? 1 : 0;
        SetValueAt(3, value);
        _instructionPointer += 4;
        return C_CONTINUE;

    } case 9: { // adjust relative base
        _relativeBase += (int) GetValueAt(1);
        _instructionPointer += 2;
        return C_CONTINUE;

    } case 99: { // exit
        if(_isDebug) cout << "graceful exit\n";
        return C_END;

    } default: {
        cout << "something went wrong, _currentInstruction=" << _currentInstruction; 
        cout << ", _instructionPointer=" << _instructionPointer << "\n";
        return C_END;
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

void IntcodeComputer::PrintData(map<int, mp::int128_t> const& input) {
    map<int, mp::int128_t>::const_iterator it;
    for(it = input.begin(); it != input.end(); ++it) {
        cout << it->first << " -> " << it->second << ", ";
    }
    cout << "\n";
}


// ---------------- Public Methods ---------------- //


IntcodeComputer::IntcodeComputer(bool isDebug) {
    _isDebug = isDebug;
    _isReset = false;
}

// Can be called to re-init the program. (Resets the program)
void IntcodeComputer::LoadProgram(string filename) {
    // clear the program backup.
    //vector<mp::int128_t> empty;
    //swap( , empty );
    _programBackup.clear();
    
    // todo: check if a file exists at filename
    ifstream file(filename);
    
    // fill vector
    string str;
    while (getline(file, str, ',')) {
        _programBackup.push_back( lexical_cast<mp::int128_t>(str) );
    }

    file.close();  // .. did i not close file?
   
    ResetProgram();
}

int IntcodeComputer::RunProgram() {
    int exitCode = C_CONTINUE;

    // loop until program exits.
    while(exitCode == C_CONTINUE) {
        exitCode = RunOpCode();
    }

    _isReset = false;
    return exitCode;
}

// reseting must be done manually
void IntcodeComputer::ResetProgram() {
    _program = _programBackup;

    _relativeBase = 0;  // move _relativeBase to start as well.
    _instructionPointer = 0;  // move instruction pointer to start.
    
    // flush temp data
    _inputQueue.clear();
    _outputQueue.clear();
    _extraMemory.clear();

    _isReset = true;
}

void IntcodeComputer::EnqueInput(mp::int128_t val) { 
    _inputQueue.push_back(val); 
} 

mp::int128_t IntcodeComputer::DequeueOutput() { 
    mp::int128_t val = _outputQueue.front();
    _outputQueue.pop_front();  // remove from front
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

void IntcodeComputer::PrintExtraMemory() {
    PrintData(_extraMemory);
}
