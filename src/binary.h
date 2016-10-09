#ifndef BINARY_H
#define	BINARY_H

#include <elf.h>
#include "global.h"



struct struct_executable {
    ADDRESS     pCodeStart;         // code segment start address
    ADDRESS     pCodeEnd;           // code segment end address
    
    char*       pSectionStringTable;    // section name string table
    INT32       nSectionStringTable;    // section name string table size
    
    ADDRESS*    pGot;   // array of GOT entries
    INT32       nGot;   // number of GOT entries
    
    Elf32_Sym*  pSymbolTable;   // array of symbols
    INT32       nSymbolTable;   // number of symbols
    char*       pStringTable;   // array of symbol strings
    INT32       nStringTable;   // length of symbol strings

    Elf32_Sym*  pDynamicSymbolTable;    // array of dynamic symbols
    INT32       nDynamicSymbolTable;    // number of dynamic symbols
    char*       pDynamicStringTable;    // array of dynamic symbol strings
    INT32       nDynamicStringTable;    // length of dynamic symbol strings
}; 

extern struct struct_executable Executable;



#define	FOR_EACH_FUNCTION(fun) for(fun =Executable.pSymbolTable; fun <Executable.pSymbolTable + Executable.nSymbolTable; fun++  ) 
#define FOR_EACH_INSN_IN_FUN(fun, addr) for(addr = fun->st_value; addr < fun->st_value + fun->st_size; addr += 4)



void pjz_IintExecutable(int fp);

void pjz_executable_flush();

void pjz_dump_function();

void pjz_pjz_dump_function2();

#endif
