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
#include <stdint.h>
#include <pwd.h>
#include <regex.h>
#include <glob.h>
#include <errno.h>

#define MAX_PATH_LEN    512
#define MAX_LINE_LEN    512
#define MAX_READ_ARGS   32

/* Variables globales */
const char *internals[] = { "cd", "read", "umask", "time" };
int mandatopid = -1;
int bg;

#endif

extern int obtain_order();		/* See parser.y for description */

void setEnv(char *varName, char *value) {
	if (!bg) {
		char var[256];
		sprintf(var, "%s=%s", varName, value);
		putenv(var);
	}
}

void functionSignal(int signum){
	if(mandatopid != -1){
		printf("Killing %d...\n", mandatopid);
		kill(mandatopid, SIGKILL);
	}
}

void exitHandler(int status) {
	switch (status) {
		case EXIT_SUCCESS:
			setEnv("status", "0");
			exit(EXIT_SUCCESS);
			break;
		default:
			setEnv("status", "1");
			exit(EXIT_FAILURE);
			break;
	}
}

// ##############################
// # ------- UTILIDADES ------- #
// ##############################

int arg_count(char **cmd) {
    // Conteo de argumentos.
    int argc = 0;
    while(cmd[argc]) argc++;

    // Restamos 1 dado que el primer elemento del array cmd es
    // el nombre del comando.
    return argc - 1;
}

unsigned char check_umask(char *mask) {
    // Más de 4 dígitos.
    if (strlen(mask) > 4) return 0;

    // Convertimos el caracter a entero mediante su diferencia con '0'.
    // Luego chequeamos que no sea mayor a 7.
    for (int i=1;i<=4; i++) {
        if ((mask[i] - '0') > 7) return 0;
    }

    return 255;
}

unsigned char is_internal(char *cmd) {
	// Función para comprobar si un comando es interno
	// Devuelve 0x0 si lo es, 0xFF si no y 0x01 si hubo error.
	if (!cmd) return 0x1;
	int index = 0;

	while (internals[index]) {
		if (strcmp(internals[index], cmd) == 0) return 0x0;
		index++;
	}

	return 0xFF;
}

// METACARACTERES
// Chequeo de argumentos de comandos internos
int arg_checker(char ***argvv) {
	int argIndex = 0;
	int cmdIndex;
	struct passwd *user;

	while(argvv[argIndex]) {
		char **cmd = argvv[argIndex];
		cmdIndex = 1;

		if (!cmd[cmdIndex])	{
			argIndex++;
			continue;
		}

		char *found;
		
		while(cmd[cmdIndex]) {
			// Buscamos la virgulilla o tilde.
			//found = strchr(cmd[cmdIndex], 0x7E);
			//if (found) {
			while((found = strchr(cmd[cmdIndex], 0x7E))) {
				char preffix[MAX_LINE_LEN] = "";
				char line[MAX_LINE_LEN] = "";
				char name[MAX_LINE_LEN] = "";
				char *suffix;
				size_t preffix_size = strlen(cmd[cmdIndex]) - strlen(found);

				strncpy(preffix, cmd[cmdIndex], preffix_size);
				sscanf(found, "~%[_a-zA-Z0-9]", name);
				strncpy(line, cmd[cmdIndex], preffix_size);

				user = getpwnam(name);
				
				if (user) {
					strcat(line, user->pw_dir);
					//sprintf(cmd[cmdIndex], "%s%s", preffix, user->pw_dir);
				}
				else if (strlen(name) == 0) {
					strcat(line, getenv("HOME"));
				}

				suffix = cmd[cmdIndex];
				suffix += preffix_size + strlen(name) + 1;
				strcat(line, suffix);

				cmd[cmdIndex] = realloc(cmd[cmdIndex], strlen(line) * sizeof(char) + 1);
				sprintf(cmd[cmdIndex], "%s", line);

			}

			// Buscamos el signo dólar.
			while((found = strchr(cmd[cmdIndex], 0x24))) {
				char var_name[MAX_LINE_LEN] = "";
				char line[MAX_LINE_LEN] = "";
				char *suffix;
				size_t preffix_size = strlen(cmd[cmdIndex]) - strlen(found);

				sscanf(found, "$%[_a-zA-Z0-9]", var_name);
				const char *value	= getenv(var_name);
				
				strncpy(line, cmd[cmdIndex], preffix_size);

				if (value) {
					strcat(line, value);
				}
				
				suffix = cmd[cmdIndex];
				suffix += preffix_size + strlen(var_name) + 1;
				
				strcat(line, suffix);

				cmd[cmdIndex] = realloc(cmd[cmdIndex], strlen(line) * sizeof(char) + 1);
				sprintf(cmd[cmdIndex], "%s", line);
			}

			// Chequeamos la expansión de nombres de fichero
			char *argument = cmd[cmdIndex];
			int subIndex = 0;
			int noExpand = 0;
			int question = 0;

			while(argument[subIndex]) {
				// Buscamos la barra dentro del argumento.
				if (argument[subIndex] == 0x2F) noExpand = 1;	// Barra (/)
				if (argument[subIndex] == 0x2A) noExpand = 1;	// Asterisco (*)
				if (argument[subIndex] == 0x5B) noExpand = 1;	// Corchete ([)
				if (argument[subIndex] == 0x3F) question = 1;
				subIndex++;
			}

			// Se procederá a la expansión sólo si no se encuentra una barra,
			// un asterisco o un corchete y sí se encuentra un signo de interrogación.
			if (noExpand == 0 && question == 1) {
				glob_t globbuf;
				globbuf.gl_offs = 0;

				if (glob(argument, GLOB_DOOFFS, NULL, &globbuf) == 0) {
					// Se encontraron nombres de archivo.
					int i = 0;

					while(globbuf.gl_pathv[i]) {
						cmd[cmdIndex+globbuf.gl_pathc] = cmd[cmdIndex+i];
						cmd[cmdIndex+i] = globbuf.gl_pathv[i];
						i++;
					}
				}
			}

			cmdIndex++;
		}
		argIndex++;
	}

	return 0;
}

// #######################################################
// # ------- DEFINICIÓN DE LOS COMANDOS INTERNOS ------- #
// #######################################################

// COMANDO CD
int _cd(char **dir) {
    // Se espera que la cantidad de argumentos sea 1.
	int argc = arg_count(dir);

	if (argc > 1) {
		perror("msh: cd: Demasiados argumentos.\n");
		return 1;
	}
	// Comando cd sin argumentos. Cambiar al directorio de la
	// variable $HOME.
    if(argc == 0) {
		if(chdir(getenv("HOME")) == -1){
			perror("msh: cd: No existe el archivo o el directorio.\n");
			return 2;
		}
		return 0;
	}

	// Llegado a este punto, significa que tenemos un argumento.
    if(strlen(dir[1]) >= MAX_PATH_LEN){
		perror("msh: cd: nombre de directorio demasiado largo.\n");
		return 3;
	}

	if(chdir(dir[1]) == -1){
		perror("msh: cd: No existe el archivo o el directorio.\n");
		return 2;
	}

    char ret[MAX_PATH_LEN];
	getcwd(ret, MAX_PATH_LEN);
	printf("%s\n",ret);
	return 0;
}

// COMANDO READ
int _read(char **cmd) {
    int argc = arg_count(cmd);

    if (argc < 1) {
        perror("msh: read: ingrese como mínimo un nombre de variable.");
        return 1;
    }

    char line[MAX_LINE_LEN];
    printf("Ingrese los valores a asignar a la(s) variable(s):\n");
    //scanf("%s", line);
	// Dado que el modificador %s hace que scanf lea hasta el primer
	// espacio en blanco, usamos una expresión para modificar
	// este comportamiento.
	fflush(stdin);
	scanf("%[^\n\r]", line);

    // Tokenización de la línea ingresada.
	// Usamos el espacio en blanco y el tabulador como delimitadores.
    const char *delimiters = " \t";
    char *token = strtok(line, delimiters);
    int index = 1;
	char *toEnv;

    while(token != NULL && index < MAX_READ_ARGS) {
        if (cmd[index]) {
			const size_t var_len = (strlen(cmd[index]) + strlen(token) + 1) * sizeof(char);
			toEnv = (char*) malloc (var_len);
			sprintf(toEnv, "%s=%s", cmd[index], token);
			printf("%s\n", toEnv);
			if (putenv(toEnv) != 0) {
				perror("msh: read: error al establecer variable de entorno.");
				return 2;
			}
		}
		else {
			break;
		}
		index++;
        token = strtok(NULL, delimiters);
    }

    return 0;
}

// COMANDO TIMES
int _time(char **cmd) {
    int status;
    int tics_per_second = sysconf(_SC_CLK_TCK);
    struct timespec realTime1;
    struct timespec realTime2;

    if (cmd[1]) {
        // Obtenemos el momento inicial del proceso.
        if (clock_gettime(CLOCK_REALTIME, &realTime1) == -1) {
            perror("clock_gettime error.");
			return 1;
        }

        if (fork() == 0) {
            cmd++;
            exit(execvp(cmd[0], (char* const*)cmd));
        }

        if (wait(&status) == -1) {
            perror("wait() error");
			return 2;
		}

        // Obtenemos el momento final del proceso.
        if (clock_gettime(CLOCK_REALTIME, &realTime2) == -1) {
            perror("clock_gettime error.");
			return 3;
        }

        long int secs = (intmax_t) realTime2.tv_sec - (intmax_t) realTime1.tv_sec;
        long int msecs = (realTime2.tv_nsec - realTime1.tv_nsec) / 1000000;
        
        if(msecs < 0) {
            secs--;
            msecs = 1000 + msecs;
        }

        printf("%ld.%03lds\n", secs, msecs);

    }
    else {
        //for (int i=0;i<1000000000; i++) {}
        struct tms t;
        times(&t);
        //printf("%d.%03du %d.%03ds %d.%03dr\n", (double)t->tms_cstime);

        long int total_utime = (t.tms_utime + t.tms_cutime);
        long int total_stime = (t.tms_stime + t.tms_cstime);
        long int total_rtime = total_utime + total_stime;

        int total_utime_sec = 0;
        while (total_utime > tics_per_second) {
            total_utime -= tics_per_second;
            total_utime_sec++;
        }

        int total_utime_msec = total_utime * 1000 / tics_per_second;

        int total_stime_sec = 0;
        while (total_stime > tics_per_second) {
            total_stime -= tics_per_second;
            total_stime_sec++;
        }

        int total_stime_msec = total_stime * 1000 / tics_per_second;
        
        int total_rtime_sec = 0;
        while (total_rtime > tics_per_second) {
            total_rtime -= tics_per_second;
            total_rtime_sec++;
        }

        int total_rtime_msec = total_rtime * 1000 / tics_per_second;

        printf("%d.%03du %d.%03ds %d.%03dr\n",
            total_utime_sec,
            total_utime_msec,
            total_stime_sec,
            total_stime_msec,
            total_rtime_sec,
            total_rtime_msec);
    }

    return 0;
}

// COMANDO UMASK
int _umask(char **cmd) {
    mode_t oldMask;
    // Se espera que la cantidad de argumentos sea 1.
	int argc = arg_count(cmd);

	if (argc > 1) {
		perror("msh: umask: Demasiados argumentos.\n");
        return 1;
	}

    if (cmd[1]) {
        if(!check_umask(cmd[1])) {
            perror("msh: umask: formato de máscara inválido.\n");
            return 2;
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

// Creación de un proceso hijo y encadenado de entradas
// y salidas para el nuevo comando.
int new_command_process(int in, int out, char **cmd) {
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

		exit(execvp(cmd[0], (char* const*)cmd));
	}

	return pid;
}

/*	Implementación de la cadena de mandatos.  */
int commandPipeline (char ***argvv, int argvc) {
	int i;
	int in;
	int fd[2];

	// Se crean estas tuberías auxiliares para retornar el último
	// comando y sus argumentos, en caso de que sean internos, para
	// ser ejecutados por el propio minishell.
	int auxPipes[2];
	pipe(auxPipes);
	close(auxPipes[1]);

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

			new_command_process(in, fd[1], argvv[i]);
			
			/* Ya no es necesario este descriptor ya que el proceso hijo escribe aquí  */
			close(fd[1]);
			
			/* Mantemenos el "read end" de la tubería para que el próximo proceso hijo
				lea de allí.  */
			in = fd[0];
		}
		/* Por último, establecemos stdin como "read end" de la tubería anterior */  
		if (in != 0)
			dup2(in, 0);

		// Si el último comando es interno, leer la tubería y enviar
		// lo leido al preceso principal.
		/*
		if (strcmp(argvv[i][0], "cd") == 0 ||
			strcmp(argvv[i][0], "umask") == 0 ||
			strcmp(argvv[i][0], "time") == 0 ||
			strcmp(argvv[i][0], "read") == 0) {
				*/
		if (is_internal(argvv[i][0]) == 0x0) {
			char buf[MAX_LINE_LEN];
			int bytes = read(0, buf, MAX_LINE_LEN);
			write(auxPipes[1],buf, bytes-1);
			exit(EXIT_SUCCESS);
		}
		/* Ahora, el proceso actual correrá el último comando y retornará
			el valor de la secuencia. */
		if(execvp(argvv[i][0], (char* const*)argvv[i]) == -1)
			exit(EXIT_FAILURE);
		else
			exit(EXIT_SUCCESS);
	}

	int status;
	wait(&status);

	if (is_internal(argvv[argvc-1][0]) == 0x0) {
		char buf[MAX_LINE_LEN] = "";
		int bytes = read(auxPipes[0], buf, MAX_LINE_LEN);
		printf("is internal...-%s-\n", buf);
		if (argvv[argvc-1][1] == NULL) {
			argvv[argvc-1][1] = (char*) malloc (bytes * sizeof(char));
		}

		strncpy(argvv[argvc-1][1], buf, bytes);

		if (strcmp(argvv[argvc-1][0], "cd") == 0) return _cd(argvv[argvc-1]);
		if (strcmp(argvv[argvc-1][0], "umask") == 0) return _umask(argvv[argvc-1]);
		if (strcmp(argvv[argvc-1][0], "time") == 0) return _time(argvv[argvc-1]);
		if (strcmp(argvv[argvc-1][0], "read") == 0) return _read(argvv[argvc-1]);
	}
	
	close(auxPipes[0]);
	return status;
}

// Selector de comandos. Distingue entre comandos internos y otros.
int command_selector(char ***argvv, int argvc) {
	arg_checker(argvv);

    if (argvc == 1) {
        // Los comandos internos se ejecutan en el proceso del minishell.
        char **cmd = argvv[0];
        if (strcmp(cmd[0], "cd") == 0) return _cd(cmd);
        else if (strcmp(cmd[0], "umask") == 0) return _umask(cmd);
        else if (strcmp(cmd[0], "time") == 0) return _time(cmd);
        else if (strcmp(cmd[0], "read") == 0) return _read(cmd);
        else if (strcmp(cmd[0], "exit") == 0) exit(EXIT_SUCCESS);
        else if (strcmp(cmd[0], "quit") == 0) exit(EXIT_SUCCESS);
        else {
            // Tenemos un comando simple que no es interno.
            // Lo ejecutamos en una subshell.
			pid_t pid = fork();
            if (pid == 0) {
                if(execvp(cmd[0], (char* const*)cmd) == -1)
					exitHandler(EXIT_FAILURE);
				else exitHandler(EXIT_SUCCESS);
            }

			int status;
			wait(&status);

			return status;
        }
    }
    else {
        // Tenemos una secuencia de comandos o una redirección.
        return commandPipeline(argvv, argvc);
    }
    return 0;
}

int main(void)
{
	char ***argvv = NULL;
	int argvc;
	char *filev[3] = { NULL, NULL, NULL };
	int ret;
	int status;

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
		argvc = ret - 1;			/* Line */
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
				signal(SIGINT, SIG_IGN);
				signal(SIGQUIT, SIG_IGN);

				int status = command_selector(argvv, argvc);
				exit(status);
			}
			else {
				char bgpid[MAX_LINE_LEN];
				sprintf(bgpid, "bgpid=%d", pid);
				putenv(bgpid);
				fprintf(stderr,"[%d]\n", pid);
			}
		}
		else {
			status = command_selector(argvv, argvc);
			char var[16] = "";
			sprintf(var, "status=%d", status);
			putenv(var);
		}

		//wait(&status);

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
	
	return 0;
}
