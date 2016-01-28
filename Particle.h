#ifndef PARTICLE_H
#define PARTICLE_H


#include <vector>
#include <random>
using namespace std;


namespace pso
{
	struct Particle
	{
        /* Constructor. */
        template<class URNG>
        Particle(size_t size, URNG& g)
        {
            tour.resize(size);
            
            for (int i = 0; i < size; i++)
                tour[i] = i;
            
            shuffle(tour.begin(), tour.end(), g);
        }

        
        double cost(const vector<vector<double>>& distances);
        
        
        // TSP tour
        vector<int> tour;
	};
}


#endif