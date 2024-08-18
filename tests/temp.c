#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>
#include "../src/HashTable.c"

void print_file_info(const char *filename) {
  struct stat file_stat;
  struct passwd *pw;
  struct group *gr;
  char time_str[20];

  // Get file information using stat
  if (stat(filename, &file_stat) < 0) {
    perror("stat");
    return;
  }

  // Print file type and permissions
  printf( (S_ISDIR(file_stat.st_mode)) ? "d" : "-");
  printf( (file_stat.st_mode & S_IRUSR) ? "r" : "-");
  printf( (file_stat.st_mode & S_IWUSR) ? "w" : "-");
  printf( (file_stat.st_mode & S_IXUSR) ? "x" : "-");
  printf( (file_stat.st_mode & S_IRGRP) ? "r" : "-");
  printf( (file_stat.st_mode & S_IWGRP) ? "w" : "-");
  printf( (file_stat.st_mode & S_IXGRP) ? "x" : "-");
  printf( (file_stat.st_mode & S_IROTH) ? "r" : "-");
  printf( (file_stat.st_mode & S_IWOTH) ? "w" : "-");
  printf( (file_stat.st_mode & S_IXOTH) ? "x" : "-");

  // Print number of links
  printf(" %ld", (long)file_stat.st_nlink);

  // Print owner and group names
  pw = getpwuid(file_stat.st_uid);
  gr = getgrgid(file_stat.st_gid);
  printf(" %s %s", pw->pw_name, gr->gr_name);

  // Print file size
  printf(" %ld", (long)file_stat.st_size);

  // Print last modification time
  strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));
  printf(" %s", time_str);

  // Print filename
  printf(" %s\n", filename);
}

int main(int argc, char *argv[]) {
  DIR *dir;
  struct dirent *entry;

  // Open the directory
  dir = opendir(".");
  if (dir == NULL) {
    perror("opendir");
    return 1;
  }

  // Read each entry in the directory
  while ((entry = readdir(dir)) != NULL) {
    print_file_info(entry->d_name);
  }

  // Close the directory
  closedir(dir);
  return 0;
}

