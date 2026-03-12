/*******************************************************************************
*
*       Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
*
*  This software is copyrighted by and is the property of Sunplus mMedia
*  Inc., Ltd. All rights are reserved by Sunplus mMedia Inc., Ltd.
*  This software may only be used in accordance with the corresponding
*  license agreement. Any unauthorized use, duplication, distribution,
*  or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of Sunplus mMedia Inc., Ltd.
*
*  Sunplus mMedia Inc., Ltd. reserves the right to modify this software
*  without notice.
*
*  Sunplus mMedia Inc., Ltd.
*  19-1, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan, R.O.C.
*
*  File Description:
*
*  Author: Matt Wang
*
*******************************************************************************/

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "general.h"
#include "disp_api.h"
#include "gpio_api.h"
#include "reg_front.h"
#include "hal_global.h"
#include "i2c.h"
#include "sp1k_hal_api.h"
#include "sp1k_ae_api.h"

#include "ANX7150_Sys7150.h"
#include "ANX7150_System_Config.h"
#include "solution.h"

#include "host_func_opt.h"

#if FUNC_HOST_HDMI

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/* I2C define*/
#define ANX7150_PORT0_ADDR	0x72  /*ANX7150 */
#define ANX7150_PORT1_ADDR	0x7A  /*ANX7150 */

/* render dimension. */
#define DEV_TYPE       DISP_DEVICE_TYPE_UPS051
#define WIDTH          1280
#define HEIGHT         720
#define HSYNCW         40
#define VSYNCW         5
#define HPIXEL_50HZ    1980
#define VLINE_50HZ     750
#define HPIXEL_60HZ    1650
#define VLINE_60HZ     750
#define PNLH0          260
#define PNLV0          25
#define PNLH1          (PNLH0 + WIDTH - 1)
#define PNLV1          (PNLV0 + HEIGHT - 1)

/* pv size */
#define PV_SIZE_X 320
#define PV_SIZE_Y 240

/* image region */
#define PVX0      0
#define PVY0      0
#define PVX1      (PV_SIZE_X - 1)
#define PVY1      (PV_SIZE_Y - 1)
#define PVH0      PNLH0
#define PVV0      PNLV0
#define PVH1      PNLH1
#define PVV1      PNLV1

/* osd region */
#define OSDX0     0 
#define OSDY0     0 
#define OSDX1     (PV_SIZE_X - 1) 
#define OSDY1     (PV_SIZE_Y - 1) 
#define OSDH0     PNLH0 +28
#define OSDV0     PNLV0 +16
#define OSDH1     PNLH1 -28*2
#define OSDV1     PNLV1 -16*2

/* clock config. */
#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX      7
#define CLK_DIV1X      1
#define UPS_CLK_PHASE  8

#define CCIR_SEQ     DISP_CCIR_CRYCBY
#define FILTER_EVEN  DISP_UPS051_EVEN_BRG
#define FILTER_ODD   DISP_UPS051_ODD_BRG

#define HDMI_I2C_CLK_DIV   1

#define GPIO_FUNC  0x00
#define GPIO_DIR   0x04
#define GPIO_GATE  0x00
#define GPIO_OUT   0x00

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/
/* function type definition. */
#define __STATIC  //static

/* operating function. */
#define dispOpDevNameGet     hdmiOpDevNameGet
#define dispOpCapabilityGet  hdmiOpCapabilityGet
#define dispOpPowerOn        hdmiOpPowerOn
#define dispOpPowerOff       hdmiOpPowerOff

typedef enum fieldRate_e {
	FIELD_RATE_SUPPORT_50HZ = (UINT8)(0x01 << 0),
	FIELD_RATE_SUPPORT_60HZ = (UINT8)(0x01 << 1),

	FIELD_RATE_SUPPORT_UNKNOWN = (UINT8)(0x00 << 0),
} fieldRate_t;

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/

/* operating function. */
__STATIC UINT8 *dispOpDevNameGet(void);
__STATIC void dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc);
__STATIC void dispOpPowerOn(void);
__STATIC void dispOpPowerOff(void);

void ANX7150_Open(void);

extern ANX7150_edid_result_4_system ANX7150_edid_result;
/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/
/* device name. */
static code UINT8 dispDevName[] = "HDMI - ANA ANX7150 1280720 @ 50HZ";

/* device capability. */
static code dispCapabDesc_t dispCapabDesc = {
	WIDTH,
	HEIGHT,
	PNLH0,
	PNLV0,
	16,
	9,
};

static UINT8 xdata i2cDevAddr;
static UINT8 xdata i2cClkDiv;

/**
 * @fn        void hdmiReset(void)
 * @brief     hdmiReset
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-1-6
 * @todo      N/A
 * @bug       N/A
*/
static void
hdmiReset(
	void
)
{
	/*
	HDMI & panle share the reset pin and HDMI is reseted in panel reset flow ,so remove HDMI reset code .
	the other way round,HDMI must be reseted at here.
	*/	

#if 1
#if (_HW_SOLUTION_ == _HW_RDK_A_)
	/* reset the HDMI chip */
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (10 & 0x07), 0x01 << (10 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (10 & 0x07), 0x00 << (10 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (10 & 0x07), 0x01 << (10 & 0x07));
	HAL_GlobalTimerWait(100);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (10 & 0x07), 0x01 << (10 & 0x07));
	HAL_GlobalTimerWait(50);
#elif (_HW_SOLUTION_ == _HW_EVB_)
	/* reset the HDMI chip */
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (11 & 0x07), 0x01 << (11 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (11 & 0x07), 0x00 << (11 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (11 & 0x07), 0x01 << (11 & 0x07));
	HAL_GlobalTimerWait(100);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (11 & 0x07), 0x01 << (11 & 0x07));
	HAL_GlobalTimerWait(50);
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
	/* reset the HDMI chip */
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x00 << (9 & 0x07));
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
	HAL_GlobalTimerWait(100);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x01 << (9 & 0x07));
	HAL_GlobalTimerWait(50);
#endif
#endif
	
}

/**
 * @fn        fieldRate_s hdmiFieldRateGet(void)
 * @brief     hdmiFieldRateGet
 * @param     NONE
 * @retval    return fieldRate_s
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-1-6
 * @todo      N/A
 * @bug       N/A
*/
static fieldRate_t
hdmiFieldRateGet(
	void
)
{
	UINT8 bSupport50Hz = 0;
	UINT8 bSupport60Hz = 0;
	fieldRate_t ret = FIELD_RATE_SUPPORT_UNKNOWN;
	
	/*
		After read the EDID from TV 
		We should set the field rate below
	*/
	
	/* customized process start */
	
	bSupport50Hz = ANX7150_edid_result.supported_720p_50Hz;
	bSupport60Hz = ANX7150_edid_result.supported_720p_60Hz;
	
	/* customized process end */
	
	/* return value process */
	if (bSupport50Hz) {
		ret |= FIELD_RATE_SUPPORT_50HZ;
		printf("fRate 50Hz\n");
	}

	if (bSupport60Hz) {
		ret |= FIELD_RATE_SUPPORT_60HZ;
		printf("fRate 60Hz\n");
	}

	return ret;
}

/**
 * @fn        void hdmiBusReq(void)
 * @brief     hdmiBusReq
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-9
 * @todo      N/A
 * @bug       N/A
*/
static void 
hdmiBusI2CReq(
	void
)
{
	sp1kAeSetModeEn(0);
	
	/* configure I2C GPIO */
	gpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, GPIO_FUNC);
	gpioByteDirSet(GPIO_BYTE_TG0, 0xFF, GPIO_DIR);
	gpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, GPIO_GATE);
	gpioByteOutSet(GPIO_BYTE_TG0, 0xFF, GPIO_OUT);

	i2cClkCfgGet(&i2cClkDiv);
	i2cDeviceAddrGet(&i2cDevAddr);
	
	i2cClkCfgSet(HDMI_I2C_CLK_DIV);	
}

/**
 * @fn        void hdmiBusFree(void)
 * @brief     hdmiBusFree
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-9
 * @todo      N/A
 * @bug       N/A
*/
static void 
hdmiBusI2CFree(
	void
)
{
	i2cClkCfgSet(i2cClkDiv);
	i2cDeviceAddrSet(i2cDevAddr);
	
	sp1kAeSetModeEn(1);
}

/**
 * @brief	open device (register operating function).
 * @param	None.
 * @retval	return = NULL or device id.
 * @see
 * @author	Matt Wang
 * @since	2008-07-30
 * @todo	N/A
 * @bug		N/A
*/
UINT8
hdmiOpen(
	void
)
{
	dispOperatingFunc_t dispOpFunc;


	dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;
	dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;
	dispOpFunc.pfOpPowerOn       = dispOpPowerOn;
	dispOpFunc.pfOpPowerOff      = dispOpPowerOff;

	return dispDevOpen(&dispOpFunc, sizeof(dispOperatingFunc_t));
}

/**
 * @brief	operating function to get device name (ascii string).
 * @param	None.
 * @retval	return = device name string.
 * @see
 * @author	Matt Wang
 * @since	2008-07-30
 * @todo	N/A
 * @bug		N/A
*/
__STATIC UINT8 *
hdmiOpDevNameGet(
	void
)
{
	return dispDevName;
}

/**
 * @brief	operating function to get device capability.
 * @param	*pcapabDesc = [out] capability describer.
 * @param   descSize = [in] describer structure size.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-07-30
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
hdmiOpCapabilityGet(
	dispCapabDesc_t *pcapabDesc
)
{
	*pcapabDesc = dispCapabDesc;
}

/**
 * @brief	operating function to power on device.
 * @param	None.
 * @retval	None.
 * @see
 * @author	Matt Wang
 * @since	2008-07-30
 * @todo	N/A
 * @bug		N/A
*/
__STATIC void
hdmiOpPowerOn(
	void
)
{	
	fieldRate_t fRate;
	
	dispParaSet(DISP_PARA_GLOB_ENABLE, 0, 0, 0, 0, 0);
    dispParaSet(DISP_PARA_SW_RST, DISP_DEV_RST_ALL, 0, 0, 0, 0);
	
	/* set clk and type. */
	dispParaSet(DISP_PARA_CLK_CFG, CLK_SRC, CLK_DIVNX, CLK_DIV1X, 0, 0);
	dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);

	/* set vh timing and format */
	dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL_50HZ, VLINE_50HZ, HSYNCW, VSYNCW, 0);
	dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);
    dispParaSet(DISP_PARA_DEVICE_IMG_REGION_CROP, PVX0, PVX1, PVY0, PVY1, 0);
    dispParaSet(DISP_PARA_DEVICE_IMG_REGION_RENDER, PVH0, PVH1, PVV0, PVV1, 0);
    dispParaSet(DISP_PARA_DEVICE_OSD_REGION_CROP, OSDX0, OSDX1, OSDY0, OSDY1, 0);
    dispParaSet(DISP_PARA_DEVICE_OSD_REGION_RENDER, OSDH0, OSDH1, OSDV0, OSDV1, 0);
	dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 0);

	/* set ups interface. */
	dispParaSet(DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_MODE_DISABLE, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS_HSD_DELAY_CFG, DISP_UPS051_HSD_RGB, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_UPS051_RGB888_ENABLE, 1, 0, 0, 0, 0); /* set RGB888 single clock rate mode */
	dispParaSet(DISP_PARA_UPS052_RGB_ONLY_ENABLE, 0, 0, 0, 0, 0);
	dispParaSet(DISP_PARA_RB_SWITCH_SET, 1, 0, 0, 0, 0); /* enable rb switch */
	dispParaSet(DISP_PARA_REQ_LENTH_SET, DISP_IMG_REQ_LENTH_32 | DISP_OSD_REQ_LENTH_32, 0, 0, 0, 0);
	dispParaSet(
		DISP_PARA_CLK_SIGNAL_POLAR_CFG, \
		DISP_SIGNAL_CLK_INVERSE | DISP_SIGNAL_VSYNC_ACT_HIGH | DISP_SIGNAL_HSYNC_ACT_HIGH, \
		0, 0, 0, 0
	);
	dispParaSet(DISP_PARA_EDGE_GAIN_SET, 0x80, 0, 0, 0, 0); //disable edge enhancement
	dispParaSet(DISP_PARA_COLOR_MATRIX_ENABLE, 0, 0, 0, 0, 0); //disable color matrix

	dispParaSet(DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0);

	/* configure I2C for HDMI */
	hdmiBusI2CReq();
	
	/* set I2S for HDMI audio (GPIO19 ~ 22)*/	
	sp1kHalAudI2SSet(1);

	//hdmiReset();
	HAL_AudSoundPlayCfg(44100,16,1); //add for HDMI beep voice
	ANX7150_Open();

	fRate = hdmiFieldRateGet();
	if (!(fRate & FIELD_RATE_SUPPORT_50HZ)) {
		if ((fRate & FIELD_RATE_SUPPORT_60HZ)) {
			dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL_60HZ, VLINE_60HZ, HSYNCW, VSYNCW, 0);
		} else {
			/* Read EDID Fail */
		}
	}

	/* free the I2C bus */
	hdmiBusI2CFree();
}

/**
 * @fn        void hdmiOpPowerOff(void)
 * @brief     operating function to power off device
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-9
 * @todo      N/A
 * @bug       N/A
*/
__STATIC void
hdmiOpPowerOff(
	void
)
{
	dispParaSet(DISP_PARA_RB_SWITCH_SET, 0, 0, 0, 0, 0); /* disable rb switch */
	dispParaSet(DISP_PARA_EDGE_GAIN_SET, 0x08, 0, 0, 0, 0); //enable edge enhancement, 0x08-> gain: 1.0
	dispParaSet(DISP_PARA_REQ_LENTH_SET, DISP_IMG_REQ_LENTH_16 | DISP_OSD_REQ_LENTH_16, 0, 0, 0, 0);
	dispParaSet(
		DISP_PARA_CLK_SIGNAL_POLAR_CFG, \
		DISP_SIGNAL_CLK_NORMAL | DISP_SIGNAL_VSYNC_ACT_LOW | DISP_SIGNAL_HSYNC_ACT_LOW, \
		0, 0, 0, 0
	);	
	dispParaSet(DISP_PARA_UPS051_RGB888_ENABLE, 0, 0, 0, 0, 0); /* Disable RGB888 single clock rate mode */
	sp1kHalAudI2SSet(0);
	
	hdmiReset();
}

void ANX7150_i2c_read_p0_reg(BYTE offset, BYTE *d)
{
	UINT8 regaddr[1];

	regaddr[0]= offset;

	//printf(" read_p0 off= %bx, data= %bx\n",offset,*d);
	i2cDeviceAddrSet(ANX7150_PORT0_ADDR);
	i2cRegRead(regaddr, d, 1, 1, 1, 1, 0);
}

void ANX7150_i2c_read_p1_reg(BYTE offset, BYTE *d)
{
	UINT8 regaddr[1];

	regaddr[0]= offset;

	//printf(" read_p1 off= %bx, data= %bx\n",offset,*d);
	i2cDeviceAddrSet(ANX7150_PORT1_ADDR);
	i2cRegRead(regaddr, d, 1, 1, 1, 1, 0);
}
void ANX7150_i2c_write_p0_reg(BYTE offset, BYTE d)
{
	UINT8 cmdTbl[2];

	cmdTbl[0] = offset;
	cmdTbl[1] = d;
	
	//printf(" write_p0 off= %bx, data= %bx\n",offset,d);
	i2cDeviceAddrSet(ANX7150_PORT0_ADDR);
	i2cCmdTableSend(cmdTbl, 1, 1, 0, 0, 0);

}

void ANX7150_i2c_write_p1_reg(BYTE offset, BYTE d)
{
	UINT8 cmdTbl[2];

	cmdTbl[0] = offset;
	cmdTbl[1] = d;

	//printf(" write_p1 off= %bx, data= %bx\n",offset,d);
	i2cDeviceAddrSet(ANX7150_PORT1_ADDR);
	i2cCmdTableSend(cmdTbl, 1, 1, 0, 0, 0);
}

void delay_ms(int n)
{
	HAL_GlobalTimerWait(n);
}

void ANX7150_Open(void)
{
	UINT32 i;
	UINT8 c = 0x00;

	ANX7150_API_DetectDevice();
	ANX7150_API_Initial();
	ANX7150_API_ShutDown(0);
	ANX7150_API_HDCP_ONorOFF(0);
	
	for(i=0; i<100; i++)
	{
		if(!ANX7150_shutdown)
		{
			ANX7150_API_Input_Changed();
			ANX7150_Task();
										/*config ANX7150 after system is stable*/
			if(!ANX7150_app_hold_system_config)
			{
				if( ANX7150_parse_edid_done == 1 && ANX7150_system_config_done == 0)
				{
					//printf("ANX7150_parse_edid_done = %lu \n", (UINT32)ANX7150_parse_edid_done);
					//printf("ANX7150_system_config_done = %lu \n", (UINT32)ANX7150_system_config_done);

					/*system should config all the parameters here*/
					//printf("******** system config here.");
				    /*  ANX7150_API_PRINT_EDID_Parsing_Result();*/
					ANX7150_API_System_Config();
					ANX7150_system_config_done = 1;
				}
			}
			else
			{
				ANX7150_system_config_done = 0;
			}
		}
		
		delay_ms(5);
	}
	
	ANX7150_i2c_read_p1_reg(0xc0, &c);
	ANX7150_i2c_write_p1_reg(0xc0,c|0x01);
	ANX7150_i2c_read_p1_reg(0xc0, &c);
	//printf("0x07, data = 0x%02bx\n",c );

}
#endif //FUNC_HOST_HDMI
