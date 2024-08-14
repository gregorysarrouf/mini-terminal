#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include "PathNode.c"
#include "functions.h"
#include "constants.h"
#include "flow.h"

struct PathNode *pathHead = NULL;

struct PathNode* pathToList(char cwd[])
{
  struct PathNode *head = NULL;
  struct PathNode *tail = NULL;

  strtok(cwd, "/");

  while (cwd != NULL)
  {
    // Create new node and add directory to it
    struct PathNode *new_node = (struct PathNode*) malloc(sizeof(struct PathNode));
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

char* clearScreen()
{
  return "\e[1;1H\e[2J";
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

void* executeCommand(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  

  //if (!handleBuiltIn(tokens))

  return "EXECUTING ALTERNATE COMMAND"; 
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

char* printwd()
{
  char wd[1024];
  return getcwd(wd, sizeof(wd));
}

void *handleBuiltIn(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  const char *commands[] = { "exit", "clear", "pwd", "cd", "rm", "touch", "ls" };
  int numCommands = sizeof(commands) / sizeof(commands[0]), i;

  for (i = 0; i < numCommands; i++)
    if (strcmp(*tokens, commands[i]) == 0)
      break;

  switch(i)
  {
    case 0:
      exit(0);
    case 1:
      return clearScreen();
    case 2:
      return printwd();
    case 3:
      changeDirectory(tokens[1]); // return 0 or 1
      return EXIT;
    case 4:
      rm(tokens); // return 0 or 1
      return EXIT;
    case 5:
      touch(tokens); // return 0 or 1
      return EXIT;
    case 6:
      return list();
    default:
      return (void *)2; // returns 2
  }
}

void rm(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  int i = 1;
  int res;

  while (tokens[i][0] != '\0')
  {
    res = unlink(tokens[i]);
    if (res == -1)
      printf("Error: cannot remove '%s': No such file or directory\n", tokens[i]);
    i++;
  }
}

void touch(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  int i = 1;
  int fd;

  while (tokens[i][0] != '\0')
  {
    fd = open(tokens[i], O_CREAT | O_RDWR, 0644);
    if (fd == -1) 
    {
      printf("Error Creating File\n");
      return;
    }

    close(fd);
    i++;
  }
}

char* list()
{
  const char *dirname = ".";
  DIR *dir;
  struct dirent *entry;
  int column = 0;
  char* output = {};
  char* dName = {};
  char newLine = '\n';

  dir = opendir(dirname);
  if (dir == NULL)
    return "Error Opening Directory\n";

  while ((entry = readdir(dir)) != NULL)
  {
    if (column == 6)
    {
      strcat(output, &newLine);
      column = 0;
    }
    dName = entry->d_name;
    strcat(dName, "  ");
    strcat(output, dName);
    
    column++;
  }
  strcat(output, &newLine);

  if (closedir(dir) == -1)
    return "Error Closing Directory\n";

  return output;
}

void getCommands(char *input, char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  clearTokens(commands);
  trim(input);

  int counter = 0; 
  char *command = strtok(input, "|");

  while (command != NULL)
  {
    trim(command);
    strcpy(commands[counter], command);
    command = strtok(NULL, "|");
    counter++;
  }

  fixCommandSpacing(commands);
  printCommands(commands);
}

int getNumCommands(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  int counter = 0;

  while (commands[counter][0] != '\0')
    counter++;

  return counter;
}

void execSingleCommand(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  printf("Executing Single Command\n");
}

void execPipeline(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  printf("Executing Pipeline\n");
}

void printCommands(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  for (int i = 0; i < 10; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      if (commands[i][j] == '\0')
        printf(".");
      else 
         printf("%c", commands[i][j]);
    }
    printf("\n");
  }
}

void fixCommandSpacing(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE])
{
  int i, j;

  for (i = 0; i < MAX_TOKEN_NUMBER - 1; i++)
  {
    if (commands[i][0] == '\0')
      for (j = i + 1; j < MAX_TOKEN_NUMBER; j++)
      {
        if (commands[j][0] != '\0')
        {
          switchRows(commands, i, j); // Switch rows if a command is found
          break;
        }
      }

    if (j == MAX_TOKEN_NUMBER) // if there are no more commands, end the loop
      break;
  }
}

void switchRows(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE], int row1, int row2)
{
  char temp[MAX_INPUT_SIZE];

  strcpy(temp, commands[row2]);
  memcpy(commands[row2], commands[row1], MAX_INPUT_SIZE);
  strcpy(commands[row1], temp);
}
