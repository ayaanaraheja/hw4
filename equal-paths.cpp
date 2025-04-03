#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool check_depth(Node* root, int depth, int& leaf_depth) {
    if (!root) return true;

    if (!root->left && !root->right) {
      if (leaf_depth == -1) {
        leaf_depth = depth;
        return true;
        }

        return depth == leaf_depth;

    }
        return check_depth(root->left, depth + 1, leaf_depth) && check_depth(root->right, depth + 1, leaf_depth);
}

bool equalPaths(Node * root) {
    // Add your code below
    if (!root) return true;
    
    int leaf_depth = -1; 
    return check_depth(root, 0, leaf_depth);
}