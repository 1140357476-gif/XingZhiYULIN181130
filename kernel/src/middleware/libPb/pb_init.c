/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 
/**
 * @file	  pb_init.c
 * @brief	  set and get play back parameters
 * @author	  Tq.Zhang
 * @since	  2010-05-05
 * @date	  2010-05-05
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "hal_dram.h"
#include "hal_disp.h"
#include "initio.h"
#include "hal_front.h"
#include "hal_front_para.h"
//#include "dosusr.h"
#include "doslink.h"
#include "sdramhw.h"
//#include "disk.h"
#include "fileindex.h"
#include "pb_general.h"
#include "pb_api.h"
#include "disp_api.h"
#include "app_main.h"
#include "pb_debug.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "hal_pb.h"
#include "hal_gprm.h"
#include "hal_global.h"
#include "dcf_api.h"
#include "reg_def.h"
#include "os_api.h"

/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
UINT16 xdata pbParam[PB_PARAM_MAX];
//static UINT8 xdata pbDispBuf = PB_DISP_BUF_A;
//static UINT16 xdata pbDecodeW = PB_DECODE_WIDTH;
//static UINT16 xdata pbDecodeH = PB_DECODE_HEIGHT;

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
 
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//pbInit()
//-----------------------------------------------------------------------------
/**
 * @fn		  void pbInit(void)
 * @brief	  play back initialize directory intormation
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_LIB_PB_DMY
void pbInit(void) USING_0
{
}
#else
void pbInit(void) USING_0
{
	UINT8 sts;
	UINT16 /*TempCountIndex,*/ TempCount;
	//UINT8 tmp1, tmp2;
	//UINT16 startCluster;
	// Disable frame rate conversion
	// set display buffer index is equal to B frame buffer index

	XBYTE[0x20D0]  = XBYTE[0x20D0] & 0xDF;// ylee@0320 : Turn off VD falling interrupt
	// reset to idle mode when Initial playback
	if ( (XBYTE[0x2288] == 0x00) && (XBYTE[0x2277] & 0x01 == 0x00) ) {//check if image of cdsp source is from sensor
		while((XBYTE[REG_Dram_MpgSb_CdspWRegIdx]&0x03) != 0x01){//wait until CDSP write preview BFB
			printBugReproduce("27B1=%bx\n",XBYTE[0x27B1]); 
		}//There exits a bug,but can't reproduce now,so add printf info here
	}
	HAL_CdspRawImgSource(1);
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
	HAL_GlobalCamModeSet(CAM_MODE_IDLE);
	// set camera operation mode: playback
	HAL_GlobalCamModeSet(CAM_MODE_PB);

	pbDispBufModeSet(PB_DISP_BUF_MODE_INIT); //xian ++
	osSysModeSet(OS_SYS_MODE_STILL_PB);
	/* power control -- still playback */
	//appModePowerCtrl(MODE_STILLPB);

		//Phil: BFB size is the same as AFB?
		//HAL_SetBFBSize(G_DSPHsize,G_DSPVsize);

	//If this directory has no file, then change directory until find one
#if 1	//xian rewrite, but not tested
	TempCount = dcfTotalDirectoryGet();
	while (dcfFileCountGet() == 0 && TempCount){
		sts = dcfFolderNext(1);
		if(sts == FAIL){ //only one dir
			break;
		}
		
		sts = dcfFileInitialize();
		TempCount --;
	}
#else
	if ((G_USR_FileCount == 0) && (G_USR_TotalDirectory > 1)) {
		//patch4.4@ada@DirectoryIndexOrder begin
		//TempCountIndex = G_USR_Dir1CountIndex;
		TempCountIndex = G_USR_Dir1Count;
		//If all directory has no file,then switch back to the default directory.
		TempCount = G_USR_TotalDirectory + 1;

		//Find a directory with file exists
		while (TempCount > 0) {
			//printf("Switch forward directory Once\n");

			HAL_DramStartAddrSet((K_SDRAM_DcfDirBufAddr + (((UINT32) TempCountIndex) - 100) * 8 + 7), K_SDRAM_Prefetch);
			HAL_DramWordDataRead(&tmp1, &tmp2);
			TempCountIndex = (UINT16) tmp1 | (((UINT16) tmp2) << 8);

			if (TempCountIndex == 0) {
				TempCountIndex = G_USR_MinDir1Count;
			}

			HAL_DramStartAddrSet((K_SDRAM_DcfDirBufAddr
							   + ((UINT32) TempCountIndex - 100) * 8),
								K_SDRAM_Prefetch);
			HAL_DramWordDataRead(&tmp1, &tmp2);
			startCluster = ((UINT16) tmp2) << 8 | (UINT16) tmp1;

			G_USR_Dir1Cluster = startCluster;

			G_USR_Dir1Count = TempCountIndex;

#if TAG_TODO
			DOS_SetCurrDir(startCluster);
#endif
			sts = dcfFileInitialize();

			//G_USR_FileIndex++;  // [33792] By phil@08.11.13 #Already added in dcf init

			//Find directory with file in it
			if ((sts) && (G_USR_FileCount > 0)) {
				break;
			}

			TempCount--;
		}
	}
	//patch4.4@ada@DirectoryIndexOrder end

	G_USR_SearchNowFileIndex = G_USR_LastFileIndex;
#endif

#if (DBG_Level > 0)
	printf("FileCount = %u\n", G_USR_FileCount);
#endif //(DBG_Level > 0)
}
#endif
/**
 * @fn		  void pbDispInit(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize)
 * @brief	  initialize display parameters
 * @param	  [in] srcAddrWord : display source buffer
 * @param	  [in] srcHSize : source width (pixel)
 * @param	  [in] srcVSize : source height (pixel)
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_LIB_PB_DMY
void pbDispInit(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize) USING_0
{
        srcAddrWord=srcAddrWord;
        srcHSize=srcHSize;
        srcVSize=srcVSize;
}
#else
void pbDispInit(UINT32 srcAddrWord, UINT16 srcHSize, UINT16 srcVSize) USING_0
{
	UINT32 startAddr = pbDispBuffGet(1); //
	UINT32 freezenAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 endAddr = K_SDRAM_TotalSize;
	UINT32 tempAddr;
	UINT16 decWidth = srcHSize;
	UINT16 decHeight = srcVSize;

	pbDecodeSizeSet(decWidth, decHeight);
	tempAddr = srcAddrWord;
	if(tempAddr > startAddr) {
		if(tempAddr - startAddr >= (UINT32)srcHSize * (UINT32)srcVSize){
			HAL_GprmCopy(tempAddr, srcHSize, srcVSize, 0, 0,
							startAddr, srcHSize, srcVSize, 0, 0,
							srcHSize,srcVSize, 0, 1);
			dispPbBufSet(startAddr, decWidth, decHeight);
			HAL_DispScaleSet(decWidth, decHeight, 100,1);
			//HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);//phil: No waiting needed, use the internal sync!
			HAL_PbSyncPara2DispSof();	// For update display controller registers
		} else {
			HAL_GprmCopy(tempAddr, srcHSize, srcVSize, 0, 0,
							tempAddr + (UINT32)srcHSize * (UINT32)srcVSize, srcHSize, srcVSize, 0, 0,
							srcHSize,srcVSize, 0, 1);
			dispPbBufSet(tempAddr + (UINT32)srcHSize * (UINT32)srcVSize, decWidth, decHeight);
			HAL_DispScaleSet(decWidth, decHeight, 100,1);
			//HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);//phil: No waiting needed, use the internal sync!
			HAL_PbSyncPara2DispSof();			  // For update display controller registers
			HAL_GprmCopy(tempAddr + (UINT32)srcHSize * (UINT32)srcVSize, srcHSize, srcVSize, 0, 0,
							startAddr, srcHSize, srcVSize, 0, 0,
							srcHSize,srcVSize, 0, 1);
			dispPbBufSet(startAddr, decWidth, decHeight);
			HAL_DispScaleSet(decWidth, decHeight, 100,1);
			//HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);//phil: No waiting needed, use the internal sync!
			HAL_PbSyncPara2DispSof();			  // For update display controller registers
		}

	}else {
		if(startAddr - tempAddr >= (UINT32)srcHSize * (UINT32)srcVSize){
			HAL_GprmCopy(tempAddr, srcHSize, srcVSize, 0, 0,
							startAddr, srcHSize, srcVSize, 0, 0,
							srcHSize,srcVSize, 0, 1);
			dispPbBufSet(startAddr, decWidth, decHeight);
			HAL_DispScaleSet(decWidth, decHeight, 100,1);
			//HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING); //phil: No waiting needed, use the internal sync!
			HAL_PbSyncPara2DispSof();			  // For update display controller registers
		} else {
			HAL_GprmCopy(tempAddr, srcHSize, srcVSize, 0, 0,
				startAddr + (UINT32)srcHSize * (UINT32)srcVSize, srcHSize, srcVSize, 0, 0,
				srcHSize,srcVSize, 0, 1);
			dispPbBufSet(startAddr + (UINT32)srcHSize * (UINT32)srcVSize, decWidth, decHeight);
			HAL_DispScaleSet(decWidth, decHeight, 100,1);
			//HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);//phil: No waiting needed, use the internal sync!
			HAL_PbSyncPara2DispSof();			  // For update display controller registers
			HAL_GprmCopy(startAddr + (UINT32)srcHSize * (UINT32)srcVSize, srcHSize, srcVSize, 0, 0,
							startAddr, srcHSize, srcVSize, 0, 0,
							srcHSize,srcVSize, 0, 1);
			dispPbBufSet(startAddr, decWidth, decHeight);
			HAL_DispScaleSet(decWidth, decHeight, 100,1);
			//HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_FALLING);//phil: No waiting needed, use the internal sync!
			HAL_PbSyncPara2DispSof();			  // For update display controller registers
		}
	}

}
#endif
/**
 * @fn		  UINT8 pbParamSet(UINT8 paraId, UINT16 value)
 * @brief	  set play back parameters
 * @param	  [in] paraId
 * @param	  [in] value
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_LIB_PB_DMY
UINT8 pbParamSet(UINT8 paraId, UINT16 value)
{
        paraId=paraId;
        value=value;
        return SUCCESS;
}
#else
UINT8 pbParamSet(UINT8 paraId, UINT16 value)
{
	if (paraId >= PB_PARAM_MAX) {
		return FAIL;
	}

	//xian ++
	switch (paraId){
		case PB_PARAM_ROT:
#if (DBG_Level > 2)
			printf("Org Rot : %d\n", pbParam[paraId]);
#endif //(DBG_Level > 2)
			if(!value){
					pbParam[paraId] = value;
				}
			else{
			pbParam[paraId]++;
			if(pbParam[paraId] >= (UINT16)PB_ROT_SET_NUM){
				pbParam[paraId] = 0;
			}
			}
#if (DBG_Level > 2)
			printf("Set Rot : %d\n", pbParam[paraId]);
#endif //(DBG_Level > 2)
			break;

		default:
			pbParam[paraId] = value;
	}

	return SUCCESS;
}
#endif
/**
 * @fn		  UINT16 pbParamGet(UINT8 paraId)
 * @brief	  get playback parameters
 * @param	  [in] paraId
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
#if STILL_LIB_PB_DMY
UINT16 pbParamGet(UINT8 paraId)
{
	paraId=paraId;
	return FAIL;
}
#else
UINT16 pbParamGet(UINT8 paraId)
{
	if (paraId >= PB_PARAM_MAX) {
		return FAIL;
	}

	return pbParam[paraId];
}
#endif
