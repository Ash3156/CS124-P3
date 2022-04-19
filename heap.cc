
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <assert.h>
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
        assert(size > 0);
        swap(h[0], h[size - 1]);
        int output = h[size - 1];
        h.pop_back();
        size = h.size();
        heapify(0);
        return output;
    }
};

// helper function for printing the heap
void h_print(heap h) {
    for (int i = 0; i < h.h.size(); i++)
        printf("%i ", h.h[i]);
    printf("\n");
}

// helper for printing vector
void v_print(vector<int> v) {
    for (int i = 0; i < v.size(); i++)
        printf("%i ", v[i]);
    printf("\n");
}


heap v_to_h(vector<double> v) {
    heap h;
    for (int i = 0; i < v.size(); i++) {
        h.insert(int(v[i]));
    }
    return h;
}

int main() {
    vector<double> test = {2.0, 4.0, 90.0, 1.0, 880.0, 6001.0, 1000.0, 12.0, 3.0};
    heap b = v_to_h(test);
    h_print(b);
    heap a;
    a.insert(3);
    a.insert(2);
    a.insert(4);
    a.insert(100);
    a.insert(12);
    a.insert(1);
    a.insert(880);
    a.insert(1000);
    h_print(a);
    printf("%i\n", a.pop());
    printf("%i\n", a.pop());
    printf("%i\n", a.pop());
    a.insert(6);
    printf("%i\n", a.pop());
    printf("%i\n", a.pop());
    a.insert(1234);
    printf("%i\n", a.pop());
    printf("%i\n", a.pop());
    printf("%i\n", a.pop());
    printf("done popping\n");
    h_print(a);
}