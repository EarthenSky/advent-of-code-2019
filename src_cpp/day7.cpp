#include <iostream>
#include <fstream>
#include <string>

#include <cmath>
#include <vector>

#include "day7.h"

namespace day7 {

    const int PHASE_START = 0;
    const int PHASE_END = 4;
    const int AMP_COUNT = 5;

    void part1();
    void part2();

    void day7() {
        part1();
        //part2();
        std::cout << "/* ending day7 */\n";
    }

    // For debugging
    void printVector(std::vector<int> const& input) {
        for(unsigned int i = 0; i < input.size(); i++) {
            std::cout << input[i] << ", ";
        }
        std::cout << "\n";
    }

    // instruction % (10^2 * 10^n) // n > 0
    // pos = 1 is 1st parameter's mode.
    int getParamMode(int pos, int instruction) {
        return (instruction % (100 * (int)pow(10, pos))) / (10 * (int)pow(10, pos));
    }

    int getValueAt(int index, int pos, int instruction, std::vector<int>& prog) {
        if(getParamMode(pos, instruction) == 1) {
            return prog[index + pos];  // immediate mode
        } else {
            return prog[prog[index + pos]];  // position mode
        }
    }
    
    int runOpCode(int index, std::vector<int>& prog, int input, int& output) {
        int instruction = prog[index];
        int opcode = instruction % 100;  // gets last two digits of number

        //std::cout << "instruction = " << instruction << "\n";
        //printVector(prog);

        switch (opcode) {
        case 1:
            prog[prog[index + 3]] = getValueAt(index, 1, instruction, prog) + getValueAt(index, 2, instruction, prog);
            return index + 4;
        case 2:
            prog[prog[index + 3]] = getValueAt(index, 1, instruction, prog) * getValueAt(index, 2, instruction, prog);
            return index + 4;
        case 5: // jump if true
            if( getValueAt(index, 1, instruction, prog) != 0) 
                return getValueAt(index, 2, instruction, prog);
            else 
                return index + 3;
        case 6: // jump if false
            if( getValueAt(index, 1, instruction, prog) == 0) 
                return getValueAt(index, 2, instruction, prog);
            else 
                return index + 3;
        case 7: // lt
            if( getValueAt(index, 1, instruction, prog) < getValueAt(index, 2, instruction, prog)) 
                prog[prog[index + 3]] = 1;
            else 
                prog[prog[index + 3]] = 0;
            return index + 4;
        case 8: // equal
            if( getValueAt(index, 1, instruction, prog) == getValueAt(index, 2, instruction, prog)) 
                prog[prog[index + 3]] = 1;
            else 
                prog[prog[index + 3]] = 0;
            return index + 4;
        case 4: // out
            output = getValueAt(index, 1, instruction, prog);
            return index + 2;
        case 3: // in
            //std::cout << "got input " << input << " | ";
            prog[prog[index + 1]] = input;
            return index + 2;
        case 99: // exit
            std::cout << "graceful exit\n";
            return -1;
        default:
            std::cout << "something went wrong, instruction=" << instruction << ", index=" << index << "\n";
            return -1;
        }
    }

    // run program on each amp (5x) and generate an output signal.
    int getOutputSignal(int phaseSetting[], std::vector<int>& prog, std::vector<int>& backup) {
        int input = 0, output;  // initial input is preset to 0
        for (int i = 0; i < AMP_COUNT; i++) {
            prog = backup;  // reset program before using
            int instruction = 0;

            // send first kind of input.
            instruction = runOpCode(instruction, prog, phaseSetting[i], output);
            
            // loop for second kind of input and for output.
            while(instruction != -1) {
                instruction = runOpCode(instruction, prog, input, output);
            }

            input = output;
        }

        return input;
    }

    void iSwap(int& a, int& b) {
        int c = a; a = b; b = c;
    }

    // this is a good place to use inline, even though the compiler will do it anyways.
    inline int iMax(int a, int b) {
        return a > b ? a : b;
    }

    // runs the program on all permutations of an array. (recursive)
    void runOnPermutations(int array[], int arrayTail[], int n, std::vector<int>& prog, 
                           std::vector<int>& backup, int& maxOutput) {
        for(int i = 0; i < n; i++) { 
            if (n == 1) {  // run program on this permutation
                int output = getOutputSignal(array, prog, backup);
                maxOutput = iMax(maxOutput, output);  // update max value.
            } else { // generate two more permutations
                iSwap(arrayTail[0], arrayTail[i]);
                runOnPermutations(array, arrayTail + 1, n - 1, prog, backup, maxOutput);
                iSwap(arrayTail[0], arrayTail[i]);
            }
        }
    }

    void part1() {
        std::cout << "/* starting day7 part1 */\n";

        std::vector<int> program_backup;
        std::vector<int> program;
        std::ifstream file ("resc/day7a.txt");
    
        // fill vector
        std::string str;
        while (std::getline(file, str, ',')) {
            program_backup.push_back(std::stoi(str));
        }

        std::cout << "run program:\n";

        int array[AMP_COUNT] = {0, 1, 2, 3, 4};  // possible amp values
        int maxOutputSignal = -1;
        runOnPermutations(array, array, AMP_COUNT, program, program_backup, maxOutputSignal);
        
        std::cout << "maxOutputSignal = " << maxOutputSignal << "\n";
    }

    // -------------------------------------------------------------------------------------------------------------- //

    //TODO: fix this to support fake concurrency.
    // run program on each amp (5x) and generate an output signal.
    int getOutputSignal(int phaseSetting[], std::vector<int>& prog, std::vector<int>& backup) {
        int instructions[5] = {0, 0, 0, 0, 0};

        int input = 0, output;  // initial input is preset to 0
        for (int i = 0; i < AMP_COUNT; i++) {
            prog = backup;  // reset program before using

            // send first kind of input.
            instructions[i] = runOpCode(instructions[i], prog, phaseSetting[i], output);
            
            // loop for second kind of input and for output.
            while(instructions[i] != -1) {
                instructions[i] = runOpCode(instructions[i], prog, input, output);
            }

            input = output;
        }

        return input;
    }

    // runs the program on all permutations of an array. (recursive)
    void runOnPermutations_v2(int array[], int arrayTail[], int n, std::vector<int>& prog, 
                           std::vector<int>& backup, int& maxOutput) {
        for(int i = 0; i < n; i++) { 
            if (n == 1) {  // run program on this permutation
                int output = getOutputSignal(array, prog, backup);
                maxOutput = iMax(maxOutput, output);  // update max value.
            } else { // generate two more permutations
                iSwap(arrayTail[0], arrayTail[i]);
                runOnPermutations(array, arrayTail + 1, n - 1, prog, backup, maxOutput);
                iSwap(arrayTail[0], arrayTail[i]);
            }
        }
    }

    void part2() {
        std::cout << "/* starting day7 part2 */\n";

        std::vector<int> program_backup;
        std::vector<int> program;
        std::ifstream file ("resc/day7a.txt");
    
        // fill vector
        std::string str;
        while (std::getline(file, str, ',')) {
            program_backup.push_back(std::stoi(str));
        }

        std::cout << "run program:\n";

        int array[AMP_COUNT] = {5, 6, 7, 8, 9};  // possible amp values
        int maxOutputSignal = -1;
        runOnPermutations(array, array, AMP_COUNT, program, program_backup, maxOutputSignal);
        
        std::cout << "maxOutputSignal = " << maxOutputSignal << "\n";
    }
}