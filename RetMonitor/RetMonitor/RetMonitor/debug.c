/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "debug.h"
#include "checks.h"

/* DbgPrint wrapper
 * It is a way of automatically printing driver name every call
 * Useful for debug filtering -- string-based
 */
void debug(char msg[])
{
#ifdef DEBUG
	DbgPrint("[%s] %s",DRIVER_NAME,msg);
#else
	UNREFERENCED_PARAMETER(msg);
	return;
#endif
}

/* Printing error message associated to an error code */
void emit_error(int code){
	DbgPrint("[%s] ERROR: %s",DRIVER_NAME,enum_string[code]);
}

void emit_status(NTSTATUS status)
{
	char message[MAX_MSG_STRING];
	sprintf(message,"[%s] Status: %llx",DRIVER_NAME,status);
	DbgPrint(message);
}