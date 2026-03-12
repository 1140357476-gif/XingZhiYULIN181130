/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "utility.h"
#include "Sdramhw.h"

#include "Asicreg.h"
#include "gpio_api.h"
#include "hal_gpio.h"
#include "initio.h"
#include "sp1k_util_api.h"
#include "hal_dram.h"
#include "hal_global.h"
#include "pv_photoframe.h"
#include "reg_def.h"
//#include "video_clip.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
extern UINT8 flag1627;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
void
utilRegDump(
	UINT16 reg
)
{
	UINT16 i;

	for( i = 0; i < 256; i++ ) {
		if( i % 16 == 0x00 ) {
			printf( "%04X:", reg + i );
		}

		printf( " %02bX", XBYTE[ reg + i ] );

		if( i % 16 == 0x07 ) {
			printf( " |" );
		}

		if( i % 16 == 0x0f ) {
			printf( "\n" );
		}
	}

	return;
}


/**
 * @fn        void sp1kSuspendUiIntrSet(UINT8 enable)
 * @brief     sp1kSuspendUiIntrSet
 * @param     [in] enable
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2011-2-28
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSuspendUiIntrSet(UINT8 enable)
{
	if(enable)
	{
		/*UI Interrupt seting*/
		XBYTE[0x20C0] &= ~0x01;//UI resume interrupt event clear
		XBYTE[0x20D0] |= 0x21;//UI & RTC interrupt enable
		/*RDK Snap key resume*/
		XBYTE[0x203D] = 0x00;//gpio9 rising event clear
		XBYTE[0x207B] = 0x00;//gpio9 falling event clear
		XBYTE[0x2041] |= 0x02;//gpio9 rising interrupt enable
		XBYTE[0x2045] |= 0x02;//gpio9 falling interrupt enable


		if(flag1627 == 2)
		{
			XBYTE[0x2D02] &= ~0x02;//Dgpio Vsync rising interrupt disable
			XBYTE[0x2D00] = 0x02;//Dgpio Vsync falling intr sts clear
		}

		XBYTE[0x2708] &= ~0x20;//cdsp1sof_mpgsb_sts interrupt disable
		XBYTE[0x2700] = 0x20;//cdsp1sof_mpgsb_sts interrupt sts clear

		//printf("2648=%bx,2603=%bx,260b=%bx,203d=%bx\n",XBYTE[0x2648],XBYTE[0x2603],XBYTE[0x260b],XBYTE[0x203d]);
	}
	else
	{
		XBYTE[0x20C0] &= ~0x01;//UI resume interrupt event clear
		XBYTE[0x20D0] &= ~0x21;//UI & RTC interrupupt Disable
		/*RDK Snap key resume*/
		XBYTE[0x2041] &= ~0x02;//gpio9 rising interrupt Disable
		XBYTE[0x2045] &= ~0x02;//gpio9 falling interrupt Disable
		XBYTE[0x203D] = 0x00;//gpio9 rising event clear
		XBYTE[0x207B] = 0x00;//gpio9 falling event clear

		if(flag1627 == 2)
		{
			XBYTE[0x2D00] = 0x02;//Dgpio Vsync falling intr sts clear
			XBYTE[0x2D02] |= 0x02;//Dgpio Vsync rising interrupt enable
		}

		XBYTE[0x2700] = 0x20;//cdsp1sof_mpgsb_sts interrupt sts clear
		XBYTE[0x2708] |= 0x20;//cdsp1sof_mpgsb_sts interrupt enable

	}

}





/**
 * @brief	sp1k API Do Suspend Resume.
 * @param	None
 * @retval	void
 * @see
 * @author	Eric Su
 * @since	2008-03-14
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
 */
UINT8 sp1kSuspendResume(void) USING_0
{
	UINT32 i;
	UINT8 buf[2];
	UINT8 status, Temp2AD0, Temp21EC,Temp2277,Temp2a3a,Temp27f2;

	ENTER_CRITICAL( status );
	/* Save interrupt setting */
	Temp2AD0 = XBYTE[REG_Front_Finten_low];
	Temp21EC = XBYTE[REG_Cdsp_Cdspinten_lo];
	Temp2277 = XBYTE[0x2277];
	Temp2a3a = XBYTE[0x2A3A];
	Temp27f2 = XBYTE[REG_Dram_FrcEn];

	XBYTE[REG_Front_Finten_low] = 0x00;
	XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
	XBYTE[REG_Cpu_ShadowEn] &= 0xfe;	/* Shadow imem disable */
	SDRAM_CardSetStartAddress(0x2000 >> 1, TRUE);
	for( i = 0 ; i < 0x700 ; i = i + 2 ){
		M_DRAM_ReadWord(buf[0],buf[1]);
		XBYTE[(UINT16)0x3000 + (UINT16)i] = buf[0];
		XBYTE[(UINT16)0x3000 + (UINT16)(i + 1)] = buf[1];
	}
	HAL_GlobalSuspendResume();
	XBYTE[REG_Cpu_ShadowEn] &= 0xfe;	/* Shadow imem disable */
	//HAL_CpuDramSdToImem();

	XBYTE[0x2288] = 0x00;
	/* Restore interrupt setting */
	Temp2a3a = 0x10;
	XBYTE[REG_Cdsp_Cdspinten_lo] = Temp21EC;
	XBYTE[REG_Front_Finten_low] = Temp2AD0;
	XBYTE[0x2277] = Temp2277;
	XBYTE[0x2A3A] = Temp2a3a;
	XBYTE[REG_Dram_FrcEn] = Temp27f2;
	EXIT_CRITICAL( status );
	return SUCCESS;
}


