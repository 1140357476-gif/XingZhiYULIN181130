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
 
#ifndef __SP1K_GPIO_API_H__
#define __SP1K_GPIO_API_H__

/** \file sp1k_aud_api.h
 * @brief Spca1628 GPIO related interface 
 */
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sp1k_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP1K_GPIO_BYTE_NULL,
	SP1K_GPIO_BYTE_GEN0,   /* gpio[7:0] 		*/
	SP1K_GPIO_BYTE_GEN1,   /* gpio[15:8] 	  */
	SP1K_GPIO_BYTE_GEN2,   /* gpio[23:16]   */
	SP1K_GPIO_BYTE_GEN3,   /* gpio[26:24]	  */
	SP1K_GPIO_BYTE_FM0,	   /* fmgpio[7:0]   */
	SP1K_GPIO_BYTE_FM1,	   /* fmgpio[15:8]  */
	SP1K_GPIO_BYTE_FM2,	   /* fmgpio[19:16] */
	SP1K_GPIO_BYTE_TG0,    /* tggpio[5:0]	*/
	SP1K_GPIO_BYTE_DISP0,  /* tvegpio[7:0]	*/
	SP1K_GPIO_BYTE_DISP1,  /* tvegpio[15:8]	*/
	SP1K_GPIO_BYTE_DISP2,  /* tvegpio[23:16] */
	SP1K_GPIO_BYTE_DISP3,  /* tvegpio[27:24] */
	SP1K_GPIO_BYTE_MA1,    /* ma[14:14]		*/
	SP1K_GPIO_BYTE_RGB0,   /* rgb[7:0]		*/
	SP1K_GPIO_BYTE_RGB1,   /* rgb[9:8]		*/
	SP1K_GPIO_BYTE_MAX,
} sp1kGpioByte_e; 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#if 0
 typedef struct {
	UINT8 TypeId;
	UINT8 polarity;
} sp1kGpioPolarity_t;
#endif
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
 * @fn void sp1kGpioByteFuncSet(UINT8 byteId, UINT8 msk, UINT8 func)
 * @brief	Set gpio function in byte.
 * @param	byteId :  one of sp1kGpioByte_e
 * @param	msk : bitmap mask. please refer to param 'func'.
 * @param	func : bitmap function, 0: function, 1: gpio.
 * @retval	None.
*/
void sp1kGpioByteFuncSet(UINT8 byteId,UINT8 msk,UINT8 func);

/**
 * @fn void sp1kGpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir)
 * @brief	Set gpio direction in byte.
 * @param	byteId :  one of sp1kGpioByte_e
 * @param	msk : bitmap mask. please refer to param 'dir'.
 * @param	dir : bitmap direction, 0: input, 1: output.
 * @retval	None.
*/
void sp1kGpioByteDirSet(UINT8 byteId,UINT8 msk,UINT8 dir);

/**
 * @fn void sp1kGpioByteOutSet(UINT8 byteId, UINT8 msk, UINT8 val)
 * @brief	Set gpio output in byte.
 * @param	byteId :  one of sp1kGpioByte_e
 * @param	msk : bitmap mask. please refer to param 'val'.
 * @param	val : bitmap output value.				  
 * @retval	None.
*/
void sp1kGpioByteOutSet(UINT8 byteId,UINT8 msk,UINT8 val);

/**
 * @fn UINT8 sp1kGpioByteInGet(UINT8 byteId, UINT8 msk)
 * @brief	Get gpio input in byte.
 * @param	byteId :  one of sp1kGpioByte_e
 * @param	msk : bitmap mask. please refer to param 'val'.
 * @param	[in] polarity: 0: low active 
 *					     1: high active
 * @retval	bitmap input value, please refer to param 'val' in sp1kGpioByteOutSet().
*/
UINT8 sp1kGpioByteInGet(UINT8 byteId, UINT8 msk, UINT8 polarity );

/**
 * @fn        void sp1kPullSelSet(UINT8 byteId, UINT8 msk, UINT8 sel)
 * @brief     select pull up or down
 * @param     [in] byteId: byte id, 0~9.
 * @param     [in] msk
 * @param     [in] sel: bitmap select pull up/down 0:pull down; 1:pull up
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
 * @retval    NONE

*/
void sp1kPullSelSet(UINT8 byteId, UINT8 msk, UINT8 sel);


/**
 * @fn        UINT8 sp1kpullSelGet(UINT8 byteId, UINT8 msk)
 * @brief     get pull up or down
 * @param     [in] byteId: byte id, 0~9.
 * @param     [in] msk
 * @retval    bitmap pull up/down, 0:pull down; 1:pull up
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
*/
UINT8 sp1kpullSelGet(UINT8 byteId, UINT8 msk);

/**
 * @fn void sp1kPullEnSet(UINT8 byteId, UINT8 msk, UINT8 en)
 * @brief	Set pull high/low enable in byte.
 * @param	byteId :  byte ID, 0~6.
 * @param	msk : bitmap mask. please refer to param 'en'.
 * @param	en : bitmap function, 0: function, 1: gpio.
 *					  if byteId = 0, bit [0] = psen,
 *									 bit [1] = romwr_n,
 *									 bit [2] = fmgpio[0],
 *									 bit [3] = fmgpio[1],
 *									 bit [4] = fmgpio[5],
 *									 bit [5] = fmgpio[6],
 *									 bit [6] = fmgpio[7],
 *									 bit [7] = fmgpio[8].
 *					  if byteId = 1, bit [0] = fmgpio[9],
 *									 bit [1] = ma[0],
 *									 bit [2] = ma[1],
 *									 bit [3] = ma[2],
 *									 bit [4] = ma[3],
 *									 bit [5] = ma[4],
 *									 bit [6] = ma[5],
 *									 bit [7] = ma[6].
 *					  if byteId = 2, bit [0] = ma[7],
 *									 bit [1] = ma[14],
 *									 bit [2] = gpio[0],
 *									 bit [3] = gpio[1],
 *									 bit [4] = gpio[2],
 *									 bit [5] = gpio[3],
 *									 bit [6] = gpio[4],
 *									 bit [7] = gpio[5].
 *					  if byteId = 3, bit [0] = gpio[6],
 *									 bit [1] = gpio[7],
 *									 bit [2] = gpio[9],
 *									 bit [3] = gpio[10],
 *									 bit [4] = gpio[11],
 *									 bit [5] = gpio[13],
 *									 bit [6] = gpio[14],
 *									 bit [7] = gpio[15].
 *					  if byteId = 4, bit [0] = gpio[16],
 *									 bit [1] = gpio[17],
 *									 bit [2] = gpio[19],
 *									 bit [3] = gpio[20],
 *									 bit [4] = gpio[21],
 *									 bit [5] = gpio[22],
 *									 bit [6] = gpio[23],
 *									 bit [7] = rgb[0].
 *					  if byteId = 5, bit [0] = rgb[1],
 *									 bit [1] = rgb[2],
 *									 bit [2] = rgb[3],
 *									 bit [3] = rgb[4],
 *									 bit [4] = rgb[5],
 *									 bit [5] = rgb[6],
 *									 bit [6] = rgb[7],
 *									 bit [7] = rgb[8].
 *					  if byteId = 6, bit [0] = rgb[9],
 *									 bit [1] = tggpio[4],
 *									 bit [2] = tggpio[5].
 *					  Others: Not allowed.
 * @retval	None.
*/
void sp1kPullEnSet(UINT8 byteId, UINT8 msk, UINT8 en);

/**
 * @fn UINT8 sp1kPullEnGet(UINT8 byteId, UINT8 msk)
 * @brief	Get pull high/low enable in byte.
 * @param	byteId :  byte ID, 0~6. 
 * @see		sp1kPullEnSet
 * @param	msk : bitmap mask. please refer to param 'en' in sp1kPullEnSet().
 * @retval	bitmap pull high/low, please refer to param 'en' in sp1kPullEnSet().
*/
UINT8 sp1kPullEnGet(UINT8 byteId, UINT8 msk);

/**
 * @fn		void sp1kSDWPDectCfg(void)
 * @brief		SD card write protect detect configure
 * @param	NONE
 * @retval	NONE
 * @see		NULL
 * @author	kai.wang
 * @since		2008-03-01
*/
void sp1kSDWPDectCfg(void);

/**
 * @fn		UINT8 sp1kSDWriteProtectDetectGet(void)
 * @brief		SD card write protect detect value get
 * @param	NONE
 * @retval	current detect value
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kSDWriteProtectDetectGet(void);

/**
 * @fn        void sp1kSD_CdCfg(void)
 * @brief     SD card  detect  configure
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSD_CdCfg(void);

/**
 * @fn        UINT8 sp1kSD_CdGet(void)
 * @brief     SD card  detect value get
 * @param     NONE
 * @retval    current detect value
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kSD_CdGet(void);

/**
 * @fn        void sp1kST_TrigCfg(void)
 * @brief     ST trig configure
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kST_TrigCfg(void);

/**
 * @fn        void sp1kST_TrigSet(UINT8 value)
 * @brief     ST trig Set
 * @param     [in] value: set value
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kST_TrigSet(UINT8 value);

/**
 * @fn       	void sp1kSTChgRdyCfg(void)
 * @brief		ST CHG ready configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSTChgRdyCfg(void);

/**
 * @fn            UINT8 sp1kSTChgRdyGet(void)
 * @brief		ST CHG ready value get
 * @param	NONE
 * @retval	value: current ST CHG ready Get
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kSTChgRdyGet(void);

/**
 * @fn            void sp1kFrexCfg(void)
 * @brief		Frex configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kFrexCfg(void);					//Jim@0903 - M_SHT

/**
 * @fn            void sp1kFrexSet(UINT8 value)
 * @brief		Frex Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kFrexSet(UINT8 value);

/**
 * @fn		void sp1kExpstbCfg(void)
 * @brief		Expstb configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kExpstbCfg(void);

/**
 * @fn		void sp1kExpstbSet(UINT8 value)
 * @brief		Expstb Set
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kExpstbSet(UINT8 value);

/**
 * @fn		void sp1kSHT_ACfg(void)
 * @brief		configure Mshutter
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSHT_ACfg(void);

/**
 * @fn            void sp1kSHT_ASet(UINT8 value)
 * @brief		Enalbe MShutter 
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSHT_ASet(UINT8 value);

/**
 * @fn		void sp1kSHT_BCfg(void)
 * @brief		configure MShutter Director
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSHT_BCfg(void);

/**
 * @fn		void sp1kSHT_BSet(UINT8 value)
 * @brief		Set MShutter Director
 * @param	value : set value
 * @retval	NONE
 * @see
 * @author	Jima
 * @since		2008-09-03
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kSHT_BSet(UINT8 value);

/**
 * @fn		void sp1kPWR_HoldCfg(void);
 * @brief		power hold gpio configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kPWR_HoldCfg(void);

/**
 * @fn		void sp1kPWR_HoldSet(UINT8 value)
 * @brief		power hold gpio Set
 * @param	value : set power hold value 
 *\n                0x00:  set pwr hold pin low, 
 *\n                0xFF:  set pwr hold pin high
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kPWR_HoldSet(UINT8 value);

/**
 * @fn		void sp1kLED_SelfCfg(void)
 * @brief		self snap LED configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLED_SelfCfg(void);

/**
 * @fn		void sp1kLED_SelfSet(UINT8 value)
 * @brief		self snap LED Set
 * @param	value : set value
 *\n                0x00:  set led pin low, 
 *\n                0xFF:  set led pin high
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLED_SelfSet(UINT8 value);

/**
 * @fn		void sp1kLED_BusyCfg(void)
 * @brief		camera busy led configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLED_BusyCfg(void);

/**
 * @fn		void sp1kLED_BusySet(UINT8 value)
 * @brief		camera busy led set
 * @param	value : set value
 * @retval	NONE
 *\n                0x00:  set led pin low, 
 *\n                0xFF:  set led pin high
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLED_BusySet(UINT8 value);

/**
 * @fn		void sp1kLCD_SenCfg(void)
 * @brief		LCD SEN configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLCD_SenCfg(void);

/**
 * @fn		void sp1kLCD_SenSet(UINT8 value)
 * @brief		LCD SEN Set
 * @param	value : set value
 *\n                0x00:  set lcd_sen pin low, 
 *\n                0xFF:  set lcd_sen  pin high 
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kLCD_SenSet(UINT8 value);

/**
 * @fn		void sp1kTV_PlugCfg(UINT8 gpio, UINT8 value)
 * @brief		sp1k Turnkey TV plug detect configure
 * @param	gpio: null 
 * @param	value: null
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kTV_PlugCfg(UINT8 gpio, UINT8 value);

/**
 * @fn		void sp1kHDMI_PlugCfg(void)
 * @brief		sp1k Turnkey HDMI plug detect configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kHDMI_PlugCfg(void);

/**
 * @fn 		UINT8 sp1kHDMI_PlugGet(void)
 * @brief		sp1k Turnkey HDMI plug detect value get
 * @param	NONE
 * @retval	value: HDMI plug detect value
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kHDMI_PlugGet(void);

/**
 * @fn 		UINT8 sp1kTV_PlugGet(void)
 * @brief		sp1k Turnkey TV plug detect value get
 * @param	NONE
 * @retval	value: TV plug detect value
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kTV_PlugGet(void);

/**
 * @fn		void sp1kUSB_PlugCfg(void)
 * @brief		sp1k Turnkey USB plug detect configure
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kUSB_PlugCfg(void);

/**
 * @fn		UINT8 sp1kUSB_PlugGet(void)
 * @brief		sp1k Turnkey USB plug detect value get
 * @param	NONE
 * @retval	value: USB plug detect value
 * @see
 * @author	sun yong
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kUSB_PlugGet(void);

/**
 * @fn		void sp1kADCSarSel(UINT8 sar)
 * @brief		select SAR ADC channel
 * @param	sar : select the sar channel
 * @retval	NULL
 * @see
 * @author	kai.wang
 * @since		2011-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kADCSarSel(UINT8 sar);

/**
 * @fn		UINT8 sp1kADCSarGet(void)
 * @brief		Get SAR ADC channel
 * @param	NULL
 * @retval	SAR ADC channel
 * @see
 * @author	kai.wang
 * @since		2011-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT8 sp1kADCSarGet(void);

/**
 * @fn		UINT16 sp1kADCValueSnap(void)
 * @brief		snap ADC value
 * @param	NULL
 * @retval	ADC value
 * @see
 * @author	kai.wang
 * @since		2011-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT16 sp1kADCValueSnap(void);

/**
 * @fn		UINT16 sp1kADCValueSnapIntr(void)
 * @brief		snap ADC value just used in interrupt 
 * @param	NULL
 * @retval	ADC value
 * @see
 * @author	kai.wang
 * @since		2011-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
UINT16 sp1kADCValueSnapIntr(void);

/**
 * @fn		void sp1kAdcInit(void)
 * @brief		initialize adc gpio
 * @param	NONE
 * @retval	NONE
 * @see
 * @author	phil
 * @since		2008-03-01
 * @todo		Something need to be done.
 * @bug		NONE.
*/
void sp1kAdcInit(void);

/**
 * @fn        UINT8 sp1kAdcValGet(UINT8 channel, UINT16 *adcval)
 * @brief     sp1kAdcValGet
 * @param     [in] channel :0~4
 * @param     [in] adcval :get the value of Sar adc
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kAdcValGet(UINT8 channel, UINT16 * adcval);

/**
 * @fn        void sp1kSTChgEnCfg(void)
 * @brief     ST Charge Enable configure
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSTChgEnCfg(void);

/**
 * @fn        void sp1kSTChgEnSet(UINT8 value)
 * @brief     ST Charge Enable set
 * @param     [in] value: set value
 *\n             0x00 :disable 
 *\n             0xff: enable
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void sp1kSTChgEnSet(UINT8 value);


#endif  /* __TEMPLATE_HDR_H__ */

