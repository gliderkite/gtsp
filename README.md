#Genetic Based Algorithms applied to the Travelling Salesman Problem (TSP)

##Basic Blocks

- **Solution Representation**: Ordered list of customers

- **Initial solution**
  - A first tour is computed by means of the nearest neighbor search
  - More tours are generated through random solutions (shuffling)

- **2-opt**

- **Stopping criteria**: The execution ends when the *best known* value of the current TSP istance is reached out. In the case where this value was not available, the execution would be arrested after a specified amount of time (provided as input)


- **Parents selection**: It is selected a list of candidates between the best individuals, equal to the number of individuals divided by the minimum number of individuals per population. The parents choice is based on the fitness attribute relative to the cost of the tour associated with it: the lower the cost of tour the higher the probability that this individual is selected.

- **Mate**: Two individuals are combined together using the order crossover genetic operator. If the child just generated happens to be equal to another individual of the population (their associated tours are the same), the inversion genetic operator would be applied on it, and if this new individual was not equal to another one, it would be added to the population.

- **Population Update**: After having generated the new individuals, the population is updated considering the number of generations without any improvement. If this number exceeds the established maximum number, it would be performed a massacre ensuring: a maximum number of survivors, and making the killings according to the inverse of the probability that each individual has to be chosen for mating. This operation is followed by the generation of new random individuals, in order to return, in the best of cases, to the maximum number of individuals permitted. A new individual has to be different to all the other individuals already present in the population in order to be added. If the killing process was not performed, the update would only consist on the killing of the weakest individuals in order to comply with the constraint of the maximum number of individuals of the population (the addition of the children could temporarily exceed this limit).

##Parameters tuning

- **Population size**: The minimum number of individuals is set to 5. The maximum size of the population is function of the number of cities in the tour: max_population = max { min_population, -0.175 * #Cities + 185 }

- **Killing rate**: 50%


## How To

**Compile**: `g++ -std=c++11 -Wall -O3 -DNDEBUG main.cpp -o gtsp`

**Run**: `./gtsp <filename> <timeout [s]> [<best known>]`
