/*-
 * msh.c
 * Minishell C source
 * Shows how to use "obtain_order" input interface function.
 *
 * Copyright (c) 1993-2002-2019, Francisco Rosales <frosal@fi.upm.es>
 * Todos los derechos reservados.
 *
 * Publicado bajo Licencia de Proyecto Educativo Práctico
 * <http://laurel.datsi.fi.upm.es/~ssoo/LICENCIA/LPEP>
 *
 * Queda prohibida la difusión total o parcial por cualquier
 * medio del material entregado al alumno para la realización
 * de este proyecto o de cualquier material derivado de este,
 * incluyendo la solución particular que desarrolle el alumno.
 *
 * DO NOT MODIFY ANYTHING OVER THIS LINE
 * THIS FILE IS TO BE MODIFIED
 */

#include "../header.h"

int commandSelector(char **cmd) {
    if (strcmp(cmd[0], "cd") == 0) cd(cmd);
    else if (strcmp(cmd[0], "exit") == 0) exit(0);
    else if (strcmp(cmd[0], "quit") == 0) exit(0);
    else {
        if (fork() == 0) {
            return execvp(cmd[0], (char* const*)cmd);
        }
    }
    return 0;
}