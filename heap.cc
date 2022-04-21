#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <assert.h>
using namespace std;

struct heap {
    // use vector as heap
    vector<double> h;

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
    void insert(double i) {
        int size = h.size();
        if (size == 0) {
            h.push_back(i);
        }
        // re-heapify on parent of final slot (elt we just added)
        else {
            h.push_back(i);
            size += 1;
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
    double pop() {
        int size = h.size();
        assert(size > 0);
        swap(h[0], h[size - 1]);
        double output = h[size - 1];
        h.pop_back();
        size = h.size();
        heapify(0);
        return output;
    }
};

// function to quickly convert vectors to heaps for kar_karp calls
heap v_to_h(vector<double> v) {
    heap h;
    for (int i = 0; i < (signed int) v.size(); i++) {
        h.insert(v[i]);
    }
    return h;
}

// printing helpers

// helper function for printing the heap
void h_print(heap h) {
    for (int i = 0; i < h.h.size(); i++)
        printf("%f ", h.h[i]);
    printf("\n");
}

// helper for printing vector
void v_print(vector<double> v) {
    for (int i = 0; i < v.size(); i++)
        printf("%f, ", v[i]);
    printf("\n");
}