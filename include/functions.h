#include "constants.h"

struct PathNode* pathToList(char cwd[]);
void prompt();
void readInput(char *input);
int getArgNum(char *input);
void clearTokens(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void parseTokens(char *input, char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
char *clearScreen();
void trim(char *input);
int isValidDirectory(char *dir);
void* executeCommand(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
char* printwd();
void* handleBuiltIn(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void changeDirectory(char *dir);
void rm(char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void touch (char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
char* list();
void getCommands(char* input, char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
int getNumCommands(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void execSingleCommand(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void execPipeline(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void printCommands(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void fixCommandSpacing(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE]);
void switchRows(char commands[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE], int row1, int row2);
