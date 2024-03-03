#ifndef MYBASH_H
#define MYBASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int my_bash();
int execute_command(char *args[]);

#endif
