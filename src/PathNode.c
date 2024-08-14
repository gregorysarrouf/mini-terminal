#include <stdlib.h>
#include <stdio.h>

// Not sure if I even need this

struct PathNode {
  char *dir;
  struct PathNode *next;
};

void printList(struct PathNode *head)
{
  while (head != NULL) 
  {
    printf("%s ", head->dir);
    head = head->next;
  }
  printf("\n");

}

void freeList(struct PathNode *head)
{
  struct PathNode *temp;

  while (head != NULL)
  {
    temp = head;
    head = head->next;
    free(temp);
  }
}
