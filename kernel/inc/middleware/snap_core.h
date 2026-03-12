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

#ifndef __SNAP_CORE_H__
#define __SNAP_CORE_H__


/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
//#define COMPARE_ANSWER
//#define ONE_YUVBUF
//#define DOYUV_LSC_ON
#define DOYUV_CROP
#define DOYUV_FROM_CORE
#define DARKSUB_RAWMUX



#define CDSPJOB_HSIZE	(UINT16) 400//400
#define CDSPJOB_VSIZE 	(UINT16) 32//32//128
#define CDSPJOB_HSIZE_DOFRONT 	(UINT16) 400//420
#define CDSPJOB_VSIZE_DOFRONT 	(UINT16) 64



/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum cdspsnaptype_s{
	  CDSP_SNAP_8RAW,
	  CDSP_SNAP_YUV,
	  CDSP_SNAP_10RAW,	  
} cdspsnaptype_t;
	
typedef enum cdspsnapflow_s{
	  CDSP_DO_RAW,
	  CDSP_DO_FRONT,
	  CDSP_DO_QV,
	  CDSP_DO_YUV,
	  CDSP_DO_YUV_QV,
	  CDSP_DO_YUV2YUV,
	  CDSP_DO_BLINK,
} cdspsnapflow_t;

typedef struct cdspjobtable_s {  /* total 28 bytes */
		UINT16 rhoffset 	   ;
		UINT16 rvoffset 	   ;
		UINT16 rhsize		   ;
		UINT16 rvsize		   ;
		UINT16 whoffset 	   ;
		UINT16 wvoffset 	   ;
		UINT16 hscaleaccini    ;
		UINT16 vscaleaccini    ;
		UINT16 clamphsize	   ;
		UINT8  skippixel	   ;
		UINT8  pixsw		   ;
		UINT8  dummyline	   ;
		UINT8  hmirren		   ;
		UINT8  vmirren		   ;
} cdspjobtable_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 snapCdspDoImg( UINT8 method );
UINT32 cdspDoRaw(void);
UINT32 cdspDoFront(void);	   
UINT32 cdspDoQV(UINT8 rawType,UINT32 dstAddr,UINT16 dstHSize,UINT16 dstVSize);
UINT32 cdspDoYuv(void);
UINT8 cdspYuv2Yuv(UINT32 srcAddr,UINT32 dstAddr,UINT16 srcHSize,UINT16 srcVSize,UINT16 dstHSize, UINT16 dstVSize,UINT16 doVSize);
UINT32 snapDoJpeg(UINT16 hfactor,UINT16 vfactor,UINT32 srcYuvAddr,UINT16 srcYuvWidth,UINT16 srcYuvHeight,UINT32 dstJpegVlcAddr,UINT16 dstWidth,UINT16 dstHeight);

UINT16 cdspjobget(UINT16 srcHSize, UINT16 srcVSize, UINT16 cropHSize, UINT16 cropVSize, UINT16 dstHSize, UINT16 dstVSize, UINT16 jobHSize, UINT16 jobOutVSize, UINT8  pixelSw, 
					UINT8 yuvDoCdsp, UINT8 yuvScale, UINT8 get, cdspjobtable_t xdata* pJobTbl); 
	   
UINT32 snapVlcSizeGet(void);



//UINT8 cdspYuv2Yuv(UINT32 srcAddr,UINT32 dstAddr,UINT16 srcHSize,UINT16 srcVSize,UINT16 dstHSize, UINT16 dstVSize,UINT16 doVSize);
#endif

