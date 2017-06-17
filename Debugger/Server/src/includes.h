/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* General Libraries to be included */
#include <windows.h>	/* Windows Internals */
#include<stdio.h>		/* I/O utils *
#include<stdlib.h>		/* Default Lib Utils */
#include<string.h>		/* String Utils */
#include<stdlib.h>		/* Default Lib Utils */
#include<ctype.h>		/* Types */
#include <winioctl.h>	/* I/O Control */
#include<TlHelp32.h>	/* Process Utils */
#include<Psapi.h>		/* Process Utils */