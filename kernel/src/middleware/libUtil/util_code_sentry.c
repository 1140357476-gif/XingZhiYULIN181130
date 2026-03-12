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
 * @file      util_code_sentry.c
 * @brief     Brief
 * @author    AllenLin
 * @since     2010-07-27
 * @date      2010-07-27
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE   __FILE_ID_UTIL_CODE_SENTRY__

#include "general.h" 
#include "dos32.h"
#include "dbg_def.h"
#include "awb_api.h"
#include "sp1k_fd_api.h"
#include "sp1k_ae_api.h"
#include "initio.h"
#include "utility.h"
#include "dbg_wdt.h"
#include "dbg_mem.h"
#include "hal_moe.h"
#include "reg_dram.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
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
extern xdata UINT8 G_CodeSentry;
extern xdata UINT8 G_CodeSentry_270A;
extern xdata UINT8 G_CodeSentry_270B;
 xdata UINT8 csDbgData[32];
extern UINT8 vdProcEn;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void csResetAeAwbFd()
{
	
}
/**
 * @fn        void csSvcProc(void)
 * @brief     Code sentry service process
 * @param     The defination of Mode and Type:
 *			  Mode :
 *				0001 : CDSP1
 * 				0002 : DISPIMG
 *				0004 : OSD
 *				0008 : AUD
 *				0010 : CPU
 *				0020 : MJPG
 *				0040 : DMA
 * 				0080 : FM
 *				0100 : GPRM
 *				0200 : USB
 *				0400 : INDIRECT
 *				0800 : CDSP2
 *				1000 : CDSP3
 * 			  Type:
 *			  	(CDSP)
 *				0 : Snap raw8 data
 *				1 : R/W YUV data
 *				2 : Lens Gain Buffer
 *				3 : AE
 *				4 : AWB subsample
 *				5 : Badpixel
 *				6 : EdgeWindow
 *				7 : Snap raw 10 data
 *				(CDSP2)
 *				0 : dark raw8 data
 *				1 : preview YUV
 *				2 :	TD YUV
 *				(CDSP3)
 *				0 : MOE/FD
 *				1 : FD
 *				2 : FD
 *				(JPG)
 *				0 : video preview YUV
 *				1 : Bypass dst /Thumbnail
 *				2 : Stamp
 *				3 : Still src/ dst YUV
 *				4 : VLC
 *				5 : ME engine edge table
 *				6 : ME engine MV linear address
 *				7 : ME engine current / ref YUV
 * @retval    NONE
 * @see       NULL
 * @author    AllenLin
 * @since     2010-7-27
 * @todo      N/A
 * @bug       N/A
*/
extern UINT8 
awbEnable(
	UINT8 en
	);
extern void 
	aeInit(
		UINT8 proc
	);

/**
 * @fn        void sp1kCodeSentrySet(UINT8 enable)
 * @brief     Enable the code sentry (MPU)
 * @param     [in] enable
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kCodeSentrySet(UINT8 enable)
{
	if(enable)
	{
		XBYTE[REG_Dram_AddressErr] = 0x01; //address_err_enable 
		XBYTE[REG_Dram_Cdsp1Eof_Inten] |= 0x80; /* enable code sentry interrupt */
	}
	else
	{
		XBYTE[REG_Dram_AddressErr] &= ~0x01; //address_err_Disable 
		XBYTE[REG_Dram_Cdsp1Eof_Inten] &= ~0x80; /* Disable code sentry interrupt */
	}
}

	

void
csSvcProc(
	void
)
{
	UINT16 Mode = 0;
	UINT8 Type = 0;
		
	if (G_CodeSentry == 0x01) {

		G_CodeSentry = 0;
		//printf("270a,0b=%bx,%bx\n",G_CodeSentry_270A,G_CodeSentry_270B);
		Mode = (UINT16)((G_CodeSentry_270B & 0x1F)<<8)  | (UINT16)G_CodeSentry_270A;
		Type = G_CodeSentry_270B >> 5;
		ASSERT(0, 1);
		CPrintf2("Murder-Mode:%x,Type:%bx",Mode,Type);
#if 1 //wangjg++ for MOE/II/FD dbg
		if (Mode == 0x1000) {
			extern UINT32 aeMoeAddrQueue;
			printf("%lx,%lx\n",K_SDRAM_DPSStartAddr,aeMoeAddrQueue);
			dumpData(128, aeMoeAddrQueue, 1);
			
			if (moeDbgTrig == MOE_Trigger_Awb) {
				awbEnable(2);
			}
			else if (moeDbgTrig == MOE_Trigger_Ae) {
				aeInit(0xff);
			}
		}
#if 0			
			xdata UINT32 xdata* pt32= csDbgData;
			printf("OUT:MOE(%lx),II(%lx),FD(%lx),(%lx,%bx),(%bx)\n",pt32[0],pt32[1],pt32[2],pt32[3],pt32[4],vdProcEn);
			//dbgWdtFeed(20000);
#if 0
			dramFileSave("QUEUE", K_SDRAM_DPSStartAddr, 16*1024ul);
			dramFileSave("CSDATA", K_SDRAM_CodeSentryRedirectAddr, 64);
			dramFileSave("CODE",0,557056);
#endif
#if 0
			if(sp1kAwbGetStatus() && Type == 0)
				sp1kAwbReset();
			if(sp1kAeGetModeEn())
				aeProcReset();
			if(sp1kFdGetStatus())
				sp1kFdReset();
#endif
		}
		else if (Mode == 0x800) {
			//UINT8 i;
					
			//for(i=0;i<8;i++){				
			//	printf("[0x%x]=0x%bx\n",0x22b8+i,XBYTE[0x22b8+i]);
			//}
		}
#endif
#endif	
	}
}

