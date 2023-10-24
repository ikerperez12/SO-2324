// commands.c
#include <sys/stat.h>
#include <stdio.h>
#ifdef __linux__
#include <sys/utsname.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include "commands.h"





// Estructura para gestionar los archivos abiertos
typedef struct {
    int fd;
    char filename[MAX_COMMAND_SIZE];
    char mode[MAX_COMMAND_SIZE];
} OpenFile;

OpenFile open_files[MAX_HISTORY_SIZE]; // Almacenar archivos abiertos
int open_files_count = 0;

void authors(Command* cmd) {
    if (cmd->num_args == 0) {
        printf("Authors: Iker Jesús Perez García, Diego Losada Gómez\n");
    } else if (cmd->num_args == 1 && strcmp(cmd->args[0], "-l") == 0) {
        printf("Logins: iker.perez@udc.es , diego.lgomez@udc.es \n");
    } else if (cmd->num_args == 1 && strcmp(cmd->args[0], "-n") == 0) {
        printf("Names: Iker Perez, Diego Losada \n");
    } else {
        printf("Invalid usage of authors command.\n");
    }
}

void pid(Command* cmd) {
    if (cmd->num_args == 0) {
        printf("PID of the shell: %d\n", getpid());
    } else if (cmd->num_args == 1 && strcmp(cmd->args[0], "-p") == 0) {
        printf("Parent PID of the shell: %d\n", getpid());
    } else {
        printf("Invalid usage of pid command.\n");
    }
}

void chdir_command(Command* cmd) {
    if (cmd->num_args == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current directory: %s\n", cwd);
        } else {
            perror("getcwd");
        }
    } else if (cmd->num_args == 1) {
        if (chdir(cmd->args[0]) == 0) {
            printf("Changed directory to: %s\n", cmd->args[0]);
        } else {
            perror("chdir");
        }
    } else {
        printf("Invalid usage of chdir command.\n");
    }
}

void date(Command* cmd) {
    time_t t;
    struct tm* info;
    time(&t);
    info = localtime(&t);
    printf("Current date: %02d/%02d/%04d\n", info->tm_mday, info->tm_mon + 1, info->tm_year + 1900);
}

void time_command(Command* cmd) {
    time_t t;
    struct tm* info;
    time(&t);
    info = localtime(&t);
    printf("Current time: %02d:%02d:%02d\n", info->tm_hour, info->tm_min, info->tm_sec);
}

void hist(Command* cmd) {
    if (cmd->num_args == 0) {
        // List all commands in history
        for (int i = 0; i < history_size; i++) {
            printf("%d: %s", i, history[i].command);
            for (int j = 0; j < history[i].num_args; j++) {
                printf(" %s", history[i].args[j]);
            }
            printf("\n");
        }
    } else if (cmd->num_args == 1 && strcmp(cmd->args[0], "-c") == 0) {
        // Clear the history
        history_size = 0;
        printf("Command history cleared.\n");
    } else if (cmd->num_args == 2 && strcmp(cmd->args[0], "-N") == 0) {
        // List the first N commands in history
        int n = atoi(cmd->args[1]);
        if (n <= 0 || n > history_size) {
            printf("Invalid argument for hist -N.\n");
            return;
        }
        for (int i = 0; i < n; i++) {
            printf("%d: %s", i, history[i].command);
            for (int j = 0; j < history[i].num_args; j++) {
                printf(" %s", history[i].args[j]);
            }
            printf("\n");
        }
    } else {
        printf("Invalid usage of hist command.\n");
    }
}

void comand(Command* cmd) {
    if (cmd->num_args == 1) {
        int n = atoi(cmd->args[0]);
        if (n >= 0 && n < history_size) {
            execute_command(&history[n]);
        } else {
            printf("Invalid argument for comand.\n");
        }
    } else {
        printf("Invalid usage of comand command.\n");
    }
}

void open_command(Command* cmd) {
    if (cmd->num_args == 2) {
        const char* filename = cmd->args[0];
        const char* mode = cmd->args[1];

        int flags = 0;
        if (strcmp(mode, "cr") == 0) {
            flags = O_CREAT | O_RDWR;
        } else if (strcmp(mode, "ap") == 0) {
            flags = O_APPEND | O_RDWR;
        } else if (strcmp(mode, "ex") == 0) {
            flags = O_CREAT | O_EXCL | O_RDWR;
        } else if (strcmp(mode, "ro") == 0) {
            flags = O_RDONLY;
        } else if (strcmp(mode, "rw") == 0) {
            flags = O_RDWR;
        } else if (strcmp(mode, "wo") == 0) {
            flags = O_WRONLY;
        } else if (strcmp(mode, "tr") == 0) {
            flags = O_CREAT | O_TRUNC | O_RDWR;
        } else {
            printf("Invalid mode for open command.\n");
            return;
        }

        int fd = open(filename, flags, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("open");
            return;
        }

        // Add the file to the list of open files
        if (open_files_count < MAX_HISTORY_SIZE) {
            OpenFile new_open_file;
            new_open_file.fd = fd;
            strcpy(new_open_file.filename, filename);
            strcpy(new_open_file.mode, mode);
            open_files[open_files_count] = new_open_file;
            open_files_count++;
            printf("Opened file %s with mode %s (fd: %d)\n", filename, mode, fd);
        } else {
            printf("Maximum number of open files reached.\n");
            close(fd);
        }
    } else {
        printf("Invalid usage of open command.\n");
    }
}

void close_command(Command* cmd) {
    if (cmd->num_args == 1) {
        int df = atoi(cmd->args[0]);
        if (df >= 0 && df < open_files_count) {
            int fd_to_close = open_files[df].fd;
            if (close(fd_to_close) == 0) {
                printf("Closed file descriptor %d\n", df);
                // Remove the closed file from the list
                for (int i = df; i < open_files_count - 1; i++) {
                    open_files[i] = open_files[i + 1];
                }
                open_files_count--;
            } else {
                perror("close");
            }
        } else {
            printf("Invalid file descriptor for close command.\n");
        }
    } else {
        printf("Invalid usage of close command.\n");
    }
}

void dup_command(Command* cmd) {
    if (cmd->num_args == 1) {
        int df = atoi(cmd->args[0]);
        if (df >= 0 && df < open_files_count) {
            int new_fd = dup(open_files[df].fd);
            if (new_fd == -1) {
                perror("dup");
            } else {
                // Add the duplicated file to the list
                if (open_files_count < MAX_HISTORY_SIZE) {
                    OpenFile new_open_file;
                    new_open_file.fd = new_fd;
                    strcpy(new_open_file.filename, open_files[df].filename);
                    strcpy(new_open_file.mode, open_files[df].mode);
                    open_files[open_files_count] = new_open_file;
                    open_files_count++;
                    printf("Duplicated file descriptor %d to %d\n", df, new_fd);
                } else {
                    printf("Maximum number of open files reached.\n");
                    close(new_fd);
                }
            }
        } else {
            printf("Invalid file descriptor for dup command.\n");
        }
    } else {
        printf("Invalid usage of dup command.\n");
    }
}

void listopen(Command* cmd) {
    printf("Open Files:\n");
    for (int i = 0; i < open_files_count; i++) {
        printf("Descriptor: %d, File: %s, Mode: %s\n", i, open_files[i].filename, open_files[i].mode);
    }
}

void infosys(Command* cmd) {
#ifdef __linux__
    struct utsname info;
    if (uname(&info) != -1) {
        printf("System Information:\n");
        printf("Operating System: %s\n", info.sysname);
        printf("Node Name: %s\n", info.nodename);
        printf("Release: %s\n", info.release);
        printf("Version: %s\n", info.version);
        printf("Machine: %s\n", info.machine);
    } else {
        perror("uname");
    }
#else
    printf("System information is not available on Windows.\n");
#endif
}

void help(Command* cmd) {
    if (cmd->num_args == 0) {
        printf("Available Commands:\n");
        printf("authors [-l|-n]\n");
        printf("pid [-p]\n");
        printf("chdir [dir]\n");
        printf("date\n");
        printf("time\n");
        printf("hist [-c|-N]\n");
        printf("comand N\n");
        printf("open [file] mode\n");
        printf("close [df]\n");
        printf("dup [df]\n");
        printf("listopen\n");
        printf("infosys\n");
        printf("help [cmd]\n");
        printf("quit\n");
        printf("exit\n");
        printf("bye\n");
    } else if (cmd->num_args == 1) {
        char* command_name = cmd->args[0];
        if (strcmp(command_name, "authors") == 0) {
            printf("authors [-l|-n]: Prints the names and logins of the program authors.\n");
        } else if (strcmp(command_name, "pid") == 0) {
            printf("pid [-p]: Prints the PID of the shell process or its parent's PID.\n");
        } else if (strcmp(command_name, "chdir") == 0) {
            printf("chdir [dir]: Changes the current working directory to 'dir' or prints the current directory.\n");
        } else if (strcmp(command_name, "date") == 0) {
            printf("date: Prints the current date in the format DD/MM/YYYY.\n");
        } else if (strcmp(command_name, "time") == 0) {
            printf("time: Prints the current time in the format hh:mm:ss.\n");
        } else if (strcmp(command_name, "hist") == 0) {
            printf("hist [-c|-N]: Shows or clears the command history or lists the first N commands.\n");
        } else if (strcmp(command_name, "comand") == 0) {
            printf("comand N: Repeats command number N from the command history.\n");
        } else if (strcmp(command_name, "open") == 0) {
            printf("open [file] mode: Opens a file with the specified mode.\n");
        } else if (strcmp(command_name, "close") == 0) {
            printf("close [df]: Closes a file descriptor.\n");
        } else if (strcmp(command_name, "dup") == 0) {
            printf("dup [df]: Duplicates a file descriptor.\n");
        } else if (strcmp(command_name, "listopen") == 0) {
            printf("listopen: Lists open files and their descriptors.\n");
        } else if (strcmp(command_name, "infosys") == 0) {
            printf("infosys: Prints information about the system.\n");
        } else if (strcmp(command_name, "help") == 0) {
            printf("help [cmd]: Displays help information for a specific command.\n");
        } else if (strcmp(command_name, "quit") == 0 || strcmp(command_name, "exit") == 0 || strcmp(command_name, "bye") == 0) {
            printf("quit, exit, bye: Exits the shell.\n");
        } else {
            printf("Unknown command: %s\n", command_name);
        }
    } else {
        printf("Invalid usage of help command.\n");
    }
}
