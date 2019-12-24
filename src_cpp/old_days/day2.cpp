#include <iostream>
#include <fstream>
#include <string>

#include <cmath>
#include <vector>

#include "day2.h"

namespace day2 {
    int runOpCode(int opcode, std::vector<int>& program) {
        if (program[opcode] == 1) {
            program[program[opcode + 3]] = program[program[opcode + 1]] + program[program[opcode + 2]];
            return 0;
        } else if (program[opcode] == 2) {
            program[program[opcode + 3]] = program[program[opcode + 1]] * program[program[opcode + 2]];
            return 0;
        } else if (program[opcode] == 99) {
            return 1;
        } else {
            std::cout << "something went wrong, program[opcode]=" << program[opcode] << ", opcode=" << opcode << "\n";
            return 1;
        }
    }

    void part1();
    void part2();

    void day2() {
        //part1();
        part2();
        std::cout << "/* ending day2 */\n";
    }

    void part1() {
        std::cout << "/* starting day2 part1 */\n";

        std::vector<int> program;
        std::ifstream file ("resc/day2a.txt");
    
        // fill vector
        std::string str;
        while (std::getline(file, str, ',')) {
            program.push_back(std::stoi(str));
        }

        // run program
        int opcode = 0;
        for(;;) {
            if (runOpCode(opcode, program)) break;
            opcode += 4;
        }
        
        // print out first number value
        std::cout << program[0] << "\n";
        
        /*
        // print out finished program
        for(int n : program) {
            std::cout << n << ",";
        }
        std::cout << "\b\b\n";
        */
    }

    void part2() {
        std::cout << "/* starting day2 part2 */\n";

        std::vector<int> program_back;
        std::vector<int> program;
        std::ifstream file ("resc/day2a.txt");
        
        // fill vector
        std::string str;
        while (std::getline(file, str, ',')) {
            program_back.push_back(std::stoi(str));
        }

        for(int i = 0; i < 100; i++) {
            for(int j = 0; j < 100; j++) {
                program = program_back;  // reset program
                program[1] = i;
                program[2] = j;

                // run program
                int opcode = 0;
                for(;;) {
                    if (runOpCode(opcode, program)) break;
                    opcode += 4;
                }

                if (program[0] == 19690720) {
                    std::cout << "i: " << i << ", j: " << j << "\n";
                    goto done;
                }
            }
        }    
        
        done:
        // print out first number value
        std::cout << program[0] << "\n";

    }
}