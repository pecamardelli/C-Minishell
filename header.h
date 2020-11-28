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
#include <sys/times.h>
#include <time.h>

#define MAX_PATH_LEN    512
#define MAX_LINE_LEN    256
#define MAX_READ_ARGS   32

/* Funciones del núcleo y utilidades */
int commandPipeline (char ***argvv, int argvc);
int newCommandProc(int in, int out, char **cmd);
int commandSelector(char ***argvv, int argvc);

/* Funciones de mandatos específicos */
int _cd(char **dir);
int _umask(char **cmd);
int _times(char **cmd);
int _read(char **cmd);

/* Utilidades */
int argCount(char **cmd, int expectedArgs);

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