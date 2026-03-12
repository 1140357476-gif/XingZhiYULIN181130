#include "general.h"
#include "hal.h"
#include "exptcode.h"   

//#include "cardcon.h"
#include "cardfunc.h"
#include "cardvar.h"

#include "doslink.h"
#include "doserr.h"

#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
#include "smccon.h"
#include "smcfunc.h"
#include "smcvar.h"
#include "smcerr.h"
#else
#define K_SMC_DO_USB_ECC_BY_HOST	0
#endif

#if ( DOSRAM_OPTION == 1 )
//patch4.2@richie@ds0509 mark
//#include "drmcon.h"
//#include "drmfunc.h"
//#include "drmvar.h"
//patch4.2@richie@ds0509
#include "dramfunc.h"
#endif

#if ( SDF_OPTION == 1 )
//#include "sdcon.h"
#include "sdfunc.h"
//#include "sdvar.h"
#endif

//#include "cardebug.h"

//void Card_FillLong(UINT8 *dataAry, UINT16 offset, UINT32 ul);

extern xdata UINT8 G_Card_Type;
xdata UINT16 G_Card_Supported;
xdata UINT16 G_Card_Module;
xdata UINT16 G_Card_UI_Action;
xdata UINT32 G_Card_DOS_SDRAM_FAT1Addr;

#if 0 //xian move to disk.c
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
	if (	G_Card_Type == K_MEDIA_NANDF || \
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
#endif
#if 0
void Card_FillLong(UINT8 *dataAry, UINT16 offset, UINT32 ul) USING_1
{
	dataAry[offset]   = READ8(ul,3);
	dataAry[offset+1] = READ8(ul,2);
	dataAry[offset+2] = READ8(ul,1);
	dataAry[offset+3] = READ8(ul,0);
}


UINT8 Card_Specific(UINT16 specific, UINT32 refCon) USING_0
{
	UINT8 sts = TRUE;
	specific = 0;
	#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
	#if ( K_SMC_BUILD_BANK_AT_SDRAM )
	if ( G_Card_Type == K_MEDIA_SMC || G_Card_Type == K_MEDIA_NANDF )
	{
		SMC_BuildBankTable(refCon); // @patch5.2.1@wyeo@support xd card@072903
	}
	#endif
	#endif

	return sts;
}
#endif

#if 0 //xian move to disk.c
void diskInactivate(void) USING_0
{
        G_File_SDRAM_Read = 0;
        G_File_AudioEnable = 0;
	G_Card_Type = K_MEDIA_TYPE;
	G_Card_Supported = 0;
	G_Card_Module = 0;
	G_Card_UI_Action = 0;
        G_DOS_Stop_Initial = FALSE;

        #if ( K_DEBUG_CHECK_TIME )
	G_LONG_1 = 0;
	G_LONG_2 = 0;
	G_LONG_3 = 0;
	#endif

	G_DOS_CreateTime = 0;
	G_DOS_CreateDate = 0x0021;

	return;
}
#endif
#if 0
#if ( K_LINK_OLDCODE == 0 )

//-----------------------------------------------------------------------------
//Card_GetInformation
//
//-----------------------------------------------------------------------------
/*
	00	BYTE		structure len
	01 	WORD		G_Card_Module
	03	WORD		K_Card_Version		// Ex 1.0.0 = 0x0100
	05	BYTE		K_Card_Statge		// Final = 0x80, Beta = 0x60, Alpha = 0x40, Development = 0x20
	06	BYTE		K_Card_Non_Release	// Release = 0, Non Release = other value
	07 	WORD		G_Card_Supported
	09 	BYTE		G_Card_Type
	10 	UINT32		G_Card_SectorPerCard

	14	BYTE		G_DOS_SystemId
	15	BYTE		G_DOS_FileSystemType
	16 	WORD 		G_DOS_SectorSize
	18 	WORD		G_DOS_SectorPerCluster
	20 	UINT32		G_DOS_TotalFreeCluster
	24	UINT32		G_DOS_FatAddr
	28	UINT32		G_DOS_FatEndAddr
	32	UINT32		G_DOS_SectorPerFat
	36	UINT32		G_DOS_TotalRootDir
	40	UINT32		G_DOS_RootDirAddr
	44	UINT32		G_DOS_DataAddr
	48
*/
UINT16 Card_GetInformation(UINT8 *buf) USING_1
{
	UINT32 ultemp;
	UINT8 k;

	for ( k=0; k < 64; k++ )
		buf[k] = 0;

	buf[0]=k; // structure len

	// G_Card_Module, WORD
	G_Card_Module = ( ((UINT16)SDRAM_OPTION) << K_SDRAM_SUPPORTED_BIT ) |
			( ((UINT16)DOSRAM_OPTION) << K_DOSRAM_SUPPORTED_BIT ) |
			( ((UINT16)NANDF_OPTION) << K_NANDF_SUPPORTED_BIT ) |
			( ((UINT16)SMC_OPTION) << K_SMC_SUPPORTED_BIT ) |
			( ((UINT16)CFC_OPTION) << K_CFC_SUPPORTED_BIT ) |
			( ((UINT16)SDF_OPTION) << K_SDF_SUPPORTED_BIT ) |
			( ((UINT16)EMMC_OPTION) << K_EMMC_SUPPORTED_BIT ) |
			( ((UINT16)NORF_OPTION) << K_NORF_SUPPORTED_BIT ) |
			( ((UINT16)NEXTF_OPTION) << K_NEXTF_SUPPORTED_BIT );
	buf[1] = G_Card_Module;
	buf[2] = G_Card_Module >> 8;

	// K_Card_Version, WORD
	buf[3] = K_Card_Version;
	buf[4] = K_Card_Version >> 8;

	// K_Card_Statge, BYTE
	buf[5] = K_Card_Statge;

	// K_Card_Non_Release, BYTE
	buf[6] = K_Card_Non_Release;

	// G_Card_Supported, WORD
	buf[7] = G_Card_Supported;
	buf[8] = G_Card_Supported >> 8;

	// G_Card_Type, BYTE
	buf[9] = G_Card_Type;

	// G_Card_SectorPerCard, UINT32
	Card_FillLong(buf, 10, G_Card_SectorPerCard);

	// G_DOS_SystemId
	buf[14] = G_DOS_SystemId;

	// G_DOS_FileSystemType
	buf[15] = G_DOS_FileSystemType;

	// G_DOS_SectorSize
	buf[16] = G_DOS_SectorSize;
	buf[17] = G_DOS_SectorSize >> 8;

	// G_DOS_SectorPerCluster
	ultemp = G_DOS_ClusterSize / G_DOS_SectorSize;
	buf[18] = ultemp;
	buf[19] = ultemp >> 8;

	// G_DOS_TotalFreeCluster
	Card_FillLong(buf, 20, G_DOS_TotalFreeCluster);

	#if 1//(SQUEEZE_CODE_SPACE<=8)
	//#if 1
	// G_DOS_FatAddr
	Card_FillLong(buf, 24, G_DOS_FatAddr);

	// G_DOS_FatEndAddr
	Card_FillLong(buf, 28, G_DOS_FatEndAddr);

	// G_DOS_SectorPerFat
	Card_FillLong(buf, 32, G_DOS_SectorPerFat);
	#endif


	#if ( K_DEBUG_CHECK_TIME )
	// G_DOS_TotalRootDir
	Card_FillLong(buf, 36, G_LONG_1);

	// G_DOS_RootDirAddr
	Card_FillLong(buf, 40, G_LONG_2);

	// G_DOS_DataAddr
	Card_FillLong(buf, 44, G_LONG_3);

	#else

	#if 1//(SQUEEZE_CODE_SPACE<=8)
	//#if 1
	// G_DOS_TotalRootDir
	Card_FillLong(buf, 36, G_DOS_TotalRootDir);

	// G_DOS_RootDirAddr
	Card_FillLong(buf, 40, G_DOS_RootDirAddr);

	// G_DOS_DataAddr
	Card_FillLong(buf, 44, G_DOS_DataAddr);
	#endif

	#endif

	// Support flags
	#if (K_SMC_DO_USB_ECC_BY_HOST )
	buf[48] |= (K_SMC_DO_USB_ECC_BY_HOST << 2);
	#endif

	// DEBUG
	#if ( K_DEBUG_CHECK_TIME )
	buf[48] = G_DOS_Head;
	buf[49] = G_DOS_Cylinder;
	buf[50] = G_DOS_Cylinder>>8;
	buf[51] = G_DOS_SectorPerTrack;
	buf[52] = G_DOS_SectorPerTrack>>8;
	Card_FillLong(buf, 53, G_DOS_HiddenSector);
	#endif

	return buf[0];
}
#endif
#endif

#if 0 //xian move to disk.c
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
#endif

#if ( K_MEDIA_ONLYONE == 0 )

#if 0 //xian move to disk.c
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
		#if ( EMMC_OPTION == 1 )
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
               //version4.0@ada@0513 for EMMC
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
#endif

UINT8 Card_LCard2Sram(UINT32 logAddr) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_LCard2Sram(logAddr);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_LCard2Sram(logAddr);
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LCard2Sram(logAddr);
		break;
		#endif

		//patch4.2@richie@ds0505 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_LCard2Sram(logAddr);
		break;
		#endif
		//patch4.2@richie@ds0505 end

		default:
		break;
	}

	return sts;
}

#if 1//( PLAYBACK_OPTION )
#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 /*|| K_HAVE_HAL == 0*/ )
UINT8 Card_LPCard2Sram(UINT32 logAddr, UINT32 phyAddr) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_Card2Sram(phyAddr);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_LCard2Sram(logAddr);
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LCard2Sram(logAddr);
		break;
		#endif


		//patch4.2@richie@ds0505 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_LCard2Sram(logAddr);
		break;
		#endif
		//patch4.2@richie@ds0505 end

		default:
		break;
	}

	return sts;
}

UINT8 diskPhySectorRead(UINT32 logAddr, UINT32 phyAddr, UINT32 ramAddr) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_Card2Dram(phyAddr, ramAddr);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_LCard2Dram(logAddr,ramAddr);
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LCard2Dram(logAddr,ramAddr);
		break;
		#endif


		//patch4.2@richie@ds0505 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_LCard2DramMore(logAddr,ramAddr,1);
		break;
		#endif
		//patch4.2@richie@ds0505 end

		default:
		break;
	}

	return sts;
}
#endif
#endif

UINT8 diskSectorRead(UINT32 logAddr, UINT32 ramAddr) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_LCard2Dram(logAddr,ramAddr,K_Card_PageSize);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_LCard2Dram(logAddr,ramAddr);
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LCard2Dram(logAddr,ramAddr);
		break;
		#endif

		//patch4.2@richie@ds0505 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_LCard2DramMore(logAddr,ramAddr,1);
		break;
		#endif
		//patch4.2@richie@ds0505 end

		default:
		break;
	}

	return sts;
}

UINT8 diskPhyBlockWrite(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum, UINT32 ramAddr) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_Dram2Card(phyAddr,blockNum,ramAddr);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_LDram2Card(logAddr,ramAddr);
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LDram2Card(logAddr,ramAddr);
		break;
		#endif


		//patch4.2@richie@ds0509 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_LDram2CardMore(logAddr, ramAddr, 1);
		break;
		#endif
		//patch4.2@richie@ds0509 end

		default:
		break;
	}

 	phyAddr = phyAddr;
	blockNum = blockNum;

	return sts;
}

UINT8 Card_LCard2Usb(UINT32 logAddr) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_LCard2Usb(logAddr, 0);
		break;
		#endif

		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LCard2Usb(logAddr); // wyeo@0509
		break;
		#endif


		//patch4.2@richie@ds0511 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_LCard2Usb(logAddr);
		break;
		#endif
		//patch4.2@richie@ds0511 end

		default:
		break;
	}

	return sts;
}

UINT8 diskSectorWrite(UINT32 logAddr, UINT32 ramAddr ) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_LDram2Card(logAddr, ramAddr, K_Card_PageSize);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_LDram2Card(logAddr, ramAddr);
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LDram2Card(logAddr, ramAddr);
		break;
		#endif


		//patch4.2@richie@ds0505 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_LDram2CardMore(logAddr, ramAddr, 1);
		break;
		#endif
		//patch4.2@richie@ds0505 end

		default:
		break;
	}

	return sts;
}

UINT8 Card_LPSram2Card(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_Sram2Card( phyAddr, blockNum );
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_LSram2Card( logAddr );
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LSram2Card( logAddr );
		break;
		#endif


		//patch4.2@richie@ds0509 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_LSram2Card( logAddr );
		break;
		#endif
		//patch4.2@richie@ds0509 end

		default:
		break;
	}

 	phyAddr = phyAddr;
	blockNum = blockNum;

	return sts;
}

#if ( K_File_HaveVideo == 1 )

#if 0//( K_HAVE_HAL == 0 )
UINT8 Card_LPVideoFifo2Card(UINT32 logAddr, UINT32 phyAddr, UINT16 blockNum ) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			//sts = SMC_VideoFifo2Card( phyAddr, blockNum );
			sts = SMC_Fifo2Card(K_FIFO_DRAM2NANDF,phyAddr,blockNum);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_LVideoFifo2Card( logAddr );
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_LVideoFifo2Card( logAddr );
		break;
		#endif

          //ada@0411 test mmc
		#if ( EMMC_OPTION == 1 )
		case K_MEDIA_EMMC:
			sts = MMC_LVideoFifo2Card( logAddr );
		break;
		#endif

		//patch4.2@richie@ds0511 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRM_LVideoFifo2Card( logAddr );
		break;
		#endif
		//patch4.2@richie@ds0511 end

		default:
		break;
	}
	return sts;
}
#endif

#endif // K_File_HaveVideo

UINT8 Card_EraseTotalBlock(void) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_EraseTotalBlock();
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_EraseTotalBlock();
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			//sts = SD_EraseTotalBlock();
		break;
		#endif

		//patch4.2@richie@ds0511 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRAM_EraseTotalBlock();
		break;
		#endif
		//patch4.2@richie@ds0511 end

		default:
		break;
	}

	return sts;
}

#if 0
UINT8 Card_EraseOneBlock(UINT32 logAddr, UINT32 phyAddr) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = SMC_EraseOneBlock(phyAddr);
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_EraseOneBlock(logAddr);
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_EraseOneBlock(logAddr);
		break;
		#endif

          //ada@0411 test mmc
		#if ( EMMC_OPTION == 1 )
		case K_MEDIA_EMMC:
			sts = MMC_EraseOneBlock(logAddr);
		break;
		#endif

		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRM_EraseOneBlock(logAddr);
		break;
		#endif
		default:
		break;
	}

	return sts;
}
#endif

#if 0
UINT8 Card_BlockDram2Card( UINT32 srcRamAddr, UINT32 dstStartLogAddr, UINT32 writeSize ) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = BlockDram2Card( srcRamAddr, dstStartLogAddr, writeSize );
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_BlockDram2Card( srcRamAddr, dstStartLogAddr, writeSize );
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_BlockDram2Card( srcRamAddr, dstStartLogAddr, writeSize );
		break;
		#endif

          //ada@0411 test mmc
		#if ( EMMC_OPTION == 1 )
		case K_MEDIA_EMMC:
			sts = MMC_BlockDram2Card( srcRamAddr, dstStartLogAddr, writeSize );
		break;
		#endif

		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRM_BlockDram2Card( srcRamAddr, dstStartLogAddr, writeSize );
		break;
		#endif
		default:
		break;
	}

	return sts;
}

UINT8 Card_BlockCard2Dram( UINT32 dstRamAddr, UINT32 dstStartLogAddr, UINT32 readSize ) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = BlockCard2Dram( dstRamAddr, dstStartLogAddr, readSize );
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_BlockCard2Dram( dstRamAddr, dstStartLogAddr, readSize );
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_BlockCard2Dram( dstRamAddr, dstStartLogAddr, readSize );
		break;
		#endif

          //ada@0411 test mmc
		#if ( EMMC_OPTION == 1 )
		case K_MEDIA_EMMC:
			sts = MMC_BlockCard2Dram( dstRamAddr, dstStartLogAddr, readSize );
		break;
		#endif

		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRM_BlockCard2Dram( dstRamAddr, dstStartLogAddr, readSize );
		break;
		#endif
		default:
		break;
	}

	return sts;
}

UINT8 Card_File_Flush() USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = File_Flush();
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = CF_File_Flush();
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = SD_File_Flush();
		break;
		#endif

          //ada@0411 test mmc
		#if ( EMMC_OPTION == 1 )
		case K_MEDIA_EMMC:
			sts = MMC_File_Flush();
		break;
		#endif

		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:
			sts = DRM_File_Flush();
		break;
		#endif
		default:
		break;
	}

	return sts;
}
#endif

#if 0 //xian -- because vfsFileWrite(),vfsFileRead() are same 
#if 1//(CODE_IS_MULTIBANK)

// @patch5.1@wyeo@solve avi sync problem on sd, mmc@begin
UINT8 File_LWriteSD(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage);
// @patch5.1@wyeo@solve avi sync problem on sd, mmc@end

UINT8 Card_File_Write(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage) USING_0
{
	UINT8 sts = FALSE;
	//printf("==Card_File_Write==\n");
	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = vfsFileWrite( fHandle, srcPath, writeSize, ramAddr, findHeaderPage );
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF: // SD_File_Write
			// @patch5.1@wyeo@solve avi sync problem on sd, mmc@begin
			sts = File_LWriteSD( fHandle, srcPath, writeSize, ramAddr, findHeaderPage );
			// @patch5.1@wyeo@solve avi sync problem on sd, mmc@end
		break;
		#endif

		//patch4.2@richie@ds0507 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:// DRM_File_Write
			sts = File_LWrite( fHandle, srcPath, writeSize, ramAddr, findHeaderPage );
		break;
		#endif
		//patch4.2@richie@ds0507 end

		default:
		break;
	}

	return sts;
}
#endif

#if 1//(K_HAVE_HAL && CODE_IS_MULTIBANK )
UINT8 Card_File_Read(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos) USING_0
{
	UINT8 sts = FALSE;

	switch ( G_Card_Type )
	{
		#if ( SMC_OPTION == 1 || NANDF_OPTION == 1 )
		case K_MEDIA_SMC:
		#if ( K_MEDIA_NANDF != K_MEDIA_SMC )
		case K_MEDIA_NANDF:
		#endif
			sts = vfsFileRead( fHandle, dstPath, readSize, ramAddr, fpos );
		break;
		#endif
		#if ( CFC_OPTION == 1 )
		case K_MEDIA_CFC:
			sts = File_LRead( fHandle, dstPath, readSize, ramAddr, fpos );
		break;
		#endif
		#if ( SDF_OPTION == 1 )
		case K_MEDIA_SDF:
			sts = File_LRead( fHandle, dstPath, readSize, ramAddr, fpos );
		break;
		#endif

          //ada@0411 test mmc
		#if ( EMMC_OPTION == 1 )
		case K_MEDIA_EMMC: // MMC_File_Write
			sts = File_LRead( fHandle, dstPath, readSize, ramAddr, fpos );
		break;
		#endif

		//patch4.2@richie@ds0507 begin
		#if ( DOSRAM_OPTION == 1 )
		case K_MEDIA_DOSRAM:// DRM_File_Read
			sts = File_LRead( fHandle, dstPath, readSize, ramAddr, fpos );
		break;
		#endif
		//patch4.2@richie@ds0507 end

		default:
		break;
	}

	return sts;
}
#endif
#endif

#endif // K_MEDIA_ONLYONE


