/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "list.h"
#include "dbg\debug.h"

/* some things I have to define as global due to IRQL issues */
KSPIN_LOCK list_lock;
KIRQL irq2;
PLIST_ENTRY list_head;

BOOLEAN create_list()
{
		debug("Setting up List");
		KeInitializeSpinLock(&list_lock);
		list_head=(PLIST_ENTRY)ExAllocatePool(NonPagedPool,sizeof(LIST_ENTRY));
	if(list_head==NULL)
	{
		debug("Allocation Error");
		return FALSE;
	}else{
		InitializeListHead(list_head);
	}
	return TRUE;
}

void destroy_list()
{
	TBTS_BUFFER tmp;
	debug("Destroying List");
	while(!IsListEmpty(list_head))
	{
		Remove_List(&tmp);
	}

	ExFreePool(list_head);
}

BOOLEAN Remove_List(PTBTS_BUFFER data)
{
	PLIST_ENTRY removed;
	PLISTDATA removed_content;
	debug("Removing data from list");
	ExAcquireSpinLock(&list_lock,&irq2);
	if(IsListEmpty(list_head))
	{
		KeReleaseSpinLock(&list_lock,irq2);
		return FALSE;
	}

	removed=RemoveHeadList(list_head);
	removed_content=CONTAINING_RECORD(removed,LISTDATA,list);
	KeReleaseSpinLock(&list_lock,irq2);
	data->FROM=removed_content->data.FROM;
	data->TO=removed_content->data.TO;
	data->CORE=removed_content->data.CORE;
	ExFreePool(removed_content);

	return TRUE;
}

VOID Insert_List(ULONG64 FROM,ULONG64 TO, ULONG core)
{
	PLISTDATA data;
	debug("Inserting data into list");
	data=(PLISTDATA)ExAllocatePool(NonPagedPool,sizeof(LISTDATA));
	if(data!=NULL)
	{
		data->data.FROM=FROM;
		data->data.TO=TO;
		data->data.CORE=core;
		ExAcquireSpinLock(&list_lock,&irq2);
		InsertTailList(list_head,&(data->list));
		KeReleaseSpinLock(&list_lock,irq2);
	}
}
