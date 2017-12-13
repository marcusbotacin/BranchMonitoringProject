/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "IO.h"
#include "dbg\debug.h"
#include "Threading\thread.h"
#include "list\list.h"
#include "BTS\BTS.h"
#include <ntstrsafe.h>

/* Monitored PID */
/* Initial value = no process */
ULONG PID = NO_PID;

/* Write data from userland to the driver */
/* Receive process to be monitored */
NTSTATUS Write(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	char data[MAX_MSG_STRING];
	PVOID userbuffer;
	PIO_STACK_LOCATION PIO_STACK_IRP;
	UINT32 datasize,sizerequired;
	NTSTATUS NtStatus = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(DeviceObject);

	/* if data is available, copy to userland */
		userbuffer=Irp->AssociatedIrp.SystemBuffer;
		PIO_STACK_IRP=IoGetCurrentIrpStackLocation(Irp);
		if(PIO_STACK_IRP && userbuffer)
		{
			datasize=PIO_STACK_IRP->Parameters.Write.Length;
			sizerequired=sizeof(char)*MAX_MSG_STRING;
			if(datasize<sizerequired){
				memcpy(data,userbuffer,datasize);
				/* convert bytes to PID */
				RtlCharToInteger(data,10,&PID);
				Irp->IoStatus.Status = NtStatus;
				Irp->IoStatus.Information = sizerequired;
			}else{
				Irp->IoStatus.Status = NtStatus;
				Irp->IoStatus.Information = 0;
			}
		}

	Irp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);

	return NtStatus;

}

/* Write data from driver to the userland stack */
NTSTATUS Read(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	TBTS_BUFFER bdata;
	PVOID userbuffer;
	PIO_STACK_LOCATION PIO_STACK_IRP;
	UINT32 datasize,sizerequired;
	NTSTATUS NtStatus=STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(DeviceObject);
	NtStatus=STATUS_SUCCESS;

	/* remove an item from list */
	if(Remove_List(&bdata)==TRUE)
	{
		/* if data is available, copy to userland */
		userbuffer=Irp->AssociatedIrp.SystemBuffer;
		PIO_STACK_IRP=IoGetCurrentIrpStackLocation(Irp);
		if(PIO_STACK_IRP && userbuffer)
		{
			datasize=PIO_STACK_IRP->Parameters.Read.Length;
			sizerequired=sizeof(TBTS_BUFFER);
			if(datasize>=sizerequired){
				memcpy(userbuffer,&bdata,sizerequired);
				Irp->IoStatus.Status = NtStatus;
				Irp->IoStatus.Information = sizerequired;
			}else{
				Irp->IoStatus.Status = NtStatus;
				Irp->IoStatus.Information = 0;
			}
		}else{
			Irp->IoStatus.Status = NtStatus;
			Irp->IoStatus.Information = 0;
		}
	}else{
		/* if not available, no data is written */
		Irp->IoStatus.Status = NtStatus;
		Irp->IoStatus.Information = 0;
	}

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return NtStatus;

}

/* Create File -- start capture mechanism */
NTSTATUS Create(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	int i;
	NTSTATUS status;
	UNREFERENCED_PARAMETER(DeviceObject);
	debug("Create I/O operation");

	/* I don't know if launching threads inside an I/O routine is OK
	 * Anyway, an IOCTL would be more suitable
	 * The idea here is Create/CLose work as Start/Stop
	 */

	/* Launch setup threads */
	/* for each core */
	for(i=0;i<KeNumberProcessors;i++)
	{
		/* index as thread id */
		control_thread(LOAD_BTS,i);
	}

	Irp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	status = STATUS_SUCCESS;
	return status;
}


/* CLoseFile/CloseHandle -- stop routine */
NTSTATUS Close(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	int i;
	NTSTATUS status;
	UNREFERENCED_PARAMETER(DeviceObject);
	debug("Close I/O operation");

	/* for each core */
	for(i=0;i<KeNumberProcessors;i++)
	{
		/* index as thread id */
		control_thread(UNLOAD_BTS,i);
	}
	
	Irp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	status = STATUS_SUCCESS;
	return status;
}

/* generic routine to support non-implemented I/O */
NTSTATUS NotSupported(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	NTSTATUS status;
	UNREFERENCED_PARAMETER(DeviceObject);
	debug("Not Supported I/O operation");
	Irp->IoStatus.Status=STATUS_NOT_SUPPORTED;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	status = STATUS_NOT_SUPPORTED;
	return status;
}