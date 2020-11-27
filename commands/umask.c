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
#include "../utils/checkUmask.c"

int _umask(char **cmd) {
    mode_t oldMask;
    // Se espera que la cantidad de argumentos más el nombre del comando
    // sea igual a 2.
    int maxArgs = 2;

    // Conteo de argumentos.
    int argc = 0;
    while(cmd[++argc]) {}


    if(argc > maxArgs) {
        perror("msh: umask: demasiados argumentos.\n");
		return 1;
    }

    if (cmd[1]) {
        if(!checkUmask(cmd[1])) {
            perror("msh: umask: formato de máscara inválido.\n");
            return 1;
        }

        unsigned int mask = strtol(cmd[1], NULL, 8);

        umask(mask);
        printf("Valor de la máscara cambiado a 0%o\n", mask);
    }
    else {
        oldMask = umask(S_IRWXG);
        printf("0%o\n", oldMask);
        umask(oldMask);
    }

    return 0;
}