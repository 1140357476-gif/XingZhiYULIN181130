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
 * @file      app_suspend.h
 * @brief     Host configure the IO setting before entering suspend mode
 * @author    Phil Lin
 * @since     2011-03-01
 * @date      2011-03-01
*/

#ifndef __APP_SUSPEND_H__
#define __APP_SUSPEND_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define STS_DEF  -1 /* Keep the previous status */

#define PULL_ON   1 /* Enable internal pull */
#define PULL_OFF  0 /* Disable internal pull */
#define PULL_UP   1 /* Set internal pull up */
#define PULL_DN   0 /* Set internal pull down */
#define SEL_GPIO  1 /* Select the IO to general GPIO */
#define SEL_FUNC  0 /* Select the IO to function GPIO */
#define OUT_HI    1 /* Output high */
#define OUT_LO    0 /* Output low */
#define DIR_OUT   1 /* Set the IO direction as output */
#define DIR_IN    0 /* Set the IO direction as input */

/**************************************************************************
 *                           GPIO PULL LIST                               *
 **************************************************************************/

/* FM GPIO */

/* Available items: STS_DEF | PULL_ON | PULL_OFF */

#define APP_SPD_GPIO_PULL_FM_00 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_01 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_02 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_03 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_04 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_05 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_06 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_07 PULL_OFF

#define APP_SPD_GPIO_PULL_FM_08 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_09 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_10 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_11 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_12 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_13 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_14 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_15 PULL_OFF

#define APP_SPD_GPIO_PULL_FM_16 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_17 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_18 PULL_OFF
#define APP_SPD_GPIO_PULL_FM_19 PULL_OFF

/* MA GPIO */

/* Available items: STS_DEF | PULL_ON | PULL_OFF */

#define APP_SPD_GPIO_PULL_MA_00 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_01 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_02 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_03 PULL_OFF

#define APP_SPD_GPIO_PULL_MA_04 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_05 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_06 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_07 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_08 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_09 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_10 PULL_OFF
#define APP_SPD_GPIO_PULL_MA_11 PULL_OFF

#define APP_SPD_GPIO_PULL_MA_14 PULL_OFF

/* GENERAL GPIO */

/* Available items: STS_DEF | PULL_ON | PULL_OFF */

#define APP_SPD_GPIO_PULL_GEN_00 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_01 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_02 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_03 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_04 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_05 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_06 PULL_OFF

#define APP_SPD_GPIO_PULL_GEN_07 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_08 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_09 PULL_ON    //GPIO 9
#define APP_SPD_GPIO_PULL_GEN_10 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_11 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_12 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_13 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_14 PULL_OFF

#define APP_SPD_GPIO_PULL_GEN_15 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_16 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_17 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_18 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_19 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_20 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_21 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_22 PULL_OFF

#define APP_SPD_GPIO_PULL_GEN_23 PULL_OFF
#define APP_SPD_GPIO_PULL_GEN_24 PULL_OFF

/* SAR GPIO */

/* Available items: STS_DEF | PULL_ON | PULL_OFF */

#define APP_SPD_GPIO_PULL_SAR_03 PULL_OFF
#define APP_SPD_GPIO_PULL_SAR_04 PULL_OFF

/* DISP GPIO */

/* Available items: STS_DEF | PULL_ON | PULL_OFF */

#define APP_SPD_GPIO_PULL_DISP_00 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_01 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_02 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_03 PULL_OFF

#define APP_SPD_GPIO_PULL_DISP_04 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_05 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_06 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_07 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_08 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_09 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_10 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_11 PULL_OFF

#define APP_SPD_GPIO_PULL_DISP_12 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_13 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_14 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_15 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_16 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_17 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_18 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_19 PULL_OFF

#define APP_SPD_GPIO_PULL_DISP_20 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_21 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_22 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_23 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_24 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_25 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_26 PULL_OFF
#define APP_SPD_GPIO_PULL_DISP_27 PULL_OFF

/* RGB GPIO */

/* Available items: STS_DEF | PULL_ON | PULL_OFF */

#define APP_SPD_GPIO_PULL_RGB_00 PULL_OFF
#define APP_SPD_GPIO_PULL_RGB_01 PULL_OFF
#define APP_SPD_GPIO_PULL_RGB_02 PULL_OFF
#define APP_SPD_GPIO_PULL_RGB_03 PULL_OFF
#define APP_SPD_GPIO_PULL_RGB_04 PULL_OFF
#define APP_SPD_GPIO_PULL_RGB_05 PULL_OFF
#define APP_SPD_GPIO_PULL_RGB_06 PULL_OFF
#define APP_SPD_GPIO_PULL_RGB_07 PULL_OFF

#define APP_SPD_GPIO_PULL_RGB_08 PULL_OFF
#define APP_SPD_GPIO_PULL_RGB_09 PULL_OFF

/* TG GPIO */

/* Available items: STS_DEF | PULL_ON | PULL_OFF */

#define APP_SPD_GPIO_PULL_TG_00 PULL_OFF
#define APP_SPD_GPIO_PULL_TG_01 PULL_OFF
#define APP_SPD_GPIO_PULL_TG_02 PULL_OFF
#define APP_SPD_GPIO_PULL_TG_03 PULL_OFF
#define APP_SPD_GPIO_PULL_TG_04 PULL_OFF
#define APP_SPD_GPIO_PULL_TG_05 PULL_OFF

/**************************************************************************
 *                           GPIO FUNC LIST                               *
 **************************************************************************/

#define APP_SPD_GPIO_FUNC_TG_00 STS_DEF
#define APP_SPD_GPIO_FUNC_TG_01 STS_DEF
#define APP_SPD_GPIO_FUNC_TG_02 STS_DEF
#define APP_SPD_GPIO_FUNC_TG_03 STS_DEF
#define APP_SPD_GPIO_FUNC_TG_04 STS_DEF
#define APP_SPD_GPIO_FUNC_TG_05 STS_DEF

#define APP_SPD_GPIO_FUNC_DISP_00 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_01 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_02 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_03 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_04 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_05 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_06 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_07 STS_DEF

#define APP_SPD_GPIO_FUNC_DISP_08 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_09 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_10 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_11 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_12 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_13 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_14 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_15 STS_DEF

#define APP_SPD_GPIO_FUNC_DISP_16 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_17 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_18 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_19 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_20 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_21 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_22 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_23 STS_DEF

#define APP_SPD_GPIO_FUNC_DISP_24 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_25 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_26 STS_DEF
#define APP_SPD_GPIO_FUNC_DISP_27 STS_DEF

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define APP_SPD_VAL_CHK(v0,v1,v2,v3,v4,v5,v6,v7) \
(\
	(((v0) == STS_DEF ? 0 : ((v0) ? 1 : 0)) << 0) |\
	(((v1) == STS_DEF ? 0 : ((v1) ? 1 : 0)) << 1) |\
	(((v2) == STS_DEF ? 0 : ((v2) ? 1 : 0)) << 2) |\
	(((v3) == STS_DEF ? 0 : ((v3) ? 1 : 0)) << 3) |\
	(((v4) == STS_DEF ? 0 : ((v4) ? 1 : 0)) << 4) |\
	(((v5) == STS_DEF ? 0 : ((v5) ? 1 : 0)) << 5) |\
	(((v6) == STS_DEF ? 0 : ((v6) ? 1 : 0)) << 6) |\
	(((v7) == STS_DEF ? 0 : ((v7) ? 1 : 0)) << 7)\
)

#define APP_SPD_MASK_CHK(v0,v1,v2,v3,v4,v5,v6,v7) \
(\
	(((v0) == STS_DEF ? 0 : ((v0) ? 1 : 0)) << 0) |\
	(((v1) == STS_DEF ? 0 : ((v1) ? 1 : 0)) << 1) |\
	(((v2) == STS_DEF ? 0 : ((v2) ? 1 : 0)) << 2) |\
	(((v3) == STS_DEF ? 0 : ((v3) ? 1 : 0)) << 3) |\
	(((v4) == STS_DEF ? 0 : ((v4) ? 1 : 0)) << 4) |\
	(((v5) == STS_DEF ? 0 : ((v5) ? 1 : 0)) << 5) |\
	(((v6) == STS_DEF ? 0 : ((v6) ? 1 : 0)) << 6) |\
	(((v7) == STS_DEF ? 0 : ((v7) ? 1 : 0)) << 7)\
)


#define APP_SPD_GPIO_PULL_MASK_FM_0 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_FM_00,\
	APP_SPD_GPIO_PULL_FM_01,\
	APP_SPD_GPIO_PULL_FM_02,\
	APP_SPD_GPIO_PULL_FM_03,\
	APP_SPD_GPIO_PULL_FM_04,\
	APP_SPD_GPIO_PULL_FM_05,\
	APP_SPD_GPIO_PULL_FM_06,\
	APP_SPD_GPIO_PULL_FM_07\
)

#define APP_SPD_GPIO_PULL_VAL_FM_0 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_FM_00,\
	APP_SPD_GPIO_PULL_FM_01,\
	APP_SPD_GPIO_PULL_FM_02,\
	APP_SPD_GPIO_PULL_FM_03,\
	APP_SPD_GPIO_PULL_FM_04,\
	APP_SPD_GPIO_PULL_FM_05,\
	APP_SPD_GPIO_PULL_FM_06,\
	APP_SPD_GPIO_PULL_FM_07\
)

#define APP_SPD_GPIO_PULL_MASK_FM_1 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_FM_08,\
	APP_SPD_GPIO_PULL_FM_09,\
	APP_SPD_GPIO_PULL_FM_10,\
	APP_SPD_GPIO_PULL_FM_11,\
	APP_SPD_GPIO_PULL_FM_12,\
	APP_SPD_GPIO_PULL_FM_13,\
	APP_SPD_GPIO_PULL_FM_14,\
	APP_SPD_GPIO_PULL_FM_15\
)

#define APP_SPD_GPIO_PULL_VAL_FM_1 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_FM_08,\
	APP_SPD_GPIO_PULL_FM_09,\
	APP_SPD_GPIO_PULL_FM_10,\
	APP_SPD_GPIO_PULL_FM_11,\
	APP_SPD_GPIO_PULL_FM_12,\
	APP_SPD_GPIO_PULL_FM_13,\
	APP_SPD_GPIO_PULL_FM_14,\
	APP_SPD_GPIO_PULL_FM_15\
)

#define APP_SPD_GPIO_PULL_MASK_FM_2 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_FM_16,\
	APP_SPD_GPIO_PULL_FM_17,\
	APP_SPD_GPIO_PULL_FM_18,\
	APP_SPD_GPIO_PULL_FM_19,\
	APP_SPD_GPIO_PULL_MA_00,\
	APP_SPD_GPIO_PULL_MA_01,\
	APP_SPD_GPIO_PULL_MA_02,\
	APP_SPD_GPIO_PULL_MA_03\
)

#define APP_SPD_GPIO_PULL_VAL_FM_2 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_FM_16,\
	APP_SPD_GPIO_PULL_FM_17,\
	APP_SPD_GPIO_PULL_FM_18,\
	APP_SPD_GPIO_PULL_FM_19,\
	APP_SPD_GPIO_PULL_MA_00,\
	APP_SPD_GPIO_PULL_MA_01,\
	APP_SPD_GPIO_PULL_MA_02,\
	APP_SPD_GPIO_PULL_MA_03\
)

#define APP_SPD_GPIO_PULL_MASK_MA_0 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_MA_04,\
	APP_SPD_GPIO_PULL_MA_05,\
	APP_SPD_GPIO_PULL_MA_06,\
	APP_SPD_GPIO_PULL_MA_07,\
	APP_SPD_GPIO_PULL_MA_08,\
	APP_SPD_GPIO_PULL_MA_09,\
	APP_SPD_GPIO_PULL_MA_10,\
	APP_SPD_GPIO_PULL_MA_11\
)

#define APP_SPD_GPIO_PULL_VAL_MA_0 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_MA_04,\
	APP_SPD_GPIO_PULL_MA_05,\
	APP_SPD_GPIO_PULL_MA_06,\
	APP_SPD_GPIO_PULL_MA_07,\
	APP_SPD_GPIO_PULL_MA_08,\
	APP_SPD_GPIO_PULL_MA_09,\
	APP_SPD_GPIO_PULL_MA_10,\
	APP_SPD_GPIO_PULL_MA_11\
)

#define APP_SPD_GPIO_PULL_MASK_GEN_0 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_MA_14,\
	APP_SPD_GPIO_PULL_GEN_00,\
	APP_SPD_GPIO_PULL_GEN_01,\
	APP_SPD_GPIO_PULL_GEN_02,\
	APP_SPD_GPIO_PULL_GEN_03,\
	APP_SPD_GPIO_PULL_GEN_04,\
	APP_SPD_GPIO_PULL_GEN_05,\
	APP_SPD_GPIO_PULL_GEN_06\
)

#define APP_SPD_GPIO_PULL_VAL_GEN_0 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_MA_14,\
	APP_SPD_GPIO_PULL_GEN_00,\
	APP_SPD_GPIO_PULL_GEN_01,\
	APP_SPD_GPIO_PULL_GEN_02,\
	APP_SPD_GPIO_PULL_GEN_03,\
	APP_SPD_GPIO_PULL_GEN_04,\
	APP_SPD_GPIO_PULL_GEN_05,\
	APP_SPD_GPIO_PULL_GEN_06\
)

#define APP_SPD_GPIO_PULL_MASK_GEN_1 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_GEN_07,\
	APP_SPD_GPIO_PULL_GEN_08,\
	APP_SPD_GPIO_PULL_GEN_09,\
	APP_SPD_GPIO_PULL_GEN_10,\
	APP_SPD_GPIO_PULL_GEN_11,\
	APP_SPD_GPIO_PULL_GEN_12,\
	APP_SPD_GPIO_PULL_GEN_13,\
	APP_SPD_GPIO_PULL_GEN_14\
)

#define APP_SPD_GPIO_PULL_VAL_GEN_1 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_GEN_07,\
	APP_SPD_GPIO_PULL_GEN_08,\
	APP_SPD_GPIO_PULL_GEN_09,\
	APP_SPD_GPIO_PULL_GEN_10,\
	APP_SPD_GPIO_PULL_GEN_11,\
	APP_SPD_GPIO_PULL_GEN_12,\
	APP_SPD_GPIO_PULL_GEN_13,\
	APP_SPD_GPIO_PULL_GEN_14\
)

#define APP_SPD_GPIO_PULL_MASK_GEN_2 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_GEN_15,\
	APP_SPD_GPIO_PULL_GEN_16,\
	APP_SPD_GPIO_PULL_GEN_17,\
	APP_SPD_GPIO_PULL_GEN_18,\
	APP_SPD_GPIO_PULL_GEN_19,\
	APP_SPD_GPIO_PULL_GEN_20,\
	APP_SPD_GPIO_PULL_GEN_21,\
	APP_SPD_GPIO_PULL_GEN_22\
)

#define APP_SPD_GPIO_PULL_VAL_GEN_2 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_GEN_15,\
	APP_SPD_GPIO_PULL_GEN_16,\
	APP_SPD_GPIO_PULL_GEN_17,\
	APP_SPD_GPIO_PULL_GEN_18,\
	APP_SPD_GPIO_PULL_GEN_19,\
	APP_SPD_GPIO_PULL_GEN_20,\
	APP_SPD_GPIO_PULL_GEN_21,\
	APP_SPD_GPIO_PULL_GEN_22\
)

#define APP_SPD_GPIO_PULL_MASK_DISP_0 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_GEN_23,\
	APP_SPD_GPIO_PULL_GEN_24,\
	APP_SPD_GPIO_PULL_SAR_03,\
	APP_SPD_GPIO_PULL_SAR_04,\
	APP_SPD_GPIO_PULL_DISP_00,\
	APP_SPD_GPIO_PULL_DISP_01,\
	APP_SPD_GPIO_PULL_DISP_02,\
	APP_SPD_GPIO_PULL_DISP_03\
)

#define APP_SPD_GPIO_PULL_VAL_DISP_0 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_GEN_23,\
	APP_SPD_GPIO_PULL_GEN_24,\
	APP_SPD_GPIO_PULL_SAR_03,\
	APP_SPD_GPIO_PULL_SAR_04,\
	APP_SPD_GPIO_PULL_DISP_00,\
	APP_SPD_GPIO_PULL_DISP_01,\
	APP_SPD_GPIO_PULL_DISP_02,\
	APP_SPD_GPIO_PULL_DISP_03\
)

#define APP_SPD_GPIO_PULL_MASK_DISP_1 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_DISP_04,\
	APP_SPD_GPIO_PULL_DISP_05,\
	APP_SPD_GPIO_PULL_DISP_06,\
	APP_SPD_GPIO_PULL_DISP_07,\
	APP_SPD_GPIO_PULL_DISP_08,\
	APP_SPD_GPIO_PULL_DISP_09,\
	APP_SPD_GPIO_PULL_DISP_10,\
	APP_SPD_GPIO_PULL_DISP_11\
)

#define APP_SPD_GPIO_PULL_VAL_DISP_1 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_DISP_04,\
	APP_SPD_GPIO_PULL_DISP_05,\
	APP_SPD_GPIO_PULL_DISP_06,\
	APP_SPD_GPIO_PULL_DISP_07,\
	APP_SPD_GPIO_PULL_DISP_08,\
	APP_SPD_GPIO_PULL_DISP_09,\
	APP_SPD_GPIO_PULL_DISP_10,\
	APP_SPD_GPIO_PULL_DISP_11\
)

#define APP_SPD_GPIO_PULL_MASK_DISP_2 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_DISP_12,\
	APP_SPD_GPIO_PULL_DISP_13,\
	APP_SPD_GPIO_PULL_DISP_14,\
	APP_SPD_GPIO_PULL_DISP_15,\
	APP_SPD_GPIO_PULL_DISP_16,\
	APP_SPD_GPIO_PULL_DISP_17,\
	APP_SPD_GPIO_PULL_DISP_18,\
	APP_SPD_GPIO_PULL_DISP_19\
)

#define APP_SPD_GPIO_PULL_VAL_DISP_2 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_DISP_12,\
	APP_SPD_GPIO_PULL_DISP_13,\
	APP_SPD_GPIO_PULL_DISP_14,\
	APP_SPD_GPIO_PULL_DISP_15,\
	APP_SPD_GPIO_PULL_DISP_16,\
	APP_SPD_GPIO_PULL_DISP_17,\
	APP_SPD_GPIO_PULL_DISP_18,\
	APP_SPD_GPIO_PULL_DISP_19\
)

#define APP_SPD_GPIO_PULL_MASK_DISP_3 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_DISP_20,\
	APP_SPD_GPIO_PULL_DISP_21,\
	APP_SPD_GPIO_PULL_DISP_22,\
	APP_SPD_GPIO_PULL_DISP_23,\
	APP_SPD_GPIO_PULL_DISP_24,\
	APP_SPD_GPIO_PULL_DISP_25,\
	APP_SPD_GPIO_PULL_DISP_26,\
	APP_SPD_GPIO_PULL_DISP_27\
)

#define APP_SPD_GPIO_PULL_VAL_DISP_3 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_DISP_20,\
	APP_SPD_GPIO_PULL_DISP_21,\
	APP_SPD_GPIO_PULL_DISP_22,\
	APP_SPD_GPIO_PULL_DISP_23,\
	APP_SPD_GPIO_PULL_DISP_24,\
	APP_SPD_GPIO_PULL_DISP_25,\
	APP_SPD_GPIO_PULL_DISP_26,\
	APP_SPD_GPIO_PULL_DISP_27\
)

#define APP_SPD_GPIO_PULL_MASK_RGB_0 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_RGB_00,\
	APP_SPD_GPIO_PULL_RGB_01,\
	APP_SPD_GPIO_PULL_RGB_02,\
	APP_SPD_GPIO_PULL_RGB_03,\
	APP_SPD_GPIO_PULL_RGB_04,\
	APP_SPD_GPIO_PULL_RGB_05,\
	APP_SPD_GPIO_PULL_RGB_06,\
	APP_SPD_GPIO_PULL_RGB_07\
)

#define APP_SPD_GPIO_PULL_VAL_RGB_0 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_RGB_00,\
	APP_SPD_GPIO_PULL_RGB_01,\
	APP_SPD_GPIO_PULL_RGB_02,\
	APP_SPD_GPIO_PULL_RGB_03,\
	APP_SPD_GPIO_PULL_RGB_04,\
	APP_SPD_GPIO_PULL_RGB_05,\
	APP_SPD_GPIO_PULL_RGB_06,\
	APP_SPD_GPIO_PULL_RGB_07\
)

#define APP_SPD_GPIO_PULL_MASK_TG_0 APP_SPD_MASK_CHK \
(\
	APP_SPD_GPIO_PULL_RGB_08,\
	APP_SPD_GPIO_PULL_RGB_09,\
	APP_SPD_GPIO_PULL_TG_00,\
	APP_SPD_GPIO_PULL_TG_01,\
	APP_SPD_GPIO_PULL_TG_02,\
	APP_SPD_GPIO_PULL_TG_03,\
	APP_SPD_GPIO_PULL_TG_04,\
	APP_SPD_GPIO_PULL_TG_05\
)

#define APP_SPD_GPIO_PULL_VAL_TG_0 APP_SPD_VAL_CHK \
(\
	APP_SPD_GPIO_PULL_RGB_08,\
	APP_SPD_GPIO_PULL_RGB_09,\
	APP_SPD_GPIO_PULL_TG_00,\
	APP_SPD_GPIO_PULL_TG_01,\
	APP_SPD_GPIO_PULL_TG_02,\
	APP_SPD_GPIO_PULL_TG_03,\
	APP_SPD_GPIO_PULL_TG_04,\
	APP_SPD_GPIO_PULL_TG_05\
)

#define appSpdGpioPullEnSet() \
{\
	gpioPullEnSet(0,  0xFF /*APP_SPD_GPIO_PULL_MASK_FM_0*/,   APP_SPD_GPIO_PULL_VAL_FM_0);\
	gpioPullEnSet(1,  0xFF/*APP_SPD_GPIO_PULL_MASK_FM_1*/,   APP_SPD_GPIO_PULL_VAL_FM_1);\
	gpioPullEnSet(2,  0xFF/*APP_SPD_GPIO_PULL_MASK_FM_2*/,   APP_SPD_GPIO_PULL_VAL_FM_2);\
	gpioPullEnSet(3,  0xFF/*APP_SPD_GPIO_PULL_MASK_MA_0*/,   APP_SPD_GPIO_PULL_VAL_MA_0);\
	gpioPullEnSet(4,  0xFF/*APP_SPD_GPIO_PULL_MASK_GEN_0*/,  APP_SPD_GPIO_PULL_VAL_GEN_0);\
	gpioPullEnSet(5,  0xFF/* APP_SPD_GPIO_PULL_MASK_GEN_1*/,  APP_SPD_GPIO_PULL_VAL_GEN_1);\
	gpioPullEnSet(6,  0xFF /*APP_SPD_GPIO_PULL_MASK_GEN_2*/,  APP_SPD_GPIO_PULL_VAL_GEN_2);\
	gpioPullEnSet(7,  0xFF/*APP_SPD_GPIO_PULL_MASK_DISP_0*/, APP_SPD_GPIO_PULL_VAL_DISP_0);\
	gpioPullEnSet(8,  0xFF/*APP_SPD_GPIO_PULL_MASK_DISP_1*/, APP_SPD_GPIO_PULL_VAL_DISP_1);\
	gpioPullEnSet(9,  0xFF/*APP_SPD_GPIO_PULL_MASK_DISP_2*/, APP_SPD_GPIO_PULL_VAL_DISP_2);\
	gpioPullEnSet(10, 0xFF /*APP_SPD_GPIO_PULL_MASK_DISP_3*/, APP_SPD_GPIO_PULL_VAL_DISP_3);\
	gpioPullEnSet(11, 0xFF/*APP_SPD_GPIO_PULL_MASK_RGB_0*/,  APP_SPD_GPIO_PULL_VAL_RGB_0);\
	gpioPullEnSet(12, 0xFF/*APP_SPD_GPIO_PULL_MASK_TG_0*/,   APP_SPD_GPIO_PULL_VAL_TG_0);\
}

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
#define SUSPEND_TEST 0

UINT8 appSuspendResume(void);
void appRsmAsPwrOn(void);
#endif  /* __APP_SUSPEND_H__ */

