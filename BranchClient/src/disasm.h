/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"

#define UNKNOWN_PATH -1 /* startup value */

# define MAX_BLOCK_INSTR_BUFFER 1024 /* block buffer */
#define MAX_1_INSTR_BUFFER 32 /*Byte-buffer for disasm 1 instr*/

#define INSTR_MASK 0xFF /*bitmask for disasm */

#define FIRST_INSTR 0 /* vector id */

/* Disasm the currently pointed instruction */
void disasm_one_instr(UINT64 addr);

/* Disasm instruction from last to current branch */
void do_disasm(UINT64 last,UINT64 current);