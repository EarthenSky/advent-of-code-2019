#ifndef DAY9
#define DAY9

#include <vector>
#include <deque>
#include <string>

//#include <ctype>
using namespace std;

  
#include <boost/multiprecision/cpp_int.hpp>  // uses boost's bigint
namespace mp = boost::multiprecision;


class IntcodeComputer {
private:
    //vector<mp::int128_t> _extraMemory;  // confused about this part?

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
    void RunOpCode();

    // Private printers
    void PrintData(deque<mp::int128_t> const& input);
    void PrintData(vector<mp::int128_t> const& input);

public:
    IntcodeComputer(bool isDebug=false);

    void LoadProgram(string filename);
    void RunProgram();
    
    void FlushInput();
    void FlushOutput();
    void ResetProgram(bool flushQueues=true);

    void EnqueInput(mp::int128_t val);
    mp::int128_t DequeueOutput();

    // Printing methods
    void PrintInput();
    void PrintOutput();
    void PrintProgram(bool isBackup=false);
};

// To implement:
// - more memory
// - big numbers

#endif //!DAY9