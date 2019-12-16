#include <iostream>

#include "day1.h"
#include "day2.h"
#include "day3.h"
#include "day5.h"

const int CURRENT_DAY = 5;

int main() { 
    switch (CURRENT_DAY) {
        case 1: day1::day1(); break;
        case 2: day2::day2(); break;
        case 3: day3::day3(); break;
        case 5: day5::day5(); break;
        default: std::cout << "bad number\n";
    }
    std::cout << "finished!\n";
}



