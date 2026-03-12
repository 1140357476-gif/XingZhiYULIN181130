/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      dbg_mem.c
 * @brief     Brief
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "asicreg.h" 
#include "dbg_mem.h" 
#include "initio.h" 
#include "reg_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @brief     dbgMemDump
 * @param     [in] addr
 * @param     [in] total
 * @param     [in] mode
 * @retval    NONE
 * @see       none
 * @author    Phil Lin
 * @since     2010-01-01
 * @todo      N/A
 * @bug       N/A
*/
void
dbgMemDump(
	UINT32 total,
	UINT32 addr,
	UINT8 mode
)
{
	UINT32 i, k;
	UINT8 LSB, MSB;
	UINT8 xdata* pData;
	
	if( mode == 0 ){
		k = 0;
		pData = (UINT8 xdata*)addr;
		//printf("total=%lx, addr=%lx\n", total, (UINT32)addr & 0x0000ffff);
		while( 1 ){
			printf("X:0x%04lX ", addr);
			for( i = 0; i < 16 ; i ++ ){
				printf("%02bX ", *pData++);
				k++;
				if( k >= total ){
					printf("\n");
					return;
				}
			}
			printf("\n");
			addr += 16;
		} 
	}
	else{
		//printf("total=%lx, addr=%lx\n", total, address);
		M_DRAM_SetStartAddress(addr, TRUE);
		total = total >> 4;
		for(k = 0; k < total; k++){
			printf("D:%06lX.W ", addr);
			for( i = 0; i < 16; i = i + 2){
				M_DRAM_ReadWord(LSB, MSB);
				printf("%02bX %02bX ", LSB, MSB);
			}
			printf("\n");
			addr += 8;
		}
	}
}

#if DBG_MEM_RPT
/**
 * @brief     dump the dram consumption
 * @param     [in] ptr
 * @retval    NONE
 * @see       none
 * @author    Phil Lin
 * @since     2010-01-01
 * @todo      N/A
 * @bug       N/A
*/
void
dbgMemRpt(
	UINT8* ptr
)
{
	ptr = ptr;
	printf("COM size:0x%lx\n", K_SDRAM_CommonEndAddr << 1);
}
#endif //DBG_MEM_RPT
