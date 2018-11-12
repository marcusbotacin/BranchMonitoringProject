#include "config.h"

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    );

VOID RetMonitorUnload(PDRIVER_OBJECT  DriverObject);

#define BIT(VALUE,OFFSET) ((VALUE & (1<<OFFSET))>>OFFSET)