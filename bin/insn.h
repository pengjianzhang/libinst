#ifndef PJZ_INSNS_H
#define	PJZ_INSNS_H

/*This lib statistics instructions between pjz_before_main, pjz_after_main
 *FNAME is the executable file name, usually "argv[0]"
 *
 */
void pjz_before_main(char * fname);

void pjz_after_main();

#endif

