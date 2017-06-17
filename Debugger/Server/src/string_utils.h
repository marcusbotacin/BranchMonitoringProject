/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* General Includes */
#include "includes.h"

/* Empty string value for memset */
#define EMPTY_STRING 0
#define EMPTY_AS_STRING "0"

#define STR_OFFSET_ADD_SYMBOL " + "

#define READ_FLAG "r"

#define STRING_NEW_LINE "\n"
#define STRING_END '\0'

/* Returns the string size, including \0 */
/* TODO: Could be a macro */
int string_size(char *str);