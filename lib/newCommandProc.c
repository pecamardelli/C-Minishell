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

int newCommandProc(int in, int out, char **cmd) {
	pid_t pid;

	if ((pid = fork()) == 0) {
		if (in != 0) {
			dup2(in, 0);
			close(in);
		}

		if (out != 1) {
			dup2(out, 1);
			close(out);
		}

		return execvp(cmd[0], (char* const*)cmd);
	}

	return pid;
}
