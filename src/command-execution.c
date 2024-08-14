#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "./TreeNode.c"
#include "../include/constants.h"

// Declarations
int executeSequentially(struct ChildNode *head);
int executeUntilSuccess(struct ChildNode *head);
int executeUntilFailure(struct ChildNode *head);
int executeWithRedirection(struct ChildNode *head);
int executeCommand(char *command);
void extractCommands(char **commands, struct TreeNode *parent);
void freeCommands(char **commands, int end);


char commandsHolder[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE];
int commandsTracker = 0;

int execute(struct TreeNode *node)
{
  int isCommand = node->type;

  if (isCommand)
    return executeCommand(node->content);

  char *operators[] = { ";", "||", "&&", "|" };
  int numOprators = sizeof(operators) / sizeof(operators[0]);
  int operator;

  for (operator = 0; operator < numOprators; operator++)
    if (strcmp(node->content, operators[operator]) == 0)
      break;

  switch(operator)
  {
    case SEMI_COLUMN:
      return executeSequentially(node->childrenHead); 
    case OR:
      return executeUntilSuccess(node->childrenHead);
    case AND:
      return executeUntilFailure(node->childrenHead);
    case PIPE:
      return executeWithRedirection(node->childrenHead);
  }

  return SUCCESS;
}

int executeSequentially(struct ChildNode *head)
{
  struct ChildNode *pHead = head;

  while (pHead != NULL)
  {
    execute(pHead->child);
    pHead = pHead->next;
  }
  return SUCCESS;
}

int executeUntilSuccess(struct ChildNode *head)
{
  struct ChildNode *pHead = head;
  int output = FAILURE;

  while (pHead != NULL)
  {
    output |= execute(pHead->child);

    if (output == SUCCESS)
      return output;

    pHead = pHead->next;
  }

  return output;
}

int executeUntilFailure(struct ChildNode *head)
{
  struct ChildNode *pHead = head;
  int output = SUCCESS;

  while (pHead != NULL)
  {
    output &= execute(pHead->child);

    if (output == FAILURE)
      return output;

    pHead = pHead->next;
  }

  return output;
}

int executeWithRedirection(struct ChildNode *head)
{
  struct ChildNode *childPointer = head;
  printf("%s ", childPointer->child->content);
  childPointer = childPointer->next;
  // Implement pipelines
  while(childPointer != NULL)
  {
    printf("into %s ", childPointer->child->content);
    childPointer = childPointer->next;
  }

  printf("\n");
  return SUCCESS;
}

int executeCommand(char *command)
{
  // Implement code execution
  printf("%s\n", command);
  return SUCCESS;
}

void trim(char *input)
{
  // Trim left
  while (isspace((unsigned char)input[0]))
    memmove(input, input + 1, strlen(input));

  // Trim right
  while (isspace((unsigned char)input[strlen(input) - 1]))
    input[strlen(input) - 1] = '\0';
}

void freeCommands(char **commands, int end)
{
  for (int i = 0; i < end; i++)
    free(commands[i]);

  free(commands);
}

// Returns dynamically allocated array of strings containing tokenized commands (free after use)
char** tokenize(char *str, char *delimiter)
{
  char *token;
  char **commands = malloc(MAX_TOKEN_NUMBER * sizeof(char *)); 
  int counter = 0;

  if (commands == NULL)
  {
    perror("Could not allocate memory");
    exit(EXIT_FAILURE);
  }

  // Allocate memory for each string
  for (int i = 0; i < MAX_TOKEN_NUMBER; i++)
  {
    commands[i] = malloc(MAX_INPUT_SIZE);

    if (commands[i] == NULL)
    {
      perror("Could not allocate memory");
      freeCommands(commands, i);
      exit(EXIT_FAILURE);
    }
  } 

  for (int i = 0; i < MAX_TOKEN_NUMBER; i++)
    for (int j = 0; j < MAX_INPUT_SIZE; j++)
      commands[i][j] = '\0';

  while ((token = strstr(str, delimiter)) != NULL)
  {
    // token points to substring from delimiter till end of string
    // str points to the whole string
    *token = '\0'; // token set to null, str points from start till delimiter

    strcpy(commands[counter], str);

    str = token + strlen(delimiter); // this causes str to point from delimiter till end of string
    counter++;
  }

  strcpy(commands[counter], str);

  return commands;
}

void extractCommands(char **commands, struct TreeNode *parent)
{
  char *delimiters[] = { ";", "||", "&&", "|" };
  int delimNum = sizeof(delimiters) / sizeof(delimiters[0]);
  int delimiter;
  int i = 0;
  struct TreeNode *childToAdd;
  char **tokenizedCommands;

  while (**(commands + i) != '\0')
  {
    for (delimiter = 0; delimiter < delimNum; delimiter++)
      if (strstr(*(commands + i), delimiters[delimiter]) != NULL) // is an operator
        break;

    if (delimiter < delimNum)
    {
      childToAdd = createTreeNode(0, delimiters[delimiter], NULL);
      addChild(parent, childToAdd);
      tokenizedCommands = tokenize(*(commands + i), delimiters[delimiter]);
      extractCommands(tokenizedCommands, childToAdd);
    }
    else
    {
      trim(*(commands + i));

      if (strcmp(*(commands + i), "") == 0)
      {
        printf("ignoring %s\n", *(commands + i));
        continue;
      }

      strcpy(commandsHolder[commandsTracker], *(commands + i));
      childToAdd = createTreeNode(1, commandsHolder[commandsTracker], NULL);
      addChild(parent, childToAdd);
      commandsTracker++;
    }

    i++;
  }

  freeCommands(commands, MAX_TOKEN_NUMBER);
}

struct TreeNode* turnToTree(char *buffer)
{
  if (strcmp(buffer, "") == 0)
    return NULL;

  struct TreeNode *pRoot = createTreeNode(0, NULL, NULL);
  char **commands = malloc(MAX_TOKEN_NUMBER * sizeof(char *));

  if (commands == NULL)
  {
    perror("Could not allocate memory");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAX_TOKEN_NUMBER; i++)
  {
    commands[i] = malloc(MAX_INPUT_SIZE);

    if (commands[i] == NULL)
    {
      perror("Could not allocate memory");
      freeCommands(commands, i);
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < MAX_TOKEN_NUMBER; i++)
    for (int j = 0; j < MAX_INPUT_SIZE; j++)
      commands[i][j] = '\0';

  strcpy(*commands, buffer);

  extractCommands(commands, pRoot); 

  return pRoot; 
}
