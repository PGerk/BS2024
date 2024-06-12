#define _POSIX_C_SOURCE 200809L
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
        // Aktuelles Arbeitsverzeichnis als Prompt anzeigen
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s: ", cwd);
        } else {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }

        // Eingabe lesen
        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) {
                break; // EOF (Ctrl-D)
            } else {
                perror("fgets");
                clearerr(stdin); // Fehlerstatus zurücksetzen
                continue;
            }
        }

        // Eingabe verarbeiten und in Kommandonamen und Argumente zerlegen
        char *command = strtok(input, "\n"); // Entferne das neue Zeilenzeichen
        if (command == NULL) {
            continue;
        }

        int background = 0;
        if (command[strlen(command) - 1] == '&') {
            background = 1;
            command[strlen(command) - 1] = '\0';
        }

        // Befehl ausführen
        execute_command(command, background);

        // Beendete Hintergrundprozesse aufräumen
        handle_background_processes();
    }

    fflush(stdout);
    return 0;
}

void execute_command(char *command, int background) {
    char *args[MAX_INPUT_LEN / 2 + 1]; // Angenommen, maximal die Hälfte der Eingabe besteht aus Argumenten
    int i = 0;
    args[i] = strtok(command, " \t");
    while (args[i] != NULL && i < MAX_INPUT_LEN / 2) {
        i++;
        args[i] = strtok(NULL, " \t");
    }

    if (args[0] == NULL) {
        return; // Keine Eingabe
    }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: fehlendes Argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        return;
    }

    if (strcmp(args[0], "jobs") == 0) {
        // Funktion zur Anzeige der Hintergrundprozesse aufrufen
        walkList(print_job);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Kindprozess
        if (execvp(args[0], args) == -1) {
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    } else {
        // Elternprozess
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
            printf("Exitstatus [%s] = %d\n", command, WEXITSTATUS(status));
        } else {
            if (insertElement(pid, command) < 0) {
                fprintf(stderr, "Fehler beim Hinzufügen des Hintergrundprozesses\n");
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
            printf("Hintergrundprozess beendet: PID %d, Befehl [%s], Exitstatus %d\n", pid, cmd, WEXITSTATUS(status));
        }
    }
}

bool print_job(pid_t pid, const char *cmdLine) {
    printf("PID: %d, Befehl: %s\n", pid, cmdLine);
    return false;
}
