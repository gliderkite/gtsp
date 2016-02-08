#ifndef HEURISTIC_HPP
#define HEURISTIC_HPP


#include "TSP.hpp"
using namespace tsp;


#include <vector>
#include <utility>
#include <limits>
#include <cmath>
#include <algorithm>
#include <stdexcept>
using namespace std;



namespace tsp
{
    
    /** Calculate distance between two points. */
    template<class T = double>
    static double norm(const pair<T, T>& p1, const pair<T, T>& p2)
    {
        double xDiff = p1.first - p2.first;
        double yDiff = p1.second - p2.second;
        
        return round(sqrt(xDiff * xDiff + yDiff * yDiff));
    }
    
    
    /* Gets the tour of nodes according to the nearest neighbor heuristic. */
    vector<unsigned> nearest_neighbor(const vector<vector<unsigned>>& nearest)
    {
        if (nearest.empty())
            throw invalid_argument("nearest");
        
        const auto len = nearest.front().size() + 1;
        vector<unsigned> tour(len);
        
        // list of nodes still available
        vector<bool> available(len, true);
        // the first city has index equal to 0
        available[0] = false;
        
        for (size_t i = 1; i < len; i++)
        {
            // index of the final node
            auto idx = 0;
            // index of the starting node
            const auto j = tour[i - 1];
            
            // select the index of the closest node still available
            while (!available[nearest[j][idx]])
                idx++;
            
            tour[i] = nearest[j][idx];
            available[tour[i]] = false;
        }
        
        return tour;
    }
    
    
    /* https://en.wikipedia.org/wiki/2-opt */
    template<class T>
    double opt2(vector<unsigned>& tour, const vector<vector<T>>& distances,
              unsigned max_attempts = 20)
    {
        // Get tour size
        const auto size = tour.size();
        vector<unsigned> ntour(size);
        
        // repeat until no improvement is made
        unsigned improve = 0;
        auto best_cost = TSP::cost(tour, distances);
        
        while (improve++ < max_attempts)
        {
            for (int i = 0; i < size - 1; i++)
            {
                for (int k = i + 1; k < size; k++)
                {
                    // 1. take route[0] to route[i-1] and add them in order to new_route
                    for (int c = 0; c <= i - 1; c++)
                        ntour[c] = tour[c];
                    
                    // 2. take route[i] to route[k] and add them in reverse order to new_route
                    for (int c = i, dec = 0; c <= k; c++, dec++)
                        ntour[c] = tour[k - dec];
                    
                    // 3. take route[k+1] to end and add them in order to new_route
                    for (int c = k + 1; c < size; c++)
                        ntour[c] = tour[c];
                    
                    const auto cost = TSP::cost(ntour, distances);
                    
                    if (cost < best_cost)
                    {
                        // reset improvement
                        improve = 0;
                        tour = ntour;
                        best_cost = cost;
                    }
                }
            }
        }
        
        return best_cost;
    }

}



#endif

