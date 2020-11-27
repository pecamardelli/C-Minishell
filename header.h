#ifndef HEADER_H
#define HEADER_H

#include <stddef.h>			/* NULL */
#include <stdio.h>			/* setbuf, printf */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAX_PATH_LEN    512

/* Funciones del núcleo y utilidades */
int commandPipeline (char ***argvv, int argvc);
int newCommandProc(int in, int out, char **cmd);
int commandSelector(char **cmd);

/* Funciones de mandatos específicos */
int cd(char **dir);

/* Variables globales */
int mandatopid = -1;
const char *internalCommands[] = {
    "cd",
    "umask",
    "time",
    "read",
    "exit",
    "quit"
};

#endif