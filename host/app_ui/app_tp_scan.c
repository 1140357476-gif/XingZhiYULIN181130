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
 * @file      app_tp_scan.c
 * @brief     touch panel scan process related function
 * @author    Phil Lin
 * @since     2012-02-02
 * @date      2012-02-02
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_APP_TP_SCAN__
#include "general.h"
#include "hal_disp_tp.h"
#include "gui_api.h"
#include "gui_tp.h"
#include "gui_evt.h"
#include "sp1k_osd_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_rsvblk_api.h"
#include "app_tp_scan.h"
#include "dbg_def.h"
#include "host_func_opt.h"
#include "sp1k_gpio_api.h"
#include "initio.h"
#include "app_msg.h"
#include "app_ui_osd.h"
#if FUNC_HOST_TOUCH_PANEL
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TP_PEN_UP_DELAY  (20) //Timebase: 10ms

#define PIX_AXIS_FIX_CNT    10
#define PIX_JUMP_LEN_MIN_FACT    1
#define PIX_JUMP_LEN_MAX_FACT    1
#define PIX_X_JUMP_LEN_MIN	(PIX_JUMP_LEN_MIN_FACT*4)
#define PIX_Y_JUMP_LEN_MIN	(PIX_JUMP_LEN_MIN_FACT*3)
#define PIX_X_JUMP_LEN_MAX	(PIX_JUMP_LEN_MAX_FACT*4)
#define PIX_Y_JUMP_LEN_MAX	(PIX_JUMP_LEN_MAX_FACT*3)

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//#define TP_CAL_MODE_SET(en)  do{G_tpCalibratioFlag = (en);}while(0)
//#define IS_TP_CAL_MODE()  ((G_tpCalibratioFlag == TP_CALIBRATION_EN) ? 1 : 0)
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT16 G_tpCalibratioFlag; //"TP_CALIBRATION_DIS" by default
static UINT8 tpCalibrationPoitNum;

static UINT16 G_tpXaxisCoord=0xffff, G_tpYaxisCoord=0xffff;

static UINT16 G_tpOrigPoitXadval, G_tpOrigPoitYadval;
static FINT32 G_tpPerPoitXadval, G_tpPerPoitYadval;

static UINT8 tpPenUpDly;

static SINT16 xHis = -1, yHis = -1;
static UINT8 cntMin, cntMax;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT16 LCD_WIDTH;// ((UINT16)320)
extern UINT16 LCD_HEIGHT;// ((UINT16)240)
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 UINT8 tpGetCurrCalPoint(void)
 {
	return tpCalibrationPoitNum;
 }
void tpCalModeSet(UINT8 enable)
{
	G_tpCalibratioFlag = enable;
}
UINT8 tpCalModeGet(void)
{
	return G_tpCalibratioFlag;
}
/**
 * @fn        UINT16 tpXaxisCoordGet(void)
 * @brief     tpXaxisCoordGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Zhu Xiao Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
UINT16 tpXaxisCoordGet(void)
{
	UINT16 tpXaxisCoord;
	UINT16 tpPoitXadcval;

	tpPoitXadcval=HAL_TpXadcvalMsure();
	//printf("\nxad:%x ",tpPoitXadcval);
	tpXaxisCoord=(UINT16)(40+((FINT32)tpPoitXadcval-(FINT32)G_tpOrigPoitXadval)/G_tpPerPoitXadval);
	if(tpXaxisCoord & 0x8000)
	{
		tpXaxisCoord = 0xffff;
	}
	else if(tpXaxisCoord > LCD_WIDTH)
	{
		tpXaxisCoord = LCD_WIDTH;
	}
	return tpXaxisCoord;
}

/**
 * @fn        UINT16 tpYaxisCoordGet(void)
 * @brief     tpYaxisCoordGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Zhu Xiao Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
UINT16 tpYaxisCoordGet(void)
{
	UINT16 tpYaxisCoord;
	UINT16 tpPoitYadcval;

	tpPoitYadcval=HAL_TpYadcvalMsure();
	//printf("yad:%x\n",tpPoitYadcval);
	tpYaxisCoord=(UINT16)(30+((FINT32)G_tpOrigPoitYadval-(FINT32)tpPoitYadcval)/G_tpPerPoitYadval);
	if(tpYaxisCoord & 0x8000)
	{
		tpYaxisCoord = 0xffff;
	}
	else if(tpYaxisCoord > LCD_HEIGHT)
	{
		tpYaxisCoord = LCD_HEIGHT;
	}
	return tpYaxisCoord;
}


/**
 * @fn        void tpPixAxisGet(SINT16 *x, SINT16 *y)
 * @brief     tpPixAxisGet
 * @param     [in] x
 * @param     [in] y
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
tpPixAxisGet(
	SINT16 *x,
	SINT16 *y
)
{
	if (x) *x = G_tpXaxisCoord;
	if (y) *y = G_tpYaxisCoord;
}

/**
 * @fn        void tpPixAxisFix(SINT16 x, SINT16 y)
 * @brief     tpPixAxisFix
 * @param     [in] x
 * @param     [in] y
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-6
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void
tpPixAxisFix(
	SINT16 x,
	SINT16 y
)
{
	G_tpXaxisCoord = x;
	G_tpYaxisCoord = y;
}

/**
 * @fn        void tpCoordFtRst(void)
 * @brief     tpCoordFtRst
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-31
 * @todo      N/A
 * @bug       N/A
*/
void
tpCoordFtRst(
	void
)
{
	xHis = -1;
	yHis = -1;
	cntMin = 0;
	cntMax = 0;
}

/**
 * @fn        UINT8 tpCoordFtProc(void)
 * @brief     tpCoordFtProc
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
tpCoordFtProc(
	void
)
{
	UINT8 en = 1;
	SINT16 x, y;

	if (!en) return SUCCESS;

	tpPixAxisGet(&x, &y);
	if (x == -1 || y == -1) {
		tpCoordFtRst();
		return SUCCESS;
	}

	if (x < 0 || y < 0) {
		return FAIL;
	}

	if (xHis != -1 && \
		(x > xHis + PIX_X_JUMP_LEN_MAX || y > yHis + PIX_Y_JUMP_LEN_MAX) && \
		++cntMax < PIX_AXIS_FIX_CNT
	) {
		//printf("x:%i,y:%i -> x:%i,y:%i\n", x, y, xHis, yHis);
		tpPixAxisFix(xHis, yHis);
		return FAIL;
	} else {
		cntMax = 0;
		xHis = x;
		yHis = y;
	}

	return SUCCESS;

}

/**
 * @fn        void tpScan(void)
 * @brief     Zhu Xiao Lin
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
void tpScan(void)
{
	static UINT8 tpSkip = 0;
	//static UINT8 cnt = 0;
	UINT8 keyAdcChanel;
	UINT16 xPos,yPos;
	keyAdcChanel = sp1kADCSarGet();

	if (tpPenUpDly) --tpPenUpDly;

	/*cnt ++;
	if(cnt >= 200)
	{
		xPos= tpXaxisCoordGet();
		yPos= tpYaxisCoordGet();
		cnt = 0;
	}*/

	switch(tpCurrStsGet())
	{
		case TP_PENDOWN:
			if (tpScanProcGet() & TP_PROC_PD) {
				//INTR_putchar('+');
				//break;
			}
			if(tpCalModeGet()) {
				tpCalibrationDo();
				sp1kADCSarSel(keyAdcChanel);
				tpCurrStsSet(TP_WAITINTR);
			} else
			{
				if (guiObjOpModeGet(GUI_OP_MODE_PENDN_ONLY)) {
					tpCurrStsSet(TP_WAITINTR);
				}

				/*else
				{
					HAL_TpadcvalPrep();
					tpCurrStsSet(TP_PENDOWN);
				}*/
				if (!tpPenUpDly) {
					xPos= tpXaxisCoordGet();
					yPos= tpYaxisCoordGet();
					sp1kADCSarSel(keyAdcChanel);
					if((xPos != -1)&&(yPos !=-1))
					{
						G_tpXaxisCoord=xPos;
						G_tpYaxisCoord=yPos;
					guiObjSlideHdl(G_tpXaxisCoord,G_tpYaxisCoord);
					tpScanProcAdd(TP_PROC_PD);
					}
					else
					{
						tpCurrStsSet(TP_PENUP);
					}
					tpSkip = 0;
				} else {
					tpSkip = 1;
					//printf("dly:%bu\n", tpPenUpDly);
				}
			}
			HAL_TpPenupChek();
			break;

		case TP_PENUP:
			if (tpScanProcGet() & TP_PROC_PU) {
				//INTR_putchar('-');
				//break;
			}
			HAL_TpReCfg();
			tpCurrStsSet(TP_WAITINTR);
			if (!tpPenUpDly) {
				if (!tpSkip) {
					guiObjFisrtPdStsClr();
					tpScanProcAdd(TP_PROC_PU);
				}
			}
			if(!tpCalModeGet()){
				G_tpXaxisCoord = -1;
				G_tpYaxisCoord = -1;
				if (!tpPenUpDly) {
					tpPenUpDly = TP_PEN_UP_DELAY;
				}
			}
			//printf("adpu\n");
			break;

		case TP_WAITINTR:
			break;

		default:
			break;
	}

}

/**
 * @fn        void tpCalibrationEnter(void)
 * @brief     tpCalibrationEnter
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Zhu Xiao Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
void tpCalibrationEnter(void)
{

	sp1kOsdStrDrawExt(60,120,"touch panel correct", 0x88);
	appCrossLineDraw(40-2,30-2,OSD_ATTR_COLOR_YELLOW,12);
	tpCalModeSet(TP_CALIBRATION_EN);
	tpCalibrationPoitNum=0;
}

/**
 * @fn        void tpCalibrationDo(void)
 * @brief     tpCalibrationDo
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Zhu Xiao Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
void tpCalibrationDo(void)
{
	static UINT16 tpPoit2Xadval,tpPoit2Yadval;
	static UINT16 tpPoit3Xadval,tpPoit3Yadval;
	static UINT16 tpPoit4Xadval,tpPoit4Yadval;

	tpCalibrationPoitNum++;
	if(tpCalibrationPoitNum==TP_CALIBRATION_POIT1)
	{
		G_tpOrigPoitXadval=HAL_TpXadcvalMsure();
		G_tpOrigPoitYadval=HAL_TpYadcvalMsure();
		printf("xo:%x ",G_tpOrigPoitXadval);
		printf("yo:%x\n",G_tpOrigPoitYadval);
		//sp1kOsdStrDrawExt(280,30,".",0x88);
	}
	else if(tpCalibrationPoitNum==TP_CALIBRATION_POIT2)
	{
		tpPoit2Xadval=HAL_TpXadcvalMsure();
		tpPoit2Yadval=HAL_TpYadcvalMsure();

		G_tpPerPoitXadval=((FINT32)tpPoit2Xadval-(FINT32)G_tpOrigPoitXadval)/240;
		printf("xper:%f\n",G_tpPerPoitXadval);
		//sp1kOsdStrDrawExt(280,190,".",0x88);
	}
	else if(tpCalibrationPoitNum==TP_CALIBRATION_POIT3)
	{
		tpPoit3Xadval=HAL_TpXadcvalMsure();
		tpPoit3Yadval=HAL_TpYadcvalMsure();
		G_tpPerPoitYadval=((FINT32)tpPoit2Yadval-(FINT32)tpPoit3Yadval)/160;
		printf("yper:%f\n",G_tpPerPoitYadval);
		//sp1kOsdStrDrawExt(40,190,".",0x88);
	}
	else if(tpCalibrationPoitNum==TP_CALIBRATION_POIT4)
	{
		tpPoit4Xadval=HAL_TpXadcvalMsure();
		tpPoit4Yadval=HAL_TpYadcvalMsure();
		G_tpPerPoitXadval=(G_tpPerPoitXadval+((FINT32)tpPoit3Xadval-(FINT32)tpPoit4Xadval)/240)/2;
		G_tpPerPoitYadval=(G_tpPerPoitYadval+((FINT32)G_tpOrigPoitYadval-(FINT32)tpPoit4Yadval)/160)/2;
		printf("xper:%f ",G_tpPerPoitXadval);
		printf("yper:%f\n",G_tpPerPoitYadval);
		tpCalModeSet(TP_CALIBRATION_DIS);
		tpCalibrationPoitNum=0;
	}
}

/**
 * @fn        UINT8 tpCalDatSave(UINT16 resIdx, UINT32 freeAddr)
 * @brief     tpCalDatSave
 * @param     [in] resIdx
 * @param     [in] freeAddr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
UINT8
tpCalDatSave(
	UINT16 resIdx,
	UINT32 freeAddr
)
{
	UINT32 sum = G_tpOrigPoitXadval + G_tpOrigPoitYadval + G_tpPerPoitXadval + G_tpPerPoitYadval;

	sp1kHalDRAMSetStartAddr(freeAddr, 0);
	sp1kHalDRAMWriteWord(READ8(G_tpOrigPoitXadval, 1), READ8(G_tpOrigPoitXadval, 0));
	sp1kHalDRAMWriteWord(READ8(G_tpOrigPoitYadval, 1), READ8(G_tpOrigPoitYadval, 0));
	sp1kHalDRAMWriteWord(READ8(G_tpPerPoitXadval, 3), READ8(G_tpPerPoitXadval, 2));
	sp1kHalDRAMWriteWord(READ8(G_tpPerPoitXadval, 1), READ8(G_tpPerPoitXadval, 0));
	sp1kHalDRAMWriteWord(READ8(G_tpPerPoitYadval, 3), READ8(G_tpPerPoitYadval, 2));
	sp1kHalDRAMWriteWord(READ8(G_tpPerPoitYadval, 1), READ8(G_tpPerPoitYadval, 0));

	sp1kHalDRAMWriteWord(READ8(sum, 3), READ8(sum, 2));
	sp1kHalDRAMWriteWord(READ8(sum, 1), READ8(sum, 0));

	return sp1kNandRsvWrite(resIdx, freeAddr << 1);
}

/**
 * @fn        UINT8 tpCalDatLoad(UINT16 resIdx, UINT32 freeAddr)
 * @brief     tpCalDatLoad
 * @param     [in] resIdx
 * @param     [in] freeAddr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-2-8
 * @todo      N/A
 * @bug       N/A
*/
UINT8
tpCalDatLoad(
	UINT16 resIdx,
	UINT32 freeAddr
)
{
	UINT32 sum;

	if (sp1kNandRsvRead(resIdx, freeAddr << 1) != TRUE) {
		return FAIL;
	}

	sp1kHalDRAMSetStartAddr(freeAddr, 1);
	sp1kHalDRAMReadWord((UINT8*)&G_tpOrigPoitXadval + 1, (UINT8*)&G_tpOrigPoitXadval + 0);
	sp1kHalDRAMReadWord((UINT8*)&G_tpOrigPoitYadval + 1, (UINT8*)&G_tpOrigPoitYadval + 0);
	sp1kHalDRAMReadWord((UINT8*)&G_tpPerPoitXadval + 3, (UINT8*)&G_tpPerPoitXadval + 2);
	sp1kHalDRAMReadWord((UINT8*)&G_tpPerPoitXadval + 1, (UINT8*)&G_tpPerPoitXadval + 0);
	sp1kHalDRAMReadWord((UINT8*)&G_tpPerPoitYadval + 3, (UINT8*)&G_tpPerPoitYadval + 2);
	sp1kHalDRAMReadWord((UINT8*)&G_tpPerPoitYadval + 1, (UINT8*)&G_tpPerPoitYadval + 0);

	sp1kHalDRAMReadWord((UINT8*)&sum + 3, (UINT8*)&sum + 2);
	sp1kHalDRAMReadWord((UINT8*)&sum + 1, (UINT8*)&sum + 0);

	if (sum != G_tpOrigPoitXadval + G_tpOrigPoitYadval + G_tpPerPoitXadval + G_tpPerPoitYadval) {
		ASSERT(0, 1);
		//printf("tp cal error\n");
		return FAIL;
	} else {
		if (sum == 0) {
			ASSERT(0, 1);
			//printf("Pls CAL TP\n"); /* The touch panel need calibration before using */
		}
		//printf("sum:%lx\n", sum);
	}
	return SUCCESS;
}
#endif /* FUNC_HOST_TOUCH_PANEL */
