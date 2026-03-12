/**************************************************************************
*
*      Copyright (c) 2005-2007 by Sunplus mMedia Technology Co., Ltd.
*
*  This software is copyrighted by and is the property of Sunplus mMedia
*  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Technology
*  Co., Ltd. This software may only be used in accordance with the
*  corresponding license agreement. Any unauthorized use, duplication,
*  distribution, or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of Sunplus mMedia Technology Co., Ltd.
*
*  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this
*  software without notice.
*
*  Sunplus mMedia Technology Co., Ltd.
*  19, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan.
*
**************************************************************************/
/**
* @file		pccam.c
* @brief		PCCAM related function
* @author	Allen
* @since		2008-06-04
* @date		2008-12-12
*/

/**************************************************************************
*                            H E A D E R   F I L E S
**************************************************************************/
#include "general.h"
#include "initio.h"
#include "pv_task.h"
#include "disp_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_pv_api.h"
#include "awb_api.h"//wangjg++
#include "snap_qtable.h"
#include "hal_jpeg.h"
#include "main.h"
#include "pccam.h"

#include "class.h"  //cx++
#include "hal_dram.h"
#include "hal_usb.h"
#include "doslink.h"
#include "utility.h"
#include "sp1k_rsvblk_api.h"
#include "front_logi.h"
#include "sp1k_front_api.h"
#include "awb_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_fd_api.h"
#include "hal_moe.h"//wangjg++
#include "hal_fd.h"//wangjg++
#include "reg_def.h"
#include "video_mem.h"
#include "rsc_id.h"
#include "sp1k_video_api.h"
#include "app_video_view.h"

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
xdata   UINT8    G_ucSensorType;
xdata   UINT8    G_ucSensorSize;
xdata   UINT8    G_ucDscIndex;
xdata   UINT8    G_ucClipIndex;
xdata   UINT8    G_ucPccameraIndex;
xdata   UINT8    G_ucDscIndexMax;
xdata   UINT8    G_ucClipIndexMax;
xdata   UINT8    G_ucPccameraIndexMax;
xdata	UINT16	G_usPostJpegWidth;
xdata	UINT16	G_usPostJpegHeight;
xdata   UINT8 	G_usbPvFrameCount;

/**************************************************************************
*				   E X T E R N A L	  R E F E R E N C E S				  *
**************************************************************************/

extern xdata 	UINT8	G_PowerFreq;
extern  xdata   UINT8     G_PCCAME_FD_ON;

#if STILL_SNAP_ON
extern  xdata	UINT8	G_StillFormatIndex;
extern  xdata	UINT8	G_StillFrameIndex;
extern xdata   UINT8     G_ucStillImageTriggerFlag;
#endif


code UINT8 ProductDescriptor1[] =
{
	//-------------------------------------------------------------------
	//TotalLength = 0x0040 byte
	//-------------------------------------------------------------------

	/*Product (0x40 byte)*/
	0x40,                   /*bLength: 0x40 byte*/
	0x01,                   /*bDescriptorType: Sensor information*/
	0x02,                   /*1:CCD,  2:CMOS*/
	0x04,                   /*1:CIF,  2:VGA,  3:800k,  4:1.0M,  5:1.3M*/
	/*6:2.0M, 7:3.0M, 8:4.0M*/

	/*8 Byte  :
	//byte 1~7 :DSC size index
	//byte 8   :DSC default Size index
	/*/
	0x08,                   /*byte1:dsc size index 8 = 1152 x 864*/
	0x00,                   /*byte2:               0 = no use*/
	0x00,                   /*byte3:               0 = no use*/
	0x00,                   /*byte4:               0 = no use*/
	0x00,                   /*byte5:               0 = no use*/
	0x00,                   /*byte6:               0 = no use*/
	0x00,                   /*byte7:               0 = no use*/
	0x01,                   /*byte8:dsc size default size : 1 (1152 x 864)*/

	/*8 Byte  :
	//byte 1~7 :Video clip size index
	//byte 8   :Video Clip default Size index
	/*/
	0x04,                   /*byte1:clip size index 4 = 320 x 240*/
	0x00,                   /*byte2:                0 = no use*/
	0x00,                   /*byte3:                0 = no use*/
	0x00,                   /*byte4:                0 = no use*/
	0x00,                   /*byte5:                0 = no use*/
	0x00,                   /*byte6:                0 = no use*/
	0x00,                   /*byte7:                0 = no use*/
	0x01,                   /*byte8:clip size default size : 1 (320 x 240)*/

	/*8 Byte  :
	//byte 1~7 :pc camera size index
	//byte 8   :pc camera default Size index
	/*/
	0x0A,                   /*byte1:pc camera size index 10 = 640 x 480*/
	0x05,                   /*byte2:                     5 = 320 x 240*/
	0x05,                   /*byte3:                     5 = 320 x 240*/
	0x00,                   /*byte4:                     0 = no use*/
	0x00,                   /*byte5:                     0 = no use*/
	0x00,                   /*byte6:                     0 = no use*/
	0x00,                   /*byte7:                     0 = no use*/
	0x01,                   /*byte8:pc camera size default size : 1 (320 x 240)*/

	//sensor name description: 20 Byte
	'O','m','n','i','v','i','s','i','o','n',' ','3','.','1','M',' ',  0,  0,  0,  0,

	//idvendor description: 16 Byte
	'S','u','n','p','l','u','s',' ','C','A','1','5','2','8',  0,  0,
};


code UINT8 PcCameraSizeMapping1[] =
{
	/*-------------------------------------------------------------------
	//TotalLength = 0x0028 byte
	//-------------------------------------------------------------------*/

	/*Product (0x28 byte)*/
	0x28,                   /*bLength: 0x28 byte*/
	0x03,                   /*bDescriptorType:  DscSizeInformation*/
	0x00,                   /*reserve*/
	0x00,                   /*reserve*/

	0x00,                   /* 160   : width     , DSCSizeIndex = 1*/
	0xa0,                   /**/
	0x00,                   /* 128   : height*/
	0x80,

	0x00,                   /* 160   : width     , DSCSizeIndex = 2*/
	0xa0,                   /**/
	0x00,                   /* 160   : height*/
	0xa0,

	0x00,                   /* 160               , DSCSizeIndex = 3*/
	0xa0,                   /**/
	0x00,                   /* 224*/
	0xe0,

	0x00,                   /* 160               , DSCSizeIndex = 4*/
	0xa0,                   /**/
	0x01,                   /* 400*/
	0x90,

	0x01,                   /* 320               , DSCSizeIndex = 5*/
	0x40,                   /**/
	0x00,                   /* 240*/
	0xF0,

	0x01,                   /* 464               , DSCSizeIndex = 6*/
	0xd0,                   /**/
	0x01,                   /* 336*/
	0x50,

	0x01,                   /* 464               , DSCSizeIndex = 7*/
	0xd0,                   /**/
	0x01,                   /* 352*/
	0x60,

	0x01,                   /* 464               , DSCSizeIndex = 8*/
	0xd0,                   /**/
	0x01,                   /* 432*/
	0xb0,

	0x01,                   /* 464               , DSCSizeIndex = 9*/
	0xd0,                   /**/
	0x01,                   /* 480*/
	0xe0,

	0x02,                   /* 640               , DSCSizeIndex = 10*/
	0x80,                   /**/
	0x01,                   /* 480*/
	0xe0,

	0x01,                   /* 320               , DSCSizeIndex = 11*/
	0x40,                   /**/
	0x00,                   /* 240*/
	0xf0,

	0x01,                   /* 464               , DSCSizeIndex = 12*/
	0xd0,                   /**/
	0x01,                   /* 336*/
	0x50,

	0x01,                   /* 464               , DSCSizeIndex = 13*/
	0xd0,                   /**/
	0x01,                   /* 352*/
	0x60,

	0x01,                   /* 464               , DSCSizeIndex = 14*/
	0xd0,                   /**/
	0x01,                   /* 432*/
	0xb0,

	0x01,                   /* 464               , DSCSizeIndex = 15*/
	0xd0,                   /**/
	0x01,                   /* 480*/
	0xe0
};

void sp1kUsbIntrDataTx(UINT8 Event,UINT8 Value)
{	/*pdatabuf: bStatusType : 2=videostreaming inteface
                         bOriginator   : 1= video streaming inteface number
                         bEvent:        : 0=button press
                         bVale:          : 1=button pressed*/
	UINT8 pdatabuf[4];
	pdatabuf[0]=2;
	pdatabuf[1]=1;
	pdatabuf[2]=Event;
	pdatabuf[3]=Value;
	HAL_UsbEP7IntrDataTx(pdatabuf, 4);
}

void SetCameraSize(UINT8 type)
{
	UINT8 FormatIndex, FrameIndex;// FrameRate;
	if (type == 0) {
		FormatIndex = G_FormatIndex;
		FrameIndex = G_FrameIndex;
		//FrameRate = G_FrameRate;
	}
#if STILL_SNAP_ON
	else if (type == 1) {
		FormatIndex =G_StillFormatIndex;// VSVideoStillProbeCommitControl.bFormatIndex;
		FrameIndex = G_StillFrameIndex;//VSVideoStillProbeCommitControl.bFrameIndex;
		//FrameRate = G_StillFrameRate;
	}
#endif

	if(type == 0) //video preview
	{
		if (G_bUSBSpeed == 0x01)
		{
			switch (FormatIndex)
			{
			case VC_FORMAT_INDEX_MJPEG_YUV422:			//MJPEG
				switch (FrameIndex)
				{
				case VC_VIDEO_FRAME_SIZE_160x120:
					G_usPostJpegWidth=160;
					G_usPostJpegHeight=120;
					break;
				case VC_VIDEO_FRAME_SIZE_320x240:
					G_usPostJpegWidth=320;
					G_usPostJpegHeight=240;
					break;
				case VC_VIDEO_FRAME_SIZE_640x480:
					G_usPostJpegWidth=640;
					G_usPostJpegHeight=480;
					break;
				case VC_VIDEO_FRAME_SIZE_1280x720:
					G_usPostJpegWidth=1280;
					G_usPostJpegHeight=720;
					break;
				default:
					break;
				}
				break;
			case VC_FORMAT_INDEX_UNCOMPRESSED_YUY2:		//YUV2
			case VC_FORMAT_INDEX_RAW10:
				switch (FrameIndex)
				{
				case VC_VIDEO_FRAME_SIZE_160x120:
					G_usPostJpegWidth=160;
					G_usPostJpegHeight=120;
					break;
				case VC_VIDEO_FRAME_SIZE_320x240:
					G_usPostJpegWidth=320;
					G_usPostJpegHeight=240;
					break;
				case VC_VIDEO_FRAME_SIZE_640x480:
					G_usPostJpegWidth=640;
					G_usPostJpegHeight=480;
					break;
				case VC_VIDEO_FRAME_SIZE_1280x720:
					G_usPostJpegWidth=1280;
					G_usPostJpegHeight=720;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		else
		{
			switch (FormatIndex)
			{
			case VC_FORMAT_INDEX_MJPEG_YUV422:			//MJPEG
				switch (FrameIndex)
				{
				case 0x01:
					G_usPostJpegWidth=320;
					G_usPostJpegHeight=240;
					break;
				case 0x02:
					G_usPostJpegWidth=160;
					G_usPostJpegHeight=120;
					break;
				default:
					break;
				}
				break;
			case VC_FORMAT_INDEX_UNCOMPRESSED_YUY2:		//YUV2
			case VC_FORMAT_INDEX_RAW10:
				switch (FrameIndex)
				{
				case 0x01:
					G_usPostJpegWidth=160;
					G_usPostJpegHeight=120;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
	}
#if STILL_SNAP_ON
	else if( type == 1)
	{
		if (G_bUSBSpeed == 0x01)
		{
			switch (FormatIndex)
			{
			case VC_FORMAT_INDEX_MJPEG_YUV422:			//MJPEG
			case VC_FORMAT_INDEX_UNCOMPRESSED_YUY2:
				switch (FrameIndex)
				{
				case VC_VIDEO_FRAME_SIZE_3456x2592: //9M
					G_usPostJpegWidth = 3456;
					G_usPostJpegHeight = 2592;
					break;
				case VC_VIDEO_FRAME_SIZE_2592x1944: //2M
					G_usPostJpegWidth = 2592;
					G_usPostJpegHeight = 1944;
					break;
				case VC_VIDEO_FRAME_SIZE_1024x768: //1M
					G_usPostJpegWidth = 1024;
					G_usPostJpegHeight = 768;
					break;
				case VC_VIDEO_FRAME_SIZE_160x120:
					G_usPostJpegWidth = 160;
					G_usPostJpegHeight = 120;
					break;
				case VC_VIDEO_FRAME_SIZE_320x240:
					G_usPostJpegWidth = 320;
					G_usPostJpegHeight = 240;
					break;

				case VC_VIDEO_FRAME_SIZE_640x480:
					G_usPostJpegWidth = 640;
					G_usPostJpegHeight = 480;
					break;
				case VC_VIDEO_FRAME_SIZE_1280x720:
					G_usPostJpegWidth = 1280;
					G_usPostJpegHeight = 720;
					break;

				default:
					G_usPostJpegWidth = 0;
					G_usPostJpegHeight = 0;
					break;
				}
				break;
			default:
				break;
			}
		}
		else //usb1.1
		{
			switch (FormatIndex)
			{
			case VC_FORMAT_INDEX_MJPEG_YUV422:			//MJPEG
			case VC_FORMAT_INDEX_UNCOMPRESSED_YUY2:
				switch (FrameIndex)
				{
				case 0x01:
					G_usPostJpegWidth=320;
					G_usPostJpegHeight=240;
					break;
				case 0x02:
					G_usPostJpegWidth=160;
					G_usPostJpegHeight=120;
					break;
				default:
					break;
				}
			default:
				break;
			}
		}
	}
#endif

//#if (STILL_NEW_ZOOM == 1)
	switch ( FrameIndex )
	{
	case VC_VIDEO_FRAME_SIZE_160x120:
		sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_PCCAM_QQVGA);
		break;
	case VC_VIDEO_FRAME_SIZE_320x240:
		sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_PCCAM_QVGA);
		break;
	case VC_VIDEO_FRAME_SIZE_640x480:
		sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_PCCAM_VGA);
		break;
	case VC_VIDEO_FRAME_SIZE_1280x720:
		sp1kVideoZoomResLoad(SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR,SP1K_RSC_ID_VIDEO_720P_ZOOM_REG,SP1K_RSC_ID_PCCAM_HD_88);
		break;
	default:
		break;
	}
//#endif
}

//-----------------------------------------------------------------------------
//sp1kUsbPccamStart
//-----------------------------------------------------------------------------
/**
* @brief		Start PCCAM by different resolution. (resolution index is sent by vendor command 0x25)
* @param		None
* @retval	None
* @see
* @author	allenlin
* @since		2008-06-04
*/
void
sp1kUsbPccamStart(
				  void
				  )
{
	UINT16 pvW, pvH;//,dly;
	UINT32 size;
	UINT16 frontHSize,frontVSize;
	UINT16 cropHSize,cropVSize;
	UINT32 addrA = K_SDRAM_CommonEndAddr;
	UINT32 addrB = K_SDRAM_CommonEndAddr + 75ul*1024;

	SetCameraSize(0);
	//printf("cropHSize=%d,%d\n",G_usPostJpegWidth,G_usPostJpegHeight);
	dispPowerOff(dispDevActGet());

	frontPreviewSizeGet(0, &frontHSize, &frontVSize);

	sp1kYuvBufModeSet();

	if (frontHSize*3 > frontVSize*4) {//16:9

		if (G_usPostJpegWidth*3 > G_usPostJpegHeight*4) { /* 16:9 */
			cropHSize = frontHSize;
			cropVSize = frontVSize;
		}
		else {
			cropHSize = frontVSize*4/3;
			cropVSize = frontVSize;
		}
	}
	else {
		if (G_usPostJpegWidth*3 > G_usPostJpegHeight*4) { /* 16:9 */
			cropHSize = frontHSize;
			cropVSize = frontVSize*3/4;
		}
		else {
			cropHSize = frontHSize;
			cropVSize = frontVSize;
		}

	}

	sp1kPreviewCropSizeSet(cropHSize,cropVSize);

	//while(dly){dly--;};
	//printf("cropHSize=%d,%d\n",cropHSize,cropVSize);
	sp1kPreviewSet(1, G_usPostJpegWidth, G_usPostJpegHeight, 100, 0);    /*will revise*/
	//printf("cropHSize=%d,%d\n",cropHSize,cropVSize);
	XBYTE[REG_Dram_ZfUpdate] = 0x01;
	HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, (UINT16)300);
	HAL_UsbPccamInit(G_FormatIndex);

	if (G_FormatIndex == VC_FORMAT_INDEX_MJPEG_YUV422) {
		//printf("@UVC MJPEG mode\n");
		/* Load jpeg header file */

		sp1kNandRsvRead(0x49,addrA<<1);
		sp1kNandRsvRead(0x49,addrB<<1);

#if 0
		HAL_DramStartAddrSet(K_SDRAM_AviCapVLCBufAddrA, 1);
		HAL_DramWordDataRead(&LSB, &MSB);
		printf("A=%bx,%bx\n",LSB,MSB);
		HAL_DramStartAddrSet(K_SDRAM_AviCapVLCBufAddrB, 1);
		HAL_DramWordDataRead(&LSB, &MSB);
		printf("B=%bx,%bx\n",LSB,MSB);

		HAL_DramStartAddrSet(K_SDRAM_AviCapVLCBufAddrA+0x118, 1);
		HAL_DramWordDataRead(&LSB, &MSB);
		printf("R=%bx,%bx\n",LSB,MSB);
		HAL_DramWordDataRead(&LSB, &MSB);
		printf("%bx,%bx\n",LSB,MSB);
		HAL_DramWordDataRead(&LSB, &MSB);
		printf("%bx,%bx\n",LSB,MSB);
#endif
		HAL_DramStartAddrSet(addrA+0x118, 0);
		HAL_DramWordDataWrite(0x08, 0x00);
		HAL_DramWordDataWrite(0x00, 0x00);
		HAL_DramWordDataWrite(0x00, 0x03);

		HAL_DramStartAddrSet(addrB+0x118, 0);
		HAL_DramWordDataWrite(0x08, 0x00);
		HAL_DramWordDataWrite(0x00, 0x00);
		HAL_DramWordDataWrite(0x00, 0x03);
#if 0
		HAL_DramStartAddrSet(K_SDRAM_AviCapVLCBufAddrA+0x118, 1);
		HAL_DramWordDataRead(&LSB, &MSB);
		printf("R=%bx,%bx\n",LSB,MSB);
		HAL_DramWordDataRead(&LSB, &MSB);
		printf("%bx,%bx\n",LSB,MSB);
		HAL_DramWordDataRead(&LSB, &MSB);
		printf("%bx,%bx\n",LSB,MSB);
#endif

		jpegQTableSet(50, NULL, NULL, 1);//65//85
		size = 0x253;

		G_usbPvFrameCount = 0;
		XBYTE[REG_Jpeg_Intr_Enable] |= 0x02;

		XBYTE[REG_Jpeg_VLCofset_Low] = READ8(&size, 3);/*VLC offset bytecnt*/
		XBYTE[REG_Jpeg_VLCofset_High] = READ8(&size, 2);

		pvSizeGet(&pvW, &pvH);
		HAL_JpgZFactorSet( 100, 100, pvW, pvH, G_usPostJpegWidth, G_usPostJpegHeight, 0 );

		if ((G_usPostJpegHeight == 120) && (G_usPostJpegWidth == 160)) {
			HAL_UsbAutoCompress( addrA, addrB, 0); /* 0/1=yuv422/yuv420 *///YUY422 for 160*120
		}
		else {
			HAL_UsbAutoCompress( addrA, addrB, 1); /* 0/1=yuv422/yuv420 */
		}

		HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV); /*ZF update JPG parameters*/
		XBYTE[REG_Dram_ZfUpdate] = 0x01;
		//HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_MPEG, (UINT16)300);
		HAL_DramZUpdateWait(HAL_DRAM_ZF_UPD_LAYER_PV, (UINT16)300);
		//   printf("@MJPEG finish\n");
	}
	else if (G_FormatIndex == VC_FORMAT_INDEX_UNCOMPRESSED_YUY2) {
		//printf("@UVC YUY2 mode\n");
	}

	if ((G_usPostJpegHeight == 120) && (G_usPostJpegWidth == 160)) {
		aePccamSrcSizeSet(AE_PCCAM_160_120);
	}
	else {
		aePccamSrcSizeSet(AE_PCCAM_320_240);
	}

	/*PCCAM AE/AWB/VD INTERRUPT ENABLE S*/
	sp1kAeSetModeEn(1);
	// wangjg++
	//sp1kAwbReset();
	sp1kVdInit();
	sp1kAwbEnable();
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,0x00);
	HAL_MoeEnable();
	HAL_FdEnable(1,0);
	// wangjg--

#if (DDX_on)
	if (G_PCCAME_FD_ON == 2) {
		// sp1kFdReset();
		sp1kFdEnable();//cx++ for  fd open
		//  printf("@pc cam fd on\n");
	}
#endif
	/*PCCAM AE/AWB/VD INTERRUPT ENABLE S*/
}

void usbPvSizeSet()
{
	UINT32 addrA = K_SDRAM_CommonEndAddr;
	UINT32 addrB = K_SDRAM_CommonEndAddr + 75ul*1024;

	XBYTE[REG_Jpeg_Intr_Enable] &= (~0x02); /*stop jpg eof*/

	HAL_DramStartAddrSet(addrA+0x118, 0);
	HAL_DramWordDataWrite(0x08, READ8(&G_usPostJpegHeight, 0));
	HAL_DramWordDataWrite(READ8(&G_usPostJpegHeight, 1), READ8(&G_usPostJpegWidth, 0));
	HAL_DramWordDataWrite(READ8(&G_usPostJpegWidth, 1), 0x03);

	if ((G_usPostJpegHeight == 120) && (G_usPostJpegWidth == 160)) {
		HAL_DramWordDataWrite(0x01,0x21); //YUY422 for 160*120
	}

	HAL_DramStartAddrSet(addrB+0x118, 0);
	HAL_DramWordDataWrite(0x08, READ8(&G_usPostJpegHeight, 0));
	HAL_DramWordDataWrite(READ8(&G_usPostJpegHeight, 1), READ8(&G_usPostJpegWidth, 0));
	HAL_DramWordDataWrite(READ8(&G_usPostJpegWidth, 1), 0x03);

	if ((G_usPostJpegHeight == 120) && (G_usPostJpegWidth == 160)) {
		HAL_DramWordDataWrite(0x01,0x21); //YUY422 for 160*120
	}
}

//-----------------------------------------------------------------------------
//sp1kUsbPccamStop
//-----------------------------------------------------------------------------
/**
* @brief		Stop PCCAM and enable display module.
* @param		None
* @retval	None
* @see
* @author	allenlin
* @since		2008-06-04
*/
void
sp1kUsbPccamStop(
				 void
				 )
{
#if 0 /* mark for 1628 FPGA */
	dispEn(1);
	/* 20080611 mattwang modify, because use new disp drv. */
#if 1  /* new. */
	HAL_DispRegUpdate();
#else  /* old. */
	dispRegUpd();
#endif
#endif

#if (DDX_on)
	if(G_PCCAME_FD_ON==2)
	{
		sp1kFdDisable();
	}
#endif

	//#if TAG_TODO
	HAL_JpegMjpgAbort();
	//#endif
	//wangjg++
	sp1kAeSetModeEn(AE_DISABLE);
	sp1kAwbDisable();
	HAL_MoeReset();
	HAL_FdEnable(0,0);
	//wangjg--
}

//-----------------------------------------------------------------------------
//INITIO_SetCameraSize
//-----------------------------------------------------------------------------
void INITIO_SetCameraSize(void) USING_0    //cch
/*++

Routine Description:

set sensor width ,height ,Hsubsample ,Vsubsample

Arguments:

cameraMode : 2 :dsc , 3:vido clip mode : 4: Pc camera

Return Value:

none

--*/
{
	UINT16	k;
	UINT8*	ProductDescriptor;
	UINT8*	PcCameraSizeMapping;


	ProductDescriptor = ProductDescriptor1;
	PcCameraSizeMapping = PcCameraSizeMapping1;


	k = 4 + ( ProductDescriptor[3 + 8 * 2 + G_ucPccameraIndex] - 1 ) * 4;

	G_usPostJpegWidth = ((UINT16)PcCameraSizeMapping[k] << 8) | ((UINT16)PcCameraSizeMapping[k + 1]);
	G_usPostJpegHeight = ((UINT16)PcCameraSizeMapping[k + 2] << 8) | ((UINT16)PcCameraSizeMapping[k + 3]);

	//printf("w,h=%d,%d\n",G_usPostJpegWidth,G_usPostJpegHeight);

}


//-----------------------------------------------------------------------------
//INITIO_GetCameraSizeIndex      //cch
//-----------------------------------------------------------------------------
void INITIO_GetCameraSizeIndex(UINT16 index) USING_1
/*++

Routine Description:


Arguments:

none

Return Value:

none

--*/
{
	switch (index)
	{
	case 0x0001:      //Get Preview Size Index
		G_pucCtrlDataPointer[0] = G_ucDscIndex;
		break;

	case 0x0002:      //Get DSC Size Index
		G_pucCtrlDataPointer[0] = G_ucDscIndex;
		break;

	case 0x0003:      //Get Video Clip Size Index
		G_pucCtrlDataPointer[0] = G_ucClipIndex;
		break;

	case 0x0004:      //Get PC camera Index
		G_pucCtrlDataPointer[0] = G_ucPccameraIndex;
		break;

	case 0x0005:      //Get Playback Index
		G_pucCtrlDataPointer[0] = G_ucDscIndex;
		break;

	case 0x0011:      //Get Preview Size Index Max
		G_pucCtrlDataPointer[0] = G_ucDscIndexMax;
		break;

	case 0x0012:      //Get DSC Size Index Max
		G_pucCtrlDataPointer[0] = G_ucDscIndexMax;
		break;

	case 0x0013:      //Get Video Clip Size Index Max
		G_pucCtrlDataPointer[0] = G_ucClipIndexMax;
		break;

	case 0x0014:      //Get PC camera Index Max
		G_pucCtrlDataPointer[0] = G_ucPccameraIndexMax;
		break;

	case 0x0015:      //Get Playback Index
		G_pucCtrlDataPointer[0] = G_ucDscIndexMax;
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
//INITIO_SetCameraSizeIndex     //cch
//-----------------------------------------------------------------------------
void INITIO_SetCameraSizeIndex(UINT16 index) USING_1
/*++

Routine Description:


Arguments:

none

Return Value:

none

--*/
{
	switch (index)
	{
	case 0x0001:      //Set Preview Size Index
		G_ucDscIndex = G_pucCtrlDataPointer[0];
		break;

	case 0x0002:      //Set DSC Size Index
		G_ucDscIndex = G_pucCtrlDataPointer[0];
		break;

	case 0x0003:      //Set Video Clip Size Index
		G_ucClipIndex = G_pucCtrlDataPointer[0];
		break;

	case 0x0004:      //Set PC camera Index
		G_ucPccameraIndex = G_pucCtrlDataPointer[0];
		break;

	case 0x0005:      //St Playback Index
		G_ucDscIndex = G_pucCtrlDataPointer[0];
		break;

	case 0x0011:      //St Preview Size Index Max
		G_ucDscIndexMax = G_pucCtrlDataPointer[0];
		break;

	case 0x0012:      //Set DSC Size Index Max
		G_ucDscIndexMax = G_pucCtrlDataPointer[0];
		break;

	case 0x0013:      //Set Video Clip Size Index Max
		G_ucClipIndexMax = G_pucCtrlDataPointer[0];
		break;

	case 0x0014:      //Set PC camera Index Max
		G_ucPccameraIndexMax = G_pucCtrlDataPointer[0];
		break;

	case 0x0015:      //Set Playback Index Max
		G_ucDscIndexMax = G_pucCtrlDataPointer[0];
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
//INITIO_GetInformation
//-----------------------------------------------------------------------------
void INITIO_GetInformation(UINT16 infoType) USING_1
/*++

Routine Description:

get system information

Arguments:

none

Return Value:

none

--*/
{
	UINT16 i = infoType;
	//version4.0 richie@ps0503
	UINT8*	ProductDescriptor;
	UINT8*	PcCameraSizeMapping;
	UINT8   PcCameraSizeMappingSize;

	ProductDescriptor = ProductDescriptor1;
	PcCameraSizeMapping = PcCameraSizeMapping1;
	PcCameraSizeMappingSize = K_PcCameraSizeMapping1;

	switch (infoType)
	{
	case 0x0000:      //Get ProductDescriptor information
		for (i = 0; i < K_ProductDescriptorSize; i++)
			G_pucCtrlDataPointer[i] = (UINT8) ProductDescriptor[i];
		break;

	case 0x0001:      //Get SensorSizeInformation
		//version4.0 richie@ps0503
		for (i = 0; i < PcCameraSizeMappingSize; i++)
			G_pucCtrlDataPointer[i] = PcCameraSizeMapping[i];
		break;
#if 0
	case 0x0002:      //Get DscSizeMapping Information
		for (i = 2; i < K_DscSizeMapping; i++)
			G_pucCtrlDataPointer[i] = DscSizeMapping[i];
		break;

	case 0x0064:      //Get Storage information
		i = DOS_GetClusterSize();
		i /=  512;
		G_pucCtrlDataPointer[0] = i;
		G_pucCtrlDataPointer[1] = i >> 8;
		for (i = 2; i < K_CtrlDataSize; i++)
			G_pucCtrlDataPointer[i] = i;
		//Card_GetInformation(G_pucCtrlDataPointer);   // @WYEO, 1008
		break;

		//version4.0 richie@0514
	case 0x0065:      //Get Storage information   compiler error
		G_pucCtrlDataPointer[48] = 0;         //cch@02/26
		Card_GetInformation(&(G_pucCtrlDataPointer[0])); // @HISWYEO
		break;
#endif
	default:
		break;
	}
}
