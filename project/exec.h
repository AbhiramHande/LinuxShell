#ifndef EXEC_H
#define EXEC_H

#define _POSIX_C_SOURCE 200809L
#define CMD_LIST_SIZE 5

#include <fcntl.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/types.h>

int exec_command(char** arg_list, long* arg_count);
char* listen(FILE* stream);
#endif