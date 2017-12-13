/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "config.h"
#include<ntstrsafe.h>

/* Debug messages prototype */

#define MAX_MSG_STRING 200

void debug(char msg[]);
void emit_error(int code);
void emit_status(NTSTATUS status);