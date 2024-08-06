#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include "constants.h"
#include "functions.c"

  
int main(int argc, char **argv) {

  char buffer[MAX_INPUT_SIZE];
  char tokens[MAX_TOKEN_NUMBER][MAX_INPUT_SIZE];

  if (isatty(STDIN_FILENO)) 
  {
    while (1)
    {
      prompt();
      readInput(buffer);

      if(!isValidInput(buffer))
      {
        printf("Handle Error\n");
        continue;
      }

      if ((strcmp(buffer, "exit")) == 0)
        break;

      parseTokens(buffer, tokens);
    }
    

  } 
  else 
  {
    // Execute commands from scripts and bash files
    printf("is not atty"); 
  }


  printf("\n");

  return 0;
}
