/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "IO.h"
#include "dbg\debug.h"
#include "Threading\thread.h"
#include "list\list.h"
#include "BTS\BTS.h"

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
	NTSTATUS status;
	UNREFERENCED_PARAMETER(DeviceObject);
	debug("Create I/O operation");

	/* I don't know if launching threads inside an I/O routine is OK
	 * Anyway, an IOCTL would be more suitable
	 * The idea here is Create/CLose work as Start/Stop
	 */

	/* Launch setup threads */
	control_thread(LOAD_BTS,BTS_CORE);
	control_thread(LOAD_WORKER,SYSTEM_WIDE);

	Irp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	status = STATUS_SUCCESS;
	return status;
}


/* CLoseFile/CloseHandle -- stop routine */
NTSTATUS Close(PDEVICE_OBJECT DeviceObject,PIRP Irp)
{
	NTSTATUS status;
	UNREFERENCED_PARAMETER(DeviceObject);
	debug("Close I/O operation");

	control_thread(UNLOAD_WORKER,SYSTEM_WIDE);
	control_thread(UNLOAD_BTS,BTS_CORE);

	/* On a multicore scenario, you can do something like:
	 * n_proc=KeNumberProcessors;
	 * for(i=0;i<n_proc;i++)
	 * control_thread(action,i);
	 */

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