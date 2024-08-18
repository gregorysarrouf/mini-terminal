#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TABLE_SIZE 256
#define MAX_FILENAME_LENGTH 256
#define MAX_INFO_LENGTH 1024

struct Entry {
  char filename[MAX_FILENAME_LENGTH];
  char info[MAX_INFO_LENGTH];
  struct Entry *next;
};

unsigned long hash(char *name)
{
  unsigned long hash = 5381;
  int c;

  while ((c = *name++))
    hash = ((hash << 5) + hash) + c;

  return (hash % TABLE_SIZE);
}

struct Entry* createEntry(char filename[MAX_FILENAME_LENGTH], char info[MAX_INFO_LENGTH])
{
  struct Entry *temp = malloc(sizeof(struct Entry));

  strncpy(temp->filename, filename, MAX_FILENAME_LENGTH);
  strncpy(temp->info, info, MAX_INFO_LENGTH);
  temp->next = NULL;

  return temp;
}

struct Entry** createHT()
{
  struct Entry **temp = malloc(sizeof(struct Entry) * TABLE_SIZE);

  for (int i = 0; i < TABLE_SIZE; i++)
    temp[i] = NULL;

  return temp;
}

void putEntry(struct Entry **table, struct Entry *entry)
{
  unsigned long index = hash(entry->filename);

  if (table[index] != NULL)
  {
    if (strcmp(table[index]->filename, entry->filename) == 0)
    {
      struct Entry *temp = table[index];
      table[index] = entry;
      table[index]->next = temp->next;
      free(temp);
      return;
    }

    struct Entry *next = table[index]->next;
    struct Entry *prev = table[index];

    if (next == NULL)
    {
      table[index]->next = entry;
      return;
    }

    while (next->next != NULL && strcmp(next->filename, entry->filename) != 0)
    {
      prev = next;
      next = next->next;
    }

    if (strcmp(next->filename, entry->filename) == 0)
    {
      prev->next = entry;
      prev->next->next = next->next;
      free(next);
      return;
    }

    next->next = entry; 
    return;
  }

  table[index] = entry;
}

char* getEntry(struct Entry **table, char *name)
{
  unsigned long index = hash(name); 
  struct Entry *entry = table[index];

  if (entry != NULL)
  {
    while (strcmp(entry->filename, name) != 0)
    {
      if (entry->next == NULL)
      {
        printf("no value found\n");
        return NULL;
      }

      entry = entry->next;
    }

    return entry->info;
  }

  printf("no value found\n");
  return NULL;
}

void printHT(struct Entry **table)
{
  for (int i = 0; i < TABLE_SIZE; i++)
  {
    printf("%d ", i);

    if (table[i] == NULL)
      printf("---\n");
    else 
    {
      printf("%s", table[i]->filename);
      struct Entry *next = table[i]->next;
      while (next != NULL)
      {
        printf("->%s", next->filename);
        next = next->next;
      }
      printf("\n");
    }
  }
}

void freeHT(struct Entry **table)
{
  for (int i = 0; i < TABLE_SIZE; i++)
    if (table[i] != NULL)
    {
      struct Entry *next = table[i]->next;

      while (next != NULL)
      {
        struct Entry *temp = next;
        next = next->next;
        free(temp);
      }
      
      free(table[i]);
    }

  free(table);
}
