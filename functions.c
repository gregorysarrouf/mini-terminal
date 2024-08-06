#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "constants.h"

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
}

int isEmptySpace(char *input)
{
  int flag = 1;

  for (int i = 0; i < strlen(input) - 1; i++)
    if (input[i] != ' ')
      flag = 0;

  return flag;
}

int isValidInput(char *input)
{
  if (strlen(input) == 0)  
    return 0;

  return !isEmptySpace(input);
}

int getArgNum(char *input)
{
  int spaces = 0;
  
  for (int i = 0; i < strlen(input); i++)
    if (input[i] == ' ')
      spaces++;

  return spaces + 1;
}

void parseTokens(char *input, char tokens[][MAX_INPUT_SIZE])
{
  int counter = 0, argNum = getArgNum(input);
  char *token = strtok(input, " ");

  while (token != NULL)
  {
    strcpy(tokens[counter], token);
    token = strtok(NULL, " ");
    counter++;
  }

  for (int i = 0; i < argNum; i++)
    printf("%s\n", tokens[i]);
}
