#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "node.c"
#include "functions.h"
#include "constants.h"
#include "flow.c"

struct node *pathHead = NULL;

struct node* pathToList(char cwd[])
{
  struct node *head = NULL;
  struct node *tail = NULL;

  strtok(cwd, "/");

  while (cwd != NULL)
  {
    // Create new node and add directory to it
    struct node *new_node = (struct node*) malloc(sizeof(struct node));
    new_node->dir = cwd;
    new_node->next = NULL;

    // Link node to list
    if (head == NULL)
    {
      head = new_node;
      tail = new_node;
    }
    else 
  {
      tail->next = new_node;
      tail = new_node;
    }

    cwd = strtok(NULL, "/");
  }

  return head;
}

// Display username and current working directory as prompt
void prompt()
{
  // Free old path list if exists
  if (pathHead != NULL)
    freeList(pathHead);

  char cwd[1024], *username = getlogin();
  getcwd(cwd, sizeof(cwd));

  printf("%s@%s$ ", username, cwd);
  pathHead = pathToList(cwd); // Save new path to global var
}

// Reads input from the user
void readInput(char *input)
{
  fgets(input, MAX_INPUT_SIZE, stdin); 
  input[strlen(input) - 1] = '\0'; // Remove newLine char from the end for strcmp
}

int getArgNum(char *input)
{
  int spaces = 0;

  for (int i = 0; i < strlen(input); i++)
    if (input[i] == ' ')
      spaces++;

  return spaces + 1;
}

void clearTokens(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  for (int i = 0; i < MAX_TOKEN_NUMBER; i++)
    for (int j = 0; j < MAX_INPUT_SIZE; j++)
      tokens[i][j] = '\0';
}

void parseTokens(char *input, char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  trim(input);
  clearTokens(tokens);

  int counter = 0; 
  char *token = strtok(input, " ");

  while (token != NULL)
  {
    strcpy(tokens[counter], token);
    token = strtok(NULL, " ");
    counter++;
  }
}

void clearScreen()
{
  printf("\e[1;1H\e[2J"); 
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

int isValidDirectory(char *dir)
{
  struct stat stats;
  stat(dir, &stats);

  return S_ISDIR(stats.st_mode);
}

void executeCommand(char token[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  if (strlen(*token) == 0) // Skip over empty space
    return;

  if (!handleBuiltIn(token))
    return;

  printf("EXECUTED ALTERNATE COMMAND\n");
  
}

void changeDirectory(char *dir)
{
    if (!isValidDirectory(dir))
    {
      printf("Error: Invalid Directory\n");
      return;
    }

    chdir(dir);
}

void printwd()
{
  char wd[1024];
  getcwd(wd, sizeof(wd));
  printf("%s\n", wd);
}

int handleBuiltIn(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  const char *commands[] = { "exit", "clear", "pwd", "cd" };
  int numCommands = sizeof(commands) / sizeof(commands[0]), i;

  for (i = 0; i < numCommands; i++)
    if (strcmp(*tokens, commands[i]) == 0)
      break;

  switch(i)
  {
    case 0:
      exit(0);
    case 1:
      clearScreen();
      return EXIT;
    case 2:
      printwd();
      return EXIT;
    case 3:
      changeDirectory(tokens[1]);
      return EXIT;
    default:
      return CONTINUE;
  }
}
