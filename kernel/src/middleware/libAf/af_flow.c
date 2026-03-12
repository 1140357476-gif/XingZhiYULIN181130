/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
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
#include "general.h"
#include "asicreg.h"
#include "initio.h"
#include "main.h"
#include "front_logi.h"
#include "sp1k_rsvblk_api.h"
#include "utility.h"
#include "math.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "reg_def.h"
#include "awb_api.h"
#include "sp1k_ae_api.h"
#include "ae_api.h"
#include "sp1k_fd_api.h" 
#include "hal_dram.h"
#include "ae_def.h"
#include "ae_debug.h"
#include "intr.h"
#include "cdsp_task.h"
#include "hal_front_common.h"
#include "hal_front.h"
#include "icat_stub.h"
#include "pwrlog.h"
#include "hal_moe.h"
#include "hal_fd.h"
#include "os_api.h"
#include "pv_task.h"
#include "hal_gprm.h"
#include "pv_photoframe.h"
#if (DBG_PV_PF_TIMING == 1)
#include "gpio_api.h"
#endif
#include "sp1k_hal_api.h"
#include "af_api.h"


#if (AF_FIXTURE_EN == 1)

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* debug. */
#define __FILE	__FILE_ID_LIB_AF_FLOW__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
extern UINT8 afEn;
extern UINT32 AFWinvalue;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/ 
 void 
 afIntrWinValueGet(
	 void
 )
 {
	 UINT16 lValue, hValue;
 
	 if ( 0/*afEn*/ ) {
		 if ((XBYTE[REG_Cdsp_evnt1]&0x10) == 0x00) {
			#if 0
			 intrPutChar('N');
			 intrPutChar('\n');
			#endif
		 }
		 else {
			 XBYTE[REG_Cdsp_evnt1] = 0x10;
			 WRITE8(&lValue,1,XBYTE[REG_Cdsp_AFvalue_byte0]);
			 WRITE8(&lValue,0,XBYTE[REG_Cdsp_AFvalue_byte1]);
			 //intrPutUint16(lValue);
			 //lValue = ( UINT8 ) XBYTE[ REG_Cdsp_AFvalue_byte0 ];
			 //lValue = lValue + ( UINT16 ) ( XBYTE[ REG_Cdsp_AFvalue_byte1 ] << 8 );
			 //intrPutUint16(lValue);
			 WRITE8(&hValue,1,XBYTE[REG_Cdsp_AFvalue_byte2]);
			 WRITE8(&hValue,0,XBYTE[REG_Cdsp_AFvalue_byte3]);
			 //intrPutUint16(hValue);
			 //hValue = ( UINT8 ) XBYTE[ REG_Cdsp_AFvalue_byte2 ] ;
			 //hValue = hValue + ( UINT16 ) ( XBYTE[ REG_Cdsp_AFvalue_byte3 ] << 8 );
			 //intrPutUint16(hValue);
 
			 WRITE16(&AFWinvalue,2,lValue);
			 WRITE16(&AFWinvalue,0,hValue);
			 //intrPutUint32(AFWinvalue);
			 //AFWinvalue = ( UINT32 ) ( ( UINT32 )hValue << 16 ) + ( UINT32 ) lValue;
 
			#if 0
			 intrPutChar('\n');
			 intrPutChar('A');
			 intrPutChar('F');
			 intrPutChar(':');
			 intrPutUint32(AFWinvalue);
			 intrPutChar('\n');
			#endif
		 }
	 }
 }

#endif
