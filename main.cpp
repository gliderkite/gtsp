#include "GTSP.hpp"


#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>
#include <chrono>
using namespace std;
using namespace chrono;


static long long timeout = 0;
static long long elapsed;
static decltype(system_clock::now()) start;


static bool stop()
{
    elapsed = duration_cast<seconds>(system_clock::now() - start).count();
    return elapsed >= timeout;
}


int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "gtsp <filename> <timeout> [<best known>]" << endl;
        return 1;
    }
    
    try
    {
        timeout = stoi(argv[2]);
        const auto best_known = (argc == 4 ? stoi(argv[3]) : 0);
        
        GTSP<int> gtsp(argv[1]);
        
        start = system_clock::now();
        const auto best = gtsp.solve(stop, best_known);
        
        cout << "Best: " << best.cost;
        
        if (best_known < best.cost && best_known != 0)
            cout << " " << (double)best_known / best.cost << "%";
        
        cout << endl;
        cout << "Elapsed: " << elapsed << " [s]" << endl << endl;
        
        cout << "Best tour:" << endl << "{";
        for (size_t i = 0; i < best.tour.size(); i++)
        {
            cout << best.tour[i];
            
            if (i != best.tour.size() - 1)
                cout << ", ";
        }
        cout << "}" << endl;
    }
    catch (exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }
    
	return 0;
}












