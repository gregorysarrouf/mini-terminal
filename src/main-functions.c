#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "./command-execution.c"


// Display username and current working directory as prompt
void prompt()
{
  char cwd[1024], *username = getlogin();
  getcwd(cwd, sizeof(cwd));

  printf("%s@%s$ ", username, cwd);
}

// Reads input from the user
void readInput(char *input)
{
  fgets(input, MAX_INPUT_SIZE, stdin); 
  input[strlen(input) - 1] = '\0'; // Remove newLine char from the end for strcmp
  trim(input);
}

int validateSemiColumns(char *input)
{
  int flag, i, j, k;

  for (i = 0; input[i] != '\0'; i++)
  {
    if (input[i] == ';')
    {
      flag = 0;
      j = i - 1;
      k = i + 1;

      if (j < 0)
        return 0;

      while (j >= 0 && k != '\0')
      {
        if (isspace(input[j]))
          j--;

        if (isspace(input[k]))
          k++;

        if (input[j] == ';' || input[k] == ';')
        {
          flag = 0;
          break;
        }

        if (!isspace(input[j]) && !isspace(input[k]))
        {
          flag = 1;
          break;
        }
      }

      if (!flag)
        return 0;
    }
  }
  return 1;
}

struct TreeNode* toTree(char *buffer)
{
  return turnToTree(buffer);
}

void executeTree(struct TreeNode *node)
{
  execute(node);
}

int validateTree(struct TreeNode *node)
{
  if (node->type || strcmp(node->content, ";") == 0) // is Command or semi-column operator
    return 1;

  int childCount = 0;
  struct ChildNode *childPointer = node->childrenHead;

  while (childPointer != NULL)
  {
    childCount++;
    validateTree(childPointer->child);
    childPointer = childPointer->next;
  }

  return childCount >= 2 ? 1 : 0;
}
