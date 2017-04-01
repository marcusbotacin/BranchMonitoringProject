/* Marcus Botacin
 * UNICAMP - 2017
 * GDB Stub implementation
 * BranchMonitoring Project
 */

/* avoid multiple includes */
#pragma once

#include<stdio.h>
#include<Windows.h>

/* Trap flag definition
 * Required for original GDB backend
 * Not used on BranchMonitor 
 */
#define X86_TRAP_FLAG (1 << 8)

/* define architectures will be debugged
 * It changes the registers will be available 
 */

#define __x86_64__
#define __i386__

/* state struct */

struct state {
	FILE *fp;
	int first_break;
};

/* Register name enumeration */

enum
{
  REG_R8 = 0,
# define REG_R8		REG_R8
  REG_R9,
# define REG_R9		REG_R9
  REG_R10,
# define REG_R10	REG_R10
  REG_R11,
# define REG_R11	REG_R11
  REG_R12,
# define REG_R12	REG_R12
  REG_R13,
# define REG_R13	REG_R13
  REG_R14,
# define REG_R14	REG_R14
  REG_R15,
# define REG_R15	REG_R15
  REG_RDI,
# define REG_RDI	REG_RDI
  REG_RSI,
# define REG_RSI	REG_RSI
  REG_RBP,
# define REG_RBP	REG_RBP
  REG_RBX,
# define REG_RBX	REG_RBX
  REG_RDX,
# define REG_RDX	REG_RDX
  REG_RAX,
# define REG_RAX	REG_RAX
  REG_RCX,
# define REG_RCX	REG_RCX
  REG_RSP,
# define REG_RSP	REG_RSP
  REG_RIP,
# define REG_RIP	REG_RIP
  REG_EFL,
# define REG_EFL	REG_EFL
  REG_CSGSFS,		/* Actually short cs, gs, fs, __pad0.  */
# define REG_CSGSFS	REG_CSGSFS
  REG_ERR,
# define REG_ERR	REG_ERR
  REG_TRAPNO,
# define REG_TRAPNO	REG_TRAPNO
  REG_OLDMASK,
# define REG_OLDMASK	REG_OLDMASK
  REG_CR2
# define REG_CR2	REG_CR2
};

/* GDB context struct */

struct gdb_regs {
#if defined(__i386__)
	UINT32 eax, ecx, edx, ebx, esp, ebp, esi, edi;
	UINT32 eip, eflags;
	UINT32 cs, ss, ds, es, fs, gs;
#elif defined(__x86_64__)
	UINT64 rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp;
	UINT64 r8, r9, r10, r11, r12, r13, r14, r15;
	UINT64 rip;
	UINT32 eflags;
	UINT32 cs, ss, ds, es, fs, gs;
#endif
};

/* Functions */

/* fill data
 * put your data on gdb internal struct
 * change function prototype to include your data
 */

VOID fill_Data(struct gdb_regs *regs);
int hex_to_int(char ch);
char int_to_hex(unsigned val);
void write_hex_byte(char *dest, UINT8 byte);
void write_hex_bytes(char *dest, UINT8 *data, size_t size);
int log_getc(FILE *fp);
void copy_regs_to_gdb(struct gdb_regs *regs, struct gdb_regs *mcontext);
void get_packet(FILE *fp, char *buffer, int buffer_size);
void get(char request[],int size);
void put_packet(FILE *fp, char *packet);

/* constants */

#define HEX 16
#define NUMBER_BASE 10
#define FIRST_PART_BYTE 0
#define SECOND_PART_BYTE 1
#define BYTE_OFFSET 4
#define BYTE_MASK 0xF
