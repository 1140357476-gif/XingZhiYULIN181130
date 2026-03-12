/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		L1_Storage.c

Abstract:

		Storage	related	 function

Environment:

		Keil C51 Compiler

Revision History:

		11/30/2001		WZH	   created

--*/

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "dbg_def.h"
#include "hal.h"
#include "cardopt.h"
#include "setmode.h"
#include "main.h"

#include "sd_def.h"
#include "hal_sd.h"
#include "dos_dev_support.h"
#if	1//(SD)
//#include "sdvar.h"	//ada@0218
#include "dosvar.h"
#include "sdfunc.h"
#include "uiflow.h"

#include "doslink.h"

#endif
#include "gpio.h"
#include "gpio_api.h"
#include "sd_private.h"
#include "cardlink.h"
#include "reg_def.h"
#include "hal_global.h"
//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
#define __FILE		__FILE_ID_SD_HW__

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
#if	1//(SD)
static xdata UINT8	SD_OCR[4];
static xdata UINT8	SD_RCA[2];
static xdata UINT8	SD_CID[16];
static xdata UINT8	SD_CSD[16];
#endif

static xdata UINT8 mmcSupport=1;
static UINT8 isMMC;
static UINT8 isSDHC;
static UINT8 sdSpeedClass = 0;
	
UINT8 RspBuf[17];
UINT8 sdPageShift;
UINT8 sdByteMode;
UINT8 sdModePostIdle=0;  //SD controller will send dummy clock after waiting data 0 high in this mode

xdata UINT32 G_SD_MultiSecNum;
xdata UINT8 G_SD_UnderWrite;

xdata UINT8 G_Sd_Detect = 1;
xdata UINT32 SD_PhySectors;


code UINT8 sdSpeedTable[] = {
	0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
};
code UINT32 sdSpeedFactor[] = {100, 1000, 10000, 100000}; 	/* unit in kHz */
code UINT32 tTAACFactor[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000	};
xdata UINT8 G_Detect_PinGrp = GPIO_BYTE_GEN1;
xdata UINT8 G_Detect_PinMux = 0x08;
xdata UINT8 speedupSdMount;

#define MMC_SUPPORT_SET(val)                     do{mmcSupport=(val);}while(0)
#define MMC_SUPPORT_GET()                          mmcSupport

extern UINT8 HAL_GlobalICVerGet(void);
UINT8 sd_flag =0;
//patch4.5@richie@msdc plug	out	exception
//#include "uiflow.h"
//=============================================================================
//Program
//=============================================================================
#if	1//(SD)

#if 1
UINT8 sd_IdentificationCmd(UINT8 mmcMode, UINT8 cardType, UINT16 BlockSize)
{
	UINT8 error;
	UINT32 arg, i;

	//printf("fuc : 2\n");
	isMMC =	0;
	isSDHC = -1;
	G_SD_MultiSecNum = SD_MULTI_RW_MAX;//mantis-27450
	
	sd_flag = 0;

	HAL_FlashMode(cardType, 1, 1);
	HAL_SDInit(mmcMode);
	HAL_SDConfig(3,0, mmcMode);  //375KHz,1 bit bus
	HAL_SDBlockSize(BlockSize);
	for(i=0; i<100; i++)	//100 -> 10
	{					// Tx 80 clock for power on	sequence
		HAL_SDTxDummy();
	}

	error =	HAL_SDTxCommand(0, 0,RSP_TYPE_R1, NULL);					// Tx "CMD0"
	if (error != 0)
	{
		return 8;
	}

	HAL_SDTxDummy();

	if ( mmcMode == SD_MODE ) {
		/* According to SD's specification, the worst case of power-on could
		 * be one second, while MMC's specification does not describe it.
		 */
		if(isSDHC==(UINT8)-1)
		{
			error = HAL_SDTxCommand(8, 0x000001aa,RSP_TYPE_R1, RspBuf);
            isSDHC = (error==0);
        }
	}
	else
	{
		isSDHC= 0;
	}

	arg  = isSDHC ? 0x40040000 : 0x003c0000; /* Bit 30: HCS */
	printf("%s %d, isSdVer2=%d, %s, arg=%08lX\n", __FILE__,  (unsigned short)__LINE__,\
		(UINT16)isSDHC, mmcMode==SD_MODE ? "SD" : "MMC", arg);
	
	error	= 0;
	if(sD_CdGet()){
		return 1;
	}
	if ( mmcMode == SD_MODE ) {
		if(error==0)
			error = HAL_SDTxCommand(55, 0, RSP_TYPE_R1, RspBuf);
		if(error==0)
			error = HAL_SDTxCommand(41, arg, RSP_TYPE_R3, RspBuf);
		if(error) {
			//printf("%s %d, SD ACMD41:FAIL, arg=%08lx\n",  __FILE__,  (unsigned short)__LINE__, arg);
			return 1;
		}
	}
	else {
		error = HAL_SDTxCommand(1, 0x003c0000, RSP_TYPE_R3, RspBuf);
		if(error) {
			//printf("%s %d, MMC CMD1:FAIL\n",  __FILE__, (UINT16)__LINE__);
			return 1;
		}
	}
	if ( (RspBuf[1] & 0x80) != 0 ){
		sd_flag = 1; //sd ready
	}

	return error;
}

UINT8 sd_IdentificationInfoGet(UINT8 mmcMode,UINT8 cardType)
{
	UINT8 error;
	UINT8 READ_BL_LEN;
	UINT16 BLOCK_LEN;
	UINT16 C_SIZE;
	UINT8 C_SIZE_MULT;
	UINT16	MULT;
	UINT32 BLOCKNR;
	UINT32 arg, i;
	UINT16 timeOut;
	UINT8 ret = 0;
	
	//printf("fuc : 3\n");
	HAL_FlashMode(cardType,K_MEDIA_NANDF,1);

	arg  = isSDHC ? 0x40040000 : 0x003c0000; /* Bit 30: HCS */
	timeOut = 2000;
	error	= 0;
	if(sd_flag == 0){
		do{
			if(sD_CdGet()){
				ret = 1;
				goto RET_ENTRY;
				//return 1;
			}
			if ( mmcMode == SD_MODE ) {
				if(error==0)
					error = HAL_SDTxCommand(55, 0, RSP_TYPE_R1, RspBuf);
				if(error==0)
					error = HAL_SDTxCommand(41, arg, RSP_TYPE_R3, RspBuf);
				if(error) {
					//printf("%s %d, SD ACMD41:FAIL, arg=%08lx\n",  __FILE__,  (unsigned short)__LINE__, arg);
					//return 1;
					ret =1;
					goto RET_ENTRY;
				}
			}
			else {
				error = HAL_SDTxCommand(1, 0x003c0000, RSP_TYPE_R3, RspBuf);
				if(error) {
					//printf("%s %d, MMC CMD1:FAIL\n",  __FILE__, (UINT16)__LINE__);
					//return 1;
					ret =1;
					goto RET_ENTRY;
				}
			}
			timeOut--;
			if ( timeOut == 0 ) {
				//printf("ACMD41 or CMD1 ERROR, Power Negotiate Fail!\n");
				//return 1;
				ret =1;
				goto RET_ENTRY;
			}
			//for(i=0; i<7000; i++);	/* delay > 3 ms for some LAN-CARD */
		}while ( (RspBuf[1] & 0x80) == 0 ); /* Bit31: busy bit, low->card is busy */
	}
	printf("tm:%u\n", timeOut); // for card error test

#if	OCR_OUTPUT_ENABLE
	printf("OCR: ");
	for ( i = 0; i < 4; i++ ) { /* Get card's OCR register */
		SD_OCR[i] = RspBuf[i + 1];
		printf("%02bX ", SD_OCR[i]);
	}
	printf("\n");
#endif

	/* Check OCR bit 30 to see if SDHC */
	if(isSDHC) {
		if(SD_OCR[0] & 0x40) {
			printf("SDHC\n");
		}
		else {
			isSDHC = 0;
		}
	}
	//sdPageShift = isSDHC ? 0 : 9; /* SD2.0 uses sector addressing, while 1.1 was byte addressing */
	sdPageShift = isSDHC ? 9 : 0; /* SD2.0 uses sector addressing, while 1.1 was byte addressing */
	sdByteMode = !isSDHC;

	if (HAL_SDTxCommand(2, 0,RSP_TYPE_R2, RspBuf) != 0){
		ret = 8;
		goto RET_ENTRY;
		//return 8;
	}

#if	CID_OUTPUT_ENABLE
	printf("CID:");
	for(i=0; i<16; i++) // get card's CID register
	{
		SD_CID[i] = RspBuf[i+1];
		printf(" %02bX", SD_CID[i]);
	}
	printf("\n");
#endif

	if (HAL_SDTxCommand(3, 0, RSP_TYPE_R6, RspBuf) != 0){
		//return 8;
		ret =8;
		goto RET_ENTRY;
	}
	// get card's RCA register
	SD_RCA[0] = RspBuf[1];
	SD_RCA[1] = RspBuf[2];

	arg = ((UINT32)SD_RCA[0]<<24) + ((UINT32)SD_RCA[1]<<16);
	if (HAL_SDTxCommand(9, arg, RSP_TYPE_R2, RspBuf) != 0){
		//return 8;
		ret = 8;
		goto RET_ENTRY;
	}

	// get card's CSD register
#if	CSD_OUTPUT_ENABLE
	printf("CSD:");
	for(i=0; i<16; i++)
	{
		SD_CSD[i] = RspBuf[i+1];
		printf(" %02bX", SD_CSD[i]);
	}
	printf("\n");
#endif

//	   //printf("SD card READ_BL_LEN = %bx\n",SD_CSD[5] & 0x0f);

	 READ_BL_LEN  = SD_CSD[5] & 0x0f;
	 BLOCK_LEN	  = 0x0001	<< READ_BL_LEN;

//	   //printf("SD card C_SIZE = %x\n",(((UINT16)SD_CSD[6]) << 10) | ((UINT16)SD_CSD[7] << 2) | ((UINT16)SD_CSD[8] >> 6));
	 C_SIZE = (((UINT16)SD_CSD[6]) << 10) | ((UINT16)SD_CSD[7] << 2) | ((UINT16)SD_CSD[8] >> 6);

//	   //printf("SD card C_SIZE_MULT = %bx\n",((SD_CSD[9] & 0x03) << 1) | (SD_CSD[10] >> 7));
	 C_SIZE_MULT = ((SD_CSD[9] & 0x03) << 1) | (SD_CSD[10] >> 7);
	 MULT = 0x0001 << (C_SIZE_MULT + 2);
//	   //printf("MULT = %x\n",MULT);
	 BLOCKNR = (UINT32)(C_SIZE +	1) * (UINT32)MULT;
//	   //printf("BLOCKNR = %lx\n",BLOCKNR);

	 G_Card_TotalCardSize = BLOCKNR * (UINT32)BLOCK_LEN;
//	   //printf("G_Card_TotalCardSize = %lx\n",G_Card_TotalCardSize);

//HAL_SDConfig(SD_12M,SD_1BIT_MODE);	// 24MHz, 1bit bus mode

		//PRINT_L1("Exit Idnetification\n");

RET_ENTRY:
	speedupSdMount = 0;
	
	return ret;
}

//patch4.5@richie@msdc plug	out	exception
//#include "uiflow.h"
//=============================================================================
//Program
//=============================================================================

UINT8 L1_SDIdentification(UINT8 mmcMode, UINT8 cardType, UINT16 BlockSize)		   //ada@0219
{
	UINT8 ret;

	//printf("fuc : 4\n");
	
	ret = sd_IdentificationCmd( mmcMode, cardType,BlockSize);
	if(ret!=0){
		return ret;
	}
	
	ret = sd_IdentificationInfoGet(mmcMode,cardType);
	
	return ret;
}
#else
//-----------------------------------------------------------------------------
//L1_SDIdentification
//-----------------------------------------------------------------------------
// input paremeters:
//		BlockSize -	data length	of one block
// return value:
//		0x00 - No error
//		0x01 -  CMD55 error
//		0x02 - ACMD41 error
//		0x03 -  CMD2  error
//		0x04 -  CMD3  error
//		0x05 -  CMD9  error
//		0x06 - ACMD51 error
//		0x07 - CRC16  error
//		0x08 - time	out	error
UINT8 L1_SDIdentification(UINT8 mmcMode, UINT8 cardType, UINT16 BlockSize)		   //ada@0219
{
	UINT8 error;
	UINT8 READ_BL_LEN;
	UINT16	BLOCK_LEN;
	UINT16	C_SIZE;
	UINT8 C_SIZE_MULT;
	UINT16	MULT;
	UINT32 BLOCKNR;
	UINT32 arg, i;
	UINT16 timeOut;
	//PRINT_L1("Enter Idnetification\n");

	HAL_FlashMode(cardType, 1, 1);
	HAL_SDInit(mmcMode);
	HAL_SDConfig(3,0, mmcMode);  //375KHz,1 bit bus
	HAL_SDBlockSize(BlockSize);
	for(i=0; i<100; i++)	//100 -> 10
	{					// Tx 80 clock for power on	sequence
		HAL_SDTxDummy();
	}

	//patch4.5@ada@Add timeout count begin
	error =	HAL_SDTxCommand(0, 0,RSP_TYPE_R1, NULL);					// Tx "CMD0"
	if (error != 0)
	{
		return 8;
	}
	//patch4.5@ada@Add timeout count end

	HAL_SDTxDummy();

	if ( mmcMode == SD_MODE ) {
		/* According to SD's specification, the worst case of power-on could
		 * be one second, while MMC's specification does not describe it.
		 */
		if(isSDHC==(UINT8)-1)
		{
			error = HAL_SDTxCommand(8, 0x000001aa,RSP_TYPE_R1, RspBuf);
            isSDHC = (error==0);
        }
	}
	else
	{
		isSDHC= 0;
	}

	arg  = isSDHC ? 0x40040000 : 0x003c0000; /* Bit 30: HCS */
	printf("%s %d, isSdVer2=%d, %s, arg=%08lX\n", __FILE__,  (unsigned short)__LINE__,\
		(UINT16)isSDHC, mmcMode==SD_MODE ? "SD" : "MMC", arg);

	timeOut = 2000;
	error   = 0;
	do {
		if(sD_CdGet()){
			return 1;
		}
		if ( mmcMode == SD_MODE ) {
			if(error==0)
			    error = HAL_SDTxCommand(55, 0, RSP_TYPE_R1, RspBuf);
			if(error==0)
    			error = HAL_SDTxCommand(41, arg, RSP_TYPE_R3, RspBuf);
			if(error) {
				printf("%s %d, SD ACMD41:FAIL, arg=%08lx\n",  __FILE__,  (unsigned short)__LINE__, arg);
				return 1;
			}
		}
		else {
			error = HAL_SDTxCommand(1, 0x003c0000, RSP_TYPE_R3, RspBuf);
			if(error) {
				printf("%s %d, MMC CMD1:FAIL\n",  __FILE__, (UINT16)__LINE__);
				return 1;
			}
		}
		timeOut--;
		if ( timeOut == 0 ) {
			printf("ACMD41 or CMD1 ERROR, Power Negotiate Fail!\n");
			return 1;
		}
		//for(i=0; i<7000; i++);	/* delay > 3 ms for some LAN-CARD */
	}while ( (RspBuf[1] & 0x80) == 0 ); /* Bit31: busy bit, low->card is busy */

	printf("tm:%u\n", timeOut); // for card error test

#if	OCR_OUTPUT_ENABLE
	printf("OCR: ");
	for ( i = 0; i < 4; i++ ) { /* Get card's OCR register */
		SD_OCR[i] = RspBuf[i + 1];
		printf("%02bX ", SD_OCR[i]);
	}
	printf("\n");
#endif

	/* Check OCR bit 30 to see if SDHC */
	if(isSDHC) {
		if(SD_OCR[0] & 0x40) {
			printf("SDHC\n");
		}
		else {
			isSDHC = 0;
		}
	}
	//sdPageShift = isSDHC ? 0 : 9; /* SD2.0 uses sector addressing, while 1.1 was byte addressing */
	sdPageShift = isSDHC ? 9 : 0; /* SD2.0 uses sector addressing, while 1.1 was byte addressing */
	sdByteMode = !isSDHC;

	if (HAL_SDTxCommand(2, 0,RSP_TYPE_R2, RspBuf) != 0)
		return 8;

#if	CID_OUTPUT_ENABLE
	printf("CID:");
	for(i=0; i<16; i++)	// get card's CID register
	{
		SD_CID[i] =	RspBuf[i+1];
		printf(" %02bX", SD_CID[i]);
	}
	printf("\n");
#endif

	if (HAL_SDTxCommand(3, 0, RSP_TYPE_R6, RspBuf) != 0)
		return 8;

	// get card's RCA register
	SD_RCA[0] = RspBuf[1];
	SD_RCA[1] = RspBuf[2];

	arg	= ((UINT32)SD_RCA[0]<<24) +	((UINT32)SD_RCA[1]<<16);
	if (HAL_SDTxCommand(9, arg, RSP_TYPE_R2, RspBuf) != 0)
		return 8;

	// get card's CSD register
#if	CSD_OUTPUT_ENABLE
	printf("CSD:");
	for(i=0; i<16; i++)
	{
		SD_CSD[i] =	RspBuf[i+1];
		printf(" %02bX", SD_CSD[i]);
	}
	printf("\n");
#endif

//	   //printf("SD	card READ_BL_LEN = %bx\n",SD_CSD[5]	& 0x0f);

	 READ_BL_LEN  =	SD_CSD[5] &	0x0f;
	 BLOCK_LEN	  =	0x0001	<< READ_BL_LEN;

//	   //printf("SD	card C_SIZE	= %x\n",(((UINT16)SD_CSD[6]) <<	10)	| ((UINT16)SD_CSD[7] <<	2) | ((UINT16)SD_CSD[8]	>> 6));
	 C_SIZE	= (((UINT16)SD_CSD[6]) << 10) |	((UINT16)SD_CSD[7] << 2) | ((UINT16)SD_CSD[8] >> 6);

//	   //printf("SD	card C_SIZE_MULT = %bx\n",((SD_CSD[9] &	0x03) << 1)	| (SD_CSD[10] >> 7));
	 C_SIZE_MULT = ((SD_CSD[9] & 0x03) << 1) | (SD_CSD[10] >> 7);
	 MULT =	0x0001 << (C_SIZE_MULT + 2);
//	   //printf("MULT =	%x\n",MULT);
	 BLOCKNR = (UINT32)(C_SIZE +	1) * (UINT32)MULT;
//	   //printf("BLOCKNR = %lx\n",BLOCKNR);

	 G_Card_TotalCardSize =	BLOCKNR	* (UINT32)BLOCK_LEN;
//	   //printf("G_Card_TotalCardSize =	%lx\n",G_Card_TotalCardSize);

//HAL_SDConfig(SD_12M,SD_1BIT_MODE);	// 24MHz, 1bit bus mode

		//PRINT_L1("Exit Idnetification\n");

	return 0x00;
}
#endif
//-----------------------------------------------------------------------------
//L1_SDCheckState
//-----------------------------------------------------------------------------
// input paremeters:
//		none
// return value:
//		0x00 - No error
//		0x01 - CMD13 error
//		0x02 - CMD7	error
//		 0x03 -	the	card is	not	in the correct satet
//		0x04 - time	out	error

UINT8 L1_SDCheckState(void)
{
	UINT8 error;
	UINT32 arg;

	arg	= ((UINT32)SD_RCA[0]<<24) +	((UINT32)SD_RCA[1]<<16);

	//patch4.5@ada@Add timeout count begin
	error =	HAL_SDTxCommand(13,arg,RSP_TYPE_R1, RspBuf);	// TX "CMD13" to get card's	state
	if (error != 0)
	{
		return 4;
	}
	//patch4.5@ada@Add timeout count end

	if((RspBuf[3] &	0x1e)==0x06) {			// if in the stand-by state, go	to the transfer	state
		//patch4.5@ada@Add timeout count begin
		error =	HAL_SDTxCommand(7, arg,RSP_TYPE_R1, RspBuf);		// TX "CMD7" to	go the the transfer	state
		if (error != 0)
		{
			return 4;
		}
		//patch4.5@ada@Add timeout count end
	}
	else
	 {
		// if not in standby state or transfer state, discard the operation
		if((RspBuf[3] &	0x1e)!=0x08)	return 0x03;
	}

	//patch4.5@ada@Add timeout count begin
	error =	HAL_SDTxCommand(13, arg,RSP_TYPE_R1, RspBuf);	// TX "CMD13" to get card's	state
	if (error != 0)
	{
		return 4;
	}
	//patch4.5@ada@Add timeout count end

	// if not in transfer state, discard the operation
	if((RspBuf[3] &	0x1e)!=0x08)
		return 0x03;

	//PRINT_L1("Check State	is finished!\n");

	return 0x00;
}


//-----------------------------------------------------------------------------
//L1_SDSetBusMode
//-----------------------------------------------------------------------------
// input paremeters:
//		BusMode	- 0: 1-bit mode, 1:	4-bit mode
// return value:
//		0x00 - No error
//		0x01 - CMD55 error
//		0x02 - ACMD42 error
//		 0x03 -	ACMD6 error
//		0x04 - time	out	error

UINT8 L1_SDSetBusMode(UINT8	BusMode, UINT8 busSpeed, UINT8 mmcMode)
{
	UINT8 error=0;
	UINT32 arg;

	 //PRINT_L1("Enter SetBusMode\n");

	if(mmcMode)
	{
		HAL_SDTxCommand(16, 512, RSP_TYPE_R1, RspBuf); /* Set block length */
		if( BusMode == SD_4BIT_MODE ){
			error = HAL_SDTxCommand(6, 0x03B70100, RSP_TYPE_R1, RspBuf);
		}
	}
	else
	{
		arg	= ((UINT32)SD_RCA[0] <<	24)	+ ((UINT32)SD_RCA[1]<<16);
		error =	HAL_SDTxCommand(55, arg, RSP_TYPE_R1, RspBuf);	//TX "CMD55"
		if(BusMode==0)
			error =	HAL_SDTxCommand(42, 1,RSP_TYPE_R1, RspBuf);	// TX "ACMD42" to connect the pull-up resistor on line 'data3'
		else
			error =	HAL_SDTxCommand(42, 0,RSP_TYPE_R1, RspBuf);	// TX "ACMD42" to disconnect	the	pull-up	resistor on	line 'data3'
		error =	HAL_SDTxCommand(55,arg,RSP_TYPE_R1, RspBuf);	// TX "CMD55"
		if(BusMode==0)
			error =	HAL_SDTxCommand(6,0,RSP_TYPE_R1, RspBuf);	// TX "ACMD6" to set the card's	to 1-bit bus
		else
			error =	HAL_SDTxCommand(6,2,RSP_TYPE_R1, RspBuf);	// TX "ACMD6" to set the card's	to 4-bit bus
	}
	if (error != 0)
	{
		return 4;
	}

	HAL_SDConfig(busSpeed, BusMode, mmcMode);
	if( !mmcMode ){
		HAL_SDTxCommand(16, 512, RSP_TYPE_R1, RspBuf); /* Set block length */
	}
	return 0x00;
}

//-----------------------------------------------------------------------------
//L1_SDEraseData
//-----------------------------------------------------------------------------
// input paremeters:
//		SAdr - erase starting address
//		EAdr - erase ending	address
// return value:
//		0x00 - No error
//		0x01 - CMD32 error
//		0x02 - CMD33 error
//		 0x03 -	CMD38 error
//		0x04 - time	out	count

#ifndef _DBG_CLR_WARNING_
UINT8 L1_SDEraseData(UINT32 SAdr, UINT32 EAdr)
{
	UINT8 error	= 0;

	//patch4.5@ada@Add timeout count begin
	UINT16 timeout_count = 0xffff;
	//patch4.5@ada@Add timeout count end

		//PRINT_L1("Enter EraseData\n");

	//patch4.5@ada@Add timeout count begin
	error =	HAL_SDTxCommand(32,SAdr,RSP_TYPE_R1, RspBuf);	// TX "CMD32" to set erase starting	address
	if (error != 0)
	{
		return 4;
	}
	//patch4.5@ada@Add timeout count end

	//patch4.5@ada@Add timeout count begin
	error =	HAL_SDTxCommand(33,EAdr,RSP_TYPE_R1, RspBuf);	// TX "CMD33" to set erase ending address

	if (error != 0)
	{
		return 4;
	}
	//patch4.5@ada@Add timeout count end

	//patch4.5@ada@Add timeout count begin
	error =	HAL_SDTxCommand(38,0,RSP_TYPE_R1, RspBuf);	// TX "CMD38" to do	the	erase operation

	if (error != 0)
	{
		return 4;
	}
	//patch4.5@ada@Add timeout count end

	return sdWaitDat0High();	// wait	until 'data0' to be	high
}
#endif /*_DBG_CLR_WARNING_*/

UINT8 L1_SDStatusGet(UINT8 mmcMode)
{
	UINT8 error=FAIL;
	UINT32 arg;
	UINT8 i;

	sdSpeedClass = 0;
	
	if(!mmcMode)
	{
		if (L1_SDCheckState() != 0x00) //xian ++ 20101109
			return error;
		
		arg	= ((UINT32)SD_RCA[0] <<	24)	+ ((UINT32)SD_RCA[1]<<16);
		error =	HAL_SDTxCommand(55, arg, RSP_TYPE_R1, RspBuf);	//TX "CMD55"
		error =	HAL_SDTxCommand(13, 0, RSP_TYPE_R1, NULL);
		if(!error){
			HAL_DmaSetSramAddr(0x0000);
			HAL_DmaDo(2,1,512/8,0,0);

			sdSpeedClass = G_ucStorData[8];
			#if 1
			/*ematic 4GB
			OCR: C0 FF 80 00
			SDHC
			CID: 1D 41 44 53 44 20 20 20 10 00 00 0D 25 00 A8 9D
			CSD: 40 0E 00 32 5B 59 00 00 1E B9 7F 80 0A 40 00 C7
			0 0 0 0 2 0 0 0 2 2 90 0 8 5 0 0
			*/
			//size= (0x1EB9+1)*512K=0xF5D00000 bytes
			if ((SD_CID[0] == 0x1D) && (SD_CID[1] == 0x41) && (SD_CID[2] == 0x44)) { // MID, OID
				if ( sdSpeedClass==2 && G_Card_TotalCardSize >= 0xf5000000 && G_Card_SectorPerCard == 0) { /*~4GB */
					sdSpeedClass = 1;
				}
			}
			/*kinston 
			OCR: C0 FF 80 00 
			SDHC 
			CID: 02 54 4D 53 41 30 38 47 06 10 C3 32 D7 00 A9 3B 
			CSD: 40 0E 00 32 5B 59 00 00 39 AF 7F 80 0A 40 00 93
			0 0 0 0 3 0 0 0 2 2 90 1 0 aa 0 0
			*/
			//size=(0x39AF+1)*1024sectors=0xE6C000 sectors
			if ((SD_CID[0] == 0x02) && (SD_CID[1] == 0x54) && (SD_CID[2] == 0x4D)) { // MID, OID
				if ( sdSpeedClass==2 && G_Card_SectorPerCard > 0xE60000 && G_Card_SectorPerCard < 0x1000000) { /*4G~8GB */
					sdSpeedClass = 1;
				}
			}
			#endif

			printf("class:%bu,%bu\n", sdSpeedClass, G_ucStorData[9]);
			#if 1
			for(i=0; i < 16/*64*/; i ++){
				if( (i & 0xf) == 0)
					printf("\n");
				printf("%bx ", G_ucStorData[i]);
			}
			printf("\n");
			#endif
		}
	}
	
	return error;
}
//-----------------------------------------------------------------------------
//L1_DMAWrSD
//-----------------------------------------------------------------------------
// input paremeters:
//		Src	 - the source of DMA
//			   0: DRAM
//			   1: CPU 4K SRAM (0x1000 ~	0x1FFF)
//			   2: forbidden	(flash itself)
//			   3: Audio
//			   4: USB
//			   5: DMA data port	(0x2300)
//		SAdr - write starting address
//		BlockCnt - number of block to be written
//		BlockSize -	data length	of write one block to SD
// return value:
//		0x00 - No error
//		0x01 - CMD24/CMD25 error
//		0x02 - SD card's CRC error
//		0x03 - CMD12 error
//		  0x04 - CRC timeout
//		0x05 - time	out	error
UINT8 L1_DMAWrSD(UINT8 Src,	UINT32 SAdr, UINT32 BlockCnt, UINT16 BlockSize)
{
	UINT32 dramaddr;
	UINT32 sdaddr;
	UINT8 error;
	UINT8 j, wrt_num;
	#if	SD_FAST_W==0
	UINT8 tmp;
	#endif
	UINT16 timeout_count = 0xffff;
	//UINT8 status;

	//PRINT_L1("Enter DMAWriteData\n");

	HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1);
	if (Src	== 0)
	{
		HAL_DmaReadDramAddr(&dramaddr);
	}

	sdaddr = SAdr;

	while(BlockCnt > 0)
	{
		if(sD_CdGet()){	
			return 1;
		}
		wrt_num   =  (BlockCnt > 32) ? 32 :  (UINT8)BlockCnt;
		BlockCnt -= (UINT32)wrt_num;
		//patch4.5@ada@Add timeout count begin
		//ENTER_CRITICAL( status ); //Add for AVI will trigger interrupt and it might delay the dummy clock of 25 CMD
		error =	HAL_SDTxCommand(25,sdaddr>>sdPageShift,RSP_TYPE_R1, RspBuf);	//TX "CMD25" to	write multiple	block data
		//EXIT_CRITICAL( status );
		if (error != 0)
		{
			return 5;
		}
		//patch4.5@ada@Add timeout count end
		for	(j = 0;	j <	wrt_num; j++)
		{
			if (Src	== 0)
			{
				HAL_DmaSetDramAddr(dramaddr);
			}

			//richie@mi0415	mark
			//HAL_DmaDo(Src,2,BlockSize,0,0);
			//richie@mi0415	begin
			if (Src	!= K_DMA_USB)
			{
				//	 sync mode
				//patch4.5@ada@Add timeout count begin
				if (Src	== 1)
				{
					HAL_DmaSetSramAddr(0x0000);
				}

				error =	HAL_DmaDo(Src, 2, BlockSize,0, 0);

				if (error != 0)
				{
					return 5;
				}
				//patch4.5@ada@Add timeout count end
			}
			else
			{
#if DOS_DEV_SUPPORT_USB
				UINT8 bi;

				//	 async mode
				//patch4.5@ada@Add timeout count begin
				error =	HAL_DmaDo(Src, 2, BlockSize,1, 0);

				if (error != 0)
				{
					return 5;
				}
				//patch4.5@ada@Add timeout count end
				if (G_bUSBSpeed == 0){
					for	(bi	= 0; bi	< 8; bi++)
					{
						while (XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0)	{  //wait for 0	in bulk	buffer
							if((sD_CdGet()) || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
						}
						XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
						while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
							if((sD_CdGet()) || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
					}
					while (XBYTE[K_USB_CLASS_OUT_BUF_SIZE] > 0)	{  //wait for 0	in bulk	buffer
						if((sD_CdGet()) || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
					}
				}
				else {
					while ((XBYTE[K_USB_CLASS_OUT_BUF_SIZE]+XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) > 0)	{  //wait for 0	in bulk	buffer
						if((sD_CdGet()) || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
					}
					XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
					while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
						if((sD_CdGet()) || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
					}
					XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
				}
#endif //DOS_DEV_SUPPORT_USB                
			}
			//richie@mi0415	end

			if (HAL_SDWaitIdle() != 0)
			{
				printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
				return 5;
			}

		#if	SD_FAST_W==0
			HAL_SDRxCRC();								 //	trigger	to read	the	Card CRC
			if (SD_TIME_OUT)	//	check CRC timeout
			{
				return 0x04;
			}

			// check the SD	card's CRC status
			HAL_SDCardCRCStat(&tmp);
			HAL_SDTxDummy();

			if (tmp	== 0)
			{
				return 0x02;
			}

			sdWaitDat0High();	// wait	until 'data0' to be	high
		#endif
			if (Src	== 0)
			{
				dramaddr +=	(UINT32)(BlockSize >> 1);
			}
		}

		//patch4.5@ada@Add timeout count begin
		error =	HAL_SDTxCommand(12,0,RSP_TYPE_R1, RspBuf);	//TX "CMD12" to	stop write
		if (error != 0)
		{
			return 5;
		}
		//patch4.5@ada@Add timeout count end
		sdWaitDat0High();	// wait	until 'data0' to be	high
		sdaddr += (UINT32)BlockSize << 5;
	}

	//PRINT_L1("Exit DMAWriteData\n");
	return error;

}

//-----------------------------------------------------------------------------
//L1_DMARdSD
//-----------------------------------------------------------------------------
// input paremeters:
//		Dst	 - the source of DMA
//			   0: DRAM
//			   1: CPU 4K SRAM (0x1000 ~	0x1FFF)
//			   2: forbidden	(flash itself)
//			   3: Audio
//		   4: USB
//		SAdr - read	starting address
//		BlockCnt - number of block to be read
//		BlockSize -	read length	of one block from SD
// return value:
//		0x00 - No error
//		0x01 - CMD17/CMD18 error
//		0x02 - data	CRC16 error
//		0x03 - CMD12 error
//		0x04 - time	out	error

UINT8 L1_DMARdSD(UINT8 Dst,	UINT32 SAdr, UINT32 BlockCnt, UINT16 BlockSize)
{
	UINT32 dramaddr;
	UINT32 sdaddr;
	//patch4.5@richie@sd sidc
	UINT8 error;
	//UINT8 tmp;
	UINT8 j, rd_num;;

	HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1);
	//printf("Enter	DMAReadData\n");
	if (Dst	== 0)
	{
		HAL_DmaReadDramAddr(&dramaddr);
	}

	sdaddr = SAdr;

	while(BlockCnt > 0)
	{
		if(sD_CdGet())	return 1;
		rd_num = (BlockCnt > 32) ? 32 : (UINT8)BlockCnt;
		BlockCnt -= (UINT32)rd_num;
		//patch4.5@ada@Add timeout count begin
		error =	HAL_SDTxCommand(18,sdaddr,RSP_TYPE_R1, NULL);
		if (error != 0)
		{
			return 5;
		}
		//patch4.5@ada@Add timeout count end

		for	(j = 0;	j <	rd_num;	j++)
		{
			//patch4.5@ada@Add timeout count begin
			if (Dst	== 1)
			{
				HAL_DmaSetSramAddr(0x0000);
				error =	HAL_DmaDo(2,	Dst, BlockSize,	0, 0);
				if (error != 0)
				{
printf("%s %u ERR\n", __FILE__, (unsigned short)__LINE__);
					return 4;
				}
			}
			//patch4.5@ada@Add timeout count end

			//richie@mi0415	begin
			else if	(Dst ==	K_DMA_USB)
			{
#if DOS_DEV_SUPPORT_USB
				#if 0
				UINT8 bi;
				//patch4.5@richie@sd sidc begin
				data UINT8 leftbs;

				leftbs = XBYTE[K_USB_CLASS_IN_BUF_SIZE];
				//patch4.5@richie@sd sidc end

				//patch4.4@yichang@change SRAM address begin
				//HAL_DmaSetSramAddr(0x0A00);
				HAL_DmaSetSramAddr(0x0C00);
				//patch4.4@yichang@change SRAM address end

				//patch4.5@ada@Add timeout count begin
				error =	HAL_DmaDo(K_DMA_FLASH, K_DMA_SRAM, BlockSize, 0,	0);

				if (error != 0)
				{
					return 4;
				}
				//patch4.5@ada@Add timeout count end
				if (G_bUSBSpeed == 0){
					for	(bi	= 0; bi	< 8; bi++)
					{
						//patch4.5@ada@Add timeout count begin
						//patch4.5@richie@sd sidc begin
                	    XBYTE[REG_Usb_BufClr] = 0x18;
						if (bi == 0)
							error =	HAL_DmaDo(K_DMA_SRAM,K_DMA_USB,64 - leftbs,0,0);
						else
							error =	HAL_DmaDo(K_DMA_SRAM,K_DMA_USB,64,0,0);
						//patch4.5@richie@sd sidc end

						if (error != 0)
						{
							return 4;
						}
						//patch4.5@ada@Add timeout count end
						//patch5.2.1@richie@msdc abnormal plug out sd begin
						while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64)&& (G_UI_USBConnect	== K_UI_USB_CONNECT)) {	//wait for 64 in bulk buffer
							if(sD_CdGet() & G_Detect_PinMux)	return 1;
						}
						XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
						while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
							if(sD_CdGet() & G_Detect_PinMux)	return 1;
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
						//patch5.2.1@richie@msdc abnormal plug out sd end
					}

					//patch4.5@richie@sd sidc begin
					if (leftbs > 0)
					{
						HAL_DmaDo(K_DMA_SRAM,K_DMA_USB,leftbs,0,0);
						while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < leftbs) &&	(G_UI_USBConnect ==	K_UI_USB_CONNECT)) {  //wait for 0 in bulk buffer
							if(sD_CdGet() & G_Detect_PinMux)	return 1;
						}
					}
					//patch4.5@richie@sd sidc end
				}
				else {
					XBYTE[REG_Usb_BufClr] = 0x18;
					error =	HAL_DmaDo(K_DMA_SRAM,K_DMA_USB,512,0,0);
					if (error != 0)
					{
						return 4;
					}
					while (((XBYTE[K_USB_CLASS_IN_BUF_SIZE] +XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8)< 512)&& (G_UI_USBConnect	== K_UI_USB_CONNECT)) {	//wait for 64 in bulk buffer
						if(sD_CdGet() & G_Detect_PinMux)	return 1;
					}
					XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
					while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
						if(sD_CdGet() & G_Detect_PinMux)	return 1;
					}
					XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
					//patch5.2.1@richie@msdc abnormal plug out sd end

				}
				#else
				UINT8 bi;

				error =	HAL_DmaDo(K_DMA_FLASH, K_DMA_USB, BlockSize, 1,	0);

				if (error != 0)
				{
					return 4;
				}
				if (G_bUSBSpeed == 0){
					for	(bi	= 0; bi	< 8; bi++)
					{
                	    //XBYTE[REG_Usb_BufClr] = 0x18;

						while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64)&& (G_UI_USBConnect	== K_UI_USB_CONNECT)) {	//wait for 64 in bulk buffer
							if((sD_CdGet() & G_Detect_PinMux)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
						}
						XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
						while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
							if((sD_CdGet() & G_Detect_PinMux)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
						}
						XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
					}

				}
				else {
					//XBYTE[REG_Usb_BufClr] = 0x18;
					while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE+1] != 0x02)&& (G_UI_USBConnect	== K_UI_USB_CONNECT)) {	//wait for 64 in bulk buffer
						if((sD_CdGet() & G_Detect_PinMux)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
					}
					XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
					while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
						if((sD_CdGet() & G_Detect_PinMux)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
					}
					XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
				}
				#endif
#endif //DOS_DEV_SUPPORT_USB                
			}
			//richie@mi0415	end
			else
			{
				if (Dst	== 0)
				{
					HAL_DmaSetDramAddr(dramaddr);
				}

				//patch4.5@ada@Add timeout count begin
				error =	HAL_DmaDo(2, Dst, BlockSize,0, 0);

				if (error != 0)
				{
					printf("%s %u ERR\n", __FILE__, (unsigned short)__LINE__);
					return 4;
				}
			}

			//patch4.5@ada@Add timeout count begin
			error =	HAL_SDWaitIdle();

			if (error != 0)
			{
				printf("%s %u ERR\n", __FILE__, (UINT16)__LINE__);
				return 5;
			}
			//patch4.5@ada@Add timeout count end

			if (SD_CRC16_OK==0)
			{
				printf("%s %d CRC16 ERR, dst=%bd a=%08lx c=%ld s=%d\n", \
					__FILE__, (UINT16)__LINE__,Dst, SAdr,BlockCnt,BlockSize);
				return 0x02;					   // check	data CRC16
			}

			if (Dst	== 0)
			{
				dramaddr +=	(UINT32)(BlockSize >> 1);
			}
		}
		HAL_SDTxDummy();
		//patch4.5@ada@Add timeout count begin
		error =	HAL_SDTxCommand(12,0,RSP_TYPE_R1, RspBuf);	// TX "CMD12" to stop read
		if (error != 0)
		{
			return 5;
		}
		#if	0
		if(sdWaitDat0High()==FALSE) {
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
			return 6;
		}
		#endif
		//patch4.5@ada@Add timeout count end
		sdaddr += (UINT32)BlockSize << 5;
	}

	return 0x00;
}

//richie@vc0422	begin
//-----------------------------------------------------------------------------
//L1_DMARdSDBulk2
//-----------------------------------------------------------------------------
// input paremeters:
//		Dst	 - the source of DMA
//			   0: DRAM
//			   1: CPU 4K SRAM (0x1000 ~	0x1FFF)
//			   2: forbidden	(flash itself)
//			   3: Audio
//		   4: USB
//		SAdr - read	starting address
//		BlockCnt - number of block to be read
//		BlockSize -	read length	of one block from SD
// return value:
//		0x00 - No error
//		0x01 - CMD17/CMD18 error
//		0x02 - data	CRC16 error
//		0x03 - CMD12 error


UINT8 L1_DMARdSDBulk2(UINT8	Dst, UINT32 SAdr, UINT32 BlockCnt, UINT16	BlockSize)
{
	UINT32 dramaddr;
	UINT32 sdaddr;
	UINT8 error;
	//UINT8 tmp;
	UINT8 j, rd_num;

	HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1);
	//printf("Enter	DMAReadData\n");
	if (Dst	== 0)
	{
		HAL_DmaReadDramAddr(&dramaddr);
	}

	sdaddr = SAdr;

	while(BlockCnt > 0)
	{
		if(sD_CdGet())	return 1;
		rd_num = (BlockCnt > 32) ? 32 : (UINT8)BlockCnt;
		BlockCnt -= (UINT32)rd_num;
		//patch4.5@ada@Add timeout count begin
		error =	HAL_SDTxCommand(18, sdaddr,RSP_TYPE_R1, NULL);
		if (error != 0)
		{
			return 5;
		}
		//patch4.5@ada@Add timeout count end

		for	(j = 0;	j <	rd_num;	j++)
		{
			//patch4.5@ada@Add timeout count begin
			if (Dst	== 1)
			{
				error =	HAL_DmaDo(2,Dst, BlockSize,	0, 0);
				if (error != 0)
				{
					return 4;
				}
			}
			//patch4.5@ada@Add timeout count end

			//richie@mi0415	begin
			else if	(Dst ==	K_DMA_USB)
			{
#if DOS_DEV_SUPPORT_USB			
				UINT8 bi;
				//patch4.4@yichang@change SRAM address begin
				//HAL_DmaSetSramAddr(0x0A00);
				HAL_DmaSetSramAddr(0x0000);
				//patch4.4@yichang@change SRAM address end

				//patch4.5@ada@Add timeout count begin
				error =	HAL_DmaDo(DMA_PATH_FLASH, DMA_PATH_SRAM, BlockSize, 0,	0);

				if (error != 0)
				{
					return 4;
				}
				//patch4.5@ada@Add timeout count end

				for	(bi=0;bi<8;bi++)
				{
					//patch4.5@ada@Add timeout count begin
					error =	HAL_DmaDo(DMA_PATH_SRAM, DMA_PATH_USB,	64,	0, 0);

					if (error != 0)
					{
						return 4;
					}
					//patch4.5@ada@Add timeout count end

					//patch5.2.1@richie@msdc abnormal plug out sd begin
					while ((XBYTE[K_USB_AP_IN_BUF_SIZE]	< 64)&&	(G_UI_USBConnect ==	K_UI_USB_CONNECT)) {  //wait for 64	in bulk	buffer
						if((sD_CdGet() & G_Detect_PinMux)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
					}
					XBYTE[REG_Usb_TransactionEn] =	K_USB_AP_IN_OPEN;//0x08;//open bulk	in
					while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_AP_IN_ACK_MASK) == 0)&& (G_UI_USBConnect	== K_UI_USB_CONNECT)) {	 //wait	pc ack
						if((sD_CdGet() & G_Detect_PinMux)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10))	return 1;
					}
					XBYTE[REG_Usb_EpAckIntSts] =	K_USB_AP_IN_ACK_RESET;//reset 0x25c2
					//patch5.2.1@richie@msdc abnormal plug out sd end
				}
#endif //DOS_DEV_SUPPORT_USB                
			}
			//richie@mi0415	end
			else
			{
				//patch4.4@yichang@change SRAM address begin
				//HAL_DmaSetSramAddr(0x0A00);
				HAL_DmaSetSramAddr(0x0000);
				//patch4.4@yichang@change SRAM address end

				//patch4.5@ada@Add timeout count begin
				error =	HAL_DmaDo(2,1,BlockSize,0,0);

				if (error != 0)
				{
					return 4;
				}
				//patch4.5@ada@Add timeout count end

				if (Dst	== 0)
				{
					HAL_DmaSetDramAddr(dramaddr);
				}

				//patch4.5@ada@Add timeout count begin
				error =	HAL_DmaDo(1,	Dst, BlockSize,	0, 0);

				if (error != 0)
				{
					return 4;
				}
				//patch4.5@ada@Add timeout count end
			}

			//patch4.5@ada@Add timeout count begin
			error =	HAL_SDWaitIdle();

			if (error != 0)
			{
				return 5;
			}
			//patch4.5@ada@Add timeout count end

			if (SD_CRC16_OK==0)
			{
printf("%s %d CRC16 ERR\n", __FILE__, (UINT16)__LINE__);
				return 0x02;					   // check	data CRC16
			}

			if (Dst	== 0)
			{
				dramaddr +=	(UINT32)(BlockSize >> 1);
			}
		}
		HAL_SDTxDummy();
		//patch4.5@ada@Add timeout count begin
		error =	HAL_SDTxCommand(12, 0,RSP_TYPE_R1, RspBuf);	//TX "CMD12" to	stop read
		if (error != 0)
		{
			return 5;
		}
		//patch4.5@ada@Add timeout count end
		sdaddr += (UINT32)BlockSize << 5;
	}

	return 0x00;
}
//richie@vc0422	end
//#pragma disable //disable interrupt
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

UINT8 L1_SDUsb2CardDmaWait(UINT32 wrt_num)
{
	UINT8 ret = 0;
	UINT32 bi;
	
	if (G_bUSBSpeed == 0){
		for	(bi	= 0; bi	< (UINT32)(wrt_num<<3); bi++)
		{
			while ((((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) & 0xff00)) > 0)	{  //wait for 0	in bulk	buffer
				if(sD_CdGet() || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
					return 1;
				}
			}
			XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
			while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
				if(sD_CdGet() || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
					return 1;
				}
			}
			XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
		}
	}
	else {
		for	(bi	= 0; bi	< (wrt_num); bi++)
		{
			while ((((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_OUT_BUF_SIZE+1]<<8) & 0xff00)) > 0)	{  //wait for 0	in bulk	buffer
				if(sD_CdGet() || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
					return 1;
				}
			}
			XBYTE[REG_Usb_BufClr] =	K_USB_CLASS_OUT_OPEN;
			if (G_USBReset == 0x00){
				XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_OUT_OPEN;//0x02;//open bulk	out
			}
			while ((XBYTE[REG_Usb_EpAckIntSts] &	K_USB_CLASS_OUT_ACK_MASK) == 0)	{  //wait pc ack
				if(sD_CdGet() || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
					return 1;
				}
				if (G_USBReset == 0x01){
					XBYTE[REG_Usb_BufClr] = K_USB_CLASS_OUT_OPEN;
					/* exit while loop */
					break;
				}
			}
			XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_OUT_ACK_RESET;//reset 0x25c2
			/* exit for loop */
			if (G_USBReset == 0x01){
				break;
			}
		}
		
		while(XBYTE[REG_Dma_DmaCmp] != 0x01)
		{
			if(sD_CdGet() || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
				return 1;
			}
			if (G_USBReset == 0x01){
				XBYTE[REG_Dma_DmaIdle] = 0x01;
				XBYTE[REG_Dma_DmaIdle] = 0x00;
				/* exit while loop */
				break;
			}

		}
		/* exit while loop */
		if (G_USBReset == 0x01){
			return 2;
		}
	}
	
	return ret;
}

UINT8 L1_SDCard2UsbDmaWait(UINT32 rd_num)
{
	UINT8 ret = 0;
	UINT32 bi;
	
	if (G_bUSBSpeed == 0){
		for	(bi	= 0; bi	< (rd_num<<3); bi++)
		{
			//while ((XBYTE[K_USB_CLASS_IN_BUF_SIZE] < 64)&& (G_UI_USBConnect	== K_UI_USB_CONNECT)) {	//wait for 64 in bulk buffer
			while ((((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8) & 0xff00)) < 64)	{  //wait for 0	in bulk	buffer
				//printf("size=%x\n",(UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] + (UINT16)(XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8));
				if(sD_CdGet() || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
					return 1;
				}
			}
			XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
			while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
				if(sD_CdGet()||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
					return 1;
				}
			}
			XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
		}

	}
	else {
		for	(bi	= 0; bi	< rd_num; bi++)
		{
			while ((((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] & 0x00ff)+ (((UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8) & 0xff00)) < 512)	{  //wait for 0	in bulk	buffer
				//printf("size=%x\n",(UINT16)XBYTE[K_USB_CLASS_IN_BUF_SIZE] + (UINT16)(XBYTE[K_USB_CLASS_IN_BUF_SIZE+1]<<8));
				if(sD_CdGet() || (G_UI_USBConnect == K_UI_USB_DISCONNECT)||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
					return 1;
				}
			}
			XBYTE[REG_Usb_TransactionEn] =	K_USB_CLASS_IN_OPEN;//0x01;//open bulk in
			while (((XBYTE[REG_Usb_EpAckIntSts] & K_USB_CLASS_IN_ACK_MASK) == 0)&& (G_UI_USBConnect == K_UI_USB_CONNECT)) {  //wait pc ack
				if(sD_CdGet()||((XBYTE[REG_Usb_Status] & 0x10) == 0x10)){
					return 1;
				}
			}
			XBYTE[REG_Usb_EpAckIntSts] =	K_USB_CLASS_IN_ACK_RESET;//reset 0x25c2
		}
	}
	return ret;
}

void sdRdPhaseCheck(void)
{
	/* fixed at 0x08 */
	//xian -- 20100428, move to hal_sd.c
	//G_SD_ReadPhase = 0x08;
}

/**********************************************************************/
/* check if it is a special card need sent dummy clock after data0 return to high*/
/*********************************************************************/
code UINT8 specialCardID[][SD_CID_CMP_NUM]={{0x00,0x20,0x20,0x4E,0x2F,0x41,0x20,0x20}};
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

void sd_PwrOnMountSpeedupEn(UINT8 en)
{
	speedupSdMount = en;
}

UINT8 SD_HwInitialize(UINT8 cardType)
{
	//patch	4.4-7@wyeo@remove debug	message
	//M_Card_Printf("SD	card start	the	identification process\n");	
	UINT32 cSize,nrSects,arg;
	UINT32 tTAAC,tNSAC,busSpeed, busMode;
	UINT16 cSizeMult;
	UINT8 readBlkLen, rwFactor,	tmp, specVer, err;
	UINT8 icVersion=0; //A version

	//printf("fuc : 1\n");

	if(speedupSdMount){
		tmp = sd_IdentificationInfoGet(SD_MODE,cardType);
	}
	else{
		isMMC = 0;
		isSDHC = -1;
		G_SD_MultiSecNum = SD_MULTI_RW_MAX;//mantis-27450
		
		tmp = L1_SDIdentification(SD_MODE, cardType, K_Card_PageSize);
	}
	
//	HAL_GlobalReadGTimer(&t2);
//	printf("##time=%ld\n",t2-t1);
	if(tmp != 0)	// not SD ?
	{
		if(MMC_SUPPORT_GET()){
			tmp = L1_SDIdentification(MMC_MODE, cardType, K_Card_PageSize);
		}
		if(tmp != 0)	// not MMC ?
		{
			//printf("%s %u, Can't detect	SD/MMC\n", __FILE__, (UINT16)__LINE__);
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
		printf("MMC Ver=%bu ", specVer);
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

		HAL_SDHighSpeedStsSet(0);
		icVersion = HAL_GlobalICVerGet();
		if(specVer != SD_SPEC_VER_10 && icVersion != 0) {
			G_SD_UnderWrite = 0;
			err = HAL_SDTxCommand(6, 0x00FFFF01, RSP_TYPE_R1, RspBuf);
			if(!err){
				HAL_DmaSetSramAddr(0x0000);
				HAL_DmaDo(2,1,512/8,0,0);
				#if	0
				printf("CMD6: ");
				for(tmp=0; tmp<17; tmp++)
					printf("%02bX ", G_ucStorData[tmp]);
				printf("\n");
				#endif				
				if( G_ucStorData[13] & 0x02) {// 2:bit2, function 1 is support
					printf("HiSpeed? ");
	
					HAL_SDTxCommand(6, 0x80FFFF01, RSP_TYPE_R1, RspBuf);
					HAL_DmaSetSramAddr(0x0000);
					HAL_DmaDo(2,1,512/8,0,0);
					if((G_ucStorData[16] & 0x0F) == 0x01) {// 1:function 1(high speed is enable)
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
	if(isSDHC && (HAL_SDHighSpeedStsGet() == 1)){ //xian ++ 20100715, maybe, some SDCH dont support hispeed
		busSpeed = 48000L;
	}
	if((busSpeed >=	48000L)	&& (!isMMC || specVer>=4))
	{
#if 1//TAG_TODO
		printf("48MHz ");
		busSpeed = SD_48M;
	    //XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = 0x08;
#else
		printf("24MHz (48MHz is not OK)");
		busSpeed = SD_24M;
#endif
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

	//Get SD Speed Class
	L1_SDStatusGet(isMMC); // xian move here, 20101109, some card err(panasonic 2G), #41618
	
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
		printf("busMode=%bu\n", (UINT8)busMode);
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

/************************************************************************/
/**/
/************************************************************************/
/**
 * @fn        void sp1kMmcSupportSet(UINT8 val)
 * @brief     Set if the MMC is supported.
 * @param     [in] val	The val is 1 if need support mmc, else set val to 0.
 * @retval    NONE
 * @see       NULL
*/
void sp1kMmcSupportSet(UINT8 val)
{
	MMC_SUPPORT_SET(val);
}
/**
 * @fn        UINT8 sp1kSDSpeedClassGet(void)
 * @brief     Get SD speed class
 * @param     NONE
 * @retval    return = Speed class, the value is SD_SPEED_CLASS0, SD_SPEED_CLASS2,SD_SPEED_CLASS4,SD_SPEED_CLASS6.
 * @see       SD_SPEED_CLASS_e
*/
UINT8 sp1kSDSpeedClassGet(void)
{	
	return sdSpeedClass;	//mark for size switch if support HD
}
#endif
