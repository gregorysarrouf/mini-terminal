#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <stdarg.h>
#include "../../include/ls.h"
#include "../HashTable.c"

#define ENTRIES_NUM 25

/*READ ME
 *
 *
 * -a: do not ignore files starting with . 
 * -A: ignore only the obvious (., ..)
 * -l: long listing
 * -r: sort in reverse order
 *  raAl: order
 * */


int main(int argc, char* argv[])
{
  int contentLen = 0;
  int longestFileName = 0;
  char **content;
  struct Entry **details;
  int originalLen;
  char option;
  int options_flag = 0;

  while ((option = getopt(argc, argv, "raAl")) != -1)
  {
    switch(option)
    {
      case 'r':
        options_flag |= 8;
        break;
      case 'a':
        options_flag |= 4;
        break;
      case 'A':
        options_flag |= 2;
        break;
      case 'l':
        options_flag |= 1;
        break;
    }
  }

  int lOptionIsSet = options_flag & 1;

  if (lOptionIsSet)
  {
    details = createHT();
    content = getContent(1, &contentLen, &longestFileName, details);
  }
  else 
    content = getContent(0, &contentLen, &longestFileName);

  originalLen = contentLen;

  applyOptions(content, &contentLen, options_flag); // check for -aAr
  listDir(content, contentLen, longestFileName, options_flag); // check for -l

  free2DArray(content, originalLen);
  // if (details not null) freeHT();
  return 0;
}

char** getContent(int containsDetails, int *outputFinalSize, int *longestFileName, ...)
{
  const char *dirname = ".";
  DIR *dir;
  struct dirent *entry;
  int runningMax = 0;
  int index = 0;
  int outputMaxSize = ENTRIES_NUM;
  char* dName; 
  char** output = malloc2DArray(outputMaxSize);

  dir = opendir(dirname);
  if (dir == NULL)
  {
    perror("Error Opening Directory");
    exit(EXIT_FAILURE);
  }

  while ((entry = readdir(dir)) != NULL)
  {
    dName = entry->d_name;
    runningMax = strlen(dName) > runningMax ? strlen(dName) : runningMax;
    
    if (index == outputMaxSize)
      output = realloc2DArray(output, &outputMaxSize);

    strcpy(output[index], dName);

    // if (details exists as an argument)
    //   get file info (+ symlinks)
    //   store it in a single string with | sep 
    //   entry = createEntry(dName, info)
    //   putEntry(table, entry)

    index++;
  }

  if (closedir(dir) == -1)
  {
    perror("Error Closing Directory");
    exit(EXIT_FAILURE);
  }


  *outputFinalSize = index;
  *longestFileName = runningMax;
  return output;
}

void free2DArray(char **array, int end)
{
  for (int i = 0; i < end; i++)
    free(array[i]);

  free(array);
}

char** realloc2DArray(char **output, int *outputMaxSize)
{
  *outputMaxSize <<= 1;
  char **temp = realloc(output, *outputMaxSize * sizeof(char *));

  if (temp == NULL)
  {
    perror("Error allocating memory");
    free2DArray(output, *outputMaxSize >> 1);
    exit(EXIT_FAILURE);
  }

  output = temp;

  for (int i = *outputMaxSize >> 1; i < *outputMaxSize; i++)
  {
    output[i] = malloc(MAX_FILENAME_LENGTH * sizeof(char));

    if (output[i] == NULL)
    {
      perror("Error allocating memory");
      free2DArray(output, i);
      exit(EXIT_FAILURE);
    }
  }

  return output;
}

char** malloc2DArray(int outputMaxSize)
{
  char **output = malloc(outputMaxSize * sizeof(char *));

  if (output == NULL)
  {
    perror("Error allocating memory");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < outputMaxSize; i++)
  {
    output[i] = malloc(MAX_FILENAME_LENGTH * sizeof(char));

    if (output[i] == NULL)
    {
      perror("Error allocating memory");

      free2DArray(output, i);
      exit(EXIT_FAILURE);
    }
  }

  return output;
}

void applyOptions(char **content, int *contentLen, int options)
{
  int r_flag = options & 8;
  int a_flag = options & 4;
  int A_flag = options & 2;


  if (A_flag)
    hideImplied(content, contentLen);
  else if (!a_flag)
    hideHiddenFiles(content, contentLen);

  sort2d(content, *contentLen, r_flag);

  // a A   : hide only . and ..
  // !a A  : hide only . and ..
  // a !A  : do not hide anyting
  // !a !A : hide files beginning with .
}

void listDir(char **content, int contentLen, int longestFileName, int options)
{
  
  int l_flag = options & 1;

  if (l_flag)
  {
    listDetails(content, contentLen, longestFileName);
    return;
  }

  listConcise(content, contentLen, longestFileName);

}

void listDetails(char **content, int contentLen, int longestFileName)
{
}

void listConcise(char **content, int contentLen, int longestFileName)
{
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);  
  int padding = 3;
  int colNum = w.ws_col - 2 * padding; // padding left and right
  int wordColNum = colNum / longestFileName;
  int counter = -1;
}

void sort2d(char **content, int contentLen, int decOrder)
{
  if (!decOrder)  
  {
    qsort(content, contentLen, sizeof(const char*), compare);
    return;
  }

  qsort(content, contentLen, sizeof(const char*), compareReverse);
}

int compare(const void* a, const void* b)
{
  const char **strA = (const char **)a;
  const char **strB = (const char **)b;

  return strcasecmp(*strA, *strB);
}

int compareReverse(const void* a, const void* b)
{
  const char **strA = (const char **)a;
  const char **strB = (const char **)b;

  return -(strcasecmp(*strA, *strB));
}

void hideImplied(char **content, int *contentLen)
{
  int i = -1;
  int j = *contentLen - 1;
  int count = 0;

  while (++i < j)
  {

    if ((strcmp(content[i], ".") == 0) || (strcmp(content[i], "..")) == 0)
    {
      switchRows(content, i, j);
      j--;
      count++;
    }

    if (count == 2)
      break;
  }

  *contentLen -= count;
}

void hideHiddenFiles(char **content, int *contentLen)
{
  int i = -1;
  int j = *contentLen - 1;
  int count = 0;

  while (++i < j)
    if (content[i][0] == '.')
    {
      switchRows(content, i, j);
      j--;
      count++;
    }

  *contentLen -= count;
}

void switchRows(char **content, int i, int j)
{
  char temp[MAX_FILENAME_LENGTH];

  strcpy(temp, content[i]);
  strcpy(content[i], content[j]);
  strcpy(content[j], temp);
}
