/**************************************************************************
 *         Copyright(c) 2008 by Sunplus mMedia  Technology Co., Ltd.      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus mMedia *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno-*
 *  logy Co., Ltd. This software may only be used in accordance with the  *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.         *
 *                                                                        *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus mMedia Technology Co., Ltd.                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Ted Chiu                                                      *
 **************************************************************************/
#include "general.h"
#include "disp_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_osd_api.h"
#include "reg_def.h"


void dispPbBufSet(UINT32 addr, UINT16 w, UINT16 h)
{
	#if 0
	h = h;
	#endif
	if(addr != -1) {
		/* set PBBaddr. */
		XBYTE[REG_Dram_PBBAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_PBBAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_PBBAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_PBBAddr_B3] = READ8(addr,0);
	}
	if(w != -1) {
		/* set PBBhsize. */
		XBYTE[REG_Dram_PBBHSize_Low] = READ8(w,1);
		XBYTE[REG_Dram_PBBHSize_High] = READ8(w,0);
	}

	h = h;
}

