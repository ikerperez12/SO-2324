// commands.h
#ifndef COMMANDS_H
#define COMMANDS_H
#include <sys/stat.h>

#define MAX_COMMAND_SIZE 256
#define MAX_HISTORY_SIZE 4096
#define MAX_ARG_SIZE 64

typedef struct {
    int num_args;
    char args[MAX_ARG_SIZE][MAX_COMMAND_SIZE]; // Cambio en la declaración de args
    char command[MAX_COMMAND_SIZE];
} Command;

extern Command history[MAX_HISTORY_SIZE];
extern int history_size;

void execute_command(Command* cmd);

void authors(Command* cmd);
void pid(Command* cmd);
void chdir_command(Command* cmd);
void date(void);
void time_command(void);
void hist(Command* cmd);
void comand(Command* cmd);
void open_command(Command* cmd);
void close_command(Command* cmd);
void dup_command(Command* cmd);
void listopen(void);
void infosys(void);
void help(Command* cmd);


void create_command(Command* cmd);
void stat_command(Command* cmd);
void list_command(Command* cmd);
void delete_command(Command* cmd);
void deltree_command(Command* cmd);

// Función de conversión de modo
char* ConvierteModo2(mode_t m);

#endif /* COMMANDS_H */
