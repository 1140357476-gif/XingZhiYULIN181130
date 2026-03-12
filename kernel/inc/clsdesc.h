#ifndef _clsdesc_h_
#define _clsdesc_h_

#include "class.h"
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------

//=============================================================================
//Variable
//=============================================================================
typedef struct _VIDEO_PROBE_COMMIT_CONTROL
{
        UINT16	bmHint;
        UINT8	bFormatIndex;
        UINT8	bFrameIndex;
        UINT32	dwFrameInterval;
        UINT16	wKeyFrameRate;
        UINT16	wPFrameRate;
        UINT16	wCompQuality;
        UINT16	wCompWindowSize;
        UINT16	wDelay;
        UINT32	dwMaxVideoFrameSize;
        UINT32	dwMaxPayloadTransferSize; 
} VIDEO_PROBE_COMMIT_CONTROL, *PVIDEO_PROBE_COMMIT_CONTROL;

#if STILL_SNAP_ON
typedef struct _VIDEO_STILL_PROBE_COMMIT_CONTROL
{
        UINT8	bFormatIndex;
        UINT8	bFrameIndex;
        UINT8	bCompressionIndex;
        UINT32	dwMaxVideoFrameSize;
        UINT32	dwMaxPayloadTransferSize;
} VIDEO_STILL_PROBE_COMMIT_CONTROL;
#endif

xdata	VIDEO_PROBE_COMMIT_CONTROL	VSVideoProbeCommitControl=

//code	 VIDEO_PROBE_COMMIT_CONTROL	VSVideoProbeCommitControlInit =
{
		0x0000,          				/*bmHint: Bitfield control indicating to the function what parameter shall be kept fixed */
        VC_VIDEO_DEFAULT_DATA_FORMAT,	/*bFormatIndex: Video format index from a format descriptor */
        VC_VIDEO_DEFAULT_FRAME_SIZE,	/*bFrameIndex: Video frame index from a frame descriptor */
        VC_VIDEO_DEFAULT_FRAME_INTERVAL,/*dwFrameInterval: Frame interval in 100 ns units */
        0x0000,          				/*wKeyFrameRate: Key frame rate in key frame/video frame units */
        0x0000,          				/*wPFrameRate: Pframe rate in Pframe/video frame units */
        0x001f,          				/*wCompQuality: Compression quality control in abstract units 0 to 10000 */
        0x0000,          				/*wCompWindowSize: Window size for average bit rate control */
        0x0000,          				/*wDelay: Internal video streaming interface latency in ms from video data captureto presentation on the USB */
        0x00000000, 					/*dwMaxVideoFrameSize: Maximum video frame size in bytes */
        0x00000000, 					/*dwMaxPayloadTransferSize: Specifies the maximum number of bytes that the device can transmit or receive in single payload transder */
};

#if STILL_SNAP_ON
xdata	VIDEO_STILL_PROBE_COMMIT_CONTROL	VSVideoStillProbeCommitControl;

code	VIDEO_STILL_PROBE_COMMIT_CONTROL	VSVideoStillProbeCommitControlInit =
{
        VC_VIDEO_DEFAULT_DATA_FORMAT,	/*bFormatIndex: Video format index from a format descriptor */
        VC_VIDEO_DEFAULT_FRAME_SIZE,	/*bFrameIndex: Video frame index from a frame descriptor */
	    0x01,							/*bCompressionIndex */
        0x00000000, 					/*dwMaxVideoFrameSize: Maximum video frame size in bytes */
        0x00000000, 					/*dwMaxPayloadTransferSize: Specifies the maximum number of bytes that the device can transmit or receive in single payload transder */
};
#endif

#endif /*_clsdesc_h_*/

