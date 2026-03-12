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
/**
 * @file		hal_disp_lcm_cmd.c
 * @brief		hal api for disp lcm command.
 * @author		Matt Wang
 * @since		2008-06-16
 * @date		2008-10-24
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "reg_def.h"
#include "hal.h"
#include "hal_disp_internal.h"
#include "hal_disp.h"
#include "hal_global.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define HAL_DISP_LCM_CMD_DUMP_DBG  0

typedef enum {
	HAL_DISP_LCM_CMD_HDR_INST = 0x01,
	HAL_DISP_LCM_CMD_HDR_PARA = 0x02,
	HAL_DISP_LCM_CMD_HDR_WAIT = 0x03,
	HAL_DISP_LCM_CMD_HDR_STOP = 0x04,
} HAL_DispLcmCmdHeader_e;

#define LCM_CMD_GPIO_CTRL    1
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
UINT8 HAL_DispCs0Act;
UINT8 HAL_DispCs0Inact;
UINT8 HAL_DispA0Cmd;
UINT8 HAL_DispA0Data;

/**
 * @brief	send command table to lcm interface.
 * @param	cmdTbl = [in] command table.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-06-16
 * @todo	N/A
 * @bug		N/A
*/
UINT8
HAL_DispLcmCmdTableSend(
	UINT32 *cmdTbl
)
{
	UINT8 run, cmdHdr;
	UINT32 cmd;
	UINT8 pinWR;

	/* if dispEn or dispPhaseClkEn disabled, no need to send. */
	if ( !(XBYTE[REG_Disp_DispEn_Flip] & 0x01) || !(XBYTE[0x2013] & 0x10) ) {
        //printf("disp disable\n");
		return FAIL;
	}

	/* wait current frame data done. */
	XBYTE[REG_Disp_LcmVSUpd_DmaEn] &= ~0x10;  /* disable dma. */
	while ( XBYTE[REG_Disp_LcmVSUpd_DmaEn] & 0x20 ) {  /* wait current frame image data done. */
	}

#if LCM_CMD_GPIO_CTRL	
	/* check the WR active */
	if (XBYTE[REG_Disp_LcmMode_BusW] & 0x01) {
		pinWR = 0x01; //M68
	} else {
		pinWR = 0x00; //i80
	}
	HAL_GpioByteFuncSet(HAL_GPIO_BYTE_DISP1, 0x01 << (11 & 0x07), 0x01 << (11 & 0x07));
	HAL_GpioByteOutSet(HAL_GPIO_BYTE_DISP1, 0x01 << (11 & 0x07), !pinWR << (11 & 0x07));
	HAL_GpioByteDirSet(HAL_GPIO_BYTE_DISP1, 0x01 << (11 & 0x07), 0x01 << (11 & 0x07));
#endif
    
    /* write command main body. */
    XBYTE[REG_Disp_LcmCmdCtrl] |= 0x01; // enable command mode
	XBYTE[REG_Disp_LcmCS_RS] = HAL_DispCs0Act | HAL_DispA0Cmd;
    run = 1;

    while(run) {
		cmdHdr = READ8(*cmdTbl, 0);
		cmd = *cmdTbl & 0x00FFFFFFL;
#if 0
		printf("cmdHdr=0x%02X cmd=0x%06lX\n", (UINT16)cmdHdr, cmd);
#endif

		if ( cmdHdr <= HAL_DISP_LCM_CMD_HDR_PARA ) {
			if ( cmdHdr == HAL_DISP_LCM_CMD_HDR_INST ) {
				XBYTE[REG_Disp_LcmCS_RS] = HAL_DispCs0Act | HAL_DispA0Cmd;
			}
			else {  /* HAL_DISP_LCM_CMD_HDR_PARA. */
				XBYTE[REG_Disp_LcmCS_RS] = HAL_DispCs0Act | HAL_DispA0Data;
			}        
 			XBYTE[REG_Disp_LcmAutoCmd00] = READ8(cmd, 3);  /* lcmcmd[7:0]   */
			XBYTE[REG_Disp_LcmAutoCmd01] = READ8(cmd, 2);  /* lcmcmd[15:8]  */
			XBYTE[REG_Disp_LcmAutoCmd02] = READ8(cmd, 1);  /* lcmcmd[17:16] */

#if !LCM_CMD_GPIO_CTRL            
			XBYTE[REG_Disp_LcmPioTrg_Busy] = 0x01;  /* trig piowrite. */
			while(XBYTE[REG_Disp_LcmPioTrg_Busy] & 0x10); /* wait command finish */
#else
			HAL_GpioByteOutSet(HAL_GPIO_BYTE_DISP1, 0x01 << (11 & 0x07), pinWR << (11 & 0x07));
			HAL_GpioByteOutSet(HAL_GPIO_BYTE_DISP1, 0x01 << (11 & 0x07), !pinWR << (11 & 0x07));
#endif
		}
		else if ( cmdHdr == HAL_DISP_LCM_CMD_HDR_WAIT ) {
			HAL_GlobalTimerWait(cmd);
		}
		else {  /* HAL_DISP_LCM_CMD_HDR_STOP. */
			run = 0;
		}

		cmdTbl++;
        
    }

#if LCM_CMD_GPIO_CTRL
	HAL_GpioByteFuncSet(HAL_GPIO_BYTE_DISP1, 0x01 << (11 & 0x07), 0x00 << (11 & 0x07));
#endif

	/* restore to frame data mode. */
	XBYTE[REG_Disp_LcmCmdCtrl] &= ~0x01;  /* disable command mode. */
	XBYTE[REG_Disp_LcmCS_RS] = HAL_DispCs0Act | HAL_DispA0Data;
	XBYTE[REG_Disp_LcmVSUpd_DmaEn] |= 0x10;  /* enable dma. */

	return SUCCESS;
}

