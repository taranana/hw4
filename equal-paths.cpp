#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool nodeTest(Node* root, int depth, int& nodeDepth) {
  if(root == nullptr) { return true;}

  if(root->left == nullptr && root->right == nullptr) {
    if(nodeDepth == -1) {
      nodeDepth = depth;
      return true;
    }
    return depth == nodeDepth;
  }

  bool leftSuccess = nodeTest(root->left, depth+1, nodeDepth);
  bool rightSuccess = nodeTest(root->right, depth+1, nodeDepth);
  
  return leftSuccess && rightSuccess;
}

bool equalPaths(Node * root)
{
    // Add your code below
    int nodeDepth = -1;
    return nodeTest(root,0,nodeDepth);

}

