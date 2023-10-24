#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_COMMAND_SIZE 256
#define MAX_HISTORY_SIZE 4096
#define MAX_ARG_SIZE 64

typedef struct {
    int num_args;
    char* args[MAX_ARG_SIZE];
    char command[MAX_COMMAND_SIZE];
} Command;


void execute_command(Command* cmd);



extern Command history[MAX_HISTORY_SIZE];
extern int history_size;

void authors(Command* cmd);
void pid(Command* cmd);
void chdir_command(Command* cmd);
void date(Command* cmd);
void time_command(Command* cmd);
void hist(Command* cmd);
void comand(Command* cmd);
void open_command(Command* cmd);
void close_command(Command* cmd);
void dup_command(Command* cmd);
void listopen(Command* cmd);
void infosys(Command* cmd);
void help(Command* cmd);

#endif /* COMMANDS_H */
