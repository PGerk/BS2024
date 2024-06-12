#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include "plist.h"

#define MAX_INPUT_LEN 1337

void execute_command(char *command, int background);
void handle_background_processes();
bool print_job(pid_t pid, const char *cmdLine);

int main() {
    char cwd[PATH_MAX];
    char input[MAX_INPUT_LEN];

    while (1) {
        //Display working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s: ", cwd);
        } else {
            perror("Couldn't read current working directory");
            exit(EXIT_FAILURE);
        }

        //Read input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            //End of file / STRG+D
            if (feof(stdin)) {
                break;
            } else {
                perror("Input Error");
                clearerr(stdin);
                continue;
            }
        }

        //Process input
        char *command = strtok(input, "\n");
        if (command == NULL) {
            continue;
        }

        int background = 0;
        if (command[strlen(command) - 1] == '&') {
            background = 1;
            command[strlen(command) - 1] = '\0';
        }

        execute_command(command, background);

        handle_background_processes();
    }

    fflush(stdout);
    return 0;
}

void execute_command(char *command, int background) {
    //Let's just assume a somewhat sane, but high, number of arguments
    char *args[MAX_INPUT_LEN / 2 + 1];
    int i = 0;
    //Split along Space and Tab
    args[i] = strtok(command, " \t");
    while (args[i] != NULL && i < MAX_INPUT_LEN / 2) {
        i++;
        args[i] = strtok(NULL, " \t");
    }

    //No command supplied
    if (args[0] == NULL) {
        return;
    }

    //cd command
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd needs an argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        return;
    }

    //jobs command
    if (strcmp(args[0], "jobs") == 0) {
        walkList(print_job);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return;
    }

    if (pid == 0) {
        //Child
        if (execvp(args[0], args) == -1) {
            perror("Execution failed");
        }
        exit(EXIT_FAILURE);
    } else {
        //Parent
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
            printf("Exit status [%s] = %d\n", command, WEXITSTATUS(status));
        } else {
            if (insertElement(pid, command) < 0) {
                fprintf(stderr, "Element insertion error\n");
            }
        }
    }
}

void handle_background_processes() {
    pid_t pid;
    int status;
    char cmd[256];

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (removeElement(pid, cmd, sizeof(cmd)) > 0) {
            printf("Finished: PID %d, command [%s], exit status %d\n", pid, cmd, WEXITSTATUS(status));
        }
    }
}

bool print_job(pid_t pid, const char *cmdLine) {
    printf("PID: %d, command: %s\n", pid, cmdLine);
    return false;
}
