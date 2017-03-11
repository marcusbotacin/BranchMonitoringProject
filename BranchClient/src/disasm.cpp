/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "disasm.h"
#include "C:\Users\marcus\Downloads\capstone-3.0.4-win64\include\capstone.h"

extern HANDLE proc;

#pragma comment(lib, "C:\\Users\\marcus\\Downloads\\capstone-3.0.4-win64\\capstone.lib")

/* Disasm the current instruction */
void disasm_one_instr(UINT64 addr)
{
	/* since we don't know which instruction is
	 * and x86 instructions are not fixed-size
	 * alloc some extra bytes 
	 */
	uint8_t instruction_buffer[MAX_1_INSTR_BUFFER];
	cs_insn *insn;
	size_t count;
	csh handle;
	int i,n=MAX_1_INSTR_BUFFER;
	char buffer[MAX_1_INSTR_BUFFER];

	/* some debugging */
	/* TODO: verbosity control */
	printf("Disasm 1 instr from %llx\n",addr);

	/* Read some bytes from the memory pointed by addr */
	if(ReadProcessMemory(proc,(LPCVOID)addr,&buffer,n,NULL)>0)
	{
		for(i=0;i<n;i++)
		{
			/* mask to avoid close bytes, sign extension and other issues */
			instruction_buffer[i]=buffer[i] & INSTR_MASK;
		}
	}

	if(cs_open(CS_ARCH_X86,CS_MODE_64,&handle) != CS_ERR_OK)
	{
		printf("cs error\n");
	}else{
		count = cs_disasm(handle,instruction_buffer,n,0x1000,0,&insn);
		if(count>0)
		{
			/* consider only the first instruction */
			/* remember the buffer can contain more than one instr
			* since we guessed its size */
			printf("%s %s\n",insn[FIRST_INSTR].mnemonic,insn[FIRST_INSTR].op_str);
		}else{
			printf("disasm error\n");
		}
	}

}

/* disasm entire blocks of code starting on last addr until the current */
void do_disasm(UINT64 last,UINT64 current)
{
	uint8_t instruction_buffer[MAX_BLOCK_INSTR_BUFFER];
	cs_insn *insn;
	size_t count;
	csh handle;
	int i;
	UINT64 n=current-last; /* block size */
	char buffer[MAX_BLOCK_INSTR_BUFFER];
	printf("should disasm from %llx to %llx\n",last,current);
	if(ReadProcessMemory(proc,(LPCVOID)last,&buffer,n,NULL)>0)
	{
		for(i=0;i<n;i++)
		{
			instruction_buffer[i]=buffer[i] & INSTR_MASK;
		}
	}

	if(cs_open(CS_ARCH_X86,CS_MODE_64,&handle) != CS_ERR_OK)
	{
		printf("cs error\n");
	}else{
		count = cs_disasm(handle,instruction_buffer,n,0x1000,0,&insn);
		if(count>0)
		{
			size_t j;
			/* for each instr */
			for(j=0;j<count;j++)
			{
				printf("%s %s\n",insn[j].mnemonic,insn[j].op_str);
			}
			/* we have the disasm for the executed block */
			/* now we want to know what is the current instr */
			/* it will be a branch-related instr (block ending)
			*/
			disasm_one_instr(current);
			/* we have to disasm it now because 
			 * it will not appear on the next block disasm
			 * and we don't want to lose any information
			 */
		}else{
			printf("disasm error\n");
		}
	}
}
