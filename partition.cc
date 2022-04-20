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
    vector<double> neighbor = opt_sol;
    double neighbor_res;

    for (int i = 0; i < 25000; i++) {
        //Calculate new neighbor of current opt_sol
        neighbor[switch_gen(mersenne)] *= -1;
        neighbor_res = res_calc(A_input, neighbor);
        //Assign sign sequence w/ better residue to main solution
        if (neighbor_res < opt_residue) {
            opt_sol = neighbor;
            opt_residue = neighbor_res;
            neighbor = opt_sol;
        }
    }
    return opt_sol;
}

vector<double> std_simulated_annealing(vector<double> A_input) {
    //Generate initial random solution w/ residue; for loop will potentially update
    int si = A_input.size();
    vector<double> S_double_prime = rand_sol(si);
    double S_double_residue = res_calc(A_input, S_double_prime);
    vector<double> S = S_double_prime;
    double S_res = res_calc(A_input, S);
    vector<double> neighbor = S;
    double neighbor_res;

    for (int i = 0; i < 25000; i++) {
        //Calculate new neighbor of current opt_sol
        neighbor[switch_gen(mersenne)] *= -1;
        neighbor_res = res_calc(A_input, neighbor);

        float random_annealing = annealing_gen(mersenne);

        bool a = random_annealing <= exp(-((neighbor_res - S_res)/(pow(10,10) * pow(0.8,(i/300)))));
        printf("%s\n", a ? "true" : "false");
        printf("%i\n", i);
        printf("neighbor r: %f, S_res: %f, global best:%f\n", neighbor_res, S_res, S_double_residue);
        printf("rand: %f, expression: %f\n", random_annealing, exp(-((neighbor_res - S_res)/(pow(10,10) * pow(0.8,(i/300))))));
        

        // if neighbor better then curr or certain prob for worse, we update S
        if (neighbor_res < S_res || random_annealing <= exp(-((neighbor_res - S_res)/(pow(10,10) * pow(0.8,(i/300)))))) {
            S = neighbor;
            S_res = neighbor_res;
            neighbor = S;
        }

        // regardless of above, we check if S'' should be updated
        if (S_res < S_double_residue) {
            S_double_prime = S;
            S_double_residue = S_res;
        }
    }
    return S_double_prime;
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
    // holy shit random found a 0 solution on this?????
    // vector<double> example = {10, 8, 7, 5, 6};
    vector<double> test = {97725, 69240, 19858, 36237, 27054, 69525, 66572, 59683,74672,50282,40959,14400,48795,27008,66661,6140,
 669,99491,25016,44444,66321,46995,88199,96919,34443,11061,22914,16659,8940,53968,66220,22373,79052,62036,66344,44659,22214,78224,
 71498,25511,26822,24063,28956,73900,82771,84078,16543,56554,4840,4946,26090,43344,33081,8841,24260,93440,26501,68028,41423,80139,63093,
 60513,16125,61841,1157,68361,57639,46768,66356,6008,12176,10279,70664,48531,58744,64776,7157,28905,93329,82728,8932,39164,92415,42719,
 39459,51752,86787,87753,25007,60069,32111,50605,73197,87371,41257,55844,69568,43030,40609,45306};

    // heap a = v_to_h(example);
    // h_print(a);
    // printf("bruh: %i\n", kar_karp(a));


    vector<double> sol = std_simulated_annealing(test);
    double res = res_calc(test, sol);
    // v_print(sol);
    printf("res calc: %f\n", res);

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