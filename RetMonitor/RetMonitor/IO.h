/* Branch Monitor - Multi-Core version
 * Marcus Botacin - 2017
 * Federal University of Paraná (UFPR)
 */

#include "config.h"

/* I/O routines */
NTSTATUS NotSupported(PDEVICE_OBJECT DeviceObject,PIRP Irp);
NTSTATUS Write(PDEVICE_OBJECT DeviceObject,PIRP Irp);
NTSTATUS Read(PDEVICE_OBJECT DeviceObject,PIRP Irp);
NTSTATUS Create(PDEVICE_OBJECT DeviceObject,PIRP Irp);
NTSTATUS Close(PDEVICE_OBJECT DeviceObject,PIRP Irp);