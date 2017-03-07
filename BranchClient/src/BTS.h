/* Branch Monitor 
 * Introspection Client
 * Marcus Botacin, 2017
 */

#include "config.h"

/* BTS data
TODO: Use the same .h definition from the driver
*/

typedef struct st_branch
{
	UINT64 FROM,TO,MISC;
}BRANCHDATA,*PBRANCHDATA;
