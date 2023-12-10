#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void free_cmds(char **cmds);
char **tokenize(char *str, const char *delim);
void get_cmd_path(char **command);
extern char **environ;

#endif
