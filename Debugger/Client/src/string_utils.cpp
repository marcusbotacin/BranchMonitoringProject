/* Branch Monitoring Project
 * Debugger Client
 * Marcus Botacin
 * UNICAMP - 2017
 */

/* string utils */
#include "string_utils.h"

/* return string size including \0 */
int string_size(char *str)
{
	return strlen(str)+1;
}

/* build string from option char */
void str_from_char(char c,char *str)
{
	str[0]=c;
	str[1]='\0';
}