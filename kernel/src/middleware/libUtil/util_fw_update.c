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
 * @file      util_fw_update.c
 * @brief     Brief
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "hal.h"
#include "hal_global.h"
#include "hal_disp.h"
#include "hal_gprm.h"
#include "hal_dram.h"
#include "osd_mem.h"
#include "initio.h"
#include "impt.h"
#include "gpio.h"
#include "dos32.h"
#include "utility.h"
#include "smcfunc.h"
#include "dbg_file_list.h"
#include "osd_api.h"
#include "Sp1k_util_api.h"
#include "dos32.h"
#include "Dbg_wdt.h"
#include "Sp1k_rsvblk_api.h"
#include "Sp1k_gpio_api.h"
#include "reg_def.h"
#include "dbg_def.h"
#include "Sp1k_hal_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	CARD_MOUNT_ISP	1
#define __FILE	__FILE_ID_FW_UPDATE__
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define CARD_FW_UPD_FMT_TAG  "ICATCH" 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
xdata UINT32 G_ISP_Addr = 0;
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern	xdata	UINT32	 G_BulkDRAMAddr;
extern	xdata	UINT32	 G_BulkFlashType;
extern	xdata	UINT32	 G_BulkSRAMAddr;
extern	xdata	UINT32	G_LastBulkSize;

extern xdata UINT8    G_Firmware_Update;
extern void bootCore(UINT32 srcAddrWord, UINT32 dstAddrWord, UINT32 bootSizeByte);

static UINT8 cardFirmwareChkFmt(UINT8 *pInfo);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#if CARD_MOUNT_ISP
static UINT32
fwUpdateAddrGet(
	void
)
{
	UINT32 rsvA,rsvB, rsvSize, tmp;
	UINT32 *pInfo = &G_ucStorData[16];
	
	tmp = 0;
	rsvA = pInfo[0];
	tmp += (rsvA * 10);
	
	rsvB = pInfo[1];
	tmp += (rsvB * 10);
	tmp += 10;// FW ID
	
	rsvSize = pInfo[2];
	tmp += rsvSize;//rsvA size
	
	rsvSize = pInfo[3];
	tmp += rsvSize;//rsvB size
	tmp += 32;//Hdr size
	
	tmp = (tmp + 1) & 0x01;
	
	return tmp;
}

static void
fwUpdateAdcGet(
	UINT8 index,
	UINT16* value
)
{
	UINT8 i = 0;
	
	ET0 = 0;
	XBYTE[REG_Audio_SarChnlSel] = index;
	HAL_GlobalTimerWait(1);//delay several cycle
	XBYTE[REG_Audio_SnapSarAdcVal0] = 0x01;
	*((UINT8*)value + 1) = XBYTE[REG_Audio_SnapSarAdcVal0];
	*((UINT8*)value + 0) = (XBYTE[REG_Audio_SnapSarAdcVal1]^0x02);
	ET0 = 1;
}

static UINT8
fwUpdateConfirmOsd(
	UINT32 addr
)
{
	/* disable image layers. */
	UINT8 OsdMaxX, OsdMaxY;
	UINT32 i, j;
	UINT16 value0, value1;
	//UINT8 value;
	osdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	sp1kHalCtrlScreen(2);
	osdClear(0);
	osdStrDraw(0,0,"Do ISP ???", (3<<4)+0);
	osdStrDraw(0,1,"Press any key", (1<<4)+0);
	osdStrDraw(0,2,"to do ISP !!", (1<<4)+0);
	HAL_GlobalTimerWait(500);
	i = 0;
	j = 0;
	#if 1//if update FW is controled by pressing key ,open this option
	while(i < 0x50)
	{
		fwUpdateAdcGet(1, &value0);
		fwUpdateAdcGet(2, &value1);
		if(((0x70 < value0) && (value0 < 0x370)) || ((0x70 < value1) && (value1 < 0x370))) 
		{
			i ++;
			continue;
		}
		i = 0;
		HAL_GlobalTimerWait(1);
		j ++;
		if( j > 2000 ) 
		{
			osdClear(0);
			sp1kHalCtrlScreen(3);
			return FALSE;
		}
		
	}
	#endif
	osdClear(0);
	osdStrDraw(OsdMaxX/2 - 2,OsdMaxY/2,"ISP...", (3<<4)+0);
	addr = addr;
	return TRUE;
}

void
fwUpdateOkOsd(
	UINT32 addr
)
{
	UINT8 OsdMaxX, OsdMaxY;
	osdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	osdStrDraw(OsdMaxX/2 - 2,OsdMaxY/2,"OK !!!", (1<<4)+0);
	addr = addr;
}
#endif

/**
 * @fn        void sp1kFwUpdateAddrSet(UINT32 addr)
 * @brief     Set the dram address for f/w update
 * @param     [in] addr : dram word address
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kFwUpdateAddrSet(
	UINT32 addr
)
{
	G_ISP_Addr = addr;
}

/**
 * @fn        UINT8 sp1kCardFirmwareUpdate(UINT8 *name, UINT32 addr)
 * @brief     card firmware update process
 * @param     [in] name :the target file name to update
 * @param     [in] addr :the dram word address for firmware update
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kCardFirmwareUpdate(
	UINT8 *name,
	UINT32 addr
)
{
	UINT8 sts = SUCCESS, status, fd;
	UINT32 size, temp;
	UINT32 startAddr = K_SDRAM_CommonEndAddr;
	UINT8 OsdMaxX, OsdMaxY;
	UINT8 buf[3];
	UINT32 bootSrc, bootSize;
	osdMem_t* pMem;
	osdMemCbGet(&pMem);

	if( addr != 0 ){
		startAddr = addr;
	}

	// skip CDSP write to DRAM
	buf[0] = XBYTE[REG_Front_Finten_low];
	buf[1] = XBYTE[REG_Cdsp_Cdspinten_lo];
	buf[2] = XBYTE[REG_Cdsp_pSkipwr];
	XBYTE[REG_Front_Finten_low] = 0x00;
	XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	//XBYTE[REG_Cdsp_pSkipwr] = 0x07;
	HAL_CdspVDramSkipWrSyncSet(0);
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0x0f);
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);

	ENTER_CRITICAL(status);
	printf("FW %s, %lu KB\n",name, ((K_SDRAM_TotalSize - startAddr) << 1) >> 10);
	if( FALSE == dramFileLoad(name, startAddr, &size)){
		dosRwErr(__FILE,(unsigned short)__LINE__);
		EXIT_CRITICAL(status);
		XBYTE[REG_Front_Finten_low] = buf[0];
		XBYTE[REG_Cdsp_Cdspinten_lo] = buf[1];
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		//XBYTE[REG_Cdsp_pSkipwr] = regBuf[2];
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0x0f);
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,buf[2]);
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		HAL_CdspVDramSkipWrSyncSet(((buf[2]&0x10)>>4));
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		return FAIL;
	}
	if( size > ((K_SDRAM_TotalSize - startAddr) << 1)){
		EXIT_CRITICAL(status);
		dosRwErr(__FILE,__LINE__);
		XBYTE[REG_Front_Finten_low] = buf[0];
		XBYTE[REG_Cdsp_Cdspinten_lo] = buf[1];
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		//XBYTE[REG_Cdsp_pSkipwr] = regBuf[2];
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0x0f);
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,buf[2]);
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		HAL_CdspVDramSkipWrSyncSet(((buf[2]&0x10)>>4));
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		return FAIL;
	}
	//printf("size=%lx\n",size);
	/* Read 1st 32 bytes for BRN format check */
	SDRAM_Dram2Sram( startAddr, G_ucStorData, 512 );
	if( cardFirmwareChkFmt(G_ucStorData) == SUCCESS)//it's 1628.bin
	{
		fd = DOS_FileOpen("ISP.BIN", 'r');
		if(fd != 0) {
			temp = startAddr + ((size + 1) >> 1);
			HAL_GprmDramFill(temp, 1024, 0xFF);
			//This will be used inside isp.bin to set the start wroking address
			//Use these register to save the address
			XBYTE[REG_Cdsp_badbaseaddr_byte0] = READ8(temp, 3);
			XBYTE[REG_Cdsp_badbaseaddr_byte1] = READ8(temp, 2);
			XBYTE[REG_Cdsp_badbaseaddr_byte2] = READ8(temp, 1);
			XBYTE[REG_Cdsp_badbaseaddr_byte3] = READ8(temp, 0);
			SDRAM_CardSetStartAddress(temp, FALSE);
			//Keep 1628.bin size
			M_DRAM_WriteWord(READ8(size, 3), READ8(size, 2));
			M_DRAM_WriteWord(READ8(size, 1), READ8(size, 0));
			//Keep 1628.bin starting addr
			M_DRAM_WriteWord(READ8(startAddr, 3), READ8(startAddr, 2));
			M_DRAM_WriteWord(READ8(startAddr, 1), READ8(startAddr, 0));
			
			//Where to put isp.bin, offset 1024 bytes to put the 1628.bin size, address and isp.bin size
			//This value will be used inside isp.bin
			startAddr += ((size + 1 + 1024) >> 1);
			DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
			DOS_FileRead(fd, startAddr << 1, size, 0);
			DOS_FileClose(fd);
			bootSrc = startAddr;
			bootSize = size;
			//Keep isp.bin size
			SDRAM_CardSetStartAddress(temp + 4, FALSE);
			M_DRAM_WriteWord(READ8(size, 3), READ8(size, 2));
			M_DRAM_WriteWord(READ8(size, 1), READ8(size, 0));
			//if DRAMPARA.TXT exist, load it to Workin address + 256(WORD)
			fd = DOS_FileOpen("DRAMPARA.TXT", 'r');
			if(fd != 0) {
				DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
				//if DRAMPARA.TXT exist, load it to Workin address + 256(WORD)
				DOS_FileRead(fd, (temp + 256) << 1, size, 0);
			}
			else{
				//if no dram para file, load from nand/spi block 0, page 1
				flashLoadDramPara(temp + 256);
				size = 512;
			}
			DOS_FileClose(fd);
			//DRAM PARA size
			SDRAM_CardSetStartAddress(temp + 6, FALSE);
			M_DRAM_WriteWord(READ8(size, 3), READ8(size, 2));
			M_DRAM_WriteWord(READ8(size, 1), READ8(size, 0));
			//LCD window
			osdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
			M_DRAM_WriteWord(OsdMaxX, 0);
			M_DRAM_WriteWord(OsdMaxY, 0);
			//osd display buffer
			M_DRAM_WriteWord((UINT8)K_SDRAM_OsdBufAddr, (UINT8)(K_SDRAM_OsdBufAddr >> 8));
			M_DRAM_WriteWord((UINT8)(K_SDRAM_OsdBufAddr >> 16), (UINT8)(K_SDRAM_OsdBufAddr >> 24));
			//char/font buffer
			M_DRAM_WriteWord(READ8(pMem->char8.addr, 3), READ8(pMem->char8.addr, 2));
			M_DRAM_WriteWord(READ8(pMem->char8.addr, 1), READ8(pMem->char8.addr, 0));

			bootCore(bootSrc, 0, bootSize);
			//printf("open err...\n");
		}
		DOS_FileClose(fd);
		startAddr = startAddr + (fwUpdateAddrGet() >> 1) + 1;
	}
	
	G_Firmware_Update = 0x5a;//to do update
	sp1kDiskRsvWrite(0x1000, startAddr << 1);

	EXIT_CRITICAL(status);
	printf("OK..\n");
	return sts;
}

/**
 * @fn        UINT8 sp1kCardFirmwareBoot(UINT8 *name, UINT32 addr)
 * @brief     boot from card
 * @param     [in] name :the target file name to boot
 * @param     [in] addr :the dram word address for firmware boot
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kCardFirmwareBoot(
	UINT8 *name,
	UINT32 addr
)
{
	UINT8 sts = SUCCESS;
	UINT32 size;
	UINT32 startAddr = K_SDRAM_CommonEndAddr;
	UINT8 buf[3];
	UINT32 bootSrc, bootSize;

	if( addr != 0 ){
		startAddr = addr;
	}
	// skip CDSP write to DRAM
	buf[0] = XBYTE[REG_Front_Finten_low];
	buf[1] = XBYTE[REG_Cdsp_Cdspinten_lo];
	buf[2] = XBYTE[REG_Cdsp_pSkipwr];
	XBYTE[REG_Front_Finten_low] = 0x00;
	XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	//XBYTE[REG_Cdsp_pSkipwr] = 0x07;
	HAL_CdspVDramSkipWrSyncSet(0);
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0x0f);
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);

	printf("Boot: %s, Free: %luKB\n",name, ((K_SDRAM_TotalSize - startAddr) << 1) >> 10);
	if( FALSE == dramFileLoad(name, startAddr, &size)){
		dosRwErr(__FILE,(unsigned short)__LINE__);
		//EXIT_CRITICAL(sts);
		XBYTE[REG_Front_Finten_low] = buf[0];
		XBYTE[REG_Cdsp_Cdspinten_lo] = buf[1];
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		//XBYTE[REG_Cdsp_pSkipwr] = regBuf[2];
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0x0f);
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,buf[2]);
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		HAL_CdspVDramSkipWrSyncSet(((buf[2]&0x10)>>4));
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		return FAIL;
	}
	if( size > ((K_SDRAM_TotalSize - startAddr) << 1)){
		//EXIT_CRITICAL(status);
		dosRwErr(__FILE,__LINE__);
		XBYTE[REG_Front_Finten_low] = buf[0];
		XBYTE[REG_Cdsp_Cdspinten_lo] = buf[1];
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		//XBYTE[REG_Cdsp_pSkipwr] = regBuf[2];
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0x0f);
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,buf[2]);
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		HAL_CdspVDramSkipWrSyncSet(((buf[2]&0x10)>>4));
		//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
		return FAIL;
	}
	#if 1
	/* Read 1st 32 bytes for BRN format check */
	//SDRAM_Dram2Sram( startAddr, G_ucStorData, 512 );
	HAL_DramDmaRead(G_ucStorData, startAddr, 512, 1);
	if(cardFirmwareChkFmt(G_ucStorData) == SUCCESS)//it's 1528.bin
	{
		
		bootSrc = startAddr + (fwUpdateAddrGet() >> 1) + 1;
		bootSize = 0x80000;
		//dumpData(512, G_ISPDRAMAddr, 1);
	}
	else{//supposed to be fw.bin
		bootSrc = startAddr;
		bootSize = size;
	}
	#else
	G_ISPDRAMAddr = startAddr;
	G_BulkSize = size;
	#endif

	bootCore(bootSrc, 0, bootSize);
	//EXIT_CRITICAL(sts);
	return sts;
}


UINT8
fwUpdateDo(
	void
)
{
#if !CARD_MOUNT_ISP
	return SUCCESS;
#else
	UINT8 i, file;
	UINT8 sts = FAIL, fd;
	UINT32 size;
	UINT32 startAddr = K_SDRAM_CommonEndAddr;
	UINT32 addr = G_ISP_Addr;
	
	if( addr != 0 ){
		startAddr = addr;
	}
	fd = DOS_FileOpen("ISPKEY.TXT", 'r');
//	dbgWdtFeed(40000);  //wangxing del for spi card isp  timeout //06/27
	if ( fd==0 ) 
	{	
		//dosRwErr(__FILE,(unsigned short)__LINE__);
		return FAIL;
	}
	HAL_GprmDramFill(startAddr, 512, 0x00);
	DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
	DOS_FileRead(fd, startAddr << 1, 512, 0);
	DOS_FileClose(fd);
	//printf("size=%ld\n",size);
	if( size != 343 ){
		dosRwErr(__FILE,(unsigned short)__LINE__);
		return FAIL;
	}
	SDRAM_Dram2Sram( startAddr, G_ucStorData, 512 );
	sts = FALSE;
	//dumpData(512, &G_ucStorData[0], 0);
	if( G_ucStorData[0] == 0x54 && G_ucStorData[0x11] == 0x4F && \
		G_ucStorData[0x22] == 0x69 && G_ucStorData[0x33] == 0x40 && \
		G_ucStorData[0x44] == 0x61 && G_ucStorData[0x55] == 0x67 && \
		G_ucStorData[0x66] == 0x74 && G_ucStorData[0x77] == 0x6E)
	{
		//printf("magic\n");
		i = 0;
		if(DOS_FileOpen("FW.BIN", 'r') != 0)
		{

			
			file = 1;
			DOS_FileClose(fd);
		}
		else if(DOS_FileOpen("1628.BIN", 'r') != 0)
		{
			
			file = 2;
			DOS_FileClose(fd);
		}
		else
		{
			file = 0;
			dosRwErr(__FILE,(unsigned short)__LINE__);
			return FAIL;
		}

		if(FALSE == fwUpdateConfirmOsd(startAddr))
		{
			dosRwErr(__FILE,(unsigned short)__LINE__);
			return FAIL;
		}
		
		fd = DOS_FileOpen("OXOXOXOX.XOX", 'r');
		if ( fd == 0 ) 
		{
			DOS_FileDel("ISPKEY.TXT");
			osdStrDraw(0,0 + i,"Del ISPKey", (1<<4)+0);
			i++;
		}
		else
		{
			osdStrDraw(0,0 + i,"OXOXOXOX.XOX", (1<<4)+0);
			i++;
		}
		
		DOS_FileClose(fd);
		
		if( file == 2 )
		{
			osdStrDraw(0,0 + i,"1628.BIN", (1<<4)+0);
			i++;
			if(DOS_FileOpen("DRAMPARA.TXT", 'r') != 0)
			{
				osdStrDraw(0,0 + i,"DRAMPARA.TXT", (1<<4)+0);
				i++;
				DOS_FileClose(fd);
			}
			sts = sp1kCardFirmwareUpdate("1628.BIN", startAddr);
		}
		else
		{
			osdStrDraw(0,0 + i,"FW.BIN", (1<<4)+0);
			i++;
			sts = sp1kCardFirmwareUpdate("FW.BIN", startAddr);
		}
		
		if( sts == SUCCESS )
		{
			fwUpdateOkOsd(startAddr);
			HAL_SetGPIOBit(PWR_ON_EN,0);
			HAL_GlobalTimerWait(3000);
		}
	}
	return sts;
#endif
}

/**
 * @fn        void sp1kCardResourceUpdate(void)
 * @brief     sp1kCardResourceUpdate
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2011-1-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCardResourceUpdate(void)
{
	UINT8 sts;
	FDB RetFdb;
	UINT8 ret, i8;
	UINT8 regBuf[3];
	UINT8 strBuf[5]={0};
	UINT32 freeBuffAddr = K_SDRAM_ImagePlaybackBufAddr;
	UINT32 size;
	UINT32 resId;
	UINT32 resSize;
	UINT8 *p;
	// skip CDSP write to DRAM
	regBuf[0] = XBYTE[REG_Front_Finten_low];
	regBuf[1] = XBYTE[REG_Cdsp_Cdspinten_lo];
	regBuf[2] = XBYTE[REG_Cdsp_pSkipwr];
	XBYTE[REG_Front_Finten_low] = 0x00;
	XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	//XBYTE[REG_Cdsp_pSkipwr] = 0x07;
	HAL_CdspVDramSkipWrSyncSet(0);
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0x0f);
	//printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	DIR_SearchReset();// reset will search from 0
	while(1) 
	{
		// search 1 file a times from previous index
		// if search JPG file, ret = DIR_Search(0, "????????JPG", (FDB *)&RetFdb, 3);
		// if ret = FAIL means search no file
		ret = DIR_Search(DOS_CurrDriverGet(), "UPD_????*", (FDB *)&RetFdb, 3);
		if(ret==FIND_FILE_FAIL)
		{
			printf("No Res!\n");
			break;
		}
		
		for(i8=0;i8<4;i8++)
		{
			strBuf[i8] =RetFdb.name[i8+4];
		}
		resId=strtoul(strBuf,&p,16);
		printf("resId=0x%lx\n",resId);
		sp1kNandRsvSizeGet(resId, NULL, &resSize);

		sts= sp1kDramFileLoad(RetFdb.name, freeBuffAddr, &size);
		if(( size > ((K_SDRAM_TotalSize - freeBuffAddr) << 1))||(size > resSize))
		{
			printf("file too large!\n");
			continue;
		}
		if(sts)
		{
			 sp1kNandRsvWrite(resId, freeBuffAddr << 1);
		}
		dbgWdtFeed(10000);
	}

	XBYTE[REG_Front_Finten_low] = regBuf[0];
	XBYTE[REG_Cdsp_Cdspinten_lo] = regBuf[1];
	printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	//XBYTE[REG_Cdsp_pSkipwr] = regBuf[2];
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0x0f);
	printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,regBuf[2]);
	printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	HAL_CdspVDramSkipWrSyncSet(((regBuf[2]&0x10)>>4));
	printf("CARD Res U 0x2277=%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
}

/**
 * @fn        UINT8 cardFirmwareChkFmt(UINT8 *pInfo)
 * @brief     cardFirmwareChkFmt
 * @param     [in] pInfo
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-11-22
 * @todo      N/A
 * @bug       N/A
*/
static UINT8 cardFirmwareChkFmt(UINT8 *pInfo)
{
	UINT8 fmtStr[] = CARD_FW_UPD_FMT_TAG;
	
	return strncmp(pInfo, fmtStr, strlen(fmtStr));
}
