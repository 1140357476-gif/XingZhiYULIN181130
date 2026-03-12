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
 * @file      cdsp_misc.c
 * @brief     The cdsp misc function
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "asicreg.h"
#include "initio.h"
#include "sp1k_calibration_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_hal_api.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h"
#include "sp1k_cdsp_api.h"
#include "Cdsp_misc.h"
#include "Doslink.h"
#include "utility.h"
#include "cdsp_debug.h"
#include "cdsp_init.h"
#include "snap_core.h"
#include "cdsp_task.h"
#include "reg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* debug. */
#define __FILE	__FILE_ID_LIB_CDSP_MISC__

#define CDSP_MISC_NEW_FILE
#define CDSP_REMOVE_UNCALL_FUNCTION

#define CDSP_MISC_TAG_TODO
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 cMatrix[18];
UINT8 cdspIdHdr[ 6 ] = { 0x31, 0x35, 0x32, 0x38, 0x00, 0x01 };
UINT8 xdata dynamicIqTbl[256] _at_ 0xbe00;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#ifndef CDSP_MISC_TAG_TODO
extern xdata UINT8 calibrationObData[ SP1K_CALIBRATION_OB_MAX ];
extern xdata UINT8 calibrationAwbData[ SP1K_CALIBRATION_AWB_MAX ];
extern xdata UINT32 calibrationCriteriaData[ SP1K_CALIBRATION_CRITERIA_MAX ];
#endif

extern SysDynamicEnhan SysDynamicValue;

/**************************************************************************
 *               S T A T I C   L O C A L   F U N C T I O N                *
 **************************************************************************/
static void cdspColorMatrixBackup(void);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**
 * @fn        void cdspBinRead(UINT8 *f, UINT32 startAddr)
 * @brief     read cdsp bin file
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] startAddr : dram address for cdsp parameters
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
cdspBinRead(
	UINT8 *f,
	UINT32 startAddr
)
{
    UINT32 filesize;
	UINT16 i;
	UINT16 datalength,regaddr;
	UINT8  RLSB,RMSB,regdata,datamask;
	UINT8  temp,sts,value;

	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load cdsp para err\n");
		}
		else{
			cdspPrintfLoadRes("load cdsp para suc\n");
		}

	}

    #if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
   		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
	#endif

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);

	M_DRAM_ReadWord(RLSB, RMSB);
	datalength = (UINT16)(RMSB&0xff)<<8|(RLSB&0xff);

	cdspPrintfLoadRes("cdsp para datalength=%d\n",datalength);

	datalength= datalength/4;

	ENTER_CRITICAL(sts);

	for ( i=0 ; i< datalength ;i++ ) {
		M_DRAM_ReadWord(RLSB, RMSB);
		regaddr = (UINT16)((RMSB&0xff)<<8| RLSB );

		M_DRAM_ReadWord(RLSB, RMSB);
		regdata = RLSB;
		datamask= RMSB;

        //cdspPrintfLoadRes("REG[0x%04x]=0x%b02x\n",regaddr,regdata);
		temp = (XBYTE[regaddr]&datamask)|(regdata&(~(datamask)));
		//cdspPrintfLoadRes("REG[0x%04x]=0x%b02x\n",regaddr,temp);
		XBYTE[regaddr] = temp;
		value = XBYTE[regaddr];
		//cdspPrintfLoadRes("REG[0x%04x]=0x%b02x\n",regaddr,value);
	}


	//XBYTE[0x21d0] &= ~0x02;
	//cdspPrintfLoadRes("XBYTE[0x2121]=0x%bx\n",XBYTE[0x2121]);

	#ifdef COMPARE_ANSWER	/* will: set immediate update , for FPGA compare answer*/
		XBYTE[REG_Cdsp_wboffseten] &= 0xef;
		XBYTE[REG_Cdsp_AFwinen] &= 0xef;
		XBYTE[REG_Cdsp_AWBwinen] &= 0xcf;
		XBYTE[REG_Cdsp_AEwinen] &= 0xcf;
		XBYTE[REG_Cdsp_Edgewinen] &= 0xef;
		XBYTE[REG_Cdsp_3Dtden] &= 0xcf;
		XBYTE[REG_Cdsp_WDRen] &= 0xcf;
	#endif

	//XBYTE[REG_Cdsp_Lutgammaen] |= 0x10;   /* gamma inverse */
    EXIT_CRITICAL(sts);

	cdspColorMatrixBackup();

}

/**
 * @fn        void hueBinRead(UINT8 *f, UINT32 startAddr)
 * @brief     read hue bin file
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] channel : channel of hue (x = 0,y = 1,z = 2)
 * @param     [in] startAddr : dram address for hue parameters
 * @retval    NONE
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
hueBinRead(
	UINT8 *f,
	UINT8 channel,
	UINT32 startAddr
)
{
    UINT32 filesize;
	UINT16 i;
	UINT16 datalength,regaddr;
	UINT8  RLSB,RMSB,regdata,datamask;
	UINT8  temp,sts,value;
	UINT16 reg[3]={REG_Cdsp_hcangx_low, REG_Cdsp_hcangy_low, REG_Cdsp_ythrz};

	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load hue para err\n");
		}
		else{
			cdspPrintfLoadRes("load hue para suc\n");
		}

	}

    #if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
   		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
	#endif

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);

	M_DRAM_ReadWord(RLSB, RMSB);
	datalength = (UINT16)(RMSB&0xff)<<8|(RLSB&0xff);

	cdspPrintfLoadRes("hue para datalength=%d\n",datalength);

	datalength= datalength/4;

	ENTER_CRITICAL(sts);

	for ( i=0 ; i< datalength ;i++ ) {
		M_DRAM_ReadWord(RLSB, RMSB);
		regaddr = (UINT16)((RMSB&0xff)<<8| RLSB );

		M_DRAM_ReadWord(RLSB, RMSB);
		regdata = RLSB;
		datamask= RMSB;

		if (i == 0) {
			ASSERT((reg[channel] == regaddr),1);
		}

        //cdspPrintfLoadRes("REG[0x%04x]=0x%b02x\n",regaddr,regdata);
		temp = (XBYTE[regaddr]&datamask)|(regdata&(~(datamask)));
		//cdspPrintfLoadRes("REG[0x%04x]=0x%b02x\n",regaddr,temp);
		XBYTE[regaddr] = temp;
		value = XBYTE[regaddr];
		//cdspPrintfLoadRes("REG[0x%04x]=0x%b02x\n",regaddr,value);
	}

    EXIT_CRITICAL(sts);

}


/**
 * @fn        void lscBinRead(UINT8 *f, UINT32 startAddr)
 * @brief     lscBinRead
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] startAddr : dram address for saving lsc table
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
lscBinRead(
	UINT8 *f,
	UINT32 startAddr
)
{

    UINT32 filesize;
    UINT8 lsb,msb;

	//UINT16 i;

	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;
		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load LSC table error\n");
		}
		else{
			cdspPrintfLoadRes("load LSC table success\n");
		}
	}

	#ifdef COMPARE_ANSWER
		lsb=lsb;
		msb=msb;
		HAL_CdspLscWinStepSet(2,0);
		HAL_CdspLscWinIncStepSet(1,1,1,1);  /* increasing step=1 for capture */
		HAL_CdspLscWinSizeSet(42);
		HAL_CdspLscWinOffsetSet(0,0);
		HAL_CdspLscWinAddrset(startAddr+12);

	#else
		M_DRAM_SetStartAddress((UINT32)(startAddr+4), 1);

		M_DRAM_ReadWord(lsb, msb);
		HAL_CdspLscWinStepSet((lsb&0x0f),(lsb&0xf0)>>4);    //capture
		HAL_CdspLscResolution(msb);

		M_DRAM_ReadWord(lsb, msb);
		HAL_CdspLscWinSizeSet(lsb);

		M_DRAM_ReadWord(lsb, msb);
		HAL_CdspLscModeSet(msb&0x01);

		HAL_CdspLscWinOffsetSet(0,0);
		//HAL_CdspLscModeSet(1);  /* 4 channel mode	*/

		#if 0
			M_DRAM_SetStartAddress((UINT32)(startAddr+0x284f), 1);
			for (i=0;i<52;i=i+2){
				M_DRAM_ReadWord(lsb, msb);
				printf("lsb[%d]=0x%bx,0x%bx\n",i,lsb,msb);
			}
		#endif

		#if 0    /* write test value */
			i=0;
			//M_DRAM_SetStartAddress((UINT32)(startAddr+12), 0);
			for (i=0;i<10332;i=i+2){
				M_DRAM_SetStartAddress((startAddr+(UINT32)12+(UINT32)i), 1);

				M_DRAM_ReadWord(lsb,msb);
				lsb=0;
				msb=0;
				//printf("lsb=%bx,msb=%bx\n",lsb,msb);
				//printf("2764~REG_Dram_IndirectAddr_B3=%bx %bx %bx %bx\n",XBYTE[REG_Dram_IndirectAddr_B0],XBYTE[REG_Dram_IndirectAddr_B1],XBYTE[REG_Dram_IndirectAddr_B2],XBYTE[REG_Dram_IndirectAddr_B3]);
				M_DRAM_SetStartAddress((startAddr+(UINT32)12+(UINT32)i), 1);
				M_DRAM_WriteWord(lsb,msb);

				M_DRAM_SetStartAddress((startAddr+(UINT32)12+(UINT32)i), 1);
				M_DRAM_ReadWord(lsb,msb);
				lsb=0;
				//msb=0;
				M_DRAM_SetStartAddress((startAddr+(UINT32)12+(UINT32)i), 1);
				M_DRAM_WriteWord(lsb,msb);

				//M_DRAM_WriteWord(0x30, 0x30);
				//M_DRAM_WriteWord(0x30, 0x30);
			}
		#endif
		HAL_CdspLscWinAddrset(startAddr+12);
		//HAL_CdspLscEnSet( 1 );

	#endif

	//printf("LSC add=0x%lx\n",startAddr+12);
}


/**
 * @fn        void gammaBinRead(UINT8 *f, UINT32 startAddr)
 * @brief     gammaBinRead
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] startAddr : dram address for saving gamma table
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
gammaBinRead(
	UINT8 *f,
	UINT32 startAddr
)
{

    UINT32 filesize;
	UINT16 i;
	UINT8  LSB,MSB;

	#if defined(COMPARE_ANSWER) //|| defined(ICAT_OPTION)
		UINT8 GammaTbl[1024];
	#else
		UINT8 GammaTbl[384];
	#endif


	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load Gamma table error\n");
		}
		else{
			cdspPrintfLoadRes("load Gamma table success\n");
		}
	}


	//M_DRAM_SetStartAddress((UINT32)(startAddr), 1);
	#if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
   		M_DRAM_ReadWord(LSB, MSB);
		printf("gamma para LSB=%bx,MSB=%bx\n",LSB,MSB);
		M_DRAM_ReadWord(LSB, MSB);
		printf("gamma para LSB=%bx,MSB=%bx\n",LSB,MSB);
		M_DRAM_ReadWord(LSB, MSB);
		printf("gamma para LSB=%bx,MSB=%bx\n",LSB,MSB);
		M_DRAM_ReadWord(LSB, MSB);
		printf("gamma para LSB=%bx,MSB=%bx\n",LSB,MSB);
	#endif




	#if defined(COMPARE_ANSWER) //|| defined(ICAT_OPTION)
		M_DRAM_SetStartAddress((UINT32)(startAddr), 1);
		for (i=0;i<1024 ;i=i+2) {

			M_DRAM_ReadWord(LSB, MSB);

			GammaTbl[ i   ] = LSB;
			GammaTbl[ i+1 ]	= MSB;
		}
	#else
		M_DRAM_SetStartAddress((UINT32)(startAddr+4), 1);
		for (i=0;i<384 ;i=i+3) {

			M_DRAM_ReadWord(LSB, MSB);

			GammaTbl[ i   ] = LSB;
			GammaTbl[ i+1 ] = MSB;

			M_DRAM_ReadWord(LSB, MSB);

			GammaTbl[ i+2 ] = LSB;
		}
	#endif

	HAL_CdspGammalTbInit(GammaTbl);
}

/**
 * @fn        void wdrBinRead(UINT8 *f, UINT32 startAddr)
 * @brief     wdrBinRead
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] startAddr : dram address for saving wdr table
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void wdrBinRead(
	UINT8 *f,
	UINT32 startAddr
)
{
    UINT32 size;
	UINT16 i;
	UINT8 LSB,MSB;
    UINT8 WDRTbl[256];




	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f,startAddr ,&size)){
			cdspPrintfLoadRes("load WDR table error\n");
		}
		else{
			cdspPrintfLoadRes("load WDR table success\n");

		}
	}

	#if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr+4, 1);
		M_DRAM_ReadWord(LSB, MSB);
		printf("wdr para LSB=%bx,MSB=%bx\n",LSB,MSB);
		M_DRAM_ReadWord(LSB, MSB);
		printf("wdr para LSB=%bx,MSB=%bx\n",LSB,MSB);
		M_DRAM_ReadWord(LSB, MSB);
		printf("wdr para LSB=%bx,MSB=%bx\n",LSB,MSB);
		M_DRAM_ReadWord(LSB, MSB);
		printf("wdr para LSB=%bx,MSB=%bx\n",LSB,MSB);
	#endif

	#ifdef COMPARE_ANSWER
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
	#else
		M_DRAM_SetStartAddress((UINT32)startAddr+4, 1);
	#endif

	for ( i=0 ; i< 256 ;i=i+2 ) {
		M_DRAM_ReadWord(LSB, MSB);
        WDRTbl[ i   ] = LSB;
		WDRTbl[ i+1 ] = MSB;
	}

	HAL_CdspWDRTblnit(WDRTbl);

}

/**
 * @fn        void cdspHueBinSet(UINT8 fileId, UINT8 channel)
 * @brief     cdsp hue set (load tables)
 * @param     [in] fileId
 * @param     [in] channel
 * @retval    NONE
 * @see       NULL
 * @author   LinJieCheng
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
cdspHueBinSet(
	UINT8 fileId,
	UINT8 channel
)
{
	UINT8 sts;

	#ifdef COMPARE_ANSWER
		sts=sts;
		fileId=fileId;
	#else
		do {
			sts = sp1kNandRsvRead(fileId, (K_SDRAM_CommonEndAddr<< 1));
		}while(!sts);

		hueBinRead( "", channel, K_SDRAM_CommonEndAddr);

	#endif

}


/**
 * @fn        void wdrBinRead(UINT8 *f, UINT32 startAddr)
 * @brief     wdrBinRead
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] startAddr : dram address for saving wdr table
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void pvCdspBinRead(
	UINT8 *f,
	UINT32 startAddr
)
{
    UINT32 filesize;
	UINT16 i;
	UINT16 datalength,regAddr,idx;
	UINT8  lsb,msb;
	UINT8  sts;
	UINT8  reg[18];

	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load pv cdsp para err\n");
		}
		else{
			cdspPrintfLoadRes("load pv cdsp para suc\n");

		}

	}

	#if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
   		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
	#endif

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);
	M_DRAM_ReadWord(lsb, msb);
	datalength = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.pvMode.weakThr = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.pvMode.middleThr = (UINT16)(msb&0xff)<<8|(lsb);

	cdspPrintfLoadRes("pv cdsp para datalength=%d\n",datalength);
	cdspPrintfLoadRes("weakThr=%d, middleThr=%d\n",SysDynamicValue.pvMode.weakThr,SysDynamicValue.pvMode.middleThr);

	datalength= datalength/4;
	datalength = datalength/3;  /*only save one set of parameters*/

	for (i=0;i<18;i++){
		reg[i]=0;
	}

	ENTER_CRITICAL(sts);

	for ( i=0 ; i< datalength ;i++ ) {

		M_DRAM_ReadWord(lsb, msb);
		regAddr = (UINT16)((msb&0xff)<<8| lsb );
		M_DRAM_ReadWord(lsb, msb);

		if (regAddr<0x21d0){
			idx= regAddr-0x216c+16;
		}
		else{
			idx= regAddr-0x21d0;
		}

		if (idx<18){
			reg[idx] = lsb;
		}

		cdspPrintfLoadRes("R[0x%x]=0x%b02x\n",regAddr,lsb);

	}

	cdspPvEdgeParaSet(reg[2],reg[3],reg[4],reg[5],reg[6],reg[7],reg[8]);
	cdspPvEdgeFilterSet(reg[9],reg[14],reg[13],reg[12],reg[11],reg[10]);
	cdspPvEdgeFilterEnSet(reg[0]&0x0f);

	cdspPvIntLowThrSet(reg[16]);
	cdspPvMedianModeSet(reg[15]);

	SysDynamicValue.pvMode.currEnhanMode = CDSP_DYNAMIC_WEAK;

	//XBYTE[ REG_Cdsp_intpllowthr ]=reg[16];

    EXIT_CRITICAL(sts);

}

/**
 * @fn        void pvCdspDynamicBinRead(UINT8 *f, UINT8 intensity, UINT32 startAddr)
 * @brief     pvCdspDynamicBinRead
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] intensity : weak, middle, strong
 * @param     [in] startAddr : dram address for saving wdr table
 * @retval    NONE
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-27
 * @todo      N/A
 * @bug       N/A
*/
void pvCdspDynamicBinRead(
	UINT8 *f,
	UINT8 intensity,
	UINT32 startAddr
)
{
    UINT32 filesize;
	UINT16 i;
	UINT16 datalength,regAddr,idx;
	UINT8  lsb,msb;
	UINT8  sts;
	UINT8  reg[18];

	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load pv cdsp para err\n");
		}
		else{
			cdspPrintfLoadRes("load pv cdsp para suc\n");
		}

	}

	#if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
   		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
	#endif

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);
	M_DRAM_ReadWord(lsb, msb);
	datalength = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.pvMode.weakThr = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.pvMode.middleThr = (UINT16)(msb&0xff)<<8|(lsb);

	cdspPrintfLoadRes("pv cdsp para datalength=%d\n",datalength);
	cdspPrintfLoadRes("weakThr=%d, middleThr=%d\n",SysDynamicValue.pvMode.weakThr,SysDynamicValue.pvMode.middleThr);

	datalength= datalength/4;	//address + data = 4bytes
	datalength = datalength/3;	//number of level = 3

	/* offset set of parameters */
	M_DRAM_SetStartAddress((UINT32)(startAddr+6+(intensity*datalength*2)), 1);

	for (i=0;i<18;i++){
		reg[i]=0;
	}

	ENTER_CRITICAL(sts);

	for ( i=0 ; i< datalength ;i++ ) {

		M_DRAM_ReadWord(lsb, msb);
		regAddr = (UINT16)((msb&0xff)<<8| lsb );
		M_DRAM_ReadWord(lsb, msb);

		if (regAddr<0x21d0){
			idx= regAddr-0x216c+16;
		}
		else{
			idx= regAddr-0x21d0;
		}

		if (idx<18){
			reg[idx] = lsb;
		}

		cdspPrintfLoadRes("R[0x%x]=0x%b02x\n",regAddr,lsb);
	}

	cdspPvEdgeParaSet(reg[2],reg[3],reg[4],reg[5],reg[6],reg[7],reg[8]);
	cdspPvEdgeFilterSet(reg[9],reg[14],reg[13],reg[12],reg[11],reg[10]);
	cdspPvEdgeFilterEnSet(reg[0]&0x0f);

	cdspPvIntLowThrSet(reg[16]);
	cdspPvMedianModeSet(reg[15]);

	//XBYTE[ REG_Cdsp_intpllowthr ]=reg[16];

	SysDynamicValue.pvMode.nextEnhanMode = intensity;

    EXIT_CRITICAL(sts);

}

/**
 * @fn        void pvCdspDynamicBinReadtoPool(UINT8 *f, UINT8 intensity, UINT32 startAddr)
 * @brief     pvCdspDynamicBinLoad to G_FPGAMemPool
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] startAddr : dram address for saving wdr table
 * @retval    NONE
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-27
 * @todo      N/A
 * @bug       N/A
*/
void pvCdspDynamicBinReadtoPool(
	UINT8 *f,
	UINT32 startAddr
)
{
    UINT32 filesize;
	UINT16 i;
	UINT16 datalength,regAddr,idx,datanum,dataspace;
	UINT8  lsb,msb;
	UINT8  sts;
	//UINT8  reg[18];
	//UINT16 location;

	cdspPrintfLoadRes("load pv dynamic para\n");

	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load pv dynamic para err\n");
		}
		else{
			cdspPrintfLoadRes("load pv dynamic para suc\n");
		}

	}

	#if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
   		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
	#endif

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);
	M_DRAM_ReadWord(lsb, msb);
	datalength = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.pvMode.weakThr = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.pvMode.middleThr = (UINT16)(msb&0xff)<<8|(lsb);


	cdspPrintfLoadRes("pv cdsp para datalength=%d\n",datalength);
	cdspPrintfLoadRes("weakThr=%d, middleThr=%d\n",SysDynamicValue.pvMode.weakThr,SysDynamicValue.pvMode.middleThr);


	datalength= datalength/4;	//address + data = 4bytes
	datanum = datalength/3;	//number of level = 3
	dataspace = 18;// 18 reg space

	if (datalength > 256){
		cdspPrintfLoadRes("datalength > 256\n",datalength);
	}

	SysDynamicValue.pvModeOffset = 0;

	for (i=0;i<sizeof(dynamicIqTbl);i++){
		dynamicIqTbl[i]=0;
	}

	/* set of parameters start address */
	M_DRAM_SetStartAddress((UINT32)(startAddr+6), 1);

	ENTER_CRITICAL(sts);

	for ( i=0 ; i< datalength ;i++ ) {

		M_DRAM_ReadWord(lsb, msb);
		regAddr = (UINT16)((msb&0xff)<<8| lsb );
		M_DRAM_ReadWord(lsb, msb);

		if (regAddr<0x21d0){
			idx= regAddr-0x216c+16;
		}
		else{
			idx= regAddr-0x21d0;
		}

		if (idx<18){
			dynamicIqTbl[idx+((i/datanum)*dataspace)] = lsb;
			cdspPrintfLoadRes("R[%d]=0x%b02x\n",idx+((i/datanum)*dataspace),lsb);
		}
	}

    EXIT_CRITICAL(sts);

}

/**
 * @fn        void cdspPvDynamicEnhanSet(UINT8 fileId,	UINT8 intensity)
 * @brief     Set egge level for preview
 * @param     [in] fileId : resource id
 * @param     [in] intensity : 0 weak, 1 middle, 2 strong
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-7
 * @todo      N/A
 * @bug       N/A
*/
void
cdspPvDynamicEnhanSet(
	UINT8 fileId,
	UINT8 intensity
)
{
	#ifdef COMPARE_ANSWER
		fileId=fileId;
	#else
		UINT8 sts;

		do {
			sts = sp1kNandRsvRead(fileId, ( ( K_SDRAM_CommonEndAddr)<< 1));
		}while(!sts);

		pvCdspDynamicBinRead( "", intensity, K_SDRAM_CommonEndAddr);

	#endif
}

/**
 * @fn        void cdspPvDynamicParaFullLoad(UINT8 fileId)
 * @brief     load preview cdsp bin file
 * @param     [in] fileId : resource id
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
cdspPvDynamicParaFullLoad(
	UINT8 fileId
)
{
	#ifdef COMPARE_ANSWER
		fileId=fileId;
	#else

		UINT8 sts;

		do {
			sts = sp1kNandRsvRead(fileId, ( ( K_SDRAM_CommonEndAddr+0x1000)<< 1));
		}while(!sts);

		pvCdspDynamicBinReadtoPool( "", K_SDRAM_CommonEndAddr+0x1000);

	#endif
}


/**
 * @fn        void stillCdspBinRead(UINT8 *f, UINT32 startAddr)
 * @brief     stillCdspBinRead
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] startAddr : dram address for saving wdr table
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
stillCdspBinRead(
	UINT8 *f,
	UINT32 startAddr
)
{
    UINT32 filesize;
	UINT16 i;
	UINT16 datalength,regAddr,idx;
	UINT8  lsb,msb;
	UINT8  sts;
	UINT8  reg[34];

	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load still cdsp para err\n");
		}
		else{
			cdspPrintfLoadRes("load still cdsp para suc\n");
		}

	}

	#if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
   		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
	#endif

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);
	M_DRAM_ReadWord(lsb, msb);
	datalength = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.stillMode.weakThr = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.stillMode.middleThr = (UINT16)(msb&0xff)<<8|(lsb);

	cdspPrintfLoadRes("still cdsp para datalength=%d\n",datalength);
	cdspPrintfLoadRes("weakThr=%d, middleThr=%d\n",SysDynamicValue.stillMode.weakThr,SysDynamicValue.stillMode.middleThr);

	datalength= datalength/4;
	datalength = datalength/3;  /*only save one set of parameters*/

	for (i=0;i<34;i++){
		reg[i]=0;
	}

	ENTER_CRITICAL(sts);

	for ( i=0 ; i< datalength ;i++ ) {

		M_DRAM_ReadWord(lsb, msb);
		regAddr = (UINT16)((msb&0xff)<<8| lsb );
		M_DRAM_ReadWord(lsb, msb);

		if (regAddr==0x216c){
			idx= 16;
		}
		else if(regAddr<0x22a0){
			idx= regAddr-0x21d0;
		}
		else{
			idx= regAddr-0x22a0+17;
		}

		if (idx<34){
			reg[idx] = lsb;
		}

		cdspPrintfLoadRes("R[0x%x]=0x%b02x\n",regAddr,lsb);
	}

	cdspStillEdgeParaSet(reg[2],reg[3],reg[4],reg[5],reg[6],reg[7],reg[8]);
	cdspStillEdgeFilterSet(reg[9],reg[14],reg[13],reg[12],reg[11],reg[10]);
	cdspStillEdgeFilterEnSet(reg[0]&0x0f);

	cdspStillIntLowThrSet(reg[16]);

	cdspStillYCoringParaSet(&reg[17]);

	//XBYTE[ REG_Cdsp_intpllowthr ]=reg[16];

	SysDynamicValue.stillMode.currEnhanMode = CDSP_DYNAMIC_WEAK;

    EXIT_CRITICAL(sts);

}

/**
 * @fn        void stillCdspDynamicBinRead(UINT8 *f, UINT8 intensity, UINT32 startAddr)
 * @brief     stillCdspDynamicBinRead
 * @param     [in] f : filename (char[12], 8.3 format)
 * @param     [in] intensity : 0 weak, 1 middle, 2 strong
 * @param     [in] startAddr : dram address for saving wdr table
 * @retval    NONE
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-10-27
 * @todo      N/A
 * @bug       N/A
*/
void
stillCdspDynamicBinRead(
	UINT8 *f,
	UINT8 intensity,
	UINT32 startAddr
)
{
    UINT32 filesize;
	UINT16 i;
	UINT16 datalength,regAddr,idx;
	UINT8  lsb,msb;
	UINT8  sts;
	UINT8  reg[34];

	cdspPrintfLoadRes("load still dynamic para\n");

	if( f[ 0 ] != 0 ) {
		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		if (!dramFileLoad(f, startAddr, &filesize)){
			cdspPrintfLoadRes("load still dynamic para err\n");
		}
		else{
			cdspPrintfLoadRes("load still dynamic para suc\n");
		}

	}

	#if 0 /* for debug */
		M_DRAM_SetStartAddress((UINT32)startAddr, 1);
   		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
		M_DRAM_ReadWord(RLSB, RMSB);
		printf("cdsp para LSB=%bx,MSB=%bx\n",RLSB,RMSB);
	#endif

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);
	M_DRAM_ReadWord(lsb, msb);
	datalength = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.stillMode.weakThr = (UINT16)(msb&0xff)<<8|(lsb);
	M_DRAM_ReadWord(lsb, msb);
	SysDynamicValue.stillMode.middleThr = (UINT16)(msb&0xff)<<8|(lsb);

	cdspPrintfLoadRes("still cdsp para datalength=%d\n",datalength);
	cdspPrintfLoadRes("weakThr=%d, middleThr=%d\n",SysDynamicValue.stillMode.weakThr,SysDynamicValue.stillMode.middleThr);

	datalength= datalength/4;	//address + data = 4bytes
	datalength = datalength/3;	//number of level = 3

	/* offset set of parameters */
	M_DRAM_SetStartAddress((UINT32)(startAddr+6+(intensity*datalength*2)), 1);

	for (i=0;i<34;i++){
		reg[i]=0;
	}

	ENTER_CRITICAL(sts);

	for ( i=0 ; i< datalength ;i++ ) {

		M_DRAM_ReadWord(lsb, msb);
		regAddr = (UINT16)((msb&0xff)<<8| lsb );
		M_DRAM_ReadWord(lsb, msb);

		if (regAddr==0x216c){
			idx= 16;
		}
		else if(regAddr<0x22a0){
			idx= regAddr-0x21d0;
		}
		else{
			idx= regAddr-0x22a0+17;
		}

		if (idx<34){
			reg[idx] = lsb;
		}


		cdspPrintfLoadRes("R[0x%x]=0x%b02x\n",regAddr,lsb);
	}

	cdspStillEdgeParaSet(reg[2],reg[3],reg[4],reg[5],reg[6],reg[7],reg[8]);
	cdspStillEdgeFilterSet(reg[9],reg[14],reg[13],reg[12],reg[11],reg[10]);
	cdspStillEdgeFilterEnSet(reg[0]&0x0f);

	cdspStillIntLowThrSet(reg[16]);

	cdspStillYCoringParaSet(&reg[17]);

	//XBYTE[ REG_Cdsp_intpllowthr ]=reg[16];

	SysDynamicValue.stillMode.nextEnhanMode = intensity;

    EXIT_CRITICAL(sts);

}

/**
 * @fn        void cdspStillDynamicEnhanSet(UINT8 fileId,	UINT8 intensity)
 * @brief     load resource for preview edge
 * @param     [in] fileId : resource id
 * @param     [in] intensity : 0 weak, 1 middle, 2 strong
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-7
 * @todo      N/A
 * @bug       N/A
*/
void
cdspStillDynamicEnhanSet(
	UINT8 fileId,
	UINT8 intensity
)
{
	#ifdef COMPARE_ANSWER
		fileId=fileId;
	#else
		UINT8 sts;

		do {
			sts = sp1kNandRsvRead(fileId, ( ( K_SDRAM_CommonEndAddr+0x1000)<< 1)); //skip sanp sound address by kai
		}while(!sts);

		stillCdspDynamicBinRead( "", intensity, K_SDRAM_CommonEndAddr+0x1000);  //skip sanp sound address by kai

	#endif
}


/**
 * @fn        void iqDataWrite(UINT8 id, UINT32 startAddr)
 * @brief     write iq data
 * @param     [in] id
 * @param     [in] startAddr :  : dram address for saving iq data
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
iqDataWrite(
	UINT8 id,
	UINT32 startAddr
)
{
	UINT16 datalength;
	UINT8 t0, t1, t2, t3;

	M_DRAM_SetStartAddress( startAddr, 0 );
	M_DRAM_WriteWord(cdspIdHdr[ 0 ], cdspIdHdr[ 1 ]);
	M_DRAM_WriteWord(cdspIdHdr[ 2 ], cdspIdHdr[ 3 ]);
	M_DRAM_WriteWord(cdspIdHdr[ 4 ], cdspIdHdr[ 5 ]);

	if( id == SP1K_CALIBRATION_OB_FILE_ID ) {
		datalength = SP1K_CALIBRATION_OB_MAX << 2;

		t0 = datalength & 0xff;
		t1 = ( datalength >> 8 ) & 0xff;

		M_DRAM_WriteWord(t0, t1);

        #ifndef CDSP_MISC_TAG_TODO
			t0 = calibrationObData[ SP1K_CALIBRATION_OB_R ];
			t1 = calibrationObData[ SP1K_CALIBRATION_OB_Gr ];
			t2 = calibrationObData[ SP1K_CALIBRATION_OB_B ];
			t3 = calibrationObData[ SP1K_CALIBRATION_OB_Gb ];
        #endif

		M_DRAM_WriteWord(t0, t1);
		M_DRAM_WriteWord(t2, t3);
	}
	else if( id == SP1K_CALIBRATION_AWB_FILE_ID ) {
		datalength = SP1K_CALIBRATION_AWB_MAX << 2;

		t0 = datalength & 0xff;
		t1 = ( datalength >> 8 ) & 0xff;

		M_DRAM_WriteWord(t0, t1);

        /*   //will mark
		t0 = calibrationAwbData[ SP1K_CALIBRATION_AWB_R ];
		t1 = calibrationAwbData[ SP1K_CALIBRATION_AWB_Gr ];
		t2 = calibrationAwbData[ SP1K_CALIBRATION_AWB_B ];
		t3 = calibrationAwbData[ SP1K_CALIBRATION_AWB_Gb ];
        */

		M_DRAM_WriteWord(t0, t1);
		M_DRAM_WriteWord(t2, t3);
	}

}

/**
 * @fn        void criteriaDataRead(UINT32 startAddr)
 * @brief     read criteria Data
 * @param     [in] startAddr : criteria data dram address
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
criteriaDataRead(
	UINT32 startAddr
)
{
	UINT16 i,tmp;
	UINT16 datalength;
	UINT8 t0, t1, t2, t3;
	UINT32 value;

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);

	M_DRAM_ReadWord(t0, t1);
	datalength = (UINT16)(t1&0xff)<<8|(t0&0xff);

	tmp =datalength/4;

	for ( i=0 ; i< (tmp); i++ ) {
		M_DRAM_ReadWord(t0, t1);
		M_DRAM_ReadWord(t2, t3);

		*( (UINT8 *)&value + 0 ) = t3;
		*( (UINT8 *)&value + 1 ) = t2;
		*( (UINT8 *)&value + 2 ) = t1;
		*( (UINT8 *)&value + 3 ) = t0;

		#ifndef CDSP_MISC_TAG_TODO
			calibrationCriteriaData[ i ] = value;
		#endif
	}
}

/**
 * @fn        void criteriaDataWrite(UINT32 startAddr)
 * @brief     write criteria data to dram
 * @param     [in] startAddr : dram address for saving criteria data
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
criteriaDataWrite(
	UINT32 startAddr
)
{
	UINT16 i,tmp;
	UINT16 datalength;
	UINT8 t0, t1, t2, t3;
	UINT32 value;

	datalength = SP1K_CALIBRATION_CRITERIA_MAX << 2;

	M_DRAM_SetStartAddress((UINT32)(startAddr+3), 0);

	t0 = ( datalength >> 8 ) & 0xff;
	t1 = datalength & 0xff;

	M_DRAM_WriteWord(t0, t1);

	tmp =datalength/4;

	for ( i=0 ; i< (tmp); i++ ) {
		#ifndef CDSP_MISC_TAG_TODO
			value = calibrationCriteriaData[ i ];    //will mark
		#endif
		t3 = *( (UINT8 *)&value + 0 );
		t2 = *( (UINT8 *)&value + 1 );
		t1 = *( (UINT8 *)&value + 2 );
		t0 = *( (UINT8 *)&value + 3 );

		M_DRAM_WriteWord(t0, t1);
		M_DRAM_WriteWord(t2, t3);
	}
}


/**
 * @fn        void cdspColorMatrixRestore(void)
 * @brief     restore color matrix
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
cdspColorMatrixRestore(
	void
)
{
	UINT16 i;
	for(i=REG_Cdsp_ColorM_A00_low;i<=REG_Cdsp_ColorM_A22_high;i++){
		XBYTE[i] = cMatrix[i-REG_Cdsp_ColorM_A00_low];
	}

}


/**
 * @fn        void cdspColorMatrixBackup(void)
 * @brief     backup color matrix
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
cdspColorMatrixBackup(
	void
)
{
	UINT16 i;
	for(i=REG_Cdsp_ColorM_A00_low;i<=REG_Cdsp_ColorM_A22_high;i++)
		cMatrix[i-REG_Cdsp_ColorM_A00_low] = XBYTE[i];

}

void
cdspWdrInit(
	void
)
{
	XBYTE[REG_Cdsp_WDRyga]=0x08;
	XBYTE[REG_Cdsp_WDRqs]= 0x40;
	XBYTE[REG_Cdsp_WDRes]= 0x80;
	XBYTE[REG_Cdsp_WDRyqthr]=0xff;
	XBYTE[REG_Cdsp_WDRyoff]= 0x80;
	XBYTE[REG_Cdsp_WDRyqf_low]=0x4c;
	XBYTE[REG_Cdsp_WDRyqf_high]=0x01;

	XBYTE[REG_Cdsp_WDRqfslope_low]=0xbf;
	XBYTE[REG_Cdsp_WDRqfslope_high]=0x01;
	XBYTE[REG_Cdsp_WDRcbfthr_low]=0x80;
	XBYTE[REG_Cdsp_WDRcbfthr_high]=0x01;
	XBYTE[REG_Cdsp_WDRcrfthr_low]=0x80;
	XBYTE[REG_Cdsp_WDRcrfthr_high]=0x01;
}


/**
 * @fn        void sp1kHalCdspImgSrcSel(UINT8 srcSel)
 * @brief     switch cdsp source from front or dram data.
 * @param     [in] srcSel :  0 -> From Front Interface
 *							 1 -> From SDRAM
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kHalCdspImgSrcSel(
	UINT8 srcSel
)
{
	HAL_CdspRawImgSource( srcSel );
}

/**************************************************************************
 *                       T E M P   F U N C T I O N                        *
 **************************************************************************/
#if 0
	void ReadIqData( UINT32 startAddr )
	{
	//	  UINT32 filesize;
		UINT16 i,tmp;
		UINT16 datalength;
		UINT8 t0, t1, t2, t3;
		UINT32 value;

		M_DRAM_SetStartAddress((UINT32)(startAddr+3), 1);

		M_DRAM_ReadWord(t0, t1);
		datalength = (UINT16)(t1&0xff)<<8|(t0&0xff);
		printf("datalength=%d \n",datalength);
		tmp =datalength/4;
		printf("tmp=%d \n",tmp);


		for ( i=0 ; i< (tmp); i++ ) {
			//printf("i=%d \n",i);
			M_DRAM_ReadWord(t0, t1);
			M_DRAM_ReadWord(t2, t3);

			*( (UINT8 *)&value + 0 ) = t3;
			*( (UINT8 *)&value + 1 ) = t2;
			*( (UINT8 *)&value + 2 ) = t1;
			*( (UINT8 *)&value + 3 ) = t0;


			//calibrationIqData[ i ] = value;  //will mark
		}

		return;
	}
#endif

