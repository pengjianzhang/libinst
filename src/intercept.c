/*!
 * \file this is the interceptor
 */

 
//#ifndef __cplusplus
//#define _GNU_SOURCE
//#endif


#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>      // open
#include <unistd.h>     // close
#include <string.h>
#include "global.h"

extern void pjz_Optimize(int);
//***********************

/*
void (*real_fini)(void); 	 

extern void pjz_Optimize(int);

void Optfini()
{
	pjz_Collection();
	real_fini();
}

//**********************


// parameters for __libc_start_main
#define PARAMS_START_MAIN  int (*main) (int, char **, char **), \
                        int argc,                               \
                        char *__unbounded *__unbounded ubp_av,  \
                        void (*auxvec) (void),                  \
                        void (*init) (void),                    \
                        void (*fini) (void),                    \
                        void (*rtld_fini) (void),               \
                        void *__unbounded stack_end


//
//brief intercept __libc_start_main 
//
void __libc_start_main(PARAMS_START_MAIN) {
    char *pEnv;
    char exe_name[256]; 	
//    void (*real_fini)(void); 	 
    // open error file

    FILE* Error_File;
    pEnv = getenv("DB_ERRORFILE");
    Error_File = fopen(pEnv, "at");
    if (Error_File==0)
        Error_File = stderr;

    // locate original __libc_start_main
    void (*real_libc_start_main)(PARAMS_START_MAIN);
    real_libc_start_main = (void(*)(PARAMS_START_MAIN)) dlsym 
    		(RTLD_NEXT, "__libc_start_main");
    real_fini = fini;

    // do with exe. profiling, optimizing, or running.
    sprintf(exe_name, "%s", ubp_av[0]);
    if (strcmp(exe_name, "/bin/sh")!=0 && strcmp(exe_name, "sh")!=0 && strcmp(exe_name, "/bin/bash")!=0 && strcmp(exe_name, "echo")!=0) {

        // mmap the executable
        INT32 fp = open(exe_name, O_RDONLY);
        
        if (fp<0) {
            char cpath[256];
            char *path = getenv("PATH");
            while (fp<0) {
                char* next = strstr(path, ":");
                if (next==NULL) {
                    strcpy(cpath, path);
                    strcat(cpath, "/");
                    strcat(cpath, exe_name);
                    fp = open(cpath, O_RDONLY);
//                    CFATAL(fp<0, ("Cannot find file \"%s\", use full path please.\n", exe_name));
                    break;
                }
                else {
                    *next = '\0';
                    strcpy(cpath, path);
                    strcat(cpath, "/");
                    strcat(cpath, exe_name);
                    fp = open(cpath, O_RDONLY);
                    path = next+1;
                }
            }
        }

        //REPORT(("Optimizing %s\n", exe_name));
        pjz_Optimize(fp);
//	printf("Hello, I love you!\n");
        close (fp);
    }

    real_libc_start_main(main, argc, ubp_av, auxvec, init, Optfini, rtld_fini, stack_end);

}

*/


void pjz_before_main(char * fname)
{

        FILE * fp = open(fname, O_RDONLY);
        pjz_Optimize(fp);
        close (fp);

}


void pjz_after_main()
{
	pjz_Collection();
}
