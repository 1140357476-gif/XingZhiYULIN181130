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
 * @file		hal_gpio.c
 * @brief		Hal API for GPIO module
 * @author		kai.wang
 * @since		2010-01-01
 * @date		2010-01-01
 */

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_gpio.h" 
#include "hal_global.h"
#include "reg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define HAL_GPIO_BYTE_FUNC_SET_DBG     0
#define HAL_GPIO_BYTE_FUNC_GET_DBG     0
#define HAL_GPIO_BYTE_DIR_SET_DBG      0
#define HAL_GPIO_BYTE_DIR_GET_DBG      0
#define HAL_GPIO_BYTE_OUT_SET_DBG      0
#define HAL_GPIO_BYTE_OUT_GET_DBG      0
#define HAL_GPIO_BYTE_IN_GATE_SET_DBG  0
#define HAL_GPIO_BYTE_IN_GET_DBG       0
#define HAL_PULL_EN_SET_DBG            0
#define HAL_PULL_EN_GET_DBG            0 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/*Mux pin function selection*/
static code UINT16 HAL_GpioSelRegTbl[] = { 
	NULL,    /* HAL_GPIO_BYTE_NULL  */
	NULL,    /* HAL_GPIO_BYTE_GEN0  */
	NULL,    /* HAL_GPIO_BYTE_GEN1  */
	NULL,    /* HAL_GPIO_BYTE_GEN2  */
	NULL,    /* HAL_GPIO_BYTE_GEN3  */
	NULL,    /* HAL_GPIO_BYTE_FM0   */
	NULL,    /* HAL_GPIO_BYTE_FM1   */
	NULL,    /* HAL_GPIO_BYTE_FM2   */
	/*NULL,	  HAL_GPIO_BYTE_FM3   */
	0x2A60,  /* HAL_GPIO_BYTE_TG0   */
	0x2D80,  /* HAL_GPIO_BYTE_DISP0 */
	0x2D81,  /* HAL_GPIO_BYTE_DISP1 */
	0x2D82,  /* HAL_GPIO_BYTE_DISP2 */
	0x2D83,  /* HAL_GPIO_BYTE_DISP3 */
	NULL,    /* HAL_GPIO_BYTE_MA1   */
	NULL,    /* HAL_GPIO_BYTE_RGB0  */
	NULL,    /* HAL_GPIO_BYTE_RGB1  */
};

/* output enable. */
static code UINT16 HAL_GpioOenRegTbl[] = {
	NULL,    /* HAL_GPIO_BYTE_NULL  */
	0x2034,  /* HAL_GPIO_BYTE_GEN0  */
	0x2035,  /* HAL_GPIO_BYTE_GEN1  */
	0x2036,  /* HAL_GPIO_BYTE_GEN2  */
	0x2037,  /* HAL_GPIO_BYTE_GEN3  */
	NULL,//0x2413,  /* HAL_GPIO_BYTE_FM0   */
	NULL,//0x2414,  /* HAL_GPIO_BYTE_FM1   */
	NULL,//0x2415,  /* HAL_GPIO_BYTE_FM2   */
	/*NULL,     HAL_GPIO_BYTE_FM3   */
	0x2A62,  /* HAL_GPIO_BYTE_TG0   */
	0x2D88,  /* HAL_GPIO_BYTE_DISP0 */
	0x2D89,  /* HAL_GPIO_BYTE_DISP1 */
	0x2D8A,  /* HAL_GPIO_BYTE_DISP2 */
	0x2D8B,  /* HAL_GPIO_BYTE_DISP3 */
	NULL,    /* HAL_GPIO_BYTE_MA1   */
	NULL,    /* HAL_GPIO_BYTE_RGB0  */
	NULL,    /* HAL_GPIO_BYTE_RGB1  */
};

/* output. */
static code UINT16 HAL_GpioORegTbl[] = {
	NULL,    /* HAL_GPIO_BYTE_NULL  */
	0x2030,  /* HAL_GPIO_BYTE_GEN0  */
	0x2031,  /* HAL_GPIO_BYTE_GEN1  */
	0x2032,  /* HAL_GPIO_BYTE_GEN2  */
	0x2033,  /* HAL_GPIO_BYTE_GEN3  */
	NULL,//0x2416,  /* HAL_GPIO_BYTE_FM0   */
	NULL,//0x2417,  /* HAL_GPIO_BYTE_FM1   */
	NULL,//0x2418,  /* HAL_GPIO_BYTE_FM2   */
	/* NULL,	HAL_GPIO_BYTE_FM3   */
	0x2A64,  /* HAL_GPIO_BYTE_TG0   */
	0x2D84,  /* HAL_GPIO_BYTE_DISP0 */
	0x2D85,  /* HAL_GPIO_BYTE_DISP1 */
	0x2D86,  /* HAL_GPIO_BYTE_DISP2 */
	0x2D87,  /* HAL_GPIO_BYTE_DISP3 */
	NULL,    /* HAL_GPIO_BYTE_MA1   */
	NULL,    /* HAL_GPIO_BYTE_RGB0  */
	NULL,    /* HAL_GPIO_BYTE_RGB1  */
};

/* input gate. */
static code UINT16 HAL_GpioIgRegTbl[] = {
	NULL,    /* HAL_GPIO_BYTE_NULL  */
	NULL,    /* HAL_GPIO_BYTE_GEN0  */
	NULL,    /* HAL_GPIO_BYTE_GEN1  */
	NULL,    /* HAL_GPIO_BYTE_GEN2  */
	NULL,    /* HAL_GPIO_BYTE_GEN3  */
	NULL,    /* HAL_GPIO_BYTE_FM0   */
	NULL,    /* HAL_GPIO_BYTE_FM1   */
	NULL,    /* HAL_GPIO_BYTE_FM2   */
	/*NULL,    HAL_GPIO_BYTE_FM3   */
	0x2A5C,  /* HAL_GPIO_BYTE_TG0   */
	NULL,    /* HAL_GPIO_BYTE_DISP0 */
	NULL,    /* HAL_GPIO_BYTE_DISP1 */
	NULL,    /* HAL_GPIO_BYTE_DISP2 */
	NULL,    /* HAL_GPIO_BYTE_DISP3 */
	NULL,    /* HAL_GPIO_BYTE_MA1   */
	NULL,    /* HAL_GPIO_BYTE_RGB0  */
	NULL,    /* HAL_GPIO_BYTE_RGB1  */
};

/* input. */
static code UINT16 HAL_GpioIRegTbl[] = {
	NULL,    /* HAL_GPIO_BYTE_NULL  */
	0x2038,  /* HAL_GPIO_BYTE_GEN0  */
	0x2039,  /* HAL_GPIO_BYTE_GEN1  */
	0x203a,  /* HAL_GPIO_BYTE_GEN2  */
	0x203b,  /* HAL_GPIO_BYTE_GEN3  */
	NULL,//0x2410,  /* HAL_GPIO_BYTE_FM0   */
	NULL,//0x2411,  /* HAL_GPIO_BYTE_FM1   */
	NULL,//0x2412,  /* HAL_GPIO_BYTE_FM2   */
	/*NULL,	  HAL_GPIO_BYTE_FM3   */
	0x2A66,  /* HAL_GPIO_BYTE_TG0   */
	0x2D8C,  /* HAL_GPIO_BYTE_DISP0 */
	0x2D8D,  /* HAL_GPIO_BYTE_DISP1 */
	0x2D8E,  /* HAL_GPIO_BYTE_DISP2 */
	0x2D8F,  /* HAL_GPIO_BYTE_DISP3 */
	NULL,    /* HAL_GPIO_BYTE_MA1   */
	0x2A6C,  /* HAL_GPIO_BYTE_RGB0  */
	0x2A6D,  /* HAL_GPIO_BYTE_RGB1  */
};

static UINT8 HAL_GpioGen0FuncBuf = 0xE0;
static UINT8 HAL_GpioGen1FuncBuf = 0x01;

static UINT8 HAL_GpioMa1FuncBuf = 0x00;

static UINT8 HAL_GpioMa1DirBuf = 0x00;
static UINT8 HAL_GpioMa1OutBuf = 0x00; 

static UINT8 HAL_GpioFm0DirBuf = 0x00;
static UINT8 HAL_GpioFm1DirBuf = 0x00;
static UINT8 HAL_GpioFm2DirBuf = 0x00;

static UINT8 HAL_GpioFm0OutBuf = 0x00;
static UINT8 HAL_GpioFm1OutBuf = 0x00;
static UINT8 HAL_GpioFm2OutBuf = 0x00;

//static UINT8 HAL_GpioFm3DirBuf = 0x00;
//static UINT8 HAL_GpioFm3OutBuf = 0x00;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        void HAL_GpioByteFuncSet(UINT8 byteId, UINT8 msk, UINT8 func)
 * @brief     set gpio function in byte.
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] func: bitmap function, 0: function, 1: gpio.
 * @              Please Note that the FMGPIO can only set to gpio not function! 
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
HAL_GpioByteFuncSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 func
)
{
	UINT16 reg;


	#if HAL_GPIO_BYTE_FUNC_SET_DBG
	printf("HAL_GpioByteFuncSet() %bu 0x%02bX 0x%02bX\n", byteId, msk, func);
	#endif

	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		return;
	}

	switch ( byteId ) {
	case HAL_GPIO_BYTE_GEN0:
		if (( msk & 0x18 )&&((XBYTE[0x20f8] & 0x08) != 0x08 )) {  /* gpio[4:3] */
			/* set rxnselect & txnselect. */
			XBYTE[0x201A] = (XBYTE[0x201A] & ~((msk & 0x18) << 2)) | (((~func) & msk & 0x18) << 2);
		}
		else if ( msk & 0x10 ) {  /*gpio[4]*/
			/* set pwm0select */
			if (func & msk & 0x10) {
				XBYTE[0x201B] &= ~0x01; /* set to gpio */ 
			}
			else {
				XBYTE[0x201B] |= 0x01; /* set to function */ 
			}
		}
		if ( msk & 0x20 ) {  /* gpio[5] */
			/* set pwm1select */
			if ( func & msk & 0x20 ) {
				XBYTE[0x201B] &= ~0x02;  /* set to gpio. */
			}
			else {
				XBYTE[0x201B] |= 0x02;  /* set to function. */
			}
		}

		if ( msk & 0x40 ) {  /* gpio[6] */
			/* set pwm2select. */
			if ( func & msk & 0x40 ) {
				XBYTE[0x201B] &= ~0x04;  /* set to gpio. */
			}
			else {
				XBYTE[0x201B] |= 0x04;  /* set to function. */
			}
		}

		if ( msk & 0x80) {  /* gpio[7] */
			/* set pwm0select. */
			if ( func & msk & 0x80 ) {
				XBYTE[0x201B] &= ~0x01;  /* set to gpio. */
			}
			else {
				XBYTE[0x201B] |= 0x01;  /* set to function. */
			}
		}

		HAL_GpioGen0FuncBuf = (HAL_GpioGen0FuncBuf & ~msk) | (func & msk);

		break;

	case HAL_GPIO_BYTE_GEN1:
		
		if ( msk & 0x40 ) {  /* gpio[14] */
			/* set PWM3select. */
			if ( func & msk & 0x40 ) {
				XBYTE[0x201B] &= ~0x08;  /* set to gpio. */
			}
			else {
				XBYTE[0x201B] |= 0x08;  /* set to function. */
			}
			//XBYTE[0x201A] = (XBYTE[0x201A] & ~((msk & 0xC0) >> 6)) | ((~func & msk & 0xC0) >> 6);
		}

		HAL_GpioGen1FuncBuf = (HAL_GpioGen1FuncBuf & ~msk) | (func & msk);

		break;
		
	case HAL_GPIO_BYTE_MA1:
		if ( msk & 0x40 ) {  /* ma[14] */
			/* set Ma14value_en. */
			if ( func & msk & 0x40 ) {
				XBYTE[REG_Dram_Dummy0] |= 0x02;	/* set to gpio. */
			}
			else {
				XBYTE[REG_Dram_Dummy0] &= ~0x02;  /* set to function. */
			}
		}

		HAL_GpioMa1FuncBuf = (HAL_GpioMa1FuncBuf & ~msk) | (func & msk);

		break;
	case HAL_GPIO_BYTE_FM0:
	case HAL_GPIO_BYTE_FM1:
	case HAL_GPIO_BYTE_FM2:
	//case HAL_GPIO_BYTE_FM3:
		if(func & msk) {
			XBYTE[REG_Fm_Mediatype_rst] = 0x00; //all the flash memory control pin are uses as GPIOS
		}
		break;	
	default:
		reg = HAL_GpioSelRegTbl[byteId];
		if ( reg != NULL ) {
			XBYTE[reg] = (XBYTE[reg] & ~msk) | (func & msk);
		}
	}
}


/**
 * @fn        UINT8 HAL_GpioByteFuncGet(UINT8 byteId, UINT8 msk)
 * @brief     get gpio function in byte.
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap function value, 0: function, 1: gpio.
 * @see       NULL
 * @author    Matt Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
HAL_GpioByteFuncGet(
	UINT8 byteId,
	UINT8 msk
)
{
	UINT8 ret;
	UINT16 reg;


	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		ret = 0;
	}
	else {
		switch ( byteId ) {
		case HAL_GPIO_BYTE_GEN0:
			ret = HAL_GpioGen0FuncBuf;

			break;

		case HAL_GPIO_BYTE_GEN1:
			ret = HAL_GpioGen1FuncBuf;

			break;

		case HAL_GPIO_BYTE_MA1:
			ret = HAL_GpioMa1FuncBuf;

			break;
		case HAL_GPIO_BYTE_FM0:
		case HAL_GPIO_BYTE_FM1:
		case HAL_GPIO_BYTE_FM2:
		//case HAL_GPIO_BYTE_FM3:
			if (XBYTE[REG_Fm_Mediatype_rst] !=0x00) {  //as function
				ret = 0x00;
			}
			else { 					//as gpio	
				ret = 0xff;
			}
			break;
		default:
			reg = HAL_GpioSelRegTbl[byteId];
			if ( reg == NULL ) {
				ret = 0;
			}
			else {
				ret = XBYTE[reg];
			}
		}

		ret &= msk;
	}

	#if HAL_GPIO_BYTE_FUNC_GET_DBG
	printf("HAL_GpioByteFuncGet %bu 0x%02bX 0x%02bX\n", byteId, msk, ret);
	#endif

	return ret;
}


/**
 * @fn        void HAL_GpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir)
 * @brief     set gpio direction in byte.
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] dir: bitmap direction, 0: input, 1: output.
 * @retval    NONE
 * @see       NULL
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
HAL_GpioByteDirSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 dir
)
{
	UINT16 reg;
	UINT8 tmpmsk;

	#if HAL_GPIO_BYTE_DIR_SET_DBG
	printf("HAL_GpioByteDirSet() %bu 0x%02bX 0x%02bX\n", byteId, msk, dir);
	#endif

	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		return;
	}

	switch ( byteId ) {
	case HAL_GPIO_BYTE_MA1:
		if ( msk & 0x40 ) {  /* ma[14] */
			/* set ma14oe_n. */
			if ( dir & msk & 0x40 ) {
				XBYTE[REG_Dram_SdckMode] &= ~0x40;  /* set to output. */
			}
			else {
				XBYTE[REG_Dram_SdckMode] |= 0x40;  /* set to input. */
			}
		}

		HAL_GpioMa1DirBuf = (HAL_GpioMa1DirBuf & ~msk) | (dir & msk);

		break;
	case HAL_GPIO_BYTE_FM0:
		if (msk & 0x0F) {  /*fmgpio[3:0]*/
			tmpmsk = (dir & msk & 0x0F);
		
			XBYTE[REG_Fm_ctrloe_1] = (XBYTE[REG_Fm_ctrloe_1]& ~(msk & 0x0F)) | (tmpmsk);
		}
		if (msk & 0xF0) {  /*fmgpio[7:4]*/
			tmpmsk = (dir & msk & 0xF0);
		
			XBYTE[REG_Fm_ctrloe_2] = (XBYTE[REG_Fm_ctrloe_2] & ~((msk&0xF0)>>4)) | (tmpmsk>>4);
		}
		
		HAL_GpioFm0DirBuf = (HAL_GpioFm0DirBuf & ~msk) | (dir & msk);
		break;
	case HAL_GPIO_BYTE_FM1:	
		if (msk & 0x03) {  /*fmgpio[9:8]*/
			tmpmsk = (dir & msk & 0x03);
			
			XBYTE[REG_Fm_ctrloe_2] = (XBYTE[REG_Fm_ctrloe_2] & ~((msk&0x03)<<4)) | (tmpmsk<<4);
		}
		if (msk & 0xFC ) {  /*fmgpio[15:10]*/
			tmpmsk = (dir & msk & 0xFC);
			
		XBYTE[REG_Fm_ctrloe_3] = (XBYTE[REG_Fm_ctrloe_3] & ~((msk&0xFC)>>2)) | (tmpmsk>>2);	
		}
		HAL_GpioFm1DirBuf = (HAL_GpioFm1DirBuf & ~msk) | (dir & msk);
		break;
	case HAL_GPIO_BYTE_FM2:
		if (msk & 0x03) {  /*fmgpio[17:16]*/
			tmpmsk = (dir & msk & 0x03);
			
			XBYTE[REG_Fm_ctrloe_3] = (XBYTE[REG_Fm_ctrloe_3] & ~((msk&0x03)<<6)) | (tmpmsk<<6);
		}
		if (msk & 0x0C ) {  /*fmgpio[19:18]*/
			tmpmsk = (dir & msk & 0x0C);
			
			XBYTE[REG_Fm_ctrloe_1] = (XBYTE[REG_Fm_ctrloe_1] & ~((msk&0x0C)<<2)) | (tmpmsk<<2);
		}
		HAL_GpioFm2DirBuf = (HAL_GpioFm2DirBuf & ~msk) | (dir & msk);
		break;
#if 0
	case HAL_GPIO_BYTE_FM3:
		if ( msk & 0x01 ) {  /*fmgpio[18]*/
			/* set fmgpio[18]oe_n. */
			if ( dir & msk & 0x01 ) {  
				XBYTE[REG_Fm_ctrloe_1] |= 0x10;
			}
			else {
				XBYTE[REG_Fm_ctrloe_1] &= ~0x10;
			}
		}
		if ( msk & 0x02 ) {  /*fmgpio[19]*/
			/* set fmgpio[19]oe_n. */
			if ( dir & msk & 0x02 ) {  
				XBYTE[REG_Fm_ctrloe_1] |= 0x20;
			}
			else {
				XBYTE[REG_Fm_ctrloe_1] &= ~0x20;
			}
		}
		HAL_GpioFm3DirBuf = (HAL_GpioFm3DirBuf & ~msk) | (dir & msk);
		
		break;
#endif		
	default:
		reg = HAL_GpioOenRegTbl[byteId];
		if ( reg != NULL ) {
			XBYTE[reg] = (XBYTE[reg] & ~msk) | (dir & msk);
		}
	}
}

/**
 * @fn        void HAL_GpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir)
 * @brief     set gpio direction in byte used in interrupter server.
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] dir: bitmap direction, 0: input, 1: output.
 * @retval    NONE
 * @see       NULL
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_GpioByteDirSet_Intr(
	UINT8 byteId,
	UINT8 msk,
	UINT8 dir
)
{
	UINT16 reg;
	UINT8 tmpmsk;

	#if HAL_GPIO_BYTE_DIR_SET_DBG
	printf("HAL_GpioByteDirSet() %bu 0x%02bX 0x%02bX\n", byteId, msk, dir);
	#endif

	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		return;
	}

	switch ( byteId ) {
	case HAL_GPIO_BYTE_MA1:
		if ( msk & 0x40 ) {  /* ma[14] */
			/* set ma14oe_n. */
			if ( dir & msk & 0x40 ) {
				XBYTE[REG_Dram_SdckMode] &= ~0x40;  /* set to output. */
			}
			else {
				XBYTE[REG_Dram_SdckMode] |= 0x40;  /* set to input. */
			}
		}

		HAL_GpioMa1DirBuf = (HAL_GpioMa1DirBuf & ~msk) | (dir & msk);

		break;
	case HAL_GPIO_BYTE_FM0:
		if (msk & 0x0F) {  /*fmgpio[3:0]*/
			tmpmsk = (dir & msk & 0x0F);
		
			XBYTE[REG_Fm_ctrloe_1] = (XBYTE[REG_Fm_ctrloe_1]& ~(msk & 0x0F)) | (tmpmsk);
		}
		if (msk & 0xF0) {  /*fmgpio[7:4]*/
			tmpmsk = (dir & msk & 0xF0);
		
			XBYTE[REG_Fm_ctrloe_2] = (XBYTE[REG_Fm_ctrloe_2] & ~((msk&0xF0)>>4)) | (tmpmsk>>4);
		}
		
		HAL_GpioFm0DirBuf = (HAL_GpioFm0DirBuf & ~msk) | (dir & msk);
		break;
	case HAL_GPIO_BYTE_FM1:	
		if (msk & 0x03) {  /*fmgpio[9:8]*/
			tmpmsk = (dir & msk & 0x03);
			
			XBYTE[REG_Fm_ctrloe_2] = (XBYTE[REG_Fm_ctrloe_2] & ~((msk&0x03)<<4)) | (tmpmsk<<4);
		}
		if (msk & 0xFC ) {  /*fmgpio[15:10]*/
			tmpmsk = (dir & msk & 0xFC);
			
		XBYTE[REG_Fm_ctrloe_3] = (XBYTE[REG_Fm_ctrloe_3] & ~((msk&0xFC)>>2)) | (tmpmsk>>2);	
		}
		HAL_GpioFm1DirBuf = (HAL_GpioFm1DirBuf & ~msk) | (dir & msk);
		break;
	case HAL_GPIO_BYTE_FM2:
		if (msk & 0x03) {  /*fmgpio[17:16]*/
			tmpmsk = (dir & msk & 0x03);
			
			XBYTE[REG_Fm_ctrloe_3] = (XBYTE[REG_Fm_ctrloe_3] & ~((msk&0x03)<<6)) | (tmpmsk<<6);
		}
		if (msk & 0x0C ) {  /*fmgpio[19:18]*/
			tmpmsk = (dir & msk & 0x0C);
			
			XBYTE[REG_Fm_ctrloe_1] = (XBYTE[REG_Fm_ctrloe_1] & ~((msk&0x0C)<<2)) | (tmpmsk<<2);
		}
		HAL_GpioFm2DirBuf = (HAL_GpioFm2DirBuf & ~msk) | (dir & msk);
		break;
#if 0		
	case HAL_GPIO_BYTE_FM3:
		if ( msk & 0x01 ) {  /*fmgpio[18]*/
			/* set fmgpio[18]oe_n. */
			if ( dir & msk & 0x01 ) {  
				XBYTE[REG_Fm_ctrloe_1] |= 0x10;
			}
			else {
				XBYTE[REG_Fm_ctrloe_1] &= ~0x10;
			}
		}
		if ( msk & 0x02 ) {  /*fmgpio[19]*/
			/* set fmgpio[19]oe_n. */
			if ( dir & msk & 0x02 ) {  
				XBYTE[REG_Fm_ctrloe_1] |= 0x20;
			}
			else {
				XBYTE[REG_Fm_ctrloe_1] &= ~0x20;
			}
		}
		HAL_GpioFm3DirBuf = (HAL_GpioFm3DirBuf & ~msk) | (dir & msk);
		
		break;
#endif		
	default:
		reg = HAL_GpioOenRegTbl[byteId];
		if ( reg != NULL ) {
			XBYTE[reg] = (XBYTE[reg] & ~msk) | (dir & msk);
		}
	}
}

/**
 * @fn        UINT8 HAL_GpioByteDirGet(UINT8 byteId, UINT8 msk)
 * @brief     get gpio direction in byte.
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap direction value.
 * @see       NULL
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
HAL_GpioByteDirGet(
	UINT8 byteId,
	UINT8 msk
)
{
	UINT8 ret;
	UINT16 reg;


	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		ret = 0;
	}
	else {
		switch ( byteId ) {
		case HAL_GPIO_BYTE_MA1:
			ret = HAL_GpioMa1DirBuf;

			break;
		case HAL_GPIO_BYTE_FM0:
			ret = HAL_GpioFm0DirBuf;
			break;
			
		case HAL_GPIO_BYTE_FM1:
			ret = HAL_GpioFm1DirBuf;
			break;
			
		case HAL_GPIO_BYTE_FM2:
			ret = HAL_GpioFm2DirBuf;
			break;	
		//case HAL_GPIO_BYTE_FM3:
		//	ret = HAL_GpioFm3DirBuf;
		//	break;
		default:
			reg = HAL_GpioOenRegTbl[byteId];
			if ( reg == NULL ) {
				ret = 0;
			}
			else {
				ret = XBYTE[reg];
			}
		}

		ret &= msk;
	}

	#if HAL_GPIO_BYTE_DIR_GET_DBG
	printf("HAL_GpioByteDirGet %bu 0x%02bX 0x%02bX\n", byteId, msk, ret);
	#endif

	return ret;
}

/**
 * @fn        void HAL_GpioByteOutSet(UINT8 byteId, UINT8 msk, UINT8 val)
 * @brief     set gpio output in byte.
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] val: bitmap output value.
 * @retval    NONE
 * @see       NULL
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
HAL_GpioByteOutSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 val
)
{
	UINT16 reg;
	UINT8 tmpmsk;

	#if HAL_GPIO_BYTE_OUT_SET_DBG
	printf("HAL_GpioByteOutSet() %bu 0x%02bX 0x%02bX\n", byteId, msk, val);
	#endif

	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		return;
	}

	switch ( byteId ) {

	case HAL_GPIO_BYTE_MA1:
		if ( msk & 0x40 ) {  /* ma[14] */
			/* set Ma14value. */
			if ( val & msk & 0x40 ) {
				XBYTE[REG_Dram_Dummy0] |= 0x01;  /* set to high. */
			}
			else {
				XBYTE[REG_Dram_Dummy0] &= ~0x01;  /* set to low. */
			}
		}

		HAL_GpioMa1OutBuf = (HAL_GpioMa1OutBuf & ~msk) | (val & msk);

		break;
	case HAL_GPIO_BYTE_FM0:
		if (msk & 0x0F) {  /*fmgpio[3:0]*/
			tmpmsk = (val & msk & 0x0F);
			
			XBYTE[REG_Fm_ctrlo_1] = (XBYTE[REG_Fm_ctrlo_1]& ~(msk & 0x0F)) | (tmpmsk);

		}
		if (msk & 0xF0) {  /*fmgpio[7:4]*/
			tmpmsk = (val & msk & 0xF0);
			
			XBYTE[REG_Fm_ctrlo_2] = (XBYTE[REG_Fm_ctrlo_2] & ~((msk&0xF0)>>4)) | (tmpmsk>>4);
		}
		
		HAL_GpioFm0OutBuf = (HAL_GpioFm0OutBuf & ~msk) | (val & msk);
		break;	
	case HAL_GPIO_BYTE_FM1:	
		if (msk & 0x03) {  /*fmgpio[9:8]*/
			tmpmsk = (val & msk & 0x03);
			
			XBYTE[REG_Fm_ctrlo_2] = (XBYTE[REG_Fm_ctrlo_2] & ~((msk&0x03)<<4)) | (tmpmsk<<4);
		}
		if (msk & 0xFC ) {  /*fmgpio[15:10]*/
			tmpmsk = (val & msk & 0xFC);
			
			XBYTE[REG_Fm_ctrlo_3] = (XBYTE[REG_Fm_ctrlo_3] & ~((msk&0xFC)>>2)) | (tmpmsk>>2);
		}
		HAL_GpioFm1OutBuf = (HAL_GpioFm1OutBuf & ~msk) | (val & msk);
		break;
	case HAL_GPIO_BYTE_FM2:
		if (msk & 0x03) {  /*fmgpio[17:16]*/
			tmpmsk = (val & msk & 0x03);
			
			XBYTE[REG_Fm_ctrlo_3] = (XBYTE[REG_Fm_ctrlo_3] & ~((msk&0x03)<<6)) | (tmpmsk<<6);
		}
		if (msk & 0x0C ) {  /*fmgpio[19:18]*/
			tmpmsk = (val & msk & 0x0C);
			
			XBYTE[REG_Fm_ctrlo_1] = (XBYTE[REG_Fm_ctrlo_1] & ~((msk&0x0C)<<2)) | (tmpmsk<<2);
		}
		HAL_GpioFm2OutBuf = (HAL_GpioFm2OutBuf & ~msk) | (val & msk);
		break;	
#if 0		
	case HAL_GPIO_BYTE_FM3:
		if ( msk & 0x01 ) {  /* fmgpio[18] */
			/* set fmgpio[18] value. */
			if ( val & msk & 0x01 ) {
				XBYTE[REG_Fm_ctrlo_1] |= 0x10;  /* set to high. */
			}
			else {
				XBYTE[REG_Fm_ctrlo_1] &= ~0x10;  /* set to low. */
			}
		}
		if ( msk & 0x02 ) {  /* fmgpio[19] */
			/* set fmgpio[19] value. */
			if ( val & msk & 0x01 ) {
				XBYTE[REG_Fm_ctrlo_1] |= 0x20;  /* set to high. */
			}
			else {
				XBYTE[REG_Fm_ctrlo_1] &= ~0x20;  /* set to low. */
			}
		}
		HAL_GpioFm3OutBuf = (HAL_GpioFm3OutBuf & ~msk) | (val & msk);
		break;
#endif		
	default:
		reg = HAL_GpioORegTbl[byteId];
		if ( reg != NULL ) {
			XBYTE[reg] = (XBYTE[reg] & ~msk) | (val & msk);
		}
	}
}


/**
 * @fn        void HAL_GpioByteOutSet_Intr(UINT8 byteId, UINT8 msk, UINT8 val)
 * @brief     set gpio output in byte used in interrupter server
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] val: bitmap output value.
 * @retval    NONE
 * @see       NULL
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
HAL_GpioByteOutSet_Intr(
	UINT8 byteId,
	UINT8 msk,
	UINT8 val
)
{
	UINT16 reg;
	UINT8 tmpmsk;

	#if HAL_GPIO_BYTE_OUT_SET_DBG
	printf("HAL_GpioByteOutSet() %bu 0x%02bX 0x%02bX\n", byteId, msk, val);
	#endif

	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		return;
	}

	switch ( byteId ) {
	case HAL_GPIO_BYTE_MA1:
		if ( msk & 0x40 ) {  /* ma[14] */
			/* set Ma14value. */
			if ( val & msk & 0x40 ) {
				XBYTE[REG_Dram_Dummy0] |= 0x01;  /* set to high. */
			}
			else {
				XBYTE[REG_Dram_Dummy0] &= ~0x01;  /* set to low. */
			}
		}

		HAL_GpioMa1OutBuf = (HAL_GpioMa1OutBuf & ~msk) | (val & msk);

		break;
		case HAL_GPIO_BYTE_FM0:
			if (msk & 0x0F) {  /*fmgpio[3:0]*/
				tmpmsk = (val & msk & 0x0F);
				
				XBYTE[REG_Fm_ctrlo_1] = (XBYTE[REG_Fm_ctrlo_1]& ~(msk & 0x0F)) | (tmpmsk);
	
			}
			if (msk & 0xF0) {  /*fmgpio[7:4]*/
				tmpmsk = (val & msk & 0xF0);
				
				XBYTE[REG_Fm_ctrlo_2] = (XBYTE[REG_Fm_ctrlo_2] & ~((msk&0xF0)>>4)) | (tmpmsk>>4);
			}
			
			HAL_GpioFm0OutBuf = (HAL_GpioFm0OutBuf & ~msk) | (val & msk);
			break;	
		case HAL_GPIO_BYTE_FM1: 
			if (msk & 0x03) {  /*fmgpio[9:8]*/
				tmpmsk = (val & msk & 0x03);
				
				XBYTE[REG_Fm_ctrlo_2] = (XBYTE[REG_Fm_ctrlo_2] & ~((msk&0x03)<<4)) | (tmpmsk<<4);
			}
			if (msk & 0xFC ) {  /*fmgpio[15:10]*/
				tmpmsk = (val & msk & 0xFC);
				
				XBYTE[REG_Fm_ctrlo_3] = (XBYTE[REG_Fm_ctrlo_3] & ~((msk&0xFC)>>2)) | (tmpmsk>>2);
			}
			HAL_GpioFm1OutBuf = (HAL_GpioFm1OutBuf & ~msk) | (val & msk);
			break;
		case HAL_GPIO_BYTE_FM2:
			if (msk & 0x03) {  /*fmgpio[17:16]*/
				tmpmsk = (val & msk & 0x03);
				
				XBYTE[REG_Fm_ctrlo_3] = (XBYTE[REG_Fm_ctrlo_3] & ~((msk&0x03)<<6)) | (tmpmsk<<6);
			}
			if (msk & 0x0C ) {  /*fmgpio[19:18]*/
				tmpmsk = (val & msk & 0x0C);

				XBYTE[REG_Fm_ctrlo_1] = (XBYTE[REG_Fm_ctrlo_1] & ~((msk&0x0C)<<2)) | (tmpmsk<<2);
			}
			HAL_GpioFm2OutBuf = (HAL_GpioFm2OutBuf & ~msk) | (val & msk);
			break;	

#if 0		
	case HAL_GPIO_BYTE_FM3:
		if ( msk & 0x01 ) {  /* fmgpio[18] */
			/* set fmgpio[18] value. */
			if ( val & msk & 0x01 ) {
				XBYTE[REG_Fm_ctrlo_1] |= 0x10;	/* set to high. */
			}
			else {
				XBYTE[REG_Fm_ctrlo_1] &= ~0x10;  /* set to low. */
			}
		}
		if ( msk & 0x02 ) {  /* fmgpio[19] */
			/* set fmgpio[19] value. */
			if ( val & msk & 0x01 ) {
				XBYTE[REG_Fm_ctrlo_1] |= 0x20;	/* set to high. */
			}
			else {
				XBYTE[REG_Fm_ctrlo_1] &= ~0x20;  /* set to low. */
			}
		}
		HAL_GpioFm3OutBuf = (HAL_GpioFm3OutBuf & ~msk) | (val & msk);
		break;
#endif		
	default:
		reg = HAL_GpioORegTbl[byteId];
		if ( reg != NULL ) {
			XBYTE[reg] = (XBYTE[reg] & ~msk) | (val & msk);
		}
	}
}


/**
 * @fn        UINT8 HAL_GpioByteOutGet(UINT8 byteId, UINT8 msk)
 * @brief     get gpio output in byte.
 * @param     [in] byteId: yte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap output value.
 * @see       NULL
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
HAL_GpioByteOutGet(
	UINT8 byteId,
	UINT8 msk
)
{
	UINT8 ret;
	UINT16 reg;


	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		ret = 0;
	}
	else {
		switch ( byteId ) {
		
		case HAL_GPIO_BYTE_MA1:
			ret = HAL_GpioMa1OutBuf;

			break;
		case HAL_GPIO_BYTE_FM0:
			ret = HAL_GpioFm0OutBuf;
			break;
		case HAL_GPIO_BYTE_FM1:
			ret = HAL_GpioFm1OutBuf;
			break;
		case HAL_GPIO_BYTE_FM2:
			ret = HAL_GpioFm2OutBuf;
			break;	
		//case HAL_GPIO_BYTE_FM3:
		//	ret = HAL_GpioFm3OutBuf;
		//	break;
		default:
			reg = HAL_GpioORegTbl[byteId];
			if ( reg == NULL ) {
				ret = 0;
			}
			else {
				ret = XBYTE[reg];
			}
		}

		ret &= msk;
	}

	#if HAL_GPIO_BYTE_OUT_GET_DBG
	printf("HAL_GpioByteOutGet %bu 0x%02bX 0x%02bX\n", byteId, msk, ret);
	#endif

	return ret;
}

/**
 * @fn        void HAL_GpioByteInGateSet(UINT8 byteId, UINT8 msk, UINT8 gate)
 * @brief     set gpio input gating in byte.
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] gate: bitmap gating, 0: disable, 1: enable.
 * @retval    NONE
 * @see       NULL
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
HAL_GpioByteInGateSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 gate
)
{
	UINT16 reg;


	#if HAL_GPIO_BYTE_IN_GATE_SET_DBG
	printf("HAL_GpioByteInGateSet() %bu 0x%02bX 0x%02bX\n", byteId, msk, gate);
	#endif

	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		return;
	}

	reg = HAL_GpioIgRegTbl[byteId];
	if ( reg != NULL ) {
		XBYTE[reg] = (XBYTE[reg] & ~msk) | (gate & msk);
	}
}


/**
 * @fn        UINT8 HAL_GpioByteInGet(UINT8 byteId, UINT8 msk, UINT8 polarity)
 * @brief     get gpio input in byte.
 * @param     [in] byteId: byte id, one of HAL_GpioByte_e.
 * @param     [in] msk: bitmap mask.
 * @param     [in] polarity: hardware input 0: low active  1: high active
 * @retval    bitmap output value. 
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
HAL_GpioByteInGet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 polarity 
)
{
	UINT8 ret;
	UINT16 reg;


	if ( byteId >= HAL_GPIO_BYTE_MAX ) {
		ret = 0;
	}
	else {
		switch ( byteId ) {

		case HAL_GPIO_BYTE_MA1:
			if ( msk & 0x40 ) {  /* ma[14] */
				/* get Ma14value. */
				ret = (XBYTE[REG_Dram_Dummy0] & 0x01) << 6;
			}

			break;
		case HAL_GPIO_BYTE_FM0:
			ret = (XBYTE[REG_Fm_ctrli_1] & 0x0F) | ((XBYTE[REG_Fm_ctrli_2]&0x0F)<<4);	
			break;

		case HAL_GPIO_BYTE_FM1:
			ret = (XBYTE[REG_Fm_ctrli_2] & 0x03) | ((XBYTE[REG_Fm_ctrli_3]&0x3F)<<2);
			break;

		case HAL_GPIO_BYTE_FM2:
			ret = ((XBYTE[REG_Fm_ctrli_3] & 0xC0)>>6) | ((XBYTE[REG_Fm_ctrli_1]&0x30)>>2);
			break;
			
		//case HAL_GPIO_BYTE_FM3:
		//	if ( msk & 0x03 ) {
		//		/* get fmgpio[18][19] */
		//		ret = (XBYTE[REG_Fm_ctrli_1] & 0x30) >> 4;
		//	}
		//	break;
		default:
			reg = HAL_GpioIRegTbl[byteId];
			if ( reg == NULL ) {
				ret = 0;
			}
			else {
				ret = XBYTE[reg];
			}
		}
		
		if ( polarity == 0 ) {
			ret = ~ret; 
		}
		ret &= msk;
		
	}

	#if HAL_GPIO_BYTE_IN_GET_DBG
	printf("HAL_GpioByteInGet %bu 0x%02bX 0x%02bX\n", byteId, msk, ret);
	#endif

	return ret;
}

/**
 * @fn        void HAL_GpioPullEnSet(UINT8 byteId, UINT8 msk, UINT8 en)
 * @brief     set pull high/low enable in byte.
 * @param     [in] byteId: byte id, 0~12.
 * @param     [in] msk: bitmap mask.
 * @param     [in] en: bitmap pull high/low, 0: disable, 1: enable.
 *					  if byteId = 0, bit [0] = fmgpio[0],
 *                                   bit [1] = fmgpio[1],
 *                                   bit [2] = fmgpio[2],
 *                                   bit [3] = fmgpio[3],
 *                                   bit [4] = fmgpio[4],
 *                                   bit [5] = fmgpio[5],
 *                                   bit [6] = fmgpio[6],
 *                                   bit [7] = fmgpio[7].
 *					  if byteId = 1, bit [0] = fmgpio[8],
 *                                   bit [1] = fmgpio[9],
 *                                   bit [2] = fmgpio[10],
 *                                   bit [3] = fmgpio[11],
 *                                   bit [4] = fmgpio[12],
 *                                   bit [5] = fmgpio[13],
 *                                   bit [6] = fmgpio[14],
 *                                   bit [7] = fmgpio[15].
 *					  if byteId = 2, bit [0] = fmgpio[16],			 
 *                                   bit [1] = fmgpio[17],
 *                                   bit [2] = fmgpio[18],
 *                                   bit [3] = fmgpio[19],
 *                                   bit [4] = ma[0],
 *                                   bit [5] = ma[1],
 *                                   bit [6] = ma[2],
 *                                   bit [7] = ma[3].
 *					  if byteId = 3, bit [0] = ma[4],
 *                                   bit [1] = ma[5],
 *                                   bit [2] = ma[6],
 *                                   bit [3] = ma[7],
 *                                   bit [4] = ma[8],
 *                                   bit [5] = ma[9],
 *                                   bit [6] = ma[10],
 *                                   bit [7] = ma[11].
 *					  if byteId = 4, bit [0] = ma[14],
 *                                   bit [1] = gpio[0],
 *                                   bit [2] = gpio[1],
 *                                   bit [3] = gpio[2],
 *                                   bit [4] = gpio[3],
 *                                   bit [5] = gpio[4],
 *                                   bit [6] = gpio[5],
 *                                   bit [7] = gpio[6].
 *					  if byteId = 5, bit [0] = gpio[7],
 *                                   bit [1] = gpio[8],
 *                                   bit [2] = gpio[9],
 *                                   bit [3] = gpio[10],
 *                                   bit [4] = gpio[11],
 *                                   bit [5] = gpio[12],
 *                                   bit [6] = gpio[13],
 *                                   bit [7] = gpio[14].
 *					  if byteId = 6, bit [0] = gpio[15],
 *                                   bit [1] = gpio[16],
 *                                   bit [2] = gpio[17],
 *                                   bit [3] = gpio[18],
 *                                   bit [4] = gpio[19],
 *                                   bit [5] = gpio[20],
 *                                   bit [6] = gpio[21],
 *                                   bit [7] = gpio[22].
 *					  if byteId = 7, bit [0] = gpio[23],
 *                                   bit [1] = gpio[24],
 *                                   bit [2] = sar3,
 *                                   bit [3] = sar4,
 *                                   bit [4] = digtv[0],
 *                                   bit [5] = digtv[1],
 *                                   bit [6] = digtv[2],
 *                                   bit [7] = digtv[3].
 *					  if byteId = 8, bit [0] = digtv[4],
 *                                   bit [1] = digtv[5],
 *                                   bit [2] = digtv[6],
 *                                   bit [3] = digtv[7],
 *                                   bit [4] = digtv[8],
 *                                   bit [5] = digtv[9],
 *                                   bit [6] = digtv[10],
 *                                   bit [7] = digtv[11],
 *					  if byteId = 9, bit [0] = digtv[12],
 *                                   bit [1] = digtv[13],
 *                                   bit [2] = digtv[14],
 *                                   bit [3] = digtv[15],
 *                                   bit [4] = digtv[16],
 *                                   bit [5] = digtv[17],
 *                                   bit [6] = digtv[10],
 *                                   bit [7] = digtv[19],
 *					  if byteId = 10, bit [0] = digtv[20],
 *                                   bit [1] = digtv[21],
 *                                   bit [2] = digtv[22],
 *                                   bit [3] = digtv[23],
 *                                   bit [4] = digtv[24],
 *                                   bit [5] = digtv[25],
 *                                   bit [6] = digtv[26],
 *                                   bit [7] = digtv[27].
 *					  if byteId = 11, bit [0] = rgb[0],
 *                                   bit [1] = rgb[1],
 *                                   bit [2] = rgb[2],
 *                                   bit [3] = rgb[3],
 *                                   bit [4] = rgb[4],
 *                                   bit [5] = rgb[5],
 *                                   bit [6] = rgb[6],
 *                                   bit [7] = rgb[7].
 *					  if byteId = 12, bit [0] = rgb[8],
 *                                   bit [1] = rgb[9],
 *                                   bit [2] = tggpio[0],
 *                                   bit [3] = tggpio[1],
 *                                   bit [4] = tggpio[2],
 *                                   bit [5] = tggpio[3],
 *                                   bit [6] = tggpio[4],
 *                                   bit [7] = tggpio[5].
 *					  Others: Not allowed.
 
 * @retval    NONE
 * @see       HAL_PullEnSet
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
HAL_GpioPullEnSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 en
)
{
	UINT16 reg;


	#if HAL_PULL_EN_SET_DBG
	printf("HAL_GpioPullEnSet() %bu 0x%02bX 0x%02bX\n", byteId, msk, en);
	#endif

	if ( byteId > 12 ) {
		return;
	}
	
	reg = 0x20A0 + byteId;

	XBYTE[reg] = (XBYTE[reg] & ~msk) | (en & msk);
}

/**
 * @fn        UINT8 HAL_GpioPullEnGet(UINT8 byteId, UINT8 msk)
 * @brief     get pull high/low enable in byte.
 * @param     [in] byteId: byte id, 0~12.
 * @param     [in] msk: bitmap mask.
 * @retval    bitmap pull high/low, 0: disable, 1: enable.
 *					  if byteId = 0, bit [0] = fmgpio[0],
 *                                   bit [1] = fmgpio[1],
 *                                   bit [2] = fmgpio[2],
 *                                   bit [3] = fmgpio[3],
 *                                   bit [4] = fmgpio[4],
 *                                   bit [5] = fmgpio[5],
 *                                   bit [6] = fmgpio[6],
 *                                   bit [7] = fmgpio[7].
 *					  if byteId = 1, bit [0] = fmgpio[8],
 *                                   bit [1] = fmgpio[9],
 *                                   bit [2] = fmgpio[10],
 *                                   bit [3] = fmgpio[11],
 *                                   bit [4] = fmgpio[12],
 *                                   bit [5] = fmgpio[13],
 *                                   bit [6] = fmgpio[14],
 *                                   bit [7] = fmgpio[15].
 *					  if byteId = 2, bit [0] = fmgpio[16],			 
 *                                   bit [1] = fmgpio[17],
 *                                   bit [2] = fmgpio[18],
 *                                   bit [3] = fmgpio[19],
 *                                   bit [4] = ma[0],
 *                                   bit [5] = ma[1],
 *                                   bit [6] = ma[2],
 *                     		   bit [7] = ma[3].
 *					  if byteId = 3, bit [0] = ma[4],
 *                                   bit [1] = ma[5],
 *                                   bit [2] = ma[6],
 *                                   bit [3] = ma[7],
 *                                   bit [4] = ma[8],
 *                                   bit [5] = ma[9],
 *                                   bit [6] = ma[10],
 *                                   bit [7] = ma[11].
 *					  if byteId = 4, bit [0] = ma[14],
 *                                   bit [1] = gpio[0],
 *                                   bit [2] = gpio[1],
 *                                   bit [3] = gpio[2],
 *                                   bit [4] = gpio[3],
 *                                   bit [5] = gpio[4],
 *                                   bit [6] = gpio[5],
 *                                   bit [7] = gpio[6].
 *					  if byteId = 5, bit [0] = gpio[7],
 *                                   bit [1] = gpio[8],
 *                                   bit [2] = gpio[9],
 *                                   bit [3] = gpio[10],
 *                                   bit [4] = gpio[11],
 *                                   bit [5] = gpio[12],
 *                                   bit [6] = gpio[13],
 *                                   bit [7] = gpio[14].
 *					  if byteId = 6, bit [0] = gpio[15],
 *                                   bit [1] = gpio[16],
 *                                   bit [2] = gpio[17],
 *                                   bit [3] = gpio[18],
 *                                   bit [4] = gpio[19],
 *                                   bit [5] = gpio[20],
 *                                   bit [6] = gpio[21],
 *                                   bit [7] = gpio[22].
 *					  if byteId = 7, bit [0] = gpio[23],
 *                                   bit [1] = gpio[24],
 *                                   bit [2] = sar3,
 *                                   bit [3] = sar4,
 *                                   bit [4] = digtv[0],
 *                                   bit [5] = digtv[1],
 *                                   bit [6] = digtv[2],
 *                                   bit [7] = digtv[3].
 *					  if byteId = 8, bit [0] = digtv[4],
 *                                   bit [1] = digtv[5],
 *                                   bit [2] = digtv[6],
 *                                   bit [3] = digtv[7],
 *                                   bit [4] = digtv[8],
 *                                   bit [5] = digtv[9],
 *                                   bit [6] = digtv[10],
 *                                   bit [7] = digtv[11],
 *					  if byteId = 9, bit [0] = digtv[12],
 *                                   bit [1] = digtv[13],
 *                                   bit [2] = digtv[14],
 *                                   bit [3] = digtv[15],
 *                                   bit [4] = digtv[16],
 *                                   bit [5] = digtv[17],
 *                                   bit [6] = digtv[10],
 *                                   bit [7] = digtv[19],
 *					  if byteId = 10, bit [0] = digtv[20],
 *                                   bit [1] = digtv[21],
 *                                   bit [2] = digtv[22],
 *                                   bit [3] = digtv[23],
 *                                   bit [4] = digtv[24],
 *                                   bit [5] = digtv[25],
 *                                   bit [6] = digtv[26],
 *                                   bit [7] = digtv[27].
 *					  if byteId = 11, bit [0] = rgb[0],
 *                                   bit [1] = rgb[1],
 *                                   bit [2] = rgb[2],
 *                                   bit [3] = rgb[3],
 *                                   bit [4] = rgb[4],
 *                                   bit [5] = rgb[5],
 *                                   bit [6] = rgb[6],
 *                                   bit [7] = rgb[7].
 *					  if byteId = 12, bit [0] = rgb[8],
 *                                   bit [1] = rgb[9],
 *                                   bit [2] = tggpio[0],
 *                                   bit [3] = tggpio[1],
 *                                   bit [4] = tggpio[2],
 *                                   bit [5] = tggpio[3],
 *                                   bit [6] = tggpio[4],
 *                                   bit [7] = tggpio[5].
 *					  Others: Not allowed.
 * @see       HAL_PullEnGet
 * @author    kai.Wang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
HAL_GpioPullEnGet(
	UINT8 byteId,
	UINT8 msk
)
{
	UINT8 ret;
	UINT16 reg;


	if ( byteId > 12 ) {
		ret = 0;
	}
	else {
		reg = 0x20A0 + byteId;

		ret = XBYTE[reg] & msk;
	}

	#if HAL_PULL_EN_GET_DBG
	printf("HAL_GpioPullEnGet %bu 0x%02bX 0x%02bX\n", byteId, msk, ret);
	#endif

	return ret;
}

/**
 * @fn        void HAL_GpioPullSelSet(UINT8 byteId, UINT8 msk, UINT8 sel)
 * @brief     HAL_GpioPullSelSet
 * @param     [in] byteId 0~9
 * @param     [in] msk
 * @param     [in] sel :0 pull down; 1:pull up
 *					  if byteId = 0, bit [0] = fmgpio[0],
 *                                   bit [1] = fmgpio[1],
 *                                   bit [2] = fmgpio[2],
 *                                   bit [3] = fmgpio[3],
 *                                   bit [4] = fmgpio[4],
 *                                   bit [5] = fmgpio[5],
 *                                   bit [6] = fmgpio[6],
 *                                   bit [7] = fmgpio[7].
 *					  if byteId = 1, bit [0] = fmgpio[8],
 *                                   bit [1] = fmgpio[9],
 *                                   bit [2] = fmgpio[10],
 *                                   bit [3] = fmgpio[11],
 *                                   bit [4] = fmgpio[12],
 *                                   bit [5] = fmgpio[13],
 *                                   bit [6] = fmgpio[14],
 *                                   bit [7] = fmgpio[15].
 *					  if byteId = 2, bit [0] = fmgpio[16],			 
 *                                   bit [1] = fmgpio[17],
 *                                   bit [2] = fmgpio[18],
 *                                   bit [3] = fmgpio[19],
 *                                   bit [4] = ma[14],
 *                                   bit [5] = gpio[0],
 *                                   bit [6] = gpio[1],
 *                     				 bit [7] = gpio[2],
 *					  if byteId = 3, bit [0] = gpio[3],
 *                                   bit [1] = gpio[4],
 *                                   bit [2] = gpio[5],
 *                                   bit [3] = gpio[6].
 *                                   bit [4] = gpio[7],
 *                                   bit [5] = gpio[8],
 *                                   bit [6] = gpio[9],
 *                                   bit [7] = gpio[10],
 *					  if byteId = 4, bit [0] = gpio[11],
 *                                   bit [1] = gpio[12],
 *                                   bit [2] = gpio[13],
 *                                   bit [3] = gpio[14],
 *                                   bit [4] = gpio[15],
 *                                   bit [5] = gpio[16],
 *                                   bit [6] = gpio[17],
 *                                   bit [7] = gpio[18],
 *					  if byteId = 5, bit [0] = gpio[19],
 *                                   bit [1] = gpio[20],
 *                                   bit [2] = gpio[21],
 *                                   bit [3] = gpio[22].
 *                                   bit [4] = gpio[23],
 *                                   bit [5] = gpio[24],
 *                                   bit [6] = sar3,
 *                                   bit [7] = sar4,
 *					  if byteId = 6, bit [0] = digtv[0],
 *                                   bit [1] = digtv[1],
 *                                   bit [2] = digtv[2],
 *                                   bit [3] = digtv[3].
 *                                   bit [4] = digtv[4],
 *                                   bit [5] = digtv[5],
 *                                   bit [6] = digtv[6],
 *                                   bit [7] = digtv[7],
 *					  if byteId = 7, bit [0] = digtv[8],
 *                                   bit [1] = digtv[9],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[11],
 *                                   bit [4] = digtv[12],
 *                                   bit [5] = digtv[13],
 *                                   bit [6] = digtv[14],
 *                                   bit [7] = digtv[15],
 *					  if byteId = 8, bit [0] = digtv[16],
 *                                   bit [1] = digtv[17],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[19],
 *                                   bit [4] = digtv[20],
 *                                   bit [5] = digtv[21],
 *                                   bit [6] = digtv[22],
 *                                   bit [7] = digtv[23],
 *					  if byteId = 9, bit [0] = digtv[24],
 *                                   bit [1] = digtv[25],
 *                                   bit [2] = digtv[26],
 *                                   bit [3] = digtv[27].
 *                                   bit [4] = tggpio[0],
 *                                   bit [5] = tggpio[1],
 *                                   bit [6] = tggpio[2],
 *                                   bit [7] = tggpio[3].
 *					  Others: Not allowed.
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void 
HAL_GpioPullSelSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 sel
)
{
	UINT16 reg;
	if ( byteId > 9 ) {
		return;
	}
	
	reg = 0x20D1 + byteId;

	XBYTE[reg] = (XBYTE[reg] & ~msk) | (sel & msk);
}

/**
 * @fn        UINT8 HAL_GpioPullSelGet(UINT8 byteId, UINT8 msk)
 * @brief     HAL_GpioPullSelGet
 * @param     [in] byteId 0~9
 * @param     [in] msk
 * @retval    
 *					  if byteId = 0, bit [0] = fmgpio[0],
 *                                   bit [1] = fmgpio[1],
 *                                   bit [2] = fmgpio[2],
 *                                   bit [3] = fmgpio[3],
 *                                   bit [4] = fmgpio[4],
 *                                   bit [5] = fmgpio[5],
 *                                   bit [6] = fmgpio[6],
 *                                   bit [7] = fmgpio[7].
 *					  if byteId = 1, bit [0] = fmgpio[8],
 *                                   bit [1] = fmgpio[9],
 *                                   bit [2] = fmgpio[10],
 *                                   bit [3] = fmgpio[11],
 *                                   bit [4] = fmgpio[12],
 *                                   bit [5] = fmgpio[13],
 *                                   bit [6] = fmgpio[14],
 *                                   bit [7] = fmgpio[15].
 *					  if byteId = 2, bit [0] = fmgpio[16],			 
 *                                   bit [1] = fmgpio[17],
 *                                   bit [2] = fmgpio[18],
 *                                   bit [3] = fmgpio[19],
 *                                   bit [4] = ma[14],
 *                                   bit [5] = gpio[0],
 *                                   bit [6] = gpio[1],
 *                                   bit [7] = gpio[2],
 *					  if byteId = 3, bit [0] = gpio[3],
 *                                   bit [1] = gpio[4],
 *                                   bit [2] = gpio[5],
 *                                   bit [3] = gpio[6].
 *                                   bit [4] = gpio[7],
 *                                   bit [5] = gpio[8],
 *                                   bit [6] = gpio[9],
 *                                   bit [7] = gpio[10],
 *					  if byteId = 4, bit [0] = gpio[11],
 *                                   bit [1] = gpio[12],
 *                                   bit [2] = gpio[13],
 *                                   bit [3] = gpio[14],
 *                                   bit [4] = gpio[15],
 *                                   bit [5] = gpio[16],
 *                                   bit [6] = gpio[17],
 *                                   bit [7] = gpio[18],
 *					  if byteId = 5, bit [0] = gpio[19],
 *                                   bit [1] = gpio[20],
 *                                   bit [2] = gpio[21],
 *                                   bit [3] = gpio[22].
 *                                   bit [4] = gpio[23],
 *                                   bit [5] = gpio[24],
 *                                   bit [6] = sar3,
 *                                   bit [7] = sar4,
 *					  if byteId = 6, bit [0] = digtv[0],
 *                                   bit [1] = digtv[1],
 *                                   bit [2] = digtv[2],
 *                                   bit [3] = digtv[3].
 *                                   bit [4] = digtv[4],
 *                                   bit [5] = digtv[5],
 *                                   bit [6] = digtv[6],
 *                                   bit [7] = digtv[7],
 *					  if byteId = 7, bit [0] = digtv[8],
 *                                   bit [1] = digtv[9],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[11],
 *                                   bit [4] = digtv[12],
 *                                   bit [5] = digtv[13],
 *                                   bit [6] = digtv[14],
 *                                   bit [7] = digtv[15],
 *					  if byteId = 8, bit [0] = digtv[16],
 *                                   bit [1] = digtv[17],
 *                                   bit [2] = digtv[10],
 *                                   bit [3] = digtv[19],
 *                                   bit [4] = digtv[20],
 *                                   bit [5] = digtv[21],
 *                                   bit [6] = digtv[22],
 *                                   bit [7] = digtv[23],
 *					  if byteId = 9, bit [0] = digtv[24],
 *                                   bit [1] = digtv[25],
 *                                   bit [2] = digtv[26],
 *                                   bit [3] = digtv[27].
 *                                   bit [4] = tggpio[0],
 *                                   bit [5] = tggpio[1],
 *                                   bit [6] = tggpio[2],
 *                                   bit [7] = tggpio[3].
 *					  Others: Not allowed.
 * @retval
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-6
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
HAL_GpioPullSelGet(
	UINT8 byteId,
	UINT8 msk
)
{
	UINT8 ret;
	UINT16 reg;


	if ( byteId > 9 ) {
		ret = 0;
	}
	else {
		reg = 0x20D1+ byteId;

		ret = XBYTE[reg] & msk;
	}

	return ret;
}

/**
 * @fn        UINT8 HAL_GpioDrivingSet(UINT8 byteId, UINT8 msk, UINT8 sel)
 * @brief     HAL_GpioDrivingSet
 * @param     [in] byteId
 * @param     [in] msk
 * @param     [in] sel:
 *				0:2mA
 *				1:4mA
 *				2:6mA
 *				3:8mA
 *				
 *			  if byteId = 0, FMGPIO[3:0]
 *			  if byteId = 1, FMGPIO[7:4]
 *			  if byteId = 2, FMGPIO[11:8]
 *			  if byteId = 3, FMGPIO[15:12]
 *			  if byteId = 4, FMGPIO[19:16]
 *			  if byteId = 5, bit[0]:sdramclk
 *			  				 bit[1]:ras_n
 *			  				 bit[2]:cas_n
 *			  				 bit[3]:mwe_n,
 *			  if byteId = 6, bit[0]:udqm
 *			  				 bit[1]:ldqm
 *			  				 bit[2]:cke
 *			  				 bit[3]:md[0],
 *			  if byteId = 7, md[4:1]
 *			  if byteId = 8, md[8:5]
 *			  if byteId = 9, md[12:9]
 *			  if byteId = 10, md[15:13]
 *			  				  ma[0],
 *			  if byteId = 11, ma[4:1]
 *			  if byteId = 12, ma[12:9]
 *			  if byteId = 13, ma[14:13]
 *			  				  gpio[1:0],
 *			  if byteId = 14, gpio[5:2]
 *			  if byteId = 15, gpio[9:6]
 *			  if byteId = 16, gpio[13:10]
 *			  if byteId = 17, gpio[17:14]
 *			  if byteId = 18, gpio[21:18]
 *			  if byteId = 19, gpio[24:22]
 *			  				  sar3,
 *			  if byteId = 20, sar4
 *			  				  digtv[2:0],
 *			  if byteId = 21, digtv[6:3]
 *			  if byteId = 22, digtv[10:7]
 *			  if byteId = 23, digtv[14:11]
 *			  if byteId = 24, digtv[18:15]
 *			  if byteId = 25, digtv[22:19]
 *			  if byteId = 26, digtv[26:23]
 *			  if byteId = 27, digtv[27]
 *			  				  tggpio[2:0],
 *			  if byteId = 28, tggpio[5:3]
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-5
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void 
HAL_GpioDrivingSet(
	UINT8 byteId,
	UINT8 msk,
	UINT8 sel
)
{
	UINT8 tempmsk = 0;
	UINT16 reg;
	if ( byteId >= 29 ) {
		return;
	}
	else { 
	
		if ( msk & 0x08 ) {
			tempmsk |= 0xc0;
		}	
		if ( msk & 0x04 ) {	
			tempmsk |= 0x30;
		}
		if ( msk & 0x02 ) {	
			tempmsk |= 0x0c;
		}	
		if ( msk & 0x01 ) {
			tempmsk |= 0x03;
		}	
		reg = 0x2048 + byteId;
		XBYTE[reg] = ((sel<<6) & (tempmsk & 0xc0)) | ((sel<<4) & (tempmsk & 0x30)) \
				| ((sel<<2) & (tempmsk & 0x0c)) | (sel & (tempmsk & 0x03));
 	}
}

/**
 * @fn        UINT8 HAL_GpioDriingGet(UINT8 byteId, UINT8 msk)
 * @brief     HAL_GpioDriingGet
 * @param     [in] byteId
 * @param     [in] msk
 * @retval    2/4/6/8mA
 *				
 *			  if byteId = 0, FMGPIO[3:0]
 *			  if byteId = 1, FMGPIO[7:4]
 *			  if byteId = 2, FMGPIO[11:8]
 *			  if byteId = 3, FMGPIO[15:12]
 *			  if byteId = 4, FMGPIO[19:16]
 *			  if byteId = 5, bit[0]:sdramclk
 *			  				 bit[1]:ras_n
 *			  				 bit[2]:cas_n
 *			  				 bit[3]:mwe_n,
 *			  if byteId = 6, bit[0]:udqm
 *			  				 bit[1]:ldqm
 *			  				 bit[2]:cke
 *			  				 bit[3]:md[0],
 *			  if byteId = 7, md[4:1]
 *			  if byteId = 8, md[8:5]
 *			  if byteId = 9, md[12:9]
 *			  if byteId = 10, md[15:13]
 *			  				  ma[0],
 *			  if byteId = 11, ma[4:1]
 *			  if byteId = 12, ma[12:9]
 *			  if byteId = 13, ma[14:13]
 *			  				  gpio[1:0],
 *			  if byteId = 14, gpio[5:2]
 *			  if byteId = 15, gpio[9:6]
 *			  if byteId = 16, gpio[13:10]
 *			  if byteId = 17, gpio[17:14]
 *			  if byteId = 18, gpio[21:18]
 *			  if byteId = 19, gpio[24:22]
 *			  				  sar3,
 *			  if byteId = 20, sar4
 *			  				  digtv[2:0],
 *			  if byteId = 21, digtv[6:3]
 *			  if byteId = 22, digtv[10:7]
 *			  if byteId = 23, digtv[14:11]
 *			  if byteId = 24, digtv[18:15]
 *			  if byteId = 25, digtv[22:19]
 *			  if byteId = 26, digtv[26:23]
 *			  if byteId = 27, digtv[27]
 *			  				  tggpio[2:0],
 *			  if byteId = 28, tggpio[5:3]
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-5
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8
HAL_GpioDrivingGet(
	UINT8 byteId,
	UINT8 msk
)
{	UINT8 ret;
	UINT16 reg;
	if ( byteId >= 29 ) {
		return 0xFF;
	}
	else {
		reg = 0x2048 + byteId;
		ret = XBYTE[reg];
		if ( msk & 0x08 ) {
			ret &= 0xc0;
			ret >>= 6;
		}	
		else if ( msk & 0x04 ) {	
			ret &= 0x30;
			ret >>= 4;
		}
		else if ( msk & 0x02 ) {	
			ret &= 0x0c;
			ret >>= 2;
		}	
		else if ( msk & 0x01 ) {
			ret &= 0x03;
		}	

		return ret;
	}
}

/**
 * @fn        UINT8 HAL_GpioSarAdcConfig(UINT8 pwrctl, UINT8 channel, UINT8 sarclk)
 * @brief     config adc group
 * @param     [in] tp_en:
 *					0: Disable. SARIN3 is used as the same way as SAR0, SAR1 and SAR2.
 *					1: Enable. SRARIN3 can be used as an touch panel input, in which tp_intnint_evt interrupt is available.
 * @param     [in] sarclk:
 *					0: ADC CLK=250KHz,  maximum sample frequency=15.625KHz
 *					1: ADC CLK=500KHz,  maximum sample frequency=31.25KHz
 *					2: ADC CLK=1MHz,  maximum sample frequency=62.5KHz 
 *					3: ADC CLK=2MHz, maximum sample frequency=125KHz
 * @param     [in] pwrctl:
 *					0: power down
 *					1: normal operation
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-9
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 HAL_GpioSarAdcConfig(
	UINT8 tp_en,
	UINT8 sarclk,
	UINT8 pwrctl
)
{
	XBYTE[0x2014] = 0xFF;
	XBYTE[0x2015] |= 0x01;
	XBYTE[0x2016] |= 0x02;
	/* Enable vref */
	XBYTE[REG_Audio_VrefPwrEn] =0x05;
	/*Enable SRARIN3 be used as an touch panel input*/
	if ( tp_en ) {
		XBYTE[REG_Audio_SarAdcPwrEn] = 0x02;
	}

	/*SAR ADC clock frequency selection*/
	if ( sarclk == 0x00 ) { /*ADC CLK=250KHz,*/
		XBYTE[REG_Audio_SarClkSel] = 0x00;
	}
	else if ( sarclk == 0x01 ) { /*ADC CLK=500KHz,*/
		XBYTE[REG_Audio_SarClkSel] = 0x01;	
	}
	else if ( sarclk == 0x02 ) { /*ADC CLK=1MHz,*/
		XBYTE[REG_Audio_SarClkSel] = 0x02;
	}
	else if ( sarclk == 0x03 ) { /*ADC CLK=2MHz,*/
		XBYTE[REG_Audio_SarClkSel] = 0x03;
	}
	else {
		return FAIL;
	}
	/*SAR ADC enable*/
	if ( pwrctl == 1 ) { /*normal operation*/
		XBYTE[REG_Audio_SarAdcPwrEn] |= 0x01;
	}
	else {
		XBYTE[REG_Audio_SarAdcPwrEn] &= 0xFE; /*power down*/
	}
	return SUCCESS;
}

/**
 * @fn        void HAL_GpioSarAdcValGet(UINT16 *adcval)
 * @brief     get sar adc value
 * @param     [in] channel:
 *					0: SAR0
 *					1: SAR1
 *					2: SAR2
 *					3: SAR3
 *					4: SAR4
 * @param     [in] *adcval: ADC value
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-9
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 HAL_GpioSarAdcValGet(
	UINT8 channel,
	UINT16 * adcval
)
{
	/* SAR ADC input selection */
	if ( channel == 0x00 ) { /*SAR 0*/
		XBYTE[REG_Audio_SarChnlSel] = 0x00;
	}
	else if ( channel == 0x01 ) { /*SAR 1*/
		XBYTE[REG_Audio_SarChnlSel] = 0x01;
	}
	else if ( channel == 0x02 ) { /*SAR 2*/
		XBYTE[REG_Audio_SarChnlSel] = 0x02;
	}
	else if ( channel == 0x03 ) { /*SAR 3*/
		XBYTE[REG_Audio_SarChnlSel] = 0x03;
	}
	else if ( channel == 0x04 ) { /*SAR 4*/
		XBYTE[REG_Audio_SarChnlSel] = 0x04;
	}
	else {
		return FAIL;
	}
	HAL_GlobalTimerWait(5);
	
	XBYTE[REG_Audio_SnapSarAdcVal0] = 0x01; /* snap adc data */
	*adcval = ( (XBYTE[REG_Audio_SnapSarAdcVal1]^0x02) <<8) | (XBYTE[REG_Audio_SnapSarAdcVal0]);

	return SUCCESS;
}
