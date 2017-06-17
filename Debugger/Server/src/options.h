/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* General Includes */
#include "includes.h"

#define SAMPLE_UPLOAD 0

/* Command Options Definitions */
#define NEXT_BRANCH "n"
#define MEM_DUMP "d"
#define SUSPEND "s"
#define REGISTERS "R"
#define INTROSPECT_FROM "i"
#define INTROSPECT_TO "I"
#define LIST_MODULES "l"
#define GET_NAME "N"
#define RESUME "r"
#define QUIT "q"
#define CONTEXT "C"