/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		HAL_flash.c

Abstract:

		Module related to HAL flash functions

Environment:

		Keil C51 Compiler

Revision History:

		11/12/2001  	WZH    created

--*/
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "hal.h"
#include "hal_device.h"
#include "sd_def.h"
#include "sdfunc.h"
#include "gpio.h"
#include "reg_fm.h"

#define __FILE		__FILE_ID_HAL_FLASH__

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
//=============================================================================
//Program
//=============================================================================

//-----------------------------------------------------------------------------
//HAL_FlashMode
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 HAL_FlashMode(UINT8 ActFlash, UINT8 InactFlash, UINT8 PageSize)
 * @brief     Select flash type
 * @param     [in] ActFlash: active flash type
 *\n	0: None (fmgpio[17:0] is used as GPIO's)
 *\n	1: NAND gate flash
 *\n	5: EMMC
 *\n	6: SD
 *\n	7: SPI interface
 * @param     [in] InactFlash: inactive flash type
 *\n	0: None (only one storage media type used)
 *\n	1: NAND gate flash
 *\n	5: EMMC
 *\n	6: SD
 *\n	7: SPI interface
 * @param     [in] PageSize: page size of active flash
 *\n	0: 256 bytes/page
 *\n	1: 512 bytes/page
 *\n	2: 1024 bytes/page
 * @retval    return = SUCCESS / error code
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_FlashMode(UINT8 ActFlash, UINT8 InactFlash, UINT8 PageSize) USING_0
{
	devAttr_t* pDev = NULL;//add by phil for disk driver lib release

	if (ActFlash != 1) {
#if (FPGA_VERIFY != 1)
		/* add by phil for disk driver lib release s*/
		pDev = sp1kDevHdlGet();
		pDev += DEV_ID_DISK;
		if (pDev->devType == DEV_TYPE_DISK_NAND) {
			HAL_NANDChkRdy();
		}
		/* add by phil for disk driver lib release e*/
#endif        
	}
	#if SD_W_BG_EN
	//if ( ActFlash != 5 && ActFlash != 6 ) {
		sdBgCheckRdy();
	//}
	#endif

	switch (InactFlash) {
	case 0:
		break;
	case 1:
		XBYTE[REG_Fm_ctrlo_1] |= 0x01; XBYTE[REG_Fm_ctrloe_1] |= 0x01; break;
	case 5:
		XBYTE[REG_Fm_ctrlo_2] |= 0x01; XBYTE[REG_Fm_ctrloe_2] |= 0x01; break;
	case 6:
		XBYTE[REG_Fm_ctrlo_2] |= 0x01; XBYTE[REG_Fm_ctrloe_2] |= 0x01; break;
	case 7:
		XBYTE[REG_Fm_ctrlo_1] |= 0x01; XBYTE[REG_Fm_ctrloe_1] |= 0x01; break;
	default :
		return 0x01; break;
	}

	switch (ActFlash) {
	case 0:
		XBYTE[REG_Fm_Mediatype_rst] = 0x00; break;
	case 1:
		XBYTE[REG_Fm_Mediatype_rst] = 0x11; break;
	case 5:
		printf("eMMC!\n");
		XBYTE[REG_Fm_Mediatype_rst] = 0x45; XBYTE[REG_Fm_SD_CRCRst] = 0x01; break;
	case 6:
		//printf("SD!\n");
		XBYTE[REG_Fm_Mediatype_rst] = 0x46; XBYTE[REG_Fm_SD_CRCRst] = 0x01; break;
    case 7: 
    	XBYTE[REG_Fm_Mediatype_rst] = 0x24; //XBYTE[REG_Fm_SIfreq] = 0x00; XBYTE[REG_Fm_SPIcsnn] = 0x00; XBYTE[REG_Fm_SPImode] = 0x00; XBYTE[0x2437] = 0x01; break;
	default :
		return 0x01; break;
	}

	switch (PageSize) {
	case 0:
		XBYTE[REG_Fm_ECCMode] = 0x00; XBYTE[REG_Fm_DataLen_Low] = 0x00; XBYTE[REG_Fm_DataLen_High] = 0x01; break;
	case 1:
		XBYTE[REG_Fm_ECCMode] = 0x01; XBYTE[REG_Fm_DataLen_Low] = 0x00; XBYTE[REG_Fm_DataLen_High] = 0x02; break;
	case 2:
		XBYTE[REG_Fm_ECCMode] = 0x02; XBYTE[REG_Fm_DataLen_Low] = 0x00; XBYTE[REG_Fm_DataLen_High] = 0x04; break;
	default :
		return 0x01; break;
	}

	//PRINT_HAL("        HAL_FlashMode: Exit HAL_FlashMode\n");

	return HALK_SUCCESS;
}

//-----------------------------------------------------------------------------
//HAL_ReadECC
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 HAL_ReadECC(UINT8 ECCCount, UINT8* ECCData)
 * @brief     Read ECC code
 * @param     [in] ECCCount: the number of ECC code (3/6/12 is valid), otherwise invalid
 * @param     [out] ECCData: the ECC data
 * @retval    return = SUCCESS / error code
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_ReadECC(UINT8 ECCCount, UINT8* ECCData) USING_0 {
	//PRINT_HAL("        HAL_ReadECC: Enter HAL_ReadECC(ECC count=8'h%x)\n",(UINT16)ECCCount);

	if (ECCCount == 12) {
		ECCData[11] = XBYTE[REG_Fm_ECC_b]; ECCData[10] = XBYTE[REG_Fm_ECC_a]; ECCData[9] = XBYTE[REG_Fm_ECC_9];
		ECCData[8] = XBYTE[REG_Fm_ECC_8]; ECCData[7] = XBYTE[REG_Fm_ECC_7]; ECCData[6] = XBYTE[REG_Fm_ECC_6];
	}
	if ((ECCCount == 6) || (ECCCount == 12)) {
		ECCData[5] = XBYTE[REG_Fm_ECC_5]; ECCData[4] = XBYTE[REG_Fm_ECC_4]; ECCData[3] = XBYTE[REG_Fm_ECC_3];
	}
	if ((ECCCount == 6) || (ECCCount == 12) || (ECCCount == 3)) {
		ECCData[2] = XBYTE[REG_Fm_ECC_2]; ECCData[1] = XBYTE[REG_Fm_ECC_1]; ECCData[0] = XBYTE[REG_Fm_ECC_0];
	}

	//PRINT_HAL("        HAL_ReadECC: Exit HAL_ReadECC\n");

	return HALK_SUCCESS;
}

//-----------------------------------------------------------------------------
//HAL_ClearECC
//-----------------------------------------------------------------------------
/**
 * @fn        void HAL_ClearECC(void)
 * @brief     Restart ECC code calculation
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_ClearECC(void) USING_0 {
	//PRINT_HAL("        HAL_ClearECC: Enter HAL_ClearECC\n");
	XBYTE[REG_Fm_ECCRst] = 0x01;
	//PRINT_HAL("        HAL_ClearECC: Exit HAL_ClearECC\n");
}

//-----------------------------------------------------------------------------
//HAL_ECCMode
//-----------------------------------------------------------------------------
/**
 * @fn        void HAL_ECCMode(UINT8 Mode)
 * @brief     Enable or disable ECC generator
 * @param     [in] Mode: 
 *\n	0 enable ECC generator
 *\n	1 disable ECC generator
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_ECCMode(UINT8 Mode) USING_0 {
	//PRINT_HAL("        HAL_ECCMode: Enter HAL_ECCMode(Mode=8'h%x)\n",(UINT16)Mode);
	if (Mode)
		XBYTE[REG_Fm_ECCMask] = 0x01;
	else
		XBYTE[REG_Fm_ECCMask] = 0x00;
	//PRINT_HAL("        HAL_ECCMode: Exit HAL_ECCMode\n");
}

#ifdef TestModeEn

//-----------------------------------------------------------------------------
//HAL_TestFlash
//-----------------------------------------------------------------------------
/*
routine description:
  Flash module test.
arguments:
  TestLevel: the level of test

return value:
  0x00	 - success
  others - error
*/

UINT8 HAL_TestFlash(UINT8 TestLevel) USING_0 {
	UINT8 error, Detect, Ready, Status;
	UINT16 pagesize;
	UINT32 block, blocksize;
	UINT8 ByteData;
	UINT32 Addr;

	//  NAND
	UINT8 MarkerID;
	UINT8 DeviceID;

	UINT16 tmp;
	UINT8 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;

	if ((TestLevel == 0) || (TestLevel == 3)) {
		// NAND test ///////////////////////////////////////////////////////////////////////
		switch (HALK_FMactType) {
		case 1:
			if (HALK_FMPageSize == 0) {
				blocksize = (UINT32) HALK_FMSize << 12; pagesize = 256;
			} else if (HALK_FMPageSize == 1) {
				blocksize = (UINT32) HALK_FMSize << 11; pagesize = 512;
			} else {
				blocksize = (UINT32) HALK_FMSize << 10; pagesize = 1024;
			}

			HAL_FlashMode(1, HALK_FMinactType, 1);
			HAL_NANDInit(1, 1);

			// READ ID command
			L1_ReadNandID(&MarkerID, &DeviceID);
			printf("HAL_TestFlash: MarkerID = %x, DeviceID = %x\n", (UINT16) MarkerID, (UINT16) DeviceID);
			// READ ID command

			// Erase
			Addr = 0;
			if (HALK_FMPageSize == 0)
				Addr = Addr >> 8;
			else if (HALK_FMPageSize == 1)
				Addr = Addr >> 9;
			else if (HALK_FMPageSize == 2)
				Addr = Addr >> 10;
			else
				return 0x01;
			for (block = 0; block < blocksize; block += 16) {
				P1 = block;
				if (HALK_FMSize < 16)
					L1_EraseNandBlock(2, Addr, &Status);
				else
					L1_EraseNandBlock(3, Addr, &Status);
				if (Status != 0xE0)
					return 0x02;
				Addr += 16;
			}
			printf(" HAL_TestFlash: Erase Nand Type Flash Success\n");
			// Erase whole flash

			// PREPARE Data in CPU 4K SRAM
			XBYTE[REG_Cpu_MemEn] |= 0x02;
			for (tmp = 0x1000; tmp < (0x1000 + pagesize) ; tmp++)
				XBYTE[tmp] = tmp;
			// PREPARE Data in CPU 4K SRAM

			Addr = 0;
			error = 0;
			for (block = 0; block < blocksize; block++) {
				P1 = block;
				HAL_DmaSetSramAddr(0x0000);
				if (L1_DMAWrNAND(1, 8, Addr, 1, pagesize) != 0)
					error = 1;
				tmp = 0x0000 + pagesize;
				HAL_DmaSetSramAddr(tmp);
				if (L1_DMARdNAND(1, 8, Addr, 1, pagesize) != 0)
					error = 1;

				for (tmp = 0x1000; tmp < (0x1000 + pagesize); tmp++)
					if (XBYTE[tmp + pagesize] != XBYTE[tmp])
						error = 1;
				Addr += pagesize;
			}
			printf("HAL_TestFlash: Nand Type Flash data error = %x\n", (UINT16) error);

			break;

		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		default:
			return 0x01; break;
		}
	}

	return 0x00;
}

#endif

UINT8 HAL_FlashMode_Intr(UINT8 ActFlash, UINT8 InactFlash, UINT8 PageSize) USING_0
{
	devAttr_t* pDev = NULL;//add by phil for disk driver lib release

	if (ActFlash != 1) {
#if (FPGA_VERIFY != 1)
		/* add by phil for disk driver lib release s*/
		pDev = sp1kDevHdlGet();
		pDev += DEV_ID_DISK;
		if (pDev->devType == DEV_TYPE_DISK_NAND) {
			HAL_NANDChkRdy();
		}
		/* add by phil for disk driver lib release e*/
#endif        
	}
	#if SD_W_BG_EN
	//if ( ActFlash != 5 && ActFlash != 6 ) {
		sdBgCheckRdy();
	//}
	#endif

	switch (InactFlash) {
	case 0:
		break;
	case 1:
		XBYTE[REG_Fm_ctrlo_1] |= 0x01; XBYTE[REG_Fm_ctrloe_1] |= 0x01; break;
	case 5:
		XBYTE[REG_Fm_ctrlo_2] |= 0x01; XBYTE[REG_Fm_ctrloe_2] |= 0x01; break;
	case 6:
		XBYTE[REG_Fm_ctrlo_2] |= 0x01; XBYTE[REG_Fm_ctrloe_2] |= 0x01; break;
	case 7:
		XBYTE[REG_Fm_ctrlo_1] |= 0x01; XBYTE[REG_Fm_ctrloe_1] |= 0x01; break;
	default :
		return 0x01; break;
	}

	switch (ActFlash) {
	case 0:
		XBYTE[REG_Fm_Mediatype_rst] = 0x00; break;
	case 1:
		XBYTE[REG_Fm_Mediatype_rst] = 0x11; break;
	case 5:
		//printf("eMMC!\n");
		XBYTE[REG_Fm_Mediatype_rst] = 0x45; XBYTE[REG_Fm_SD_CRCRst] = 0x01; break;
	case 6:
		//printf("SD!\n");
		XBYTE[REG_Fm_Mediatype_rst] = 0x46; XBYTE[REG_Fm_SD_CRCRst] = 0x01; break;
    case 7: 
    	XBYTE[REG_Fm_Mediatype_rst] = 0x24; //XBYTE[REG_Fm_SIfreq] = 0x00; XBYTE[REG_Fm_SPIcsnn] = 0x00; XBYTE[REG_Fm_SPImode] = 0x00; XBYTE[0x2437] = 0x01; break;
	default :
		return 0x01; break;
	}

	switch (PageSize) {
	case 0:
		XBYTE[REG_Fm_ECCMode] = 0x00; XBYTE[REG_Fm_DataLen_Low] = 0x00; XBYTE[REG_Fm_DataLen_High] = 0x01; break;
	case 1:
		XBYTE[REG_Fm_ECCMode] = 0x01; XBYTE[REG_Fm_DataLen_Low] = 0x00; XBYTE[REG_Fm_DataLen_High] = 0x02; break;
	case 2:
		XBYTE[REG_Fm_ECCMode] = 0x02; XBYTE[REG_Fm_DataLen_Low] = 0x00; XBYTE[REG_Fm_DataLen_High] = 0x04; break;
	default :
		return 0x01; break;
	}

	//PRINT_HAL("        HAL_FlashMode: Exit HAL_FlashMode\n");

	return HALK_SUCCESS;
}

