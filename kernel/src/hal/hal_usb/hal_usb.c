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
 * @file		hal_usb.c
 * @brief		Hal API for USB module
 * @author		kai.wang
 * @since		2010-01-01
 * @date		2010-04-21
 */

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "bulkout.h"
#include "main.h"
#include "initio.h"
#include "gpio.h"
#include "setmode.h"
#include "cardopt.h"
#include "l1.h"
#include "sp1k_hal_api.h"
#include "usbopt.h"
#include "class.h"	//cx++
#include "hal_dram.h"
#include "hal_global.h"
#include "hal_usb.h"
#include "dbg_wdt.h"
#include "reg_def.h"

/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
//#define DEBUG_PRINTHALUSB 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#ifdef DEBUG_PRINTHALUSB
#define PRINT_HALUSB printf
#else
#define PRINT_HALUSB ;/SLASH2
#endif
#define USB_EL17_TEST 0
#define USB_IDX_EP7_IN       0x40

/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
static UINT32 S_BulkDRAMAddrWord;	/*static variable*/
static UINT32 S_BulkSRAMAddrByte;	/*static variable*/
static UINT32 S_BulkFlashtype;
static UINT32 S_BulkOutSize = 0;
static UINT32 S_Dmapktcnt = 0;
static UINT8  S_BulkInEn = 0;
static UINT8  S_BulkOutEn = 0;
xdata UINT8 G_USBSpeedStartCheck = 0x00;
xdata UINT8 G_UsbSlunShowNand = 0x00;
xdata UINT32 G_USBCheckNewConfigStartTime;
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
//extern xdata UINT16	G_usPostJpegWidth;
extern xdata UINT32	G_LastBulkSize;
extern xdata UINT32	G_ISPDRAMAddr;
extern xdata UINT32	G_BulkSize;
extern xdata UINT8 	G_USBReset;
extern void bootCore(UINT32 srcAddrWord, UINT32 dstAddrWord, UINT32 bootSizeByte);
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
//-----------------------------------------------------------------------------
//HAL_BulkInFromDRAM
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_UsbBulkInFromDRAM(UINT32 DRAMAddrWord, UINT16 nByte)
 * @brief	  BulkInFromDRAM to PC
 * @param	  [in] DRAMAddr	:Source DRAM word address
 * @param	  [in] nByte	:Transfer size	
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_BulkInFromDRAM
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_UsbBulkInFromDRAM(UINT32 DRAMAddrWord, UINT16 nByte)
{
	UINT8 status;

	status = HAL_DmaSetDramAddr(DRAMAddrWord);
	status = HAL_DmaDo(DMA_PATH_DRAM, DMA_PATH_USB, nByte, 0, 0); // DMA source DRAM, destination USB
#if 0
	S_Dmapktcnt ++;
	
	if ( S_Dmapktcnt > 0xffffff ) {
		S_Dmapktcnt = 0;
	}
	if ((XBYTE[REG_Usb_BulkAutoMode] & 0x08) == 0x08 ) { /*auto mode*/

		XBYTE[REG_Usb_DMAPktCntBkIn0] = READ8(S_Dmapktcnt, 3);
		XBYTE[REG_Usb_DMAPktCntBkIn1] = READ8(S_Dmapktcnt, 2);
		XBYTE[REG_Usb_DMAPktCntBkIn2] = READ8(S_Dmapktcnt, 1);
	}
#endif
	return(status);
}

//-----------------------------------------------------------------------------
//HAL_UsbBulkOutToDRAM
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_UsbBulkOutToDRAM(UINT32 DRAMAddrWord, UINT16 nByte)
 * @brief	  Bulk out from host to DRAM (DMA)
 * @param	  [in] DRAMAddr	:Destination DRAM word address:					
 * @param	  [in] nByte	:Transfer size
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_BulkOutToDRAM
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_UsbBulkOutToDRAM(UINT32 DRAMAddrWord, UINT16 nByte)
{
	UINT8 status;
#if 0	
	S_Dmapktcnt ++;
		
	if ( S_Dmapktcnt > 0xffffff ) {
		S_Dmapktcnt = 0;
	}
	if ((XBYTE[REG_Usb_BulkAutoMode] & 0x10) == 0x10 ) { /*auto mode*/
		
		XBYTE[REG_Usb_DMAPktCntBkOut0] = READ8(S_Dmapktcnt, 3);
		XBYTE[REG_Usb_DMAPktCntBkOut1] = READ8(S_Dmapktcnt, 2);
		XBYTE[REG_Usb_DMAPktCntBkOut2] = READ8(S_Dmapktcnt, 1);
	}
#endif
	status = HAL_DmaSetDramAddr(DRAMAddrWord);
	status = HAL_DmaDo(DMA_PATH_USB, DMA_PATH_DRAM, nByte, 0, 0); // DMA source DRAM, destination USB

	return(status);
}

/**
 * @fn        UINT8 HAL_UsbBulkInFromSRAM(UINT32 SRAMAddrByte, UINT16 nByte)
 * @brief     HAL_UsbBulkInFromSRAM
 * @param     [in] SRAMAddrByte
 * @param     [in] nByte
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-28
 * @todo      N/A
 * @bug       N/A
*/

UINT8 HAL_UsbBulkInFromSRAM(UINT32 SRAMAddrByte, UINT16 nByte)
{
	UINT8 status;
	if ( SRAMAddrByte == 512 ) { /*512 on SPCA1528*/
		SRAMAddrByte = 0x00;
		status = HAL_DmaSetSramAddr(0x0000);
		//PRINT_HALUSB("G_BulkSize = %lx ", G_BulkSize);
		//PRINT_HALUSB("S_BulkSRAMAddrByte = %lx\n", S_BulkSRAMAddrByte);
	}
	else {
		status = HAL_DmaSetSramAddr((UINT16)SRAMAddrByte+0x0000);		
	}
	status = HAL_DmaDo(DMA_PATH_SRAM, DMA_PATH_USB, nByte, 0, 0);
#if 0
	S_Dmapktcnt ++;
	
	if ( S_Dmapktcnt > 0xffffff ) {
		S_Dmapktcnt = 0;
	}
	if ((XBYTE[REG_Usb_BulkAutoMode] & 0x08) == 0x08 ) { /*auto mode*/

		XBYTE[REG_Usb_DMAPktCntBkIn0] = READ8(S_Dmapktcnt, 3);
		XBYTE[REG_Usb_DMAPktCntBkIn1] = READ8(S_Dmapktcnt, 2);
		XBYTE[REG_Usb_DMAPktCntBkIn2] = READ8(S_Dmapktcnt, 1);
	}
#endif
	return(status);
}

/**
 * @fn        UINT8 HAL_UsbBulkOutToSRAM(UINT32 SRAMAddrByte, UINT16 nByte)
 * @brief     HAL_UsbBulkOutToSRAM
 * @param     [in] SRAMAddrByte
 * @param     [in] nByte
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-28
 * @todo      N/A
 * @bug       N/A
*/

UINT8 HAL_UsbBulkOutToSRAM(UINT32 SRAMAddrByte, UINT16 nByte)
{
	UINT8 status;
#if 0
	S_Dmapktcnt ++;
		
	if ( S_Dmapktcnt > 0xffffff ) {
		S_Dmapktcnt = 0;
	}
	if ((XBYTE[REG_Usb_BulkAutoMode] & 0x10) == 0x10 ) { /*auto mode*/
		
		XBYTE[REG_Usb_DMAPktCntBkOut0] = READ8(S_Dmapktcnt, 3);
		XBYTE[REG_Usb_DMAPktCntBkOut1] = READ8(S_Dmapktcnt, 2);
		XBYTE[REG_Usb_DMAPktCntBkOut2] = READ8(S_Dmapktcnt, 1);
	}
#endif	
	status = HAL_DmaSetSramAddr((UINT16)SRAMAddrByte+0x0000);
	status = HAL_DmaDo(DMA_PATH_USB, DMA_PATH_SRAM, nByte, 0, 0);	

	return(status);
}

/**
 * @fn        UINT8 HAL_UsbBulkIn(UINT32 *bulksize)
 * @brief     HAL_UsbBulkIn
 * @param     [in] bulksize
 * @retval    return = SUCCESS / FAIL
 * @see       HAL_BulkInOut
 * @author    kai.wang
 * @since     2010-4-29
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_UsbBulkIn(UINT32 *bulksize)USING_0
{
	UINT16 BulkInSize;
	
	if ( *bulksize == 0 ) {
		return FAIL;
	}
	
	if ( XBYTE[REG_Usb_HighSpeed] & 0x01 == 0x01 ) {	/*high speed*/    
		BulkInSize = 0x200;
	}
	else {	/*full speed*/
		BulkInSize = 0x40;
	}
	
//	if ( (XBYTE[REG_Usb_BulkAutoMode] & 0x08) == 0x08 ) {	/*auto mode*/
//		XBYTE[REG_Usb_AckCntRst] &= 0xf7;	/*clear reset the BulkInAckCnt*/
//	}
	if ( S_BulkFlashtype == 0x00 ) {/*Bulkin to Dram*/
		if ( *bulksize < (UINT32)(BulkInSize + 1)) {
			HAL_UsbBulkInFromDRAM(S_BulkDRAMAddrWord,(UINT16)*bulksize);
		}
		else {
			HAL_UsbBulkInFromDRAM(S_BulkDRAMAddrWord,BulkInSize);
		}
//		if ( (XBYTE[REG_Usb_BulkAutoMode]&0x08) != 0x08 ) {	/*manual mode*/
			XBYTE[REG_Usb_TransactionEn] |= K_USB_AP_IN_OPEN;//0x08;
//		}
		//printf("0x2500=%bx\n",XBYTE[0x2500]);
		while ( S_BulkInEn ) {	// Loop for BulkIn From DRAM	
		dbgWdtFeed(20000);
		//printf("b1\n");
			if ((XBYTE[REG_Usb_EpAckIntSts] & 0x08) == K_USB_AP_IN_ACK_MASK){
				XBYTE[REG_Usb_EpAckIntSts] |= K_USB_AP_IN_ACK_MASK;
				if ( *bulksize < (UINT32)(BulkInSize + 1) ) {
					S_BulkDRAMAddrWord = S_BulkDRAMAddrWord + ((*bulksize) >> 1);
					printf("LBIn\n");
					XBYTE[REG_Usb_BufClr] = K_USB_AP_IN_OPEN;//0x08;
					XBYTE[REG_Usb_TransactionEn] = 0x00;
					S_BulkInEn = 0;
				}	/*BulkIn has completed*/
				else {
					*bulksize = *bulksize - BulkInSize;
					S_BulkDRAMAddrWord = S_BulkDRAMAddrWord + (BulkInSize >> 1);
					//PRINT_HALUSB("@G_BulkSize2 = %lx,ADDR=%lx\n", (UINT32)*bulksize,S_BulkDRAMAddrWord);
					
					if ( *bulksize < (BulkInSize + 1) ) {
						HAL_UsbBulkInFromDRAM(S_BulkDRAMAddrWord,(UINT16)*bulksize);
					}
					else {
						HAL_UsbBulkInFromDRAM(S_BulkDRAMAddrWord,BulkInSize);
					}
//					if ( (XBYTE[REG_Usb_BulkAutoMode]&0x08) != 0x08 ) { 	/*manual mode*/
						XBYTE[REG_Usb_TransactionEn] |= K_USB_AP_IN_OPEN;//0x08;
//					}
				}
			}
		}
	} // S_BulkFlashtype == 0
	else if ( S_BulkFlashtype == 0x01 ) {/*bulkin to other flash*/			
		
		//PRINT_HALUSB("upload flash function\n");
		if ( *bulksize < (UINT32)(BulkInSize + 1) ) {
			HAL_UsbBulkInFromSRAM(S_BulkSRAMAddrByte,(UINT16)*bulksize);
		}
		else {
			HAL_UsbBulkInFromSRAM(S_BulkSRAMAddrByte,BulkInSize);
		}
		while ( S_BulkInEn ) { // Loop for BulkIn From DRAM	
		
			if ( (XBYTE[REG_Usb_EpAckIntSts] & 0x08) == K_USB_AP_IN_ACK_MASK ) {
			
				XBYTE[REG_Usb_EpAckIntSts] |= K_USB_AP_IN_ACK_MASK;
				
				if ( *bulksize < (UINT32)(BulkInSize + 1) ) {
					XBYTE[REG_Usb_TransactionEn] = 0x00;
					S_BulkFlashtype = 0x00;
					S_BulkInEn = 0;
				}	/* BulkIn has completed*/
				
				else {
					*bulksize = *bulksize - BulkInSize;
					S_BulkSRAMAddrByte = S_BulkSRAMAddrByte + BulkInSize;
								
					if ( *bulksize<(BulkInSize+1) ) {
						HAL_UsbBulkInFromSRAM(S_BulkSRAMAddrByte,(UINT16)*bulksize);

//						if ( (XBYTE[REG_Usb_BulkAutoMode]&0x08) != 0x08 ) {	/*manual mode*/
							XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;
//						}
						*bulksize = 0x00;
						//S_BulkFlashtype = 0x00;
						S_BulkSRAMAddrByte = 0x00;
					}
					else {
						HAL_UsbBulkInFromSRAM(S_BulkSRAMAddrByte,(UINT16)*bulksize);
						
//						if ( (XBYTE[REG_Usb_BulkAutoMode]&0x08) != 0x08 ) {	/*manual mode*/
							XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;
//						}
					}
				}
				//PRINT_HALUSB("*bulksize = %lx ", *bulksize);
				//PRINT_HALUSB("S_BulkSRAMAddrByte = %lx\n", S_BulkSRAMAddrByte);
			}
		}
	} // S_BulkFlashtype == 0x01
	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_UsbBulkOut(UINT32 *bulksize)
 * @brief     HAL_UsbBulkOut
 * @param     [in] bulksize
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-6-1
 * @todo      N/A
 * @bug       N/A
*/
#pragma OT(7, SIZE) 
UINT8 HAL_UsbBulkOut(UINT32 *bulksize)USING_0
{
	UINT16 BulkOutBufCnt;
	
	if ( *bulksize == 0 ) {
		return FAIL;
	}
//	if ( (XBYTE[REG_Usb_BulkAutoMode] & 0x08) == 0x08 ) {	/*auto mode*/
//		XBYTE[REG_Usb_AckCntRst] &= 0xef;	/*clear reset the BulkOutAckCnt*/
//	}		
	//printf("@out2500=%bx\n",XBYTE[0x2500]);	
	while ( S_BulkOutEn ) {  // while Loop for BulkOutToDRAM	
	
		if ( XBYTE[REG_Usb_EpAckIntSts] == K_USB_AP_OUT_ACK_MASK ) {
		
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_AP_OUT_ACK_MASK;			
			if (S_BulkFlashtype==0x00) {/* Bulkout to DRAM*/		
				
				//PRINT_HALUSB("@S_BulkoutSize0= %lx\n", S_BulkOutSize);
				//PRINT_HALUSB(" S_BulkDRAMAddrWord1 = %lx\n",S_BulkDRAMAddrWord);
				BulkOutBufCnt = (UINT16)XBYTE[K_USB_AP_OUT_BUF_SIZE_LSB] | (UINT16)(XBYTE[K_USB_AP_OUT_BUF_SIZE_MSB] << 8);

				HAL_UsbBulkOutToDRAM(S_BulkDRAMAddrWord,(UINT16)BulkOutBufCnt);	
				
				S_BulkDRAMAddrWord = S_BulkDRAMAddrWord + ((UINT32)BulkOutBufCnt/2);				
				S_BulkOutSize = S_BulkOutSize + (UINT32)BulkOutBufCnt;
				//PRINT_HALUSB("S_BulkOutsize=%lx\n",S_BulkOutSize);
				
				if ( S_BulkOutSize == *bulksize ) {
					XBYTE[REG_Usb_BufClr] = K_USB_AP_OUT_OPEN;//0x08;
					XBYTE[REG_Usb_TransactionEn] = 0x00;
					S_BulkOutSize = 0x00;
					//S_BulkFlashtype = 0x00;
					printf("LBout\n");

					if( G_ISP ){
						//recover the isp.bin size
						*bulksize = G_LastBulkSize;
						bootCore(G_ISPDRAMAddr, 0, G_BulkSize);
					}	
					S_BulkOutEn = 0;
				}
				else {
//					if( (XBYTE[REG_Usb_BulkAutoMode]&0x10) != 0x10 ) {   /*manual mode*/
						XBYTE[REG_Usb_TransactionEn] = K_USB_AP_OUT_OPEN;//0x10;
//					}
				}			
			} // Bulkout to DRAM
			else {// Bulkout to Flash
				//PRINT_HALUSB("Download to Flash\n");
				BulkOutBufCnt = XBYTE[K_USB_AP_OUT_BUF_SIZE_LSB];

				// DMA to CPU SRAM
				HAL_UsbBulkOutToSRAM(S_BulkSRAMAddrByte,(UINT16)BulkOutBufCnt);

				S_BulkSRAMAddrByte = S_BulkSRAMAddrByte + ((UINT32)BulkOutBufCnt);
				S_BulkOutSize = S_BulkOutSize + (UINT32)BulkOutBufCnt;
				
				if ( S_BulkSRAMAddrByte == 512 ) {
					HAL_DmaSetSramAddr(0x0000);
					S_BulkSRAMAddrByte = 0x00;
					//PRINT_HALUSB("G_BulkSize = %lx ", *bulksize);
					//PRINT_HALUSB("S_BulkSRAMAddrByte = %lx\n", S_BulkSRAMAddrByte);
				}						 
							
				if (S_BulkOutSize == *bulksize) {
					*bulksize = 0x00;
					S_BulkSRAMAddrByte = 0x00; 
					S_BulkOutSize = 0x00;
					//S_BulkFlashtype = 0x00;
					S_BulkOutEn = 0;
					// Bulkout done
				}
				else {
//					if((XBYTE[REG_Usb_BulkAutoMode]&0x10)!=0x10) {   /*manual mode*/
						XBYTE[REG_Usb_TransactionEn] = K_USB_AP_OUT_OPEN;//0x10;	
//					}
				}
				//PRINT_HALUSB("G_BulkSize = %lx ", *bulksize);
				//PRINT_HALUSB("S_BulkSRAMAddrByte = %lx\n", S_BulkDRAMAddrWord);
			} // Bulkout to Flash
		} 
	}
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//HAL_USBSwPlugOut
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_UsbSwPlugOut(void)
 * @brief	  USB software plug out function.
 * @param	  NONE
 * @retval	  NONE
 * @see 	  HAL_USBSwPlugOut
 * @author	  Allen Lin
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
#pragma OT(9, SIZE) 

void HAL_UsbSwPlugOut(void)
{	
	XBYTE[REG_Usb_Status] |= 0x10; // plug out
	
	XBYTE[REG_Usb_Suspend] = 0x01; // suspend PHY
	XBYTE[REG_Usb_UsbClkEn] &= 0xE0; // disable usb internal clock 

	XBYTE[0x2014] &= (~0x03);
	XBYTE[0x2015] &= (~0x20);

	XBYTE[REG_Usb_BufClr] = 0xFF;
	G_USBReset = 0;
}

//-----------------------------------------------------------------------------
//HAL_UsbSwPlugOut_Intr
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_UsbSwPlugOut_Intr(void)
 * @brief	  USB software plug out function used in intrrupter server.
 * @param	  NONE
 * @retval	  NONE
 * @see 	  HAL_USBSwPlugOut_Intr
 * @author	  Allen Lin
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_UsbSwPlugOut_Intr(void)
{
	UINT8 reg2504,reg2506,reg2507;
	
	reg2504 = XBYTE[REG_Usb_EpAckIntEn];
	reg2506 = XBYTE[REG_Usb_IntEn];
	reg2507 = XBYTE[REG_Usb_StdCmdIntEn];

	G_USBSpeedStartCheck = 0x00;
	
	XBYTE[REG_Usb_Status] |= 0x10; // plug out
	//Can't printf in interrupt routine
	XBYTE[0x2002]  |= 0x08;// global usb reset 
	XBYTE[0x2002] &=~ 0x08; 
	
	XBYTE[REG_Usb_Suspend] = 0x01; // suspend PHY
	XBYTE[REG_Usb_UsbClkEn] &= 0xE0; // disable usb internal clock 

	XBYTE[0x2014] &= (~0x03);
	XBYTE[0x2015] &= (~0x20);

	XBYTE[REG_Usb_BufClr] = 0xFF;
	XBYTE[REG_Usb_EpAckIntEn] = reg2504;
	XBYTE[REG_Usb_IntEn] = reg2506;
	XBYTE[REG_Usb_StdCmdIntEn] = reg2507;
	G_USBReset = 0;
}

//-----------------------------------------------------------------------------
//HAL_UsbSwPlugIn
//-----------------------------------------------------------------------------
/**
 * @fn		  void HAL_UsbSwPlugIn(void)
 * @brief	  USB software plug in function.
 * @param	  NONE
 * @retval	  NONE
 * @see 	  HAL_USBSwPlugIn
 * @author	  Allen Lin
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_UsbSwPlugIn(void)
{
	UINT32 time;
#if USB_EL17_TEST		
	UINT8 temp[11];
#endif	
	G_USBSpeedStartCheck = 0x01;
	HAL_GlobalReadGTimer(&time);
	G_USBCheckNewConfigStartTime = time;
#if !USBSIDC_OPTION
	XBYTE[REG_Usb_IntEn] |= 0x10;
#endif	
	
	XBYTE[0x2015] |= 0x20;
	XBYTE[0x2014] |= 0x03;
#if K_AUDIOINTER	
	XBYTE[REG_Usb_UsbClkEn] |= 0x1f; // enable usb internal clock , cx modify for UAC
#else
	XBYTE[REG_Usb_UsbClkEn] |= 0x17; // enable usb internal clock  
#endif
	
	XBYTE[REG_Usb_Suspend] |= 0x02; // RST Suspendcnt
	XBYTE[REG_Usb_Suspend] = 0x00; // wake up PHY
#if USB_EL17_TEST	
	/* 20100802 shengkai Modified for USB EL-17 test start */
	temp[0] = XBYTE[0x2008];
	temp[1] = XBYTE[0x2A60];
	temp[2] = XBYTE[0x2A5C];
	temp[3] = XBYTE[0x2A62];
	temp[4] = XBYTE[0x290C];
	temp[5] = XBYTE[0x2900];  
	temp[6] = XBYTE[0x2910];  
	temp[7] = XBYTE[0x2914];  
	temp[8] = XBYTE[0x2905];  
	temp[9] = XBYTE[0x2930];	
	temp[10] = XBYTE[0x2931];  
	
	HAL_GlobalTimerWait(11);
	/*printf("i2c enable write\n");*/
	XBYTE[0x2008] = 0x01;
	XBYTE[0x2A60] = 0;
	XBYTE[0x2A5C] = 0;
	XBYTE[0x2A62] = 0;
	XBYTE[0x290C] = 0;
	XBYTE[0x2900] = 0x02;  //set slave_addr
	XBYTE[0x2910] = 0x02;  //set frequency
	XBYTE[0x2914] = 0x11;  //set r/w counter
	XBYTE[0x2905] = 0x01;  // set Pos
	XBYTE[0x2930]  = 0x06;	//set addr
	XBYTE[0x2931]  = 0x2D;  // set value
	while((XBYTE[0x29A0]&0x01)==0x01){/*printf("BUSY!! \n");*/}
	/*printf("ACK error = %bx \n",XBYTE[0x29a1]);*/
	
	XBYTE[0x2A60] = temp[1];
	XBYTE[0x2A5C] = temp[2];
	XBYTE[0x2A62] = temp[3];
	XBYTE[0x290C] = temp[4];
	XBYTE[0x2900] = temp[5];  
	XBYTE[0x2910] = temp[6];  
	XBYTE[0x2914] = temp[7];  
	XBYTE[0x2905] = temp[8];  
	XBYTE[0x2930] = temp[9];	
	XBYTE[0x2931] = temp[10];  
	XBYTE[0x2008] = temp[0];
	/* 20100802 shengkai Modified for USB EL-17 test end */
#else
	HAL_GlobalTimerWait(6);
#endif
	XBYTE[REG_Usb_Status] &= (~0x10); // plug in
}

/**
 * @fn             void HAL_UsbPccamInit(UINT8 FormatIndex)
 * @brief         USB initial setting for pccam mode
 * @param      [in] FormatIndex 1:MJPEG; 2: YUY2
 * @retval       NONE
 * @see          NULL
 * @author      cx
 * @since        2010-4-30
 * @todo         N/A
 * @bug          N/A
*/
void
HAL_UsbPccamInit(
    UINT8 FormatIndex
 )
{	
	#if 0
	/* CSDP init */
	XBYTE[0x21B0] |= 0x11;
	XBYTE[0x21D0] |= 0x02;
	XBYTE[0x21D4] = 0x00;
	#endif
      // XBYTE[REG_Usb_Status]|=0x04; //cx mask
/*#if (K_AUDIOINTER)
	XBYTE[REG_Usb_UsbMode] = 0x01;
	XBYTE[REG_Usb_LogicEp1_Disable] = 0xe0;
#else
    XBYTE[REG_Usb_UsbMode] = 0x03;
    XBYTE[REG_Usb_LogicEp1_Disable] = 0x00;
#endif*/

	if ( FormatIndex == VC_FORMAT_INDEX_UNCOMPRESSED_YUY2 ){
		/* image type to 8Y4U4V */
		XBYTE[REG_Dram_ImgType]= 0x03;
		/* For YUY2 mode only, YUV buffer control enable for USB */
		XBYTE[REG_Dram_LensGainUpdt]|= 0x10;	//{3'b0, mpgsb_usb_en, mpgsb_mjpg_en, mpgsb_dispimg_en, 2'h0}
		XBYTE[REG_Dram_LensGainUpdt]&= 0xFB; /* disable display */
		XBYTE[REG_Usb_PccamDataFormat]=0x00;  //0;yuy2 1; vlc		
	}

	else if ( FormatIndex == VC_FORMAT_INDEX_MJPEG_YUV422 ){
		/* image type to MJPEG */
		XBYTE[REG_Dram_ImgType]= 0x02;	 
		XBYTE[REG_Dram_LensGainUpdt]&= 0xFB; /* disable display */
		XBYTE[REG_Usb_PccamDataFormat]=0x01;  //0;yuy2 1; vlc		
	}


	XBYTE[REG_Usb_VidAudBufRst] |= 0x01;
	XBYTE[REG_Usb_HeadLength] = 0x0c; //12 bit header
	XBYTE[REG_Usb_HeadInfo0] = 0x8c;
	XBYTE[REG_Usb_IsoPipeEn]|=0x01;	//Enable video ISO pipe
	XBYTE[0x2000]=0x04;	//Set to pccam mode
#if TAG_TODO
	if (XBYTE[REG_Usb_HighSpeed] == 0x00){
		if (G_usPostJpegWidth == 640){
			XBYTE[0x2027] = 0x0F;
		}
		else {
		/* 
		remove front nx clock set when enter usb pccam mode 
		because we can't modify the setting of sensor init	  
		*/
		//XBYTE[0x2027] = 0x03; 
		}				
	}
	else {
		//XBYTE[0x2027] = 0x03; //remove front nx clock set when enter usb pccam mode
	}
#endif	
		//XBYTE[REG_Dram_ClipIdle]=0x01;			//startclip/clipidle
}
//-----------------------------------------------------------------------------
//HAL_UsbAutoCompress
//-----------------------------------------------------------------------------

/**
 * @fn		  UINT8 HAL_UsbAutoCompress(UINT32 VLCAAddrWord, UINT32 VLCBAddrWord, UINT8 Option)
 * @brief	         auto compress vlc data
 * @param	  [in] VLCAAddrWord
 * @param	  [in] VLCBAddrWord
 * @param	  [in] Option
 * @retval	  return = SUCCESS / FAIL
 * @see 	         HAL_USBAutoCompress
 * @author	  cx
 * @since	         2010-4-30
 * @todo	          N/A
 * @bug 	          N/A
*/
UINT8
HAL_UsbAutoCompress(
	UINT32 VLCAAddrWord, 
	UINT32 VLCBAddrWord, 
	UINT8 Option
) USING_0
{
	UINT8 status = HALK_SUCCESS;
	UINT8 imgtype;

	imgtype   = Option&0x03;
	HAL_DramAVLCAddrSet(VLCAAddrWord);
	HAL_DramBVLCAddrSet(VLCBAddrWord);
	XBYTE[REG_Jpeg_PbreScale]=0x00; // [mantis 20298] reset decode rescale factor
	XBYTE[REG_Jpeg_Rotation_Type]=0x00;    // no rotate
	XBYTE[REG_Jpeg_Turboen] = 0; // no turbo mode
	XBYTE[REG_Jpeg_YUV_Type]=imgtype&0xFF; //set img type
	XBYTE[REG_Jpeg_Enthumb]=0x00; //disable the thumbnail(DCT DC value)
	XBYTE[REG_Jpeg_JFIF]=0x01;    // JFIF format
	XBYTE[REG_Jpeg_Intr_Event]=0xFF;    // clear sts

	HAL_DramVLCRdyClr(0);
	HAL_DramVLCRdyClr(1);

	// reset vlc index
	XBYTE[REG_Dram_VlcRegIdx] = 0x00;
	XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x10;
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x10;

	XBYTE[REG_Dram_LensGainUpdt]|=0x08;		   //{3'b0, mpgsb_usb_en, mpgsb_mjpg_en, mpgsb_dispimg_en, 2'h0}
	HAL_DramZfUpdTrig(0x00); //clrpvzfupdate
	XBYTE[REG_Dram_ZfUpdEn]=0x07; 		  //zfupden

	if( XBYTE[REG_Jpeg_PbreScale] != (UINT8)0x00 ) {
	//PRINT_HALUSB( "ERROR: 0x2825[%bx]\n", XBYTE[REG_Jpeg_PbreScale] );
	}

	XBYTE[0x2000]=0x04; 		  //cammode: pccam mode
	XBYTE[REG_Dram_ClipIdle]=0x01; 		  //startclip/clipidle

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//HAL_UsbBulkEnable
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_UsbBulkEnable(UINT8 BulkDir)
 * @brief	  USB bulk out/in enable
 * @param	  [in] BulkDir	:0: bulk out enable, 1:bulk in enable
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_UsbBulkEnable
 * @author	  kai.wang
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_UsbBulkEnable(UINT8 BulkDir)
{
	/* Solve Mantis#35376 */
	XBYTE[REG_Usb_EpAckIntEn] &= 0x07;

	if ( BulkDir == 0 ){			/*enable bulk out*/
		if ( (XBYTE[REG_Usb_BulkAutoMode]&0x10) != 0x10) {	/*manual mode*/	
			XBYTE[REG_Usb_TransactionEn] |= K_USB_AP_OUT_OPEN;
		}
#if 0
		else {	/*auto mode*/
			
			XBYTE[REG_Usb_AckCntRst] |=0x10;
			S_Dmapktcnt = 0;
			XBYTE[REG_Usb_DMAPktCntBkIn0] = 0x00;
			XBYTE[REG_Usb_DMAPktCntBkIn1] = 0x00;
			XBYTE[REG_Usb_DMAPktCntBkIn2] = 0x00;
			XBYTE[REG_Usb_TransactionEn] |= K_USB_AP_OUT_OPEN;	/* first allow the next Bulk-out transactiont */
		}
#endif			
		S_BulkOutEn = 1;
	
	}
	else if ( BulkDir == 0x01 ){	/*enable bulk in*/
		if ( (XBYTE[REG_Usb_BulkAutoMode]&0x08) != 0x08) {	/*manual mode*/
		//	XBYTE[REG_Usb_TransactionEn] |= K_USB_AP_IN_OPEN;
			//printf("en\n");
		}
#if 0		
		else {	/*auto mode*/
			XBYTE[REG_Usb_AckCntRst] |=0x08;
			S_Dmapktcnt = 0;
			XBYTE[REG_Usb_DMAPktCntBkIn0] = 0x00;
			XBYTE[REG_Usb_DMAPktCntBkIn1] = 0x00;
			XBYTE[REG_Usb_DMAPktCntBkIn2] = 0x00;
		}
#endif		
		S_BulkInEn = 1;
		
	}
	else {
		return FAIL;	
	}

	return SUCCESS;
	
}

//-----------------------------------------------------------------------------
//HAL_USBForceFS
//-----------------------------------------------------------------------------

/**
 * @fn		  void HAL_UsbForceFS(UINT8 usbconfigflag)
 * @brief	  Force usb run at full speed
 * @param	  usbconfigflag
 * @retval	  NONE
 * @see 	  HAL_USBForceFS
 * @author	  Allen Lin
 * @since	  2010-4-26
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_UsbForceFS(
	UINT8 usbconfigflag
)
{
	if ( usbconfigflag == 0 ) {
		//TIMER1_Stop();
		XBYTE[REG_Usb_Status] |= 0x10; // plug out
		
		XBYTE[REG_Usb_Suspend] = 0x01; // suspend PHY
		XBYTE[REG_Usb_UsbClkEn] = 0x00; // disable usb internal clock 

		XBYTE[0x2014] &= (~0x02);
		XBYTE[0x2015] &= (~0x20);

		XBYTE[REG_Usb_BufClr] = 0xFF;

		/* plug in and force to FS */
		XBYTE[REG_Usb_EpAckIntEn] |= 0x07;
		
		XBYTE[0x2014] |= 0x02;
		XBYTE[0x2015] |= 0x20;

		XBYTE[REG_Usb_UsbClkEn] = 0x07; // enable usb internal clock
		XBYTE[REG_Usb_Suspend] = 0x00; // wake up PHY
		HAL_GlobalTimerWait(1000);
		printf("@@Force to FS\n");
		XBYTE[REG_Usb_Status] |= 0x02;
		XBYTE[REG_Usb_Status] &= (~0x10); // plug in
	}
}

//-----------------------------------------------------------------------------
//HAL_UsbParaSet
//-----------------------------------------------------------------------------
/**
 * @fn		  UINT8 HAL_UsbParaSet(UINT8 selector, UINT8 value)
 * @brief	  USB related parameter set.
 * @param	  [in] selector
 * @param	  [in] value
 * @retval	  return = SUCCESS / FAIL
 * @see 	  HAL_UsbParaSet
 * @author	  kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 HAL_UsbParaSet(
	UINT8 selector,
	UINT8 value
)
{
	if ( selector < USB_PARA_MAX_ID ) {
		switch(selector){
			case USB_PARA_SINGLE_LUN_SHOW_NAND:
				G_UsbSlunShowNand = value;
				break;
			default:
				break;
		}
		return SUCCESS;
	}
	else {
		return FAIL;
	}
}

/**
 * @fn        UINT8 HAL_UsbBulkFlashAddrSet(UINT32 dramaddr)
 * @brief     HAL_UsbBulkFlashAddrSet
 * @param     [in] dramaddr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_UsbBulkFlashAddrSet(
	UINT32 flashtype,
	UINT32 flashaddr
)
{
	S_BulkFlashtype = flashtype;
	if ( S_BulkFlashtype == 0 ) { /*DRAM*/
		S_BulkDRAMAddrWord = flashaddr;
	}
	else if ( S_BulkFlashtype == 1 ) { /*SRAM*/
		S_BulkSRAMAddrByte = flashaddr;
	}
	else {
		return FAIL;
	}

	return SUCCESS;
		
}
/**
 * @fn        UINT32 HAL_UsbDramAddrGet(void)
 * @brief     HAL_UsbDramAddrGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT32 HAL_UsbBulkFlashAddrGet(UINT32 flashtype)
{
	S_BulkFlashtype = flashtype;
	if ( S_BulkFlashtype == 0 ) { /*DRAM*/
		return S_BulkDRAMAddrWord;
	}
	else if ( S_BulkFlashtype == 1 ) { /*DRAM*/
		return S_BulkSRAMAddrByte;
	}
}

/**
 * @fn        void HAL_UsbBulkAutoModeSet(UINT8 mode)
 * @brief     set bulk auto mode
 * @param     [in] mode
 *				1: auto mode
 *				0: manual mode
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
void HAL_UsbBulkAutoModeSet(UINT8 mode)
{
	if( mode == 0x01)
	{
		XBYTE[REG_Usb_BulkAutoMode] |= 0x18;	/* auto mode */
	}
	else
	{
		XBYTE[REG_Usb_BulkAutoMode] &= 0xE7; 	/* manual mode */
	}

}

void
HAL_UsbEP7IntrDataTx(
	UINT8 *pdataAddr,
	UINT16 dataSize
)

{
	UINT32 eachSize, i;
	UINT8 *ptr;
	volatile UINT8 ack;
	UINT32 sys;


	/* Disable Bulk-In ACK Intr */
	XBYTE[REG_Usb_EpAckIntEn] |= USB_IDX_EP7_IN; //pusbReg->ep0SetupAckEn &= ~USB_IDX_EP7_IN;
	ptr = pdataAddr;
	while ((dataSize) > 0) {
		/* Calculate each size */
		eachSize = dataSize;
		if (eachSize > 16) {
			eachSize = 16;
		}

		/* Clear Intr-In buffer */
		XBYTE[REG_Usb_BufClr] = USB_IDX_EP7_IN;//pusbReg->ep0BufClr = USB_IDX_EP7_IN;

		/* Clear the interrupt. */
		 XBYTE[REG_Usb_EpAckIntSts] = USB_IDX_EP7_IN;//pusbReg->ep0SetupAck = USB_IDX_EP7_IN;

		/* Fill data */
		for (i = 0; i < eachSize; i++) {
			 XBYTE[REG_Usb_Ep7BufData] = ptr[i];//pusbReg->ep7BufData = ptr[i];
		}

		/* Open Intr-In for data to Host */
		ENTER_CRITICAL(sys);
		 XBYTE[REG_Usb_TransactionEn] |= USB_IDX_EP7_IN; //pusbReg->ep0InEn |= USB_IDX_EP7_IN;
		EXIT_CRITICAL(sys);

		/* Polling for Intr-Ack */
		ack = XBYTE[REG_Usb_EpAckIntSts] & USB_IDX_EP7_IN;
		while ((ack != 0x40)) {
			ack = XBYTE[REG_Usb_EpAckIntSts] & USB_IDX_EP7_IN;		
		}
		ptr += eachSize;
		dataSize -= eachSize;
	}

}


