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
 * @file      File name
 * @brief     Brief
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "dbg_def.h"
#include "hal.h"
#include "hal_sd.h"
#include "gpio.h"
#include "sd_def.h"
#include "sdfunc.h"
#include "dos_dev_support.h"
#include "sd_private.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE		__FILE_ID_SD_533_BGWR__
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if (SD_W_INT_EN || SD_R_INT_EN)
static UINT32 sd_rw_addr, sd_rw_num;
static UINT8 sd_rw_mode=SD_BG_FREE;	/* 1=write, 0=read */
xdata UINT8 G_IE_Value;
UINT8 sd_bg_r=0;
UINT8 sd_bg_w=0;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#if (SD_R_INT_EN || SD_W_INT_EN)
UINT8 sdBgRwStsGet(void)
{
	UINT8 status, mode;
	ENTER_CRITICAL(status);
	if(sD_CdGet()){
		sd_rw_mode = SD_BG_FREE;
		XBYTE[REG_Fm_intEn] &= ~(1 << 2);	//SD controller interrupt disable
		EXIT_CRITICAL(status);
		return sd_rw_mode;
	}
	mode = sd_rw_mode;
	EXIT_CRITICAL(status);
	return mode;
}

void sdBgRwInit(UINT32 addr, UINT32 num, UINT8 mode)
{
	#if 1
	UINT32 rw_bytes;
	UINT16  rw_sectors;

	if(sD_CdGet()){
		return;
	}

	sd_rw_addr = addr;
	sd_rw_num  = num;
	sd_rw_mode = mode;
	//printf("%s %d %lu,%lx\n", __FILE__, (unsigned short)__LINE__,sd_rw_num,sd_rw_addr);
	if(sd_rw_num) {
        if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10){    //xian ++
		XBYTE[REG_Dma_BaseAddrL] = READ8(sd_rw_addr, 3);
		XBYTE[REG_Dma_BaseAddrML] = READ8(sd_rw_addr, 2);
		XBYTE[REG_Dma_BaseAddrMH] = READ8(sd_rw_addr, 1);
		XBYTE[REG_Dma_BaseAddrH] = READ8(sd_rw_addr, 0) & 0x01;
        }
		rw_sectors = (sd_rw_num > G_SD_MultiSecNum) ? G_SD_MultiSecNum : sd_rw_num;
#if 0
		WRITE8(rw_bytes,3,0);
		if(rw_sectors < 128) {	//128*2=256
			WRITE8(rw_bytes,2,rw_sectors<<1);
			WRITE8(rw_bytes,1,0);
		}
		else {
			WRITE8(rw_bytes,2,0);
			WRITE8(rw_bytes,1,1);
		}
		WRITE8(rw_bytes,0,0);

		sd_rw_num  -= (UINT32)rw_sectors;
		sd_rw_addr += (rw_bytes >> 1);	//word address
#else
            WRITE8(rw_bytes,3,0);
            WRITE8(rw_bytes,2,READ8(rw_sectors, 1));			
            WRITE8(rw_bytes,1,READ8(rw_sectors, 0));			
            WRITE8(rw_bytes,0,0);

            sd_rw_num  -= (UINT32)rw_sectors;
            sd_rw_addr += rw_bytes;	//word address
            rw_bytes <<= 1;     // *512
#endif
		//XBYTE[REG_Fm_evt] |= 1 << 2;	//clr int
		XBYTE[REG_Fm_evt] = 1 << 2;	//clr int
		XBYTE[REG_Fm_intEn] |= 1 << 2;	//SD controller interrupt enable
		if(sd_rw_mode==SD_BG_WRITE)	{	//write
                if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10){    //xian ++
                    HAL_DmaDo(DMA_PATH_DRAM,DMA_PATH_FLASH,rw_bytes,1,0);
                }
                else{
                    HAL_SDDram(sd_rw_addr-(rw_bytes>>1), rw_bytes, 0);
                }
			//HAL_Wait(1);
		}
		else if(sd_rw_mode==SD_BG_READ) {	//read
			#if SD_BLOCK_AUTO_DUMMY
			if(sd_rw_num==0)
				XBYTE[REG_Fm_autorsp_dummy_datarspen] |= 0x04;
			#endif
                    if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10){    //xian ++
			    HAL_DmaDo(DMA_PATH_FLASH,DMA_PATH_DRAM,rw_bytes,1,0);
                    }
                    else{
                        HAL_SDDram(sd_rw_addr-(rw_bytes>>1), rw_bytes, 1);
                    }
		}
		//Save the IE value
		G_IE_Value = IE;
		// Force enable interrupt
		if((G_IE_Value & 0x81) != 0x81 ){//EA and EX0 has to be enabled
			//printf("IE=%bx\n",IE);
			__ASSERT(__FILE, __LINE__, 1);
			IE |= 0x81;
			G_IE_Value = IE;
		}
		return;
	}
	sd_rw_mode = SD_BG_DMA_DONE;
	XBYTE[REG_Fm_intEn] &= ~(1 << 2);	//SD controller interrupt disable

	return;
	#else
	UINT8 status;
	sd_rw_addr = addr;
	sd_rw_num  = num;
	sd_rw_mode = mode;
	ENTER_CRITICAL(status);
	sdBgRw();
	EXIT_CRITICAL(status);
	return;
	#endif
}

void INTR_SDBgRw(void)
{
	UINT32 rw_bytes;
	UINT16  rw_sectors;
	//printf("%s %d\n", __FILE__, (unsigned short)__LINE__);
	//XBYTE[REG_Dma_DmaCmp] = 0x01;
	//printf("%s %d %lu, %lx\n", __FILE__, (unsigned short)__LINE__,sd_rw_num,sd_rw_addr);
	if(sd_rw_num) {
		//dumpData(16, sd_rw_addr, 1);
		//HAL_SetDRAMDMA(sd_rw_addr);
        if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10){    //xian ++
		XBYTE[REG_Dma_BaseAddrL] = READ8(sd_rw_addr, 3);
		XBYTE[REG_Dma_BaseAddrML] = READ8(sd_rw_addr, 2);
		XBYTE[REG_Dma_BaseAddrMH] = READ8(sd_rw_addr, 1);
		XBYTE[REG_Dma_BaseAddrH] = READ8(sd_rw_addr, 0) & 0x01;
        }
		rw_sectors = (sd_rw_num > G_SD_MultiSecNum) ? G_SD_MultiSecNum : sd_rw_num;
#if 0 //xian --
		WRITE8(rw_bytes,3,0);
		if(rw_sectors < 128) {	//128*2=256
			WRITE8(rw_bytes,2,rw_sectors<<1);
			WRITE8(rw_bytes,1,0);
		}
		else {
			WRITE8(rw_bytes,2,0);
			WRITE8(rw_bytes,1,1);
		}
		WRITE8(rw_bytes,0,0);

		sd_rw_num  -= (UINT32)rw_sectors;
		//sd_rw_addr += rw_bytes;	//word address
		sd_rw_addr += (rw_bytes >> 1);	//word address
#else
            WRITE8(rw_bytes,3,0);
            WRITE8(rw_bytes,2,READ8(rw_sectors, 1));			
            WRITE8(rw_bytes,1,READ8(rw_sectors, 0));			
            WRITE8(rw_bytes,0,0);

            sd_rw_num  -= (UINT32)rw_sectors;
            sd_rw_addr += rw_bytes;	//word address
            rw_bytes <<= 1;     // *512
#endif            
		//XBYTE[REG_Fm_evt] |= 1 << 2;	//clr int
		XBYTE[REG_Fm_evt] = 1 << 2;	//clr int
		XBYTE[REG_Fm_intEn] |= 1 << 2;	//SD controller interrupt enable
		if(sd_rw_mode==SD_BG_WRITE)	{	//write
			//printf("W:%bu r=%lu a=%08lx b=%x\n", rw_sectors, sd_rw_num, sd_rw_addr, READ16(rw_bytes,2));
                if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10){    //xian ++
                    INTR_HAL_DmaDo(DMA_PATH_DRAM,DMA_PATH_FLASH,rw_bytes,1,0);
                }
                else{
                    INTR_HAL_SDDram(sd_rw_addr-(rw_bytes>>1), rw_bytes, 0);
                }
		}
		else if(sd_rw_mode==SD_BG_READ) {	//read
			//printf("R:%bu r=%lu a=%08lx b=%x\n", rw_sectors, sd_rw_num, sd_rw_addr, READ16(rw_bytes,2));
			#if SD_BLOCK_AUTO_DUMMY
			//if(sd_rw_num==rw_sectors)
			if(sd_rw_num==0)
				XBYTE[REG_Fm_autorsp_dummy_datarspen] |= 0x04;
			#endif
                if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10){    //xian ++
			INTR_HAL_DmaDo(DMA_PATH_FLASH,DMA_PATH_DRAM,rw_bytes,1,0);
			//printf("%s %d\n", __FILE__, (unsigned short)__LINE__);
                }
                else{
                    INTR_HAL_SDDram(sd_rw_addr-(rw_bytes>>1), rw_bytes, 1);
                }
		}
		return;
	}
	#if 0
	if(sd_rw_mode==SD_BG_WRITE) {
		HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);		// TX "CMD12" to stop write
		sdWaitDat0High();
	}
	#endif

	#if SD_BLOCK_AUTO_DUMMY
	XBYTE[REG_Fm_autorsp_dummy_datarspen] &= ~0x04;
	#endif

	sd_rw_mode = SD_BG_DMA_DONE;
	XBYTE[REG_Fm_intEn] &= ~(1 << 2);	//SD controller interrupt disable
	//return original IE value
	IE = G_IE_Value;
	//printf("<BE>");
	return;
}

UINT8 sdBgWriteEn(UINT8 en)
{
	UINT8 ret;
	ret = sd_bg_w;
	if(en != -1)  sd_bg_w = en;
	return	ret;
}

UINT8 sdBgReadEn(UINT8 en)
{
	UINT8 ret;
	ret = sd_bg_r;
	if(en != -1)  sd_bg_r = en;
	return	ret;
}

void sdIntEnable(void)
{
	if(sd_rw_mode != SD_BG_FREE)
		XBYTE[REG_Fm_intEn] |= 1 << 2;	//SD controller interrupt enable
}

void sdIntDisable(void)
{
	XBYTE[REG_Fm_intEn] &= ~(1 << 2);	//SD controller interrupt disable
}

void sdBgWriteEnd(void)
{
	UINT8 ret;
	ret = HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);		// TX "CMD12" to stop write
	if(ret==0)
		sdWaitDat0High();
	
	#if SD_W_INT_EN
	sd_rw_mode = SD_BG_FREE;
	#endif
}
void sdBgReadEnd(void)
{
	#if	SD_BLOCK_AUTO_DUMMY
	XBYTE[REG_Fm_autorsp_dummy_datarspen] &= ~0x04;
	#else
	HAL_SDTxDummy();
	#endif
	HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);		// TX "CMD12" to stop write
	#if SD_R_INT_EN
	sd_rw_mode = SD_BG_FREE;
	#endif
}

#if (SD_R_BG_EN || SD_W_BG_EN)
void sdBgCheckRdy(void)
{
	UINT32 retry;

	if( sdBgRwStsGet() == SD_BG_FREE ) return;

	retry = 0;
	while(( sdBgRwStsGet() == SD_BG_WRITE ) || ( sdBgRwStsGet() == SD_BG_READ )) {
		//printf("sdBgRwStsGet()=%bx\n",sdBgRwStsGet());
		//if(sD_CdGet() & G_Detect_PinMux)	return;
		G_IE_Value = IE;
		// Force enable interrupt
		if((G_IE_Value & 0x81) != 0x81 ){//EA and EX0 has to be enabled
			//printf("IE=%bx\n",IE);
			__ASSERT(__FILE, __LINE__, 1);
			IE |= 0x81;
			G_IE_Value = IE;
		}
		retry ++;
		if((retry & 0xFFFF)==0){
			__ASSERT(__FILE, __LINE__, 1);
		}
	}
	if( sdBgRwStsGet() == SD_BG_DMA_DONE ){
		if( sdBgWriteEn(-1) ){
			//set stop cmd here
			sdIntDisable();
			sdBgWriteEn(0);
			sdBgWriteEnd();
		}
		else if( sdBgReadEn(-1) ){
			//set stop cmd here
			sdIntDisable();
			//sdBgReadEn(0);
			sdBgReadEnd();
		}
		else{
			sdBgWriteEn(0);
			//sdBgReadEn(0);
		}
	}
}
#endif

UINT8 sdBgRwFreeStsWait(void)
{
	UINT32 retry = 0;

	while(sdBgRwStsGet() != SD_BG_FREE) {
		if(sD_CdGet()){
			return FALSE;
		}
		retry ++;
		if((retry & 0xFFFF)==0){
			//printf("R.wait(%02bX:%02bX:%02bX)-", XBYTE[REG_Fm_evt], XBYTE[REG_Fm_intEn], XBYTE[REG_Fm_SD_Datastate_CRDSts]
			//__ASSERT(__FILE, __LINE__, 1);
			return FALSE;
		}
	}
	return TRUE;
}
#else
void INTR_SDBgRw(void)
{
}
#endif

#if (SD_W_INT_EN)
UINT8 sdSectorW(UINT32 start, UINT32 num, UINT32 SrcAddr)
{
	UINT8 ret, tmp = 0, Src;//, SrcPath;
	UINT32 addr;
	UINT32 wrt_num;
	//UINT8 status;
	
	G_SD_UnderWrite = 1;
	if(sdBgRwFreeStsWait() == FALSE){
		__ASSERT(__FILE, __LINE__, 1);
	}

	addr = start;
#if 1//def DOS32_SUPPORT
	if(sdByteMode)
	{
		start <<= 1;
		WRITE8(addr,0,READ8(start,1));
		WRITE8(addr,1,READ8(start,2));
		WRITE8(addr,2,READ8(start,3));
		WRITE8(addr,3,0);
		#if	SD_BLOCK_W_DEBUG
		printf("ByteW.%d", (UINT16)__LINE__);
		#endif
	}
	else	/* block addressing mode */
	{
		#if	SD_BLOCK_W_DEBUG
		printf("BlockW.%d", (UINT16)__LINE__);
		#endif
	}
#else
	if(sdByteMode) {
		#if	SD_BLOCK_W_DEBUG
		printf("F_ByteW.%d", (UINT16)__LINE__);
		#endif
	}
	else{
		start >>= 1;
		WRITE8(addr,0,0);
		WRITE8(addr,1,READ8(start,0));
		WRITE8(addr,2,READ8(start,1));
		WRITE8(addr,3,READ8(start,2));
	}
#endif
	ret = HAL_SDTxCommand(25, addr, RSP_TYPE_R1, RspBuf);
	#if	SD_BLOCK_W_DEBUG
	printf(" s=%08lx n=%ld p=%lx ret=%bx\n", addr, num, SrcAddr, ret);
	#endif
	if(ret != 0)
	{
		//printf("%s.%d W_ERR\n", __FILE__, __LINE__);
		__ASSERT(__FILE, __LINE__, 1);
		return ret;
	}

#if 1//SD_W_INT_EN
	Src = DMA_PATH_DRAM;
	if( SrcAddr == 0 ){
		Src = DMA_PATH_USB;
		SrcAddr = 0x200000;
		#if SD_W_BG_EN
		sdBgWriteEn(0);
		#endif
	}
	if( Src != DMA_PATH_USB ){
		#if SD_W_BG_EN
		sdBgWriteEn(1);
		#endif
		sdBgRwInit(SrcAddr,num,SD_BG_WRITE);
		if(sd_bg_w) {
			//printf("BG Write\n");
			return 0;
		}

		while(sdBgRwStsGet() != SD_BG_DMA_DONE) {
			if(sD_CdGet()){
				return 0;
			}
			//printf("W.wait...%bu", sdBgRwStsGet());
			if((num & 0xFFFF)==0){
				//printf("W.wait...%bu", sdBgRwStsGet());
				__ASSERT(__FILE, __LINE__, 1);
			}
			num++;
		};
	}
	else{
		while(num) {
			HAL_DmaSetDramAddr(SrcAddr);
			wrt_num = (num > G_SD_MultiSecNum) ? G_SD_MultiSecNum : num;
			//XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
			XBYTE[REG_Fm_evt] = 1<<2;	//clr int
			{
				UINT32 bi;

				ret = HAL_DmaDo(Src,DMA_PATH_FLASH,wrt_num<<9,1,0);

				if (ret != 0)
				{
					__ASSERT(__FILE, __LINE__, 1);
					return 5;
				}
				#if 1
				ret = L1_SDUsb2CardDmaWait(wrt_num);
				if(ret == 1){
					return 1;
				}
				#else
				if (G_bUSBSpeed == 0){
					for	(bi	= 0; bi	< (UINT32)(wrt_num<<3); bi++)
					{
					while ((((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) & 0xff00)) > 0)	{  //wait for 0	in bulk	buffer
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
						while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
					}
				}
				else {
					for	(bi	= 0; bi	< (wrt_num); bi++)
					{
						while ((((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) & 0xff00)) > 0)	{  //wait for 0	in bulk	buffer
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_BufClr] =	K_USB_CLASS_OUT_OPEN;
						XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
						while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
					}
					while(XBYTE[REG_Dma_DmaCmp] != 0x01)
					{
						if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
					}
				}
				#endif
			}
			while(!sD_CdGet()) {
				if(XBYTE[REG_Fm_evt] & (1<<2))
					break;
				#if	1
				tmp++;
				if(tmp==0)	printf(".");
				#endif
			}
			#if	SD_BLOCK_W_DEBUG
			printf("W[2471]=%bu [2447]=%bu", XBYTE[REG_Fm_mbblkcnt_Low],XBYTE[REG_Fm_autorsp_dummy_datarspen]);
			#endif
			//XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
			XBYTE[REG_Fm_evt] = 1<<2;	//clr int
			num -= wrt_num;
			SrcAddr += wrt_num << (9-1);	//word address
		}
	}
#else
	Src = DMA_PATH_DRAM;
	if( SrcAddr == 0 ){
		Src = DMA_PATH_USB;
		SrcAddr = 0x200000;
	}
    while(num) {
        if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10)    //xian ++
            HAL_DmaSetDramAddr(SrcAddr);
        wrt_num = (num > G_SD_MultiSecNum) ? G_SD_MultiSecNum : num;
        //XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
        XBYTE[REG_Fm_evt] = 1<<2;	//clr int
        if (Src != DMA_PATH_USB)
        {
            if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10)    //xian ++
                ret = HAL_DmaDo(Src,DMA_PATH_FLASH,wrt_num<<9,0,0);
            else 
                ret = HAL_SDDram(SrcAddr, wrt_num<<9, 0);
        }
        else
        {
#if DOS_DEV_SUPPORT_USB			
				UINT32 bi;

				ret = HAL_DmaDo(Src,DMA_PATH_FLASH,wrt_num<<9,1,0);

				if (ret != 0)
				{
					__ASSERT(__FILE, __LINE__, 1);
					return 5;
				}
				if (G_bUSBSpeed == 0){
					for	(bi	= 0; bi	< (UINT32)(wrt_num<<3); bi++)
					{
						while ((((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) & 0xff00)) > 0)	{  //wait for 0	in bulk	buffer
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
						while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
					}
				}
				else {
					for	(bi	= 0; bi	< (wrt_num); bi++)
					{
						while ((((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) & 0xff00)) > 0)	{  //wait for 0	in bulk	buffer
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_BufClr] =	K_USB_CLASS_OUT_OPEN;
						if (G_USBReset == 0x00){
							XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
						}
						while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
							if (G_USBReset == 0x01){
								XBYTE[REG_Usb_BufClr] = K_USB_CLASS_OUT_OPEN;
								/* exit while loop */
								break;
							}
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
						/* exit for loop */
						if (G_USBReset == 0x01)	break;

					}
					while(XBYTE[REG_Dma_DmaCmp] != 0x01)
					{
						if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						if (G_USBReset == 0x01){
							XBYTE[REG_Dma_DmaIdle] = 0x01;
							XBYTE[REG_Dma_DmaIdle] = 0x00;
							/* exit while loop */
							break;
						}

					}
					/* exit while loop */
					if (G_USBReset == 0x01)	break;

				}
#endif //DOS_DEV_SUPPORT_USB
			}


		while(!sD_CdGet()) {
			if(XBYTE[REG_Fm_evt] & (1<<2))
				break;
			if (G_USBReset == 0x01)
				/* exit while loop */
				break;
			#if	1
			tmp++;
			if(tmp==0)	printf(".");
			#endif
		}
		//printf("W[2471]=%bu [2447]=%bu", XBYTE[REG_Fm_mbblkcnt_Low],XBYTE[REG_Fm_autorsp_dummy_datarspen]);
		#if	SD_BLOCK_W_DEBUG
		{
		    UINT16 cnt;

		    WRITE8(cnt, 0, XBYTE[REG_Fm_mbblkcnt_High]);
		    WRITE8(cnt, 1, XBYTE[REG_Fm_mbblkcnt_Low]);

//		    printf("mt cnt: 0x%x\n", cnt);

		    if(cnt > 64)
		        printf("%bx,%bx\n", XBYTE[REG_Fm_mbblkcnt_High], XBYTE[REG_Fm_mbblkcnt_Low]);
		}
		printf("W[2471]=%bu [2447]=%bu", XBYTE[REG_Fm_mbblkcnt_Low],XBYTE[REG_Fm_autorsp_dummy_datarspen]);
		#endif
		//XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
		XBYTE[REG_Fm_evt] = 1<<2;	//clr int
		num -= wrt_num;
		SrcAddr += wrt_num << (9-1);	//word address
		if (G_USBReset == 0x01)
			break;
	}
	#if	SD_BLOCK_W_DEBUG
	printf("sdSectorW %d <END>\n",(unsigned short)__LINE__);
	#endif
#endif
	ret = HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);		// TX "CMD12" to stop write
	if(ret==0)
		sdWaitDat0High();
	if(SD_MODE_POST_IDLE_GET()){
		HAL_SDTxDummy();
	}
	//HAL_Wait(50);
#if 1//SD_W_INT_EN
	sd_rw_mode = SD_BG_FREE;
#endif
	return ret;
}
#endif

#if (SD_R_INT_EN)
UINT8 sdSectorR(UINT32 start, UINT32 num, UINT32 TarAddr)
{
	UINT8 ret = 0, dst;//,tmp, TarPath;
	UINT32 addr;
	UINT32 rd_num;

	G_SD_UnderWrite = 0;
	if(sdBgRwFreeStsWait() == FALSE){
		__ASSERT(__FILE, __LINE__, 1);
	}
	
	addr = start;
#if 1//def DOS32_SUPPORT
	if(sdByteMode)
	{
		start <<= 1;
		WRITE8(addr,0,READ8(start,1));
		WRITE8(addr,1,READ8(start,2));
		WRITE8(addr,2,READ8(start,3));
		WRITE8(addr,3,0);// key point
		#if	SD_BLOCK_R_DEBUG
		printf("ByteR.%u", (UINT16)__LINE__);
		#endif
	}
	else	/* block addressing mode */
	{
		#if	SD_BLOCK_R_DEBUG
		printf("BlockR.%u", (UINT16)__LINE__);
		#endif
	}
#else
	if(sdByteMode) {
		#if	SD_BLOCK_R_DEBUG
		printf("F_ByteR.%u", (UINT16)__LINE__);
		#endif
	}
	else{
		start >>= 1;
		WRITE8(addr,0,0);
		WRITE8(addr,1,READ8(start,0));
		WRITE8(addr,2,READ8(start,1));
		WRITE8(addr,3,READ8(start,2));
		#if	SD_BLOCK_R_DEBUG
		printf("ByteR.%u", (UINT16)__LINE__);
		#endif
	}
#endif
	ret = HAL_SDTxCommand(18, addr, RSP_TYPE_R1, NULL);
	#if	SD_BLOCK_R_DEBUG
	printf(" s=%08lx n=%ld p=%lx ret=%bx\n", addr, num, TarAddr, ret);
	#endif
	if(ret != 0){
		//printf("%s %d CMD18(READ) ERR!\n", __FILE__, __LINE__);
		__ASSERT(__FILE, __LINE__, 1);
		return ret;
	}
#if 1//SD_R_INT_EN
	dst = DMA_PATH_DRAM;
	if( TarAddr == 0 ){
		dst = DMA_PATH_USB;
		TarAddr = 0x200000;
		#if SD_R_BG_EN
		sdBgReadEn(0);
		#endif
	}
	if (dst != DMA_PATH_USB){
		//1st time enable
		#if SD_R_BG_EN
		//sdBgReadEn(1);
		#endif
		sdBgRwInit(TarAddr,num,SD_BG_READ);
		if(sd_bg_r) return 0;

		while(sdBgRwStsGet() != SD_BG_DMA_DONE) {
			if(sD_CdGet()){
				break;
			}
			if((num & 0xFFFF)==0){
				//printf("R.wait(%02bX:%02bX:%02bX)-", XBYTE[REG_Fm_evt], XBYTE[REG_Fm_intEn], XBYTE[REG_Fm_SD_Datastate_CRDSts]
				__ASSERT(__FILE, __LINE__, 1);
			}
			num++;
		};
	}
	else{
		while(num) {
			HAL_DmaSetDramAddr(TarAddr);
			rd_num = (num > G_SD_MultiSecNum) ? G_SD_MultiSecNum : num;
			//XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
			XBYTE[REG_Fm_evt] = 1<<2;	//clr int
			#if SD_BLOCK_AUTO_DUMMY
			if(num == rd_num){
				//XBYTE[REG_Fm_autorsp_dummy_datarspen] &= 0xfb;
				XBYTE[REG_Fm_autorsp_dummy_datarspen] |= 0x04;
			}
			#endif
			{
				UINT32 bi;
				ret = HAL_DmaDo(DMA_PATH_FLASH, dst, rd_num<<9, 1,0);
				if (ret != 0)
				{
					__ASSERT(__FILE, __LINE__, 1);
					return 4;
				}
				#if 1
				ret = L1_SDCard2UsbDmaWait(rd_num);
				if(ret == 1){
					return 1;
				}
				#else
					if (G_bUSBSpeed == 0){
						for	(bi	= 0; bi	< (rd_num<<3); bi++)
						{
							while ((((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8) & 0xff00)) < 64)	{  //wait for 0	in bulk	buffer
								//printf("size=%x\n",(UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] + (UINT16)(XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8));
								if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
							}
							XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
							while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
								if(sD_CdGet() & G_Detect_PinMux)	return 1;
							}
							XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
						}

					}
					else {
						for	(bi	= 0; bi	< rd_num; bi++)
						{
							while ((((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8) & 0xff00)) < 512)	{  //wait for 0	in bulk	buffer
								//printf("size=%x\n",(UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] + (UINT16)(XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8));
								if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
							}
							XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
							while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
								if(sD_CdGet() & G_Detect_PinMux)	return 1;
							}
							XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
						}
					}
				#endif
			}
			{
				UINT32 timeout_count = 0xFFFF;
				while(!sD_CdGet()) {
					if(XBYTE[REG_Fm_evt] & (1<<2))
						break;
					if (timeout_count > 0)
					{
						timeout_count--;
						//printf(".");
					}
					else
					{
						ret = 1;
						break;
					}
				}
			}
			//XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
			XBYTE[REG_Fm_evt] = 1<<2;	//clr int
			#if	SD_BLOCK_R_DEBUG
				#if SD_BLOCK_AUTO_DUMMY
					printf("A:[2471]=%bu ", XBYTE[REG_Fm_mbblkcnt_Low]);
				#else
					printf("M:[2471]=%bu ", XBYTE[REG_Fm_mbblkcnt_Low]);
				#endif
			#endif
			ret = HAL_SDWaitIdle();
			if( ret != HALK_SUCCESS ) break;
			#if 1	//check 2400
			if (SD_CRC16_OK==0){
				//printf("%s %d CRC16 ERR, num=%ld\n", __FILE__, (UINT16)__LINE__, num);
				__ASSERT(__FILE, __LINE__, 1);
				ret = 1;
				break;
			}
			#endif
			num -= rd_num;
			TarAddr += rd_num << (9-1);	//WORD ADDRESS
		}
	}
	#else
	//XBYTE[REG_Fm_evt] |= 1<<3;	//clr CRC
	XBYTE[REG_Fm_evt] = 1<<3;	//clr CRC
	dst = DMA_PATH_DRAM;
	if( TarAddr == 0 ){
		dst = DMA_PATH_USB;
		TarAddr = 0x200000;
	}
    while(num) {
        if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10)
            HAL_DmaSetDramAddr(TarAddr);
        
        rd_num = (num > G_SD_MultiSecNum) ? G_SD_MultiSecNum : num;
        //XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
        XBYTE[REG_Fm_evt] = 1<<2;	//clr int
        #if SD_BLOCK_AUTO_DUMMY
        if(num == rd_num){
        	//XBYTE[REG_Fm_autorsp_dummy_datarspen] &= 0xfb;
        	XBYTE[REG_Fm_autorsp_dummy_datarspen] |= 0x04;
        }
        #endif
        if (dst != DMA_PATH_USB){
            if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10)
                ret = HAL_DmaDo(DMA_PATH_FLASH, dst, rd_num<<9, 0,0);
            else
                ret = HAL_SDDram(TarAddr, (rd_num<<9), 1);
	}
	else
	{
#if DOS_DEV_SUPPORT_USB		
			UINT32 bi;
			ret = HAL_DmaDo(DMA_PATH_FLASH, dst, rd_num<<9, 1,0);


				if (ret != 0)
				{
					__ASSERT(__FILE, __LINE__, 1);
					return 4;
				}
				if (G_bUSBSpeed == 0){
					for	(bi	= 0; bi	< (rd_num<<3); bi++)
					{
						//while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64)&& (G_UI_USBConnect	== K_UI_USB_CONNECT)) {	//wait for 64 in bulk buffer
						while ((((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8) & 0xff00)) < 64)	{  //wait for 0	in bulk	buffer
							//printf("size=%x\n",(UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] + (UINT16)(XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8));
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
						while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
							if(sD_CdGet() & G_Detect_PinMux)	return 1;
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
					}

				}
				else {
					for	(bi	= 0; bi	< rd_num; bi++)
					{
						while ((((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8) & 0xff00)) < 512)	{  //wait for 0	in bulk	buffer
							//printf("size=%x\n",(UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] + (UINT16)(XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8));
							if((sD_CdGet() & G_Detect_PinMux) || (G_UI_USBConnect == K_UI_USB_DISCONNECT))	return 1;
						}
						XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
						while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
							if(sD_CdGet() & G_Detect_PinMux)	return 1;
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
					}
				}
#endif //DOS_DEV_SUPPORT_USB        
		}
		{
			UINT32 timeout_count = 0xFFFF;
			while(!sD_CdGet()) {
				if(XBYTE[REG_Fm_evt] & (1<<2))
					break;
				if (timeout_count > 0)
				{
					timeout_count--;
				}
				else
				{
					ret = 1;
					break;
				}
			}
		}
		//XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
		XBYTE[REG_Fm_evt] = 1<<2;	//clr int
		#if	SD_BLOCK_R_DEBUG
			{
			    UINT16 cnt;

			    WRITE8(cnt, 0, XBYTE[REG_Fm_mbblkcnt_High]);
			    WRITE8(cnt, 1, XBYTE[REG_Fm_mbblkcnt_Low]);

			    if(cnt > 64)
			        printf("mt cnt: 0x%x=%bx,%bx\n", cnt,  XBYTE[REG_Fm_mbblkcnt_High], XBYTE[REG_Fm_mbblkcnt_Low]);
			}
			#if SD_BLOCK_AUTO_DUMMY
				printf("A:[2471]=%bu ", XBYTE[REG_Fm_mbblkcnt_Low]);
			#else
				printf("M:[2471]=%bu ", XBYTE[REG_Fm_mbblkcnt_Low]);
			#endif
		#endif
		ret = HAL_SDWaitIdle();
		if( ret != HALK_SUCCESS ) break;
		#if 1	//check 2400
		if (SD_CRC16_OK==0){
			//printf("%s %d CRC16 ERR, num=%ld\n", __FILE__, (UINT16)__LINE__, num);
			__ASSERT(__FILE, __LINE__, 1);
			ret = 1;
			break;
		}
		#endif
		num -= rd_num;
		TarAddr += rd_num << (9-1);	//WORD ADDRESS
	}
	#endif

	#if SD_BLOCK_AUTO_DUMMY
	XBYTE[REG_Fm_autorsp_dummy_datarspen] &= ~0x04;
	#else
	HAL_SDTxDummy();
	#endif
	if( ret == 0 ){
		ret = HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);		// TX "CMD12" to stop write
	}
	else{
		HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);		// TX "CMD12" to stop write
	}
	#if	SD_BLOCK_R_DEBUG
	printf("%s %d<END>\n", __FILE__, (unsigned short)__LINE__);
	#endif
#if 1//SD_R_INT_EN
	sd_rw_mode = SD_BG_FREE;
#endif
	return ret;
}
#endif

