#include <iostream>
#include <fstream>
#include <string>

#include <cmath>
#include <vector>

#include "day5.h"

namespace day5 {

    const int INPUT_VALUE = 1;
    const int INPUT_VALUE_pt2 = 5;

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

    void part1();
    void part2();

    int runOpCode(int index, std::vector<int>& prog) {
        int instruction = prog[index];
        int opcode = instruction % 100;  // gets last two digits of number

        if (opcode == 1) {
            prog[prog[index + 3]] = getValueAt(index, 1, instruction, prog) + getValueAt(index, 2, instruction, prog);
            return 4;
        } else if (opcode == 2) {
            prog[prog[index + 3]] = getValueAt(index, 1, instruction, prog) * getValueAt(index, 2, instruction, prog);
            return 4;
        } else if (opcode == 4) {
            std::cout << "output: " << getValueAt(index, 1, instruction, prog) << "\n";
            return 2;
        } else if (opcode == 3) {
            prog[prog[index + 1]]  = INPUT_VALUE;
            return 2;
        } else if (opcode == 99) {
            return -1;
        } else {
            std::cout << "something went wrong, program[opcode]=" << prog[opcode] << ", opcode=" << opcode << "\n";
            return -1;
        }
    }

    void day5() {
        //part1();
        part2();
        std::cout << "/* ending day5 */\n";
    }

    void part1() {
        std::cout << "/* starting day5 part1 */\n";

        std::vector<int> program;
        std::ifstream file ("resc/day5a.txt");
    
        // fill vector
        std::string str;
        while (std::getline(file, str, ',')) {
            program.push_back(std::stoi(str));
        }

        std::cout << "run program:\n";
        
        // run program
        int opcode = 0;
        int step;
        while(step != -1) {
            step = runOpCode(opcode, program);
            opcode += step;
        }
        
    }

    // -------------------------------------------------------------------------------------------------------------- //
    
    int runOpCode_improved(int index, std::vector<int>& prog) {
        int instruction = prog[index];
        int opcode = instruction % 100;  // gets last two digits of number

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
            std::cout << "output: " << getValueAt(index, 1, instruction, prog) << "\n";
            return index + 2;
        case 3: // in
            prog[prog[index + 1]] = INPUT_VALUE_pt2;
            return index + 2;
        case 99: // exit
            std::cout << "graceful exit\n";
            return -1;
        default:
            std::cout << "something went wrong, program[opcode]=" << prog[opcode] << ", opcode=" << opcode << "\n";
            return -1;
        }
    }

    void part2() {
        std::cout << "/* starting day5 part2 */\n";

        std::vector<int> program;
        std::ifstream file ("resc/day5a.txt");
    
        // fill vector
        std::string str;
        while (std::getline(file, str, ',')) {
            program.push_back(std::stoi(str));
        }

        std::cout << "run program:\n";
        
        // run program
        int instruction = 0;
        while(instruction != -1) {
            instruction = runOpCode_improved(instruction, program);
        }

    }
}