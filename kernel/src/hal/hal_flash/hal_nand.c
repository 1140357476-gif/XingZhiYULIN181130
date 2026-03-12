/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		HAL_nand.c

Abstract:

		Module related to HAL NAND flash functions

Environment:

		Keil C51 Compiler

Revision History:

		11/12/2001  	WZH    created  			   
--*/
//=============================================================================
//Header file
//=============================================================================
#define __FILE	__FILE_ID_HAL_NAND__

#include "general.h"
#include "hal.h"
#include "dbg_def.h"
#include "cardopt.h"

//patch5.2.1@ada@Different chip select pin for SMC begin
#if (K_CARD_TYPE == 0x007C)
#include "cardlink.h"
#endif
//patch5.2.1@ada@Different chip select pin for SMC end
#include "mtd_general.h"
#include "mtd_macro_api.h"
#include "doslink.h"
#include "reg_def.h"
//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
static BIT data sramflag;

//extern xdata   UINT8 G_SMC_AddrMode;
//=============================================================================
//Program   	
//=============================================================================

//-----------------------------------------------------------------------------
//HAL_NANDInit
//-----------------------------------------------------------------------------
#if 1
/**
 * @fn        UINT8 HAL_NANDInit(UINT8 ActiveCycle, UINT8 RecovCycle)
 * @brief     NAND gate flash interface initialization
 * @param     [in] ActiveCycle : the read/write strobe pulse widht in DMA
 *				0 - 15:  from 1T(20.83ns) to 16T(333.28ns)
 * @param     [in] RecovCycle : the recovery time in DMA
 *				0 - 15:  from 1T(20.83ns) to 16T(333.28ns)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_NANDInit(UINT8 ActiveCycle, UINT8 RecovCycle)
{
	UINT8 tmp;
	//PRINT_HAL("        HAL_NANDInit: Enter HAL_NANDInit(ActiveCycle=8'h%x,RecovCycle=8'h%x)\n",(UINT16)ActiveCycle,(UINT16)RecovCycle);
	ActiveCycle = ActiveCycle;
	RecovCycle = RecovCycle;
	//XBYTE[REG_Fm_Nand_Acttime_Rcvtime] = (RecovCycle<<4)|ActiveCycle;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle]&(WPBIT);
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02&(~WPBIT))|tmp;

	//PRINT_HAL("        HAL_NANDInit: Exit HAL_NANDInit\n");
	return HALK_SUCCESS;
}
#endif 
/**
 * @fn        UINT8 HAL_NANDSendCmd(UINT8 Cmd)
 * @brief     Do the send command phase
 * @param     [in] Cmd : the command to be sent
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_NANDSendCmd(UINT8 Cmd)
{
	UINT8 tmp;
	//PRINT_HAL("        HAL_NANDSendCmd: Enter HAL_NANDSendCmd(Cmd=8'h%x)\n",(UINT16)Cmd);

	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle]&(WPBIT);
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x0A & (~WPBIT))|tmp;
	XBYTE[REG_Fm_NandDataPort] = Cmd;
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;

	//PRINT_HAL("        HAL_NANDSendCmd: Exit HAL_NANDSendCmd\n");

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_NANDSendAddr(UINT8 Count, UINT32 Addr)
 * @brief     Do the NAND gate flash address phase
 * @param     [in] Count: the number of byte for address
 * @param     [in] Addr: the address to be sent 
 *				if Count = 1, Addr[7:0] is sent
 *		if Count = 2, Addr[7:0], Addr[15:8] is sent
 *		if Count = 3, Addr[7:0], Addr[15:8], Addr[23:16] is sent
 *		if Count = 4, Addr[7:0], Addr[15:8], Addr[23:16], Addr[31:24] is sent
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_NANDSendAddr(UINT8 Count, UINT32 Addr)
{
	UINT8 tmp;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle]&(WPBIT);
	//PRINT_HAL("        HAL_NANDSendAddr: Enter HAL_NANDSendAddr(Addr=32'h%lx)\n",Addr);

	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp;
	if (Count == 0)
		;
	else if (Count == 1)
		XBYTE[REG_Fm_NandDataPort] = (UINT8) Addr;
	else if (Count == 2) {
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 3);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 2);
	} else if (Count == 3) {
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 3);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 2);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 1);
	} else if (Count == 4) {
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 3);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 2);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 1);
		XBYTE[REG_Fm_NandDataPort] = READ8(Addr, 0);
	} else
		return 0x01;

	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;

	//PRINT_HAL("        HAL_NANDSendAddr: Exit HAL_NANDSendAddr\n");

	return HALK_SUCCESS;
}

/**
 * @fn        void HAL_NANDCompleteOperation(void)
 * @brief     when operation complete, disable NAND gate flash chip select pin
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_NANDCompleteOperation(void)   //ada@0227
{
	UINT8 tmp;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle]&(WPBIT);
	// CLE  ALE  /WP  /CE
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x03 & (~WPBIT))|tmp;   //   0    0    1	1
}

/**
 * @fn        void HAL_NANDEnableWriteProtect(void)
 * @brief     enable write protect
 * @param     NONE
 * @retval    NONE
 * @see       HAL_NANDWriteProtect
 * @author    XianXin
*/
void HAL_NANDEnableWriteProtect(UINT8 en)
{
#if ( K_NAND_WP_ENABLE == 1 )
	if ((XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01) != 0x00) {
		//Check Nand Rdy
		// CLE  ALE  /WP  /CE
		if(en)
		{
			XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] &= 0xfd; //   X    X    0    X
		}
		else
		{
			XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] |= 0x02; //   X    X    1    X
		}
	}
#else
	en=en;
#endif
	
}

#if 1
/**
 * @fn        void HAL_NANDWritePort(UINT8 PortData)
 * @brief     Write the NAND gate flash data port
 * @param     [in] PortData: write data
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_NANDWritePort(UINT8 PortData) 
{
		//PRINT_HAL("        HAL_NANDWritePort: Enter HAL_NANDWritePort(PortData=8'h%x)\n",(UINT16)PortData);

		XBYTE[REG_Fm_NandDataPort] = PortData;

		//PRINT_HAL("        HAL_NANDWritePort: Exit HAL_NANDWritePort\n");
}
#endif
/**
 * @fn        UINT8 HAL_NANDReadPort(void)
 * @brief     Read the NAND gate flash data port
 * @param     NONE
 * @retval    return : read data
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_NANDReadPort()
{
	UINT8 tmp0;
	
	tmp0 = XBYTE[REG_Fm_NandDataPort];  // prefetch
	tmp0 = XBYTE[REG_Fm_NandIpioRdata];
	
	return tmp0;
}

/**
 * @fn        UINT8 HAL_NANDChkRdy(void)
 * @brief     Check the NAND gate flash ready pin
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_NANDChkRdy(void)
{
	UINT16 timeOut = MTD_NAND_RDY_TIMEOUT;

	do {
		if (XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01) {
			return SUCCESS;
		}
	} while (--timeOut);

	/*reset nand flash*/
	MTD_NandTimeOutRst();
	ASSERT(0, 1);

	return FAIL;
}

/**
 * @fn        UINT8 HAL_NANDAutoCmdCfg(xdata* pCmd, UINT8 CmdCnt, xdata* pAddr, UINT8 AddrCnt)
 * @brief     config NAND auto command
 * @param     [in] pCmd : specify the command
 * @param     [in] CmdCnt : command bytes
 * @param     [in] pAddr : specify the address
 * @param     [in] AddrCnt : address bytes
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_NANDAutoCmdCfg(
	UINT8 xdata* pCmd,
	UINT8 CmdCnt,
	UINT8 xdata* pAddr,
	UINT8 AddrCnt
)
{
	UINT8 tmp;
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle]&(WPBIT);
	/*The max command count is two, max address count is six*/
	if (CmdCnt > 2 || AddrCnt > 6) {
		return FAIL;
	}

	XBYTE[REG_Fm_waiten] = 0x13;   	//nand auto cmd/addr act/rcv time, wait rdy pin enable
	
	XBYTE[REG_Fm_cmdsize_addrsize] = ((AddrCnt << 4) |CmdCnt);
	
	XBYTE[REG_Fm_cmdbuf_0] = pCmd[0];
	XBYTE[REG_Fm_cmdbuf_1] = pCmd[1];
	
	XBYTE[REG_Fm_addrbuf_0] = pAddr[0];
	XBYTE[REG_Fm_addrbuf_1] = pAddr[1];
	XBYTE[REG_Fm_addrbuf_2] = pAddr[2];
	XBYTE[REG_Fm_addrbuf_3] = pAddr[3];
	XBYTE[REG_Fm_addrbuf_4] = pAddr[4];
	XBYTE[REG_Fm_addrbuf_5] = pAddr[5];

	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;   //   0    0    1    0

	return SUCCESS;
}

/**
 * @fn        void HAL_NANDSetDRAMDMA(UINT32 DRAMAddrWord)
 * @brief     HAL_NANDSetDRAMDMA
 * @param     [in] DRAMAddrWord : dram address
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_NANDSetDRAMDMA(UINT32 DRAMAddrWord)
{
	XBYTE[REG_Dma_BaseAddrL] = READ8(DRAMAddrWord, 3);
	XBYTE[REG_Dma_BaseAddrML] = READ8(DRAMAddrWord, 2);
	XBYTE[REG_Dma_BaseAddrMH] = READ8(DRAMAddrWord, 1);
	XBYTE[REG_Dma_BaseAddrH] = READ8(DRAMAddrWord, 0) & 0x01;
}

/**
 * @fn        void HAL_NANDDoDMACfg(UINT8 Src, UINT8 Dst, UINT32 nByte, UINT8 FlashPageDMA)
 * @brief     HAL_NANDDoDMACfg
 * @param     [in] Src
 * @param     [in] Dst
 * @param     [in] nByte
 * @param     [in] FlashPageDMA
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_NANDDoDMACfg(UINT8 Src, UINT8 Dst, UINT32 nByte, UINT8 FlashPageDMA)
{
	if (Src == 0 && Dst == 2) {
		XBYTE[REG_Dma_DmaSrcDst] = 0x20;
	} else if (Src == 2 && Dst == 0) {
		XBYTE[REG_Dma_DmaSrcDst] = 0x02;
	} else {
		XBYTE[REG_Dma_DmaSrcDst] = (Dst << 4) | Src;
	}

	XBYTE[REG_Dma_DmaIdle] = 0x01;  // reset DMA machine
	XBYTE[REG_Dma_DmaIdle] = 0x00;
	XBYTE[REG_Dma_DmaCmp] = 0x01;

	if (FlashPageDMA) {
		/* No such function in 1628 */
		//XBYTE[REG_Dma_DmaSizeH] |= 0x02;  		  // padding 0's to flash
	}

	nByte--;
	XBYTE[REG_Dma_DmaSizeL] = READ8(nByte, 3);
	XBYTE[REG_Dma_DmaSizeM] = READ8(nByte, 2);
	XBYTE[REG_Dma_DmaSizeH] = READ8(nByte, 1);

	sramflag = 0;

	if (Src == 0x01 || Dst == 0x01) {
		XBYTE[REG_Cpu_MemEn] |= 0x07;	  	// enable sram
		XBYTE[REG_Cpu_SramMod] = 0x12;

		sramflag = 1;
	}
}

/**
 * @fn        UINT8 HAL_NANDDoDMAWait(void)
 * @brief     HAL_NANDDoDMAWait
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_NANDDoDMAWait(void)
{
	UINT32 timeOut;

	/* For dma quick check sts*/
	if (XBYTE[REG_Dma_DmaCmp]) {
		if (sramflag) {
			//XBYTE[REG_Cpu_MemEn] |= 0x07;
			XBYTE[REG_Cpu_SramMod] = 0x00;    	// set sram as normal operation
		}
		return SUCCESS;
	}
	
	timeOut = MTD_DMA_TIME_OUT;

	// wait for DMA complete
	do {
		if (XBYTE[REG_Dma_DmaCmp]) {
			if (sramflag) {
				//XBYTE[REG_Cpu_MemEn] |= 0x07;
				XBYTE[REG_Cpu_SramMod] = 0x00;    	// set sram as normal operation
			}
			return SUCCESS;
		}
	} while (--timeOut);

	XBYTE[REG_Dma_DmaCmp] = 0x01;

	if (sramflag) {
		//XBYTE[REG_Cpu_MemEn] |= 0x07;
		XBYTE[REG_Cpu_SramMod] = 0x00;    	// set sram as normal operation
	}

	ASSERT(0, 1);
	XBYTE[REG_Dma_DmaIdle] = 0x01;  // reset DMA machine
	XBYTE[REG_Dma_DmaIdle] = 0x00;

	return FAIL;
}

/**
 * @fn        void HAL_NANDCheckRdy(UINT8* Ready)
 * @brief     Check the NAND gate flash ready pin
 * @param     [in] Ready : 0:busy, 1:ready
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_NANDCheckRdy(UINT8* Ready)
{
	UINT8 tmp0;
	UINT16 timeOut = 6000;

	//PRINT_HAL("        HAL_NANDCheckRdy: Enter HAL_NANDCheckRdy\n");

	tmp0 = 0x00;

	while ((XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01) == 0x00) {
		timeOut--;
		if (timeOut == 0) {
			tmp0 = 0x02;
			//printf("no rdy\n");
			break;
		}
	}

	*Ready = tmp0;

	//PRINT_HAL("            HAL_NANDCheckRdy: Ready = 8'h%x (0 is for ready)\n",(UINT16)tmp0);
	//PRINT_HAL("        HAL_NANDCheckRdy: Exit HAL_NANDCheckRdy\n");
}

/**
 * @fn        void HAL_NANDWriteProtect(void)
 * @brief     HAL_NANDWriteProtect
 * @param     NONE
 * @retval    NONE
 * @see       HAL_NANDEnableWriteProtect
 * @author    XianXin
*/
void HAL_NANDWriteProtect(void)
{
	if(((XBYTE[REG_Fm_Nandrdy_wrnn_rdnn] & 0x01) == 0x01) && ((XBYTE[REG_Fm_Mediatype_rst] & 0x0f) == 0x01)){
		XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] &= 0xfd;//WP set to low
	}
}

/**
 * @fn        void HAL_NANDSramFlagSet(void)
 * @brief     HAL_NANDSramFlagSet
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_NANDSramFlagSet(void)
{
	sramflag = 1;
}

