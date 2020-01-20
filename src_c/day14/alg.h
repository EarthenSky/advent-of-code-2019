#ifndef ALG
#define ALG

#include <stdlib.h>

// returns the integer division n / m, but rounded up.
// internally does no floating point calculations.
int ceil_div(int n, int m) {
    return (n / m) + ((n % m == 0) ? 0 : 1);
}

int round_up_to_multiple(int number, int multiple) {
    return number += (multiple - (number % multiple));
}

int gcd(int a, int b) { 
    if (a == 0) return b;  
    return gcd(b % a, a);  
} 
  
// Function to return LCM of two numbers  
int lcm(int a, int b) {  
    return (a * b) / gcd(a, b);  
}

/*
// returns the integer division n / m, but rounded up.
// internally does no floating point calculations.
inline int ceil_div(int n, int m) {
    div_t result;
    result = div(n, m);
    return result.quot + (result.rem == 0) ? 0 : 1;
}
*/

#endif //!ALG