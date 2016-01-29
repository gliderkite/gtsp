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
        explicit Chromosome(size_t size)
        : cost(T())
        {
            tour.resize(size);
        }
        
        
        /* Constructs a new chromosome with a random tour. */
        template<class G>
        explicit Chromosome(const vector<vector<T>>& distances, G& engine)
        {
            const auto size = distances[0].size();
            tour.resize(size);
            
            for (size_t i = 0; i < size; i++)
                tour[i] = i;
            
            shuffle(tour.begin(), tour.end(), engine);
            opt2(distances);
        }
        
        /* Constructor. */
        explicit Chromosome(const vector<size_t>& tour, const vector<vector<T>>& distances)
        : tour(tour)
        {
            opt2(distances);
        }
        
        
        void opt2(const vector<vector<T>>& distances)
        {
            // optimize the tour
            tsp::opt2(tour, distances);
            cost = TSP::cost(tour, distances);
        }
        
        bool operator<(const Chromosome& c) const
        {
            return (cost < c.cost);
        }
        
        
    //private:
        
        vector<size_t> tour;
        T cost;
    };
}


#endif 
