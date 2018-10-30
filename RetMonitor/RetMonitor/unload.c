#include "config.h"
#include "debug.h"
#include "BTS.h"

VOID RetMonitorUnload(PDRIVER_OBJECT  DriverObject)
{
	pdevice_ext ext;
	UNICODE_STRING path;
	debug("Unload Routine: In");

	debug("Unregistering callback");
	ext=(pdevice_ext)DriverObject->DeviceObject->DeviceExtension;
	
	/* Unhook PMI handler */
	unhook_handler();

	ExFreePool(ext);

	//debug("clear list entries");
	//destroy_list();

	debug("Removing Device");

	RtlInitUnicodeString(&path,DOSDRIVERNAME);
	IoDeleteSymbolicLink(&path);
	IoDeleteDevice(DriverObject->DeviceObject);

	debug("Unload Routine: Out");
	return;
}