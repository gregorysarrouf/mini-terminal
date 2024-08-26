#include <stdio.h>


int main(int argc, char* argv[])
{
  // Strip quotes first
  for (int i = 1; i < argc; i++)
    printf("%s ", argv[i]);

  printf("\n");
  return 0;
}
