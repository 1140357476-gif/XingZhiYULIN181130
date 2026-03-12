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
 * @file      Lsc_cal.c
 * @brief     Standalone calibration LSC
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/




/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "common.h"	 
#include "front_logi.h"

#include "lsc_cal.h" 
#include "utility.h"
#include "vfs_api.h"
#include "initio.h"
#include "asicreg.h"
#include "dbg_wdt.h"
#include "cdsp_filter.h"
#include "ae_api.h"
#include "sp1k_calibration_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_ae_api.h"
#include "hal_front.h"
#include "front_logi.h"
#include "app_msg.h"
#include "rsc_id.h"
#include "app_audio_play.h"
#include "sp1k_rsvblk_api.h"
#include "app_ui_para.h"
#include "sp1k_hal_api.h"
#include "lsc_debug.h"
#include "reg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/ 


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/



#ifdef LSC_CALIB_ENABLE

#define _FIRMWARE_

#define MEM_POOL_ADDR  (UINT32)0xC000

//#define HW_GAIN_INT_BIT			2
//#define gainRes			6	/* HW Gain = 2.6 bit for 4ch */
									/*           2.8 bit for 3ch */		
//#define HW_MAX_GAIN				((1<<(HW_GAIN_INT_BIT+gainRes))-1)
//#define HW_1X_GAIN				(1<<(gainRes))

#define SUBSAMPLE_UPPER_LEFT  	(UINT8)0
#define SUBSAMPLE_UPPER_RIGHT 	(UINT8)1
#define SUBSAMPLE_LOWER_LEFT  	(UINT8)2
#define SUBSAMPLE_LOWER_RIGHT 	(UINT8)3


/* LSC */
#define LSC_DATA_BITS			8    /* 8 bit */
#define LSC_DATA_MASK			0xff /* mask 8 bit */

#define MASK_RADIUS        (UINT8)6
#define TBL_FILTER_SIZE    (UINT8)2	/*max 3*/

#endif

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifdef LSC_CALIB_ENABLE
#define abs(a)			( (a) > 0 ? (a) : -(a) )
#define CLampMax(a,b)   (a)>(b)?(b):(a)
#define CLampmin(a,b)   (a)<(b)?(b):(a)
#endif 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#ifdef LSC_CALIB_ENABLE 
UINT8 lscCalLevel;
//UINT8 lscCalCenterEnable=0;
//UINT8 lscCalvflipEnable=0;
//UINT8 lscCalhflipEnable=0;
UINT8 lscGravgupT;
UINT8 lscGravglowT;
UINT8 Maskradius;
UINT8 stepX,stepY;
UINT16 filtersize;
UINT8 channelMode;
UINT8 gainRes;

float PPgr[3];
UINT8 StpFac1,StpFac2;
UINT16 SamPts;

//UINT16 *lscCalLogBuf; /* x, y, factor1, factor2, ratio_enable, ratio */
//float SamGaGr[256],CorrGa[256];
//float Pos[256];
//float SamYgr_n[256],SamYgr[256];
//float Segnum = 11;
//UINT8 shadingOBValue = 0;

//int ellipseEN;	
#endif

#ifdef MI_LSC_CAL_OPTION
UINT8 lensShadingByFront = 0;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#ifdef MI_LSC_CAL_OPTION
	//extern UINT32 (*pcalibLscCfgSet)(UINT32 selector, UINT32 value);
	//extern UINT32 (*pcalibDoLsc)(UINT8 *);
	extern xdata UINT32 snapRawDataAddr;
#endif	
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#ifdef LSC_CALIB_ENABLE 

//static UINT8 *addrAlignedTo16(UINT8 *psrc); 
static UINT8 lscRawPixRead(UINT32 bufAddr,UINT16 width,UINT16 height,UINT16 xPos,UINT16 yPos);
static void subSamplePosGet(UINT16 *xpos,UINT16 *ypos,UINT8 sampleMethod);

#endif


#ifdef LSC_CALIB_ENABLE 

/**
 * @fn        void subSamplePosGet(UINT16 *xpos, UINT16 *ypos, UINT8 sampleMethod)
 * @brief     subSamplePosGet
 * @param     [in] xpos
 * @param     [in] ypos
 * @param     [in] sampleMethod
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-11-23
 * @todo      N/A
 * @bug       N/A
*/
static void
subSamplePosGet(
	UINT16 *xpos,
	UINT16 *ypos,
	UINT8 sampleMethod
)
{
	switch(sampleMethod) {
	case SUBSAMPLE_UPPER_LEFT:
	default:
		break;
	case SUBSAMPLE_UPPER_RIGHT:
		*xpos = *xpos + 1;
		break;
	case SUBSAMPLE_LOWER_LEFT:
		*ypos = *ypos + 1;
		break;
	case SUBSAMPLE_LOWER_RIGHT:
		*xpos = *xpos + 1;
		*ypos = *ypos + 1;
		break;
	}
}

static UINT8
lscRawPixRead(
	UINT32 bufAddr,
	UINT16 width,
	UINT16 height,
	UINT16 xPos,
	UINT16 yPos
)
{
	UINT8 lsb,msb;
	UINT32 addr,order;
	
	if(( xPos > width ) || ( yPos > height )) {

		lscPrintf("Input Overflow .. xPos[%d]yPos[%d]\n", xPos, yPos);
		//errReport(ERR_BP_RAWREAD_OVER, DBG_ERR_WEIGHT_CRITICAL);

		return 0;
	}
	else{
		addr =((UINT32)yPos * (UINT32)width + (UINT32)xPos);

		order = (UINT8)(addr&0x01);
		addr = addr>>1;		
		addr += bufAddr;
		
		M_DRAM_SetStartAddressV(addr, 1);


		M_DRAM_ReadWord(lsb,msb);

		if (order==0){		
			return lsb;
		}
		else{
			return msb;
		}
	}
}

/**
 * @fn        void lscTblGen(UINT16 xx, UINT16 yy, UINT8 *rgbRawAddr, UINT8 sampleMethod, UINT16 imgWidth, UINT16 imgHeight, UINT32 *Gain, UINT8 FalloffPer, UINT32 Maxval)
 * @brief     lscTblGen
 * @param     [in] xx
 * @param     [in] yy
 * @param     [in] rgbRawAddr
 * @param     [in] sampleMethod
 * @param     [in] imgWidth
 * @param     [in] imgHeight
 * @param     [in] Gain
 * @param     [in] FalloffPer
 * @param     [in] Maxval
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-11-10
 * @todo      N/A
 * @bug       N/A
*/
void 
lscTblGen (
	UINT32 rawAddr,
	UINT16 hSize,
	UINT16 vSize,
	UINT16 posX, 
	UINT16 posY,
	UINT8 cfa,
	UINT16 *gbGain,
	UINT16 *bGain,
	UINT16 *grGain,
	UINT16 *rGain,
	UINT8 FalloffPer,
	UINT8 maxGbVal,
	UINT8 maxBVal,
	UINT8 maxGrVal,
	UINT8 maxRVal
)
{

	UINT16 Hpeak,Vpeak;
	UINT16 xstart,xend,ystart,yend;
  	UINT16 pixsum;
	UINT16 rSum,grSum,bSum,gbSum;

  	UINT16 Xdif,Ydif,Refidx;  	 

  	UINT32 rSamTarCurve,grSamTarCurve,bSamTarCurve,gbSamTarCurve;
	float temp;
	float r,gr,b,gb;

	lscPrintf("hSize/2=%d \n",hSize>>1);
	lscPrintf("vSize/2=%d \n",vSize>>1); 
	lscPrintf("posX=%d\n",posX);		
	lscPrintf("posY=%d\n",posY);

	rSum=grSum=bSum=gbSum=0;
	pixsum=0;
	xstart= CLampmin((SINT16)posX-Maskradius,0);
	xend= CLampMax(posX+Maskradius,(hSize>>1));
	ystart= CLampmin((SINT16)posY-Maskradius,0);
	yend= CLampMax(posY+Maskradius,(vSize>>1));

	lscPrintf("xstart=%d\n",xstart);
	lscPrintf("xend=%d\n",xend);		
	lscPrintf("ystart=%d\n",ystart);
	lscPrintf("yend=%d\n",yend);		

	lscRawSumGet(rawAddr,hSize,xstart<<1,ystart<<1,xend<<1,yend<<1,cfa,&rSum,&grSum,&bSum,&gbSum);
	pixsum=(xend-xstart)*(yend-ystart);

	lscPrintf("pixsum=%d\n",pixsum);

	if (pixsum==0){
		return; //SP5K_ERR_CALIBRATION_CFG;
	}

	#if 1
		if(channelMode==LSC_3CH){
			r=(float)rSum/pixsum;
			gr=(((float)grSum+(float)gbSum)/(pixsum<<1));
			b=(float)bSum/pixsum;
			if ((r==0)||(gr==0)||(b==0)){
				return; 
			}			
		}
		else{
			r=(float)rSum/pixsum;
			gr=(float)grSum/pixsum;
			b=(float)bSum/pixsum;
			gb=(float)gbSum/pixsum;			
		}
	#else
		if(channelMode==LSC_3CH){		
			rSum=rSum/pixsum;
			grSum= (UINT16)(((UINT32)grSum+(UINT32)gbSum)/(pixsum<<1));
			bSum=bSum/pixsum;
			
			if ((rSum==0)||(grSum==0)||(bSum==0)){
				return; 
			}	
		
		}
		else{
		
			rSum=rSum/pixsum;
			grSum=grSum/pixsum;
			bSum=bSum/pixsum;
			gbSum=gbSum/pixsum;
			if ((rSum==0)||(grSum==0)||(bSum==0)||(gbSum==0)){
				return; 
			}		
		}
	#endif		  
	
	lscPrintf("x=%d,y=%d\n",posX,posY);
	lscPrintf("rSum=%d\n",rSum);  
	lscPrintf("maxRVal=%bu\n",maxRVal);		 

	lscPrintf("bSum=%d\n",bSum);  
	lscPrintf("maxBVal=%bu\n",maxBVal);		 
	
	lscPrintf("grSum=%d\n",grSum);  
	lscPrintf("maxGrVal=%bu\n",maxGrVal);  
	
	lscPrintf("gbSum=%d\n",gbSum); 
	lscPrintf("maxGbVal=%bu\n",maxGbVal);						

  	if (FalloffPer==100){
		#if 1
			if(channelMode==LSC_3CH){		
				*rGain=(UINT16)((float)((UINT16)maxRVal<<gainRes)/r);
				*grGain=(UINT16)((float)((UINT16)maxGrVal<<gainRes)/gr);
				*bGain=(UINT16)((float)((UINT16)maxBVal<<gainRes)/b);			
			}
			else{
				*rGain=(UINT16)((float)((UINT16)maxRVal<<gainRes)/r);
				*grGain=(UINT16)((float)((UINT16)maxGrVal<<gainRes)/gr);
				*bGain=(UINT16)((float)((UINT16)maxBVal<<gainRes)/b);						
				*gbGain=(UINT16)((float)((UINT16)maxGbVal<<gainRes)/gb);						
			}			
	    #else
	    
			if(channelMode==LSC_3CH){				
				*rGain=((UINT16)maxRVal<<gainRes)/(rSum);
				*grGain=((UINT16)maxGrVal<<gainRes)/(grSum);
				*bGain=((UINT16)maxBVal<<gainRes)/(bSum);

			}
			else{		  	
				*rGain=((UINT16)maxRVal<<gainRes)/(rSum);
				*grGain=((UINT16)maxGrVal<<gainRes)/(grSum);
				*bGain=((UINT16)maxBVal<<gainRes)/(bSum);
				*gbGain=((UINT16)maxGbVal<<gainRes)/(gbSum);	
			}		
		#endif
		//if(channelMode==LSC_3CH){
		//	*grGain= (*grGain+*gbGain)>>1; 
		//}
  
		lscPrintf("rGain=%d\n",*rGain);
		lscPrintf("grGain=%d\n",*grGain);
		lscPrintf("bGain=%d\n",*bGain);
		lscPrintf("gbGain=%d\n",*gbGain);
  
	}	  
	else{
	
		Hpeak=hSize>>1;
		Vpeak=vSize>>1;

		posX=posX<<1;
		posY=posY<<1;
		
		if(posX>Hpeak){
			Xdif=((posX)-Hpeak)>>StpFac1;		
		}
		else{
			Xdif=(Hpeak-posX)>>StpFac1;		
		}

		if(posY>Vpeak){
			Ydif=(posY-Vpeak)>>StpFac1;		
		}
		else{
			Ydif=(Vpeak-posY)>>StpFac1;		
		}
		  	
	  	Refidx=(Xdif*Xdif+Ydif*Ydif)>>StpFac2;	  

		lscPrintf("Xdif=0x%x\n",Xdif);
		lscPrintf("Ydif=0x%x\n",Ydif);			
		lscPrintf("StpFac1=%bd\n",StpFac1);
		lscPrintf("StpFac1=%bd\n",StpFac2);
		lscPrintf("Refidx=0x%x\n",Refidx);
		
		
		if(Refidx<SamPts){
			temp=(PPgr[0]*(float)Refidx*Refidx+PPgr[1]*(float)Refidx+PPgr[2]);	
		}
		else{
			temp=(PPgr[0]*((float)Refidx-1)*((float)Refidx-1)+PPgr[1]*((float)Refidx-1)+PPgr[2]);			
		}


		if(channelMode==LSC_3CH){
			rSamTarCurve =(UINT32)(temp*(float)maxRVal);	
			grSamTarCurve=(UINT32)(temp*(float)maxGrVal);	
			bSamTarCurve =(UINT32)(temp*(float)maxBVal);	
		  	
	        *rGain=(rSamTarCurve<<gainRes)/(r);	 
	        *grGain=(grSamTarCurve<<gainRes)/(gr);	 
	        *bGain=(bSamTarCurve<<gainRes)/(b);	 
		}
		else{
			rSamTarCurve =(UINT32)(temp*(float)maxRVal);	
			grSamTarCurve=(UINT32)(temp*(float)maxGrVal);	
			bSamTarCurve =(UINT32)(temp*(float)maxBVal);	
			gbSamTarCurve=(UINT32)(temp*(float)maxGbVal);	

		  	
	        *rGain=(rSamTarCurve<<gainRes)/(r);	 
	        *grGain=(grSamTarCurve<<gainRes)/(gr);	 
	        *bGain=(bSamTarCurve<<gainRes)/(b);	 
	        *gbGain=(gbSamTarCurve<<gainRes)/(gb);	 				
		}

		lscPrintf("rGain=%d\n",*rGain);
		lscPrintf("grGain=%d\n",*grGain);
		lscPrintf("bGain=%d\n",*bGain);
		lscPrintf("gbGain=%d\n",*gbGain);
		
		lscPrintf("rSamTarCurve=%ld\n",rSamTarCurve);
		lscPrintf("grSamTarCurve=%ld\n",grSamTarCurve);
		lscPrintf("bSamTarCurve=%ld\n",bSamTarCurve);
		lscPrintf("gbSamTarCurve=%ld\n",gbSamTarCurve);
	  	
	}


	if(channelMode==LSC_3CH){		
		*rGain=CLampMax(*rGain,1023);
		*grGain=CLampMax(*grGain,1023);
		*bGain=CLampMax(*bGain,1023);
	}
	else{
		*rGain=CLampMax(*rGain,255);	
		*grGain=CLampMax(*grGain,255);	
		*bGain=CLampMax(*bGain,255);	
		*gbGain=CLampMax(*gbGain,255);	
	}
	
}

void
lscMaxValGetbyFilter(
	UINT32 rawAddr,
	UINT16 hSize,
	UINT16 vSize,
	UINT8 cfa,
	UINT8 *maxR,
	UINT8 *maxGr,
	UINT8 *maxB,
	UINT8 *maxGb,
	UINT8 *warningM
)
{

	UINT16 row,col;
	UINT16 rSum,grSum,bSum,gbSum;
	UINT16 pixNum;
	UINT16 grRatio,gbRatio;
	UINT8 maxGrTmp,maxRTmp,maxBTmp,maxGbTmp;
	
	*maxR=0;
	*maxGr=0;
	*maxB=0;
	*maxGb=0;

    for(row=(vSize/4); row<(vSize*3/4); row+=filtersize){
		for(col=(hSize/4); col<(hSize*3/4); col+=filtersize){ 	

			rSum=0;
			grSum=0;
			bSum=0;	
			gbSum=0;

			lscRawSumGet(rawAddr,hSize,col,row,col+filtersize,row+filtersize,cfa,&rSum,&grSum,&bSum,&gbSum);

			pixNum=(filtersize*filtersize)>>2;
				
			if ((UINT8)(grSum/pixNum)>lscGravgupT){
				*warningM=1;
			}
			else if ((UINT8)(grSum/pixNum)<lscGravglowT){	
				*warningM=2;
			}
			else{
				*warningM=0;
			}

			maxGrTmp=(UINT8)(grSum/pixNum);		
			maxGbTmp=(UINT8)(gbSum/pixNum);	
			maxRTmp = (UINT8)(rSum/pixNum);
			maxBTmp = (UINT8)(bSum/pixNum);
			
			grRatio=((UINT32)grSum<<8)/rSum;
			//gbRatio=((UINT32)grSum<<8)/bSum;
			gbRatio=((UINT32)gbSum<<8)/bSum;

#if 0
			maxRTmp=(UINT8)(((UINT16)maxGrTmp<<8)/grRatio);
			//maxBTmp=(UINT8)(((UINT16)maxGrTmp<<8)/gbRatio);
			maxBTmp=(UINT8)(((UINT16)maxGbTmp<<8)/gbRatio);
#endif        	
           	if(maxRTmp> *maxR){   
           		*maxR=maxRTmp;
           	}
           	if(maxGrTmp> *maxGr){
           		*maxGr=maxGrTmp;
           	}
           	if(maxBTmp> *maxB){
           		*maxB=maxBTmp;
           	}
		    if(maxGbTmp>*maxGb){
      		       *maxGb=maxGbTmp;
		    }

			lscPrintf("grSum=0x%x\n",grSum);
			lscPrintf("warningM=%bd \n",*warningM);			
			lscPrintf("Rratio=0x%x,Bratio=0x%x \n",GRratio,GBratio);			
			lscPrintf("pixNum=%d\n",pixNum);
    		lscPrintf("maxRval=%d,maxGrval=%d,maxBval=%d \n",*maxR,*maxG,*maxB);
		
		}
	}

	lscPrintf("maxRval=0x%bx,maxGrval=0x%bx,maxBval=0x%bx \n",*maxR,*maxG,*maxB);

}

/**
 * @fn        UINT8 lscCalibration(UINT8 *rawAddr)
 * @brief     lscCalibration
 * @param     [in] rawAddr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-11-10
 * @todo      N/A
 * @bug       N/A
*/
UINT8
lscCalibration(
	UINT32 rawAddr,
	UINT16 hSize,
	UINT16 vSize,
	UINT8 cfa
)
{
	UINT8 xdata* pTbl;
	
	UINT8 warningM=0;
	UINT32 tblAddr,tblNum;
	UINT16 bkCnt;
	UINT16 i;
	UINT32 pixSize;
	UINT8 maxRval,maxGrval,maxBval,maxGbval;
	UINT8 sxreg,syreg,tblbitreg;
	UINT8 Wnum,Hnum;	
	UINT8 hdr[8];
    UINT8 hdr2[16];    
	UINT16  y,x; /*2010/11/05 by YC*/	
    UINT16 Gbgain, Grgain, Rgain, Bgain; 
	UINT32 Gain32b;
	UINT8* pGain = (UINT8*)&Gain32b;
	UINT32 addrTemp;
	//UINT8 sts;
	UINT32 time2,time3;	
	UINT32 PreLineAddr;
	UINT8 retVal;

	lscPrintfLog("LSC CAL Start\n");

	//cfa=0;	
   	dbgWdtSet(0);	/*stop watch dog*/

	//dramFileSave("LSCRAW01.RAW",rawAddr,(UINT32)hSize*(UINT32)vSize);

	
	pixSize = (UINT32)hSize*(UINT32)vSize;

    HAL_GlobalReadGTimer(&time2);        
   
	Wnum=(hSize>>1)/stepX;
	Hnum=(vSize>>1)/stepY;

    if ((hSize>>1)%stepX!=0){
		Wnum=Wnum+2;
	}
	else{
	 	Wnum=Wnum+1;	
	}
	 
	if ((vSize>>1)%stepY!=0){
		Hnum=Hnum+2;
	}
	else{
		Hnum=Hnum+1;	
	} 

	if ((((UINT32)Wnum*Hnum*4)+24)>K_SDRAM_LscTableBufSize*2){
		lscPrintf("Table too big\n");
		return FAIL;
	}
	
   	lscPrintf("rawAddr:0x%lx\n",rawAddr);
   	lscPrintf("cfa_pattern=%bd\n",cfa);
	lscPrintf("hSize=%d\n",hSize);
	lscPrintf("vSize=%d\n",vSize);       
   	lscPrintf("stepX=%bd\n",stepX);  
   	lscPrintf("stepY=%bd\n",stepY);
   	lscPrintf("Wnum=%bd\n",Wnum);
   	lscPrintf("Hnum=%bd\n",Hnum);	   
	
	lscPrintf("lscCalLevel=%bd \n",lscCalLevel);
	 
	lscMaxValGetbyFilter(rawAddr,hSize,vSize,cfa,&maxRval,&maxGrval,&maxBval,&maxGbval,&warningM);  
     
	if (channelMode==LSC_3CH){	/* 3 channel mode,  gr=(gb=gr+gb)/2 */
		//maxGbval=(maxGrval+maxGrval)>>1;
		maxGbval=(UINT8)(((UINT16)maxGrval + (UINT16)maxGbval) >> 1);
		maxGrval=maxGbval;
	}

	//maxGrval=maxGbval;

	lscPrintf("maxRval=0x%bx,maxGrval=0x%bx,maxBval=0x%bx,maxGbval=0x%bx \n",maxRval,maxGrval,maxBval,maxGbval);
	
	/* chiaying said the warning is the wrong case */
	if (warningM != 0) {
		lscPrintf("error! Warning: %bd\n", warningM);
		return FAIL;
	}

	/* -- Do the LSC --------------------------------------------------------------------------------> */
  	
	tblNum=0;		
	pTbl=(UINT8 xdata*)MEM_POOL_ADDR;	/* store one row of table */

	tblAddr=K_SDRAM_LSCTableBufAddr+(UINT32)12;

	if(lscCalLevel!=100){
		lscTableShapeParCal(hSize,vSize,lscCalLevel);
	}
	
   	for (y=0;y<(vSize>>1);y+=stepY){ 
   		bkCnt=0;
       	for (x=0;x<(hSize>>1);x+=stepX){ 		     
			lscTblGen(rawAddr,hSize,vSize,x,y,cfa,&Gbgain,&Bgain,&Grgain,&Rgain,lscCalLevel,
						maxGbval,maxBval,maxGrval,maxRval);			       		
			lscPrintf("(%d,%d):rGain=%d,grGain=%d,bGain=%d,gbGain=%d\n",x,y,Rgain,Grgain,Bgain,Gbgain);						            

			if (channelMode==LSC_3CH){
				Gain32b = ((UINT32)Bgain&0x3ff) + (((UINT32)Grgain&0x3ff)<<10) + (((UINT32)Rgain&0x3ff)<<20);
			}
			else{
				pGain[0]=(UINT8)Gbgain;
				pGain[1]=(UINT8)Bgain;
				pGain[2]=(UINT8)Grgain;
				pGain[3]=(UINT8)Rgain;						
			}

			lscPrintf("x=%d,y=%d\n",x,y);
			lscPrintf("gain0=0x%bx\t",pGain[0]);
			lscPrintf("gain1=0x%bx\t",pGain[1]);
			lscPrintf("gain2=0x%bx\t",pGain[2]);			 
			lscPrintf("gain3=0x%bx\n",pGain[3]);				            

			bkCnt+=4;

			addrTemp=(tblAddr+(tblNum<<1));				
			M_DRAM_SetStartAddressV(addrTemp, 0);

			M_DRAM_WriteWord(pGain[3], pGain[2]);
			M_DRAM_WriteWord(pGain[1], pGain[0]);			
			
			tblNum++;	

  		}

		addrTemp=(tblAddr+(tblNum<<1));				
		M_DRAM_SetStartAddressV(addrTemp, 0);
		M_DRAM_WriteWord(pGain[3], pGain[2]);
		M_DRAM_WriteWord(pGain[1], pGain[0]);	
		tblNum++;
			
		lscPrintf("W2:0x%bx%bx%bx%bx\n",pGain[0],pGain[1],pGain[2],pGain[3]);

   	}

	PreLineAddr = tblAddr+((tblNum-((bkCnt/4)+1))<<1);
       	addrTemp=(tblAddr+(tblNum<<1));				
	lscPrintf("s=%lx,d=%lx\n",PreLineAddr,addrTemp);
	retVal = HAL_GprmDramDma((PreLineAddr<<1), (addrTemp<<1), (UINT32)Wnum*4, 1);
	lscPrintf("retVal=%bu\n",retVal);
	
	HAL_GlobalReadGTimer(&time3);

	if (channelMode==LSC_3CH){
		if (gainRes==9){
	    	tblbitreg=0;  
	    }
		else if (gainRes==8){
			tblbitreg=1;
		}
		else if (gainRes==7){
			tblbitreg=2;	
		}  	
	}
	else{
		if (gainRes==7){
	    	tblbitreg=0;  
	    }
		else if (gainRes==6){
			tblbitreg=1;
		}
		else if (gainRes==5){
			tblbitreg=2;	
		}  
	}

	if (stepX==8){	
		sxreg=0; 	
	}
	else if (stepX==16){
		sxreg=1;	
	}
	else if (stepX==32){ 	
		sxreg=2;
	}  

	if (stepY==8){
		syreg=0; 	
	}
	else if (stepY==16){
		syreg=1;
	}
	else if (stepY==32){ 	
		syreg=2;
	}

	/* Save table */
	/* fsFileDelete(LscTable); */

    hdr[0]=0;
    hdr[1]=0x0;
    hdr[2]=0x5c;
    hdr[3]=0x06;
    hdr[4]=0;
    hdr[5]=0;
    hdr[6]=(UINT8)(((UINT16)Wnum*(UINT16)Hnum*4)&0xff);
    hdr[7]=(UINT8)(((UINT16)Wnum*(UINT16)Hnum*4)/256);
  	
    hdr2[0]=(UINT8)((syreg<<4)+sxreg);
    hdr2[1]=(UINT8)tblbitreg;
    hdr2[2]=(UINT8)Wnum;
    hdr2[3]=(UINT8)Hnum;
    hdr2[4]=0;

    if(channelMode==LSC_3CH){
	    hdr2[5]=0x00;    /*3 channgel Mode*/
    }
    else{
    	hdr2[5]=0x01;    /*4 channgel Mode*/
    }
    
	for (i=6;i<16;i++){
		hdr2[i]=0;
	}	

	addrTemp=tblAddr-(UINT32)12;
	M_DRAM_SetStartAddressV(addrTemp, 0);

	for(i=0;i<8;i=i+2){
		M_DRAM_WriteWord(hdr[i],hdr[i+1]);
	}

	for(i=0;i<16;i=i+2){
		M_DRAM_WriteWord(hdr2[i],hdr2[i+1]);
	}


	HAL_GlobalReadGTimer(&time3);
	//lscPrintf("sizeof(buf)=%d\n",sizeof(buf));
	//lscPrintf("ptable_n=0x%x \n",buf);
	lscPrintf("Total %d ms\n",  time3- time2);

	lscPrintf("lsc addr=0x%lx\n",K_SDRAM_LSCTableBufAddr);

	#if 0
		/* 1628 file save */
		sts = dramFileSave("LENSLUT0.BIN",addrTemp,(UINT32)(((UINT32)Wnum*Hnum*4)+24));
		if( sts != TRUE ) {
			printf("\n...Failed to save...\n");
			vfsFileDelete("LENSLUT0.BIN"); //xian ++ 20090717, #38169
			return FAIL;
		}	
	#endif
	
	lscTblFilter(tblAddr,Wnum,Hnum,TBL_FILTER_SIZE);	
	//lscTblFilter(tblAddr,Wnum,Hnum,TBL_FILTER_SIZE);

	#if 0
		/* 1628 file save */
		sts = dramFileSave("LENSLUT1.BIN",addrTemp,(UINT32)(((UINT32)Wnum*Hnum*4)+24));
		if( sts != TRUE ) {
			printf("\n...Failed to save...\n");
			vfsFileDelete("LENSLUT1.BIN"); //xian ++ 20090717, #38169
			return FAIL;
		}	
	#endif
	
	#if 0	
		if (lscCalLogBuf) {
			lscCalLogBuf[0] = sxreg;
			lscCalLogBuf[1] = syreg;
			lscCalLogBuf[2] = tblbitreg;
			lscCalLogBuf[3] = Wnum;
			lscCalLogBuf[4] = Hnum;
			lscCalLogBuf[5] = 0;
			lscCalLogBuf[6] = ws;
			lscCalLogBuf[7] = warningM;
			lscCalLogBuf[8] = lscGravgupT;
			lscCalLogBuf[9] = lscGravglowT;
		}
	#endif
	
	lscPrintfLog("LSC CAL Finish\n");

	dbgWdtSet(1);	/*start watch dog*/
	return SUCCESS;
}

/**
 * @fn        UINT8 lscCfgSet(UINT8 selector, UINT8 value)
 * @brief     lscCfgSet
 * @param     [in] selector
 * @param     [in] value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-11-10
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
sp1kLscCfgSet(
	UINT8 selector, 
	UINT8 value
)
{
	switch (selector) {
	case SP1K_CALIB_LSC_LUMA_LEVEL:
		lscCalLevel = value;
	break;
	case SP1K_CALIB_LSC_GR_AVG_HIGH:
		lscGravgupT = value;
	break;
	case SP1K_CALIB_LSC_GR_AVG_LOW:
		lscGravglowT = value;
	break;
	case SP1K_CALIB_LSC_STEP_X:
		stepX=value;
	break;
	case SP1K_CALIB_LSC_STEP_Y:	
		stepY=value;
	break;
	case SP1K_CALIB_LSC_CHANNEL:
		channelMode=value;
	break;
	case SP1K_CALIB_LSC_RESOLUTION:
		gainRes=value;
	break;
#if 0		
	case SP1K_CALIB_LSC_CENTER_ENABLE:
		lscCalCenterEnable = value;
	break;
	case SP1K_CALIB_LSC_VFLIP_ENABLE:
		lscCalvflipEnable = value;
	break;
	case SP1K_CALIB_LSC_HFLIP_ENABLE:
		lscCalhflipEnable = value;
	break;
		
	
	case SP1K_CALIB_LSC_OUTPUT_NAME:
		if (strlen((char *)value)+1 >+ sizeof(LscTable))
			return FAIL;
		strcpy(LscTable, (char *)value);
	break;
	case SP1K_CALIB_LSC_LOAD_RAW:
	case SP1K_CALIB_LSC_SAVE_RAW:
		if (value==0) {
			LscRawName[0] = 0;
			lscCalRawSaveLoad = RAW_NONE;
			break;
		}
		if (strlen((char *)value)+1 > sizeof(LscRawName))
			return FAIL;
					
		strcpy(LscRawName, (char *)value);
		lscCalRawSaveLoad = selector==SP1K_CALIB_LSC_LOAD_RAW ? RAW_LOAD : RAW_SAVE;
	break;
	case SP1K_CALIB_LSC_LOG_BUF:
		lscCalLogBuf = (UINT16 *)value;
	break;
#endif		

	default:
		return FAIL;
	}
	
	return SUCCESS;
}


/**
 * @fn        UINT8 lsEq(float *xin, float *yin, t len, float C)
 * @brief     lsEq
 * @param     [in] xin
 * @param     [in] yin
 * @param     [in] len
 * @param     [in] C
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-11-10
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
lsEq(
	float *xin, 
	float *yin, 
	UINT8 len, 
	float C[3]
)
{
    float DET,Tmp[3][3],A[3][3],B[3];
    float K1,K2,K3,K4,K5,K6,K7,K8;				
    UINT8 col;

	K1=0;
	for(col=0; col<len; col++){
		K1=K1+xin[col]*xin[col]*xin[col]*xin[col];
	}

	K2=0;
	for(col=0; col<len; col++){
		K2=K2+xin[col]*xin[col]*xin[col];
	}
	
	K3=0;
	for(col=0; col<len; col++){
		K3=K3+xin[col]*xin[col];
	}

	K4=0;
	for(col=0; col<len; col++){
		K4=K4+xin[col];
	}

	K5=0;
	for(col=0; col<len; col++){	
		K5=K5+1.0;
	}

	K6=0;
	for(col=0; col<len; col++){
		K6=K6+xin[col]*xin[col]*yin[col];
	}	

	K7=0;
	for(col=0; col<len; col++){	
		K7=K7+xin[col]*yin[col];
	}

	K8=0;
	for(col=0; col<len; col++){	
		K8=K8+yin[col];
	}


	//printf("K1=%f\n",(K1));
	//printf("K2=%f\n",(K2));
	//printf("K3=%f\n",(K3));
	//printf("K4=%f\n",(K4));
	//printf("K5=%f\n",(K5));
	
	
		
	A[0][0]=K1; A[0][1]=K2; A[0][2]=K3;
	A[1][0]=K2; A[1][1]=K3; A[1][2]=K4;
	A[2][0]=K3; A[2][1]=K4; A[2][2]=K5;




	B[0]=K6; B[1]=K7; B[2]=K8;
	
	DET=A[0][0]*(A[2][2]*A[1][1]-A[2][1]*A[1][2])-A[1][0]*(A[2][2]*A[0][1]-A[2][1]*A[0][2])+A[2][0]*(A[1][2]*A[0][1]-A[1][1]*A[0][2]);

  	//printf("DET=%f\n",DET);
  	
	if(DET!=0)
	{
		Tmp[0][0]= (A[2][2]*A[1][1]-A[2][1]*A[1][2])/DET;
		Tmp[0][1]=-(A[2][2]*A[0][1]-A[2][1]*A[0][2])/DET;
		Tmp[0][2]= (A[1][2]*A[0][1]-A[1][1]*A[0][2])/DET;
		Tmp[1][0]=-(A[2][2]*A[1][0]-A[2][0]*A[1][2])/DET;
		Tmp[1][1]= (A[2][2]*A[0][0]-A[2][0]*A[0][2])/DET;
		Tmp[1][2]=-(A[1][2]*A[0][0]-A[1][0]*A[0][2])/DET;
		Tmp[2][0]= (A[2][1]*A[1][0]-A[2][0]*A[1][1])/DET;
		Tmp[2][1]=-(A[2][1]*A[0][0]-A[2][0]*A[0][1])/DET;
		Tmp[2][2]= (A[1][1]*A[0][0]-A[1][0]*A[0][1])/DET;

		C[0]=Tmp[0][0]*B[0]+Tmp[0][1]*B[1]+Tmp[0][2]*B[2];
		C[1]=Tmp[1][0]*B[0]+Tmp[1][1]*B[1]+Tmp[1][2]*B[2];
		C[2]=Tmp[2][0]*B[0]+Tmp[2][1]*B[1]+Tmp[2][2]*B[2];

		//printf("Tmp[0][0]=%f\n",Tmp[0][0]);
	}
	else
	{
		C[0]=1.0;
		C[1]=1.0;
		C[2]=1.0;
	}

	//printf("c0=%ld,c1=%ld,c2=%ld\n",(UINT32)(C[0]*1000),(UINT32)(C[1]*1000),(UINT32)(C[2]*1000));
	return SUCCESS;
}


/**
 * @fn        void lscCalInit(void)
 * @brief     lscCalInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-12-13
 * @todo      N/A
 * @bug       N/A
*/
void
lscCalInit(
	void
)
{
	//pcalibLscCfgSet =  calibLscCfgSet;
	//pcalibDoLsc = calibLsc;
	//lscCalLevel=100;
	lscGravgupT=100;
	lscGravglowT=20;
	Maskradius=MASK_RADIUS;/*maxGrval*(Maskradius*2*2+1)*(Maskradius*2*2+1)/4<65535*/
	                                     /*if maxGrval=80,Maskradius<15*/
	filtersize=16;
	//channelMode=LSC_3CH;
	//gainRes=8;
	//stepX=stepY=16;
}	

/**
 * @fn        void lscRawSumGet(UINT32 rawAddr, UINT16 hSize, UINT16 xStart, UINT16 yStart, UINT16 xEnd, UINT16 yEnd, UINT8 cfa, UINT16 *r, UINT16 *gr, UINT16 *b, UINT16 *gb)
 * @brief     lscRawSumGet
 * @param     [in] rawAddr
 * @param     [in] hSize
 * @param     [in] xStart
 * @param     [in] yStart
 * @param     [in] xEnd
 * @param     [in] yEnd
 * @param     [in] cfa
 * @param     [in] r
 * @param     [in] gr
 * @param     [in] b
 * @param     [in] gb
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-12-13
 * @todo      N/A
 * @bug       N/A
*/
void 
lscRawSumGet(
	UINT32 rawAddr,
	UINT16 hSize,
	UINT16 xStart,
	UINT16 yStart,
	UINT16 xEnd,
	UINT16 yEnd,	
	UINT8 cfa,
	UINT16 *r,
	UINT16 *gr,
	UINT16 *b,
	UINT16 *gb
)
{
	UINT16 i,j;
	UINT8 type;
	UINT32 addr;
	UINT8 lsb,msb;
		
	for(j=yStart; j<yEnd ; j++){
		addr =((UINT32)j * (UINT32)hSize + (UINT32)xStart);
		addr = addr>>1;
		addr += rawAddr;
		M_DRAM_SetStartAddressV(addr, 1);
		if ((j&0x0001)==0){ /*the same cfa*/
			type=cfa;
		}
		else{ /*different cfa*/
			switch (cfa){
				case FRONT_BAYER_PTN_GRRBGB:
					type=FRONT_BAYER_PTN_BGBGRR;
					break;
				case FRONT_BAYER_PTN_RGRGBB:
					type=FRONT_BAYER_PTN_GBBRGR;	
					break;
				case FRONT_BAYER_PTN_BGBGRR:
					type=FRONT_BAYER_PTN_GRRBGB;	
					break;
				case FRONT_BAYER_PTN_GBBRGR:
					type=FRONT_BAYER_PTN_RGRGBB;	
					break;
			}				
		
		}
	
		for(i=xStart ; i <xEnd ; i=i+2){
			M_DRAM_ReadWord(lsb,msb);
			
			switch (type){
				case FRONT_BAYER_PTN_GRRBGB:
					*gr+=lsb;
					*r+=msb;
				break;
				case FRONT_BAYER_PTN_RGRGBB:
					*r+=lsb;
					*gr+=msb;								
				break;
				case FRONT_BAYER_PTN_BGBGRR:					
					*b+=lsb;
					*gb+=msb;
				break;
				case FRONT_BAYER_PTN_GBBRGR:				
					*gb+=lsb;	
					*b+=msb;
				break;
			}						
		}
			
	}
	
}
	
/**
 * @fn        void lscTableShapeParCal(UINT16 hSize, UINT16 vSize, UINT8 per)
 * @brief     Get parameters for compensation rate is lower than 100%
 * @param     [in] hSize
 * @param     [in] vSize
 * @param     [in] per
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-12-13
 * @todo      N/A
 * @bug       N/A
*/
void 
lscTableShapeParCal(
	UINT16 hSize,
	UINT16 vSize,
	UINT8 per
)
{
#if 1
	UINT16 Hpeak,Vpeak;
	UINT16 SamStp,MaxLen;
	
  	float E[3],F[3]; 
	UINT32 DifLen,DifLen1;
	  	
	Hpeak=hSize>>1;
	Vpeak=vSize>>1;
	
	DifLen=(UINT32)Hpeak*Hpeak+(UINT32)Vpeak*Vpeak;
	
	SamStp=1<<((UINT16)(logFun(DifLen/256)/logFun(2))+1);
	
	if (Hpeak>Vpeak){
		MaxLen=Hpeak;
	}
	else{
		MaxLen=Vpeak;
	}
	
	StpFac1=(UINT16)(logFun(MaxLen)/logFun(2))+1-8;
	DifLen1=((Hpeak)>>StpFac1)*((Hpeak)>>StpFac1)+((Vpeak)>>StpFac1)*((Vpeak)>>StpFac1);
	StpFac2= (UINT16)(logFun(DifLen1/256)/logFun(2))+1;
	SamPts=(UINT16)(DifLen/SamStp)+1;

	E[0]=0;
	E[1]=SamPts>>1;
	E[2]=SamPts-1;

	F[0]=1;
	F[1]=((((100-(float)per)*75/100+(float)per)))/100;
	F[2]=(float)per/100;

	lscPrintf("StpFac1=0x%bx,StpFac2=0x%bx\n",StpFac1,StpFac2);
	lscPrintf("E1=%d,E2=%d,E3=%d\n",(UINT16)(E[0]*100),(UINT16)(E[1]*100),(UINT16)(E[2]*100));
	lscPrintf("F1=%d,F2=%d,F3=%d\n",(UINT16)(F[0]*100),(UINT16)(F[1]*100),(UINT16)(F[2]*100));

	lsEq(E,F,3,PPgr);
	//printf("PPgr0=%ld,PPgr1=%ld,PPgr2=%ld\n",(UINT32)(PPgr[0]*1000),(UINT32)(PPgr[1]*1000),(UINT32)(PPgr[2]*1000));
#endif	
}


void 
lscTblFilter(
	UINT32 addr,
	UINT16 hSize,
	UINT16 vSize,
	UINT8 maskSize
)
{
	UINT32 tblAddr;
	UINT16 x,y,i,j;
	UINT16 xStart,xEnd,yStart,yEnd;
	UINT8 cnt;
	UINT16 idx;	
	UINT16 rSum,bSum,grSum,gbSum;
	//UINT8 gain0,gain1,gain2,gain3;
	UINT16 r_3ch,g_3ch,b_3ch;
	UINT8 xdata *pTbl;
	UINT32 Gain32b;
	UINT8* pGain = (UINT8*)&Gain32b;

	if (maskSize > 3)
		maskSize = 3;

	pTbl=(UINT8 xdata*)MEM_POOL_ADDR;	/* store old table */

	//printf("hSize=%d,vSize=%d\n",hSize,vSize);
	for (y=0;y<vSize;y++){
		
		/*****************************************/
		/* Get old gain table for mask operation */
		/*****************************************/
		//printf("%d tbl addr=0x%lx\n",y,tblAddr);
		yStart= CLampmin((SINT16)y-maskSize,0);
		yEnd= CLampMax(y+maskSize,vSize-1);
		yEnd++;
		//printf("ystart=%d,yend=%d\n",yStart,yEnd);

		lscTblFilterMaskDataGet(addr,pTbl,hSize,yStart,yEnd);
	
		tblAddr = addr+ (((UINT32)y * (UINT32)hSize)<<1);
		M_DRAM_SetStartAddressV(tblAddr, 0);	
		for(x=0;x<hSize;x++){
			xStart= CLampmin((SINT16)x-maskSize,0);
			xEnd= CLampMax(x+maskSize,hSize-1);
			xEnd++;
			cnt=0;
			
			gbSum=bSum=grSum=rSum=0;
			
			for(j=yStart;j<yEnd;j++){

				idx= (j-yStart)*(hSize<<2)+(xStart<<2);
				for(i=xStart;i<xEnd;i++){									

					if(channelMode==LSC_3CH){
						b_3ch=(UINT16)(*(pTbl+idx))+ ((UINT16)(*(pTbl+idx+1)&0x03))*256;
						g_3ch=(UINT16)((*(pTbl+idx+1))>>2) + ((UINT16)(*(pTbl+idx+2)&0x0f))*64;
						r_3ch=(UINT16)((*(pTbl+idx+2)&0xf0)>>4) + ((UINT16)(*(pTbl+idx+3)))*16;


						lscPrintf("%bx %bx %bx %bx\n",*(pTbl+idx),*(pTbl+idx+1),*(pTbl+idx+2),*(pTbl+idx+3)); 
						lscPrintf("%x %x %x \n",b_3ch,g_3ch,r_3ch); 


						//if(bSum<64512){	/* avoid overflow */
							bSum+= b_3ch;
						//}

						//if(grSum<64512){ /* avoid overflow */
							grSum+= g_3ch;
						//}
						
						//if(rSum<64512){  /* avoid overflow */
							rSum+= r_3ch;						
						//}
					}
					else{
						gbSum+= *(pTbl+idx);
						bSum += *(pTbl+idx+1);				
						grSum+= *(pTbl+idx+2);
						rSum += *(pTbl+idx+3);
					}
					//printf("idx=%d\n",idx);
					//printf("rSum=%d\n",rSum);
					idx=idx+4;					
					cnt++;
				}								
			}



			lscPrintf("x=%d,y=%d,cnt=%bd\n",x,y,cnt);
			lscPrintf("gbSum=%x\t",gbSum);
			lscPrintf("grSum=%x\t",grSum);
			lscPrintf("rSum=%x\t",rSum);
			lscPrintf("bSum=%x\n",bSum);

			
			if(channelMode==LSC_3CH){
				b_3ch=bSum/cnt;
				g_3ch=grSum/cnt;
				r_3ch=rSum/cnt;
	
				b_3ch=CLampMax(b_3ch,1023);
				g_3ch=CLampMax(g_3ch,1023);
				r_3ch=CLampMax(r_3ch,1023);

				Gain32b = ((UINT32)b_3ch&0x3ff) + (((UINT32)g_3ch&0x3ff)<<10) + (((UINT32)r_3ch&0x3ff)<<20);
			}
			else{
				gbSum=(gbSum/cnt);
				bSum=(bSum/cnt);
				grSum=(grSum/cnt);
				rSum=(rSum/cnt);			

				gbSum=CLampMax(gbSum,255);
				bSum=CLampMax(bSum,255);
				grSum=CLampMax(grSum,255);
				rSum=CLampMax(rSum,255);
				
				pGain[0]=(UINT8)gbSum;
				pGain[1]=(UINT8)bSum;
				pGain[2]=(UINT8)grSum;
				pGain[3]=(UINT8)rSum;	
			}
			
			//lscPrintf("gb=%bd\n",gb);			
			M_DRAM_WriteWord(pGain[3], pGain[2]);
			M_DRAM_WriteWord(pGain[1], pGain[0]);
		}
		
	}
	
}

void
lscTblFilterMaskDataGet(
	UINT32 addr,
	UINT8 xdata* pTbl, 
	UINT16 hSize,
	UINT16 yStart,
	UINT16 yEnd
)
{
	UINT32 tblAddr;
	UINT16 x,y,idx;
	
	tblAddr = addr+ (((UINT32)yStart * (UINT32)hSize)<<1);
	M_DRAM_SetStartAddressV(tblAddr, 1);
	
	idx=0;
	for(y=yStart;y<yEnd;y++){
		for(x=0;x<hSize;x++){
			M_DRAM_ReadWord(*(pTbl+idx),*(pTbl+idx+1)); 			
			M_DRAM_ReadWord(*(pTbl+idx+2),*(pTbl+idx+3));
			//printf("tbl[%d]=0x%bx\n",idx,*(pTbl+idx));
			//printf("tbl[%d]=0x%bx\n",idx+1,*(pTbl+idx+1));
			//printf("tbl[%d]=0x%bx\n",idx+2,*(pTbl+idx+2));
			//printf("tbl[%d]=0x%bx\n",idx+3,*(pTbl+idx+3));				
			idx+=4;
		}
	}
	
}

UINT8
sp1kLscCal(
	void
)
{
	//UINT8 aeGId;
	#if 0
		//UINT8 fileName[12]="OV565300.RAW";
		//UINT8 fileName[12]="OV971200.RAW";
		//UINT8 fileName[12]="MI160000.RA2";
		UINT8 fileName[12]="MI5131E0.RA2";
		
		sp1kSnapParamSet(SP1K_SNAP_PARAM_CHANGERAW, 1 );  
		chgRawFileNameSet(fileName);
		chgRawSizeSet( (UINT16)2592, (UINT16)1944 );
		//chgRawSizeSet( (UINT16)1280, (UINT16)720 );
		//chgRawSizeSet( (UINT16)1440, (UINT16)1080 );
	#else		
		sp1kCalibrationAEWinConfigSet(LUMA_CAL_LSC); /* change AE mode and set luma target */

		//sp1kCalibrationObSet(1); /* turn on OB */
	#endif
	
	sp1kSnapParamSet(SP1K_SNAP_PARAM_10_BIT_RAW_OPT,0);
	sp1kSnapParamSet(SP1K_SNAP_PARAM_DEBUG_RAW_ONLY,0);
	
	sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, 0 );
    sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_YUV_OPT, 0 );
    sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_JPG_OPT, 1 );

	sp1kSnapParamSet(SP1K_SNAP_PARAM_LSCCAL,1);
		
	lscCalInit();
	
	//sp1kLscCfgSet(SP1K_CALIB_LSC_STEP_X,16);
	//sp1kLscCfgSet(SP1K_CALIB_LSC_STEP_Y,16);
	//sp1kLscCfgSet(SP1K_CALIB_LSC_LUMA_LEVEL,100);
	//sp1kLscCfgSet(SP1K_CALIB_LSC_CHANNEL,LSC_3CH);
	//sp1kLscCfgSet(SP1K_CALIB_LSC_RESOLUTION,8);
	
	while(sp1kAeReadyGet(1)!=SUCCESS);/*wait ae ready for snap */

	sp1kCalibrationAESet(0);  /* turn off AE */

	frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 4);
	sp1kCalibrationObSet(1); /* turn on OB */

	//sp1kAeStatusGet(SP1K_AE_gId,&aeGId);
	//printf("AE Gid=%bx\n",aeGId);
	
	appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);

	if(sp1kSnapTask()){
		return FAIL;
	}

	sp1kCalibrationAESet(1);  /* turn on AE */	
	sp1kNandRsvWrite(SP1K_RSC_ID_LSC, K_SDRAM_LSCTableBufAddr<<1);

	return SUCCESS;
}

#endif


#ifdef MI_LSC_CAL_OPTION

void appMILSCCalSet(void)
{
	uiPara_t* puiPara = appUiParaGet();
	//frontLscOff();
	sp1kCalibrationObSet(0);
	sp1kCalibrationLscSet(0); 
	sp1kCalibrationGammaSet(0);
	sp1kCalibrationAWBSet(0);
	sp1kAeParamSet(SP1K_AE_TARGET_LUMA, MI_CAL_TAG_LUMA);
	sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
	sp1kCalibrationAESet(1);					

	osMsgFlush(MSG_CTRL_FLUSH_ALL);
	osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
	puiPara->USBMode = 1;	//USB_PCCAM
	sp1kHalUSBSwPlugIn();	
}
void appSetMILSCCalSts(UINT8 enable)
{
	lensShadingByFront = enable;
}
UINT8 appGetMILSCCalSts(void)
{
	return lensShadingByFront;
}

#endif

/**************************************/
/*  unused functions                  */
/**************************************/
#if 0
void 
LSC2DTBL(
	UINT8 *rgbRawAddr,	
	UINT8 sampleMethod,
	int imgWidth,
	int imgHeight,
    UINT32 **HWTBL,	
    UINT8 FalloffPer,
    UINT32 Maxval,
    UINT16 stepx,
    UINT16 stepy,
    UINT16 Wnum,
    UINT16 Hnum
)
{
	int Hpeak,Vpeak,DifLen,SamStp,MaxLen;
	int DifLen1,SamPts,Xdif,Ydif,Refidx,m,n;
  	int ii=0,jj=0,xstart,xend,ystart,yend;

	UINT16 i,j;
	UINT16 iW,iH;
	
  	#if 1
  		float E[3],F[3],PPgr[3];
  	#else /* 2010/10/25 */
  		float E[3],F[3],PPgr[3];
   	#endif
  
	UINT8 StpFac1,StpFac2;
  	UINT16 xpos, ypos;
  	UINT32 MSum,pixsum;
  	UINT8 tempSum;

  	Hpeak=imgWidth>>1;
  	Vpeak=imgHeight>>1;
  	iW=imgWidth;
  	iH=imgHeight;
  
  	if (FalloffPer==100){

		lscPrintf("FalloffPer=%d \n",FalloffPer);
		
		for (j=0;j<(iH>>1);j+=stepy){ 
     		ii=0;  
	  		for (i=0;i<(iW>>1);i+=stepx){ 
       			/*xpos=i<<1;
	   			ypos=j<<1;
	   			subSamplePosGet(xpos, ypos, sampleMethod);
	   			tempSum = lscRawPixRead(rgbRawAddr, iW, iH, xpos, ypos);*/
				   			
				MSum=0; 
				pixsum=0;
				xstart= CLampmin(i-Maskradius,0);
				xend= CLampMax(i+Maskradius,iW>>1);
				ystart= CLampmin(j-Maskradius,0);
				yend= CLampMax(j+Maskradius,iH>>1);
	 
	  			for (n=ystart;n<yend;n++){
	   				for (m=xstart;m<xend;m++){
          				xpos=m<<1;
          				ypos=n<<1;
		  				subSamplePosGet(&xpos, &ypos, sampleMethod);
		  				tempSum = lscRawPixRead(rgbRawAddr, iW, iH, xpos, ypos);
		  				MSum+=tempSum;
		  				pixsum++;
					}
      			} 
     
 	  			if (!pixsum){
					return; //SP5K_ERR_CALIBRATION_CFG;
				}
 	  
          		MSum=MSum/pixsum;
          
	  			if (!MSum){
	  				return; //SP5K_ERR_CALIBRATION_CFG;
	  			}	
	  
	 		 	HWTBL[jj][ii]=(Maxval<<gainRes)/(MSum);
	 	 		HWTBL[jj][ii]=CLampMax(HWTBL[jj][ii],255);

	  			lscPrintf("Maxval=%d,tempSum=%d,F=%d \n",Maxval,MSum,HWTBL[jj][ii]);

	  			ii=ii+1;
      		}
	  		jj=jj+1;
  	 	}


   
     	for (i=0;i<Hnum;i++){
	 		HWTBL[i][ii]=HWTBL[i][ii-1];
	 	}
   
     	for (i=0;i<Wnum;i++){
	 		HWTBL[jj][i]=HWTBL[jj-1][i];   
	 	}
	}
  	else{ 

    	lscPrintf("FalloffPer=%d \n",FalloffPer);
    	
    	DifLen=Hpeak*Hpeak+Vpeak*Vpeak;
    	SamStp=1<<((int)(log(DifLen/256)/log(2))+1);
    	
    	if (Hpeak>Vpeak){
	 		MaxLen=Hpeak;
	 	}
    	else{
	 		MaxLen=Vpeak;
	 	}

    	StpFac1=(int)(log(MaxLen)/log(2))+1-8;
    	DifLen1=((Hpeak)>>StpFac1)*((Hpeak)>>StpFac1)+((Vpeak)>>StpFac1)*((Vpeak)>>StpFac1);
    	StpFac2= (int)(log(DifLen1/256)/log(2))+1;
    	SamPts=(int)(DifLen/SamStp)+1;
    	E[0]=0;E[1]=SamPts>>1;E[2]=SamPts-1;
    	F[0]=1;
    	F[1]=((((100-(float)FalloffPer)*75/100+(float)FalloffPer)))/100;
    	F[2]=(float)FalloffPer/100;

    	lscPrintf("E1=%d,E2=%d,E3=%d,F1=%d,F2=%d,F3=%d \n",(int)(E[0]*100),(int)(E[1]*100),(int)(E[2]*100),(int)(F[0]*100),(int)(F[1]*100),(int)(F[2]*100));

    	lsEq(E,F,3,PPgr);


    	for (i=0;i<SamPts;i++){ 
   			SamTarCurve[i]=(UINT32)((PPgr[0]*i*i+PPgr[1]*i+PPgr[2])*((float)Maxval));
      		lscPrintf("SamTarCurve[%d]=%d \n",i,(int)((PPgr[0]*i*i+PPgr[1]*i+PPgr[2])*1000))	;
    	}
    
   		for (i=SamPts;i<256;i++){
     		SamTarCurve[i]=SamTarCurve[SamPts-1];
     	}
	
    	for (j=0;j<(iH>>1);j+=stepy){ 
    		ii=0;
	  		for (i=0;i<(iW>>1);i+=stepx){ 
		    	/*xpos=i<<1;
				ypos=j<<1;
				subSamplePosGet(xpos, ypos, sampleMethod);
				tempSum = lscRawPixRead(rgbRawAddr, iW, iH, xpos, ypos);*/

	  			MSum=0; 
	  			pixsum=0;
	  			xstart= CLampmin(i-Maskradius,0);
	  			xend= CLampMax(i+Maskradius,iW>>1);
	  			ystart= CLampmin(j-Maskradius,0);
	  			yend= CLampMax(j+Maskradius,iH>>1);
	  
	  			for (n=ystart;n<yend;n++){
	   				for (m=xstart;m<xend;m++){
          				xpos=m<<1;
          				ypos=n<<1;
		  				subSamplePosGet(&xpos, &ypos, sampleMethod);
		  				tempSum = lscRawPixRead(rgbRawAddr, iW, iH, xpos, ypos);
		  				MSum+=tempSum;
		  				pixsum++;
					}
				}	

		  		lscPrintf("Msum=%d,pixsum=%d \n",MSum,pixsum);
	  
      			if (!pixsum) return; //SP5K_ERR_CALIBRATION_CFG;

     			MSum=MSum/pixsum;
     			
     			if (!MSum) return; //SP5K_ERR_CALIBRATION_CFG;
	  
      			Xdif=abs((i<<1)-Hpeak)>>StpFac1;
	  			Ydif=abs((j<<1)-Vpeak)>>StpFac1;
	  			Refidx=(Xdif*Xdif+Ydif*Ydif)>>StpFac2;

		  		lscPrintf("i=%d,j=%d,Xdif=%d,Ydif=%d,Refidx=%d \n",i,j,Xdif,Ydif,Refidx);	  
				
      			HWTBL[jj][ii]=(SamTarCurve[Refidx]<<gainRes)/(MSum);

	  			lscPrintf("Refidx=%d SamTarCurve=%d,HWTBL=%d \n",Refidx,SamTarCurve[Refidx],HWTBL[jj][ii]);
	  			
	  			HWTBL[jj][ii]=CLampMax(HWTBL[jj][ii],255);
	  			ii=ii+1;
      		}
	  		jj=jj+1;
  		}
	}
 	
    lscPrintf("iW/2=%d,iH/2=%d,ii=%d,jj=%d ,i=%d,j=%d\n",iW>>1,iH>>1,ii,jj,i,j);
    
    for (i=0;i<Hnum;i++){
	 	HWTBL[i][ii]=HWTBL[i][ii-1];
	}
   
    for (i=0;i<Wnum;i++){
		HWTBL[jj][i]=HWTBL[jj-1][i];
	}

   	/*    Tblmin=HWTBL[0][0];
	for(j=0;j<Hnum;j++)
	  for(i=0;i<Wnum;i++)
	     if (HWTBL[j][i]<Tblmin)
		 Tblmin =  HWTBL[j][i];	
    for(j=0;j<Hnum;j++)
	  for(i=0;i<Wnum;i++) 
	    HWTBL[j][i]=(HWTBL[j][i]<<gainRes)/Tblmin;*/
     	  	
	 
}

/*******************************************************************************
 *
 *  Function Name: addrAlignedTo16
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 ******************************************************************************/
static UINT8 *
addrAlignedTo16(
	UINT8 *psrc
)
{
	UINT32	tmpaddr;

	tmpaddr = (UINT32) psrc;
	tmpaddr = (tmpaddr + 15) >> 4 << 4;

	return ((UINT8 *) tmpaddr);
}


void 
lscTblGen (
	UINT32 rawAddr,	
	UINT16 imgWidth,
	UINT16 imgHeight,
	UINT16 xx, 
	UINT16 yy,
	UINT8 sampleMethod,
	UINT16 *Gain,	
	UINT8 FalloffPer,
	UINT8 Maxval
)
{

	UINT16 Hpeak,Vpeak,SamStp,MaxLen;
	float DifLen,DifLen1;
  	UINT16 SamPts,Xdif,Ydif,Refidx;
  	UINT16 i;
  	UINT16 m,n;
  	UINT16 xstart,xend,ystart,yend;
  	float E[3],F[3],PPgr[3];  
  	UINT8 StpFac1,StpFac2;
  	UINT16 xpos, ypos;
  	UINT16 MSum,pixsum;
  	UINT16 tempSum;

  	UINT32 SamTarCurve[256];
  	
  	Hpeak=imgWidth>>1;
  	Vpeak=imgHeight>>1;

  	lscPrintf("imgWidth/2=%d \n",imgWidth>>1);
  	lscPrintf("imgHeight/2=%d \n",imgHeight>>1); 
  	lscPrintf("xx=%d\n",xx);		
  	lscPrintf("yy=%d\n",yy);

  	if (FalloffPer==100){
    	    
        MSum=0; 
	  	pixsum=0;
	  	xstart= CLampmin((SINT16)xx-Maskradius,0);
	  	xend= CLampMax(xx+Maskradius,(imgWidth>>1));
	  	ystart= CLampmin((SINT16)yy-Maskradius,0);
	  	yend= CLampMax(yy+Maskradius,(imgHeight>>1));

		//if ((xx<320)&&(yy==0)){
        	lscPrintf("xstart=%d\n",xstart);
        	lscPrintf("xend=%d\n",xend);		
        	lscPrintf("ystart=%d\n",ystart);
        	lscPrintf("yend=%d\n",yend);		
        //}

	 
	  	for (n=ystart;n<yend;n++){
	   		for (m=xstart;m<xend;m++){
                xpos=m<<1;
                ypos=n<<1;            				
		  		subSamplePosGet(&xpos, &ypos, sampleMethod);		  
		  		tempSum = lscRawPixRead(rawAddr, imgWidth, imgHeight, xpos, ypos);
		  		MSum+=tempSum;
		  		pixsum++;
			}
	  	}  

 	  	if (pixsum==0){
 	  		return; //SP5K_ERR_CALIBRATION_CFG;
 	  	}


		//if ((xx<320)&&(yy==0)){
        	lscPrintf("MSum=%d\n",MSum);  
        	lscPrintf("Maxval=%bd\n",Maxval);		 
        	//lscPrintf("Gain=%ld\n",*Gain);
        //}  		

        MSum=MSum/pixsum;
        
	  	if (MSum==0){
	  		return; //SP5K_ERR_CALIBRATION_CFG;
	  	}

                  	  
	  	*Gain=(Maxval<<gainRes)/(MSum);

	  	
	  	*Gain=CLampMax(*Gain,255);
	  	//printf_4("Gain=%d\n",*Gain);
	  
	}	  
	else{

    	DifLen=Hpeak*Hpeak+Vpeak*Vpeak;
    	SamStp=1<<((int)(log(DifLen/256)/log(2))+1);
    	
    	if (Hpeak>Vpeak){
	 		MaxLen=Hpeak;
		}
    	else{
	 		MaxLen=Vpeak;
	 	}

    	StpFac1=(UINT16)(log(MaxLen)/log(2))+1-8;
    	DifLen1=((Hpeak)>>StpFac1)*((Hpeak)>>StpFac1)+((Vpeak)>>StpFac1)*((Vpeak)>>StpFac1);
    	StpFac2= (UINT16)(log(DifLen1/256)/log(2))+1;
    	SamPts=(UINT16)(DifLen/SamStp)+1;
    	E[0]=0;E[1]=SamPts>>1;E[2]=SamPts-1;
    	F[0]=1;
    	F[1]=((((100-(float)FalloffPer)*75/100+(float)FalloffPer)))/100;
    	F[2]=(float)FalloffPer/100;

    	lscPrintf("E1=%d,E2=%d,E3=%d,F1=%d,F2=%d,F3=%d \n",(int)(E[0]*100),(int)(E[1]*100),(int)(E[2]*100),(int)(F[0]*100),(int)(F[1]*100),(int)(F[2]*100));
    	
    	lsEq(E,F,3,PPgr);
	
     	for (i=0;i<SamPts;i++){ 
     		SamTarCurve[i]=(UINT32)((PPgr[0]*i*i+PPgr[1]*i+PPgr[2])*((float)Maxval));
   			lscPrintf("SamTarCurve[%d]=%ld \n",i,(int)((PPgr[0]*i*i+PPgr[1]*i+PPgr[2])*1000));
    	}

     	for (i=SamPts;i<256;i++){
      		SamTarCurve[i]=SamTarCurve[SamPts-1];
      	}

        MSum=0; 
	  	pixsum=0;
	  	xstart= CLampmin(xx-Maskradius,0);
	  	xend= CLampMax(xx+Maskradius,(imgWidth>>1));
	  	ystart= CLampmin(yy-Maskradius,0);
	  	yend= CLampMax(yy+Maskradius,(imgHeight>>1));
	  
	  	for (n=ystart;n<yend;n++){
	   		for (m=xstart;m<xend;m++){
          		xpos=m<<1;
          		ypos=n<<1;
		  		subSamplePosGet(&xpos, &ypos, sampleMethod);
		  		tempSum = lscRawPixRead(rawAddr, imgWidth, imgHeight, xpos, ypos);
		  		MSum+=tempSum;
		  		pixsum++;
			}
	  	}
		  
        if (!pixsum){
        	return; //S5K_ERR_CALIBRATION_CFG;
        }
        MSum=MSum/pixsum;
        if (!MSum){
        	return; //SP5K_ERR_CALIBRATION_CFG;
        }
	  
        Xdif=abs((xx<<1)-Hpeak)>>StpFac1;
	  	Ydif=abs((yy<<1)-Vpeak)>>StpFac1;
	  	Refidx=(Xdif*Xdif+Ydif*Ydif)>>StpFac2;	  
        *Gain=(SamTarCurve[Refidx]<<gainRes)/(MSum);	 
	  	*Gain=CLampMax(*Gain,255);
	}

}

#endif








