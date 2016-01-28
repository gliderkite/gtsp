//
//  PSO.hpp
//  si
//
//  Created by Marco Conte on 1/28/16.
//  Copyright © 2016 Marco Conte. All rights reserved.
//

#ifndef PSO_h
#define PSO_h



void update(vector<Particle>& particles, double c1, double c2)
{
    const auto seed = system_clock::now().time_since_epoch().count();
    default_random_engine generator((unsigned)seed);
    uniform_real_distribution<double> distribution;
    
    const auto k1 = c1 * distribution(generator);
    const auto k2 = c2 * distribution(generator);
    
    evect<double> gbest;
    
    for (auto& p : particles)
    {
        /*const auto v = p.velocity;
         
         // update velocity
         p.velocity += (k1 * (p.best - p.position)) +
         (k2 * (gbest - p.position));
         
         // update position
         p.position += v;
         */
    }
}


/* Gets the list of swaps required to transform the permutation b into
 permutation a. */
vector<pair<int, int>> swaps(const vector<int>& a, const vector<int>& b)
{
    if (a.size() != b.size())
        throw invalid_argument("vectors must have the same size");
    
    vector<pair<int, int>> s;
    vector<bool> c(a.size());
    
    // map index-value
    unordered_map<int, int> map(a.size());
    
    for (int i = 0; i < a.size(); i++)
        map[a[i]] = i;
    
    // compute swap operations to transform permutation b into a
    for (size_t i = 0; i < a.size(); i++)
    {
        if (c[i])
            continue;
        
        auto idx = i;
        const auto first = a[idx];
        
        vector<int> cycle;
        cycle.push_back(a[idx]);
        c[idx] = true;
        
        // compute cycle
        while (b[idx] != first)
        {
            cycle.push_back(b[idx]);
            idx = map[b[idx]];
            c[idx] = true;
        }
        
        // store swap operations
        for (int j = (signed)cycle.size() - 1; j > 0; j--)
            s.emplace_back(make_pair(cycle[j], cycle[j-1]));
    }
    
    return s;
}




#endif /* PSO_h */
