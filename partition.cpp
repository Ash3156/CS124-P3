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
#include <string>
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

//Initialize random int generator to choose which sign we switch for hill_climbing
uniform_int_distribution<> part_idx_gen(0,99);


// Karmarker-Karp algorithm using heap: heap will update in the 
// following manner: delete max and second_max, insert 
// |max - second_max|, repeat until only 1 element left

double kar_karp(heap input) {
    while (input.size() > 1) {
        double max = input.pop();
        double second_max = input.pop();
        input.insert(max - second_max);
    }
    return input.h[0];
}

// residue calculator

double res_calc(vector<double> input, vector<double> sol) {
    double residue = 0;
    for(int k = 0; k < (signed int) input.size(); k++) {
        residue += input[k]*sol[k];
    }
    if (residue < 0) {
        residue = -1.0 * residue;
    }
    return residue;
}

// rand sol generator

vector<double> rand_sol_standard(int size) {
    vector<double> signs = {-1, 1};
    vector<double> sol;
    for (int j = 0; (signed int) j < size; j++){
        sol.push_back(signs[value_gen(mersenne)]);
    }
    return sol;
}

//The three following functions are implementations of the NP-heuristics detailed
//in the programming assignment specifications for the standard representation

double std_repeated_random(vector<double> A_input) {
    //Generate initial random solution w/ residue; for loop will potentially update
    int s = A_input.size();
    vector<double> opt_sol = rand_sol_standard(s);
    double opt_residue = res_calc(A_input, opt_sol);

    for (int i = 0; i < 25000; i++) {
        // generate random sol and its residue
        vector<double> potential_sol = rand_sol_standard(s);
        double potential_residue = res_calc(A_input, potential_sol);
        //Assign sign sequence w/ better residue to main solution
        if (potential_residue < opt_residue) {
            opt_sol = potential_sol;
            opt_residue = potential_residue;
        }
    }
    // return opt_sol;
    return opt_residue;
}

double std_hill_climbing(vector<double> A_input) {
    //Generate initial random solution w/ residue; for loop will potentially update
    int s = A_input.size();
    vector<double> opt_sol = rand_sol_standard(s);
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
        }
        // always reset neighbor so we're finding neighbors of curr_opt
        neighbor = opt_sol;
    }
    // return opt_sol;
    return opt_residue;
}

double std_simulated_annealing(vector<double> A_input) {
    //Generate initial random solution w/ residue; for loop will potentially update
    int si = A_input.size();
    vector<double> S_double_prime = rand_sol_standard(si);
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

        // if neighbor better then curr or certain prob for worse, we update S
        if (neighbor_res < S_res || random_annealing <= exp(-((neighbor_res - S_res)/(pow(10,10) * pow(0.8,(i/300)))))) {
            S = neighbor;
            S_res = neighbor_res;
        }

        // regardless of above, we check if S'' should be updated
        if (S_res < S_double_residue) {
            S_double_prime = S;
            S_double_residue = S_res;
        }
        // always reset neighbor to S, so on next iter we look for neighbors of current S
        neighbor = S;
    }
    // return S_double_prime;
    return S_double_residue;
}

//The next three functions are implementations of the NP-heuristics detailed
//in the programming assignment specifications for the prepartitioned representation

// rand sol generator for prepartioning solution

vector<double> rand_sol_prepart(int size) {
    vector<double> sol;
    for (int j = 0; (signed int) j < size; j++) {
        // want to generate random index for prepartioning
        sol.push_back(part_idx_gen(mersenne));
    }
    return sol;
}

vector<double> A_prime(vector<double> input, vector<double> sol) {
    int s = input.size();
    vector<double> output(s, 0);
    for(int k = 0; (signed int) k < s; k++) {
        output[sol[k]] += input[k]; 
    }
    return output;
}

// random prepartitoning fn
double prep_repeated_random(vector<double> A_input) {
    // first generate a random sol, its A', and residue - for loop will update this!
    int s = A_input.size();
    vector<double> sol = rand_sol_prepart(s);
    vector<double> sol_prime = A_prime(A_input, sol);
    double sol_res = kar_karp(v_to_h(sol_prime));

    for (int i = 0; i < 25000; i++) {
        // generate random sol and its residue
        vector<double> potential_sol = rand_sol_prepart(s);
        vector<double> potential_prime = A_prime(A_input, potential_sol);
        double potential_residue = kar_karp(v_to_h(potential_prime));

        // Assign prepartitioning sequence w/ better residue to main solution
        if (potential_residue < sol_res) {
            sol = potential_sol;
            sol_prime = potential_prime;
            sol_res = potential_residue;
        }
    }
    // return sol;
    return sol_res;
}

// prepartioning hill climbing
double prep_hill_climbing(vector<double> A_input) {
    // first generate a random sol, its A', and residue - for loop will update this!
    int s = A_input.size();
    vector<double> opt_sol = rand_sol_prepart(s);
    vector<double> sol_prime = A_prime(A_input, opt_sol);
    double sol_res = kar_karp(v_to_h(sol_prime));

    // initialize neighbor stuff, will be found in for loop
    vector<double> neighbor = opt_sol;
    vector<double> neighbor_prime = sol_prime;
    double neighbor_res;

    for (int i = 0; i < 25000; i++) {
        // calculate neighbor to current optimal, off at 1 idx
        neighbor[part_idx_gen(mersenne)] = part_idx_gen(mersenne);
        neighbor_prime = A_prime(A_input, neighbor);
        neighbor_res = kar_karp(v_to_h(neighbor_prime));

        // If neighbor sol is better, make it opt
        if (neighbor_res < sol_res) {
            opt_sol = neighbor;
            sol_prime = neighbor_prime;
            sol_res = neighbor_res;
        }
        // update so on next iter we look at neighbors of curr opt - neighbor prime and
        // res are updated based on neighbor each iter, so only need to reset neighbor
        neighbor = opt_sol;
    }
    // return opt_sol;
    return sol_res;
}

double prep_simulated_annealing(vector<double> A_input) {
    // first generate a random sol, its A', and residue - for loop will update this!
    int s = A_input.size();
    vector<double> S_double_sol = rand_sol_prepart(s);
    vector<double> S_double_prime = A_prime(A_input, S_double_sol);
    double S_double_res = kar_karp(v_to_h(S_double_prime));

    // initialize S and neighbor vals to same, will be modified
    vector<double> S_sol = S_double_sol;
    vector<double> S_prime = S_double_prime;
    double S_res = S_double_res;

    vector<double> neighbor = S_double_sol;
    vector<double> neighbor_prime = S_double_prime;
    double neighbor_res = S_double_res;

    for (int i = 0; i < 25000; i++) {
        // calculate neighbor to current optimal, off at 1 idx
        neighbor[part_idx_gen(mersenne)] = part_idx_gen(mersenne);
        neighbor_prime = A_prime(A_input, neighbor);
        neighbor_res = kar_karp(v_to_h(neighbor_prime));

        float random_annealing = annealing_gen(mersenne);

        // if neighbor is better or annealing prob, we update S
        if (neighbor_res < S_res || random_annealing <= exp(-((neighbor_res - S_res)/(pow(10,10) * pow(0.8,(i/300)))))) {
            S_sol = neighbor;
            S_prime = neighbor_prime;
            S_res = neighbor_res;
        }

        // regardless of above, we check if S'' should be updated
        if (S_res < S_double_res) {
            S_double_sol = S_sol;
            S_double_prime = S_prime;
            S_double_res = S_res;
        }
        // always reset neighbor to S, so on next iter we look for neighbors of current S
        neighbor = S_sol;
    }
    // return best sol we've ever seen
    // return S_double_sol;
    return S_double_res;
}

int main(int argc, char** argv) {
    assert(argc == 4);
    // flag 0 for grading as described in P3 description
    // int flag = atoi(argv[1]);
    
    // algorithm codes in P3 description
    int algorithm = atoi(argv[2]);

    // Read the 100 numbers from the input file
    // Initialize the input file outputs, prepare for reading
    heap input_heap;
    vector<double> input_vector;


    ifstream input_file;
    input_file.open(argv[3]);
    string line;
    while(getline(input_file, line)) {
        input_heap.insert(stod(line));
        input_vector.push_back(stod(line));
    }

    double answer;

    switch (algorithm) {
        case 0:
            answer = kar_karp(input_heap);
            printf("%lld\n", (long long) answer);
            break;
        case 1:
            answer = std_repeated_random(input_vector);
            printf("%lld\n", (long long) answer);
            break;
        case 2:
            answer = std_hill_climbing(input_vector);
            printf("%lld\n", (long long) answer);
            break;
        case 3:
            answer = std_simulated_annealing(input_vector);
            printf("%lld\n", (long long) answer);
            break;
        case 11:
            answer = prep_repeated_random(input_vector);
            printf("%lld\n", (long long) answer);
            break; 
        case 12:
            answer = prep_hill_climbing(input_vector);
            printf("%lld\n", (long long) answer);
            break;  
        case 13:
            answer = prep_simulated_annealing(input_vector);
            printf("%lld\n", (long long) answer);
            break; 
        default:
            assert(false);
            break;
    }
}