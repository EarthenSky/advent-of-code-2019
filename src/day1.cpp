#include <iostream>
//#include <sstream>
#include <fstream>
#include <cmath>

#include "day1.h"

namespace day1 {

    void part1();
    void part2();

    void day1() {
        part1();
        part2();
        std::cout << "/* ending day1 */\n";
    }

    void part1() {
        std::cout << "/* starting day1 part 1 */\n";
        int fuelSum = 0;
        std::ifstream file ("resc/day1a.txt");
    
        int m;
        while (file >> m) {
            fuelSum += floor(m/3)-2;
        }

        std::cout << "total mass: " << fuelSum << '\n';
    }

    // recursive... yeah.
    int recursiveGetFuel(int mass) {
        int fuel = floor(mass / 3) - 2;
        if (fuel > 0) 
            fuel += recursiveGetFuel(fuel);
        else
            fuel = 0;  // wishing real hard?
        return fuel;
    }

    void part2() {
        std::cout << "/* starting day1 part 2 */\n";
        int fuelSum = 0;
        std::ifstream file ("resc/day1a.txt");
    
        int m;
        while (file >> m) {
            fuelSum += recursiveGetFuel(m);
        }

        std::cout << "total mass (fuel included): " << fuelSum << '\n';
    }
}