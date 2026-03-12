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
 * @file      usbmsdc2.c
 * @brief     Module related to Bulk-Only Transport of USB Mass Storage Device Class
 * @author    Chi-Yeh Tsai
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "usbopt.h"
#if (USBMSDC_OPTION == 2)
#include "usbmsdc2.h"
#include "ctlsetup.h"
#include "bulkout.h"
#include "main.h"
#include "setmode.h"
#include "scsi.h"
#include "cardlink.h"
//richie@0114
#include "smccon.h"
#include "smcvar.h"
// WWW1
#include "dosvar.h"
//richie@0315
#include "msdc_dma.h"
//patch4.5@richie@mlun
#include "smcfunc.h"
#include "initio.h"
//patch5.1@richie@mlunsn begin
#include "sdramhw.h"
#include "dramfunc.h"
//patch5.1@richie@mlunsn en
#include "dos32.h"
#include "disk.h"
#include "gpio.h"
#include "reg_def.h"
#include "sp1k_gpio_api.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define MWEBTOOL_ENABLE 0
#define K_CARD_SUPPORT_MAX	5

#define K_MLUN_NANDF_LUN_POS	0
#define K_MLUN_CFC_LUN_POS		1
#define K_MLUN_NANDF_RSV_LUN_POS  1 //there's no CF card supported
#define K_MLUN_SDF_LUN_POS		2
#define K_MLUN_SMC_LUN_POS		3
#define K_MLUN_DOSRAM_LUN_POS	4
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct _MLUN_SAVED_XDATA
{	
	UINT16	mediaBehavior; 			//G_Media_Behavior;
	#if (NANDF_OPTION == 1 && SMC_OPTION ==1)
	UINT32 	smcBankBufferAddr;
	//WORD	cardToatalSizeMB; 		//G_Card_TotalSizeMB;
	//WORD    cardBlockSize;			//G_Card_BlockSize;
	//BYTE    cardBlockSizeShift;		//G_Card_BlockSizeShift;
	UINT8    smcCardTotalBank;		//G_Card_TotalBank;
	UINT8    smcCardCurrBank;		//G_Card_CurrBank;
	UINT16    smcCardBlackExtSize;	//G_Card_BlockExtSize;
	UINT16    smcCardBlockWholeSize; 	//G_Card_BlockWholeSize;
	UINT16    smcCardpageperBlock;	//G_Card_PagePerBlock; // @012403@wyeo@support 256M nand
	UINT32   smcPhysicalbanlSize; 	//G_Card_PhysicalBankSize;
	
	UINT16    smcAllocateTableSize; 	// G_SMC_AllocatTableSize;
	UINT8    smcEntryTAbleSize; 		//G_SMC_EntryTableSize;
	
	UINT32   smcCisBlockAddr;		//G_SMC_CisBlockAddress;
	
	UINT8    smcEccByte[K_SMC_EccByteSize];	//G_SMC_EccByte[K_SMC_EccByteSize];
	UINT8	smcEccErr; 						//G_SMC_EccErr = 0;
	UINT16	smcErrCode;						//G_SMC_ErrCode = 0;
	UINT16    smcFirstCISFound;				//G_SMC_FirstCISFound;	// K_Check_Nand_Bad_Block_Infomation
	UINT32	smcEccErrCount;					//G_SMC_EccErrCount;
	UINT32	smcEcc1BitErrCount; 			//G_SMC_Ecc1BitErrCount;
	UINT32	smcEcc2BitErrCount;				//G_SMC_Ecc2BitErrCount;
	
	UINT16    nandReserveBlockTotal; 			//G_NAND_ReservBlockTotal; 
	UINT16    nandReserveBlockMax;			//G_NAND_ReservBlockMax;
	UINT16    smcCisBlockFound; 				//G_SMC_CisBlockFound;
	UINT32   smcNewCisBlockAddr;				//G_SMC_NewCisBlockAddress;

	#if 1//(K_HAVE_HAL)
	UINT8   smcAddrMode; //G_SMC_AddrMode;
	#endif
	
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
	UINT8	smcBuildBankMap[K_SMC_BUILD_BANK_MAP_SIZE]; //G_SMC_BuildBankMap[K_SMC_BUILD_BANK_MAP_SIZE];
	#endif

// @012403@wyeo@support 256M nand, begin
    UINT32   smcGoodSectorPerCard;           // G_SMC_GoodSectorPerCard
    UINT8    nandMakerByte;                  // G_NANDF_Maker_Byte
    UINT8    nand3RD_Byte;                   // G_NANDF_3RD_Byte
    UINT8    nand4TH_Byte;                   // G_NANDF_4TH_Byte          
    UINT8    nand5TH_Byte;                   // G_NANDF_5TH_Byte
    UINT8    nandCategory;                   // G_NANDF_Category // 0 is old, 1 is new and SMC2_
    UINT16    nandPageSize;                   // G_NANDF_PageSize
    UINT16    nandPageExtSize;                // G_NANDF_PageExtSize
    UINT32   nandBlockSize;                  // G_NANDF_BlockSize
    UINT8    nandBlockSizeShift;             // G_NANDF_BlockSizeShift
    /*
    BYTE    nandHasOldBlk;                  // G_NANDF_HasOldBlk
    UINT32   nandStartLogAddr;               // G_NANDF_StartLogAddr
    UINT32   nandEndBlkAddr;                 // G_NANDF_EndBlkAddr
    UINT32   nandNewPhyAddr;                 // G_NANDF_NewPhyAddr
    UINT32   nandOldPhyStartAddr;            // G_NANDF_OldPhyStartAddr
    WORD    nandBlockNum;                   // G_NANDF_OldPhyStartAddr
    UINT32   nandOldPhyAddr;                 // G_NANDF_OldPhyAddr
    BYTE    nandFindHeaderPage;             // G_NANDF_FindHeaderPage
    BYTE    nandSetHdrPageAddr;             // G_NANDF_SetHdrPageAddr
    BYTE    nandFirstEnter;                 // G_NANDF_FirstEnter
    BYTE    nandRsvIndex;                   // G_NANDF_RsvIndex
    BYTE    nandStartPage;                  // G_NANDF_StartPage
    */
// @012403@wyeo@support 256M nand, end

	#endif
} MLUN_SAVED_XDATA, *PMLUN_SAVED_XDATA; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
PUSBMSDC_CBW    G_USBMSDC_pCBW; 
xdata   UINT32  G_USBMSDC_ulLogicalBlockAddress;
xdata   UINT32  G_USBMSDC_ulSdramBufferAddress;
xdata   UINT32  G_USBMSDC_usSizeProcessed;
xdata   UINT8   G_USBMSDC_ucSenseCodeIdx;
xdata   UINT8   G_USBMSDC_ucCSWStatus;
xdata   UINT32  G_USBMSDC_ulAllocateLength;
xdata   UINT8   G_USBMSDC_ulLatestCmd;
xdata   UINT8   G_USBMSDC_ucReadInBuf[64];
//richie@ref 0326
xdata   UINT8   G_USBMSDC_ubRefreshNeeded;
//patch4.5@richie@eject
xdata   UINT8   G_USBMSDC_ucEject;
xdata	UINT8	G_USBMSDC_ucCardLock;
//patch4.5@richie@mlun begin
xdata UINT8 G_ucMSDC_MLUN;
//patch4.5@richie@mlun end
xdata	UINT8	G_MLUN_Support[K_CARD_SUPPORT_MAX];
xdata	UINT8	G_MLUN_Support_Type[K_CARD_SUPPORT_MAX];

xdata	UINT8	G_MLUN_Media_Present[K_CARD_SUPPORT_MAX];
xdata	UINT8	G_MLUN_Media_ucCardLock[K_CARD_SUPPORT_MAX];
xdata	UINT16	G_MLUN_Media_SizeMB[K_CARD_SUPPORT_MAX];
xdata	UINT16	G_MLUN_Media_BlockSize[K_CARD_SUPPORT_MAX];
xdata	UINT8	G_MLUN_Media_BlockSizeShift[K_CARD_SUPPORT_MAX];
xdata	UINT8	G_MLUN_Media_ucEject[K_CARD_SUPPORT_MAX];

xdata	UINT8	G_MLUN_LUN_Map[K_CARD_SUPPORT_MAX];
xdata	UINT8	G_MLUN_Supported_Num;

xdata	UINT8	G_MLUN_Media_Pos_Current;
xdata	UINT8	G_MLUN_Media_Pos_Next;

xdata	UINT8	G_MLUN_MEdia_Pos_LastCardSelect;
xdata	UINT8	G_MLUN_DramSmcNandData;
xdata	UINT32	G_MLUN_SMC_DramAddr;
xdata	UINT32	G_MLUN_NAND_DramAddr;

//patch5.2.1@richie@mmlun begin
xdata	UINT8	G_MLUN_CardSupport;
//patch5.2.1@richie@mmlun end
extern xdata 	UINT32	G_timerCheckTestUnit;
extern xdata	UINT8	G_testUnitReadyFlag;
UINT8	ejectPowerOffFlag = 0;
UINT8 	startStopCommandFlag = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdata UINT8 G_Detect_PinMux;
extern xdata UINT8 G_UsbSlunShowNand; 
#if (NANDF_OPTION == 1 && SMC_OPTION ==1)
extern void MLUN_SaveSMCData(UINT8 cardType);
extern void MLUN_LoadSMCData(UINT8 cardType);
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 usbWPCheck(UINT8 cardType);
#if MWEBTOOL_ENABLE
UINT8 mWebCMD_CheckSecurity(UINT8* pSCSI, UINT32* pSize);   //xian ++ 20090108
#endif

void USBMSDC_GlobalTimerRead10us(UINT32 xdata* G10ustimerPt)
{
	BIT mode;
	UINT32 val;

	mode = XBYTE[0x20C5] & 0x01;

	do {
		*((UINT8 xdata*)G10ustimerPt + 3) = XBYTE[0x20C1];
		*((UINT8 xdata*)G10ustimerPt + 2) = XBYTE[0x20C2];
		*((UINT8 xdata*)G10ustimerPt + 1) = XBYTE[0x20C3];
		*((UINT8 xdata*)G10ustimerPt + 0) = XBYTE[0x20C4];
		
		WRITE8(val, 0, XBYTE[0x20C4]);
		WRITE8(val, 1, XBYTE[0x20C3]);
		WRITE8(val, 2, XBYTE[0x20C2]);
		WRITE8(val, 3, XBYTE[0x20C1]);
	} while(mode ? *G10ustimerPt > val : *G10ustimerPt < val);
}

void USBMSDC_GlobalReadGTimer(UINT32* GtimerPt)
{
	UINT32 usTime;
	USBMSDC_GlobalTimerRead10us(& usTime);
	*GtimerPt = usTime / 100 ;
	return;
}

//-----------------------------------------------------------------------------
//USBMSDC_BoNoDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_BoNoDataPhase(void)
 * @brief     process class request without data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_BoNoDataPhase(void) USING_1
{
    switch (G_pCtrlCommand->bRequest)
    {
		case K_USBMSDC_BULK_ONLY_MASS_STORAGE_RESET:
			break;

		default:
			break;
    }
}

//-----------------------------------------------------------------------------
//USBMSDC_BoPreInDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_BoPreInDataPhase(void)
 * @brief     pre-process setting of following in data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_BoPreInDataPhase(void) USING_1
{
	G_pucCtrlDataPointer = G_ucCtrlData;
    switch (G_pCtrlCommand->bRequest)
    {
		case K_USBMSDC_GET_MAX_LUN:
			if (G_ucMSDC_MLUN)
			G_pucCtrlDataPointer[0] = G_MLUN_Supported_Num - 1;
			else
			G_pucCtrlDataPointer[0] = 0;
		    break;

		default:
			break;
    }
}

//-----------------------------------------------------------------------------
//USBMSDC_BoPreOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_BoPreOutDataPhase(void)
 * @brief     pre-process setting of following out data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_BoPreOutDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//USBMSDC_BoPostInDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_BoPostInDataPhase(void)
 * @brief     post-process data of complete in data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_BoPostInDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//USBMSDC_BoPostOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_BoPostOutDataPhase(void)
 * @brief     post-process data of complete out data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_BoPostOutDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//USBMSDC_BoBulkInData
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_BoBulkInData(void)
 * @brief     process in data packet
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_BoBulkInData(void) USING_1
{
	UINT32 len;
	//sss mark
	//UINT32 blockNum;
	//	int i;
	UINT16 pktsize = 64;
    if (G_ucBulkPhase == K_InDataPhase)
    {
        len = G_ulStorDataLength - G_ulStorDataIndex; 
	 if(G_bUSBSpeed) {
		pktsize = 512;
	 }
        USBMSDC_BoSetBulkInPacket(((len > pktsize) ? pktsize : len));
        if ((len < pktsize) || ((len == pktsize) && (G_btDataShort == 0)))
        {
			//richie@1203
			if (G_USBMSDC_ucCSWStatus == K_USBMSDC_NoMedium)
			{
				G_ucBulkPhase = K_InResponsePhase;
			}
			else
			{
                G_USBMSDC_ucSenseCodeIdx = 0x00;
                G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandPassed;
    	        G_ucBulkPhase = K_InResponsePhase;
			}
		}
    }
    else if (G_ucBulkPhase == K_InDataPhaseRead)
    {
		len = G_ulStorDataLength - G_ulStorDataIndex;                 

		if (len > pktsize)
		{
			len = pktsize;
		}
		if ((len < pktsize) || (G_ulStorDataIndex >= G_ulStorDataLength))  //size reached                
        {
			//richie@1203
			if (G_USBMSDC_ucCSWStatus == K_USBMSDC_NoMedium)
			{
				G_ucBulkPhase = K_InResponsePhase;
			}
			else
			{
				///USBMSDC_BoPostInBlock();	
				if ( G_USBMSDC_ucCSWStatus == K_USBMSDC_CommandPassed ){
					G_USBMSDC_ucSenseCodeIdx = 0x00;
					G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandPassed;
					G_ucBulkPhase = K_InResponsePhase;
				}
				if (len != 0) //never run into this because block size % 64 == 0
				{
					//G_USBMSDC_usSizeProcessed += len;
				}
				USBMSDC_BoBulkInData();
			}
		}
		else //send 64 byte to usb
		{	
			//sss mark
			//blockNum = G_ulStorDataLength / G_DOS_SectorSize;

			//richie@mi 0409
			XBYTE[REG_Usb_EpAckIntEn] &= K_USB_CLASS_IN_ACK_DISABLE;  //Bulk in 1 acked interrupt disable
	     	XBYTE[0x20D0] &= 0xdf;  //Turn off VD falling interrupt
			return;
     	}
    }
    else if (G_ucBulkPhase == K_InResponsePhase)
    {
		if (G_USBMSDC_ucCSWStatus == K_USBMSDC_NoMedium)
		{
			G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;

			if (G_USBMSDC_ulLatestCmd != K_SCSICMD_TEST_UNIT_READY)
			{	
				//richie@1212
				//USBMSDC_StallBulkInOutPacket();	
				//richie@0110
				XBYTE[REG_Usb_TransactionStallEn] = K_USB_CLASS_IN_STALL;//in stall
			}
		}
		//richie@1203
    	G_pucStorDataPointer = G_ucStorData;
            len = USBMSDC_BoStatus();
		//richie@1129
		G_ulStorDataIndex = 0;

		USBMSDC_BoSetBulkInPacket(len);
		G_ucBulkPhase = K_InCompletionPhase;
	}
    else if (G_ucBulkPhase == K_InCompletionPhase)
    {
		//richie@1219mark
		//USBMSDC_ClearStallBulkInOutPacket();
		G_ucBulkPhase = K_CommandPhase;	//cytsai@1122
    }
    else
    {
        //phase transition error - out packet should not appear
        //                         at current phase
        //DbgPrint("Phase Error G_ucBulkPhase %d\n",(WORD)G_ucBulkPhase);
        G_USBMSDC_ucSenseCodeIdx = 0x0e;        //UNKNOWN ERROR
        G_USBMSDC_ucCSWStatus = K_USBMSDC_PhaseError;
        G_ucBulkPhase = K_InResponsePhase;
        USBMSDC_BoBulkInData();
        USBMSDC_StallBulkInOutPacket();
    }
}

//-----------------------------------------------------------------------------
//USBMSDC_BoBulkOutData
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_BoBulkOutData(void)
 * @brief     process out data packet
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_BoBulkOutData(void) USING_1
{
	UINT32 len;
//	UINT32 blockNum;
//	BIT firstOut;
//	int i;

	if (G_ucBulkPhase == K_CommandPhase)
	{
		G_pucStorDataPointer = G_ucStorCommand;
		//richie
		G_ulStorDataIndex = 0;

		len = USBMSDC_BoGetBulkOutPacket();
		if (len)
		{
			if (USBMSDC_BoCommand(len))
			{
				if (G_ucBulkPhase & K_XferDirMask)
				{
					USBMSDC_BoBulkInData();
				}
			}
		}
    }
	else if (G_ucBulkPhase == K_OutDataPhase)
	{
		len = USBMSDC_BoGetBulkOutPacket();
		G_USBMSDC_usSizeProcessed += len;
		if ((len < 64) ||                       //short packet
			(G_ulStorDataIndex >= G_ulStorDataLength))  //size reached
		{
			if (USBMSDC_BoPostBulkOutDataPhase())
			{
				G_USBMSDC_ucSenseCodeIdx = 0x00;
				G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandPassed;
			}
			else
			{
				G_USBMSDC_ucSenseCodeIdx = 0x0e;
				G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;
			}
				G_ucBulkPhase = K_InResponsePhase;
				USBMSDC_BoBulkInData();
		}
	}
	else if (G_ucBulkPhase == K_OutDataPhaseWrite)
	{
		//richie@mi 0411
		XBYTE[REG_Usb_EpAckIntEn] &= K_USB_CLASS_OUT_ACK_DISABLE;  //Bulk out 1 acked interrupt disable
		XBYTE[0x20D0] &= 0xdf;  //Turn off VD falling interrupt
		return;
	}
	else
	{
		//phase transition error - out packet should not appear
		//                         at current phase
		//DbgPrint("out Phase Error G_ucBulkPhase %d\n",(WORD)G_ucBulkPhase);
		G_USBMSDC_ucSenseCodeIdx = 0x0e;        //UNKNOWN ERROR
		G_USBMSDC_ucCSWStatus = K_USBMSDC_PhaseError;
		G_ucBulkPhase = K_InResponsePhase;
		USBMSDC_BoBulkInData();
		USBMSDC_StallBulkInOutPacket();
	}
}


//-----------------------------------------------------------------------------
//USBMSDC_BoCommand
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_BoCommand(UINT8 len)
 * @brief     process command
 * @param     [in] len: length of command
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_BoCommand(UINT8 len) USING_1
{
	UINT8 status = FALSE;
	UINT8* pSCSI;
	UINT32 dataLength = 0;
	UINT8 cmdLength = len;
	G_pucStorDataPointer = G_ucStorData;

	G_USBMSDC_pCBW = (PUSBMSDC_CBW) G_ucStorCommand;

	G_ulStorDataLength = M_ByteSwapOfDword(G_USBMSDC_pCBW->dCBWDataTransferLength);

	//richie
	G_USBMSDC_ulAllocateLength = G_ulStorDataLength;

	G_ulStorDataIndex = 0;
	G_USBMSDC_usSizeProcessed = 0;
	G_btDataShort = 0;

	G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandPassed;
	
	//richie
	G_USBMSDC_ulLatestCmd = 0xff;

	pSCSI = G_USBMSDC_pCBW->CBWCB;
	////printf("pSCSI[0]=%bx\n",pSCSI[0]);

	//patch4.5@richie@mlun begin
	if ( G_ucMSDC_MLUN )
	{		
		G_MLUN_Media_Pos_Current = G_MLUN_Media_Pos_Next;
		G_MLUN_Media_Pos_Next = G_MLUN_LUN_Map[G_USBMSDC_pCBW->bCBWLUN];
		G_Card_TotalSizeMB = G_MLUN_Media_SizeMB[G_MLUN_Media_Pos_Next];
		G_Card_BlockSize = G_MLUN_Media_BlockSize[G_MLUN_Media_Pos_Next];			
		G_Card_BlockSizeShift = G_MLUN_Media_BlockSizeShift[G_MLUN_Media_Pos_Next];
		G_USBMSDC_ucCardLock = G_MLUN_Media_ucCardLock[G_MLUN_Media_Pos_Next];
		G_USBMSDC_ucEject = G_MLUN_Media_ucEject[G_MLUN_Media_Pos_Next];
		if ( G_MLUN_Media_Present[G_MLUN_Media_Pos_Next] ) {
			G_ucStorageType = G_Card_Type;
		}
		else {
			G_ucStorageType = K_MEDIA_NONE;		
		}
	}//end of if (G_ucMSDC_MLUN)
	//printf("G_ucStorageType1=%bx\n",G_ucStorageType);
	//printf("scsi=%bx\n",pSCSI[0]);
	switch (pSCSI[0])
    {
		//---------------------------------------------------
		//6-byte command
		//---------------------------------------------------
		case K_SCSICMD_TEST_UNIT_READY: 				//0x00
			//richie
			if(ejectPowerOffFlag)
			{
				G_testUnitReadyFlag=1;
				if(startStopCommandFlag==0)USBMSDC_GlobalReadGTimer(&G_timerCheckTestUnit);
			}
			G_USBMSDC_ulLatestCmd = K_SCSICMD_TEST_UNIT_READY;
			status = SCSICMD_TestUnitReady(pSCSI, &dataLength);
			if( (G_ulStorDataLength !=0)&&(G_USBMSDC_pCBW->bmCBWFlags == 0x00))
				G_ucBulkPhase = K_OutDataPhase;
			else
				G_ucBulkPhase = K_InResponsePhase;
			break;
		/*
		case K_SCSICMD_REZERO_UNIT: 					//0x01
			status = SCSICMD_RezeroUnit(pSCSI, &dataLength);
			G_ucBulkPhase = K_InResponsePhase;
			break;
		*/
		case K_SCSICMD_REQUEST_SENSE:					//0x03
			status = SCSICMD_RequestSense(pSCSI, &dataLength);
			G_ucBulkPhase = K_InDataPhase;
			break;
		/*
		case K_SCSICMD_FORMAT_UNIT: 					//0x04
			status = SCSICMD_FormatUnit(pSCSI, &dataLength);
			G_ucBulkPhase = K_OutDataPhase;
			break;
		*/								  
		case K_SCSICMD_INQUIRY: 						//0x12
			//richie
			G_USBMSDC_ulLatestCmd = K_SCSICMD_INQUIRY;
			status = SCSICMD_Inquiry(pSCSI, &dataLength);
			G_ucBulkPhase = K_InDataPhase;
			break;
		
		case K_SCSICMD_MODE_SELECT_06:					//0x15	
			G_USBMSDC_ulLatestCmd = K_SCSICMD_MODE_SELECT_06;
			//status = SCSICMD_ModeSelect06(pSCSI, &dataLength);
			G_ucBulkPhase = K_OutDataPhase;
			break;

		//patch4.5@richie@wp xp begin
/*		*/
		case K_SCSICMD_MODE_SENSE_06:					//0x1a	 
			///G_USBMSDC_ulLatestCmd = K_SCSICMD_MODE_SENSE_06;
			status = SCSICMD_ModeSense06(pSCSI, &dataLength);
			G_ucBulkPhase = K_InDataPhase;
			break;
		/*
		//patch4.5@richie@wp xp end

		//patch4.5@richie@eject begin
		*/
		case K_SCSICMD_START_STOP_UNIT: 				//0x1b	 
			G_USBMSDC_ulLatestCmd = K_SCSICMD_START_STOP_UNIT;
			status = SCSICMD_StartStopUnit(pSCSI, &dataLength);
			if(G_USBMSDC_ucEject&&ejectPowerOffFlag) startStopCommandFlag=1;
			G_ucBulkPhase = K_InResponsePhase;
			//G_USBMSDC_ucEject = 1;
			//patch5.1@richie@mlun eject begin
			
		//patch5.1@richie@mlun eject end
			break;
		/*
		//patch4.5@richie@eject begin
		case K_SCSICMD_SEND_DIAGNOSTIC: 				//0x1d	 
			status = SCSICMD_SendDiagnostic(pSCSI, &dataLength);
			G_ucBulkPhase = K_InResponsePhase;
			break;
		*/
		case K_SCSICMD_PREVENT_ALLOW_MEDIUM_REMOVAL:	//0x1e
			//patch4.5@richie@eject
			G_USBMSDC_ulLatestCmd = K_SCSICMD_PREVENT_ALLOW_MEDIUM_REMOVAL;
			status = SCSICMD_PreventAllowMediumRemoval(pSCSI, &dataLength);
			G_ucBulkPhase = K_InResponsePhase;
			break;

		//---------------------------------------------------
		//10-byte command
		//---------------------------------------------------
		case K_SCSICMD_READ_CAPACITY:					//0x25
			status = SCSICMD_ReadCapacity(pSCSI, &dataLength);
			G_ucBulkPhase = K_InDataPhase;
			break;

		case K_SCSICMD_READ_10: 						//0x28	
			USBMSDC_GlobalReadGTimer(&G_timerCheckTestUnit);	   
			status = SCSICMD_Read10(pSCSI, &dataLength);
			G_ucBulkPhase = K_InDataPhaseRead;
			if(G_USBMSDC_pCBW->bmCBWFlags == 0x00 && G_ulStorDataLength != 0)
			{
				G_ucBulkPhase =K_OutDataPhase;
				G_USBMSDC_ucCSWStatus = K_USBMSDC_PhaseError;
			}
			//DbgPrint("G_ulStorDataLength = %ld ,dataLength = %ld \n ",(UINT32)G_ulStorDataLength,(UINT32)dataLength);
			break;

		case K_SCSICMD_WRITE_10:						//0x2a
			USBMSDC_GlobalReadGTimer(&G_timerCheckTestUnit);
			G_USBMSDC_ulLatestCmd = K_SCSICMD_WRITE_10;
			status = SCSICMD_Write10(pSCSI, &dataLength);
			//DbgPrint("G_ulStorDataLength = %ld ,dataLength = %ld \n ",(UINT32)G_ulStorDataLength,(UINT32)dataLength);
			G_ucBulkPhase = K_OutDataPhaseWrite;
			if(G_USBMSDC_pCBW->bmCBWFlags == 0x80 && G_ulStorDataLength != 0)
			{
				G_ucBulkPhase =K_InDataPhaseRead;
				G_USBMSDC_ucCSWStatus = K_USBMSDC_PhaseError;
			}
			break;
/*
		case K_SCSICMD_SEEK_10: 						//0x2b
			status = SCSICMD_Seek10(pSCSI, &dataLength);
			G_ucBulkPhase = K_InResponsePhase;
			break;

		case K_SCSICMD_WRITE_AND_VERIFY_10: 			//0x2e
			status = SCSICMD_WriteAndVerify10(pSCSI, &dataLength);
			G_ucBulkPhase = K_OutDataPhaseWrite;
			break;
*/
		case K_SCSICMD_VERIFY_10:						//0x2f
			status = SCSICMD_Verify10(pSCSI, &dataLength);
			G_ucBulkPhase = K_InResponsePhase;
			break;

		case K_SCSICMD_MODE_SELECT_10:					//0x55
			G_USBMSDC_ulLatestCmd = K_SCSICMD_MODE_SELECT_10;
			//status = SCSICMD_ModeSelect10(pSCSI, &dataLength);
			G_ucBulkPhase = K_OutDataPhase;
			break;
/**/
		case K_SCSICMD_MODE_SENSE_10:					//0x5a	 
			status = SCSICMD_ModeSense10(pSCSI, &dataLength);
			G_ucBulkPhase = K_InDataPhase;
			break;

		//---------------------------------------------------
		//12-byte command
		//---------------------------------------------------
		case K_SCSICMD_READ_12: 						//0xa8		
			G_USBMSDC_ulLatestCmd = K_SCSICMD_READ_12;
			status = SCSICMD_Read12(pSCSI, &dataLength);
			G_ucBulkPhase = K_InDataPhaseRead;
			break;

		case K_SCSICMD_WRITE_12:						//0xaa
			G_USBMSDC_ulLatestCmd = K_SCSICMD_WRITE_12;
			status = SCSICMD_Write12(pSCSI, &dataLength);
			G_ucBulkPhase = K_OutDataPhaseWrite;
			break;
		//richie@1129
/*	case K_SCSICMD_READ_FORMAT_CAPACITIES:			//0x23								
			G_USBMSDC_ulLatestCmd = K_SCSICMD_READ_FORMAT_CAPACITIES;
			status = SCSIDATA_ReadFormatCapacities(pSCSI, &dataLength);
			G_ulStorDataLength = dataLength;
			G_ucBulkPhase = K_InDataPhase;
			break;
*/
		//---------------------------------------------------
		//16-byte command
		//---------------------------------------------------
		//xian ++ 20090108
		#if MWEBTOOL_ENABLE    
		case 0xc2: //for mWeb2.0
			if(G_USBMSDC_pCBW->bmCBWFlags & 0x80){
				status = mWebCMD_CheckSecurity(pSCSI, &dataLength);
				G_ucBulkPhase = K_InDataPhase;
			}
			else{
				status = TRUE;
				dataLength = 0;
				G_ucBulkPhase = K_InResponsePhase;
			}
			break;
		#endif							  

		//---------------------------------------------------
		//Unsupport command
		//---------------------------------------------------
		default:
			//DbgPrint("%x [err command]\n",(WORD)pSCSI[0]);
			break;
	}				 

	//richie@ref0326 begin
	if ((G_USBMSDC_ulLatestCmd == K_SCSICMD_TEST_UNIT_READY) && (G_USBMSDC_ubRefreshNeeded == 1))
	{
	    //G_USBMSDC_ucSenseCodeIdx = 0x1c;
	    G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;
		G_USBMSDC_ucSenseCodeIdx = 0x27;
		G_USBMSDC_ubRefreshNeeded = 0;

		return TRUE;
	}
	//patch4.5@richie@eject begin
	else if ((G_USBMSDC_ulLatestCmd == K_SCSICMD_TEST_UNIT_READY)&& (G_USBMSDC_ucEject == 1))
	{
	    	G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;
			G_USBMSDC_ucSenseCodeIdx = 0x0B;

		return TRUE;
	}

	//if (G_USBMSDC_ulLatestCmd == K_SCSICMD_PREVENT_ALLOW_MEDIUM_REMOVAL)
	//{
	//        G_USBMSDC_ucSenseCodeIdx = 0x1c;
	//        G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;
	//	return TRUE;
	//}
	//patch4.5@richie@eject end
	//richie@ref0326 end

	//patch4.5@richie@cardlock begin
	if ((G_USBMSDC_ulLatestCmd == K_SCSICMD_WRITE_10) && (G_USBMSDC_ucCardLock == 1))
	{
		G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;
		G_USBMSDC_ucSenseCodeIdx = 0x25; //WriteProtectMedia
		//???
		G_USBMSDC_ubRefreshNeeded = 0;

		XBYTE[REG_Usb_EpNakIntSts] = K_USB_CLASS_OUT_ACK_MASK;//0x01;
		while (XBYTE[REG_Usb_EpNakIntSts] == 0) ;

		XBYTE[REG_Usb_TransactionStallEn] = K_USB_CLASS_OUT_STALL;//out stall
		//DbgPrint("2XBYTE[0x25e8]=%bx\n",XBYTE[0x25e8]);
		G_ucBulkPhase = K_InResponsePhase;
		//richie@1219mark
		USBMSDC_BoBulkInData();

		return FALSE;
	}
	//patch4.5@richie@cardlock end

	if (status)
	{
	    G_USBMSDC_ucSenseCodeIdx = 0x00;

	   if ((G_ulStorDataLength == 0) && (dataLength != 0))
	   {
		status = FALSE;
		G_USBMSDC_ucCSWStatus = K_USBMSDC_PhaseError;
		G_USBMSDC_ucSenseCodeIdx = 0x1C;
		G_ucBulkPhase = K_InResponsePhase;
		USBMSDC_BoBulkInData();

	   }
	   else if (pSCSI[0] == 0x28 || pSCSI[0] == 0x2A)
	   {
		G_USBMSDC_ucSenseCodeIdx =0x1c;
		if (G_ulStorDataLength > dataLength)
		{
			G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;
		}
		else if(G_ulStorDataLength < dataLength)
		{
			G_USBMSDC_ucCSWStatus = K_USBMSDC_PhaseError;
		}
	   }
	   else if (G_ulStorDataLength > dataLength)
	    {
			G_btDataShort = 1; 
			G_ulStorDataLength = dataLength;
	    }
	}
	//richie@1203
	else if (G_USBMSDC_ucCSWStatus == K_USBMSDC_NoMedium)
	{
	    G_USBMSDC_ucSenseCodeIdx = 0x0b;

	    if (G_ulStorDataLength > dataLength)
	    {
			G_btDataShort = 1; 
			G_ulStorDataLength = dataLength;
	    }

		return (TRUE);
	}
	else if (G_USBMSDC_ucCSWStatus == K_USBMSDC_PhaseError)
	{
		G_USBMSDC_ucSenseCodeIdx = 0x1c;
		G_ucBulkPhase = K_InResponsePhase;
		USBMSDC_BoBulkInData();

	}
	else
	{
		G_USBMSDC_ucSenseCodeIdx = 0x1c;
		G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;

		//patch4.5.1@bulk out stall begin
		if ( (G_USBMSDC_pCBW->dCBWDataTransferLength != 0) && ((G_USBMSDC_pCBW->bmCBWFlags & 0x80) == 0x00) )
		{	//OUT stall
			XBYTE[REG_Usb_EpNakIntSts] = K_USB_CLASS_OUT_ACK_MASK;//0x01;
			while (XBYTE[REG_Usb_EpNakIntSts] == 0) ;

			XBYTE[REG_Usb_TransactionStallEn] = K_USB_CLASS_OUT_STALL;//out stall
		}		
		
		else if ( (G_USBMSDC_pCBW->dCBWDataTransferLength != 0) && ((G_USBMSDC_pCBW->bmCBWFlags & 0x80) == 0x80) )
		{	//IN stall
			XBYTE[REG_Usb_EpNakIntSts] = K_USB_CLASS_IN_ACK_MASK;//0x01;
			while (XBYTE[REG_Usb_EpNakIntSts] == 0) ;

			XBYTE[REG_Usb_TransactionStallEn] = K_USB_CLASS_IN_STALL;//in stall
		}
		//patch4.5.1@bulk out stall end
		else if ((G_USBMSDC_ulLatestCmd==K_SCSICMD_MODE_SELECT_06)||(G_USBMSDC_ulLatestCmd==K_SCSICMD_MODE_SELECT_10))
		{
			G_USBMSDC_ucSenseCodeIdx = 0x1c;
			G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;
		}
		
		G_ucBulkPhase = K_InResponsePhase;
		//richie@1219mark
		USBMSDC_BoBulkInData();
	}
	if ( (G_USBMSDC_ulLatestCmd == K_SCSICMD_READ_12) ||(G_USBMSDC_ulLatestCmd == K_SCSICMD_WRITE_12)) {
		 G_USBMSDC_ucSenseCodeIdx = 0x1c;
		 G_USBMSDC_ucCSWStatus = K_USBMSDC_CommandFailed;
		//printf("*$$*\n");

	}
	return(status);
	}

//-----------------------------------------------------------------------------
//USBMSDC_BoStatus
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_BoStatus(void)
 * @brief     status phase
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_BoStatus(void) USING_1
{
	PUSBMSDC_CSW pCSW;

	pCSW = (PUSBMSDC_CSW) G_pucStorDataPointer;

	//richie@1129
	pCSW->dCSWSignature = 0x55534253;
	pCSW->dCSWTag = G_USBMSDC_pCBW->dCBWTag;
	pCSW->dCSWDataResidue = M_ByteSwapOfDword(G_USBMSDC_ulAllocateLength - G_ulStorDataIndex);
	pCSW->bCSWStatus = G_USBMSDC_ucCSWStatus;

	return(K_USBMSDC_CSW_SIZE);
}

//-----------------------------------------------------------------------------
//USBMSDC_BoSetBulkInPacket
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_BoSetBulkInPacket(UINT8 len)
 * @brief     set data for current in packet
 * @param     [in] len: length of bulk in packet
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_BoSetBulkInPacket(UINT16 len) USING_1
{
	UINT16 i;

	if (G_ucPktStorIntrf == K_StorageInterface1)
	{
		XBYTE[REG_Usb_BufClr] = 0x18;

		USBMSDC_Sram2Usb(len);
		//patch4.3@richie@res0530
		G_ulStorDataIndex += len;
	}
	else //if (G_ucPktStorIntrf == K_StorageInterface2)
	{
		for (i = 0; i < len; i++)
		XBYTE[K_USB_AP_IN_DATA] = G_pucStorDataPointer[G_ulStorDataIndex++];
	}
}

//-----------------------------------------------------------------------------
//USBMSDC_BoGetBulkOutPacket
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_BoGetBulkOutPacket(void)
 * @brief     get data for current out packet
 * @param     NONE
 * @retval    length of bulk out packet
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_BoGetBulkOutPacket(void) USING_1
{
	UINT8 i;
	UINT16 len;

	if (G_ucPktStorIntrf == K_StorageInterface1)
	{
		len = XBYTE[K_USB_CLASS_OUT_BUF_SIZE];
		//richie@0116 for speed
		USBMSDC_Usb2Sram(G_pucStorDataPointer, len);
		//USBMSDC_CF_LUsb2Sram(len);
		XBYTE[REG_Usb_BufClr] = 0x18;

		
	}
	else //if (G_ucPktStorIntrf == K_StorageInterface2)
	{
		len = (UINT16)XBYTE[K_USB_AP_OUT_BUF_SIZE_LSB] | (UINT16)(XBYTE[K_USB_AP_OUT_BUF_SIZE_MSB] << 8);
		for (i = 0; i < len; i++) {
			G_pucStorDataPointer[G_ulStorDataIndex++] = XBYTE[K_USB_AP_OUT_DATA];
		}
	}


	return len;
}

//-----------------------------------------------------------------------------
//USBMSDC_BoGetBulkOutPacketToBlock
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn        UINT8 USBMSDC_BoGetBulkOutPacketToBlock(void)
 * @brief     get data for current out packet accumulated to size of logical block
 * @param     NONE
 * @retval    length of bulk out packet
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_BoGetBulkOutPacketToBlock(void) USING_1
{
	UINT8 len;

	if (G_ucPktStorIntrf == K_StorageInterface1)
	{
		len = XBYTE[K_USB_CLASS_OUT_BUF_SIZE];
		G_ulStorDataIndex += (UINT32)len;
	}
	else //if (G_ucPktStorIntrf == K_StorageInterface2)
	{
		len = XBYTE[K_USB_AP_OUT_BUF_SIZE];
		G_ulStorDataIndex += (UINT32)len;
	}

	return len;
}
#endif /*_DBG_CLR_WARNING_*/

//-----------------------------------------------------------------------------
//USBMSDC_BoPostBulkOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 USBMSDC_BoPostBulkOutDataPhase(void)
 * @brief     post-process data of complete bulk out data phase
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 USBMSDC_BoPostBulkOutDataPhase(void) USING_1
{
	UINT8* pSCSI;
	UINT8 status = TRUE;  

	G_pucStorDataPointer = G_ucStorData;

	pSCSI = G_USBMSDC_pCBW->CBWCB;

	switch (pSCSI[0])
	{
		//---------------------------------------------------
		//6-byte command
		//---------------------------------------------------
		case K_SCSICMD_FORMAT_UNIT: 					//0x04
			status = SCSIDATA_FormatUnit(pSCSI);
			break;
						
		case K_SCSICMD_MODE_SELECT_06:					//0x15	 
			status = SCSIDATA_ModeSelect06(pSCSI);
			break;

		//---------------------------------------------------
		//10-byte command
		//---------------------------------------------------
		case K_SCSICMD_MODE_SELECT_10:					//0x55
			status = SCSIDATA_ModeSelect10(pSCSI);
			break;

		//---------------------------------------------------
		//12-byte command
		//---------------------------------------------------

		//---------------------------------------------------
		//16-byte command
		//---------------------------------------------------

		//---------------------------------------------------
		//Unsupport command
		//---------------------------------------------------
		default:
			status = FALSE;
			break;
	}

	return status;
}
//-----------------------------------------------------------------------------
//USBMSDC_StallBulkInOutPacket
//-----------------------------------------------------------------------------
/**
 * @fn        void USBMSDC_StallBulkInOutPacket(void)
 * @brief     stall handshake of bulk in/out packet
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_StallBulkInOutPacket(void) USING_1
{
	if (G_ucPktStorIntrf == K_StorageInterface1)
	{
		XBYTE[REG_Usb_TransactionStallEn] |= 0xfc;//0x1c;
	}
	else //if (G_ucPktStorIntrf == K_StorageInterface2)
	{
		XBYTE[REG_Usb_TransactionStallEn] |= 0xfc;//0xe0;
	}
}

//-----------------------------------------------------------------------------
//USBMSDC_ClearStallBulkInOutPacket
//-----------------------------------------------------------------------------
#ifndef _DBG_CLR_WARNING_
/**
 * @fn        void USBMSDC_ClearStallBulkInOutPacket(void)
 * @brief     clear stall handshake of bulk in/out packet
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void USBMSDC_ClearStallBulkInOutPacket(void) USING_1
{
	XBYTE[REG_Usb_TransactionStallEn] &= 0x03;		  
}
#endif


//-----------------------------------------------------------------------------
//MLUN_Initialize
//-----------------------------------------------------------------------------

/**
 * @fn        void MLUN_Initialize(void)
 * @brief     Initialize Multilun
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void MLUN_Initialize(void) USING_0
{
#if (FPGA_VERIFY)/* mark for 1628 FPGA */
	UINT8	i;
	devAttr_t* pDev;//add by phil for disk driver lib release

	G_MLUN_DramSmcNandData = K_MEDIA_NONE;

	for (i = 0;i < K_CARD_SUPPORT_MAX;i++) {
		G_MLUN_Support[i] = 0;
		G_MLUN_LUN_Map[i] = 0;
	}

	i = 0;
	G_MLUN_Supported_Num = 0;

	HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_NANDF,1);
	HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_SDF,1);
	HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_EMMC,1);

	printf("%CS=%bx\n",G_MLUN_CardSupport);
	if (G_MLUN_CardSupport & K_NANDF_SUPPORTED) {
		/* if SLUN and don't show nand as default, then pass below procedure */
		if ((G_ucMSDC_MLUN == 0x01) || (G_UsbSlunShowNand == 0x01)){

			/* add by phil for disk driver lib release s*/
			pDev = sp1kDevHdlGet();
			pDev += DEV_ID_DISK;
			if (pDev->devType == DEV_TYPE_DISK_NAND) {
				G_MLUN_LUN_Map[G_MLUN_Supported_Num] = K_MLUN_NANDF_LUN_POS;
				G_MLUN_Supported_Num++;
				printf("NS\n");
				G_MLUN_Support[K_MLUN_NANDF_LUN_POS] = 1;
				G_MLUN_Support_Type[K_MLUN_NANDF_LUN_POS] = K_MEDIA_NANDF;
				
				G_Card_Type = K_MEDIA_NANDF;
				i = K_MLUN_NANDF_LUN_POS;
				G_MLUN_Media_Present[i] = 1; //SMC_Initialize(FALSE);
				//G_MLUN_Media_Present[i] = FALSE;		
				//printf("G_MLUN_Media_Present[i]=%bx\n",G_MLUN_Media_Present[i]);		
				//printf("G_Card_TotalSizeMB = %d\n",G_Card_TotalSizeMB);		
				G_MLUN_Media_SizeMB[i] = G_Card_TotalSizeMB;
				G_MLUN_Media_BlockSize[i] = G_Card_BlockSize;
				G_MLUN_Media_BlockSizeShift[i] = G_Card_BlockSizeShift;
				G_MLUN_Media_ucCardLock[i] = 0;
				HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_NANDF,1);

				G_MLUN_DramSmcNandData = K_MEDIA_NANDF;
			}
		}
		/* add by phil for disk driver lib release e*/
	}
	if (G_MLUN_CardSupport & K_SDF_SUPPORTED) {
		if ((G_ucMSDC_MLUN == 0x01) || ((sD_CdGet() & G_Detect_PinMux) == 0x00)){
			#if (!NAND_SUPPORT_ONLY)
			G_MLUN_LUN_Map[G_MLUN_Supported_Num] = K_MLUN_SDF_LUN_POS;
			G_MLUN_Supported_Num++;
			printf("SS\n");
			G_MLUN_Support[K_MLUN_SDF_LUN_POS] = 1;
			G_MLUN_Support_Type[K_MLUN_SDF_LUN_POS] = K_MEDIA_SDF;

			G_Card_Type = K_MEDIA_SDF;
			i = K_MLUN_SDF_LUN_POS;
			if ((sD_CdGet() & G_Detect_PinMux) == 0x00){
				G_MLUN_Media_Present[i] = 1; //diskDetect();		
			}
			else{ 
				G_MLUN_Media_Present[i] = 0; //diskDetect();		
			}
			//printf("G_MLUN_Media_Present[i]=%bx\n",G_MLUN_Media_Present[i]);		
			//printf("G_Card_TotalSizeMB = %d\n",G_Card_TotalSizeMB);		
			G_MLUN_Media_SizeMB[i] = G_Card_TotalSizeMB;
			G_MLUN_Media_BlockSize[i] = G_Card_BlockSize;
			G_MLUN_Media_BlockSizeShift[i] = G_Card_BlockSizeShift;
			G_MLUN_Media_ucCardLock[i] = usbWPCheck(K_MEDIA_SDF);
			HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_SDF,1);
			HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_EMMC,1);
			#endif
		}
	}
	if (DOS_DosRamStsGet() == TRUE) {
		if (G_MLUN_CardSupport & K_DOSRAM_SUPPORTED) {
			if ((G_ucMSDC_MLUN == 0x01) || ((sD_CdGet() & G_Detect_PinMux) != 0x00)){
			G_MLUN_LUN_Map[G_MLUN_Supported_Num] = K_MLUN_DOSRAM_LUN_POS;
			G_MLUN_Supported_Num++;
			//printf("===DOSRAM Card Support\n");
			G_MLUN_Support[K_MLUN_DOSRAM_LUN_POS] = 1;
			G_MLUN_Support_Type[K_MLUN_DOSRAM_LUN_POS] = K_MEDIA_DOSRAM;
		
			G_Card_Type = K_MEDIA_DOSRAM;
			i = K_MLUN_DOSRAM_LUN_POS;
			G_MLUN_Media_Present[i] = diskDetect();		
			G_MLUN_Media_SizeMB[i] = G_Card_TotalSizeMB;
			G_MLUN_Media_BlockSize[i] = G_Card_BlockSize;
			G_MLUN_Media_BlockSizeShift[i] = G_Card_BlockSizeShift;
			G_MLUN_Media_ucCardLock[i] = 0;
			}
		}
	}
#if (DOS_RSV_PART == 1)	
	if (DOS_RsvPartSizeGet()) {

		pDev = sp1kDevHdlGet();
		pDev += DEV_ID_DISK;
		if (pDev->devType == DEV_TYPE_DISK_NAND) {
			G_MLUN_LUN_Map[G_MLUN_Supported_Num] = K_MLUN_NANDF_RSV_LUN_POS;
			G_MLUN_Supported_Num++;
			printf("NS\n");
			G_MLUN_Support[K_MLUN_NANDF_RSV_LUN_POS] = 1;
			G_MLUN_Support_Type[K_MLUN_NANDF_RSV_LUN_POS] = K_MEDIA_NANDF;
			
			G_Card_Type = K_MEDIA_NANDF;
			i = K_MLUN_NANDF_RSV_LUN_POS;
			G_MLUN_Media_Present[i] = 1; //SMC_Initialize(FALSE);
			//G_MLUN_Media_Present[i] = FALSE;		
			//printf("G_MLUN_Media_Present[i]=%bx\n",G_MLUN_Media_Present[i]);		
			//printf("G_Card_TotalSizeMB = %d\n",G_Card_TotalSizeMB);		
			G_MLUN_Media_SizeMB[i] = G_Card_TotalSizeMB;
			G_MLUN_Media_BlockSize[i] = G_Card_BlockSize;
			G_MLUN_Media_BlockSizeShift[i] = G_Card_BlockSizeShift;
			G_MLUN_Media_ucCardLock[i] = DOS_RsvPartWPGet();
			HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_NANDF,1);
	
			G_MLUN_DramSmcNandData = K_MEDIA_NANDF;
		}
	}
#endif
//	#endif	
	HAL_FlashMode(G_Card_Type,K_MEDIA_NONE,1);
	
	if (G_MLUN_Media_Present[i]) {
		G_ucStorageType = G_Card_Type;
	}	
	else {
		G_ucStorageType = K_MEDIA_NONE;
	}
	G_MLUN_Media_Pos_Current = i;
	G_MLUN_Media_Pos_Next = i;
	G_MLUN_MEdia_Pos_LastCardSelect = i;
	G_USBMSDC_ucCardLock = G_MLUN_Media_ucCardLock[i];
	//patch5.2.1@richie@mmlun end
#endif

}


//-----------------------------------------------------------------------------
//MLUN_CheckCard
//-----------------------------------------------------------------------------
/**
 * @fn        void MLUN_CheckCard(void)
 * @brief     MLUN_CheckCard
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void MLUN_CheckCard(void) USING_0
{
	if (G_MLUN_MEdia_Pos_LastCardSelect == G_MLUN_Media_Pos_Next)
		return;
	#if 0
	if (G_MLUN_Media_Pos_Next == K_MLUN_DOSRAM_LUN_POS) {
		G_Card_Type = K_MEDIA_DOSRAM;
		G_ucStorageType = G_Card_Type;

		return;
	}
	#endif

	HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_NANDF,1);
	HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_SDF,1);
	HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_EMMC,1);
	HAL_FlashMode(K_MEDIA_NONE,K_MEDIA_SMC,1);


	HAL_FlashMode(G_MLUN_Support_Type[G_MLUN_Media_Pos_Next],G_MLUN_Support_Type[G_MLUN_MEdia_Pos_LastCardSelect],0x01);
	G_Card_Type = G_MLUN_Support_Type[G_MLUN_Media_Pos_Next];
	if (G_MLUN_Media_Present[G_MLUN_Media_Pos_Next])
		G_ucStorageType = G_Card_Type;
	else
		G_ucStorageType = K_MEDIA_NONE;

#if 1//def DOS32_SUPPORT
	/* for FAT32 MLUN issue */
	if (G_ucStorageType == K_MEDIA_NANDF){
#if (DOS_RSV_PART == 1)
		if (G_MLUN_Media_Pos_Next == K_MLUN_NANDF_RSV_LUN_POS) {
			DOS_RsvPartActSet(1);
			DOS_CurrDriverSet(DRIVE_NAND_RSV);
			//printf("->(RSV)\n");
		} else {
			DOS_RsvPartActSet(0);
			DOS_CurrDriverSet(DRIVE_NAND);
			//printf("->(N)\n");
		}
#else
		DOS_CurrDriverSet(DRIVE_NAND);
#endif
	}
	else if (G_ucStorageType == K_MEDIA_SDF){
		DOS_CurrDriverSet(DRIVE_SD);
	}
	else if (G_ucStorageType == K_MEDIA_DOSRAM){
		DOS_CurrDriverSet(DRIVE_DRAM);
	}
	else {
		DOS_CurrDriverSet(DRIVE_NONE);
	}
#endif

	G_MLUN_MEdia_Pos_LastCardSelect = G_MLUN_Media_Pos_Next;	
}

void MLUN_CheckCard_Intr(void) USING_0
{
	if (G_MLUN_MEdia_Pos_LastCardSelect == G_MLUN_Media_Pos_Next)
		return;
	#if 0
	if (G_MLUN_Media_Pos_Next == K_MLUN_DOSRAM_LUN_POS) {
		G_Card_Type = K_MEDIA_DOSRAM;
		G_ucStorageType = G_Card_Type;

		return;
	}
	#endif

	HAL_FlashMode_Intr(K_MEDIA_NONE,K_MEDIA_NANDF,1);
	HAL_FlashMode_Intr(K_MEDIA_NONE,K_MEDIA_SDF,1);
	HAL_FlashMode_Intr(K_MEDIA_NONE,K_MEDIA_EMMC,1);
	HAL_FlashMode_Intr(K_MEDIA_NONE,K_MEDIA_SMC,1);


	HAL_FlashMode_Intr(G_MLUN_Support_Type[G_MLUN_Media_Pos_Next],G_MLUN_Support_Type[G_MLUN_MEdia_Pos_LastCardSelect],0x01);
	G_Card_Type = G_MLUN_Support_Type[G_MLUN_Media_Pos_Next];
	if (G_MLUN_Media_Present[G_MLUN_Media_Pos_Next])
		G_ucStorageType = G_Card_Type;
	else
		G_ucStorageType = K_MEDIA_NONE;

#if 1//def DOS32_SUPPORT
	/* for FAT32 MLUN issue */
	if (G_ucStorageType == K_MEDIA_NANDF){
#if (DOS_RSV_PART == 1)
		if (G_MLUN_Media_Pos_Next == K_MLUN_NANDF_RSV_LUN_POS) {
			DOS_RsvPartActSet_Intr(1);
			DOS_CurrDriverSet_Intr(DRIVE_NAND_RSV);
			//printf("->(RSV)\n");
		} else {
			DOS_RsvPartActSet_Intr(0);
			DOS_CurrDriverSet_Intr(DRIVE_NAND);
			//printf("->(N)\n");
		}
#else
		DOS_CurrDriverSet_Intr(DRIVE_NAND);
#endif
	}
	else if (G_ucStorageType == K_MEDIA_SDF){
		DOS_CurrDriverSet_Intr(DRIVE_SD);
	}
	else if (G_ucStorageType == K_MEDIA_DOSRAM){
		DOS_CurrDriverSet_Intr(DRIVE_DRAM);
	}
	else {
		DOS_CurrDriverSet_Intr(DRIVE_NONE);
	}
#endif

	G_MLUN_MEdia_Pos_LastCardSelect = G_MLUN_Media_Pos_Next;	
}

#if (NANDF_OPTION == 1 && SMC_OPTION ==1)
//-----------------------------------------------------------------------------
//MLUN_SaveSMCData
//-----------------------------------------------------------------------------
/**
 * @fn        void MLUN_SaveSMCData(UINT8 cardType)
 * @brief     MLUN_SaveSMCData
 * @param     [in] cardType
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void MLUN_SaveSMCData(UINT8 cardType) USING_0
{
	UINT16 idx;
	PMLUN_SAVED_XDATA pSaveGlobal;

	UINT32 dstRamAddr;
	UINT8 bTemp, bt;// @patch5.2.1_24@wyeo@support xd card@110603

	if (cardType == K_MEDIA_SMC)
	{
		dstRamAddr = G_MLUN_SMC_DramAddr;
	}
	else
	{
		dstRamAddr = G_MLUN_NAND_DramAddr;
	}


	//printf("sizeof(MLUN_SAVED_XDATA)=%d\n",(WORD)sizeof(MLUN_SAVED_XDATA));

	pSaveGlobal = (PMLUN_SAVED_XDATA)G_ucStorData;

	// Save SMC globals
	#if (SMC_OPTION == 1 && NANDF_OPTION ==1)
	pSaveGlobal->smcBankBufferAddr		= G_DOS_SDRAM_WorkAddr;
	pSaveGlobal->mediaBehavior			= G_Media_Behavior;
	pSaveGlobal->smcCardTotalBank		= G_Card_TotalBank;
	pSaveGlobal->smcCardCurrBank		= G_Card_CurrBank;
	pSaveGlobal->smcCardBlackExtSize	= G_Card_BlockExtSize;
	pSaveGlobal->smcCardBlockWholeSize	= G_Card_BlockWholeSize;
	pSaveGlobal->smcCardpageperBlock	= G_Card_PagePerBlock;
	pSaveGlobal->smcPhysicalbanlSize	= G_Card_PhysicalBankSize;
	
	pSaveGlobal->smcAllocateTableSize	= G_SMC_AllocatTableSize;
	pSaveGlobal->smcEntryTAbleSize		= G_SMC_EntryTableSize;
	pSaveGlobal->smcCisBlockAddr		= G_SMC_CisBlockAddress;

	for (idx = 0; idx < K_SMC_EccByteSize; idx++)
		pSaveGlobal->smcEccByte[idx] = G_SMC_EccByte[idx];

	pSaveGlobal->smcEccErr				= G_SMC_EccErr;
	pSaveGlobal->smcErrCode 			= G_SMC_ErrCode;
	pSaveGlobal->smcFirstCISFound		= G_SMC_FirstCISFound;
	pSaveGlobal->smcEccErrCount 		= G_SMC_EccErrCount;
	pSaveGlobal->smcEcc1BitErrCount 	= G_SMC_Ecc1BitErrCount;
	pSaveGlobal->smcEcc2BitErrCount 	= G_SMC_Ecc2BitErrCount;
	
	pSaveGlobal->nandReserveBlockTotal	= G_NAND_ReservBlockTotal; 
	pSaveGlobal->nandReserveBlockMax	= G_NAND_ReservBlockMax;
	pSaveGlobal->smcCisBlockFound		= G_SMC_CisBlockFound;
	pSaveGlobal->smcNewCisBlockAddr		= G_SMC_NewCisBlockAddress;

	#if 1//(K_HAVE_HAL)
	pSaveGlobal->smcAddrMode = G_SMC_AddrMode;
	#endif

// @012403@wyeo@support 256M nand, begin
		pSaveGlobal->smcGoodSectorPerCard = G_SMC_GoodSectorPerCard;
		pSaveGlobal->nandMakerByte = G_NANDF_Maker_Byte;
		pSaveGlobal->nand3RD_Byte = G_NANDF_3RD_Byte;
		pSaveGlobal->nand4TH_Byte = G_NANDF_4TH_Byte;		   
		pSaveGlobal->nand5TH_Byte = G_NANDF_5TH_Byte;
		pSaveGlobal->nandCategory = G_NANDF_Category; // 0 is old, 1 is new and SMC2_
		pSaveGlobal->nandPageSize = G_NANDF_PageSize;
		pSaveGlobal->nandPageExtSize = G_NANDF_PageExtSize;
		pSaveGlobal->nandBlockSize = G_NANDF_BlockSize;
		pSaveGlobal->nandBlockSizeShift = G_NANDF_BlockSizeShift;
		/*
		pSaveGlobal->nandHasOldBlk = G_NANDF_HasOldBlk;
		pSaveGlobal->nandStartLogAddr = G_NANDF_StartLogAddr;
		pSaveGlobal->nandEndBlkAddr = G_NANDF_EndBlkAddr;
		pSaveGlobal->nandNewPhyAddr = G_NANDF_NewPhyAddr;
		pSaveGlobal->nandOldPhyStartAddr = G_NANDF_OldPhyStartAddr;
		pSaveGlobal->nandBlockNum = G_NANDF_OldPhyStartAddr;
		pSaveGlobal->nandOldPhyAddr = G_NANDF_OldPhyAddr;
		pSaveGlobal->nandFindHeaderPage = G_NANDF_FindHeaderPage;
		pSaveGlobal->nandSetHdrPageAddr = G_NANDF_SetHdrPageAddr;
		pSaveGlobal->nandFirstEnter = G_NANDF_FirstEnter;
		pSaveGlobal->nandRsvIndex = G_NANDF_RsvIndex;
		pSaveGlobal->nandStartPage = G_NANDF_StartPage;
		*/
// @012403@wyeo@support 256M nand, end

	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
		for (idx = 0; idx < K_SMC_BUILD_BANK_MAP_SIZE; idx++)
			pSaveGlobal->smcBuildBankMap[idx] = G_SMC_BuildBankMap[idx];
		//SMC_UpdateBankTable( G_Card_CurrBank, FALSE );
	#else
		//SMC_MakeTable(G_Card_CurrBank);
	#endif


	HAL_DmaSetDramAddr(dstRamAddr);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaDo(DMA_PATH_SRAM,DMA_PATH_DRAM,(UINT16)sizeof(MLUN_SAVED_XDATA),0,0);	
	HAL_CpuSramModeSet(0x00);

	dstRamAddr += (512 / 2);
	
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
	//move Bank table in SRAM to DRAM div into 4 step
//patch5.1@richie@mlunsn begin
		SDRAM_Sram2Dram((UINT8 *)(&G_SMC_AllocatTable[0]), dstRamAddr, K_SIZEOF_G_SMC_AllocatTable);
		dstRamAddr += (K_SIZEOF_G_SMC_AllocatTable / 2);

		//SDRAM_Dram2Sram(ramAddr+k, (BYTE *)(&G_SMC_AllocatTable[k]), 32);
/*
for (i = 0;i < 4;i++)
{
		for (idx = 0;idx < (K_SMC_MaxAllocatTableSize / 2);idx++)
		{
			G_ucStorData[idx] = G_SMC_AllocatTable[idx + (i * (K_SMC_MaxAllocatTableSize / 2))];
		}

		HAL_DmaSetDramAddr(dstRamAddr);
		HAL_DmaSetSramAddr(0x0c00);
		HAL_DmaDo(K_DMA_SRAM,K_DMA_DRAM,(K_SMC_MaxAllocatTableSize / 2),0,0);
		HAL_CpuSramModeSet(0x00);

		dstRamAddr += ((K_SMC_MaxAllocatTableSize) / 4);
}
*/
//patch5.1@richie@mlunsn end

	//move Entry table in SRAM to DRAM
		for (idx = 0;idx < K_SMC_MaxEntryTableSize;idx++)
		{
			G_ucStorData[idx] = G_SMC_EntryTable[idx];
		}

	HAL_DmaSetDramAddr(dstRamAddr);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaDo(DMA_PATH_SRAM,DMA_PATH_DRAM,K_SMC_MaxEntryTableSize,0,0); 	
	HAL_CpuSramModeSet(0x00);

	dstRamAddr += (K_SMC_MaxEntryTableSize / 2);

	//move bank data in Dram to Dram
		for (idx = 0; idx < G_Card_TotalBank; idx++)
		{
			// @patch5.2.1_24@wyeo@support xd card@110603, begin
			bTemp = idx >> 3;
			bt = 0x01 << (idx % 8);
			if ( ( G_SMC_BuildBankMap[bTemp] & bt ) == 0 )
			//if (G_SMC_BuildBankMap[idx])
			// @patch5.2.1_24@wyeo@support xd card@110603, end
			{
				UINT32 SrcAddr;

				SrcAddr = G_DOS_SDRAM_WorkAddr + K_SMC_MAX_ClusterSize 
					+ ( ( ( K_SIZEOF_G_SMC_AllocatTable + K_SIZEOF_G_SMC_EntryTable ) * (UINT32)idx ) >> 1 );			
//patch5.1@richie@mlunsn begin

				HAL_DoDRAMDMA(SrcAddr,dstRamAddr,K_SMC_MaxAllocatTableSize * 2 + K_SMC_MaxEntryTableSize);
			}
			dstRamAddr += ((K_SMC_MaxAllocatTableSize * 2 + K_SMC_MaxEntryTableSize) / 2);
//patch5.1@richie@mlunsn end
		}


	
	#else
	#endif


	#endif
}


//-----------------------------------------------------------------------------
//MLUN_LoadSMCData
//-----------------------------------------------------------------------------
/**
 * @fn        void MLUN_LoadSMCData(UINT8 cardType)
 * @brief     MLUN_LoadSMCData
 * @param     [in] cardType
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void MLUN_LoadSMCData(UINT8 cardType) USING_0
{
	UINT16 idx;
	PMLUN_SAVED_XDATA pSaveGlobal;
	UINT8 bTemp, bt;// @patch5.2.1_24@wyeo@support xd card@110603
	
	UINT32 srcRamAddr;
	
	if (cardType == K_MEDIA_NANDF)
	{
		srcRamAddr = G_MLUN_NAND_DramAddr;
	}
	else
	{
		srcRamAddr = G_MLUN_SMC_DramAddr;
	}

	//printf("sizeof(MLUN_SAVED_XDATA)=%d\n",(WORD)sizeof(MLUN_SAVED_XDATA));

	HAL_DmaSetDramAddr(srcRamAddr);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaDo(DMA_PATH_DRAM,DMA_PATH_SRAM,(UINT16)sizeof(MLUN_SAVED_XDATA),0,0);	
	HAL_CpuSramModeSet(0x00);

	srcRamAddr += (512 / 2);


	pSaveGlobal = (PMLUN_SAVED_XDATA)G_ucStorData;

	// Save SMC globals
	#if (SMC_OPTION == 1 && NANDF_OPTION == 1)
	G_DOS_SDRAM_WorkAddr			= pSaveGlobal->smcBankBufferAddr;
	G_Media_Behavior				= pSaveGlobal->mediaBehavior;
	G_Card_TotalBank				= pSaveGlobal->smcCardTotalBank;
	G_Card_CurrBank 				= pSaveGlobal->smcCardCurrBank;
	G_Card_BlockExtSize 			= pSaveGlobal->smcCardBlackExtSize ;
	G_Card_BlockWholeSize			= pSaveGlobal->smcCardBlockWholeSize;
	G_Card_PagePerBlock 			= pSaveGlobal->smcCardpageperBlock;
	G_Card_PhysicalBankSize 		= pSaveGlobal->smcPhysicalbanlSize;
	
	G_SMC_AllocatTableSize			= pSaveGlobal->smcAllocateTableSize;
	G_SMC_EntryTableSize			= pSaveGlobal->smcEntryTAbleSize;
	G_SMC_CisBlockAddress			= pSaveGlobal->smcCisBlockAddr; 

	for (idx = 0; idx < K_SMC_EccByteSize; idx++)
		 G_SMC_EccByte[idx] 		= pSaveGlobal->smcEccByte[idx];

	G_SMC_EccErr					= pSaveGlobal->smcEccErr;
	G_SMC_ErrCode					= pSaveGlobal->smcErrCode;
	G_SMC_FirstCISFound 			= pSaveGlobal->smcFirstCISFound;
	G_SMC_EccErrCount				= pSaveGlobal->smcEccErrCount;
	G_SMC_Ecc1BitErrCount			= pSaveGlobal->smcEcc1BitErrCount;
	G_SMC_Ecc2BitErrCount			= pSaveGlobal->smcEcc2BitErrCount;
	
	G_NAND_ReservBlockTotal 		= pSaveGlobal->nandReserveBlockTotal;
	G_NAND_ReservBlockMax			= pSaveGlobal->nandReserveBlockMax;
	G_SMC_CisBlockFound 			= pSaveGlobal->smcCisBlockFound;
	G_SMC_NewCisBlockAddress		= pSaveGlobal->smcNewCisBlockAddr;

	#if 1//(K_HAVE_HAL)
	G_SMC_AddrMode					= pSaveGlobal->smcAddrMode;
	#endif

// @012403@wyeo@support 256M nand, begin
	G_SMC_GoodSectorPerCard = pSaveGlobal->smcGoodSectorPerCard;
	G_NANDF_Maker_Byte = pSaveGlobal->nandMakerByte;
	G_NANDF_3RD_Byte = pSaveGlobal->nand3RD_Byte;
	G_NANDF_4TH_Byte = pSaveGlobal->nand4TH_Byte;		   
	G_NANDF_5TH_Byte = pSaveGlobal->nand5TH_Byte;
	G_NANDF_5TH_Byte = pSaveGlobal->nandCategory; // 0 is old, 1 is new and SMC2_
	G_NANDF_PageSize = pSaveGlobal->nandPageSize;
	G_NANDF_PageExtSize = pSaveGlobal->nandPageExtSize;
	G_NANDF_BlockSize = pSaveGlobal->nandBlockSize;
	G_NANDF_BlockSizeShift = pSaveGlobal->nandBlockSizeShift;
	/*
	G_NANDF_HasOldBlk = pSaveGlobal->nandHasOldBlk;
	G_NANDF_StartLogAddr = pSaveGlobal->nandStartLogAddr;
	G_NANDF_EndBlkAddr = pSaveGlobal->nandEndBlkAddr;
	G_NANDF_NewPhyAddr = pSaveGlobal->nandNewPhyAddr;
	G_NANDF_OldPhyStartAddr = pSaveGlobal->nandOldPhyStartAddr;
	G_NANDF_OldPhyStartAddr = pSaveGlobal->nandBlockNum;
	G_NANDF_OldPhyAddr = pSaveGlobal->nandOldPhyAddr;
	G_NANDF_FindHeaderPage = pSaveGlobal->nandFindHeaderPage;
	G_NANDF_SetHdrPageAddr = pSaveGlobal->nandSetHdrPageAddr;
	G_NANDF_FirstEnter = pSaveGlobal->nandFirstEnter;
	G_NANDF_RsvIndex = pSaveGlobal->nandRsvIndex;
	G_NANDF_StartPage = pSaveGlobal->nandStartPage;
	*/
// @012403@wyeo@support 256M nand, end
	
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
		for (idx = 0; idx < K_SMC_BUILD_BANK_MAP_SIZE; idx++)
			G_SMC_BuildBankMap[idx] 	= pSaveGlobal->smcBuildBankMap[idx];
		//SMC_UpdateBankTable( G_Card_CurrBank, TRUE );
	#else
		//SMC_MakeTable(G_Card_CurrBank);
	#endif

	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
	//move Bank table in DRAM to SRAM div into 4 step
//patch5.1@richie@mlunsn begin
		SDRAM_Dram2Sram(srcRamAddr, (UINT8 *)(&G_SMC_AllocatTable[0]), K_SIZEOF_G_SMC_AllocatTable);
		srcRamAddr += (K_SIZEOF_G_SMC_AllocatTable / 2);
//patch5.1@richie@mlunsn end
	//move Entry table in DRAM to SRAM
	HAL_DmaSetDramAddr(srcRamAddr);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaDo(DMA_PATH_DRAM,DMA_PATH_SRAM,K_SMC_MaxEntryTableSize,0,0); 	
	HAL_CpuSramModeSet(0x00);

	for (idx = 0;idx < K_SMC_MaxEntryTableSize;idx++)
	{
		G_SMC_EntryTable[idx] = G_ucStorData[idx];
	}

	srcRamAddr += (K_SMC_MaxEntryTableSize / 2);

	//move bank data in Dram to Dram
	for (idx = 0; idx < G_Card_TotalBank; idx++)
	{
		// @patch5.2.1_24@wyeo@support xd card@110603, begin
		bTemp = idx >> 3;
		bt = 0x01 << (idx % 8);
		if ( ( G_SMC_BuildBankMap[bTemp] & bt ) == 0 )
		//if (G_SMC_BuildBankMap[idx])
		// @patch5.2.1_24@wyeo@support xd card@110603, end
		{
			UINT32 DstAddr;

			DstAddr = G_DOS_SDRAM_WorkAddr + K_SMC_MAX_ClusterSize
				+ ( ( ( K_SIZEOF_G_SMC_AllocatTable + K_SIZEOF_G_SMC_EntryTable ) * (UINT32)idx ) >> 1 );
	//patch5.1@richie@mlunsn begin

			HAL_DoDRAMDMA(srcRamAddr,DstAddr,K_SMC_MaxAllocatTableSize * 2 + K_SMC_MaxEntryTableSize);			
		}
		srcRamAddr += ((K_SMC_MaxAllocatTableSize * 2 + K_SMC_MaxEntryTableSize) / 2);
	//patch5.1@richie@mlunsn end
	}
	
	#else
	#endif


	#endif
}
#endif //end of nand && sme


/////#endif //end of define mlun
//patch4.5@richie@mlun end

/**
 * @fn        void usbResetEjectFlag(void)
 * @brief     usbResetEjectFlag
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void usbResetEjectFlag(void) USING_0
{
	UINT8 i;
	for (i = 0;i < K_CARD_SUPPORT_MAX;i++)
	{
		G_MLUN_Media_ucEject[i] = 0;
	}
}

#ifndef _DBG_CLR_WARNING_
/**
 * @fn        void usbSetEject(UINT8 cardType)
 * @brief     usbSetEject
 * @param     [in] cardType
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void usbSetEject(UINT8 cardType) USING_0
{
	if (cardType == K_MEDIA_NANDF)
		G_MLUN_Media_ucEject[K_MLUN_NANDF_LUN_POS] = 1;
	if (cardType == K_MEDIA_CFC)
		G_MLUN_Media_ucEject[K_MLUN_CFC_LUN_POS] = 1;
	if (cardType == K_MEDIA_SDF)
		G_MLUN_Media_ucEject[K_MLUN_SDF_LUN_POS] = 1;
	if (cardType == K_MEDIA_SMC)
		G_MLUN_Media_ucEject[K_MLUN_SMC_LUN_POS] = 1;
	if (cardType == K_MEDIA_DOSRAM)
		G_MLUN_Media_ucEject[K_MLUN_DOSRAM_LUN_POS] = 1;
}

/**
 * @fn        UINT8 usbSetLunPos(UINT8 cardType, cardType,UINT8 pos)
 * @brief     usbSetLunPos
 * @param     [in] cardType
 * @param     [in] pos
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 usbSetLunPos(UINT8 cardType,UINT8 pos) USING_0
{
	UINT8 i;
	
	if (pos >= G_MLUN_Supported_Num)
		return FALSE;

	switch (cardType)
	{
		case K_MEDIA_NANDF :
			if (G_MLUN_Support[K_MLUN_NANDF_LUN_POS])
			{
				for (i = 0;i < G_MLUN_Supported_Num;i++)
				{
					if (G_MLUN_LUN_Map[i] == K_MLUN_NANDF_LUN_POS)
					{
						if (i == pos) return TRUE;
						G_MLUN_LUN_Map[i] = G_MLUN_LUN_Map[pos];
						G_MLUN_LUN_Map[pos] = K_MLUN_NANDF_LUN_POS;
						return TRUE;
					}
				}
			}
			else
				return FALSE;			
			break;
			
		case K_MEDIA_CFC :
			if (G_MLUN_Support[K_MLUN_CFC_LUN_POS])
			{
				for (i = 0;i < G_MLUN_Supported_Num;i++)
				{
					if (G_MLUN_LUN_Map[i] == K_MLUN_CFC_LUN_POS)
					{
						if (i == pos) return TRUE;
						G_MLUN_LUN_Map[i] = G_MLUN_LUN_Map[pos];
						G_MLUN_LUN_Map[pos] = K_MLUN_CFC_LUN_POS;
						return TRUE;
					}
				}
			}
			else
				return FALSE;
			break;

		case K_MEDIA_SDF :
		case K_MEDIA_EMMC :
			if (G_MLUN_Support[K_MLUN_SDF_LUN_POS])
			{
				for (i = 0;i < G_MLUN_Supported_Num;i++)
				{
					if (G_MLUN_LUN_Map[i] == K_MLUN_SDF_LUN_POS)
					{
						if (i == pos) return TRUE;
						G_MLUN_LUN_Map[i] = G_MLUN_LUN_Map[pos];
						G_MLUN_LUN_Map[pos] = K_MLUN_SDF_LUN_POS;
						return TRUE;
					}
				}
			}
			else
				return FALSE;
			break;

		case K_MEDIA_SMC :
			if (G_MLUN_Support[K_MLUN_SMC_LUN_POS])
			{
				for (i = 0;i < G_MLUN_Supported_Num;i++)
				{
					if (G_MLUN_LUN_Map[i] == K_MLUN_SMC_LUN_POS)
					{
						if (i == pos) return TRUE;
						G_MLUN_LUN_Map[i] = G_MLUN_LUN_Map[pos];
						G_MLUN_LUN_Map[pos] = K_MLUN_SMC_LUN_POS;
						return TRUE;
					}
				}
			}
			else
				return FALSE;
			break;

		case K_MEDIA_DOSRAM :
			if (G_MLUN_Support[K_MLUN_DOSRAM_LUN_POS])
			{
				for (i = 0;i < G_MLUN_Supported_Num;i++)
				{
					if (G_MLUN_LUN_Map[i] == K_MLUN_DOSRAM_LUN_POS)
					{
						if (i == pos) return TRUE;
						G_MLUN_LUN_Map[i] = G_MLUN_LUN_Map[pos];
						G_MLUN_LUN_Map[pos] = K_MLUN_DOSRAM_LUN_POS;
						return TRUE;
					}
				}
			}
			else
				return FALSE;
			break;

		default:
			return FALSE;
	}

	return FALSE;
}
#endif /*_DBG_CLR_WARNING_*/

//xian ++ 20090108
#if MWEBTOOL_ENABLE
/**
 * @fn        UINT8 mWebCMD_CheckSecurity(UINT8* pSCSI, UINT32* pSize)
 * @brief     mWebCMD_CheckSecurity
 * @param     [in] pSCSI
 * @param     [in] pSize
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 mWebCMD_CheckSecurity(UINT8* pSCSI, UINT32* pSize) USING_1
{
	UINT8 status = TRUE;
	UINT8 k1, k2;
	UINT8 SecurityData[] = {
		0x09, 0x2F, 0x34, 0x2A, 0x36, 0x2F, 0x29, 0x7A,
		0x0E, 0x3F, 0x39, 0x32, 0x34, 0x35, 0x36, 0x35,
		0x3D, 0x23, 0x7A, 0x19, 0x35, 0x74, 0x76, 0x7A,
		0x16, 0x2E, 0x3E, 0x74
	};
	UINT8 i;
	UINT8 *pBuff = G_pucStorDataPointer;
   
	if (G_ucStorageType != G_Card_Type)
	{
		G_USBMSDC_ucCSWStatus = K_USBMSDC_NoMedium;
		*pSize = 0;
		return (FALSE);	
	}
	
	k1 = pSCSI[4] ^ 0x5A;
	k2 = pSCSI[5] ^ 0x5A;
	
	for (i = 0; i < 28; )
	{
		*pBuff ++ = SecurityData[i++] ^ k1;
		*pBuff ++ = SecurityData[i++] ^ k2; 	   
	}

	*pSize = 28;

	return(status);
}
#endif
//xian --
UINT8 usbWPCheck(UINT8 cardType) USING_0
{
		if (cardType == K_MEDIA_SDF)
		{
			/*
			UINT8 tmp;
			XBYTE[0x2405] &= 0xef;
			tmp = XBYTE[0x2409];
			tmp &= 0x80;
			if (tmp)
			{	
				//printf("WP!!!\n");
				return 1;
			}
			else
			{
				//printf("NOT WP\n");
				return 0;
			}
			*/
			//return HAL_ReadRegister(SD_WP_VLU) & 0x01;
			return sp1kSDWriteProtectDetectGet();
		}
		else if (cardType == K_MEDIA_SMC)
		{
			UINT8 tmp;
			XBYTE[0x2406] &= 0xfd;
			tmp = XBYTE[0x240A];
			tmp &= 0x02;
			if (tmp)
			{	
				//printf("1NOT WP\n");
				return 0;
			}
			else
			{
				//printf("1WP!!!\n");
				return 1;
			}
		}

	return 0;
}

void ejectPwrOffEnable(UINT8 enable)
{
	ejectPowerOffFlag = enable;
}

#endif //end if of if MSDC2

