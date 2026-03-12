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
 * @file      msdc_dma.c
 * @brief     Module related to Bulk-Only Transport of USB Mass Storage Device Class
 * @author    Richie li
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE	__FILE_ID_LIB_USB__

#include "general.h"
//#include "sidcOption.h"
#include "usbopt.h"
#include "hal.h"
#include "dbg_def.h"
//richie@si0417
#if ((USBMSDC_OPTION == 2) || USBSIDC_OPTION)
//richie@si0417 mark
//#include "usbmsdc2.h"
#include "msdc_dma.h" 
#include "reg_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/ 
#define MSDC_DMA_TIME_OUT		((UINT32)0x80000)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern  xdata   UINT32   G_ulStorDataIndex; 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
//USBMSDC_HAL_DoDMA
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_HAL_DoDMA(UINT8 Src, Src,UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 FlashPageDMA)
 * @brief     DMA data transfer, the source and the desitination cannot be the same
 * @param     [in] Src: the source of DMA
			     0: DRAM
				 1: CPU 4K SRAM (0x1000 ~ 0x1FFF)
				 2: Storage Media
			     3: Audio
				 4: USB
				 5: DMA data port (0x2300)
 * @param     [in] Dst: the destination of DMA
				 0: DRAM
				 1: CPU 4K SRAM (0x1000 ~ 0x1FFF)
				 2: Storage Media
				 3: Audio
				 4: USB
				 5: DMA data port (0x2300)
 * @param     [in] nByte: the number of byte in the DMA data transfer (tha maximum size is 1024)
 * @param     [in] Mode: 0: the function return after the DMA is completed
						 1: the function return immediately after the DMA is trigger
 * @param     [in] FlashPageDMA
  				 0: normal operation
  				 1: if flash is one of the DMA source/destination, flash accessing is always in the unit of page
  			  	    the insufficent data is stuffing by 0's
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_HAL_DoDMA(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 FlashPageDMA) USING_1
{

	data UINT8 sramflag;	// WWWW
	data UINT8 dmamode;	// WWWW
	UINT32 timeout = MSDC_DMA_TIME_OUT;
    ////PRINT_HAL("        HAL_DmaDo: Enter HAL_DmaDo(Src=%x,Dst=%x,nByte=%x,Mode=%x,FlashPageDMA=%x)\n",(UINT16)Src,(UINT16)Dst,nByte,(UINT16)Mode,(UINT16)FlashPageDMA);

	nByte = nByte - 1;
	dmamode = Mode;

	XBYTE[REG_Dma_DmaSrcDst] = (Dst<<4)|Src;

	XBYTE[REG_Dma_DmaIdle] = 0x01;                                // reset DMA machine
	XBYTE[REG_Dma_DmaIdle] = 0x00;
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	#if 1 /* No such function in 1628 */
	if(FlashPageDMA)   XBYTE[REG_Dma_DmaIdle] |= 0x02;            // padding 0's to flash
	else               XBYTE[REG_Dma_DmaIdle] &= 0xfd;
	#endif

	if((nByte>>10)!=0) return 0x01;                      // DMA size

	#if 0
	XBYTE[REG_Dma_DmaSizeL] = (UINT8)nByte&0xff;
	XBYTE[REG_Dma_DmaSizeM] = (UINT8)(nByte>>8)&0x03;
	#else
	XBYTE[REG_Dma_DmaSizeL] = READ8(nByte, 3); 
	XBYTE[REG_Dma_DmaSizeM] = READ8(nByte, 2);
	XBYTE[REG_Dma_DmaSizeH] = READ8(nByte, 1);
	#endif

	sramflag = 0;
	if(Src==0x01 || Dst==0x01)
	{
		#if 0
		XBYTE[REG_Cpu_MemEn] &= 0x1d;
		XBYTE[REG_Cpu_SramMod]  = 0x02;
		XBYTE[REG_Cpu_MemEn] |= 0x02;
		#endif
		  XBYTE[REG_Cpu_MemEn] |= 0x07;	  	// enable sram
		  XBYTE[REG_Cpu_SramMod]  = 0x12;

		sramflag = 1;
	}

	XBYTE[REG_Dma_DmaStart] = 0x01;                                // trigger DMA

	if(dmamode) {
		return SUCCESS;
	}
	else {															// wait for DMA complete
		do {
			if (XBYTE[REG_Dma_DmaCmp]) {				
				break;
			}
		} while (--timeout);
	}
	XBYTE[REG_Dma_DmaCmp] = 0x01;
	if(sramflag == 1)
	{
	 	#if 0
		XBYTE[REG_Cpu_MemEn] &= 0x1d;
		XBYTE[REG_Cpu_SramMod]  = 0x00;
		XBYTE[REG_Cpu_MemEn] |= 0x02;
		#endif
		XBYTE[REG_Cpu_MemEn]	|= 0x07;
		XBYTE[REG_Cpu_SramMod]  = 0x00;    	// set sram as normal operation 	
	}
	if(timeout==0){							// check if timeout 
		ASSERT(0, 1);
		XBYTE[REG_Dma_DmaIdle] = 0x01;  // reset DMA machine
		XBYTE[REG_Dma_DmaIdle] = 0x00;
		return FAIL;
	}
    ////PRINT_HAL("        HAL_DmaDo: Exit HAL_DmaDo\n");
	return SUCCESS;
}

#ifndef _DBG_CLR_WARNING_
//-----------------------------------------------------------------------------
//USBMSDC_SDRAM_Dram2Usb
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_Dram2Usb(UINT32 DramAddr, DramAddr,UINT16 size)
 * @brief     USBMSDC_Dram2Usb
 * @param     [in] DramAddr
 * @param     [in] size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_Dram2Usb(UINT32 DramAddr,UINT16 size) USING_1
{
	UINT32 temp = DramAddr;
	USBMSDC_HAL_SetDRAMDMA(DramAddr);
	//USBMSDC_HAL_SetUSBDMA(K_DMA_UsbDontCare,K_USB_CLASS_DMA_IN);//for class
	USBMSDC_HAL_DoDMA(DMA_PATH_DRAM,DMA_PATH_USB,size,1,0); 		          
	//DMA from CF to sdram for "size" bytes and wait till end and no nandf
	return TRUE;
}

//-----------------------------------------------------------------------------
//USBMSDC_Usb2Dram
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_Usb2Dram(UINT32 DramAddr, DramAddr,UINT16 size)
 * @brief     USBMSDC_Usb2Dram
 * @param     [in] DramAddr
 * @param     [in] size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_Usb2Dram(UINT32 DramAddr,UINT16 size) USING_1
{
	USBMSDC_HAL_SetDRAMDMA(DramAddr);
	//USBMSDC_HAL_SetUSBDMA(K_USB_CLASS_DMA_OUT,K_DMA_UsbDontCare);//for class
	USBMSDC_HAL_DoDMA(DMA_PATH_USB,DMA_PATH_DRAM,size,1,0); 		          //DMA from CF to sdram for "size" bytes and wait till end and no nandf
	return SUCCESS;
}
#endif /*_DBG_CLR_WARNING_*/
//-----------------------------------------------------------------------------
//USBMSDC_Sram2Usb
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_Sram2Usb(UINT16 size)
 * @brief     USBMSDC_Sram2Usb
 * @param     [in] size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_Sram2Usb(UINT16 size) USING_1
{
	if (size <= 1024)
	{
		//USBMSDC_HAL_SRAMLow2High(size,sramAddr);
		//patch4.3@richie@res0530
		USBMSDC_HAL_SetSRAMDMA(0x0000 + G_ulStorDataIndex);
		//USBMSDC_HAL_SetUSBDMA(K_DMA_UsbDontCare,K_USB_CLASS_DMA_IN);//for class
		USBMSDC_HAL_DoDMA(DMA_PATH_SRAM,DMA_PATH_USB,size,0,0);
		USBMSDC_HAL_Set4KSRAMMode(0x00);  
		return TRUE;          
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------------------------------------
//USBMSDC_CF_LUsb2Sram
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_Usb2Sram(UINT8* sramAddr, UINT16 size)
 * @brief     USBMSDC_Usb2Sram
 * @param     [in] sramAddr
 * @param     [in] size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_Usb2Sram(UINT8* sramAddr, UINT16 size) USING_1
{
	if (size <= 1024)
	{
		USBMSDC_HAL_SetSRAMDMA(0x0000);
		//USBMSDC_HAL_SetUSBDMA(K_USB_CLASS_DMA_OUT,K_DMA_UsbDontCare);//for class
		USBMSDC_HAL_DoDMA(DMA_PATH_USB,DMA_PATH_SRAM,size,0,0);
		USBMSDC_HAL_SRAMHigh2Low(size,sramAddr);
		//richie@0117
		USBMSDC_HAL_Set4KSRAMMode(0x00);
  
		return TRUE;          
	}
	else
	{
		return FALSE;
	}
}

//-----------------------------------------------------------------------------
//USBMSDC_HAL_Set4KSRAMMode
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_HAL_Set4KSRAMMode(UINT8 Mode)
 * @brief     high bank 4k sram mode selection
 * @param     [in] Mode: 0: addressible SRAM (0x1000 - 0x1FFF)
			 			 1: access data port (0x2C10) (0x0000 - 0x0FFF)
						 2: used for DMA buffer (0x0000 - 0x0FFF)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_HAL_Set4KSRAMMode(UINT8 Mode) USING_1
{
	UINT8 tmp0;
	tmp0 = XBYTE[REG_Cpu_MemEn];
	XBYTE[REG_Cpu_MemEn] = tmp0 & 0x1d;

	if(Mode == 0)        XBYTE[REG_Cpu_SramMod] = 0x00;                    
	else if(Mode == 1)   XBYTE[REG_Cpu_SramMod] = 0x11;
	else if(Mode == 2)   XBYTE[REG_Cpu_SramMod] = 0x12;
	else                {XBYTE[REG_Cpu_MemEn] = tmp0 | 0x02; return 0x01;}

	XBYTE[REG_Cpu_MemEn] = tmp0 | 0x02;

	return SUCCESS;  
}

//-----------------------------------------------------------------------------
//USBMSDC_HAL_SRAMHigh2Low
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_HAL_SRAMHigh2Low(UINT16 siz, siz,UINT8* buffer)
 * @brief     USBMSDC_HAL_SRAMHigh2Low
 * @param     [in] siz
 * @param     [in] buffer
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_HAL_SRAMHigh2Low(UINT16 siz,UINT8* buffer) USING_1
{
	UINT16 i;

	XBYTE[REG_Cpu_SramMod] &= 0xFC;   //High bank sram can is addressed from 0x1000 to 0x1fff

	for (i = 0; i < siz; i++)
	{
		buffer[i] = XBYTE[0x3800 + i];
	}

	return SUCCESS;
}

#ifndef _DBG_CLR_WARNING_
//-----------------------------------------------------------------------------
//USBMSDC_HAL_SRAMLow2High
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_HAL_SRAMLow2High(UINT16 siz, siz,UINT8* buffer)
 * @brief     USBMSDC_HAL_SRAMLow2High
 * @param     [in] siz
 * @param     [in] buffer
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_HAL_SRAMLow2High(UINT16 siz,UINT8* buffer) USING_1
{
	UINT16 i;

	XBYTE[REG_Cpu_SramMod] &= 0xFC;   //High bank sram can is addressed from 0x1000 to 0x1fff

	for (i = 0; i < siz; i++)
	{
		XBYTE[0x3800 + i] = buffer[i];
	}

	return SUCCESS;
}
#endif /*_DBG_CLR_WARNING_*/
//-----------------------------------------------------------------------------
//USBMSDC_HAL_SetSRAMDMA
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_HAL_SetSRAMDMA(UINT16 SRAMAddr)
 * @brief     SRAM DMA setting
 * @param     [in] SRAMAddr: 4K SRAM buffer, starting address (0x0000 - 0x0FFF)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_HAL_SetSRAMDMA(UINT16 SRAMAddr) USING_1
{
	if((SRAMAddr & 0xF000)!=0) return 0x01 ;
	XBYTE[REG_Cpu_SramRstIdxL] = (UINT8)SRAMAddr&0xFF;
	XBYTE[REG_Cpu_SramRstIdxH] = (UINT8)(SRAMAddr>>8)&0x0F;    
	#if 0
	XBYTE[REG_Cpu_SramMod] |= 0x04;
	XBYTE[REG_Cpu_SramMod] &= 0xFB;
	#endif
	return SUCCESS;
}


//-----------------------------------------------------------------------------
//USBMSDC_HAL_SetUSBDMA
//-----------------------------------------------------------------------------
/* 
routine description:
  USB DMA setting
arguments:
  USBDMASrc: 
          0: endpoint 3
		  1: endpoint 8
  USBDMADst:
          0: endpoint 2
		  1: endpoint 4
		  2: endpoint 7
		  3: endpoint 9

return value:
  0x00	 - success
  others - error

*/

/*
UINT8 USBMSDC_HAL_SetUSBDMA(UINT8 USBDMASrc, UINT8 USBDMADst) USING_1
{
	XBYTE[REG_Usb_BISTMode] = USBDMASrc;
	XBYTE[REG_Usb_SramBIST] = USBDMADst;

	return HALK_SUCCESS;
}
*/

//-----------------------------------------------------------------------------
//USBMSDC_HAL_SetDRAMDMA
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn        UINT8 USBMSDC_HAL_SetDRAMDMA(UINT32 DRAMAddr)
 * @brief     DRAM DMA setting
 * @param     [in] DRAMAddr: DRAM starting address
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_HAL_SetDRAMDMA(UINT32 DRAMAddr) USING_1
{
	XBYTE[REG_Dram_SdramType] = (UINT8)(DRAMAddr&0x000000ff);
	XBYTE[REG_Dram_SdckMode] = (UINT8)((DRAMAddr>>8)&0x000000ff);
	XBYTE[0x2752] = (UINT8)((DRAMAddr>>16)&0x000000ff);

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//USBMSDC_HAL_ReadDRAMDMAAdr
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_HAL_ReadDRAMDMAAdr(UINT32* DRAMAddr)
 * @brief     Read DRAM DMA address
 * @param     [in] DRAMAddr: DRAM DMA starting address
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_HAL_ReadDRAMDMAAdr(UINT32* DRAMAddr) USING_1
{
	*DRAMAddr = (UINT32)XBYTE[REG_Dram_SdramType];
	*DRAMAddr |= (UINT32)XBYTE[REG_Dram_SdckMode]<<8;
	*DRAMAddr |= (UINT32)XBYTE[0x2752]<<16;
}
#endif /*_DBG_CLR_WARNING_*/
#endif
//end if USBMSDC_OPTION
