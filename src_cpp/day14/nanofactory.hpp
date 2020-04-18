#ifndef NANO_FACTORY
#define NANO_FACTORY

#include <vector>
#include <string>
using namespace std;

#include "reaction.hpp"


class NanoFactory {
private:
    // extraChemicals are the chemicals that reactions produce extras of. However we
    // sometimes must reuse these extras.
    vector<Chemical*> neededChemicals;
    vector<Chemical*> extraChemicals;  

    vector<Reaction*> reactions;
    bool reactionsLoaded;

    // Either appends, or fills existing if it is there.
    void AddToChemVec(vector<Chemical*> &chemVec, Chemical* chemical);

public:
    NanoFactory();
    ~NanoFactory();

    void LoadReactions(string filename);
    long CalculateOreFuelRatio();

    long ProduceFuel(long ore);

};

#endif //!NANO_FACTORY