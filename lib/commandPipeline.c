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

/*	Implementación de la cadena de mandatos.  */
int commandPipeline (char ***argvv, int argvc) {
	int i;
	int in, fd[2];

	/* El primer proceso debe recibir su entrada del descriptor original 0. */
	in = 0;

	/* Aquí se genera el árbol de procesos y tuberías para cada comando
	a excepción del último.  */
	if (fork() == 0) {

		for (i = 0; i < argvc-1; ++i) {
			pipe(fd);

			/* 	Aquí pasamos los siguientes argumentos:
					in:			El descriptor de lectura del proceso anterior para que sea utilizado
								como entrada por el nuevo proceso.
					fd[1]:		El descriptor de escritura por defecto.
					argvv[i]:	El puntero al array de caracteres que contiene el comando
								a ejecutar y sus argumentos.
				
				Nótese que f[1] es el "write end" de la tubería, el cual tomamos de la iteración previa.
			*/
		
			newCommandProc(in, fd[1], argvv[i]);

			/* Ya no es necesario este descriptor ya que el proceso hijo escribe aquí  */
			close(fd[1]);
			
			/* Mantemenos el "read end" de la tubería para que el próximo proceso hijo
				lea de allí.  */
			in = fd[0];
		}
		/* Por último, establecemos stdin como "read end" de la tubería anterior */  
		if (in != 0)
			dup2(in, 0);

		/* Ahora, el proceso actual correrá el último comando y retornará
			el valor de la secuencia. */
		return execvp(argvv[i][0], (char* const*)argvv[i]);
	}

	return 0;
}