#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // open
#include <sys/mman.h>   // mprotect, mmap
#include <sys/stat.h>   // fstat
#include <asm/cachectl.h>   // cacheflush
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include "global.h"
#include "binary.h"


struct struct_executable Executable;




/*!
\brief open the file, and read executable sections. Sort them.
*/
static void pjz_BinaryLoad(void *start_fp) {
    INDEX i, j;
    Elf32_Ehdr *pElfHeader = (Elf32_Ehdr*) start_fp;
    UINT32      nSection = pElfHeader->e_shnum;
    Elf32_Shdr *pSectionHeaderStart = (Elf32_Shdr*) ( ((char*)pElfHeader) + pElfHeader->e_shoff );

    // section name string table
    Elf32_Shdr *pSectionHeader = pSectionHeaderStart +  pElfHeader->e_shstrndx;
    char *pSectionNameStringTable = (char*)( ((char*)pElfHeader) + pSectionHeader->sh_offset );
    Executable.pSectionStringTable = (char*) malloc(pSectionHeader->sh_size);
    Executable.nSectionStringTable = 0;



    //as .got .symtab .strtab is unique, in Executable has one position for each, this can identity them
    //.text may have many, need an section to keep them	 
    // get all executable sections
    for (i=1; i<nSection; ++i) {    // section 0 is NULL
//    	printf("abc\n");
        pSectionHeader = pSectionHeaderStart + i;
        char *pSectionName = pSectionNameStringTable + pSectionHeader->sh_name;

        if (strcmp(pSectionName, ".got")==0) {
            // global offset table exists when executing, get its position
            Executable.pGot = (ADDRESS*) (pSectionHeader->sh_addr); //virtual address
            Executable.nGot = (pSectionHeader->sh_size) / sizeof(ADDRESS); // got is an array of addresses
        }
        else if (strcmp(pSectionName, ".symtab")==0) {
            // symbol table does not exist after loading, need copy
            Executable.pSymbolTable = 
			(Elf32_Sym*) (((char*)pElfHeader) + pSectionHeader->sh_offset ); //address in mmap space
            Executable.nSymbolTable = (pSectionHeader->sh_size) / sizeof(Elf32_Sym);
        }
        else if (strcmp(pSectionName, ".strtab")==0) {
            // string table does not exist after loading, need copy
            Executable.pStringTable = 
			(char*) (((char*)pElfHeader) + pSectionHeader->sh_offset); //address in mmap space
            Executable.nStringTable = (pSectionHeader->sh_size) / sizeof(char); //str table is  lists of strings
        }
        else if (strcmp(pSectionName, ".dynsym")==0) {
            // dynamic symbol table exists when executing, get its position
            Executable.pDynamicSymbolTable = (Elf32_Sym*) (pSectionHeader->sh_addr); // virtual address
            Executable.nDynamicSymbolTable = (pSectionHeader->sh_size) / sizeof(Elf32_Sym);
        }
        else if (strcmp(pSectionName, ".dynstr")==0) {
            // dynamic symbol string table exists when executing, get its position
            Executable.pDynamicStringTable = (char*) (pSectionHeader->sh_addr);
            Executable.nDynamicStringTable = (pSectionHeader->sh_size) / sizeof(char);
        }
        //else if ( (CurrentSec->sh_flags & SHF_EXECINSTR)==SHF_EXECINSTR) {
        else if (strcmp(pSectionName, ".text")==0) {				//asume .text unique
		Executable.pCodeStart = pSectionHeader->sh_addr;
		Executable.pCodeEnd = pSectionHeader->sh_addr + pSectionHeader->sh_size;

        }
    }
	   

    ADDRESS pPageStart = Executable.pCodeStart&~(pagesize-1);
    mprotect((void*)pPageStart, Executable.pCodeEnd-pPageStart, PROT_READ | PROT_WRITE | PROT_EXEC);
}




static void pjz_function_sort(Elf32_Sym * sym, Elf32_Sym ** rank, int n)
{
	int i,j,k;
	ADDRESS min = 0, tmp;
	Elf32_Sym * sym_p;
	for(i = 0; i< n; i++)
		rank[i]	= sym + i;
	

	for(i = 0; i< n - 1; i++)
	{
		k = i;
		min = rank[k]->st_value;	
		for(j = i + 1; j < n; j++)
		{
			tmp = rank[j]->st_value;
			if(tmp < min)
			{
				min = tmp;
				k = j;
			}	
		}
		
		if(k != i)
		{
			sym_p = rank[k];
			rank[k] = rank[i];
			rank[i] = sym_p;
		}	
	}
}

/*!
\brief Mark functions' begin and end according to the symbols in symbol table.
The ins at the function address is marked with FI_FUN_BEG and FI_BBL_BEG.
The ins before it is marked with FI_FUN_END and FI_BBL_END.
Copy function symbols into quick symbol table.
*/
static void pjz_IRMarkFunctions() {
    // get all functions
    INDEX i, j;

    Elf32_Sym*  pSymbolTable = Executable.pSymbolTable;
    INT32       nSymbolTable = Executable.nSymbolTable;    
    char*       pStringTable = Executable.pStringTable;
    INT32       nStringTable = Executable.nStringTable;
    char c; 	
/*
    if ( Executable.pSymbolTable == NULL) {
        printf("No symbol table found, use dynamic symbol table instead.\n");
        pSymbolTable = Executable.pDynamicSymbolTable;
        nSymbolTable = Executable.nDynamicSymbolTable;    
        pStringTable = Executable.pDynamicStringTable;
        nStringTable = Executable.nDynamicStringTable;
    }
*/

    // these tables will disappear after munmap, so we have to copy some useful information
    Executable.pSymbolTable = NULL;
    Executable.nSymbolTable = 0;
    Executable.pStringTable = (char*) malloc(nStringTable);
    Executable.nStringTable = 0;
    


    // temporary arrays for function sorting
    Elf32_Sym *aSymbolTemp = (Elf32_Sym*) calloc(sizeof(Elf32_Sym), nSymbolTable);
    Elf32_Sym **  symRank =  (Elf32_Sym**) malloc(sizeof(Elf32_Sym *)*nSymbolTable);	 
    
    for (i=1; i<nSymbolTable; ++i) {
        Elf32_Sym *pSymbol = pSymbolTable + i;
        INT32 SymbolType = ELF32_ST_TYPE(pSymbol->st_info);
        
        if ( SymbolType==STT_FUNC && pSymbol->st_value >= Executable.pCodeStart && pSymbol->st_value < Executable.pCodeEnd ) {
            
            char *pSymbolName = pStringTable + pSymbol->st_name;
//            if (strchr(pSymbolName, '@')!=NULL) {
                // functions in the lib, skip
//               continue;
//           }
           
//	if( strncmp(pSymbolName,"fstat",5) == 0) continue;
//	if( strncmp(pSymbolName,"__fstat",7) == 0) continue;

	if( strncmp(pSymbolName,"__libc_start_main",17)==0) continue;

	   if(pSymbol->st_size == 0) continue;
	   if(strlen(pSymbolName) > 11)
	   {
	  	 if(strncmp(pSymbolName,"__libc_csu",10)==0) continue;
		
	   }
	   if(pSymbolName[0]=='p' && pSymbolName[1]=='j' && pSymbolName[2]=='z') continue;	 		
            // put into func_array
		int j = 0;
		int flag = 0;
		for(j = 0; j < Executable.nSymbolTable; j++)
			if(pSymbol->st_value == aSymbolTemp[j].st_value) {
//				printf("dup\n");
				flag = 1;
				break;//continue;				
			}
		if(flag == 1) continue;
            aSymbolTemp[Executable.nSymbolTable].st_value = pSymbol->st_value;
            aSymbolTemp[Executable.nSymbolTable].st_size = pSymbol->st_size;
            aSymbolTemp[Executable.nSymbolTable].st_name = Executable.nStringTable;
            strcpy(Executable.pStringTable + Executable.nStringTable, pSymbolName);
            Executable.nStringTable += strlen(pSymbolName)+1;

            Executable.nSymbolTable ++;
        }
    }

    // sort
    Executable.pSymbolTable = (Elf32_Sym*) malloc (sizeof(Elf32_Sym)*Executable.nSymbolTable);
    
    pjz_function_sort(aSymbolTemp, symRank, Executable.nSymbolTable);
    for (i=0; i<Executable.nSymbolTable; ++i)
        Executable.pSymbolTable[i] = *(symRank[i]);

    free(aSymbolTemp);
    free(symRank);

    // the remaining symbol name string table must be smaller than original one, so the pointer will not change
    Executable.pStringTable = realloc(Executable.pStringTable, Executable.nStringTable);

}

//flush executable binary immage
void pjz_executable_flush()
 {
//    cacheflush(code_cache,(code_cache_n+1)*4,ICACHE);
    cacheflush(Executable.pCodeStart, Executable.pCodeEnd-Executable.pCodeStart, ICACHE);
}


/*
//how many branch instructions
static int branch_nums()
{
	
        Elf32_Sym * fun;
	ADDRESS addr;
	INSN_T	insn;
	int count = 0;

	FOR_EACH_FUNCTION(fun)
	{
		FOR_EACH_INSN_IN_FUN(fun,addr)
		{
			if(pjz_is_branch(*((unsigned *)addr))) count++;
		}
	}

	branch_num = count;
	return count;
}
*/

/*
static void instrument()
{	
        Elf32_Sym * fun;
	ADDRESS addr;
	INSN_T	insn;

	

	FOR_EACH_FUNCTION(fun)
	{
		FOR_EACH_INSN_IN_FUN(fun,addr)
		{
	//		printf("addr %x code %x\n", addr,*(unsigned *)addr);
			if(pjz_is_branch(*(unsigned *)addr)) counting_branch(addr); 
		
		}
	}
}
*/



/*!
\brief fluch the code cache and the executable
*/

/*
void CodeCacheFlush()
 {
    cacheflush(code_cache,(code_cache_n+1)*4,ICACHE);
    cacheflush(Executable.pCodeStart, Executable.pCodeEnd-Executable.pCodeStart, ICACHE);
}





static void dump_instruction()
{
	ADDRESS start = Executable.pCodeStart; 
	ADDRESS end = Executable.pCodeEnd;          
	ADDRESS i;

	for(i = start; i < end; i+=4)
		printf("%10x %8x\n",i, *((int*)i));
}

*/

void pjz_dump_function()
{
	Elf32_Sym*  funTable = Executable.pSymbolTable;   
    	INT32       funNum = Executable. nSymbolTable;  
	char*       funName  = Executable.pStringTable;  
	INT32       nameSize  = Executable.nStringTable;
	int i; 
	char * pName; 
	ADDRESS addr;
	int 	size;

	for(i = 0; i< funNum; i++)
	{
		pName = funName + funTable[i].st_name;
		addr = funTable[i].st_value;
		size = funTable[i].st_size;
		printf("%s\tstart= 0X%x start = %d end = %d size = %d \n",pName,addr,addr,addr + size, size);
	}
}


void pjz_pjz_dump_function2()
{
	Elf32_Sym*  funTable = Executable.pSymbolTable;   
    	INT32       funNum = Executable. nSymbolTable;  
	char*       funName  = Executable.pStringTable;  
	INT32       nameSize  = Executable.nStringTable;
	int i; 
	char * pName; 
	ADDRESS addr;
	int 	size;

	Elf32_Sym * fun;

	FOR_EACH_FUNCTION(fun)
	{
		pName = funName + fun->st_name;
		addr = fun->st_value;
		size = fun->st_size;
		printf("%s\tstart= 0X%x start = %d end = %d size = %d \n",pName,addr,addr,addr + size, size);
	}

}


void pjz_IintExecutable(int fp)
{
	struct stat stat_data;
 	int fs = fstat(fp,&stat_data);
//        CASSERT(fs==0, ("fstat error!\n"));
	void *start_fp = mmap(0, stat_data.st_size, PROT_READ, MAP_SHARED, fp, 0);
	pjz_BinaryLoad(start_fp);
	pjz_IRMarkFunctions();
	fs = munmap(start_fp, stat_data.st_size);
//	CASSERT(fs==0, ("munmap error!\n"));
}


//brief Main optimization routine. Call all kinds of optimizations.
/*
void pjz_Optimize(int fp) 
{

	pjz_global_env_init();
	pjz_IintExecutable(fp);
	
	pjz_init_stack();

	branch_nums();
	map_space();
	init_counting_insns();
	instrument();
	CodeCacheFlush();	
	
}


void pjz_Collection()
{
	print_counter();
	unmap_space();
}

void (*real_fini)(void);

void Optfini()
{
	pjz_Collection();
	real_fini();
}
*/
