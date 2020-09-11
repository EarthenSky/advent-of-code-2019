#include <iostream>
#include <vector>
using namespace std;

template <class T>
void DisplayVecAsMat(vector<T> const &vec, unsigned int width) {
    unsigned int y = 0, x = 0;
    while (y*width + x < vec.size()) {
        cout << vec[y*width + x];
        x += 1;
        if (x == width) {
            y += 1;
            x = 0;
            cout << endl;
        }
    }
    cout << endl;
}