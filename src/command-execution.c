#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include "./TreeNode.c"
#include "../include/constants.h"

char cmdPath[1024];

// Declarations
int executeSequentially(struct ChildNode *head);
int executeUntilSuccess(struct ChildNode *head);
int executeUntilFailure(struct ChildNode *head);
int executeWithRedirection(struct ChildNode *head);
int executeCommand(char *command);
void extractCommands(char **commands, struct TreeNode *parent);
void freeCommands(char **commands, int end);
char **tokenize(char *str, char *delim);


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

int getChildCount(struct ChildNode *head)
{
  int childCounter = 0;
  struct ChildNode *childPointer = head;

  while(childPointer != NULL)
  {
    childPointer = childPointer->next;
    childCounter++;
  }

  return childCounter;
}

void parseCommands(struct ChildNode *head, char commands[][MAX_INPUT_SIZE])
{
  struct ChildNode *headPtr = head;
  int counter = 0;
  long unsigned int len;

  while (headPtr != NULL)
  {
    len = strlen(headPtr->child->content);
    strcpy(commands[counter], headPtr->child->content);
    commands[counter][len] = '\0';
    headPtr = headPtr->next;
    counter++;
  }
}

int executeWithRedirection(struct ChildNode *head)
{
  int pipefd[2 * (MAX_TOKEN_NUMBER - 1)];
  pid_t pid;
  int i, j;
  int numCommands = getChildCount(head);
  char commands[numCommands][MAX_INPUT_SIZE];
  parseCommands(head, commands);

  // Create the necessary pipes
  for (i = 0; i < numCommands - 1; i++)
    if (pipe(pipefd + i * 2) < 0)
    {
      perror("Pipe Error");
      return FAILURE;
    }

  for (i = 0; i < numCommands; i++)
  {
    pid = fork();
    if (pid == 0) // Child process
    {       
      if (i != 0) // If not the first command, get input from previous pipe
        dup2(pipefd[(i - 1) * 2], STDIN_FILENO);

      // If not the last command, output to the next pipe
      if (i != numCommands - 1)
        dup2(pipefd[i * 2 + 1], STDOUT_FILENO);
      //
      // Close all pipe file descriptors
      for (j = 0; j < 2 * (numCommands - 1); j++) {
        close(pipefd[j]);
      }

      // Execute the command
      executeCommand(commands[i]);
      exit(EXIT_FAILURE); // Exit after execution, in case of execve failure
      //
    } else if (pid < 0) {
      perror("Fork Error");
      return FAILURE;
    }
  }

  // Close all pipe file descriptors in the parent
  for (i = 0; i < 2 * (numCommands - 1); i++)
    close(pipefd[i]);

  // Wait for all child processes to finish
  for (i = 0; i < numCommands; i++)
    wait(NULL);

  return SUCCESS;
}

void switchRows(char **commands, int row1, int row2)
{
  char temp[MAX_INPUT_SIZE];

  strcpy(temp, commands[row2]);
  memcpy(commands[row2], commands[row1], MAX_INPUT_SIZE);
  strcpy(commands[row1], temp);
}

void fixCommandSpacing(char **commands)
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

int getTokenNum(char **tokens)
{
  int index = 0;

  while (tokens[index][0] != '\0' && index != MAX_TOKEN_NUMBER)
    index++;

  return index;
}

int executeCommand(char *command)
{
  char **tokens = tokenize(command, " ");

  if (strcmp(command, "exit") == 0)
    exit(EXIT_SUCCESS);

  pid_t pid = fork();  // Create a child process

  if (pid < 0)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  } 
  else if (pid == 0)
  {
    // In the child process
    // Storing commands path in temporary variable
    char path[1100];
    strcpy(path, cmdPath);

    fixCommandSpacing(tokens); // Removing empty spaces
    int tokenNum = getTokenNum(tokens);

    tokens[0] = strcat(path, tokens[0]); // Including the path to commands directory
    tokens[tokenNum] = NULL;

    execve(tokens[0], tokens, NULL);

    // If execve returns, it must have failed
    perror("execve");
    //return FAILURE;
    exit(EXIT_FAILURE);
  }
  else 
  wait(NULL);  // Wait for the child process to finish

  free(tokens);
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
