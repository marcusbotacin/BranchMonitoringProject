/* Branch Monitor
 * Marcus Botacin
 * 2017
 */

#include "config.h"
#include "dbg\debug.h"
#include "checks\checks.h"
#include "device\device.h"
#include "BTS\BTS.h"
#include "list\list.h"

/*************************************************************************
    Prototypes
*************************************************************************/

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    );

VOID BranchMonitorUnload(PDRIVER_OBJECT  DriverObject);



/*************************************************************************
    Driver initialization and unload routines.
*************************************************************************/

PDRIVER_OBJECT drv;

NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )

{
	pdevice_ext ext;
	int ret_code;
    NTSTATUS status=STATUS_SUCCESS;
    UNREFERENCED_PARAMETER( RegistryPath );
	debug("Entry Point: In");

	DriverObject->DriverUnload=BranchMonitorUnload;

	/* Check all required resources */
	if((ret_code=perform_checks())!=NO_ERROR)
	{
		emit_error(ret_code);
		status=STATUS_NOT_SUPPORTED;
	}

	/* Set up device */
	if((status=CreateDevice(DriverObject))!=STATUS_SUCCESS)
	{
		debug("Error Creating Device");
	}

	/* driver data extension */
	debug("Setting driver extension");
	DriverObject->DeviceObject->DeviceExtension=ExAllocatePool(NonPagedPool,sizeof(device_ext));

	/* Hooking PMI handler */
	debug("Registering callback");
	hook_handler();
	
	drv=DriverObject;

	/* setting list */
	
	if(create_list()==FALSE)
	{
		debug("Create List error");
	}

	debug("Entry Point: Out");
    return status;
}

VOID BranchMonitorUnload(PDRIVER_OBJECT  DriverObject)
{
	pdevice_ext ext;
	UNICODE_STRING path;
	debug("Unload Routine: In");

	debug("Unregistering callback");
	ext=(pdevice_ext)DriverObject->DeviceObject->DeviceExtension;
	
	/* Unhook PMI handler */
	unhook_handler();

	ExFreePool(ext);

	debug("clear list entries");
	destroy_list();

	debug("Removing Device");

	RtlInitUnicodeString(&path,DOSDRIVERNAME);
	IoDeleteSymbolicLink(&path);
	IoDeleteDevice(DriverObject->DeviceObject);

	debug("Unload Routine: Out");
	return;
}