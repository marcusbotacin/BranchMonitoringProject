/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* avoid multiple includes */
#pragma once

/* General includes */
#include "includes.h"

/* screen size */
#define N_COLUMNS 80
#define N_LINES 3

/* line character */
#define LINE_CHAR "*"

/* function prototypes */
void print_line(int size);
void print_head();
void debug_options();
void console_message(char *msg);
void unimplemented();