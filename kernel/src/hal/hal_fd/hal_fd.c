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
 * @file      hal_fd.c
 * @brief     face detect hal function
 * @author   wangjg
 * @since     2010-04-01
 * @date      2010-04-01
*/
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 #include "general.h"
//#include "initio.h"
#include "hal_fd.h"
#include "hal_dram.h"
#include "hal.h"
#include "reg_def.h"
#include "fd_debug.h"

#if HAL_FD_ENABLE
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define FD_FUNC_NoUse	0 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern UINT8 G_ucStorData[];

/***********************************************
 *	typedef enum ii_trigger_t
 *	0: N/A
 *	1: FD flow II trigger
 *	2: AE flow II trigger
***********************************************/
UINT8 iiDbgTrig = II_Trigger_None;//0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 
HAL_FdSIISet(
	UINT32 siiaddr,
	UINT8 siihsize
)
{
	UINT32 rst = 0;
	if (siiaddr==0xffffffff) {
		WRITE8(rst,3,XBYTE[REG_Dram_SIIAddr2_B0]);
		WRITE8(rst,2,XBYTE[REG_Dram_SIIAddr2_B1]);
		WRITE8(rst,1,XBYTE[REG_Dram_SIIAddr2_B2]);
		WRITE8(rst,0,XBYTE[REG_Dram_SIIAddr2_B3]);
	}
	else if (siihsize==0xff) {
		rst = siihsize;
	}	
	else {
		XBYTE[REG_Dram_SIIAddr2_B0] = ((unsigned char *)&(siiaddr))[3];
		XBYTE[REG_Dram_SIIAddr2_B1] = ((unsigned char *)&(siiaddr))[2];
		XBYTE[REG_Dram_SIIAddr2_B2] = ((unsigned char *)&(siiaddr))[1];
		XBYTE[REG_Dram_SIIAddr2_B3] = ((unsigned char *)&(siiaddr))[0];

		XBYTE[REG_Dram_SII_HSize2] = siihsize;
	}
	
	return rst;
}
void HAL_FdWinStepSet(UINT8 subwinstep)
{
	XBYTE[REG_FD_SubWinStep] = subwinstep;
}
void 
HAL_FdSet(
	UINT32 iiaddr,//(w)
	UINT8  iihsize,
	UINT32 fdoutaddr,//(w)
	UINT8 scalefactor,
	UINT16 areafactor
)
{
	XBYTE[REG_Dram_IIAddr2_B0] = ((unsigned char *)&(iiaddr))[3];
	XBYTE[REG_Dram_IIAddr2_B1] = ((unsigned char *)&(iiaddr))[2];
	XBYTE[REG_Dram_IIAddr2_B2] = ((unsigned char *)&(iiaddr))[1];
	XBYTE[REG_Dram_IIAddr2_B3] = ((unsigned char *)&(iiaddr))[0];
	XBYTE[REG_Dram_II_HSize2] = iihsize;
	
	XBYTE[REG_FD_OutStartAddr0] = ((unsigned char *)&(fdoutaddr))[3];
	XBYTE[REG_FD_OutStartAddr1] = ((unsigned char *)&(fdoutaddr))[2];
	XBYTE[REG_FD_OutStartAddr2] = ((unsigned char *)&(fdoutaddr))[1];
	XBYTE[REG_FD_OutStartAddr3] = ((unsigned char *)&(fdoutaddr))[0];	
	

	XBYTE[REG_FD_SaleFactor] = scalefactor;
	XBYTE[REG_FD_Areafactor0] = ((unsigned char *)&(areafactor))[1];
	XBYTE[REG_FD_Areafactor1] = ((unsigned char *)&(areafactor))[0];
}
/**
 * @fn        void HAL_FdTagSet(UINT16* areaEn)
 * @brief     HAL_FdTagSet
 * @param     [in] areaEn
 * @retval    NONE
 * @author    wangjg
 * @since     2010-6-21
*/
void HAL_FdTagSet(
	UINT16* areaEn
	)
{
	if (areaEn) {
		//memcpy(&(XBYTE[REG_FD_Tag000]),(UINT8*)areaEn,16);
		UINT8 i;
		UINT8 *pt = areaEn;

		for (i=0;i<16;i+=2) {
			XBYTE[REG_FD_Tag000+i]=pt[i+1];
			XBYTE[REG_FD_Tag001+i]=pt[i];
		}
	}
}
/**
 * @fn        UINT8 HAL_FdTagEnable(UINT8 en, UINT8 scaleEn)
 * @brief     HAL_FdTagEnable
 * @param     [in] en		:0xff: return XBYTE[REG_FD_TagEnable]
 * @param     [in] scaleEn
 * @retval    return = XBYTE[REG_FD_TagEnable]
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-21
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
HAL_FdTagEnable(
	UINT8 en,
	UINT8 scaleEn
	)
{
	if (en!=0xff) {
		XBYTE[REG_FD_TagEnable] = (en?0x01:0) | (scaleEn?0x02:0);
	}
	return XBYTE[REG_FD_TagEnable];
}
/**
 * @fn        void HAL_FdRst(void)
 * @brief     face detect fw reset
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdRst(void)
{
	XBYTE[REG_FD_SWReset] = 1;
	XBYTE[REG_FD_SWReset] = 1;
	XBYTE[REG_FD_SWReset]   = 0;
}
/*
void HAL_FdDisable()
{
	//XBYTE[REG_II_ModuleControl] = 0x11;
	//XBYTE[REG_II_ModuleControl] = 0x11;
	XBYTE[REG_FD_SWReset] = 0x01;
	//XBYTE[REG_FD_SWReset] = 0x01;
	XBYTE[REG_FD_ClockGating] = 0x01;
	//XBYTE[REG_FD_ClockGating] = 0x01;
}*/
void HAL_FdEnable(UINT8 iien,UINT8 fden)
{
	//XBYTE[REG_II_ModuleControl] = 0;
	XBYTE[REG_II_ModuleControl] = iien?0:0x11;//0x00->disable ii soft reset, enable clock gating
								//0x11->enable ii soft reset, disable clock gating
	//XBYTE[REG_FD_SWReset] = 0;
	XBYTE[REG_FD_SWReset] = fden?0:1;	//0x00->disable fd local reset normal
								//0x01->enable fd local reset reset
	//XBYTE[REG_FD_ClockGating] = 0;
	XBYTE[REG_FD_ClockGating] = fden?0:1;	//0x00->enable fd clock gating 
								//0x01->disable fd clock gating
	
}

/**
 * @fn        void HAL_FdTrig(void)
 * @brief     face detect trigger
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_FdTrig(UINT32 waitCnt)
{
	UINT32 cnt = waitCnt+1;
	while (XBYTE[REG_FD_Trigger]  && cnt) {
		if (!(--cnt)) {
			//halfdSysInfo("fdBusyCnt:%lx\n",waitCnt);
			return FALSE;
		}
	}
	//halfdSysInfo("fd:%bx\n",XBYTE[REG_FD_Trigger]);
	XBYTE[REG_FD_Trigger]   = 1;
	return TRUE;
}

/*FIXME*/ /*coordinates only 0..255?*/
/*FIXME*/ /*what's different from HAL_FdRangeSet()?*/
/*FIXME*/ /*what's different from HAL_FdIIRangeSet()?*/
/**
 * @fn        void HAL_FdRangeSet(UINT8 strx, UINT8 stry, UINT8 endx, UINT8 endy)
 * @brief     face detect RIO set
 * @param     [in] strx
 * @param     [in] stry
 * @param     [in] endx
 * @param     [in] endy
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdRangeSet(
	UINT8 strx, UINT8 stry, 
	UINT8 endx, UINT8 endy
	)
{
	XBYTE[REG_FD_ROIx0] = strx;
	XBYTE[REG_FD_ROIy0] = stry;
	XBYTE[REG_FD_ROIx1] = endx;
	XBYTE[REG_FD_ROIy1] = endy;
}

/**
 * @fn        void HAL_FdDBAddrSet(UINT32 dbaddr)
 * @brief     face detect data base Dram addr(w) set;
 * @param     [in] dbaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdDBAddrSet(UINT32 dbaddr)//(w)
{
	XBYTE[REG_FD_DataBaseAddr0] = ((unsigned char *)&(dbaddr))[3];
	XBYTE[REG_FD_DataBaseAddr1] = ((unsigned char *)&(dbaddr))[2];
	XBYTE[REG_FD_DataBaseAddr2] = ((unsigned char *)&(dbaddr))[1];
	XBYTE[REG_FD_DataBaseAddr3] = ((unsigned char *)&(dbaddr))[0];
}
#if 0
/**
 * @fn        void HAL_FdBufAddrSet(UINT32 iiaddr, UINT8 iihsize, UINT32 siiaddr, UINT8 siihsize, UINT32 fdoutaddr)
 * @brief     face detect temp & out buf Dram addr(w) set
 * @param     [in] iiaddr(w)
 * @param     [in] iihsize
 * @param     [in] siiaddr(w)
 * @param     [in] siihsize
 * @param     [in] fdoutaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdBufAddrSet(
		UINT32 iiaddr,//(w)
		UINT8  iihsize,
		UINT32 siiaddr,//(w)
		UINT8  siihsize,
		UINT32 fdoutaddr//(w)
		)
{
	XBYTE[REG_Dram_IIAddr2_B0] = ((unsigned char *)&(iiaddr))[3];
	XBYTE[REG_Dram_IIAddr2_B1] = ((unsigned char *)&(iiaddr))[2];
	XBYTE[REG_Dram_IIAddr2_B2] = ((unsigned char *)&(iiaddr))[1];
	XBYTE[REG_Dram_IIAddr2_B3] = ((unsigned char *)&(iiaddr))[0];
	
	XBYTE[REG_Dram_SIIAddr2_B0] = ((unsigned char *)&(siiaddr))[3];
	XBYTE[REG_Dram_SIIAddr2_B1] = ((unsigned char *)&(siiaddr))[2];
	XBYTE[REG_Dram_SIIAddr2_B2] = ((unsigned char *)&(siiaddr))[1];
	XBYTE[REG_Dram_SIIAddr2_B3] = ((unsigned char *)&(siiaddr))[0];
	
	XBYTE[REG_Dram_SII_HSize2] = siihsize;
	XBYTE[REG_Dram_II_HSize2] = iihsize;
	
	XBYTE[REG_FD_OutStartAddr0] = ((unsigned char *)&(fdoutaddr))[3];
	XBYTE[REG_FD_OutStartAddr1] = ((unsigned char *)&(fdoutaddr))[2];
	XBYTE[REG_FD_OutStartAddr2] = ((unsigned char *)&(fdoutaddr))[1];
	XBYTE[REG_FD_OutStartAddr3] = ((unsigned char *)&(fdoutaddr))[0];
		
}
#endif
#if FD_FUNC_NoUse
/**
 * @fn        UINT32 HAL_FdOutTailAddrGet(void)
 * @brief     get face detect out tail dram addr(w)
 * @param     NONE
 * @retval    return = out tail dram addr(w)
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT32 HAL_FdOutTailAddrGet(
		void
		)
{
  UINT32 fdoutaddr;
  ((unsigned char *)&(fdoutaddr))[3] = XBYTE[REG_FD_OutCurrentAddr0];
	((unsigned char *)&(fdoutaddr))[2] = XBYTE[REG_FD_OutCurrentAddr1];
	((unsigned char *)&(fdoutaddr))[1] = XBYTE[REG_FD_OutCurrentAddr2];
	((unsigned char *)&(fdoutaddr))[0] = XBYTE[REG_FD_OutCurrentAddr3];	
	return fdoutaddr;
}
#endif
/**
 * @fn        void HAL_FdDBRotateSet(UINT8 rotate)
 * @brief     set face detect rangle
 * @param     [in] rotate
 *			bitmask 	1:h mirror, 2:v mirror, 4:x,y exchanging;
 * 			0:normal, 
 *			1:h mirror, 
 *			2:v mirror, 
 *			3: rotate 180, 
 *			4: rotate 90 + h mirror,
 * 			5: rotate 270, 
 *			6: rotate 90, 
 *			7: rotate 90 + v mirror
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdDBRotateSet(fd_rotate_t rotate)
{
	XBYTE[REG_FD_DataBaseRotation] = rotate;
}
/**
 * @fn        void HAL_FdBufMaxCntSet(UINT16 maxcnt)
 * @brief     set fd max face count 
 * @param     [in] maxcnt (0:no limit)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdBufMaxCntSet(
		UINT16 maxcnt
		)
{
	XBYTE[REG_FD_OutMaxCnt0] = ((unsigned char *)&(maxcnt))[1];
	XBYTE[REG_FD_OutMaxCnt1] = ((unsigned char *)&(maxcnt))[0];
}

#if FD_FUNC_NoUse
/*FIXME*/ /*meaning and range?*/
/**
 * @fn        void HAL_FdFacSet(UINT8 scalefactor, UINT16 areafactor, UINT8 subwinstep)
 * @brief     set fd scale/area/sub win setp
 * @param     [in] scalefactor
 * @param     [in] areafactor
 * @param     [in] subwinstep
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdFacSet(UINT8 scalefactor, UINT16 areafactor, UINT8 subwinstep)
{
	XBYTE[REG_FD_SaleFactor] = scalefactor;
	XBYTE[REG_FD_Areafactor0] = ((unsigned char *)&(areafactor))[1];
	XBYTE[REG_FD_Areafactor1] = ((unsigned char *)&(areafactor))[0];
	XBYTE[REG_FD_SubWinStep] = subwinstep;// / 2;
}
#endif
/**
 * @fn        void HAL_FdStage1Set(UINT8 stgmax)
 * @brief     set fd max stage
 * @param     [in] stgmax
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdStage1Set(UINT8 stgmax)
{
	XBYTE[REG_FD_StageMax] = stgmax;
}
#if FD_FUNC_NoUse
/**
 * @fn        void HAL_FdTblini(UINT8* tbldata)
 * @brief     initialize fd engine using data base
 * @param     [in] tbldata
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdTblini(
	UINT8* tbldata
)
{
	UINT16 i;
	XBYTE[REG_FD_DB0] = tbldata[0];	//Database in REG
	XBYTE[REG_FD_DB1] = tbldata[1];	//Database in REG
	XBYTE[REG_FD_DB2] = tbldata[2];	//Database in REG
	XBYTE[REG_FD_DB3] = tbldata[3]; 	//Database in REG
	
	XBYTE[ REG_Cdsp_macroSel ] = 0x06;
	XBYTE[ REG_Cdsp_switchclk ] = 0x06;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;

#if 1
	for ( i = 0; i < 0x400; i = i + 4 ) {
		XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
		XBYTE[0x2b00 + i/4] = tbldata[i+4];
		
		//halfdSysInfo("*tbldata[%d]=0x%bx \n", i, tbldata[i]);
		
		while ( XBYTE[0x2b00+i/4] != tbldata[i+4]) {
			XBYTE[0x2b00 + i/4] = tbldata[i+4];
			halfdSysInfo("FD Tbl Retry \n");
		}

		XBYTE[ REG_Cdsp_macropagsel ] = 0x01;
		XBYTE[0x2b00 + i/4] = tbldata[i+5];
		
		//halfdSysInfo("*tbldata[%d]=0x%bx \n", i+1, tbldata[i+1]);
		
		while ( XBYTE[0x2b00+i/4] != tbldata[i+5]) {
			XBYTE[0x2b00 + i/4] = tbldata[i+5];
			halfdSysInfo("FD Tbl Retry \n");
		}
	
		XBYTE[ REG_Cdsp_macropagsel ] = 0x02;
		XBYTE[0x2b00 + i/4] = tbldata[i+6];
		
		//halfdSysInfo("*tbldata[%d]=0x%bx \n", i+2, tbldata[i+2]);
		
		while ( XBYTE[0x2b00+i/4] != tbldata[i+6]) {
			XBYTE[0x2b00 + i/4] = tbldata[i+6];
			halfdSysInfo("FD Tbl Retry \n");
		}

		XBYTE[ REG_Cdsp_macropagsel ] = 0x03;
		XBYTE[0x2b00 + i/4] = tbldata[i+7];
		
		//halfdSysInfo("*tbldata[%d]=0x%bx \n", i+3, tbldata[i+3]);
		
		while ( XBYTE[0x2b00+i/4] != tbldata[i+7]) {
			XBYTE[0x2b00 + i/4] = tbldata[i+7];
			halfdSysInfo("FD Tbl Retry \n");
		}
		
		//halfdSysInfo("i = %d \n!!", i);
	}
#endif
	XBYTE[ REG_Cdsp_macroSel ] = 0x00;
	XBYTE[ REG_Cdsp_switchclk ] = 0x00;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
}

/**
 * @fn        void HAL_FdIIGen(UINT16 srchsize, UINT16 srcvsize, UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, UINT8 dsthsize, UINT8 dstvsize, UINT32 srcaddr, UINT32 dstaddr)
 * @brief     set II src/dst addr & RIO
 * @param     [in] srchsize
 * @param     [in] srcvsize
 * @param     [in] roihoff
 * @param     [in] roivoff
 * @param     [in] roihsize
 * @param     [in] roivsize
 * @param     [in] dsthsize
 * @param     [in] dstvsize
 * @param     [in] srcaddr(w)
 * @param     [in] dstaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdIIGen(
		UINT16 srchsize, UINT16 srcvsize, 
		UINT16 roihoff, UINT16 roivoff, 
		UINT16 roihsize, UINT16 roivsize, 
		UINT8 dsthsize, UINT8 dstvsize, 
		UINT32 srcaddr, UINT32 dstaddr)
{
	UINT32 hfactini, vfactini, hfact, vfact;
	
	XBYTE[REG_II_CmdSrc] = 0; 	//Cpu Trigger mode
	hfact = ((UINT32)dsthsize * 1024)/srchsize;
	vfact = ((UINT32)dstvsize * 1024)/srcvsize;
	hfactini = 1024 - ((hfact * (srchsize -1))%1024);
	vfactini = 1024 - ((vfact * (srcvsize -1))%1024);
	
	//halfdSysInfo ("hfact = %lx \n", hfact);
	//halfdSysInfo ("vfact = %lx \n", vfact);
	
	XBYTE[REG_II_SrcHSize0] = ((unsigned char *)&(srchsize))[1];
	XBYTE[REG_II_SrcHSize1] = ((unsigned char *)&(srchsize))[0];
	XBYTE[REG_II_SrcHOff2] = ((unsigned char *)&(roihoff))[1];
	XBYTE[REG_II_SrcHOff3] = ((unsigned char *)&(roihoff))[0];
	XBYTE[REG_II_SrcVOff0] = ((unsigned char *)&(roivoff))[1];
	XBYTE[REG_II_SrcVOff1] = ((unsigned char *)&(roivoff))[0];
	XBYTE[REG_II_ROIHSize0] = ((unsigned char *)&(roihsize))[1];
	XBYTE[REG_II_ROIHSize1] = ((unsigned char *)&(roihsize))[0];
	XBYTE[REG_II_ROIVSize0] = ((unsigned char *)&(roivsize))[1];
	XBYTE[REG_II_ROIVSize1] = ((unsigned char *)&(roivsize))[0];
	
	XBYTE[REG_II_DstHSize] = dsthsize;
	XBYTE[0x2E7F] = 0x00;
	
	XBYTE[REG_II_HFactorIni0] = ((unsigned char *)&(hfactini))[3];
	XBYTE[REG_II_HFactorIni1] = ((unsigned char *)&(hfactini))[2];
	XBYTE[REG_II_VFactorIni0] = ((unsigned char *)&(vfactini))[3];
	XBYTE[REG_II_VFactorIni1] = ((unsigned char *)&(vfactini))[2];
	
	XBYTE[REG_II_HScaleFactor0] = ((unsigned char *)&(hfact))[3];
	XBYTE[REG_II_HScaleFactor1] = ((unsigned char *)&(hfact))[2];
	XBYTE[REG_II_VScaleFactor0] = ((unsigned char *)&(vfact))[3];
	XBYTE[REG_II_VScaleFactor1] = ((unsigned char *)&(vfact))[2];
	
	XBYTE[REG_II_SrcAddr0] = ((unsigned char *)&(srcaddr))[3];
	XBYTE[REG_II_SrcAddr1] = ((unsigned char *)&(srcaddr))[2];
	XBYTE[REG_II_SrcAddr2] = ((unsigned char *)&(srcaddr))[1];
	XBYTE[REG_II_SrcAddr3] = ((unsigned char *)&(srcaddr))[0];
	
	XBYTE[REG_II_DstAddr0] = ((unsigned char *)&(dstaddr))[3];
	XBYTE[REG_II_DstAddr1] = ((unsigned char *)&(dstaddr))[2];
	XBYTE[REG_II_DstAddr2] = ((unsigned char *)&(dstaddr))[1];
	XBYTE[REG_II_DstAddr3] = ((unsigned char *)&(dstaddr))[0];
	
	
	XBYTE[REG_II_Func] = 0; 	//II Gene mode
	XBYTE[REG_II_Intr] = 0x03;	//Clear done interrupt
	XBYTE[REG_II_CmdTrigger] = 0x01; 	//Trigger
			 
}

/**
 * @fn        void HAL_FdSIIGen(UINT16 srchsize, UINT16 srcvsize, UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, UINT8 dsthsize, UINT8 dstvsize, UINT32 srcaddr, UINT32 dstaddr)
 * @brief     set SII src/dst addr & RIO
 * @param     [in] srchsize
 * @param     [in] srcvsize
 * @param     [in] roihoff
 * @param     [in] roivoff
 * @param     [in] roihsize
 * @param     [in] roivsize
 * @param     [in] dsthsize
 * @param     [in] dstvsize
 * @param     [in] srcaddr(w)
 * @param     [in] dstaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdSIIGen(
		UINT16 srchsize, UINT16 srcvsize, 
		 UINT16 roihoff, UINT16 roivoff, 
		 UINT16 roihsize, UINT16 roivsize, 
		 UINT8 dsthsize, UINT8 dstvsize, 
		 UINT32 srcaddr, UINT32 dstaddr
		 )
{
	UINT32 hfactini, vfactini, hfact, vfact;
	
	XBYTE[REG_II_CmdSrc] = 0; 	//Cpu Trigger mode
	hfact = ((UINT32)dsthsize * 1024)/srchsize;
	vfact = ((UINT32)dstvsize * 1024)/srcvsize;
	hfactini = 1024 - ((hfact * (srchsize -1))%1024);
	vfactini = 1024 - ((vfact * (srcvsize -1))%1024);
	
	//halfdSysInfo ("hfact = %lx \n", hfact);
	//halfdSysInfo ("vfact = %lx \n", vfact);
	
	XBYTE[REG_II_SrcHSize0] = ((unsigned char *)&(srchsize))[1];
	XBYTE[REG_II_SrcHSize1] = ((unsigned char *)&(srchsize))[0];
	XBYTE[REG_II_SrcHOff2] = ((unsigned char *)&(roihoff))[1];
	XBYTE[REG_II_SrcHOff3] = ((unsigned char *)&(roihoff))[0];
	XBYTE[REG_II_SrcVOff0] = ((unsigned char *)&(roivoff))[1];
	XBYTE[REG_II_SrcVOff1] = ((unsigned char *)&(roivoff))[0];
	XBYTE[REG_II_ROIHSize0] = ((unsigned char *)&(roihsize))[1];
	XBYTE[REG_II_ROIHSize1] = ((unsigned char *)&(roihsize))[0];
	XBYTE[REG_II_ROIVSize0] = ((unsigned char *)&(roivsize))[1];
	XBYTE[REG_II_ROIVSize1] = ((unsigned char *)&(roivsize))[0];
	
	XBYTE[REG_II_DstHSize] = dsthsize;
	XBYTE[0x2E7F] = 0x00;
	
	XBYTE[REG_II_HFactorIni0] = ((unsigned char *)&(hfactini))[3];
	XBYTE[REG_II_HFactorIni1] = ((unsigned char *)&(hfactini))[2];
	XBYTE[REG_II_VFactorIni0] = ((unsigned char *)&(vfactini))[3];
	XBYTE[REG_II_VFactorIni1] = ((unsigned char *)&(vfactini))[2];
	
	XBYTE[REG_II_HScaleFactor0] = ((unsigned char *)&(hfact))[3];
	XBYTE[REG_II_HScaleFactor1] = ((unsigned char *)&(hfact))[2];
	XBYTE[REG_II_VScaleFactor0] = ((unsigned char *)&(vfact))[3];
	XBYTE[REG_II_VScaleFactor1] = ((unsigned char *)&(vfact))[2];
	
	XBYTE[REG_II_SrcAddr0] = ((unsigned char *)&(srcaddr))[3];
	XBYTE[REG_II_SrcAddr1] = ((unsigned char *)&(srcaddr))[2];
	XBYTE[REG_II_SrcAddr2] = ((unsigned char *)&(srcaddr))[1];
	XBYTE[REG_II_SrcAddr3] = ((unsigned char *)&(srcaddr))[0];
	
	XBYTE[REG_II_DstAddr0] = ((unsigned char *)&(dstaddr))[3];
	XBYTE[REG_II_DstAddr1] = ((unsigned char *)&(dstaddr))[2];
	XBYTE[REG_II_DstAddr2] = ((unsigned char *)&(dstaddr))[1];
	XBYTE[REG_II_DstAddr3] = ((unsigned char *)&(dstaddr))[0];
	
	
	XBYTE[REG_II_Func] = 1; 	//SII Gene mode
	XBYTE[REG_II_Intr] = 0x03;	//Clear done interrupt
	XBYTE[REG_II_CmdTrigger] = 0x01; 	//Trigger		 
}
#if FD_FUNC_NoUse
/**
 * @fn        void HAL_FdIICpuWait(void)
 * @brief     Waiting II done
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdIICpuWait(void)
{
	while ((XBYTE[REG_II_Intr] & 0x02) == 0x00);
}
#endif
/**
 * @fn        void HAL_FdIIScale(UINT8 *srchsize, UINT8 *srcvsize, UINT8 *dsthsize, UINT8 *dstvsize, UINT32 srcaddr, UINT32 *dstaddr)
 * @brief     set II Scale size & addr(w) and trigger
 * @param     [in] srchsize
 * @param     [in] srcvsize
 * @param     [in] dsthsize
 * @param     [in] dstvsize
 * @param     [in] srcaddr(w)
 * @param     [in] dstaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdIIScale(
		UINT8 *srchsize, UINT8 *srcvsize, 
		UINT8 *dsthsize, UINT8 *dstvsize, 
		UINT32 srcaddr, 
		UINT32 *dstaddr)
{
	fd_iijobtable_t xdata* jtpt;
	UINT8 i;
	UINT32 hfactini, vfactini, hfact, vfact;
	UINT32 iidstaddr;
	UINT32 queaddr = (0x00080000UL + 0x8000UL) >> 1;
	
	jtpt = (fd_iijobtable_t xdata*) 0x4000;	
	
	for (i =0; i<12; i++) {
		
		//halfdSysInfo ("srcsize h = %bx, srcsize v = %bx \n", srchsize[i], srcvsize[i]);
		//halfdSysInfo ("dstsize h = %bx, dstsize v = %bx \n", dsthsize[i], dstvsize[i]);
		
		iidstaddr = dstaddr[i];
		
		//halfdSysInfo ("111 iidstaddr = %lx \n", iidstaddr);
		
		hfact = ((UINT32)dsthsize[i] * 1024)/(UINT32)srchsize[i];
		vfact = ((UINT32)dstvsize[i] * 1024)/(UINT32)srcvsize[i];
		hfactini = 1024 - ((hfact * ((UINT32)srchsize[i] -1))%1024);
		vfactini = 1024 - ((vfact * ((UINT32)srcvsize[i] -1))%1024);
			
		(jtpt+i)->func0		  = 0x02;
		(jtpt+i)->func1		  = 0x02;
		(jtpt+i)->roihoff0 	= 0x00;
		(jtpt+i)->roihoff1 	= 0x00;
		(jtpt+i)->roivoff0 	= 0x00;
		(jtpt+i)->roivoff1 	= 0x00;
		(jtpt+i)->roihsize0 = srchsize[i];
		(jtpt+i)->roihsize1 = 0x00;
		(jtpt+i)->roivsize0	= srcvsize[i];
		(jtpt+i)->roivsize1	= 0x00;
		(jtpt+i)->hfact0 	  = ((unsigned char *)&(hfact))[3]; 
		(jtpt+i)->hfact1 	  = ((unsigned char *)&(hfact))[2]; 
		(jtpt+i)->vfact0 	  = ((unsigned char *)&(vfact))[3]; 
		(jtpt+i)->vfact1 	  = ((unsigned char *)&(vfact))[2]; 
		(jtpt+i)->hfactini0 = ((unsigned char *)&(hfactini))[3]; 
		(jtpt+i)->hfactini1 = ((unsigned char *)&(hfactini))[2]; 
		(jtpt+i)->vfactini0 = ((unsigned char *)&(vfactini))[3]; 
		(jtpt+i)->vfactini1 = ((unsigned char *)&(vfactini))[2]; 	
		(jtpt+i)->srcaddr0  = ((unsigned char *)&(srcaddr))[3];
		(jtpt+i)->srcaddr1  = ((unsigned char *)&(srcaddr))[2];
		(jtpt+i)->srcaddr2  = ((unsigned char *)&(srcaddr))[1];
		(jtpt+i)->srcaddr3  = ((unsigned char *)&(srcaddr))[0];
		(jtpt+i)->srchsize0 = srchsize[i];
		(jtpt+i)->srchsize1 = 0x00;
		(jtpt+i)->dstaddr0  = ((unsigned char *)&(iidstaddr))[3];
		(jtpt+i)->dstaddr1  = ((unsigned char *)&(iidstaddr))[2];
		(jtpt+i)->dstaddr2  = ((unsigned char *)&(iidstaddr))[1];
		(jtpt+i)->dstaddr3  = ((unsigned char *)&(iidstaddr))[0];
		(jtpt+i)->dsthsize0 = dsthsize[i];
		(jtpt+i)->dsthsize1 = 0x00;		
		//srcaddr = dstaddr[i];
	}	
	(jtpt+11)->func0 		= 0x0203;
	XBYTE[REG_II_CmdQueueAddr0] = ((unsigned char *)&(queaddr))[3];
	XBYTE[REG_II_CmdQueueAddr1] = ((unsigned char *)&(queaddr))[2];
	XBYTE[REG_II_CmdQueueAddr2] = ((unsigned char *)&(queaddr))[1];
	XBYTE[REG_II_CmdQueueAddr3] = ((unsigned char *)&(queaddr))[0];


	XBYTE[REG_II_CmdSrc] = 1; 	  //Queue Trigger mode	
	XBYTE[REG_II_Intr] = 0x03;	//Clear done interrupt
	
	//halfdSysInfo ("queaddr = %lx \n", queaddr);
	
#if 1		
	XBYTE[REG_II_CmdTrigger] = 0x01; 	//Trigger
	
#endif
		
}

/**
 * @fn        void HAL_FdIISII(UINT16 yuvsrchsize, UINT16 yuvsrcvsize, UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, UINT8 iidsthsize, UINT8 iidstvsize, UINT32 yuvsrcaddr, UINT32 iisrcaddr, UINT32 siisrcaddr)
 * @brief     set and trigger II  & SII
 * @param     [in] yuvsrchsize
 * @param     [in] yuvsrcvsize
 * @param     [in] roihoff
 * @param     [in] roivoff
 * @param     [in] roihsize
 * @param     [in] roivsize
 * @param     [in] iidsthsize
 * @param     [in] iidstvsize
 * @param     [in] yuvsrcaddr(w)
 * @param     [in] iisrcaddr(w)
 * @param     [in] siisrcaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdIISII(UINT16 yuvsrchsize, UINT16 yuvsrcvsize, 
				UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
				UINT8  iidsthsize, UINT8 iidstvsize, 
				UINT32 yuvsrcaddr, UINT32 iisrcaddr, UINT32 siisrcaddr)
{
	fd_iijobtable_t xdata* jtpt;
//	UINT8 i;
	UINT32 hfactini, vfactini, hfact, vfact;
//	UINT32 iidstaddr;
	UINT32 queaddr = (0x00080000UL + 0x8000UL) >> 1;	

	jtpt = (fd_iijobtable_t xdata*) 0x4000;	
	/* II Generation Job ------------------------------------------------*/	
	hfact = ((UINT32)iidsthsize * 1024)/yuvsrchsize;
	vfact = ((UINT32)iidstvsize * 1024)/yuvsrcvsize;
	hfactini = 1024 - ((hfact * (yuvsrchsize -1))%1024);
	vfactini = 1024 - ((vfact * (yuvsrcvsize -1))%1024);
		
	jtpt->func0		  = 0x02;
	jtpt->func1		  = 0x00;
	jtpt->roihoff0  = ((unsigned char *)&(roihoff))[1];
	jtpt->roihoff1  = ((unsigned char *)&(roihoff))[0];
	jtpt->roivoff0  = ((unsigned char *)&(roivoff))[1];
	jtpt->roivoff1  = ((unsigned char *)&(roivoff))[0];
	jtpt->roihsize0 = ((unsigned char *)&(roihsize))[1];
	jtpt->roihsize1 = ((unsigned char *)&(roihsize))[0];
	jtpt->roivsize0 = ((unsigned char *)&(roivsize))[1];
	jtpt->roivsize1 = ((unsigned char *)&(roivsize))[0];
	jtpt->hfact0 	  = ((unsigned char *)&(hfact))[3];
	jtpt->hfact1 	  = ((unsigned char *)&(hfact))[2];
	jtpt->vfact0 	  = ((unsigned char *)&(vfact))[3];
	jtpt->vfact1 	  = ((unsigned char *)&(vfact))[2];
	jtpt->hfactini0 = ((unsigned char *)&(hfactini))[3];
	jtpt->hfactini1 = ((unsigned char *)&(hfactini))[2];
	jtpt->vfactini0 = ((unsigned char *)&(vfactini))[3];
	jtpt->vfactini1 = ((unsigned char *)&(vfactini))[2];
	jtpt->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	jtpt->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	jtpt->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	jtpt->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	jtpt->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	jtpt->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	jtpt->dstaddr0  = ((unsigned char *)&(iisrcaddr))[3];
	jtpt->dstaddr1  = ((unsigned char *)&(iisrcaddr))[2];
	jtpt->dstaddr2  = ((unsigned char *)&(iisrcaddr))[1];
	jtpt->dstaddr3  = ((unsigned char *)&(iisrcaddr))[0];
	jtpt->dsthsize0 = iidsthsize;
	jtpt->dsthsize1 = 0x00;
	/* SII Generation Job ------------------------------------------------*/	
	(jtpt+1)->func0		  = 0x03;
	(jtpt+1)->func1		  = 0x01;
	(jtpt+1)->roihoff0  = ((unsigned char *)&(roihoff))[1];
	(jtpt+1)->roihoff1  = ((unsigned char *)&(roihoff))[0];
	(jtpt+1)->roivoff0  = ((unsigned char *)&(roivoff))[1];
	(jtpt+1)->roivoff1  = ((unsigned char *)&(roivoff))[0];
	(jtpt+1)->roihsize0 = ((unsigned char *)&(roihsize))[1];
	(jtpt+1)->roihsize1 = ((unsigned char *)&(roihsize))[0];
	(jtpt+1)->roivsize0 = ((unsigned char *)&(roivsize))[1];
	(jtpt+1)->roivsize1 = ((unsigned char *)&(roivsize))[0];
	(jtpt+1)->hfact0 	  = ((unsigned char *)&(hfact))[3];
	(jtpt+1)->hfact1 	  = ((unsigned char *)&(hfact))[2];
	(jtpt+1)->vfact0 	  = ((unsigned char *)&(vfact))[3];
	(jtpt+1)->vfact1 	  = ((unsigned char *)&(vfact))[2];
	(jtpt+1)->hfactini0 = ((unsigned char *)&(hfactini))[3];
	(jtpt+1)->hfactini1 = ((unsigned char *)&(hfactini))[2];
	(jtpt+1)->vfactini0 = ((unsigned char *)&(vfactini))[3];
	(jtpt+1)->vfactini1 = ((unsigned char *)&(vfactini))[2];
	(jtpt+1)->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	(jtpt+1)->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	(jtpt+1)->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	(jtpt+1)->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	(jtpt+1)->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	(jtpt+1)->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	(jtpt+1)->dstaddr0  = ((unsigned char *)&(siisrcaddr))[3];
	(jtpt+1)->dstaddr1  = ((unsigned char *)&(siisrcaddr))[2];
	(jtpt+1)->dstaddr2  = ((unsigned char *)&(siisrcaddr))[1];
	(jtpt+1)->dstaddr3  = ((unsigned char *)&(siisrcaddr))[0];
	(jtpt+1)->dsthsize0 = iidsthsize;
	(jtpt+1)->dsthsize1 = 0x00;	
	
	XBYTE[REG_II_CmdQueueAddr0] = ((unsigned char *)&(queaddr))[3];
	XBYTE[REG_II_CmdQueueAddr1] = ((unsigned char *)&(queaddr))[2];
	XBYTE[REG_II_CmdQueueAddr2] = ((unsigned char *)&(queaddr))[1];
	XBYTE[REG_II_CmdQueueAddr3] = ((unsigned char *)&(queaddr))[0];
	
	XBYTE[REG_II_CmdSrc] = 1; 	  //Queue Trigger mode	
	XBYTE[REG_II_Intr] = 0x03;	//Clear done interrupt
	
	//halfdSysInfo ("queaddr = %lx \n", queaddr);
	
#if 1		
	XBYTE[REG_II_CmdTrigger] = 0x01; 	//Trigger
	
#endif
		
}
#if 0
void HAL_FdIISIIQueue(UINT16 yuvsrchsize, //UINT16 yuvsrcvsize, 
				UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
				UINT8  iidsthsize, UINT8 iidstvsize, 
				UINT32 yuvsrcaddr, UINT32 iisrcaddr, UINT32 siisrcaddr,
				UINT32 queaddr)
{
	fd_iijobtable_t xdata* jtpt;
//	UINT8 i;
	UINT32 hfactini, vfactini, hfact, vfact;
	UINT8 up;
	UINT8 buf[sizeof(fd_iijobtable_t)*2];
//	UINT32 iidstaddr;
	//UINT32 queaddr = (0x00080000UL + 0x8000UL) >> 1;	
	jtpt = (fd_iijobtable_t xdata*) buf;//0x4000;	
	/* II Generation Job ------------------------------------------------*/	
	//printf("(%u,%u)(%u,%u)(%bu,%bu)\n",yuvsrchsize,yuvsrcvsize,roihsize,roivsize,iidsthsize,iidstvsize);
	if(roihsize>=iidsthsize){
		hfact = ((UINT32)iidsthsize * 1024)/roihsize;
		vfact = ((UINT32)iidstvsize * 1024)/roivsize;
		hfactini = 1024 - ((hfact * (roihsize -1))%1024);
		vfactini = 1024 - ((vfact * (roivsize -1))%1024);
		up=0;
	}
	else{
		hfact = ((UINT32)roihsize * 1024)/iidsthsize;
		vfact = ((UINT32)roivsize * 1024)/iidstvsize;
		hfactini = 1024 - ((hfact * (iidsthsize -1))%1024);
		vfactini = 1024 - ((vfact * (iidstvsize -1))%1024);
		up=0x04;
	}
	jtpt->func0		  = 0x02;
	jtpt->func1		  = 0x00;
	jtpt->roihoff0  = ((unsigned char *)&(roihoff))[1];
	jtpt->roihoff1  = ((unsigned char *)&(roihoff))[0];
	jtpt->roivoff0  = ((unsigned char *)&(roivoff))[1];
	jtpt->roivoff1  = ((unsigned char *)&(roivoff))[0];
	jtpt->roihsize0 = ((unsigned char *)&(roihsize))[1];
	jtpt->roihsize1 = ((unsigned char *)&(roihsize))[0];
	jtpt->roivsize0 = ((unsigned char *)&(roivsize))[1];
	jtpt->roivsize1 = ((unsigned char *)&(roivsize))[0];
	jtpt->hfact0 	  = ((unsigned char *)&(hfact))[3];
	jtpt->hfact1 	  = ((unsigned char *)&(hfact))[2]|up;
	jtpt->vfact0 	  = ((unsigned char *)&(vfact))[3];
	jtpt->vfact1 	  = ((unsigned char *)&(vfact))[2]|up;
	jtpt->hfactini0 = ((unsigned char *)&(hfactini))[3];
	jtpt->hfactini1 = ((unsigned char *)&(hfactini))[2];
	jtpt->vfactini0 = ((unsigned char *)&(vfactini))[3];
	jtpt->vfactini1 = ((unsigned char *)&(vfactini))[2];
	jtpt->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	jtpt->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	jtpt->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	jtpt->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	jtpt->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	jtpt->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	jtpt->dstaddr0  = ((unsigned char *)&(iisrcaddr))[3];
	jtpt->dstaddr1  = ((unsigned char *)&(iisrcaddr))[2];
	jtpt->dstaddr2  = ((unsigned char *)&(iisrcaddr))[1];
	jtpt->dstaddr3  = ((unsigned char *)&(iisrcaddr))[0];
	jtpt->dsthsize0 = iidsthsize;
	jtpt->dsthsize1 = 0x00;
	/* SII Generation Job ------------------------------------------------*/	
	(jtpt+1)->func0		  = 0x02;
	(jtpt+1)->func1		  = 0x01;
	(jtpt+1)->roihoff0  = ((unsigned char *)&(roihoff))[1];
	(jtpt+1)->roihoff1  = ((unsigned char *)&(roihoff))[0];
	(jtpt+1)->roivoff0  = ((unsigned char *)&(roivoff))[1];
	(jtpt+1)->roivoff1  = ((unsigned char *)&(roivoff))[0];
	(jtpt+1)->roihsize0 = ((unsigned char *)&(roihsize))[1];
	(jtpt+1)->roihsize1 = ((unsigned char *)&(roihsize))[0];
	(jtpt+1)->roivsize0 = ((unsigned char *)&(roivsize))[1];
	(jtpt+1)->roivsize1 = ((unsigned char *)&(roivsize))[0];
	(jtpt+1)->hfact0 	  = ((unsigned char *)&(hfact))[3];
	(jtpt+1)->hfact1 	  = ((unsigned char *)&(hfact))[2];
	(jtpt+1)->vfact0 	  = ((unsigned char *)&(vfact))[3];
	(jtpt+1)->vfact1 	  = ((unsigned char *)&(vfact))[2];
	(jtpt+1)->hfactini0 = ((unsigned char *)&(hfactini))[3];
	(jtpt+1)->hfactini1 = ((unsigned char *)&(hfactini))[2];
	(jtpt+1)->vfactini0 = ((unsigned char *)&(vfactini))[3];
	(jtpt+1)->vfactini1 = ((unsigned char *)&(vfactini))[2];
	(jtpt+1)->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	(jtpt+1)->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	(jtpt+1)->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	(jtpt+1)->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	(jtpt+1)->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	(jtpt+1)->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	(jtpt+1)->dstaddr0  = ((unsigned char *)&(siisrcaddr))[3];
	(jtpt+1)->dstaddr1  = ((unsigned char *)&(siisrcaddr))[2];
	(jtpt+1)->dstaddr2  = ((unsigned char *)&(siisrcaddr))[1];
	(jtpt+1)->dstaddr3  = ((unsigned char *)&(siisrcaddr))[0];
	(jtpt+1)->dsthsize0 = iidsthsize;
	(jtpt+1)->dsthsize1 = 0x00;	
	
	//HAL_DramDmaWrite(buf,queaddr,sizeof(fd_iijobtable_t)*2,0);
	HAL_DramWriteBytesAddr(queaddr,buf,sizeof(fd_iijobtable_t)*2,0);
		
}
#endif
#endif

#if 0
void HAL_FdJobQueue2( 
			UINT16 yuvsrchsize, UINT16 yuvsrcvsize, 
			//UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
			UINT8  iidsthsize, UINT8 iidstvsize, 
			UINT32 yuvsrcaddr, //(w)
			UINT32 iisrcaddr,  //(w)
			UINT32 siisrcaddr, //(w)
			UINT8 iisrchsize, UINT8 iisrcvsize, 
			UINT8 *dsthsize, UINT8 *dstvsize, 
			UINT32 *dstaddr, //(w)
			UINT32 queaddr //(w)
			)
{
	//UINT8 jtbuf[sizeof(fd_iijobtable_t)*14];
	UINT8 *jtbuf=G_ucStorData;//[sizeof(fd_iijobtable_t)*14];
	fd_iijobtable_t xdata* jtpt;
	UINT8 i;
	UINT32 hfactini, vfactini, hfact, vfact;
	UINT32 iidstaddr;
	//UINT32 queaddr = (0x00080000UL + 0x8000UL) >> 1;	
	jtpt = (fd_iijobtable_t*)jtbuf;//(fd_iijobtable_t xdata*) 0x4000;	
	/* II Generation Job ------------------------------------------------*/	
	hfact = ((UINT32)iidsthsize * 1024)/yuvsrchsize;
	vfact = ((UINT32)iidstvsize * 1024)/yuvsrcvsize;
	hfactini = 1024 - ((hfact * (yuvsrchsize -1))%1024);
	vfactini = 1024 - ((vfact * (yuvsrcvsize -1))%1024);
	#if 0
	jtpt->func0		  = 0x02;
	jtpt->func1		  = 0x00;
	jtpt->roihoff0  = ((unsigned char *)&(roihoff))[1];
	jtpt->roihoff1  = ((unsigned char *)&(roihoff))[0];
	jtpt->roivoff0  = ((unsigned char *)&(roivoff))[1];
	jtpt->roivoff1  = ((unsigned char *)&(roivoff))[0];
	jtpt->roihsize0 = ((unsigned char *)&(roihsize))[1];
	jtpt->roihsize1 = ((unsigned char *)&(roihsize))[0];
	jtpt->roivsize0 = ((unsigned char *)&(roivsize))[1];
	jtpt->roivsize1 = ((unsigned char *)&(roivsize))[0];
	jtpt->hfact0 	  = ((unsigned char *)&(hfact))[3];
	jtpt->hfact1 	  = ((unsigned char *)&(hfact))[2];
	jtpt->vfact0 	  = ((unsigned char *)&(vfact))[3];
	jtpt->vfact1 	  = ((unsigned char *)&(vfact))[2];
	jtpt->hfactini0 = ((unsigned char *)&(hfactini))[3];
	jtpt->hfactini1 = ((unsigned char *)&(hfactini))[2];
	jtpt->vfactini0 = ((unsigned char *)&(vfactini))[3];
	jtpt->vfactini1 = ((unsigned char *)&(vfactini))[2];
	jtpt->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	jtpt->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	jtpt->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	jtpt->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	jtpt->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	jtpt->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	jtpt->dstaddr0  = ((unsigned char *)&(iisrcaddr))[3];
	jtpt->dstaddr1  = ((unsigned char *)&(iisrcaddr))[2];
	jtpt->dstaddr2  = ((unsigned char *)&(iisrcaddr))[1];
	jtpt->dstaddr3  = ((unsigned char *)&(iisrcaddr))[0];
	jtpt->dsthsize0 = iidsthsize;
	jtpt->dsthsize1 = 0x00;
	/* SII Generation Job ------------------------------------------------*/	
	(jtpt+1)->func0		  = 0x02;
	(jtpt+1)->func1		  = 0x01;
	(jtpt+1)->roihoff0  = ((unsigned char *)&(roihoff))[1];
	(jtpt+1)->roihoff1  = ((unsigned char *)&(roihoff))[0];
	(jtpt+1)->roivoff0  = ((unsigned char *)&(roivoff))[1];
	(jtpt+1)->roivoff1  = ((unsigned char *)&(roivoff))[0];
	(jtpt+1)->roihsize0 = ((unsigned char *)&(roihsize))[1];
	(jtpt+1)->roihsize1 = ((unsigned char *)&(roihsize))[0];
	(jtpt+1)->roivsize0 = ((unsigned char *)&(roivsize))[1];
	(jtpt+1)->roivsize1 = ((unsigned char *)&(roivsize))[0];
	(jtpt+1)->hfact0 	  = ((unsigned char *)&(hfact))[3];
	(jtpt+1)->hfact1 	  = ((unsigned char *)&(hfact))[2];
	(jtpt+1)->vfact0 	  = ((unsigned char *)&(vfact))[3];
	(jtpt+1)->vfact1 	  = ((unsigned char *)&(vfact))[2];
	(jtpt+1)->hfactini0 = ((unsigned char *)&(hfactini))[3];
	(jtpt+1)->hfactini1 = ((unsigned char *)&(hfactini))[2];
	(jtpt+1)->vfactini0 = ((unsigned char *)&(vfactini))[3];
	(jtpt+1)->vfactini1 = ((unsigned char *)&(vfactini))[2];
	(jtpt+1)->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	(jtpt+1)->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	(jtpt+1)->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	(jtpt+1)->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	(jtpt+1)->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	(jtpt+1)->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	(jtpt+1)->dstaddr0  = ((unsigned char *)&(siisrcaddr))[3];
	(jtpt+1)->dstaddr1  = ((unsigned char *)&(siisrcaddr))[2];
	(jtpt+1)->dstaddr2  = ((unsigned char *)&(siisrcaddr))[1];
	(jtpt+1)->dstaddr3  = ((unsigned char *)&(siisrcaddr))[0];
	(jtpt+1)->dsthsize0 = iidsthsize;
	(jtpt+1)->dsthsize1 = 0x00;	
	#endif
	for (i =0; i<12; i++) {
		//printf ("dstsize h = %bx, dstsize v = %bx \n", dsthsize[i], dstvsize[i]);
		iidstaddr = dstaddr[i];
		//printf ("111 iidstaddr = %lx \n", iidstaddr);
		hfact = ((UINT32)dsthsize[i] * 1024)/(UINT32)iisrchsize;
		vfact = ((UINT32)dstvsize[i] * 1024)/(UINT32)iisrcvsize;
		hfactini = 1024 - ((hfact * ((UINT32)iisrchsize -1))%1024);
		vfactini = 1024 - ((vfact * ((UINT32)iisrcvsize-1))%1024);
			
		(jtpt+i+2)->func0		  = 0x02;
		(jtpt+i+2)->func1		  = 0x02;
		(jtpt+i+2)->roihoff0 	= 0x00;
		(jtpt+i+2)->roihoff1 	= 0x00;
		(jtpt+i+2)->roivoff0 	= 0x00;
		(jtpt+i+2)->roivoff1 	= 0x00;
		(jtpt+i+2)->roihsize0 = iisrchsize;
		(jtpt+i+2)->roihsize1 = 0x00;
		(jtpt+i+2)->roivsize0	= iisrcvsize;
		(jtpt+i+2)->roivsize1	= 0x00;
		(jtpt+i+2)->hfact0 	  = ((unsigned char *)&(hfact))[3]; 
		(jtpt+i+2)->hfact1 	  = ((unsigned char *)&(hfact))[2]; 
		(jtpt+i+2)->vfact0 	  = ((unsigned char *)&(vfact))[3]; 
		(jtpt+i+2)->vfact1 	  = ((unsigned char *)&(vfact))[2]; 
		(jtpt+i+2)->hfactini0 = ((unsigned char *)&(hfactini))[3]; 
		(jtpt+i+2)->hfactini1 = ((unsigned char *)&(hfactini))[2]; 
		(jtpt+i+2)->vfactini0 = ((unsigned char *)&(vfactini))[3]; 
		(jtpt+i+2)->vfactini1 = ((unsigned char *)&(vfactini))[2]; 	
		(jtpt+i+2)->srcaddr0  = ((unsigned char *)&(iisrcaddr))[3];
		(jtpt+i+2)->srcaddr1  = ((unsigned char *)&(iisrcaddr))[2];
		(jtpt+i+2)->srcaddr2  = ((unsigned char *)&(iisrcaddr))[1];
		(jtpt+i+2)->srcaddr3  = ((unsigned char *)&(iisrcaddr))[0];
		(jtpt+i+2)->srchsize0 = iisrchsize;
		(jtpt+i+2)->srchsize1 = 0x00;
		(jtpt+i+2)->dstaddr0  = ((unsigned char *)&(iidstaddr))[3];
		(jtpt+i+2)->dstaddr1  = ((unsigned char *)&(iidstaddr))[2];
		(jtpt+i+2)->dstaddr2  = ((unsigned char *)&(iidstaddr))[1];
		(jtpt+i+2)->dstaddr3  = ((unsigned char *)&(iidstaddr))[0];
		(jtpt+i+2)->dsthsize0 = dsthsize[i];
		(jtpt+i+2)->dsthsize1 = 0x00;		
	}	
	(jtpt+13)->func0 		= 0x0203;

	memcpy((jtpt+14),jtpt,sizeof(fd_iijobtable_t)*14);

	HAL_DmaSetDramAddr(queaddr);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaDo(1, 0, sizeof(fd_iijobtable_t)*14*2, 0, 0);

}
#endif
void HAL_FdIISIIQueue(				
			UINT16 yuvsrchsize, //UINT16 yuvsrcvsize, 
			UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
			UINT8  iidsthsize, UINT8 iidstvsize, 
			UINT32 yuvsrcaddr, UINT32 iisrcaddr, UINT32 siisrcaddr,
			UINT8* buf,
			UINT32 queaddr)
{
	UINT32 hfactini, vfactini, hfact, vfact;
	UINT8 up;
	fd_iijobtable_t* jtpt = (fd_iijobtable_t*)buf;
	
	/* II Generation Job ------------------------------------------------*/	
	//halfdSysInfo("(%u,%u)(%u,%u)(%bu,%bu)\n",yuvsrchsize,yuvsrcvsize,roihsize,roivsize,iidsthsize,iidstvsize);
	if (roihsize>=iidsthsize) {
		hfact = ((UINT32)iidsthsize * 1024)/roihsize;
		vfact = ((UINT32)iidstvsize * 1024)/roivsize;
		hfactini = 1024 - ((hfact * (roihsize -1))%1024);
		vfactini = 1024 - ((vfact * (roivsize -1))%1024);
		up=0;
	}
	else {
		hfact = ((UINT32)roihsize * 1024)/iidsthsize;
		vfact = ((UINT32)roivsize * 1024)/iidstvsize;
		hfactini = 1024 - ((hfact * (iidsthsize -1))%1024);
		vfactini = 1024 - ((vfact * (iidstvsize -1))%1024);
		up=0x04;
	}
	
	jtpt->func0		  = 0x02;
	jtpt->func1		  = 0x00;
	jtpt->roihoff0  = ((unsigned char *)&(roihoff))[1];
	jtpt->roihoff1  = ((unsigned char *)&(roihoff))[0];
	jtpt->roivoff0  = ((unsigned char *)&(roivoff))[1];
	jtpt->roivoff1  = ((unsigned char *)&(roivoff))[0];
	jtpt->roihsize0 = ((unsigned char *)&(roihsize))[1];
	jtpt->roihsize1 = ((unsigned char *)&(roihsize))[0];
	jtpt->roivsize0 = ((unsigned char *)&(roivsize))[1];
	jtpt->roivsize1 = ((unsigned char *)&(roivsize))[0];
	jtpt->hfact0 	  = ((unsigned char *)&(hfact))[3];
	jtpt->hfact1 	  = ((unsigned char *)&(hfact))[2]|up;
	jtpt->vfact0 	  = ((unsigned char *)&(vfact))[3];
	jtpt->vfact1 	  = ((unsigned char *)&(vfact))[2]|up;
	jtpt->hfactini0 = ((unsigned char *)&(hfactini))[3];
	jtpt->hfactini1 = ((unsigned char *)&(hfactini))[2];
	jtpt->vfactini0 = ((unsigned char *)&(vfactini))[3];
	jtpt->vfactini1 = ((unsigned char *)&(vfactini))[2];
	jtpt->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	jtpt->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	jtpt->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	jtpt->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	jtpt->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	jtpt->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	jtpt->dstaddr0  = ((unsigned char *)&(iisrcaddr))[3];
	jtpt->dstaddr1  = ((unsigned char *)&(iisrcaddr))[2];
	jtpt->dstaddr2  = ((unsigned char *)&(iisrcaddr))[1];
	jtpt->dstaddr3  = ((unsigned char *)&(iisrcaddr))[0];
	jtpt->dsthsize0 = iidsthsize;
	jtpt->dsthsize1 = 0x00;
	jtpt++;
	/* SII Generation Job ------------------------------------------------*/	
	jtpt->func0		  = 0x02;
	jtpt->func1		  = 0x01;
	jtpt->roihoff0  = ((unsigned char *)&(roihoff))[1];
	jtpt->roihoff1  = ((unsigned char *)&(roihoff))[0];
	jtpt->roivoff0  = ((unsigned char *)&(roivoff))[1];
	jtpt->roivoff1  = ((unsigned char *)&(roivoff))[0];
	jtpt->roihsize0 = ((unsigned char *)&(roihsize))[1];
	jtpt->roihsize1 = ((unsigned char *)&(roihsize))[0];
	jtpt->roivsize0 = ((unsigned char *)&(roivsize))[1];
	jtpt->roivsize1 = ((unsigned char *)&(roivsize))[0];
	jtpt->hfact0 	  = ((unsigned char *)&(hfact))[3];
	jtpt->hfact1 	  = ((unsigned char *)&(hfact))[2];
	jtpt->vfact0 	  = ((unsigned char *)&(vfact))[3];
	jtpt->vfact1 	  = ((unsigned char *)&(vfact))[2];
	jtpt->hfactini0 = ((unsigned char *)&(hfactini))[3];
	jtpt->hfactini1 = ((unsigned char *)&(hfactini))[2];
	jtpt->vfactini0 = ((unsigned char *)&(vfactini))[3];
	jtpt->vfactini1 = ((unsigned char *)&(vfactini))[2];
	jtpt->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	jtpt->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	jtpt->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	jtpt->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	jtpt->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	jtpt->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	jtpt->dstaddr0  = ((unsigned char *)&(siisrcaddr))[3];
	jtpt->dstaddr1  = ((unsigned char *)&(siisrcaddr))[2];
	jtpt->dstaddr2  = ((unsigned char *)&(siisrcaddr))[1];
	jtpt->dstaddr3  = ((unsigned char *)&(siisrcaddr))[0];
	jtpt->dsthsize0 = iidsthsize;
	jtpt->dsthsize1 = 0x00;	
	
	//HAL_DramDmaWrite(buf,queaddr,sizeof(fd_iijobtable_t)*2,0);
	if (queaddr) {
		HAL_DramWriteBytesAddr(queaddr,buf,sizeof(fd_iijobtable_t)*2,0);
	}

	/*
	HAL_DramSetAddr(queaddr, 0);
	HAL_DramWriteByte(0x02, 0x00);
	HAL_DramWrite16(roihoff);
	HAL_DramWrite16(roivoff);
	HAL_DramWrite16(roihsize);
	HAL_DramWrite16(roivsize);
	HAL_DramWrite16(hfact);
	HAL_DramWrite16(vfact);
	HAL_DramWrite16(hfactini);
	HAL_DramWrite16(vfactini);
	HAL_DramWrite32(yuvsrcaddr);
	HAL_DramWrite16(yuvsrchsize);
	HAL_DramWrite32(iisrcaddr);
	HAL_DramWriteByte(iidsthsize,0);
	HAL_DramWriteByte(0,0);
	HAL_DramWriteByte(0x02, 0x01);
	HAL_DramWrite16(roihoff);
	HAL_DramWrite16(roivoff);
	HAL_DramWrite16(roihsize);
	HAL_DramWrite16(roivsize);
	HAL_DramWrite16(hfact);
	HAL_DramWrite16(vfact);
	HAL_DramWrite16(hfactini);
	HAL_DramWrite16(vfactini);
	HAL_DramWrite32(yuvsrcaddr);
	HAL_DramWrite16(yuvsrchsize);
	HAL_DramWrite32(siisrcaddr);
	HAL_DramWriteByte(iidsthsize,0);
	HAL_DramWriteByte(0,0);
	*/
}
void HAL_FdIIQueue(			
			UINT32 iisrcaddr,  //(w)
			UINT8 iisrchsize, UINT8 iisrcvsize, 
			UINT8 *dsthsize, UINT8 *dstvsize, 
			UINT32 *dstaddr, //(w)
			UINT8* buf,
			UINT32 queaddr, //(w)
			UINT8 count
			)
{	
	UINT8 i;
	UINT32 hfactini, vfactini, hfact, vfact;
	UINT32 iidstaddr;
	fd_iijobtable_t* jtpt = (fd_iijobtable_t*)buf;

	for (i =0; i<count; i++) {
		iidstaddr = dstaddr[i];
		hfact = ((UINT32)dsthsize[i] * 1024)/(UINT32)iisrchsize;
		vfact = ((UINT32)dstvsize[i] * 1024)/(UINT32)iisrcvsize;
		hfactini = 1024 - ((hfact * ((UINT32)iisrchsize -1))%1024);
		vfactini = 1024 - ((vfact * ((UINT32)iisrcvsize-1))%1024);
		
		jtpt->func0		  = 0x02;
		jtpt->func1		  = 0x02;
		jtpt->roihoff0 	= 0x00;
		jtpt->roihoff1 	= 0x00;
		jtpt->roivoff0 	= 0x00;
		jtpt->roivoff1 	= 0x00;
		jtpt->roihsize0 = iisrchsize;
		jtpt->roihsize1 = 0x00;
		jtpt->roivsize0	= iisrcvsize;
		jtpt->roivsize1	= 0x00;
		jtpt->hfact0 	  = ((unsigned char *)&(hfact))[3]; 
		jtpt->hfact1 	  = ((unsigned char *)&(hfact))[2]; 
		jtpt->vfact0 	  = ((unsigned char *)&(vfact))[3]; 
		jtpt->vfact1 	  = ((unsigned char *)&(vfact))[2]; 
		jtpt->hfactini0 = ((unsigned char *)&(hfactini))[3]; 
		jtpt->hfactini1 = ((unsigned char *)&(hfactini))[2]; 
		jtpt->vfactini0 = ((unsigned char *)&(vfactini))[3]; 
		jtpt->vfactini1 = ((unsigned char *)&(vfactini))[2]; 	
		jtpt->srcaddr0  = ((unsigned char *)&(iisrcaddr))[3];
		jtpt->srcaddr1  = ((unsigned char *)&(iisrcaddr))[2];
		jtpt->srcaddr2  = ((unsigned char *)&(iisrcaddr))[1];
		jtpt->srcaddr3  = ((unsigned char *)&(iisrcaddr))[0];
		jtpt->srchsize0 = iisrchsize;
		jtpt->srchsize1 = 0x00;
		jtpt->dstaddr0  = ((unsigned char *)&(iidstaddr))[3];
		jtpt->dstaddr1  = ((unsigned char *)&(iidstaddr))[2];
		jtpt->dstaddr2  = ((unsigned char *)&(iidstaddr))[1];
		jtpt->dstaddr3  = ((unsigned char *)&(iidstaddr))[0];
		jtpt->dsthsize0 = dsthsize[i];
		jtpt->dsthsize1 = 0x00;		

		jtpt++;
	}	
	(jtpt-1)->func0 		= 0x0203;

	if (queaddr) {
		HAL_DramDmaWrite(jtpt, queaddr, sizeof(fd_iijobtable_t)*(count), 0);
	}	

}

/**
 * @fn        void HAL_FdIIJobQueue(UINT16 yuvsrchsize, UINT16 yuvsrcvsize, UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, UINT8 iidsthsize, UINT8 iidstvsize, UINT32 yuvsrcaddr, UINT32 iisrcaddr, UINT32 siisrcaddr, UINT8 iisrchsize, UINT8 iisrcvsize, UINT8 *dsthsize, UINT8 *dstvsize, UINT32 *dstaddr, UINT32 queaddr)
 * @brief     create II&SII job queue
 * @param     [in] yuvsrchsize
 * @param     [in] yuvsrcvsize
 * @param     [in] roihoff
 * @param     [in] roivoff
 * @param     [in] roihsize
 * @param     [in] roivsize
 * @param     [in] iidsthsize
 * @param     [in] iidstvsize
 * @param     [in] yuvsrcaddr(w)
 * @param     [in] iisrcaddr(w)
 * @param     [in] siisrcaddr(w)
 * @param     [in] iisrchsize
 * @param     [in] iisrcvsize
 * @param     [in] dsthsize
 * @param     [in] dstvsize
 * @param     [in] dstaddr(w)
 * @param     [in] queaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
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
			)
{
	//UINT8 i;
	//UINT32 hfactini, vfactini, hfact, vfact;
	//UINT32 iidstaddr;

	/* II Generation Job ------------------------------------------------*/	
	#if 0
	hfact = ((UINT32)iidsthsize * 1024)/yuvsrchsize;
	vfact = ((UINT32)iidstvsize * 1024)/yuvsrcvsize;
	hfactini = 1024 - ((hfact * (yuvsrchsize -1))%1024);
	vfactini = 1024 - ((vfact * (yuvsrcvsize -1))%1024);
		
	jtpt->func0		  = 0x02;
	jtpt->func1		  = 0x00;
	jtpt->roihoff0  = ((unsigned char *)&(roihoff))[1];
	jtpt->roihoff1  = ((unsigned char *)&(roihoff))[0];
	jtpt->roivoff0  = ((unsigned char *)&(roivoff))[1];
	jtpt->roivoff1  = ((unsigned char *)&(roivoff))[0];
	jtpt->roihsize0 = ((unsigned char *)&(roihsize))[1];
	jtpt->roihsize1 = ((unsigned char *)&(roihsize))[0];
	jtpt->roivsize0 = ((unsigned char *)&(roivsize))[1];
	jtpt->roivsize1 = ((unsigned char *)&(roivsize))[0];
	jtpt->hfact0 	  = ((unsigned char *)&(hfact))[3];
	jtpt->hfact1 	  = ((unsigned char *)&(hfact))[2];
	jtpt->vfact0 	  = ((unsigned char *)&(vfact))[3];
	jtpt->vfact1 	  = ((unsigned char *)&(vfact))[2];
	jtpt->hfactini0 = ((unsigned char *)&(hfactini))[3];
	jtpt->hfactini1 = ((unsigned char *)&(hfactini))[2];
	jtpt->vfactini0 = ((unsigned char *)&(vfactini))[3];
	jtpt->vfactini1 = ((unsigned char *)&(vfactini))[2];
	jtpt->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	jtpt->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	jtpt->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	jtpt->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	jtpt->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	jtpt->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	jtpt->dstaddr0  = ((unsigned char *)&(iisrcaddr))[3];
	jtpt->dstaddr1  = ((unsigned char *)&(iisrcaddr))[2];
	jtpt->dstaddr2  = ((unsigned char *)&(iisrcaddr))[1];
	jtpt->dstaddr3  = ((unsigned char *)&(iisrcaddr))[0];
	jtpt->dsthsize0 = iidsthsize;
	jtpt->dsthsize1 = 0x00;
	/* SII Generation Job ------------------------------------------------*/	
	(jtpt+1)->func0		  = 0x02;
	(jtpt+1)->func1		  = 0x01;
	(jtpt+1)->roihoff0  = ((unsigned char *)&(roihoff))[1];
	(jtpt+1)->roihoff1  = ((unsigned char *)&(roihoff))[0];
	(jtpt+1)->roivoff0  = ((unsigned char *)&(roivoff))[1];
	(jtpt+1)->roivoff1  = ((unsigned char *)&(roivoff))[0];
	(jtpt+1)->roihsize0 = ((unsigned char *)&(roihsize))[1];
	(jtpt+1)->roihsize1 = ((unsigned char *)&(roihsize))[0];
	(jtpt+1)->roivsize0 = ((unsigned char *)&(roivsize))[1];
	(jtpt+1)->roivsize1 = ((unsigned char *)&(roivsize))[0];
	(jtpt+1)->hfact0 	  = ((unsigned char *)&(hfact))[3];
	(jtpt+1)->hfact1 	  = ((unsigned char *)&(hfact))[2];
	(jtpt+1)->vfact0 	  = ((unsigned char *)&(vfact))[3];
	(jtpt+1)->vfact1 	  = ((unsigned char *)&(vfact))[2];
	(jtpt+1)->hfactini0 = ((unsigned char *)&(hfactini))[3];
	(jtpt+1)->hfactini1 = ((unsigned char *)&(hfactini))[2];
	(jtpt+1)->vfactini0 = ((unsigned char *)&(vfactini))[3];
	(jtpt+1)->vfactini1 = ((unsigned char *)&(vfactini))[2];
	(jtpt+1)->srcaddr0  = ((unsigned char *)&(yuvsrcaddr))[3];
	(jtpt+1)->srcaddr1  = ((unsigned char *)&(yuvsrcaddr))[2];
	(jtpt+1)->srcaddr2  = ((unsigned char *)&(yuvsrcaddr))[1];
	(jtpt+1)->srcaddr3  = ((unsigned char *)&(yuvsrcaddr))[0];
	(jtpt+1)->srchsize0 = ((unsigned char *)&(yuvsrchsize))[1];
	(jtpt+1)->srchsize1 = ((unsigned char *)&(yuvsrchsize))[0];
	(jtpt+1)->dstaddr0  = ((unsigned char *)&(siisrcaddr))[3];
	(jtpt+1)->dstaddr1  = ((unsigned char *)&(siisrcaddr))[2];
	(jtpt+1)->dstaddr2  = ((unsigned char *)&(siisrcaddr))[1];
	(jtpt+1)->dstaddr3  = ((unsigned char *)&(siisrcaddr))[0];
	(jtpt+1)->dsthsize0 = iidsthsize;
	(jtpt+1)->dsthsize1 = 0x00;	

	for (i =0; i<12; i++) {
		//printf ("dstsize h = %bx, dstsize v = %bx \n", dsthsize[i], dstvsize[i]);
		iidstaddr = dstaddr[i];
		//printf ("111 iidstaddr = %lx \n", iidstaddr);
		hfact = ((UINT32)dsthsize[i] * 1024)/(UINT32)iisrchsize;
		vfact = ((UINT32)dstvsize[i] * 1024)/(UINT32)iisrcvsize;
		hfactini = 1024 - ((hfact * ((UINT32)iisrchsize -1))%1024);
		vfactini = 1024 - ((vfact * ((UINT32)iisrcvsize-1))%1024);
			
		(jtpt+i+2)->func0		  = 0x02;
		(jtpt+i+2)->func1		  = 0x02;
		(jtpt+i+2)->roihoff0 	= 0x00;
		(jtpt+i+2)->roihoff1 	= 0x00;
		(jtpt+i+2)->roivoff0 	= 0x00;
		(jtpt+i+2)->roivoff1 	= 0x00;
		(jtpt+i+2)->roihsize0 = iisrchsize;
		(jtpt+i+2)->roihsize1 = 0x00;
		(jtpt+i+2)->roivsize0	= iisrcvsize;
		(jtpt+i+2)->roivsize1	= 0x00;
		(jtpt+i+2)->hfact0 	  = ((unsigned char *)&(hfact))[3]; 
		(jtpt+i+2)->hfact1 	  = ((unsigned char *)&(hfact))[2]; 
		(jtpt+i+2)->vfact0 	  = ((unsigned char *)&(vfact))[3]; 
		(jtpt+i+2)->vfact1 	  = ((unsigned char *)&(vfact))[2]; 
		(jtpt+i+2)->hfactini0 = ((unsigned char *)&(hfactini))[3]; 
		(jtpt+i+2)->hfactini1 = ((unsigned char *)&(hfactini))[2]; 
		(jtpt+i+2)->vfactini0 = ((unsigned char *)&(vfactini))[3]; 
		(jtpt+i+2)->vfactini1 = ((unsigned char *)&(vfactini))[2]; 	
		(jtpt+i+2)->srcaddr0  = ((unsigned char *)&(iisrcaddr))[3];
		(jtpt+i+2)->srcaddr1  = ((unsigned char *)&(iisrcaddr))[2];
		(jtpt+i+2)->srcaddr2  = ((unsigned char *)&(iisrcaddr))[1];
		(jtpt+i+2)->srcaddr3  = ((unsigned char *)&(iisrcaddr))[0];
		(jtpt+i+2)->srchsize0 = iisrchsize;
		(jtpt+i+2)->srchsize1 = 0x00;
		(jtpt+i+2)->dstaddr0  = ((unsigned char *)&(iidstaddr))[3];
		(jtpt+i+2)->dstaddr1  = ((unsigned char *)&(iidstaddr))[2];
		(jtpt+i+2)->dstaddr2  = ((unsigned char *)&(iidstaddr))[1];
		(jtpt+i+2)->dstaddr3  = ((unsigned char *)&(iidstaddr))[0];
		(jtpt+i+2)->dsthsize0 = dsthsize[i];
		(jtpt+i+2)->dsthsize1 = 0x00;		
	}	
	(jtpt+13)->func0 		= 0x0203;
	#endif
	//HAL_DmaSetDramAddr(queaddr);
	//HAL_DmaSetSramAddr(0x0000);
	//HAL_DmaDo(1, 0, sizeof(fd_iijobtable_t)*14, 0, 0);
	HAL_FdIISIIQueue(				
			yuvsrchsize, //UINT16 yuvsrcvsize, 
			roihoff,  roivoff,  roihsize,  roivsize, 
			iidsthsize,  iidstvsize, 
			yuvsrcaddr,  iisrcaddr,  siisrcaddr,
			buf,
			0
	);
	
	HAL_FdIIQueue(			
			iisrcaddr,  //(w)
			iisrchsize,  iisrcvsize, 
			dsthsize,  dstvsize, 
			dstaddr, //(w)
			buf+sizeof(fd_iijobtable_t)*2,
			0, //(w)
			count
	);
	
	if (queaddr) {
		HAL_DramDmaWrite(buf, queaddr, sizeof(fd_iijobtable_t)*(2+count), 0);
	}
}
/**
 * @fn        void HAL_FdIITrig(UINT32 queaddr)
 * @brief     set II & SII job queue addr(w) and trigger
 * @param     [in] queaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_FdIITrig(UINT32 queaddr,UINT32 waitCnt)
{	
	UINT32 cnt = waitCnt+1;
	
	while ((!(XBYTE[REG_II_Status] & 0x01)) && cnt) {
		if (!(--cnt)) {
			//halfdSysInfo("ii sts:%bx,cnt:%lx\n",XBYTE[REG_II_Status],waitCnt);
			return FALSE;
		}
	}
	
	//halfdSysInfo("ii:%bx\n",XBYTE[REG_II_Status]);
	
	if (queaddr) {
		XBYTE[REG_II_CmdQueueAddr0] = ((unsigned char *)&(queaddr))[3];
		XBYTE[REG_II_CmdQueueAddr1] = ((unsigned char *)&(queaddr))[2];
		XBYTE[REG_II_CmdQueueAddr2] = ((unsigned char *)&(queaddr))[1];
		XBYTE[REG_II_CmdQueueAddr3] = ((unsigned char *)&(queaddr))[0];
	}
	
	XBYTE[REG_II_CmdSrc] = 1; 	  //Queue Trigger mode	
	XBYTE[REG_II_Intr] = 0x03;	//Clear done interrupt
	XBYTE[REG_II_CmdTrigger] = 0x01; 	//Trigger

	return TRUE;
}
#if FD_FUNC_NoUse
/**
 * @fn        void HAL_FdII(UINT16 yuvsrchsize, UINT16 yuvsrcvsize, UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, UINT8 iidsthsize, UINT8 iidstvsize, UINT32 yuvsrcaddr, UINT32 iisrcaddr, UINT32 siisrcaddr, UINT8 iisrchsize, UINT8 iisrcvsize, UINT8 *dsthsize, UINT8 *dstvsize, UINT32 *dstaddr)
 * @brief     set II&SII and trigger
 * @param     [in] yuvsrchsize
 * @param     [in] yuvsrcvsize
 * @param     [in] roihoff
 * @param     [in] roivoff
 * @param     [in] roihsize
 * @param     [in] roivsize
 * @param     [in] iidsthsize
 * @param     [in] iidstvsize
 * @param     [in] yuvsrcaddr(w)
 * @param     [in] iisrcaddr(w)
 * @param     [in] siisrcaddr(w)
 * @param     [in] iisrchsize
 * @param     [in] iisrcvsize
 * @param     [in] dsthsize
 * @param     [in] dstvsize
 * @param     [in] dstaddr(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdII( 
		UINT16 yuvsrchsize, UINT16 yuvsrcvsize, 
		UINT16 roihoff, UINT16 roivoff, UINT16 roihsize, UINT16 roivsize, 
		UINT8  iidsthsize, UINT8 iidstvsize, 
		UINT32 yuvsrcaddr, 
		UINT32 iisrcaddr, 
		UINT32 siisrcaddr,
              UINT8 iisrchsize, UINT8 iisrcvsize, 
              UINT8 *dsthsize, UINT8 *dstvsize, 
              UINT32 *dstaddr)
{
	UINT32 queaddr;
	HAL_FdIIJobQueue( yuvsrchsize, yuvsrcvsize, 
		roihoff, roivoff, roihsize, roivsize, 
		 iidsthsize, iidstvsize, yuvsrcaddr, iisrcaddr, siisrcaddr,
              iisrchsize, iisrcvsize, dsthsize, dstvsize, dstaddr,
              queaddr);
	HAL_FdIITrig(queaddr,0);
}
/**
 * @fn        void HAL_FdIIQueueWait(void)
 * @brief     waiting II job queue done
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void HAL_FdIIQueueWait(void)
{
	while ((XBYTE[REG_II_Intr] & 0x01) == 0x00);
}
/**
 * @fn        UINT8 HAL_FdIIQueueDone(void)
 * @brief     II job queue trigger is done
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_FdIIQueueDone(void)
{
	//printf("done:%bx\n",XBYTE[REG_II_Intr] );
	return ((XBYTE[REG_II_Intr] & 0x01) != 0x00);
}
void HAL_FdIIRst(void)
{
	XBYTE[REG_II_ModuleControl]=1;
	XBYTE[REG_II_ModuleControl]=1;
	XBYTE[REG_II_ModuleControl]=0;
}
#endif
void HAL_FDIntrEnable(
	UINT8 IIQueueDone,
	UINT8 IICmdDone,
	UINT8 IISIIOverFlow,
	UINT8 fdDone
	)
{
	//Enable/Disable II interrupt	
	XBYTE[REG_II_IntrEnable] = (IIQueueDone?0x01:0)|(IICmdDone?0x02:0)|(IISIIOverFlow?0x04:0);
	//Enable/Disable FD interrupt	
	XBYTE[REG_FD_IntrEnable]= (fdDone?0x01:0); 
}

void HAL_FdSetDataBase(UINT32 addr)
{
	UINT8 t0,t1;
	UINT16 i;
	UINT8 xdata *pt=G_ucStorData;
	HAL_DramStartAddrSet(addr,1);
	HAL_DramWordDataRead(&t0,&t1);
	XBYTE[REG_FD_DB0] = t0;//tbldata[0];	//Database in REG
	XBYTE[REG_FD_DB1] = t1;//tbldata[1];	//Database in REG
	HAL_DramWordDataRead(&t0,&t1);
	XBYTE[REG_FD_DB2] = t0;//tbldata[2];	//Database in REG
	XBYTE[REG_FD_DB3] = t1;//tbldata[3]; 	//Database in REG
	
	XBYTE[ REG_Cdsp_macroSel ] = 0x06;
	XBYTE[ REG_Cdsp_switchclk ] = 0x06;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;

	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(addr+2);
	HAL_DmaDo(0,1,0x400,0,0);
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
	for(i=0;i<0x100;i++){
		XBYTE[0x2b00|i]=pt[i<<2];
	}
	XBYTE[ REG_Cdsp_macropagsel ] = 0x01;
	for(i=0;i<0x100;i++){
		XBYTE[0x2b00|i]=pt[(i<<2)|1];
	}
	XBYTE[ REG_Cdsp_macropagsel ] = 0x02;
	for(i=0;i<0x100;i++){
		XBYTE[0x2b00|i]=pt[(i<<2)|2];
	}
	XBYTE[ REG_Cdsp_macropagsel ] = 0x03;
	for(i=0;i<0x100;i++){
		XBYTE[0x2b00|i]=pt[(i<<2)|3];
	}

	XBYTE[ REG_Cdsp_macroSel ] = 0x00;
	XBYTE[ REG_Cdsp_switchclk ] = 0x00;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
	
	addr += 0x202;
	HAL_FdDBAddrSet(addr);
}
#if 0
void HAL_FdSetDataBaseIntr(UINT32 addr)
{
	//UINT8 t0,t1;
	UINT16 i;
	UINT8 buf[1028];
	UINT8 xdata *pt=buf;//G_ucStorData;
	//UINT8 xdata *ptReg;
	//UINT32 tt0,tt1;

	//halFdGetTimer(&tt0);
	#if 1
	//INTR_HAL_DmaSetDramAddr(addr);
	//INTR_HAL_DmaSetSramAddr((UINT16)buf);
	//INTR_HAL_DmaDo(0, 1, 1028, 1, 2);
	HAL_DmaSetSramAddr((UINT16)buf);
	HAL_DmaSetDramAddr(addr);
	HAL_DmaDo(0,1,0x400,0,0);
	XBYTE[REG_FD_DB0] = *pt++;
	XBYTE[REG_FD_DB1] = *pt++;
	XBYTE[REG_FD_DB2] = *pt++;
	XBYTE[REG_FD_DB3] = *pt++;
	#endif
	//HAL_DramDmaRead(pt, addr, 1028, 0);	
	#if 0
	HAL_DramStartAddrSet(addr,1);
	HAL_DramWordDataRead(&t0,&t1);
	XBYTE[REG_FD_DB0] = t0;//tbldata[0];	//Database in REG
	XBYTE[REG_FD_DB1] = t1;//tbldata[1];	//Database in REG
	HAL_DramWordDataRead(&t0,&t1);
	XBYTE[REG_FD_DB2] = t0;//tbldata[2];	//Database in REG
	XBYTE[REG_FD_DB3] = t1;//tbldata[3]; 	//Database in REG
	#endif
	XBYTE[ REG_Cdsp_macroSel ] = 0x06;
	XBYTE[ REG_Cdsp_switchclk ] = 0x06;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
	//pt+=4;
	//HAL_DmaSetSramAddr(0x0000);
	//HAL_DmaSetDramAddr(addr+2);
	//HAL_DmaDo(0,1,0x400,0,0);
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
	for(i=0;i<0x100;i++)
	{
		XBYTE[0x2b00|i]=pt[i<<2];
	}
	
	XBYTE[ REG_Cdsp_macropagsel ] = 0x01;
	for(i=0;i<0x100;i++)
	{
		XBYTE[0x2b00|i]=pt[(i<<2)|1];
	}
	XBYTE[ REG_Cdsp_macropagsel ] = 0x02;
	for(i=0;i<0x100;i++)
	{
		XBYTE[0x2b00|i]=pt[(i<<2)|2];
	}
	XBYTE[ REG_Cdsp_macropagsel ] = 0x03;
	
	for(i=0;i<0x100;i++)
	{
		XBYTE[0x2b00|i]=pt[(i<<2)|3];
	}

	XBYTE[ REG_Cdsp_macroSel ] = 0x00;
	XBYTE[ REG_Cdsp_switchclk ] = 0x00;
	XBYTE[ REG_Cdsp_macropagsel ] = 0x00;
	
	addr += 0x202;
	XBYTE[REG_FD_DataBaseAddr0] = ((unsigned char *)&(addr))[3];
	XBYTE[REG_FD_DataBaseAddr1] = ((unsigned char *)&(addr))[2];
	XBYTE[REG_FD_DataBaseAddr2] = ((unsigned char *)&(addr))[1];
	XBYTE[REG_FD_DataBaseAddr3] = ((unsigned char *)&(addr))[0];
	//HAL_FdDBAddrSet(addr);
	//halFdGetTimer(&tt1);
	//printf("%lx,db:%lu\n",(UINT32)pt,tt1-tt0);
}
#endif
#endif
