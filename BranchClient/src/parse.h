/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"
#include "addresses.h"


/* Error messages */
static const char *enum_string[]={
	"Everything OK",
	"-bin not found",
	"Other"
};

/* Error codes */

enum error_codes
{
	FOUND_NO_ERROR, /* No error */
	NO_BINARY, /* missing -bin */
	UNDEF_VALUE /* could not convert string to address */
};

/* parser prototype */
int parse(pimgs binary,pimgs libs,int argc,char *argv[],int *lib_count,int *pid);