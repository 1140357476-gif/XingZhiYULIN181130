/**************************************************************************
 *         Copyright(c) 2010 by ICatch Technology Co., Ltd.                *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/

#ifndef __CDSP_TASK_H__
#define __CDSP_TASK_H__


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define CDSP_MAX_YUV_CORING ( 8 )
#define CDSP_MAX_YUV_H_SIZE ( 256 )

typedef struct _DynamicEnhanThr {
	UINT8 resourceId;
	UINT8 preResourceId;
	UINT16 middleThr;
	UINT16 weakThr;
	UINT8 currEnhanMode;
	UINT8 nextEnhanMode;
}DynamicEnhanThr;

typedef struct _SysDynamicEnhan {
	UINT8 pvDynamicEn;
	UINT8 stillDynamicEn;
	DynamicEnhanThr pvMode;
	DynamicEnhanThr stillMode;
	UINT16 pvModeOffset;
}SysDynamicEnhan;
	
typedef enum {
	CDSP_STAGE_SENSOR_TRIGGER = 0,
	CDSP_STAGE_1ST_LC,
	CDSP_STAGE_2ND_LC,
	CDSP_STAGE_QV,
	CDSP_STAGE_MAIN_1ST_PASS,
	CDSP_STAGE_MAIN_2ND_PASS,
	CDSP_STAGE_PV
} cdspStage_e;

typedef enum {
	CDSP_DYNAMIC_WEAK = 0,
	CDSP_DYNAMIC_MIDDLE,
	CDSP_DYNAMIC_STRONG,
	CDSP_DYNAMIC_MAX
}CdspDynamicMode_e;

typedef enum {
	CDSP_DYNAMIC_ENABLE = 0,
	CDSP_DYNAMIC_DISABLE
}CdspDynamicEn_e;



/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

// cdsp_task.c

void cdspInit(void);

void cdspPreProcessOff( UINT8 rawMode ,UINT8 preProcMode);
void cdspPreProcessReload( void );
void cdspPostProcessOff(void);
void cdspPostProcessReload(void);
void cdspPostProcessCtrl( UINT8 mode, UINT8 opt );

UINT8 cdspRawProcess( UINT32 rawDataAddr, UINT16 rawHSize, UINT16 rawVSize, UINT8 rawMode, UINT8 mode );
UINT8 cdspRawBadPixel( UINT32 rawDataAddr, UINT16 rawHSize, UINT8 rawMode );



void sensorTrigger( UINT32 rawBufAddr, UINT16 sensorHSize, UINT16 sensorVSize, UINT16 dzoomHSize, UINT16 dzoomVSize, UINT16 rawHSize, UINT16 rawVSize, UINT16 snapHSize, UINT8 soundOpt, UINT8 raw10BitOpt );


//void sensorBurstToPV( void );
void cdspDoBufInit( UINT32 rawBufAddr, UINT32 yuvBufAddr1, UINT32 yuvBufAddr2, UINT32 yuvBufAddr3, UINT32 yuvBufAddr4, UINT16 rawXSize, UINT16 rawYSize, UINT16 yuvXSize, UINT16 yuvLines, UINT8 abBufOpt, UINT8 raw10BitOpt );

void cdspSnapDramImgTypeSet(UINT8 imgMode);

UINT8 cdspPvParameterSet(void);

//UINT8 cdspYuv2Yuv(UINT32 srcAddr,UINT32 dstAddr,UINT16 hSize,UINT16 vSize,UINT16 doVSize);

void cdspContrastEffectSet( void );
void cdspBlackWhiteEffectSet( void );
void cdspColdEffectSet( void );
void cdspWarmEffectSet( void );
void cdspSunsetEffectSet( void );
void cdspSepiaEffectSet( void );
void cdspNegativeEffectSet( void );
void cdspBinarizeEffectSet( void );
void cdspRedEffectSet( void );
void cdspBlueEffectSet( void );
void cdspGreenEffectSet( void );
void cdspNormalEffectSet( void );
void cdspBlendEffectSet(UINT32 srcAddr,UINT32 dstAddr,UINT16 hSize,UINT16 vSize,UINT8 idx);

//for normal flow
void cdspPvEdgeFilterSet(UINT8 ftr0,UINT8 ftr1,UINT8 ftr2,UINT8 ftr3,UINT8 ftr4,UINT8 ftr5);
void cdspPvEdgeParaSet(UINT8 clamp,UINT8 div,UINT8 lowGain,UINT8 highGain,UINT8 lowThr,UINT8 highThr,UINT8 yhtbw);
void cdspPvEdgeFilterEnSet(UINT8 pvFilterEn);
void cdspPvIntLowThrSet(UINT8 lowThr);
void cdspPvMedianModeSet(UINT8 mode);

//for intr used
void cdspIntrPvEdgeParaSet(UINT8 clamp, UINT8 div, UINT8 lowGain, UINT8 highGain, UINT8 lowThr, UINT8 highThr, UINT8 yhtbw);
void cdspIntrPvEdgeFilterSet( UINT8 ftr0, UINT8 ftr1, UINT8 ftr2, UINT8 ftr3, UINT8 ftr4, UINT8 ftr5);
void cdspIntrPvEdgeFilterEnSet( UINT8 pvFilterEn);
void cdspIntrPvIntLowThrSet(UINT8 lowThr);
void cdspIntrPvMedianModeSet(UINT8 mode);

void cdspStillEdgeFilterSet(UINT8 ftr0,UINT8 ftr1,UINT8 ftr2,UINT8 ftr3,UINT8 ftr4,UINT8 ftr5);
void cdspStillEdgeParaSet(UINT8 clamp,UINT8 div,UINT8 lowGain,UINT8 highGain,UINT8 lowThr,UINT8 highThr,UINT8 yhtbw);
void cdspStillEdgeFilterEnSet(UINT8 stillFilterEn);
void cdspStillIntLowThrSet(UINT8 lowThr);

void cdspStillYCoringParaSet(UINT8 par[]);
void cdspStillDynamicEnhanFlow(void);
void cdspPvDynamicEnhanFlow(void);

void cdspIntrPvDynamicIqParamSet(void);
void INTR_PvDynamicEnhanCheck(void);

void cdspTempDenoiseEn(UINT8 en,UINT8 level);

void cdspPvDynamicTempDenoiseEn(UINT8 en,UINT8 level);

void INTR_PvDynamicTempoalDenoiseCheck(void);
void INTR_PvDynamicTempoalDenoisePclkSet(void);
void INTR_PvDynamicTempoalDenoiseRegSet(void);
void INTR_PvDynamicTempoalDenoiseGateSet(void);

void cdspSharpnessEnhanSet(UINT8 stillDynamicEn, UINT8 stillDynamicEnhanMode);
void cdspSaturationEnhanSet(UINT8 en,UINT8 uvsat,UINT8 uOffset,UINT8 vOffset);
void cdspContrastEnhanSet(UINT8 en,UINT8 yConFactor);
void cdspBrightnessEnhanSet(UINT8 en,SINT8 brightnessOffset);
void cdspHueEnhanSet(UINT8 en,UINT8 channel,UINT8 fileId);

void sensorPanoramaTrigger(UINT8 soundOpt);


//void cdspDoFinish( void );  // unused function
//void cdspQvDo( UINT8 rawMode, UINT32 rawBufAddr, UINT32 qvBufAddr, UINT16 rawHSize, UINT16 rawVSize, UINT16 qvHSize, UINT16 qvVSize, UINT16 hFactor, UINT16 vFactor);
//UINT8 cdspRawCB( UINT8 mode, UINT32 rawDataAddr, UINT16 rawHSize, UINT16 rawVSize, UINT8 rawMode );

#endif

