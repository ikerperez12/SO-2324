// TITLE: Sistemas Operativos Práctica 0
// AUTHOR 1: Iker Jesús Perez García  LOGIN 1: iker.perez@udc.es
// AUTHOR 2: Diego Losada  Gómez LOGIN 2: diego.lgomez@udc.es
// GROUP: 4.2
// DATE: 29/09/2023


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <unistd.h> #include <sys/types.h> #include <fcntl.h> #include <time.h>
#include "commands.h"

#define MAX_INPUT_SIZE 1024
#define MAX_HISTORY_SIZE 4096
#define MAX_ARG_SIZE 64
#define MAX_COMMAND_SIZE 256

// Variables globales para el historial de comandos
Command history[MAX_HISTORY_SIZE];
int history_size = 0;

void print_prompt() {
    printf("shell> ");
    fflush(stdout);
}

void tokenize_input(char* input, Command* cmd) {
    char* token = strtok(input, " \t\n");
    if (token == NULL) {
        strcpy(cmd->command, "");
        cmd->num_args = 0;
        return;
    }

    strcpy(cmd->command, token);
    cmd->num_args = 0;

    while ((token = strtok(NULL, " \t\n")) != NULL && cmd->num_args < MAX_ARG_SIZE) {
        strcpy(cmd->args[cmd->num_args], token);
        cmd->num_args++;
    }
}

void execute_command(Command* cmd) {
    if (strcmp(cmd->command, "authors") == 0) {
        authors(cmd);
    } else if (strcmp(cmd->command, "pid") == 0) {
        pid(cmd);
    } else if (strcmp(cmd->command, "chdir") == 0) {
        chdir_command(cmd);
    } else if (strcmp(cmd->command, "date") == 0) {
        date(cmd);
    } else if (strcmp(cmd->command, "time") == 0) {
        time_command(cmd);
    } else if (strcmp(cmd->command, "hist") == 0) {
        hist(cmd);
    } else if (strcmp(cmd->command, "comand") == 0) {
        comand(cmd);
    } else if (strcmp(cmd->command, "open") == 0) {
        open_command(cmd);
    } else if (strcmp(cmd->command, "close") == 0) {
        close_command(cmd);
    } else if (strcmp(cmd->command, "dup") == 0) {
        dup_command(cmd);
    } else if (strcmp(cmd->command, "listopen") == 0) {
        listopen(cmd);
    } else if (strcmp(cmd->command, "infosys") == 0) {
        infosys(cmd);
    } else if (strcmp(cmd->command, "help") == 0) {
        help(cmd);
    } else if (strcmp(cmd->command, "quit") == 0 || strcmp(cmd->command, "exit") == 0 || strcmp(cmd->command, "bye") == 0) {
        exit(0);
    } else {
        printf("Unknown command: %s\n", cmd->command);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        print_prompt();

        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            perror("fgets");
            exit(1);
        }

        // Formato
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        // Trocear entrada
        Command cmd;
        tokenize_input(input, &cmd);

        // Guarda en el historial (si hay espacio)
        if (history_size < MAX_HISTORY_SIZE) {
            history[history_size] = cmd;
            history_size++;
        } else {
            // Si el historial está lleno, elimina el comando más antiguo
            for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
                history[i] = history[i + 1];
            }
            history[MAX_HISTORY_SIZE - 1] = cmd;
        }

        // Ejecuta el comando
        execute_command(&cmd);
    }
}
