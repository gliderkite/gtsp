#ifndef GTSP_HPP
#define GTSP_HPP


#include "Heuristic.hpp"
#include "TSP.hpp"

#include <iostream>

#include <vector>
#include <utility>
using namespace std;



namespace tsp
{
    template<class T = double>
    struct GTSP
    {
        /* Constrcts the object with a TSPLIB file. */
        explicit GTSP(const string& filename)
        : GTSP(TSP::parse_tsplib(filename))
        {
        }
        
        /* Constructs the object with a list of node coordinates. */
        explicit GTSP(const vector<pair<T, T>>& coordinates)
        : coordinates(coordinates), psize(coordinates.size()),
        distances(TSP::distances(coordinates)),
        minp(5), maxp(max_population())
        {
            nearest.resize(psize);
            
            for (size_t i = 0; i != psize; i++)
            {
                nearest[i].resize(psize - 1);
                
                for (size_t j = 0, k = 0; j != psize; j++)
                {
                    if (i != j)
                        nearest[i][k++] = j;
                }
                
                // sort the indexes according to the distances between nodes
                // the closest node will appear to the front
                sort(nearest[i].begin(), nearest[i].end(), [this, i](size_t j1, size_t j2)
                     { return distances[i][j1] < distances[i][j2]; });
            }
        }
        
        
        
        void solve()
        {
            init_population();
        }
        
        
        
        
    private:
        
        
        /* Initialize the population. */
        void init_population()
        {
            best = nearest_neighbor(nearest);
            
            cout << TSP::cost(best, distances) << endl;
            opt2(best, distances);
            cout << TSP::cost(best, distances) << endl;
        }
        
        
        /* Regulate the maximum number of individuals. */
        size_t max_population() const
        {
            return max(minp, decltype(minp)(185 - 0.175 * psize));
        }
        
        
        
        
        
        // nodes coordinates
        const vector<pair<T, T>> coordinates;
        
        // number of nodes
        const size_t psize;
        
        // matrix of distances between nodes
        const vector<vector<double>> distances;
        
        // Minimum number of individuals (avoid extincion)
        const size_t minp;
        
        // Maximum number of individuals
        const size_t maxp;
        
        // matrix of nearest nodes
        vector<vector<size_t>> nearest;
        
        // best tour
        vector<size_t> best;
        
        
    };
}



#endif 
