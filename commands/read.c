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

int _read(char **cmd) {

    int argc = argCount(cmd, -1);

    if (argc < 1) {
        perror("msh: read: ingrese como mínimo un nombre de variable.");
        return(1);
    }

    char line[MAX_LINE_LEN];
    printf("Ingrese los valores a asignar a la(s) variable(s):\n");
    scanf("%s", line);

    // Tokenización de la línea ingresada
    const char *delimiters = " ";
    char *args[MAX_READ_ARGS];
    char *token = strtok(line, delimiters);
    int index = 0;

    while(token != NULL && index < MAX_READ_ARGS) {
        args[++index] = token;
        printf("%s\n", token);
        token = strtok(NULL, delimiters);
    }

    return 0;
}