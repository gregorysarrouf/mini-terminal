#include <stdlib.h>
#include <stdio.h>

struct node {
  char *dir;
  struct node *next;
};

void printList(struct node *head)
{
  while (head != NULL) 
  {
    printf("%s ", head->dir);
    head = head->next;
  }
  printf("\n");

}

void freeList(struct node *head)
{
  struct node *temp;

  while (head != NULL)
  {
    temp = head;
    head = head->next;
    free(temp);
  }
}
