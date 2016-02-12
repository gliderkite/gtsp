#Genetic Based Algorithms applied to the Travelling Salesman Problem (TSP)

##Basic Blocks

- Solution Representation: ordered list of customers

- Initial solution
  - A first tour is computed by means of the nearest neighbor search
  - More tours are generated through random solutions (shuffling)

- 2-opt

- Stopping criteria: The execution ends when the *best known* value of the current TSP istance is reached out. In the case where this value was not available, the execution would be arrested after a specified amount of time (provided as input)

##

