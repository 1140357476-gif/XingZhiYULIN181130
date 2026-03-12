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
 * @file      pv_photoframe.c
 * @brief     phot frame 
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/


/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "pv_photoframe.h"
#include "sp1k_pv_api.h"
#include "hal_gprm.h"
#include "video_clip.h"
#include "hal_dram.h"
#include "initio.h"
#include "utility.h"
#include "reg_def.h"
#include "dbg_def.h"
#include "ae_api.h"
#if (DBG_PV_PF_TIMING == 1)
#include "gpio_api.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define __FILE	__FILE_ID_PV_PHOTO_FRAME__
/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/
UINT8 cdspWrEofFlag = 0;
UINT8 blendingMode = PRV_BLEND_PHOTOFRM;
UINT32 dtnaddr[5];
UINT8 photoEnable = 0;
UINT8 PvDlyPhofrmBlendCnt;
UINT8 switchAeMode;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
//extern xdata videoStampCfg_t g_vidDateStamp;
extern xdata UINT16 dsParam[];
extern UINT16 g_vidParam[VD_PARAM_MAX];//wangrf++
extern UINT8 videoStampFlg;
extern UINT8 aeProc;


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/

/**
 * @fn        UINT8 pvPhoFrmCdspEofGet(void)
 * @brief     pvPhoFrmCdspEofGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
pvPhoFrmCdspEofGet(
	void
)
{
    return cdspWrEofFlag;
}

/**
 * @fn        void pvPhoFrmCdsoEofSet(UINT8 eof)
 * @brief     pvPhoFrmCdsoEofSet
 * @param     [in] eof
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvPhoFrmCdsoEofSet(
	UINT8 eof
)
{
    UINT32 sts;

    ENTER_CRITICAL(sts);
    cdspWrEofFlag = eof;
    EXIT_CRITICAL(sts);
}

/**
 * @fn        void pvPhoFrmCdspEofIntrSet(UINT8 eof)
 * @brief     pvPhoFrmCdspEofIntrSet
 * @param     [in] eof
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvPhoFrmCdspEofIntrSet(
	UINT8 eof
)
{
    cdspWrEofFlag = eof;
}

/**
 * @fn        void pvPhofrmBlend(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize)
 * @brief     pvPhofrmBlend
 * @param     [in] srcAddr
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvPhofrmBlend(
	UINT32 srcAddr, 
	UINT16 srcHSize, 
	UINT16 srcVSize
)
{
    UINT8 cdspIdx;

    cdspIdx = XBYTE[REG_Dram_MpgSb_CdspWRegIdx] & 0x03;

    switch(blendingMode) {
	    default:
	    case PRV_BLEND_PHOTOFRM:    //xian ++ 20081217
			if (HAL_GprmCopy(srcAddr, srcHSize, srcVSize, 0, 0, dtnaddr[cdspIdx], srcHSize, srcVSize, 0, 0, srcHSize, srcVSize, 0x9F, 1)) {
				ASSERT(0,1);
			}
			break;
	    case PRV_BLEND_DATESTAMP: //xian ++ 20081103
			#if 0
			srcAddr = videoRecStampAddrGet();
			srcHSize = g_vidDateStamp.stampW;
			srcVSize = g_vidDateStamp.stampH;

			if (HAL_GprmCopy(srcAddr, srcHSize, srcVSize, 0, 0,
			dtnaddr[cdspIdx], g_vidDateStamp.imgW, g_vidDateStamp.imgH,
			g_vidDateStamp.disHOff, g_vidDateStamp.disVOff, srcHSize, srcVSize, 
			dsParam[SP1K_DS_PARAM_STAMP_PASTETHD]/*0*//*0x9F*/, 1)) { //xian modify 20090522
				ASSERT(0,1);
			}  
			#endif
	        break;
   }
}

/**
 * @fn        void pvvideostampIntr()
 * @brief     videostampSetIntr
 * @param     [in] NULL
 * @retval    NONE
 * @see       NULL
 * @author    rf.wang
 * @since     2010-12-16
 * @todo      N/A
 * @bug       N/A
*/
void pvVideoStampSetIntr()
{
	UINT32 srcAddr;
	//static UINT8 disBufIdPrev = 0;

	/*
	if (g_vidDateStamp.disBufId != disBufIdPrev) {
		srcAddr = g_vidDateStamp.stampAddr;
		if (g_vidDateStamp.disBufId) {
			srcAddr += ((UINT32)720*48); // see video_clip.h K_SDRAM_AviStampBufSize;
		}
		*/
		srcAddr=INTR_videoRecStampAddrGet();

		XBYTE[REG_Dram_StampStAddr_B0] = *((UINT8*)&srcAddr+3); //stamp addr 
		XBYTE[REG_Dram_StampStAddr_B1] = *((UINT8*)&srcAddr+2);
		XBYTE[REG_Dram_StampStAddr_B2] = *((UINT8*)&srcAddr+1);
		XBYTE[REG_Dram_StampStAddr_B3] = *((UINT8*)&srcAddr+0);
	//}	

	//disBufIdPrev = g_vidDateStamp.disBufId;
}

/**
 * @fn        void pvPhofrmBlendIntr(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize)
 * @brief     pvPhofrmBlendIntr
 * @param     [in] srcAddr
 * @param     [in] srcHSize
 * @param     [in] srcVSize
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvPhofrmBlendIntr(
	UINT32 srcAddr, 
	UINT16 srcHSize, 
	UINT16 srcVSize
)
{
	UINT8 cdspIdx;
	UINT8 err = 0;
	UINT32 tempAddr;
	
	cdspIdx = XBYTE[REG_Dram_MpgSb_CdspWRegIdx] & 0x03;

	if (PvDlyPhofrmBlendCnt) {
		if (cdspIdx != 2) {// idx 2 is II AE work
			PvDlyPhofrmBlendCnt--;
		}
		return;
	}
	
	if(cdspIdx>0)
	{
		cdspIdx = cdspIdx-1;
	}
	else
	{
		cdspIdx = 3;
	}

	switch (blendingMode) 
	{    
		case PRV_BLEND_PHOTOFRM:
			#if (DBG_PV_PF_TIMING == 1)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x20);
			#endif
			if (HAL_GprmCopy_Intr(srcAddr, srcHSize, srcVSize, 0, 0, dtnaddr[cdspIdx], srcHSize, srcVSize, 0, 0, srcHSize, srcVSize, 0x80 + 0x14, 0)) {
				INTR_ASSERT(0,1);
			}
			#if (DBG_PV_PF_TIMING == 1)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);
			#endif
			break;
		case PRV_BLEND_DATESTAMP: //xian ++ 20090717 
			//videoStampSetIntr();
			break;
		case PRV_BLEND_ROTATION:
			tempAddr = dtnaddr[cdspIdx];
			dtnaddr[cdspIdx] = dtnaddr[4];
			dtnaddr[4] = tempAddr;
			switch (cdspIdx) 
			{
				case 0:
					HAL_Dram1stPathFrmBufDAddrSet_Intr(dtnaddr[cdspIdx]);
					break;
				case 1:
					HAL_Dram1stPathFrmBufAAddrSet_Intr(dtnaddr[cdspIdx]);
					break;
				case 2:
					HAL_Dram1stPathFrmBufBAddrSet_Intr(dtnaddr[cdspIdx]);
					break;
				case 3:
					HAL_Dram1stPathFrmBufCAddrSet_Intr(dtnaddr[cdspIdx]);
					break;
				default:
					break;
			}
			if (HAL_GprmRot_Intr(5, dtnaddr[4], dtnaddr[cdspIdx], K_Preview_Width, K_Preview_Width, 1)) {
				INTR_ASSERT(0,1);
			}
			break;
		default:
			break;
	}
}

/**
 * @fn        void pvPhofrmInit(void)
 * @brief     pvPhofrmInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvPhofrmInit(
	void
)
{
    UINT8 cdspIdx;
    UINT8 offset;

	#if 1//TAG_TODO
	    XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] = 0xC0;
	#endif    

    for (cdspIdx = 0; cdspIdx< 4; cdspIdx++) 
    {
        if (cdspIdx) 
        {
            offset =cdspIdx  * 4;
        }
        else
        {
		offset = 0;
        }
        dtnaddr[cdspIdx]  = ((UINT32)XBYTE[REG_Dram_DispSb0_StAddr_B0 + offset]) |
                      ((UINT32)XBYTE[REG_Dram_DispSb0_StAddr_B1 + offset]) << 8  |
                      ((UINT32)XBYTE[REG_Dram_DispSb0_StAddr_B2 + offset]) << 16 |
                      ((UINT32)XBYTE[REG_Dram_DispSb0_StAddr_B3 + offset]) << 24;
    }
    dtnaddr[4] = K_SDRAM_ImagePlaybackBufAddr;
}


/**
 * @fn        UINT8 pvPhofrmEnableGet(void)
 * @brief     pvPhofrmEnableGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
pvPhofrmEnableGet(
	void
) USING_1
{
    return photoEnable;
}


/**
 * @fn        void sp1kPvPhotofrmEnable(UINT8 enable)
 * @brief     sp1kPvPhotofrmEnable
 * @param     [in] enable
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kPvPhotofrmEnable(
	UINT8 enable
)
{
    UINT32 sts;
	UINT8 currAe;

	/* disable ae */
	currAe = aeModeEnableGet();
	if (currAe == AE_ENABLE) {
		aeModeEnableSet(AE_DISABLE);
	}

	/* used (RDK) GPIO12, GPIO13, (EVB) TP_YN & TP_XN GPIO */
	#if (DBG_PV_PF_TIMING == 1)
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	gpioByteInGateSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	#endif	
	
    ENTER_CRITICAL(sts);
    if (enable) {
        //if (init)
        pvPhofrmInit();
        blendingMode = PRV_BLEND_PHOTOFRM;
		#if MANU_CDSP_IDX
        	XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x02; //xian ++ 20090717
		#endif	    
        XBYTE[REG_Dram_Cdsp1Eof_Inten] |= 0x02;
        photoEnable = 1;

		if (aeProc == 1) {
			switchAeMode = 1;
			
			aeProc = 0;
			aeInitial();
		}
    }
    else {
		
		#if MANU_CDSP_IDX
        	XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x02; //xian ++ 20090717
		#endif
        XBYTE[REG_Dram_Cdsp1Eof_Inten] &= 0xFD;
        pvPhoFrmCdsoEofSet(0);
        photoEnable = 0;

		if (switchAeMode == 1) {
			switchAeMode = 0;

			aeProc = 1;
			aeInitial();
		}

    }
    EXIT_CRITICAL(sts);

	aeModeEnableSet(AE_ENABLE);
}

/**
 * @fn        void sp1kPvPhotofrmModeSet(UINT8 mode)
 * @brief     sp1kPvPhotofrmModeSet
 * @param     [in] mode
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kPvPhotofrmModeSet(
	UINT8 mode
)

{
    if (mode < PRV_BLEND_END) {
        blendingMode = mode;
	}
}

/**
 * @fn        void sp1kPvDlyPhofrmBlendCntSet(UINT8 cnt)
 * @brief     Pv Dly Phofrm Blend Cnt
 * @param     [in] dlyCnt
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kPvDlyPhofrmBlendCntSet(
	UINT8 dlyCnt
)

{
    PvDlyPhofrmBlendCnt = dlyCnt;
}





