#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "./main-functions.c"
#include "../include/cmdPath.h"

int main(int argc, char **argv) {

  char buffer[MAX_INPUT_SIZE];
  getcwd(cmdPath, sizeof(cmdPath));
  strcat(cmdPath, "/build/");

  if (isatty(STDIN_FILENO)) 
  {
    while (1)
    {
      prompt();
      readInput(buffer);

      if (!validateSemiColumns(buffer))
      {
        printf("Parse Error\n");
        continue;
      }

      struct TreeNode *rootPtr = toTree(buffer);

      if (rootPtr == NULL)
        continue;

      struct TreeNode *root = rootPtr->childrenHead->child;

      if (!validateTree(root))
      {
        freeTreeNode(rootPtr);
        printf("Parse Error\n");
        continue;
      }

      execute(root);
      freeTreeNode(rootPtr);
    }
  } 
  else 
  {
    // Execute commands from scripts and bash files
    printf("is not atty"); 
  }

  return EXIT_SUCCESS;
}
