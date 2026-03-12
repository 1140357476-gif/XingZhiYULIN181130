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
#include <string.h>
#include <stdio.h>
#include "general.h"
#include "dbg_def.h"
#include "dos32.h"
#include "cardopt.h"
#include "cardlink.h"
#include "sdfunc.h"
#include "smcfunc.h"
#include "dramfunc.h"
#include "mtd_general.h"
#include "mtd_macro_api.h"
#include "sd_bg.h"

#define __FILE	__FILE_ID_DOS_RW__
#define	DEBUG	0
extern DOS dos;
extern xdata UINT32 SD_PhySectors;
extern xdata UINT32 diskPhySectors;
extern xdata UINT32 DOSRAM_PhySectors;

#ifndef FPGA_MEM_POOL_ADDR
static UINT8 xdata dosRwDbg _at_ 0x5001;
#else
static UINT8 xdata dosRwDbg;
#endif

UINT8 DOS_SectorRead(UINT8 DevId, UINT32 Start, UINT32 Num, UINT32 TarAddr)
{
	//UINT32 t0,t1;
	UINT8 err=TRUE;

	if( Num == 0 ) return dosRwErr(__FILE,__LINE__);	
/*
 *  DOS_DEV_SUPPORT_CARD
 */
#if DOS_DEV_SUPPORT_CARD
	if(DevId==DRIVE_SD){
#if DEBUG
		printf("%s %u DevId=%bu,sector=%lu,Num=%lu,addr=%lx,SD=%lu\n", __FILE__, (unsigned short)__LINE__,DevId,Start,Num,TarAddr,SD_PhySectors);
#endif
		if (( Start + Num ) > SD_PhySectors) {
			return dosRwErr(__FILE,__LINE__);
		}		
		bgRwAbort();
		HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1); //phil change to G_Card_Type for temp
		err = SD_LCard2DramMore(Start, TarAddr, Num);
		#if 0
		dumpData(512, TarAddr, 1);
		#endif
		bgRwResume();
		if( err != TRUE ) return dosRwErr(__FILE,__LINE__);
	}
#endif //DOS_DEV_SUPPORT_CARD

/*
 *  DOS_DEV_SUPPORT_FLASH
 */
#if DOS_DEV_SUPPORT_FLASH
	else if(DevId==DRIVE_NAND) {
#if DEBUG
	printf("%s %u DevId=%bu,sector=%lu,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,DevId,Start,Num,TarAddr);
#endif
		#if 0
		if (( Start + Num ) > diskPhySectors ) {
			return dosRwErr(__FILE,__LINE__);
		}
		Start = Start << 9;
		while(Num) {
			SMC_LCard2Dram(Start,TarAddr,512);
			TarAddr += 256L;
			Start += 512;
			Num--;
		}
		#else
#if (DOS_RSV_PART == 1)
        if (DOS_RsvPartStsGet()) {
            Start += DOS_PartActOffsetGet();
        } else {
    		if (( Start + Num ) > diskPhySectors ) {
    			return dosRwErr(__FILE,__LINE__);
    		}
        }
#else
		if (( Start + Num ) > diskPhySectors ) {
			return dosRwErr(__FILE,__LINE__);
		}				
#endif

		MTD_SelfShiftL8(Num);// 3.6us
		Num = Num << 1;// 9.2us
		MTD_SelfShiftL8(Start);// 3.6us
		Start = Start << 1;// 9.2us
		//HAL_GlobalReadGTimer(&t0);
		HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_Sdf, 1);
		SMC_LCard2Dram(Start,TarAddr,Num);
		//HAL_GlobalReadGTimer(&t1);
		#endif
	}
#endif //DOS_DEV_SUPPORT_FLASH   

/*
 *  DOS_DEV_SUPPORT_DRAM
 */
#if DOS_DEV_SUPPORT_DRAM
#if ( DOSRAM_OPTION == 1 )
	else if(DevId == DRIVE_DRAM) {
#if DEBUG
	printf("%s %u DevId=%bu,sector=%lu,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,DevId,Start,Num,TarAddr);
#endif
		if (( Start + Num ) > DOSRAM_PhySectors ) {
			return dosRwErr(__FILE,__LINE__);
		}
		DRAM_LCard2DramMore(Start << 8,TarAddr,Num);
	}
#endif

#if (DOS_RSV_PART == 1)
    else if(DevId == DRIVE_NAND_RSV) {

#if DEBUG
	printf("%s %u DevId=%bu,sector=%lu,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,DevId,Start,Num,TarAddr);
#endif
        /*if (DOS_RsvPartStsGet()) {
            Start += DOS_PartActOffsetGet();
            //printf("R offset: %lx\n", (UINT32)DOS_PartActOffsetGet());
        }*/
		MTD_SelfShiftL8(Num);// 3.6us
		Num = Num << 1;// 9.2us
		MTD_SelfShiftL8(Start);// 3.6us
		Start = Start << 1;// 9.2us
		//HAL_GlobalReadGTimer(&t0);
		HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_Sdf, 1);

		SMC_LCard2Dram(Start,TarAddr,Num);
      
    }
#endif
#endif//DOS_DEV_SUPPORT_DRAM

	else {
		err = 0;
	}
	return err;
}


UINT8 DOS_SectorWrite(UINT8 DevId, UINT32 Start, UINT32 Num, UINT32 SrcAddr)
{
	UINT32 err = TRUE;
#if DEBUG    
	UINT32 tmr0, tmr1;
#endif

	if( Num == 0 ) return dosRwErr(__FILE,__LINE__);
       if( Start == 0 ) { printf("F(%x), L(%u) RwSectorErr\n", __FILE,__LINE__);}
#if DEBUG
	if (dosRwDbg) {
		HAL_GlobalReadGTimer(&tmr0);
	}
#endif

/*
 *  DOS_DEV_SUPPORT_CARD
 */
#if DOS_DEV_SUPPORT_CARD
	if(DevId==DRIVE_SD){
#if DEBUG
	printf("%s %u DevId=%bu,sector=%lu,Num=%lu,addr=%lx,SD=%lu\n", __FILE__, (unsigned short)__LINE__,DevId,Start,Num,SrcAddr,SD_PhySectors);
#endif
		if (( Start + Num ) > SD_PhySectors ) {
			return dosRwErr(__FILE,__LINE__);
		}
		bgRwAbort();
		HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1); //phil change to G_Card_Type for temp
		err = SD_LDram2CardMore(Start, SrcAddr, Num);
		bgRwResume();
		if( err != TRUE ) return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
#endif //DOS_DEV_SUPPORT_CARD


/*
 *  DOS_DEV_SUPPORT_FLASH
 */
#if DOS_DEV_SUPPORT_FLASH
	else if(DevId==DRIVE_NAND) {
        
#if (DOS_RSV_PART == 1)
        if (DOS_RsvPartStsGet()) {
            Start += DOS_PartActOffsetGet();
        } else {
    		if (( Start + Num ) > diskPhySectors ) {
    			return dosRwErr(__FILE,__LINE__);
    		}
        }
#else
		if (( Start + Num ) > diskPhySectors ) {
			return dosRwErr(__FILE,__LINE__);
		}		
#endif

#if DEBUG
	printf("%s %u DevId=%bu,sector=%lu,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,DevId,Start,Num,SrcAddr);
#endif

		MTD_SelfShiftL8(Start);// 3.6us
		Start = Start << 1;// 9.2us
#if 0
dumpData(512, SrcAddr, 1);
#endif
		HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_Sdf, 1);
		SMC_LDram2Card(Start, SrcAddr, Num);
	}
#endif //DOS_DEV_SUPPORT_FLASH

/*
 *  DOS_DEV_SUPPORT_DRAM
 */
#if DOS_DEV_SUPPORT_DRAM
#if ( DOSRAM_OPTION == 1 )
	else if(DevId == DRIVE_DRAM) {
#if DEBUG
	printf("%s %u DevId=%bu,sector=%lu,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,DevId,Start,Num,SrcAddr);
#endif
		if (( Start + Num ) > DOSRAM_PhySectors ) {
			return dosRwErr(__FILE,__LINE__);
		}
		DRAM_LDram2CardMore(Start << 8,SrcAddr,Num);
	}
#endif

#if (DOS_RSV_PART == 1)
    else if (DevId == DRIVE_NAND_RSV) {
#if DEBUG
    printf("%s %u DevId=%bu,sector=%lu,Num=%lu,addr=%lx\n", __FILE__, (unsigned short)__LINE__,DevId,Start,Num,SrcAddr);
#endif
        /*if (DOS_RsvPartStsGet()) {
            Start += DOS_PartActOffsetGet();
            //printf("W offset: %lx\n", (UINT32)DOS_PartActOffsetGet());
        }*/

        MTD_SelfShiftL8(Start);// 3.6us
        Start = Start << 1;// 9.2us
#if 0
dumpData(512, SrcAddr, 1);
#endif
        HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_Sdf, 1);

        SMC_LDram2Card(Start, SrcAddr, Num);

    }    
#endif
#endif //DOS_DEV_SUPPORT_DRAM
	else {
		err = 0;
	}
#if DEBUG    
	if (dosRwDbg) {
		HAL_GlobalReadGTimer(&tmr1);
		printf("s%lu\n", tmr1 - tmr0);
	}
#endif    
	return err;
}

