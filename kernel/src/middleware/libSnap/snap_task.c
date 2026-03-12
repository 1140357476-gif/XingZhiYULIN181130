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
 * @file      snap_task.c
 * @brief     The main file of snap flow
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_jpeg.h"
//#include "dosusr.h"
#include "hal_global.h"
#include "hal.h"

#include "sp1k_cdsp_api.h"
#include "front_logi.h"
#include "pv_task.h"
#include "doslink.h"
#include "disp_api.h"
#include "exiffunc.h"
#include "initio.h"
#include "cardlink.h"
#include "app_still_view.h"
#include "app_video_view.h"
#include "icat_stub.h"
#include "os_msg.h"
#include "app_ui_osd.h"
#include "app_ui_osd.h"
#include "fileindex.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "sp1k_pv_api.h"
#include "sp1k_ae_api.h"
#include "awb_api.h"//wangjg++
#include "sp1k_util_api.h"
#include "util_api.h"
#include "dos32.h"   //eric@20080502
#include "solution.h"  //eric@20080502
#include "key_scan.h"  //phil
#include "sp1k_calibration_api.h"
#include "pb_general.h"
#include "sd_def.h"
#include "sdfunc.h"
#include "dpf_scan.h"
#include "uiflow.h"
#include "dbg_def.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "hal_cdsp_core.h"
#include "sp1k_snap_api.h"
#include "doslink.h"
#include "Hal_jpeg.h"
#include "snap_task.h"
#include "snap_core.h"
#include "snap_qtable.h"
#include "cmd_func_FPGA.h"
#include "Sdramhw.h"
#include "Asicreg.h"
#include "Hal_dram.h"
#include "cdsp_misc.h"
#include "cdsp_init.h"
#include "reg_def.h"
#include "cdsp_task.h"
#include "hal_disp.h"
#include "hal_usb.h"
#include "hal_front.h"
#include "hal_gprm.h"
#include "utility.h"
#include "vfs_api.h"
//#include "exifhdr.h"

#include "snap_debug.h"
#include "dcf_api.h"
#include "os_api.h"
#include "date_stamp.h"
#include "sp1k_hal_api.h"
#include "awb_api.h"
#include "lsc_cal.h"
#include "sp1k_fd_api.h"
#include "Sp1k_gpio_api.h"
#include "motion_det.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_aud_api.h"
#include "gprm.h"
#include "stamp.h"
#include "gpio_api.h"
#include "exif_entry.h"
#include "timelapse.h"
#include "video_clip.h"
#include "class.h"
#include "panorama_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
//#define PV_RAW_DATA

#undef CDSP_JPEG_VLC_FLOW_CONTROL
#undef SNAP_TEMP_BURST_TEST
#undef SNAP_FILL_3A_INFO_TO_HEADER
#undef DRAM_8MB_10_BIT_RAW
#undef SNAP_SKIP_JPEG_ENCODE

//#define SNAP_CDSP_REG_BACKUP

//#define SNAP_TIME_MEASURE
#define SNAP_FILL_3A_INFO_TO_HEADER

#ifdef DRAM_8MB_10_BIT_RAW
	#define SNAP_SKIP_JPEG_ENCODE
#endif


#define __FILE	__FILE_ID_SNAP_TASK__


#define SNAP_ONE_YUV_LINES ( (UINT16)1944 ) //1944 )    /*It's for compare answer of FPGA */

#define SNAP_MAX_SHUTTER_SOUND_SIZE ( 16 * 1024 )

#define SNAP_RAW_BUF_MARGIN_LINES ( 0 )
#define SNAP_JPEG_CURRENT_Y_MARGIN ( 0 )

#define SNAP_WRITE_FILE_TIME ( 250 )
#define SNAP_BACK_PV_OVERHEAD ( 190 )



/*  photo frame  */

//#define SNAP_PH_W_SIZE ( 320 )
//#define SNAP_PH_H_SIZE ( 240 )
#define SNAP_PH_SCALING_MORE_Y ( (UINT16)4 )

// memory define

#define SNAP_JPEG_READ_IN_ADVANCE ( 10 )

/* temp */
#if 0
	//#define SNAP_JPEG_HDR_SIZE ( 0x266 )
	#define SNAP_JPEG_HDR_SIZE ( 0x270 )     /* will revise */
#else
	#define SNAP_JPEG_HDR_SIZE ( (UINT32)65536 )
#endif

#define SNAP_CDSP_REG_SIZE ( 0x200 )

#define SNAP_3A_INFO_SIZE 	(UINT32)4096
#define SNAP_3A_INFO_OFFSET (UINT32)600

#define SNAP_MAIN_Q_ID_STEP ( 2 )
#define SNAP_QV_Q_ID_STEP ( 1 )


// FIXME: define 648 and 480
#define SNAP_BUF_BETWEEN_PH_AND_AD_DS ( (UINT32)640 * 480 * 2 )

#define MAX_G_TIME_MS  (UINT32)(0xffffffff/100)

#define SNAP_MAX_DEBUG_TIME_ENTRY	( 32 )

#if STILL_SNAP_ON
#define SNAP_TASK_SUCCESS 0
#define SNAP_TASK_FAIL    1
#endif

#define SNAP_TASK_TAG_TODO


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define HighByte(x) 	(unsigned char) ((x) >> 8)
#define LowByte(x)  	(unsigned char) (x)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
//xdata UINT32 snapDramMByteSize;
xdata UINT32 snapMemAddr;
xdata UINT32 snapMemEndAddr;

xdata UINT32 snapShutterSoundAddr;

xdata UINT32 snapJpegHeaderAddr;
xdata UINT32 snapJpegVlcAddr;
xdata UINT32 snapJpegVlcEndAddr;

xdata UINT32 snapBurstJpegVlcAddr[SNAP_DOFLOW_BURST_NUMBER];
xdata UINT32 snapBurstMainVlcSize[SNAP_DOFLOW_BURST_NUMBER];
xdata UINT8  snapBurstVlcOverflow;


xdata UINT32 snapRawDataAddr;
xdata UINT32 snapYuv1Addr;
xdata UINT32 snapYuv2Addr;

xdata UINT32 snapPostYuvAddr;
xdata UINT32 snapQvSmallDefQvBufAddr;
xdata UINT32 snapQvAddr;
xdata UINT32 snapQvVlcAddr; // for BRC pre-encode
xdata UINT32 snapThbAddr;
xdata UINT32 snapThbVlcAddr;
xdata UINT32 snapQvShowAddr;
xdata UINT32 snapQvEffectAddr;
xdata UINT32 snapQvRotAddr;

// photo frame
xdata UINT8 snapPhotoFrmOpt;
xdata UINT16 snapPhHSize;
xdata UINT16 snapPhVSize;
xdata UINT32 snapPhotoFrmYuvAddr;
xdata UINT32 snapPhotoFrmYuvScalingAddr;
xdata UINT32 snapPhotoFrmYuvScalingTempAddr;
xdata UINT16 snapPhotoFrmYuvScalingBufYSize;
xdata UINT16 snapPhotoFrmLastNum;

#if FUNC_KNL_FRM
UINT16 xdata snapPhotoFrmRscId;
UINT32 xdata snapPhotoFrmRscAddr;
#endif



xdata UINT32 snapCdspYuvToYuvBufAddr0;
xdata UINT32 snapCdspYuvToYuvBufAddr1;

xdata UINT32 snapQvStampFontPtnAddr;
xdata UINT32 snapMainStampFontPtnAddr;

xdata UINT32 snapMainDateStampTempAddr;

xdata UINT32 snapQvDateStampAddr;
xdata UINT32 snapMainDateStampAddr;

/* for snap do front */
xdata UINT32 snapVlcDecodeAddr;

// size
xdata UINT16 snapSensorHSize;
xdata UINT16 snapSensorVSize;
xdata UINT16 snapSensorCropHSize;
xdata UINT16 snapSensorCropVSize;
xdata UINT16 snapRawHSize;
xdata UINT16 snapRawVSize;
xdata UINT16 snapYuvHSize;
xdata UINT16 snapYuvVSize;
xdata UINT16 snapHSize;
xdata UINT16 snapVSize;
xdata UINT16 snapHFactor = 100;
xdata UINT16 snapVFactor = 100;
xdata UINT16 snapQvYuvBufHSize;
xdata UINT16 snapQvYuvBufVSize;
xdata UINT16 snapQvHSize;
xdata UINT16 snapQvVSize;
xdata UINT16 snapDsQvHSize;
xdata UINT16 snapDsQvVSize;



xdata UINT16 snapYuvLines;
xdata UINT8 GmountCard;

xdata UINT16 snapVlcDecodeHSize;
xdata UINT16 snapVlcDecodeVSize;

xdata UINT8 snapPbScaleRatio;

#ifdef DPF_SCANNER
	xdata UINT32 snapHdrSize;
	xdata UINT32 snapMainVlcSize;
#else
	static xdata UINT32 snapHdrSize;
	//static xdata UINT32 snapMainVlcSize;
	xdata UINT32 snapMainVlcSize;	/* for debug, exif broken */
#endif

xdata UINT16 snapYuvToYuvVSize;

// control
xdata UINT8 snapYuvABBufOpt;
xdata UINT8 snapBurstOpt;
xdata UINT8 snapRatioOpt;//0:4:3, 1:16:9
xdata UINT8 snapQvSmallDefQvSizeOpt;//0:QV yuv >= 320 * 240, 1:QV yuv < 320*240
xdata UINT16 snapQvSmallDefQvSizeHsize;
xdata UINT16 snapQvSmallDefQvSizeVsize;
xdata UINT8 snapNoQvThbOpt;
xdata UINT8 snapDoFlowBurstStoreFirst;

//Panorama
xdata UINT8 snapPanoramaOpt;


// misc
xdata UINT32 snapQvVlcSize;
xdata UINT32 snapThbVlcSize;
xdata UINT8 snapQuality;
xdata UINT32 snapDiskFullThd;
xdata UINT8 snapAeMode;
xdata UINT8 snapAwbMode;

xdata UINT8 snapQvQIdx;
xdata UINT8 snapThbQIdx;

xdata UINT8 snapMainRuningQIdx;
xdata UINT8 snapQvRuningQIdx;
xdata UINT8 snapThbRuningQIdx;

xdata UINT8 snapSuperFineCompressRatio;
xdata UINT8 snapFineCompressRatio;
xdata UINT8 snapNormalCompressRatio;

// cdsp
xdata UINT8 snapBufNum;
xdata UINT8 snapBurstCnt;
xdata UINT8 snapRawMode;

// QV time
static xdata UINT32 snapQvShowTime;
static xdata UINT8 snapBackToPv;

static xdata UINT16 snapQvXFactor0;
static xdata UINT16 snapQvYFactor0;
static xdata UINT16 snapQvXFactor1;
static xdata UINT16 snapQvYFactor1;


// Stamp
static xdata snapStampOpt;
static xdata UINT32 snapStampDateQvXOffSet[STAMP_TYPE_MAX],snapStampDateQvYOffSet[STAMP_TYPE_MAX];
static xdata UINT32 snapStampDateMainXOffSet[STAMP_TYPE_MAX],snapStampDateMainYOffSet[STAMP_TYPE_MAX];
static xdata UINT32 snapStampDateQvColorKey[STAMP_TYPE_MAX];
static xdata UINT8 snapStampDateMainColorKey[STAMP_TYPE_MAX];

// Date Stamp
static dateStc_t snapDSInfo[STAMP_RTC_NUMBER];
static xdata UINT8 snapDateStampOpt;
static xdata UINT16 snapDateStampQvXSize;
static xdata UINT16 snapDateStampQvYSize;
static xdata UINT16 snapDateStampMainXSize;
static xdata UINT16 snapDateStampMainYSize;


static xdata UINT16 snapStampQvXPos;
static xdata UINT16 snapStampQvYPos;
static xdata UINT16 snapStampMainXPos;
static xdata UINT16 snapStampMainYPos;
static xdata UINT16 snapStampXRatio;
static xdata UINT16 snapStampYRatio;


//static xdata UINT8 snapDateStampFlag;
//static xdata UINT16 snapDateStampMainYRemainSize;
//static xdata UINT16 snapDateStampYDoOffset;

// ad DS
xdata UINT32 snapQvBgAddr;
xdata UINT32 snapMainBgTempAddr;
xdata UINT32 snapMainBgAddr;
xdata UINT32 snapBgYuvAddr;


xdata UINT16 snapBgHSize;
xdata UINT16 snapBgVSize;


static xdata UINT8 snapBgOpt;
static xdata UINT16 snapBgQvXSize;
static xdata UINT16 snapBgQvYSize;
static xdata UINT16 snapBgMainXSize;
static xdata UINT16 snapBgMainYSize;
static xdata UINT16 snapBgMainYRemainSize;

// BRC
static xdata UINT32 snapMaxTargetVlcSize;
static xdata UINT32 snapNonOverlapVlcBufSize;
static xdata UINT8 snapPostBRCFinalF = 0;

static UINT16 snapParam[ SP1K_SNAP_PARAM_MAX ];

static UINT32 snapInfo[ SNAP_INFO_MAX ];

static UINT32 snapStartTime;
#if (NORMAL_BREAK_DOWN == 1 || BURST_BREAK_DOWN == 1 || BURST_STORE_FIRST_BREAK_DOWN == 1)
static UINT32 snapMountStartTime;
static UINT32 snapMountEndTime;
static UINT32 snapRawStartTime;
static UINT32 snapRawEndTime;
static UINT32 snapLoadStampResStartTime;
static UINT32 snapLoadStampResEndTime;
static UINT32 snapDoQvYuvStartTime;
static UINT32 snapDoQvYuvEndTime;
static UINT32 snapMainStartTime;
static UINT32 snapMainEndTime;
static UINT32 snapQvEncodeStartTime;
static UINT32 snapQvEncodeEndTime;
static UINT32 snapThbStartTime;
static UINT32 snapThbEndTime;
static UINT32 snapPostEndTime;
static UINT32 snapSaveStartTime;
static UINT32 snapSaveEndTime;
static UINT32 snapStartPvTime;
#endif
static UINT32 snapQvTime;
static UINT32 snapEndPvTime;
static UINT32 snapEndTime;

#ifdef SNAP_TIME_MEASURE
	static xdata UINT16 snapDebugEntryCnt;
	static UINT32 snapDebugTime[ SNAP_MAX_DEBUG_TIME_ENTRY ];
	static xdata UINT16 snapDebugPos[ SNAP_MAX_DEBUG_TIME_ENTRY ];

	static xdata UINT32 snapStartTime;
	static xdata UINT32 snapEndTime;
	static xdata UINT32 snapPreviousTime;
#endif

#ifdef SNAP_IQ_FLOW
	xdata UINT8 snapIQLoadRaw = 1;
	xdata UINT8 snapIQLoadCdspParam = 1;
	xdata UINT8 snapIQSizeConfig = 1;

	xdata UINT16 snapIQConfigXSize = 2592;
	xdata UINT16 snapIQConfigYSize = 1944;
#endif

static UINT8 (*psnapBurst)( void *ptr );
static UINT8 (*psnapBackgroundResLoad)( void *ptr);
static UINT8 (*psnapHostStamp)( void *ptr);


static UINT16 snapTaskRawCnt = 0;
static UINT16 snapTaskYuvCnt = 0;
static UINT16 snapTaskJpgCnt = 0;
static UINT8 chgRawFileName[ 12 ];	 /* for FPGA verification */
static UINT8 cdspBinFileName[ 12 ];  /* for FPGA verification */
static UINT16 chgRawHSize; /*for FPGA verification */
static UINT16 chgRawVSize;  /*for FPGA verification */

UINT16 snapZoomFactor;


//#if defined(SNAP_DEBUG_SAVE_YUV) || defined(SNAP_IQ_FLOW) || defined(SNAP_DEBUG_SAVE_PH_YUV)
//static UINT16 snapYuvFHandler;
//static UINT8 snapYuvFName[ 13 ];
//static UINT32 snapYuvFileSize;
//#endif

/**************************************************************************
 *       I N T E R N A L    F U N C T I O N    D E C L A R A T I O N S    *
 **************************************************************************/
static void	snapTimeRec(UINT16 line);

static UINT8 snapConfigSize( UINT8 dzoomEn );
static void snapStampSizeConfig(UINT32 *qvDateStampTempSize,UINT32 *mainDateStampTempSize,UINT32 *qvDateStampSize,UINT32 *mainDateStampSize,UINT32 *bgSize,UINT32 *qvbgSize,UINT32 *mainbgTempSize,UINT32 *mainbgSize);
static void snapPhotoFrameSizeConfig(UINT32 *phYuvSize,UINT32 *phYuvScalingSize,UINT32 *phYuvScalingTempSize);

static UINT8 snapOne( void );
static UINT8 snapPostProcess( void );
static void snapJpegInit( void );
//static UINT8 snapStore( void );
static UINT8 snapYuvSave(void);
static UINT8 snapRawSave(void);
static UINT8 snapChangeRaw(void);

static void snapInitExifParam(exifJpgFileInfo_t *exifJpg);
static UINT8 snapQvToPv( UINT16 margin, UINT16 pos );
static UINT8 snapBurstToPv( UINT16 pos );
//static UINT8 snapPvTurnOn( void );
static UINT8 snapAeAwbTurnOn(void);

static UINT8 snapRawCB( UINT32 rawDataAddr, UINT16 rawHSize, UINT16 rawVSize, UINT8 rawMode );

//static void snapDateStampInit( void );
static void snapStampPreInit(void);



static void snapPvYuvSave(UINT8  rstOpt,UINT16 opt);
static UINT8 snapBlinkDetect(void);


//static UINT8 snapQvDo( UINT8 rawMode );
//static UINT8 snapCdspDo( UINT8 mode );
//static UINT8 snapCdspVScaleDo( UINT8 mode );
//static UINT32 snapCdspWaitForJpeg( UINT16 posY, UINT16 dis, UINT8 yuvABBufOpt );
//static UINT32 snapYuvBufSwitch( UINT16 cdspY, UINT16 chunkLines, UINT16 *cdspWriteOffset, UINT8 *bottom, UINT8 *Bufnum, UINT8 yuvABBufOpt );
//static UINT32 snapCdspVScaleCal( UINT16 vfactor, UINT32 vrmder, UINT16 ySize, UINT32 *tmpvini, UINT16 *vfactini, UINT16 *predosubheight, UINT16 *dosubheight );
//static UINT32 snapCdspJpegFinish( UINT8 yuvABBufOpt );


//static UINT8 snapYuvFileOpen( void );
//static UINT8 snapYuvFileClose( void );
//static UINT32 snapYuvCurAddrGet( UINT16 offset, UINT16 lines, UINT8 bottom );
//static void snapCdspJpegTrigger( UINT16 gapLines, UINT8 abBufOpt, UINT8 jpegOpt );


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//extern xdata UINT32 G_DOS_ClusterSize;

// AE AWB

extern xdata UINT8 G_AEReady;
extern xdata UINT8 G_AESetting;


// zoom
//#if (STILL_NEW_ZOOM == 0)
//extern appPvZoom_t xdata PvZoomCtrl;
//#else
extern appPvZoom_NBlocking_t xdata pvZoomNBCtrl;
//#endif

//extern UINT32 RemainPictureNO;   //will mark
extern xdata UINT8 G_calibrationMode;

extern xdata UINT16 G_USR_FileIndex;

//extern void aaaFillInfo( UINT32 addr );

extern  xdata   UINT32   G_BulkSize;
extern  xdata   UINT32   G_BulkDRAMAddr;

#if STILL_SNAP_ON
extern xdata UINT8 G_ucStillImageTriggerFlag; //cx add
extern xdata	UINT16	G_usPostJpegWidth;
extern xdata	UINT16	G_usPostJpegHeight;
#endif

extern xdata UINT16 G_JpgRstMcuNum;
extern xdata UINT8 exifVlcAtOdd;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT32 snapJpgFileSizeGet_Intr(void)
 * @brief     Get jpg file size
 * @param     NONE
 * @retval    return = jpg file size
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
snapJpgFileSizeGet_Intr(
	void
)
{
	return (snapMainVlcSize + (UINT32)snapHdrSize);
}



/**
 * @fn        void snapTimeRec(UINT16 line)
 * @brief     save system time for the measurement of snap
 * @param     [in] line : code line
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapTimeRec(
	UINT16 line
)
{
	#ifdef SNAP_TIME_MEASURE
		UINT32 time;

		if( snapDebugEntryCnt >= SNAP_MAX_DEBUG_TIME_ENTRY ) {
			printf( "<%d> overflow!\n", line );

			return;
		}

		HAL_GlobalReadGTimer(&time);

		printf( "<%d>\n", line );

		snapDebugTime[ snapDebugEntryCnt ] = time;
		snapDebugPos[ snapDebugEntryCnt ] = line;

		snapDebugEntryCnt++;
	#else
	    line=line;
	#endif
}

/**
 * @fn        void snapTimeDump(void)
 * @brief     print all of the data which the function snapTimeRec saves.
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapTimeDump(
	void
)
{
	#ifdef SNAP_TIME_MEASURE
		UINT16 i;
	    UINT32 time;    //xian ++ 20090722

		//	printf( "==Dump Time Entry==\n" );

		for( i = 0; i < snapDebugEntryCnt; i++ ) {
			//printf( "%5u: \t%5lu, \t%5lu\n", snapDebugPos[ i ], snapDebugTime[ i ], snapDebugTime[ i ] - snapDebugTime[ i - 1 ] );
	        time = snapDebugTime[ i ];
	        if(time < snapDebugTime[ i - 1 ]){
	            time += MAX_G_TIME_MS;
	        }
	        time -= snapDebugTime[ i - 1 ];
			printf( "%5u: \t%5lu, \t%5lu\n", snapDebugPos[ i ], snapDebugTime[ i ], time );
		}

		//printf( "==Total takes %lu==\n", snapDebugTime[ snapDebugEntryCnt - 1 ] - snapDebugTime[ 0 ] );
	    time = snapDebugTime[ snapDebugEntryCnt - 1 ];
	    if(time < snapDebugTime[ 0 ]){
	        time += MAX_G_TIME_MS;
	    }
	    time -= snapDebugTime[ 0 ];
		printf( "==Total takes %lu==\n", time );

	#endif
}

/**
 * @fn        UINT8 snapConfigSize(UINT8 dzoomEn)
 * @brief     Set size information for snap
 * @param     [in] dzoomEn :
 *\n					0: digital zoom disable
 *\n					1: digital zoom enable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
snapConfigSize(
	UINT8 dzoomEn
)
{
    UINT8 snapChgRawOpt;
    UINT8 snapFlowOpt;
	UINT8 snapSensorInputRatioOpt;

	zoomDevZoomFactorGet(ZOOM_LOGI_STILL_MODE,&snapZoomFactor);
	snapPrintf("zoom f=%d\n",snapZoomFactor);

	snapChgRawOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_CHANGERAW ]);
    snapFlowOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DOFLOW_OPT ]);

	#ifdef ICAT_OPTION
	if ( icatFlowModeGet() & ICAT_RAW_UPLD_MODE ) {
		frontSnapSizeGet(0, &snapSensorHSize, &snapSensorVSize);
	}
	else if ( icatStateGet() == SNAP_ICATE_SENSOR_FINISH ) {
		chgRawSizeGet(&snapSensorHSize, &snapSensorVSize);
	}
	else {
		frontSnapSizeGet(0, &snapSensorHSize, &snapSensorVSize);
	}
	#else
	if ( snapChgRawOpt ) {
		chgRawSizeGet(&snapSensorHSize, &snapSensorVSize);
	}
	else {
		#if PANORAMA_EN
		if ( snapPanoramaOpt ) {
			extern PanoramaInf_t G_PanoramaInf;
			snapSensorHSize = panoFinalWidthGet(G_PanoramaInf.param.direction);
			snapSensorVSize = panoFinalHeightGet(G_PanoramaInf.param.direction);

		}
		else
		#endif
		{
			frontSnapSizeGet(0, &snapSensorHSize, &snapSensorVSize);
		}
	}
	#endif
	snapPrintf("Sensor(H,V)(%d,%d)\n",snapSensorHSize,snapSensorVSize);

	/* check snap ratio */
	if ( !snapPanoramaOpt ) {
		if ( snapHSize * 3 > snapVSize * 4 ) {
			snapRatioOpt = 1;// 16:9
		}
		else {
			snapRatioOpt = 0;// 4:3
		}
	}
	snapPrintf("sRO=%bu\n",snapRatioOpt);

	/* check sensor input ratio */
	if ( !snapPanoramaOpt ) {
		if ( snapSensorHSize * 3 > snapSensorVSize * 4 ) {
			#if SUPPORT_QVGA_SENSOR
			snapSensorInputRatioOpt = 0;//The TI_TP5101_YUV sensor is 8:3 YUV sensor , We must scale it from 8:3 to 4:3 , else after crop the view of the image is flat
			#else
			snapSensorInputRatioOpt = 1;// 16:9
			#endif
		}
		else {
			snapSensorInputRatioOpt = 0;// 4:3
		}
	}
	snapPrintf("sSIRO=%bu\n",snapSensorInputRatioOpt);

	/* check crop size (cdsp use) */
	#ifdef ICAT_OPTION
	snapSensorCropHSize = snapSensorHSize;
	snapSensorCropVSize = snapSensorVSize;
	dzoomEn = dzoomEn;
	#else
	if ( !snapPanoramaOpt ) {
		if ( snapSensorInputRatioOpt ) {
			/*sensor input size :16:9*/
			if ( snapRatioOpt ) {
				/*snap 16:9*/
				snapSensorCropVSize = snapSensorVSize;
				snapSensorCropHSize = snapSensorHSize; /*will: keep image size is 16:3*/
			}
			else {
				/*snap 4:3*/
				snapSensorCropVSize = snapSensorVSize;
				snapSensorCropHSize = (snapSensorVSize<<2)/3; /*will: keep image size is 4:3*/
			}

			if ( dzoomEn ) {
				snapSensorCropHSize = (UINT16)( (UINT32)snapSensorCropHSize * (UINT32)100 / (UINT32)snapZoomFactor); //will  mark for FPGA Crop Verification
				snapSensorCropVSize = (UINT16)( (UINT32)snapSensorCropVSize * (UINT32)100 / (UINT32)snapZoomFactor ); //will mark for FPGA Crop Verification
			}
		}
		else {
			/*sensor input size :4:3*/

			if ( dzoomEn ) {
				snapSensorCropHSize = (UINT16)( (UINT32)snapSensorHSize * (UINT32)100 / (UINT32)snapZoomFactor ); //will  mark for FPGA Crop Verification
				snapSensorCropVSize = (UINT16)( (UINT32)snapSensorVSize * (UINT32)100 / (UINT32)snapZoomFactor ); //will mark for FPGA Crop Verification
			}

			if ( snapRatioOpt ) {
				/*snap size 16:9*/
				snapSensorCropVSize = (UINT16)((UINT32)snapSensorCropVSize * (UINT32)3 / (UINT32)4);
			}

		}
	}
	#if PANORAMA_EN
	else {
		snapSensorCropHSize = snapSensorHSize;
		snapSensorCropVSize = snapSensorVSize;
	}
	#endif
	#endif

	/* YUV H Size must be 16 alignment, V Size must be 8 alignment.*/
	snapSensorCropHSize = snapSensorCropHSize >> 4 << 4;
	snapSensorCropVSize = snapSensorCropVSize >> 3 << 3;
	snapPrintf("cdsp crop(H,V)(%d,%d)\n",snapSensorCropHSize,snapSensorCropVSize);

	#ifdef DOYUV_CROP
	snapRawHSize = snapSensorHSize; /* will: it doesn't crop here, crop at the process of do yuv */
	snapRawVSize = snapSensorVSize;
	#else
	snapRawHSize = ( snapHSize < snapSensorCropHSize ) ? snapHSize : snapSensorCropHSize;
	snapRawVSize = snapSensorCropVSize;
	#endif
	snapPrintf("cdsp raw(H,V)(%d,%d)\n",snapRawHSize,snapRawVSize);
	snapYuvHSize = (snapSensorCropHSize < snapHSize) ? snapSensorCropHSize : snapHSize;
	snapYuvVSize = (snapSensorCropVSize < snapVSize) ? snapSensorCropVSize : snapVSize;
	snapPrintf("cdsp yuv(H,V)(%d,%d)\n",snapYuvHSize,snapYuvVSize);

	/* quickview */
	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		//snapQvHSize = snapYuvHSize > (UINT16)SNAP_CDSP_QV_HSIZE ? (UINT16)SNAP_CDSP_QV_HSIZE : snapYuvHSize;
		//snapQvVSize = snapYuvVSize > (UINT16)SNAP_CDSP_QV_VSIZE ? (UINT16)SNAP_CDSP_QV_VSIZE : snapYuvVSize;
		snapQvHSize = snapYuvHSize > (UINT16)SNAP_CDSP_QV_HSIZE ? (UINT16)SNAP_CDSP_QV_HSIZE : snapYuvHSize;
		snapQvVSize = (UINT16)((UINT32)snapQvHSize*snapYuvVSize/snapYuvHSize);

	}
	else
	#endif
	{
		if ( snapRatioOpt ) {
			/* snap 16:9 */
			snapQvHSize = snapYuvHSize > (UINT16)SNAP_CDSP_QV_HSIZE_16_9 ? (UINT16)SNAP_CDSP_QV_HSIZE_16_9 : snapYuvHSize;
			snapQvVSize = snapYuvVSize > (UINT16)SNAP_CDSP_QV_VSIZE_16_9 ? (UINT16)SNAP_CDSP_QV_VSIZE_16_9 : snapYuvVSize;
		}
		else {
			/* snap 4:3 */
			snapQvHSize = snapYuvHSize > (UINT16)SNAP_CDSP_QV_HSIZE ? (UINT16)SNAP_CDSP_QV_HSIZE : snapYuvHSize;
			snapQvVSize = snapYuvVSize > (UINT16)SNAP_CDSP_QV_VSIZE ? (UINT16)SNAP_CDSP_QV_VSIZE : snapYuvVSize;
		}
		//ASSERT( ( snapRawHSize > SNAP_CDSP_QV_HSIZE ) && ( snapRawVSize > SNAP_CDSP_QV_VSIZE ), 0 );

		//printf("\033[32msnapQvHSize=%u,%u\n",snapQvHSize,snapQvVSize);
		//printf("snapYuvHSize=%u,%u\n",snapYuvHSize,snapYuvVSize);
	}
	snapPrintf("quickview yuv(H,V)(%d,%d)\n",snapQvHSize,snapQvVSize);

	snapQvSmallDefQvSizeOpt = 0;

	/* yuv buffer allocation */
	snapQvYuvBufHSize = snapQvHSize;
	snapQvYuvBufVSize = snapQvVSize;

	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		snapQvSmallDefQvSizeOpt = 0;
	}
	else
	#endif
	{
		#if SUPPORT_QVGA_SENSOR  /*Add for QV size is small than 320*240 after zoom in*/
		/* snap size 16:9 */
		if ( snapRatioOpt ) {
			if ((snapQvYuvBufHSize < SNAP_CDSP_QV_HSIZE_16_9) || (snapQvYuvBufVSize < SNAP_CDSP_QV_VSIZE_16_9)) {
				/* yuv buffer allocation */
				snapQvSmallDefQvSizeOpt = 1;
			}
		}
		else{
			if ((snapQvYuvBufHSize < SNAP_CDSP_QV_HSIZE) || (snapQvYuvBufVSize < SNAP_CDSP_QV_VSIZE)) {
				/* yuv buffer allocation */
				snapQvSmallDefQvSizeOpt = 1;
			}
		}
		#else
		/* adjust 16:9 Qv Yuv Buf allocation h,v size to 4:3 */
		if ( snapRatioOpt ) {
			#if 1
			/* snap size 16:9 */
			if ( snapQvYuvBufVSize < snapQvYuvBufHSize*3/4 ) {
				snapQvYuvBufVSize = snapQvYuvBufHSize*3/4;
				snapQvYuvBufVSize = (snapQvYuvBufVSize >> 3) << 3;
			}
			#endif

			if ((snapQvYuvBufHSize < SNAP_CDSP_QV_HSIZE) || (snapQvYuvBufVSize < SNAP_CDSP_QV_VSIZE)) {
				/* yuv buffer allocation */
				snapQvSmallDefQvSizeOpt = 1;
			}
		}
		#endif

		//printf("snapQvYuvBufHSize=%d,%d\n",snapQvYuvBufHSize,snapQvYuvBufVSize);

		if ( snapQvSmallDefQvSizeOpt ==1 ) {
			if(snapRatioOpt){
				snapQvSmallDefQvSizeHsize = SNAP_CDSP_QV_HSIZE_16_9;
				snapQvSmallDefQvSizeVsize = SNAP_CDSP_QV_VSIZE_16_9;
			}else{
				snapQvSmallDefQvSizeHsize = SNAP_CDSP_QV_HSIZE;
				snapQvSmallDefQvSizeVsize = SNAP_CDSP_QV_VSIZE;
			}
			//printf("snapQvSmallDefQvSizeHsize=%u,%u\n",snapQvSmallDefQvSizeHsize,snapQvSmallDefQvSizeVsize);
		}
		//printf("snapQvYuvBufHSize=%u,%u\n",snapQvYuvBufHSize,snapQvYuvBufVSize);

		#if 0
		/* adjust target Qv h,v size */
		if ( snapRatioOpt ) {
			/* snap size 16:9 */

			if ( snapSensorInputRatioOpt ) {
				/* sensor input size 16:9 */
				snapQvVSize = snapQvVSize;
			}
			else {
				/* sensor input size 4:3 */
				//sensor crop 16:9
				//snapQvVSize = snapQvVSize*3/4;
			}
		}
		//snapQvVSize = snapQvVSize>>4<<4; //align to 16
		#endif
	}
	snapPrintf("smallDef(En,H,V)(%bu,%d,%d)\n",snapQvSmallDefQvSizeOpt,snapQvSmallDefQvSizeHsize,snapQvSmallDefQvSizeVsize);
	snapPrintf("buf qv(H,V)(%d,%d)\n",snapQvYuvBufHSize,snapQvYuvBufVSize);

	/* Ds Qv post location */
	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		snapDsQvHSize = snapYuvHSize > (UINT16)SNAP_CDSP_DS_QV_W ? (UINT16)SNAP_CDSP_DS_QV_W : snapYuvHSize;
		snapDsQvVSize = snapYuvVSize > (UINT16)SNAP_CDSP_DS_QV_H ? (UINT16)SNAP_CDSP_DS_QV_H : snapYuvVSize;
	}
	else
	#endif
	{
		if ( snapStampOpt == 0 ) {
			snapDsQvHSize = snapQvHSize;
			snapDsQvVSize = snapQvVSize;
		}
		else {
			if ( snapRatioOpt ) {
				/* snap 16:9 */
				snapDsQvHSize = snapYuvHSize > (UINT16)SNAP_CDSP_DS_QV_W_16_9 ? (UINT16)SNAP_CDSP_DS_QV_W_16_9 : snapYuvHSize;
				snapDsQvVSize = snapYuvVSize > (UINT16)SNAP_CDSP_DS_QV_H_16_9 ? (UINT16)SNAP_CDSP_DS_QV_H_16_9 : snapYuvVSize;
			}
			else {
				/* snap 4:3 */
				snapDsQvHSize = snapYuvHSize > (UINT16)SNAP_CDSP_DS_QV_W ? (UINT16)SNAP_CDSP_DS_QV_W : snapYuvHSize;
				snapDsQvVSize = snapYuvVSize > (UINT16)SNAP_CDSP_DS_QV_H ? (UINT16)SNAP_CDSP_DS_QV_H : snapYuvVSize;
			}
		}
	}
	snapPrintf("Ds Qv(H,V)(%d,%d)\n",snapDsQvHSize,snapDsQvVSize);

	#if 0
	/* adjust Ds Qv post location */
	if ( snapRatioOpt ) {
		if ( snapSensorInputRatioOpt ) {
			/* sensor input size 16:9 */
			snapDsQvVSize = snapDsQvVSize;
		}
		else {
			/* sensor input size 4:3 */
			//sensor crop 16:9
			//snapDsQvVSize = snapDsQvVSize*3/4;
		}
	}
	#endif

	if ( !snapPanoramaOpt ) {
		if ( snapPhotoFrmOpt ) {
			snapPhHSize = snapParam[ SP1K_SNAP_PARAM_PH_WIDTH ];
			snapPhVSize = snapParam[ SP1K_SNAP_PARAM_PH_HEIGHT ];
		}
	}

	if ( !snapPanoramaOpt ) {
		if ( snapFlowOpt == CDSP_DO_FRONT ) {
			snapVlcDecodeHSize = snapHSize >> 3;//324

			if (snapVlcDecodeHSize < SNAP_CDSP_THB_HSIZE) {
				snapPbScaleRatio = ( (SNAP_CDSP_QV_HSIZE + snapVlcDecodeHSize - 1) / snapVlcDecodeHSize);
				snapVlcDecodeHSize *= snapPbScaleRatio;
			}
			else {
				snapPbScaleRatio = 1;
			}
			snapVlcDecodeVSize = snapVSize >> 3;						//243
			snapVlcDecodeHSize = (snapVlcDecodeHSize + 7) >> 3 << 3;		//328
			snapVlcDecodeVSize *= snapPbScaleRatio;
			#if 0//linjie check by jay not need this
			snapVlcDecodeVSize = (snapVlcDecodeVSize + 1) >> 1 << 1;		//244
			#endif

		}
	}

	snapPrintfSize( "zoom %d\n", (UINT16) snapZoomFactor);
	snapPrintfSize( "sensor \tH %d, \tV %d\n", snapSensorHSize, snapSensorVSize );
	snapPrintfSize( "crop \tH %d, \tV %d\n", snapSensorCropHSize, snapSensorCropVSize );
	snapPrintfSize( "raw \tH %d, \tV %d\n", snapRawHSize, snapRawVSize );
	snapPrintfSize( "yuv \tH %d, \tV %d\n", snapYuvHSize, snapYuvVSize );
	snapPrintfSize( "jpg \tH %d, \tV %d\n", snapHSize, snapVSize );
	snapPrintfSize( "DsQv \tH %d, \tV %d\n", snapDsQvHSize, snapDsQvVSize );
	snapPrintfSize( "Vlc \tH %d, \tV %d\n", snapVlcDecodeHSize, snapVlcDecodeVSize );
	snapPrintfSize( "Pb Scale Ratio \t %bd\n", snapPbScaleRatio );

	return SUCCESS;
}

/**
 * @fn        void snapPhotoFrameSizeConfig(void)
 * @brief     configure size of Photo Frame
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapPhotoFrameSizeConfig(
	UINT32 *phYuvSize,
	UINT32 *phYuvScalingSize,
	UINT32 *phYuvScalingTempSize
)
{
	//phYuvSrcLines = (UINT16)( (UINT32)snapYuvLines * (UINT32)snapPhVSize / (UINT32)snapYuvVSize );
	//snapPhotoFrmYuvScalingBufYSize = (UINT16)( (UINT32)(phYuvSrcLines + SNAP_PH_SCALING_MORE_Y ) * (UINT32)snapYuvVSize / (UINT32)snapPhVSize );
	snapPhotoFrmYuvScalingBufYSize = snapYuvLines + SNAP_PH_SCALING_MORE_Y;

	*phYuvSize = (UINT32)snapPhHSize * (UINT32)snapPhVSize * 2;

	*phYuvScalingSize = (UINT32)snapYuvHSize * (UINT32)snapPhotoFrmYuvScalingBufYSize * 2 ;

	if (snapYuvHSize > snapPhHSize) {
		*phYuvScalingTempSize = (UINT32)snapYuvHSize * (UINT32)snapPhotoFrmYuvScalingBufYSize * 2;
	}
	else {
		*phYuvScalingTempSize = (UINT32)snapPhHSize * (UINT32)snapPhotoFrmYuvScalingBufYSize * 2;
	}

	snapPrintf("PhotoFrmYuvScalingBufYSize=%d\n",snapPhotoFrmYuvScalingBufYSize);
	snapPrintf("phYuvScalingSize=%ld\n",*phYuvScalingSize);
	snapPrintf("phYuvScalingTempSize=%ld\n",*phYuvScalingTempSize);
	snapPrintf("PhHSize=%d,YuvHSize=%d\n",snapPhHSize,snapYuvHSize);
}

/**
 * @fn        void snapStampSizeConfig(void)
 * @brief     configure size of data stamp
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapStampSizeConfig(
	UINT32 *qvDateStampTempSize,
	UINT32 *mainDateStampTempSize,
	UINT32 *qvDateStampSize,
	UINT32 *mainDateStampSize,
	UINT32 *bgSize,
	UINT32 *qvbgSize,
	UINT32 *mainbgTempSize,
	UINT32 *mainbgSize
)
{
	stamp_text_t *pDate;
	stamp_text_t *pTime;
	stamp_text_t *pTxt;
	UINT8 backgroundOpt, textOpt;

	pDate = stampTextGet(STAMP_DATE);
	pTime = stampTextGet(STAMP_TIME);
	pTxt = stampTextGet(STAMP_TEXT);

	backgroundOpt = stampBackgroundEnable(0xFF);
	textOpt = pDate->enable | pTime->enable | pTxt->enable;

	snapDateStampOpt = textOpt;
	snapBgOpt = backgroundOpt;

	//printf("snapStampSizeConfig : sOpt=%bu,bgOpt=%bu\n",snapDateStampOpt,snapBgOpt);

	if ( backgroundOpt ) {
		snapBgHSize = snapParam[ SP1K_SNAP_PARAM_BACKGROUND_WIDTH ];
		snapBgVSize = snapParam[ SP1K_SNAP_PARAM_BACKGROUND_HEIGHT ];
		snapPrintfDate("Bg w=%d,h=%d\n",snapBgHSize,snapBgVSize);
	}

	/* DateStamp -> QvXSize & QvYSize */
	if ( textOpt ) {
		snapDateStampQvXSize = (UINT16)( (UINT32)SNAP_DATE_STAMP_QV_DEFAULT_H_SIZE * snapDsQvHSize  / SNAP_DATE_STAMP_QV_DEFAULT_H_REF_SIZE );
		snapDateStampQvYSize = (UINT16)( (UINT32)SNAP_DATE_STAMP_QV_DEFAULT_V_SIZE * snapDsQvVSize  / SNAP_DATE_STAMP_QV_DEFAULT_V_REF_SIZE );

		// align to 2
		snapDateStampQvXSize = (snapDateStampQvXSize + 1) >>1 <<1;
		snapDateStampQvYSize = (snapDateStampQvYSize + 1) >>1 <<1;

		snapPrintfDate("snapDateStampQvXSize=%d\n",snapDateStampQvXSize);
		snapPrintfDate("snapDateStampQvYSize=%d\n",snapDateStampQvYSize);
	}

	/* BgStamp -> QvXSize & QvYSize (based on MCU, need 8x) */
	if ( backgroundOpt ) {
		snapBgQvXSize = (UINT16)( (UINT32)snapBgHSize * snapDsQvHSize / SNAP_BG_DEFAULT_X_REF_SIZE );
		(snapBgQvXSize%8 == 0) ? snapBgQvXSize : (snapBgQvXSize += (8-(snapBgQvXSize%8)));

		snapBgQvYSize = (UINT16)( (UINT32)snapBgVSize * snapDsQvVSize / SNAP_BG_DEFAULT_Y_REF_SIZE );
		snapBgQvYSize = snapBgQvYSize >> 1 << 1;
		(snapBgQvYSize%8 == 0) ? snapBgQvYSize : (snapBgQvYSize += (8-(snapBgQvYSize%8)));

		snapPrintfDate("snapBgQvXSize=%d\n",snapBgQvXSize);
		snapPrintfDate("snapBgQvYSize=%d\n",snapBgQvYSize);
	}

	/* DateStamp -> MainXSize & MainYSize */
	if ( textOpt ) {
		snapDateStampMainXSize = (UINT16)( (UINT32)SNAP_DATE_STAMP_MAIN_DEFAULT_H_SIZE * snapHSize / SNAP_DATE_STAMP_MAIN_DEFAULT_H_REF_SIZE );
		snapDateStampMainYSize = (UINT16)( (UINT32)SNAP_DATE_STAMP_MAIN_DEFAULT_V_SIZE * snapVSize / SNAP_DATE_STAMP_MAIN_DEFAULT_V_REF_SIZE );

		// align to 2
		snapDateStampMainXSize = (snapDateStampMainXSize + 1) >>1 <<1;
		snapDateStampMainYSize = (snapDateStampMainYSize + 1) >>1 <<1;

		snapPrintfDate("snapDateStampMainXSize=%d\n",snapDateStampMainXSize);
		snapPrintfDate("snapDateStampMainYSize=%d\n",snapDateStampMainYSize);
	}

	/* BgStamp -> MainXSize & MainYSize (based on MCU, need 8x) */
	if ( backgroundOpt ) {
		snapBgMainXSize = (UINT16)( (UINT32)snapBgHSize * snapHSize / SNAP_BG_DEFAULT_X_REF_SIZE );
		(snapBgMainXSize%8 == 0) ? snapBgMainXSize : (snapBgMainXSize += (8-(snapBgMainXSize%8)));

		snapBgMainYSize = (UINT16)( (UINT32)snapBgVSize * snapVSize / SNAP_BG_DEFAULT_Y_REF_SIZE );
		snapBgMainYSize = snapBgMainYSize >> 1 << 1;
		(snapBgMainYSize%8 == 0) ? snapBgMainYSize : (snapBgMainYSize += (8-(snapBgMainYSize%8)));

		snapPrintfDate("snapBgMainXSize=%d\n",snapBgMainXSize);
		snapPrintfDate("snapBgMainYSize=%d\n",snapBgMainYSize);
	}

	if ( textOpt ) {
		*qvDateStampTempSize = (UINT32)snapDateStampQvXSize * snapDateStampQvYSize * 2 * DATE_STAMP_TEMP_NUM;
		*mainDateStampTempSize = (UINT32)snapDateStampMainXSize * snapDateStampMainYSize * 2 * DATE_STAMP_TEMP_NUM;
		*qvDateStampSize = (UINT32)snapDateStampQvXSize * snapDateStampQvYSize * 2 * DATE_STAMP_NUM;
		*mainDateStampSize = (UINT32)snapDateStampMainXSize * snapDateStampMainYSize * 2 * DATE_STAMP_NUM;

		snapPrintfDate("qvDateStampTempSize=%lu\n",*qvDateStampTempSize);
		snapPrintfDate("mainDateStampTempSize=%lu\n",*mainDateStampTempSize);
		snapPrintfDate("qvDateStampSize=%lu\n",*qvDateStampSize);
		snapPrintfDate("mainDateStampSize=%lu\n",*mainDateStampSize);
	}
	else {
		*qvDateStampTempSize = 0;
		*mainDateStampTempSize = 0;
		*qvDateStampSize = 0;
		*mainDateStampSize = 0;
	}

	if ( backgroundOpt ) {
		*bgSize = (UINT32)snapBgHSize * snapBgVSize * 2;
		*qvbgSize = (UINT32)snapBgQvXSize * snapBgQvYSize * 2 ;
		*mainbgTempSize = (UINT32)snapBgMainXSize * snapBgMainYSize * 2 ;
		*mainbgSize = (UINT32)snapHSize * snapBgMainYSize * 2 ;

		snapPrintfDate("qvDateStampTempSize=%lu\n",*bgSize);
		snapPrintfDate("qvbgSize=%lu\n",*qvbgSize);
		snapPrintfDate("mainbgTempSize=%lu\n",*mainbgTempSize);
		snapPrintfDate("mainbgSize=%lu\n",*mainbgSize);
	}
	else {
		*bgSize = 0;
		*qvbgSize = 0;
		*mainbgTempSize = 0;
		*mainbgSize = 0;
	}
}

/**
 * @fn        UINT8 snapMemInit(void)
 * @brief     allocate memory for snap
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
UINT8
snapMemInit(
	void
)
{
	UINT32 jpegHeaderSize;
	UINT32 rawDataSize;
	UINT32 yuvSize;
	UINT32 qVSize, thbSize, qVEffectSize;
	UINT32 phYuvSize;
	UINT32 phYuvScalingSize;
	UINT32 phYuvScalingTempSize;
	UINT32 shutterSoundAddr, photoFrmYuvAddr;

	UINT32 yuvRawBufMargin;  /*for one yuv buffer*/
	UINT32 yuvToYuvSize;

	UINT32 qVRotSize,qVSmallDefQvBufSize;

	UINT32 mainDateStampTempSize;
	UINT32 qvDateStampTempSize;
	UINT32 mainDateStampSize;
	UINT32 qvDateStampSize;

	UINT32 bgSize;
	UINT32 qvBgSize;
	UINT32 mainBgSize;
	UINT32 mainBgTempSize;

	UINT8 qvEffect, qvRotOpt, raw10BitOpt, dzoomEn;
	UINT8 snapFlowOpt;
	//UINT16 phYuvSrcLines;

	UINT32 cdspRegAddr;
	UINT16 cdspRegSize;

	qvEffect = (UINT8)(snapParam[ SP1K_SNAP_PARAM_QV_EFFECT ]);
	qvRotOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_QV_ROT_OPT ]);
	raw10BitOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_10_BIT_RAW_OPT ]);
    dzoomEn = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DZOOM_OPT ]);
    snapFlowOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DOFLOW_OPT ]);

	//if( postWB == 0 ) {
	//	postWB = (UINT8)(snapParam[ SP1K_SNAP_PARAM_AUTO_POST_WB_OPT ]);
	//}

	/* normal snap size config */
	snapConfigSize(dzoomEn);

	/* stamp size config */
	if( snapStampOpt == 1 ) {
		snapStampSizeConfig(&qvDateStampTempSize,
							&mainDateStampTempSize,
							&qvDateStampSize,
							&mainDateStampSize,
							&bgSize,
							&qvBgSize,
							&mainBgTempSize,
							&mainBgSize);
	}
	else {
		qvDateStampTempSize = 0;
		mainDateStampTempSize = 0;
		qvDateStampSize = 0;
		mainDateStampSize = 0;
		bgSize = 0;
		qvBgSize = 0;
		mainBgTempSize = 0;
		mainBgSize = 0;
	}

	/* Determine control flow */
	#ifdef ONE_YUVBUF
		snapYuvABBufOpt =SNAP_ONE_YUVBUF;
	#else
		snapYuvABBufOpt =SNAP_TWO_YUVBUF;
	#endif

	#ifdef ICAT_OPTION
		icatSnapYuvBufTypeGet(&snapYuvABBufOpt);
	#endif

	snapPrintf( "YUV Buf= %s \n", (snapYuvABBufOpt==SNAP_ONE_YUVBUF) ? "One Buf" : "Two Buf");

	/* Determine size */
	jpegHeaderSize = SNAP_JPEG_HDR_SIZE;

	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		rawDataSize = 0;
		yuvSize = 0;
	}
	else
	#endif
	{
		if( snapRawMode == CDSP_SNAP_8RAW ) {
			rawDataSize = (UINT32)snapRawHSize * (UINT32)( snapRawVSize + SNAP_RAW_BUF_MARGIN_LINES );
		}
		else {
			rawDataSize = (UINT32)snapRawHSize * (UINT32)( snapRawVSize + SNAP_RAW_BUF_MARGIN_LINES ) * 2;
		}

		if ( snapYuvABBufOpt == SNAP_TWO_YUVBUF ) {
			/* will: YUV Two buffer */
			if( snapRawMode == CDSP_SNAP_YUV ) {
				snapYuvLines = CDSPJOB_VSIZE;
			}
			else {
				if (snapFlowOpt== CDSP_DO_FRONT){
					snapYuvLines = CDSPJOB_VSIZE_DOFRONT;
				}
				else if ( snapPhotoFrmOpt ) {
					snapYuvLines = CDSPJOB_VSIZE << 1;	/*photo frame needs using 4 buffer mode*/
				}
				else{
					snapYuvLines = CDSPJOB_VSIZE;
				}
			}

			yuvSize = (UINT32)snapYuvHSize * (UINT32)snapYuvLines * 2;
			snapPrintf("snapYuvLines=%d\n",snapYuvLines);
		}
		else {
			/* will: YUV One buffer */
			if( snapRawMode == CDSP_SNAP_YUV || snapBgOpt == 1 ) {
				//snapYuvLines = (CDSPJOB_VSIZE >>1);	/* it becomes smaller because of memory size */
				//snapYuvLines = SNAP_ONE_YUV_LINES;
				snapYuvLines = snapYuvVSize;
			}
			else {
				//snapYuvLines = SNAP_ONE_YUV_LINES;
				snapYuvLines = snapYuvVSize;
			}

			//snapYuvChunkLines = snapYuvLines;

			//snapYuvChunkLines = ( snapYuvLines >> 1 );

			yuvSize = (UINT32)snapYuvHSize * (UINT32)snapYuvVSize * 2;
		}
	}


	#ifdef DRAM_8MB_10_BIT_RAW
		if( raw10BitOpt == 1 ) {
			snapYuvLines = snapYuvLines >> 1;
			yuvSize = yuvSize >> 1;
		}
	#endif

	/* photo frame size config */
	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		snapPhotoFrmYuvScalingBufYSize = 0;
		phYuvSize = phYuvScalingSize = phYuvScalingTempSize = 0;
	}
	else
	#endif
	{
		if ( snapPhotoFrmOpt ) {
			snapPhotoFrameSizeConfig(&phYuvSize, &phYuvScalingSize, &phYuvScalingTempSize);
		}
		else {
			snapPhotoFrmYuvScalingBufYSize = 0;
			phYuvSize = phYuvScalingSize = phYuvScalingTempSize = 0;
		}
	}

	#if 0
	// ph YUV scaling buffer used for DS QV temp buffer
	if( snapDsQvFlag == 1 ) {
		UINT32 dsQvSize = (UINT32)snapDsQvHSize * (UINT32)snapDsQvVSize * 2;

		phYuvScalingSize = phYuvScalingSize > dsQvSize ? phYuvScalingSize : dsQvSize;
	}
	#endif

	qVSize = (UINT32)snapQvYuvBufHSize * (UINT32)snapQvYuvBufVSize * 2;

	if ( snapQvSmallDefQvSizeOpt == 1 ) {
		qVSmallDefQvBufSize = (UINT32)snapQvSmallDefQvSizeHsize * (UINT32)snapQvSmallDefQvSizeVsize * 2;
	}
	else {
		qVSmallDefQvBufSize = 0;
	}

	if( qvEffect != SP1K_SNAP_QV_EFFECT_NORMAL ) {
		qVEffectSize = (UINT32)snapDsQvHSize * (UINT32)snapDsQvVSize * 2;
	}
	else {
		qVEffectSize = 0;
	}

	if( qvRotOpt != 0 ) {
		qVRotSize = (UINT32)snapQvYuvBufHSize * (UINT32)snapQvYuvBufVSize * 2;
	}
	else {
		qVRotSize = 0;
	}

	#ifdef DRAM_8MB_10_BIT_RAW
		if( raw10BitOpt == 1 ) {
			qVSize = qVEffectSize = qVRotSize = 0;
		}
	#endif

	thbSize = (UINT32)SNAP_CDSP_THB_HSIZE * (UINT32)SNAP_CDSP_THB_VSIZE * (UINT32)2;

	yuvToYuvSize = 0;


	#ifdef SNAP_CDSP_REG_BACKUP
		cdspRegSize = SNAP_CDSP_REG_SIZE * ( cdspYuvCoringTime + 3 );
	#else
		cdspRegSize = 0;
	#endif

	snapDiskFullThd = (UINT32)snapHSize * snapVSize;  // half src size


	/*******************************************************************/
	/* 						allocate memory 									     */
	/*******************************************************************/

	shutterSoundAddr = snapMemAddr;

	ASSERT( snapShutterSoundAddr == shutterSoundAddr, 0 );

	photoFrmYuvAddr = snapShutterSoundAddr + ( (UINT32)SNAP_MAX_SHUTTER_SOUND_SIZE >> 1 );
	photoFrmYuvAddr = ( photoFrmYuvAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	ASSERT( snapPhotoFrmYuvAddr == photoFrmYuvAddr, 0 );

	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		extern PanoramaInf_t G_PanoramaInf;
		snapBgYuvAddr = G_PanoramaInf.frameTmp.startBufAddr + ( phYuvSize >> 1 );
	}
	else
	#endif
	{
		snapBgYuvAddr = snapPhotoFrmYuvAddr + ( phYuvSize >> 1 );
	}
	snapBgYuvAddr = ( snapBgYuvAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	cdspRegAddr = snapBgYuvAddr + ( bgSize >> 1 );

	snapQvStampFontPtnAddr = cdspRegAddr + ( cdspRegSize >> 1 );
	snapQvStampFontPtnAddr = ( snapQvStampFontPtnAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	snapMainStampFontPtnAddr = snapQvStampFontPtnAddr + ( qvDateStampTempSize >> 1 );
	snapMainStampFontPtnAddr = ( snapMainStampFontPtnAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	snapQvDateStampAddr = snapMainStampFontPtnAddr + + ( mainDateStampTempSize >> 1 );
	snapQvDateStampAddr = ( snapQvDateStampAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	snapMainDateStampTempAddr = snapQvDateStampAddr + ( qvDateStampSize >> 1 );
	snapMainDateStampTempAddr = ( snapMainDateStampTempAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	#if (SNAP_NORMAL_STAMP_WITH_HOST_STAMP == 1)
	snapMainDateStampAddr = snapMainDateStampTempAddr + ( mainDateStampSize >> 1 );
	#else
	snapMainDateStampAddr = snapMainDateStampTempAddr;
	#endif
	snapMainDateStampAddr = ( snapMainDateStampAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	/****************/
	snapQvBgAddr = snapMainDateStampAddr + ( mainDateStampSize >> 1 );
	snapQvBgAddr = ( snapQvBgAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	snapMainBgTempAddr = snapQvBgAddr + ( qvBgSize >> 1 );
	snapMainBgTempAddr = ( snapMainBgTempAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	snapMainBgAddr = snapMainBgTempAddr + ( mainBgTempSize >> 1 );
	snapMainBgAddr = ( snapMainBgAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	/***************/

	snapQvSmallDefQvBufAddr = snapMainBgAddr + ( mainBgSize >> 1 );
	snapQvSmallDefQvBufAddr = ( snapQvSmallDefQvBufAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	snapQvAddr = snapQvSmallDefQvBufAddr + ( qVSmallDefQvBufSize >> 1 );
	snapQvAddr = ( snapQvAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	snapQvEffectAddr = snapQvAddr + ( qVSize >> 1 );
	snapQvEffectAddr = ( snapQvEffectAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	snapQvRotAddr = snapQvEffectAddr + ( qVEffectSize >> 1 );
	snapQvRotAddr = ( snapQvRotAddr + 15 ) >> 4 << 4;  // YUV align to 16 word


	snapPhotoFrmYuvScalingAddr = snapQvRotAddr + ( qVRotSize >> 1 );
	snapPhotoFrmYuvScalingAddr = ( snapPhotoFrmYuvScalingAddr + 15 ) >> 4 << 4;  // YUV align to 16 word


	//#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
	//sp1kSnapIcatCdspParamAddr = snapPhotoFrmYuvScalingAddr + (K_SDRAM_ICatRrcBufSize>>1);
	//snapPhotoFrmYuvScalingTempAddr = ( snapPhotoFrmYuvScalingTempAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	//snapPhotoFrmYuvScalingTempAddr = sp1kSnapIcatCdspParamAddr + ( phYuvScalingSize >> 1 );
	//snapPhotoFrmYuvScalingTempAddr = ( snapPhotoFrmYuvScalingTempAddr + 15 ) >> 4 << 4;  // YUV align to 16 word
	//#else
	snapPhotoFrmYuvScalingTempAddr = snapPhotoFrmYuvScalingAddr + ( phYuvScalingSize >> 1 );
	snapPhotoFrmYuvScalingTempAddr = ( snapPhotoFrmYuvScalingTempAddr + 15 ) >> 4 << 4;  // YUV align to 16 word
	//#endif


	snapPrintfMemBuf( "start addr 0x%lx\n", snapMemAddr );
	snapPrintfMemBuf( "end addr 0x%lx\n", snapMemEndAddr );

	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapShutterSoundAddr\n", snapShutterSoundAddr, (UINT32)SNAP_MAX_SHUTTER_SOUND_SIZE, ( snapPhotoFrmYuvAddr - snapShutterSoundAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapPhotoFrmYuvAddr\n", snapPhotoFrmYuvAddr, (UINT32)( phYuvSize ), ( snapBgYuvAddr - snapPhotoFrmYuvAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapBgYuvAddr\n", snapBgYuvAddr, (UINT32)( bgSize ), ( cdspRegAddr - snapBgYuvAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] cdspRegAddr\n", cdspRegAddr, (UINT32)( cdspRegSize ), ( snapQvStampFontPtnAddr - cdspRegAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapQvStampFontPtnAddr\n", snapQvStampFontPtnAddr, (UINT32)( qvDateStampTempSize ), ( snapMainStampFontPtnAddr - snapQvStampFontPtnAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapMainStampFontPtnAddr\n", snapMainStampFontPtnAddr, (UINT32)( mainDateStampTempSize ), ( snapQvDateStampAddr - snapMainStampFontPtnAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapQvDateStampAddr\n", snapQvDateStampAddr, (UINT32)( qvDateStampSize ), ( snapMainDateStampTempAddr - snapQvDateStampAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapMainDateStampTempAddr\n", snapMainDateStampTempAddr, (UINT32)( mainDateStampSize ), ( snapMainDateStampAddr - snapMainDateStampTempAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapMainDateStampAddr\n", snapMainDateStampAddr, (UINT32)( mainDateStampSize ), ( snapQvBgAddr - snapMainDateStampAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapQvBgAddr\n", snapQvBgAddr, (UINT32)( qvBgSize ), ( snapMainBgTempAddr - snapQvBgAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapMainBgTempAddr\n", snapMainBgTempAddr, (UINT32)( mainBgTempSize ), ( snapMainBgAddr - snapMainBgTempAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapMainBgAddr\n", snapMainBgAddr, (UINT32)( mainBgSize ), ( snapQvAddr - snapMainBgAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapQvAddr\n", snapQvAddr, (UINT32)( qVSize ), ( snapQvEffectAddr - snapQvAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapQvEffectAddr\n", snapQvEffectAddr, (UINT32)( qVEffectSize ), ( snapQvRotAddr - snapQvEffectAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapQvRotAddr\n", snapQvRotAddr, (UINT32)( qVRotSize ), ( snapPhotoFrmYuvScalingAddr - snapQvRotAddr ) * 2 );
	snapPrintfMemBuf( "0x%08lx %ld[%ld] snapPhotoFrmYuvScalingAddr\n", snapPhotoFrmYuvScalingAddr, (UINT32)( phYuvScalingSize ), ( snapPhotoFrmYuvScalingTempAddr - snapPhotoFrmYuvScalingAddr ) * 2 );

	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		extern PanoramaInf_t G_PanoramaInf;
		UINT32 EndAddr = G_PanoramaInf.frameTmp.startBufAddr + G_PanoramaInf.frameTmp.cdsp1stOneWSize*G_PanoramaInf.frameTmp.totalNum;
		snapPostYuvAddr = snapPhotoFrmYuvScalingTempAddr + ( phYuvScalingTempSize >> 1 );
		snapQvVlcAddr = snapPostYuvAddr;
		snapJpegHeaderAddr = snapQvVlcAddr + ( ( qVSize >>1 ) >> 1 );
		snapJpegVlcAddr = snapJpegHeaderAddr + ( jpegHeaderSize >> 1 );

		snapNonOverlapVlcBufSize = ( EndAddr - snapJpegVlcAddr ) << 1;

		snapRawDataAddr = G_PanoramaInf.dst.bufAddr;
		snapThbAddr = (snapRawDataAddr + (UINT32)G_PanoramaInf.dst.hSize*G_PanoramaInf.dst.vSize) >> 4 << 4;	// YUV align to 16 word;
		snapThbVlcAddr = snapThbAddr + (thbSize >>1);

		snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapQvVlcAddr\n", snapQvVlcAddr, (UINT32)( qVSize>>1 ), ( snapJpegHeaderAddr - snapQvVlcAddr ) * 2 );
		snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapJpegHeaderAddr\n", snapJpegHeaderAddr, (UINT32)( jpegHeaderSize ), ( snapJpegVlcAddr - snapJpegHeaderAddr ) * 2 );
		snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapJpegVlcAddr\n", snapJpegVlcAddr, (UINT32)( snapMemEndAddr - snapJpegVlcAddr ) * 2, ( snapMemEndAddr - snapJpegVlcAddr ) * 2 );
		snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapRawDataAddr\n", snapRawDataAddr, (UINT32)G_PanoramaInf.dst.hSize*G_PanoramaInf.dst.vSize*2, ( snapMemEndAddr - snapRawDataAddr ) * 2 );
		snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapThbAddr\n", snapThbAddr, (UINT32)( thbSize ), ( 0 ) * 2 );
		snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapThbVlcAddr\n", snapThbVlcAddr, (UINT32)( thbSize ), ( 0 ) * 2 );
		snapPrintfMemTwoBuf(  "0x%08lx %ld[%ld] snapVlcDecodeAddr\n", snapVlcDecodeAddr, (UINT32)( snapVlcDecodeHSize * snapVlcDecodeVSize )<<1, ( 0 ) * 2 );
	}
	else
	#endif
	{
		if ( snapYuvABBufOpt == SNAP_TWO_YUVBUF ) {
			/*will: YUV Two Buffer */
			snapYuv1Addr = snapPhotoFrmYuvScalingTempAddr + ( phYuvScalingTempSize >> 1 );
			snapYuv1Addr = ( snapYuv1Addr + 15 ) >> 4 << 4;  // YUV align to 16 word

			snapYuv2Addr = snapYuv1Addr + ( yuvSize >> 1 );
			snapYuv2Addr = ( snapYuv2Addr + 15 ) >> 4 << 4;  // YUV align to 16 word

			snapPostYuvAddr = snapYuv1Addr;

			snapCdspYuvToYuvBufAddr0 = snapYuv2Addr + ( yuvSize >> 1 );
			snapCdspYuvToYuvBufAddr0 = ( snapCdspYuvToYuvBufAddr0 + 15 ) >> 4 << 4;

			snapCdspYuvToYuvBufAddr1 = snapCdspYuvToYuvBufAddr0;
			snapCdspYuvToYuvBufAddr1 = ( snapCdspYuvToYuvBufAddr1 + 15 ) >> 4 << 4;


			snapQvVlcAddr = snapCdspYuvToYuvBufAddr1 + ( yuvToYuvSize >> 1 );

			snapJpegHeaderAddr = snapQvVlcAddr + ( ( qVSize >>1 ) >> 1 );
			snapJpegVlcAddr = snapJpegHeaderAddr + ( jpegHeaderSize >> 1 );


			if ( snapFlowOpt == CDSP_DO_FRONT ) {
				snapRawDataAddr = snapMemEndAddr - ( rawDataSize >> 2 );
			}
			else {
				snapRawDataAddr = snapMemEndAddr - ( rawDataSize >> 1 );
			}

			snapRawDataAddr = ( snapRawDataAddr ) >> 4 << 4;

			if ( snapFlowOpt == CDSP_DO_FRONT ) {
				snapVlcDecodeAddr = snapRawDataAddr - (snapVlcDecodeHSize * snapVlcDecodeVSize);
				snapVlcDecodeAddr = (snapVlcDecodeAddr) >> 4 << 4;
			}

			#if 1
			snapPrintfMemTwoBuf("K_SDRAM_PvAeAddr=0x%lx\n",K_SDRAM_PvAeAddr);
			snapThbAddr = K_SDRAM_PvAeAddr- (thbSize >>1);
			snapThbAddr = ( snapThbAddr ) >> 4 << 4;	// YUV align to 16 word

			//snapThbVlcAddr = snapThbAddr - ( ( thbSize ) >> 2 );
			snapThbVlcAddr = snapThbAddr - ( thbSize >> 1 );
			#else
			//snapThbAddr = K_SDRAM_PvBufAddrD - ( ( thbSize ) >> 1 );
			snapThbAddr = snapMemEndAddr - ( ( thbSize ) >> 1 );
			snapThbAddr = ( snapThbAddr + 15  ) >> 4 << 4;  // YUV align to 16 word

			//snapThbVlcAddr = snapThbAddr - ( ( thbSize ) >> 2 );
			snapThbVlcAddr = snapThbAddr - ( ( thbSize ) >> 1 );
			#endif

			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapPhotoFrmYuvScalingTempAddr\n", snapPhotoFrmYuvScalingTempAddr, (UINT32)( phYuvScalingTempSize ), ( snapYuv1Addr - snapPhotoFrmYuvScalingTempAddr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapYuv1Addr\n", snapYuv1Addr, (UINT32)yuvSize, ( snapYuv2Addr - snapYuv1Addr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapYuv2Addr\n", snapYuv2Addr, (UINT32)yuvSize, ( snapCdspYuvToYuvBufAddr0 - snapYuv2Addr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapCdspYuvToYuvBufAddr0\n", snapCdspYuvToYuvBufAddr0, (UINT32)0, ( snapCdspYuvToYuvBufAddr1 - snapCdspYuvToYuvBufAddr0 ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapCdspYuvToYuvBufAddr1\n", snapCdspYuvToYuvBufAddr1, (UINT32)yuvToYuvSize, ( snapQvVlcAddr - snapCdspYuvToYuvBufAddr1 ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapQvVlcAddr\n", snapQvVlcAddr, (UINT32)( qVSize>>1 ), ( snapJpegHeaderAddr - snapQvVlcAddr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapJpegHeaderAddr\n", snapJpegHeaderAddr, (UINT32)( jpegHeaderSize ), ( snapJpegVlcAddr - snapJpegHeaderAddr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapJpegVlcAddr\n", snapJpegVlcAddr, (UINT32)( snapMemEndAddr - snapJpegVlcAddr ) * 2, ( snapMemEndAddr - snapJpegVlcAddr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapRawDataAddr\n", snapRawDataAddr, (( snapFlowOpt == CDSP_DO_FRONT ) ? (UINT32)( rawDataSize >> 1 ):(UINT32)( rawDataSize )), ( snapMemEndAddr - snapRawDataAddr ) * 2 );
			//snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapThbAddr\n", snapThbAddr, (UINT32)( thbSize ), ( snapMemEndAddr - snapThbAddr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapThbAddr\n", snapThbAddr, (UINT32)( thbSize ), ( K_SDRAM_PvAeAddr - snapThbAddr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapThbVlcAddr\n", snapThbVlcAddr, (UINT32)( thbSize ), ( snapThbAddr - snapThbVlcAddr ) * 2 );
			snapPrintfMemTwoBuf(  "0x%08lx %ld[%ld] snapVlcDecodeAddr\n", snapVlcDecodeAddr, (UINT32)( snapVlcDecodeHSize * snapVlcDecodeVSize )<<1, ( snapRawDataAddr - snapVlcDecodeAddr ) * 2 );


			#ifdef DRAM_8MB_10_BIT_RAW
			if( raw10BitOpt == 0 ) {
				ASSERT( snapJpegVlcAddr < snapRawDataAddr, 0 );
			}
			else {
				ASSERT( snapJpegHeaderAddr < snapRawDataAddr, 0 );
			}
			#else
			/* debug for check main vlc and main raw */
			#if 0
			snapPrintfMemTwoBuf("vlc start addr=0x%lx, raw start addr=0x%lx\n",snapJpegVlcAddr,snapRawDataAddr);
			if ( !(snapJpegVlcAddr < snapRawDataAddr) ) {
				snapPrintfMemTwoBuf("diff=%lx\n",snapJpegVlcAddr-snapRawDataAddr);
			}
			#endif
			ASSERT( snapJpegVlcAddr < snapRawDataAddr, 0 );
			#endif

			snapNonOverlapVlcBufSize = ( snapRawDataAddr - snapJpegVlcAddr ) << 1;
		}
		else {
			/*will: YUV One Buffer */
			snapCdspYuvToYuvBufAddr0 = snapPhotoFrmYuvScalingTempAddr + ( phYuvScalingTempSize >> 1 );
			snapCdspYuvToYuvBufAddr0 = ( snapCdspYuvToYuvBufAddr0 + 15 ) >> 4 << 4;

			snapCdspYuvToYuvBufAddr1 = snapCdspYuvToYuvBufAddr0;
			snapCdspYuvToYuvBufAddr1 = ( snapCdspYuvToYuvBufAddr1 + 15 ) >> 4 << 4;

			#if 1
			snapPrintfMemOneBuf("K_SDRAM_PvAeAddr=0x%lx\n",K_SDRAM_PvAeAddr);
			snapQvVlcAddr = snapCdspYuvToYuvBufAddr1 + ( yuvToYuvSize >> 1 );
			snapJpegHeaderAddr = snapQvVlcAddr + ( ( qVSize >>1 ) >> 1 );
			snapJpegVlcAddr = snapJpegHeaderAddr + ( jpegHeaderSize >> 1 );

			snapThbAddr = K_SDRAM_PvAeAddr;
			snapThbAddr = ( snapThbAddr ) >> 4 << 4;	// YUV align to 16 word

			snapThbVlcAddr = snapThbAddr - ( ( thbSize ) >> 1 );
			#else
			snapJpegHeaderAddr = snapCdspYuvToYuvBufAddr1 + ( yuvToYuvSize >> 1 );
			snapJpegVlcAddr = snapJpegHeaderAddr + ( jpegHeaderSize >> 1 );

			snapQvVlcAddr = K_SDRAM_PvBufAddrD - ( ( qVSize / 2 ) >> 1 );
			snapThbAddr = snapQvVlcAddr - ( ( thbSize ) >> 1 );
			snapThbAddr = ( snapThbAddr  ) >> 4 << 4;  // YUV align to 16 word

			snapThbVlcAddr = snapThbAddr - ( ( thbSize / 2 ) >> 1 );
			#endif

			if ( snapFlowOpt == CDSP_DO_FRONT) {
				snapRawDataAddr = snapMemEndAddr - ( rawDataSize >> 2 );
			}
			else {
				snapRawDataAddr = snapMemEndAddr - ( rawDataSize >> 1 );
			}

			snapRawDataAddr = snapRawDataAddr >> 4 << 4;

			/* check which is larger, YUV or RAW*/

			snapPrintf("yuvSize=%ld,rawDataSize=%ld\n",yuvSize,rawDataSize);


			yuvRawBufMargin = (UINT32)snapYuvHSize * CDSPJOB_VSIZE * 2 * 2;

			if ( yuvSize > rawDataSize ) { // align to bottom
				snapYuv1Addr = ( snapRawDataAddr + ( rawDataSize >> 1 ) ) - ( yuvSize >> 1 ) - ( yuvRawBufMargin >> 1 );
				snapYuv1Addr = ( snapYuv1Addr ) >> 4 << 4;  // YUV align to 16 word
				snapYuv2Addr = snapYuv1Addr;
			}
			else { // align to top
				snapYuv1Addr = snapRawDataAddr - ( yuvRawBufMargin >> 1 );
				snapYuv1Addr = ( snapYuv1Addr ) >> 4 << 4;  // YUV align to 16 word

				if ( snapFlowOpt == CDSP_DO_FRONT ) {   /* snap do flow */
					snapYuv2Addr = snapYuv1Addr - ( yuvRawBufMargin >> 1 );
					snapYuv2Addr = ( snapYuv2Addr ) >> 4 << 4;	// YUV align to 16 word
				}
				else{
				    snapYuv2Addr = snapYuv1Addr;
				}
			}

			snapPostYuvAddr = snapYuv1Addr;

			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapPhotoFrmYuvScalingTempAddr\n", snapPhotoFrmYuvScalingTempAddr, (UINT32)( phYuvScalingTempSize ), ( snapCdspYuvToYuvBufAddr0 - snapPhotoFrmYuvScalingTempAddr ) * 2 );

			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapCdspYuvToYuvBufAddr0\n", snapCdspYuvToYuvBufAddr0, (UINT32)0, ( snapCdspYuvToYuvBufAddr1 - snapCdspYuvToYuvBufAddr0 ) * 2 );
			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapCdspYuvToYuvBufAddr1\n", snapCdspYuvToYuvBufAddr1, (UINT32)yuvToYuvSize, ( snapQvVlcAddr - snapCdspYuvToYuvBufAddr1 ) * 2 );
			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapQvVlcAddr\n", snapQvVlcAddr, (UINT32)( qVSize >> 1 ), ( snapJpegHeaderAddr - snapQvVlcAddr ) * 2 );
			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapJpegHeaderAddr\n", snapJpegHeaderAddr, (UINT32)( jpegHeaderSize ), ( snapJpegVlcAddr - snapJpegHeaderAddr ) * 2 );
			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapJpegVlcAddr\n", snapJpegVlcAddr, (UINT32)( snapMemEndAddr - snapJpegVlcAddr ) * 2, ( snapMemEndAddr - snapJpegVlcAddr ) * 2 );
			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapThbAddr\n", snapThbAddr, (UINT32)( thbSize ), (UINT32)( thbSize ) );
			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapThbVlcAddr\n", snapThbVlcAddr, (UINT32)( thbSize >> 1 ), ( snapThbAddr - snapThbVlcAddr ) * 2 );
			snapPrintfMemTwoBuf( "0x%08lx %ld[%ld] snapRawDataAddr\n", snapRawDataAddr, (( snapFlowOpt == CDSP_DO_FRONT ) ? (UINT32)( rawDataSize >> 1 ):(UINT32)( rawDataSize )), ( snapMemEndAddr - snapRawDataAddr ) * 2 );
			snapPrintfMemOneBuf( "0x%08lx %ld[%ld] snapYuvAddr\n", snapYuv1Addr, (UINT32)yuvSize, ( snapMemEndAddr - snapYuv1Addr ) * 2 );

	  		#ifdef DRAM_8MB_10_BIT_RAW
			if ( raw10BitOpt == 0 ) {
				ASSERT( snapJpegVlcAddr < snapYuv1Addr, 0 );
			}
			else {
				ASSERT( snapJpegHeaderAddr < snapRawDataAddr, 0 );
			}
	  		#else
			ASSERT( snapJpegVlcAddr < snapYuv1Addr, 0 );
	  		#endif

			snapNonOverlapVlcBufSize = ( snapYuv1Addr - snapJpegVlcAddr ) << 1;
		}
	}

	/* Jpeg Vlc boundary maybe over 16 byte, so decrease buffer size for safety */
	snapNonOverlapVlcBufSize -= (UINT32)HAL_JPEG_VLC_BOUND_MARGIN;

	#if FUNC_KNL_FRM
	/* photoframe source decode */
	if ( snapPhotoFrmOpt ) {
		if (snapPhotoFrmDecode(photoFrmYuvAddr)) {
			ASSERT(0,0);
			return FAIL;
		}
	}
	#endif

	/* assign size */
	//snapPhHSize = (UINT32)SNAP_PH_W_SIZE;
	//snapPhVSize = (UINT32)SNAP_PH_H_SIZE;

	snapInfo[ SNAP_INFO_RAW_BUF_ADDR ] = snapRawDataAddr;

	HAL_CdspRegBackupBufSet(cdspRegAddr,cdspRegSize);
	HAL_CdspRegBackupCntSet(0);

	return SUCCESS;
}

/**
 * @fn        UINT32 snapOne(void)
 * @brief     snap one picture, which inlcude QV, Mian image and thumbnail
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
snapOne(
	void
)
{
	//UINT32 vlcSize = 0;
	UINT8 preBrc, postBrc, soundOpt;
	UINT8 raw10BitOpt;
	UINT8 rawOnly;
	UINT8 snapFlowOpt,snapChgRawOpt;
	UINT8 snapLscCalOpt;
	UINT8 saveRawOpt;
	UINT8 qv;
	UINT8 preProcessMode;

	UINT32 qvYuvBufAddBk;

	#ifdef PV_RAW_DATA
	UINT16 prevW, prevH;
	#endif

	snapPrintf( "snap size X %d, Y %d, yuv X %d, Y %d\n", snapHSize, snapVSize, snapYuvHSize, snapYuvVSize );

	//printf("test A\n");
	preBrc = (UINT8)(snapParam[ SP1K_SNAP_PARAM_PRE_BRC ]);
	postBrc = (UINT8)(snapParam[ SP1K_SNAP_PARAM_POST_BRC ]);
	soundOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT ]);
	preProcessMode = (UINT8)(snapParam[ SP1K_SNAP_PARAM_REDO_PREPROCESS]);
	rawOnly = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DEBUG_RAW_ONLY ]);
	raw10BitOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_10_BIT_RAW_OPT ]);
	snapFlowOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DOFLOW_OPT ]);
	snapChgRawOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_CHANGERAW ]);
	saveRawOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_SAVE_RAW_OPT ]);
	qv = (UINT8)snapParam[ SP1K_SNAP_PARAM_QV ];

	snapLscCalOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_LSCCAL]);
	//snapPrintf("preProcessMode=%bx\n",preProcessMode);
	//snapFlowOpt=0;
	#if defined(AMCAP_OPTION)
		soundOpt =0;
	#endif

	if ( postBrc ) {
		snapMaxTargetVlcSize = ( (UINT32)snapHSize * snapVSize * 2 * snapParam[ SP1K_SNAP_PARAM_COMPRESS_RATIO ] ) >> 10;
	}
	else {
		//snapMaxTargetVlcSize = (snapThbVlcAddr-snapJpegVlcAddr-(UINT32)snapQvHSize*snapQvVSize)<<1;
		snapMaxTargetVlcSize = (snapThbVlcAddr-snapJpegVlcAddr-(UINT32)SNAP_CDSP_QV_HSIZE*SNAP_CDSP_QV_VSIZE)<<1;
	}

	dispImgWinEn(0); // hlc, Mantis #0021510

	//snapTimeRec( (UINT16)__LINE__ );
	if ( snapBurstCnt == 0 ) {
		snapAeMode = sp1kAeGetModeEn();
		#if 0	//LinJieCheng removed, because ae disable always need wait AE_SET_FINISH
		if ( snapAeMode == AE_ENABLE ) {
			while( !( G_AEReady == AE_NOT_READY && G_AESetting == AE_SET_FINISH ) );
		}
		#endif
		aeModeEnableSet(AE_DISABLE);   /* disable AE */
		snapAwbMode = sp1kAwbDisable();
		awbCMSet(AWB_CM_A);
	}

	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		snapAeMode = AE_ENABLE;
	}
	#endif

	#if (SD_R_INT_EN || SD_W_INT_EN)
	/* wait until SD finish writing last file */
	while( sdBgRwStsGet() == SD_BG_WRITE );
	#endif

	/**************************************************/
 	/* Sensor Trigger Process                             					*/
 	/**************************************************/

	snapNormalBreakDown( &snapRawStartTime );
	snapBurstBreakDown( &snapRawStartTime );
	snapBurstStoreFirstBreakDown( &snapRawStartTime );

	if ( !snapPanoramaOpt ) {
		#ifdef PV_RAW_DATA
		frontPreviewSizeGet(0, &prevW, &prevH);
		sensorTrigger( snapRawDataAddr, prevW, prevH, prevW, prevH, prevW, prevH, soundOpt, snapRawMode );
		#else

	   	snapPrintfLog("Sen Trig\n");

		#ifdef DARKSUB_RAWMUX
		if ( (frontSnapMethodGet() == SNAP_MS_DARK) || (preProcessMode == SP1K_SNAP_POST_WB) ) {
			cdspPreProcessOff(snapRawMode,preProcessMode);
			//snapParam[SP1K_SNAP_PARAM_REDO_PREPROCESS]=SP1K_SNAP_PRE_REDO_DARKSUB;
			//printf("redo process=0x%x\n",snapParam[SP1K_SNAP_PARAM_REDO_PREPROCESS]);
		}
		#endif


		#ifdef DOYUV_CROP
		sensorTrigger(	snapRawDataAddr,
						snapSensorHSize, snapSensorVSize,
						snapSensorHSize, snapSensorVSize,
						snapRawHSize, snapRawVSize,snapHSize,
						soundOpt, snapRawMode );
		#else
		sensorTrigger(	snapRawDataAddr,
						snapSensorHSize, snapSensorVSize,
						snapSensorCropHSize, snapSensorCropVSize,
						snapRawHSize, snapRawVSize,snapHSize,
						soundOpt, snapRawMode );
		#endif

		#endif
	}
	#if PANORAMA_EN
	else {
		sensorPanoramaTrigger(soundOpt);
	}
	#endif

	//printf("%bx,%bx,%bx,%bx,%bx\n",XBYTE[0x2A3A],XBYTE[0x2277],XBYTE[0x2138],XBYTE[0x2144],XBYTE[0x2145]);

	snapNormalBreakDown( &snapRawEndTime );
	snapBurstBreakDown( &snapRawEndTime );
	snapBurstStoreFirstBreakDown( &snapRawEndTime );

	/**************************************************/
 	/* Save Raw 			                             					*/
 	/**************************************************/

	if ( !snapPanoramaOpt ) {
		if ( saveRawOpt ) {
			if ( snapRawSave() ) {
				return FAIL;
			}
		}
	}

	snapTimeRec( (UINT16)__LINE__ );

	/**************************************************/
 	/* Snap Stamp Pre Init Process                             			*/
 	/**************************************************/

	snapNormalBreakDown( &snapLoadStampResStartTime );
	snapBurstBreakDown( &snapLoadStampResStartTime );
	snapBurstStoreFirstBreakDown( &snapLoadStampResStartTime );

	/* stamp enable to scale font & scale background & load background */
	if ( snapStampOpt ) {
		snapStampPreInit();
	}

	snapNormalBreakDown( &snapLoadStampResEndTime );
	snapBurstBreakDown( &snapLoadStampResEndTime );
	snapBurstStoreFirstBreakDown( &snapLoadStampResEndTime );

	/**************************************************/
 	/* Claibration Raw Cal Process                             				*/
 	/**************************************************/

	if ( !snapPanoramaOpt ) {
		sp1kCalibrationRawCal( snapRawDataAddr, snapRawHSize, snapRawVSize );
	}

	/**************************************************/
 	/* Change Raw Process                             					*/
 	/**************************************************/

	/* will change raw and load bin file*/
	if ( !snapPanoramaOpt ) {
	    if ( snapChgRawOpt ) {
			snapChangeRaw();
	    }
	}

	/**************************************************/
 	/* LSC Calibration		                          					*/
 	/**************************************************/

 	#ifdef LSC_CALIB_ENABLE
	if ( !snapPanoramaOpt ) {
		if ( snapLscCalOpt ) {
			UINT16 para1,para2;
			UINT8 pvCfa,snapCfa;
			frontDeviceInfoGet(0,FRONT_DEV_INFO_BAYER_FMT,&para1,&para2);
			pvCfa = (UINT8)para1;
			snapCfa = (UINT8)para2;
			//snapCfa=2;
			if ( lscCalibration(snapRawDataAddr,snapSensorHSize,snapSensorVSize,snapCfa) ) {
				return FAIL;
			}
		}
	}
	#endif

	/**************************************************/
 	/* ICat Resource Download Process                 				*/
 	/**************************************************/

	#ifdef ICAT_OPTION
	icatResDownload();
	rawOnly=(UINT8)(snapParam[ SP1K_SNAP_PARAM_DEBUG_RAW_ONLY ]);
	icatSnapRawUpload(snapRawDataAddr,(UINT32)snapRawHSize*(UINT32)snapRawVSize);
	//printf("rawOnly=%bd\n",rawOnly);
	#endif

	/**************************************************/
 	/* Raw To Raw Process                             					*/
 	/**************************************************/

	if ( !snapPanoramaOpt ) {
		snapRawCB( snapRawDataAddr, snapRawHSize, snapRawVSize, snapRawMode );
	}

	snapTimeRec( (UINT16)__LINE__ );

	/* raw only, skip the rest steps */
	if ( rawOnly ) {
		return SUCCESS;
	}

	#ifdef PV_RAW_DATA
	{
		UINT8 f[ 13 ];
		//dcfJpgNameGet(f, dcfFileIndexGet());
	    dcfMediaNameGet(f, dcfFileIndexGet(),MEDIA_JPG);

		f[ 8 ] = 'R';
		f[ 9 ] = 'A';
		f[ 10 ] = 'W';
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

  		#if 1
		dramFileSave(f, snapQvAddr, (UINT32)pcap->width * pcap->height);
		#else
		dramFileSave( f, snapQvAddr, (UINT32)snapPvXSize * snapPvYSize );
		#endif
	}
	#endif

	#ifdef SNAP_IQ_FLOW
	if ( snapIQLoadRaw ) {
		UINT32 fileSize;
		UINT8 f[ 13 ] = "BLK00000RAW";

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		dramFileLoad( f, snapRawDataAddr, &fileSize);

		XBYTE[ 0x2110 ] = 0x02;
	}
	#endif

	if ( !snapPanoramaOpt ) {
		#ifndef COMPARE_ANSWER
		#ifdef DARKSUB_RAWMUX
		if ( ( frontSnapMethodGet() != SNAP_MS_DARK ) &&
			( snapFlowOpt != CDSP_DO_FRONT ) &&
			( preProcessMode != SP1K_SNAP_POST_WB ) ) {
			cdspPreProcessOff(snapRawMode,preProcessMode);
		}
		#else
		cdspPreProcessOff(snapRawMode,preProcessMode);
		#endif
		#endif
	}

	//dateStampActive( snapDSInfo, snapMainDateStampTempAddr, snapDateStampMainXSize, snapDateStampMainYSize, snapMainAdDateStampAddr, snapDateStampMainXSize*16, snapDateStampMainYSize, 0, 0 , 0 );
	//dramFileSave("STAMP000.YUV",snapMainAdDateStampAddr,(UINT32)snapDateStampMainXSize*16*snapDateStampMainYSize*2);


	/**************************************************/
 	/* Raw To Qv YUV Process                      					*/
 	/**************************************************/

	snapNormalBreakDown( &snapDoQvYuvStartTime );

	qvYuvBufAddBk = snapQvAddr;

	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		gprmBlackFill(snapQvAddr,snapQvYuvBufHSize,snapQvYuvBufVSize);
		//printf("Qv snapQvAddr(%lx)%d,%d,%d,%d\n",snapQvAddr,snapQvYuvBufHSize,snapQvYuvBufVSize,snapQvHSize,snapQvVSize);
		snapQvAddr = ((snapQvAddr + (UINT32)((snapQvYuvBufVSize - snapQvVSize)>>1) * snapQvYuvBufHSize)+ 15) >> 4 << 4;  // YUV align to 16 word;
		//printf("Qv snapQvAddr(%lx)\n",snapQvAddr);
	}
	else
	#endif
	{
		/* snap 16:9 fill QV YUV buffer black */
		if ( snapRatioOpt ) {
			gprmBlackFill(snapQvAddr,snapQvYuvBufHSize,snapQvYuvBufVSize);
			//printf("Qv snapQvAddr(%lx)%d,%d,%d,%d\n",snapQvAddr,snapQvYuvBufHSize,snapQvYuvBufVSize,snapQvHSize,snapQvVSize);
			snapQvAddr = ((snapQvAddr + (UINT32)((snapQvYuvBufVSize - snapQvVSize)>>1) * snapQvYuvBufHSize)+ 15) >> 4 << 4;  // YUV align to 16 word;
			//printf("Qv snapQvAddr(%lx)\n",snapQvAddr);
		}
	}

	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		HAL_GprmScale(snapRawDataAddr,
						snapYuvHSize,
						snapYuvVSize,
						snapQvAddr,
						snapQvHSize,
						snapQvVSize,
						snapJpegVlcAddr,
						1);

		//dramFileSave("GPRM0000.YUV",snapQvAddr,(UINT32)snapQvHSize*snapQvVSize*2);
		//dramFileSave("GPRM0001.YUV",snapRawDataAddr,(UINT32)snapYuvHSize*snapYuvHSize*2);
	}
	else
	#endif
	{
		#ifdef DRAM_8MB_10_BIT_RAW
		if ( raw10BitOpt == 0 ) {
			snapQvVlcSize=snapCdspDoImg(CDSP_DO_QV);
		}
		#else

		if ( snapNoQvThbOpt == 0 ) {

			snapPrintfLog("Qv Do\n");

			if ( snapFlowOpt != CDSP_DO_FRONT ) {

				snapCdspDoImg(CDSP_DO_QV);

				if ( snapPhotoFrmOpt )
				#if 0
				{
					HAL_GprmScale(snapPhotoFrmYuvAddr,
									snapPhHSize,
									snapPhVSize,
									snapJpegVlcAddr,
									SNAP_CDSP_QV_HSIZE,
									SNAP_CDSP_QV_VSIZE,
									snapJpegVlcAddr+(UINT32)SNAP_CDSP_QV_HSIZE*SNAP_CDSP_QV_VSIZE,
									1);

					HAL_GprmCopy(snapJpegVlcAddr,
									SNAP_CDSP_QV_HSIZE,
									SNAP_CDSP_QV_VSIZE,
									0,
									0,
									snapQvAddr,
									SNAP_CDSP_QV_HSIZE,
									SNAP_CDSP_QV_VSIZE,
									0,
									0,
									SNAP_CDSP_QV_HSIZE,
									SNAP_CDSP_QV_VSIZE,
									(UINT16)0x80 + 0x14,
									1);

				}
				#else
				{
					HAL_GprmScale(snapPhotoFrmYuvAddr,
									snapPhHSize,
									snapPhVSize,
									snapJpegVlcAddr,
									snapQvHSize,
									snapQvVSize,
									snapJpegVlcAddr+(UINT32)snapQvHSize*snapQvVSize,
									1);

					HAL_GprmCopy(snapJpegVlcAddr,
									snapQvHSize,
									snapQvVSize,
									0,
									0,
									snapQvAddr,
									snapQvHSize,
									snapQvVSize,
									0,
									0,
									snapQvHSize,
									snapQvVSize,
									(UINT16)0x80 + 0x14,
									1);

				}
				#endif
				//HAL_GlobalReadGTimer( &snapQvTime );
			}

			#if 0 /* save qv for test */
			if ( snapRatioOpt ) {
				dramFileSave( "QV000000.YUV", qvYuvBufAddBk, (UINT32)snapQvYuvBufHSize*snapQvYuvBufVSize*2);
			}
			else {
				dramFileSave( "QV000000.YUV", snapQvAddr, (UINT32)snapQvYuvBufHSize*snapQvYuvBufVSize*2);
			}
			#endif
		}

		#endif
	}

	#if (FRONT_SNAP_BREAK_DOWN == 3)
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
	#endif

	snapTimeRec( (UINT16)__LINE__ );

	if (snapNoQvThbOpt == 0) {
		/* Qv stamp & thb source */
		snapStampQvPrepare();

		if ( qv ) {

			if ( snapFlowOpt != CDSP_DO_FRONT ) {

				sp1kHalCtrlScreen(3);
				//printf("snapQvYuvBufHSize=%d,snapQvYuvBufVSize=%d\n",snapQvYuvBufHSize,snapQvYuvBufVSize);

				if ( snapQvSmallDefQvSizeOpt == 1 ) {
					HAL_GprmScale(qvYuvBufAddBk,snapQvYuvBufHSize,snapQvYuvBufVSize,snapQvSmallDefQvBufAddr,snapQvSmallDefQvSizeHsize,snapQvSmallDefQvSizeVsize,snapJpegVlcAddr+(UINT32)snapQvHSize*snapQvVSize,1);
					dispShow(snapQvSmallDefQvBufAddr, snapQvSmallDefQvSizeHsize, snapQvSmallDefQvSizeVsize, 100);/* will:  show QV on the pannel */
				}
				else {
					dispShow(((snapRatioOpt) ? qvYuvBufAddBk : snapQvAddr), snapQvYuvBufHSize, snapQvYuvBufVSize, 100);/* will:  show QV on the pannel */
				}
			}
		}

		if ( snapRatioOpt) {
			snapQvAddr = qvYuvBufAddBk;/*restore the quickview YUV addr for quickview*/
		}

		//HAL_DispRegUpdate();	  /*Phil: DO Not use disp reg update directly*/
		HAL_GlobalReadGTimer(&snapQvShowTime);
	}

	HAL_GlobalReadGTimer( &snapQvTime );

	snapNormalBreakDown( &snapDoQvYuvEndTime );

	#ifdef SNAP_IQ_FLOW
	if ( snapIQSizeConfig ) {
		snapRawHSize = snapYuvHSize = snapHSize = snapIQConfigXSize;
		snapRawVSize = snapYuvVSize = snapVSize = snapIQConfigYSize;
	}
	#endif

	/**************************************************/
 	/* Blink Process                                     					*/
 	/**************************************************/

	if ( !snapPanoramaOpt ) {
		if ( snapBlinkDetect() == FAIL ) {
			return FAIL;
		}
	}

	/**************************************************/
 	/* CDSP DO YUV and JPG Encode                   					*/
 	/**************************************************/

    printf("Main Do\n");

	snapNormalBreakDown( &snapMainStartTime );
	snapBurstBreakDown( &snapMainStartTime );
	snapBurstStoreFirstBreakDown( &snapMainStartTime );

	snapStampMainPrepare();

	// post BRC init
	jpegMainImgQIdxGet(&snapMainRuningQIdx);

	//if (snapFlowOpt == CDSP_DO_FRONT){
	//	snapMainRuningQIdx = 80;
	//}

	//snapPrintf("snap q=%bd\n",snapMainRuningQIdx);

	snapPostBRCFinalF = 0;
	//snapMainRuningQIdx += SNAP_MAIN_Q_ID_STEP;


	//printf("snap q=%bd\n",snapMainRuningQIdx);
	snapJpegInit();

	//snapRegDump(0);

	#if PANORAMA_EN
	if ( snapPanoramaOpt ) {
		snapMainVlcSize = snapDoJpeg(100,
									100,
									snapRawDataAddr,
									snapYuvHSize,
									snapYuvVSize,
									snapJpegVlcAddr,
									snapHSize,
									snapVSize);
	}
	else
	#endif
	{
		//vlcSize = snapCdspDoImg(snapFlowOpt);
		switch ( snapRawMode ) {
			case CDSP_SNAP_8RAW:
				if ( snapFlowOpt == CDSP_DO_RAW ) {
					snapMainVlcSize = snapCdspDoImg(CDSP_DO_RAW);

					/* debug for normal capture save main vlc */
					#if 0
					dramFileSave("VLC00000.BIN", snapJpegVlcAddr, snapMainVlcSize);
					printf("snapMainVlcSize=%lu\n",snapMainVlcSize);
					#endif
				}
				else {
					if ( snapBurstOpt == 0 ) {
						snapMainVlcSize = snapCdspDoImg(CDSP_DO_FRONT);
						snapBurstBreakDown( &snapMainEndTime );
					}
					else {
						//UINT8 burstAbort=0;
						#if (BURST_BREAK_DOWN == 1 || BURST_STORE_FIRST_BREAK_DOWN == 1)
						UINT32 burstTime;
						#endif

						while ( 1 ) {

							snapMainVlcSize = snapCdspDoImg(CDSP_DO_FRONT);

							snapPrintf("cnt=%bu,vlc addr=0x%lx, size=0x%lx\n",snapBurstCnt,snapJpegVlcAddr,snapMainVlcSize);
							snapBurstBreakDown( &snapMainEndTime );
							snapBurstStoreFirstBreakDown( &snapMainEndTime );
							snapPrintfBurstBreakDown( "[%bu],S->MS,ME:%lu,%lu\n", snapBurstCnt, snapMainStartTime - snapStartTime, snapMainEndTime - snapStartTime );
							snapPrintfBurstStoreFirstBreakDown( "[%bu],S->MS,ME:%lu,%lu\n", snapBurstCnt, snapMainStartTime - snapStartTime, snapMainEndTime - snapStartTime );

							snapBurstJpegVlcAddr[snapBurstCnt] = snapJpegVlcAddr;
							snapBurstMainVlcSize[snapBurstCnt] = snapMainVlcSize;
							//printf("snapJpegVlcAddr=%lx,snapRawDataAddr=%lx\n",snapJpegVlcAddr,snapRawDataAddr);

							if ( snapDoFlowBurstStoreFirst == 1 ) {
								UINT8 sts;

								if (snapVLCOverflowGet()==0x00){  /* VLC size is ok */
									snapJpegVlcEndAddr = snapJpegVlcAddr + ( ( snapMainVlcSize + 1 ) >> 1 );

								}
								else{	/* VLC overflow */
									snapJpegVlcEndAddr = snapJpegVlcAddr + ( ( snapMaxTargetVlcSize + 1 ) >> 1 );
									snapPrintf("vlc overflow\n");
								}

								/* set Q of Main Jpeg again */
								jpegQTableSet( snapMainRuningQIdx, 0, 0, 1 );

								snapJpegVlcAddr = snapBurstJpegVlcAddr[snapBurstCnt];
								snapMainVlcSize = snapBurstMainVlcSize[snapBurstCnt];
								snapJpegHeaderAddr = snapJpegVlcAddr - (SNAP_JPEG_HDR_SIZE>>1);

								sts = snapPostProcess();

								snapBurstStoreFirstBreakDown( &snapSaveStartTime );
								snapStore( &snapDSInfo[snapBurstCnt] );
								snapBurstStoreFirstBreakDown( &snapSaveEndTime );
								snapPrintfBurstStoreFirstBreakDown( "[%bu],S->SS,SE:%lu,%lu\n", snapBurstCnt, snapSaveStartTime - snapStartTime, snapSaveEndTime - snapStartTime );
							}

							snapBurstCnt++;

							if ( (*psnapBurst)( &snapBurstCnt ) == FALSE ) {

								//burstAbort=1;
								snapBurstBreakDown( &burstTime );
								snapPrintfBurstBreakDown( "Burst Time: %lu\n", burstTime - snapStartTime );
								snapBurstStoreFirstBreakDown( &burstTime );
								snapPrintfBurstStoreFirstBreakDown( "Burst Time: %lu\n", burstTime - snapStartTime );

								/* debug save vlc flow */
								#if 0
								dramFileSave("JPEG0000.BIN",snapBurstJpegVlcAddr[0],snapBurstMainVlcSize[0]);
								dramFileSave("JPEG0001.BIN",snapBurstJpegVlcAddr[1],snapBurstMainVlcSize[1]);
								dramFileSave("JPEG0002.BIN",snapBurstJpegVlcAddr[2],snapBurstMainVlcSize[2]);
								#endif
								break;
							}

							snapBurstBreakDown( &snapMainStartTime );
							snapBurstStoreFirstBreakDown( &snapMainStartTime );

							if ( snapDoFlowBurstStoreFirst == 0 ) {
								snapJpegVlcAddr += ((snapMainVlcSize + SNAP_JPEG_HDR_SIZE + 1)>>1);
								snapJpegVlcAddr = (snapJpegVlcAddr + (UINT32)15) >>4 <<4;

							}
							//printf("snapJpegVlcAddr=%lx,snapRawDataAddr=%lx\n",snapJpegVlcAddr,snapRawDataAddr);

							/* check JPG Size Max = Raw Size * 3/5 */
							if ( snapJpegVlcAddr > (snapRawDataAddr - ((UINT32)snapRawHSize * snapRawVSize * 3 /10)) ) {
								snapJpegVlcAddr = snapBurstJpegVlcAddr[snapBurstCnt-1];
								snapPrintfLog("jpeg is over\n");
								snapBurstVlcOverflow=1;
								//ASSERT(FALSE,0);
								break;
							}

							/* check Burst Number >= SNAP_DOFLOW_BURST_NUMBER */
							if ( snapBurstCnt >= SNAP_DOFLOW_BURST_NUMBER ) {
								snapPrintfLog("burst num is over\n");
								ASSERT(0,0);
							}

							/* set JPG VLC boundary again */
							snapMaxTargetVlcSize = (snapThbVlcAddr-snapJpegVlcAddr-(UINT32)snapQvHSize*snapQvVSize)<<1;
							snapJpegInit();

							//snapPrintf("vlc addr[%bd]=0x%lx\n",snapBurstCnt,snapJpegVlcAddr);
							if ( UI_LCD_ON_WITH_OSD == appGetOsdMode() ) {
								// UI update remain pix count
								uiRemainPixCount(1);
							}
							else {
								uiRemainPixCount(0);
							}

							/* Sutter Sound Play */
							if ( soundOpt == 1 ) {
								sp1kAudioPlay(snapShutterSoundAddr);
							}

							/* Do Front Burst stamp */
							//dbgTimerStart();
							snapStampDoFrontBurstMainPrepare();
							//dbgTimerStop()

							/*stop & start WDT debug flow */
							#if 0
							XBYTE[0x2075] = 0x02;	/*stop WDT*/
							while (!getchar());
							XBYTE[0x2075] = 0x01;	/*start WDT*/
							#endif

						}
					}
					//snapMainVlcSize = snapCdspDoImg(CDSP_DO_FRONT);
				}
				break;
			case CDSP_SNAP_YUV:
				snapMainVlcSize = snapCdspDoImg(CDSP_DO_YUV);
				break;
			default:
				break;
		}
	}

	if ( snapMainVlcSize == 0 ) {
		return FAIL;
	}

	snapNormalBreakDown( &snapMainEndTime );

	snapPrintfLog("Main Do End\n");

	if (snapFlowOpt == CDSP_DO_FRONT){
		cdspPreProcessOff(snapRawMode,preProcessMode);
	}

	if(( snapDateStampOpt == 1 )||(snapBgOpt == 1)) {
		HAL_JpegDataStampEnable(0x00, 0x00);
	}

	//snapJpegVlcEndAddr = snapJpegVlcAddr + ( ( vlcSize + 1 ) >> 1 );

	if (snapVLCOverflowGet()==0x00){  /* VLC size is ok */
		snapJpegVlcEndAddr = snapJpegVlcAddr + ( ( snapMainVlcSize + 1 ) >> 1 );

	}
	else{	/* VLC overflow */
		snapJpegVlcEndAddr = snapJpegVlcAddr + ( ( snapMaxTargetVlcSize + 1 ) >> 1 );
		snapPrintf("vlc overflow\n");
	}

	snapPrintf("vlc end addr=0x%lx\n",snapJpegVlcEndAddr);
    //snapRegDump(2);

	snapYuvSave();

	if ( UI_LCD_ON_WITH_OSD == appGetOsdMode() ) {
		// UI update remain pix count
		uiRemainPixCount(1);
		//uiUpdateRemainPix();
	}
	else {
		uiRemainPixCount(0);
	}

	/**************************************************/
 	/* Earliest Back to PV                             					*/
 	/**************************************************/

   	if ( snapFlowOpt != CDSP_DO_FRONT ) {
		snapQvToPv( 0, (UINT16)__LINE__  ); /* Earliest Back to PV */
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 sp1kSnapTask(void)
 * @brief     It triggers snap flow to capture image.
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kSnapTask(
	void
)
{
	UINT32 burstAbort;
	UINT32 diskFreeSpace;
	UINT8 sts;
	UINT8 rawOnly, raw10BitOpt;
	UINT16 para1, para2;
	UINT8 qv;
	UINT8 snapFlowOpt;
	UINT8 i;

	//snapParam[ SP1K_SNAP_PARAM_REDO_PREPROCESS]=SP1K_SNAP_POST_WB;	/*FOR TEST POSTWB*/

	//printf("snap task start\n");

	#if defined(ICAT_OPTION) || defined(AMCAP_OPTION) //xian ++ 20091027
	icatIsConnected();
	#endif

	#ifdef SNAP_TEMP_BURST_TEST
	UINT16 i = 0;
	sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 1 );
	#endif

	#ifdef DRAM_8MB_10_BIT_RAW
	sp1kSnapParamSet( SP1K_SNAP_PARAM_10_BIT_RAW_OPT, 1 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 1 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_YUV_OPT, 1 );
	//sp1kSnapParamSet( SP1K_SNAP_PARAM_POST_WB_OPT, 1 );
	#endif

	#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
	icatSnapInitial();
	#endif

	snapBurstOpt = (UINT8)snapParam[ SP1K_SNAP_PARAM_BURST ];
	snapStampOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_STAMP_OPT ]);
	snapPhotoFrmOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_PHOTO_FRM_OPT ]);
	snapNoQvThbOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_BYPASS_QV_THB_OPT ]);
	snapDoFlowBurstStoreFirst = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DOFLOW_STORE_FIRST_OPT ]);
	snapPanoramaOpt = (UINT8)(snapParam[SP1K_SNAP_PARAM_PANORAMA_OPT]);

	if (snapNoQvThbOpt == 1 || snapDoFlowBurstStoreFirst == 1) {
		snapParam[ SP1K_SNAP_PARAM_QV ] = 0;
	}

	qv = (UINT8)snapParam[ SP1K_SNAP_PARAM_QV ];
	rawOnly = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DEBUG_RAW_ONLY ]);
	raw10BitOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_10_BIT_RAW_OPT ]);
	snapFlowOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DOFLOW_OPT ]);

	HAL_GlobalReadGTimer( &snapStartTime );

	snapQvTime = 0;
	snapEndPvTime = 0;
	snapEndTime = 0;

	#ifdef SNAP_TIME_MEASURE
	snapDebugEntryCnt = 0;
	HAL_GlobalReadGTimer(&snapPreviousTime);
	//printf( "@@ start time %ld\n", time );
	snapStartTime = snapPreviousTime ;
	#endif

	frontDeviceInfoGet( 0, FRONT_DEV_INFO_DATA_FMT, &para1, &para2 );

	burstAbort = 0;

	/* init Raw mode */
	if( para1 == FRONT_DATA_FMT_YUV ) { /* YUV sensor */
		snapRawMode = CDSP_SNAP_YUV;
	}
	else if( raw10BitOpt == 1 ) {
		snapRawMode = CDSP_SNAP_10RAW;
	}
	else {
		snapRawMode = CDSP_SNAP_8RAW;
	}

	ASSERT( !( raw10BitOpt == 1 && para1 == FRONT_DATA_FMT_YUV ), 0 );

	//XBYTE[REG_Disp_DispEn_Flip]=0x02;
	dispParaSet(DISP_PARA_IMG_LAYER_ENABLE, 0, 0, 0, 0, 0);
	HAL_DispRegUpdate();
	//pvFwZFSet(0);
	//printf("snap mem init\n");
	// init

	/* snap memory init */
	snapMemInit();

	#if 0
	#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
		icatSnapInitial();
	#endif
	#endif

	if (snapPhotoFrmOpt) {
		snapPhotoFrmLastNum = ( snapYuvVSize + ( snapYuvLines - 1 ) ) / snapYuvLines - 1;

		gprmPartialVScaleInit( snapPhHSize, snapPhVSize, snapYuvHSize, snapYuvVSize, snapYuvLines ); //will mark
		snapPrintf("photofrm block num=%d\n",snapPhotoFrmLastNum);
		snapPrintf("photo h=%d,v=%d\n",snapPhHSize,snapPhVSize);
		snapPrintf("snap h=%d,v=%d\n",snapYuvHSize,snapYuvVSize);
	}

	snapQvShowTime = 0;			/* for the time of showing qv */
	snapBackToPv = 0;			/* state of preview , 1: preview  0: not preview */
	snapBurstCnt = 0;			/* count of burst capture */
	snapBurstVlcOverflow = 0;	/* do front burst caputure. 1:vlc overflow  0:normal */

	while ( burstAbort == 0 ) {

		/* give 10s counter value to  watch dog */
		dbgWdtFeed(10000);

		diskFreeSpace = (UINT32)1024 * (UINT32)DOS_GetDiskSpace();

		//printf("snapDiskFullThd=%ld\n",snapDiskFullThd);
		//printf("diskFreeSpace=%ld\n",diskFreeSpace);

		if ( snapDiskFullThd > diskFreeSpace ) {
			printf( "Disk maybe full!!!\n" );
			//break;
		}

		HAL_CdspRegBackupCntSet(0);

		if ( snapBurstCnt != 0 ) {
			cdspPreProcessReload();
			cdspPostProcessCtrl(1,0);
		}

		snapTimeRec( (UINT16)__LINE__ );

		snapPvYuvSave(0,0);

        if ( snapOne() == FAIL ) {
        	//return FAIL;
        	break;
        }

		/* card check */
		if ( GmountCard ) {

			GmountCard = 0;

			snapNormalBreakDown( &snapMountStartTime );
			snapBurstBreakDown( &snapMountStartTime );
			snapBurstStoreFirstBreakDown( &snapMountStartTime );

			if ( sp1kSD_CdGet() ) {
				/* NO card */
				appStorageMount(K_MEDIA_NANDF);
			}
			else {
				appStorageMount(K_MEDIA_SDF);
			}

			snapNormalBreakDown( &snapMountEndTime );
			snapBurstBreakDown( &snapMountEndTime );
			snapBurstStoreFirstBreakDown( &snapMountEndTime );
		}

      	/* raw only, skip the rest steps */
		if ( rawOnly == 1 ) {

			sp1kHalCtrlScreen(3);

			//#ifdef ICAT_OPTION
			//	icatSnapFinish();
			//#endif

			while( snapQvToPv( 0, (UINT16)__LINE__  ) != 1 );

			break;
		}


		#ifdef DRAM_8MB_10_BIT_RAW
		#if 0  /* will mark */
		if ( raw10BitOpt == 1 ) {
			// save a fake jpeg to increase file index
			snapHdrSize = SNAP_JPEG_HDR_SIZE;
			snapMainVlcSize = 0;

			snapStore();
			break;
		}
		#endif
		#endif

		snapTimeRec( (UINT16)__LINE__ );


		#ifdef SNAP_TEMP_BURST_TEST
		if ( i > 3 ) {
			sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );
		}
		#endif

		if ( qv == 0 ) { /*auto review==0, trun on display here*/
			sp1kHalCtrlScreen(2);//mantis #41936
		}

		if ( (snapBurstOpt == 1) && (snapFlowOpt == CDSP_DO_FRONT) ) {
			/*
			snapBurstJpegVlcAddr[snapBurstCnt] = snapJpegVlcAddr;
			printf("vlc addr[%bd]=0x%lx\n",snapBurstCnt,snapJpegVlcAddr);
			snapStore();
			snapJpegVlcAddr += snapMainVlcSize;
			snapJpegVlcAddr = (snapJpegVlcAddr + 15) >>4 <<4;
			snapBurstCnt++;

			if ((*psnapBurst)( &snapBurstCnt ) == FALSE){
				burstAbort=1;
				//snapStore();
			}
			*/
			//dramFileSave("JPEG0000.BIN",snapBurstJpegVlcAddr[0],snapBurstMainVlcSize[0]);
			//dramFileSave("JPEG0001.BIN",snapBurstJpegVlcAddr[1],snapBurstMainVlcSize[1]);
			//dramFileSave("JPEG0002.BIN",snapBurstJpegVlcAddr[2],snapBurstMainVlcSize[2]);

			//snapJpegVlcAddr = snapBurstJpegVlcAddr[0];
			//snapMainVlcSize = snapBurstMainVlcSize[0];
			//snapJpegHeaderAddr = snapJpegVlcAddr - (SNAP_JPEG_HDR_SIZE>>1);

			//snapStore();

			/* save all the images (snapBurstCnt frames)*/
			if ( snapDoFlowBurstStoreFirst == 0 ) {
				for ( i = 0 ; i < snapBurstCnt ; i++ ) {

					/* set Q of Main Jpeg again */
					jpegQTableSet( snapMainRuningQIdx, 0, 0, 1 );

					snapJpegVlcAddr = snapBurstJpegVlcAddr[i];
					snapMainVlcSize = snapBurstMainVlcSize[i];
					snapJpegHeaderAddr = snapJpegVlcAddr - (SNAP_JPEG_HDR_SIZE>>1);

					sts = snapPostProcess();

					snapBurstStoreFirstBreakDown( &snapSaveStartTime );
					snapStore( &snapDSInfo[i] );
					snapBurstStoreFirstBreakDown( &snapSaveEndTime );
				}
			}
			else {
				sts = SUCCESS;
			}

		}
		else {
			sts = snapPostProcess();

			#if (FRONT_SNAP_BREAK_DOWN == 3)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
			#endif

			snapNormalBreakDown( &snapSaveStartTime );
			snapStore( &snapDSInfo[snapBurstCnt] );
			snapNormalBreakDown( &snapSaveEndTime );

			#if (FRONT_SNAP_BREAK_DOWN == 3)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
			#endif

			snapBurstCnt++;
		}

		/* first burst check point , don't go back to pv before storing data*/
		/* AE/AWB/EDGE Window would write data to DRAM while preview is on */
		burstAbort = snapBurstToPv((UINT16)__LINE__);

		if ( sts != SUCCESS ) {
			printf( "card full......\n" );

			if ( snapBackToPv == 0 ) {
				snapPvTurnOn();

				snapTimeRec( (UINT16)__LINE__ );

				snapBackToPv = 1;
			}

			burstAbort = 1;

			break;
		}

		#if 0  //will mark
		if(RemainPictureNO==0)//jintao.liu 2008-4-30 burst snap count #23742
		{
			//sp1kBtnDisableAll();//jintao.liu 2008-4-29
			//uiUpdateOSDMemFull();//show mem full
			//sp1kBtnEnableAll();//jintao.liu 2008-4-29

			if( snapBackToPv == 0 ) {
				snapPvTurnOn();
				snapBackToPv = 1;
			}

			burstAbort = 1;

			break;
		}
		#endif

		#if (FRONT_SNAP_BREAK_DOWN >= 2)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif

		/* second burst check point */
		burstAbort = snapBurstToPv((UINT16)__LINE__);

		if ( snapBurstOpt == 0 && snapBackToPv == 0 ) {
			/* wait until back to PV */
			while( snapQvToPv( 0, (UINT16)__LINE__  ) != 1 );
		}

		/* no burst, abort */
		if( snapBurstOpt == 0 ) {
			burstAbort = 1;
		}

		#if (FRONT_SNAP_BREAK_DOWN >= 2)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif

		#if 0
		else {
			if (snapBgOpt == 1) {
				if (snapDateStampOpt == 1) {
					//Bg stamp & date stamp & snap burst capture need to re-load logo to snapMainBgAddr
					HAL_GprmCopy(snapMainBgTempAddr,snapBgMainXSize,snapBgMainYSize,0,0,
						snapMainBgAddr,snapHSize,snapBgMainYSize,0,0,snapBgMainXSize,snapBgMainYSize,
						0x3a,1);
				}
			}
		}
		#endif

		#ifdef SNAP_TEMP_BURST_TEST
		i++;
		#endif

		snapPrintfNormalBreakDown( "S->SDS,SDE:%lu,%lu\n", snapMountStartTime - snapStartTime, snapMountEndTime - snapStartTime );
		snapPrintfNormalBreakDown( "S->RS,RE:%lu,%lu\n", snapRawStartTime - snapStartTime, snapRawEndTime - snapStartTime );
		snapPrintfNormalBreakDown( "S->LRS,LRE:%lu,%lu\n", snapLoadStampResStartTime - snapStartTime, snapLoadStampResEndTime - snapStartTime );
		snapPrintfNormalBreakDown( "S->DQYS,DQYE:%lu,%lu\n", snapDoQvYuvStartTime - snapStartTime, snapDoQvYuvEndTime - snapStartTime );
		snapPrintfNormalBreakDown( "S->MS,ME:%lu,%lu\n", snapMainStartTime - snapStartTime, snapMainEndTime - snapStartTime );
		snapPrintfNormalBreakDown( "S->PVS,PVE:%lu,%lu\n", snapStartPvTime - snapStartTime, snapEndPvTime - snapStartTime  );
		snapPrintfNormalBreakDown( "S->ThbS,ThbE:%lu,%lu\n", snapThbStartTime - snapStartTime, snapThbEndTime - snapStartTime );
		snapPrintfNormalBreakDown( "S->QVES,QVEE:%lu,%lu\n", snapQvEncodeStartTime - snapStartTime, snapQvEncodeEndTime - snapStartTime );
		snapPrintfNormalBreakDown( "S->PostE: %lu\n", snapPostEndTime - snapStartTime );
		snapPrintfNormalBreakDown( "S->SS,SE:%lu,%lu\n", snapSaveStartTime - snapStartTime, snapSaveEndTime - snapStartTime );

		snapPrintfBurstBreakDown( "S->SDS,SDE:%lu,%lu\n", snapMountStartTime - snapStartTime, snapMountEndTime - snapStartTime );
		snapPrintfBurstBreakDown( "S->RS,RE:%lu,%lu\n", snapRawStartTime - snapStartTime, snapRawEndTime - snapStartTime );
		snapPrintfBurstBreakDown( "S->LRS,LRE:%lu,%lu\n", snapLoadStampResStartTime - snapStartTime, snapLoadStampResEndTime - snapStartTime );
		//snapPrintfBurstBreakDown( "S->DQYS,DQYE:%lu,%lu\n", snapDoQvYuvStartTime - snapStartTime, snapDoQvYuvEndTime - snapStartTime );
		//snapPrintfBurstBreakDown( "S->MS,ME:%lu,%lu\n", snapMainStartTime - snapStartTime, snapMainEndTime - snapStartTime );
		snapPrintfBurstBreakDown( "S->PVS,PVE:%lu,%lu\n", snapStartPvTime - snapStartTime, snapEndPvTime - snapStartTime  );
		//snapPrintfBurstBreakDown( "S->ThbS,ThbE:%lu,%lu\n", snapThbStartTime - snapStartTime, snapThbEndTime - snapStartTime );
		//snapPrintfBurstBreakDown( "S->QVES,QVEE:%lu,%lu\n", snapQvEncodeStartTime - snapStartTime, snapQvEncodeEndTime - snapStartTime );
		snapPrintfBurstBreakDown( "S->PostE: %lu\n", snapPostEndTime - snapStartTime );
		snapPrintfBurstBreakDown( "S->SS,SE:%lu,%lu\n", snapSaveStartTime - snapStartTime, snapSaveEndTime - snapStartTime );

		snapPrintfBurstStoreFirstBreakDown( "S->SDS,SDE:%lu,%lu\n", snapMountStartTime - snapStartTime, snapMountEndTime - snapStartTime );
		snapPrintfBurstStoreFirstBreakDown( "S->RS,RE:%lu,%lu\n", snapRawStartTime - snapStartTime, snapRawEndTime - snapStartTime );
		snapPrintfBurstStoreFirstBreakDown( "S->LRS,LRE:%lu,%lu\n", snapLoadStampResStartTime - snapStartTime, snapLoadStampResEndTime - snapStartTime );
		snapPrintfBurstStoreFirstBreakDown( "S->DQYS,DQYE:%lu,%lu\n", snapDoQvYuvStartTime - snapStartTime, snapDoQvYuvEndTime - snapStartTime );
		//snapPrintfBurstStoreFirstBreakDown( "S->MS,ME:%lu,%lu\n", snapMainStartTime - snapStartTime, snapMainEndTime - snapStartTime );
		snapPrintfBurstStoreFirstBreakDown( "S->PVS,PVE:%lu,%lu\n", snapStartPvTime - snapStartTime, snapEndPvTime - snapStartTime  );
		//snapPrintfBurstStoreFirstBreakDown( "S->ThbS,ThbE:%lu,%lu\n", snapThbStartTime - snapStartTime, snapThbEndTime - snapStartTime );
		//snapPrintfBurstStoreFirstBreakDown( "S->QVES,QVEE:%lu,%lu\n", snapQvEncodeStartTime - snapStartTime, snapQvEncodeEndTime - snapStartTime );
		snapPrintfBurstStoreFirstBreakDown( "S->PostE: %lu\n", snapPostEndTime - snapStartTime );
		//snapPrintfBurstStoreFirstBreakDown( "S->SS,SE:%lu,%lu\n", snapSaveStartTime - snapStartTime, snapSaveEndTime - snapStartTime );
	}

	//snapTimeRec( (UINT16)__LINE__ );

	snapAeAwbTurnOn();  						/*turn on AE/AWB here to avoid overwriting jpg */
	HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0);	/*turn on first frame buffer here to avoid overwrite jpg*/

	HAL_Cdsp2ndPathCfg(0,0,0);	/*turn on second frame buffer here to avoid overwrite jpg*/

	HAL_GlobalReadGTimer( &snapEndTime );

	snapTimeDump();

	#ifdef ICAT_OPTION
		icatSnapFinish();
	#endif

	snapPrintf( "0x2277=0x%bx\n",XBYTE[REG_Cdsp_pSkipwr]);
	snapPrintf( "0x2258=0x%bx\n",XBYTE[REG_Cdsp_skipwr1]);

   	snapPrintfLog( "==Time ==\n" );
	snapPrintfLog( "S->QV: %lu\n", snapQvTime - snapStartTime );
	snapPrintfLog( "S->PV: %lu\n", snapEndPvTime - snapStartTime );
	snapPrintfLog( "S->S:  %lu\n", snapEndTime - snapStartTime );
	//sp1kPrevewUnfreeze();	/* start to preview*/

	if (snapVLCOverflowGet()==1){	/* VLC overflow */
		snapPrintfLog("VLC overflow\n");
		XBYTE[REG_Jpeg_Intr_Event] |= 0x08;	/*clear VLC overflow interrupt*/
		return FAIL;
	}
	else{
		return sts;
	}


}


/**
 * @fn        UINT8 snapPostProcess(void)
 * @brief     generate a thumbnail
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
snapPostProcess(
	void
)
{
	UINT8 jpegOpt, rotFlipOpt;
	UINT32 maxQvVlcSize;
	UINT8 snapFlowOpt;
	UINT32 qvYuvBufAddBk;
	UINT8 qv;


	jpegOpt = 0;

	jpegOpt = jpegOpt | 0x20;	// jfif

	rotFlipOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_ROT_FLIP_OPT ]);

	rotFlipOpt = rotFlipOpt < 8 ? rotFlipOpt : 0;
	jpegOpt = jpegOpt | ( rotFlipOpt << 2 ); 						/* rotate flip */
    snapFlowOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DOFLOW_OPT ]);
	qv = (UINT8)snapParam[ SP1K_SNAP_PARAM_QV ];

	//snapTimeRec( (UINT16)__LINE__ );

	if (snapNoQvThbOpt == 0) {

		if ( snapRatioOpt ) {
			qvYuvBufAddBk = snapQvAddr;
		}

		if ( snapFlowOpt == CDSP_DO_FRONT ) {  /*Do front mode qv YUV is decode from MainVlc*/
			UINT16 decHSize,decVSize;

			snapBurstBreakDown( &snapDoQvYuvStartTime );
			snapBurstStoreFirstBreakDown( &snapDoQvYuvStartTime );

			decHSize = (snapHSize >>3)*snapPbScaleRatio;//324
			decVSize = (snapVSize >>3)*snapPbScaleRatio;//243
			//printf("dec h=%d,v=%d\n",decHSize,decVSize);
			//printf("dec h=%d,v=%d\n",snapVlcDecodeHSize,snapVlcDecodeVSize);
			HAL_JpegDecode(snapJpegVlcAddr,snapHSize,snapVSize,snapVlcDecodeAddr,snapPbScaleRatio,0x04,G_JpgRstMcuNum);
			//dramFileSave("VLCDEC00.YUV",snapVlcDecodeAddr,(UINT32)snapVlcDecodeHSize*snapVlcDecodeVSize*2);
			//gprmBlackFill(snapQvAddr,snapQvYuvBufHSize,snapQvYuvBufVSize);
			//dramFileSave("QV000001.YUV",snapQvAddr,(UINT32)SNAP_CDSP_QV_HSIZE*SNAP_CDSP_QV_VSIZE*2);

			XBYTE[REG_Jpeg_Swreset]=0x01;    //JPG software reset
			XBYTE[REG_Jpeg_Swreset]=0x00;

			if( snapRatioOpt == 1 ) {//snapQvHSize*3 > snapQvVSize*4){/*16:9 image , modify QV YUV Addr for thumbnail*/
				//printf("snapQvAddr(%lu)\n",snapQvAddr);
				snapQvAddr = ((snapQvAddr + (UINT32)((snapQvYuvBufVSize - snapQvVSize)>>1) * snapQvYuvBufHSize)+ 15) >> 4 << 4;  // YUV align to 16 word;
				//printf("snapQvAddr(%lu)\n",snapQvAddr);
			}

			//if ( snapQvSmallDefQvSizeOpt == 1 ) {
			//}

			if ((decHSize & 0x07)!= 0){
				decHSize = decHSize >>3 <<3;
				decVSize = decVSize >>3 <<3;
				if ((decHSize == snapQvHSize) && (decVSize == snapQvVSize)){
					HAL_GprmCopy(snapVlcDecodeAddr,snapVlcDecodeHSize,snapVlcDecodeVSize,0,0,
								snapQvAddr,decHSize,decVSize,0,0,decHSize,decVSize,0,1);
				}
				else{
					HAL_GprmCopy(snapVlcDecodeAddr,snapVlcDecodeHSize,snapVlcDecodeVSize,0,0,
								snapVlcDecodeAddr,decHSize,decVSize,0,0,decHSize,decVSize,0,1);

					HAL_GprmScale(snapVlcDecodeAddr,decHSize,decVSize,snapQvAddr,snapQvHSize,snapQvVSize,snapJpegVlcEndAddr,1);
				}
			}
			else{
				if ((decHSize != snapQvHSize) || (decVSize != snapQvVSize)){
					HAL_GprmScale(snapVlcDecodeAddr,snapVlcDecodeHSize,snapVlcDecodeVSize,snapQvAddr,snapQvHSize,snapQvVSize,snapJpegVlcEndAddr,1);
				}
				else{
					HAL_GprmCopy(snapVlcDecodeAddr,snapVlcDecodeHSize,snapVlcDecodeVSize,0,0,
								snapQvAddr,snapQvHSize,snapQvVSize,0,0,snapQvHSize,snapQvVSize,0,1);
				}
			}

			//dramFileSave("QV000000.YUV",snapQvAddr,(UINT32)SNAP_CDSP_QV_HSIZE*SNAP_CDSP_QV_VSIZE*2);

			//sp1kHalCtrlScreen(3);

			//printf("QvHSize=%d,%d\n",snapQvHSize,snapQvVSize);

			snapBurstBreakDown( &snapDoQvYuvEndTime );
			snapPrintfBurstBreakDown( "S->DQYS,DQYE:%lu,%lu\n", snapDoQvYuvStartTime - snapStartTime, snapDoQvYuvEndTime - snapStartTime );
			snapBurstStoreFirstBreakDown( &snapDoQvYuvEndTime );
			snapPrintfBurstStoreFirstBreakDown( "S->DQYS,DQYE:%lu,%lu\n", snapDoQvYuvStartTime - snapStartTime, snapDoQvYuvEndTime - snapStartTime );

			if ( qv ) {
				if ( snapQvSmallDefQvSizeOpt == 1 ) {
					dispShow(snapQvSmallDefQvBufAddr, snapQvSmallDefQvSizeHsize, snapQvSmallDefQvSizeVsize, 100);/* will:  show QV on the pannel */
				}
				else {
					dispShow(((snapRatioOpt) ? qvYuvBufAddBk : snapQvAddr), snapQvYuvBufHSize, snapQvYuvBufVSize, 100);/* will:  show QV on the pannel */
				}
				//dispShow(snapQvAddr, snapQvHSize, snapQvVSize, 100);/* will:  show QV on the pannel */


			}

			if ( snapBurstJpegVlcAddr[0] == snapJpegVlcAddr ) {
				HAL_GlobalReadGTimer( &snapQvTime );
			}

		}

		#if 0//linjie fixed, not need do qv ds again
		if (snapFlowOpt != CDSP_DO_FRONT){
			snapStampQvPrepare();
		}
		#endif

		if ( snapRatioOpt ) {
			snapQvAddr = qvYuvBufAddBk;
		}

		//if ((snapYuvHSize<SNAP_CDSP_QV_HSIZE)||(snapYuvVSize<SNAP_CDSP_QV_VSIZE)){
		//	qvHSize=snapYuvHSize;
		//	qvVSize=snapYuvVSize;
		//}
		//else{
		//	qvHSize=SNAP_CDSP_QV_HSIZE;
		//	qvVSize=SNAP_CDSP_QV_VSIZE;
		//}
	}

	/*********************************************/
	/*             thumbnail encode              */
	/*********************************************/
	// scaling down to thumbnail

	snapNormalBreakDown( &snapThbStartTime );
	snapBurstBreakDown( &snapThbStartTime );
	snapBurstStoreFirstBreakDown( &snapThbStartTime );

	if (snapNoQvThbOpt == 0) {

		snapPrintf("snapThbAddr=0x%lx\n",snapThbAddr);
		snapPrintf("jpg end addr=0x%lx\n",snapJpegVlcEndAddr);
		snapPrintf("qvh=%d,qvv=%d\n",snapQvHSize,snapQvVSize);

		ASSERT( snapJpegVlcEndAddr + ( (UINT32)snapQvYuvBufHSize* snapQvYuvBufVSize ) < (UINT32)snapThbAddr, 0 );

		//dramFileSave("QVYUV000.YUV",snapQvAddr,(UINT32)snapQvHSize*snapQvVSize*2);/*save thumbnail for test*/

		//HAL_GprmScale(snapQvAddr, snapQvHSize, snapQvVSize, snapThbAddr, SNAP_CDSP_THB_HSIZE, SNAP_CDSP_THB_VSIZE, snapJpegVlcEndAddr, 1);

		#if (FRONT_SNAP_BREAK_DOWN == 3)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);//thb yuv (Qv -> thb)
		#endif


		//HAL_GprmScale(snapQvAddr, snapQvHSize, snapQvVSize, snapThbAddr, SNAP_CDSP_THB_HSIZE, SNAP_CDSP_THB_VSIZE, snapJpegVlcEndAddr, 1);
		HAL_GprmScale(snapQvAddr, snapQvYuvBufHSize, snapQvYuvBufVSize, snapThbAddr, SNAP_CDSP_THB_HSIZE, SNAP_CDSP_THB_VSIZE, snapJpegVlcEndAddr, 1);

		#if (FRONT_SNAP_BREAK_DOWN == 3)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);//thb yuv end (Qv -> thb)
		#endif

		//dramFileSave("THBYUV00.YUV",snapThbAddr,(UINT32)SNAP_CDSP_THB_HSIZE*SNAP_CDSP_THB_VSIZE*2);/*save thumbnail for test*/

		//dramFileSave("VLC00001.BIN", snapJpegVlcAddr, snapMainVlcSize);

		snapThbRuningQIdx = snapThbQIdx = 85;
		//snapThbRuningQIdx = snapThbQIdx = 100;  /* FPGA verification */

		jpegQTableSet( snapThbQIdx, 0, 0, 1 );

		// thumbnail use same snapHFactor, snapVFactor

		//HAL_JpgZFactorSet( 100, 100, SNAP_CDSP_THB_HSIZE, SNAP_CDSP_THB_VSIZE, SNAP_CDSP_THB_HSIZE, SNAP_CDSP_THB_VSIZE, rotFlipOpt );
		HAL_JpgZFactorSet( 100, 100, SNAP_CDSP_THB_HSIZE, SNAP_CDSP_THB_VSIZE, SNAP_CDSP_THB_HSIZE, SNAP_CDSP_THB_VSIZE, rotFlipOpt );

		#if (FRONT_SNAP_BREAK_DOWN == 3)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);//thb vlc
		#endif

		HAL_JpegEncode( snapThbAddr, snapThbVlcAddr, 0, jpegOpt, 1 );

		//HAL_JpegReadEncodeSize( &snapThbVlcSize );
		snapThbVlcSize = snapVlcSizeGet();

		snapPrintf("Thb vlc Size = %lu\n",snapThbVlcSize);
		snapPrintf("Thb vlc end=0x%lx\n",(snapThbVlcAddr+snapThbVlcSize));

		//dramFileSave("VLC00001.BIN", snapJpegVlcAddr, snapMainVlcSize);
		//dramFileSave("SVLC0001.BIN", snapThbVlcAddr, snapThbVlcSize);

		#if (FRONT_SNAP_BREAK_DOWN == 3)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);//thb vlc end
		#endif

		snapTimeRec( (UINT16)__LINE__ );

		snapQvToPv( 0, (UINT16)__LINE__  );
	}
	else {
		snapThbRuningQIdx = snapThbQIdx = 85;
		//jpegQTableSet( snapThbQIdx, 0, 0, 1 );
		snapThbVlcSize = 0;
	}

	snapNormalBreakDown( &snapThbEndTime );
	snapBurstBreakDown( &snapThbEndTime );
	snapBurstStoreFirstBreakDown( &snapThbEndTime );

	snapPrintfBurstBreakDown( "S->ThbS,ThbE:%lu,%lu\n", snapThbStartTime - snapStartTime, snapThbEndTime - snapStartTime );
	snapPrintfBurstStoreFirstBreakDown( "S->ThbS,ThbE:%lu,%lu\n", snapThbStartTime - snapStartTime, snapThbEndTime - snapStartTime );

	/*********************************************/
	/*			  QuickView encode				          */
	/*********************************************/

	maxQvVlcSize = exifQvAvailableSizeGet( snapThbVlcSize ) - SNAP_3A_INFO_SIZE - SNAP_3A_INFO_OFFSET;

	//maxQvVlcSize = (snapJpegHeaderAddr - snapQvVlcAddr)<<1;

	snapPrintf("max qv vlc size=%ld\n",maxQvVlcSize);

	snapNormalBreakDown( &snapQvEncodeStartTime);
	snapBurstBreakDown( &snapQvEncodeStartTime );
	snapBurstStoreFirstBreakDown( &snapQvEncodeStartTime );

	snapQvQIdx = 85;
	snapQvRuningQIdx = snapQvQIdx + 1;

	if (snapNoQvThbOpt == 0) {

		do{
			snapQvRuningQIdx -= SNAP_QV_Q_ID_STEP;

			jpegQTableSet( snapQvRuningQIdx, 0, 0, 1 );

			ASSERT( ( snapQvYuvBufHSize <= SNAP_CDSP_QV_HSIZE ) && ( snapQvYuvBufVSize <= SNAP_CDSP_QV_VSIZE ), 0 );

			//HAL_JpgZFactorSet( 100, 100, snapQvHSize, snapQvVSize, SNAP_CDSP_QV_HSIZE, SNAP_CDSP_QV_VSIZE, rotFlipOpt );
			//HAL_JpgZFactorSet( 100, 100, snapQvHSize, snapQvVSize, SNAP_CDSP_QV_HSIZE, SNAP_CDSP_QV_VSIZE, rotFlipOpt );
			HAL_JpgZFactorSet( 100, 100, snapQvYuvBufHSize, snapQvYuvBufVSize, SNAP_CDSP_QV_HSIZE, SNAP_CDSP_QV_VSIZE, rotFlipOpt );

			#if (FRONT_SNAP_BREAK_DOWN == 3)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);//Qv vlc
			#endif

			HAL_JpegEncode( snapQvAddr, snapQvVlcAddr, 0, jpegOpt, 1 );

			snapQvVlcSize=snapVlcSizeGet();
			snapPrintf("qv vlc size=%lu\n",snapQvVlcSize);

			#if (FRONT_SNAP_BREAK_DOWN == 3)
			gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);//Qv vlc end
			#endif
		} while( snapQvVlcSize > maxQvVlcSize );
	}
	else {
		//jpegQTableSet( snapQvRuningQIdx, 0, 0, 1 );
		snapQvVlcSize = 0;
	}

	snapNormalBreakDown( &snapQvEncodeEndTime );
	snapBurstBreakDown( &snapQvEncodeEndTime );
	snapBurstStoreFirstBreakDown( &snapQvEncodeEndTime );

	snapPrintfBurstBreakDown( "S->QVES,QVEE:%lu,%lu\n", snapQvEncodeStartTime - snapStartTime, snapQvEncodeEndTime - snapStartTime );
	snapPrintfBurstStoreFirstBreakDown( "S->QVES,QVEE:%lu,%lu\n", snapQvEncodeStartTime - snapStartTime, snapQvEncodeEndTime - snapStartTime );

	snapNormalBreakDown( &snapPostEndTime );
	snapBurstBreakDown( &snapPostEndTime );
	snapBurstStoreFirstBreakDown( &snapPostEndTime );

	snapQvToPv( 0, (UINT16)__LINE__  );

	return SUCCESS;

}

/**
 * @fn        void snapJpegInit(void)
 * @brief     initialize vlc buffer size and q table for snap
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapJpegInit(
	void
)
{
	UINT8 rotFlipOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_ROT_FLIP_OPT ]);
	UINT8 postBrc = (UINT8)(snapParam[ SP1K_SNAP_PARAM_POST_BRC ]);


	// cdsp/jpeg do main image
	//HAL_JpgZFactorSet(100, 100, snapYuvHSize, snapYuvVSize, snapHSize, snapVSize, rotFlipOpt );


	//if( snapPostBRCFinalF == 0 ) {
	//	snapMainRuningQIdx -= SNAP_MAIN_Q_ID_STEP;
	//}

	jpegQTableSet( snapMainRuningQIdx, 0, 0, 1 );

	if( postBrc == 1 ) {
		if( snapYuvABBufOpt == SNAP_TWO_YUVBUF) {
			if( snapPostBRCFinalF == 0 ) {
				if( snapMaxTargetVlcSize <= snapNonOverlapVlcBufSize ) {
					HAL_JpegVlcBoundSet( 1, 0,snapMaxTargetVlcSize );	// turn on BRC
				}
				else {
					HAL_JpegVlcBoundSet( 1,0, snapNonOverlapVlcBufSize );	// turn on BRC
				}
			}
			else {
				HAL_JpegVlcBoundSet( 0, 0, 0 );
			}
		}
		else {
			HAL_JpegVlcBoundSet( 1, 0,snapNonOverlapVlcBufSize );	// turn on BRC
		}
	}
	else {
		//HAL_JpegVlcBoundSet( 0, 0,0 );
		//HAL_JpegVlcBoundSet( 1, (snapRawDataAddr-snapJpegVlcAddr)<<1);
		//printf(" Max VLC Size=%ld\n",snapMaxTargetVlcSize);
		//snapMaxTargetVlcSize= (UINT32)0x2DC6C0; 	/*VLC 3M Bytes limitation */
		HAL_JpegVlcBoundSet( 1, 0,snapMaxTargetVlcSize);

	}

}



/**
 * @fn        UINT8 snapStore(void)
 * @brief     save jpg file
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
extern UINT32 exifGPSTagTest(const exifJpgFileInfo_t *jpgInfo);
UINT8
snapStore(
	dateStc_t* pRtc
)
{
	UINT8 fileName[ 12 ];//= "SUNP0000.JPG";
	//UINT8 testName[ 12 ];//= "SUNP0000.JP2";
	UINT8 sts;
	UINT8 saveJpgOpt;
	UINT32 timelapseHdrSize=0x104;
	UINT32 tempExifHdrAddr;
	UINT16 fileIdx;
	exifJpgFileInfo_t exifJpg;
	UINT32 awbAddrW;
#if STILL_SNAP_ON
    UINT16 stillsnapH,stillsnapW;
	UINT32 Filesize;
    #if 1 //for test
	UINT8 testName[ 12 ]= "SUNP0000.JPG";
	#endif
#endif

	UINT32 addr;
	UINT8 tlsMode = 0;
	UINT32 mmsHdrSize;

    //UINT32 tempExifHdrAddr;   /* will : mark it temporarily*/

	saveJpgOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_SAVE_JPG_OPT ]);
	if(1 != snapParam[SP1K_SNAP_TIMESLAPE_PAUSE] && snapParam[SP1K_SNAP_TIMESLAPE_STS]){
		tlsMode = 1;
	}

	if (snapVLCOverflowGet()==1){
		return FAIL;
	}

	if (saveJpgOpt!=0){
		/*********************************************/
		/*			 generate jpeg file 			 */
		/*********************************************/
		tempExifHdrAddr = snapJpegVlcEndAddr;
		tempExifHdrAddr = ( tempExifHdrAddr + 3 ) >> 2 << 2;

		//printf("exif addr=0x%lx\n",tempExifHdrAddr);
		#ifdef SNAP_FILL_3A_INFO_TO_HEADER
		//aaaFillInfo( snapJpegVlcAddr - ( SNAP_3A_INFO_SIZE >> 1 ) );
		awbAddrW = awbFileInfo(snapJpegVlcAddr - (( SNAP_3A_INFO_SIZE + SNAP_3A_INFO_OFFSET)>> 1 ));//wangjg++
		exifJpg.dbgInfoAddr = awbAddrW;
		exifJpg.dbgInfoSize = 4096;
		#endif
		snapInitExifParam(&exifJpg);

		snapQvToPv( 0, (UINT16)__LINE__  );
		snapTimeRec( (UINT16)__LINE__ );

		//EXIFHDR_Initialize_AllExifHeader( tempExifHdrAddr );

		//snapTimeRec( (UINT16)__LINE__ );

		/* exif time sync stamp time */
		if ( snapStampOpt ) {
			exifRtcSet( pRtc );
		}
		snapHdrSize = exifCreateFile(&exifJpg);
		//snapHdrSize = exifGPSTagTest(&exifJpg);

		/* exif time sync stamp time */
		if ( snapStampOpt ) {
			exifRtcSet( NULL );
		}

		//snapTimeRec( (UINT16)__LINE__ );

		ASSERT( snapHdrSize < ( ( snapJpegVlcAddr - snapQvAddr ) << 1 ), 0 );

		#ifdef SNAP_FILL_3A_INFO_TO_HEADER
			//aaaFillInfo( snapJpegVlcAddr - ( SNAP_3A_INFO_SIZE >> 1 ) );


		#endif

		snapTimeRec( (UINT16)__LINE__ );

		/*will: don't trun on preivew before finishing saving*/
		//snapQvToPv( SNAP_WRITE_FILE_TIME, (UINT16)__LINE__	);

		//snapQvToPv( 0, (UINT16)__LINE__  );

		#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
			icatSnapJpgUpload(snapJpegVlcAddr - ( snapHdrSize >> 1 ), snapMainVlcSize + snapHdrSize);
		#endif

#if STILL_SNAP_ON
                //cx add for still pin snap
    if( G_ucStillImageTriggerFlag==2)
    {
        //   printf("cxhe entry snapstore\n");
        Filesize=snapMainVlcSize + snapHdrSize;
		// fileaddr=snapJpegVlcAddr - ( snapHdrSize >> 1 );
		//	printf("file size=%ldn",Filesize);
#if 1  	//for test
        sts = dramFileSave( testName, snapJpegVlcAddr - ( snapHdrSize >> 1 ),Filesize);
#endif
                     stillsnapH=G_usPostJpegHeight;
                     stillsnapW=((Filesize/(G_usPostJpegHeight*2))+1);
                     printf("snapstor w h=%d,%d\n",stillsnapW,stillsnapH);
		// sts = dramFileSave( testName, snapJpegVlcAddr - ( snapHdrSize >> 1 ),Filesize);

		HAL_DramRawFrmBufAddrSet(snapJpegVlcAddr - ( snapHdrSize >> 1 ));
		HAL_DramRawFrmBufSizeSet(stillsnapH, stillsnapW);

        return SNAP_TASK_SUCCESS;
        //µØÖ·       snapJpegVlcAddr - ( snapHdrSize >> 1 )
        //ÎÄ¼þsize  snapMainVlcSize + snapHdrSize
    }
#endif

		#if defined(AMCAP_OPTION)
			return SUCCESS;
		#endif

		#ifdef MULTI_FOLDER_AS_ONE	//xian ++ 20080806
			if(USR_LastFolder(0)==SUCCESS) //go to the last folder
				DOS_FileOrderTblInit(void)();
		#endif
		if(!tlsMode){
			sts = dcfFileIndexUpdate();	//xian ++ 20081203
			if(sts != TRUE ){
				ASSERT(0, 1);
				return FAIL;
			}
			//dcfJpgNameGet(fileName, dcfFileIndexGet());
			fileIdx = dcfFileIndexGet();
			dcfMediaNameGet(fileName, fileIdx,MEDIA_JPG);
			fileName[11] = 0x00;
			//dcfJpgNameGet(testName,dcfFileIndexGet());
			//testName[11]=0;
			//testName[3]='_';
			//printf("save file %s\n",testName);
			//sts=dramFileSave(testName,snapJpegVlcAddr - ( snapHdrSize >> 1 ), (UINT32)snapHdrSize );
			//printf("save 0x%bx\n",sts);
			//HAL_GlobalTimerWait(1000);

			//printf("\nsave file %s\n",fileName);
			sts = dramFileSave( fileName, snapJpegVlcAddr - ( snapHdrSize >> 1 ), snapMainVlcSize + (UINT32)snapHdrSize );
			//printf("save 0x%bx\n",sts);

			//G_USR_NewFileSizeInFdb = snapMainVlcSize + (UINT32)snapHdrSize;

			snapTimeRec( (UINT16)__LINE__ );
		}else{
			sts = TRUE;
		}

		if( sts != TRUE ) {
			printf( "Wr err[%bd]\n", sts );
			vfsFileDelete(fileName); //xian ++ 20090717, #38169
			return SP1K_SNAP_MSG_FILE_WRITE_ERROR;
		}
		else {
		#if 1 //xian add new api
				if(!tlsMode){
					dcfNewFileDramFdbParameterUpdate(K_DCF_JPG_TYPE, snapMainVlcSize + (UINT32)snapHdrSize);
				}
		#else
				G_USR_NewFileSizeInFdb = snapMainVlcSize + (UINT32)snapHdrSize;
				dcfFileDramFdbParameterWrite(K_DCF_JPG_TYPE,G_USR_FileIndex);
				G_USR_LastFileIndex = G_USR_FileIndex; //yichang@0510

				//Fix file index bug begin
				if (G_USR_FileCount == 0) {
				G_USR_FirstFileIndex = G_USR_FileIndex;
				}
				//Fix file index bug end

			#ifdef MULTI_FOLDER_AS_ONE	//xian ++ 20080806
					USR_UpdateCurFolderInfo(K_UPDATE_MODE_ADD_ONE);  //xian ++ 20080806
			#endif
				G_USR_FileIndex ++;
				G_USR_FileCount++;
				//DOS_FileOrderWrite(G_USR_FileCount , G_USR_LastFileIndex); //will mark
		#endif
			//return SP1K_SNAP_MSG_OK;
		}
	}
	/*timelapse funciton */
	if( tlsMode ){
		sts = SUCCESS;
		/* add frame header */
		addr = snapJpegVlcAddr -(timelapseHdrSize>>1);
		timelapse_AddFrameHdr(addr,snapHSize,snapVSize,snapMainRuningQIdx);
		if(snapParam[SP1K_SNAP_TIMESLAPE_STS] == 1){
			sts = timelapse_Entry(addr, snapHSize, snapVSize,snapMainVlcSize+timelapseHdrSize); // reserve 1024bytes
			if( sts != SUCCESS){
				snapParam[SP1K_SNAP_TIMESLAPE_STS] = 0;
				return SP1K_SNAP_MSG_FILE_WRITE_ERROR;
			}
			snapParam[SP1K_SNAP_TIMESLAPE_STS] = 2;
			return SP1K_SNAP_MSG_OK;
		}

		if(snapParam[SP1K_SNAP_TIMESLAPE_STS] == 2){
			sts = timelapse_AppendFrame(addr,snapMainVlcSize+timelapseHdrSize);
			if( sts == FAIL ){
				printf("append err!\n");
				snapParam[SP1K_SNAP_TIMESLAPE_STS] = 1;
				return SP1K_SNAP_MSG_OK;
			}else if(sts == TIMELAPSE_CARDFULL){
				snapParam[SP1K_SNAP_TIMESLAPE_STS] = 0;
				return TIMELAPSE_CARDFULL;
			}else if(sts == TIMELAPSE_MAX_FILESIZE){
				snapParam[SP1K_SNAP_TIMESLAPE_STS] = 1;
				return SP1K_SNAP_MSG_OK;
			}else{
				return SP1K_SNAP_MSG_OK;
			}
		}
	}

	/* MMS function */
	if(snapParam[SP1K_SANP_PARAM_MMS_STS]){
		mmsHdrSize = exifAddMmsHdr(snapThbVlcAddr,snapThbVlcSize,160,120, snapThbRuningQIdx);
		dcfMmsFileNameGet(fileName,fileIdx);
		dcfEntryMmsDir();
		sts = dramFileSave( fileName, snapThbVlcAddr-(0x254>>1), snapThbVlcSize+mmsHdrSize);
		dcfExitMmsDir();
	}

	return SUCCESS;


}

/**
 * @fn        UINT8 snapYuvSave(void)
 * @brief     save yuv (if single yuv buffer option is enable)
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
snapYuvSave(
	void
)
{
#if 0
	UINT8 saveYuvOpt;
	UINT8 snapChgRawOpt;
	UINT8 sts;
	UINT32 imgSize;
	UINT8 filename[ 12 ]= "SUNP0000YUV";


	saveYuvOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_SAVE_YUV_OPT ]);
    snapChgRawOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_CHANGERAW ]);

	if (saveYuvOpt==1){

	    //dcfJpgNameGet(f, dcfFileIndexGet());  //will mark

		//imgsize  = (UINT32)snapYuvHSize*(UINT32)snapYuvVSize*2;
		imgSize  = (UINT32)snapYuvHSize*(UINT32)snapYuvVSize*2;

		if (snapChgRawOpt==1){    /* will: compare answer */
            cdspBinFileName[7]  = 'W';

			cdspBinFileName[8]  = 'Y';
			cdspBinFileName[9]  = 'U';
			cdspBinFileName[10] = 'V';

			ENTER_CRITICAL(sts);

			if(dramFileSave(cdspBinFileName, snapYuv1Addr, imgSize)){
		   		snapTaskYuvCnt++;
		   		snapPrintf("Save YUV orrect\n");
			}

			/*
			if(dramFileSave(cdspBinFileName, (snapYuv1Addr+(160*2*2)), (imgsize-(160*2*4)))){
				printf("Save YUV orrect\n");
				snapTaskYuvCnt++;
			}
			*/
			EXIT_CRITICAL(sts);
		}
		else {
			filename[ 4 ] = '0' + snapTaskYuvCnt / 1000;
			filename[ 5 ] = '0' + (snapTaskYuvCnt / 100)-((snapTaskYuvCnt / 1000)*10);
			filename[ 6 ] = '0' + (snapTaskYuvCnt / 10)-((snapTaskYuvCnt / 1000)*100)-(((snapTaskYuvCnt % 1000)/100)*10);
			filename[ 7 ] = '0' + snapTaskYuvCnt % 10;


			filename[ 8 ] = 'Y';
			filename[ 9 ] = 'U';
			filename[ 10 ] = 'V';
			filename[ 11 ] = 0x20;

	    	ENTER_CRITICAL(sts);
			if(dramFileSave(filename, snapYuv1Addr, imgSize)){
		   		snapTaskYuvCnt++;
   	   			snapPrintf("Save YUV correct\n");
			}
			EXIT_CRITICAL(sts);
		}
        //printf("start saving YUV\n");
	}
#endif

	#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)

		icatSnapYuvUpload(snapYuv1Addr, (UINT32)snapYuvHSize*(UINT32)snapYuvVSize*2);

	#endif

	return SUCCESS;
}

/**
 * @fn        UINT8 snapRawSave(void)
 * @brief     save raw data (8 bits)
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/

static UINT8
snapRawSave(
	void
)
{
	UINT8 filename[ 12 ]="SUNP0000RAW";
	//dcfJpgNameGet(filename, G_USR_FileIndex);
	dcfMediaNameGet(filename,G_USR_FileIndex,MEDIA_JPG);
	filename[ 8 ] = 'R';
	filename[ 9 ] = 'A';
	filename[ 10 ] = 'W';
	filename[ 11 ] = 0x20;
	//filename[ 12 ] = 0x00;

	//printf( "Raw data size %d x %d\n", snapRawHSize, snapRawVSize );
	//printf( "Raw data size %d x %d\n", snapSensorHSize, snapSensorVSize );

	if( snapRawMode == CDSP_SNAP_8RAW ) {
		dramFileSave( filename, snapRawDataAddr, (UINT32)snapRawHSize * snapRawVSize );
		//dramFileSave( filename, snapRawDataAddr, (UINT32)snapSensorHSize * snapSensorVSize );
	}
	else {
		dramFileSave( filename, snapRawDataAddr, (UINT32)snapRawHSize * snapRawVSize * 2 );
		//dramFileSave( filename, snapRawDataAddr, (UINT32)snapSensorHSize * snapSensorVSize *2);

	}
	snapTaskRawCnt++;

	////////////for dark sub /////////////
	#if 0
	filename[ 4 ] = '0' + snapTaskRawCnt / 1000;
	filename[ 5 ] = '0' + (snapTaskRawCnt / 100)-((snapTaskRawCnt / 1000)*10);
	filename[ 6 ] = '0' + (snapTaskRawCnt / 10)-((snapTaskRawCnt / 1000)*100)-(((snapTaskRawCnt % 1000)/100)*10);
	filename[ 7 ] = '0' + snapTaskRawCnt % 10;


	filename[ 8 ] = 'R';
	filename[ 9 ] = 'A';
	filename[ 10 ] = 'W';
	filename[ 11 ] = 0x20;
	//f[ 12 ] = 0x00;

	//printf( "Raw data size %d x %d\n", snapRawHSize, snapRawVSize );

	if( snapRawMode == CDSP_SNAP_8RAW ) {
		dramFileSave( filename, (UINT32)0x331c00, (UINT32)snapRawHSize * snapRawVSize );
	}
	else {
		dramFileSave( filename, (UINT32)0x60000, (UINT32)snapRawHSize * snapRawVSize * 2 );
	}
	snapTaskRawCnt++;

	#endif
	return SUCCESS;
}

static UINT8
snapChangeRaw(
	void
)
{

	UINT32 size;

	#ifndef COMPARE_ANSWER
		if(!dramFileLoad(chgRawFileName, snapRawDataAddr, &size)) {
			snapPrintf("Change RAW error \n");
		}
		else{
			snapPrintf("Change RAW ok \n");
		}

	#else
		/**************************************************/
	 	/* Change raw data for Compare Answer               */
	 	/**************************************************/
		dosCdCmd("SRC");
		if(!dramFileLoad(chgRawFileName, snapRawDataAddr, &size)) {
		    snapPrintf("Change RAW error \n");
		}
		//else{
		//	DBG_PRINTF("Change RAW ok \n");
		//}
		dosCdCmd("..");


		/**************************************************/
	 	/* Set Parameter for Compare Answer               */
	 	/**************************************************/

		dosCdCmd("PAR");
		cdspBinRead(cdspBinFileName, K_SDRAM_CommonEndAddr );
		dosCdCmd("..");

		HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 3);
	#endif

	return SUCCESS;
}

/**
 * @fn        void snapInitExifParam(exifJpgFileInfo_t *exifJpg)
 * @brief     initialize exif for snap
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapInitExifParam(exifJpgFileInfo_t *exifJpg)
{
	UINT8 rotFlipOpt;

	if( snapParam[SP1K_SNAP_PARAM_FORMAT] == SP1K_SNAP_FORMAT_422 ) {
		exifJpg->fileFormat = K_File_DataType_YUV422;
	}
	else {
		exifJpg->fileFormat = K_File_DataType_YUV420;
	}
	rotFlipOpt = (UINT8)snapParam[SP1K_SNAP_PARAM_ROT_FLIP_OPT];
	exifJpg->mainVlcAddrW = snapJpegVlcAddr;
	exifJpg->qvVlcAddrW = snapQvVlcAddr;
	exifJpg->thbVlcAddr = snapThbVlcAddr;

	exifJpg->mainQidx = snapMainRuningQIdx;
	exifJpg->qvQidx = snapQvRuningQIdx;
	exifJpg->thbQidx = snapThbRuningQIdx;

	exifJpg->qvSize = snapQvVlcSize;
	exifJpg->thbSize = snapThbVlcSize;
	exifJpg->qvCropWidth = SNAP_CDSP_QV_HSIZE;
	exifJpg->qvCropHeight = SNAP_CDSP_QV_VSIZE;

	if( rotFlipOpt & 0x01 == 0x01) {
		exifJpg->mainWidth = snapHSize;
		exifJpg->mainHeight = snapVSize;

		exifJpg->qvWidth = SNAP_CDSP_QV_VSIZE;
		exifJpg->qvHeight = SNAP_CDSP_QV_HSIZE;

		exifJpg->thbWidth = SNAP_CDSP_THB_VSIZE;
		if(exifJpg->qvCropWidth * 3 > exifJpg->qvCropWidth *4 ){
			exifJpg->thbWidth = ((UINT16)SNAP_CDSP_THB_VSIZE*3/4+7)>>3<<3;
		}

		exifJpg->thbHeight = SNAP_CDSP_THB_HSIZE;
	}
	else {
		exifJpg->mainWidth = snapHSize;
		exifJpg->mainHeight = snapVSize ;

		exifJpg->qvWidth = SNAP_CDSP_QV_HSIZE;
		exifJpg->qvHeight = SNAP_CDSP_QV_VSIZE;

		exifJpg->thbWidth = SNAP_CDSP_THB_HSIZE;
		exifJpg->thbHeight = SNAP_CDSP_THB_VSIZE;

		if(exifJpg->qvCropWidth * 3 > exifJpg->qvCropHeight*4 ){
			exifJpg->thbHeight = ((UINT16)SNAP_CDSP_THB_VSIZE*3/4+7)>>3<<3;
		}
	}

	exifJpg->rotOpt = 1;
	exifJpg->viViLinkEn = 0;
}

/**
 * @fn        UINT8 snapQvToPv(UINT16 margin, UINT16 pos)
 * @brief     snapQvToPv
 * @param     [in] margin
 * @param     [in] pos : code line
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
snapQvToPv(
	UINT16 margin,
	UINT16 pos
)
{
	UINT32 currentTime;
	UINT8 skipPVOpt = (UINT8)snapParam[ SP1K_SNAP_PARAM_SKIP_PV ];

	snapPrintf("opt=%bu,flag=%bu\n",snapBurstOpt,snapBackToPv);

	if( snapBurstOpt == 0 && snapBackToPv == 0 ) {
		if( (UINT8)(snapParam[ SP1K_SNAP_PARAM_QV ]) == 0 ) {

			snapPvTurnOn();
			snapBackToPv = 1;

			//printf( "[%d] ct %ld\n", pos, currentTime );
		}
		else {

			HAL_GlobalReadGTimer(&currentTime);

            //xian ++ 20090722, fix bug #37897
            if(currentTime < snapQvShowTime){
                currentTime += MAX_G_TIME_MS;
            }
            //xian --
            //printf("pos=%d\n",pos);
			//printf("snapQvShowTime=%ld\n",snapQvShowTime);
			//printf("currentTime=%ld\n",currentTime);

			if( skipPVOpt == 1 || ( snapQvShowTime + (UINT32)(snapParam[ SP1K_SNAP_PARAM_QV_PERIOD ]) < currentTime + (UINT32)margin + SNAP_BACK_PV_OVERHEAD ) ) {
				snapPvTurnOn();
				sp1kHalCtrlScreen(3); /*show image and osd*/
				snapBackToPv = 1;

				snapPrintfTimeToPV( "[%d] ct %ld\n", pos, currentTime );
				pos = pos;
			}


		}
	}
	return snapBackToPv;
}

/**
 * @fn        UINT8 snapBurstToPv(void)
 * @brief     snapBurstToPv
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
snapBurstToPv(
	UINT16 pos
)
{

#if 1
	if( snapBurstOpt == 1 ) {
		if( ( psnapBurst == NULL ) || ((*psnapBurst)( &snapBurstCnt ) == FALSE) || (snapBurstVlcOverflow==1) ) {
			if( snapBackToPv == 0 ) {
				snapPvTurnOn();

				snapBackToPv = 1;

				snapTimeRec( (UINT16)__LINE__ );
				pos=pos;
				//printf("burst to PV %d\n",pos);
			}
		}
	}

	osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);  // phil for flushing msg unused
#else

	if( snapBurstOpt == 1 ) {
		if( ( psnapBurst == NULL ) || (*psnapBurst)( &snapBurstCnt ) == FALSE ) {
			if( snapBackToPv == 0 ) {
				snapPvTurnOn();

				snapBackToPv = 1;

				snapTimeRec( (UINT16)__LINE__ );
				printf("burst to PV %d\n",pos);
			}
		}
		else {

			osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);  // phil for flushing msg unused
		}
	}
	else {

		osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);  // phil for flushing msg unused
	}
#endif

	return snapBackToPv;
}


/**
 * @fn        UINT8 snapTurnOnPv(void)
 * @brief     Go back to preview state
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
UINT8
snapPvTurnOn(
	void
)
{
	UINT8 skipPVOpt = (UINT8)snapParam[ SP1K_SNAP_PARAM_SKIP_PV ];
	UINT8 pvFreezeOpt = (UINT8)snapParam[ SP1K_SNAP_PARAM_PV_FREEZE_OPT ];
	UINT8 qV = (UINT8)snapParam[ SP1K_SNAP_PARAM_QV ];
	UINT16 hSize,vSize;
	UINT16 factor;
	UINT8 zoomType;
	//UINT16 para1, para2;
	//UINT16 i;

	//HAL_CdspRstBlock(CDSPRstCDSP);/* Rst CDSP */

	if( skipPVOpt == 1 ) {
		return 0;
	}

	snapNormalBreakDown( &snapStartPvTime );
	snapBurstBreakDown( &snapStartPvTime );
	snapBurstStoreFirstBreakDown( &snapStartPvTime );

	if ( !snapPanoramaOpt ) {
		cdspPreProcessReload();
		cdspPostProcessCtrl(1,0);		/* preview mode */
	}
	pvSizeGet(&hSize,&vSize);

	//printf("turn on sysmode=%bd\n",osSysModeGet());
	zoomDevActGet(SP1K_ZOOM_LOGI_STILL_MODE,&zoomType);
	if(zoomType==ZOOM_LOGI_BLOCKING_TYPE){
		if ( snapRatioOpt == 1 ) {
			//dispImgWinEn(0); // hlc, Mantis #0021510
			XBYTE[0x2de1] = 0x02;
		}
	}
	else{
	//linjiecheng mask, because 4:3 and 16:9 dzoom 2.0~4.0 snap to pv display factor error issue
	//if ( snapRatioOpt == 1 ) {
		//dispImgWinEn(0); // hlc, Mantis #0021510
		XBYTE[0x2de1] = 0x02;
	//}
	}
	HAL_Cdsp2ndPathCfg(1,0,0);
	if (osSysModeGet()==OS_SYS_MODE_VIDEO_PV){
		#if 0
		#if (STILL_NEW_ZOOM == 0)
		sp1kPreviewSet(1, hSize, vSize, PvZoomCtrl.factor, 1);
		#else
		sp1kPreviewSet(1, hSize, vSize, videoZoomFctGet_NBlocking(), 1);
		#endif
		#endif

		zoomDevZoomFactorGet(ZOOM_LOGI_VIDEO_MODE,&factor);
		sp1kPreviewSet(1, hSize, vSize, factor, 1);
	}
	else{
		#if 0
		#if (STILL_NEW_ZOOM == 0)
		//sp1kPreviewSet(0, hSize, vSize, PvZoomCtrl.factor, 1);
		sp1kPreviewSet(0, hSize, vSize, PvZoomCtrl.factor, 1);
		#else
		sp1kPreviewSet(0, hSize, vSize, pvZoomFctGet_NBlocking(), 1);
		#endif
		#endif

		zoomDevZoomFactorGet(ZOOM_LOGI_STILL_MODE,&factor);
		sp1kPreviewSet(0, hSize, vSize, factor, 1);
	}
	// TODO : restore AE setting here

	if( G_calibrationMode == 0 ) {
		aeForceSet();
	}


	//xian mask, snap will wait 3A, 20081205
	//#ifdef ICAT_OPTION
	    //if(sp1kSnapIcatMode == 1){
			//aeModeEnableSet( 0 );
			//AAA_AWBModeEnable( 0 );
			//sp1kAwbDisable();
		//}
	//#else


	#if 0
	aeModeEnableSet( snapAeMode );

	if (snapAwbMode == 0){ /* disable */
		sp1kAwbDisable();
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,0xff);
	}
	else {
		sp1kAwbEnable();
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,0x00);
	}
 	#endif

 	//#if (STILL_NEW_ZOOM == 0)
 	if(zoomType==ZOOM_LOGI_BLOCKING_TYPE){
		if( pvFreezeOpt == 0 ) {
			sp1kPrevewUnfreeze(0);  /*will: It can't start 1st path here, because it might overwrite jpg data in dram*/
			if ( snapRatioOpt == 1 ) {
				XBYTE[0x2de1] = 0x03;
			}
		}
 	}else{
		if( pvFreezeOpt == 0 ) {
			//linjiecheng mask, because 4:3 and 16:9 dzoom 2.0~4.0 snap to pv display factor error issue
			//if ( snapRatioOpt == 1 ) {
				HAL_DramZUpdateWait(HAL_DRAM_ZUPDATE_PV, 300);
				XBYTE[0x2de1] = 0x03;
			//}

			sp1kPrevewUnfreeze(0);  /*will: It can't start 1st path here, because it might overwrite jpg data in dram*/
		}
 	}

	if( qV != 1 && snapBurstOpt != 1 ) { // if QV is not on
		dispImgWinEn(0); // hlc, Mantis #0021510
	}


	//sp1kHalCtrlScreen(2);	/* Show OSD, turn off image layer*/mark for mantis #41367 by dong
	HAL_GlobalReadGTimer( &snapEndPvTime );

	snapTimeRec( (UINT16)__LINE__ );

	return SUCCESS;
}


static UINT8
snapAeAwbTurnOn(
	void
)
{
	//UINT16 para1, para2;
	UINT8 val;
	aeModeEnableSet( snapAeMode );

	if (snapAwbMode == 0){ /* disable */
		sp1kAwbDisable();
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,0xff);
	}
	else {
		awbCMSet(AWB_CM_D);
		sp1kAwbEnable();

		HAL_CdspVDramSkipWrGet(&val);
		HAL_CdspVDramSkipWrSyncSet(0);
		HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWB,0x00);
		HAL_CdspVDramSkipWrSyncSet(val>>4);
	}

	#if 0 //wangjg--
	if( snapAeMode == 1 ) {

		frontDeviceInfoGet( 0, FRONT_DEV_INFO_DATA_FMT, &para1, &para2 );

		// FIXME: YUV should turn off AE/AWB
		#ifdef AMCAP_OPTION
			if( sp1kSnapIcatMode == 0 && para1 != FRONT_DATA_FMT_YUV ) { // YUV sensor
				//wangjg++
				//while( !( G_AEReady == 0 && G_AESetting == 0 ) );
			}
		#else
			if( para1 != FRONT_DATA_FMT_YUV ) { // YUV sensor
				//wangjg++ [40831] waiting is not  necessary,sometime hung up.
				//while( !( G_AEReady == 0 && G_AESetting == 0 ) );
			}
		#endif
	}
	#endif

	return SUCCESS;
}


/**
 * @fn        UINT8 snapRawCB(UINT32 rawDataAddr, UINT16 rawXSize, UINT16 rawYSize, UINT8 rawMode)
 * @brief     raw data calibration (post white balance, bad pixel compensation)
 * @param     [in] rawDataAddr : raw data dram address
 * @param     [in] rawHSize : raw horizontal size
 * @param     [in] rawVSize : raw vertical size
 * @param     [in] rawMode :
 *\n						0: 8  bits raw data
 *\n						1: 16 bits raw data
 *\n						2: YUV data
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
snapRawCB(
	UINT32 rawDataAddr,
	UINT16 rawHSize,
	UINT16 rawVSize,
	UINT8 rawMode
)
{
	UINT8 badPixelOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_BAD_PIXEL_OPT ]);
	UINT8 preProcessMode = (UINT8)(snapParam[ SP1K_SNAP_PARAM_REDO_PREPROCESS]);
	UINT8 snapFlowOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DOFLOW_OPT ]);

	if(( badPixelOpt == SNAP_BP_OFFLINE )&&(snapFlowOpt != CDSP_DO_FRONT)) {
		//printf("bad pixel off line\n");
		cdspRawBadPixel( rawDataAddr, rawHSize, rawMode);
	}

	if( preProcessMode != SP1K_SNAP_PRE_REDO_DISABLE ){
		cdspRawProcess( rawDataAddr, rawHSize, rawVSize, rawMode, preProcessMode);
	}

	return SUCCESS;
}

/**
 * @fn        void snapStampPreInit(void)
 * @brief     scale font & scale background & load background
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapStampPreInit(
	void
)
{
	UINT8 conditions;
	UINT32 bgMainAddrWord;
	UINT16 bgMainXSize, bgMainYSize;
	UINT32 bgQvAddrWord;
	UINT16 bgQvXSize, bgQvYSize;
	UINT32 tempBuffer;
	stamp_text_t *pDate;
	stamp_text_t *pTime;
	stamp_text_t *pTxt;

	/* conditions */
	conditions = ((snapBurstOpt == 0) || (snapBurstOpt == 1 && snapBurstCnt == 0));

	//printf("snapStampPreInit : c=%bu,sBOpt=%bu,sBC=%bu\n",conditions,snapBurstOpt,snapBurstCnt);

	/* read RTC */
	HAL_GlobalReadRTC( &snapDSInfo[snapBurstCnt] );
	stampTextModifyByRTC( &snapDSInfo[snapBurstCnt] );

	//printf("snapStampPreInit=%bu\n",snapBurstCnt);

	//printf("%bu/%bu/%bu,%bu:%bu:%bu\n",snapDSInfo.Year,snapDSInfo.Month,snapDSInfo.Day,snapDSInfo.Hour,snapDSInfo.Minute,snapDSInfo.Second);

	/* stamp pattern resource file load */
	if ( snapDateStampOpt && conditions ) {

		/* set stamp font load buffer */
		tempBuffer = snapQvAddr;

		if (snapNoQvThbOpt == 0) {
			/* DS pattern resource file load & scale to temp address (Qv) */
			//printf("Q=%d,%d\n",snapDateStampQvXSize,snapDateStampQvYSize);
			if (NULL != stampFontSet( snapQvStampFontPtnAddr, snapDateStampQvXSize, snapDateStampQvYSize, tempBuffer )) {
				//dramFileSave("QVFONT01.YUV",snapQvStampFontPtnAddr,(UINT32)snapDateStampQvXSize*DATE_STAMP_TEMP_NUM*snapDateStampQvYSize*2);
			}
			else {
				ASSERT(0,0);
			}
		}

		/* DS pattern resource file load & scale to temp address (Main) */
		//printf("M=%d,%d\n",snapDateStampMainXSize,snapDateStampMainYSize);
		if (NULL != stampFontSet( snapMainStampFontPtnAddr, snapDateStampMainXSize, snapDateStampMainYSize, tempBuffer )) {
			//dramFileSave("MAFONT01.YUV",snapMainStampFontPtnAddr,(UINT32)snapDateStampMainXSize*DATE_STAMP_TEMP_NUM*snapDateStampMainYSize*2);
		}
	}

	/* Bg stamp resource file load */
	if ( snapBgOpt && conditions ) {

		/* host Bg pattern resource file load to source yuv address (original size) */
		if (psnapBackgroundResLoad != NULL) {

			UINT32 parameter[2];
			parameter[0] = snapBgYuvAddr;
			parameter[1] = (UINT32)(((UINT32)snapBgHSize*snapBgVSize)<<1);

			if ((*psnapBackgroundResLoad)(&parameter[0]) == FAIL) {
				ASSERT(0,0);
			}
			//printf("Bg src h=%d,v=%d\n",snapBgHSize,snapBgVSize);
			//dramFileSave("BGDATE00.YUV",snapBgYuvAddr,(((UINT32)snapBgHSize*snapBgVSize)<<1));
		}

		if ( psnapHostStamp == NULL ) {
			/* set stamp font load buffer */
			tempBuffer = snapQvAddr;

			if (snapNoQvThbOpt == 0) {
				/* Bg (original size) scale to (Qv) Bg size */
				HAL_GprmScale(snapBgYuvAddr,
								snapBgHSize,
								snapBgVSize,
								snapQvBgAddr,
								snapBgQvXSize,
								snapBgQvYSize,
								tempBuffer,
								1);
			}
		}

		/* set stamp font load buffer */
		tempBuffer = snapQvAddr;

		/* Bg (original size) scale to (Main) Bg size */
		HAL_GprmScale(snapBgYuvAddr,
						snapBgHSize,
						snapBgVSize,
						snapMainBgTempAddr,
						snapBgMainXSize,
						snapBgMainYSize,
						tempBuffer,
						1);

		//printf("BgMain src h=%d,v=%d\n",snapBgMainXSize,snapBgMainYSize);
		//dramFileSave("BGTOMA00.YUV",snapMainBgTempAddr,(((UINT32)snapBgMainXSize*snapBgMainYSize)<<1));
	}

	/* set stamp position */
	if ( conditions ) {

		snapStampXRatio = (UINT16)snapParam[ SP1K_SNAP_PARAM_STAMP_X_RATIO ];
		snapStampYRatio = (UINT16)snapParam[ SP1K_SNAP_PARAM_STAMP_Y_RATIO ];

		snapStampQvXPos = (UINT16)( (UINT32)snapDsQvHSize * snapStampXRatio / SNAP_DATE_STAMP_MAX_RATIO );
		snapStampQvYPos = (UINT16)( (UINT32)snapDsQvVSize * snapStampYRatio / SNAP_DATE_STAMP_MAX_RATIO );

		snapStampMainXPos = (UINT16)( (UINT32)snapHSize * snapStampXRatio / SNAP_DATE_STAMP_MAX_RATIO );
		snapStampMainYPos = (UINT16)( (UINT32)snapVSize * snapStampYRatio / SNAP_DATE_STAMP_MAX_RATIO );

		/* Qv & Main sync (based on MCU, need 8x) */
		(snapStampQvXPos%8 == 0) ? snapStampQvXPos : (snapStampQvXPos -= (snapStampQvXPos%8));
		(snapStampQvYPos%8 == 0) ? snapStampQvYPos : (snapStampQvYPos -= (snapStampQvYPos%8));

		(snapStampMainXPos%8 == 0) ? snapStampMainXPos : (snapStampMainXPos -= (snapStampMainXPos%8));
		(snapStampMainYPos%8 == 0) ? snapStampMainYPos : (snapStampMainYPos -= (snapStampMainYPos%8));

		/* set main size,addr for kernel release position */
		if ( snapBgOpt ) {
			bgMainAddrWord = snapMainBgAddr;
			bgMainXSize = snapBgMainXSize;
			bgMainYSize = snapBgMainYSize;
			bgQvAddrWord = snapQvBgAddr;
			bgQvXSize = snapBgQvXSize;
			bgQvYSize = snapBgQvYSize;
		}
		else {
			bgMainAddrWord = snapMainDateStampAddr;
			bgMainXSize = snapDateStampMainXSize * DATE_STAMP_NUM;
			bgMainYSize = snapDateStampMainYSize;
			bgQvAddrWord = snapQvDateStampAddr;
			bgQvXSize = snapDateStampQvXSize*DATE_STAMP_NUM;
			bgQvYSize = snapDateStampQvYSize;
		}

		/* check boundary */
		((snapStampQvXPos + bgQvXSize) > snapDsQvHSize) ? snapStampQvXPos = (snapDsQvHSize - bgQvXSize) : snapStampQvXPos;
		((snapStampQvYPos + bgQvYSize) > snapDsQvVSize) ? snapStampQvYPos = (snapDsQvVSize - bgQvYSize) : snapStampQvYPos;
		((snapStampMainXPos + bgMainXSize) > snapHSize) ? snapStampMainXPos = (snapHSize - bgMainXSize) : snapStampMainXPos;
		((snapStampMainYPos + bgMainYSize) > snapVSize) ? snapStampMainYPos = (snapVSize - bgMainYSize) : snapStampMainYPos;

	}

	if (snapNoQvThbOpt == 0) {
		/* clear Qv ds addr context */
		gprmBlackFill(snapQvDateStampAddr,snapDateStampQvXSize*DATE_STAMP_NUM,snapDateStampQvYSize);
	}

	#if (SNAP_NORMAL_STAMP_WITH_HOST_STAMP == 1)
	/* clear host stamp addr context */
	gprmBlackFill(snapMainDateStampTempAddr,snapDateStampMainXSize*DATE_STAMP_NUM,snapDateStampMainYSize);
	#endif

	#if (SNAP_NORMAL_STAMP_WITH_HOST_STAMP == 1)
	/* callback for host User stamp */
	if ( conditions && psnapHostStamp != NULL )
	#else
	/* callback for host User stamp */
	if ( ( snapBgOpt && conditions ) && psnapHostStamp != NULL )
	#endif
	{
		/* set main size,addr for host release position */
		if ( snapBgOpt ) {
			bgMainAddrWord = snapMainBgTempAddr;
			bgMainXSize = snapBgMainXSize;
			bgMainYSize = snapBgMainYSize;
			bgQvAddrWord = snapQvBgAddr;
			bgQvXSize = snapBgQvXSize;
			bgQvYSize = snapBgQvYSize;
		}
		else {
			bgMainAddrWord = snapMainDateStampTempAddr;
			bgMainXSize = snapDateStampMainXSize * DATE_STAMP_NUM;
			bgMainYSize = snapDateStampMainYSize;
			bgQvAddrWord = snapQvDateStampAddr;
			bgQvXSize = snapDateStampQvXSize*DATE_STAMP_NUM;
			bgQvYSize = snapDateStampQvYSize;
		}

		stampBackgroundSet(bgMainAddrWord, bgMainXSize, bgMainYSize);

		if ((*psnapHostStamp)(NULL) == FAIL) {
			ASSERT(0,0);
		}
		else {
			/* set stamp font load buffer */
			tempBuffer = snapQvAddr;

			if (snapNoQvThbOpt == 0) {
				HAL_GprmScale( bgMainAddrWord,
								bgMainXSize,
								bgMainYSize,
								bgQvAddrWord,
								bgQvXSize,
								bgQvYSize,
								tempBuffer,
								1);
			}

			//printf("bgM=%d,%d, bgQ=%d,%d\n",bgMainXSize,bgMainYSize,bgQvXSize,bgQvYSize);

			//dramFileSave("HOSTSTMA.YUV",bgMainAddrWord,(((UINT32)bgMainXSize*bgMainYSize)<<1));
			//dramFileSave("HOSTSTQV.YUV",bgQvAddrWord,(((UINT32)bgQvXSize*bgQvYSize)<<1));
		}
	}

	if ((UINT8)(snapParam[ SP1K_SNAP_PARAM_STAMP_DT_DEFAULT_LOCATION_OPT ]) == 1) {
		if ( snapBgOpt ) {
			/* set default date postion(x,y) at background,and pasteThr */
			snapStampDateMainXOffSet[STAMP_DATE] = (snapBgMainXSize>>1);
			snapStampDateMainYOffSet[STAMP_DATE] = (snapBgMainYSize-snapDateStampMainYSize)>>1;
			snapStampDateMainColorKey[STAMP_DATE] = 0xba;

			/* set default time postion(x,y) at background,and pasteThr */
			snapStampDateMainXOffSet[STAMP_TIME] = (snapBgMainXSize>>1)+(snapDateStampMainXSize*11);
			snapStampDateMainYOffSet[STAMP_TIME] = (snapBgMainYSize-snapDateStampMainYSize)>>1;
			snapStampDateMainColorKey[STAMP_TIME] = 0xba;

			/* set default str postion(x,y) at background,and pasteThr */
			snapStampDateMainXOffSet[STAMP_TEXT] = 250;
			snapStampDateMainYOffSet[STAMP_TEXT] = (snapBgMainYSize-snapDateStampMainYSize)>>1;
			snapStampDateMainColorKey[STAMP_TEXT] = 0xba;

			/* set default date postion(x,y) at background,and pasteThr */
			snapStampDateQvXOffSet[STAMP_DATE] = (snapBgQvXSize>>1);
			snapStampDateQvYOffSet[STAMP_DATE] = (snapBgQvYSize-snapDateStampQvYSize)>>1;
			snapStampDateQvColorKey[STAMP_DATE] = 0xba;

			/* set default time postion(x,y) at background,and pasteThr */
			snapStampDateQvXOffSet[STAMP_TIME] = (snapBgQvXSize>>1)+(snapDateStampQvXSize*11);
			snapStampDateQvYOffSet[STAMP_TIME] = (snapBgQvYSize-snapDateStampQvYSize)>>1;
			snapStampDateQvColorKey[STAMP_TIME] = 0xba;

			/* set default str postion(x,y) at background,and pasteThr */
			snapStampDateQvXOffSet[STAMP_TEXT] = 250;
			snapStampDateQvYOffSet[STAMP_TEXT] = (snapBgQvYSize-snapDateStampQvYSize)>>1;
			snapStampDateQvColorKey[STAMP_TEXT] = 0xba;
		}
		else {
			/* set default date postion(x,y) at background,and pasteThr */
			snapStampDateMainXOffSet[STAMP_DATE] = 0;
			snapStampDateMainYOffSet[STAMP_DATE] = 0;
			snapStampDateMainColorKey[STAMP_DATE] = 0xba;

			/* set default time postion(x,y) at background,and pasteThr */
			snapStampDateMainXOffSet[STAMP_TIME] = snapDateStampMainXSize*11;
			snapStampDateMainYOffSet[STAMP_TIME] = 0;
			snapStampDateMainColorKey[STAMP_TIME] = 0xba;

			/* set default str postion(x,y) at background,and pasteThr */
			snapStampDateMainXOffSet[STAMP_TEXT] = 96;
			snapStampDateMainYOffSet[STAMP_TEXT] = 0;
			snapStampDateMainColorKey[STAMP_TEXT] = 0xba;

			/* set default date postion(x,y) at background,and pasteThr */
			snapStampDateQvXOffSet[STAMP_DATE] = 0;
			snapStampDateQvYOffSet[STAMP_DATE] = 0;
			snapStampDateQvColorKey[STAMP_DATE] = 0xba;

			/* set default time postion(x,y) at background,and pasteThr */
			snapStampDateQvXOffSet[STAMP_TIME] = snapDateStampQvXSize*11;
			snapStampDateQvYOffSet[STAMP_TIME] = 0;
			snapStampDateQvColorKey[STAMP_TIME] = 0xba;

			/* set default str postion(x,y) at background,and pasteThr */
			snapStampDateQvXOffSet[STAMP_TEXT] = 96;
			snapStampDateQvYOffSet[STAMP_TEXT] = 0;
			snapStampDateQvColorKey[STAMP_TEXT] = 0xba;
		}
	}
	else {
		pDate = stampTextGet(STAMP_DATE);
		pTime = stampTextGet(STAMP_TIME);
		pTxt = stampTextGet(STAMP_TEXT);
		/* set date postion(x,y) at background,and pasteThr */
		snapStampDateMainXOffSet[STAMP_DATE] = pDate->xOff;
		snapStampDateMainYOffSet[STAMP_DATE] = pDate->yOff;
		snapStampDateMainColorKey[STAMP_DATE] = pDate->pasteThd;

		/* set time postion(x,y) at background,and pasteThr */
		snapStampDateMainXOffSet[STAMP_TIME] = pTime->xOff;
		snapStampDateMainYOffSet[STAMP_TIME] = pTime->yOff;
		snapStampDateMainColorKey[STAMP_TIME] = pTime->pasteThd;

		/* set str postion(x,y) at background,and pasteThr */
		snapStampDateMainXOffSet[STAMP_TEXT] = pTxt->xOff;
		snapStampDateMainYOffSet[STAMP_TEXT] = pTxt->yOff;
		snapStampDateMainColorKey[STAMP_TEXT] = pTxt->pasteThd;

		/* set date postion(x,y) at background,and pasteThr */
		snapStampDateQvXOffSet[STAMP_DATE] = snapStampDateMainXOffSet[STAMP_DATE]*bgQvXSize/bgMainXSize;
		snapStampDateQvYOffSet[STAMP_DATE] = snapStampDateMainYOffSet[STAMP_DATE]*bgQvYSize/bgMainYSize;
		snapStampDateQvColorKey[STAMP_DATE] = snapStampDateMainColorKey[STAMP_DATE];

		/* set time postion(x,y) at background,and pasteThr */
		snapStampDateQvXOffSet[STAMP_TIME] = snapStampDateMainXOffSet[STAMP_TIME]*bgQvXSize/bgMainXSize;
		snapStampDateQvYOffSet[STAMP_TIME] = snapStampDateMainYOffSet[STAMP_TIME]*bgQvYSize/bgMainYSize;
		snapStampDateQvColorKey[STAMP_TIME] = snapStampDateMainColorKey[STAMP_TIME];

		/* set str postion(x,y) at background,and pasteThr */
		snapStampDateQvXOffSet[STAMP_TEXT] = snapStampDateMainXOffSet[STAMP_TEXT]*bgQvXSize/bgMainXSize;
		snapStampDateQvYOffSet[STAMP_TEXT] = snapStampDateMainYOffSet[STAMP_TEXT]*bgQvYSize/bgMainYSize;
		snapStampDateQvColorKey[STAMP_TEXT] = snapStampDateMainColorKey[STAMP_TEXT];
	}

}

/**
 * @fn        void snapPvYuvSave(UINT8 resetOpt, UINT16 opt)
 * @brief     save yuv of preview
 * @param     [in] rstOpt :
 *\n						0: no reset parameters
 *\n						1: reset parameters of yuv
 * @param     [in] opt
 *\n						0: enable save of yuv
 *\n						1: disable save of yuv
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapPvYuvSave(
	UINT8  rstOpt,
	UINT16 opt
)
{
	UINT8 idx,savePvYuvOpt;
	UINT32 addrPv;

	/*+++++++++++ Pv YUV data before capture ++++++++*/
	if (rstOpt==1){
       	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_PV_YUV_OPT, opt ); //tq.zhang 20091014
       	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_YUV_OPT, opt );    //tq.zhang 20091014
	}

	savePvYuvOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_SAVE_PV_YUV_OPT ]);


	idx = HAL_CdspWriteBufIdxGet();

	if( idx == 0 ) {
		addrPv = K_SDRAM_PvBufAddrD;
	}
	else if( idx == 1 ) {
		addrPv = K_SDRAM_PvBufAddrA;
	}
	else if( idx == 2 ) {
		addrPv = K_SDRAM_PvBufAddrB;
	}
	else if( idx == 3 ) {
		addrPv = K_SDRAM_PvBufAddrC;
	}
	if (savePvYuvOpt==1 ){
		dramFileSave("BEFORCAPYUV", addrPv, (UINT32)320*240*2);//tq.zhang 20091014
	}

	/*----------------------------------------------*/
}

/**
 * @fn        UINT8 sp1kSnapInit(UINT32 dramMByteSize, UINT32 memBeginAddrWord, UINT32 memEndAddrWord)
 * @brief     initialize memory for snap
 * @param     [in] memBeginAddrWord : start address of memory which can be used for snap (word address)
 * @param     [in] memEndAddrWord : end address of meomory which can be used for snap (word address)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8
sp1kSnapInit(
	UINT32 memBeginAddrWord,
	UINT32 memEndAddrWord
)
{

	snapMemAddr = memBeginAddrWord;
	snapMemEndAddr = memEndAddrWord;

	#ifdef SNAP_TIME_MEASURE
		snapDebugEntryCnt = 0;
	#endif

	// temp
	snapParam[ SP1K_SNAP_PARAM_QV_EFFECT ] = 0;

	// prepare address for host
	snapShutterSoundAddr = snapMemAddr;

	snapPhotoFrmYuvAddr = snapShutterSoundAddr + ( (UINT32)SNAP_MAX_SHUTTER_SOUND_SIZE >> 1 );
	snapPhotoFrmYuvAddr = ( snapPhotoFrmYuvAddr + 15 ) >> 4 << 4;  // YUV align to 16 word

	//snapParam[ SP1K_SNAP_PARAM_SOUND_LOW_ADDR ] = snapShutterSoundAddr & 0xffff;
	//snapParam[ SP1K_SNAP_PARAM_SOUND_HIGH_ADDR ] = ( snapShutterSoundAddr >> 16 ) & 0xffff;

	snapParam[ SP1K_SNAP_PARAM_PH_LOW_ADDR ] = snapPhotoFrmYuvAddr & 0xffff;
	snapParam[ SP1K_SNAP_PARAM_PH_HIGH_ADDR ] = ( snapPhotoFrmYuvAddr >> 16 ) & 0xffff;

	snapParam[ SP1K_SNAP_PARAM_STAMP_X_RATIO ] = SNAP_DEFAULT_STAMP_X_RATIO;
	snapParam[ SP1K_SNAP_PARAM_STAMP_Y_RATIO ] = SNAP_DEFAULT_STAMP_Y_RATIO;

	//snapParam[ SP1K_SNAP_PARAM_QV_WIDTH ] = 320;
	//snapParam[ SP1K_SNAP_PARAM_QV_HEIGHT ] = 240;

	return SUCCESS;
}

/**
 * @fn        UINT8 sp1kSnapParamSet(UINT8 attrId, UINT16 value)
 * @brief     set parameters of snap
 * @param     [in] attrId : idx of parameters
 * @param     [in] value : parameter value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8
snapParamSet(
	UINT8 attrId,
	UINT16 value
)
{
	if ( attrId > SP1K_SNAP_PARAM_MAX ) {
		return FAIL;
	}


	if( attrId == SP1K_SNAP_PARAM_BURST /*in case host set wrong parameters*/
		|| attrId == SP1K_SNAP_PARAM_QV
		|| attrId == SP1K_SNAP_PARAM_PRE_BRC
		|| attrId == SP1K_SNAP_PARAM_POST_BRC
		|| attrId == SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT
		|| attrId == SP1K_SNAP_PARAM_STAMP_OPT
		|| attrId == SP1K_SNAP_PARAM_PHOTO_FRM_OPT
		|| attrId == SP1K_SNAP_PARAM_SKIP_PV
		|| attrId == SP1K_SNAP_PARAM_PV_FREEZE_OPT
		|| attrId == SP1K_SNAP_PARAM_SAVE_RAW_OPT
		|| attrId == SP1K_SNAP_PARAM_SAVE_YUV_OPT
		|| attrId == SP1K_SNAP_PARAM_SAVE_PV_YUV_OPT
		|| attrId == SP1K_SNAP_PARAM_10_BIT_RAW_OPT) {
		if( value != 0 && value != 1 ) {
			value = 1;
		}
	}

	snapParam[ attrId ] = value;

	return SUCCESS;
}

/**
 * @fn        UINT16 sp1kSnapParamGet(UINT8 attrId)
 * @brief     get parameters of snap
 * @param     [in] attrId : index of parameters
 * @retval    return = parameters value
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
UINT16
snapParamGet(
	UINT8 attrId
)
{
	if ( attrId > SP1K_SNAP_PARAM_MAX ) {
		return FAIL;
	}

	return snapParam[ attrId ];
}

/**
 * @fn        void sp1kSnapCallbackReg(UINT32 id, void *pfunct)
 * @brief     register callback of snap
 * @param     [in] id : index of callback
 * @param     [in] pfunct : function point you wanna register
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kSnapCallbackReg(
	UINT32 id,
	void *pfunct
)
{
	switch (id) {
		case SP1K_SNAP_CALLBACK_BURST:
			psnapBurst = pfunct;
			break;
		case SP1K_SNAP_CALLBACK_BACKGROUND_RES_LOAD:
			psnapBackgroundResLoad = pfunct;
			break;
		case SP1K_SNAP_CALLBACK_HOST_STAMP:
			psnapHostStamp = pfunct;
			break;
		default:
			ASSERT(0,0);
			break;
	}
}


/**
 * @fn        void sp1kSnapSizeSet(UINT16 hSize, UINT16 vSize)
 * @brief     Set horizontal/vertical size of capture image
 * @param     [in] hSize : horizontal size
 * @param     [in] vSize : vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kSnapSizeSet(
	UINT16 hSize,
	UINT16 vSize
)
{
	snapHSize = hSize;
	snapVSize = vSize;

}

/**
 * @fn        void sp1kSnapSizeGet(UINT16* hSize, UINT16* vSize)
 * @brief     Get horizontal/vertical size of capture image
 * @param     [out] hSize : horizontal size
 * @param     [out] vSize : vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kSnapSizeGet(
	UINT16* hSize,
	UINT16* vSize
)
{
	*hSize = snapHSize;
	*vSize = snapVSize;

}


/**
 * @fn        void sp1kSnapDzoomFactorSet(UINT16 hFactor, UINT16 vFactor)
 * @brief     Set digital zoom factor for snap
 * @param     [in] hFactor : horizontal zoom factor (100 -> 1x , 200 -> 2x)
 * @param     [in] vFactor : vertical zoom factor (100 -> 1x , 200 -> 2x)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kSnapDzoomFactorSet(
	UINT16 hFactor,
	UINT16 vFactor
)
{
	snapHFactor = hFactor;
	snapVFactor = vFactor;

}


/**
 * @fn        void sp1kSnapCropSizeSet(UINT16 cropHSize, UINT16 cropVSize)
 * @brief     Set crop size for capture. It can't be bigger than sensor full size.
 * @param     [In] cropHSize : horizontal size
 * @param     [In] cropVSize : vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kSnapCropSizeSet(
	UINT16 cropHSize,
	UINT16 cropVSize
)
{
	snapSensorCropHSize=cropHSize;
	snapSensorCropVSize=cropVSize;
}





/**
 * @fn        void chgRawFileNameSet(UINT8 *filename)
 * @brief     Set change raw file name
 * @param     [in] filename : 8.3 format ( ex: CHGRAW00RAW , filename: CHGRAW00.RAW)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
chgRawFileNameSet(
	UINT8 *filename
)
{
	UINT8 i;
	for (i=0;i<12;i++)
		chgRawFileName[i]=filename[i];
}

/**
 * @fn        void cdspBinFileNameSet(UINT8 *filename)
 * @brief     Set cdsp bin file name
 * @param     [in] filename :  : 8.3 format ( ex: CDSP0000BIN , filename: CDSP0000.BIN)
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
cdspBinFileNameSet(
	UINT8 *filename
)
{
	UINT8 i;
	for (i=0;i<12;i++)
		cdspBinFileName[i]=filename[i];

}

/**
 * @fn        void chgRawSizeSet(UINT16 hsize, UINT16 vsize)
 * @brief     Set change raw size
 * @param     [in] hsize : horizontal size
 * @param     [in] vsize : vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
chgRawSizeSet(
	UINT16 hsize,
	UINT16 vsize
)
{
	chgRawHSize = hsize;
	chgRawVSize = vsize;
}

/**
 * @fn        void chgRawSizeGet(UINT16 *hsize, UINT16 *vsize)
 * @brief     Get change raw size
 * @param     [in] hsize : horizontal size
 * @param     [in] vsize : vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void
chgRawSizeGet(
	UINT16 *hsize,
	UINT16 *vsize
)
{
	*hsize = chgRawHSize;
	*vsize = chgRawVSize;
}


UINT8
snapBlinkDetect(
	void
)
{

	UINT8 blinkDetected;
	//printf("blink state=0x%x\n",snapParam[ SP1K_SNAP_PARAM_BLINK]);
	//snapParam[ SP1K_SNAP_PARAM_BLINK]=1;
	if(snapParam[ SP1K_SNAP_PARAM_BLINK]!=0){	/*blink enable*/
		snapCdspDoImg(CDSP_DO_BLINK);
		//dramFileSave( "BLINK000.YUV", snapJpegVlcAddr, (UINT32)SNAP_CDSP_BLINK_HSIZE*SNAP_CDSP_BLINK_VSIZE*2);
		sp1kFdEnable();
		blinkDetected = sp1kPbFdProcBlink(snapQvAddr,snapJpegVlcAddr); /*640x480 YUV is put in jpeg vlc buffer*/
		sp1kFdDisable();
		if( blinkDetected ){
			osMsgPost(SP1K_MSG_EYE_BLINK);
			return FAIL;
		}
	}
	return SUCCESS;
}

UINT8
snapVLCOverflowGet(
	void
)
{
	return ((XBYTE[REG_Jpeg_Intr_Event]&0x08)>>3);
}
/********************************************************/
/* Snap Photo Frame										*/
/********************************************************/

/**
 * @fn        void sp1kSnapPhotoFrameParaSet(UINT8 resId, UINT32 rscAddr)
 * @brief     Set parameters for photo frame in capture mode.
 * @param     [in] resId : resource id
 * @param     [in] rscAddr : the address of photo frame in DRAM.
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-16
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kSnapPhotoFrameParaSet(
	UINT16 resId,
	UINT32 rscAddr
)
{
	#if FUNC_KNL_FRM
		snapPhotoFrmRscId = resId;
		snapPhotoFrmRscAddr = rscAddr;
	#else
		resId = resId;
		rscAddr = rscAddr;
	#endif
}




/********************************************************/
/* Snap Date Stamp										*/
/********************************************************/

/**
 * @fn        void snapStampQvPrepare(void)
 * @brief     snap Stamp Qv Prepare
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-16
 * @todo      N/A
 * @bug       N/A
*/
void
snapStampQvPrepare(
	void
)
{
	UINT8 conditions;

	if ( !snapStampOpt ) {
		return;
	}

	//printf("snapBurstOpt=%bu,snapBurstCnt=%bu\n",snapBurstOpt,snapBurstCnt);
	conditions = ((snapBurstOpt == 0) || (snapBurstOpt == 1 && snapBurstCnt == 0));

	/* set Qv background addr */
	if ( snapBgOpt ) {
		stampBackgroundSet(snapQvBgAddr, snapBgQvXSize, snapBgQvYSize);
	}
	else {
		stampBackgroundSet(snapQvDateStampAddr, snapDateStampQvXSize*DATE_STAMP_NUM, snapDateStampQvYSize);
	}

	/* set default date postion(x,y) at background,and pasteThr */
	stampTextSet(STAMP_DATE, snapStampDateQvXOffSet[STAMP_DATE], snapStampDateQvYOffSet[STAMP_DATE], snapStampDateQvColorKey[STAMP_DATE]);

	/* set default time postion(x,y) at background,and pasteThr */
	stampTextSet(STAMP_TIME, snapStampDateQvXOffSet[STAMP_TIME], snapStampDateQvYOffSet[STAMP_TIME], snapStampDateQvColorKey[STAMP_TIME]);

	/* set default str postion(x,y) at background,and pasteThr */
	stampTextSet(STAMP_TEXT, snapStampDateQvXOffSet[STAMP_TEXT], snapStampDateQvYOffSet[STAMP_TEXT], snapStampDateQvColorKey[STAMP_TEXT]);

	/* Date Stamp */
	if ( snapDateStampOpt ) {
		stampFontSet( snapQvStampFontPtnAddr, snapDateStampQvXSize, snapDateStampQvYSize, NULL );
	}

	//dramFileSave("QVFONT02.YUV",snapQvStampFontPtnAddr,(UINT32)snapDateStampQvXSize*snapDateStampQvYSize*DATE_STAMP_TEMP_NUM*2);

	//dramFileSave("QVDATE02.YUV",snapQvAddr,(UINT32)snapDsQvHSize*snapDsQvVSize*2);

	stampCombineToImage(snapQvAddr,snapDsQvHSize,snapDsQvVSize,snapStampQvXPos,snapStampQvYPos,0xba);

	//dramFileSave("QVSTAMP0.YUV",snapQvAddr,(UINT32)snapDsQvHSize*snapDsQvVSize*2);
}

/********************************************************/
/* Snap Main Date Stamp										*/
/********************************************************/
/**
 * @fn        void snapStampDoFrontBurstMainPrepare(void)
 * @brief     snap Stamp DoFront Burst Main Prepare
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-16
 * @todo      N/A
 * @bug       N/A
*/
void
snapStampDoFrontBurstMainPrepare(
	void
)
{
	//UINT8 conditions;
	UINT32 combineAddr;
	//UINT16 jpegStampHSize, jpegStampVSize;

	if ( !snapStampOpt ) {
		return;
	}

	#if (SNAP_DOFLOW_BURST_CAPTURE_BG_STAMP_UPDATE == 1)
	#if (SNAP_NORMAL_STAMP_WITH_HOST_STAMP == 1)
	/* callback for host User stamp */
	if ( psnapHostStamp != NULL )
	#else
	/* callback for host User stamp */
	if ( snapBgOpt && psnapHostStamp != NULL )
	#endif
	{
		UINT32 bgMainAddrWord, tempBuffer;
		UINT16 bgMainXSize, bgMainYSize;

		#if (SNAP_NORMAL_STAMP_WITH_HOST_STAMP == 1)
		/* clear host stamp addr context */
		gprmBlackFill(snapMainDateStampTempAddr,snapDateStampMainXSize*DATE_STAMP_NUM,snapDateStampMainYSize);
		#else
		tempBuffer = snapMainBgAddr;
		/* Bg (original size) scale to (Main) Bg size */
		HAL_GprmScale(snapBgYuvAddr,
						snapBgHSize,
						snapBgVSize,
						snapMainBgTempAddr,
						snapBgMainXSize,
						snapBgMainYSize,
						tempBuffer,
						1);
		#endif

		/* set main size,addr for host release position */
		if ( snapBgOpt ) {
			bgMainAddrWord = snapMainBgTempAddr;
			bgMainXSize = snapBgMainXSize;
			bgMainYSize = snapBgMainYSize;
		}
		else {
			bgMainAddrWord = snapMainDateStampTempAddr;
			bgMainXSize = snapDateStampMainXSize * DATE_STAMP_NUM;
			bgMainYSize = snapDateStampMainYSize;
		}

		stampBackgroundSet(bgMainAddrWord, bgMainXSize, bgMainYSize);

		//printf("snapBurstCnt=%bu\n",snapBurstCnt);
		if ((*psnapHostStamp)(NULL) == FAIL) {
			ASSERT(0,0);
		}

		#if 0
		printf("bgM=%d,%d, bgQ=%d,%d\n",bgMainXSize,bgMainYSize,bgQvXSize,bgQvYSize);
		switch ( snapBurstCnt ) {
			case 1:
				dramFileSave("HOSTSTM1.YUV",bgMainAddrWord,(((UINT32)bgMainXSize*bgMainYSize)<<1));
				break;
			case 2:
				dramFileSave("HOSTSTM2.YUV",bgMainAddrWord,(((UINT32)bgMainXSize*bgMainYSize)<<1));
				break;

		}
		#endif
	}
	#endif

	/* read RTC */
	HAL_GlobalReadRTC( &snapDSInfo[snapBurstCnt] );
	stampTextModifyByRTC( &snapDSInfo[snapBurstCnt] );

	//printf("snapStampDoFrontBurstMainPrepare=%bu\n",snapBurstCnt);

	/* set main background addr */
	if ( snapBgOpt ) {
		//jpegStampHSize = snapBgMainXSize;
		//jpegStampVSize = snapBgMainYSize;
		//stampBackgroundSet(snapMainBgTempAddr, jpegStampHSize, jpegStampVSize);
		combineAddr = snapMainBgAddr;
	}
	else {
		//jpegStampHSize = snapDateStampMainXSize*DATE_STAMP_NUM;
		//jpegStampVSize = snapDateStampMainYSize;
		//stampBackgroundSet(snapMainDateStampTempAddr, jpegStampHSize, jpegStampVSize);
		combineAddr = snapMainDateStampAddr;
	}

	/* Date Stamp */
	if ( snapDateStampOpt ) {

		if ( snapBgOpt ) {
			/* clear main ds addr context */
			gprmBlackFill(snapMainBgAddr,snapBgMainXSize,snapBgMainYSize);
		}
		else {
			/* clear main ds addr context */
			gprmBlackFill(snapMainDateStampAddr,snapDateStampMainXSize*DATE_STAMP_NUM,snapDateStampMainYSize);
		}

		stampFontSet( snapMainStampFontPtnAddr, snapDateStampMainXSize, snapDateStampMainYSize, NULL );
	}

	stampCombine(combineAddr);

}

/**
 * @fn        void snapStampMainPrepare(void)
 * @brief     snap Stamp Main Prepare
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-16
 * @todo      N/A
 * @bug       N/A
*/
void
snapStampMainPrepare(
	void
)
{
	UINT8 conditions;
	UINT32 combineAddr;
	UINT16 jpegStampHSize, jpegStampVSize;

	if ( !snapStampOpt ) {
		return;
	}

	//printf("snapBurstOpt=%bu,snapBurstCnt=%bu\n",snapBurstOpt,snapBurstCnt);
	conditions = ((snapBurstOpt == 0) || (snapBurstOpt == 1 && snapBurstCnt == 0));

	/* set main background addr */
	if ( snapBgOpt ) {
		jpegStampHSize = snapBgMainXSize;
		jpegStampVSize = snapBgMainYSize;
		stampBackgroundSet(snapMainBgTempAddr, jpegStampHSize, jpegStampVSize);
		combineAddr = snapMainBgAddr;
	}
	else {
		jpegStampHSize = snapDateStampMainXSize*DATE_STAMP_NUM;
		jpegStampVSize = snapDateStampMainYSize;
		stampBackgroundSet(snapMainDateStampTempAddr, jpegStampHSize, jpegStampVSize);
		combineAddr = snapMainDateStampAddr;
	}

	/* set default date postion(x,y) at background,and pasteThr */
	stampTextSet(STAMP_DATE, snapStampDateMainXOffSet[STAMP_DATE], snapStampDateMainYOffSet[STAMP_DATE], snapStampDateMainColorKey[STAMP_DATE]);

	/* set default time postion(x,y) at background,and pasteThr */
	stampTextSet(STAMP_TIME, snapStampDateMainXOffSet[STAMP_TIME], snapStampDateMainYOffSet[STAMP_TIME], snapStampDateMainColorKey[STAMP_TIME]);

	/* set default str postion(x,y) at background,and pasteThr */
	stampTextSet(STAMP_TEXT, snapStampDateMainXOffSet[STAMP_TEXT], snapStampDateMainYOffSet[STAMP_TEXT], snapStampDateMainColorKey[STAMP_TEXT]);

	/* Date Stamp */
	if ( snapDateStampOpt ) {

		if ( snapBgOpt ) {
			/* clear main ds addr context */
			gprmBlackFill(snapMainBgAddr,snapBgMainXSize,snapBgMainYSize);
		}
		else {
			/* clear main ds addr context */
			gprmBlackFill(snapMainDateStampAddr,snapDateStampMainXSize*DATE_STAMP_NUM,snapDateStampMainYSize);
		}

		stampFontSet( snapMainStampFontPtnAddr, snapDateStampMainXSize, snapDateStampMainYSize, NULL );
	}

	stampCombine(combineAddr);

	//dramFileSave("MAFINL01.YUV",combineAddr,(UINT32)jpegStampHSize*jpegStampVSize*2);

	HAL_JpegDataStampSet(combineAddr,jpegStampHSize,
							snapStampMainXPos>>4,snapStampMainYPos>>3,
							(UINT8)(jpegStampHSize>>4),jpegStampVSize>>3,0x3f);

	HAL_JpegDataStampEnable(0x01, 0x00);
}

#if FUNC_KNL_FRM

/**
 * @fn        UINT8 snapPhotoFrmDecode(UINT32 addr)
 * @brief     snapPhotoFrmDecode
 * @param     [in] addr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2011-3-16
 * @todo      N/A
 * @bug       N/A
*/
UINT8
snapPhotoFrmDecode(
	UINT32 addr
)
{
	UINT8 opt=0;
	UINT32 size;
	//struct Exif_Decompress_Parameter imgPara;
	ExifImage_t imgPara;
	UINT8 pbScaleFactor;
	//UINT32 t1,t2;
	//HAL_GlobalReadGTimer( &t1 );

	if(!sp1kNandRsvRead((UINT32)snapPhotoFrmRscId, snapRawDataAddr << 1)){
		ASSERT(0,1);
		return FAIL;
	}

	if (!sp1kNandRsvSizeGet((UINT32)snapPhotoFrmRscId, NULL, &size)) {
		ASSERT(0,1);
		return FAIL;
	}

	//if (!exifParsingFile(snapRawDataAddr, size, 0, NULL, &imgPara, NULL)) {
	if (!exifParserStart(exifGetMainInfo,&imgPara,snapRawDataAddr, size)) {
		ASSERT(0,1);
		return FAIL;
	}

	/* check YUV format and if the vlc data is at odd address */
	if (imgPara.dataType == K_File_DataType_YUV420) {
		opt |= 0x05;
	} else if (imgPara.dataType == K_File_DataType_YUV422) {
		opt |= 0x04;
	}
	if (exifVlcAtOdd) {
		exifVlcAtOdd = 0;
		opt |= 0x10;
	}

	pbScaleFactor=0;
	if (imgPara.width > snapYuvHSize){

		if ( (imgPara.width >>1) <= snapYuvHSize ){		/* 4/8 */
			pbScaleFactor=4;
			snapPhHSize=imgPara.width >> 1;
			snapPhVSize=imgPara.height >> 1 ;
		}
		else if ( (imgPara.width >>2) <= snapYuvHSize ){	/* 2/8 */
			pbScaleFactor=2;
			snapPhHSize=imgPara.width >> 2;
			snapPhVSize=imgPara.height >> 2;
		}
		else{
			/* photo frame is too big */
			ASSERT(0,1);
			return FAIL;
		}

		snapPrintf("pbScaleFactor=%bd\n",pbScaleFactor);

		sp1kHalJpegDecode(imgPara.addr,imgPara.width,imgPara.height,addr,pbScaleFactor,opt,G_JpgRstMcuNum);

		//dramFileSave("PHSCALED.YUV",addr,(UINT32)snapPhHSize * snapPhVSize * 2);
	}
	else{
		sp1kHalJpegDecode(imgPara.addr,imgPara.width,imgPara.height,addr,0,opt,G_JpgRstMcuNum);
		//printf("ph h=%d,v=%d\n",imgPara.Width,imgPara.Height);
		//dramFileSave("PHSCALED.YUV",addr,(UINT32)imgPara.Width * imgPara.Height * 2);
	}


	return SUCCESS;
}
/**
 * @fn        void snapPhotoFrmPaste(UINT32 addr, UINT16 number)
 * @brief     Copy photo frame to yuv buffer
 * @param     [in] addr : yuv data address
 * @param     [in] number : number of photo frame
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
void
snapPhotoFrmPaste(
	UINT32 addr,
	UINT16 blkIdx
)
{
	UINT16 overlap;
	UINT16 lines;
	UINT32 writeSize;
	//UINT8 saveYuvOpt;

	//saveYuvOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_SAVE_YUV_OPT ]);

	//printf("photo paste=%bd\n",phFrmOpt);
	if( snapPhotoFrmOpt == 0 ) {
		return;
	}

	if(  blkIdx == snapPhotoFrmLastNum) {
		lines = snapYuvVSize - snapPhotoFrmLastNum * snapYuvLines;

		writeSize = (UINT32)snapYuvHSize * lines * 2;

		writeSize = ( writeSize + 511 ) / 512 * 512;
	}
	else {
		lines = snapYuvLines;

		writeSize = (UINT32)snapYuvHSize * lines * 2;

		ASSERT( ( writeSize % 512 == 0 ), 0 );
	}


	gprmPartialVScale( snapPhotoFrmYuvAddr, snapPhotoFrmYuvScalingAddr, snapPhotoFrmYuvScalingTempAddr, blkIdx, &overlap );

	//if (blkIdx==2){
		//printf("photo h=%d,v=%d\n",snapYuvHSize,lines);
		//	dramFileSave("PHOTO001.YUV",snapPhotoFrmYuvScalingAddr,writeSize);
	//}

	//#ifdef SNAP_DEBUG_SAVE_PH_YUV
	#if 0
		//dramFileSave();
		if( saveYuvOpt == 1  ) {
			vfsFileWrite( snapYuvFHandler, K_File_SrcDataDram2Card, writeSize, snapPhotoFrmYuvScalingAddr + (UINT32)overlap * snapYuvHSize, 0 );
		}

		if( saveYuvOpt == 1  ) {
			vfsFileWrite( snapYuvFHandler, K_File_SrcDataDram2Card, writeSize, addr, 0 );
		}
	#endif

	//if (blkIdx==30){
	//	dramFileSave("PHOTO030.YUV",addr,writeSize);
	//}

	HAL_GprmCopy(snapPhotoFrmYuvScalingAddr, snapYuvHSize, (UINT16)snapPhotoFrmYuvScalingBufYSize, 0, overlap,
		addr, snapYuvHSize, snapYuvLines, 0, 0,
		(UINT16)snapYuvHSize, lines, (UINT16)0x80 + 0x14, 1);


	/*
	if (blkIdx==30){
		dramFileSave("PHOTO030.YUV",addr,writeSize);
	}
	if (blkIdx==2){
		dramFileSave("PHOTO002.YUV",addr,writeSize);
	}
	if (blkIdx==3){
		dramFileSave("PHOTO003.YUV",addr,writeSize);
	}
	if (blkIdx==4){
		dramFileSave("PHOTO004.YUV",addr,writeSize);
	}
	*/

	return;
}
#endif


#if 0

/**************************************************************************
 *   U N U S E D    F U N C T I O N S                                     *
 **************************************************************************/
	/**
	 * @fn		  void snapRegDump(UINT8 idx)
	 * @brief	  snapRegDump
	 * @param	  [in] idx :
	 *\n					0: JPG register
	 *\n					1: xxx.bin register
	 *\n					2: 0x2100~0x22ff
	 * @retval	  NONE
	 * @see 	  NULL
	 * @author	  Will Chiu
	 * @since	  2010-5-17
	 * @todo	  N/A
	 * @bug 	  N/A
	*/

	void
	snapRegDump(
		UINT8 idx
	)
	{
		UINT16 i;
		UINT16 LSB,MSB,DataLen,RegAdd;
		UINT32 filesize;

		switch (idx){
			case 0:
				for (i=0;i<0x200;i=i+8){

					snapPrintfLog("R[0x%x:0x%x]=0x%02bx,0x%02bx,0x%02bx,0x%02bx,0x%02bx,0x%02bx,0x%02bx,0x%02bx\n",REG_Dram_Cdsp1Eof_Sts+i,REG_Dram_Cdsp1Eof_Sts+i+7,
													  XBYTE[REG_Dram_Cdsp1Eof_Sts+i  ],XBYTE[REG_Dram_Cdsp1Eof_Sts+i+1],
													  XBYTE[REG_Dram_Cdsp1Eof_Sts+i+2],XBYTE[REG_Dram_Cdsp1Eof_Sts+i+3],
													  XBYTE[REG_Dram_Cdsp1Eof_Sts+i+4],XBYTE[REG_Dram_Cdsp1Eof_Sts+i+5],
													  XBYTE[REG_Dram_Cdsp1Eof_Sts+i+6],XBYTE[REG_Dram_Cdsp1Eof_Sts+i+7]);
				}
			break;
			case 1:

				if (!dramFileLoad("IWBOFFENBIN", K_SDRAM_CommonEndAddr, &filesize)){
					snapPrintf("load cdsp parameter error\n",0);
					return;
				}
				else{
					snapPrintf("load cdsp parameter success\n",0);
				}

				M_DRAM_SetStartAddress((UINT32)(K_SDRAM_CommonEndAddr+3), 1);

				M_DRAM_ReadWord(LSB, MSB);
				DataLen = (UINT16)(MSB&0xff)<<8|(LSB&0xff);

				snapPrintf("cdsp para datalength=%d\n",DataLen);

				DataLen =DataLen>>2;

				for( i=0 ; i< DataLen ;i++ ) {
					M_DRAM_ReadWord(LSB, MSB);
					RegAdd = (UINT16)(MSB&0xff)<<8|(LSB&0xff);
					M_DRAM_ReadWord(LSB, MSB);
					snapPrintf("REG[0x%04x]=0x%b02x\n",RegAdd,XBYTE[RegAdd]);
				}
				break;
			case 2:
				for (i=0;i<0x100;i=i+8){
					snapPrintfLog("R[0x%4x : 0x%04x]=%02bx %02bx %02bx %02bx %02bx %02bx %02bx %02bx\n",
									REG_Cdsp_macroSel+i,REG_Cdsp_macroSel+i+7,
									XBYTE[REG_Cdsp_macroSel+i	],XBYTE[REG_Cdsp_macroSel+i+1],
									XBYTE[REG_Cdsp_macroSel+i+2],XBYTE[REG_Cdsp_macroSel+i+3],
									XBYTE[REG_Cdsp_macroSel+i+4],XBYTE[REG_Cdsp_macroSel+i+5],
									XBYTE[REG_Cdsp_macroSel+i+6],XBYTE[REG_Cdsp_macroSel+i+7]);
					}

				for (i=0;i<0x100;i=i+8){
					snapPrintfLog("R[0x%4x : 0x%04x]=%02bx %02bx %02bx %02bx %02bx %02bx %02bx %02bx\n",
									REG_Cdsp_AFwinen+i,REG_Cdsp_AFwinen+i+7,
									XBYTE[REG_Cdsp_AFwinen+i	],XBYTE[REG_Cdsp_AFwinen+i+1],
									XBYTE[REG_Cdsp_AFwinen+i+2],XBYTE[REG_Cdsp_AFwinen+i+3],
									XBYTE[REG_Cdsp_AFwinen+i+4],XBYTE[REG_Cdsp_AFwinen+i+5],
									XBYTE[REG_Cdsp_AFwinen+i+6],XBYTE[REG_Cdsp_AFwinen+i+7]);
				}
				break;
			break;
			default:
			break;

		}
	}

//#if defined(SNAP_DEBUG_SAVE_YUV) || defined(SNAP_IQ_FLOW) || defined(SNAP_DEBUG_SAVE_PH_YUV)
static UINT8
snapYuvFileOpen(
	void
)
{
    UINT16 numCluster;
    UINT32 diskFreeSpace;

    snapYuvFileSize = (UINT32)snapYuvHSize * snapYuvVSize * 2;
	snapYuvFileSize = ( snapYuvFileSize + 511 ) / 512 * 512;

#if 1  //will FAT32
	//eric@20080502,begin
	diskFreeSpace = (UINT32)1024 * (UINT32)DOS_GetDiskSpace();

	numCluster = 0;
#else
	diskFreeSpace = (UINT32)G_DOS_ClusterSize * (UINT32)DOS_GetDiskSpace();

	numCluster = (snapYuvFileSize  + G_DOS_ClusterSize - 1) / G_DOS_ClusterSize;

	ASSERT( ( numCluster + 1 <= G_DOS_TotalFreeCluster ), 0 );
#endif

	ASSERT( ( snapYuvFileSize < diskFreeSpace ), 0 );

    dcfJpgNameGet(snapYuvFName, dcfFileIndexGet());

	snapYuvFName[8] = 'Y';
	snapYuvFName[9] = 'U';
	snapYuvFName[10] = 'V';

	snapYuvFName[11] = 0x20;
	snapYuvFName[12] = 0x00;

	if (vfsFileDelete( snapYuvFName)) {
//		printf("delete %s\n",fakefilename);
	}
	else {
//		printf("crite file %s\n",fakefilename);
	}

	File_SendMessage(K_File_UI_Start_Save_File, snapYuvFileSize);
	G_Card_UI_Action = K_Act_Go_SaveFile;
	snapYuvFHandler = vfsFileOpen(snapYuvFName, K_DOS_NewFile, numCluster);

	ASSERT( ( snapYuvFHandler != 0 ), 0 );

	return 0;
}

static UINT8
snapYuvFileClose(
	void
)
{
    vfsFileClose(snapYuvFHandler, snapYuvFName, K_DOS_NewFile, snapYuvFileSize);

	return 0;
}
//#endif






UINT8
sp1kSnapTurnOnPv(
	void
)
{
	return snapPvTurnOn();
}

UINT8
sp1kSnapStore(
	void
)
{
	return snapStore();
}


static UINT8
snapCdspDo(
	UINT8 mode
)
{
    UINT16 i, cdspy=0;
    UINT16 cdspWriteOffset;
    UINT8 bottom = 0;
	UINT16 hSize, vSub, hSub;
	UINT8 dateStampType = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DATE_STAMP_TYPE ]);
	UINT8 saveYuvOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_SAVE_YUV_OPT ]);
	UINT32 yuvAddr;
	UINT16 phI = 0;
	UINT16 doYSize;

	if( saveYuvOpt == 1 ) {
	    snapYuvFileOpen();
	}

#ifdef ICAT_OPTION
	if( sp1kSnapIcatMode == 1 ) {
		sp1kSnapIcatState = SNAP_ICATE_CDSP_READY;
	}
#endif

	halCdspSubSizeCal( mode, 0, &hSize, &hSub, &vSub );

	snapBufNum = 0;

	if( snapYuvABBufOpt == 1 ) {
		// adjust snapDateStampYDoOffset
		snapDateStampYDoOffset = snapDateStampYDoOffset % snapYuvChunkLines;
	}

	cdspWriteOffset = 0;

	HAL_CdspFrmCtrlEnSet(0);
	HAL_CdspWriteBufIdxSet(0);

	yuvAddr = snapYuvCurAddrGet( cdspWriteOffset, snapYuvChunkLines, bottom );

	if( snapYuvVSize > snapYuvLines ) {
		doYSize = snapYuvLines;
		HAL_CdspImgDoABBuf( mode, 0, snapYuvHSize, 0, snapYuvLines, 3, 1, cdspWriteOffset );
	}
	else {
		doYSize = snapYuvVSize;
		HAL_CdspImgDoABBuf( mode, 0, snapYuvHSize, 0, snapYuvVSize, 3, 3, cdspWriteOffset );
	}

	if( saveYuvOpt == 1 && snapYuvSaveOpt == 0 ) {
		vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card, (UINT32)snapYuvHSize * doYSize * 2, snapYuv1Addr, 0);
	}

	// photo frame
	if( doYSize > snapYuvChunkLines ) {
		snapPhotoFrmPaste( yuvAddr, phI++ );
		snapPhotoFrmPaste( yuvAddr + (UINT32)snapYuvHSize * snapYuvChunkLines, phI++ );
	}
	else {
		snapPhotoFrmPaste( yuvAddr, phI++ );
	}

#ifdef ICAT_OPTION
	if( sp1kSnapIcatMode == 1 ) {
		sp1kSnapIcatState = SNAP_ICATE_CDSP_DOING;
		if(iCatFlowMode != ICAT_RAW_ONLY_MODE)
			icatBulkIn( snapYuv1Addr, (UINT32)snapYuvHSize* doYSize * 2 );
	}
#endif

	snapDateStampDo( yuvAddr, dateStampType, snapYuvABBufOpt, doYSize, doYSize );

	if( saveYuvOpt == 1 && snapYuvSaveOpt == 1 ) {
		vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card, (UINT32)snapYuvHSize * doYSize * 2, snapYuv1Addr, 0);
	}

	if( snapYuvVSize <= snapYuvLines ) {
		if( saveYuvOpt == 1 ) {
			snapYuvFileClose();
		}

		return 0;
	}

	cdspy += doYSize;
	snapYuvBufSwitch( cdspy, snapYuvChunkLines, &cdspWriteOffset, &bottom, &snapBufNum, snapYuvABBufOpt );

	for( i = snapYuvLines - vSub; (SINT16) i < (SINT16) ( snapYuvVSize - snapYuvChunkLines - vSub ); i = i + snapYuvChunkLines ) {
		snapYuvBufSwitch( cdspy, snapYuvChunkLines, &cdspWriteOffset, &bottom, &snapBufNum, snapYuvABBufOpt );

#ifdef CDSP_JPEG_VLC_FLOW_CONTROL
		if((XBYTE[REG_Dram_VlcRdy]&0x01)==0x01) {HAL_GprmDramDma(vlcsrcaddr_g, vlcdtnaddr_g, 32768, 1);XBYTE[REG_Dram_VlcRdy]=0x01;vlcsrcaddr_g=vlcabtoggle?vlcsrcaddr_g-(UINT32)32768:vlcsrcaddr_g+(UINT32)32768;vlcdtnaddr_g+=(UINT32)32768;vlcabtoggle=vlcabtoggle?0:1;}
		if((XBYTE[REG_Dram_VlcRdy]&0x02)==0x02) {HAL_GprmDramDma(vlcsrcaddr_g, vlcdtnaddr_g, 32768, 1);XBYTE[REG_Dram_VlcRdy]=0x02;vlcsrcaddr_g=vlcabtoggle?vlcsrcaddr_g-(UINT32)32768:vlcsrcaddr_g+(UINT32)32768;vlcdtnaddr_g+=(UINT32)32768;vlcabtoggle=vlcabtoggle?0:1;}
#endif

		yuvAddr = snapYuvCurAddrGet( cdspWriteOffset, snapYuvChunkLines, bottom );

#if 1
		if( cdspy < snapYuvLines + snapYuvChunkLines ) {
			snapCdspWaitForJpeg( cdspy, snapYuvLines, snapYuvABBufOpt );
		}
		else {
			snapCdspWaitForJpeg( cdspy, snapYuvLines + snapYuvChunkLines - SNAP_JPEG_CURRENT_Y_MARGIN , snapYuvABBufOpt );
		}
#else
		snapCdspWaitForJpeg( cdspy, snapYuvLines, snapYuvABBufOpt );
#endif

		HAL_CdspImgDoABBuf( mode, 0, snapYuvHSize, i, snapYuvChunkLines, 3, 0, cdspWriteOffset );

#if 0
		if( snapYuvABBufOpt == 1 ) {
			if( bottom == 1 ) {
				snapPhotoFrmPaste( yuvAddr - (UINT32)snapYuvHSize * snapYuvChunkLines, phI++ );
			}
		}
		else {
			snapPhotoFrmPaste( yuvAddr, phI++ );
		}
#else
		snapPhotoFrmPaste( yuvAddr, phI++ );
#endif

#ifdef ICAT_OPTION
		if( sp1kSnapIcatMode == 1 ) {
		    if(iCatFlowMode != ICAT_RAW_ONLY_MODE)
				icatBulkIn( yuvAddr, (UINT32)snapYuvHSize* snapYuvChunkLines * 2 );
		}
#endif

		if( saveYuvOpt == 1 && snapYuvSaveOpt == 0 ) {
			vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card,  (UINT32)snapYuvHSize * snapYuvChunkLines * 2, yuvAddr, 0);
		}

		snapDateStampDo( yuvAddr, dateStampType, snapYuvABBufOpt, snapYuvChunkLines, i + vSub + snapYuvChunkLines );

		if( saveYuvOpt == 1 && snapYuvSaveOpt == 1 ) {
			vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card,  (UINT32)snapYuvHSize * snapYuvChunkLines * 2, yuvAddr, 0);
		}

	    cdspy += snapYuvChunkLines;
	}

	snapYuvBufSwitch( cdspy, snapYuvChunkLines, &cdspWriteOffset, &bottom, &snapBufNum, snapYuvABBufOpt );

	yuvAddr = snapYuvCurAddrGet( cdspWriteOffset, snapYuvChunkLines, bottom );

#if 1
	if( cdspy < snapYuvLines + snapYuvChunkLines ) {
		snapCdspWaitForJpeg( cdspy, snapYuvLines, snapYuvABBufOpt );
	}
	else {
		snapCdspWaitForJpeg( cdspy, snapYuvLines + snapYuvChunkLines - SNAP_JPEG_CURRENT_Y_MARGIN, snapYuvABBufOpt );
	}
#else
	snapCdspWaitForJpeg( cdspy, snapYuvLines, snapYuvABBufOpt );
#endif

	HAL_CdspImgDoABBuf( mode, 0, snapYuvHSize, i, snapYuvVSize - i - vSub, 3, 2, cdspWriteOffset );

#if 0
	if( snapYuvABBufOpt == 1 ) {
		if( bottom == 1 ) {
			snapPhotoFrmPaste( yuvAddr - (UINT32)snapYuvHSize * snapYuvChunkLines, phI++ );
		}
		else {
			snapPhotoFrmPaste( yuvAddr, phI++ );
		}
	}
	else {
		snapPhotoFrmPaste( yuvAddr, phI++ );
	}
#else
	snapPhotoFrmPaste( yuvAddr, phI++ );
#endif

#ifdef ICAT_OPTION
	if( sp1kSnapIcatMode == 1 ) {
		if(iCatFlowMode != ICAT_RAW_ONLY_MODE)
			icatBulkIn( yuvAddr, (UINT32)snapYuvHSize * ( snapYuvVSize - i - vSub ) * 2 );

		sp1kSnapIcatState = SNAP_ICATE_CDSP_FINISH;
	}
#endif

	if( saveYuvOpt == 1 && snapYuvSaveOpt == 0 ) {
		UINT32 size = (UINT32)snapYuvHSize * ( snapYuvVSize - i - vSub ) * 2;

		size = ( size + 511 ) / 512 * 512;

		vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card, size, yuvAddr, 0);
	}

	snapDateStampDo( yuvAddr, dateStampType, snapYuvABBufOpt, snapYuvChunkLines, snapYuvVSize );

	if( saveYuvOpt == 1 && snapYuvSaveOpt == 1 ) {
		UINT32 size = (UINT32)snapYuvHSize * ( snapYuvVSize - i - vSub ) * 2;

		size = ( size + 511 ) / 512 * 512;

		vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card, size, yuvAddr, 0);
	}

	if( saveYuvOpt == 1 ) {
		snapYuvFileClose();
	}

	return 0;
}

static UINT8
snapCdspVScaleDo(
	UINT8 mode
)
{
	UINT32 vrmder, tmpvini;
	UINT32 factor1;
	UINT16 vfactor, vfactini;
	UINT16 vSub, hSub, hSize;
	UINT16 cdspWriteOffset, doYSize, cdspy, i;
	UINT8 bottom;
	UINT16 dosubheight, predosubheight;
	UINT32 yuvAddr;
	UINT8 dateStampType = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DATE_STAMP_TYPE ]);
	UINT8 saveYuvOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_SAVE_YUV_OPT ]);
	UINT16 phI = 0;

	cdspy = 0;

	if( saveYuvOpt == 1 ) {
		snapYuvFileOpen();
	}

#ifdef ICAT_OPTION
	if( sp1kSnapIcatMode == 1 ) {
		sp1kSnapIcatState = SNAP_ICATE_CDSP_READY;
	}
#endif

	vrmder = ( ( UINT32 ) 65536 ) % ( ( ( UINT32 ) snapYuvVSize << 16 ) / ( UINT32 ) snapRawVSize );

	if( snapYuvVSize >= snapRawVSize ) {
		vfactor = 0;
	}
	else {
		factor1 = ( ( ( UINT32 ) snapYuvVSize << 16 ) / ( UINT32 ) snapRawVSize );
		if ( vrmder != 0 ) {
			factor1++;
		}
		vfactor = ( UINT16 ) factor1;
		vfactini = vfactor;
	}

	tmpvini = 0;
	dosubheight = predosubheight = 0;

	snapBufNum = 0;
	bottom = 0;

	if( snapYuvABBufOpt == 1 ) {
		// adjust snapDateStampYDoOffset
		snapDateStampYDoOffset = snapDateStampYDoOffset % snapYuvChunkLines;
	}

	cdspWriteOffset = 0;

	halCdspSubSizeCal( mode, vrmder, &hSize, &hSub, &vSub );

	// start
	HAL_CdspFrmCtrlEnSet(0);
 	HAL_CdspWriteBufIdxSet(0);

	yuvAddr = snapYuvCurAddrGet( cdspWriteOffset, 0, bottom );

	if( snapYuvVSize > snapYuvLines ) {
		doYSize = snapYuvLines;

		HAL_CarryWork( &tmpvini, &dosubheight, doYSize, vfactor );

		HAL_CdspVScaleDoABBuf( mode, 0, snapYuvHSize, 0, dosubheight, 3, 1, 0, vfactor, 0, vfactini, doYSize, cdspWriteOffset );
	}
	else {
		doYSize = snapYuvVSize;

		HAL_CarryWork( &tmpvini, &dosubheight, doYSize, vfactor );

		HAL_CdspVScaleDoABBuf( mode, 0, snapYuvHSize, 0, dosubheight, 3, 3, 0, vfactor, 0, vfactini, doYSize, cdspWriteOffset );
	}

 	if( saveYuvOpt == 1 && snapYuvSaveOpt == 0 ) {
		vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card, (UINT32)snapYuvHSize * doYSize * 2, snapYuv1Addr, 0);
	}

	// photo frame
	if( doYSize > snapYuvChunkLines ) {
		snapPhotoFrmPaste( yuvAddr, phI++ );
		snapPhotoFrmPaste( yuvAddr + (UINT32)snapYuvHSize * snapYuvChunkLines, phI++ );
	}
	else {
		snapPhotoFrmPaste( yuvAddr, phI++ );
	}

#ifdef ICAT_OPTION
	if( sp1kSnapIcatMode == 1 ) {
		sp1kSnapIcatState = SNAP_ICATE_CDSP_DOING;
		if(iCatFlowMode != ICAT_RAW_ONLY_MODE)
			icatBulkIn( snapYuv1Addr, (UINT32)snapYuvHSize* doYSize * 2 );
	}
#endif

	snapDateStampDo( yuvAddr, dateStampType, snapYuvABBufOpt, doYSize, doYSize );

	if( saveYuvOpt == 1 && snapYuvSaveOpt == 1 ) {
		vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card, (UINT32)snapYuvHSize * doYSize * 2, snapYuv1Addr, 0);
	}

	if( snapYuvVSize <= snapYuvLines ) {
		if( saveYuvOpt == 1 ) {
			snapYuvFileClose();
		}

		return 0;
	}

	cdspy += doYSize;
	snapYuvBufSwitch( cdspy, snapYuvChunkLines, &cdspWriteOffset, &bottom, &snapBufNum, snapYuvABBufOpt );

	snapCdspVScaleCal( vfactor, vrmder, snapYuvChunkLines, &tmpvini, &vfactini, &predosubheight, &dosubheight );

	for( i = predosubheight - vSub; (SINT16)i < (SINT16)( snapRawVSize - dosubheight - vSub ); i = i + predosubheight ) {
		snapYuvBufSwitch( cdspy, snapYuvChunkLines, &cdspWriteOffset, &bottom, &snapBufNum, snapYuvABBufOpt );

		yuvAddr = snapYuvCurAddrGet( cdspWriteOffset, snapYuvChunkLines, bottom );

		if( cdspy < snapYuvLines + snapYuvChunkLines ) {
			snapCdspWaitForJpeg( cdspy, snapYuvLines, snapYuvABBufOpt );
		}
		else {
			snapCdspWaitForJpeg( cdspy, snapYuvLines + snapYuvChunkLines - SNAP_JPEG_CURRENT_Y_MARGIN, snapYuvABBufOpt );
		}

		// vfactini != 0 means the previous last line is still used for scaling
		// Therefore, offset, i, should be minus one
		if( vfactini != 0 ) {
			HAL_CdspVScaleDoABBuf( mode, 0, snapYuvHSize, i - 1, dosubheight, 3, 0, 0, vfactor, 0, vfactini, snapYuvChunkLines, cdspWriteOffset );
		}
		else {
			HAL_CdspVScaleDoABBuf( mode, 0, snapYuvHSize, i, dosubheight, 3, 0, 0, vfactor, 0, vfactini, snapYuvChunkLines, cdspWriteOffset );
		}

		snapCdspVScaleCal( vfactor, vrmder, snapYuvChunkLines, &tmpvini, &vfactini, &predosubheight, &dosubheight );

#if 0
		if( snapYuvABBufOpt == 1 ) {
			if( bottom == 1 ) {
				snapPhotoFrmPaste( yuvAddr - (UINT32)snapYuvHSize * snapYuvChunkLines, phI++ );
			}
		}
		else {
			snapPhotoFrmPaste( yuvAddr, phI++ );
		}
#else
		snapPhotoFrmPaste( yuvAddr, phI++ );
#endif

#ifdef ICAT_OPTION
		if( sp1kSnapIcatMode == 1 ) {
		    if(iCatFlowMode != ICAT_RAW_ONLY_MODE)
				icatBulkIn( yuvAddr, (UINT32)snapYuvHSize* snapYuvChunkLines * 2 );
		}
#endif

		if( saveYuvOpt == 1 && snapYuvSaveOpt == 0 ) {
			vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card,  (UINT32)snapYuvHSize * snapYuvChunkLines * 2, yuvAddr, 0);
		}

		snapDateStampDo( yuvAddr, dateStampType, snapYuvABBufOpt, snapYuvChunkLines, cdspy + snapYuvChunkLines );

		if( saveYuvOpt == 1 && snapYuvSaveOpt == 1 ) {
			vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card,  (UINT32)snapYuvHSize * snapYuvChunkLines * 2, yuvAddr, 0);
		}

	    cdspy += snapYuvChunkLines;
	}

	snapYuvBufSwitch( cdspy, snapYuvChunkLines, &cdspWriteOffset, &bottom, &snapBufNum, snapYuvABBufOpt );

	yuvAddr = snapYuvCurAddrGet( cdspWriteOffset, snapYuvChunkLines, bottom );

#if 1
	if( cdspy < snapYuvLines + snapYuvChunkLines ) {
		snapCdspWaitForJpeg( cdspy, snapYuvLines, snapYuvABBufOpt );
	}
	else {
		snapCdspWaitForJpeg( cdspy, snapYuvLines + snapYuvChunkLines - SNAP_JPEG_CURRENT_Y_MARGIN, snapYuvABBufOpt );
	}
#else
	snapCdspWaitForJpeg( cdspy, snapYuvLines, snapYuvABBufOpt );
#endif

	// vfactini != 0 means the previous last line is still used for scaling
	// Therefore, offset, i, should be minus one
	if( vfactini != 0 ) {
		HAL_CdspVScaleDoABBuf( mode, 0, snapYuvHSize, i, snapRawVSize - i - vSub, 1, 2, 0, vfactor, 0, vfactini, snapYuvVSize - cdspy, cdspWriteOffset );
	}
	else {
		HAL_CdspVScaleDoABBuf( mode, 0, snapYuvHSize, i - 1, snapRawVSize - i - vSub, 1, 2, 0, vfactor, 0, vfactini, snapYuvVSize - cdspy, cdspWriteOffset );
	}

#if 0
	if( snapYuvABBufOpt == 1 ) {
		if( bottom == 1 ) {
			snapPhotoFrmPaste( yuvAddr - (UINT32)snapYuvHSize * snapYuvChunkLines, phI++ );
		}
		else {
			snapPhotoFrmPaste( yuvAddr, phI++ );
		}
	}
	else {
		snapPhotoFrmPaste( yuvAddr, phI++ );
	}
#else
	snapPhotoFrmPaste( yuvAddr, phI++ );
#endif

#ifdef ICAT_OPTION
	if( sp1kSnapIcatMode == 1 ) {
		if(iCatFlowMode != ICAT_RAW_ONLY_MODE)
			icatBulkIn( yuvAddr, (UINT32)snapYuvHSize * ( snapYuvVSize - cdspy ) * 2 );

		sp1kSnapIcatState = SNAP_ICATE_CDSP_FINISH;
	}
#endif

	if( saveYuvOpt == 1 && snapYuvSaveOpt == 0 ) {
		UINT32 size = (UINT32)snapYuvHSize * ( snapYuvVSize - cdspy ) * 2;

		size = ( size + 511 ) / 512 * 512;

		vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card, size, yuvAddr, 0);
	}

	snapDateStampDo( yuvAddr, dateStampType, snapYuvABBufOpt, snapYuvChunkLines, snapYuvVSize );

	if( saveYuvOpt == 1 && snapYuvSaveOpt == 1 ) {
		UINT32 size = (UINT32)snapYuvHSize * ( snapYuvVSize - cdspy ) * 2;

		size = ( size + 511 ) / 512 * 512;

		vfsFileWrite(snapYuvFHandler, K_File_SrcDataDram2Card, size, yuvAddr, 0);
	}

	if( saveYuvOpt == 1 ) {
		snapYuvFileClose();
	}

	cdspy = snapYuvVSize;

	return 0;
}

static UINT32
snapCdspWaitForJpeg(
	UINT16 posY,
	UINT16 dis,
	UINT8 yuvABBufOpt
)
{
#ifdef DRAM_8MB_10_BIT_RAW
	UINT8 raw10BitOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_10_BIT_RAW_OPT ]);
#endif

#ifdef SNAP_SKIP_JPEG_ENCODE
	return 0;
#endif

#ifdef DRAM_8MB_10_BIT_RAW
	if( yuvABBufOpt == 0 || raw10BitOpt == 1 ) {
		return 0;
	}
#else
	if( yuvABBufOpt == 0 ) {
		return 0;
	}
#endif

	if( yuvABBufOpt == 1 ) {
		// FIXME
		// 0x27fa is in unit of 4 lines
		// Jpeg may read 11 lines in advance. 1 line backward and 10 lines forward
		// Jpeg currentY is in unit of 16 lines
//		ASSERT( dis > (UINT16)( XBYTE[ 0x27fa ] << 1 ) + SNAP_JPEG_READ_IN_ADVANCE + 16, 0 );
	}

	while( ( (UINT16)XBYTE[ REG_Jpeg_Currenty ] << 4 ) < ( posY - dis ) ) {
//		printf( "0x284c=%bx, 0x2868=%bx, 0x286a=%bx\n", XBYTE[ REG_Jpeg_Currenty ], XBYTE[ REG_Jpeg_Scl_Mcux ], XBYTE[ REG_Jpeg_Scl_Mcuy_Low ] );
//		printf( "posY = %d\n", posY );

//		printf( "<%bx>", XBYTE[ REG_Jpeg_Currenty ] );

		if( ( XBYTE[REG_Jpeg_Intr_Event] & 0x02 ) == 0x02 ) {
//			printf( "jpeg finish\n" );

			break;
		}
	}

	return 0;
}

static UINT32
snapYuvBufSwitch(
	UINT16 cdspY,
	UINT16 chunkLines,
	UINT16 *cdspWriteOffset,
	UINT8 *bottom,
	UINT8 *Bufnum,
	UINT8 yuvABBufOpt
)
{
	if( yuvABBufOpt == 0 ) { // One buffer
		*cdspWriteOffset = cdspY;

		return 0;
	}

	if( *bottom == 1 ) {
		*cdspWriteOffset = 0;

		if ( *Bufnum  == 0 ) {
		 	*Bufnum = 1;
		 	HAL_CdspFrmCtrlEnSet(0);
		   	HAL_CdspWriteBufIdxSet( 1 );
	  	}
	 	else {
			*Bufnum=0;
			HAL_CdspFrmCtrlEnSet(0);
		  	HAL_CdspWriteBufIdxSet(0);
		}

		*bottom = 0;
	}
	else {
		*cdspWriteOffset = chunkLines;

		*bottom = 1;
	}

    return 0;
}

static UINT32
snapCdspVScaleCal(
	UINT16 vfactor,
	UINT32 vrmder,
	UINT16 ySize,
	UINT32 *tmpvini,
	UINT16 *vfactini,
	UINT16 *predosubheight,
	UINT16 *dosubheight

)
{
	// *tmpvini = 0 means no factor left, so start with vfactor
	*vfactini = ( *tmpvini == 0 ) ? vfactor : ( UINT16 )*tmpvini;

	*predosubheight = *dosubheight;

	HAL_CarryWork( tmpvini, dosubheight, ySize, vfactor );

	vrmder = vrmder;

	return 0;
}


static UINT32
snapCdspJpegFinish(
	UINT8 yuvABBufOpt
)
{
	yuvABBufOpt = yuvABBufOpt;

   	XBYTE[REG_Dram_Cdsp1or2]=0x10;		//{2'h0, gatingjpgreqqen, hwjpgen, 3'h0, cdspvflipen}

   	while( ( XBYTE[REG_Jpeg_Intr_Event] & 0x02 ) == 0x00 ) {
//   		printf( "REG_Jpeg_Currenty = 0x%bx\n", XBYTE[REG_Jpeg_Currenty] );
	}

#if 1
   	XBYTE[REG_Dram_TargetYuv] = 0x00;		//{7'b0, targetyuv}
#endif
	XBYTE[REG_Dram_Cdsp1or2]=0x00;

   	if( ( XBYTE[REG_Cdsp_evnt1] & 0x20 ) == 0x20 ) {
		printf( "cdsp overflow!\n" );
	}

	return 0;
}

static UINT8
snapQvDo(
	UINT8 rawMode
)
{
	UINT32 qvAddr;
	UINT8 qV, qvRotOpt, qvEffect, photoFrmOpt, dateStampType;

	qV = (UINT8)(snapParam[ SP1K_SNAP_PARAM_QV ]);
	qvRotOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_QV_ROT_OPT ]);
	qvEffect = (UINT8)(snapParam[ SP1K_SNAP_PARAM_QV_EFFECT ]);
	photoFrmOpt = (UINT8)(snapParam[ SP1K_SNAP_PARAM_PHOTO_FRM_OPT ]);
	dateStampType = (UINT8)(snapParam[ SP1K_SNAP_PARAM_DATE_STAMP_TYPE ]);

	if( snapDsQvFlag == 0 ) {
		qvAddr = snapQvAddr;
	}
	else {
		qvAddr = snapQvTemp1Addr;
	}

	// CDSP do QV
	cdspQvDo( rawMode, snapRawDataAddr, qvAddr, snapRawHSize, snapRawVSize, snapDsQvHSize, snapDsQvVSize, 100, 100);

	snapTimeRec( (UINT16)__LINE__ );

	// CDSP do effect
	if( qvEffect != SP1K_SNAP_QV_EFFECT_NORMAL ) {
		UINT8 reg21d0;
		UINT8 reg2110;

		if( snapDsQvFlag != 0 ) {
			// snapQvTemp2Addr will be used for destination and temp buffer to gprm QV
			// Buffer can be used from snapQvTemp2Addr to snapRawDataAddr
			ASSERT( snapQvTemp2Addr + (UINT32) snapDsQvHSize * snapQvVSize * 2 < snapRawDataAddr, 0 );

			HAL_GprmScale( qvAddr, snapDsQvHSize, snapDsQvVSize, snapYuv1Addr, snapQvHSize, snapQvVSize, snapQvTemp2Addr, 1 );
		}

		reg21d0 = XBYTE[ 0x21d0 ];
		reg2110 = XBYTE[ 0x2110 ];

		if( qvEffect == SP1K_SNAP_QV_EFFECT_BLACKWHITE ) {
			cdspBlackWhiteEffectSet();
		}
		else if( qvEffect == SP1K_SNAP_QV_EFFECT_SEPIA ) {
			cdspSepiaEffectSet();
		}
		else if( qvEffect == SP1K_SNAP_QV_EFFECT_BINARIZE ) {
			cdspBinarizeEffectSet();
		}

		HAL_CdspEdgeCoringMedianEnSet( 0, 0, 0, 0, 0 );

		if( snapDsQvFlag == 0 ) {
			cdspYuv2Yuv( qvAddr, snapQvEffectAddr, snapQvHSize, snapQvVSize, 128 );
		}
		else {
			cdspYuv2Yuv( snapYuv1Addr, snapQvEffectAddr, snapQvHSize, snapQvVSize, 128 );
		}

		cdspNormalEffectSet();

		XBYTE[ 0x21d0 ] = reg21d0;
		XBYTE[ 0x2110 ] = ( reg2110 & 0x01 ) | ( ( reg2110 >> 3 ) & 0x02 );
	}

	if( photoFrmOpt == 1 ) {
		if( snapDsQvHSize != snapPhHSize || snapDsQvVSize != snapPhVSize ) {

			// snapQvTemp2Addr will be used for destination and temp buffer to gprm QV
			// Buffer can be used from snapQvTemp2Addr to snapRawDataAddr
			ASSERT( snapQvTemp2Addr + (UINT32)snapPhHSize * snapDsQvVSize * 2 + (UINT32)snapDsQvHSize * snapDsQvVSize * 2 < snapRawDataAddr, 0 );

			HAL_GprmScale( snapPhotoFrmYuvAddr, snapPhHSize, snapPhVSize, snapQvTemp2Addr, snapDsQvHSize, snapDsQvVSize, snapQvTemp2Addr + (UINT32)snapDsQvHSize * snapDsQvVSize, 1 );

			HAL_GprmCopy( snapQvTemp2Addr, snapDsQvHSize, snapDsQvVSize, 0, 0,
				qvAddr, snapDsQvHSize, snapDsQvVSize, 0, 0,
				snapDsQvHSize, snapDsQvVSize, (UINT16)0x80 + 0x3a, 1 );
		}
		else {
			HAL_GprmCopy( snapPhotoFrmYuvAddr, snapPhHSize, snapPhVSize, 0, 0,
				qvAddr, snapDsQvHSize, snapDsQvVSize, 0, 0,
				snapDsQvHSize, snapDsQvVSize, (UINT16)0x80 + 0x3a, 1 );
		}
	}

	if( snapAdDateStampOpt == 1 ) {
		HAL_GprmCopy(snapQvAdDateStampAddr, snapAdDateStampQvXSize, snapAdDateStampQvYSize, 0, 0,
			qvAddr, snapDsQvHSize, snapDsQvVSize, snapAdDateStampQvXPos, snapAdDateStampQvYPos,
			snapAdDateStampQvXSize, snapAdDateStampQvYSize, (UINT16)0, 1);
	}

	if( snapDateStampOpt == 1 ) {
		//dateStampActive( snapDSInfo, snapQvDateStampTempAddr, qvAddr, snapDsQvHSize, snapDsQvVSize, snapDateStampQvXPos, snapDateStampQvYPos, snapDateStampQvXSize, snapDateStampQvYSize, 0, snapDateStampQvYSize, dateStampType );
	}

	snapTimeRec( (UINT16)__LINE__ );

	// GPRM to snapQvAddr
	if( snapDsQvFlag != 0 ) {
		// snapQvTemp2Addr will be used for destination and temp buffer to gprm QV
		// Buffer can be used from snapQvTemp2Addr to snapRawDataAddr
		ASSERT( snapQvTemp2Addr + (UINT32) snapDsQvHSize * snapQvVSize * 2 < snapRawDataAddr, 0 );

		HAL_GprmScale( qvAddr, snapDsQvHSize, snapDsQvVSize, snapQvAddr, snapQvHSize, snapQvVSize, snapQvTemp2Addr, 1 );
	}

	// At this point, QV is snapQvHSize*snapQvVSize(320*240) at snapQvAddr

	/*********************************************/
	/*                 show QV                   */
	/*********************************************/
	if( qV == 1 || snapBurstOpt == 1 ) {
		if( qvRotOpt != 0 ) {
			// using photo frame buffer
			ASSERT( photoFrmOpt != 1, 0 );

			if( qvEffect == SP1K_SNAP_QV_EFFECT_NORMAL ) {
				HAL_gprmrot( qvRotOpt, snapQvAddr, snapQvRotAddr, snapQvHSize, snapQvVSize, 1 );
			}
			else {
				HAL_gprmrot( qvRotOpt, snapQvEffectAddr, snapQvRotAddr, snapQvHSize, snapQvVSize, 1 );
			}

			snapQvShowAddr = snapQvRotAddr;

			//dispShow( snapQvShowAddr, snapQvVSize, snapQvHSize, 100 ); //will mark
		}
		else {
			if( qvEffect == SP1K_SNAP_QV_EFFECT_NORMAL ) {
				snapQvShowAddr = snapQvAddr;
			}
			else {
				snapQvShowAddr = snapQvEffectAddr;
			}

			//dispShow( snapQvShowAddr, snapQvHSize, snapQvVSize, 100 ); //will mark
		}

		HAL_GlobalReadGTimer(&snapQvShowTime);
	}

	HAL_ReadGTimer( &snapQvTime );

	snapTimeRec( (UINT16)__LINE__ );

	return 0;
}

static void
snapCdspJpegTrigger(
	UINT16 gapLines,
	UINT8 abBufOpt,
	UINT8 jpegOpt
)
{

	abBufOpt = abBufOpt;

	ASSERT( ( gapLines >= 4 ), 0 );

	if( cdspYuvCoringTime == 0 ) {
		XBYTE[REG_Dram_TargetYuv]=0x01;		//{7'b0, targetyuv}
	}
	else {
		XBYTE[REG_Dram_TargetYuv]=0x00;		//{7'b0, targetyuv}
	}

	XBYTE[0x27fa] = (UINT8)( gapLines >> 2 );      // jpeg after cdsp by N lines

#ifdef CDSP_JPEG_VLC_FLOW_CONTROL
	XBYTE[REG_Dram_II_HSize2]=0x01;      // [0]enable vlc ab buffer [5:4]16kword=32kbyte
#else
	XBYTE[REG_Dram_II_HSize2]=0x00;      // [0]enable vlc ab buffer [5:4]16kword=32kbyte
#endif

	XBYTE[REG_Dram_Cdsp1or2]=0x30;		//{2'h0, gatingjpgreqqen, hwjpgen, 3'h0, cdspvflipen}

#ifndef SNAP_SKIP_JPEG_ENCODE
	HAL_JpegEncode( 0, snapJpegVlcAddr, 0, jpegOpt, 0 ); // src address is cdsp YUV
#endif

}

static UINT32
snapYuvCurAddrGet(
	UINT16 offset,
	UINT16 lines,
	UINT8 bottom
)
{
	if( snapYuvABBufOpt == 1 ) {
		if( bottom == 0 ) {
			if( snapBufNum == 0 ) {
				return snapYuv1Addr;
			}
			else {
				return snapYuv2Addr;
			}
		}
		else {
			if( snapBufNum == 0 ) {
				return snapYuv1Addr + (UINT32)snapYuvHSize * lines;
			}
			else {
				return snapYuv2Addr + (UINT32)snapYuvHSize * lines;
			}
		}
	}
	else {
		return( snapYuv1Addr + (UINT32)offset * snapYuvHSize );
	}
}

UINT32
snapInfoGet(
	UINT8 id
)
{
	if ( id > SNAP_INFO_MAX ) {
		return FAIL;
	}

	return snapInfo[ id ];

}

/**
 * @fn        UINT8 snapDateStampDo(UINT32 addr, UINT8 type, UINT8 abBufOpt, UINT16 chkLines, UINT16 pos)
 * @brief     copy data stamp to yuv buffer
 * @param     [in] addr : yuv data address
 * @param     [in] type :
 *\n				bit[3~0]: 0: yyyy/mm/dd
 *\n                          1: dd/mm/yyyy
 *\n                          2: mm/dd/yyyy
 *\n                bit[5~4]: 0: no time
 *\n                          1: hh:mm
 *\n                          3: hh:mm:dd
 *\n                bit[7~6]: 0: no custom string
 *\n                          1: custom string with  date
 *\n                          2: custom string only
 * @param     [in] abBufOpt
 *\n				0: single yuv buffer
 *\n				1: double yuv buffer
 * @param     [in] chkLines
 * @param     [in] pos
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
UINT8
snapDateStampDo(
	UINT32 addr,
	UINT8 type,
	UINT8 abBufOpt,
	UINT16 chkLines,
	UINT16 pos
)
{

	#ifndef SNAP_TASK_TAG_TODO
		UINT16 dsYOffset, adDsYOffset;
		UINT16 ySize, adYSize;
		UINT16 mainYOffset, adMainYOffset;

		// paste ad datestamp
		if( snapAdDateStampOpt == 1 && snapAdDateStampFlag == 0 && pos > snapAdDateStampMainYPos ) {
			adDsYOffset = snapAdDateStampMainYSize - snapAdDateStampMainYRemainSize;

			if( adDsYOffset == 0 ) {
				adYSize = snapAdDateStampYDoOffset + snapAdDateStampMainYRemainSize > chkLines ? chkLines - snapAdDateStampYDoOffset : snapAdDateStampMainYRemainSize;
				adMainYOffset = snapAdDateStampYDoOffset;
			}
			else {
				adYSize = snapAdDateStampMainYRemainSize > chkLines ? chkLines : snapAdDateStampMainYRemainSize;
				adMainYOffset = 0;
			}

			if( abBufOpt == 1 ) {
				HAL_GprmCopy(snapMainAdDateStampAddr, snapAdDateStampMainXSize, snapAdDateStampMainYSize, 0, adDsYOffset,
					addr, snapYuvHSize, snapYuvLines, snapAdDateStampMainXPos, adMainYOffset,
					snapAdDateStampMainXSize, adYSize, (UINT16)0, 1);
			}
			else {
				HAL_GprmCopy(snapMainAdDateStampAddr, snapAdDateStampMainXSize, snapAdDateStampMainYSize, 0, adDsYOffset,
					addr, snapYuvHSize, snapYuvVSize, snapAdDateStampMainXPos, adMainYOffset,
					snapAdDateStampMainXSize, adYSize, (UINT16)0, 1);
			}

			snapAdDateStampMainYRemainSize -= adYSize;

			if( snapAdDateStampMainYRemainSize == 0 ) {
				snapAdDateStampFlag = 1;
			}
		}


		if( !( snapDateStampOpt == 1 && snapDateStampFlag == 0 && pos > snapDateStampMainYPos ) ) {
			return 0;
		}

		dsYOffset = snapDateStampMainYSize - snapDateStampMainYRemainSize;

		if( dsYOffset == 0 ) {
			ySize = snapDateStampYDoOffset + snapDateStampMainYRemainSize > chkLines ? chkLines - snapDateStampYDoOffset : snapDateStampMainYRemainSize;
			mainYOffset = snapDateStampYDoOffset;
		}
		else {
			ySize = snapDateStampMainYRemainSize > chkLines ? chkLines : snapDateStampMainYRemainSize;
			mainYOffset = 0;
		}

		if( abBufOpt == 1 ) {
			dateStampActive( snapDSInfo, snapMainDateStampTempAddr, snapDateStampMainXSize, snapDateStampMainYSize, addr, snapYuvHSize, snapYuvLines, snapDateStampMainXPos, mainYOffset, type );
		}
		else {
			dateStampActive( snapDSInfo, snapMainDateStampTempAddr, snapDateStampMainXSize, snapDateStampMainYSize, addr, snapYuvHSize, snapYuvVSize, snapDateStampMainXPos, mainYOffset, type );
		}

		snapDateStampMainYRemainSize -= ySize;

		if( snapDateStampMainYRemainSize == 0 ) {
			snapDateStampFlag = 1;
		}
	#else
		addr=addr;
		type=abBufOpt;
		chkLines=pos;
	#endif

	return SUCCESS;
}

/**
 * @fn        void snapDateStampInit(void)
 * @brief     initialize data stamp
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-15
 * @todo      N/A
 * @bug       N/A
*/
static void
snapDateStampInit(
	void
)
{
	UINT16 number;

	number = snapDateStampMainYPos / snapYuvChunkLines;

	snapDateStampYDoOffset = snapDateStampMainYPos - ( number * snapYuvChunkLines );
	snapDateStampMainYRemainSize = snapDateStampMainYSize;
	snapDateStampFlag = 0;

	number = snapAdDateStampMainYPos / snapYuvChunkLines;

	snapAdDateStampYDoOffset = snapAdDateStampMainYPos - ( number * snapYuvChunkLines );
	snapAdDateStampMainYRemainSize = snapAdDateStampMainYSize;
	snapAdDateStampFlag = 0;

	return;
}
#endif
