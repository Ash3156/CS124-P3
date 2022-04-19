#include <cstdlib>
#include <cassert>
#include <stdio.h>
#include <vector>
#include "heap.cc"

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

int main(int argc, char** argv) {
    assert(argc == 4);
    // please use flag 0 for grading as described in P3 description
    int flag = atoi(argv[1]);
    int algorithm = atoi(argv[2]);
    char* inputfile = argv[3];


    heap input;
    input.insert(10);
    input.insert(15);
    input.insert(0);
    input.insert(6);
    input.insert(5);

    int output = kar_karp(input);


    printf("%i\n", output);
}