#include "constants.h"

struct node* pathToList(char cwd[]);
void prompt();
void readInput(char *input);
int getArgNum(char *input);
void clearTokens(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void parseTokens(char *input, char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void clearScreen();
void trim(char *input);
int isValidDirectory(char *dir);
void executeCommand(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void printwd();
int handleBuiltIn(char *buffer);
