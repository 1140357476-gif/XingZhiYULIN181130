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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_device.h"
#include "hal_nand.h"
#include "smccon.h"
#include "mtd_macro_api.h"
#include "smcfunc.h"
#include "smcvar.h"
#include "main.h"
#include "reg_def.h"

#define __FILE   __FILE_ID_NAND_AUTOFLOW__

#ifdef DISK_DRV_NAND

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
static UINT32 g_nandRedundAddr;
static UINT32 g_nandEccAddr;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 G_NANDF_Category;
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


UINT32 SMC_NandAutoFlowErrorSectIdxGet(void)
{
    UINT8 errSectCnt;
    UINT16 errPageCnt;
    UINT8 sectNumPerPage;
    UINT32 errorSectIdx;
    
    errSectCnt = XBYTE[REG_Fm_cmpcnt];
    WRITE8(errPageCnt, 1, XBYTE[REG_Fm_pagecnt_Low]);
    WRITE8(errPageCnt, 0, XBYTE[REG_Fm_pagecnt_High]);
    //printf("err loc: %bx, %x\n", errSectCnt, errPageCnt);

    sectNumPerPage = XBYTE[REG_Fm_sectornum];
    
    if(!errSectCnt)
        errSectCnt = sectNumPerPage;
        
    errSectCnt -= 1;

    errorSectIdx = (UINT32)errPageCnt*sectNumPerPage + (UINT32)errSectCnt;

    return errorSectIdx;
}

void SMC_NandAutoFlowECCRead(UINT32 dramAddr, UINT8 ecc[],  UINT32 errSectIdx)
{
    dramAddr = dramAddr + (errSectIdx << 3) + 4; 

    XBYTE[REG_Dram_IndirectAddr_B0] = *((UINT8*)(&dramAddr) + 3);	//set SDRAM starting addr
    XBYTE[REG_Dram_IndirectAddr_B1] = *((UINT8*)(&dramAddr) + 2);
    XBYTE[REG_Dram_IndirectAddr_B2] = *((UINT8*)(&dramAddr) + 1);
    XBYTE[REG_Dram_IndirectAddr_B3] = *((UINT8*)(&dramAddr));
    XBYTE[REG_Dram_IndirectPrefetch] = 0x01;						//set prefetch

    while(XBYTE[REG_Dram_IndirectPrefetch]);
    ecc[3] = XBYTE[REG_Dram_IndirectRData_Low];
    ecc[4] = XBYTE[REG_Dram_IndirectRData_High];
    
    while(XBYTE[REG_Dram_IndirectPrefetch]);
    ecc[5] = XBYTE[REG_Dram_IndirectRData_Low];
    ecc[0] = XBYTE[REG_Dram_IndirectRData_High];

    while(XBYTE[REG_Dram_IndirectPrefetch]);
    ecc[0] = XBYTE[REG_Dram_IndirectRData_Low];
    ecc[0] = XBYTE[REG_Dram_IndirectRData_High];
   
    while(XBYTE[REG_Dram_IndirectPrefetch]);
    ecc[1] = XBYTE[REG_Dram_IndirectRData_Low];
    ecc[2] = XBYTE[REG_Dram_IndirectRData_High];

}

void SMC_NandAutoFlowRedundRead(UINT32 dramAddr, UINT8 redund[], UINT32 errSectIdx)
{
    UINT8 i;

    dramAddr = dramAddr + (errSectIdx << 3); 

    XBYTE[REG_Dram_IndirectAddr_B0] = *((UINT8*)(&dramAddr) + 3);	//set SDRAM starting addr
    XBYTE[REG_Dram_IndirectAddr_B1] = *((UINT8*)(&dramAddr) + 2);
    XBYTE[REG_Dram_IndirectAddr_B2] = *((UINT8*)(&dramAddr) + 1);
    XBYTE[REG_Dram_IndirectAddr_B3] = *((UINT8*)(&dramAddr));
    XBYTE[REG_Dram_IndirectPrefetch] = 0x01;						//set prefetch

    for(i = 0; i < 16; ){
        while(XBYTE[REG_Dram_IndirectPrefetch]);
        redund[i++] = XBYTE[REG_Dram_IndirectRData_Low];	//LSB
        redund[i++] = XBYTE[REG_Dram_IndirectRData_High];	//MSB
    }

}

void SMC_NandSpareAddrSet(UINT32 addr)
{
    g_nandRedundAddr = addr;
    
    XBYTE[REG_Fm_sprstraddr_0] = READ8(addr, 3);
    XBYTE[REG_Fm_sprstraddr_1] = READ8(addr, 2);
    XBYTE[REG_Fm_sprstraddr_2] = READ8(addr, 1);
    XBYTE[REG_Fm_sprstraddr_3] = READ8(addr, 0)&0x01;
}

void SMC_NandEccAddrSet(UINT32 addr)
{
    g_nandEccAddr = addr;
    
    XBYTE[REG_Fm_eccstraddr_0] = READ8(addr, 3);
    XBYTE[REG_Fm_eccstraddr_1] = READ8(addr, 2);
    XBYTE[REG_Fm_eccstraddr_2] = READ8(addr, 1);
    XBYTE[REG_Fm_eccstraddr_3] = READ8(addr, 0)&0x01;
}

static void SMC_NandAutoFlowConfig(
    UINT8 isLargeBlk,    
    UINT8 sectorNum,
    UINT32 pageShift,
    UINT16 pageShiftH16
)
{
    //printf("largblk:%bu\n", isLargeBlk);

    if(isLargeBlk)
        XBYTE[REG_Fm_nandautoen_blocktype] = XBYTE[REG_Fm_nandautoen_blocktype] | 0x10;
    else
        XBYTE[REG_Fm_nandautoen_blocktype] = XBYTE[REG_Fm_nandautoen_blocktype] & ~0x10;

    XBYTE[REG_Fm_sectornum] = sectorNum;

    XBYTE[REG_Fm_shiftaddrbuf_0] = READ8(pageShift, 3);
    XBYTE[REG_Fm_shiftaddrbuf_1] = READ8(pageShift, 2);
    XBYTE[REG_Fm_shiftaddrbuf_2] = READ8(pageShift, 1);
    XBYTE[REG_Fm_shiftaddrbuf_3] = READ8(pageShift, 0);
    XBYTE[REG_Fm_shiftaddrbuf_4] = READ8(pageShiftH16, 1);
    XBYTE[REG_Fm_shiftaddrbuf_5] = READ8(pageShiftH16, 0);

    XBYTE[REG_Fm_drambytecnt_Low] = 0xff;       //size=512
    XBYTE[REG_Fm_drambytecnt_High] = 0x1;
}


UINT8 SMC_NandAutoReadStart(
    UINT32 startAddr,
    UINT32 dramAddr, 
    UINT16 pageNum,
    UINT8 wait
)
{
    UINT8 ret = TRUE;
    UINT8 status = TRUE;
    UINT32 timeout_count = 0xFFFF;
    UINT8 cmdSize = 2;
    const UINT8 cmdBuf[2] = {K_SMC_Read1, 0x30}; 
    UINT32 errSectIdx;
    
    HAL_NANDChkRdy();
    
    XBYTE[REG_Fm_evt] = 0x83;                      //clr nandautoint, nandrdyintrevt, nandintrevt  event 
    XBYTE[REG_Fm_dramfmread_dmasel] = 0x01;                       // DRAM sel, read mode
    XBYTE[REG_Fm_nandautoen_blocktype] |= 0x01;                      // enable auto flow

    XBYTE[REG_Fm_pagenum_Low]=READ8(pageNum, 1);
    XBYTE[REG_Fm_pagenum_High]=READ8(pageNum, 0)&3;

    XBYTE[REG_Fm_datstraddr_0] = READ8(dramAddr, 3);
    XBYTE[REG_Fm_datstraddr_1] = READ8(dramAddr, 2);
    XBYTE[REG_Fm_datstraddr_2] = READ8(dramAddr, 1);
    XBYTE[REG_Fm_datstraddr_3] = READ8(dramAddr, 0)&0x01;

    if(G_NANDF_Category == 0)
        cmdSize = 1;

//printf("cmdSize: %bx\n", cmdSize);    
    SMC_AutoCmdSet(cmdBuf, cmdSize, startAddr);

    /*Enable ECC*/
    MTD_EccEnable();

    /*Clear ECC*/
    MTD_EccClear();

    
    /*Wait nand ready, it's in page program mode maybe*/
    HAL_NANDChkRdy();

    /*Check if auto command buffer ready*/
    if (XBYTE[REG_Fm_cmdsize_addrsize]) {
        /*Enable CE, Disable WP*/	
        MTD_NandCtrl(0x02);
        /*Trigger auto command/address mode*/
        MTD_NandAutoCmdTrig(); 
    }

    ENTER_CRITICAL(status);

    //XBYTE[REG_Fm_dramstr] = 1;  //triger
    while ((XBYTE[REG_Fm_evt] & 0x1) == 0x00)
    {
        if (timeout_count >	0)
        {
            timeout_count--;
        }
        else
        {
            //printf("%s %d, HAL_SDWaitIdle ERR\n", __FILE__, (UINT16)__LINE__);
            //printf("HAL_NandAutoReadStart: %bx\n", XBYTE[REG_Fm_evt]);
            //printf("%bx, %bx\n", XBYTE[REG_Fm_dramwbufptr], XBYTE[REG_Fm_dramrbufptr]);
            ret = FALSE;
            break;
        }
    }
 
    /*Disable ECC*/
    MTD_EccDisable();	

    EXIT_CRITICAL(status);

#if 1    
    /*Check ECC*/
//    if (SMC_ChkEcc() != SUCCESS) {
    if(XBYTE[REG_Fm_evt] & 0x80){
        CPrintf("E");
        G_SMC_EccErrCount++;
        //HAL_ReadECC(6, G_SMC_EccByte);		//lryy, mantis #38137
        errSectIdx = SMC_NandAutoFlowErrorSectIdxGet();
        SMC_NandAutoFlowECCRead(g_nandEccAddr, G_SMC_EccByte, errSectIdx);
        SMC_NandAutoFlowRedundRead(g_nandRedundAddr, &G_ucStorData[512], errSectIdx);
        status = SMC_CorrectPage(2, dramAddr + (errSectIdx<<8), 1);
        if (!status) {
        	printf("B->%lx\n", startAddr / G_NANDF_BlockSize);
        	status = SMC_BadBlockMark(startAddr / G_NANDF_BlockSize);
             ret = FALSE;
        }
        else
            ret = TRUE;
    }
#endif

    XBYTE[REG_Fm_evt] = 0x83; //clr
    XBYTE[REG_Fm_nandautoen_blocktype] &= 0xfe;//disable auto flow
    XBYTE[REG_Fm_dramfmread_dmasel] = 0x10; //back to dma sel

    /*Complete Nand operation*/
    MTD_NandCtrl(0x03);

    return ret;
	wait = wait;
}

UINT8 SMC_NandAutoWriteStart(
    UINT32 startAddr,
    UINT32 dramAddr, 
    UINT16 pageNum,
    UINT8 wait
)
{
    UINT8 ret = TRUE;
    UINT8 status;
    UINT32 timeout_count = 0xFFFF;
    const UINT8 cmdBuf[2] = {K_SMC_SequentialDataInput, K_SMC_PageProgram};    

    HAL_NANDChkRdy();
    if(G_NANDF_Category == 0)
        MTD_NandSendCmd(K_SMC_Read1);	//xian ++
    
    XBYTE[REG_Fm_evt] = 0x83;                      //clr nandautoint, nandrdyintrevt, nandintrevt  event 
    XBYTE[REG_Fm_dramfmread_dmasel] = 0x00;                       // [4]DRAM sel, [0]read mode
    XBYTE[REG_Fm_nandautoen_blocktype] |= 0x01;                      //enable auto flow

    XBYTE[REG_Fm_pagenum_Low]=READ8(pageNum, 1);
    XBYTE[REG_Fm_pagenum_High]=READ8(pageNum, 0)&3;
    
    XBYTE[REG_Fm_datstraddr_0] = READ8(dramAddr, 3);
    XBYTE[REG_Fm_datstraddr_1] = READ8(dramAddr, 2);
    XBYTE[REG_Fm_datstraddr_2] = READ8(dramAddr, 1);
    XBYTE[REG_Fm_datstraddr_3] = READ8(dramAddr, 0)&0x01;

    SMC_AutoCmdSet(cmdBuf, 2, startAddr);

    /*Enable ECC*/
    MTD_EccEnable();

    /*Clear ECC*/
    MTD_EccClear();

    
    /*Wait nand ready, it's in page program mode maybe*/
    HAL_NANDChkRdy();

    /*Check if auto command buffer ready*/
    if (XBYTE[REG_Fm_cmdsize_addrsize]) {
        /*Enable CE, Disable WP*/	
        MTD_NandCtrl(0x02);
        /*Trigger auto command/address mode*/
        MTD_NandAutoCmdTrig(); 
        /*Wait auto command/address finished*/
        //MTD_NandAutoCmdWait();
    }

    ENTER_CRITICAL(status);

    //XBYTE[REG_Fm_dramstr] = 1;  //triger
    while ((XBYTE[REG_Fm_evt] & 0x1) == 0x00)
    {
        if (timeout_count >	0)
        {
            timeout_count--;
        }
        else
        {
            //printf("%s %d, HAL_SDWaitIdle ERR\n", __FILE__, (UINT16)__LINE__);
            //printf("HAL_NandAutoWriteStart: %bx\n", XBYTE[REG_Fm_SD_freq_DBWitdh_RspType_TmEn_MMCmode]);
            ret= FALSE;
            break;
        }
    }
 
    /*Disable ECC*/
    MTD_EccDisable();	

    EXIT_CRITICAL(status);
    XBYTE[REG_Fm_evt] = 0xff; //clr
    XBYTE[REG_Fm_nandautoen_blocktype] &= 0xfe;//disable auto flow
    XBYTE[REG_Fm_dramfmread_dmasel] = 0x10; //back to dma sel
    
    /*Complete Nand operation*/
    MTD_NandCtrl(0x03);

    return ret;
	wait = wait;
}

void SMC_NandAutoFlowCfg(UINT32 eccAddrWord, UINT32 redundAddrWord)
{        
    SMC_NandSpareAddrSet(redundAddrWord);
    SMC_NandEccAddrSet(eccAddrWord);
    if(G_NANDF_Category){
        SMC_NandAutoFlowConfig(0x10, 4, 0x10000, 0x00);         
    }
    else{
        SMC_NandAutoFlowConfig(0x00, 1, 0x100, 0);       //??????
    }

   // printf("red/ecc dram: 0x%lx, 0x%lx\n",redundAddrWord, eccAddrWord);
}

#endif

