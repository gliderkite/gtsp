#include "Particle.h"


namespace pso
{
    double Particle::cost(const vector<vector<double>>& distances)
    {
        double dist = 0;
        const auto len = tour.size();
        
        for (size_t i = 0; i < len - 1; i++)
            dist += distances[tour[i]][tour[i+1]];
        
        return dist + distances[tour[len-1]][tour[0]];
    }
}