/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"
#include "addresses.h"

/* max name size */
#define LIB_STRING 1024

/* ERRORs */

/* Unknown function after trying introspect on a given address */
#define UNKNOWN_MESSAGE "Unknown Function"

/* Not inside a binary */

#define NOT_BIN -1

/* Not inside libray */

#define NOT_LIB -1

/* Offset calculation buffer */
#define MAX_OFFSET 64

/* name buffers */
#define MAX_INTROSPECTION_NAMES 1024

#pragma once
/* function offset definition */
typedef struct st_fintro{
	char name[LIB_STRING];
	UINT64 addr;
}FUNCTION_INTROSPECTION,*PFUNCTION_INTROSPECTION;

/* Prototypes */

BOOL is_process(UINT64 addr); /* Check if address is on binary image space */
int is_library(UINT64 addr); /* Check if address is on binary library space */
void flow_analysis(UINT64 FROM,UINT64 TO); /* branch transitions */



