#ifndef HEURISTIC_HPP
#define HEURISTIC_HPP


#include "TSP.hpp"
using namespace tsp;


#include <vector>
#include <utility>
#include <limits>
#include <cmath>
#include <algorithm>
using namespace std;



namespace tsp
{
    
    /** Calculate distance between two points. */
    template<class T = double>
    static double norm(const pair<T, T>& p1, const pair<T, T>& p2)
    {
        double xDiff = p1.first - p2.first;
        double yDiff = p1.second - p2.second;
        
        return sqrt(xDiff * xDiff + yDiff * yDiff);
    }
    
    
    /* Gets the tour of nodes according to the nearest neighbor heuristic. */
    template<class T>
    vector<size_t> nearest_neighbor(const vector<pair<T, T>>& coordinates)
    {
        const auto len = coordinates.size();
        vector<bool> available(len, true);
        vector<size_t> tour(len);
        
        for (int i = 1; i < len; i++)
        {
            auto closest = -1;
            auto dist = numeric_limits<double>::max();
            
            for (int j = 1; j < len; j++)
            {
                if (!available[j])
                    continue;

                double d = norm(coordinates[tour[i-1]], coordinates[j]);
                
                if (d < dist)
                {
                    dist = d;
                    closest = j;
                }
            }
            
            tour[i] = closest;
            available[closest] = false;
        }
        
        return tour;
    }
    
    
    /* Gets the tour of nodes according to the nearest neighbor heuristic. */
    vector<size_t> nearest_neighbor(const vector<vector<size_t>>& nearest)
    {
        const auto len = nearest[0].size() + 1;
        vector<size_t> tour(len);
        
        vector<bool> available(len, true);
        available[0] = false;
        
        for (size_t i = 1; i < len; i++)
        {
            size_t idx = 0;
            
            while (!available[nearest[tour[i-1]][idx]])
                idx++;
            
            tour[i] = nearest[tour[i-1]][idx];
            available[tour[i]] = false;
        }
        
        return tour;
    }
    
    
    void opt2(vector<size_t>& tour, const vector<vector<double>>& distances)
    {
        // Get tour size
        const auto size = tour.size();
        vector<size_t> ntour(size);
        
        // repeat until no improvement is made
        size_t improve = 0;
        auto best_cost = TSP::cost(tour, distances);
        
        while (improve < 20)
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
            
            improve++;
        }
    }
    
    
    
   

}



#endif

