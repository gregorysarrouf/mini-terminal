#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdarg.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include <dirent.h>
#include "../include/ls.h"

#define MAX_FILENAME_LENGTH 256
#define MAX_INFO_LENGTH 1024
#define ENTRIES_NUM 25

/*READ ME
 *
 *
 * -a: do not ignore files starting with . 
 * -A: ignore only the obvious (., ..)
 * -l: long listing
 * -r: sort in reverse order
 *  raAl: order
 *
 *  !!! contains memory leak somewhere !!!
 * */


int main(int argc, char* argv[])
{
  int contentLen = 0;
  int longestFileName = 0;
  int rowsToFree = 0;
  char **content = getContent(&contentLen, &longestFileName, &rowsToFree);
  int originalLen = contentLen;
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

  applyOptions(content, &contentLen, options_flag);
  listDir(content, contentLen, longestFileName, options_flag);

  free2DArray(content, rowsToFree);
  return 0;
}

char** getContent(int *outputFinalSize, int *longestFileName, int *rowsToFree)
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
    struct stat file_stat;

    if (stat(entry->d_name, &file_stat) < 0)
      perror("stat");

    dName = entry->d_name;
    if (S_ISDIR(file_stat.st_mode))
      strcat(dName, "/");

    runningMax = strlen(dName) > runningMax ? strlen(dName) : runningMax;
    
    if (index == outputMaxSize)
      output = realloc2DArray(output, &outputMaxSize);

    strcpy(output[index], dName);

    index++;
  }

  if (closedir(dir) == -1)
  {
    perror("Error Closing Directory");
    exit(EXIT_FAILURE);
  }


  *outputFinalSize = index;
  *rowsToFree = outputMaxSize;
  *longestFileName = runningMax;
  return output;
}

void free2DArray(char **array, int end)
{
  for (int i = 0; i < end; i++)
  {
    free(array[i]);
    printf("freed row\n");
  }

  free(array);
  printf("freed array\n");
}

char** realloc2DArray(char **output, int *outputMaxSize)
{
  *outputMaxSize <<= 1;
  char **temp = realloc(output, *outputMaxSize * sizeof(char *));
  printf("realloc array\n");

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
    printf("malloc new array row\n");

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
  printf("malloc array\n");

  if (output == NULL)
  {
    perror("Error allocating memory");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < outputMaxSize; i++)
  {
    output[i] = malloc(MAX_FILENAME_LENGTH * sizeof(char));
    printf("malloc array row\n");

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

  printf("\n");

  if (l_flag)
  {
    listDetails(content, contentLen, longestFileName);
    printf("\n");
    return;
  }

  listConcise(content, contentLen, longestFileName);

  printf("\n");

}

void listDetails(char **content, int contentLen, int longestFileName)
{
  char details[contentLen][MAX_FILENAME_LENGTH + 200];
  char formatting[2][100];
  char linkTemp[sizeof(int)];
  char sizeTemp[sizeof(int)];

  for (int i = 0; i < contentLen; i++)
  {
    struct stat file_stat;
    struct passwd *pw;
    struct group *gr;
    char time_str[20];
    char *filename = content[i];
    strcpy(details[i], "");


    // Get file information using stat
    if (stat(filename, &file_stat) < 0) {
      perror("stat");
      return;
    }

    // Print file type and permissions
    strcat(details[i], (S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    strcat(details[i], (file_stat.st_mode & S_IRUSR) ? "r" : "-");
    strcat(details[i], (file_stat.st_mode & S_IWUSR) ? "w" : "-");
    strcat(details[i], (file_stat.st_mode & S_IXUSR) ? "x" : "-");
    strcat(details[i], (file_stat.st_mode & S_IRGRP) ? "r" : "-");
    strcat(details[i], (file_stat.st_mode & S_IWGRP) ? "w" : "-");
    strcat(details[i], (file_stat.st_mode & S_IXGRP) ? "x" : "-");
    strcat(details[i], (file_stat.st_mode & S_IROTH) ? "r" : "-");
    strcat(details[i], (file_stat.st_mode & S_IWOTH) ? "w" : "-");
    strcat(details[i], (file_stat.st_mode & S_IXOTH) ? "x" : "-");
    strcat(details[i], "|");

    int links = file_stat.st_nlink; 

    strcat(details[i], intToString(linkTemp, links));
    strcat(details[i], "|");
    if (atoi(formatting[0]) < atoi(linkTemp));
      strcpy(formatting[0], linkTemp);

    // Print owner and group names
    pw = getpwuid(file_stat.st_uid);
    gr = getgrgid(file_stat.st_gid);

    strcat(details[i], pw->pw_name);
    strcat(details[i], "|");
    strcat(details[i], gr->gr_name);
    strcat(details[i], "|");

    int size = file_stat.st_size; 

    strcat(details[i], intToString(sizeTemp, size));
    strcat(details[i], "|");
    if (atoi(formatting[1]) < atoi(sizeTemp))
      strcpy(formatting[1], sizeTemp);


    strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));
    strcat(details[i], time_str);
    strcat(details[i], "|");

    strcat(details[i], filename);
  }

  for (int i = 0; i < contentLen; i++)
  {
    int counter = 0; 
    char *token = strtok(details[i], "|");
    char *padding = "  ";

    printf("%s", padding);

    while (token != NULL)
    {
      if ((counter == 1) && (strlen(formatting[0]) > strlen(token)))
        for (int j = 0; j < strlen(formatting[0]) - strlen(token); j++)
          printf(" ");

      if ((counter == 4) && (strlen(formatting[1]) > strlen(token)))
        for (int j = 0; j < strlen(formatting[1]) - strlen(token); j++)
          printf(" ");

      if (token[strlen(token) - 1] == '/')
        token[strlen(token) - 1] = '\0';

      printf("%s ", token);
      token = strtok(NULL, "|");
      counter++;
    }
    printf("\n");

  }
}

void listConcise(char **content, int contentLen, int longestFileName)
{
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);  
  int numRows = contentLen / 2;
  char *padding = "  ";
  int i, j;

  for (i = 0; i < numRows; i++)
  {
    int spaces = longestFileName - strlen(content[i]);
    printf("%s", padding);
    printf("%s", content[i]);


    if (i != numRows - 1 || contentLen % 2 == 0)
    {
      for (j = 0; j < spaces; j++)
        printf(" ");

      printf("  ");

      printf("%s\n", content[numRows + i]);
    }
  }
  
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

char* intToString(char *dst, int num)
{
  int i = 0;

  while (num > 0)
  {
    dst[i++] = num % 10 + '0';
    num /= 10;
  } 

  dst[i] = '\0';

  if (strlen(dst) == 0)
  {
    dst[0] = '0';
    dst[1] = '\0';
  }

  for (int j = 0, k = i - 1; j < k; j++, k--) {
    char temp = dst[j];
    dst[j] = dst[k];
    dst[k] = temp;
  }

  
    return dst;
}
