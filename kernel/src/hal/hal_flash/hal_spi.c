/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		HAL_spi.c

Abstract:

		Module related to HAL serial flash functions

Environment:

		Keil C51 Compiler

Revision History:

		08/09/2007    swyang    created
--*/
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
//#include "cardopt.h"
#include "hal.h"
#include "reg_def.h"

//patch5.2.1@ada@Different chip select pin for SMC begin
//#if (K_CARD_TYPE == 0x007C)
//#include "cardlink.h"
//#endif
//patch5.2.1@ada@Different chip select pin for SMC end

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
#if 1
/**
 * @fn        UINT8 HAL_SPIInit(UINT8 Freq, UINT8 Mode)
 * @brief     Serial flash interface initialization
 * @param     [in] Freq: SPI interface freq.
 *			   0- 24MHz, 1- 12MHz,  2- 6MHz,  3- 3MHz,
 *			   4- 1.5MHz 5- 750kHz, 6- 200kHz 7- 100kHz
 * @param     [in] Mode: spimode[3:0]
 *			   100- spi mode 0 and read data delay 1T (Modify by sony)
 *			   000- spi mode 0 and read data without delay
 *			   x01- spi mode 1
 *			   x10- spi mode 2
 *			   x11- spi mode 3
 *			   [3] is spisdi default value when host is reading from SPI.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_SPIInit(UINT8 Freq, UINT8 Mode) USING_0
{

		XBYTE[REG_Fm_SIfreq] = Freq;
		XBYTE[REG_Fm_SPImode] = Mode;
		XBYTE[0x2437] = 0x01; // SPICRCRst
		XBYTE[REG_Fm_SPIcsnn] = 0x00; // SPIcsnn

		XBYTE[REG_Fm_spipinmux] = 0x01;	//spi pin mux
		return HALK_SUCCESS;
}
#endif

/**
 * @fn        void HAL_SPIWritePort(UINT8 PortData)
 * @brief     Write the serial flash data port
 * @param     [in] PortData: write data
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_SPIWritePort(UINT8 PortData) USING_0
{

	UINT32 retry =0;
	XBYTE[REG_Fm_SPIpioWdata] = PortData;

	//polling SPIbusy
	while ((XBYTE[REG_Fm_SPIbusy] & 0x01) != 0x00)
	{
		retry++;
		if( retry > 100000 ){
    			printf("Fail.. %s %d\n", __FILE__, (UINT16)__LINE__);
		}
	}

}

/**
 * @fn        UINT8 HAL_SPIReadPort(void)
 * @brief     Read the serial flash data port
 * @param     NONE
 * @retval    return : read data
 * @see       NULL
 * @author    XianXin
*/
UINT8 HAL_SPIReadPort(void) USING_0
{
	UINT8 tmp0;
	UINT32 retry =0;
	tmp0 = XBYTE[REG_Fm_SPIpioRdata];  // prefetch
	//polling SPIbusy
	while ((XBYTE[REG_Fm_SPIbusy] & 0x01) != 0x00){
		retry++;
		if( retry > 100000 ){
    			printf("Fail.. %s %d\n", __FILE__, (UINT16)__LINE__);
			return FALSE;
		}
	}
	return XBYTE[REG_Fm_SPIpioRdataTmp];
}

/**
 * @fn        void HAL_SPIBusWidthSet(UINT8 busW)
 * @brief     Set spi bus width
 * @param     [in] busW : bus width(1bit,2bit,4bit)
 * @retval    NONE
 * @see       NULL
 * @author    XianXin
*/
void HAL_SPIBusWidthSet(UINT8 busW)
{
	if(busW == 4){
		XBYTE[REG_Fm_spibuswidth] = 0x02;	//buswidth
	}
	else if(busW == 2){
		XBYTE[REG_Fm_spibuswidth] = 0x01;	//buswidth
	}
	else{
		XBYTE[REG_Fm_spibuswidth] = 0x00;	 //buswidth
	}
}

//-----------------------------------------------------------------------------
//HAL_SPICRCRst
//-----------------------------------------------------------------------------
/*
routine description:
		Clear the SPI CRC buffer
arguments:
		None
return value:
		None
*/
#if 0
void HAL_SPICRCRst(void) USING_0
{
		XBYTE[0x2437] = 0x01;
}

//-----------------------------------------------------------------------------
//HAL_SPIReadCRC7
//-----------------------------------------------------------------------------
/*
routine description:
		Read the SPI CRC7 value
arguments:
		Value  : CRC7 buffer value
return value:
		None
*/
void HAL_SPIReadCRC7(UINT8* Value) USING_0
{

		*Value = XBYTE[0x2438];

}
#endif


