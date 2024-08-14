#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void execute_echo(const char *input) {
    // Simulates the built-in 'echo' command by writing to stdout
    write(STDOUT_FILENO, input, strlen(input));
}

void execute_wc() {
    // Simulates the 'wc -c' command by reading from stdin and counting bytes
    char buffer[1024];
    ssize_t n;
    int count = 0;

    while ((n = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        count += n;
    }

    printf("%d\n", count);
}

int main() {
    int pipefd[2];
    pid_t pid;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Child Process\n");
        // Child process

        // Close the unused read end of the pipe
        close(pipefd[0]);

        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);

        // Close the write end of the pipe (it's now duped)
        close(pipefd[1]);

        // Execute the first "internal" command (echo)
        //execute_echo("12\n");
        execlp("ls", "ls", NULL);

        // Exit the child process
        exit(EXIT_SUCCESS);
    } else {

        printf("Parent Process\n");
        // Parent process

        // Close the unused write end of the pipe
        close(pipefd[1]);

        // Redirect stdin to the read end of the pipe
        dup2(pipefd[0], STDIN_FILENO);

        // Close the read end of the pipe (it's now duped)
        close(pipefd[0]);

        // Execute the second "internal" command (wc -c)
        execute_wc();

        // Wait for the child process to finish
        wait(NULL);
    }

    return 0;
}
