#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char** parse_command(const char* cmd_line);

#endif