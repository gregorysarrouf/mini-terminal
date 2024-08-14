#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Operators {
  SEMI_COLUMN,
  OR,
  AND,
  PIPE
};

enum Outcome {
  FAILURE,
  SUCCESS
};

struct TreeNode {
  int type;
  char *content;
  struct ChildNode *childrenHead;
};

// Linked List holding children from left to right
struct ChildNode {
  struct TreeNode *child;
  struct ChildNode *next;
};


struct TreeNode* createTreeNode(int type, char *content, struct ChildNode *childrenHead)
{
  struct TreeNode *node =  malloc(sizeof(struct TreeNode));

  if (node == NULL)
  {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  node->type = type;
  node->content = content;
  node->childrenHead = childrenHead;

  return node;
}

void addChild(struct TreeNode *parent, struct TreeNode *child)
{
  struct ChildNode *childListNode = malloc(sizeof(struct ChildNode));

  if (childListNode == NULL)
  {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  childListNode->child = child;
  childListNode->next = NULL;

  struct ChildNode *childHead = parent->childrenHead;

  if (childHead == NULL)
    parent->childrenHead = childListNode;
  else 
  {
    while (childHead->next != NULL)
      childHead = childHead->next;

    childHead->next = childListNode;
  }
}

// Recursively free each node and the linkedlist containing its children
void freeTreeNode(struct TreeNode *root)
{
  if (root == NULL)
    return;

  struct ChildNode *currentChild = root->childrenHead;

  while (currentChild != NULL) 
  {
    struct TreeNode *tempTreeNode = currentChild->child;
    struct ChildNode *tempListNode = currentChild;
    currentChild = currentChild->next;
    freeTreeNode(tempTreeNode);
    free(tempListNode);
  }

  free(root);
}

void toString(struct TreeNode *node)
{
  char *type = node->type ? "command" : "operator";

  printf("Type: %s\n", type); 
  printf("Content: %s\n", node->content); 
  printf("Children:\n\n");

  struct ChildNode *child = node->childrenHead;

  while (child != NULL)
  {
    toString(child->child);
    child = child->next;
  }
}
