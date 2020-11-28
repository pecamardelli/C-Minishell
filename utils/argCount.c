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

int argCount(char **cmd, int expectedArgs) {
    // Conteo de argumentos.
    int argc = -1;
    while(cmd[++argc]) {}

    if(expectedArgs >= 0 && argc > expectedArgs) {
        char msg[32];
        sprintf(msg, "msh: %s: demasiados argumentos.\n", cmd[0]);
        perror(msg);
		return -1;
    }

    // Restamos 1 dado que el primer elemento del array cmd es
    // el nombre del comando.
    return argc - 1;
}