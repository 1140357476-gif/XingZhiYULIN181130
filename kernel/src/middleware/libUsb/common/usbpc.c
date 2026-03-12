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
 * @file      usbpc.c
 * @brief     Brief
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "usbpc.h"
//#include "hal.h"
#include "main.h"
#include "sdramhw.h"
#include "asicreg.h"
#include "hal_usb.h"

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
extern	xdata	UINT32	 G_BulkSize;
extern	xdata	UINT32	 G_BulkDRAMAddr;
extern	xdata	UINT32	 G_BulkFlashType;
extern	xdata	UINT32	 G_BulkSRAMAddr;
extern	xdata	UINT32	G_LastBulkSize;

 
extern UINT8 HAL_DramSkipWrite2Dram(void);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/ 

/**
 * @fn        UINT8 sp1kUsb_Pc(UINT8 G_FuncID, UINT32* ArgBuf)
 * @brief     sp1kUsb_Pc
 * @param     [in] G_FuncID
 * @param     [in] ArgBuf
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kUsb_Pc(UINT8 G_FuncID, UINT32* ArgBuf)
{
	switch(G_FuncID) {
		#if TAG_TODO/* mark for 1628 FPGA */
		case 0xf1:
			sp1kUsb_DownloadISP(ArgBuf[0], ArgBuf[1], ArgBuf[2], (UINT16)ArgBuf[3]);
			break;
		#endif

		case 0xf2:
			sp1kUsb_UploadFromDRAM(ArgBuf[0], ArgBuf[1]);
			break;

		case 0xf3:
			sp1kUsb_DownloadToDRAM(ArgBuf[0], ArgBuf[1]);
			break;

		case 0xf7:/*USB booter isp.bin bulk out*/
			//skip write DRAM
			HAL_DramSkipWrite2Dram();
			printf("\nUSB BOOT !, size=0x%lx\n",ArgBuf[1]);
			G_ISP = 1;
			sp1kUsb_DownloadToDRAM(ArgBuf[0], ArgBuf[1]);
			break;

		default:
			break;
	}

	return SUCCESS;
}

#if TAG_TODO /* mark for 1628 FPGA */
//-----------------------------------------------------------------------------
//sp1kUsb_DownloadISP
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 sp1kUsb_DownloadISP(UINT32 ISPDRAMAddr, UINT32 ISPROMAddr, UINT32 ISPSecAddr, UINT16 ISPSecCnt)
 * @brief     BulkOut setting for ISP function
 * @param     [in] ISPDRAMAddr
 * @param     [in] ISPROMAddr
 * @param     [in] ISPSecAddr
 * @param     [in] ISPSecCnt
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kUsb_DownloadISP(UINT32 ISPDRAMAddr, UINT32 ISPROMAddr, UINT32 ISPSecAddr, UINT16 ISPSecCnt) USING_0
{
	UINT8 status;

	//skip write DRAM
	HAL_DramSkipWrite2Dram();
	ISPDRAMAddr = ISPDRAMAddr;
	ISPSecAddr = ISPSecAddr;
	//G_BulkDRAMAddr = ISPROMAddr + 0x4000;
	G_BulkDRAMAddr = ISPDRAMAddr + 0x10000;
	G_BulkSize = ((UINT32)ISPSecCnt) << 8;
//	printf("L1_DownloadISP %lx %lx\n",G_BulkDRAMAddr, G_BulkSize);
	while ((G_usbState & 0x01) != 0x01);
	G_usbState &= 0xfe;
	G_ISP = 1;
	ENTER_CRITICAL( status );
	/* Will be used inside isp.bin ,0x4000 is to have a free space*/
	//printf("L1_DownloadISP %lx G_BulkDRAMAddr=%lx %lx\n",ISPROMAddr, G_BulkDRAMAddr,G_BulkSize);
	SDRAM_CardSetStartAddress(ISPROMAddr, FALSE);
	M_DRAM_WriteWord((UINT8)G_BulkSize,(UINT8)(G_BulkSize >> 8));
 	M_DRAM_WriteWord((UINT8)(G_BulkSize >> 16),(UINT8)(G_BulkSize >> 24));
	M_DRAM_WriteWord((UINT8)G_BulkDRAMAddr,(UINT8)(G_BulkDRAMAddr >> 8));
 	M_DRAM_WriteWord((UINT8)(G_BulkDRAMAddr >> 16),(UINT8)(G_BulkDRAMAddr >> 24));
	HAL_UsbBulkEnable(0);

	return SUCCESS;
}
#endif

//-----------------------------------------------------------------------------
//sp1kUsb_DownloadToDRAM
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 sp1kUsb_DownloadToDRAM(UINT32 BulkDRAMAddr, UINT32 BulkSize)
 * @brief     Download File to DRAM
 * @param     [in] BulkDRAMAddr
 * @param     [in] BulkSize
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kUsb_DownloadToDRAM(UINT32 BulkDRAMAddr, UINT32 BulkSize)
{
	//printf("BulkOut DRAMAddr=%lx,BulkSize=%lx\n",BulkDRAMAddr,(UINT32)BulkSize);
	G_BulkDRAMAddr = BulkDRAMAddr;
	G_ISPDRAMAddr = BulkDRAMAddr;
	G_BulkSize = BulkSize;
	G_LastBulkSize = BulkSize;
	G_BulkFlashType = 0x00;
	//printf("L1_DownloadToDRAM %lx %lx\n",G_ISPDRAMAddr, G_BulkSize);
	//richie@0125
	while ((G_usbState & 0x01) != 0x01);
	G_usbState &= 0xfe;
//	printf("L1_DownloadToDRAM %lx %lx\n",BulkDRAMAddr, BulkSize);
	HAL_UsbBulkEnable(0);
	HAL_UsbBulkFlashAddrSet(G_BulkFlashType,G_BulkDRAMAddr);
	HAL_UsbBulkOut(&G_BulkSize);
	G_BulkDRAMAddr = HAL_UsbBulkFlashAddrGet(G_BulkFlashType);
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//sp1kUsb_UploadFromDRAM
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 sp1kUsb_UploadFromDRAM(UINT32 BulkDRAMAddr, UINT32 BulkSize)
 * @brief     Upload File From DRAM
 * @param     [in] BulkDRAMAddr
 * @param     [in] BulkSize
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kUsb_UploadFromDRAM(UINT32 BulkDRAMAddr, UINT32 BulkSize)
{
	UINT16 BulkInSize;

	BulkInSize = 0x40;
	printf("a,s=%lx,%lx\n",BulkDRAMAddr,BulkSize);
	if (G_bUSBSpeed == 0x01) {
		BulkInSize = 0x200;
	}

	G_BulkDRAMAddr = BulkDRAMAddr;
 	G_BulkSize = BulkSize;
	G_BulkFlashType = 0x00;

	while ((G_usbState & 0x01) != 0x01);
	G_usbState &= 0xfe;

	

	HAL_UsbBulkEnable(1);
	HAL_UsbBulkFlashAddrSet(G_BulkFlashType,G_BulkDRAMAddr);
	HAL_UsbBulkIn(&G_BulkSize);	
	G_BulkDRAMAddr = HAL_UsbBulkFlashAddrGet(G_BulkFlashType);
	return SUCCESS;
}

