#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <random>
#include <vector>
#include <cstdarg>

using namespace std;


random_device dev; //Will be used to obtain a seed for the random number engine
mt19937 mersenne(dev()); //Standard mersenne_twister_engine seeded with dev()

//Initialize random number generator for matrix values between 0 and 1
uniform_int_distribution<> value_gen(0,1);

//Initialize random int generator to choose which sign we switch for hill_climbing
uniform_int_distribution<> switch_gen(0,99);

//Initialize random real number generator to determine how to move for annealing
uniform_real_distribution<> annealing_gen(0.0,1.0);


//The three following functions are implementations of the NP-heuristics detailed
//in the programming assignment specifications

vector<double> repeated_random(vector<double> A_input){

    vector<double> solution;
    vector<double> signs = {-1, 1};
    double residue = 0;

    //Generate initial random solution w/ residue
    for (long unsigned j = 0; j < A_input.size(); j++){
        solution.push_back(signs[value_gen(mersenne)]);
    }
    for(long unsigned k = 0; k < A_input.size(); k++){
        residue += A_input[k]*solution[k];
    }

    for (int i = 0; i < 25000; i++){

        double temp = 0;
        vector<double> potential;

        //Generate a random solution
        for (long unsigned j = 0; j < A_input.size(); j++){
            potential.push_back(signs[value_gen(mersenne)]);
        }

        //Use solution on input
        for(long unsigned k = 0; k < A_input.size(); k++){
            temp += A_input[k]*potential[k];
        }

        //Assign sign sequence w/ better residue to main solution
        if (temp < residue){
            for (long unsigned p = 0; p < potential.size(); p++){
                solution[p] = potential[p];
            }

            residue = temp;
        }
    }

    return solution;
}


vector<double> hill_climbing(vector<double> A_input){

    vector<double> solution;
    vector<double> neighbor;
    vector<double> signs = {-1, 1};
    
    double residue = 0;
    double temp;
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
        
        //Use solution on input to generate temp residue
        for(long unsigned k = 0; k < A_input.size(); k++){
            temp += A_input[k]*neighbor[k];
        }

        //Assign sign sequence w/ better residue to main solution
        if (temp < residue){
            solution = neighbor;
            residue = temp;
        }

        //Calculate new neighbor from solution
        switch_idx = switch_gen(mersenne);
        neighbor[switch_idx] *= -1;
    }  


    return solution;
}

vector<double> simulated_annealing(vector<double> A_input){

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


int main(int argc, char** argv) {

    //assert(argc == 4);
    // please use flag 0 for grading as described in P3 description
    // double flag = atoi(argv[1]);
    // char* algorithm = argv[2];

    //Initialize the input file to prepare for reading
    std::ifstream input_file;
    input_file.open(argv[3]);

    // Read the 100 numbers from the input file
    //while(getline(input_file, line)){

}