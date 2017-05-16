MPI Program:-

In MPI directory:

1. For 0/1 Knapsack Problem:
>>mpic++ knapsack.cpp
>>time mpirun -np x ./a.out < Knapsack_Test1.txt
>>time mpirun -np x ./a.out < Knapsack_Test2.txt

2. For TSP Problem:
>>mpic++ tsp.cpp
>>time mpriun -np x ./a.out < TSP_Test1.txt
>>time mpriun -np x ./a.out < TSP_Test2.txt

//x is number of processors


OpenMP Program:-

In OpenMP directory:

1. For 0/1 Knapsack Problem:
>>g++ -fopenmp knapsack.cpp
>>time ./a.out < Knapsack_Test1.txt
>>time ./a.out < Knapsack_Test2.txt

2. For TSP Problem:
>>g++ -fopenmp tsp.cpp
>>time ./a.out < TSP_Test1.txt
>>time ./a.out < TSP_Test2.txt
----------------------------------------------------------------------------------------------

TestCases are written in Knapsack_Test.txt and TSP_Test.txt 

TestCase Format in Knapsack_Test.txt  :
n - number of bags
W - weight limit of bag
w1 w2 w3 ..... wn (weights of bags)
v1 v2 v3 ..... vn (values of bags)

TestCase Format in TSP_Test.txt  :
N (number of vertices)
NxN adjacency matrix

In the case of OpenMP the first line is the number of threads and rest is the same.
In the case of MPI the first line is the explorationDiameter(Explained in implementation details) and rest is the same.



