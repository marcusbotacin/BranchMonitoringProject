/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"
#include "addresses.h"

/* max name size */
#define LIB_STRING 1024

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



