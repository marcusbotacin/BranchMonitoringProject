/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"

#pragma once
/* structure to handle LIB addresses */
typedef struct _imgs
{
	char *name; /* lib name */
	UINT64 addr_min; /* base address */
	UINT64 addr_max; /* considering image size */
}imgs,*pimgs;