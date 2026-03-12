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
 * @file      awb_api.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __AWB_API_H__
#define __AWB_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sp1k_awb_api.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef struct
{
	UINT8 darkLuma;
	UINT8 brightLuma;
	UINT8 darkGid;
	UINT8 brightGid;	
	UINT8 darkContrast;
	UINT8 brightContrast;	
	UINT8 jumpMax;
	UINT8 jumpStable;
	
	UINT8 shiftRg;
	UINT8 shiftBg;
	UINT8 spotThr;
	UINT8 stepCnt;	
	UINT8 scene;
	UINT8 stable;//bit0=stable,bit1=tag stable,bit2=set stable
	UINT8 stepIdx;
	UINT8 jumpIdx;
	
	UINT8 sceneIdx; 
	UINT8 spotIdx;
	UINT8 fixWp;	
	UINT8 luma;
	UINT8 gid;
	UINT8 contrast;
	UINT8 proc;
	UINT8 mode;
	
	UINT8 spotEn[5];	
	UINT8 obEn;
	UINT8 obThr;
	UINT8 ob0[4];
	UINT8 ob[4];
	UINT8 passIdx;
	
	UINT8 rawFmt;
	/*
	UINT8 obR0;
	UINT8 obGr0;
	UINT8 obB0;
	UINT8 obGb0;
	UINT8 obR;
	UINT8 obGr;
	UINT8 obB;
	UINT8 obGb;
	*/
	UINT16 ctcRg[4],ctcBg[4];
	UINT16 avgRg, avgBg;
	UINT16 tagRg, tagBg;
	UINT16 setRg, setBg;
	UINT16 fixRg, fixBg ;
	UINT16 globeGain;

	//UINT32 addrQueue[5];		
	//UINT32 addrInfoPara;		// 512-16
	//UINT32 addrInfoCtt;		// 256
	//UINT32 addrOut;			// 240=24*10
	//UINT32 addrSamp;		// 3k
	//UINT32 addrTmp;
	//UINT32 addrWnd;
	//UINT32 addrWndRst;
	UINT16 flowFlag;
	
	UINT8 queueCreate;
	UINT8 aLightMode;
	
	UINT8 cmEn;
	
	UINT8 satEnDark;
	
	UINT8 specMode;

	UINT8 spotDelayIdx;
	UINT8 jumpErr;
	UINT8 gainSetCnt;
	//UINT8 initComplete;
	UINT8 newGain;
	
	UINT8 spotDelayCnt;
	UINT16 fixRg0,fixRg1,fixBg0,fixBg1;
	//UINT16 rangeRg0,rangeRg1,rangeBg0,rangeBg1;
	//UINT16 ctmin,ctmax;
	UINT8 jumpStep;
}awb_t;
typedef struct
{
	UINT32 buf;
	UINT32 wnd;
	UINT32 moe;	
	UINT32 que;
	UINT32 queue[5];
	UINT32 ctc;
	UINT32 para;	
	UINT32 addr;
}awb_addr_t;

typedef struct 
{
	// 0
	UINT16 header[8];
	// 1
	UINT16 ctc[4*2];
	// 2
	UINT16 cal[4*2];
	// 3 4 5
	UINT8 ratio[16*3];
	// 6 7 8 9
	UINT16 rect[4*8];
	// a b c d e f
	awb_t param;//87
	//
	//UINT8 reserve[256-160-sizeof(awb_t)];
}awb_ctt_t; //v5001
#define AWB_SIZE_Addr	sizeof(awb_addr_t)
#define AWB_SIZE_Ctt 256//(sizeof(awb_ctt_t)

typedef struct
{	
	UINT8 flag[8];//55 aa 00 00 55 aa ff ff  //8
	
	awb_ctt_t ctt;					//256
	UINT8 out[240]; 				//240
	awb_addr_t addr;				//32
	UINT8 reserve[1024-sizeof(awb_ctt_t)-240-sizeof(awb_addr_t)-8];
	
	UINT8 r[1024];
	UINT8 g[1024];
	UINT8 b[1024];
}awb_fileInfo_t;

typedef enum
{
	AWB_CM_A = AWB_FUNC_Mode,
	AWB_CM_CW,
	AWB_CM_D,
	AWB_CM_Auto,
	
}awb_cm_t;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define AWB_GAIN_BASE 256
#define AWB_CDSP_GAIN_BASE 64
#define AWB_GLOBE_GAIN_BASE 256
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//0:normal,1:flash,2:calibration
void awbPostProc();
void awbSvcProc(void* ptr);

void awbCMSet(awb_cm_t mode);

/**
 * @fn        void awbFileInfo(UINT32 addr)
 * @brief     save awb info to exif(call by snap)
 * @param     [in] addr
 * @retval    return awb info addrW(word)
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT32 awbFileInfo(UINT32 addrW);
UINT16 awbGlobeGainSetIntr(UINT16 gain);
#define awbGainUpdateSyncVdIntr() awbGlobeGainSetIntr(0)
#define awbGlobeGainGetIntr() awbGlobeGainSetIntr(0xffff)

void awbIntrTrigger();

/**
 * @fn        UINT16 sp1kAwbGlobleGainSet(UINT16 gain)
 * @brief     set globe gain* awb gain  to cdsp
 * @param     [in] gain :base on 0x40, if gain==0xffff,return globle gain only
 * @param     [in] sycnVd :0-immediately,1-next vd
 * @retval    last globle gain
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT16 
awbGlobeGainSet(
	UINT16 gain
	);
UINT16 
awbGlobeGainSetIntr(
	UINT16 gain
	);


void sp1kAwbGainAdjust(UINT16 rGainPer , UINT16 bGainPer);

void awbGainSetIntr(UINT16 rg,UINT16 bg);

void 
awbGainSet(
	UINT16 rg,
	UINT16 bg
	);

void 
awbTest(
	UINT8* pt
	);


/**
 * @fn        void awbGainReset(void)
 * @brief     reset gain to cdsp
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
#define awbGainReset() awbGainSet(0xffff, 0xffff)


#endif  /* __TEMPLATE_HDR_H__ */
