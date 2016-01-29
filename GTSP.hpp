#ifndef GTSP_HPP
#define GTSP_HPP


#include "Chromosome.hpp"
#include "Heuristic.hpp"
#include "TSP.hpp"

#include <iostream>

#include <vector>
#include <utility>
#include <chrono>
#include <random>
#include <algorithm>
#include <functional>
#include <cmath>
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
        best_cost(numeric_limits<T>::max()),
        engine((unsigned)system_clock::now().time_since_epoch().count()),
        mprob(0.2),
        not_improving_gen(0),
        max_not_improving_gen(50),
        massacre_percentage(0.5f)
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
        
        
        template<class S>
        Chromosome<T> solve(S& stopCriteria, double best_known = 0)
        {
            init_population();
            
            do
            {
                if (best_cost <=  best_known)
                    break;
                
                // FATHER AND MOTHER CAN BE THE SAME!!!
                const auto& father = parent();
                const auto& mather = parent();
                
                mate(father, mather);
                
                update_population();
            }
            while (!stopCriteria());
            
            return population.front();
        }
        
        
        
        
    //private:
        
        
        
        vector<Chromosome<T>> crossover(const Chromosome<T>& p1, const Chromosome<T>& p2)
        {
            // get the size of the tours
            const auto size = p1.tour.size();
            
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
        
        
        void mutate(Chromosome<T>& chromosome)
        {
            const auto len = (int)chromosome.tour.size() - 1;
            uniform_int_distribution<int> distribution(0, len);
            
            const auto pos1 = distribution(engine);
            const auto pos2 = distribution(engine);
            swap(chromosome.tour[pos1], chromosome.tour[pos2]);
        }
        
        
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
            auto newTour = chromosome.tour;
            
            if (invertGenes == false)
            {
                // Invert the string inside the cut
                for (int i = start, j = end - 1; i < end; i++, j--)
                    newTour[i] = chromosome.tour[j];
            }
            else
            {
                // reverses the two genes at the ends of the crossing section
                short temp = chromosome.tour[start];
                newTour[start] = chromosome.tour[end - 1];
                newTour[end - 1] = temp;
                
                // Copy the others genes
                for (int i = start + 1; i < end - 1; i++)
                    newTour[i] = chromosome.tour[i];
            }
            
            // Copy the genes after the cut
            for (int i = end; i < size; i++)
                newTour[i] = chromosome.tour[i];
            
            // Store the new tour
            chromosome.tour = newTour;
        }
        
        
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
                child.opt2(distances);
                
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
                    child.opt2(distances);
                    
                    // find if the populations contains the "same" tour already
                    it = find_if(begin(population), end(population), equal);
                    
                    // Avoid similar individuals
                    if (it == end(population))
                        population.emplace_back(child);
                }
            }
        }
        
        
        void update_population()
        {
            // previous best cost
            const auto pbest = best_cost;
            
            sort(population.begin(), population.end(), less<Chromosome<T>>());
            best_cost = population.front().cost;
            
            // Store the best tour
            if (pbest > best_cost)
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
            
            // Remove the weakest if any
            if (population.size() > maxp)
                population.erase(begin(population) + maxp, end(population));
        }
        
        
        
        void extinction()
        {
            const auto size = population.size();
            double totFitness = 0;
            
            //const auto nKill = int(size * massacre_percentage) + 1;
            //int maxSurvivors = size - nKill;
            
            // compute the total cost of all tours
            for (int i = 0; i < size; i++)
                totFitness += population[i].cost;
            
            vector<double> selprob(size);
            
            // compute the probability of being killed
            for (size_t i = 0; i < size; i++)
                selprob[i] = population[i].cost;
            
            uniform_int_distribution<int> distribution(0, (int)totFitness);
            int index = distribution(engine) + 1;
            int sum = 0, i;
            
            // select which is the weakest survivor
            for (i = 0; sum < index && i < size; i++)
                sum += selprob[i];
            
            // Kill individuals
            for (int j = (int)population.size() - 1; j >= i && (int)population.size() > minp; j--)
                population.erase(begin(population) + j);
            
            // Kill individuals in excess
            //while (population.size() > GAParameters.minPopulationSize && population.size() > maxSurvivors)
                //population.remove(population.size() - 1);
        }
        
        
        
        const Chromosome<T>& parent()
        {
            const auto candidates_size = population.size() / minp + 2;
            vector<double> selprob(candidates_size);
            double totFitness = 0;
            
            // compute the total cost of all tours
            for (size_t i = 0; i < candidates_size; i++)
                totFitness += population[i].cost;
            
            // compute the probability of being selected
            for (int i = 0, j = (int)candidates_size - 1; i < (int)candidates_size; i++, j--)
                selprob[i] = population[j].cost;
            
            uniform_int_distribution<int> distribution(0, totFitness + 1);
            
            int index = distribution(engine) + 1;
            int sum, i;
            
            // select the parent
            for (sum = 0, i = 0; sum < index && i < candidates_size; i++)
                sum += selprob[i];
            
            return population[i - 1];
        }
        
        
        /* Initialize the population. */
        void init_population()
        {
            population.emplace_back(Chromosome<T>(nearest_neighbor(nearest), distances));
            
            fill_population();
        }
        
        
        void fill_population()
        {
            int max_attempts = int(population.size() * (maxp / minp + 1));
            
            // Fills the population with random tours
            while (max_attempts > 0 && population.size() < maxp)
            {
                //Chromosome<> chromosome(distances, engine);
                
                // Add the new chromosome if and only if it is not similar to the others
                //if (population.contains(chromosome) == false)
                    //population.add(chromosome);
                
                population.emplace_back(Chromosome<T>(distances, engine));
                
                max_attempts--;
            }
            
            sort(population.begin(), population.end(), less<Chromosome<T>>());
            best_cost = population.front().cost;
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
        vector<vector<size_t>> nearest;
        
        // best tour cost
        T best_cost;
        
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
