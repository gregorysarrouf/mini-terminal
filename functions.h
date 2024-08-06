#include "constants.h"

void prompt();
void readInput(char *input);
int isEmptySpace(char *input);
int isValidInput(char *input);
int getArgNum(char *input);
void parseTokens(char *input, char tokens[][MAX_INPUT_SIZE]);
