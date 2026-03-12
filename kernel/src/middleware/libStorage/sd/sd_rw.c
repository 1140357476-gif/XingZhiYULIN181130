/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		sd533.c

Abstract:

		Module related to read write a 512 byte	between	asic and cf.
		All	subroutines	in this	file should	not	customer dependent.
		If there customer dependent	such as	GPIO, please use global
		variables.

Environment:

		Keil C51 Compiler

Revision History:

		02/06/2002		Leeada	  created

--*/

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "dbg_def.h"
#if	1//(SD)
#include "asicreg.h"
#include "main.h"
//#include "cardcon.h"
//#include "cardfunc.h"
#include "cardvar.h"
#include "dosvar.h"
#include "doslink.h"
#include "dos32.h"

//#include "sdcon.h"
#include "sdfunc.h"
//#include "sdvar.h"

#include "sdramhw.h"
#include "setmode.h"
#include "sd_def.h"
#include "initio.h"
#include "gpio.h"
#include "gpio_api.h"
#include "uiflow.h"
//#include "l1.h"
#include "hal.h"
#include "hal_sd.h"
#include "reg_def.h"

#include "sd_private.h"
#include "cardlink.h"

#define __FILE		__FILE_ID_SD_RW__

//=============================================================================
//Symbol
//=============================================================================
//void sdBgRwInit(UINT32 addr, UINT32 num, UINT8 mode);
//void INTR_SDBgRw(void);
//UINT8 sdBgRwStsGet(void);
//void sdSpecialCheck(UINT8* pcardID);
UINT8 sdSectorW(UINT32 start, UINT32 num, UINT32 SrcAddr);
UINT8 sdSectorR(UINT32 start, UINT32 num, UINT32 TarAddr);

//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
//extern xdata UINT8 SD_CID[];
//extern xdata UINT8 SD_CSD[];
//extern xdata UINT8 SD_RCA[];
//extern UINT8 RspBuf[], sdPageShift, sdByteMode;
//extern bit sd_insert;
//UINT8 isSDHC;
//UINT8 ForceByteMode=1;	//for old file system, force to byte addressing mode
//UINT32 SdCurrRwByteAddr;
// @patch5.1@wyeo@solve	avi	sync problem on	sd,	mmc@begin
//xdata UINT32	G_SD_StartLogAddr;
//xdata UINT32	G_SD_EndBlkAddr;
//xdata UINT16 G_SD_BlockSize;
//xdata UINT8 G_SD_BlockSizeShift;
//xdata UINT8 G_SD_ReadPhase = 0;
//xdata UINT8 G_SD_WritePhase = 0;
//xdata UINT8 G_Sd_Detect = 1;
//xdata UINT8 sdHiSpeed = 0;
//xdata UINT8 G_SD_Phase = 0;
//xdata UINT8 G_SD_UnderWrite;
//xdata UINT32 SD_PhySectors;
//xdata UINT32 G_SD_MultiSecNum;
//static xdata UINT8 mmcSupport=1;

//#if (SD_W_INT_EN || SD_R_INT_EN)
//static UINT32 sd_rw_addr, sd_rw_num;
//static UINT8 sd_rw_mode=SD_BG_FREE;	/* 1=write, 0=read */
//xdata UINT8 G_IE_Value;
//UINT8 sd_bg_r=0;
//UINT8 sd_bg_w=0;
//#endif

//code UINT8 sdSpeedTable[] = {
//	0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
//};
//code UINT32 sdSpeedFactor[] = {100, 1000, 10000, 100000}; 	/* unit in kHz */
//code UINT32 tTAACFactor[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000	};
//xdata UINT8 G_Detect_PinGrp = GPIO_BYTE_GEN1;
//xdata UINT8 G_Detect_PinMux = 0x08;


//code UINT8 specialCardID[][SD_CID_CMP_NUM]={{0x00,0x20,0x20,0x4E,0x2F,0x41,0x20,0x20}};
//UINT8 xdata sdModePostIdle=0;  //SD controller will send dummy clock after waiting data 0 high in this mode

// @patch5.1@wyeo@solve	avi	sync problem on	sd,	mmc@end

//extern xdata UINT8 G_USBReset;
//UINT8 isMMC;

//=============================================================================
//Program
//=============================================================================
#if 0 //xian move to sdhw.c
void sdRdPhaseCheck(void)
{
	/* fixed at 0x08 */
	//xian -- 20100428, move to hal_sd.c
	//G_SD_ReadPhase = 0x08;
}
//-----------------------------------------------------------------------------
//SD_HwInitialize
//-----------------------------------------------------------------------------
UINT8 SD_HwInitialize(UINT8 cardType)
{
	//patch	4.4-7@wyeo@remove debug	message
	//M_Card_Printf("SD	card start	the	identification process\n");
	UINT32 cSize,nrSects,arg;
	UINT32 tTAAC,tNSAC,busSpeed, busMode;
	UINT16 cSizeMult;
	UINT8 readBlkLen, rwFactor,	tmp, specVer;
	isMMC =	0;
	isSDHC = -1;
	G_SD_MultiSecNum = SD_MULTI_RW_MAX;//mantis-27450
	tmp = L1_SDIdentification(SD_MODE, cardType, K_Card_PageSize);
	if(tmp != 0)	// not SD ?
	{
		if(MMC_SUPPORT_GET()){
			tmp = L1_SDIdentification(MMC_MODE, cardType, K_Card_PageSize);
		}
		if(tmp != 0)	// not EMMC ?
		{
			//printf("%s %u, Can't detect	SD/EMMC\n", __FILE__, (UINT16)__LINE__);
			__ASSERT(__FILE, __LINE__, 1);
			return FALSE;
		}
		isMMC =	1;
	}
	sdSpecialCheck(SD_CID);
	if (isSDHC==1) {
		cSize =	((SD_CSD[7]	& 0x3F)	<< 16) + (SD_CSD[8]	<< 8) +	SD_CSD[9];
		nrSects	= (cSize + 1) *	1024;
		//ForceByteMode = 0;
	}

	if(isMMC)
	{
		specVer	= (SD_CSD[0] & 0x3c) >>	2;
		printf("EMMC Ver=%bu ", specVer);
	}
	if (isSDHC == 0	) {
		readBlkLen = SD_CSD[5] & 0x0f;
		cSize	   = ((SD_CSD[6] & 0x03) <<	10)	+
					 ((SD_CSD[7]	   ) <<	2 )	+
					 ((SD_CSD[8] & 0xc0) >>	6 );
		cSizeMult  = ((SD_CSD[9] & 0x03) <<	1 )	+
					 ((SD_CSD[10]& 0x80) >>	7 );
		nrSects	= (cSize + 1) *	(1 << (cSizeMult + 2));
		nrSects	= nrSects << (readBlkLen - 9);
		printf("blkLen=%bu, cSize=%lu, cSizeMult=%u\n", \
			 readBlkLen, cSize, cSizeMult);
	}
	G_Card_TotalCardSize = 0;
#if 1//def DOS32_SUPPORT
	//printf("line=%d	SDHC nrSects=%ld\n",	(UINT16)__LINE__,	nrSects);
#else
	if( nrSects > 4194304L ){//For FAT 16, let it smaller than 2GB
		//nrSects = 3984588L;
		G_Card_TotalCardSize = nrSects/2048;
		//printf("G_Card_TotalCardSize=%lu\n",G_Card_TotalCardSize);
		__ASSERT(__FILE, __LINE__, 1);
		return	FALSE;
	}
#endif
	if( nrSects >= 8388607 ){//total size with byte unit will overflow
		G_Card_SectorPerCard = nrSects;
		G_Card_TotalCardSize = 0;
	}
	else{
		G_Card_SectorPerCard = 0;
		G_Card_TotalCardSize = nrSects * 512;
	}
	#if	0
	printf("%s(%d):	mb=%ld cSize=%lu nrSects=%ld\n", \
		__FILE__, (UINT16)__LINE__,nrSects/2048, cSize, nrSects);
	#endif
	SD_PhySectors = nrSects;
	//sdStateCheck();
	tTAAC =	(SD_CSD[1] & 0x78) >> 3;
	tTAAC =	sdSpeedTable[tTAAC];
	tmp	= SD_CSD[1]	& 0x07;
	tTAAC =	tTAACFactor[tmp] * tTAAC / 10;
	tNSAC =	SD_CSD[2] *	100L;
	rwFactor = SD_CSD[12] &	0x1C;
#if	SPEED_OUTPUT_ENABLE
	if(tTAAC > 1000*1000*10L)
		printf("tTAAC=%ld(ms) ", tTAAC/(1000*1000L));
	else if(tTAAC >	1000*10L)
		printf("tTAAC=%ld(us) ", tTAAC/1000L);
	else
		printf("tTAAC=%ld(ns) ", tTAAC);
	printf("tNSAC=%ld(clk)\n", tNSAC);
	printf("%s %d, rwFactor=%bd ", __FILE__, (UINT16)__LINE__, rwFactor);
#endif

#if	0
	printf("sdSpeedTable:");
	for(tmp=0; tmp<sizeof(sdSpeedTable); tmp++)
		printf("%bd ", sdSpeedTable[tmp]);
	printf("\n");
#endif
	tmp	= (SD_CSD[3] & 0x78) >> 3;
	busSpeed = (UINT32)sdSpeedTable[tmp];
	busSpeed = (busSpeed * sdSpeedFactor[SD_CSD[3] & 0x07])	/ 10;

	if (L1_SDCheckState() != 0x00)
	{
		//printf(" Set SD bus mode failed\n");
		__ASSERT(__FILE, __LINE__, 1);
		return	FALSE;
	}

	if(!isMMC) {	//data transfer state only
		arg	= ((UINT32)SD_RCA[0] <<	24)	+ ((UINT32)SD_RCA[1]<<16);
		HAL_SDTxCommand(55, arg, RSP_TYPE_R1, RspBuf);
		HAL_SDTxCommand(51, 0, RSP_TYPE_R1, RspBuf);
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaDo(2,1,64/8,0,0);
		specVer = G_ucStorData[0] & 0x0F;
		if(specVer > SD_SPEC_VER_20)
			specVer = SD_SPEC_VER_10;

		#if	SCR_OUTPUT_ENABLE
		printf("SCR: ");
		for(tmp=0; tmp<8; tmp++)
			printf("%02bX ", G_ucStorData[tmp]);
		#endif

		printf("SD Ver=%bu\n", specVer);

		if(specVer != SD_SPEC_VER_10) {
			G_SD_UnderWrite = 0;
			HAL_SDTxCommand(6, 0x00FFFF01, RSP_TYPE_R1, RspBuf);
			HAL_DmaSetSramAddr(0x0000);
			HAL_DmaDo(2,1,512/8,0,0);
			#if	0
			printf("CMD6: ");
			for(tmp=0; tmp<17; tmp++)
				printf("%02bX ", G_ucStorData[tmp]);
			printf("\n");
			#endif
			if(G_ucStorData[13] & 0x02) {
				printf("HiSpeed? ");
				HAL_SDTxCommand(6, 0x80FFFF01, RSP_TYPE_R1, RspBuf);
				HAL_DmaSetSramAddr(0x0000);
				HAL_DmaDo(2,1,512/8,0,0);
				if(G_ucStorData[16] & 0x01) {
					busSpeed = 50000L;
					printf("YES! ");
					//sdHiSpeed = 1;
					HAL_SDHighSpeedStsSet(1);
				}
				else{
					printf("NO! ");
					//sdHiSpeed = 0;
					HAL_SDHighSpeedStsSet(0);
				}

				#if	0
				printf("CMD6: ");
				for(tmp=0; tmp<17; tmp++)
					printf("%02bX ", G_ucStorData[tmp]);
				printf("\n");
				#endif

				#if	0
				HAL_SDTxCommand(6, 0x80FFFF00, RSP_TYPE_R1, RspBuf);
				HAL_DmaSetSramAddr(0x0c00);
				HAL_DmaDo(2,1,512/8,0,0);
				printf("CMD6: ");
				for(tmp=0; tmp<17; tmp++)
					printf("%02bX ", G_ucStorData[tmp]);
				printf("\n");
				#endif
			}
		}
	}

#if	0
	//busSpeed = 24000L;
	busMode	 = SD_1BIT_MODE;
#else
	busMode	= SD_4BIT_MODE;
#endif
	if (isMMC && (specVer <	4))
		busMode	= SD_1BIT_MODE;
//busSpeed = 16000L;
	if(isSDHC){ 
		busSpeed = 48000L;
	}
	if((busSpeed >=	48000L)	&& (!isMMC || specVer>=4))
	{
		printf("48MHz ");
		busSpeed = SD_48M;
	    //XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = 0x08;
	}
	else if((busSpeed >=	24000L) || (isMMC && specVer>=4))	{
		printf("24MHz ");
		busSpeed = SD_24M;
	}
	#if 0//16M has ASIC problems
	else if(busSpeed >= 16000L  && isMMC)	{
		printf("16MHz ");
		busSpeed = SD_16M;
	}
	#endif
	else if	(busSpeed >= 12000L) {
		printf("12MHz ");
		busSpeed = SD_12M;
	}
	else if	(busSpeed >= 6000L ) {
		printf("6MHz ");
		busSpeed = SD_6M;
	}
	else {
		//printf("375KHz ");
		busSpeed = SD_24M;
	}
	printf("\n");

	//sdBusSpeed = busSpeed;

	G_Sd_Detect = 0;
	sdRdPhaseCheck();
	
	G_Sd_Detect = 1;

	if (isMMC && (specVer >= 4)/* && busMode==SD_4BIT_MODE*/) {
		busMode = SD_4BIT_MODE;
		G_ucStorData[0]	= 0x5a;		/* 4 bit bus test pattern */
		HAL_SDConfig(busSpeed, busMode, isMMC);
		HAL_DmaSetSramAddr(0x0000);
		HAL_SDTxCommand(19, 0, RSP_TYPE_R1, RspBuf);
		HAL_DmaDo(1,2,512,0,0);
		G_ucStorData[0]	= 0x33;
		HAL_DmaSetSramAddr(0x0000);
		HAL_SDTxCommand(14, 0, RSP_TYPE_R1, RspBuf);
		HAL_DmaDo(2,1,512,0,0);
		if (G_ucStorData[0] != ~0x5a) {
			//printf("4 bits bus pattern fail: 0x%bx\n", G_ucStorData[0]);
			busMode	= SD_1BIT_MODE;
		}
	}

	L1_SDSetBusMode(busMode, busSpeed, isMMC);

	#if SD_FAST_W
	XBYTE[REG_Fm_autorsp_dummy_datarspen] = 1;
	#endif
	/*
	Cannon SD 32MB
	OCR: 80 FF 80 00
	CID: 01 50 41 53 30 33 32 42 45 D3 63 75 13 00 74 4D
	CSD: 00 5D 01 32 13 59 81 DA F6 D9 CF FF 16 40 00 7D
	*/
	if ((SD_CID[0] == 0x01) && (SD_CID[1] == 0x50) && (SD_CID[2] == 0x41)) {
		if (  G_Card_TotalCardSize >= 29360128 && G_Card_TotalCardSize <= 67108864 ) { /*32MB~64MB */
			G_SD_MultiSecNum = 64L;//mantis-27450
		}
	}

	//HAL_SDTxCommand(16, 512, RSP_TYPE_R1, RspBuf); /* Set block length */

//#if (SD_CMD_OPTION == 1)
		printf("busMode=%bu\n", busMode);
    #if (SD_R_INT_EN)
    printf("SD_R_INT_EN\n");
    #endif

    #if(SD_W_INT_EN)
    printf("SD_W_INT_EN\n");
    #endif

    #if(SD_R_BG_EN)
    printf("SD_R_BG_EN\n");
    #endif

    #if(SD_W_BG_EN)
    printf("SD_W_BG_EN\n");
    #endif
//#endif
	return TRUE;
}
#endif
//-----------------------------------------------------------------------------
//SD_LCard2Sram
//-----------------------------------------------------------------------------
//patch4.5@ada@Add timeout count begin
UINT8 SD_LCard2Sram(UINT32 SAdr)
{
	UINT8 tmp;
	//UINT16 i;
	UINT16 timeout_count = 0xffff;
	//TX "CMD17" to	read single	block data
	//printf("%s(%d), \n", __FILE__, (UINT16)__LINE__);
	G_SD_UnderWrite = 0;
	//XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
	HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1);
	//XBYTE[REG_Fm_evt] |= 1<<3;	//clr CRC, will clr all 
	XBYTE[REG_Fm_evt]|= 1<<3;	//clr CRC
	HAL_DmaSetSramAddr(0x0000);
	if(HAL_SDTxCommand(17, SAdr>>sdPageShift, RSP_TYPE_R1, NULL) != 0)
	{
		//printf("%s %d, CMD17 ERR\n", __FILE__, (UINT16)__LINE__);
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}
	//DMA from	SD to sram
	tmp	= HAL_DmaDo(2,1,0x200,0,0);
#if	1
	if (tmp	!= 0)
	{
		//printf("%s %u ERR\n", __FILE__, (unsigned short)__LINE__);
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}
#endif
	tmp	= HAL_SDWaitIdle();
	HAL_SDTxDummy();
	if (tmp	!= 0)
	{
		//printf("err2\n");
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}
#if	1
	if (SD_CRC16_OK==0)		//check	data CRC16
	 {
		//printf("%s %d CRC16 ERR\n", __FILE__, (UINT16)__LINE__);
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	 }
#endif
	#if 0
	dumpData(32, &G_ucStorData[479], 0);
	#endif
	return TRUE;
}
//patch4.5@ada@Add timeout count end

#if 0 //move to sdhw.c
UINT8 sdWaitDat0High(void)
{
	UINT32 timeout_count = 0x400000L;
	XBYTE[REG_Fm_Tx_Rx_trigger] = 1<<6;
	do
	{
		if (timeout_count >	0)
		{
			timeout_count--;
		}
		else
		{
			return FALSE;
		}
		if(sD_CdGet()){
			return FALSE;
		}
	} while((XBYTE[REG_Fm_SD_Status] & 0x20) == 0);
	return TRUE;
}
#endif
//-----------------------------------------------------------------------------
//SD_LSram2Card
//-----------------------------------------------------------------------------
//patch4.5@ada@Add timeout count begin
UINT8 SD_LSram2Card(UINT32 sdaddr)
{
	UINT8 err;
	UINT16 timeout_count = 0xffff;

	HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1);
	G_SD_UnderWrite = 1;
	err	= HAL_SDTxCommand(24, sdaddr>>sdPageShift,RSP_TYPE_R1, RspBuf);	//TX "CMD24" to	write single block data
	if (err	!= 0)
	{
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}

	//DMA from sram to SD
	HAL_DmaSetSramAddr(0x0000);
	err	= HAL_DmaDo(1,2,512,0,0);

	if (err	!= 0)
	{
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}
	err = HAL_SDWaitIdle();

	if (err	!= 0)
	{
		return FALSE;
	}

#if	SD_FAST_W
	return TRUE;
#else
	//trigger to read the Card CRC
	HAL_SDRxCRC();
	err = HAL_SDWaitIdle();
	if(SD_TIME_OUT)	//check CRC timeout
	{
		return FALSE;
	}

	if (err	!= 0)
	{
		return FALSE;
	}

	//check the SD	card's CRC status
	HAL_SDCardCRCStat(&err);

	if	(err ==	0)
	{
		return FALSE;
	}
	//wait	until 'data0' to be	high
	return sdWaitDat0High();
#endif
}
//patch4.5@ada@Add timeout count end

//-----------------------------------------------------------------------------
//SD_LCard2Dram
//-----------------------------------------------------------------------------
//patch4.5@ada@Add timeout count begin
UINT8 SD_LCard2Dram(UINT32 sdaddr,UINT32 dramAddr)
{
	UINT8 tmp=0;

	UINT16 timeout_count = 0xffff;
	//DMA from	SD to dram
	HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1);
	G_SD_UnderWrite = 0;
	//XBYTE[REG_Fm_evt] |= 1<<2;	//clr int
	//XBYTE[REG_Fm_evt] |= 1<<3;	//clr CRC
	XBYTE[REG_Fm_evt] = 1<<3;	//clr CRC
	HAL_DmaSetDramAddr(dramAddr);
	if (HAL_SDTxCommand(17,sdaddr>>sdPageShift,RSP_TYPE_R1, NULL) != 0)	//TX "CMD17" to	read single	block data
	{
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}
	if (HAL_DmaDo(2,0,512,0,0) != 0)
	{
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}
	tmp = HAL_SDWaitIdle();
	HAL_SDTxDummy();
	if(tmp != 0)
	{
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}

	if(SD_CRC16_OK == 0)		//check	data CRC16
	{
		__ASSERT(__FILE, __LINE__, 1);
		return FALSE;
	}
	 return	TRUE;
}
//patch4.5@ada@Add timeout count end


//-----------------------------------------------------------------------------
//SD_LDram2Card
//-----------------------------------------------------------------------------
//patch4.5@ada@Add timeout count begin
UINT8 SD_LDram2Card(UINT32 sdaddr,UINT32 dramAddr)
{
#if	SD_FAST_W==0
	UINT8 tmp;
#endif
	UINT16 timeout_count = 0xffff;
	G_SD_UnderWrite = 1;
	//DMA from	dram to	SD
	HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1);
	HAL_DmaSetDramAddr(dramAddr);

	if (HAL_SDTxCommand(24, sdaddr>>sdPageShift,RSP_TYPE_R1,RspBuf)	!= 0)	//TX "CMD24" to	write single block data
		return FALSE;

	if (HAL_DmaDo(0,2,512,0,0) != 0)
		return FALSE;

	if (HAL_SDWaitIdle() != 0)
		return FALSE;
#if	SD_FAST_W
	return TRUE;
#else
	//trigger to read the Card CRC
	HAL_SDRxCRC();
	if(SD_TIME_OUT)	//check	CRC	timeout
	{
		return FALSE;
	}

	tmp	= HAL_SDWaitIdle();
	HAL_SDTxDummy();

	if (tmp	!= 0)		//check	data CRC16
	{
		return FALSE;
	}

	//check	the	SD	card's CRC status
	HAL_SDCardCRCStat(&tmp);
	if	(tmp ==	0)
	{
		return FALSE;
	}

	//wait	until 'data0' to be	high
	return sdWaitDat0High();
#endif
}
//patch4.5@ada@Add timeout count end

//-----------------------------------------------------------------------------
//SD_LDram2CardMore
//-----------------------------------------------------------------------------
UINT8 SD_LDram2CardMore(UINT32 logAddr, UINT32 dramAddr, UINT32 numSector)
{
	 UINT8 sts = FALSE;
	G_SD_UnderWrite = 1;
#if	1//SD_SECTOR_W_ALWAYS
	#if	1//SD_48MHZ_OK
	//xian -- 20100428
	//if( sdHiSpeed == 1 ){
	//	XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_WritePhase;
	//	while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != G_SD_WritePhase );
	//}
	HAL_SDPhaseChange(HAL_SD_PHASE_HI_WRITE);
	#endif
	if(sdSectorW(logAddr, numSector, dramAddr)==0)
		sts = TRUE;
	#if	1//SD_48MHZ_OK
	//xian -- 20100428
	//if( sdHiSpeed == 1 ){
	//	XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_Phase;
	//}
	HAL_SDPhaseChange(HAL_SD_PHASE_NORMAL);
	#endif
	return sts;
#else
	HAL_DmaSetDramAddr(dramAddr);

	 if	(L1_DMAWrSD(0,logAddr,numSector,K_Card_PageSize) ==	0x00)
	 {
		  sts =	TRUE;
	 }
	 return	sts;
#endif
}

//-----------------------------------------------------------------------------
//SD_LCard2DramMore
//-----------------------------------------------------------------------------
UINT8 SD_LCard2DramMore(UINT32 logAddr, UINT32 dramAddr, UINT32 numSector)
{
	 UINT8 sts = FALSE;
	G_SD_UnderWrite = 0;
#if	1//SD_SECTOR_R_ALWAYS
	#if	1//SD_48MHZ_OK
	//xian -- 20100428
	//if( sdHiSpeed == 1 ){
	//	XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_ReadPhase;
	//	while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != G_SD_ReadPhase );
	//}
//xian -- 20100608 	
	HAL_SDPhaseChange(HAL_SD_PHASE_HI_READ);
	#endif
	if(sdSectorR(logAddr, numSector, dramAddr)==0)
		sts = TRUE;
	#if	1//SD_48MHZ_OK
	//xian -- 20100428
	//if( sdHiSpeed == 1 ){
	//	XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_Phase;
	//}
//xian -- 20100608	
	HAL_SDPhaseChange(HAL_SD_PHASE_NORMAL);
	#endif
	return sts;
#else
	sts = L1_DMARdSD(0,logAddr,numSector,K_Card_PageSize);
	if(sts==0x00)
	 {
		  sts =	TRUE;
	 }
	else {
		__ASSERT(__FILE, __LINE__, 1);
	 	//printf("%s %d logAddr=%08lx ret=% ERR\n", __FILE__, (UINT16)__LINE__,logAddr, sts);
	}
	 return	sts;
#endif
}

//richie@mi0415
//-----------------------------------------------------------------------------
//SD_LCard2UsbMore
//-----------------------------------------------------------------------------
UINT8 SD_LCard2UsbMore(UINT32 logAddr, UINT32	numSector)
{
	 UINT8 sts = FALSE;

#if 1//def DOS32_SUPPORT
	sts = SD_LCard2DramMore(logAddr, 0, numSector);
#else
	G_SD_UnderWrite = 0;
	if	(L1_DMARdSD(K_DMA_USB,logAddr,numSector,K_Card_PageSize) ==	0x00)
	{
	  sts =	TRUE;
	}
#endif
	 return	sts;
}

//richie@mi0415
//-----------------------------------------------------------------------------
//SD_LUsb2CardMore
//-----------------------------------------------------------------------------
UINT8 SD_LUsb2CardMore(UINT32 logAddr, UINT32	numSector)
{
	 UINT8 sts = FALSE;

#if 1//def DOS32_SUPPORT
	sts = SD_LDram2CardMore(logAddr, 0, numSector);
#else
	G_SD_UnderWrite = 1;
	if	(L1_DMAWrSD(K_DMA_USB,logAddr,numSector,K_Card_PageSize) ==	0x00)
	{
	  sts =	TRUE;
	}
#endif
	 return	sts;
}

//-----------------------------------------------------------------------------
//SD_LCard2Usb
//-----------------------------------------------------------------------------
UINT8 SD_LCard2Usb(UINT32 logAddr)
{
	 UINT8 sts = FALSE;
	G_SD_UnderWrite = 0;
	 if	(L1_DMARdSDBulk2(K_DMA_USB,logAddr,1,K_Card_PageSize) == 0x00)
	 {
		  sts =	TRUE;
	 }

	 return	sts;
}

//-----------------------------------------------------------------------------
//sdSectorW
//-----------------------------------------------------------------------------
//TX "CMD25" to write multiple	block data
#if (SD_W_INT_EN == 0)
UINT8 sdSectorW(UINT32 start, UINT32 num, UINT32 SrcAddr)
{
	UINT8 ret, tmp = 0, Src;//, SrcPath;
	UINT32 addr;
	UINT32 wrt_num;
	#if 1 //for dma error test
	UINT32 tm1, tm2;
	#endif

	//UINT8 status;
	G_SD_UnderWrite = 1;
	#if (SD_R_INT_EN)
	if(sdBgRwFreeStsWait() == FALSE){
		__ASSERT(__FILE, __LINE__, 1);
	}
	#endif

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

#if 0//SD_W_INT_EN
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
			if(sD_CdGet() & G_Detect_PinMux)	return 0;
			//printf("W.wait...%bu", sdBgRwStsGet());
			if((num & 0xFFFF)==0){
				//printf("W.wait...%bu", sdBgRwStsGet());
				__ASSERT(__FILE, __LINE__, 1);
			}
			num++;
		};
	}
	else{
#if DOS_DEV_SUPPORT_USB
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
#endif //DOS_DEV_SUPPORT_USB        
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
        	HAL_GlobalTimerRead10us(&tm1); //dma error test
            if(XBYTE[REG_Fm_dramfmread_dmasel] & 0x10)    //xian ++
                ret = HAL_DmaDo(Src,DMA_PATH_FLASH,wrt_num<<9,0,0);
            else 
                ret = HAL_SDDram(SrcAddr, wrt_num<<9, 0);
        	HAL_GlobalTimerRead10us(&tm2);
        }
        else
        {
			//UINT32 bi;

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
			else if(ret == 2){
				break;
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
			#endif
		}

		if(ret){ //dma error
			#if 1
			printf("%bx,%bx\n", XBYTE[REG_Fm_SD_Status], XBYTE[REG_Fm_SD_Datastate_CRDSts]);
			printf("%bx,%bx\n", XBYTE[REG_Fm_SD_Cmdstate], XBYTE[REG_Fm_mbblkcnt_Low]);
			printf("%lu\n", tm2-tm1);
			#endif
			break;
		}

		while(!sD_CdGet()) {
			if(XBYTE[REG_Fm_evt] & (1<<2))
				break;
			if (G_USBReset == 0x01){
				/* exit while loop */
				break;
			}
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
		if (G_USBReset == 0x01){
			break;
		}

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
	return ret;
}
#endif

//-----------------------------------------------------------------------------
//sdSectorR
//-----------------------------------------------------------------------------
#if (SD_R_INT_EN == 0)
UINT8 sdSectorR(UINT32 start, UINT32 num, UINT32 TarAddr)
{
	UINT8 ret = 0, dst;//,tmp, TarPath;
	UINT32 addr;
	UINT32 rd_num;

	G_SD_UnderWrite = 0;
	#if (SD_W_INT_EN)
	if(sdBgRwFreeStsWait() == FALSE){
		__ASSERT(__FILE, __LINE__, 1);
	}
	#endif

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
#if 0//SD_R_INT_EN
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
			if(sD_CdGet() & G_Detect_PinMux)
				break;
			if((num & 0xFFFF)==0){
				//printf("R.wait(%02bX:%02bX:%02bX)-", XBYTE[REG_Fm_evt], XBYTE[REG_Fm_intEn], XBYTE[REG_Fm_SD_Datastate_CRDSts]
				__ASSERT(__FILE, __LINE__, 1);
			}
			num++;
		};
	}
	else{
#if DOS_DEV_SUPPORT_USB        
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
#endif //DOS_DEV_SUPPORT_USB        
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
		if(sD_CdGet())  //mantis 0041557
			return FAIL;
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
			//UINT32 bi;
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
			#else //xian move to L1_SDCard2UsbDmaWait()
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
		#if 1 //xian -- 20100608 mount error????		//check 2400
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
	return ret;
}
#endif

#if 0 //xian move to sdhw.c
/**********************************************************************/
/* check if it is a special card need sent dummy clock after data0 return to high*/
/*********************************************************************/
void sdSpecialCheck(UINT8* pcardID)
{
	UINT8 cnt=0;
	UINT8 sdSpecialNum;
	sdSpecialNum=(sizeof(specialCardID))/SD_CID_CMP_NUM;
	//printf("sdN:%bu\n",sdSpecialNum);
	while(cnt<sdSpecialNum){
		if(0==memcmp(pcardID, specialCardID[cnt], SD_CID_CMP_NUM)){           //compare the MID;OID and PNM chick if it is special card
			SD_MODE_POST_IDLE_SET(1);
			//printf("special card\n");
			return;
		}
		cnt++;
	}
	SD_MODE_POST_IDLE_SET(0);
	return;
}
/************************************************************************/
/**/
/************************************************************************/
void sp1kMmcSupportSet(UINT8 val)
{
	MMC_SUPPORT_SET(val);
}
#endif

#endif

