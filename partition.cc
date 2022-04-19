#include <cstdlib>
#include <cassert>
#include <stdio.h>
#include <vector>

// Karmarker-Karp algorithm using heap: heap will update in the 
// following manner: delete max and second_max, insert 
// |max - second_max|, repeat until only 1 element left
int kar_karp(std::vector<int> input) {

    

}

int main(int argc, char** argv) {
    assert(argc == 4);
    // please use flag 0 for grading as described in P3 description
    int flag = atoi(argv[1]);
    int algorithm = atoi(argv[2]);
    char* inputfile = argv[3];
    printf("%s\n", inputfile);
}