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


int mandatopid = -1;

extern int obtain_order();		/* See parser.y for description */

void functionSignal(int signum){
	if(mandatopid != -1){
		kill(mandatopid, SIGKILL);
	}
}

int commandCd(char **argv, int argc){
	char *dir, ret[500];
	if(argc >= 2){
		perror("bash: cd: demasiados argumentos\n");
		return 1;
	}
	else if(argc == 1)
		dir = argv[argc-1];
	else
		dir = getenv("HOME");

	if(chdir(dir) == -1){
		perror("bash: cd: No existe el archivo o el directorio\n");
		return 1;
	}

	getcwd(ret,500);
	printf("%s\n",ret);
	return 0;
}

int otherCommands(char **argv, int argc){
	int pid = fork();

	if(pid == 0){
		if(execvp(argv[0],argv) == -1)
			exit(-1);
		exit(0);
	}
	else{
		int status;
		mandatopid = pid;
		wait(&status);
		if(status == -1)
			perror("Ha habido un problema al ejecutar el mandato indicado");
		mandatopid = -1;
	}
	return 0;
}

int chooseCommand(char ***argvv,char **argv, int argvc){
	int argc;

	for (argvc = 0; (argv = argvv[argvc]); argvc++) {
		for (argc = 0; argv[argc]; argc++);
		
		if(strcmp(argv[0], "cd") == 0){
			commandCd(argv+1, argc-1);
		}
		else{
			otherCommands(argv,argc);
		}			
	}
	return 0;
}

int main(void)
{
	char ***argvv = NULL;
	int argvc;
	char **argv = NULL;
	char *filev[3] = { NULL, NULL, NULL };
	int bg;
	int ret;

	int fd_in = -1,fd_out = -1, fd_err = -1;
	int std_out,std_in,std_err;

	setbuf(stdout, NULL);			/* Unbuffered */
	setbuf(stdin, NULL);

	signal(SIGINT, functionSignal);
	signal(SIGQUIT, functionSignal);

	char mypid[256];
	sprintf(mypid, "mypid=%d",  getpid());
	putenv(mypid);
	putenv("prompt=msh> ");

	while (1) {
		fprintf(stderr, "%s", getenv("prompt"));	/* Prompt */
		ret = obtain_order(&argvv, filev, &bg);
		if (ret == 0) break;		/* EOF */
		if (ret == -1) continue;	/* Syntax error */
		argvc = ret - 1;		/* Line */
		if (argvc == 0) continue;	/* Empty line */


		if (filev[0]) {
			if((fd_in = open(filev[0], O_RDONLY)) == -1){
				perror("No existe el fichero o el directorio");
				continue;
			}
			std_in = dup(0);
			close(0);
			dup2(fd_in, 0);
		}/* IN */
		if (filev[1]) {
			mode_t mask = umask(0);
			if((fd_out = open(filev[1], O_WRONLY | O_CREAT, 0666)) == -1){
				umask(mask);
				continue;
			}
			umask(mask);
			std_out = dup(1);
			close(1);
			dup2(fd_out, 1);
		}/* OUT */
		if (filev[2]) {
			mode_t mask = umask(0);
			if((fd_err = open(filev[2], O_WRONLY | O_CREAT, 00666)) == -1){
				umask(mask);
				continue;
			}
			umask(mask);
			std_err = dup(2);
			close(2);
			dup2(fd_err, 2);
		}/* ERR */
		if (bg) {
			pid_t pid = fork();

			if(pid == 0){
				int ret = chooseCommand(argvv,argv, argvc);
				char status[256];
				sprintf(status, "status=%d", ret);
				putenv(status);
				exit(ret);
			}
			else {
				char bgpid[256];
				sprintf(bgpid, "bgpid=%d", pid);
				putenv(bgpid);
				fprintf(stderr,"[%d]\n", pid);
			}
		}

		chooseCommand(argvv,argv, argvc);

		if(filev[0]){
			dup2(std_in,0);
			close(std_in);
			close(fd_in);
		}
		if(filev[1]){
			dup2(std_out,1);
			close(std_out);
			close(fd_out);
		}
		if(filev[2]){
			dup2(std_err,2);
			close(fd_err);
			close(std_err);
		}

	}
	exit(0);
	return 0;
}
