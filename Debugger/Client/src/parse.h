/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* avoid multiple includes */
#pragma once

/* General includes */
#include "includes.h"

/* MAX string size */
#define MAX_STRING 256

/* String compare value */
#define EQUALS 0

/* minimum number of args */
#define MIN_ARGS 2

/* parse step, skip argument after flag */
#define PARSER_STEP 2

/* function prototypes */
bool parse(int argc,char *argv[],char **sample,char **port,char **addr,char **pid);
void usage(char *argv[]);