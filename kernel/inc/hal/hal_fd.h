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
 * @file      hal_fd.h
 * @brief     face detect
 * @author    wangjg
 * @since     2010-04-01
 * @date      2010-04-01
*/

#ifndef _HAL_FD_H__
#define _HAL_FD_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 #include "general.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define HAL_FD_ENABLE 1
typedef enum {
	/*param rotate, bitmask 1:h mirror, 2:v mirror, 4:x,y exchanging;
	* 0:normal, 1:h mirror, 2:v mirror, 3: rotate 180, 4: rotate 90 + h mirror,
	* 5: rotate 270, 6: rotate 90, 7: rotate 90 + v mirror
	*/
	FD_ROTATE_Normal = 0,
	FD_ROTATE_H_Mirror,
	FD_ROTATE_V_Mirror,
	FD_ROTATE_Rotate180,
	FD_ROTATE_Rotate90_HMirror,
	FD_ROTATE_Rotate270,
	FD_ROTATE_Rotate90,
	FD_ROTATE_Rotate90_VMirror,
 
}fd_rotate_t;

typedef enum {
	FD_SUBWIN_MOVE_STEP_1 = 0,
	FD_SUBWIN_MOVE_STEP_2,
	FD_SUBWIN_MOVE_STEP_4,
}fd_sub_win_moving_step;

typedef enum {
	II_Trigger_None = 0,
	II_Trigger_Fd,
	II_Trigger_Ae,
	II_Trigger_YUV,
}ii_trigger_t;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 typedef struct  {
	UINT8  func0           ;
	UINT8  func1           ;
	UINT8  roihsize0       ;
	UINT8  roihsize1       ;
	UINT8  roivsize0       ;
	UINT8  roivsize1       ;	
	UINT8  hfact0          ;
	UINT8  hfact1          ;
	UINT8  vfact0          ;
	UINT8  vfact1          ;
	UINT8  hfactini0       ;
	UINT8  hfactini1       ;
	UINT8  vfactini0       ;
	UINT8  vfactini1       ;	
	UINT8  srcaddr0        ;
	UINT8  srcaddr1        ;
	UINT8  srcaddr2        ;
	UINT8  srcaddr3        ;	
	UINT8  srchsize0       ;	
	UINT8  srchsize1       ;	
	UINT8  roihoff0        ;
	UINT8  roihoff1        ;
	UINT8  roivoff0        ;
	UINT8  roivoff1        ;
	UINT8  dstaddr0        ;
	UINT8  dstaddr1        ;
	UINT8  dstaddr2        ;
	UINT8  dstaddr3        ;
	UINT8  dsthsize0       ;
	UINT8  dsthsize1       ;
	UINT8  dummy0          ;
	UINT8  dummy1          ;
} fd_iijobtable_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 iiDbgTrig ; 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void HAL_FdTagSet(UINT16* areaEn);

UINT8 
HAL_FdTagEnable(
	UINT8 en,
	UINT8 scaleEn
	);
UINT32
HAL_FdSIISet(
	UINT32 siiaddr,
	UINT8 siihsize
);
void HAL_FdWinStepSet(UINT8 subwinstep);

void 
HAL_FdSet(
	UINT32 iiaddr,//(w)
	UINT8  iihsize,
	UINT32 fdoutaddr,//(w)
	UINT8 scalefactor,
	UINT16 areafactor
);


void HAL_FdRst(void);
UINT8 HAL_FdTrig(UINT32 waitCnt);

void HAL_FdRangeSet(UINT8 strx,UINT8 stry,UINT8 endx,UINT8 endy);
void HAL_FdDBAddrSet(UINT32 dbaddr);
void HAL_FdBufAddrSet(
		UINT32 iiaddr,
		UINT8  iihsize,
		UINT32 siiaddr,
		UINT8  siihsize,
		UINT32 fdoutaddr
		);

UINT32 HAL_FdOutTailAddrGet(void);
void HAL_FdDBRotateSet(fd_rotate_t rotate);
//#define HAL_FdDBRotateSet(rotate) XBYTE[REG_FD_DataBaseRotation] = rotate
void HAL_FdBufMaxCntSet(UINT16 maxcnt);
void HAL_FdFacSet(UINT8 scalefactor, UINT16 areafactor, UINT8 subwinstep);
void HAL_FdStage1Set(UINT8 stgmax);
//#define HAL_FdStage1Set(stgmax) XBYTE[REG_FD_StageMax] = stgmax
void HAL_FdTblini(UINT8* tbldata);

void HAL_FdIIGen(UINT16 srchsize, UINT16 srcvsize, 
		UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
		UINT8 dsthsize, UINT8 dstvsize, UINT32 srcaddr, UINT32 dstaddr);


void HAL_FdSIIGen(UINT16 srchsize, UINT16 srcvsize, 
		 UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
		 UINT8 dsthsize, UINT8 dstvsize, UINT32 srcaddr, UINT32 dstaddr);


void HAL_FdIICpuWait(void);


void HAL_FdIIScale(UINT8 *srchsize, UINT8 *srcvsize, UINT8 *dsthsize, UINT8 *dstvsize, UINT32 srcaddr, UINT32 *dstaddr);
void HAL_FdIIQueueWait(void);
UINT8 HAL_FdIIQueueDone(void);

void HAL_FdII( UINT16 yuvsrchsize, UINT16 yuvsrcvsize, 
			UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
			UINT8  iidsthsize, UINT8 iidstvsize, UINT32 yuvsrcaddr, UINT32 iisrcaddr, UINT32 siisrcaddr,
			UINT8 iisrchsize, UINT8 iisrcvsize, UINT8 *dsthsize, UINT8 *dstvsize, UINT32 *dstaddr);

void HAL_FdIISIIQueue(				
			UINT16 yuvsrchsize, //UINT16 yuvsrcvsize, 
			UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
			UINT8  iidsthsize, UINT8 iidstvsize, 
			UINT32 yuvsrcaddr, UINT32 iisrcaddr, UINT32 siisrcaddr,
			UINT8* buf,
			UINT32 queaddr);

void HAL_FdIIQueue(			
			UINT32 iisrcaddr,  //(w)
			UINT8 iisrchsize, UINT8 iisrcvsize, 
			UINT8 *dsthsize, UINT8 *dstvsize, 
			UINT32 *dstaddr, //(w)
			UINT8* buf,
			UINT32 queaddr, //(w)
			UINT8 count
			);


void HAL_FdIIJobQueue( 
			UINT16 yuvsrchsize,// UINT16 yuvsrcvsize, 
			UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
			UINT8  iidsthsize, UINT8 iidstvsize, 
			UINT32 yuvsrcaddr, //(w)
			UINT32 iisrcaddr,  //(w)
			UINT32 siisrcaddr, //(w)
			UINT8 iisrchsize, UINT8 iisrcvsize, 
			UINT8 *dsthsize, UINT8 *dstvsize, 
			UINT32 *dstaddr, //(w)
			UINT8* buf,
			UINT32 queaddr, //(w)
			UINT8 count
			);

UINT8 HAL_FdIITrig(UINT32 queaddr,UINT32 waitCnt);

void HAL_FdIIRst(void);

void HAL_FDIntrEnable(
	UINT8 IIQueueDone,
	UINT8 IICmdDone,
	UINT8 IISIIOverFlow,
	UINT8 fdDone
	);

//void HAL_FdDisable(void);
void HAL_FdEnable(UINT8 iien,UINT8 fden);

void HAL_FdSetDataBase(UINT32 addr);
void HAL_FdSetDataBaseIntr(UINT32 addr);

#endif
