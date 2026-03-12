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
 * @file      vndproc.c
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "icat_stub.h"
#include "initio.h"
#include "cardopt.h"
#include "sp1k_util_api.h"
#include "sp1k_ae_api.h"
#include "awb_api.h"//wangjg++
#include "timer.h"
#include "vndreq.h"

//#include "option.h"
#include "usbopt.h"
#include "uiflow.h"
#include "gpio.h"
#include "bulkout.h"
#include "ctlsetup.h"
#include "usbmsdc2.h"
#include "doslink.h"
#include "hal.h"
//#include "sidcmain.h"
#include "hal_global.h"
#include "sidc_api.h"
#include "dps_api.h"
#include "sp1k_gpio_api.h"
#include "usbpc.h"
#include "sp1k_fd_api.h"
#include "sp1k_calibration_api.h"
#include "class.h"
#include "ae_api.h"
#include "hal_cdsp_iq.h"
#include "hal_usb.h"
#include "reg_def.h"
#include "hal_audio.h"
#include "usb_uitool.h"
#include "dbg_mem.h"
#include "sp1k_snap_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
//lock fields of USB device request
xdata	UINT8	G_bRequestLock;
xdata	UINT8	G_bRequestLog;
xdata	UINT16	G_wIndexLock;
xdata	UINT16	G_wValueLock;
//#ifdef ICAT_OPTION
xdata	UINT16	G_wLengthLock; //xian ++
//#endif
xdata UINT8 G_PCCAM;
xdata UINT8 G_PowerFreq;
xdata UINT8 G_L1_ModuleID;
xdata UINT8 G_HAL_ModuleID;
xdata UINT8 G_FuncID;
xdata UINT8 G_HAL_FuncID;
xdata UINT32 G_ArgBuf[16];
xdata UINT8 G_ucState;
xdata UINT8 G_ucState2;	//cch@10/15
xdata UINT8 G_ucState3;	//cch@10/15
xdata UINT8 G_usbState;
xdata UINT32 G_ulErrorCode;
xdata UINT8 G_Card_Type;

xdata	UINT32	G_BulkSize;
xdata	UINT32	G_BulkDRAMAddr;
xdata	UINT32	G_BulkFlashType;
xdata	UINT32	G_BulkSRAMAddr;
xdata	UINT32	G_LastBulkSize;

xdata 	UINT8	G_UsbPccamStart;
xdata 	UINT8	G_UsbPccamStop;


#if STILL_SNAP_ON
xdata	UINT8	G_SnapFlag = 0;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdata UINT8 G_Image_Type;
extern xdata UINT8 PreGain;
extern xdata UINT32 PreShutter;
extern xdata UINT8 G_UI_USBConnect;
extern xdata UINT8 G_ucMSDC_MLUN;
extern xdata UINT8 G_ucStorData[];
extern xdata UINT8 G_USBSpeedStartCheck;
extern xdata UINT32 G_USBCheckNewConfigStartTime;
extern xdata UINT8 G_USBNewConfigureOK;
extern xdata UINT8 G_USBAudSetInterface;
extern xdata UINT8 G_USBAudioSetInterface;

extern xdata UINT8 G_ucPccamTuningFlag;
extern xdata UINT8 G_ae_ON;
extern xdata UINT8 G_ucCtrlData[];
extern xdata UINT8 G_PCCAME_FD_ON;
extern xdata UINT32	G_timerCheckTestUnit;
extern xdata UINT8 G_usbPvFrameCount;
extern xdata UINT8 G_FormatIndex;

extern UINT8 *G_pucCtrlDataPointer;

extern void usbPvSizeSet();

#if STILL_SNAP_ON
extern  xdata	UINT16	G_usPostJpegWidth;
extern  xdata	UINT16	G_usPostJpegHeight;
extern  xdata UINT8 G_ucStillImageTriggerFlag;
void SnapStlllImage(void);
extern void SetCameraSize(UINT8 type);
#endif

extern xdata UINT8	gAeTgtLvl;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void MAIN_PreDpc(void);
void MAIN_PostDpc(void);
void MAIN_EventPreDpc(void);
void MAIN_EventPostDpc(void);
UINT8 USBMSDC_ReadWriteData(void);
UINT8 USBMSDC_SendStatus(void);
void USBUVC_ProcessUnit(void);
UINT8 sysTimeSync(MYSYSTIME* pDate);

/**
 * @brief     brief
 * @param     [in] Param A
 * @param     [out] Param B
 * @retval    return = SUCCESS / FAIL.
 * @see       function X
 * @author    Author's name
 * @since     2010-01-01
 * @todo      N/A
 * @bug       N/A
*/

#if 1
void mainTestAp(void)
{
	UINT8 sts = sts;
	//printf("mainTestAp:%bx\n",G_bRequestLock);
	switch (G_bRequestLock) {
	//---------------------------------------------------
	//0x0X - Register Read/Write without Deferred Procedure	Call
	//---------------------------------------------------

	//---------------------------------------------------
	//0x1X - ISP Related Function
	//---------------------------------------------------
	case 0x10:
			//-------------------------------------------
			//0x10 - update	flash ROM code through ISP
			//-------------------------------------------
			MAIN_EventPreDpc();
			MAIN_EventPostDpc();
			break;

	case 0x11:
			//-------------------------------------------
			//0x11 - HAL	Function Test
			//-------------------------------------------
			MAIN_EventPreDpc();
			MAIN_EventPostDpc();
			break;

	//---------------------------------------------------
	// 0x2X	- Miscellaneous	Function without Deferred Procedure	Call
	//---------------------------------------------------

	//---------------------------------------------------
	// 0x3X	- Miscellaneous	Function with Deferred Procedure Call
	//---------------------------------------------------
	case 0x30:
			//-------------------------------------------
			//0x30 - Software Reset
			//-------------------------------------------
			MAIN_EventPreDpc();
			INITIO_SoftwareReset();
			MAIN_EventPostDpc();
			break;

	case 0x31:	   //cch@10/18
			//-------------------------------------------
			//0x31 - Set Idle /	PC Camera Mode
			//		G_wIndexLock = 0 - Idle	mode
			//					   4 - PC Camera
			//-------------------------------------------
			MAIN_EventPreDpc();

			if (G_wIndexLock == 4){
				if (G_PCCAM == 0){
					G_PCCAM = 1;

					/* Get pccam width */
					if (G_Image_Type != 0){
						printf("p1\n");
						sp1kUsbPccamStart();
						TIMER0_Start();
 					}
				}
			}
			else {
						printf("p0\n");
				G_PCCAM = 0;
				sp1kUsbPccamStop();
				TIMER0_Start();
			}


			MAIN_EventPostDpc();
			break;

	case 0x32:
			#if 0
			//-------------------------------------------
			//0x32 - Change	Camera Mode
			//		G_wIndexLock = 0 - Idle
			//					   1 - Preview
			//					   2 - Digital Still Camera
			//					   3 - Video Clip
			//					   4 - PC Camera
			//					   5 - Playback
			//					   6 - Upload/Download
			//-------------------------------------------
			MAIN_EventPreDpc();
			SETMODE_ChangeCameraMode((UINT8)G_wIndexLock);
			MAIN_EventPostDpc();
			#endif
			break;

	case 0x33:	   //cch@10/15
			//-------------------------------------------
			//0x33 - Set Initial Sensor
			//		G_wIndexLock = 0 - Initial Global
			//					   1 - Initial CDSP
			//					   2 - Initial TG
			//-------------------------------------------
			MAIN_EventPreDpc();
			//NotYet!
			MAIN_EventPostDpc();
			break;

	case 0x34:	   //cch@10/15
			//-------------------------------------------
			//0x34 - Set GPIO
			//		G_wIndexLock = 0 - Turn	on GPIP	power
			//					   1 - Turn	off	GPIP power
			//-------------------------------------------
			MAIN_EventPreDpc();
			//NotYet!
			MAIN_EventPostDpc();
			break;

	//---------------------------------------------------
	// 0x4X	- SDRAM	Related	Function
	//---------------------------------------------------
#if	(SDRAM_OPTION == 1)
	case 0x40:
			//-------------------------------------------
			//0x40 - Format
			//-------------------------------------------
			MAIN_EventPreDpc();
			SDRAM_Format();
			MAIN_EventPostDpc();
			break;

	case 0x41:
			//-------------------------------------------
			//0x41 - Capture Object
			//		G_wIndexLock = 0 - Capture One Image (JPEG)
			//					   1 - Capture Audio Stream	(WAV)
			//					   2 - Capture Video Stream	(AVI)
			//					   3 - Capture Audio/Video Stream (AVI)
			//-------------------------------------------
			MAIN_EventPreDpc();
			SENSOR_CaptureAeAwbAf();
			SDRAM_CaptureObject(G_wIndexLock);
			MAIN_EventPostDpc();
			break;

	case 0x42:
			//-------------------------------------------
			//0x42 - Delete	Object
			//		G_wValueLock = 0 - Delete All Object
			//					   otherwise - Delete Object of	Index
			//-------------------------------------------
			MAIN_EventPreDpc();
			SDRAM_DeleteObject(G_wValueLock);
			MAIN_EventPostDpc();
			break;

	case 0x43:
			//-------------------------------------------
			//0x43 - Playback Object
			//		G_wIndexLock = 0 - Playback	Nine Thumbnail
			//					   1 - Playback	Four Thumbnail
			//					   2 - Playback	One	Image
			//					   3 - Playback	Image Slide
			//					   4 - Playback	Video
			//		G_wValueLock = Index of	Object
			//-------------------------------------------
			MAIN_EventPreDpc();
			SDRAM_PlaybackObject(G_wIndexLock, G_wValueLock);
			MAIN_EventPostDpc();
			break;

	case 0x44:
			//-------------------------------------------
			//0x44 - Upload	Object
			//		G_wIndexLock = 0 - Get Object Count
			//					   1 - Get Object FDBs
			//					   2 - Upload One Object
			//		G_wValueLock = Index of	Object
			//-------------------------------------------
			MAIN_EventPreDpc();
			if (G_wIndexLock)
					SDRAM_UploadObject(G_wIndexLock, G_wValueLock);
			MAIN_EventPostDpc();
			break;
#endif

#if 0
	//---------------------------------------------------
	// 0x5X	- General Storage Related Function
	//---------------------------------------------------
	case 0x50:
			//-------------------------------------------
			//0x50 - Format
			//-------------------------------------------
			MAIN_EventPreDpc();
			// WWW3	start
// G_wIndexLock, Bit 0,	 1 is physical format, 0 is	logical	format.	It is suggested
//						 that always select	physical format	for	nand and smc card
//						 for safty unless customer request faster format time.
//				 Bit 7,	 1 is create DCIM and 100Media directory, no dir created.
//						 When this is on, it is	no need	to call	M_card_Initialize
//						 again for all M_card_Initialize action	have been included
//						 in	format action.
//				Bit	6,	 1 is select optimized format, 0 is	what card original
//						 Optimized means bigger	cluster	size or	cluster	size equals
//						 block size	of smc and nand	flash.
//
// So 0xC0 is fastest logical format, 0x01 is slowest physical format.
			sts	= diskFormat(0xc1);
			if (sts)
			{
				//DbgPrint("diskFormat:	OK\n");
				sts	= diskInitialize(sts);
				///if (sts)
				///	DbgPrint("diskInitialize: OK\n");
				///else
				///	DbgPrint("diskInitialize: Fail\n");
			}
			else
			{
			///DbgPrint("diskFormat: Fail\n");
				if (G_Card_Type	!= K_MEDIA_BUILT_IN)
				{
				// if external card	format fail, change	back to	internal
				// flash
				G_Card_Type	= K_MEDIA_BUILT_IN;
				G_ucStorageType	= K_MEDIA_BUILT_IN;
				sts	= diskInitialize(TRUE);
				///if (sts)
				///	DbgPrint("diskInitialize for built in flash: OK\n");
				///else
				///	DbgPrint("diskInitialize for built in flash: Fail\n");
				}
			}
			// WWW3	end

			MAIN_EventPostDpc();
			break;

#endif
#ifdef DPF_SCANNER
	case 0x51:
			//-------------------------------------------
			//0x51 - Capture Object
			//		G_wIndexLock = 0 - Capture One Image (JPEG)
			//					   1 - Capture Audio Stream	(WAV)
			//					   2 - Capture Video Stream	(AVI)
			//					   3 - Capture Audio/Video Stream (AVI)
			//-------------------------------------------
			MAIN_EventPreDpc();
			//printf("Vnd 0x51 %x\n",G_wIndexLock);
			if( G_wIndexLock == 0 ){
				RemainPictureNO++;
				//snapMainVlcSize = 0;
	 			//snapHdrSize = 0;
				sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_JPG_OPT, 0 ); /*don not store*/
				osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
				osMsgPost(SP1K_MSG_KEY_PRESS_S2);
			}
			else if(  G_wIndexLock == 1  ){
				HAL_JpegMjpgAbort();
				HAL_JpgZFactorSet(100, 100, 640, 480, 640, 480, 0);
				{  	UINT8 Qindex = (UINT8)( G_wValueLock >> 8 );
					if( Qindex != 0 ){
						jpegQTableSet( Qindex, NULL, NULL, 1 );
					}
					else{
						jpegQTableSet(85, NULL, NULL, 1 );
					}
				}
				HAL_JpegMjpgTrig(K_SDRAM_AviCapVLCBufAddrA+ 130, K_SDRAM_AviCapVLCBufAddrB+ 130, G_wValueLock & 0x00ff);
				osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
				XBYTE[REG_Dram_VlcRdy] = 0x03;//clear vlc
			}
			MAIN_EventPostDpc();
			break;

#endif

#if 0
	case 0x52:
			//-------------------------------------------
			//0x52 - Delete	Object
			//		G_wValueLock = 0 - Delete All Object
			//					   otherwise - Delete Object of	Index
			//-------------------------------------------
			MAIN_EventPreDpc();
			// WWW3	start
			sts	= STORAGE_DeleteObject(G_wValueLock);
			if (G_wValueLock ==	0)
			{
				/*
				if (sts	== TRUE)
					DbgPrint("STORAGE_DeleteObject All:	OK\n");
				else
					DbgPrint("STORAGE_DeleteObject All:	Fail\n");
				*/
			}
			else
			{
				/*
				if (sts	== TRUE)
					DbgPrint("STORAGE_DeleteObject:	OK\n");
				else
					DbgPrint("STORAGE_DeleteObject:	Fail\n");
				*/
			}
			// WWW3	end
			MAIN_EventPostDpc();
			break;

	case 0x53:
			//-------------------------------------------
			//0x53 - Playback Object
			//		G_wIndexLock = 0 - Playback	Nine Thumbnail
			//					   1 - Playback	Four Thumbnail
			//					   2 - Playback	One	Image
			//					   3 - Playback	Image Slide
			//					   4 - Playback	Video
			//		G_wValueLock = Index of	Object
			//-------------------------------------------
			MAIN_EventPreDpc();
			STORAGE_PlaybackObject(G_wIndexLock, G_wValueLock);
			MAIN_EventPostDpc();
			break;

	case 0x54:
			//-------------------------------------------
			//0x54 - Upload	Object
			//		G_wIndexLock = 0 - Get Object Count
			//					   1 - Get Object FDBs
			//					   2 - Upload One Object
			//		G_wValueLock = Index of	Object
			//-------------------------------------------
			MAIN_EventPreDpc();
			if (G_wIndexLock)
					STORAGE_UploadObject(G_wIndexLock, G_wValueLock);
			MAIN_EventPostDpc();
			break;

	//patch4.3@richie@md0603 begin
	case 0x55:
			//-------------------------------------------
			//0x55 - Mulitple Directory
			//		G_wIndexLock = 0 - Get Dir Count
			//					   1 - Get Dir Data
			//					   2 - Get Now Dir Index
			//			   3 - Set New Dir Index
			//		G_wValueLock
			//-------------------------------------------
			MAIN_EventPreDpc();
			#if	(SINGLE_DIRECTORY == 0)
			if (G_wIndexLock ==	1)
				STORAGE_GetDirData();
			else if	(G_wIndexLock == 3)
				USR_ChangeCurrentDir(G_wValueLock);
			#endif
			MAIN_EventPostDpc();
			break;
	//patch4.3@richie@md0603 end

	// WWW3	start
	case 0x5A: //  switch card,	TEST
			MAIN_EventPreDpc();
			G_Card_Type	= G_wIndexLock;
			G_ucStorageType	= 0xff;
			sts	= diskDetect();

			//patch4.4@richie@dsi begin
			//restore G_DOS_SDRAM_FAT1Addr
			#if	( DOSRAM_OPTION	== 1 )
			if (G_Card_Type	== K_MEDIA_DOSRAM)
			{
				if (G_ulDRAMSTORFAT1AddrTemp !=	0xFFFFFFFF)
				{
					G_DOS_SDRAM_FAT1Addr = G_ulDRAMSTORFAT1AddrTemp;
				}
			}
			#endif
			//patch4.4@richie@dsi end

			if (sts)
			{
				sts	= diskInitialize(TRUE);
				if (sts	== FALSE)
				{
						sts	= diskFormat(0xc1);
						if (sts)
								sts	= diskInitialize(sts);
				}
			}
			MAIN_EventPostDpc();
			break;
		// WWW3	end

	// wyeo@0521, patch	4.2
	#if	(NANDF_OPTION == 1 && CARD_OPTION == 1)
	#if	K_TEST_RESERVE_BLOCK
	// TEST	reserve	block
	case 0x5B:
			MAIN_EventPreDpc();
			sts	= TestReserveBlock();

			MAIN_EventPostDpc();
			break;
	#endif
	#endif
	#endif
	//patch5.1@Add copy to card function begin
	#if 1//(COPY2CARD_OPTION == 1)
	#if 0
		case 0x5C:
            //---------------------------------------------------------------
            //0x5c - Copy Disk
            //      G_wIndexLock = 0 - Copy all directories
            //                   = others - Copy G_wIndexLock directory
			//		if (G_wIndexLock != 0)
			//		{
            //      	G_wValueLock = 0 - Copy all files in G_wIndexLock directory
			//					 	 = Others - copy G_wValueLock file.
			//		}
            //-----------------------------------------------------------------
	    	MAIN_EventPreDpc();//patch 4.3.2@hlc@CopyDisk

			//The minimum buffer size for coping to SMC card should be at least
			//       	Temp Buffer Size(bytes) = G_DOS_FatSize + K_DOS_SectorSize+G_DOS_SDRAM_WorkSize*2 + 64K
			// The minimum buffer size for other card should be at least
			// 			Temp Buffer Size(Bytes) = G_DOS_FatSize + K_DOS_SectorSize + 64K
			if (G_wIndexLock == 0)
			{
				//patch5.2.1@ada@Bi-direction storage copy begin
				//USR_CopyAllDir(K_SDRAM_PvBufAddrA);
				USR_CopyAllDir(K_SDRAM_ImageCaptureBufAddr, K_Copy_Nand2Card);
				//USR_CopyAllDir(K_SDRAM_PvBufAddrA, K_Copy_Nand2Card);
				//patch5.2.1@ada@Bi-direction storage copy end
			}
			else if (G_wValueLock == 0)
			{
				//patch5.2.1@ada@Bi-direction storage copy begin
				//USR_Copy1Dir(K_SDRAM_PvBufAddrA, G_wIndexLock);
				USR_Copy1Dir(K_SDRAM_ImageCaptureBufAddr, G_wIndexLock,K_Copy_Nand2Card);
				//USR_Copy1Dir(K_SDRAM_PvBufAddrA, G_wIndexLock,K_Copy_Nand2Card);
				//patch5.2.1@ada@Bi-direction storage copy end
			}
			else
			{
				//patch5.2.1@ada@Bi-direction storage copy begin
				//USR_Copy1File(K_SDRAM_PvBufAddrA, G_wValueLock);
				USR_Copy1File(K_SDRAM_ImageCaptureBufAddr, G_wValueLock,K_Copy_Nand2Card);
				//USR_Copy1File(K_SDRAM_PvBufAddrA, G_wValueLock,K_Copy_Nand2Card);
				//patch5.2.1@ada@Bi-direction storage copy end
			}

			MAIN_EventPostDpc();
			break;

	//patch5.2.1@ada@Bi-direction storage copy begin
		case 0x5d:
            //---------------------------------------------------------------
            //0x5d - Copy Disk
            //      G_wIndexLock = 0 - Copy all directories
            //                   = others - Copy G_wIndexLock directory
			//		if (G_wIndexLock != 0)
			//		{
            //      	G_wValueLock = 0 - Copy all files in G_wIndexLock directory
			//					 	 = Others - copy G_wValueLock file.
			//		}
            //-----------------------------------------------------------------
	    	MAIN_EventPreDpc();//patch 4.3.2@hlc@CopyDisk

			//The minimum buffer size for coping to SMC card should be at least
			//       	Temp Buffer Size(bytes) = G_DOS_FatSize + K_DOS_SectorSize+G_DOS_SDRAM_WorkSize*2 + 64K
			// The minimum buffer size for other card should be at least
			// 			Temp Buffer Size(Bytes) = G_DOS_FatSize + K_DOS_SectorSize + 64K
			if (G_wIndexLock == 0)
			{
				USR_CopyAllDir(K_SDRAM_ImageCaptureBufAddr, K_Copy_Card2Nand);
			}
			else if (G_wValueLock == 0)
			{
				USR_Copy1Dir(K_SDRAM_ImageCaptureBufAddr, G_wIndexLock,K_Copy_Card2Nand);
			}
			else
			{
				USR_Copy1File(K_SDRAM_ImageCaptureBufAddr, G_wValueLock,K_Copy_Card2Nand);
			}

			MAIN_EventPostDpc();
			break;
		#endif
	#endif
	//patch5.2.1@ada@Bi-direction storage copy end

	//patch4.5@richie@partial file upload begin
	case 0x60:
		#if 0
			//-------------------------------------------
			//0x60 - Upload	Patrial	Files
			//-------------------------------------------
			MAIN_EventPreDpc();
			//printf("MAIN Partial File\n");
			//printf("MAIN arg1 =	%lx,arg2 = %lx,arg3	= %lx\n",G_ArgBuf[0],G_ArgBuf[1],G_ArgBuf[2]);
			STORAGE_UploadObject(0x0004, G_ArgBuf[2]);
			MAIN_EventPostDpc();
		#endif
			break;
	//patch4.5@richie@partial file upload end


	//patch4.5@richie@checksum isp begin
	#if	0//(ISPCHECKSUM_OPTION)
	case 0x66:

			//-------------------------------------------
			//0x66 - CheckSum input
			//		G_wValueLock = CheckSum
			//-------------------------------------------
			MAIN_EventPreDpc();
			//printf("CheckSum=%x\n",G_wValueLock);
			G_usISPCheckSum	= G_wValueLock;
			//printf("G_usISPCheckSum=%x\n",G_usISPCheckSum);

			MAIN_EventPostDpc();

			break;
	#endif
	//patch4.5@richie@checksum isp end



	#if	0//(FPGA_TEST)
	/***********************************************
	 *				FPGA UNITEST 				   *
	 *	0x70~0x7F : CPU/GPRM/USB/DRAM 	module	   *
	 *	0x80~0x8F : CDSP/FRONT	 	module		   *
	 *	0x90~0x9F : STORAGE/DISPLAY	module		   *
	 *	0xA0~0xAF : AUDIO/JPEG/MJPEG module		   *
	 *	0xB0~0xBF : Reserved					   *
	 ***********************************************/
	#if (UNITEST_CPU_DMA)
	case 0x70:
			/********************************************************
			 *		0x70 - CPU_DMA Test function						*
			 *			G_wIndexLock = 0x00	==>	IRQ test			*
			 *			G_wIndexLock = 0x01	==>	DMA test			*
			 *			G_wIndexLock = 0x02	==>	Shadow program test	*
	 		 ********************************************************/

			MAIN_EventPreDpc();

			#if (UNITEST_CPU_DMA)
			UNI_CPU_DMATest(G_wIndexLock,G_wValueLock);
			#endif

			MAIN_EventPostDpc();

			break;
	#endif

	#if (UNITEST_GPRM)
	case 0x71:
/********************************************************
					 *		0x71 - GPRM Test function						*
					 *			G_wIndexLock = 0x00	==>	GPRM Copy test		*
			 *			G_wIndexLock = 0x01	==>	GPRM Scale test		*
			 *                  G_wIndexLock = 0x02	==>	GPRM Rotation test		*
			 *			G_wIndexLock = 0x03	==>	GPRM Flip test		*
			 *			G_wIndexLock = 0x04	==>	GPRM Bad pixel test		*
			 		 ********************************************************/

			MAIN_EventPreDpc();

			#if (UNITEST_GPRM)
			UNI_GPRMTest(G_wIndexLock,G_wValueLock);
			#endif

			MAIN_EventPostDpc();

			break;
	#endif

	#if (UNITEST_DRAM)
		case 0x72:
			/****************************************************************
			 *		0x72 - DRAM Test function								*
			 *			G_wIndexLock = 0x00	==>	Failure Bank reject test	*
			 *			G_wIndexLock = 0x01	==>	Self-refresh Suspend Resume	*
	 		 ****************************************************************/

			MAIN_EventPreDpc();

			#if (UNITEST_DRAM)
			UNI_DRAMTest(G_wIndexLock,G_wValueLock);
			#endif

			MAIN_EventPostDpc();

			break;
	#endif

#if(UNITEST_GLOBAL)
	case 0x73:
		/********************************************************
		* 		0x73 -global Test function
		*			G_wIndexLock ==0x00 ==> gpio interrupt test
		*********************************************************/
		MAIN_EventPreDpc();
		UNI_GlobalTest(G_wIndexLock, G_wValueLock);
		MAIN_EventPostDpc();
		break;
#endif
#if (UNITEST_CDSP)
	case 0x80:
		break;
#endif

	case 0x90:
			/********************************************************
			 *		0x71 - GPRM Test function						*
			 *			G_wIndexLock = 0x01	==>	NAND		        *
			 *			G_wIndexLock = 0x02	==>	SD	                *
	 		 ********************************************************/
			MAIN_EventPreDpc();
			UNI_StorageTest(G_wIndexLock);
			MAIN_EventPostDpc();
			break;

	#endif



	/***********************************************
	 *				PCCAM Property API
	 *	0xC0 : Brightness tunning API
	 *	0xC1 : Contrast tunning API
	 *	0xC2 : Hue tunning API
	 *	0xC3 : Staturation tunning API
	 *	0xC4 : Sharpness tunning API
	 *	0xC5 : 50/60 Hz selection
	 ***********************************************/
#if 0
	case 0xc0:	/* Brightness */
		MAIN_EventPreDpc();
		printf("br\n");
		MAIN_EventPostDpc();
		break;
	case 0xc1:	/* Contrast */
		MAIN_EventPreDpc();
		printf("Ct\n");
		MAIN_EventPostDpc();
		break;
	case 0xc2:	/* Hue */
		MAIN_EventPreDpc();
		printf("Hu\n");
		MAIN_EventPostDpc();
		break;
	case 0xc3:	/* Staturation */
		MAIN_EventPreDpc();
		printf("Sa\n");
		MAIN_EventPostDpc();
		break;
	case 0xc4:	/* Sharpness */
		MAIN_EventPreDpc();
		printf("Sh\n");
		MAIN_EventPostDpc();
		break;

	case 0xc5:	/* Sharpness */
		MAIN_EventPreDpc();
		printf("Hz\n");
		if (G_wValueLock == 0x02){
			aeFrameRateChangeSet(SP1K_FRAME_RATE_30);
			G_PowerFreq = SP1K_FRAME_RATE_30;
			PreGain = 0;
			PreShutter = 0;

		}
		else if (G_wValueLock == 0x03){
			aeFrameRateChangeSet(SP1K_FRAME_RATE_25);
			G_PowerFreq = SP1K_FRAME_RATE_25;
			PreGain = 0;
			PreShutter = 0;
		}
#endif
		MAIN_EventPostDpc();
		break;
	case UITOOL_REQUEST:
		MAIN_EventPreDpc();
		uiToolPrintf("UITool :%x,%bx\n",G_wIndexLock,G_L1_ModuleID);
		switch(G_L1_ModuleID)
		{
		case 0x10://prepare data for usb
			uiToolPrintf("prepare\n");
			uiToolPrepareData();
			break;

		case 0x03://process data from bulk out
			uiToolPrintf("down ok\n");
			uiToolProcessData((UINT8)G_pCtrlCommand->wIndex);
			break;
		case 0x04:
			uiToolPrintf("up ok\n");

			break;
		default:
			break;
		}
		#if 0
		{
			UINT32 addr=0;
			UINT32 size=0;
			UINT8 id = (UINT8)G_pCtrlCommand->wIndex;
			UINT8 ret = sp1kUIToolGet(id,&addr,&size);
			//if(!ret)break;
			dumpData(size, addr, 1);
		}
		#endif
		MAIN_EventPostDpc();
		break;
	case 0xD0:/* RTC Time Set */
		MAIN_EventPreDpc();
		sysTimeSync((MYSYSTIME*)G_pucCtrlDataPointer);
		MAIN_EventPostDpc();
		break;



	case 0xfd:
			//-------------------------------------------
			// L1_Function
			//-------------------------------------------
			MAIN_EventPreDpc();
			switch(G_L1_ModuleID)
			{
					case 0x00:
							//App_DoL1Function_System(G_FuncID, G_ArgBuf);
							break;
					case 0x01:
							//App_DoL1Function_Preview(G_FuncID, G_ArgBuf);
							break;
					case 0x02:
							//App_DoL1Function_Playback(G_FuncID, G_ArgBuf);
							break;
					case 0x03:
							//App_DoL1Function_Audio(G_FuncID,	G_ArgBuf);
							break;
					case 0x04:
							//XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
							//XBYTE[REG_Front_Finten_low] = 0x00;
#if !TAG_TODO
							sp1kUsb_Pc(G_FuncID, G_ArgBuf);
#endif
							break;
					case 0x05:
							//App_DoL1Function_OSD(G_FuncID, G_ArgBuf);
							break;
					case 0x06:
							//App_DoL1Function_FileSystem(G_FuncID, G_ArgBuf);
							break;
					case 0x07:
							//App_DoL1Function_Storage(G_FuncID, G_ArgBuf);
							break;
					case 0x0f:
							//App_DoL1Function_Test(G_FuncID, G_ArgBuf);
							break;
					default:
							break;
			}
			MAIN_EventPostDpc();
			break; // case 0xfd

   case	0xfe:
			//-------------------------------------------
			// HAL_Function
			//-------------------------------------------
			MAIN_EventPreDpc();
/*
			switch(G_HAL_ModuleID) // Module	ID
			{
					case 0x00:
							App_DoHALFunction_Global(G_HAL_FuncID, G_ArgBuf);
							break;
					case 0x01:
							App_DoHALFunction_Front(G_HAL_FuncID,	G_ArgBuf);
							break;
					case 0x02:
							App_DoHALFunction_CDSP(G_HAL_FuncID, G_ArgBuf);
							break;
					case 0x03:
							App_DoHALFunction_SDRAM(G_HAL_FuncID,	G_ArgBuf);
							break;
					case 0x04:
							App_DoHALFunction_DMAC(G_HAL_FuncID, G_ArgBuf);
							break;
					case 0x05:
							App_DoHALFunction_CPU(G_HAL_FuncID, G_ArgBuf);
							break;
					case 0x06:
							App_DoHALFunction_FM(G_HAL_FuncID, G_ArgBuf);
							break;
					case 0x07:
							App_DoHALFunction_Audio(G_HAL_FuncID,	G_ArgBuf);
							break;
					case 0x08:
							App_DoHALFunction_LCDTV(G_HAL_FuncID,	G_ArgBuf);
							break;
					case 0x09:
							App_DoHALFunction_USB(G_HAL_FuncID, G_ArgBuf);
							break;
					case 0x0f:
							App_DoHALFunction_Misc(G_HAL_FuncID, G_ArgBuf);
							break;
					default:
							break;
			}
*/
			MAIN_EventPostDpc();
			break; // case 0xfe

//	#ifdef ICAT_OPTION//xian ++
    #if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
//xian ++
	case ICAT_REQUEST:
            MAIN_EventPreDpc();
		#ifndef AMCAP_OPTION
            XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;   //same with App_DoL1Function_PC
            XBYTE[REG_Front_Finten_low] = 0x00;
		#endif
            icatVndCmdStubDpc( G_wIndexLock, G_wValueLock, G_wLengthLock, (UINT8*)G_ArgBuf );
            MAIN_EventPostDpc();
            break;
//xian --
	#endif

	default:
			break;
	}
}

//-----------------------------------------------------------------------------
//MAIN_EventPreDpc		  cch@10/15
//-----------------------------------------------------------------------------
void MAIN_EventPreDpc(void)	USING_0
/*++

Routine	Description:

		pre-processing before deferred procedure call

Arguments:

		none

Return Value:

		none

--*/
{
		  /*
		  UINT8	temp_key;

		  HAL_ConfigGPIOBit(1,0);
		  HAL_GetGPIOBit(1,&temp_key);

		  //XBYTE[2038]	&= 0xfd;
		  //temp_key = XBYTE[0x2040] & 0x02;

		  if(temp_key >	0)
		  {
			  G_UI_USBConnect =	K_UI_USB_DISCONNECT; //yichang
			   ////printf("USB connect=%bx\n",temp_key);
		  }
		 else
		  {
			   G_UI_USBConnect = K_UI_USB_CONNECT; //yichang
			   ////printf("USB connect=%bx\n",temp_key);

		  }
		  */

		G_ucState &= 0xf0;		//set FW idle
		G_ucState2 |= 0x01;		//set FW Event Ready
		G_ucState |= 0x01;		//set FW ready

		//ada@0415 test	ui
		if ((G_bRequestLock = 0x31) && (G_PCCAM = 0x00)){
			TIMER0_Stop();
		}

/////!!!		DbgPrint("MAIN_EventPreDpc:	Start operation\n");

}
//-----------------------------------------------------------------------------
//MAIN_EventPostDpc
//-----------------------------------------------------------------------------
void MAIN_EventPostDpc(void) USING_0
/*++

Routine	Description:

		post-processing	after deferred procedure call

Arguments:

		none

Return Value:

		none

--*/
{

		G_bRequestLock = 0;		//clear	G_bRequestLock
		G_ucState &= 0xf0 ;		//set FW idle

		//ada@0415 test	ui
	//patch4.2@richie@0528
	if (G_UI_USBConnect	== K_UI_USB_DISCONNECT){
			TIMER0_Start();
	}
/////!!!		DbgPrint("MAIN_EventPostDpc: Stop operation\n");

}

#endif /*mainTestAp*/

UINT8 USBMSDC_ReadWriteData(void) USING_0
{
	sp1kLED_BusySet(1);  /* turn on LED. */

	if (G_ucBulkPhase == K_InDataPhaseRead)
	{
		if (G_ucMSDC_MLUN)
		MLUN_CheckCard();

		TIMER0_Stop();
		XBYTE[REG_Usb_EpAckIntEn] &= K_USB_CLASS_IN_ACK_DISABLE;	//Bulk in 1	acked interrupt	disable
		XBYTE[REG_Usb_EpAckIntEn] &= 0xF8;	//Disable EP0


		//XBYTE[0x2078] &= (~0x40);	//Clear GPIO6 status (for turnkey USB detection)mark for 1628evb
		//XBYTE[0x2058] |= 0x40;		//enable GPIO6 interrupt.
		DOS_ReadMoreSector(K_DOS_DstUsb,G_USBMSDC_ulLogicalBlockAddress,G_USBMSDC_ulSdramBufferAddress,G_ulStorDataLength);
		//XBYTE[0x2058] &= (~0x40);	//Disable GPIO interrupt
		if (G_UI_USBConnect == K_UI_USB_DISCONNECT){
        	G_ucBulkPhase = K_CommandPhase;
			TIMER0_Start();
			return TRUE;
		}
		if (G_USBMSDC_ucCSWStatus == K_USBMSDC_CommandPassed) {
				G_USBMSDC_ucSenseCodeIdx = 0x00;
				G_USBMSDC_ucCSWStatus =	K_USBMSDC_CommandPassed;
		}
		USBMSDC_SendStatus();
		G_ucBulkPhase =	K_InCompletionPhase;
		XBYTE[REG_Usb_TransactionEn] |= K_USB_CLASS_IN_OPEN;//0x01;  //open bulk in to	send status
		XBYTE[REG_Usb_EpAckIntEn] |= K_USB_CLASS_IN_ACK_MASK;  //Bulk in 1 acked interrupt enable
		TIMER0_Start();
	}
	else if	(G_ucBulkPhase == K_OutDataPhaseWrite)
	{
		if (G_ucMSDC_MLUN)
		MLUN_CheckCard();

		TIMER0_Stop();
		XBYTE[REG_Usb_EpAckIntEn] &= K_USB_CLASS_OUT_ACK_DISABLE;	 //Bulk	out	1 acked	interrupt disable

		DOS_WriteMoreSector(K_DOS_DstUsb,G_USBMSDC_ulLogicalBlockAddress,G_USBMSDC_ulSdramBufferAddress,G_ulStorDataLength);
		if (G_UI_USBConnect == K_UI_USB_DISCONNECT){
#if TAG_TODO
			appStorageMount(G_Card_Type);
#endif
        	G_ucBulkPhase = K_CommandPhase;
			TIMER0_Start();
			return TRUE;
		}

		if( G_USBMSDC_ucCSWStatus == K_USBMSDC_CommandPassed) {
				G_USBMSDC_ucSenseCodeIdx = 0x00;
				G_USBMSDC_ucCSWStatus =	K_USBMSDC_CommandPassed;
		}
		USBMSDC_SendStatus();
		G_ucBulkPhase =	K_InCompletionPhase;
		XBYTE[REG_Usb_TransactionEn] |= K_USB_CLASS_IN_OPEN;//0x01;  //open bulk in to	send status
		XBYTE[REG_Usb_EpAckIntEn] |= K_USB_CLASS_OUT_ACK_MASK;	//Bulk out 1 acked interrupt enable
		TIMER0_Start();
	}
	sp1kLED_BusySet(0);  /* turn off LED. */

	return TRUE;
}

UINT8 USBMSDC_SendStatus(void) USING_0
{
		PUSBMSDC_CSW pCSW;
	UINT8 i;

		pCSW = (PUSBMSDC_CSW) G_ucStorData;

		//richie@1129
		pCSW->dCSWSignature	= 0x55534253;
		pCSW->dCSWTag =	G_USBMSDC_pCBW->dCBWTag;
	//???
		pCSW->dCSWDataResidue =	0;//M_ByteSwapOfDword(G_USBMSDC_ulAllocateLength - G_ulStorDataIndex);
		if (G_USBMSDC_ucCSWStatus == K_USBMSDC_CommandFailed)
			pCSW->dCSWDataResidue = G_USBMSDC_pCBW->dCBWDataTransferLength;
		pCSW->bCSWStatus = G_USBMSDC_ucCSWStatus;


		for	(i = 0;	i <	K_USBMSDC_CSW_SIZE;	i++)
				XBYTE[K_USB_CLASS_IN_DATA] = G_ucStorData[i];

		return TRUE;
}
#if(DDX_on)
void USBUVC_ProcessUnit(void)
{
	UINT8 curAeIdx;
	UINT8 temp1, temp3;
	UINT16 temp2;
	switch((G_wValueLock&0xff00)>>8)
	{
		case 0x02: //Brightness
			#if 0
			if( G_ae_ON == 0 )
			{
				sp1kAeSetModeEn(0);
				sp1kAwbDisable();
				G_ae_ON = 2;
			}
			else if( G_ae_ON == 1 )
			{
				sp1kAeSetModeEn(1);
				sp1kAwbEnable();
				G_ae_ON = 2;
			}
			XBYTE[0x21B0] = 0x11;
			XBYTE[0x21B4] = (SINT8)(G_pucCtrlDataPointer[0]-128);
			#else
			XBYTE[0x21B0] = 0x11;
			gAeTgtLvl = G_pucCtrlDataPointer[0];
			#endif

			break;

        case 0x03: //Contrast
        	#if 0
			if( G_ae_ON == 0 )
			{
				sp1kAwbDisable();
				G_ae_ON = 2;
			}
			else if( G_ae_ON == 1 )
			{
				sp1kAwbEnable();
				G_ae_ON = 2;
			}
			#endif
			XBYTE[0x21B0] = 0x11;
			XBYTE[0x21B5] = G_pucCtrlDataPointer[0];
			break;

        case 0x05: //Power line frequency
           // printf("current power %bu",G_ucCtrlData[0]);
			sp1kAeStatusGet(SP1K_AE_gId, &curAeIdx);
			if( G_ucCtrlData[0] == 1 )
				sp1kCalibrationAEConfig(curAeIdx, 50);
			else
				sp1kCalibrationAEConfig(curAeIdx, 60);
			break;

        case 0x06: //HUE
			XBYTE[0x21B0] = 0x11;
			#if 0
			if( G_ae_ON == 0 )
			{
				sp1kAwbDisable();
				G_ae_ON = 2;
			}
			else if( G_ae_ON == 1 )
			{
				sp1kAwbEnable();
				G_ae_ON = 2;
			}
			#endif
			if( G_pucCtrlDataPointer[1] == 255 )
				temp2 = G_pucCtrlDataPointer[0]-75+180;
			else
				temp2 = G_pucCtrlDataPointer[0];
			temp1 = (UINT8)(temp2*2/3);
			if ((temp1 == 0x00) && (temp2 == 0x00))
			{
				XBYTE[0x21B8] = 0x00;
				XBYTE[0x21B9] = 0x40;
				XBYTE[0x21BA] = 0x00;
			}
			else if( temp2 <= 90 ) //Angle is positive
			{
				XBYTE[0x21B8] = temp1+0x80;
				XBYTE[0x21B9] = 0x40-temp1;
				XBYTE[0x21BA] = temp1;
			}
			else if( temp2 <=180 )
			{
				temp3 = temp1-60;
				XBYTE[0x21B8] = (0x40-temp3);
				XBYTE[0x21B9] = temp3;
				XBYTE[0x21BA] = (0x40-temp3)+0x80;
			}
			else if( temp2 <=270 )
			{
				temp3 = (0x40-(temp1-120));
				XBYTE[0x21B8] = 0x40-temp3;
				XBYTE[0x21B9] = temp3+0x80;
				XBYTE[0x21BA] = 0x40-temp3+0x80;
			}
			else
			{
				temp3 = temp1-180;
				XBYTE[0x21B8] = 0x40-temp3+0x80;
				XBYTE[0x21B9] = temp3+0x80;
				XBYTE[0x21BA] = 0x40-temp3;
			}
            break;

        case 0x07: //Saturation
        	#if 0
			if( G_ae_ON == 0 )
			{
				sp1kAwbDisable();
				G_ae_ON = 2;
			}
			else if( G_ae_ON == 1 )
			{
				sp1kAwbEnable();
				G_ae_ON = 2;
			}
			#endif
			XBYTE[0x21B0] = 0x11;
			XBYTE[0x21B7] = G_pucCtrlDataPointer[0];
            break;

        case 0x08: //Sharpness
			XBYTE[0x21D0] = 0x02;
			XBYTE[0x21D5] = G_pucCtrlDataPointer[0];
            break;
	}
}
#endif

#if STILL_SNAP_ON
void SnapStillImage(void)
{
	if (G_SnapFlag == 1)
	{
		XBYTE[0x254f] |= 0x40; //set err flag
		sp1kUsbPccamStop();
		XBYTE[0x2548] &= 0xfe;//disable video ISO pipe
		XBYTE[0x254f] &= 0xbf; //clear err flag
		G_ucStillImageTriggerFlag = 2;
		SetCameraSize(1);//get snap size
		//  printf("snap w h=%d,%d\n",G_usPostJpegWidth,G_usPostJpegHeight);
		//G_usPostJpegWidth = 3456;
		//G_usPostJpegHeight = 2592;
		sp1kSnapSizeSet(G_usPostJpegWidth, G_usPostJpegHeight);
		sp1kSnapParamSet(SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT, 0);
		//printf("start sp1kSnapTask!!!!\n");
		sp1kSnapTask();
		//printf("end sp1kSnapTask!!!!\n");
		sp1kSnapParamSet(SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT, 1);

		if(G_FormatIndex == VC_FORMAT_INDEX_UNCOMPRESSED_YUY2) {
			// set usb pccam data format to vlc.
			XBYTE[REG_Usb_PccamDataFormat] = 0x01;  //0;yuy2 1; vlc
		}

		XBYTE[0x2517] |= 0x04; 	//force DATA0
		XBYTE[0x2548] |= 0x01;	//Enable video ISO pipe
		XBYTE[0x254f] = 0xAC; 	//set still flag
		XBYTE[0x2000] = 0x04;	//Set to pccam mode
		XBYTE[0x27c2] = 0x00;	//set imgtype test mode
		XBYTE[0x2000] = 0x00;	//Set to idle mode
		// XBYTE[0x27e3]=0x01;	//start video clip
		//  XBYTE[0x2504]|=0x80; 	//enable snap done int  not used usb interrupt,modify for DRMA interrupt
		XBYTE[0x2708] |= 0x40; 	//eable dram usbrdyend_sts_en
		//  printf("snap end\n");
		// XBYTE[0x254f]|=0x20; 	//set still flag
		G_ucStillImageTriggerFlag = 0;
		//  G_ucStillImageTriggerFlag=2;
		//   sp1kUsbPccamStart(1);

		G_SnapFlag = 0;
	}


	if (G_SnapFlag == 2) //data
	{
		G_SnapFlag = 0;
		sp1kUsbPccamStart();
	}
}
#endif

void
usbSvcProc(
	void* ptr
)
{
	ptr = ptr;

		/* Usb service */
#if 1
#if(DDX_on)
		if(G_PCCAME_FD_ON==1)
		{
			//sp1kFdReset();
			sp1kFdEnable();//cx++ for  fd open
			G_PCCAME_FD_ON=2;
		}else if(G_PCCAME_FD_ON==0)
		{
			sp1kFdDisable();
			G_PCCAME_FD_ON=3;
		}
 #endif

	if (G_bRequestLock) {
		mainTestAp();
	}

	if (((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)
				&& (gUsbMode == K_UIOPTION_STORAGE_MSDC)){
		USBMSDC_ReadWriteData();
	}
#if	USBSIDC_OPTION
	if (((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)
				&& (gUsbMode == K_UIOPTION_STORAGE_SIDC)) {
		sidcSvcProc();
		dpsSvcProc();
	}
#endif
#endif
#if 0//xian move to intr, 20090717
	if (videoRecStampEofGet()) {
		pvPhofrmBlend(K_SDRAM_ImagePlaybackBufAddr, 320, 240);
		videoRecStampEofSet(0);
	}
#endif
#if 0//xian move to intr, 20090717
	//xian ++ 20081217
	if (pvPhoFrmCdspEofGet()) {
		pvPhofrmBlend(K_SDRAM_ImagePlaybackBufAddr, 320, 240);
		pvPhoFrmCdsoEofSet(0);
	}
	//xian --
#endif

		//cx++ for UVC
		if ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_PCCAM){
			if(G_usbPvFrameCount >= 4) {
				usbPvSizeSet();
				G_usbPvFrameCount = 0;
			}

			//#0043939 We use stand uvc driver only now
			if(G_UsbPccamStart) {
				if (G_PCCAM == 0){
					G_PCCAM = 1;
					printf("p1\n");
					sp1kUsbPccamStart();
					G_UsbPccamStart = 0;
				}
			}
			if(G_UsbPccamStop) {
				printf("p0\n");
				G_PCCAM = 0;
				sp1kUsbPccamStop();
				G_UsbPccamStop = 0;
			}

			#if(DDX_on)
			if(G_ucPccamTuningFlag==1)
			{
				USBUVC_ProcessUnit();
				G_ucPccamTuningFlag = 0;
			}
			#endif
#if defined(ICAT_OPTION)
			if(G_ucPccamTuningFlag==2)
			{
				icatIQTunningProcess();
				G_ucPccamTuningFlag = 0;
			}
#endif
//cx add

			/* Usb audio service */
			if(G_USBAudioSetInterface)
			{
				printf("start UAC\n");
				HAL_AudPause();
				HAL_AudStart(0, 44100, 16, 1);
				// delay 2 sample time to make sure pause function take effect
				HAL_GlobalTimerWait10us(4);
				HAL_AudUsbModeSet(1);
				HAL_AudResume();
				XBYTE[REG_Usb_IsoPipeEn]|=0x02; // Enable audio ISO pipe  //0x2548
				G_USBAudioSetInterface=0;
			}
			else if (G_USBAudSetInterface == 2) //stop audio rec
			{
				printf("stop UAC\n");
				HAL_AudStop(0);
				G_USBAudSetInterface = 0;
			}
#if STILL_SNAP_ON
			if( G_ucStillImageTriggerFlag == 1 )  //start snap
			{
				G_ucStillImageTriggerFlag = 0;		// Clear still image trigger flag

				G_SnapFlag = 1;
				//G_StillButtonDisableFlag = 1;
			}
			if ((G_PCCAM == 1)&&(G_SnapFlag != 0))
			{
				SnapStillImage();
			}
#endif

		}
		//cx++ end
	if (G_USBSpeedStartCheck == 0x01){

		UINT32 time;

		HAL_GlobalReadGTimer(&time);
		//printf("1,2=%lx,%lx\n",time,G_USBCheckNewConfigStartTime);
		if (((time - G_USBCheckNewConfigStartTime ) >= 2000) && (G_USBNewConfigureOK == 0x00)){
			#if !FUNC_HOST_DPS
			HAL_UsbForceFS(0);
			#endif
			G_USBNewConfigureOK = 0x01;
		}
	}

}

UINT8 sysTimeSync(MYSYSTIME* pDate)
{
	dateStc_t rtcDate = {0};
	UINT8 year;

	if (pDate) {
		year = (pDate->wYear >> 8) % 100; /* get the value below 100 */
		rtcDate.Year = year > 8 ? year - 8 : year;
		rtcDate.Month = (pDate->wMonth >> 8);
		rtcDate.Day = (pDate->wDay >> 8);
		rtcDate.Hour = (pDate->wHour >> 8);
		rtcDate.Minute = (pDate->wMinute >> 8);
		rtcDate.Second = (pDate->wSecond >> 8);

		/* write date to RTC register */
		HAL_GlobalWriteRTC(&rtcDate);
	}

	return SUCCESS;
}
