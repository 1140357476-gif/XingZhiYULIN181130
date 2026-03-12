#ifndef TIMELAPSE__H__
#define TIMELAPSE__H__

typedef enum TimeLapseParam_e {
	TIMELAPSE_PAR_FPS,
	TIMELAPSE_PAR_SNAPGAP,	
	TIMELAPSE_PAR_MAX,
} TimeLapseParam_t;

typedef enum {
	TIMELAPSE_SUCCESS =0,
	TIMELAPSE_FAIL =1,
	TIMELAPSE_CARDFULL,
	TIMELAPSE_MAX_FILESIZE,
}TIMELAPSE_STS_t;

UINT8 timelapse_ParamSet(UINT8 paraId, UINT16 value);
UINT16 timelapse_ParamGet(UINT8 paraId);
UINT8 timelapse_Entry(UINT32 dramAddrW, UINT16 width, UINT16 height,UINT32 vlcSize);
UINT8 timelapse_AppendFrame(UINT32 startBufAddrW, UINT32 dataLength );
void timelapse_AddFrameHdr(UINT32 addr,UINT16 width,UINT16 height,UINT8 qIdx);
#endif

