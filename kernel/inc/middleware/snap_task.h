/**************************************************************************
 *         Copyright(c) 2010 by ICatch Technology Co., Ltd.                *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/

#ifndef __SNAP_TASK_H__
#define __SNAP_TASK_H__

#include "hal_global.h"

/**************************************************************************
 *                           O P T I O N                           *
 **************************************************************************/
#define SNAP_NORMAL_STAMP_WITH_HOST_STAMP	0
#define STAMP_RTC_NUMBER	3
#define SNAP_DOFLOW_BURST_CAPTURE_BG_STAMP_UPDATE	0
#define SNAP_DOFLOW_BURST_NUMBER	STAMP_RTC_NUMBER



/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SNAP_CDSP_QV_HSIZE (UINT16)( 1280 )
#define SNAP_CDSP_QV_VSIZE (UINT16)( 960 )

#define SNAP_CDSP_QV_HSIZE_16_9 (UINT16)( 1280 )
#define SNAP_CDSP_QV_VSIZE_16_9 (UINT16)( 720 )


//#define SNAP_CDSP_QV_HSIZE ( snapParam[SP1K_SNAP_PARAM_QV_WIDTH] )
//#define SNAP_CDSP_QV_VSIZE ( snapParam[SP1K_SNAP_PARAM_QV_HEIGHT] )

#define SNAP_CDSP_THB_HSIZE (UINT16)( 160 )
#define SNAP_CDSP_THB_VSIZE (UINT16)( 120 )

#define SNAP_CDSP_BLINK_HSIZE (UINT16)( 640 )
#define SNAP_CDSP_BLINK_VSIZE (UINT16)( 480 )


/*  date stamp */

#if 1
	#define SNAP_DATE_STAMP_MAIN_DEFAULT_H_SIZE	( 48 )
	#define SNAP_DATE_STAMP_MAIN_DEFAULT_V_SIZE	( 80 )
	#define SNAP_DATE_STAMP_QV_DEFAULT_H_SIZE       ( 6  )
	#define SNAP_DATE_STAMP_QV_DEFAULT_V_SIZE       ( 10 )
#else
	#define SNAP_DATE_STAMP_MAIN_DEFAULT_H_SIZE	( 32 )
	#define SNAP_DATE_STAMP_MAIN_DEFAULT_V_SIZE	( 64 )
#endif

#if 0 // FIXME: memory is not enough for AD DS
	#define SNAP_CDSP_DS_QV_W ( 640 )
	#define SNAP_CDSP_DS_QV_H ( 480 )
#else
	/* mantis 34864 */
	/* For ds, if DS QV is 320*240, ds size is 5*10 and align to 6*10 */
	/* which cause the problem. */
	/* Therefore, DS QV is 384*288, ds size is 6*12 */
	//#define SNAP_CDSP_DS_QV_W ( 384 )
	//#define SNAP_CDSP_DS_QV_H ( 288 )
	#define SNAP_CDSP_DS_QV_W ( 1280 )
	#define SNAP_CDSP_DS_QV_H ( 960 )

	#define SNAP_CDSP_DS_QV_W_16_9 ( 1280 )
	#define SNAP_CDSP_DS_QV_H_16_9 ( 720 )
	
#endif



#if 1
	#define SNAP_DATE_STAMP_MAIN_DEFAULT_H_REF_SIZE	 (UINT16)2592 
	#define SNAP_DATE_STAMP_MAIN_DEFAULT_V_REF_SIZE	 (UINT16)1944 

	#define SNAP_DATE_STAMP_QV_DEFAULT_H_REF_SIZE	 (UINT16)320 
	#define SNAP_DATE_STAMP_QV_DEFAULT_V_REF_SIZE	 (UINT16)240
	
#else
	#define SNAP_DATE_STAMP_MAIN_DEFAULT_H_REF_SIZE	( 2048 )
	#define SNAP_DATE_STAMP_MAIN_DEFAULT_V_REF_SIZE	( 1536 )
#endif

#define SNAP_DEFAULT_STAMP_X_RATIO ( 50 )
#define SNAP_DEFAULT_STAMP_Y_RATIO ( 58 )

#define SNAP_DATE_STAMP_MAX_RATIO ( 64 )

#define SNAP_BG_DEFAULT_X_REF_SIZE	( 640 )
#define SNAP_BG_DEFAULT_Y_REF_SIZE	( 480 )


/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

typedef enum snapYuvBufType_e{
	SNAP_TWO_YUVBUF=0,
	SNAP_ONE_YUVBUF,
} snapYuvBufType_t;

typedef enum snapBPMode_e{
	SNAP_BP_DISABLE=0,
	SNAP_BP_OFFLINE,
	SNAP_BP_REALTIME,

} snapBPMode_t;


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void snapPhotoFrmPaste( UINT32 addr, UINT16 blkIdx );
UINT8 snapPhotoFrmDecode(UINT32 addr);


UINT32 snapJpgFileSizeGet_Intr(void);

void chgRawFileNameSet( UINT8 *filename);
void cdspBinFileNameSet( UINT8 *filename );

void chgRawSizeSet( UINT16 hsize, UINT16 vsize);
void chgRawSizeGet( UINT16 *hsize, UINT16 *vsize);


UINT8 snapParamSet( UINT8 attrId, UINT16 value );
UINT16 snapParamGet( UINT8 attrId );

//UINT8 snapDateStampDo( UINT32 addr, UINT8 type, UINT8 abBufOpt, UINT16 chkLines, UINT16 pos );

void snapRegDump(UINT8 idx);
UINT8 snapMemInit( void );
UINT8 snapStore(dateStc_t* pRtc);
UINT8 snapPvTurnOn(void);

UINT8 snapVLCOverflowGet(void);

void snapStampQvPrepare(void);
void snapStampMainPrepare(void);
void snapStampDoFrontBurstMainPrepare(void);


#endif

