#ifndef INTCODE
#define INTCODE

#include <vector>
#include <deque>
#include <map>  // this structure is cool
#include <string>
using namespace std;

#include <boost/multiprecision/cpp_int.hpp>  // uses boost's bigint
namespace mp = boost::multiprecision;


// exit codes
const int C_END = 0;
const int C_CONTINUE = 1;
const int C_PAUSE = 2;


class IntcodeComputer {
private:
    map<int, mp::int128_t> _extraMemory;

    vector<mp::int128_t> _programBackup;
    vector<mp::int128_t> _program;
    bool _isDebug;

    deque<mp::int128_t> _inputQueue;
    deque<mp::int128_t> _outputQueue;

    // These parameters don't support concurency.
    // (To implement concurency, I will need to make some kind of controller class)
    bool _isReset;
    int _relativeBase;
    int _instructionPointer;
    mp::int128_t _currentInstruction;

    int GetParamMode(int pos);
    mp::int128_t GetValueAt(int pos);
    void SetValueAt(int pos, mp::int128_t value);
    int RunOpCode();

    // Private printers
    void PrintData(deque<mp::int128_t> const& input);
    void PrintData(vector<mp::int128_t> const& input);
    void PrintData(map<int, mp::int128_t> const& input);

public:
    IntcodeComputer(bool isDebug=false);

    void LoadProgram(string filename);
    int RunProgram();  // returns exit code. (exit or pause)
    
    void ResetProgram();

    void EnqueInput(mp::int128_t val);
    mp::int128_t DequeueOutput();

    // Printing methods
    void PrintInput();
    void PrintOutput();
    void PrintProgram(bool isBackup=false);
    void PrintExtraMemory();
};

#endif //!INTCODE