/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "config.h"
#include<ntstrsafe.h>

/* Debug messages prototype */

#define MAX_MSG_STRING 200

//#define DEBUG

void debug(char msg[]);
void emit_error(int code);
void emit_status(NTSTATUS status);