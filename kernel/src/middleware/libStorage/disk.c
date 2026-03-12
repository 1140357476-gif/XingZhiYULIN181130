/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

        storage.c

Abstract:

        Module related to general storage media

Environment:

        Keil C51 Compiler

Revision History:

        08/28/2001      Chi-Yeh Tsai    created

--*/

// WWW3, whole file

//=============================================================================
//Header file
//=============================================================================
// WWW1 start
#include "general.h"
#include "disk.h"
#include "main.h"
//#include "setmode.h"
#include "cardlink.h"
//#include "cardusr.h"
#include "hal_device.h"
#include "reg_def.h"

// WWW1 end

#include "smcfunc.h"
#include "sdfunc.h"
//patch4.2@richie@ds0511 begin
#if (DOSRAM_OPTION)
#include "dramfunc.h"
#endif
//patch4.2@richie@ds0511 end
#include "cardfunc.h"

//patch4.3@richie@md0603 begin
#include "initio.h"
//#include "sdramhw.h"
//patch4.3@richie@md0603 end
#include "dos32.h"//eric@20080502
#include "doserr.h"
#include "exptcode.h"   

#include "dosvar.h"
#include "cardvar.h"
#include "dcf_api.h"

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//=============================================================================
//Program
//=============================================================================
//extern void appStorageMount (UINT8 type);
//extern void appSdPowerRecycle(void);
extern xdata UINT32 SD_PhySectors;
//-----------------------------------------------------------------------------
//diskInitialize
//-----------------------------------------------------------------------------

UINT8 diskInitialize(UINT8 doDOSInitial) USING_0
/*++

Routine Description:

        general routine to initialize storage media

Arguments:

        none

Return Value:

        none

--*/
{
	UINT8 sts = TRUE;
	devAttr_t* devAttr = NULL;
	if (G_ucStorageType == 0xff){    //first time initialization
		sts = diskDetect();
	}

	switch (G_ucStorageType)
	{
		#if (SDRAM_OPTION == 1)
		case K_MEDIA_None:
			SDRAM_Initialize();
			break;
		#endif

		#if (CARD_OPTION == 1)
		default:
			if (sts){
				//Write date and time    //ada@0321
				dcfRtcTimeUpdate();
				/*HAL_GlobalReadRTC(&G_DATEINFO);
		                 G_DOS_CreateDate = ((UINT16)(G_DATEINFO.Year + 20) << 9) + ((UINT16)(G_DATEINFO.Month) << 5) + (UINT16)(G_DATEINFO.Day);
		                 G_DOS_CreateTime = ((UINT16)(G_DATEINFO.Hour) << 11) + ((UINT16)(G_DATEINFO.Minute) << 5) + ((UINT16)(G_DATEINFO.Second) << 1);
				*/
				//patch4.4@richie@dsi begin
				//restore G_DOS_SDRAM_FAT1Addr
				#if ( DOSRAM_OPTION == 1 )	
				if (G_Card_Type == K_MEDIA_DOSRAM)
				{
					if (G_ulDRAMSTORFAT1AddrTemp != 0xFFFFFFFF)
					{
						G_DOS_SDRAM_FAT1Addr = G_ulDRAMSTORFAT1AddrTemp;
					}
				}
				else
				{
					G_DOS_SDRAM_FAT1Addr = K_SDRAM_DOS_FAT1Addr;
				}
				#endif
				//patch4.4@richie@dsi end

				sts = Card_Initialize(doDOSInitial);
			}
			break;
		#endif
	}

	devAttr = sp1kDevHdlGet();
	if ( G_Card_Type == K_MEDIA_NANDF || G_Card_Type == K_MEDIA_DOSRAM ) {	
		devAttr += DEV_ID_DISK;
	} 
	else {
		devAttr += DEV_ID_CARD;
	}

	if( sts != TRUE ){
		devAttr->devSts = DEV_STS_ERR;
	} 
	else {
		devAttr->devSts = DEV_STS_RDY;
	}
	return sts;
}

//-----------------------------------------------------------------------------
//diskDetect
//-----------------------------------------------------------------------------
UINT8 diskDetect(void) USING_0
/*++

Routine Description:

        detect which storage media exists

Arguments:

        none

Return Value:

        none

--*/
{
    UINT8 status = FALSE;
    G_ucStorageType = K_MEDIA_None;
	if (Card_Detect() == TRUE)
	{
		G_ucStorageType = G_Card_Type;
		status = TRUE;
	}
	else{
		printf("diskDetect fail= %bx %bx\n",G_Card_Type,G_ucStorageType);
	}		
	return status;
	
}

//-----------------------------------------------------------------------------
//diskFormat
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For Date & Time Update
//patch3.2@ada@0401 For Multi-Frame Play Back
UINT8 diskFormat(UINT8 eraseCard) USING_0
/*++

Routine Description:

        format media currently selected

Arguments:

        none

Return Value:

        none

--*/
{
	UINT8    sts = TRUE;
	devAttr_t* devAttr = NULL;
	UINT8 devId;
	UINT32 diskTotalSize;
	//eraseCard = eraseCard;
    switch (G_ucStorageType)
    {
		#if (SDRAM_OPTION == 1)
		case K_MEDIA_None:
			SDRAM_Format();
			break;
		#endif
		//eric@20080502,begin

		case K_MEDIA_NANDF:
		#if (DOS_RSV_PART == 1)
			if (DOS_RsvPartStsGet() && DOS_RsvPartActGet()) {
			    sts = DOS_Format(DRIVE_NAND_RSV, "NAND-FAT16", 16, eraseCard);
				devId = DRIVE_NAND_RSV;
			} else {
			    sts = DOS_Format(DRIVE_NAND, "NAND-FAT16", 16, eraseCard);
				devId = DRIVE_NAND;
			}
		#else
			sts = DOS_Format(DRIVE_NAND, "NAND-FAT16", 16, eraseCard);
			devId = DRIVE_NAND;
		#endif                
  			break;
		
		case K_MEDIA_SDF:
			//appStorageMount(K_MEDIA_SDF);
			//check if sd_detect ok
			devId = DRIVE_SD;
			if((G_Card_Supported & ((UINT16)1) << K_SDF_SUPPORTED_BIT) == 0){
				printf("SD card error or not supported!\n");
				sts = FALSE;
				break;
			}

			diskTotalSize = DOS_TotalSize(DRIVE_SD);
			if(diskTotalSize == 0){//may dosMount error
				diskTotalSize =  SD_PhySectors >> 1;
			}
			if( diskTotalSize > 0x200000 ){// if over 2GB
	   			sts = DOS_Format(DRIVE_SD, "SD-FAT32", 32, eraseCard);
			} 
			else{
	  			sts = DOS_Format(DRIVE_SD, "SD-FAT16", 16, eraseCard);
			}
			break;
			
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			devId = DRIVE_DRAM;
			sts = DOS_Format(DRIVE_DRAM, "NAND-FAT16", 16, eraseCard);
			break;
		#endif
		
		default:
			break;
			//eric@20080502,end
	}

 	/*add by Phil @ 08.04.15 s*/
	devAttr = sp1kDevHdlGet();
	if (G_ucStorageType == K_MEDIA_SDF) {
		devAttr += DEV_ID_CARD;
	} else {
		devAttr += DEV_ID_DISK;
	}
	
	if (sts != TRUE) {
		//printf("error!\n");
		DOS_FormatErrProc(devId);	//xian ++ #40938
		devAttr->devSts = DEV_STS_ERR;
	} else {
		//printf("ready!\n");
		devAttr->devSts = DEV_STS_RDY;
	}

	//devAttr->devSts = DEV_STS_RDY; //Phil: force to ready?
	
	/*add by Phil @ 08.04.15 e*/

	printf("fmt card: %bu\n", sts);

	return sts;
}
#if 0

//-----------------------------------------------------------------------------
//STORAGE_CaptureObject
//-----------------------------------------------------------------------------
UINT8 STORAGE_CaptureObject(UINT16 objType) USING_0
/*++

Routine Description:

        capture one object to storage media currently selected

Arguments:

        objType - type of object to capture, where 0 is JPEG file
                                                   1 is WAV file
                                                   2 is video only AVI file
                                                   3 is audio/video AVI file

Return Value:

        none

--*/
{
	UINT8 sts = TRUE;
	
        switch (G_ucStorageType)
        {
#if (SDRAM_OPTION == 1)
                case K_MEDIA_None:
                        SDRAM_CaptureObject(objType);
                        break;
#endif

                default:
#if (CARD_OPTION == 1)
			sts = Card_CaptureObject(objType);
#endif
			
                        break;
                	
        }
	
        return sts;
}
//-----------------------------------------------------------------------------
//STORAGE_DeleteObject
//-----------------------------------------------------------------------------
UINT8 STORAGE_DeleteObject(UINT16 objIndex) USING_0
/*++

Routine Description:

        delete one object to storage media currently selected

Arguments:

        objIndex - index of object to delete, where 0 is delete all
                                                    the other is delete one selected

Return Value:

        none

--*/
{
	UINT8	sts = TRUE;
	
        switch (G_ucStorageType)
        {
#if (SDRAM_OPTION == 1)
                case K_MEDIA_None:
                        SDRAM_DeleteObject(objIndex);
                        break;
#endif

                default:
#if (CARD_OPTION == 1)
			sts = Card_DeleteObject(objIndex);
#endif
                        break;
        }
        return sts;
}

//-----------------------------------------------------------------------------
//STORAGE_PlaybackObject
//-----------------------------------------------------------------------------
UINT8 STORAGE_PlaybackObject(UINT16 objType, UINT16 objIndex) USING_0
/*++

Routine Description:

        playback one object from storage media currently selected

Arguments:

        objType - type of playback, where 0 is nine thumbnail playback
                                          1 is four thumbnail playback
                                          2 is one image playback
                                          3 is image slide show playback
                                          4 is video playback
        objIndex - index of object to playback

Return Value:

        none

--*/
{
	UINT8	sts = TRUE;

        switch (G_ucStorageType)
        {
#if (SDRAM_OPTION == 1)
                case K_MEDIA_None:
                        SDRAM_PlaybackObject(objType, objIndex);
                        break;
#endif

                default:
#if (CARD_OPTION == 1)
#if (PLAYBACK_OPTION)
                        sts = Card_PlaybackObject(objType, objIndex);
#endif                        
#endif
                        break;
        }
        
        return sts;
}

//-----------------------------------------------------------------------------
//STORAGE_UploadObject
//-----------------------------------------------------------------------------
UINT8 STORAGE_UploadObject(UINT16 objType, UINT16 objIndex) USING_0
/*++

Routine Description:

        upload one object from storage media currently selected

Arguments:

        objType - type of object to capture, where 0 is to get object count
                                                   1 is to get object FDBs
                                                   2 is to upload one object
        objIndex - index of object to upload

Return Value:

        none

--*/
{
	UINT8	sts = TRUE;
	
        switch (G_ucStorageType)
        {
#if (SDRAM_OPTION == 1)
                case K_MEDIA_None:
                        SDRAM_UploadObject(objType, objIndex);
                        break;
#endif
                default:
#if (CARD_OPTION == 1)
			sts = Card_UploadObject(objType, objIndex);
#endif
                        break;
        }
        
        return sts;
}

//patch4.3@richie@md0603 begin
#if (SINGLE_DIRECTORY == 0)
//-----------------------------------------------------------------------------
//STORAGE_GetDirData
//-----------------------------------------------------------------------------
UINT8 STORAGE_GetDirData(void) USING_0
/*++

Routine Description:

        upload all dir data by index from storage media currently selected

Return Value:

        none

--*/
{
	UINT8	sts = TRUE;

        SDRAM_Dram2Usb(K_SDRAM_DcfDirBufAddr, G_USR_TotalDirectory * 14);	
        
        return sts;
}
		
#endif
#endif
//patch4.3@richie@md0603 end

//xian move form cardlink.c
/**
 * @fn        void diskGlobalInit(void)
 * @brief     Initialixe global for disk
 * @param     NONE
 * @retval    NONE
 * @see       NULL
*/
void diskGlobalInit(void) USING_0
{
        UINT8 k;

	// wyeo@0514
	#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
	// control the G_NAND_ReservBlockTotal value to
	// reserve nand block for you own purpose.
	// The value must not exceed 20.
	// good block 0 MUST for used by sensor bad pixel
	// good block 1 MUST for used by reserve
	// good block 2, 3 MUST for used by DOS
	// good block 4, 5, 6, 7 for vendor used
	//G_NAND_ReservBlockTotal = 8; 	// suggest 8 block, should not exceed 20
	//G_NAND_ReservBlockMax = 20;	// must 20
	#endif

#if 0 //xian -- 20090107, 
	for ( k= 0;k <4; k++ )
	{
		G_USR_Company_UpperSIG[k] = C_USR_Company_UpperSIG[k];
		G_USR_Company_LowerSIG[k] = C_USR_Company_LowerSIG[k]; // @051701
	}
#endif    
	for ( k= 0;k <12; k++ )
	{
		G_USR_Dir0Name[k] = C_USR_Dir0Name[k];
		//G_USR_Dir1Name[k] = C_USR_Dir1Name[k];  //rhy masked 20110504
	}

        #if (K_DOS_HaveLinearFAT)
        // always find maximal free cluster when set to 0xFFFF
        G_DOS_LinearFATMax = 0xFFFF;
        // Turn off linear FAT searching. Should always keep
        // off uless doing avi recording and playing
        G_DOS_UseLinearFAT = 0;
        #endif
        
        #if (SMC_OPTION == 1 || NANDF_OPTION == 1)
        #if (K_PreWriteOldBlock)
        G_SMC_EnablePreWriteOldBlock = 1;
        #endif
        #endif
        
        //xian 20100507 move to video_clip.c      
        //G_Image_QTableIndex = K_Qxx;
        
        Card_InitLibGlobal();
        
        return;
}

/**
 * @fn        void diskMemInit(void)
 * @brief     Initialize memory for disk
 * @param     NONE
 * @retval    NONE
 * @see       NULL
*/
void diskMemInit(void) USING_0
{
        G_DOS_SDRAM_WorkAddr = K_SDRAM_DOS_WorkAddr;
        G_DOS_SDRAM_WorkSize = K_SDRAM_DOS_WorkSize;
        G_DOS_SDRAM_FAT1Addr = K_SDRAM_DOS_FAT1Addr;
        
        #if (K_DOS_HaveLinearFAT)
        // linear FAT is located at DOS working buffer 128K offset, size must
        // at least 128K byte for FAT16.
        G_DOS_SDRAM_LinearFATAddr = G_DOS_SDRAM_WorkAddr + (UINT32)0x00010000;
        
        // @patch5.2.1@wyeo@support xd card@072903, when xd use this
        #if 0
	G_DOS_SDRAM_LinearFATAddr = G_DOS_SDRAM_WorkAddr + (UINT32)0x00028000;
	#endif
	
        #endif
        
        #if ( K_File_HaveVideo == 1 )
	#if 0 //suny mark		
        G_File_SDRAM_AviIdx1BufAddr = K_SDRAM_Avi_Idx1BufAddr;
        G_File_SDRAM_AviIdx1BufSize = K_SDRAM_Avi_Idx1BufSize;
	#endif	
        #if 0//( AUDIO_OPTION == 1 )
        G_File_SDRAM_AudioBufAddr = K_SDRAM_AudioBufAddr;
        G_File_SDRAM_AudioBufSize = K_SDRAM_AudioBufSize;
        #endif
        #endif
        //G_SDRAM_ImageAddr = K_SDRAM_AviCaptureIdx1Addr;
        G_DOS_SDRAM_MaxAddr = K_SDRAM_TotalSize - 1;

	#if ( DOSRAM_OPTION )
	//patch4.2@richie@ds0509 marked for start addr
	//patch4.2@richie@ds start addr
	//G_DOS_SDRAM_FAT1Addr = G_Card_DOS_SDRAM_FAT1Addr;
	//G_DRM_SDRAM_StartAddr = K_SDRAM_DRM_StartAddr;
	//G_DRM_SDRAM_CardSize = K_SDRAM_DRM_CardSize;
	#endif
	
        return;
}
//xian --

//xian move from cardptch.c
// patch4.4-9@wyeo@select nand, sdram as default, begin
// when select nand or dram as default and the card inserted when
// power on, the card may be written for that fmpio for card is
// in floating state, so must always pull high to disable card.
// Later, you switch card, then card library will enable card.
// Before calling this function use, you must set G_Card_Type.
// It si suggested that call this function on resume event.
void diskInactivate(void) USING_0
{
	#if 1//(K_HAVE_HAL)

	//#if ((K_CARD_TOTAL >= 2) && (K_MEDIA_BUILT_IN != K_MEDIA_CARD))
	if (G_Card_Type == K_MEDIA_NANDF || \
		G_Card_Type == K_MEDIA_DOSRAM || \
		G_Card_Type == K_MEDIA_NONE )
	{

		// patch 4.4.1@wyeo@fix mmc card is inactivated, begin
		XBYTE[REG_Fm_ctrlo_1] &= 0xF0;	//XBYTE[REG_Fm_gpioRint_1] &= 0xF0;
		XBYTE[REG_Fm_ctrloe_1] &= 0xF0;	//XBYTE[0x2405] &= 0xF0;
		// end

		if (G_Card_Type == K_MEDIA_DOSRAM || G_Card_Type == K_MEDIA_NONE)
		{
			// for when target is sd, mmc is also included
			#if (K_MEDIA_CARD == K_MEDIA_SDF)
			HAL_FlashMode(K_MEDIA_NONE, K_MEDIA_EMMC, 0x01);
			#elif (K_MEDIA_CARD == K_MEDIA_EMMC)
			HAL_FlashMode(K_MEDIA_NONE, K_MEDIA_SDF, 0x01);
			#endif
			if (G_Card_Type == K_MEDIA_CARD)
				HAL_FlashMode(K_MEDIA_NONE, 0, 0x01);
			else
				HAL_FlashMode(K_MEDIA_NONE, K_MEDIA_CARD, 0x01);
			XBYTE[REG_Fm_Mediatype_rst] = K_MEDIA_NONE;
		}
		else
		{

			// for when target is sd, mmc is also included
			#if (K_MEDIA_CARD == K_MEDIA_SDF)
			HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_EMMC, 0x01);
			#elif (K_MEDIA_CARD == K_MEDIA_EMMC)
			HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_SDF, 0x01);
			#endif

			if (G_Card_Type == K_MEDIA_CARD)
				HAL_FlashMode(K_MEDIA_NANDF, 0, 0x01);
			else
				HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_CARD, 0x01);
		}
	}
	#endif

	//#endif

	return;
}

void Card_InitLibGlobal(void) USING_0
{
	G_File_SDRAM_Read = 0;
	G_File_AudioEnable = 0;
	G_Card_Type = K_MEDIA_TYPE;
	G_Card_Supported = 0;
	G_Card_Module = 0;
	G_Card_UI_Action = 0;
	G_DOS_Stop_Initial = FALSE;

#if 0//( K_DEBUG_CHECK_TIME )
	G_LONG_1 = 0;
	G_LONG_2 = 0;
	G_LONG_3 = 0;
#endif

	G_DOS_CreateTime = 0;
	G_DOS_CreateDate = 0x0021;

	return;
}

#if ( K_DOS_ErrOption == 1 )
void Card_ClearAllError(void) USING_0
{
        M_DOS_ClearError();
        M_Card_ClearExptCode();
        #if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
        M_SMC_ClearError();
        #endif
}
#endif

#if ( K_Card_ExptOption == 1 )

xdata UINT16 G_Card_ExptCode = 0;

void Card_SetExptCode(UINT16 exptCode) USING_0
{
	if ( !G_Card_ExptCode )
		G_Card_ExptCode = exptCode;
}

void Card_ClearExptCode(void) USING_0
{
	G_Card_ExptCode = 0;
}

UINT16 Card_GetExptCode(void) USING_0
{
	return G_Card_ExptCode;
}
#endif

UINT8 Card_Initialize(UINT8 doDOSInitial) USING_0
{
	UINT8 sts = FALSE;
	//printf("c1\n");
	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_Initialize(doDOSInitial);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_Initialize(doDOSInitial);
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
		case K_MEDIA_EMMC:
			sts = SD_Initialize(doDOSInitial);
		break;
		#endif

		//patch4.2@richie@ds0509 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_Initialize(doDOSInitial);
		break;
		#endif
		//patch4.2@richie@ds0509 end

		default:
		break;
	}

	return sts;
}

#if 0
UINT8 Card_GetID(void) USING_0
{
	UINT8 sts = 0;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_GetID();
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_GetID();
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			//sts = SD_GetID();
		break;
		#endif

          //ada@0411 testmmc
		#if ( MMC_OPTION == 1 )
		case K_MEDIA_EMMC:
			//sts = MMC_GetID();
		break;
		#endif

		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			//patch4.2@richie@ds0511 marked
			//sts = DRM_GetID();
		break;
		#endif

		default:
		break;
	}

	return sts;
}
#endif

UINT8 Card_Detect(void) USING_0
{
	UINT8 sts = 0;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_CardDetect();
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_CardDetect();
		break;
		#endif

		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_CardDetect(K_MEDIA_SDF);

		#if 1//(K_HAVE_HAL) // wyeo@0521, patch 4.2
               //version4.0@ada@0513 for MMC
               if (sts)
               {
			//printf("SD_CardDetect success\n");
          		break;
               }
               else
               {
                    //G_Card_Type = K_MEDIA_EMMC;
                    break;// for card detect always return TRUE
               }
               #endif

		#endif

		//patch4.2@richie@ds0509 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_CardDetect();
		break;
		#endif
		//patch4.2@richie@ds0509 end

		default:
		break;
	}

	return sts;
}

UINT16 Card_GetBehavior(void) USING_0
{
	UINT16 behavior = 0;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			behavior = SMC_GetBehavior();
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			behavior = CF_GetBehavior();
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			behavior = SD_GetBehavior();
		break;
		#endif


		//patch4.2@richie@ds0509 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			behavior = DRAM_GetBehavior();
		break;
		#endif
		//patch4.2@richie@ds0509 end

		default:
		break;
	}

	return behavior;
}

/**
 * @fn        UINT8 diskErrorCodeGet(void)
 * @brief     Get the error code of disk operation
 * @param     NONE
 * @retval    return = SUCCESS / Error code, The error code is one of DISK_ERR_DETECT or DISK_ERR_FS_INIT.
 * @see       DISK_ERR_CODE_e.
*/
UINT8 diskErrorCodeGet(void)
{
	UINT8 err = 0;
	if(G_DOS_Initial_Status == 3){
		err = DISK_ERR_DETECT;
	}
	else if (G_DOS_Initial_Status != 0xff){
		err = DISK_ERR_FS_INIT;
	}
	return err;
}

