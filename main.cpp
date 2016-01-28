#include "GTSP.hpp"


#include "Particle.h"
using namespace pso;

#include "TSP.hpp"
using namespace tsp;

#include "Evect.hpp"


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
using namespace std;
using namespace chrono;








int main()
{
    const string dir = "/Users/mconte/Desktop/SI/si/si/";
    
    
    GTSP<double> gtsp(dir + "berlin52.tsp");
    gtsp.solve();
    
    
    /*
    
    const auto best = tsp::read_tour(dir + "berlin52.opt.tour");
    
    cout << tsp::get_fitness(best, distances) << endl;
    
    const auto seed = (unsigned)system_clock::now().time_since_epoch().count();
    auto gen = default_random_engine(seed);
    
    vector<Particle> particles;
    particles.reserve(100);
    
    int idx = 0;
    double min = numeric_limits<double>::max();
    
    for (int i = 0; i < 100; i++)
    {
        particles.emplace_back(Particle(52, gen));
        auto fit = particles.back().cost(distances);
        
        if (fit < min)
        {
            min = fit;
            idx = i;
        }
    }
    
    cout << particles[idx].cost(distances) << endl;
    //const auto diff = swaps(best, particles[idx].tour);
    */

	return 0;
}












