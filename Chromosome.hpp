#ifndef CHROMOSOME_HPP
#define CHROMOSOME_HPP


#include "Heuristic.hpp"
#include "TSP.hpp"

#include <vector>
#include <chrono>
using namespace std;


namespace tsp
{
    template<class T>
    struct Chromosome
    {
        /* Constructor. */
        explicit Chromosome(size_t size)
        : cost(T())
        {
            tour.resize(size);
        }
        
        
        /* Constructs a new chromosome with a random tour. */
        template<class G>
        explicit Chromosome(const vector<vector<T>>& distances, G& engine,
                            const vector<vector<unsigned>>& nearest)
        {
            const auto size = distances[0].size();
            tour.resize(size);
            
            for (unsigned i = 0; i < size; i++)
                tour[i] = i;
            
            shuffle(tour.begin(), tour.end(), engine);
            opt2(distances, nearest);
        }
        
        /* Constructor. */
        explicit Chromosome(const vector<unsigned>& tour,
                            const vector<vector<T>>& distances,
                            const vector<vector<unsigned>>& nearest)
        : tour(tour)
        {
            opt2(distances, nearest);
        }
        
        /* Constructor. */
        explicit Chromosome(const vector<unsigned>& tour, T cost)
        : tour(tour), cost(cost)
        {
        }
        
        
        void opt2(const vector<vector<T>>& distances,
                  const vector<vector<unsigned>>& nearest)
        {
            // optimize the tour
            cost = tsp::opt2(tour, distances);
        }
        
        bool operator<(const Chromosome& c) const
        {
            return (cost < c.cost);
        }
        
        
        vector<unsigned> tour;
        T cost;
    };
}


#endif 
