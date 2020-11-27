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

unsigned char checkUmask(char *mask) {
    // Más de 4 dígitos.
    if (strlen(mask) > 4) return 0;

    // Convertimos el caracter a entero mediante su diferencia con '0'.
    // Luego chequeamos que no sea mayor a 7.
    for (int i=1;i<=4; i++) {
        if ((mask[i] - '0') > 7) return 0;
    }

    return 255;
}