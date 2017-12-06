/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "config.h"
#include "BTS\BTS.h"

/* List operations */

BOOLEAN Remove_List(PTBTS_BUFFER data);
VOID Insert_List(ULONG64 FROM,ULONG64 TO);
BOOLEAN create_list();
void destroy_list();

typedef struct st_list_data
{
	TBTS_BUFFER data;
	LIST_ENTRY list;
}LISTDATA,*PLISTDATA;

