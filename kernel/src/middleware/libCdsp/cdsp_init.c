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
 * @file      app_CDSPInit.c
 * @brief     sp1k API for cdsp init
 * @author    sun yong
 * @since     2010-01-01
 * @date      2010-01-01
*/


#include "general.h"
#include "initio.h"
#include "sp1k_rsvblk_api.h"
#include "cdsp_init.h"
#include "sp1k_cdsp_api.h"
#include "cdsp_misc.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal_cdsp_iq.h"
#include "Cmd_test.h"
#include "snap_core.h"
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
//#define CDSP_INIT_TAG_TODO 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern SysDynamicEnhan SysDynamicValue;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**
 * @fn        void sp1kCdspParaInit(UINT8 fileId)
 * @brief     cdsp initialize (load tables)
 * @param     [in] fileId
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kCdspParaInit( 
	UINT8 fileId 
)
{
	UINT8 sts;
	
	HAL_CdspClockSet(1,7);  /* will: set CDSP clock before writing table in SRAM */
	HAL_Cdsp2ndPathCfg(1,0,0);
	HAL_Cdsp2ndPathEnSet(0);
	
	
	#ifdef COMPARE_ANSWER
		sts=sts;
		fileId=fileId;	
	#else
		do {
			sts = sp1kNandRsvRead(fileId, (K_SDRAM_CommonEndAddr<< 1));
		}while(!sts);

		cdspBinRead( "", K_SDRAM_CommonEndAddr );

	#endif

}


/**
 * @fn        void sp1kCdspGammaInit(UINT8 fileId)
 * @brief     load gamma table
 * @param     [in] fileId
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kCdspGammaInit( 
UINT8 fileId 
)
{	
	#ifdef COMPARE_ANSWER   
		fileId=fileId;
		//printf("loda gamma\n");
		
		dosCdCmd("TBL");
		//dosDirCmd("");
		gammaBinRead("GAMMATBL.BIN",K_SDRAM_CommonEndAddr);   
		dosCdCmd("..");		
   	#else

		UINT8 sts;
		
		do {
			sts = sp1kNandRsvRead(fileId, (K_SDRAM_CommonEndAddr<< 1));
		}while(!sts);
		
		gammaBinRead( "", K_SDRAM_CommonEndAddr );
		
	#endif
	//gammaBinRead("GAMMATBLBIN",(UINT32)0x4000); 
}

/**
 * @fn        void sp1kCdspLscInit(UINT8 fileId)
 * @brief     load LSC table
 * @param     [in] fileId
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kCdspLscInit( 
UINT8 fileId 
)
{
	
	#ifdef COMPARE_ANSWER   
		fileId=fileId;	
		dosCdCmd("TBL");
		lscBinRead("LSCTBL00.BIN",K_SDRAM_LSCTableBufAddr);	
		dosCdCmd("..");
	#else   
		UINT8 sts;
		
		do {
			sts = sp1kNandRsvRead(fileId, ( K_SDRAM_LSCTableBufAddr << 1));
		}while(!sts);
		
		lscBinRead( "", K_SDRAM_LSCTableBufAddr);
		//HAL_CdspLscWinAddrset(K_SDRAM_LSCTableBufAddr+12);   		

	#endif 	
   
   
} 

/**
 * @fn        void sp1kCdspWdrInit(UINT8 fileId)
 * @brief     load WDR table
 * @param     [in] fileId
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kCdspWdrInit( 
	UINT8 fileId
)
{

	#ifdef COMPARE_ANSWER   
		fileId=fileId;
		dosCdCmd("TBL");
		//wdrBinRead("D:\\DCIM\\100MEDIA\\TBL\\WDRTBL00BIN",K_SDRAM_CommonEndAddr);  
		wdrBinRead("WDRTBL00.BIN",K_SDRAM_CommonEndAddr); /* will: it saves data to SRAM, then set data to SRAM */
		dosCdCmd("..");
	#else

		UINT8 sts;
		
		do {
			sts = sp1kNandRsvRead(fileId, ( ( K_SDRAM_CommonEndAddr)<< 1));
		}while(!sts);
		
		wdrBinRead( "", K_SDRAM_CommonEndAddr); 
    	
    #endif
}                            

/**
 * @fn        void sp1kCdspPvParaInit(UINT8 fileId)
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
sp1kCdspPvParaInit( 
	UINT8 fileId
)
{
	#ifdef COMPARE_ANSWER	
		fileId=fileId;
	#else

		UINT8 sts;
		
		do {
			sts = sp1kNandRsvRead(fileId, ( ( K_SDRAM_CommonEndAddr)<< 1));
		}while(!sts);
		
		SysDynamicValue.pvMode.resourceId = fileId;
		pvCdspBinRead( "", K_SDRAM_CommonEndAddr); 
    	
	#endif
}

/**
 * @fn        void sp1kCdspPvParaInit(UINT8 fileId)
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
sp1kCdspPvDynamicParaInit( 
	UINT8 fileId
)
{
	#ifdef COMPARE_ANSWER	
		fileId=fileId;
	#else

		UINT8 sts;
		
		do {
			sts = sp1kNandRsvRead(fileId, ( ( K_SDRAM_CommonEndAddr)<< 1));
		}while(!sts);
		
		pvCdspDynamicBinReadtoPool( "", K_SDRAM_CommonEndAddr); 

		/* threshold in bin file */
		//SysDynamicValue.pvMode.weakThr = 31;
		//SysDynamicValue.pvMode.middleThr = 15;

		SysDynamicValue.pvMode.resourceId = fileId;
		SysDynamicValue.pvDynamicEn = CDSP_DYNAMIC_ENABLE;
    	
	#endif
}

/**
 * @fn        void sp1kCdspStillParaInit(UINT8 fileId)
 * @brief     load preview cdsp bin file
 * @param     [in] fileId : resource id
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-10-7
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kCdspStillParaInit( 
	UINT8 fileId
)
{
	#ifdef COMPARE_ANSWER	
		fileId=fileId;
	#else

		UINT8 sts;
		
		do {
			sts = sp1kNandRsvRead(fileId, ( ( K_SDRAM_CommonEndAddr)<< 1));
		}while(!sts);
		
		SysDynamicValue.stillMode.resourceId = fileId;
		stillCdspBinRead( "", K_SDRAM_CommonEndAddr); 
    	
	#endif
}

/**
 * @fn        void sp1kCdspPvDynamicParaInit(UINT8 fileId)
 * @brief     load still cdsp bin file
 * @param     [in] fileId : resource id
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kCdspStillDynamicParaInit( 
	UINT8 fileId
)
{	
	fileId=fileId;
	
	/* threshold in bin file */
	//SysDynamicValue.stillMode.weakThr = 31;
	//SysDynamicValue.stillMode.middleThr = 15;
	
	SysDynamicValue.stillMode.resourceId = fileId;
	SysDynamicValue.stillDynamicEn = CDSP_DYNAMIC_ENABLE;	
}


