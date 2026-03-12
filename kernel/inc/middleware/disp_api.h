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
 * @file		disp_api.h
 * @brief		display module header file.
 * @author		Ted Chiu, Matt Wang
 * @since		2008-03-14
 * @date		2009-02-20
 */
#ifndef __DISP_API_H__
#define __DISP_API_H__

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define DISP_DEVICE_TOT  (3+1)

typedef enum {
    DISP_PARA_SW_RST,
	DISP_PARA_CLK_ENABLE,
	DISP_PARA_CLK_CFG,
	DISP_PARA_CLK_PHASE_DLY_CFG,
	DISP_PARA_CLK_SIGNAL_POLAR_CFG,
	DISP_PARA_GLOB_ENABLE,
	DISP_PARA_IMG_LAYER_ENABLE,
	DISP_PARA_IMG_LAYER_FLIP,
	DISP_PARA_IMG_LAYER_MONOCHROME_ENABLE,
	DISP_PARA_OSD_LAYER_ENABLE,
	DISP_PARA_BG_COLOR,
	DISP_PARA_REQ_LENTH_SET,
	DISP_PARA_DEVICE_TYPE,
	DISP_PARA_DEVICE_TOTAL_REGION,
    DISP_PARA_DEVICE_ACTIVE_REGION,
    DISP_PARA_DEVICE_IMG_REGION_CROP,
    DISP_PARA_DEVICE_IMG_REGION_RENDER,
    DISP_PARA_DEVICE_OSD_REGION_CROP,
    DISP_PARA_DEVICE_OSD_REGION_RENDER,
	DISP_PARA_OSD_BLEND_FACTOR,
	DISP_PARA_INTERLACE_CFG,
	DISP_PARA_LCD_DITHER_ENABLE,
	DISP_PARA_UPS051_PIXEL_SHIFT_CFG,
	DISP_PARA_UPS051_LIGHTVIEW_ENABLE,
	DISP_PARA_UPS051_RGB888_ENABLE,
	DISP_PARA_UPS052_RGB_ONLY_ENABLE,
	DISP_PARA_UPS_HSD_DELAY_CFG,
	DISP_PARA_UPS_CLK_PHASE,
	DISP_PARA_UPS_LAYOUT_MODE,
	DISP_PARA_UPS_COLOR_FILTER_CFG,
	DISP_PARA_CCIR_OUTPUT_SEQUENCE_CFG,
	DISP_PARA_EDGE_TYPE_SET,
	DISP_PARA_EDGE_GAIN_SET,
	DISP_PARA_GAMMA_ENABLE,
	DISP_PARA_RB_SWITCH_SET,
	DISP_PARA_COLOR_MATRIX_TBL_CFG,
	DISP_PARA_COLOR_MATRIX_ENABLE,
	DISP_PARA_COLOR_MATRIX_FMT_CFG,
	DISP_PARA_TV_DAC_POWER_ENABLE,
	DISP_PARA_TV_SATURATION,
	DISP_PARA_TV_HUE,
	DISP_PARA_LCM_CMD_BUS_TYPE,
	DISP_PARA_LCM_CMD_BIT_WIDTH,
	DISP_PARA_LCM_CMD_PULSE_WIDTH,
	DISP_PARA_LCM_CMD_ACTIVE_LEVEL,
	DISP_PARA_LCM_DATA_FORMAT,
	DISP_PARA_LCM_RGB666_PACK_ENABLE,
	DISP_PARA_LCM_RGB666_FORMAT_10_ENABLE,
	DISP_PARA_LCM_PIXEL_ALIGN,
	DISP_PARA_LCM_COLOR_PACKAGE,
	DISP_PARA_LCM_SWITCH_G_ENABLE,
	DISP_PARA_LCM_CHANGE_RB_ENABLE,
	DISP_PARA_LCM_ONE_HALF_PIXEL_ENABLE,
	DISP_PARA_LCM_DITHER_ENABLE,
	DISP_PARA_MAX_ID,
} dispParaId_e;  /* plz keep sync to HAL_DispParaId_e. */

typedef enum {
    DISP_DEV_RST_LOCAL = (0x01 << 0),
    DISP_DEV_RST_PNL   = (0x01 << 1),
    DISP_DEV_RST_TV    = (0x01 << 2),
    DISP_DEV_RST_ALL   = (DISP_DEV_RST_LOCAL | DISP_DEV_RST_PNL | DISP_DEV_RST_TV),
} dispRstDev_e; /* plz keep sync to HAL_DispRstDev_e. */

typedef enum {
	DISP_CLK_SRC_SYSPLL522M = 0,
	DISP_CLK_SRC_XTAL27M    = 1,
} dispClkSrc_e;  /* plz keep sync to HAL_DispClkSrc_e. */

typedef enum {
	DISP_CLK_PHASE_DLY_0T = 0,
	DISP_CLK_PHASE_DLY_1T = 1,
	DISP_CLK_PHASE_DLY_2T = 2,
	DISP_CLK_PHASE_DLY_3T = 3,
	DISP_CLK_PHASE_DLY_4T = 4,
	DISP_CLK_PHASE_DLY_5T = 5,
	DISP_CLK_PHASE_DLY_6T = 6,
	DISP_CLK_PHASE_DLY_7T = 7,
} dispClkPhaseDly_e; /* plz keep sync to HAL_DispClkPhaseDly_e. */

typedef enum {
	DISP_SIGNAL_CLK_NORMAL     = (0 << 3),
	DISP_SIGNAL_CLK_INVERSE    = (1 << 3),

	DISP_SIGNAL_VSYNC_ACT_LOW  = (0 << 4),
	DISP_SIGNAL_VSYNC_ACT_HIGH = (1 << 4),

	DISP_SIGNAL_HSYNC_ACT_LOW  = (0 << 5),
	DISP_SIGNAL_HSYNC_ACT_HIGH = (1 << 5),
} dispSignalPolar_e; /* plz keep sync to HAL_DispSignalPolar_e. */

typedef enum {
	DISP_GLOB_BLEND_SRC_IMAGE = (0 << 4),
	DISP_GLOB_BLEND_SRC_BLACK = (1 << 4),
} dispGlobBlendSrc_e; /* plz keep sync to HAL_DispGlobBlendSrc_e. */

typedef enum {
	DISP_IMG_REQ_LENTH_16 = (0 << 4),
	DISP_IMG_REQ_LENTH_32 = (1 << 4),
	DISP_OSD_REQ_LENTH_16 = (0 << 5),
	DISP_OSD_REQ_LENTH_32 = (1 << 5),
} dispReqLenth_e; /* plz keep sync to HAL_DispReqLenth_e. */

typedef enum {
	DISP_DEVICE_TYPE_COMPOSITE_NTSC      = 0x00,
	DISP_DEVICE_TYPE_COMPOSITE_PAL       = 0x01,
    DISP_DEVICE_TYPE_CCIR656_NTSC        = 0x02,
    DISP_DEVICE_TYPE_CCIR656_PAL         = 0x03,
	DISP_DEVICE_TYPE_CCIR601_NTSC_8      = 0x04,
	DISP_DEVICE_TYPE_CCIR601_PAL_8       = 0x05,
	//DISP_DEVICE_TYPE_CCIR601_NTSC_16     = 0x06,
	//DISP_DEVICE_TYPE_CCIR601_PAL_16      = 0x07,
	DISP_DEVICE_TYPE_UPS051              = 0x08,
	DISP_DEVICE_TYPE_UPS052              = 0x09,
	DISP_DEVICE_TYPE_LCM                 = 0x0A,
	//DISP_DEVICE_TYPE_VGA_TFT           = 0x0C,
	//DISP_DEVICE_TYPE_USER_CCIR601_8    = 0x0E,
	//DISP_DEVICE_TYPE_USER_CCIR601_16   = 0x0F,
	DISP_DEVICE_TYPE_CCIR656_INTERLACE   = 0x10,
    DISP_DEVICE_TYPE_CCIR601_INTERLACE   = 0x13,
    DISP_DEVICE_TYPE_CCIR656_PROGRESSIVE = 0x18,
    DISP_DEVICE_TYPE_CCIR601_PROGRESSIVE = 0x1B,
} dispDeviceType_e;  /* plz keep sync to HAL_DispDeviceType_e. */

typedef enum {
	DISP_PROGRESSIVE = 0x00,
	DISP_INTERLACE   = 0x01,
} dispInterlaceCfg_e;  /* plz keep sync to HAL_DispInterlaceCfg_e. */

typedef enum {
	DISP_PIXEL_SHIFT_DISABLE = 0x00,
	DISP_PIXEL_SHIFT_ODD     = 0x10,
	DISP_PIXEL_SHIFT_EVEN    = 0x30,
} dispUps051PixelShiftCfg_e;  /* plz keep sync to HAL_DispUps051PixelShiftCfg_e. */

typedef enum {
	DISP_UPS052_HSD_DELAY_3T = (0 << 0),
	DISP_UPS052_HSD_NO_DELAY = (1 << 0),
	DISP_UPS052_HSD_DELAY_1T = (1 << 1),
	DISP_UPS052_HSD_DELAY_2T = (1 << 2),
} dispUps052HsdDelayCfg_e; /* plz keep sync to HAL_DispUps052HsdDelayCfg_e. */

typedef enum {
	DISP_UPS051_HSD_RGB = (0 << 0),
	DISP_UPS051_HSD_BRG = (1 << 0),
} dispUps051HsdDelayCfg_e; /* plz keep sync to HAL_DispUps051HsdDelayCfg_e. */

typedef enum {
	DISP_UPS_LAYOUT_MODE_DISABLE            = (0 << 4),
	DISP_UPS_LAYOUT_MODE_ODD_DELAY_1T       = (1 << 4),
	DISP_UPS_LAYOUT_MODE_EVEN_DELAY_1T      = (3 << 4),
} dispUpsLayoutMode_e;  /* plz keep sync to HAL_DispUpsLayoutMode_e. */

typedef enum {
 	DISP_UPS051_ODD_RGB = (0 << 4),
	DISP_UPS051_ODD_GBR = (1 << 4),
	DISP_UPS051_ODD_BRG = (2 << 4),
	DISP_UPS051_ODD_BGR = (3 << 4),
	DISP_UPS051_ODD_RBG = (4 << 4),
	DISP_UPS051_ODD_GRB = (5 << 4),

 	DISP_UPS052_ODD_RGB = (0 << 4),
	DISP_UPS052_ODD_GBR = (1 << 4),
	DISP_UPS052_ODD_BRG = (2 << 4),
	DISP_UPS052_ODD_RBG = (3 << 4),
	DISP_UPS052_ODD_BGR = (4 << 4),
	DISP_UPS052_ODD_GRB = (5 << 4),
} dispUpsOddColorFilter_e;  /* plz keep sync to HAL_DispUpsOddColorFilter_e. */

typedef enum {
 	DISP_UPS051_EVEN_BRG = (0 << 0),
 	DISP_UPS051_EVEN_RGB = (1 << 0),
	DISP_UPS051_EVEN_GBR = (2 << 0),
	DISP_UPS051_EVEN_GRB = (3 << 0),
	DISP_UPS051_EVEN_BGR = (4 << 0),
	DISP_UPS051_EVEN_RBG = (5 << 0),

 	DISP_UPS052_EVEN_RGB = (0 << 0),
	DISP_UPS052_EVEN_GBR = (1 << 0),
	DISP_UPS052_EVEN_BRG = (2 << 0),
	DISP_UPS052_EVEN_RBG = (3 << 0),
	DISP_UPS052_EVEN_BGR = (4 << 0),
	DISP_UPS052_EVEN_GRB = (5 << 0),
} dispUpsEvenColorFilter_e;  /* plz keep sync to HAL_DispUpsEvenColorFilter_e. */

typedef enum {
	DISP_COLOR_MATRIX_FMT_RGB   = (0 << 4),
	DISP_COLOR_MATRIX_FMT_YCBCR = (1 << 4),
	DISP_COLOR_MATRIX_FMT_YUV   = (2 << 4),
} DispColorMtxCvsFmt_e;

typedef enum {
	DISP_CCIR_YCRYCB = (0 << 0),
	DISP_CCIR_CRYCBY = (1 << 0),
	DISP_CCIR_YCBYCR = (2 << 0),
	DISP_CCIR_CBYCRY = (3 << 0),
} dispCcirOutputSeqCfg_e;  /* plz keep sync to HAL_DispCcirOutputSeqCfg_e. */

typedef enum {
	DISP_EDGE_TYPE_A = (0 << 0), //Yedge(n) = Yin(n)
	DISP_EDGE_TYPE_B = (1 << 0), //Yedge(n) = -0.5*Yin(n-1) + Yin(n) -0.5*Yin(n+1)
	DISP_EDGE_TYPE_C = (2 << 0), //Yedge(n) = -0.5*Yin(n-2) + Yin(n) -0.5*Yin(n+2)
} dispEdgeType_e;

typedef enum {
	DISP_LCM_CMD_BUS_I80 = (0 << 0),
	DISP_LCM_CMD_BUS_M68 = (1 << 0),
} dispLcmCommandBusType_e;  /* plz keep sync to HAL_DispLcmCommandBusType_e. */

typedef enum {
	DISP_LCM_CMD_8_BIT   = (0 << 4),
    DISP_LCM_CMD_9_BIT   = (1 << 4),
    DISP_LCM_CMD_16_BIT  = (2 << 4),
    DISP_LCM_CMD_18_BIT  = (3 << 4),
    DISP_LCM_CMD_24_BIT  = (4 << 4),
} dispLcmCommandBitWidth_e;  /* plz keep sync to HAL_DispLcmCommandBitWidth_e. */

typedef enum {
	DISP_LCM_DATA_TYPE_RGB565 = (0 << 4),
	DISP_LCM_DATA_TYPE_RGB666 = (1 << 4),
	DISP_LCM_DATA_TYPE_RGB444 = (2 << 4),
	DISP_LCM_DATA_TYPE_RGB888 = (3 << 4),
} dispLcmDataType_e;  /* plz keep sync to HAL_DispLcmDataType_e. */


typedef enum {
    /* data format of "8 Bit" and "RGB444" */
    DISP_LCM_DATA_FMT_B8_T444_0, /* {G1[3:0],R1[3:0]} {R2[3:0],B1[3:0]} {B2[3:0],G2[3:0]} */
    DISP_LCM_DATA_FMT_B8_T444_1, /* {R1[3:0],G1[3:0]} {B1[3:0],R2[3:0]} {G2[3:0],B2[3:0]} */

    /* data format of "8 Bit" and "RGB565" */
    DISP_LCM_DATA_FMT_B8_T565_0, /* {G[2:0],R[4:0]} {B[4:0],G[5:3]} */
    DISP_LCM_DATA_FMT_B8_T565_1, /* {R[4:0],G[5:3]} {G[2:0],B[4:0]} */
    DISP_LCM_DATA_FMT_B8_T565_2, /* {B[4:0],G[5:3]} {G[2:0],R[4:0]} */
    DISP_LCM_DATA_FMT_B8_T565_3, /* {R[4:0],G[2:0]} {G[5:3],B[4:0]} */
    DISP_LCM_DATA_FMT_B8_T565_4, /* {B[4:0],G[2:0]} {G[5:3],R[4:0]} */

    /* data format of "8 Bit" and "RGB666" */
    DISP_LCM_DATA_FMT_B8_T666_0, /* {2'b0, R[5:0]} {2'b0, G[5:0]} {2'b0, B[5:0]} */
    DISP_LCM_DATA_FMT_B8_T666_1, /* {R[7:2], 2'b0} {G[7:2], 2'b0} {B[7:2], 2'b0} */

    /* data format of "9 Bit" and "RGB666" */
    DISP_LCM_DATA_FMT_B9_T666_0, /* {R[5:0], G[5:3]} {G[2:0], B[5:0]} */
    DISP_LCM_DATA_FMT_B9_T666_1, /* {R[5:0], G[2:0]} {G[5:3], B[5:0]} */
    DISP_LCM_DATA_FMT_B9_T666_2, /* {G[2:0], R[5:0]} {B[5:0], G[5:3]} */

    /* data format of "16 Bit" and "RGB444" */
    DISP_LCM_DATA_FMT_B16_T444_0, /* {4'b0, R[3:0], G[3:0], B[3:0]} */
    DISP_LCM_DATA_FMT_B16_T444_1, /* {4'b0, B[3:0], G[3:0], R[3:0]} */
    DISP_LCM_DATA_FMT_B16_T444_2, /* {R[3:0], G[3:0], B[3:0], 4'b0} */
    DISP_LCM_DATA_FMT_B16_T444_3, /* {B[3:0], G[3:0], R[3:0], 4'b0} */

    /* data format of "16 Bit" and "RGB565" */
    DISP_LCM_DATA_FMT_B16_T565_0, /* {B[4:0], G[5:0], R[4:0]} */
    DISP_LCM_DATA_FMT_B16_T565_1, /* {R[4:0], G[5:0], B[4:0]} */

    /* data format of "16 Bit" and "RGB666" */
    DISP_LCM_DATA_FMT_B16_T666_0, /* {14'b0, R[5:4)} {R[3:0], G[5:0], B[5:0]} */
    DISP_LCM_DATA_FMT_B16_T666_1, /* {R[5:0], G[5:0], B[5:2]} {B[1:0], 14'b0} */
    DISP_LCM_DATA_FMT_B16_T666_2, /* {R[5:0], 2'b0, G[5:0], 2'b0} {8'b0, B[5:0], 2'b0} */
    DISP_LCM_DATA_FMT_B16_T666_3, /* {R[5:0], 2'b0, G[5:0], 2'b0} { B[5:0], 10'b0} */
    DISP_LCM_DATA_FMT_B16_T666_4, /* {4'b0, G1[5:0], R1[5:0]} {4'b0, R2[5:0], B1[5:0]} {4'b0, B2[5:0], G2[5:0]} */
    DISP_LCM_DATA_FMT_B16_T666_5, /* {4'b0, R1[5:0], G1[5:0]} {4'b0, B1[5:0], R2[5:0]} {4'b0, G2[5:0], B2[5:0]} */
    DISP_LCM_DATA_FMT_B16_T666_6, /* {G1[5:0], R1[5:0], 4'b0} {R2[5:0], B1[5:0], 4'b0} {B2[5:0], G2[5:0], 4'b0} */
    DISP_LCM_DATA_FMT_B16_T666_7, /* {R1[5:0], G1[5:0], 4'b0} {B1[5:0], R2[5:0], 4'b0} {G2[5:0], B2[5:0], 4'b0} */
    DISP_LCM_DATA_FMT_B16_T666_8, /* {2'b0, G1[5:0], 2'b0, R1[5:0]} {2'b0, R2[5:0], 2'b0, B1[5:0]} {2'b0, B2[5:0], 2'b0, G2[5:0]} */
    DISP_LCM_DATA_FMT_B16_T666_9, /* {2'b0, R1[5:0], 2'b0, G1[5:0]} {2'b0, B1[5:0], 2'b0, R2[5:0]} {2'b0, G2[5:0], 2'b0, B2[5:0]} */
    DISP_LCM_DATA_FMT_B16_T666_10, /* {G1[7:0], R1[7:0]} {R2[7:0], B1[7:0]} {B2[7:0], G2[7:0]} */
    DISP_LCM_DATA_FMT_B16_T666_11, /* {R1[7:0], G1[7:0]} {B1[7:0], R2[7:0]} {G2[7:0], B2[7:0]} */

    /* data format of "18 Bit" and "RGB666" */
    DISP_LCM_DATA_FMT_B18_T666_0, /* {B[5:0], G[5:0], R[5:0]} */
    DISP_LCM_DATA_FMT_B18_T666_1, /* {R[5:0], G[5:0], B[5:0]} */

    /* data format of "24 Bit" and "RGB666" */
    DISP_LCM_DATA_FMT_B24_T888_0, /* {B[7:0], G[5:0], R[7:0]} */
    DISP_LCM_DATA_FMT_B24_T888_1, /* {R[7:0], G[7:0], B[7:0]} */
} DispLcmDataFormat_e; /* plz keep sync to HAL_DispLcmDataType_e. */

typedef enum {
	DISP_LCM_PIXEL_ALIGN_NONE = (0 << 0),
	DISP_LCM_PIXEL_ALIGN_BIG  = (1 << 0),
	DISP_LCM_PIXEL_ALIGN_MID  = (2 << 0),
	DISP_LCM_PIXEL_ALIGN_BOTH = (3 << 0),
} dispLcmPixelAlign_e;  /* plz keep sync to HAL_DispLcmPixelAlign_e. */

typedef enum {
	DISP_LCM_COLOR_PACKAGE_RGB  = (1 << 0),
	DISP_LCM_COLOR_PACKAGE_BGR  = (0 << 0),
} dispLcmColorPackage_e;  /* plz keep sync to HAL_DispLcmColorPackage_e. */

typedef enum {
	DISP_WAIT_VSYNC   = (1 << 0),
	DISP_WAIT_VVALID  = (1 << 1),
	DISP_WAIT_HSYNC   = (1 << 2),
	DISP_WAIT_HVALID  = (1 << 3),
} dispWaitSource_e;  /* plz keep sync to HAL_DispWaitSource_e. */

typedef enum {
	DISP_WAIT_FALLING = 0,
	DISP_WAIT_RISING  = 1,
	DISP_WAIT_HIGH    = 2,
	DISP_WAIT_LOW     = 3,
} dispWaitMode_e;  /* plz keep sync to HAL_DispWaitMode_e. */


#define DISP_LCM_CMD_INST  0x01000000L
#define DISP_LCM_CMD_PARA  0x02000000L
#define DISP_LCM_CMD_WAIT  0x03000000L
#define DISP_LCM_CMD_STOP  0x04000000L

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct {
	UINT16 width;
	UINT16 height;
	UINT16 xoff;
	UINT16 yoff;
	UINT16 aspectRatioX;
	UINT16 aspectRatioY;
} dispCapabDesc_t;

typedef UINT8 *(code *pfOperatingDispDevNameGet_t)(void);
typedef void (code *pfOperatingDispCapabilityGet_t)(dispCapabDesc_t *pcapabDesc);
typedef void (code *pfOperatingDispPowerOn_t)(void);
typedef void (code *pfOperatingDispPowerOff_t)(void);

typedef struct {
	pfOperatingDispDevNameGet_t pfOpDevNameGet;
	pfOperatingDispCapabilityGet_t pfOpCapabilityGet;
	pfOperatingDispPowerOn_t pfOpPowerOn;
	pfOperatingDispPowerOff_t pfOpPowerOff;
} dispOperatingFunc_t;

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* disp_enable.c */
void dispEn(UINT8 en);
void dispImgWinEn(UINT8 en);

/* disp_lcm.c */
UINT8 dispLcmCmdTableSend(UINT32 *cmdTbl);

/* disp_win.c */
void dispPnlDimGet(UINT16 *dispH, UINT16 *dispW);
void dispPnlAspectRatioGet(UINT8 *ratX, UINT8 *ratY);

/* disp_misc.c */
void dispPbBufSet(UINT32 addr, UINT16 w, UINT16 h);

/* sp1k_disp_api.c */
void dispShow(UINT32 addr,UINT16 w,UINT16 h, UINT16 factor);
void dispFitShow(UINT16 w, UINT16 h, UINT8 update);

/* disp_logi.c */
UINT8 dispDevOpen(dispOperatingFunc_t *popFunc, UINT16 opFuncSize);
UINT8 dispDevActSet(UINT8 id);
UINT8 dispDevActGet(void);
UINT8 dispCapabilityGet(UINT8 id, dispCapabDesc_t *pcapabDesc);
UINT8 dispPowerOn(UINT8 id);
UINT8 dispPowerOff(UINT8 id);

/* disp_para.c */
UINT8 dispParaSet(UINT8 id, SINT16 para1, SINT16 para2, SINT16 para3, SINT16 para4, UINT8 upd);

/* disp_wait.c */
UINT8 dispSignalWait(UINT8 src, UINT8 mode);

#endif  /* __DISP_API_H__ */

