#ifndef TSP_h
#define TSP_h


#include "Heuristic.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>
#include <cmath>
#include <random>
#include <chrono>
using namespace std;
using namespace chrono;


namespace tsp
{
    
    struct TSP
    {
        
        /* Reads the TSP file and returns a list of node (2D) coordinates. */
        template<class T = double>
        static vector<pair<T, T>> parse_tsplib(const string& filename)
        {
            vector<pair<T, T>> coordinates;
            ifstream istream(filename);
            
            if (istream.is_open())
            {
                // read file
                stringstream ss;
                ss << istream.rdbuf();
                
                string str;
                size_t dim = 0;
                
                // look for the start of the data section
                while (getline(ss, str) && str != "NODE_COORD_SECTION")
                {
                    const auto tokens = split(str, ':');
                    
                    // store the number of nodes
                    if (tokens.size() == 2 && !tokens.front().compare(0, 9, "DIMENSION"))
                        dim = stoul(tokens.back());
                }
                
                coordinates.resize(dim);
                
                for (decltype(dim) i = 0; i < dim; i++)
                {
                    size_t idx;
                    // skip the index
                    ss >> idx;
                    // read coordinates
                    ss >> coordinates[i].first;
                    ss >> coordinates[i].second;
                }
                
            }
            
            return coordinates;
        }
        
        
        /* Gets the matrix of distances between nodes. */
        template<class T = double>
        static vector<vector<double>> distances(const vector<pair<T, T>>& coordinates)
        {
            const auto len = coordinates.size();
            vector<vector<double>> matrix(len);
            
            for (auto& v : matrix)
                v.resize(len);
            
            for (size_t i = 0; i < len; i++)
            {
                for (size_t j = i + 1; j < len; j++)
                    matrix[i][j] = matrix[j][i] = norm(coordinates[i], coordinates[j]);
            }
            
            return matrix;
        }
        
        
        /** Computes the distance between two points. */
        template<class T = double>
        static double norm(const pair<T, T>& p1, const pair<T, T>& p2)
        {
            const auto xdiff = p1.first - p2.first;
            const auto ydiff = p1.second - p2.second;
            
            return sqrt(xdiff * xdiff + ydiff * ydiff);
        }
        
        
        /* http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/STSP.html */
        static double cost(const vector<size_t>& tour, const vector<vector<double>>& distances)
        {
            double dist = 0;
            const auto len = tour.size();
            
            for (int i = 0; i < len - 1; i++)
                dist += distances[tour[i]][tour[i+1]];
            
            return dist + distances[tour[len-1]][tour[0]];
        }
        
        
        
    private:
        
        
        static vector<string>& split(const string& s, char delim, vector<string>& elems)
        {
            stringstream ss(s);
            string item;
            
            while (getline(ss, item, delim))
                elems.push_back(item);
            
            return elems;
        }
        
        
        static vector<string> split(const string& s, char delim)
        {
            vector<string> elems;
            split(s, delim, elems);
            
            return elems;
        }
        
    };
    

    
    
    /* Reads the TSP file and returns a list of node (2D) coordinates. */
    vector<int> read_tour(const string& filename)
    {
        vector<int> tour;
        ifstream istream(filename);
        
        if (istream.is_open())
        {
            // read file
            stringstream ss;
            ss << istream.rdbuf();
            
            string str;
            size_t dim = 0;
            
            // look for the start of the data section
            while (ss >> str && str != "TOUR_SECTION")
            {
                // store the number of nodes
                if (!str.compare(0, 9, "DIMENSION"))
                    ss >> dim;
            }
            
            tour.resize(dim);
            
            for (size_t i = 0; i < dim; i++)
            {
                ss >> tour[i];
                tour[i]--;
            }
            
        }
        
        return tour;
    }
    
    
    
    template<class T = double>
    vector<vector<double>> distances(const vector<pair<T, T>>& coord)
    {
        const auto len = coord.size();
        vector<vector<double>> matrix(len);
        
        for (auto& v : matrix)
            v.resize(len);
        
        for (size_t i = 0; i < len; i++)
        {
            for (size_t j = i + 1; j < len; j++)
                matrix[i][j] = matrix[j][i] = norm(coord[i], coord[j]);
        }
        
        return matrix;
    }
    
    
    
    
    
    
    vector<int> random_tour(size_t size)
    {
        vector<int> tour(size);
        
        for (int i = 0; i < size; i++)
            tour[i] = i;
        
        const auto seed = (unsigned)system_clock::now().time_since_epoch().count();
        shuffle(tour.begin(), tour.end(), default_random_engine(seed));
        
        return tour;
    }
    
    
    
}




#endif















