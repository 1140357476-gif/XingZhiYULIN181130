/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		sdcore.c

Abstract:

		Module uniquely	belong to cf card access and charateristic

Environment:

		Keil C51 Compiler

Revision History:

		02/6/2002	   Leeada	 created

--*/
//=============================================================================
//Header file
//=============================================================================

#include "general.h"
#if	1//(SD)

#include "asicreg.h"
#include "hal.h"
#include "impt.h"	// WWWW
//#include "cardebug.h"

#include "doslink.h"
#include "dosvar.h"

#include "cardvar.h"
//#include "cardcon.h"

//#include "sdcon.h"
#include "sdfunc.h"
//#include "sdvar.h"
#include "sd_def.h"
#include "hal_supp.h"

#include "dos32.h"

//extern UINT8 RspBuf[];
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

//-----------------------------------------------------------------------------
//SD_Initialize
//-----------------------------------------------------------------------------
UINT8 SD_Initialize(UINT8 doDOSInitial)
{
	UINT8 sts =	FALSE;

	G_DOS_Initial_Status = 1;
	SD_GetBehavior();

	//printf("sd_initialize...\n");
	if (SD_CardDetect(G_Card_Type))
	{
		//printf("%s %d, Card detect success..\n", __FILE__, (UINT16)__LINE__);
		SETMODE_ClearPostBuffer();	// WWWW
		SETMODE_SetUploadMode();	// WWWW
		if ( doDOSInitial )
		{
			if (DOS_Initialize())
			{
					sts	= TRUE;
					G_DOS_Initial_Status = 0xFF;
					//patch	4.4-7@wyeo@remove debug	message
					//printf("SD_Initialize: OK\n");
			}
			else{
				//printf("%s(%d), Dos_ini fail...\n", __FILE__, (UINT16)__LINE__);
			}
			// @patch5.1@wyeo@fix don't	correctly set initial status@begin
			#if	0
			else
					G_DOS_Initial_Status = 5;
			#endif
			// @patch5.1@wyeo@fix don't	correctly set initial status@end
			}
		else
				sts	= TRUE;
		SETMODE_SetCameraIdle();	// WWWW
	}
		else
			G_DOS_Initial_Status = 3;

	 return	sts;
}

//-----------------------------------------------------------------------------
//SD_GetBehavior
//-----------------------------------------------------------------------------
UINT16 SD_GetBehavior(void)
{
	G_Media_Behavior = K_MEDIA_SD_Behavior;
	return G_Media_Behavior;
}

//-----------------------------------------------------------------------------
//SD_CardDetect
//-----------------------------------------------------------------------------
UINT8 SD_CardDetect(UINT8 cardType)
{
	UINT8 detected	= 1;

	//HAL_CardDetect(&detected);

	//printf("carddetect1=%bx\n",detected);

	if	(detected == 1)
	{
		//patch	4.4-7@wyeo@remove debug	message
		M_Card_Printf("SD_CardDetect:	SD detected\n");
		G_Card_BlockSize = 16384L;

		//patch	4.4-7@wyeo@remove debug	message
		M_Card_Printf("SD_CardDetect:	To get SD card ID information ...\n");
		if (SD_HwInitialize(cardType))
		{
			G_Card_BytePerSector	= 512;
			if( G_Card_TotalCardSize && (G_Card_SectorPerCard == 0)){
				G_Card_SectorPerCard	= G_Card_TotalCardSize / G_Card_BytePerSector;
				//patch 4.4-7@wyeo@remove debug message
	
				#if 0
				printf("G_Card_SectorPerCard	= %lx\n",G_Card_SectorPerCard);
				printf("G_Card_BytePerSector	= %x\n",G_Card_BytePerSector);
				#endif
				// wyeo@0509, start
				if (G_Card_SectorPerCard < 2048000L)		// 1G
						G_Card_BlockSize = 16384L;	// 16K
				else if	(G_Card_SectorPerCard <	4096000L)	// 2G
					G_Card_BlockSize = 32768L;		// 32K
				else
					G_Card_BlockSize = 65536;		// 64K
				// wyeo@0509, end
	
			   	G_Card_TotalSizeMB = G_Card_SectorPerCard >> 11;
		   //patch 4.4-7@wyeo@remove debug message
		   	//printf("G_Card_TotalSizeMB = %d\n",G_Card_TotalSizeMB);
			}
			else{
			   	G_Card_TotalSizeMB = G_Card_SectorPerCard >> 11;
			}

		   	G_Card_Supported	|= (((UINT16)1) << K_SDF_SUPPORTED_BIT);

		   	//patch 4.4-7@wyeo@remove debug message
		   	M_Card_Printf("SD_CardDetect: SD	H/W	initialization success ...\n");
		}
		else
		{
		   detected	= 0;
			//patch	4.4-7@wyeo@remove debug	message
		   M_Card_Printf("SD_CardDetect: SD	H/W	initialization failed ...\n");
		}
	}
	else
	{
		detected = 0;
		//patch 4.4-7@wyeo@remove	debug message
		M_Card_Printf("SD_CardDetect:	SD not detected\n");
	}

	return detected;
}


#endif

