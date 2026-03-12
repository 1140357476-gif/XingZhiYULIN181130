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
 * @file		hal_front.c
 * @brief		hal front module api.
 * @author		Will Chiu
 * @since		2010-01-01
 * @date		2010-01-01
 */


/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
//#include "hal_front_internal.h"
#include "hal_front.h" 
#include "hal_front_para.h" 
#include "Reg_front.h"

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
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        void HAL_FrontIntrEnable(UINT32 enable, UINT8 value)
 * @brief     enable/disable front interrupt
 * @param     [in] enable : 
 *							0: disable
 *							1: enable 
 * @param     [in] value :
 *							[0] : VD rise
 *							[1] : VD fall
 *							[2] : Serial done
 *							[3] : Snap done
 *							[4] : Clear CDSP do done
 *							[5] : Front valid rise
 *							[6] : Front valid falling
 *							[7] : 
 *							[8] : 
 *							[9] : 
 *							[10]: 
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontIntrEnable(
	UINT8 enable,
	UINT8 value
)
{
	/* set Finten and FintEvt. */
	if (enable) {
		XBYTE[REG_Front_Finten_low] |= value;
		XBYTE[REG_Front_FintEvt_low] |= value;  /* clear intrrupt */
	}
	else {
		XBYTE[REG_Front_Finten_low] &= ~value;
		XBYTE[REG_Front_FintEvt_low] |= value; /* clear intrrupt */
	}

	/* set VDFintNum and VDRintNum. */
	XBYTE[REG_Front_VDRIntNum] = 0x11;  //set VD rising and falling number = 1
}

/**
 * @fn        void HAL_FrontReset(void)
 * @brief     reset front interface state machine
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontReset(
	void
)
{
	/* set Senrst. */
	XBYTE[REG_Front_Senrst] = 0x01;
	XBYTE[REG_Front_Senrst] = 0x00;
}


/**
 * @fn        UINT8 HAL_FrontSnapTrig(UINT8 snapnum, UINT8 clear)
 * @brief     Trigger snap and set the number of frame
 * @param     [in] snapNum : number of frame you wanna capture
 * @param     [in] trig : trigger snap
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_FrontSnapTrig(
	UINT8 snapNum,
	UINT8 trig
)
{
    XBYTE[REG_Front_Snapnum] = ((trig & 0x01) << 4) | (snapNum & 0x0f);

	return SUCCESS;
}

/**
 * @fn        UINT8 HAL_FrontSignalWait(UINT8 src, UINT8 mode, UINT8 num)
 * @brief     wait for signal event
 * @param     [in] src : 
 *						0x01: HSync
 *						0x02: VSync
 *						0x04: HValid
 *						0x08: VValid
 * @param     [in] mode : 
 *						0x00: Falling
 *						0x01: Rising
 *						0x02: High
 *						0x03: Low
 * @param     [in] num : times you wanna wait
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_FrontSignalWait(
	UINT8 src,
	UINT8 mode,
	UINT8 num
)
{
	if( src == HAL_FRONT_WAIT_VSYNC || src == HAL_FRONT_WAIT_HSYNC ) {
		while( num > 0 ) {
			while ( 1 ) {
				if ( XBYTE[REG_Front_Tggpioi] & ( ~src & 0x03 ) ) {  /* high detected */
					if( XBYTE[ REG_Front_exthdipol ] & src ) {
						if ( mode == HAL_FRONT_WAIT_RISING ) {
							break;
						}
						if ( mode == HAL_FRONT_WAIT_LOW ) {
							return SUCCESS;
						}
					}
					else {
						if ( mode == HAL_FRONT_WAIT_FALLING ) {
							break;
						}
						if ( mode == HAL_FRONT_WAIT_HIGH ) {
							return SUCCESS;
						}
					}
				}
				else {  /* !(XBYTE[0x2A66] & src) */  /* low detected */
					if( XBYTE[ REG_Front_exthdipol ] & src ) {
						if ( mode == HAL_FRONT_WAIT_FALLING ) {
							break;
						}
						if ( mode == HAL_FRONT_WAIT_HIGH ) {
							return SUCCESS;
						}
					}
					else {
						if ( mode == HAL_FRONT_WAIT_RISING ) {
							break;
						}
						if ( mode == HAL_FRONT_WAIT_LOW ) {
							return SUCCESS;
						}
					}
				}
			}

			while ( 1 ) {
				if ( !(XBYTE[REG_Front_Tggpioi] & ( ~src & 0x03 )) ) {  /* low detected */
					if( XBYTE[ REG_Front_exthdipol ] & src ) {
						if ( mode == HAL_FRONT_WAIT_RISING ) {
							break;
						}
					}
					else {
						if ( mode == HAL_FRONT_WAIT_FALLING ) {
							break;
						}
					}
				}
				else {  /* XBYTE[0x2A66] & src */  /* high detected */
					if( XBYTE[ REG_Front_exthdipol ] & src ) {
						if ( mode == HAL_FRONT_WAIT_FALLING ) {
							break;
						}
					}
					else {
						if ( mode == HAL_FRONT_WAIT_RISING ) {
							break;
						}
					}
				}
			}

			num--;
		}

		return SUCCESS;
	}

	/* wait for specified event. */
	while ( num > 0 ) {
		while ( 1 ) {
			if ( XBYTE[REG_Front_Hsync] & src ) {  /* high detected */
				if ( mode == HAL_FRONT_WAIT_FALLING ) {
					break;
				}
				if ( mode == HAL_FRONT_WAIT_HIGH ) {
					return SUCCESS;
				}
			}
			else {  /* !(XBYTE[0x2AB8] & src) */  /* low detected */
				if ( mode == HAL_FRONT_WAIT_RISING ) {
					break;
				}
				if ( mode == HAL_FRONT_WAIT_LOW ) {
					return SUCCESS;
				}
			}
		}

		while ( 1 ) {
			if ( !(XBYTE[REG_Front_Hsync] & src) ) {  /* low detected */
				if ( mode == HAL_FRONT_WAIT_FALLING ) {
					break;
				}
			}
			else {  /* XBYTE[0x2AB8] & src */  /* high detected */
				if ( mode == HAL_FRONT_WAIT_RISING ) {
					break;
				}
			}
		}

		num--;
	}

	return SUCCESS;
}


/**
 * @fn        void HAL_FrontParaMClkCfgSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set front master clk configuration
 * @param     [in] paraAry : 
 *							[0]: master clk source , 0: internal , 1: external
 *							[1]: master clk division factor, 1~256
 *							[2]: master clk phase, 0~15, step 1ns
 *							[3]: master clk driving, 0: 2mA, 1: 4mA, 2: 6mA, 3: 8mA
 * @param     [in] upd : 
 *							0: immediate update 
 *							1: update will sync with front vd falling edge  
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaMClkCfgSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set vclk2xsel. */
	XBYTE[0x2019] = (XBYTE[0x2019] & ~0x04) | paraAry[0];

	/* set vclk2xdiv. */
	if ( paraAry[1] >= 256 ) {
		paraAry[1] = 255;
	}
	else if ( paraAry[1] > 0 ) {
		paraAry[1]--;
	}
	XBYTE[0x2027] = paraAry[1];

	/* set Chphase[7:4]. */
	XBYTE[0x20E3] = (XBYTE[0x20E3] & ~0xF0) | (paraAry[2] << 4);

	/* set mclkdrive. */
	XBYTE[0x2064] = (XBYTE[0x2064] & ~0xC0) | (paraAry[3] << 6);

	/* set vclk2xdivstsel. */
	XBYTE[0x202B] = upd;
}

/**
 * @fn        void HAL_IntrFrontParaMClkCfgSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set front master clk configuration
 * @param     [in] paraAry : 
 *							[0]: master clk source , 0: internal , 1: external
 *							[1]: master clk division factor, 1~256
 *							[2]: master clk phase, 0~15, step 1ns
 *							[3]: master clk driving, 0: 2mA, 1: 4mA, 2: 6mA, 3: 8mA
 * @param     [in] upd : 
 *							0: immediate update 
 *							1: update will sync with front vd falling edge  
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_IntrFrontParaMClkCfgSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set vclk2xsel. */
	XBYTE[0x2019] = (XBYTE[0x2019] & ~0x04) | paraAry[0];

	/* set vclk2xdiv. */
	if ( paraAry[1] >= 256 ) {
		paraAry[1] = 255;
	}
	else if ( paraAry[1] > 0 ) {
		paraAry[1]--;
	}
	XBYTE[0x2027] = paraAry[1];

	/* set Chphase[7:4]. */
	XBYTE[0x20E3] = (XBYTE[0x20E3] & ~0xF0) | (paraAry[2] << 4);

	/* set mclkdrive. */
	XBYTE[0x2064] = (XBYTE[0x2064] & ~0xC0) | (paraAry[3] << 6);

	/* set vclk2xdivstsel. */
	XBYTE[0x202B] = upd;
}

/**
 * @fn        void HAL_FrontParaPClkCfgSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set front pixel clk configuration
 * @param     [in] paraAry : 
 *							[0]: pixel clk source
 *							[1]: pixel clk division factor, 1~16
 *							[2]: pixel clk phase, 0~15, step 1ns
 *							[3]: N/A
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaPClkCfgSet(
	SINT16 *paraAry,
	UINT8 upd	
)
{
	/* set vclk1xsel. */
	XBYTE[0x2019] = (XBYTE[0x2019] & ~0x08) | paraAry[0];

	/* set vclk1xdiv. */
	if ( paraAry[1] >= 16 ) {
		paraAry[1] = 15;
	}
	else if ( paraAry[1] > 0 ) {
		paraAry[1]--;
	}
	XBYTE[0x2026] = paraAry[1];

	/* set Chphase[11:8]. */
	XBYTE[0x20E4] = (XBYTE[0x20E4] & ~0x0F) | paraAry[2];

	upd = upd;  /* for complier */
}

/**
 * @fn        void HAL_IntrFrontParaPClkCfgSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set front pixel clk configuration
 * @param     [in] paraAry : 
 *							[0]: pixel clk source
 *							[1]: pixel clk division factor, 1~16
 *							[2]: pixel clk phase, 0~15, step 1ns
 *							[3]: N/A
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_IntrFrontParaPClkCfgSet(
	SINT16 *paraAry,
	UINT8 upd	
)
{
	/* set vclk1xsel. */
	XBYTE[0x2019] = (XBYTE[0x2019] & ~0x08) | paraAry[0];

	/* set vclk1xdiv. */
	if ( paraAry[1] >= 16 ) {
		paraAry[1] = 15;
	}
	else if ( paraAry[1] > 0 ) {
		paraAry[1]--;
	}
	XBYTE[0x2026] = paraAry[1];

	/* set Chphase[11:8]. */
	XBYTE[0x20E4] = (XBYTE[0x20E4] & ~0x0F) | paraAry[2];

	upd = upd;  /* for complier */
}

/**
 * @fn        void HAL_FrontParaSyncInvertEnableSet(SINT16 *paraAry)
 * @brief     enable/disable sync signal invert
 * @param     [in] paraAry :
 *							[0]: 0: disable, 1: enable invert hsync
 *							[1]: 0: disable, 1: enable invert vsync
 *							[2]: N/A
 *							[3]: N/A
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaSyncInvertEnableSet(
	SINT16 *paraAry,
	UINT8 upd	
)
{
	/* set extvdipol & exthdipol. */
	XBYTE[REG_Front_exthdipol] = (XBYTE[REG_Front_exthdipol] & ~0x03) | (paraAry[1] << 1) | paraAry[0];

	upd = upd;	/* for complier */
}

/**
 * @fn        void HAL_FrontParaByPassEnableSet(SINT16 *paraAry, UINT8 upd)
 * @brief     enable/disable bypass.
 * @param     [in] paraAry : 
 *							[0]: 0: disable, 1: enable h bypass
 *							[1]: 0: disable, 1: enable v bypass
 *							[2]: N/A
 *							[3]: N/A 
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaByPassEnableSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set V bypass en & H bypass en. */
	XBYTE[REG_Front_bypashref] = (XBYTE[REG_Front_bypashref] & ~0x03) | (paraAry[1] << 1) | paraAry[0];

	upd = upd;	/* for complier */
}


/**
 * @fn        void HAL_FrontParaReshapeEnableSet(SINT16 *paraAry, UINT8 upd)
 * @brief     enable/disable reshape.
 * @param     [in] paraAry : 
 *							[0]: 0: disable, 1: enable hsync reshape
 *							[1]: 0: disable, 1: enable vsync reshape
 *							[2]: N/A
 *							[3]: N/A 
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaReshapeEnableSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Vreshen & Hreshen. */
	XBYTE[REG_Front_Hreshen] = (XBYTE[REG_Front_Hreshen] & ~0x03) | (paraAry[1] << 1) | paraAry[0];

	upd = upd;	/* for complier */
	
}

/**
 * @fn        void HAL_FrontParaReshapeRegionSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set reshape region
 * @param     [in] paraAry
 *							[0]: hsync falling edge position
 *							[1]: hsync rising edge position
 *							[2]: vsync falling edge position
 *							[3]: vsync rising edge position 
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaReshapeRegionSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Hfall. */
	XBYTE[REG_Front_Hfall_low] = READ8(paraAry[0], 1);
	XBYTE[REG_Front_Hfall_high] = READ8(paraAry[0], 0);

	/* set Hrise. */
	XBYTE[REG_Front_Hrise_low] = READ8(paraAry[1], 1);
	XBYTE[REG_Front_Hrise_high] = READ8(paraAry[1], 0);

	/* set Vfall. */
	XBYTE[REG_Front_Vfall_low] = READ8(paraAry[2], 1);
	XBYTE[REG_Front_Vfall_high] = READ8(paraAry[2], 0);

	/* set Vrise. */
	XBYTE[REG_Front_Vrise_low] = READ8(paraAry[3], 1);
	XBYTE[REG_Front_Vrise_high] = READ8(paraAry[3], 0);

	upd = upd;
}

/**
 * @fn        void HAL_FrontParaCropRegionSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set crop region
 * @param     [in] paraAry
 *							[0]: horizontal size
 *							[1]: vertical size
 *							[2]: horizontal offset
 *							[3]: vertical offset  
 * @param     [in] upd :
 *							0: immediate update 
 *							1: update sync with front vd falling edge   
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaCropRegionSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Hsize. */
	XBYTE[REG_Front_Hsize_low] = READ8(paraAry[0], 1);
	XBYTE[REG_Front_Hsize_high] = READ8(paraAry[0], 0);

	/* set Vsize. */
	XBYTE[REG_Front_Vsize_low] = READ8(paraAry[1], 1);
	XBYTE[REG_Front_Vsize_high] = READ8(paraAry[1], 0);

	/* set Hoffset. */
	XBYTE[REG_Front_Hoffset_low] = READ8(paraAry[2], 1);
	XBYTE[REG_Front_Hoffset_high] = READ8(paraAry[2], 0);

	/* set Voffset. */
	XBYTE[REG_Front_Voffset_low] = READ8(paraAry[3], 1);
	XBYTE[REG_Front_Voffset_high] = READ8(paraAry[3], 0);

	/* always enable Vdupdate. */
	XBYTE[REG_Front_Vdupdate] |= 0x01;

	/* set Vsvden & Hsvden. */
	XBYTE[REG_Front_Hsvden] = upd ? 0x03 : 0x00;
}

void 
HAL_FrontSizeGet(
	UINT16 *hSize,
	UINT16 *vSize
)
{
	*hSize= (UINT16)XBYTE[REG_Front_Hsize_low]+(UINT16)XBYTE[REG_Front_Hsize_high]*256;
	*vSize= (UINT16)XBYTE[REG_Front_Vsize_low]+(UINT16)XBYTE[REG_Front_Vsize_high]*256;
}

/**
 * @fn        void HAL_FrontParaFrontToCdspGatingEnableSet(SINT16 *paraAry, UINT8 upd)
 * @brief     enable/disable front to cdsp gating
 * @param     [in] paraAry : 
 * @param     [in] paraAry
 *							[0]: 0: disable, 1: enable
 *							[1]: N/A
 *							[2]: N/A
 *							[3]: N/A   
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaFrontToCdspGatingEnableSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set F2CdspGate & F2CdspSEof. */
	XBYTE[REG_Front_F2CdspGate] = (XBYTE[REG_Front_F2CdspGate] & ~0x11) | paraAry[0] | (upd << 4);
}

/**
 * @fn        void HAL_IntrFrontParaFrontToCdspGatingEnableSet(SINT16 *paraAry, UINT8 upd)
 * @brief     enable/disable front to cdsp gating
 * @param     [in] paraAry : 
 * @param     [in] paraAry
 *							[0]: 0: disable, 1: enable
 *							[1]: N/A
 *							[2]: N/A
 *							[3]: N/A   
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_IntrFrontParaFrontToCdspGatingEnableSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set F2CdspGate & F2CdspSEof. */
	XBYTE[REG_Front_F2CdspGate] = (XBYTE[REG_Front_F2CdspGate] & ~0x11) | paraAry[0] | (upd << 4);
}



/**
 * @fn        void HAL_FrontParaTgTotalRegionSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set front tg total region
 * @param     [in] paraAry : 
 *							[0]: linetotal, total pixels per line
 *							[1]: frametotal, total lines per frame
 *							[2]: lineblank, blank pixel in a line
 *							[3]: frameblank, blank line in a frame 
 * @param     [in] upd 
 *							0: immediate update 
 *							1: update sync with front vd falling edge    
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaTgTotalRegionSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Linetotal. */
	XBYTE[REG_Front_Linetotal_low] = READ8(paraAry[0], 1);
	XBYTE[REG_Front_Linetotal_high] = READ8(paraAry[0], 0);

	/* set Frametotal. */
	XBYTE[REG_Front_Frametotal_low] = READ8(paraAry[1], 1);
	XBYTE[REG_Front_Frametotal_high] = READ8(paraAry[1], 0);

	/* set Lineblank. */
	XBYTE[REG_Front_Lineblank_low] = READ8(paraAry[2], 1);
	XBYTE[REG_Front_Lineblank_high] = READ8(paraAry[2], 0);

	/* set Frameblank. */
	XBYTE[REG_Front_Frameblank_low] = READ8(paraAry[3], 1);
	XBYTE[REG_Front_Frameblank_high] = READ8(paraAry[3], 0);

	/* set Totalsvden. */
	XBYTE[REG_Front_Syncgenen] = (XBYTE[REG_Front_Syncgenen] & ~0x02) | (upd << 1);
}

/**
 * @fn        void HAL_FrontParaTgSyncGeneratorEnableSet(SINT16 *paraAry, UINT8 upd)
 * @brief     enable/disable front tg sync generator
 * @param     [in] paraAry
 *							[0]: 0: disable, 1: enable sync generator
 *							[1]: N/A
 *							[2]: N/A
 *							[3]: N/A
 * @param     [in] upd :
 *							0: immediate update 
 *							1: update sync with front vd falling edge     
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaTgSyncGeneratorEnableSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Syncgenen & Totalsvden. */
	XBYTE[REG_Front_Syncgenen] = (XBYTE[REG_Front_Syncgenen] & ~0x03) | paraAry[0] | (upd << 1);
}

/**
 * @fn        void HAL_FrontParaYuvOutputSequenceCfgSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set yuv output sequence
 * @param     [in] paraAry
 *							[0]: 0: UYVY , 1: YVYU , 2: VYUY , 3: YUYV
 *							[1]: N/A
 *							[2]: N/A
 *							[3]: N/A 
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaYuvOutputSequenceCfgSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Uvsel. */
	XBYTE[REG_Front_Uvsel] = (XBYTE[REG_Front_Uvsel] & ~0x03) | paraAry[0];

	upd = upd;
}

/**
 * @fn        void HAL_FrontParaYuvCcirModeSet(SINT16 *paraAry, UINT8 upd)
 * @brief     set yuv ccir mode
 * @param     [in] paraAry
 *							[0]: 0: CCIR601 , 0x10: CCIR656
 *							[1]: N/A
 *							[2]: N/A
 *							[3]: N/A  
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaYuvCcirModeSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Bt656en. */
	XBYTE[REG_Front_Uvsel] = (XBYTE[REG_Front_Uvsel] & ~0x10) | paraAry[0];

	upd = upd;
}

/**
 * @fn        void HAL_FrontParaYuvSub128EnableSet(SINT16 *paraAry, UINT8 upd)
 * @brief     enable/disable y/u/v values sub 128
 * @param     [in] paraAry
 *							[0]: 0: disable, 1: enable y value sub 128
 *							[1]: 0: disable, 1: enable u value sub 128
 *							[2]: 0: disable, 1: enable v value sub 128
 *							[3]: N/A  
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaYuvSub128EnableSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Vs128en, Us128en & Ys128en. */
	XBYTE[REG_Front_Ys128en] = (XBYTE[REG_Front_Ys128en] & ~0x07)
	                | (paraAry[2] << 2)
	                | (paraAry[1] << 1)
	                | paraAry[0];

	upd = upd;
}

/**
 * @fn        void HAL_FrontParaYuvInputEnableSet(SINT16 *paraAry, UINT8 upd)
 * @brief     enable/disable yuv input
 * @param     [in] paraAry
 *							[0]: 0: disable, 1: enable
 *							[1]: N/A
 *							[2]: N/A
 *							[3]: N/A  
 * @param     [in] upd : N/A
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaYuvInputEnableSet(
	SINT16 *paraAry,
	UINT8 upd
)
{
	/* set Tvdecen. */
	XBYTE[REG_Front_Ys128en] = (XBYTE[REG_Front_Ys128en] & ~0x10) | (paraAry[0] << 4);

	upd = upd;
}

/**
 * @fn        void HAL_FrontParaMClkCfgGet(UINT8 *pSrc, UINT16 *pDiv, UINT8 *pPhase, UINT8 *pDrv, UINT8 *pUpd)
 * @brief     get front master clk configuration
 * @param     [out] pSrc : master clk source
 *							0: internal 
 *							1: external
 * @param     [out] pDiv : master clk division factor, 1~256
 * @param     [out] pPhase : master clk phase, 0~15
 * @param     [out] pDrv : master clk driving, 0: 4mA, 1: 8mA
 * @param     [out] pUpd 
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaMClkCfgGet(
	UINT8 *pSrc,
	UINT16 *pDiv,
	UINT8 *pPhase,
	UINT8 *pDrv,
	UINT8 *pUpd
)
{
	/* get vclk2xsel. */
	*pSrc = XBYTE[0x2019] & 0x04;

	/* get vclk2xdiv. */
	*pDiv = XBYTE[0x2027] + 1;

	/* get Chphase[7:4]. */
	*pPhase = (XBYTE[0x20E3] & 0xF0) >> 4;

	/* get mclkdrive. */
	*pDrv = (XBYTE[0x2085] & 0x02) >> 1;

	/* get vclk2xdivstsel. */
	*pUpd = XBYTE[0x202B];
}

/**
 * @fn        void HAL_FrontParaPClkCfgGet(UINT8 *pSrc, UINT16 *pDiv, UINT8 *pPhase)
 * @brief     get front pixel clk configuration
 * @param     [in] pSrc : pixel clk source, 0: internal, 1:external
 * @param     [in] pDiv : pixel clk division factor, 1~16
 * @param     [in] pPhase : pixel clk phase, 0~15
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_FrontParaPClkCfgGet(
	UINT8 *pSrc,
	UINT16 *pDiv,
	UINT8 *pPhase
)
{
	/* get vclk1xsel. */
	*pSrc = XBYTE[0x2019] & 0x08;

	/* get vclk1xdiv. */
	*pDiv = XBYTE[0x2026] + 1;

	/* get Chphase[11:8]. */
	*pPhase = XBYTE[0x20E4] & 0x0F;
}

/**
 * @fn        void sp1kHalFrontInputGate(UINT8 tggz)
 * @brief     Gate front data sending to CDSP. If gating, CDSP doesn't not have input from front
 * @param     [in] tggz
 *						bit[0] 0: no gating ,  1: gating
 *						bit[4] 0: immediate update , 1: sync front EOF signal
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kHalFrontInputGate(
	UINT8 tggz
)
{
	HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE,
	                 tggz & 0x01, 0, 0, 0, (tggz & 0x10) >> 4);
}
