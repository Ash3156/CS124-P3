#include <cstdlib>
#include <cassert>
#include <stdio.h>

int main(int argc, char** argv) {
    assert(argc == 4);
    // please use flag 0 for grading as described in P3 description
    int flag = atoi(argv[1]);
    char* algorithm = argv[2];
    char* inputfile = argv[3];
    printf("%s\n", inputfile);
}