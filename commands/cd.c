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

//int cd(char **argv, int argc) {
int _cd(char **dir) {
	char *aux;
    // Se espera que la cantidad de argumentos sea 1.
    argCount(dir, 1);
    
    if(strlen(dir[1]) >= MAX_PATH_LEN){
		perror("msh: cd: nombre de directorio demasiado largo.\n");
		return 1;
	}
    
    if (dir[1]) aux = dir[1];
	else aux = getenv("HOME");

	if(chdir(aux) == -1){
		perror("msh: cd: No existe el archivo o el directorio\n");
		return 1;
	}

    char ret[MAX_PATH_LEN];
	getcwd(ret, MAX_PATH_LEN);
	printf("%s\n",ret);

	return 0;
}