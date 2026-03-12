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
 * @file      front_vd.c
 * @brief     Brief
 * @author    wangjg
 * @since     2010-06-25
 * @date      2010-02-25
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "hal_global.h"
#include "sp1k_front_api.h"
#include "awb_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_fd_api.h"
#include "dist_func.h"
#include "initio.h"
#include "osd_buff.h"
#include "app_video_view.h"
#include "motion_det.h"
#include "front_vd_debug.h"
#include "reg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_LIB_FORNT_VD__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if VD_DEBUG_EN
static xdata UINT8 vdPrintCnt=0; 
#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
xdata UINT8 vdProcEn = 0; 
xdata UINT8 vdCount = 0;

VD_AVERAGE_TIME vdAverageTime;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        void sp1kVdInit(void)
 * @brief     sp1k Vd  Init
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-6
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kVdInit(
	void
)
{
	/* vd */
	XBYTE[REG_Front_Finten_low] &= ~0x02;
	
	/* number of vd rise equal to vdintnum, front vvd rising, front vvd falling */
	XBYTE[REG_Front_Finten_low] |= 0x61;	/* valid failing for dist and ae, valid vising for ae */
	
	XBYTE[REG_Cdsp_Cdspinten_lo] &= ~0x1C;

	/* VDRIntNum, VDFIntNum */
	XBYTE[REG_Front_VDRIntNum] = 0x11;	
}


/**
 * @fn        UINT8 sp1kVdProcEnable(UINT8 flag, UINT8 en)
 * @brief     sp1kVdProcEnable
 * @param     [in] flag	VD_PROC_AE		:set/get ae status
 * 					VD_PROC_AWB	:set/get awb status
 *					VD_PROC_FD		:set/get fd status
 *					VD_PROC_DIST	:set/get dist status
 *					VD_PROC_GET	:return all status
 * @param     [in] en	1		:enable
 *					0		:disable
 *					0xff		:return status
 * @retval    return = last status
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
sp1kVdProcEnable(
	UINT8 flag,
	UINT8 en
)
{
	UINT8 proc = vdProcEn;
	
	if (flag==0xff) {
		return vdProcEn;
	}
	
	if (en==0xff) {
		return vdProcEn&flag;
	}
	
	vdProcEn = (vdProcEn&~flag)|(en?flag:0);
	
	return flag & proc;
}

/**
 * @fn        UINT8 sp1kVdCountGet(void)
 * @brief     sp1kVdCountGet
 * @param     NONE
 * @retval    return = vd counter
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kVdCountGet()
{
	return vdCount;
}

/**
 * @fn        void vdSvcProc(void* ptr)
 * @brief     vdSvcProc
 * @param     [in] ptr	:in param
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-6
 * @todo      N/A
 * @bug       N/A
*/
void 
vdSvcProc(
	void* ptr
)
{
	UINT8 mode = 0;
	#if VD_DEBUG_EN	
	static UINT32 vdt0,vdt1,vdt2,vdt3,vdt4,vdt5,vdt6;
	#endif
	
	if (vdProcEn & VD_PROC_EN) { //vd
		
		#if VD_DEBUG_EN		
		if (vdPrintCnt) {
			
			if (vdPrintCnt!=0xff) {
				vdPrintCnt--;
			}
			
			frontvdSysInfo("en:%2bx,cnt:%2bx,mde:%2bx,ae:%4lu,awb:%4lu,osd:%4lu,fd:%4lu,dist:%4lu,frm:%lu\n"
				,vdProcEn,vdCount,mode
				,vdt1-vdt0
				,vdt2-vdt1
				,vdt3-vdt2
				,vdt4-vdt3
				,vdt5-vdt4
				,vdt0-vdt6
			);
			/*
			frontvdSysInfo("ae:%4lu,awb:%4lu,osd:%4lu,fd:%4lu,dist:%4lu,frm:%lu\n"
				,vdt1-vdt0
				,vdt2-vdt1
				,vdt3-vdt2
				,vdt4-vdt3
				,vdt5-vdt4
				,vdt0-vdt6
			);
			*/
		}
		#endif
		
		vdProcEn &= 0x7f;

		#if VD_DEBUG_EN		
		vdt6 = vdt0;
		#endif
		
		frontvdTimeInfo(&vdt0);		
		
		#if 0
		if ((vdProcEn & VD_PROC_AE) 
			&&(vdCount & 0x03)==3) {
			aeSvcProc(&vdCount);
		}
		#endif
		
		frontvdTimeInfo(&vdt1);
		
		if ((vdProcEn & VD_PROC_AWB) 
			//&& (vdCount & 0x07)==1
			) {			
			awbSvcProc(&vdCount);
		}
		
		frontvdTimeInfo(&vdt2);
		
		/* osd remove */	
		frontvdTimeInfo(&vdt3);
		if ((vdProcEn & VD_PROC_FD) 
			) {			
			//if ((vdCount & 0x03)==2) {
				fdSvcProc(&vdCount);
			//}
			//fdDraw();
		}
		
		frontvdTimeInfo(&vdt4);
		
		if ((vdProcEn & VD_PROC_DIST)
			//&&((vdCount & 0x01)==1)	
			) {
			distSvcProc(&vdCount);
		}
		frontvdTimeInfo(&vdt5);

		#if MOTION_DET_EN
		if (vdProcEn & VD_PROC_MD) {
			mdSvcProc(&vdCount);
		}
		#endif
		
		ptr = ptr;		
	}
}

/**
 * @fn        void sp1kVdAvgTimeSvcProc(void)
 * @brief     VdAvgTimeSvcProc
 * @param     [in] pt	:test string
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-6
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kVdAvgTimeSvcProc (
	void
)
{
	UINT8 vdc = vdCount;

	//printf("c=%bu,%bu,%bu\n",vdCount,vdAverageTime.vdCnt_s,(UINT8)(~(vdAverageTime.vdCnt_s)));
	
	if ( vdAverageTime.vdCnt_s == 0) {
		vdAverageTime.vdCnt_s = vdc;
		//printf("v=%bu\n",vdc);
		HAL_GlobalTimerRead10us(&vdAverageTime.timeTick_s);
	}
	else if ( (UINT8)((UINT8)vdc + ((UINT8)(~(vdAverageTime.vdCnt_s)) + (UINT8)1)) > 30 ) {
		vdAverageTime.vdCnt_e = vdc;
		HAL_GlobalTimerRead10us(&vdAverageTime.timeTick_e);
		
		vdAverageTime.vdAvgTime = (vdAverageTime.timeTick_e + (UINT32)(~(vdAverageTime.timeTick_s)) + 1)/(UINT8)((UINT8)vdAverageTime.vdCnt_e + ((UINT8)(~(vdAverageTime.vdCnt_s)) + (UINT8)1));
		if(vdAverageTime.vdAvgTime>(UINT32)3400){
			printf("vdAvgTime=%lu\n",vdAverageTime.vdAvgTime);
		}
		vdAverageTime.vdCnt_s = 0;
	}
}


#if VD_DEBUG_EN
//extern UINT32 DummyFrameCnt;
//extern UINT32 distMissCnt;
/**
 * @fn        void vdTest(UINT8* pt)
 * @brief     vdTest
 * @param     [in] pt	:test string
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-6
 * @todo      N/A
 * @bug       N/A
*/
void 
vdTest(
	UINT8* pt
)
{
	UINT8 i;
	UINT32 val[5],c=0,n=0,j=-1;
	
	memset(val,0,20);
	
	for (i=0;i<20;i++) {
		UINT8 b = pt[i+1];
		UINT8 a = 0;
		
		if (b==0) {
			if (j+1!=i) {				
				val[n++] = c;
			}	
			break;
		}
		else if (b==' ') {
			if (j+1!=i) {				
				val[n++] = c;
				c = 0;
			}			
			j = i;
			continue;
		}
		else if (b>='0' && b<='9') {
			a = b-'0';
		}
		else if (b>='a' && b<='f') {
			a = b-'a'+10;
		}
		else if (b>='A' && b<='F') {
			a = b-'A'+10;
		}
		else {
			continue;	
		}

		c = c*16+a;
	}

	frontvdSysInfo("%s,(%lx,%lx,%lx,%lx,%lx)\n",pt,val[0],val[1],val[2],val[3],val[4]);
	
	if (*pt=='p') {
		vdPrintCnt = val[0];
	}
	else if (*pt=='e') {
		vdProcEn = val[0];
	}
	else {
		/*
		frontvdSysInfo("miss cnt:\nrec:%lx\ndist:%lx\n"
			, DummyFrameCnt
			,distMissCnt
		);
		*/
	}
}
#endif
