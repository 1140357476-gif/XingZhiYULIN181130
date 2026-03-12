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
 * @file      hal_dram.c
 * @brief     Hal API for DRAM module
 * @author    Allen Lin
 * @since     2010-04-25
 * @date      2010-04-25
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define _DBG_WITH_ALL_FUNC_

#include "general.h"
#include "reg_def.h"
#include "hal.h"
#include "hal_dram.h"
#include "main.h"
#include "quantdef.h"
#include "asicreg.h"

#include "sp1k_snap_api.h"

#include "pv_task.h"
#include "snap_qtable.h"
#include "hal_jpeg.h"
#include "hal_cdsp.h"
#include "hal_gprm.h"
#include "initio.h"
#include "hal_global.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
sfr RamPage = 0x9b;

#undef JPG_ENC_BRC_TEST
#undef JPG_TIME_MEASURE_IN_GPIO

#ifdef JPG_ENC_BRC_TEST
#define BRC_TEST_SIZE	( 16 * 1024 )
#endif

#define	PATT_SEARCH_FAT16_FREE			0x01
#define	PATT_SEARCH_FAT16_FREE_TOTAL	0x08+PATT_SEARCH_FAT16_FREE
#define	PATT_SEARCH_FAT32_FREE			0x03
#define	PATT_SEARCH_FAT32_FREE_TOTAL	0x08+PATT_SEARCH_FAT32_FREE
#define	PATT_SEARCH_FDB					0x07

#define TYPE_DMEM 2 //DMEM (0x0000 ~ 0x1FFF)
#define TYPE_IMEM 1 //IMEM (0x3000 ~ 0x37FF)
#define TYPE_SWAP_MEM 0 //MEM for swap (0x3800 ~ 0x3BFF)
#define DMA_CHANNEL 1 ////using DRAM dma channel 1

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
static UINT8 stillPvZfTrig;
static UINT8 videoRecZfTrig;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 xdata G_DramAssert;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**
 * @fn 		void HAL_DramPattSearch(UINT8 mode, UINT32 startAddW, UINT32 length, UINT32 patt,
					   UINT32 *first, UINT32 *total, UINT8 *hit)
 * @brief	Pattern search
 * @param	mode 		:
 * @param	startAddW 	:
 * @param	length		:
 * @param	pattern		:
 * @param	firstidx	:
 * @param	totalnum	:
 * @param	cntnunum	:
 * @retval	NONE
 * @author	Ted
 * @todo
 * @since	2007-12-18
 */
/* Pre: HAL_PattSearch */
void HAL_DramPattSearch(UINT8 mode, UINT32 startAddW, UINT32 length, UINT32 patt,
					   UINT32 *first, UINT32 *total, UINT8 *hit)
{
	//if(patt) printf("PATT=<%c%c%c%c>\n", READ8(patt,0),READ8(patt,1),READ8(patt,2),READ8(patt,3));

    UINT8 LowByteData,HighByteData;
    UINT32 wordaddrtmp=startAddW;//=wordaddr;
    //printf("++++++ mode = %bx\n",XBYTE[REG_Dram_PatSchEn]);
#if 1                /* for workaround solution */
    if ((mode & 0x06) == 0x06){
        wordaddrtmp += (length << 4);
    }
    else if ((mode & 0x06) == 0x02){
        wordaddrtmp += (length << 1);
    }
    else {
        wordaddrtmp += length;
    }
    //XBYTE[REG_Dram_PatSchEn]=0x00;
    XBYTE[REG_Dram_IndirectAddr_B0] = READ8(wordaddrtmp, 3);
    XBYTE[REG_Dram_IndirectAddr_B1] = READ8(wordaddrtmp, 2);
    XBYTE[REG_Dram_IndirectAddr_B2] = READ8(wordaddrtmp, 1);
    XBYTE[REG_Dram_IndirectAddr_B3] = READ8(wordaddrtmp, 0);

    XBYTE[REG_Dram_PatSchEn]=0x00;
    XBYTE[REG_Dram_IndirectPrefetch]=0x01;                              //set prefetch

    while(XBYTE[REG_Dram_IndirectPrefetch]);
    LowByteData  = XBYTE[REG_Dram_IndirectRData_Low];
    HighByteData = XBYTE[REG_Dram_IndirectRData_High];

    XBYTE[REG_Dram_IndirectAddr_B0] = READ8(wordaddrtmp, 3);
    XBYTE[REG_Dram_IndirectAddr_B1] = READ8(wordaddrtmp, 2);
    XBYTE[REG_Dram_IndirectAddr_B2] = READ8(wordaddrtmp, 1);
    XBYTE[REG_Dram_IndirectAddr_B3] = READ8(wordaddrtmp, 0);

    while(XBYTE[REG_Dram_IndirectPrefetch]);
    XBYTE[REG_Dram_IndirectRData_Low] = patt + 1;
    XBYTE[REG_Dram_IndirectRData_High] = patt + 1;
    while(XBYTE[REG_Dram_IndirectPrefetch]);

#endif

	XBYTE[REG_Dram_PatSchEn] = mode;	//{4'h0, patschall, jm32byte, pat32bit, patschen}
	XBYTE[REG_Dram_IndirectRData_Low] = READ8(patt,0);
	XBYTE[REG_Dram_IndirectRData_High] = READ8(patt,1);
	XBYTE[REG_Dram_Pat32Msb_Low] = READ8(patt,2);
	XBYTE[REG_Dram_Pat32Msb_High] = READ8(patt,3);
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(startAddW,3);    //address_redirect
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(startAddW,2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(startAddW,1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(startAddW,0);
#if 0
    length--;
#else
    /* for workaround solution */
#endif
	XBYTE[REG_Dram_PatMemSize_B0] = READ8(length,3);
	XBYTE[REG_Dram_PatMemSize_B1] = READ8(length,2);
	XBYTE[REG_Dram_PatMemSize_B2] = READ8(length,1);
	XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//prefetch
	while(XBYTE[REG_Dram_IndirectPrefetch]==0x01);
	//printf("XBYTE[REG_Dram_IndirectPrefetch]=%02bX ", XBYTE[REG_Dram_IndirectPrefetch]);
	XBYTE[REG_Dram_PatSchEn]=0x00;
	*hit = XBYTE[REG_Dram_CntnuNum];
	if(*hit) {
		WRITEP8(first, 3, XBYTE[REG_Dram_FstIdx_B0]);
		WRITEP8(first, 2, XBYTE[REG_Dram_FstIdx_B1]);
		WRITEP8(first, 1, XBYTE[REG_Dram_FstIdx_B2]);
		WRITEP8(first, 0, 0);
		WRITEP8(total, 3, XBYTE[REG_Dram_TotalNum_B0]);
		WRITEP8(total, 2, XBYTE[REG_Dram_TotalNum_B1]);
		WRITEP8(total, 1, XBYTE[REG_Dram_TotalNum_B2]);
		WRITEP8(total, 0, 0);
	}
	XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//prefetch

#if 1                /* for workaround solution */
    while(XBYTE[REG_Dram_IndirectPrefetch]);

    XBYTE[REG_Dram_IndirectAddr_B0] = READ8(wordaddrtmp, 3);
    XBYTE[REG_Dram_IndirectAddr_B1] = READ8(wordaddrtmp, 2);
    XBYTE[REG_Dram_IndirectAddr_B2] = READ8(wordaddrtmp, 1);
    XBYTE[REG_Dram_IndirectAddr_B3] = READ8(wordaddrtmp, 0);

    while(XBYTE[REG_Dram_IndirectPrefetch]);
    XBYTE[REG_Dram_IndirectRData_Low] = LowByteData;
    XBYTE[REG_Dram_IndirectRData_High] = HighByteData;
    while(XBYTE[REG_Dram_IndirectPrefetch]);
#endif
}

/**
 * @fn    	UINT8 HAL_DramStartAddrSet(UINT32 DRAMAddrW, UINT8 Read)
 * @brief	Set the SDRAM read/write starting address
 * @param	DRAMAddr :SDRAM read/write starting address
 * @param	Read	 :Read action if set to 1
 * @retval	status : 0 is success,1 is fail.
 * @author	Allen Lin
 * @since	2007-12-18
 */
/* Pre: HAL_DRAMSetStartAddr */
UINT8 HAL_DramStartAddrSet(UINT32 DRAMAddrW, UINT8 Read) USING_0
{
	UINT8 status = SUCCESS;

	#if DRAM_ASSERT
	if (G_DramAssert == 0x01){
		if (DRAMAddr <= 0x40000){
			printf("@ASSERT %s %d\n",__FILE__, (UINT16)__LINE__);
			while(1);
		}
	}
	#endif

	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(&DRAMAddrW, 3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(&DRAMAddrW, 2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(&DRAMAddrW, 1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(&DRAMAddrW, 0);

	if(Read==0x01) {
		XBYTE[REG_Dram_IndirectPrefetch]=0x01;                              //set prefetch
	}

	return(status);
}

/**
 * @fn    	UINT8 HAL_DramWordDataRead(UINT8 *LowByteData, UINT8 *HighByteData)
 * @brief	Read a word from the SDRAM
 * @param	LowByteData		: Low byte data
 * @param	HighByteData	: High byte data
 * @retval	status : 0 is success,1 is fail.
 * @author	Allen Lin
 * @since	2007-12-18
*/
/* Pre: HAL_DRAMReadWord */
UINT8 HAL_DramWordDataRead(UINT8 *LowByteData, UINT8 *HighByteData) USING_0
{
	UINT8 status = SUCCESS;
/*Modify by Phil@07.12.27 s*/
#if 0
	UINT8 drambusy;

	//body
	do
	{
	 drambusy=XBYTE[REG_Dram_IndirectPrefetch]&0x01;
	}while(drambusy==0x01);
#else
	while(XBYTE[REG_Dram_IndirectPrefetch]);
#endif
/*Modify by Phil@07.12.27 s*/
	*LowByteData  = XBYTE[REG_Dram_IndirectRData_Low];
	*HighByteData = XBYTE[REG_Dram_IndirectRData_High];

	//return
	return(status);
}

/**
 * @fn    	UINT8 HAL_DramWordDataWrite(UINT8 LowByteData, UINT8 HighByteData)
 * @brief	Write a word to the SDRAM
 * @param	LowByteData		:The low byte data to be written to the SDRAM
 * @param	HighByteData	:The high byte data to be written to the SDRAM
 * @retval	status : 0 is success,1 is fail.
 * @author	Allen Lin
 * @since	2007-12-18
 */
/* Pre: HAL_DRAMWriteWord */
UINT8 HAL_DramWordDataWrite(UINT8 LowByteData, UINT8 HighByteData) USING_0
{
	UINT8 status = SUCCESS;
/*Modify by Phil@07.12.27 s*/
#if 0
	UINT8 drambusy;

	//body
	XBYTE[REG_Dram_IndirectRData_Low]=LowByteData;
	XBYTE[REG_Dram_IndirectRData_High]=HighByteData;
	do
	{
	 drambusy=(XBYTE[REG_Dram_IndirectPrefetch])&(0x01);
	}while(drambusy==0x01);
#else
	while (XBYTE[REG_Dram_IndirectPrefetch]);
	XBYTE[REG_Dram_IndirectRData_Low] = LowByteData;
	XBYTE[REG_Dram_IndirectRData_High] = HighByteData;
#endif
/*Modify by Phil@07.12.27 e*/
	//return
	return(status);
}

/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufAddrGet(UINT8 idx, UINT32 *addrW)
 * @brief     Set Frame Buffer A Address
 * @param     [in] AFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_Dram1stPathFrmBufAddrGet(UINT8 idx, UINT32 *addrW)
{
	code UINT16 cdsp1stBufRegTbl[] = {
		REG_Dram_MpgSrc0Addr_B0,
		REG_Dram_MpgSrc1Addr_B0,
		REG_Dram_MpgSrc2Addr_B0,
		REG_Dram_MpgSrc3Addr_B0
	};
	UINT16 bufReg = cdsp1stBufRegTbl[idx];
	UINT32 tmpaddrW;
	UINT8 *pt = (unsigned char *)&tmpaddrW;
	UINT8 status = SUCCESS;

	pt[3] = XBYTE[bufReg];
	pt[2] = XBYTE[bufReg+1];
	pt[1] = XBYTE[bufReg+2];
	pt[0] = XBYTE[bufReg+3];

	*addrW = tmpaddrW;

	return status;
}


/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufAAddrSet(UINT32 AFBAddrW)
 * @brief     Set Frame Buffer A Address
 * @param     [in] AFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_Dram1stPathFrmBufAAddrSet(UINT32 AFBAddrW) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_MpgSrc0Addr_B0] = READ8(&AFBAddrW, 3);
	XBYTE[REG_Dram_MpgSrc0Addr_B1] = READ8(&AFBAddrW, 2);
	XBYTE[REG_Dram_MpgSrc0Addr_B2] = READ8(&AFBAddrW, 1);
	XBYTE[REG_Dram_MpgSrc0Addr_B3] = READ8(&AFBAddrW, 0);

	return(status);
}


/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufBddrSet(UINT32 BFBAddrW)
 * @brief     Set Frame Buffer B Address
 * @param     [in] AFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_Dram1stPathFrmBufBAddrSet(UINT32 BFBAddrW) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_MpgSrc1Addr_B0] = READ8(&BFBAddrW, 3);
	XBYTE[REG_Dram_MpgSrc1Addr_B1] = READ8(&BFBAddrW, 2);
	XBYTE[REG_Dram_MpgSrc1Addr_B2] = READ8(&BFBAddrW, 1);
	XBYTE[REG_Dram_MpgSrc1Addr_B3] = READ8(&BFBAddrW, 0);

	return(status);
}


/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufCddrSet(UINT32 CFBAddrW)
 * @brief     Set Frame Buffer C Address
 * @param     [in] AFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_Dram1stPathFrmBufCAddrSet(UINT32 CFBAddrW) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_MpgSrc2Addr_B0] = READ8(&CFBAddrW, 3);
	XBYTE[REG_Dram_MpgSrc2Addr_B1] = READ8(&CFBAddrW, 2);
	XBYTE[REG_Dram_MpgSrc2Addr_B2] = READ8(&CFBAddrW, 1);
	XBYTE[REG_Dram_MpgSrc2Addr_B3] = READ8(&CFBAddrW, 0);

	return(status);
}


/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufDAddrSet(UINT32 DFBAddrW)
 * @brief     Set Frame Buffer D Address
 * @param     [in] DFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_Dram1stPathFrmBufDAddrSet(UINT32 DFBAddrW) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_MpgSrc3Addr_B0] = READ8(&DFBAddrW, 3);
	XBYTE[REG_Dram_MpgSrc3Addr_B1] = READ8(&DFBAddrW, 2);
	XBYTE[REG_Dram_MpgSrc3Addr_B2] = READ8(&DFBAddrW, 1);
	XBYTE[REG_Dram_MpgSrc3Addr_B3] = READ8(&DFBAddrW, 0);

	return(status);
}

/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufAAddrSet_Intr(UINT32 BFBAddrW)
 * @brief     Set Frame Buffer B Address
 * @param     [in] AFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Dram1stPathFrmBufAAddrSet_Intr(
	UINT32 BFBAddrW
)
{

	XBYTE[REG_Dram_MpgSrc0Addr_B0] = READ8(&BFBAddrW, 3);
	XBYTE[REG_Dram_MpgSrc0Addr_B1] = READ8(&BFBAddrW, 2);
	XBYTE[REG_Dram_MpgSrc0Addr_B2] = READ8(&BFBAddrW, 1);
	XBYTE[REG_Dram_MpgSrc0Addr_B3] = READ8(&BFBAddrW, 0);

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufBddrSet_Intr(UINT32 BFBAddrW)
 * @brief     Set Frame Buffer B Address
 * @param     [in] AFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Dram1stPathFrmBufBAddrSet_Intr(
	UINT32 BFBAddrW
)
{

	XBYTE[REG_Dram_MpgSrc1Addr_B0] = READ8(&BFBAddrW, 3);
	XBYTE[REG_Dram_MpgSrc1Addr_B1] = READ8(&BFBAddrW, 2);
	XBYTE[REG_Dram_MpgSrc1Addr_B2] = READ8(&BFBAddrW, 1);
	XBYTE[REG_Dram_MpgSrc1Addr_B3] = READ8(&BFBAddrW, 0);

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufCddrSet_Intr(UINT32 CFBAddrW)
 * @brief     Set Frame Buffer C Address
 * @param     [in] AFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Dram1stPathFrmBufCAddrSet_Intr(
	UINT32 CFBAddrW
)
{

	XBYTE[REG_Dram_MpgSrc2Addr_B0] = READ8(&CFBAddrW, 3);
	XBYTE[REG_Dram_MpgSrc2Addr_B1] = READ8(&CFBAddrW, 2);
	XBYTE[REG_Dram_MpgSrc2Addr_B2] = READ8(&CFBAddrW, 1);
	XBYTE[REG_Dram_MpgSrc2Addr_B3] = READ8(&CFBAddrW, 0);

	return SUCCESS;
}


/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufDAddrSet_Intr(UINT32 DFBAddrW)
 * @brief     Set Frame Buffer D Address
 * @param     [in] DFBAddrW
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_Dram1stPathFrmBufDAddrSet_Intr(
	UINT32 DFBAddrW
)
{

	XBYTE[REG_Dram_MpgSrc3Addr_B0] = READ8(&DFBAddrW, 3);
	XBYTE[REG_Dram_MpgSrc3Addr_B1] = READ8(&DFBAddrW, 2);
	XBYTE[REG_Dram_MpgSrc3Addr_B2] = READ8(&DFBAddrW, 1);
	XBYTE[REG_Dram_MpgSrc3Addr_B3] = READ8(&DFBAddrW, 0);

	return SUCCESS;
}

/**
 * @fn    	UINT8 HAL_Dram1stPathFrmBufSizeSet(UINT16 AFBHSize, UINT16 AFBVSize)
 * @brief	Set the frame buffer width and height
 * @param	AFBHSize : The frame buffer A width
 * @param	AFBVSize : The frame buffer A height
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_SetAFBSize */
UINT8 HAL_Dram1stPathFrmBufSizeSet(UINT16 AFBHSize, UINT16 AFBVSize) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_MpgSrcHSize_Low] = READ8(&AFBHSize, 1);
	XBYTE[REG_Dram_MpgSrcHSize_High] = READ8(&AFBHSize, 0);

	XBYTE[REG_Dram_MpgSrcVSize_Low] = READ8(&AFBVSize, 1);
	XBYTE[REG_Dram_MpgSrcVSize_High] = READ8(&AFBVSize, 0);

	return(status);
}


/**
 * @fn        UINT8 HAL_Dram1stPathFrmBufSizeGet(UINT16 *hSize, UINT16 *vSize)
 * @brief     Get width/height of frame buffer
 * @param     [out] hSize : frame buffer horizontal size
 * @param     [out] vSize : frame buffer vertical size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Allen Lin
 * @since     2010-6-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_Dram1stPathFrmBufSizeGet(UINT16 *hSize, UINT16 *vSize)
{
	WRITE8(*hSize,1,XBYTE[REG_Dram_MpgSrcHSize_Low]);
	WRITE8(*hSize,0,XBYTE[REG_Dram_MpgSrcHSize_High]);

	WRITE8(*vSize,1,XBYTE[REG_Dram_MpgSrcVSize_Low]);
	WRITE8(*vSize,0,XBYTE[REG_Dram_MpgSrcVSize_High]);

	//printf("hsize=%d\n",*hSize);
	//printf("vsize=%d\n",*vSize);

	return SUCCESS;
}

/**
 * @fn    	UINT8 HAL_Dram2ndPathFrmBufAAddrSet(UINT32 AFBAddrW)
 * @brief	Set 2nd path Frame buffer A address
 * @param	AFBAddrW : Frame buffer A address (word)
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_Set2AFBAddr*/
UINT8 HAL_Dram2ndPathFrmBufAAddrSet(UINT32 AFBAddrW) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_DispSb0_StAddr_B0] = READ8(&AFBAddrW, 3);
	XBYTE[REG_Dram_DispSb0_StAddr_B1] = READ8(&AFBAddrW, 2);
	XBYTE[REG_Dram_DispSb0_StAddr_B2] = READ8(&AFBAddrW, 1);
	XBYTE[REG_Dram_DispSb0_StAddr_B3] = READ8(&AFBAddrW, 0);

	return(status);
}

/**
 * @fn    	UINT8 HAL_Dram2ndPathFrmBufBAddrSet(UINT32 AFBAddrW)
 * @brief	Set 2nd path Frame buffer B address
 * @param	AFBAddrW : Frame buffer B address (word)
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_Set2BFBAddr*/
UINT8 HAL_Dram2ndPathFrmBufBAddrSet(UINT32 BFBAddrW) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_DispSb1_StAddr_B0] = READ8(&BFBAddrW, 3);
	XBYTE[REG_Dram_DispSb1_StAddr_B1] = READ8(&BFBAddrW, 2);
	XBYTE[REG_Dram_DispSb1_StAddr_B2] = READ8(&BFBAddrW, 1);
	XBYTE[REG_Dram_DispSb1_StAddr_B3] = READ8(&BFBAddrW, 0);

	return(status);
}
/**
 * @fn    	UINT8 HAL_Dram2ndPathFrmBufCAddrSet(UINT32 AFBAddrW)
 * @brief	Set 2nd path Frame buffer C address
 * @param	AFBAddrW : Frame buffer C address (word)
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_Set2CFBAddr*/
UINT8 HAL_Dram2ndPathFrmBufCAddrSet(UINT32 CFBAddrW) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_DispSb2_StAddr_B0] = READ8(&CFBAddrW, 3);
	XBYTE[REG_Dram_DispSb2_StAddr_B1] = READ8(&CFBAddrW, 2);
	XBYTE[REG_Dram_DispSb2_StAddr_B2] = READ8(&CFBAddrW, 1);
	XBYTE[REG_Dram_DispSb2_StAddr_B3] = READ8(&CFBAddrW, 0);

	return(status);
}

/**
 * @fn    	UINT8 HAL_Dram2ndPathFrmBufDAddrSet(UINT32 AFBAddrW)
 * @brief	Set 2nd path Frame buffer D address
 * @param	AFBAddrW : Frame buffer D address (word)
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_Set2DFBAddr*/
UINT8 HAL_Dram2ndPathFrmBufDAddrSet(UINT32 DFBAddrW) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_DispSb3_StAddr_B0] = READ8(&DFBAddrW, 3);
	XBYTE[REG_Dram_DispSb3_StAddr_B1] = READ8(&DFBAddrW, 2);
	XBYTE[REG_Dram_DispSb3_StAddr_B2] = READ8(&DFBAddrW, 1);
	XBYTE[REG_Dram_DispSb3_StAddr_B3] = READ8(&DFBAddrW, 0);

	return(status);
}




/**
 * @fn    	UINT8 HAL_Dram2ndPathFrmBufSizeSet(UINT16 AFBHSize)
 * @brief	Set the 2nd frame buffer A width and height
 * @param	AFBHSize : The frame buffer A width
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre : HAL_Set2AFBSize */
UINT8 HAL_Dram2ndPathFrmBufSizeSet(UINT16 AFBHSize) USING_0
{

	XBYTE[REG_Dram_DispSb_HSize_Low] = READ8(&AFBHSize, 1);
	XBYTE[REG_Dram_DispSb_HSize_High] = READ8(&AFBHSize, 0);

	return SUCCESS;
}

/**
 * @fn    	void HAL_DramPreviewStart(UINT8 pbFreeze)
 * @brief	Start preview setting.
 * @param	pbFreeze: 1: playback freeze; 0: normal.
 * @retval	None
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_DRAMPreviewStart */
void HAL_DramPreviewStart(UINT8 pbFreeze) USING_0
{
	// reset index
	XBYTE[REG_Dram_MpgSb_RegIdx_En]=0x00;

	//XBYTE[REG_Dram_MpgSb_CdspWRegIdx]=0x00;
	if ((XBYTE[REG_Dram_MpgSb_MjpgRRegIdx]&0x10) == 0x10){
		XBYTE[REG_Dram_MpgSb_CdspWRegIdx]=0x02;
	}
	else if ((XBYTE[REG_Dram_MpgSb_MjpgRRegIdx]&0x20) == 0x20){
		XBYTE[REG_Dram_MpgSb_CdspWRegIdx]=0x02;

	}else {
		XBYTE[REG_Dram_MpgSb_CdspWRegIdx]=0x01;
	}

	#if TAG_TODO
		#if (MANU_CDSP_IDX == 0)
			XBYTE[REG_Dram_MpgSb_MjpgRRegIdx]=XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] & ~0x03;
		#else
			XBYTE[REG_Dram_MpgSb_MjpgRRegIdx]=0x12; //xian ++ 20090717, for date stamp
		#endif
	#endif

	XBYTE[REG_Dram_MpgSb_RegIdx_En]=0x0e; // apply index

	XBYTE[REG_Dram_MpgSb_RegIdx_En]=0x00;//{dispimgsrc_sel_en,osd_reg_en,pbb_reg_en,vlc_reg_en,mpgsb_mjpgrregidx_en,mpgsb_dispimgrregidx_en,mpgsb_cdspwregidx_en,mpgsb_usbrregidx_en}

 	if (!pbFreeze) {
		//XBYTE[0x27f3]=0x02;//dispimgsrc_sel
		HAL_DramDispSrcSel(2);
 	}
	else {
		//XBYTE[0x27f3]=0x00;//dispimgsrc_sel
		HAL_DramDispSrcSel(0);
	}

	/*
		lensgainupd, MJPG, USB disable
		DIST, display reserved
	*/
	XBYTE[REG_Dram_LensGainUpdt] &= ~0x19; //phil modified for no display solution

	XBYTE[REG_Dram_ZfUpdEn]=0x03;//zfupden// hlc 0326 remove

	HAL_CdspFrmCtrlEnSet(1); /* frcen */
	// HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV); //{setosdupdate,setpbzfupdate,setpvzfupdate}
	// HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB); //{setosdupdate,setpbzfupdate,setpvzfupdate}
}

/**
 * @fn    	void HAL_DramDispSrcSel(UINT8 srcSel)
 * @brief	Select Display source
 * @param	srcSel: 0: Playback buffer; 2: MpgSrcBuf
 * @retval	None
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: halDramDispSrcSel */
void HAL_DramDispSrcSel(UINT8 srcSel)
{
	XBYTE[REG_Dram_DispSrc_Sel]= srcSel;//dispimgsrc_sel
}

/**
 * @fn    	void HAL_DramDispSrcGet(UINT8 srcSel)
 * @brief	get display source frame.
 * @param	None.
 * @retval	One of HAL_DramDispSrc_e.
 * @see
 * @author	Matt Wang
 * @since	2008-09-10
 * @todo	N/A
 * @bug		N/A
*/
UINT8
HAL_DramDispSrcGet(
	void
)
{
	return XBYTE[REG_Dram_DispSrc_Sel];
}

/**
 * @fn    	UINT8 HAL_DramAVLCAddrSet(UINT32 Addr)
 * @brief	Set the VLC buffer A starting address
 * @param	Addr: Starting address of A VLC buffer
 * @retval	0: Success; 1: Fail
 * @author	Wenguo
 * @todo
 * @since	2007-12-18
 */
/* Pre: HAL_SetAVLCAddr */
UINT8 HAL_DramAVLCAddrSet(UINT32 Addr)
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_VlcAAddr_B0] = READ8(&Addr, 3);
	XBYTE[REG_Dram_VlcAAddr_B1] = READ8(&Addr, 2);
	XBYTE[REG_Dram_VlcAAddr_B2] = READ8(&Addr, 1);
	XBYTE[REG_Dram_VlcAAddr_B3] = READ8(&Addr, 0);

	return(status);
}

/**
 * @fn    	UINT8 HAL_DramBVLCAddrSet(UINT32 Addr)
 * @brief	Set the VLC buffer B starting address
 * @param	Addr: Starting address of B VlC bufferreturn value
 * @retval	0: Success; 1: Fail
 * @author	Wenguo
 * @todo
 * @since	2007-12-18
 */
/* Pre: HAL_SetBVLCAddr */
UINT8 HAL_DramBVLCAddrSet(UINT32 Addr)
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_VlcBAddr_B0] = READ8(&Addr, 3);
	XBYTE[REG_Dram_VlcBAddr_B1] = READ8(&Addr, 2);
	XBYTE[REG_Dram_VlcBAddr_B2] = READ8(&Addr, 1);
	XBYTE[REG_Dram_VlcBAddr_B3] = READ8(&Addr, 0);

	return(status);
}

/**
 * @fn		UINT8 HAL_DramDoYuvSrcHSizeSet( UINT16 size )
 * @brief	Set YUV do YUV src H size
 * @param	size: Do YUV srouce H size set.
 * @retval	0: Success; 1: Fail
 * @author	Wenguo
 * @todo
 * @since	2008-3-24
 */
/* Pre: HAL_SetDoYuvSrcHSize */
UINT8 HAL_DramDoYuvSrcHSizeSet( UINT16 size ) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_DoYuvSrc_HSize_Low] = READ8(&size, 1);
	XBYTE[REG_Dram_DoYuvSrc_HSize_High] = READ8(&size, 0);

	return(status);
}


/**
 * @fn		UINT8 HAL_DramRawFrmBufAddrSet(UINT32 Addr)
 * @brief	Set the RAW frame buffer starting address
 * @param	Addr: Starting address of the raw frame buffer
 * @retval	0: Success; 1: Fail
 * @author	Cynthia
 * @todo
 * @since	2007-12-18
 */
/* Pre: HAL_SetRFBAddr */
UINT8 HAL_DramRawFrmBufAddrSet(UINT32 Addr) USING_0
{
	UINT8 status = SUCCESS;

	XBYTE[REG_Dram_SnapWstAddr_B0] = READ8(&Addr, 3);
	XBYTE[REG_Dram_SnapWstAddr_B1] = READ8(&Addr, 2);
	XBYTE[REG_Dram_SnapWstAddr_B2] = READ8(&Addr, 1);
	XBYTE[REG_Dram_SnapWstAddr_B3] = READ8(&Addr, 0);

	return(status);
}


/**
 * @fn		UINT8 HAL_DramVLCRdyClr(UINT8 Target)
 * @brief	Clear the vlcardy or vlcbrdy flag
 * @param	Target 0 :clear the vlcardy
				   1 : clear the vlcbrdy
 * @retval	0: Success; 1: Fail
 * @author	Wenguo
 * @todo
 * @since	2007-12-18
 */
/* Pre: HAL_ClrVLCRdy */
UINT8 HAL_DramVLCRdyClr(UINT8 Target)
{
	UINT8 status = SUCCESS;

	//body
	if(Target==0)
	  XBYTE[REG_Dram_VlcRdy]=0x01;   //clear vlcardy
	else
	  XBYTE[REG_Dram_VlcRdy]=0x02;   //clear vlcbrdy

	//return
	return(status);
}


/**
 * @fn		UINT8 HAL_DramRawFrmBufSizeSet(UINT16 FBHSize, UINT16 FBVSize)
 * @brief	Set the raw frame buffer width and height
 * @param	FBHSize : The raw frame buffer width
			FBVSize : The raw frame buffer height
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_SetRFBSize */
UINT8 HAL_DramRawFrmBufSizeSet(UINT16 FBHSize, UINT16 FBVSize) USING_0
{

	XBYTE[REG_Dram_SnapWHSize_Low] = READ8(&FBHSize, 1);
	XBYTE[REG_Dram_SnapWHSize_High] = READ8(&FBHSize, 0);

	XBYTE[REG_Dram_SnapWVSize_Low] = READ8(&FBVSize, 1);
	XBYTE[REG_Dram_SnapWVSize_High] = READ8(&FBVSize, 0);

	return HALK_SUCCESS;
}

/**
 * @fn		UINT8 HAL_DramDRAMDarkAddrSet(UINT32 Addr)
 * @brief	Set drak frame R buffer starting address
 * @param	Addr : drak frame R buffer starting address
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_SetDRAMDarkAddr */
UINT8 HAL_DramDRAMDarkAddrSet(
	UINT32 Addr
)
{

	XBYTE[REG_Dram_DarkRAddr_B0] = READ8(&Addr, 3);
	XBYTE[REG_Dram_DarkRAddr_B1] = READ8(&Addr, 2);
	XBYTE[REG_Dram_DarkRAddr_B2] = READ8(&Addr, 1);
	XBYTE[REG_Dram_DarkRAddr_B3] = READ8(&Addr, 0);

    return SUCCESS;
}

/**
 * @fn		UINT8 HAL_DramDRAMDarkSizeSet(UINT16 hsize)
 * @brief	Set drak frame R buffer image width
 * @param	Addr : drak frame R buffer image width
 * @retval	0: Success; 1: Fail
 * @author	Allen Lin
 * @todo
 * @since	2010-04-25
 */
/* Pre: HAL_SetDRAMDarkSize */
UINT8 HAL_DramDRAMDarkSizeSet(
	UINT16 hsize
)
{
	XBYTE[REG_Dram_DarkRHSize_Low] = (hsize) & 0xFF;			//Drak Width
	XBYTE[REG_Dram_DarkRHSize_High] = (hsize>>8) & 0xFF;
	return SUCCESS;
}

/**
 * @fn        UINT32 HAL_IntrDramZUpdateWait(UINT8 selector, UINT32 timeout)
 * @brief     Wait zfupdate done
 * @param     [in] selector
 *\n				HAL_DRAM_ZUPDATE_CDSP - preview/zoom
 *\n				HAL_DRAM_ZUPDATE_PV - preview/zoom
 *\n                HAL_DRAM_ZUPDATE_PV - display
 *\n                HAL_DRAM_ZUPDATE_MPEG - mpeg
 *                  HAL_DRAM_ZUPDATE_ALL - preview/display/mpeg
 * @param     [in] timeout
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    huiling
 * @since     2010-6-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_IntrDramZUpdateWait(UINT8 selector)
{
	UINT32 msCnt=0;

	//printf("zf wait\n");
	switch (selector) {
		case HAL_DRAM_ZUPDATE_PV:
			/* if dispEn or dispPhaseClkEn disabled, no need to wait. */
			if ( !(XBYTE[REG_Disp_DispEn_Flip] & 0x01) || !(XBYTE[0x2013] & 0x10) ) {
				//stillPvZfTrig=0;
				//videoRecZfTrig=0;
				return SUCCESS;
			}
			/* wait disppvzfupdatedone. */
			if (stillPvZfTrig==1){
				#if 1
				if ( !(XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0x20 ) ){
					return FAIL;
				}
				#else
				while ( !(XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0x20 ) ){
					#if 0
					//HAL_GlobalTimerWait(1);
					msCnt++;
					//{printf("zf 0x%bx\n",XBYTE[REG_Dram_Cdsp1ZfUpdateDone]);}
					if (msCnt>=120){
						//printf("zf t o\n");
						/*
						printf("225e=%bx\n",XBYTE[0x225e]);
						printf("225f=%bx\n",XBYTE[0x225f]);
						printf("227c=%bx\n",XBYTE[0x227c]);
						printf("227d=%bx\n",XBYTE[0x227d]);
						*/
						/*
						printf("22b8=%bx\n",XBYTE[0x22b8]);
						printf("22b9=%bx\n",XBYTE[0x22b9]);
						printf("22ba=%bx\n",XBYTE[0x22ba]);
						printf("22bb=%bx\n",XBYTE[0x22bb]);
						printf("22bc=%bx\n",XBYTE[0x22bc]);
						printf("22bd=%bx\n",XBYTE[0x22bd]);
						printf("22be=%bx\n",XBYTE[0x22be]);
						printf("22bf=%bx\n",XBYTE[0x22bf]);
						*/
						//break;
						return FAIL;
					}
					#endif
				}
				#endif
			}

			/* clear disppvzfupdatedone. */
			XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = 0x20;
			//stillPvZfTrig=0;
			//videoRecZfTrig=0;
			break;
		case HAL_DRAM_ZUPDATE_VIDEO:
			if (videoRecZfTrig==1){
				if ((XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0xa0 )!= 0xa0){
					return FAIL;
				}
			}

			XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = 0xa0;
			videoRecZfTrig=0;
			stillPvZfTrig=0;
			break;
		default:
			break;
	}

	return SUCCESS;
}


/**
 * @fn        UINT32 HAL_DramZUpdateWait(UINT8 selector, UINT32 timeout)
 * @brief     Wait zfupdate done
 * @param     [in] selector
 *\n				HAL_DRAM_ZUPDATE_CDSP - preview/zoom
 *\n				HAL_DRAM_ZUPDATE_PV - preview/zoom
 *\n                HAL_DRAM_ZUPDATE_PV - display
 *\n                HAL_DRAM_ZUPDATE_MPEG - mpeg
 *                  HAL_DRAM_ZUPDATE_ALL - preview/display/mpeg
 * @param     [in] timeout
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    huiling
 * @since     2010-6-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_DramZUpdateWait(UINT8 selector, UINT16 timeout)
{
	//timeout = 0;  /* TODO: not use now */
	UINT32 msCnt=0;

	//printf("zf wait\n");
	switch (selector) {
		case HAL_DRAM_ZUPDATE_CDSP:
			while ( !(XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0x10 ) );

			/* clear cdsp pv zfupdate done. */
			//XBYTE[REG_Dram_Cdsp1ZfUpdateDone] |= 0x10;
			XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = 0x10;

			break;
		case HAL_DRAM_ZUPDATE_PV:
			/* if dispEn or dispPhaseClkEn disabled, no need to wait. */
			if ( !(XBYTE[REG_Disp_DispEn_Flip] & 0x01) || !(XBYTE[0x2013] & 0x10) ) {
				stillPvZfTrig=0;
				videoRecZfTrig=0;
				return SUCCESS;
			}
			/* wait disppvzfupdatedone. */
			if (stillPvZfTrig==1){
				//printf("pv tg w\n");

				if ((XBYTE[REG_Front_F2CdspGate]&0x01) == 0x01) {
					//printf("f2c out\n");
					return FAIL;
				}

				while ( !(XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0x20 ) ){
					HAL_GlobalTimerWait(1);
					msCnt++;
					//{printf("zf 0x%bx\n",XBYTE[REG_Dram_Cdsp1ZfUpdateDone]);}
					if (msCnt>=timeout){
						printf("zf t o\n");
						/*
						printf("225e=%bx\n",XBYTE[0x225e]);
						printf("225f=%bx\n",XBYTE[0x225f]);
						printf("227c=%bx\n",XBYTE[0x227c]);
						printf("227d=%bx\n",XBYTE[0x227d]);
						*/
						/*
						printf("22b8=%bx\n",XBYTE[0x22b8]);
						printf("22b9=%bx\n",XBYTE[0x22b9]);
						printf("22ba=%bx\n",XBYTE[0x22ba]);
						printf("22bb=%bx\n",XBYTE[0x22bb]);
						printf("22bc=%bx\n",XBYTE[0x22bc]);
						printf("22bd=%bx\n",XBYTE[0x22bd]);
						printf("22be=%bx\n",XBYTE[0x22be]);
						printf("22bf=%bx\n",XBYTE[0x22bf]);
						*/
						//break;
						return FAIL;
					}
				}
				//printf("pv tg w d\n");
			}

			/* clear disppvzfupdatedone. */
			XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = 0x20;
			stillPvZfTrig=0;
			videoRecZfTrig=0;
			break;

		case HAL_DRAM_ZUPDATE_PB:
			/* if dispEn or dispPhaseClkEn disabled, no need to wait. */
			if ( !(XBYTE[REG_Disp_DispEn_Flip] & 0x01) || !(XBYTE[0x2013] & 0x10) ) {
				return SUCCESS;
			}

			/* wait disppbzfupdatedone. */
			while ( !(XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0x40 ) );

			/* clear disppbzfupdatedone. */
			//XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = 0x40; // Phil: keep the status for pb ready always

			break;

		case HAL_DRAM_ZUPDATE_MPEG:

			while ((XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0x80 )!= 0x80) ;//{printf("27E5=%bx\n",XBYTE[REG_Dram_Cdsp1ZfUpdateDone]);}


			XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = 0x80;

			break;
		case HAL_DRAM_ZUPDATE_VIDEO:
			if (videoRecZfTrig==1){
				if ((XBYTE[REG_Front_F2CdspGate]&0x01) == 0x01) {
					//printf("f2c out\n");
					return FAIL;
				}

				while ((XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0xa0 )!= 0xa0){
					HAL_GlobalTimerWait(1);
					msCnt++;
					//{printf("zf 0x%bx\n",XBYTE[REG_Dram_Cdsp1ZfUpdateDone]);}
					if (msCnt>=timeout){
						printf("v zf t o\n");
						//break;
						return FAIL;
					}
				}
			}

			XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = 0xa0;
			videoRecZfTrig=0;
			stillPvZfTrig=0;
			break;
		case HAL_DRAM_ZUPDATE_ALL:
			while ((XBYTE[REG_Dram_Cdsp1ZfUpdateDone] & 0xF0 )!= 0xF0) ;
			XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = (0xF0 & ~0x40);  // Phil: keep the status for pb ready always

			break;
		default:
			break;
	}

	return SUCCESS;
}

void
HAL_DramZUpdateClear(
	UINT8 val
)
{
	XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = val;

	if ((val&0x30)==0x30){
		stillPvZfTrig=0;
		videoRecZfTrig=0;
	}
	//else if ((val&0xb0)==0xb0){
	//	videoRecZfTrig=0;
	//}

}

void
HAL_IntrDramZUpdateClear(
	UINT8 val
)
{
	XBYTE[REG_Dram_Cdsp1ZfUpdateDone] = val;

	if ((val&0x30)==0x30){
		stillPvZfTrig=0;
		videoRecZfTrig=0;
	}
	//else if ((val&0xb0)==0xb0){
	//	videoRecZfTrig=0;
	//}

}


/**
 * @fn		UINT8 HAL_DramSkipWrite2Dram(void)
 * @brief	Skip write to DRAM
 * @param	NULL
 * @retval	SUCCESS
 * @see
 * @author	Allen Lin
 * @since	2008-04-28
 * @todo
 * @bug
*/
/* Pre: HAL_SkipWrite2Dram */
UINT8 HAL_DramSkipWrite2Dram(void)
{
	XBYTE[REG_Cdsp_pSkipwr] = 0x0f;
	return SUCCESS;
}

#if 0
void sp1kHalMjpgClipAbort(
	void
)
{
	HAL_JpegMjpgAbort();
}
#endif
/**
 * @brief	wait for signal event.
 * @param	layer = [in] one of HAL_DramZfUpdSrc_e.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-06-05
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_DramZfUpdTrig(
	UINT8 layer
)
{
	if (layer == HAL_DRAM_ZF_UPD_LAYER_PV){
		stillPvZfTrig=1;
		videoRecZfTrig=1;
		//printf("zf tg\n");
	} else if (layer & HAL_DRAM_ZF_UPD_LAYER_PB) {
		HAL_DramZUpdateClear(0x40); // clear pb zfupdate done
	}

	//printf("zf trig:%bx\n", layer);
	XBYTE[REG_Dram_ZfUpdate] = layer;
}

/**
 * @brief	wait for signal event.
 * @param	layer = [in] one of HAL_DramZfUpdSrc_e.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-06-05
 * @todo	N/A
 * @bug		N/A
*/
void
HAL_IntrDramZfUpdTrig(
	UINT8 layer
)
{
	if (layer == HAL_DRAM_ZF_UPD_LAYER_PV){
		stillPvZfTrig=1;
		videoRecZfTrig=1;
	} else if (layer & HAL_DRAM_ZF_UPD_LAYER_PB) {
		HAL_IntrDramZUpdateClear(0x40); // clear pb zfupdate done
	}

	XBYTE[REG_Dram_ZfUpdate] = layer;
}


//wangjg++ 20100223
/**
 * @fn        void HAL_DramRWBytes(UINT32 addrW, addrW,UINT8* pt, pt,UINT32 cnt, t,UINT8 isRead, isRead,dram_rw_format_t fmt)
 * @brief     transfer data between xbyte and sdram,convert date format by 'fmt'
 * @param     [in] addrW	:dram start addr(word)
 * @param     [in] pt		:xbyte pointer
 * @param     [in] cnt		:transfer length
 * @param     [in] isRead	:transfer direction(1:read form dram,0:write to dram)
 * @param     [in] fmt		:convert format: dram_rw_format_t
 						:DRAM_RWFormat_8,  	BYTE sequential
 						:DRAM_RWFormat_16,	WORD sequential
						:DRAM_RWFormat_32,	DWORD sequential
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void HAL_DramRWBytes(UINT32 addrW,UINT8* pt,UINT32 cnt,UINT8 isRead,UINT8 fmt)
{
	//UINT16 i;
	UINT8* pt0 = pt + cnt;

	if(addrW){
		HAL_DramStartAddrSet(addrW,isRead);
	}
	if(!pt){
		return;
	}
	//printf("%bx,cnt:%lx,%lx,%lx,%bx\n",fmt,cnt,(UINT32)pt0,(UINT32)pt,M_REG_DRAM_Control);
	if(isRead)//isread
	{
		//printf("read\n");
		switch(fmt)
		{
		case DRAM_RWFormat_8:
			while(pt<pt0)
			{
				HAL_DramReadByte(pt++,pt++);
			}
			break;
		case DRAM_RWFormat_16:
			while(pt<pt0)
			{
				HAL_DramReadByte((pt + 1 ),(pt));
				pt+=2;
			}
			break;
		case DRAM_RWFormat_32:
			while(pt<pt0)
			{
				HAL_DramReadByte((pt + 3 ),(pt+2));
				HAL_DramReadByte((pt + 1 ),(pt));
				pt+=4;
			}
			break;
		}
	}
	else
	{
		//printf("write\n");
		switch(fmt)
		{
		case DRAM_RWFormat_8:
			while(pt<pt0)
			{
				HAL_DramWriteByte(*pt++,*pt++);
			}
			break;
		case DRAM_RWFormat_16:
			while(pt<pt0)
			{
				HAL_DramWriteByte(*(pt + 1 ),*(pt));
				pt+=2;
			}
			break;
		case DRAM_RWFormat_32:
			while(pt<pt0)
			{
				HAL_DramWriteByte(*(pt + 3),*(pt+2));
				HAL_DramWriteByte(*(pt + 1 ),*(pt));
				pt+=4;
			}
			break;
		}
	}

}

/**
 * @fn        void HAL_DramSnapRip(UINT8 type)
 * @brief     HAL_DramSnapRip
 * @param     [in] type:  0	-> disable
 *\n					  1 -> 2^n
 *\n					  2 -> 1/2
 *\n					  3 -> 1/2
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-8-3
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DramSnapRip(
    UINT8 type
)
{
	XBYTE[REG_Dram_SnapWVSize_High] &= 0x0f;
    XBYTE[REG_Dram_SnapWVSize_High] |= ((type&0x03)<<4);
}

/**
 * @fn        void HAL_DramDispBufSel(UINT8 sel)
 * @brief     HAL_DramDispBufSel :
 *									0: pv buffer is 1st path
 *									2: pv buffer is 2nd path
 * @param     [in] sel
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-6-17
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DramDispBufSel(
	UINT8 sel
)
{
	XBYTE[REG_Dram_DispImgBufSel] = (sel&0x03);
}


/**
 * @fn        UINT8 HAL_DramDmaRead(void xdata* pDst, UINT32 dramAddrWord, UINT16 sizeByte, UINT8 wait)
 * @brief     HAL_DramDmaRead
 * @param     [in] pDst
 * @param     [in] dramAddrWord
 * @param     [in] sizeByte
 * @param     [in] wait
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-19
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_DramDmaRead(void xdata* pDst, UINT32 dramAddrWord, UINT16 sizeByte, UINT8 wait)
{
	UINT8 data bSram = 1; //indicate if the pDst is in SRAM
	UINT8 data memType = TYPE_DMEM; // DMEM / IMEM
	UINT16 idata sramAddr = (UINT16)pDst;

	if (sramAddr >= 0x4000) {// 0x4000 <= pDst <= 0xFFFF
		sramAddr -= 0x4000;
		bSram = 0;
	} else if (sramAddr >= 0x3800) {// 0x3800 <= pDst <= 0x3BFF
		sramAddr &= 0x3FF; // sramAddr -= 0x3800
		memType = TYPE_SWAP_MEM;
	} else if (sramAddr >= 0x3000) {// 0x3000 <= pDst <= 0x37FF
		sramAddr &= 0x7FF; // sramAddr -= 0x3000
		memType = TYPE_IMEM;
	} //else //0x0000 <= pDst <= 0x1FFF

	if (bSram) {
		/* Dma from dram to SRAM */
		HAL_DmaSetDramAddr(dramAddrWord);
		HAL_DmaSetSramAddr(sramAddr);
		return HAL_DmaDo(0, 1, sizeByte, !wait, memType);
	} else {
		/* dma from dram to dram */
		return HAL_GprmDramDmaExt(
			dramAddrWord << 1,
			(K_SDRAM_CodeSize << 1) + sramAddr,
			sizeByte,
			wait,
			DMA_CHANNEL
		);
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_DramDmaWrite(void xdata* pSrc, UINT32 dramAddrWord, UINT16 sizeByte, UINT8 wait)
 * @brief     HAL_DramDmaWrite
 * @param     [in] pSrc
 * @param     [in] dramAddrWord
 * @param     [in] sizeByte
 * @param     [in] wait
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-7-19
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_DramDmaWrite(void xdata* pSrc, UINT32 dramAddrWord, UINT16 sizeByte, UINT8 wait)
{
	UINT8 data bSram = 1; //indicate if the pSrc is in SRAM
	UINT8 data memType = TYPE_DMEM; // DMEM / IMEM
	UINT16 idata sramAddr = (UINT16)pSrc;

	if (sramAddr >= 0x4000) {// 0x4000 <= pSrc <= 0xFFFF
		sramAddr -= 0x4000;
		bSram = 0;
	} else if (sramAddr >= 0x3800) {// 0x3800 <= pSrc <= 0x3BFF
		sramAddr &= 0x3FF; // sramAddr -= 0x3800
		memType = TYPE_SWAP_MEM;
	} else if (sramAddr >= 0x3000) {// 0x3000 <= pSrc <= 0x37FF
		sramAddr &= 0x7FF; // sramAddr -= 0x3000
		memType = TYPE_IMEM;
	} //else //0x0000 <= pSrc <= 0x1FFF

	if (bSram) {
		/* Dma from SRAM to dram */
		HAL_DmaSetSramAddr(sramAddr);
		HAL_DmaSetDramAddr(dramAddrWord);
		return HAL_DmaDo(1, 0, sizeByte, !wait, memType);
	} else {
		/* dma from dram to dram */
		return HAL_GprmDramDmaExt(
			(K_SDRAM_CodeSize << 1) + sramAddr,
			dramAddrWord << 1,
			sizeByte,
			wait,
			DMA_CHANNEL
		);
	}

	return SUCCESS;
}


/**
 * @fn        void HAL_DramFrmBufNumSet(UINT8 sel)
 * @brief     Set three/four frame buffer
 * @param     [in] sel :	0: two buffer, 1: three buffer, 2: four buffer
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-7-20
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_DramFrmBufNumSet(
	UINT8 sel
)
{
	XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] &= 0xcf;

	if(sel==1){	/*three frame buffer*/
		XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] |= 0x20;
	}
	else if(sel==2){	/*four frame buffer*/
		XBYTE[REG_Dram_MpgSb_MjpgRRegIdx] |= 0x10;
	}
}

/**
 * @fn        void DramHAL_CodeSentry(UINT32 LowBoundaryAddr,UINT32 HighBoundaryAddr,UINT32 RedirectAddr)
 * @brief     Set code sentry region
 * @param     [in] LowBoundaryAddrW	 : Low boundary address of code sentry region(word)
 * @param     [in] HighBoundaryAddrW : High boundary address of code sentry region(word)
 * @param     [in] RedirectAddrW	 : Address (Word) that save the writting data.
 * @retval    NONE
 * @see       NULL
 * @author    AllenLin
 * @since     2010-7-7
 * @todo      N/A
 * @bug       N/A
*/
void HAL_DramCodeSentry(UINT32 LowBoundaryAddrW,UINT32 HighBoundaryAddrW,UINT32 RedirectAddrW)
{
	XBYTE[REG_Dram_AddressLowBdy_B0] = READ8(&LowBoundaryAddrW, 3);
	XBYTE[REG_Dram_AddressLowBdy_B1] = READ8(&LowBoundaryAddrW, 2);
	XBYTE[REG_Dram_AddressLowBdy_B2] = READ8(&LowBoundaryAddrW, 1);
	XBYTE[REG_Dram_AddressLowBdy_B3] = READ8(&LowBoundaryAddrW, 0);
	XBYTE[REG_Dram_Address_HighBrdy_B0] = READ8(&HighBoundaryAddrW, 3);
	XBYTE[REG_Dram_Address_HighBrdy_B1] = READ8(&HighBoundaryAddrW, 2);
	XBYTE[REG_Dram_Address_HighBrdy_B2] = READ8(&HighBoundaryAddrW, 1);
	XBYTE[REG_Dram_Address_HighBrdy_B3] = READ8(&HighBoundaryAddrW, 0);
	XBYTE[REG_Dram_Address_Redirect_B0] = READ8(&RedirectAddrW, 3);
	XBYTE[REG_Dram_Address_Redirect_B1] = READ8(&RedirectAddrW, 2);
	XBYTE[REG_Dram_Address_Redirect_B2] = READ8(&RedirectAddrW, 1);
	XBYTE[REG_Dram_Address_Redirect_B3] = READ8(&RedirectAddrW, 0);
	XBYTE[REG_Dram_Cdsp1Eof_Sts] = 0x80; //clear old record for address_err
	XBYTE[REG_Dram_AddressErr] = 0x01; //address_err_enable
	XBYTE[REG_Dram_Cdsp1Eof_Inten] |= 0x80; /* enable code sentry interrupt */
	//err info is record at 0x2701,0x2703,0x270a,0x270b,cpu interrupt if happened
}

void
HAL_DramABBufAutoMap(
	UINT8 en
)
{

	XBYTE[REG_Dram_Cdsp1or2] &= 0xef;	/* It's safe that set auto map = 0 first */

	if (en==1){
		XBYTE[REG_Dram_Cdsp1or2] = XBYTE[REG_Dram_Cdsp1or2] | 0x10 ;
	}

	//XBYTE[REG_Dram_Cdsp1or2] = (en==0) ? (XBYTE[REG_Dram_Cdsp1or2] & 0xef) : (XBYTE[REG_Dram_Cdsp1or2] | 0x10);
}

void
HAL_DramWrYuvVDram1(
	UINT8 en
)
{

	XBYTE[REG_Dram_Cdsp1or2] = (en==0) ? (XBYTE[REG_Dram_Cdsp1or2] & 0xfe):(XBYTE[REG_Dram_Cdsp1or2] | 0x01);
}

