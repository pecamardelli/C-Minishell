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
#include <stdint.h>

int _times(char **cmd) {
    int status;
    int tics_per_second = sysconf(_SC_CLK_TCK);
    struct timespec realTime1;
    struct timespec realTime2;

    if (cmd[1]) {
        // Obtenemos el momento inicial del proceso.
        if (clock_gettime(CLOCK_REALTIME, &realTime1) == -1) {
            perror("clock_gettime error.");
        }

        if (fork() == 0) {
            cmd++;
            exit(execvp(cmd[0], (char* const*)cmd));
        }

        if (wait(&status) == -1)
            perror("wait() error");

        // Obtenemos el momento final del proceso.
        if (clock_gettime(CLOCK_REALTIME, &realTime2) == -1) {
            perror("clock_gettime error.");
        }

        long int secs = (intmax_t) realTime2.tv_sec - (intmax_t) realTime1.tv_sec;
        long int msecs = (realTime2.tv_nsec - realTime1.tv_nsec) / 1000000;
        
        if(msecs < 0) {
            secs--;
            msecs = 1000 + msecs;
        }

        printf("Tiempo consumido por %s: %ld.%03lds\n", cmd[1], secs, msecs);

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