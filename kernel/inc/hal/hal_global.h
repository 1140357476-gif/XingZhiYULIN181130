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
 * @file      hal_global.h
 * @brief     Brief
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __HAL_GLOBAL_H__
#define __HAL_GLOBAL_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
  UINT8 Year;
  UINT8 Month;
  UINT8 Day;
  UINT8 Hour;
  UINT8 Minute;
  UINT8 Second;
} dateStc_t, *pDateStc_t;

/*add by Phil @08.01.02 s*/
typedef enum {
	CAM_MODE_IDLE  = 0x00,
	CAM_MODE_PV    = 0x01,
	CAM_MODE_STILL = 0x02,
	CAM_MODE_VIDEO = 0x03,
	CAM_MODE_PCCAM = 0x04,
	CAM_MODE_PB    = 0x05,
	CAM_MODE_UPDN  = 0x06,
} sp1kCamMode_t;
/*add by Phil @08.01.02 e*/

typedef void (code* pFuncGT2)(UINT8*);
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern pFuncGT2 code GTmr2FuncCallList[];
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void HAL_GlobalTimerInit(void);
void HAL_GlobalTimerRead10us(UINT32 xdata* G10ustimerPt); 
void HAL_GlobalReadGTimer(UINT32* GtimerPt);
UINT8 HAL_GlobalTimerWait10us(UINT8 WaitTime);
UINT8 HAL_GlobalTimerWait(UINT32 WaitTime);
UINT8 HAL_GlobalDateToBin(pDateStc_t DatePt, pDateStc_t BinPt);
UINT8 HAL_GlobalWriteRTCData(UINT8 Addr, UINT8 Data);
UINT8 HAL_GlobalWriteRTCDataIntr(UINT8 Addr, UINT8 Data);
UINT8 HAL_GlobalWriteRTC(pDateStc_t DatePt);
UINT8 HAL_GlobalRWRTCDataStsGet(void);
UINT8 HAL_GlobalReadRTCData(UINT8 Addr, UINT8* DataPt);
UINT8 HAL_GlobalBinToDate(pDateStc_t BinPt, pDateStc_t DatePt);
UINT8 HAL_GlobalReadRTC(pDateStc_t DatePt);
UINT8 HAL_GlobalWriteAlarm(pDateStc_t DatePt);
UINT8 HAL_GlobalReadAlarm(pDateStc_t DatePt);
UINT8 Hal_GlobalAlarmRTCIntrSet(UINT8 enable);
void HAL_GlobalRTCSecondIntrSet(UINT8 enable);
UINT8 HAL_GlobalConfigPG(UINT8 mode, UINT8 polarity,UINT8 idlstate,UINT16 lowDuty,UINT16 highDuty,UINT8 state);
void HAL_GlobalStartPG(UINT8  mode);
void HAL_GlobalStopPG(UINT8  mode);
UINT8 HAL_GlobalReadRegister(UINT16 reg);
void HAL_GlobalWriteRegister(UINT16 reg, UINT8 value);
UINT8 HAL_GlobalCamModeSet(sp1kCamMode_t CamMode);
void HAL_GlobalCamModeGet(UINT8* CamModePt);
UINT16	HAL_GlobalReadIOTrap(void);
void HAL_GlobalRTCTransferText(pDateStc_t DateTime,UINT8* TimeString);
void HAL_GlobalSuspendResume(void);
void HAL_GlobalPwrCtrl(UINT8 cpuclk, UINT8 dramclk, UINT8 reg2013, UINT8 reg2014, UINT8 reg2015);
void HAL_GlobalDramClkSet(UINT8 DramClk);
void HAL_GlobalBaudRateSet(UINT32 BaudRate);
void HAL_GlobalTimer2Handle_intr(void);
UINT8 HAL_GlobalTimer2Config(UINT32 loadtime, UINT8 countmode, UINT8 timebase, pFuncGT2 pFunc, UINT8* pFuncPara);
void HAL_GlobalTimer2Start(void);
UINT8 HAL_GlobalTimer2Stop(void);
UINT8 HAL_GlobalTimer2IntEvtGet(void);
UINT8 HAL_GlobalWDTCfg(UINT32 initval,UINT8 countmode,UINT8 enable);
void HAL_GlobalWDTSet(UINT8 enable);
void HAL_GlobalWDTTimeLoad(UINT32 loadtime);
void HAL_GlobalPwrMosSet(UINT8 en);
void HAL_GlobalUartSet(UINT8 en);
void HAL_GlobalRTCDrivingSet(UINT8 val);
void HAL_GlobalAlarmRTCIntrSet_Intr();

#endif  /* __HAL_GLOBAL_H__ */
