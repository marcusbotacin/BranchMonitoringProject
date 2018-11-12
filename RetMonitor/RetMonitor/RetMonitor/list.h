/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "config.h"
#include "BTS.h"

/* List operations */

BOOLEAN Remove_List(PTBTS_BUFFER data);
VOID Insert_List(ULONG64 FROM,ULONG64 TO,ULONG core);
BOOLEAN create_list();
void destroy_list();

typedef struct st_list_data
{
	TBTS_BUFFER data;
	LIST_ENTRY list;
}LISTDATA,*PLISTDATA;

