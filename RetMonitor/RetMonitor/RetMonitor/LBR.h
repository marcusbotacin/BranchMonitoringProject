/* Branch Monitor - RetMonitor version
 * Marcus Botacin - 2018
 * Federal University of Paraná (UFPR)
 */

#define LBR_TOS 0x1C9
#define LBR_ENABLE 1
#define LBR_DISABLE 0
#define LBR_MSR 473
#define LBR_BITMASK 31
#define LBR_DISABLE_KERNEL 1
#define LBR_DISABLE_USER 1<<1
#define LBR_DISABLE_JCC 1<<2
#define LBR_DISABLE_REL_CALL 1<<3
#define LBR_DISABLE_IND_CALL 1<<4
#define LBR_DISABLE_NEAR_RET 1<<5 
#define LBR_DISABLE_NEAR_IND_JMP 1<<6
#define LBR_DISABLE_NEAR_REL_JMP 1<<7
#define LBR_DISABLE_FAR_BRANCH 1<<8
#define MSR_LBR_SELECT 456
//#define LBR_BASE_FROM 0x40
#define LBR_BASE_FROM 0x680

void enable_lbr();
void disable_lbr();
UINT64 get_lbr_tos();