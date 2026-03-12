/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		HAL_SD.c

Abstract:

		Module related to HAL NAND flash	functions

Environment:

		Keil C51 Compiler

Revision History:

		11/12/2001		WZH	   created
--*/



//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "hal.h"
#include "dbg_def.h"
#include "sd_def.h"
#include "gpio.h"
#include "hal_sd.h"
#include "reg_def.h"

#define __FILE		__FILE_ID_HAL_SD__

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
static xdata UINT8 G_SD_HiSpeed;
static xdata UINT8 G_SD_Phase;
static xdata UINT8 G_SD_ReadPhase;
xdata UINT8 G_SD_WritePhase;

extern UINT8 RspBuf[];
extern UINT8 HAL_GlobalDGPIOModeGet(void);
extern UINT8 HAL_GlobalICVerGet(void);

//=============================================================================
//Program
//=============================================================================
/**
 * @fn        void HAL_SDInit(UINT8 mmcMode)
 * @brief     SD interface initialization
 * @param     [in] mmcMode
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_SDInit(UINT8 mmcMode)
{
	G_SD_HiSpeed = 0;
	G_SD_Phase = 0;
	G_SD_ReadPhase = 0x00;
	G_SD_WritePhase = 0;
	
	#if	SD_DAT_PULL_HI==0
	XBYTE[0x20A0] &= 0x0F;	//fmgpio[5~8] pull high disable
	#else
	XBYTE[0x20D1] |= 0x40;	//xian ++ 20100604
	#endif
	#if	SD_CMD_PULL_HI==0
	XBYTE[0x20A1] &= 0xFE;	//fmgpio[9] pull high disable
	#endif
	XBYTE[REG_Fm_Mediatype_rst] |= 0x40;	 //	MediaType =	SD,	SD reset
	XBYTE[REG_Fm_SD_CRCRst] = 0x01;	 //	CRC	reset
	XBYTE[REG_Fm_WaitRspTime] = 0xff;	 //	Rsp	timer setting
	XBYTE[REG_Fm_WaitCRCTime] = 0x08;	 //	CRC	timer setting
	XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] = 0x60;	 //	SDfreq = 375KHz, DataBusWidth =	1 bit,
							 //	RspType	= 48 bits, Enable Rsp timer	& CRC timer, SD	card mode
	XBYTE[REG_Fm_autorsp_dummy_datarspen] = 0;	/* DEBUG BY dlchiou - 07/11/01 17:45:07 */
	XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_Phase;		//375khz should use deault value , eric@20080328
	#if SD_CRC_INT_EN
	XBYTE[REG_Fm_intEn] |= 1 << 3;	//SD controller interrupt enable
    #endif
	#if SD_IDLE_INT_EN
	XBYTE[REG_Fm_intEn] |= 1 << 4;	//SD controller interrupt enable
    #endif

	XBYTE[REG_Fm_mbwen_ren] = 0;
	XBYTE[REG_Fm_mbwen_ren] |= 1;//SD_BLOCK_FAST_W
	XBYTE[REG_Fm_mbwen_ren] |= 2;//SD_BLOCK_FAST_R

	if(mmcMode){
		XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] |= 0x80;
	}
}

/**
 * @fn        void HAL_SDConfig(UINT8 OpFreq, UINT8 DataBusWidth, UINT8 mmc)
 * @brief     Set SD operation frequency, data bus width, mmc mode
 * @param     [in] OpFreq 
 * @				0:24MHz,	1:12MHz, 	2:6MHz,
 * @				3:375KHz, 	4:48MHz,	5:16MHz
 * @param     [in] DataBusWidth
 * @				0:1 bit  data bus,
 * @				1:4 bits data bus
 * @param     [in] mmc
 * @				0:sd mode,	1:mmc,
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_SDConfig(UINT8 OpFreq, UINT8 DataBusWidth, UINT8 mmc)
{
	UINT8 tmp;
	tmp	= XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode];
	tmp	= (tmp & 0xf0)|(OpFreq & 0x07)|((DataBusWidth &	0x01) << 3);
	if( mmc ){
		tmp |= 0x80;
	}
	else{
		tmp &= 0x7f;
	}
	/* for 1628 48MHz */
	if ((OpFreq & 0x07) == 0x04)
	{
		//xian -- for 48MHz work around, ++ high speed for B version
		if(HAL_GlobalDGPIOModeGet() == 0)
		{
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] |= 0x08;  //xian 20100608
		}
		else
		{
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] |= 0x10;  //xian 20100608
		}

	}
	else 
	{
		XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] =	0x00;
	}
	XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] =	tmp;
}

/**
 * @fn        void HAL_SDBlockSize(UINT16 BlockSize)
 * @brief     Set	SD data	length of read/write one block
 * @param     [in] BlockSize the length of read/write one block
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_SDBlockSize(UINT16 BlockSize)
{
	XBYTE[REG_Fm_DataLen_Low] =	(UINT8)(BlockSize);
	XBYTE[REG_Fm_DataLen_High] =	READ8(BlockSize,0);
}

/**
 * @fn        UINT8 HAL_SDTxCommand(UINT8 cmd, UINT32 arg, UINT8 RspType, UINT8 *Buff)
 * @brief     Send command to SD card
 * @param     [in] cmd
 * @param     [in] arg
 * @param     [in] RspType
 * @param     [in] Buff
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
//#pragma disable //disable interrupt
UINT8 HAL_SDTxCommand(UINT8 cmd, UINT32 arg, UINT8 RspType, UINT8 *Buff)
{
	UINT16 timeout_count = 0xffff;
	UINT8 err=0, i, crc7;

	XBYTE[REG_Fm_Mediatype_rst] |= 0x40;

	XBYTE[REG_Fm_SD_CmdBuf_1] =	cmd | 0x40;
	XBYTE[REG_Fm_SD_CmdBuf_2] =	READ8(arg,0);
	XBYTE[REG_Fm_SD_CmdBuf_3] =	READ8(arg,1);
	XBYTE[REG_Fm_SD_CmdBuf_4] =	READ8(arg,2);
	XBYTE[REG_Fm_SD_CmdBuf_5] =	READ8(arg,3);

	#if GET_RSP_FOR_CMD17_CMD18
	if(Buff==NULL && (cmd==17 || cmd==18)){
		Buff = RspBuf;
	}
	#endif

	#if SD_CMD_DEBUG
	printf("%s %d, cmd=%02d arg=%08lx type=%d buf=%04x ", \
		__FILE__, (UINT16)__LINE__, (UINT16)cmd, arg, (UINT16)RspType, Buff);
	#endif

	if(Buff == (UINT8 *)NULL)	//no responce
	{
		//XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] &= ~(1<<5);		
		if( G_SD_HiSpeed == 1 ){//if High spped( = 48Mhz )
			i = XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel];
			// if 48Mhz high speed, change to read phase here 
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_WritePhase;
			while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != G_SD_WritePhase );
		}
		XBYTE[REG_Fm_Tx_Rx_trigger] = 0x01; // trigger to TX command
		//err = HAL_SDWaitIdle();
		err = HAL_SDCmdWaitIdle();
		if( G_SD_HiSpeed == 1 ){//if High spped( = 48Mhz )
			// if 48Mhz high speed, change to read phase here 
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = i;
			while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != i );
		}
		if(!(cmd==17 || cmd==18) && err==0)
			err = HAL_SDTxDummy();
	        #if SD_CMD_DEBUG
	            printf("\n");
	        #endif
		if(err)
		{
			XBYTE[REG_Fm_Mediatype_rst] |= 0x40;   // MediaType = SD, SD reset
			XBYTE[REG_Fm_SD_CRCRst] = 0x01;   // CRC reset
		    printf("#%d, CMD%bd NG!!\n",(UINT16)__LINE__, cmd);
		}
		return err;
	}
	if(RspType != RSP_TYPE_R2)	//48 bits
	{
		if( G_SD_HiSpeed == 1 ){//if High spped( = 48Mhz )
			i = XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel];
			XBYTE[REG_Fm_autorsp_dummy_datarspen] |= 0x04;	//Auto trigger dummy clk after rsp complete
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_WritePhase;
			while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != G_SD_WritePhase );
			XBYTE[REG_Fm_Tx_Rx_trigger] = 0x01;	//trigger to TX command
			//err = HAL_SDWaitIdle();
			err = HAL_SDCmdWaitIdle();
			// if 48Mhz high speed, change to read phase here 
//xian -- for 48MHz work around, ++ high speed for B version
#if 1
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = i;//G_SD_ReadPhase;
			while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != i );			
			//if(HAL_GlobalDGPIOModeGet() == 0)
			if(HAL_GlobalICVerGet() == 0)				
			{
				XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] |= 0x08;  //xian 20100608
			}
			else
			{
				XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] |= 0x10;  //xian 20100608
			}
#else
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_ReadPhase;
			while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != G_SD_ReadPhase );			
#endif

			XBYTE[REG_Fm_Tx_Rx_trigger]  =  0x02;	// start receiving response
			//err = HAL_SDWaitIdle();
			err = HAL_SDCmdWaitIdle();

			// if 48Mhz high speed, change to write phase here
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = i;
			while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != i );

			if((XBYTE[REG_Fm_SD_Status] & 0x40)==0x40)
			{
				//printf("%s %u XBYTE[REG_Fm_SD_Status]=%02bX\n", __FILE__, (UINT16)__LINE__,XBYTE[REG_Fm_SD_Status]);
				__ASSERT(__FILE, __LINE__, 1);
				err |= 0x02;
			}
			for(i=0; i<5; i++)
				Buff[i] = XBYTE[0x244D+i];
			crc7 = XBYTE[0x244D+5];
			XBYTE[REG_Fm_autorsp_dummy_datarspen] &= ~0x04;	//disable auto dummy clk after rsp complete
		}
		else
		{// if Normal speed, use auto cmd ( auto rsp + dummy clock
			XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] &= ~(1<<5);
			XBYTE[REG_Fm_autorsp_dummy_datarspen] |= 0x06;	//Auto trigger rxrsp & dummy clk after cmd complete
			XBYTE[REG_Fm_Tx_Rx_trigger] = 0x01;	//trigger to TX command
			//err = HAL_SDWaitIdle();
			err = HAL_SDCmdWaitIdle();
			XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] |= (1<<5);
			for(i=0; i<5; i++)
				Buff[i] = XBYTE[0x244D+i];
			crc7 = XBYTE[0x244D+5];
			XBYTE[REG_Fm_autorsp_dummy_datarspen] &= ~0x06;	//disable auto trigger rxrsp & dummy clk after cmd complete
		}
	}
	else	//136 bits
	{
		XBYTE[REG_Fm_autorsp_dummy_datarspen] |= 0x04;	//Auto trigger dummy clk after rsp complete
		XBYTE[REG_Fm_Tx_Rx_trigger] = 0x01;	//trigger to TX command
		//err = HAL_SDWaitIdle();
		err = HAL_SDCmdWaitIdle();

		XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] |=  0x10; // Set response type to 17 bytes
		XBYTE[REG_Fm_Tx_Rx_trigger]  =  0x02;	// start receiving response
		timeout_count = 0xffff;
		while((XBYTE[REG_Fm_SD_Status] & 0x42)==0) {	// wait	until response buffer full or err
			if(sD_CdGet()){	
                return 1;
             }
			timeout_count --;
			if( timeout_count == 0 ){
				break;
			}

		}
		if((XBYTE[REG_Fm_SD_Status] & 0x40)==0x40)
		{
			//printf("%s %u XBYTE[REG_Fm_SD_Status]=%02bX\n", __FILE__, (UINT16)__LINE__,XBYTE[REG_Fm_SD_Status]);
			__ASSERT(__FILE, __LINE__, 1);
			err |= 0x02;
		}
		// IC rsp buffer only has 6 bytes (48 bits)
		for(i=0; i<6; i++)
			Buff[i] = XBYTE[0x244D+i];//after read 0x2452, receive rsp continue

		for(i=6; i<17; i++)
		{
			timeout_count = 0xffff;
			while((XBYTE[REG_Fm_SD_Status] & 0x42)==0) {	// wait	until response buffer full or err
				if(sD_CdGet()){
					return 1;
				}
				timeout_count --;
				if( timeout_count == 0 ){
					break;
				}
			}
			if(XBYTE[REG_Fm_SD_Status] & 0x40)	//	timeout
			{
				//printf("%s %u\n", __FILE__, (UINT16)__LINE__);
				__ASSERT(__FILE, __LINE__, 1);
				err |= 0x04;
			}
			Buff[i] = XBYTE[0x2452];//read this reg for 6 ~ 17
		}
		XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] &= ~0x10;	// Set response type to 6 bytes
		XBYTE[REG_Fm_autorsp_dummy_datarspen] &= ~0x04;	//disable auto dummy clk after rsp complete
	}
	// use auto dummy clock after manul rsp complete, so don't need to trigger dummy clock 
	//err = HAL_SDTxDummy();

	if((RspType==RSP_TYPE_R1) || (RspType==RSP_TYPE_R6))
	{
		#if	0
		if(crc7 != XBYTE[REG_Fm_CRC7buf])
		{
			err |= 0x08;	 //	CRC7 error
			//printf("%s %d <CMD CRC7 ERR>\n", __FILE__, (UINT16)__LINE__);
			__ASSERT(__FILE, __LINE__, 1);
		}
		#endif
		if ( Buff[0] != cmd ) {
			//printf(" #%d,ret=%Bd ",  (UINT16)__LINE__, Buff[0]);
			__ASSERT(__FILE, __LINE__, 1);
			err |= 0x10;
		}
	}
	else {
		if ( Buff[0] != 0x3f) {	/* Response	R2 or R3 */
			printf(" #%u, ",  (UINT16)__LINE__);
			err |= 0x10;
		}
	}
	if(err)
	{
		XBYTE[REG_Fm_Mediatype_rst] |= 0x40;   // MediaType = SD, SD reset
		XBYTE[REG_Fm_SD_CRCRst] = 0x01;   // CRC reset
	    printf("CMD%bd NG!!\n",cmd);
	}
	#if SD_CMD_DEBUG
	else {
		printf("\n");
	}
	#endif
	return err;
}

/**
 * @fn        UINT8 HAL_SDTxDummy(void)
 * @brief     Trigger	hardware to	transmit 8 dummy clock cycles
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
//#pragma disable
UINT8 HAL_SDTxDummy(void)
{
	//UINT16 retry=0;
	//HAL_SDWaitIdle();
	HAL_SDCmdWaitIdle();
	XBYTE[REG_Fm_Tx_Rx_trigger] =	0x20;	//send dummy clk
	//return HAL_SDWaitIdle();
	return HAL_SDCmdWaitIdle();
}

/**
 * @fn        UINT8 HAL_SDWaitIdle(void)
 * @brief     Wait SPCA1528 SD	interface to idle state
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_SDWaitIdle(void)
{
	//UINT32 timeout_count = 0x3FFFF;
	UINT32 timeout_count;
	timeout_count = (XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] & 0x07)==3 ? 0x0FFF : 0xFFFF;
#if	SD_IDLE_INT_EN	
	while ((XBYTE[REG_Fm_evt] &	(1<<4)) == 0x00)
	{
		if(sD_CdGet()){			
			return 1;
		}
		if (timeout_count >	0){
			timeout_count--;
		}
		else{
			//printf("HAL_SDWaitIdle() <TIME_OUT(%u)>\n", (UINT16)__LINE__);
			__ASSERT(__FILE, __LINE__, 1);
			return HALK_ERROR_GENERAL;
		}
	}
	XBYTE[REG_Fm_evt] = 1 << 4;
#else
	while ((XBYTE[REG_Fm_SD_Datastate_CRDSts] &	0x0F) != 0x00)
	{
		if(sD_CdGet()){
			return 1;
		}
		if (timeout_count >	0)
		{
			timeout_count--;
		}
		else
		{
			//printf("%s %d, HAL_SDWaitIdle ERR\n", __FILE__, (UINT16)__LINE__);
			__ASSERT(__FILE, __LINE__, 1);
			return HALK_ERROR_GENERAL;
		}
	}
#endif
	return HALK_SUCCESS;

	//PRINT_HAL("HAL_SDWaitIdle: Exit	HAL_SDWaitIdle\n");
}
//patch4.5@ada@Add timeout count end



/**
 * @fn        UINT8 HAL_SDCmdWaitIdle(void)
 * @brief     Wait SPCA1628 SD	interface to idle state
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
#pragma disable
UINT8 HAL_SDCmdWaitIdle(void)
{
	//UINT32 timeout_count = 0x3FFFF;
	UINT32 timeout_count;
	timeout_count = (XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode] & 0x07)==3 ? 0x0FFF : 0xFFFF;
#if	SD_IDLE_INT_EN
	while ((XBYTE[REG_Fm_evt] &	(1<<4)) == 0x00)
	{
		if(sD_CdGet()){
			return 1;
		}
		if (timeout_count >	0){
			timeout_count--;
		}
		else{
			//printf("HAL_SDWaitIdle() <TIME_OUT(%u)>\n", (UINT16)__LINE__);
			__ASSERT(__FILE, __LINE__, 1);
			return HALK_ERROR_GENERAL;
		}
	}
	XBYTE[REG_Fm_evt] = 1 << 4;
#else
	while ((XBYTE[REG_Fm_SD_Cmdstate] &	0x0F) != 0x00)
	{
		if(sD_CdGet()){
			return 1;
		}
		if (timeout_count >	0){
			timeout_count--;
		}
		else{
			//printf("%s %d, HAL_SDWaitIdle ERR\n", __FILE__, (UINT16)__LINE__);
			__ASSERT(__FILE, __LINE__, 1);
			return HALK_ERROR_GENERAL;
		}
	}
#endif
	return HALK_SUCCESS;

	//PRINT_HAL("HAL_SDWaitIdle: Exit	HAL_SDWaitIdle\n");
}

/**
 * @fn        UINT8 HAL_SDDram(UINT32 dramAddr, UINT32 nByte1, UINT8 read)
 * @brief     HAL_SDDram
 * @param     [in] dramAddr
 * @param     [in] nByte1
 * @param     [in] read
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_SDDram(UINT32 dramAddr, UINT32 nByte1, UINT8 read)
{
    UINT32 timeout_count = 0xFFFF;
    UINT8 status;
    
	#if DRAM_ASSERT
	if (dramAddr <= 0x40000){
		printf("@ASSERT 0x%x %d\n",__FILE, (UINT16)__LINE__);
		while(1);
	}
	#endif

    ENTER_CRITICAL(status);

    XBYTE[REG_Fm_evt] = 0x20;                      //clr dma done evt 

    if(read)
        XBYTE[REG_Fm_dramfmread_dmasel] = 0x01;                       //read mode
    else 
        XBYTE[REG_Fm_dramfmread_dmasel] = 0x00;                       //write mode

    nByte1 --;
    XBYTE[REG_Fm_drambytecnt_Low] = READ8(nByte1, 3);   
    XBYTE[REG_Fm_drambytecnt_High] = READ8(nByte1, 2);

    XBYTE[REG_Fm_datstraddr_0] = READ8(dramAddr, 3);
    XBYTE[REG_Fm_datstraddr_1] = READ8(dramAddr, 2);
    XBYTE[REG_Fm_datstraddr_2] = READ8(dramAddr, 1);
    XBYTE[REG_Fm_datstraddr_3] = READ8(dramAddr, 0)&0x01;

    XBYTE[REG_Fm_dramstr] = 1;  //triger
    while ((XBYTE[REG_Fm_evt] & 0x20) == 0x00)
    {
        if(sD_CdGet()){
			return 1;
        }
        if(timeout_count >	0){
            timeout_count--;
        }
        else{
            //printf("%s %d, HAL_SDWaitIdle ERR\n", __FILE__, (UINT16)__LINE__);
			printf("HAL_SDReadDMA: %bx\n", XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode]);
            EXIT_CRITICAL(status);
            return HALK_ERROR_GENERAL;
        }
    }
 
    EXIT_CRITICAL(status);
    XBYTE[REG_Fm_evt] = 0x20; //clr
    
    return HALK_SUCCESS;
}

/**
 * @fn        void HAL_SDPhaseChange(UINT8 sel)
 * @brief     HAL_SDPhaseChange
 * @param     [in] sel
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_SDPhaseChange(UINT8 sel)
{
	if( G_SD_HiSpeed == 1 ){//if High spped( = 48Mhz )
		// if 48Mhz high speed, change to read phase here 
		if(sel == HAL_SD_PHASE_HI_WRITE){
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_WritePhase;
			while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != G_SD_WritePhase );
		}
		else if(sel == HAL_SD_PHASE_HI_READ){ //not used
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_ReadPhase;
			while( XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] != G_SD_ReadPhase );
//xian -- for 48MHz work around, ++ high speed for B version
			if(HAL_GlobalDGPIOModeGet() == 0)
			{
				XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] |= 0x08;  //xian 20100608
			}
			else
			{
				XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] |= 0x10;  //xian 20100608
			}
		}
		else{
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_Phase;
		}
	}
}

/**
 * @fn        void HAL_SDHighSpeedStsSet(UINT8 en)
 * @brief     HAL_SDHighSpeedStsSet
 * @param     [in] en
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_SDHighSpeedStsSet(UINT8 en)
{
	G_SD_HiSpeed = en;
}

/**
 * @fn        UINT8 HAL_SDHighSpeedStsGet(void)
 * @brief     HAL_SDHighSpeedStsGet
 * @param     NONE
 * @retval    return 1 if high speed
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_SDHighSpeedStsGet(void)
{
	return G_SD_HiSpeed;
}

