/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* avoid multiple includes */
#pragma once

/* integer definitions */
#define SUSPENDED_PROCESS 0
#define END_OF_FILE 0

/* char definitions */
#define SLASH '\\'
#define CONTINUE_CHAR 'c'
#define BREAK_CHAR 'b'
#define LIST_CHAR 'l'
#define NAME_CHAR 'N'
#define DISASM_CHAR 'D'
#define DUMP_CHAR 'd'
#define QUIT_CHAR 'q'
#define HELP_CHAR 'h'
#define NEXT_CHAR 'n'
#define REG_CHAR 'R'
#define IFROM_CHAR 'i'
#define ITO_CHAR 'I'
#define SUSP_CHAR 's'
#define RESUME_CHAR 'r'
#define CTX_CHAR 'C'

/* string definitions */
#define ZERO_STRING "0"
#define CONTINUE_MSG "continue"
#define ERROR_MSG "error"
#define BREAK_MSG "breakpoint"
#define LIST_MSG "list loaded libs"
#define NAME_MSG "Retrieve name"
#define DISASM_MSG "Disasm code"
#define DUMP_MSG "dump memory"
#define DUMP_BASE_REQ_MSG "enter the base address"
#define DUMP_SIZE_REQ_MSG "enter the number of bytes"
#define QUIT_MSG "quit"
#define HELP_MSG "help"
#define NEXT_MSG "next branch"
#define REG_MSG "Register values"
#define IFROM_MSG "Introspect From"
#define ITO_MSG "Introspect To"
#define SUSP_MSG "suspend"
#define RESUME_MSG "resume"
#define CTX_MSG "Context"
#define SAMPLE_OPTION "-s"
#define PORT_OPTION "-p"
#define PID_OPTION "-pid"
#define ADDR_OPTION "-a"
