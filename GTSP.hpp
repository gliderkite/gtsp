#ifndef GTSP_HPP
#define GTSP_HPP


#include "Chromosome.hpp"
#include "Heuristic.hpp"
#include "TSP.hpp"

#include <vector>
#include <utility>
#include <chrono>
#include <random>
#include <algorithm>
#include <functional>
#include <cmath>
#include <cassert>
using namespace std;
using namespace chrono;



namespace tsp
{
    template<class T>
    struct GTSP
    {
        /* Constrcts the object with a TSPLIB file. */
        explicit GTSP(const string& filename)
        : GTSP(TSP::parse_tsplib(filename))
        {
        }
        
        /* Constructs the object with a list of node coordinates. */
        explicit GTSP(const vector<pair<double, double>>& coordinates)
        : coordinates(coordinates),
        psize(coordinates.size()),
        distances(TSP::distances<T>(coordinates)),
        minp(5),
        maxp(max_population()),
        engine((unsigned)system_clock::now().time_since_epoch().count()),
        mprob(0.2),
        not_improving_gen(0),
        max_not_improving_gen(50),
        massacre_percentage(0.5f)
        {
            nearest.resize(psize);
            
            for (unsigned i = 0; i != psize; i++)
            {
                nearest[i].resize(psize - 1);
                
                for (unsigned j = 0, k = 0; j != psize; j++)
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
        
        
        /* Solves the TSP problem. */
        template<class S>
        Chromosome<T> solve(S& stopCriteria, double best_known = 0)
        {
            auto best = init_population();
            //auto n = 0;
            
            do
            {
                if (best <=  best_known)
                    break;
                
                // select parents (could be the same)
                const auto& father = parent();
                const auto& mather = parent();
                mate(father, mather);
                
                best = update_population(best);
                //n++;
            }
            while (!stopCriteria());
            
            return population.front();
        }
        
        
        
        
    private:
        
        
        /* Implements the genetic crossover operator. */
        vector<Chromosome<T>> crossover(const Chromosome<T>& p1, const Chromosome<T>& p2)
        {
            // get the size of the tours
            const auto size = p1.tour.size();
            assert(size == p2.tour.size());
            
            vector<Chromosome<T>> offspring(2, Chromosome<T>(size));
            auto& tour1 = offspring[0].tour;
            auto& tour2 = offspring[1].tour;
            
            // choose two random numbers for the start and end indices of the slice
            uniform_int_distribution<int> d1(0, (int)size - 2);
            uniform_int_distribution<int> d2(0, (int)size - 1);
            const auto n1 = d1(engine);
            const auto n2 = d2(engine);
            
            // make the smaller the start and the larger the end
            int start = min(n1, n2);
            int end = max(n1, n2);
            
            // If end is equal to start the cut will have a size of 1
            // In the "worst" case start is equal to size - 2
            if (start == end)
                end++;
            
            // create the two crossing sections
            vector<size_t> cs1(end - start + 1);
            vector<size_t> cs2(end - start + 1);
            
            // copies the genes within the cut (between the two crossing sections)
            for (int i = start, k = 0; i <= end; i++, k++)
            {
                cs1[k] = tour1[i] = p1.tour[i];
                cs2[k] = tour2[i] = p2.tour[i];
            }
            
            // Final tours iterator
            int j = end + 1;
            int k = end + 1;
            
            // copies the genes outside of the cut from 'end' to the last element
            for (int i = end + 1; i < size; i++)
            {
                if (find(cs1.begin(), cs1.end(), p2.tour[i]) == cs1.end())
                    tour1[j++] = p2.tour[i];
                
                if (find(cs2.begin(), cs2.end(), p1.tour[i]) == cs2.end())
                    tour2[k++] = p1.tour[i];
            }
            
            // copies the genes outside of the cut from the first element to 'start'
            for (int i = 0; i <= end; i++)
            {
                // implements a circular buffer
                k %= size;
                j %= size;
                
                if (find(cs1.begin(), cs1.end(), p2.tour[i]) == cs1.end())
                    tour1[j++] = p2.tour[i];
                
                if (find(cs2.begin(), cs2.end(), p1.tour[i]) == cs2.end())
                    tour2[k++] = p1.tour[i];
            }
            
            return offspring;
        }
        
        
        /* Implements the genetic mutate operator. */
        void mutate(Chromosome<T>& c)
        {
            const auto len = (int)c.tour.size() - 1;
            uniform_int_distribution<int> distribution(0, len);
            
            const auto pos1 = distribution(engine);
            const auto pos2 = distribution(engine);
            swap(c.tour[pos1], c.tour[pos2]);
        }
        
        
        /* Implements the genetic invert operator. */
        void invert(Chromosome<T>& chromosome, bool invertGenes = false)
        {
            // get the size of the tours
            const auto size = chromosome.tour.size();
            
            // choose two random numbers for the start and end indices of the slice
            uniform_int_distribution<int> d1(0, (int)size - 2);
            uniform_int_distribution<int> d2(0, (int)size - 1);
            const auto n1 = d1(engine);
            const auto n2 = d2(engine);
            
            // make the smaller the start and the larger the end
            int start = min(n1, n2);
            int end = max(n1, n2);
            
            // If end is equal to start the cut will have a size of 1
            // In the "worst" case start is equal to size - 2
            if (start == end)
                end++;
            
            // instantiate the new tour
            auto ntour = chromosome.tour;
            
            if (invertGenes == false)
            {
                // Invert the string inside the cut
                for (int i = start, j = end - 1; i < end; i++, j--)
                    ntour[i] = chromosome.tour[j];
            }
            else
            {
                // reverses the two genes at the ends of the crossing section
                swap(chromosome.tour[start], chromosome.tour[end - 1]);
                
                // Copy the others genes
                for (int i = start + 1; i < end - 1; i++)
                    ntour[i] = chromosome.tour[i];
            }
            
            // Copy the genes after the cut
            for (int i = end; i < size; i++)
                ntour[i] = chromosome.tour[i];
            
            // Store the new tour
            chromosome.tour = ntour;
        }
        
        
        /* Mate parents. */
        void mate(const Chromosome<T>& p1, const Chromosome<T>& p2)
        {
            // Applies the order crossover operator to mate parents
            auto offspring = crossover(p1, p2);
            uniform_real_distribution<double> distribution;
            
            for (auto& child : offspring)
            {
                // Randomly applies the mutate operator
                if (distribution(engine) <= mprob)
                    mutate(child);
                
                // optimize the tour
                child.opt2(distances, nearest);
                
                const auto equal = [&child](const Chromosome<T>& c)
                    { return c.cost == child.cost; };
                
                // Avoid similar individuals
                auto it = find_if(begin(population), end(population), equal);
                
                if (it == end(population))
                    population.emplace_back(child);
                else
                {
                    // Apply the invert operator
                    invert(child);
                    
                    // optimize the tour
                    child.opt2(distances, nearest);
                    
                    // find if the populations contains the "same" tour already
                    it = find_if(begin(population), end(population), equal);
                    
                    // Avoid similar individuals
                    if (it == end(population))
                        population.emplace_back(child);
                }
            }
        }
        
        
        /* Kill the weakest. */
        T update_population(T pbest)
        {
            // sort the population according to the fitness of its individials
            sort(population.begin(), population.end(), less<Chromosome<T>>());
            
            if (pbest > population.front().cost)
                not_improving_gen = 0;
            else
            {
                // check if the population have to be killed
                if (++not_improving_gen == max_not_improving_gen)
                {
                    // No improvement for too many generations => extinction
                    extinction();
                    
                    // rebuild the population from survivors
                    fill_population();
                    not_improving_gen = 0;
                }
            }
            
            // kill the weakest if any
            if (population.size() > maxp)
                population.erase(begin(population) + maxp, end(population));
            
            return population.front().cost;
        }
        
        
        /* Mass extinction. */
        void extinction()
        {
            const auto size = population.size();
            T tot_fit = 0;
            
            const auto nKill = int(size * massacre_percentage) + 1;
            const auto max_survivors = max(size - nKill, minp);
            
            // compute the total cost of all tours
            for (int i = 0; i < size; i++)
                tot_fit += population[i].cost;
            
            vector<T> selprob(size);
            
            // compute the probability of being killed
            for (size_t i = 0; i < size; i++)
                selprob[i] = population[i].cost;
            
            uniform_int_distribution<int> distribution(0, (int)tot_fit);
            int index = distribution(engine) + 1;
            int sum = 0, i;
            
            // select which is the weakest survivor
            for (i = 0; sum < index && i < size; i++)
                sum += selprob[i];
            
            // Kill individuals
            for (int j = (int)population.size() - 1; j >= i && (int)population.size() > minp; j--)
                population.erase(begin(population) + j);
            
            // kill individuals in excess
            if (population.size() > max_survivors)
                population.erase(begin(population) + max_survivors, end(population));
        }
        
        
        /* Select an individual accorting to its fitness. */
        const Chromosome<T>& parent()
        {
            const int candidates_size = (int)population.size() / minp + 2;
            vector<T> selprob(candidates_size);
            T tot_fit = 0;
            
            // compute the total cost of all tours
            for (int i = 0; i < candidates_size; i++)
                tot_fit += population[i].cost;
            
            // compute the probability of being selected
            for (int i = 0, j = candidates_size - 1; i < candidates_size; i++, j--)
                selprob[i] = population[j].cost;
            
            uniform_int_distribution<int> distribution(0, tot_fit + 1);
            int index = distribution(engine) + 1;
            int sum = 0, i = 0;
            
            // select the parent
            for (; sum < index && i < candidates_size; i++)
                sum += selprob[i];
            
            return population[i - 1];
        }
        
        
        /* Initialize the population. */
        T init_population()
        {
            // init the population with the best/simplest heuristic function
            population.emplace_back(nearest_neighbor(nearest), distances, nearest);
            
            // add random tours to the population
            fill_population();
            
            return population.front().cost;
        }
        
        
        /* Fill the population with random individials. */
        void fill_population()
        {
            assert(maxp >= minp && minp > 0);
            static const auto k = maxp / minp + 1;
            auto max_attempts = int(population.size() * k);
            population.reserve(max_attempts);
            
            assert(!distances.empty());
            const auto size = distances.front().size();
            vector<unsigned> tour(size);
            
            // init the tour
            for (unsigned i = 0; i < size; i++)
                tour[i] = i;
            
            // fills the population with random tours
            while (max_attempts-- > 0 && population.size() < maxp)
            {
                // randomize the tour
                shuffle(tour.begin(), tour.end(), engine);
                // optimize the tour
                const auto cost = opt2(tour, distances);
                
                const auto it = find_if(begin(population), end(population),
                    [cost](const Chromosome<T>& c) { return cost == c.cost; });
                
                // avoid similar individuals
                if (it == end(population))
                    population.emplace_back(tour, cost);
            }
            
            sort(population.begin(), population.end(), less<Chromosome<T>>());
        }
        
        
        /* Regulate the maximum number of individuals. */
        size_t max_population() const
        {
            return max(minp, decltype(minp)(185 - 0.175 * psize));
        }
        
        
        
        
        
        // nodes coordinates
        const vector<pair<double, double>> coordinates;
        
        // number of nodes
        const size_t psize;
        
        // matrix of distances between nodes
        const vector<vector<T>> distances;
        
        // Minimum number of individuals (avoid extincion)
        const size_t minp;
        
        // Maximum number of individuals
        const size_t maxp;
        
        // matrix of nearest nodes
        vector<vector<unsigned>> nearest;
        
        // population
        vector<Chromosome<T>> population;
        
        // random engine
        default_random_engine engine;
        
        // mutation probability
        double mprob;
        
        // number of steps that didn't lead to improvements
        unsigned not_improving_gen;
        
        // maximun number of not improving steps before extinction
        const unsigned max_not_improving_gen;
        
        // minimum percentage of individuals that need to be killed during an extinction
        const double massacre_percentage;
        
    };
}



#endif 
