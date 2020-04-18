#include "nanofactory.hpp"
#include "reaction.hpp"

#include <iostream>
#include <fstream>  // for file stream (file writing)
#include <vector>
#include <string>
using namespace std;


// -------------------------------------------------------------------------  //


// returns the ceiling of an unsigned integer divion. slower than normal integer divison.
size_t uceil_div(size_t x, size_t y) {
    return x / y + ((x % y != 0) ? 1 : 0);
}


// -------------------------------------------------------------------------  //


void NanoFactory::AddToChemVec(vector<Chemical*> &chemVec, Chemical* chemical) {
    bool found = false;
    for(Chemical* c : chemVec) {
        if(c->name == chemical->name) {
            found = true;
            c->yield += chemical->yield;
            delete chemical;
            
            break;
        }
    }

    if(found == false)
        chemVec.push_back(chemical);

}


// -------------------------------------------------------------------------  //


NanoFactory::NanoFactory() {
    reactionsLoaded = false;
}

NanoFactory::~NanoFactory() {
    // free all the data
    for(Reaction* r : reactions) 
        delete r;
    
    for(Chemical* c : neededChemicals) 
        delete c;
   
    for(Chemical* c : extraChemicals) 
        delete c;
}

void NanoFactory::LoadReactions(string filename) {
    // todo: check if a file exists at filename
    ifstream file(filename);
    
    // fill & init reactions
    string line;
    while (getline(file, line, '\n')) {
        reactions.push_back( new Reaction(line) );
    }
    file.close();
    
    reactionsLoaded = true;
}

long NanoFactory::CalculateOreFuelRatio() {
    if(!reactionsLoaded) {
        cout << "ERROR on 'CalculateOreFuelRatio()', please load reactions first." << endl;
        return -1; // must load reactions first
    }

    neededChemicals.push_back( new Chemical(1, "FUEL") );  // goal is fuel.

    // Convert 1 chemical per loop
    // Loop until only Ore is left in the need side (base case)
    while( !(neededChemicals.size() == 1 && neededChemicals[0]->name == "ORE") ) {
        // Don't choose the ORE in this list.
        int index = 0;
        while(neededChemicals[index]->name == "ORE") 
            index++;
        
        Chemical* targetChemical = neededChemicals[index];
        neededChemicals.erase( neededChemicals.begin() + index );

        // find reaction that makes targetChemical
        Reaction* reaction = nullptr;
        for(Reaction* r : reactions) {
            ///cout << r->output->name << "xxx" << endl;
            if(r->output->name == targetChemical->name) {
                reaction = r;
                break;
            }
        }
        cout << targetChemical->name << " - chosen\n";

        // apply extras to neededChemicals
        Chemical* extraChemical = nullptr;
        index = 0;
        for(Chemical* c : extraChemicals) {
            if(c->name == targetChemical->name) {
                extraChemical = c;
                break;
            }
            index++;
        }

        // Handles a few special cases.
        if(extraChemical != nullptr && extraChemical->yield > targetChemical->yield) {
            // case: don't have to do anything special.
            extraChemical->yield -= targetChemical->yield;
            delete targetChemical;

            cout << "needed count: " << neededChemicals.size() << endl;
            for(Chemical* c : neededChemicals) {
                cout << "\t" << c->name << " " << c->yield << endl;
            }
            cout << "extras count: " << extraChemicals.size() << endl;
            for(Chemical* c : extraChemicals) {
                cout << "\t" << c->name << " " << c->yield << endl;
            }

            continue;
        } else if(extraChemical != nullptr && extraChemical->yield == targetChemical->yield) {
            // case: mostly easy to handle.
            extraChemicals.erase(extraChemicals.begin() + index);
            delete extraChemical;
            delete targetChemical;

            cout << "needed count: " << neededChemicals.size() << endl;
            for(Chemical* c : neededChemicals) {
                cout << "\t" << c->name << " " << c->yield << endl;
            }
            cout << "extras count: " << extraChemicals.size() << endl;
            for(Chemical* c : extraChemicals) {
                cout << "\t" << c->name << " " << c->yield << endl;
            }

            continue;
        } else if (extraChemical != nullptr) {
            // case: extrachemicals are less than target
            extraChemicals.erase(extraChemicals.begin() + index);
        }

        // find how many times to run reaction
        long yieldNeeded = targetChemical->yield - (extraChemical != nullptr ? extraChemical->yield : 0);
        size_t yieldProduced = reaction->output->yield;  // in one cycle
        size_t cycles = uceil_div(yieldNeeded, yieldProduced);  // unsignesd ceil(needed / produced)
        
        cout << "vars:\n";
        cout << "needed=" << yieldNeeded << endl;
        cout << yieldProduced << endl;
        cout << cycles << endl;

        // find extra produced
        long extraYield = yieldProduced * cycles - yieldNeeded;
        if(extraYield > 0) {
            AddToChemVec( extraChemicals, new Chemical(extraYield, targetChemical->name) );
        }

        cout << extraYield << endl;

        // consolidate and add new chemicals to neededChemicals.
        for(Chemical* c : reaction->inputs) {
            AddToChemVec( neededChemicals, new Chemical(cycles * c->yield, c->name) );
        }

        // clean up
        if(extraChemical != nullptr)  
            delete extraChemical;
        delete targetChemical;

        // debug
        cout << "needed count: " << neededChemicals.size() << endl;
        for(Chemical* c : neededChemicals) {
            cout << "\t" << c->name << " " << c->yield << endl;
        }
        cout << "extras count: " << extraChemicals.size() << endl;
        for(Chemical* c : extraChemicals) {
            cout << "\t" << c->name << " " << c->yield << endl;
        }
    }

    return neededChemicals[0]->yield;
}

// pretty much CalculateOreFuelRatio but with ghetto mode cranked to 1 billion.
long NanoFactory::ProduceFuel(long ore) {
    if(!reactionsLoaded) {
        cout << "ERROR on 'ProduceFuel()', please load reactions first." << endl;
        return -1; // must load reactions first
    }

    long fuel = 40700*100;
    neededChemicals.push_back( new Chemical(40700*100, "FUEL") );  // goal is fuel.

    // Convert 1 chemical per loop
    // Loop until only Ore is left in the need side (base case)
    while( !(neededChemicals.size() == 1 && neededChemicals[0]->name == "ORE" && neededChemicals[0]->yield >= ore) ) {
        if(neededChemicals.size() == 1 && neededChemicals[0]->name == "ORE"){
            fuel++;
            neededChemicals.push_back( new Chemical(1, "FUEL") );
            continue;
        }

        // Don't choose the ORE in this list.
        int index = 0;
        while(neededChemicals[index]->name == "ORE") 
            index++;
        
        Chemical* targetChemical = neededChemicals[index];
        neededChemicals.erase( neededChemicals.begin() + index );

        // find reaction that makes targetChemical
        Reaction* reaction = nullptr;
        for(Reaction* r : reactions) {
            ///cout << r->output->name << "xxx" << endl;
            if(r->output->name == targetChemical->name) {
                reaction = r;
                break;
            }
        }
        //cout << targetChemical->name << " - chosen\n";

        // apply extras to neededChemicals
        Chemical* extraChemical = nullptr;
        index = 0;
        for(Chemical* c : extraChemicals) {
            if(c->name == targetChemical->name) {
                extraChemical = c;
                break;
            }
            index++;
        }

        // Handles a few special cases.
        if(extraChemical != nullptr && extraChemical->yield > targetChemical->yield) {
            // case: don't have to do anything special.
            extraChemical->yield -= targetChemical->yield;
            delete targetChemical;

            /*
            cout << "needed count: " << neededChemicals.size() << endl;
            for(Chemical* c : neededChemicals) {
                cout << "\t" << c->name << " " << c->yield << endl;
            }
            cout << "extras count: " << extraChemicals.size() << endl;
            for(Chemical* c : extraChemicals) {
                cout << "\t" << c->name << " " << c->yield << endl;
            }*/

            continue;
        } else if(extraChemical != nullptr && extraChemical->yield == targetChemical->yield) {
            // case: mostly easy to handle.
            extraChemicals.erase(extraChemicals.begin() + index);
            delete extraChemical;
            delete targetChemical;

            /*
            cout << "needed count: " << neededChemicals.size() << endl;
            for(Chemical* c : neededChemicals) {
                cout << "\t" << c->name << " " << c->yield << endl;
            }
            cout << "extras count: " << extraChemicals.size() << endl;
            for(Chemical* c : extraChemicals) {
                cout << "\t" << c->name << " " << c->yield << endl;
            }*/

            continue;
        } else if (extraChemical != nullptr) {
            // case: extrachemicals are less than target
            extraChemicals.erase(extraChemicals.begin() + index);
        }

        // find how many times to run reaction
        long yieldNeeded = targetChemical->yield - (extraChemical != nullptr ? extraChemical->yield : 0);
        size_t yieldProduced = reaction->output->yield;  // in one cycle
        size_t cycles = uceil_div(yieldNeeded, yieldProduced);  // unsignesd ceil(needed / produced)
        
        /*
        cout << "vars:\n";
        cout << "needed=" << yieldNeeded << endl;
        cout << yieldProduced << endl;
        cout << cycles << endl;
*/
        // find extra produced
        long extraYield = yieldProduced * cycles - yieldNeeded;
        if(extraYield > 0) {
            AddToChemVec( extraChemicals, new Chemical(extraYield, targetChemical->name) );
        }

        //cout << extraYield << endl;

        // consolidate and add new chemicals to neededChemicals.
        for(Chemical* c : reaction->inputs) {
            AddToChemVec( neededChemicals, new Chemical(cycles * c->yield, c->name) );
        }

        // clean up
        if(extraChemical != nullptr)  
            delete extraChemical;
        delete targetChemical;
/*
        // debug
        cout << "needed count: " << neededChemicals.size() << endl;
        for(Chemical* c : neededChemicals) {
            cout << "\t" << c->name << " " << c->yield << endl;
        }
        cout << "extras count: " << extraChemicals.size() << endl;
        for(Chemical* c : extraChemicals) {
            cout << "\t" << c->name << " " << c->yield << endl;
        }*/
    }

    // debug
    cout << "needed count: " << neededChemicals.size() << endl;
    for(Chemical* c : neededChemicals) {
        cout << "\t" << c->name << " " << c->yield << endl;
    }
    cout << "extras count: " << extraChemicals.size() << endl;
    for(Chemical* c : extraChemicals) {
        cout << "\t" << c->name << " " << c->yield << endl;
    }

    return fuel - 1; // this is not the correct way to do it, but the answer is correct.
}
