/******************************************************************************\

          (c) Copyright Explore Semiconductor, Inc. Limited 2005
                           ALL RIGHTS RESERVED

--------------------------------------------------------------------------------

  File        :  DDC_If.c

  Description :  EP932E DDC Interface

\******************************************************************************/

#include <string.h>

//#include "general.h"
#include "DDC_If.h"
#include "EP932_If.h"

#include "i2c.h"

//--------------------------------------------------------------------------------------------------

#define HDCP_RX_ADDR          0x74     // HDCP RX Address
#define EDID_ADDR       		0xA0     // EDID Address
#define EDID_SEGMENT_PTR		0x60

#define HDCP_RX_BKSV_ADDR       0x00     // HDCP RX, BKSV Register Address
#define HDCP_RX_RI_ADDR         0x08     // HDCP RX, RI Register Address
#define HDCP_RX_AKSV_ADDR       0x10     // HDCP RX, AKSV Register Address
#define HDCP_RX_AINFO_ADDR      0x15     // HDCP RX, AINFO Register Address
#define HDCP_RX_AN_ADDR         0x18     // HDCP RX, AN Register Address
#define HDCP_RX_SHA1_HASH_ADDR  0x20     // HDCP RX, SHA-1 Hash Value Start Address
#define HDCP_RX_BCAPS_ADDR      0x40     // HDCP RX, BCAPS Register Address
#define HDCP_RX_BSTATUS_ADDR    0x41     // HDCP RX, BSTATUS Register Address
#define HDCP_RX_KSV_FIFO_ADDR   0x43     // HDCP RX, KSV FIFO Start Address

//--------------------------------------------------------------------------------------------------

// Private Data

//int i, j;
SMBUS_STATUS SMBUS_Sts;

unsigned char DDC_Data[128];
unsigned char TempBit;

static UINT8 code bDmy; /* add by Phil reserved for link file */


// Private Functions
SMBUS_STATUS DDC_Write(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size);
SMBUS_STATUS DDC_Read(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size);
UINT8 HDMI_i2c_write(UINT8 regaddr, UINT8 *pregdata, UINT8 cnt);
UINT8 HDMI_i2c_read(UINT8 regaddr, UINT8 *pregdata, UINT8 cnt);
//SMBUS_STATUS DDC_NoStop(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size);
//==================================================================================================
//
// Public Function Implementation
//

//--------------------------------------------------------------------------------------------------
// Hardware Interface


//--------------------------------------------------------------------------------------------------
//
// Downstream HDCP Control
//

unsigned char Downstream_Rx_read_BKSV(unsigned char *pBKSV)
{
	int i, j;
	status = DDC_Read(HDCP_RX_ADDR, HDCP_RX_BKSV_ADDR, pBKSV, 5);
	if(status != SMBUS_STATUS_Success) {
		DBG_printf(("ERROR: BKSV read - DN DDC %d\r\n", (int)status));
		return 0;
	}

	i = 0;
	j = 0;
	while (i < 5) {
		TempBit = 1;
		while (TempBit) {
			if (pBKSV[i] & TempBit) j++;
			TempBit <<= 1;
		}
		i++;
	}
	if(j != 20) {
		DBG_printf(("ERROR: BKSV read - Key Wrong\r\n"));
		DBG_printf(("ERROR: BKSV=0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\r\n", (unsigned int)pBKSV[0], (unsigned int)pBKSV[1], (unsigned int)pBKSV[2], (unsigned int)pBKSV[3], (unsigned int)pBKSV[4]));
		return 0;
	}
	return 1;
}

unsigned char Downstream_Rx_BCAPS(void)
{
	DDC_Read(HDCP_RX_ADDR, HDCP_RX_BCAPS_ADDR, DDC_Data, 1);
	return DDC_Data[0];
}

void Downstream_Rx_write_AINFO(char ainfo)
{
	DDC_Write(HDCP_RX_ADDR, HDCP_RX_AINFO_ADDR, &ainfo, 1);
}

void Downstream_Rx_write_AN(unsigned char *pAN)
{
	DDC_Write(HDCP_RX_ADDR, HDCP_RX_AN_ADDR, pAN, 8);
}

void Downstream_Rx_write_AKSV(unsigned char *pAKSV)
{
	DDC_Write(HDCP_RX_ADDR, HDCP_RX_AKSV_ADDR, pAKSV, 5);
}

unsigned char Downstream_Rx_read_RI(unsigned char *pRI)
{
	// Short Read
	status = DDC_Read(HDCP_RX_ADDR, HDCP_RX_RI_ADDR, pRI, 2);
	if(status != SMBUS_STATUS_Success) {
		DBG_printf(("ERROR: Rx Ri read - MCU IIC %d\r\n", (int)status));
		return 0;
	}
	return 1;
}

void Downstream_Rx_read_BSTATUS(unsigned char *pBSTATUS)
{
	DDC_Read(HDCP_RX_ADDR, HDCP_RX_BSTATUS_ADDR, pBSTATUS, 2);
}

void Downstream_Rx_read_SHA1_HASH(unsigned char *pSHA)
{

#if 1 /* modify for ep932 by phil */
	DDC_Read(HDCP_RX_ADDR, HDCP_RX_SHA1_HASH_ADDR, pSHA, 20);
#else	
	DDC_Read(HDCP_RX_ADDR, HDCP_RX_SHA1_HASH_ADDR, pSHA, 8);
	DDC_Read(HDCP_RX_ADDR, HDCP_RX_SHA1_HASH_ADDR+8, pSHA, 8);
	DDC_Read(HDCP_RX_ADDR, HDCP_RX_SHA1_HASH_ADDR+8, pSHA, 4);
#endif

}

// Retrive a 5 byte KSV at "Index" from FIFO
unsigned char Downstream_Rx_read_KSV_FIFO(unsigned char *pBKSV, unsigned char Index, unsigned char DevCount)
{
	int i, j;

	// Try not to re-read the previous KSV
	if(Index == 0) { // Start
		// Support a max 25 device count because of DDC_Data[] size is 128 byte
		status = DDC_Read(HDCP_RX_ADDR, HDCP_RX_KSV_FIFO_ADDR, DDC_Data, min(DevCount, 25));
	}
	memcpy(pBKSV, DDC_Data+(Index*5), 5);

	if(status != SMBUS_STATUS_Success) {
		DBG_printf(("ERROR: KSV FIFO read - DN DDC %d\r\n", (int)status));
		return 0;
	}

	i = 0;
	j = 0;
	while (i < 5) {
		TempBit = 1;
		while (TempBit) {
			if (pBKSV[i] & TempBit) j++;
			TempBit <<= 1;
		}
		i++;
	}
	if(j != 20) {
		DBG_printf(("ERROR: KSV FIFO read - Key Wrong\r\n"));
		return 0;
	}
	return 1;
}


//--------------------------------------------------------------------------------------------------
//
// Downstream EDID Control
//

unsigned char Downstream_Rx_poll_EDID(void)
{
	// Read the EDID test

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// How to implement this with CSI2C ?????????????????????????????
	// Without the Segment address implementation, it works normally.
	// But, this must be implemented for ATC test.
	//DDC_Data[0] = 0;	// Segment Pointer Address
	//SMBUS_master_rw_synchronous(DDC_Bus, EDID_SEGMENT_PTR, DDC_Data, 1, SMBUS_SkipStop);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// Base Address and Read 1
	status = DDC_Read(EDID_ADDR, 0, DDC_Data, 1);

	if(status != SMBUS_STATUS_Success) // can't read EDID
	{
		return 2;
	}
	if(DDC_Data[0] != 0x00)				// EDID header fail
	{
		return 2;
	}
	return 0;							// Read EDID success

}

EDID_STATUS Downstream_Rx_read_EDID(unsigned char *pEDID)
{
	int i;
	unsigned char seg_ptr, BlockCount, Block1Found, ChkSum, sys_ram[2];

	// =========================================================
	// I. Read the block 0

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// How to implement this with CSI2C ?????????????????????????????
	// Without the Segment address implementation, it works normally.
	// But, this must be implemented for ATC test.
	//DDC_Data[0] = 0;	// Segment Pointer Address
	//SMBUS_master_rw_synchronous(DDC_Bus, EDID_SEGMENT_PTR, DDC_Data, 1, SMBUS_SkipStop);
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// Base Address and Read 128
	//sys_ram[0] = 0;
	//DDC_NoStop(EDID_SEGMENT_PTR, 0, sys_ram, 1);	// skip stop 
	sys_ram[0] = sys_ram[0];
	
#if 1 /* modify for ep932 by phil */
	status = DDC_Read(EDID_ADDR, 0, pEDID, 128);
#else
	status = DDC_Read(EDID_ADDR, 0    , pEDID    , 8);	
	status = DDC_Read(EDID_ADDR, 0+8  , pEDID+8  , 8);
	status = DDC_Read(EDID_ADDR, 0+16 , pEDID+16 , 8);
	status = DDC_Read(EDID_ADDR, 0+24 , pEDID+24 , 8);
	status = DDC_Read(EDID_ADDR, 0+32 , pEDID+32 , 8);
	status = DDC_Read(EDID_ADDR, 0+40 , pEDID+40 , 8);
	status = DDC_Read(EDID_ADDR, 0+48 , pEDID+48 , 8);
	status = DDC_Read(EDID_ADDR, 0+56 , pEDID+56 , 8);
	status = DDC_Read(EDID_ADDR, 0+64 , pEDID+64 , 8);
	status = DDC_Read(EDID_ADDR, 0+72 , pEDID+72 , 8);
	status = DDC_Read(EDID_ADDR, 0+80 , pEDID+80 , 8);
	status = DDC_Read(EDID_ADDR, 0+88 , pEDID+88 , 8);
	status = DDC_Read(EDID_ADDR, 0+96 , pEDID+96 , 8);
	status = DDC_Read(EDID_ADDR, 0+104, pEDID+104, 8);
	status = DDC_Read(EDID_ADDR, 0+112, pEDID+112, 8);
	status = DDC_Read(EDID_ADDR, 0+120, pEDID+120, 8);
#endif
	
	if(status != SMBUS_STATUS_Success) {
		DBG_printf(("ERROR: EDID b0 read - DN DDC %d\r\n", (int)status));
		return status;
	}
	DBG_printf(("EDID b0 read:"));
	for(i=0; i<128; ++i) {
		if(i%16 == 0) DBG_printf(("\r\n"));
		if(i%8 == 0) DBG_printf((" "));
		DBG_printf(("0x%02X, ", (int)pEDID[i] ));
	}
	DBG_printf(("\r\n"));

	if( (pEDID[0] != 0x00) ||
	    (pEDID[1] != 0xFF) ||
	    (pEDID[2] != 0xFF) ||
	    (pEDID[3] != 0xFF) ||
	    (pEDID[4] != 0xFF) ||
	    (pEDID[5] != 0xFF) ||
	    (pEDID[5] != 0xFF) ||
	    (pEDID[7] != 0x00))
	{
		//return EDID_STATUS_NoAct;
	}

	// Check EDID
	if(pEDID[126] > 8) {
		DBG_printf(("ERROR: EDID Check failed, pEDID[126]=0x%02X > 8\n\r", (int)pEDID[126] ));
		return EDID_STATUS_ExtensionOverflow;
	}

	// =========================================================
	// II. Read other blocks and find Timing Extension Block

	BlockCount = pEDID[126];
	Block1Found = 0;
	for (seg_ptr = 1; seg_ptr <= BlockCount; ++seg_ptr) {

		/////////////////////////////////////////////////////////////////////////////////////////////////
		// How to implement this with Customer's I2C ?????????????????????????????
		// Without the Segment address implementation, it works normally.
		// But, this must be implemented for ATC test.
		//DDC_Data[0] = seg_ptr >> 1;	// Segment Pointer Address
		//SMBUS_master_rw_synchronous(DDC_Bus, EDID_SEGMENT_PTR, DDC_Data, 1, SMBUS_SkipStop);
		/////////////////////////////////////////////////////////////////////////////////////////////////

		// Base Address and Read 128
		//sys_ram[0] = seg_ptr >> 1;	// Segment Pointer Address
		//DDC_NoStop(EDID_SEGMENT_PTR, 0, sys_ram, 1);	// skip stop 
		
#if 1 /* modify for ep932 by phil */
		status = DDC_Read(EDID_ADDR, (seg_ptr & 0x01) << 7, DDC_Data, 128);
#else
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)	, DDC_Data	  , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+8	, DDC_Data+8  , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+16 , DDC_Data+16 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+24 , DDC_Data+24 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+32 , DDC_Data+32 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+40 , DDC_Data+40 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+48 , DDC_Data+48 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+56 , DDC_Data+56 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+64 , DDC_Data+64 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+72 , DDC_Data+72 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+80 , DDC_Data+80 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+88 , DDC_Data+88 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+96 , DDC_Data+96 , 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+104, DDC_Data+104, 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+112, DDC_Data+112, 128);
		status = DDC_Read(EDID_ADDR, ((seg_ptr & 0x01) << 7)+120, DDC_Data+120, 128);
#endif
		
		if(status != SMBUS_STATUS_Success) {
			DBG_printf(("ERROR: EDID bi read - DN DDC %d\r\n", (int)status));
			return status;
		}

		if(DDC_Data[0] == 0x02 && Block1Found == 0) {
			Block1Found = 1;
			memcpy(&pEDID[128], DDC_Data, 128);
		}

		DBG_printf(("EDID b%d read:", (int)seg_ptr));
		for(i=0; i<128; ++i) {
			if(i%16 == 0) DBG_printf(("\r\n"));
			if(i%8 == 0) DBG_printf((" "));
			DBG_printf(("0x%02X, ", (int)DDC_Data[i] ));
		}
		DBG_printf(("\r\n"));
	}

	// Check CheckSum
	ChkSum = 0;
	for(i=0; i<((Block1Found)?256:128); ++i) {
		ChkSum += pEDID[i];
	}
	if(ChkSum != 0) {
		return EDID_STATUS_ChecksumError;
	}
	if(Block1Found) {
		pEDID[126] = 1;
	}
	else {
		pEDID[126] = 0;
	}
	return EDID_STATUS_Success;
}

//==================================================================================================
//
// Private Functions
//

SMBUS_STATUS DDC_Write(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// How to implement this with Customer's I2C ?????????????????????????????
	// return 0; for success
	// return 2; for No_ACK
	// return 4; for Arbitration
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// need to use customer's I2C  function
	//result = I2C_WriteReg_EP932M(IICAddr, ByteAddr, Data, Size);

	UINT8 ret;

	//printf ("IIC write size =%x\n",Size);
	
	if (Size > 255) {
		printf("IIC wt over:%lx\n", Size); //size can't large than 255
		return 0;
	}

	i2cDeviceAddrSet(IICAddr);
	ret = HDMI_i2c_write(ByteAddr, Data, Size);
	if (ret != SUCCESS) {
		ret = 0x02; //no ack
	}

	return ret;
}

SMBUS_STATUS DDC_Read(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// How to implement this with CSI2C ?????????????????????????????
	// return 0; for success
	// return 2; for No_ACK
	// return 4; for Arbitration
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// need to use customer's I2C  function
	//result = TLGI2C_ReadReg_EP932M(IICAddr, ByteAddr, Data, Size);

	UINT8 ret;

	//printf ("IIC read size =%x\n",Size);
	
	if (Size > 255) {
		printf("IIC rd over:%lx\n", Size); //size can't large than 255
		return 0;
	}

	i2cDeviceAddrSet(IICAddr);
	ret = HDMI_i2c_read(ByteAddr, Data, Size);
	if (ret != SUCCESS) {
		ret = 0x02; //no ack
	}

	//printf(" read result =  %d, regaddr=%x\n",READ8(0xb0009248) , ByteAddr);

	return ret;

}

/*
SMBUS_STATUS DDC_NoStop(unsigned char IICAddr, unsigned char ByteAddr, void *Data, unsigned int Size)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// How to implement this with CSI2C ?????????????????????????????
	// return 0; for success
	// return 2; for No_ACK
	// return 4; for Arbitration
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int result = 1;
	
	// need to use customer's I2C  function
	//result = I2C_ReadReg_EP932M(IICAddr, ByteAddr, Data, Size);
	
	return result;
}
*/


/**
 * @fn        UINT8 HDMI_i2c_write(UINT8 regaddr, UINT8 *pregdata, UINT8 cnt)
 * @brief     HDMI_i2c_write
 * @param     [in] regaddr
 * @param     [in] pregdata
 * @param     [in] cnt
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-5-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HDMI_i2c_write(
	UINT8 regaddr,
	UINT8 *pregdata,
	UINT8 cnt
)
{

	UINT8 mode = 0; // 0: normal mode, 1: burst mode

	while(XBYTE[0x29a0]); //wait I2C busy
	
	XBYTE[0x2904] = mode;
	XBYTE[0x290c] = 0x00;
	
	/* set write count */
	XBYTE[0x2914] = 0x01;

	while(cnt) {
		
		/* set reg address */
		XBYTE[0x2930] = regaddr;		
		
		/* write data */
		//printf("w->%bx\n", *pregdata);
		XBYTE[0x2931] = *pregdata;
		
		/* wait I2C busy */
		while(XBYTE[0x29a0]); 

		regaddr++;
		pregdata++;
		cnt--;
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 HDMI_i2c_read(UINT8 regaddr, UINT8 *pregdata, UINT8 cnt)
 * @brief     HDMI_i2c_read
 * @param     [in] regaddr
 * @param     [in] pregdata
 * @param     [in] cnt
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-5-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HDMI_i2c_read(
	UINT8 regaddr,
	UINT8 *pregdata,
	UINT8 cnt
)
{
	UINT8 i;
	UINT8 rdCnt;
	UINT8 mode = 0; // 0: normal mode, 1: burst mode

	while(XBYTE[0x29a0]); //wait I2C busy

	rdCnt = 0;
	while(cnt) {
		
		XBYTE[0x2904] = mode;
		XBYTE[0x290c] = 0x00;

		/* set reg address */
		XBYTE[0x2930] = regaddr;
		
		/* set read count */
		if (cnt > 8) {
			XBYTE[0x2914] = 0; //0x08 << 4;
			rdCnt = 8;
			cnt -= rdCnt;
		} else {
			XBYTE[0x2914] = cnt << 4;
			rdCnt = cnt;
			cnt = 0;
		}
		
		/* trigger */
		XBYTE[0x2908] = 0x13; //restartMode = 1; subAddrEn = 1

		/* wait I2C busy */
		while(XBYTE[0x29a0]); 
		
		/* read the response data */
		for (i = 0; i < rdCnt; i++) {
			pregdata[i] = XBYTE[0x2931 + (i << 2)];
			//printf("pregdata[%bx]: %bx\n", i, pregdata[i]);
		}

		regaddr += rdCnt;
		pregdata += rdCnt;
	}

	return SUCCESS;
}


