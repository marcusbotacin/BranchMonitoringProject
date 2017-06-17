/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* Define Screen Size */
#define N_COLUMNS 80
#define N_LINES 3

/* Define print chars */
#define PRINT_CHAR "*"
#define DBG_PRINT_NAME "VoiDbg"

/* enable/disable debug prints */
#define ENABLE_DBG_PRINT

/* prompt char */
#define PROMPT_CHAR "> "

/* Display Functions */
void print_line(int size);
void print_head();
void console_message(char *msg);
void console_message_with_code(char *msg,int code);