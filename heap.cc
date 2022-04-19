
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

struct heap {
    // use vector as heap
    vector<int> h;

    int size() {
        return h.size();
    }

    // sorts a subtree tree with root, used post-insert and post-pop
    void heapify(int root) {
        int size = h.size();
        int max_val = root;
        // tree is setup using this invariant:
        // L child is 2*curr_idx + 1, +2 for R child
        int L = 2 * root + 1;
        int R = 2 * root + 2;
        // size check is to ensure we only look at children that exist
        if (L < size && h[L] > h[max_val]) {
            max_val = L;
        }
        if (R < size && h[R] > h[max_val]) {
            max_val = R;
        }
        // invariant not satisfied if child bigger, so swap
        if (max_val != root) {
            swap(h[root], h[max_val]);
            // recurse further down this subtree since swap may have unsorted things
            heapify(max_val);
        }
    }

    // to add new elt, we add it to end of heap, then
    // re-heapify for all subtrees
    void insert(int i) {
        int size = h.size();
        if (size == 0) {
            h.push_back(i);
        }
        // re-heapify on parent of final slot (elt we just added)
        else {
            h.push_back(i);
            size += 1;
            // for (int i = size / 2 - 1; i >= 0; i--) {
            //     heapify(i);
            // }
            int curr = size - 1;
            while (curr > 0) {
                int parent = int(ceil(curr / 2.0)) - 1;
                heapify(parent);
                curr = parent;
            }
        }
    }

    // to pop, we save the current root value, move it to 
    // end so we can get rid of it using vector ops, then 
    // need to re-heapify
    int pop() {
        int size = h.size();
        if (size == 0) {
            return 0;
        }
        swap(h[0], h[size - 1]);
        int output = h[size - 1];
        h.pop_back();
        size = h.size();
        heapify(0);
        return output;
    }


    // nice helper function for printing the heap
    void print() {
    for (int i = 0; i < h.size(); ++i)
        printf("%i ", h[i]);
    printf("\n");
    }
};

// int main() {
//     heap a;
//     a.insert(3);
//     a.insert(2);
//     a.insert(4);
//     a.insert(100);
//     a.insert(12);
//     a.insert(1);
//     a.insert(880);
//     a.insert(1000);
//     a.print();
//     printf("%i\n", a.pop());
//     printf("%i\n", a.pop());
//     printf("%i\n", a.pop());
//     printf("%i\n", a.pop());
//     printf("%i\n", a.pop());
//     printf("%i\n", a.pop());
//     printf("%i\n", a.pop());
//     printf("done popping\n");
//     a.print();
// }