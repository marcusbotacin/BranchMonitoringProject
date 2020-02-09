/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "config.h"
#include "debug.h"
#include "checks.h"
#include "device.h"
#include "BTS.h"
#include "list.h"
#include "driver.h"
#include "LBR.h"

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

	DriverObject->DriverUnload=RetMonitorUnload;

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
	//if(create_list()==FALSE)
	//{
	//	debug("Create List error");
	//}

	

	debug("Entry Point: Out");
    return status;
}