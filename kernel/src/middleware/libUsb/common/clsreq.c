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
* @file      File name
* @brief     Brief
* @author    Author's name
* @since     2010-01-01
* @date      2010-01-01
*/

/**************************************************************************
*                        H E A D E R   F I L E S
**************************************************************************/
#include "general.h"
//#include "hal.h"
#include "Clsdesc.h"
#include "Clsreq.h"
#include "main.h"
//#include "initio.h"
#include "ctlsetup.h"
#include "class.h"
#include "hal_cdsp_iq.h"
#include "sp1k_ae_api.h"
#include "reg_def.h"
#include "sp1k_uvc_api.h"
//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
//#define	SYSTEM_CLOCK		48000000
//#define	HIGH_SPEED_CLOCK	96000000
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
static UINT8	G_BitDepth;
static UINT16	G_FrameHSize;
static UINT16	G_FrameVSize;
static xdata	UINT8	G_ucProbeControlFlag=0;

xdata	UINT8	G_IsOurDrvier;

#if STILL_SNAP_ON
static UINT8	G_StillBitDepth;
static UINT16	G_StillFrameHSize;
static UINT16	G_StillFrameVSize;
static xdata	UINT8	G_ucStillProbeControlFlag=0;
#endif

static usbcbVNDRFunc_t pFun_uvcUserExtUnitPreIn;
static usbcbVNDRFunc_t pFun_uvcUserExtUnitPostOut;
static usbVNDCb_t uvcUserExtUnitCb;

extern xdata	UINT8	G_UsbPccamStart;
extern xdata	UINT8	G_UsbPccamStop;
extern xdata	UINT8	G_PCCAME_FD_ON;
extern xdata	UINT8	G_USBAudioSetInterface;

#if STILL_SNAP_ON
extern xdata	UINT8	G_StillFormatIndex;
extern xdata	UINT8	G_StillFrameIndex;
extern xdata	UINT8	G_ucStillImageTriggerFlag;
extern xdata	UINT8	snapMainQIdx;

#endif

//extern UINT8 fdRtSize[];
//extern UINT8 fdFaceNum;
extern UINT8 fdKeyRtX;
extern UINT8 fdKeyRtY;
extern UINT8 fdKeyRtS;

UINT16 uvcUserExtUnitPostOut(void);
UINT16 uvcUserExtUnitPreIn(void);

//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//CLSREQ_GetVSVideoProbeCommitControl
//-----------------------------------------------------------------------------
/**
* @fn        void CLSREQ_GetVSVideoProbeCommitControl(void)
* @brief     Remember settings from SET_CUR of Video Probe and Commit Control and do negotiationl
* @param     NONE
* @retval    NONE
* @see       NULL
* @author    cx
* @since     2010-5-14
* @todo      N/A
* @bug       N/A
*/
void CLSREQ_GetVSVideoProbeCommitControl(void)
{
	VSVideoProbeCommitControl.bmHint 					= M_UCHARToUSHORT(G_ucCtrlData[1], G_ucCtrlData[0]);
	VSVideoProbeCommitControl.bFormatIndex 				= G_ucCtrlData[2];
	VSVideoProbeCommitControl.bFrameIndex 				= G_ucCtrlData[3];
	VSVideoProbeCommitControl.dwFrameInterval		    = M_UCHARToULONG(G_ucCtrlData[7], G_ucCtrlData[6], G_ucCtrlData[5], G_ucCtrlData[4]);
	VSVideoProbeCommitControl.wKeyFrameRate				= M_UCHARToUSHORT(G_ucCtrlData[9], G_ucCtrlData[8]);
	VSVideoProbeCommitControl.wPFrameRate				= M_UCHARToUSHORT(G_ucCtrlData[11], G_ucCtrlData[10]);
	VSVideoProbeCommitControl.wCompQuality				= M_UCHARToUSHORT(G_ucCtrlData[13], G_ucCtrlData[12]);
	VSVideoProbeCommitControl.wCompWindowSize			= M_UCHARToUSHORT(G_ucCtrlData[15], G_ucCtrlData[14]);
	VSVideoProbeCommitControl.wDelay					= M_UCHARToUSHORT(G_ucCtrlData[17], G_ucCtrlData[16]);
	VSVideoProbeCommitControl.dwMaxVideoFrameSize		= M_UCHARToULONG(G_ucCtrlData[21], G_ucCtrlData[20], G_ucCtrlData[19], G_ucCtrlData[18]);
	VSVideoProbeCommitControl.dwMaxPayloadTransferSize  = M_UCHARToULONG(G_ucCtrlData[25], G_ucCtrlData[24], G_ucCtrlData[23], G_ucCtrlData[22]);
	CLSREQ_GetVideoParameters(0);
}

//-----------------------------------------------------------------------------
//CLSREQ_GetVideoParameters
//-----------------------------------------------------------------------------
/**
* @fn        void CLSREQ_GetVideoParameters(UINT8 type)
* @brief     Extract video parameters (format, frame size, frame rate and bit depth)
form VSVideoProbeCommitControl and VSStillVideoProbeCommitControl variable
* @param     [in] type: 0 -> video frame,1 -> still video frame
* @retval    NONE
* @see       NULL
* @author    cx
* @since     2010-5-14
* @todo      N/A
* @bug       N/A
*/
void CLSREQ_GetVideoParameters(UINT8 type)
{
	if (type == 0)
	{
		G_FormatIndex = VSVideoProbeCommitControl.bFormatIndex;
		G_FrameIndex = VSVideoProbeCommitControl.bFrameIndex;
		if (VSVideoProbeCommitControl.dwFrameInterval != 0) {
			G_FrameRate = 10000000/VSVideoProbeCommitControl.dwFrameInterval;
		}

		switch( G_FormatIndex )
		{
		case VC_FORMAT_INDEX_MJPEG_YUV422:
		case VC_FORMAT_INDEX_UNCOMPRESSED_YUY2:
			G_BitDepth = 16;
			break;

		case VC_FORMAT_INDEX_RAW10:
			G_BitDepth = 10;
			break;

		default:
			G_BitDepth = 0;
			break;
		}

		switch( G_FrameIndex )
		{
		case VC_VIDEO_FRAME_SIZE_160x120:
			G_FrameHSize = 160;
			G_FrameVSize = 120;
			break;

			/*	case VC_VIDEO_FRAME_SIZE_176x144:
			G_FrameHSize = 176;
			G_FrameVSize = 144;
			break;*/

		case VC_VIDEO_FRAME_SIZE_320x240:
			G_FrameHSize = 320;
			G_FrameVSize = 240;
			break;

			/*case VC_VIDEO_FRAME_SIZE_352x288:
			G_FrameHSize = 352;
			G_FrameVSize = 288;
			break;*/

		case VC_VIDEO_FRAME_SIZE_640x480:
			G_FrameHSize = 640;
			G_FrameVSize = 480;
			break;

		case VC_VIDEO_FRAME_SIZE_1280x720:
			G_FrameHSize = 1280;
			G_FrameVSize = 720;
			break;

		default:
			G_FrameHSize = 0;
			G_FrameVSize = 0;
			break;
		}
	}
#if STILL_SNAP_ON
	else if (type == 1)
	{
		G_StillFormatIndex = VSVideoStillProbeCommitControl.bFormatIndex;
		G_StillFrameIndex = VSVideoStillProbeCommitControl.bFrameIndex;

		switch( G_StillFormatIndex )
		{
		case VC_FORMAT_INDEX_MJPEG_YUV422:
		case VC_FORMAT_INDEX_UNCOMPRESSED_YUY2:
			G_StillBitDepth = 16;
			break;

		default:
			G_StillBitDepth = 0;
			break;
		}
		switch( G_StillFrameIndex )
		{
		case VC_VIDEO_FRAME_SIZE_3456x2592: //9M
			G_StillFrameHSize = 3546;
			G_StillFrameVSize = 2592;
			break;
		case VC_VIDEO_FRAME_SIZE_2592x1944: //5M
			G_StillFrameHSize = 2592;
			G_StillFrameVSize = 1944;
			break;
		case VC_VIDEO_FRAME_SIZE_1024x768: //1M
			G_StillFrameHSize = 1024;
			G_StillFrameVSize = 768;
			break;
		case VC_VIDEO_FRAME_SIZE_160x120:
			G_StillFrameHSize = 160;
			G_StillFrameVSize = 120;
			break;
		case VC_VIDEO_FRAME_SIZE_320x240:
			G_StillFrameHSize = 320;
			G_StillFrameVSize = 240;
			break;
		case VC_VIDEO_FRAME_SIZE_640x480:
			G_StillFrameHSize = 640;
			G_StillFrameVSize = 480;
			break;
		case VC_VIDEO_FRAME_SIZE_1280x720:
			G_StillFrameHSize = 1280;
			G_StillFrameVSize = 720;
			break;
		default:
			G_StillFrameHSize = 0;
			G_StillFrameVSize = 0;
			break;
		}

	}
#endif
}

//-----------------------------------------------------------------------------
//CLSREQ_SetVSVideoProbeCommitControl
//-----------------------------------------------------------------------------
/**
* @fn        void CLSREQ_SetVSVideoProbeCommitControl(void)
* @brief     Set up GET_CUR of Video Probe and Commit Control buffer
* @param   NONE
* @retval    NONE
* @see       NULL
* @author    cx
* @since     2010-5-14
* @todo      N/A
* @bug       N/A
*/
void CLSREQ_SetVSVideoProbeCommitControl(void)
{
	//	ULONG	dwMaxVideoFrameSize;
	UINT16	bmHint, wCompQuality;
	UINT8	bFormatIndex, bFrameIndex;
	UINT32	dwFrameInterval;
	//	UCHAR	dwTransferRate;

	if (G_ucProbeControlFlag == 0)								// First get_cur
	{
		if (G_bUSBSpeed == 0x01)		// initial is MJPEG
		{
			VSVideoProbeCommitControl.wCompQuality = 61;
		}
		else
		{
			VSVideoProbeCommitControl.wCompQuality = 13;
		}

	}
	else if (G_ucProbeControlFlag == 3)
	{								// change selection
		G_ucProbeControlFlag = 0; //cx add
		bmHint = VSVideoProbeCommitControl.bmHint;
		bFormatIndex = VSVideoProbeCommitControl.bFormatIndex;
		bFrameIndex = VSVideoProbeCommitControl.bFrameIndex;
		wCompQuality = VSVideoProbeCommitControl.wCompQuality;
		dwFrameInterval = VSVideoProbeCommitControl.dwFrameInterval;
		VSVideoProbeCommitControl.bmHint = bmHint;
		VSVideoProbeCommitControl.bFormatIndex = bFormatIndex;
		VSVideoProbeCommitControl.bFrameIndex = bFrameIndex;
		VSVideoProbeCommitControl.wCompQuality = wCompQuality;
		VSVideoProbeCommitControl.dwFrameInterval = dwFrameInterval;

		if (G_bUSBSpeed == 0x01)
		{
			VSVideoProbeCommitControl.wCompQuality = 61;
		}
		else
		{
			VSVideoProbeCommitControl.wCompQuality = 13;
		}

	}

	CLSREQ_GetVideoParameters(0);

	VSVideoProbeCommitControl.wDelay = 0;
	VSVideoProbeCommitControl.dwMaxVideoFrameSize = (UINT32) (((UINT32)G_FrameHSize*(UINT32)G_FrameVSize*(UINT32)G_BitDepth)>>3);
	if (G_bUSBSpeed == 0x01)
	{
#if (K_AUDIOINTER)
		VSVideoProbeCommitControl.dwMaxPayloadTransferSize = 2*640;//768;//896;//512;//640;//1020;
#else
		VSVideoProbeCommitControl.dwMaxPayloadTransferSize = 2*1020;//768;//896;//512;//640;//1020;
#endif
	}
	else
	{
#if (K_AUDIOINTER)
		VSVideoProbeCommitControl.dwMaxPayloadTransferSize = 640;//768;//896;//512;//640;//1020;
#else
		VSVideoProbeCommitControl.dwMaxPayloadTransferSize = 1020;//768;//896;//512;//640;//1020;
#endif
	}

	G_ucCtrlData[0]  = M_LoByteOfWord(VSVideoProbeCommitControl.bmHint);
	G_ucCtrlData[1]  = M_HiByteOfWord(VSVideoProbeCommitControl.bmHint);
	G_ucCtrlData[2]  = VSVideoProbeCommitControl.bFormatIndex;
	G_ucCtrlData[3]  = VSVideoProbeCommitControl.bFrameIndex;
	G_ucCtrlData[4]  = M_LoByteOfDword(VSVideoProbeCommitControl.dwFrameInterval);
	G_ucCtrlData[5]  = M_MidLoByteOfDword(VSVideoProbeCommitControl.dwFrameInterval);
	G_ucCtrlData[6]  = M_MidHiByteOfDword(VSVideoProbeCommitControl.dwFrameInterval);
	G_ucCtrlData[7]  = M_HiByteOfDword(VSVideoProbeCommitControl.dwFrameInterval);
	G_ucCtrlData[8]  = M_LoByteOfWord(VSVideoProbeCommitControl.wKeyFrameRate);
	G_ucCtrlData[9]  = M_HiByteOfWord(VSVideoProbeCommitControl.wKeyFrameRate);
	G_ucCtrlData[10] = M_LoByteOfWord(VSVideoProbeCommitControl.wPFrameRate);
	G_ucCtrlData[11] = M_HiByteOfWord(VSVideoProbeCommitControl.wPFrameRate);
	G_ucCtrlData[12] = M_LoByteOfWord(VSVideoProbeCommitControl.wCompQuality);
	G_ucCtrlData[13] = M_HiByteOfWord(VSVideoProbeCommitControl.wCompQuality);
	G_ucCtrlData[14] = M_LoByteOfWord(VSVideoProbeCommitControl.wCompWindowSize);
	G_ucCtrlData[15] = M_HiByteOfWord(VSVideoProbeCommitControl.wCompWindowSize);
	G_ucCtrlData[16] = M_LoByteOfWord(VSVideoProbeCommitControl.wDelay);
	G_ucCtrlData[17] = M_HiByteOfWord(VSVideoProbeCommitControl.wDelay);
	G_ucCtrlData[18] = M_LoByteOfDword(VSVideoProbeCommitControl.dwMaxVideoFrameSize);
	G_ucCtrlData[19] = M_MidLoByteOfDword(VSVideoProbeCommitControl.dwMaxVideoFrameSize);
	G_ucCtrlData[20] = M_MidHiByteOfDword(VSVideoProbeCommitControl.dwMaxVideoFrameSize);
	G_ucCtrlData[21] = M_HiByteOfDword(VSVideoProbeCommitControl.dwMaxVideoFrameSize);
	G_ucCtrlData[22] = M_LoByteOfDword(VSVideoProbeCommitControl.dwMaxPayloadTransferSize);
	G_ucCtrlData[23] = M_MidLoByteOfDword(VSVideoProbeCommitControl.dwMaxPayloadTransferSize);
	G_ucCtrlData[24] = M_MidHiByteOfDword(VSVideoProbeCommitControl.dwMaxPayloadTransferSize);
	G_ucCtrlData[25] = M_HiByteOfDword(VSVideoProbeCommitControl.dwMaxPayloadTransferSize);

}

#if STILL_SNAP_ON
void CLSREQ_GetVSVideoStillProbeCommitControl(void)
{
	UINT8	bFormatIndex, bFrameIndex;

	if (G_ucStillProbeControlFlag == 0)								// First get_cur
	{
		VSVideoStillProbeCommitControl = VSVideoStillProbeCommitControlInit;
	}
	else if (G_ucStillProbeControlFlag == 3)
	{								// change selection
		bFormatIndex = VSVideoStillProbeCommitControl.bFormatIndex;
		bFrameIndex = VSVideoStillProbeCommitControl.bFrameIndex;
		VSVideoStillProbeCommitControl = VSVideoStillProbeCommitControlInit;
		VSVideoStillProbeCommitControl.bFormatIndex = bFormatIndex;
		VSVideoStillProbeCommitControl.bFrameIndex = bFrameIndex;
	}

	CLSREQ_GetVideoParameters(1);

	VSVideoStillProbeCommitControl.dwMaxVideoFrameSize = (UINT32) (((UINT32)G_StillFrameHSize*(UINT32)G_StillFrameVSize*(UINT32)G_StillBitDepth)>>3);

	if (G_bUSBSpeed == 0x01)
	{
#if (K_AUDIOINTER)
		VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize = 640;//768;//896;//512;//640;//1020;
#else
		VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize = 1020;//768;//896;//512;//640;//1020;
#endif
	}
	else
	{
#if (K_AUDIOINTER)
		VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize = 640;//768;//896;//512;//640;//1020;
#else
		VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize = 1020;//768;//896;//512;//640;//1020;
#endif
	}

	G_ucCtrlData[0]  = G_StillFormatIndex;
	G_ucCtrlData[1]  = G_StillFrameIndex;
	G_ucCtrlData[2]  = VSVideoStillProbeCommitControl.bCompressionIndex;
	G_ucCtrlData[3]  = M_LoByteOfDword(VSVideoStillProbeCommitControl.dwMaxVideoFrameSize);
	G_ucCtrlData[4]  = M_MidLoByteOfDword(VSVideoStillProbeCommitControl.dwMaxVideoFrameSize);
	G_ucCtrlData[5]  = M_MidHiByteOfDword(VSVideoStillProbeCommitControl.dwMaxVideoFrameSize);
	G_ucCtrlData[6]  = M_HiByteOfDword(VSVideoStillProbeCommitControl.dwMaxVideoFrameSize);
	G_ucCtrlData[7]  = M_LoByteOfDword(VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize);
	G_ucCtrlData[8]  = M_MidLoByteOfDword(VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize);
	G_ucCtrlData[9]  = M_MidHiByteOfDword(VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize);
	G_ucCtrlData[10]  = M_HiByteOfDword(VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize);

}

void CLSREQ_SetVSVideoStillProbeCommitControl(void)
{
	VSVideoStillProbeCommitControl.bFormatIndex = G_ucCtrlData[0];
	VSVideoStillProbeCommitControl.bFrameIndex = G_ucCtrlData[1];
	VSVideoStillProbeCommitControl.bCompressionIndex = G_ucCtrlData[2];
	VSVideoStillProbeCommitControl.dwMaxVideoFrameSize = M_UCHARToULONG(G_ucCtrlData[6], G_ucCtrlData[5], G_ucCtrlData[4], G_ucCtrlData[3]);
	VSVideoStillProbeCommitControl.dwMaxPayloadTransferSize = M_UCHARToULONG(G_ucCtrlData[10], G_ucCtrlData[9], G_ucCtrlData[8], G_ucCtrlData[7]);

	CLSREQ_GetVideoParameters(1);
}
#endif

/**
* @fn        void CLSREQ_UVCPostOutDataPhase(void)
* @brief     post-process data of complete out data phase
* @param     NONE
* @retval    NONE
* @see       NULL
* @author    cx
* @since     2010-5-14
* @todo      N/A
* @bug       N/A
*/
void CLSREQ_UVCPostOutDataPhase(void)
{
	//printf("uvc Out: t=0x%bx, r=0x%bx, v=0x%x, i=0x%x, l=0x%x\n", 
	//	G_pCtrlCommand->bmRequestType, G_pCtrlCommand->bRequest, G_pCtrlCommand->wValue, G_pCtrlCommand->wIndex, G_pCtrlCommand->wLength);

	if( G_pCtrlCommand->bmRequestType == 0x21 )		// 0x21: Class OUT data to interface (Set flag)
	{
		if( G_pCtrlCommand->bRequest == SET_CUR )	// SET_CUR:0x01
		{
			if( G_pCtrlCommand->wIndex == VC_STREAMING_INTERFACE )
			{
				switch( (G_pCtrlCommand->wValue&0xff00)>>8 )
				{
				case VS_PROBE_CONTROL:
					// Get Host set Frame Format Index & Frame Size Index
					CLSREQ_GetVSVideoProbeCommitControl();
					G_ucProbeControlFlag = 2;
					break;
				case VS_COMMIT_CONTROL:
					G_ucCommitControlFlag = 1;		// Access command, wait for IN response
					G_ucProbeControlFlag = 3;
					CLSREQ_GetVSVideoProbeCommitControl();
					break;
#if STILL_SNAP_ON
				case VS_STILL_PROBE_CONTROL:	//0x03
					CLSREQ_SetVSVideoStillProbeCommitControl();
					G_ucStillProbeControlFlag = 2;
					break;
				case VS_STILL_COMMIT_CONTROL:	// 0x04
					CLSREQ_SetVSVideoStillProbeCommitControl();
					G_ucStillProbeControlFlag = 3;
					break;
				case VS_STILL_IMAGE_TRIGGER_CONTROL:	// 0x05
					G_ucStillImageTriggerFlag = 1;	// Access command, wait for IN response
					break;
				default:
					break;
#endif
				}
			}
			else if( G_pCtrlCommand->wIndex == VC_EXTENSION_UNIT )
			{
				switch( (G_pCtrlCommand->wValue&0xff00)>>8 )
				{
				case XU_SPEED_INFO_CONTROL:
					if(G_pucCtrlDataPointer[0]== 1) { //start pccam
						G_UsbPccamStart = 1;
					}
					else if(G_pucCtrlDataPointer[0]== 2) {//stop pccam
						if(G_IsOurDrvier) {
							G_UsbPccamStop = 1;
							G_IsOurDrvier = 0;
						}
					}
					break;
#if(DDX_on)
				case XU_FD_ON_OFF_FLAG:  //cx++ for  FD  on /off
					G_PCCAME_FD_ON = G_pucCtrlDataPointer[0];
					break;

				case XU_FD_VALUE:

					break;
#endif
#if STILL_SNAP_ON
				case XU_SET_Q_TALBE:
					if((G_pucCtrlDataPointer[0]>0)&&(G_pucCtrlDataPointer[0]<100)){
						snapMainQIdx = G_pucCtrlDataPointer[0];
					}
					//INTR_putchar('Q');
				break;	
#endif
				default:
					uvcUserExtUnitPostOut();
					break;	

				}
			}
			else if( G_pCtrlCommand->wIndex == VC_PROCESSING_UNIT )
			{
				G_ucPccamTuningFlag = 1;
				G_wValueLock = G_pCtrlCommand->wValue;
			}
#if (K_AUDIOINTER)
			else if( G_pCtrlCommand->wIndex == 0x0503/*AC_FEATURE_UNIT */)
			{
				switch( (G_pCtrlCommand->wValue&0xff00)>>8 )
				{
				case 0x01://MUTE_CONTROL:
					// INTR_putchar('C');
					break;

				case 0x02://VOLUME_CONTROL:
					// INTR_putchar('D');
					break;

					//default:
					// Stall
					// break;
				}
			}
#endif
		}
	}
#if (K_AUDIOINTER)
	else if( G_pCtrlCommand->bmRequestType == 0x22 )		// Class out to Endpoint
	{
		if( G_pCtrlCommand->bRequest == 0x01/*SET_CUR*/ ) 	// SET_CUR:0x01
		{
			if( G_pCtrlCommand->wIndex == 0x0086/*AC_ISOIN_ENDPOINT */)	//audio in endpoint
			{
				switch( (G_pCtrlCommand->wValue&0xff00)>>8 )
				{
				case 0x01://AS_SAMPLING_FREQ_CONTROL:			// 0x01
					G_USBAudioSetInterface=1;
					//INTR_putchar('A');
					break;

				}
			}
		}
	}
#endif
}

/**
* @fn        void CLSREQ_UVCPreInDataPhase(void)
* @brief     pre-process setting of following in data phase
* @param     NONE
* @retval    NONE
* @see       NULL
* @author    cx
* @since     2010-5-14
* @todo      N/A
* @bug       N/A
*/
void CLSREQ_UVCPreInDataPhase(void)
{
	UINT16 sizeAvailable = 0;
	UINT16 temp;
	///  G_ucCtrlPacketIndex = 0;	//initialize packet index
	G_pucCtrlDataPointer =  G_ucCtrlData;

	//printf("uvc In: t=0x%bx, r=0x%bx, v=0x%x, i=0x%x, l=0x%x\n", 
	//	G_pCtrlCommand->bmRequestType, G_pCtrlCommand->bRequest, G_pCtrlCommand->wValue, G_pCtrlCommand->wIndex, G_pCtrlCommand->wLength);

	// Check Video Class
	if( G_pCtrlCommand->bmRequestType == 0xA1 )		// 0xA1: Class IN data(Get flag)
	{
		if( G_pCtrlCommand->wIndex == VC_STREAMING_INTERFACE )
		{
			switch( (G_pCtrlCommand->wValue&0xff00)>>8 )
			{
			case VS_PROBE_CONTROL:			// VS_PROBE_CONTROL:0x01
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					CLSREQ_SetVSVideoProbeCommitControl();
					G_ucProbeControlFlag = 1;
					sizeAvailable = VS_VIDEOPROBE_COMMITCONTROL_REQUEST_SIZE;
					break;
				case GET_MIN:
					CLSREQ_SetVSVideoProbeCommitControl();
					G_ucCtrlData[12] = 2;
					G_ucCtrlData[13] = 0;
					//G_ucProbeControlFlag = 1;
					sizeAvailable = VS_VIDEOPROBE_COMMITCONTROL_REQUEST_SIZE;
					break;
				case GET_MAX:
					CLSREQ_SetVSVideoProbeCommitControl();
					G_ucCtrlData[12] = 61;
					G_ucCtrlData[13] = 0;
					//G_ucProbeControlFlag = 1;
					sizeAvailable = VS_VIDEOPROBE_COMMITCONTROL_REQUEST_SIZE;
					break;
				case GET_DEF:
					CLSREQ_SetVSVideoProbeCommitControl();
					if (G_bUSBSpeed) {
						G_ucCtrlData[12] = 61;
						G_ucCtrlData[13] = 0;
					}
					else {
						G_ucCtrlData[12] = 13;
						G_ucCtrlData[13] = 0;
					}
					//G_ucProbeControlFlag = 1;
					sizeAvailable = VS_VIDEOPROBE_COMMITCONTROL_REQUEST_SIZE;
					break;
				case GET_RES:
					CLSREQ_SetVSVideoProbeCommitControl();
					G_ucCtrlData[12] = 1;
					G_ucCtrlData[13] = 0;
					//G_ucProbeControlFlag = 1;
					sizeAvailable = VS_VIDEOPROBE_COMMITCONTROL_REQUEST_SIZE;
					break;
				case GET_LEN:
					//CLSREQ_SetVSVideoProbeCommitControl();
					G_ucCtrlData[0] = 26;
					G_ucCtrlData[1] = 0;
					//G_ucProbeControlFlag = 1;
					sizeAvailable = 2;
					break;
				default:
					// STALL
					break;
				}
				break;
#if STILL_SNAP_ON
			case VS_STILL_PROBE_CONTROL:	// VS_STILL_PROBE_CONTROL:0x03
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					CLSREQ_GetVSVideoStillProbeCommitControl();
					G_ucStillProbeControlFlag = 1;
					sizeAvailable = VS_VIDEOSTILLPROBE_COMMITCONTROL_REQUEST_SIZE;
					break;
				default:
					break;
				}
				break;
#endif
			}
		}
		else if( G_pCtrlCommand->wIndex == VC_EXTENSION_UNIT )
		{
			switch( (G_pCtrlCommand->wValue&0xff00)>>8 )
			{
			case  XU_SPEED_INFO_CONTROL	:
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					G_ucCtrlData[0] = G_bUSBSpeed;
					G_IsOurDrvier = 1; //our driver
					sizeAvailable = 2;
					break;
				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_MAX:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_INFO:
					G_ucCtrlData[0] = 3;
					sizeAvailable = 1;
					break;
				case GET_DEF:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_LEN:
					G_ucCtrlData[0] = 0x02;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				}
				break;
#if(DDX_on)
			case  XU_FD_ON_OFF_FLAG:
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					G_ucCtrlData[0] = G_PCCAME_FD_ON;
					sizeAvailable = 1;
					break;
				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_MAX:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_INFO:
					G_ucCtrlData[0] = 3;
					sizeAvailable = 1;
					break;
				case GET_DEF:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_LEN:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				}
				break;
			case XU_FD_VALUE:  //cx++ get FD information
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					if((G_PCCAME_FD_ON==2 )||(G_PCCAME_FD_ON==1))
					{
						G_ucCtrlData[0] = fdKeyRtS;
						G_ucCtrlData[1] = fdKeyRtX;
						G_ucCtrlData[2] = fdKeyRtY;
					}else if((G_PCCAME_FD_ON==0)||(G_PCCAME_FD_ON==3))
					{
						G_ucCtrlData[0] =0x00;
						G_ucCtrlData[1] =0x00;
						G_ucCtrlData[2] =0x00;
					}
					//  printf("%bu,%bu,%bu\n",G_ucCtrlData[0],G_ucCtrlData[1],G_ucCtrlData[2]);
					sizeAvailable = 3;
					break;
				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					sizeAvailable = 1;
					break;
				case GET_MAX:
					G_ucCtrlData[0] = 0xFF;
					sizeAvailable = 1;
					break;
				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_INFO:
					G_ucCtrlData[0] = 3;
					sizeAvailable = 1;
					break;
				case GET_DEF:
					G_ucCtrlData[0] = 0x00;
					sizeAvailable = 1;
					break;
				case GET_LEN:
					G_ucCtrlData[0] = 0x03;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				}
				break;
#endif
			default:
				sizeAvailable = uvcUserExtUnitPreIn();
				break;
			}
		}
		else if( G_pCtrlCommand->wIndex == VC_PROCESSING_UNIT )
		{
			switch( (G_pCtrlCommand->wValue&0xff00)>>8 )
			{
			case PU_BRIGHTNESS_CONTROL:
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					G_ucCtrlData[0] = XBYTE[REG_Cdsp_Yb]+128;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_MAX:
					G_ucCtrlData[0] = 0xff;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_INFO:
					G_ucCtrlData[0] = 0x03;
					sizeAvailable = 1;
					break;
				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_DEF:
					G_ucCtrlData[0] =0x80;
					G_ucCtrlData[1] =0x00;
					sizeAvailable = 2;
					break;
				default:
					// STALL
					break;
				}
				break;
			case PU_CONTRAST_CONTROL:
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					G_ucCtrlData[0] = XBYTE[REG_Cdsp_Yc];
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_MAX:
					G_ucCtrlData[0] = 0xff;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_INFO:
					G_ucCtrlData[0] = 0x03;
					sizeAvailable = 1;
					break;
				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_DEF:
					G_ucCtrlData[0] = 0x20;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				default:
					break;
				}
				break;
			case PU_POWER_LINE_FREQUENCY_CONTROL:
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					G_ucCtrlData[0] = 0x01; //0: normal 1:50hz 2:60hz
					sizeAvailable = 1;
					break;
				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					sizeAvailable = 1;
					break;
				case GET_MAX:
					G_ucCtrlData[0] = 0x02;
					sizeAvailable = 1;
					break;
				case GET_INFO:
					G_ucCtrlData[0] = 0x03;
					sizeAvailable = 1;
					break;
				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					sizeAvailable = 1;
					break;
				case GET_DEF:
					G_ucCtrlData[0] = 0x01;
					sizeAvailable = 1;
					break;
				default:
					break;
				}
				break;
			case PU_HUE_CONTROL:
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					if( XBYTE[REG_Cdsp_huecosdata] >= 0x80 )
					{
						if( XBYTE[REG_Cdsp_huecosdata] >= 0x80 )
						{
							temp = (0x40-XBYTE[REG_Cdsp_huecosdata])*3/2+180;
							G_ucCtrlData[1] = temp/255;
							G_ucCtrlData[0] = temp - G_ucCtrlData[1]*255;
						}
						else
						{
							G_ucCtrlData[0] = (0x40-XBYTE[REG_Cdsp_huecosdata])*3/2+15;
							G_ucCtrlData[1] = 0x01;
						}
					}
					else
					{
						if( XBYTE[REG_Cdsp_huecosdata] >= 0x80 )
							G_ucCtrlData[0] = (0x40-XBYTE[REG_Cdsp_huecosdata])*3/2+90;
						else
							G_ucCtrlData[0] = (0x40-XBYTE[REG_Cdsp_huecosdata])*3/2;
						G_ucCtrlData[1] = 0x00;
					}
					sizeAvailable = 2;
					break;
				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_MAX:
					G_ucCtrlData[0] = 0x68;
					G_ucCtrlData[1] = 0x01;
					sizeAvailable = 2;
					break;
				case GET_INFO:
					G_ucCtrlData[0] = 0x03;
					sizeAvailable = 1;
					break;
				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				case GET_DEF:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;
				default:
					break;
				}
				break;
			case PU_SATURATION_CONTROL:
				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					G_ucCtrlData[0] = XBYTE[REG_Cdsp_UVsat];
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;

				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;

				case GET_MAX:
					G_ucCtrlData[0] = 0xff;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;

				case GET_INFO:
					G_ucCtrlData[0] = 0x03;
					sizeAvailable = 1;
					break;

				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;

				case GET_DEF:
					G_ucCtrlData[0] = 0x20;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;

				default:
					break;
				}

				break;

			case PU_SHARPNESS_CONTROL:

				switch( G_pCtrlCommand->bRequest )
				{
				case GET_CUR:
					G_ucCtrlData[0] = XBYTE[REG_Cdsp_ygain1];
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;

				case GET_MIN:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;

					break;

				case GET_MAX:
					G_ucCtrlData[0] = 0x80;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;

					break;

				case GET_INFO:
					G_ucCtrlData[0] = 0x03;
					sizeAvailable = 1;

					break;

				case GET_RES:
					G_ucCtrlData[0] = 0x01;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;

					break;

				case GET_DEF:
					G_ucCtrlData[0] = 0x00;
					G_ucCtrlData[1] = 0x00;
					sizeAvailable = 2;
					break;

				default:
					break;
				}

				break;
			}
		}
#if (K_AUDIOINTER)
		else if ( G_pCtrlCommand->wIndex == 0x0503/*AC_FEATURE_UNIT*/ )
		{       //  INTR_putchar('1');
			switch( (G_pCtrlCommand->wValue&0xff00)>>8 )
			{
			case 0x01://MUTE_CONTROL:
				switch( G_pCtrlCommand->bRequest ) {
			case 0x81://GET_CUR:
				G_ucCtrlData[0] = 1;
				// INTR_putchar('2');
				sizeAvailable = 1;
				break;
				}
				break;

			case 0x02://VOLUME_CONTROL:
				switch( G_pCtrlCommand->bRequest ) {
			case 0x81://GET_CUR:
				// INTR_putchar('3');
				G_ucCtrlData[0] = 0x00;//M_LoByteOfWord(G_vdRequestACFU1VolumeCur);
				G_ucCtrlData[1] = 0x18;//M_HiByteOfWord(G_vdRequestACFU1VolumeCur);
				sizeAvailable = 2;
				break;

			case 0x82://GET_MIN:
				// INTR_putchar('4');
				G_ucCtrlData[0] = 0x00;//M_LoByteOfWord(VD_REQUEST_FU1_VOLUME_MIN);
				G_ucCtrlData[1] = 0x12;//M_HiByteOfWord(VD_REQUEST_FU1_VOLUME_MIN);
				sizeAvailable = 2;
				//   G_usCtrlDataIndex =0;
				//   G_usCtrlDataLength=2;
				//  printf("us=%d\n",G_usCtrlDataLength);
				break;

			case 0x83://GET_MAX:
				// INTR_putchar('5');
				G_ucCtrlData[0] = 0x00;//M_LoByteOfWord(VD_REQUEST_FU1_VOLUME_MAX);
				G_ucCtrlData[1] = 0x1e;//M_HiByteOfWord(VD_REQUEST_FU1_VOLUME_MAX);
				sizeAvailable = 2;
				break;

			case 0x84://GET_RES:
				// INTR_putchar('6');
				G_ucCtrlData[0] =0x01;// M_LoByteOfWord(VD_REQUEST_FU1_VOLUME_RES);
				G_ucCtrlData[1] =0x00;// M_HiByteOfWord(VD_REQUEST_FU1_VOLUME_RES);
				sizeAvailable = 2;
				break;

				}
				break;

			}
		}
#endif
	}
	if (sizeAvailable < G_usCtrlDataLength) {
		G_usCtrlDataLength = sizeAvailable;
	}
}

void sp1kUvcUserExtUnitPreInCallBackSet(usbcbVNDRFunc_t pfun)
{
	pFun_uvcUserExtUnitPreIn = pfun; 
}

void sp1kUvcUserExtUnitPostOutCallBackSet(usbcbVNDRFunc_t pfun)
{
	pFun_uvcUserExtUnitPostOut = pfun; 
}

UINT16 uvcUserExtUnitPostOut(void)
{
	pusbVNDCb_t pCb = &uvcUserExtUnitCb;

	uvcUserExtUnitCb.cmd.bmRequestType = G_pCtrlCommand->bmRequestType;
	uvcUserExtUnitCb.cmd.bRequest= G_pCtrlCommand->bRequest;
	uvcUserExtUnitCb.cmd.wValue= G_pCtrlCommand->wValue;
	uvcUserExtUnitCb.cmd.wIndex = G_pCtrlCommand->wIndex;
	uvcUserExtUnitCb.cmd.wLength= G_pCtrlCommand->wLength;
	uvcUserExtUnitCb.pData = G_pucCtrlDataPointer;

	if(pFun_uvcUserExtUnitPostOut){
		return pFun_uvcUserExtUnitPostOut(pCb);
	}

	return 0;
}

UINT16 uvcUserExtUnitPreIn(void)
{
	pusbVNDCb_t pCb = &uvcUserExtUnitCb;

	uvcUserExtUnitCb.cmd.bmRequestType = G_pCtrlCommand->bmRequestType;
	uvcUserExtUnitCb.cmd.bRequest= G_pCtrlCommand->bRequest;
	uvcUserExtUnitCb.cmd.wValue= G_pCtrlCommand->wValue;
	uvcUserExtUnitCb.cmd.wIndex = G_pCtrlCommand->wIndex;
	uvcUserExtUnitCb.cmd.wLength= G_pCtrlCommand->wLength;
	uvcUserExtUnitCb.pData = G_pucCtrlDataPointer;
		
	if(pFun_uvcUserExtUnitPreIn){
		return pFun_uvcUserExtUnitPreIn(pCb);
	}

	return 0;
}


