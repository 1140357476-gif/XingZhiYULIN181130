/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		hal_disp.h
 * @brief		hal display header file.
 * @author		Ted Chiu, Matt Wang
 * @since		2008-03-14
 * @date		2008-10-03
 */

#ifndef __HAL_DISP_H__
#define __HAL_DISP_H__

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
typedef enum {
	HAL_DISP_FIT_METHOD_SRC_STRETCH,
	HAL_DISP_FIT_METHOD_DST_OUTSIDE,
	HAL_DISP_FIT_METHOD_TOT,
} HAL_DispFitMethod_e;

typedef enum {
    HAL_DISP_PARA_SW_RST,
	HAL_DISP_PARA_CLK_ENABLE,
	HAL_DISP_PARA_CLK_CFG,
	HAL_DISP_PARA_CLK_PHASE_DLY_CFG,
	HAL_DISP_PARA_CLK_SIGNAL_POLAR_CFG,
	HAL_DISP_PARA_GLOB_ENABLE,
	HAL_DISP_PARA_IMG_LAYER_ENABLE,
	HAL_DISP_PARA_IMG_LAYER_FLIP,
	HAL_DISP_PARA_IMG_LAYER_MONOCHROME_ENABLE,
	HAL_DISP_PARA_OSD_LAYER_ENABLE,
	HAL_DISP_PARA_BG_COLOR,
	HAL_DISP_PARA_REQ_LENTH_SET,
	HAL_DISP_PARA_DEVICE_TYPE,
	HAL_DISP_PARA_DEVICE_TOTAL_REGION,
	HAL_DISP_PARA_DEVICE_ACTIVE_REGION,
    HAL_DISP_PARA_DEVICE_IMG_REGION_CROP,
    HAL_DISP_PARA_DEVICE_IMG_REGION_RENDER,
    HAL_DISP_PARA_DEVICE_OSD_REGION_CROP,
    HAL_DISP_PARA_DEVICE_OSD_REGION_RENDER,
	HAL_DISP_PARA_OSD_BLEND_FACTOR,
	HAL_DISP_PARA_INTERLACE_CFG,
	HAL_DISP_PARA_LCD_DITHER_ENABLE,
	HAL_DISP_PARA_UPS051_PIXEL_SHIFT_CFG,
	HAL_DISP_PARA_UPS051_LIGHTVIEW_ENABLE,
	HAL_DISP_PARA_UPS051_RGB888_ENABLE,
	HAL_DISP_PARA_UPS052_RGB_ONLY_ENABLE,
	HAL_DISP_PARA_UPS_HSD_DELAY_CFG,
	HAL_DISP_PARA_UPS_CLK_PHASE,
	HAL_DISP_PARA_UPS_LAYOUT_MODE,
	HAL_DISP_PARA_UPS_COLOR_FILTER_CFG,
	HAL_DISP_PARA_CCIR_OUTPUT_SEQUENCE_CFG,
	HAL_DISP_PARA_EDGE_TYPE_SET,
	HAL_DISP_PARA_EDGE_GAIN_SET,
	HAL_DISP_PARA_GAMMA_ENABLE,
	HAL_DISP_PARA_RB_SWITCH_SET,
	HAL_DISP_PARA_COLOR_MATRIX_TBL_CFG,
	HAL_DISP_PARA_COLOR_MATRIX_ENABLE,
	HAL_DISP_PARA_COLOR_MATRIX_FMT_CFG,
	HAL_DISP_PARA_TV_DAC_POWER_ENABLE,
	HAL_DISP_PARA_TV_SATURATION,
	HAL_DISP_PARA_TV_HUE,
	HAL_DISP_PARA_LCM_CMD_BUS_TYPE,
	HAL_DISP_PARA_LCM_CMD_BIT_WIDTH,
	HAL_DISP_PARA_LCM_CMD_PULSE_WIDTH,
	HAL_DISP_PARA_LCM_CMD_ACTIVE_LEVEL,
	HAL_DISP_PARA_LCM_DATA_FORMAT,
	HAL_DISP_PARA_LCM_RGB666_PACK_ENABLE,
	HAL_DISP_PARA_LCM_RGB666_FORMAT_10_ENABLE,
	HAL_DISP_PARA_LCM_PIXEL_ALIGN,
	HAL_DISP_PARA_LCM_COLOR_PACKAGE,
	HAL_DISP_PARA_LCM_SWITCH_G_ENABLE,
	HAL_DISP_PARA_LCM_CHANGE_RB_ENABLE,
	HAL_DISP_PARA_LCM_ONE_HALF_PIXEL_ENABLE,
	HAL_DISP_PARA_LCM_DITHER_ENABLE,
	HAL_DISP_PARA_MAX_ID,
} HAL_DispParaId_e;


typedef enum {
    HAL_DISP_DEV_RST_LOCAL = (0x01 << 0),
    HAL_DISP_DEV_RST_PNL   = (0x01 << 1),
    HAL_DISP_DEV_RST_TV    = (0x01 << 2),
    HAL_DISP_DEV_RST_ALL   = (HAL_DISP_DEV_RST_LOCAL | HAL_DISP_DEV_RST_PNL | HAL_DISP_DEV_RST_TV),
} HAL_DispRstDev_e; 

typedef enum {
	HAL_DISP_CLK_SRC_SYSPLL522M = 0,
	HAL_DISP_CLK_SRC_XTAL27M    = 1,
} HAL_DispClkSrc_e;

typedef enum {
	HAL_DISP_CLK_PHASE_DLY_0T = 0,
	HAL_DISP_CLK_PHASE_DLY_1T = 1,
	HAL_DISP_CLK_PHASE_DLY_2T = 2,
	HAL_DISP_CLK_PHASE_DLY_3T = 3,
	HAL_DISP_CLK_PHASE_DLY_4T = 4,
	HAL_DISP_CLK_PHASE_DLY_5T = 5,
	HAL_DISP_CLK_PHASE_DLY_6T = 6,
	HAL_DISP_CLK_PHASE_DLY_7T = 7,
} HAL_DispClkPhaseDly_e;

typedef enum {
	HAL_DISP_SIGNAL_CLK_NORMAL     = (0 << 3),
	HAL_DISP_SIGNAL_CLK_INVERSE    = (1 << 3),

	HAL_DISP_SIGNAL_VSYNC_ACT_LOW  = (0 << 4),
	HAL_DISP_SIGNAL_VSYNC_ACT_HIGH = (1 << 4),

	HAL_DISP_SIGNAL_HSYNC_ACT_LOW  = (0 << 5),
	HAL_DISP_SIGNAL_HSYNC_ACT_HIGH = (1 << 5),
} HAL_DispSignalPolar_e;

typedef enum {
	HAL_DISP_GLOB_BLEND_SRC_IMAGE = (0 << 4),
	HAL_DISP_GLOB_BLEND_SRC_BLACK = (1 << 4),
} HAL_DispGlobBlendSrc_e;

typedef enum {
	HAL_DISP_IMG_REQ_LENTH_16 = (0 << 4),
	HAL_DISP_IMG_REQ_LENTH_32 = (1 << 4),
	HAL_DISP_OSD_REQ_LENTH_16 = (0 << 5),
	HAL_DISP_OSD_REQ_LENTH_32 = (1 << 5),
} HAL_DispReqLenth_e;

typedef enum {
	HAL_DISP_DEVICE_TYPE_COMPOSITE_NTSC  = 0x00,
	HAL_DISP_DEVICE_TYPE_COMPOSITE_PAL   = 0x01,
    HAL_DEVICE_TYPE_CCIR656_NTSC        = 0x02,
    HAL_DEVICE_TYPE_CCIR656_PAL         = 0x03,
	HAL_DISP_DEVICE_TYPE_CCIR601_NTSC_8  = 0x04,
	HAL_DEVICE_TYPE_CCIR601_PAL_8       = 0x05,
	//HAL_DEVICE_TYPE_CCIR601_NTSC_16     = 0x06,
	//HAL_DEVICE_TYPE_CCIR601_PAL_16      = 0x07,
	HAL_DEVICE_TYPE_UPS051              = 0x08,
	HAL_DEVICE_TYPE_UPS052              = 0x09,
	HAL_DEVICE_TYPE_LCM                 = 0x0A,
	//HAL_DEVICE_TYPE_VGA_TFT           = 0x0C,
	//HAL_DEVICE_TYPE_USER_CCIR601_8    = 0x0E,
	//HAL_DEVICE_TYPE_USER_CCIR601_16   = 0x0F,
	HAL_DEVICE_TYPE_CCIR656_INTERLACE   = 0x10,
    HAL_DEVICE_TYPE_CCIR601_INTERLACE   = 0x13,
    HAL_DEVICE_TYPE_CCIR656_PROGRESSIVE = 0x13,
    HAL_DEVICE_TYPE_CCIR601_PROGRESSIVE = 0x13,
} HAL_DispDeviceType_e;

typedef enum {
	HAL_DISP_PROGRESSIVE = 0x00,
	HAL_DISP_INTERLACE   = 0x05,
} HAL_DispInterlaceCfg_e;

typedef enum {
	HAL_DISP_PIXEL_SHIFT_DISABLE = 0x00,
	HAL_DISP_PIXEL_SHIFT_ODD     = 0x10,
	HAL_DISP_PIXEL_SHIFT_EVEN    = 0x30,
} HAL_DispUps051PixelShiftCfg_e;

typedef enum {
	HAL_DISP_UPS052_HSD_DELAY_3T = (0 << 0),
	HAL_DISP_UPS052_HSD_NO_DELAY = (1 << 0),
	HAL_DISP_UPS052_HSD_DELAY_1T = (1 << 1),
	HAL_DISP_UPS052_HSD_DELAY_2T = (1 << 2),
} HAL_DispUps052HsdDelayCfg_e;

typedef enum {
	HAL_DISP_UPS051_HSD_RGB = (0 << 0),
	HAL_DISP_UPS051_HSD_BRG = (1 << 0),
} HAL_DispUps051HsdDelayCfg_e;

typedef enum {
	HAL_DISP_UPS_LAYOUT_MODE_DISABLE       = (0 << 4),
	HAL_DISP_UPS_LAYOUT_MODE_ODD_DELAY_1T  = (1 << 4),
	HAL_DISP_UPS_LAYOUT_MODE_EVEN_DELAY_1T = (3 << 4),
} HAL_DispUpsLayoutMode_e;

typedef enum {
 	HAL_DISP_UPS051_ODD_RGB = (0 << 4),
	HAL_DISP_UPS051_ODD_GBR = (1 << 4),
	HAL_DISP_UPS051_ODD_BRG = (2 << 4),
	HAL_DISP_UPS051_ODD_BGR = (3 << 4),
	HAL_DISP_UPS051_ODD_RBG = (4 << 4),
	HAL_DISP_UPS051_ODD_GRB = (5 << 4),

 	HAL_DISP_UPS052_ODD_RGB = (0 << 4),
	HAL_DISP_UPS052_ODD_GBR = (1 << 4),
	HAL_DISP_UPS052_ODD_BRG = (2 << 4),
	HAL_DISP_UPS052_ODD_RBG = (3 << 4),
	HAL_DISP_UPS052_ODD_BGR = (4 << 4),
	HAL_DISP_UPS052_ODD_GRB = (5 << 4),
} HAL_DispUpsOddColorFilter_e;

typedef enum {
 	HAL_DISP_UPS051_EVEN_BRG = (0 << 0),
 	HAL_DISP_UPS051_EVEN_RGB = (1 << 0),
	HAL_DISP_UPS051_EVEN_GBR = (2 << 0),
	HAL_DISP_UPS051_EVEN_GRB = (3 << 0),
	HAL_DISP_UPS051_EVEN_BGR = (4 << 0),
	HAL_DISP_UPS051_EVEN_RBG = (5 << 0),

 	HAL_DISP_UPS052_EVEN_RGB = (0 << 0),
	HAL_DISP_UPS052_EVEN_GBR = (1 << 0),
	HAL_DISP_UPS052_EVEN_BRG = (2 << 0),
	HAL_DISP_UPS052_EVEN_RBG = (3 << 0),
	HAL_DISP_UPS052_EVEN_BGR = (4 << 0),
	HAL_DISP_UPS052_EVEN_GRB = (5 << 0),
} HAL_DispUpsEvenColorFilter_e;

typedef enum {
	HAL_DISP_COLOR_MATRIX_FMT_RGB   = (0 << 4),
	HAL_DISP_COLOR_MATRIX_FMT_YCBCR = (1 << 4),
	HAL_DISP_COLOR_MATRIX_FMT_YUV   = (2 << 4),
} HAL_DispColorMtxCvsFmt_e;

typedef enum {
	HAL_DISP_CCIR_YCRYCB = (0 << 0),
	HAL_DISP_CCIR_CRYCBY = (1 << 0),
	HAL_DISP_CCIR_YCBYCR = (2 << 0),
	HAL_DISP_CCIR_CBYCRY = (3 << 0),
} HAL_DispCcirOutputSeqCfg_e;

typedef enum {
	HAL_DISP_EDGE_TYPE_A = (0 << 0), //Yedge(n) = Yin(n)
	HAL_DISP_EDGE_TYPE_B = (1 << 0), //Yedge(n) = -0.5*Yin(n-1) + Yin(n) -0.5*Yin(n+1)
	HAL_DISP_EDGE_TYPE_C = (2 << 0), //Yedge(n) = -0.5*Yin(n-2) + Yin(n) -0.5*Yin(n+2)
} HAL_DispEdgeType_e;

typedef enum {
	HAL_DISP_LCM_CMD_BUS_I80 = (0 << 0),
	HAL_DISP_LCM_CMD_BUS_M68 = (1 << 0),
} HAL_DispLcmCommandBusType_e;

typedef enum {
	HAL_DISP_LCM_CMD_8_BIT   = (0 << 4),
    HAL_DISP_LCM_CMD_9_BIT   = (1 << 4),
    HAL_DISP_LCM_CMD_16_BIT  = (2 << 4),
    HAL_DISP_LCM_CMD_18_BIT  = (3 << 4),
    HAL_DISP_LCM_CMD_24_BIT  = (4 << 4),
} HAL_DispLcmCommandBitWidth_e;

typedef enum {
	HAL_DISP_LCM_DATA_TYPE_RGB565 = (0 << 4),
	HAL_DISP_LCM_DATA_TYPE_RGB666 = (1 << 4),
	HAL_DISP_LCM_DATA_TYPE_RGB444 = (2 << 4),
	HAL_DISP_LCM_DATA_TYPE_RGB888 = (3 << 4),
} HAL_DispLcmDataType_e;


typedef enum {
    /* data format of "8 Bit" and "RGB444" */
    HAL_DISP_LCM_DATA_FMT_B8_T444_0, /* {G1[3:0],R1[3:0]} {R2[3:0],B1[3:0]} {B2[3:0],G2[3:0]} */
    HAL_DISP_LCM_DATA_FMT_B8_T444_1, /* {R1[3:0],G1[3:0]} {B1[3:0],R2[3:0]} {G2[3:0],B2[3:0]} */

    /* data format of "8 Bit" and "RGB565" */
    HAL_DISP_LCM_DATA_FMT_B8_T565_0, /* {G[2:0],R[4:0]} {B[4:0],G[5:3]} */
    HAL_DISP_LCM_DATA_FMT_B8_T565_1, /* {R[4:0],G[5:3]} {G[2:0],B[4:0]} */
    HAL_DISP_LCM_DATA_FMT_B8_T565_2, /* {B[4:0],G[5:3]} {G[2:0],R[4:0]} */
    HAL_DISP_LCM_DATA_FMT_B8_T565_3, /* {R[4:0],G[2:0]} {G[5:3],B[4:0]} */
    HAL_DISP_LCM_DATA_FMT_B8_T565_4, /* {B[4:0],G[2:0]} {G[5:3],R[4:0]} */

    /* data format of "8 Bit" and "RGB666" */
    HAL_DISP_LCM_DATA_FMT_B8_T666_0, /* {2'b0, R[5:0]} {2'b0, G[5:0]} {2'b0, B[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B8_T666_1, /* {R[7:0]} {G[7:0]} {B[7:0]} */
    
    /* data format of "9 Bit" and "RGB666" */
    HAL_DISP_LCM_DATA_FMT_B9_T666_0, /* {R[5:0], G[5:3]} {G[2:0], B[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B9_T666_1, /* {R[5:0], G[2:0]} {G[5:3], B[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B9_T666_2, /* {G[2:0], R[5:0]} {B[5:0], G[5:3]} */

    /* data format of "16 Bit" and "RGB444" */
    HAL_DISP_LCM_DATA_FMT_B16_T444_0, /* {4'b0, R[3:0], G[3:0], B[3:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T444_1, /* {4'b0, B[3:0], G[3:0], R[3:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T444_2, /* {R[3:0], G[3:0], B[3:0], 4'b0} */
    HAL_DISP_LCM_DATA_FMT_B16_T444_3, /* {B[3:0], G[3:0], R[3:0], 4'b0} */

    /* data format of "16 Bit" and "RGB565" */
    HAL_DISP_LCM_DATA_FMT_B16_T565_0, /* {B[4:0], G[5:0], R[4:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T565_1, /* {R[4:0], G[5:0], B[4:0]} */

    /* data format of "16 Bit" and "RGB666" */
    HAL_DISP_LCM_DATA_FMT_B16_T666_0, /* {14'b0, R[5:4)} {R[3:0], G[5:0], B[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_1, /* {R[5:0], G[5:0], B[5:2]} {B[1:0], 14'b0} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_2, /* {R[5:0], 2'b0, G[5:0], 2'b0} {8'b0, B[5:0], 2'b0} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_3, /* {R[5:0], 2'b0, G[5:0], 2'b0} { B[5:0], 10'b0} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_4, /* {4'b0, G1[5:0], R1[5:0]} {4'b0, R2[5:0], B1[5:0]} {4'b0, B2[5:0], G2[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_5, /* {4'b0, R1[5:0], G1[5:0]} {4'b0, B1[5:0], R2[5:0]} {4'b0, G2[5:0], B2[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_6, /* {G1[5:0], R1[5:0], 4'b0} {R2[5:0], B1[5:0], 4'b0} {B2[5:0], G2[5:0], 4'b0} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_7, /* {R1[5:0], G1[5:0], 4'b0} {B1[5:0], R2[5:0], 4'b0} {G2[5:0], B2[5:0], 4'b0} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_8, /* {2'b0, G1[5:0], 2'b0, R1[5:0]} {2'b0, R2[5:0], 2'b0, B1[5:0]} {2'b0, B2[5:0], 2'b0, G2[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_9, /* {2'b0, R1[5:0], 2'b0, G1[5:0]} {2'b0, B1[5:0], 2'b0, R2[5:0]} {2'b0, G2[5:0], 2'b0, B2[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_10, /* {G1[7:0], R1[7:0]} {R2[7:0], B1[7:0]} {B2[7:0], G2[7:0]} */
    HAL_DISP_LCM_DATA_FMT_B16_T666_11, /* {R1[7:0], G1[7:0]} {B1[7:0], R2[7:0]} {G2[7:0], B2[7:0]} */

    /* data format of "18 Bit" and "RGB666" */
    HAL_DISP_LCM_DATA_FMT_B18_T666_0, /* {B[5:0], G[5:0], R[5:0]} */
    HAL_DISP_LCM_DATA_FMT_B18_T666_1, /* {R[5:0], G[5:0], B[5:0]} */

    /* data format of "24 Bit" and "RGB666" */
    HAL_DISP_LCM_DATA_FMT_B24_T888_0, /* {B[7:0], G[5:0], R[7:0]} */   
    HAL_DISP_LCM_DATA_FMT_B24_T888_1, /* {R[7:0], G[7:0], B[7:0]} */   
} HAL_DispLcmDataFormat_e;

typedef enum {
	HAL_DISP_LCM_PIXEL_ALIGN_NONE = (0 << 0),
	HAL_DISP_LCM_PIXEL_ALIGN_BIG  = (1 << 0),
	HAL_DISP_LCM_PIXEL_ALIGN_MID  = (2 << 0),
	HAL_DISP_LCM_PIXEL_ALIGN_BOTH = (3 << 0),
} HAL_DispLcmPixelAlign_e;

typedef enum {
	HAL_DISP_LCM_COLOR_PACKAGE_RGB  = (1 << 0),
	HAL_DISP_LCM_COLOR_PACKAGE_BGR  = (0 << 0),
} HAL_DispLcmColorPackage_e;

typedef enum {
	HAL_DISP_WAIT_VSYNC   = (1 << 0),
	HAL_DISP_WAIT_VVALID  = (1 << 1),
	HAL_DISP_WAIT_HSYNC   = (1 << 2),
	HAL_DISP_WAIT_HVALID  = (1 << 3),
} HAL_DispWaitSource_e;

typedef enum {
	HAL_DISP_WAIT_FALLING = 0,
	HAL_DISP_WAIT_RISING  = 1,
	HAL_DISP_WAIT_HIGH    = 2,
	HAL_DISP_WAIT_LOW     = 3,
} HAL_DispWaitMode_e;

typedef struct {
    UINT8 busWidth;
    UINT8 dataType;
    UINT8 dataCyc;
    UINT8 pnlRBChg;
    UINT8 pnlGChg;
    UINT8 pktMode;
    UINT8 bigEnd;
    UINT8 midEnd;
} lcmSetItem_t; 

#define HAL_DISP_LCM_CMD_INST  0x01000000L
#define HAL_DISP_LCM_CMD_PARA  0x02000000L
#define HAL_DISP_LCM_CMD_WAIT  0x03000000L
#define HAL_DISP_LCM_CMD_STOP  0x04000000L

#define HAL_DISP_WAIT_TIMEOUT  0xFFFF

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
void HAL_DispFitMethodSet(UINT8 method);
UINT8 HAL_DispScaleSet(UINT16 hsize, UINT16 vsize, UINT16 zoomfac, 	UINT8 update);
UINT8 HAL_DispImgPanSet(SINT16 xoff, SINT16 yoff);
void HAL_Display_enable(void);
UINT8 HAL_DispScaleUpdate(void);

void HAL_DispRegUpdate(void);

void HAL_DispPbFrameBufferSet(UINT32 addr, UINT16 w, UINT16 h);
void HAL_DispPbFrameBufferGet(UINT32 *paddr, UINT16 *pw, UINT16 *ph);

void HAL_DispOsdBuffAddrSet(UINT32 addrWord);
 void HAL_DispOsdBuffSizeSet(UINT16 buffW, UINT16 buffH);
void HAL_DispOsdBuffAddrGet(UINT32 *pAddrWord);
void HAL_DispOsdBuffSizeGet(UINT16* pBuffW,UINT16* pBuffH);

UINT8 HAL_DispOsdScaleUpdate(void);

UINT8 HAL_DispParaSet(UINT8 id, SINT16 para1, SINT16 para2, SINT16 para3, SINT16 para4, UINT8 upd);
UINT8 HAL_DispOsdPaletteSet(UINT16 *paltTbl, UINT16 begIdx, UINT16 len);
UINT8 HAL_DispGammaTableSet(UINT8 *gammaTbl);
UINT8 HAL_DispOsdPaletteInit(UINT32 addr,UINT8 dataMode);

UINT8 HAL_DispSignalWait(UINT8 src, UINT8 mode);

UINT8 HAL_DispLcmCmdTableSend(UINT32 *cmdTbl);

void HAL_DispImgXyGet(UINT32 *x0, UINT32 *y0, UINT32 *x1, UINT32 *y1);
void HAL_DispImgXySet(UINT32 x0, UINT32 y0, UINT32 x1, UINT32 y1);
void HAL_DispImgHvSet(UINT32 h0,UINT32 v0,UINT32 h1,UINT32 v1);
void HAL_DispImgHvGet(UINT32 *h0,UINT32 *v0,UINT32 *h1,UINT32 *v1);
void HAL_DispOsdHvSet(UINT32 h0,UINT32 v0,UINT32 h1,UINT32 v1);
void HAL_DispOsdHvGet(UINT32 *h0,UINT32 *v0,UINT32 *h1,UINT32 *v1);
void HAL_DispPnlInitHvGet(UINT16 *h0,UINT16 *h1,UINT16 *v0,UINT16 *v1);
void HAL_DispPnlInitHvSet(UINT16 h0,UINT16 h1,UINT16 v0,UINT16 v1);
void HAL_DispImgXyUpdate(UINT16 newImgW, UINT16 newImgH);
void HAL_DispImgSizeGet(UINT16 *width,UINT16 *heigth);

#endif  /* __HAL_DISP_H__ */

