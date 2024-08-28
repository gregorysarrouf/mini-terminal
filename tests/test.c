#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
  char *args[] = { "/usr/bin/firefox", NULL };
  char *envp[] = {
    "DISPLAY=:0",
    "PATH=/bin:/usr/bin",
    "XAUTHORITY=/home/greg/.Xauthority",
    NULL
  };

  execve("/usr/bin/firefox", args, envp);

  return 0;
}
