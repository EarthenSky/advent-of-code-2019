#include <iostream>
#include <vector>
using namespace std;
  
#include <boost/multiprecision/cpp_int.hpp>  // uses boost's bigint
namespace mp = boost::multiprecision;


int main() { 
    cout << "running:\n";
    
    vector<mp::int128_t> p;
    p.push_back(11);
    p.push_back((mp::int128_t)100000 * 100000 * 100000 * 100000);
    cout << p[1] << " == 100000000000000000000\n";
    p.pop_back();

    cout << "ending program\n";
    return 0;
}