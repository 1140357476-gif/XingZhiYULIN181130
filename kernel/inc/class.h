#ifndef _CLASS_H_
#define _CLASS_H_

/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Abstract:

        Header file related to USB control routine

Environment:

        Keil C51 Compiler

Revision History:

        09/21/2009		cx	created

--*/

//
//	Class related definition
//

					/***** Video/Audio Class-Specific ******/
// Video/Audio Interface Class Code
#ifdef GENERAL_CLASS
	#define CC_VIDEO						0x00
	#define CC_AUDIO						0x00
#else
	#define CC_VIDEO						0x0E
	#define CC_AUDIO						0x01
#endif

// Video/Audio Interface Subclass Codes
#ifdef GENERAL_CLASS
        #define SC_VIDEOCONTROL			       0x00
        #define SC_VIDEOSTREAMING				0x00
	#define SC_VIDEO_INTERFACE_COLLETION	0x00
#else
        #define SC_VIDEOCONTROL				0x01
        #define SC_VIDEOSTREAMING				0x02
	#define SC_VIDEO_INTERFACE_COLLETION	0x03
#endif

#define K_AUDIOINTER	1
#define DDX_on   0 //for ddx demo tool
#define STILL_SNAP_ON 1

/* Video Class Interface Number */
#define VC_CONTROL_INTERFACE			0x0000
#define VC_STREAMING_INTERFACE			0x0001
#define VC_PROCESSING_UNIT				0x0200
#define VC_EXTENSION_UNIT				0x0300

/* Video Streaming Interface Control Selectors */
#define VS_CONTROL_UNDEFINED					0x00
#define VS_PROBE_CONTROL						0x01
#define VS_COMMIT_CONTROL						0x02
#define VS_STILL_PROBE_CONTROL					0x03
#define VS_STILL_COMMIT_CONTROL					0x04

#if STILL_SNAP_ON
#define VS_STILL_IMAGE_TRIGGER_CONTROL          0x05
#endif

/* Video Class-Specific Request Codes */
#define RC_UNDEFINED							0x00
#define SET_CUR									0x01
#define	SET_MIN									0x02
#define	SET_MAX									0x03
#define GET_CUR									0x81
#define GET_MIN									0x82
#define GET_MAX									0x83
#define GET_RES									0x84
#define GET_LEN									0x85
#define GET_INFO								0x86
#define GET_DEF									0x87


/* Video Class Request size */
#define VS_VIDEOPROBE_COMMITCONTROL_REQUEST_SIZE				0x001a
#define VS_VIDEOSTILLPROBE_COMMITCONTROL_REQUEST_SIZE              0x000b

/********** Streaming Interface Constant *********/
/////////////////////////////////////////////////////////////////////////////////
// Video Class-Specific Descriptor Types
#define CS_INTERFACE					0x24

// Video Class-Specific VS Interface Descriptor SubTypes
#define VS_STILL_IMAGE_FRAME	 		0x03
#define VS_FORMAT_UNCOMPRESSED			0x04
#define VS_FRAME_UNCOMPRESSED			0x05
#define VS_FORMAT_MJPEG					0x06
#define VS_FRAME_MJPEG					0x07
#define	VS_COLORFORMAT					0x0D
#define VS_FORMAT_VENDOR				0x0E
#define VS_FRAME_VENDOR					0x0F

// Video Stream Descriptor Format Index Definition
// Scalable
#define	VC_FORMAT_INDEX_UNDEFINED					0x00
#define	VC_FORMAT_INDEX_MJPEG_YUV422				0x01
#define	VC_FORMAT_INDEX_UNCOMPRESSED_YUY2			0x02
//#define	VC_FORMAT_INDEX_RAW_BAYER_PATTERN_10_BIT	0x03
//#define	VC_FORMAT_INDEX_MPEG2						0x03
#define VC_FORMAT_INDEX_RAW10						0x04

#define	VC_VIDEO_DEFAULT_DATA_FORMAT			VC_FORMAT_INDEX_MJPEG_YUV422 //VC_FORMAT_INDEX_UNCOMPRESSED_YUY2

#if STILL_SNAP_ON
/*still frame size*/
#define	VC_VIDEO_FRAME_SIZE_3456x2592				0x07  //3M
#define	VC_VIDEO_FRAME_SIZE_2592x1944				0x06  //2M
#define	VC_VIDEO_FRAME_SIZE_1024x768				0x05  //1M
#endif

/* Video frame size define */
#define	VC_VIDEO_FRAME_SIZE_160x120				0x04//0x02
//#define	VC_VIDEO_FRAME_SIZE_176x144				0x05
#define	VC_VIDEO_FRAME_SIZE_320x240				0x03//0x01
//#define	VC_VIDEO_FRAME_SIZE_352x288				0x06
#define	VC_VIDEO_FRAME_SIZE_640x480				0x01//0x03
#define	VC_VIDEO_FRAME_SIZE_1280x720			0x02//0x04

#define	VC_VIDEO_DEFAULT_FRAME_SIZE				VC_VIDEO_FRAME_SIZE_1280x720//VC_VIDEO_FRAME_SIZE_320x240

// Video Stream Descriptor Num of Frame Defintion
#define VC_NUMBER_OF_FRAME_MJPEG	  0x04
#define VC_NUMBER_OF_FRAME_UNCOMPRESSED	     0x04

// Video Stream Descriptor Default frame index
#define VC_DEFAULT_FRAME_INDEX_MJPEG					VC_VIDEO_FRAME_SIZE_320x240
#define VC_DEFAULT_FRAME_INDEX_UNCOMPRESSED		VC_VIDEO_FRAME_SIZE_320x240

/* Video frame interval define */				// Sequence is followed by descriptor describe
//#define	VC_VIDEO_FRAME_INTERVAL_UNDEFINED		0x00000000
//#define	VC_VIDEO_FRAME_INTERVAL_60FPS			0x00028B0A
#define	VC_VIDEO_FRAME_INTERVAL_30FPS			0x00051615
#define	VC_VIDEO_FRAME_INTERVAL_25FPS			0x00061A80
//#define	VC_VIDEO_FRAME_INTERVAL_20FPS			0x0007A120
//#define	VC_VIDEO_FRAME_INTERVAL_15FPS			0x000A2C2A
//#define	VC_VIDEO_FRAME_INTERVAL_12_5FPS			0x000C3500
#define	VC_VIDEO_FRAME_INTERVAL_10FPS			0x000F4240
//#define	VC_VIDEO_FRAME_INTERVAL_7_5FPS			0x00145855
//#define	VC_VIDEO_FRAME_INTERVAL_5FPS			0x001E8480

#define	VC_VIDEO_DEFAULT_FRAME_INTERVAL			VC_VIDEO_FRAME_INTERVAL_30FPS

// Video Stream Descriptor Default frame interval
#define VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG		VC_VIDEO_FRAME_INTERVAL_30FPS
#define VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED			VC_VIDEO_FRAME_INTERVAL_30FPS

#define	VC_NUMBER_OF_MJPEG_YUV422_STILL			0x06
#define  VC_NUMBER_OF_Uncompressed_YUV2_STILL  0x06

#define	VC_NUMBER_OF_MJPEG_YUV422_COMPRESS		0x04
#define	VC_NUMBER_OF_Uncompressed_YUV2_COMPRESS	0x01

/* Video compression ratio define */			// Sequence is followed by descriptor describe
#define	VC_COMPRESSION_RATIO1					0x01
#define	VC_COMPRESSION_RATIO2					0x05
#define	VC_COMPRESSION_RATIO3					0x0A
#define	VC_COMPRESSION_RATIO4					0x14

/*  Extension Unit Control Selectors */
#define	XU_SPEED_INFO_CONTROL				0x01
#if(DDX_on)
#define	XU_FD_ON_OFF_FLAG				    0x02
#define	XU_FD_VALUE				            0x03
#endif
#define XU_SET_Q_TALBE						0x04
#define   XU_LOW_LIGHT_MODE_CONTROL				0x06

/* Processing Unit Control Selectors */
#define PU_BRIGHTNESS_CONTROL						0x02
#define PU_CONTRAST_CONTROL							0x03
#define PU_GAIN_CONTROL								0x04
#define PU_POWER_LINE_FREQUENCY_CONTROL				0x05
#define PU_HUE_CONTROL								0x06
#define PU_SATURATION_CONTROL						0x07
#define PU_SHARPNESS_CONTROL						0x08
//=============================================================================
//Variable
//=============================================================================

#endif
