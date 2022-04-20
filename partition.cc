#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <random>
#include <vector>
#include <cstdarg>
#include <cassert>
#include "heap.cc"

using namespace std;


random_device dev; //Will be used to obtain a seed for the random number engine
mt19937 mersenne(dev()); //Standard mersenne_twister_engine seeded with dev()

//Initialize random number generator for matrix values between 0 and 1
uniform_int_distribution<> value_gen(0,1);

//Initialize random int generator to choose which sign we switch for hill_climbing
uniform_int_distribution<> switch_gen(0,99);

//Initialize random real number generator to determine how to move for annealing
uniform_real_distribution<> annealing_gen(0.0,1.0);


// Karmarker-Karp algorithm using heap: heap will update in the 
// following manner: delete max and second_max, insert 
// |max - second_max|, repeat until only 1 element left

int kar_karp(heap input) {
    while (input.size() > 1) {
        int max = input.pop();
        int second_max = input.pop();
        input.insert(max - second_max);
    }
    return input.h[0];
}

// residue calculator

double res_calc(vector<double> input, vector<double> sol) {
    double residue = 0;
    for(long unsigned k = 0; k < input.size(); k++){
        residue += input[k]*sol[k];
    }
    return abs(residue);
}

// rand sol generator

vector<double> rand_sol(int size) {
    vector<double> signs = {-1, 1};
    vector<double> sol;
    for (long unsigned j = 0; j < size; j++){
        sol.push_back(signs[value_gen(mersenne)]);
    }
    return sol;
}

//The three following functions are implementations of the NP-heuristics detailed
//in the programming assignment specifications for the standard representation

vector<double> std_repeated_random(vector<double> A_input) {
    //Generate initial random solution w/ residue; for loop will potentially update
    int s = A_input.size();
    vector<double> opt_sol = rand_sol(s);
    double opt_residue = res_calc(A_input, opt_sol);

    for (int i = 0; i < 25000; i++) {
        // generate random sol and its residue
        vector<double> potential_sol = rand_sol(s);
        double potential_residue = res_calc(A_input, potential_sol);

        //Assign sign sequence w/ better residue to main solution
        if (potential_residue < opt_residue) {
            opt_sol = potential_sol;
            opt_residue = potential_residue;
        }
    }
    return opt_sol;
}

vector<double> std_hill_climbing(vector<double> A_input) {
    //Generate initial random solution w/ residue; for loop will potentially update
    int s = A_input.size();
    vector<double> opt_sol = rand_sol(s);
    double opt_residue = res_calc(A_input, opt_sol);
    for (int i = 0; i < 25000; i++) {
        //Calculate new neighbor of current opt_sol
        vector<double> neighbor = opt_sol;
        neighbor[switch_gen(mersenne)] *= -1;
        double neighbor_res = res_calc(A_input, neighbor);
        printf("check\n");

        //Assign sign sequence w/ better residue to main solution
        if (neighbor_res < opt_residue) {
            opt_sol = neighbor;
            opt_residue = neighbor_res;
        }
    }

    return opt_sol;
}

vector<double> std_simulated_annealing(vector<double> A_input) {

    vector<double> solution;
    vector<double> neighbor;
    vector<double> signs = {-1, 1};


    double residue = 0;
    double temp = 0;
    int switch_idx = switch_gen(mersenne);


    //Generate initial random solution w/ residue
    for (long unsigned j = 0; j < A_input.size(); j++){
        solution.push_back(signs[value_gen(mersenne)]);
    }
    for (long unsigned k = 0; k < A_input.size(); k++){
        residue += A_input[k]*solution[k];
    }

    //Generate a neighboring solution from the current one, changing one
    //sign at a random index
    neighbor = solution;
    neighbor[switch_idx] *= -1;


    for (int i = 0; i < 25000; i++){
        
        //Holds residue of neighbor for comparison, reset for every iteration
        temp = 0;

        //Apply neighboring solution to input and obtain residue
        for(long unsigned k = 0; k < A_input.size(); k++){
            temp += A_input[k]*neighbor[k];
        }

        //Assign sign sequence w/ better residue to main solution
        //(or assign the worse one to solution with some probability specified below)

        float random_annealing = annealing_gen(mersenne);
        if ((temp < residue) || random_annealing <= exp(-((temp-residue)/((pow(10,10))*pow(0.8,(i/300)))))){
            solution = neighbor;
            residue = temp;
        } 
        //Calculate new neighbor from solution
        switch_idx = switch_gen(mersenne);
        neighbor[switch_idx] *= -1;
    }  
    return solution;

}

//The next three functions are implementations of the NP-heuristics detailed
//in the programming assignment specifications for the prepartitioned representation

vector<double> prep_repeated_random(vector<double> A_input){

    vector<double> solution;
    vector<double> neighbor;
    vector<double> signs = {-1, 1};
    int residue = 0;

    //Generate initial random solution w/ residue
    for (long unsigned j = 0; j < A_input.size(); j++){
        solution.push_back(value_gen(mersenne));
    }

    // Initialize A' vector and fill it with appropriate numbers
    vector<double> A_prime(A_input.size(), 0);
    for(long unsigned k = 0; k < A_input.size(); k++){
        A_prime[solution[k]] += A_input[k]; 
    }

    //Use kar_karp to find approx. residue for this random solution
    residue = kar_karp(v_to_h(A_prime));

    for (int i = 0; i < 25000; i++){

        double temp = 0;
        vector<double> potential;

        //Generate a random potential solution
        for (long unsigned j = 0; j < A_input.size(); j++){
            potential.push_back(value_gen(mersenne));
        }

        //Use this random potential partition to create another A' to evaluate
        vector<double> A_prime_temp(A_input.size(), 0);
        for(long unsigned k = 0; k < A_input.size(); k++){
            A_prime_temp[potential[k]] += A_input[k]; 
        }

        //Use kar_karp to find another temp residue to compare to
        temp = kar_karp(v_to_h(A_prime_temp));
        

        //Assign prepartitioning sequence w/ better residue to main solution
        if (temp < residue){
            solution = potential;
            residue = temp;
        }
    }

    return solution;
}

vector<double> prep_hill_climbing(vector<double> A_input){

    vector<double> solution;
    vector<double> neighbor;
    vector<double> signs = {-1, 1};
    int residue = 0;

    //Generate initial random solution w/ residue
    for (long unsigned j = 0; j < A_input.size(); j++){
        solution.push_back(value_gen(mersenne));
    }

    // Initialize A' vector and fill it with appropriate numbers
    vector<double> A_prime(A_input.size(), 0);
    for(long unsigned k = 0; k < A_input.size(); k++){
        A_prime[solution[k]] += A_input[k]; 
    }

    //Generate initial neighbor to the solution above
    vector<double> A_prime_temp(A_input.size(), 0);
    neighbor = solution;
    A_prime_temp = A_prime;
    int switch_idx = value_gen(mersenne);
    int new_group = value_gen(mersenne);
    int old_group = neighbor[switch_idx];
    neighbor[switch_idx] = new_group;
    A_prime[old_group] -= A_input[switch_idx];
    A_prime[new_group] += A_input[switch_idx];

    //Use kar_karp to find approx. residue for this random solution
    residue = kar_karp(v_to_h(A_prime));

    for (int i = 0; i < 25000; i++){

        double temp = 0;
        vector<double> potential;

        //Generate a random potential solution
        for (long unsigned j = 0; j < A_input.size(); j++){
            neighbor.push_back(value_gen(mersenne));
        }

        //Use this random potential partition to create another A' to evaluate
        vector<double> A_prime_temp(A_input.size(), 0);
        for(long unsigned k = 0; k < A_input.size(); k++){
            A_prime_temp[potential[k]] += A_input[k]; 
        }

        //Use kar_karp to find another temp residue to compare to
        temp = kar_karp(v_to_h(A_prime_temp));
        
        //Assign prepartitioning sequence w/ better residue to main solution
        if (temp < residue){
            solution = potential;
            residue = temp;
        }

        //Calculate new neighbor from solution
        neighbor = solution;
        A_prime_temp = A_prime;
        switch_idx = value_gen(mersenne);
        new_group = value_gen(mersenne);
        old_group = neighbor[switch_idx];
        neighbor[switch_idx] = new_group;
        A_prime_temp[old_group] -= A_input[switch_idx];
        A_prime_temp[new_group] += A_input[switch_idx];
    }

    return solution;
}

vector<double> prep_simulated_annealing(vector<double> A_input){

    vector<double> neighbor;
    vector<double> solution;
    vector<double> signs = {-1, 1};
    int residue = 0;

    //Generate initial random solution w/ residue
    for (long unsigned j = 0; j < A_input.size(); j++){
        solution.push_back(value_gen(mersenne));
    }

    // Initialize A' vector and fill it with appropriate numbers
    vector<double> A_prime(A_input.size(), 0);
    for(long unsigned k = 0; k < A_input.size(); k++){
        A_prime[solution[k]] += A_input[k]; 
    }

    //Generate initial neighbor to the solution above
    vector<double> A_prime_temp(A_input.size(), 0);
    neighbor = solution;
    A_prime_temp = A_prime;
    int switch_idx = value_gen(mersenne);
    int new_group = value_gen(mersenne);
    int old_group = neighbor[switch_idx];
    neighbor[switch_idx] = new_group;
    A_prime[old_group] -= A_input[switch_idx];
    A_prime[new_group] += A_input[switch_idx];


    //Use kar_karp to find approx. residue for this random solution
    residue = kar_karp(v_to_h(A_prime));

    for (int i = 0; i < 25000; i++){

        double temp = 0;
        vector<double> potential;

        //Generate a random potential solution
        for (long unsigned j = 0; j < A_input.size(); j++){
            neighbor.push_back(value_gen(mersenne));
        }

        //Use this random potential partition to create another A' to evaluate
        vector<double> A_prime_temp(A_input.size(), 0);
        for(long unsigned k = 0; k < A_input.size(); k++){
            A_prime_temp[potential[k]] += A_input[k]; 
        }

        //Use kar_karp to find another temp residue to compare to
        temp = kar_karp(v_to_h(A_prime_temp));
        
        //Assign prepartitioning sequence w/ better residue to main solution or
        //let solution be worse with a certain probabilty
        float random_annealing = annealing_gen(mersenne);
        if (temp < residue || random_annealing <= exp(-((temp-residue)/((pow(10,10))*pow(0.8,(i/300)))))){
            solution = potential;
            residue = temp;
        }

        //Calculate new neighbor from solution
        neighbor = solution;
        A_prime_temp = A_prime;
        switch_idx = value_gen(mersenne);
        new_group = value_gen(mersenne);
        old_group = neighbor[switch_idx];
        neighbor[switch_idx] = new_group;
        A_prime_temp[old_group] -= A_input[switch_idx];
        A_prime_temp[new_group] += A_input[switch_idx];
    }

    return solution;
}

int main(int argc, char** argv) {
    // printf("%i\n", switch_gen(mersenne));
    vector<double> test = {1, 7 ,8, 4, 6, 10};
    vector<double> sol = std_hill_climbing(test);
    v_print(sol);

    //assert(argc == 4);
    // please use flag 0 for grading as described in P3 description

    // Read the 100 numbers from the input file
    //Initialize the input file to prepare for reading
    // std::ifstream input_file;
    // input_file.open(argv[3]);
    //while(getline(input_file, line)){

    //int flag = atoi(argv[1]);
    //int algorithm = atoi(argv[2]);

    // heap input;
    // input.insert(10);
    // input.insert(15);
    // input.insert(0);
    // input.insert(6);
    // input.insert(5);

    // int output = kar_karp(input);


    // printf("%i\n", output);
}