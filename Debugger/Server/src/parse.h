/* BranchMonitoring Project 
 * Debugger Server-side
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* Avoid multiple includes */
#pragma once

/* Define when two things are equal */
#define EQUAL 0
#define PARSE_ERROR -1
#define PARSER_FAILED FALSE

/* Parser Functions */
void usage(char *argv[]);
bool Parse(int argc,char *argv[],char **port);
