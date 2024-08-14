#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../src/command-execution.c"


int main() 
{
  //char buffer[] = "command1 || cd ./hello/dir | command3 && command4 ; command5 ; echo \"hello world\" && cd ..";
  char buffer[] = "cd ./hello/dir";
  struct TreeNode *pRoot = turnToTree(buffer);  
  struct TreeNode *root = pRoot->childrenHead->child;
  
  //toString(root->childrenHead->child);
  execute(root);

  freeTreeNode(root);

  return 0;
}
