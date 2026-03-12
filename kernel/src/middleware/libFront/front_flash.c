/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
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
 * @file		front_flash.c
 * @brief		front strobe flash control.
 * @author		c.s. lin
 * @since		2008-11-04
 * @date		2008-11-05
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#define SAVE_PV_RAW 0
#define SAVE_PV_YUV 1
#define CALCULATE_YUV_LUMA 1

/*Trigger GPIO cfg here*/
#define GPIO4 0
#define GPIO5 0
#define GPIO6 1
#define GPIO14 0
 
#include "general.h"
#include "gpio.h"
#include "hal_front.h"
#include "front_logi.h"
#include "hal_front_common.h"

#include "reg_front.h"

#include "ae_api.h"
#include "pwrlog.h"

#if SAVE_PV_RAW
#include "cdsp_task.h"
#include "hal_dram.h"
#include "hal_cdsp.h"
#include "reg_cdsp.h"
#include "initio.h"
#include "hal_front_para.h"
#include "utility.h"
#include "sp1k_pv_api.h"
#endif

#if SAVE_PV_YUV
#include "initio.h"
#include "pv_task.h"
#include "hal_cdsp.h"
#include "hal_front_para.h"
#include "utility.h"
#include "sp1k_pv_api.h"
#endif

#if CALCULATE_YUV_LUMA
#include "hal_fd.h"
#include "reg_ii.h"
#include "hal_dram.h"
#endif

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/


/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/


/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#define PREFLASH_INDEX  31
#define FLASH_TABLE_MIN_INDEX 0
#define FLASH_TABLE_MAX_INDEX 52//49//52//77
#define MAIN_FLASH_AE_OFFSET 0

#define ABS(x,y)  (((x)>(y)) ? ((x)-(y)) : ((y)-(x)))

#if (GPIO6 == 1)
#define HAL_FlashTriggerPinPwmCfg(flashWidth)  {\	
	HAL_GlobalConfigPG(2,0,0,0x7ff,flashWidth,1) ;\
}

/*GPIO6 out 0*/
#define HAL_FlashTriggerPinGpioCfg() {\
	XBYTE[0x2030] &= ~0x40;\
	XBYTE[0x201b] &= ~0x04; \
	XBYTE[0x2034] |= 0x40;\
}

/*GPIO6 out 0*/
#define HAL_FlashTriggerStart(isPWM)   {\
	if(isPWM){\	
		XBYTE[0x20B0] = 0x01;\
	}\	
	else{\
		XBYTE[0x2030] |= 0x40;\
	}\	
}

/*GPIO6 out 0*/
#define HAL_FlashTriggerStop(isPWM)   {\
	if(isPWM){\
		XBYTE[0x20B0] = 0x02;\
	}\
	else{\
		XBYTE[0x2030] &= ~0x40;\
	}\	
}
#elif(GPIO4 == 1)
#define HAL_FlashTriggerPinPwmCfg(flashWidth)  {\	
	HAL_GlobalConfigPG(0,0,0,0x7ff,flashWidth,1) ;\
}

/*GPIO4 out 0*/
#define HAL_FlashTriggerPinGpioCfg() {\
	XBYTE[0x2030] &= ~0x10;\
	XBYTE[0x201b] &= ~0x01; \
	XBYTE[0x2034] |= 0x10;\
}

/*GPIO4 out 0*/
#define HAL_FlashTriggerStart(isPWM)   {\
	if(isPWM){\	
		XBYTE[0x2090] = 0x01;\
	}\	
	else{\
		XBYTE[0x2030] |= 0x10;\
	}\	
}

/*GPIO4 out 0*/
#define HAL_FlashTriggerStop(isPWM)   {\
	if(isPWM){\
		XBYTE[0x2090] = 0x02;\
	}\
	else{\
		XBYTE[0x2030] &= ~0x10;\
	}\	
}

#elif(GPIO5 == 1)
#define HAL_FlashTriggerPinPwmCfg(flashWidth)  {\	
	HAL_GlobalConfigPG(1,0,0,0x7ff,flashWidth,1) ;\
}

/*GPIO5 out 0*/
#define HAL_FlashTriggerPinGpioCfg() {\
	XBYTE[0x2030] &= ~0x20;\
	XBYTE[0x201b] &= ~0x02; \
	XBYTE[0x2034] |= 0x20;\
}

/*GPIO5 out 0*/
#define HAL_FlashTriggerStart(isPWM)   {\
	if(isPWM){\	
		XBYTE[0x2098] = 0x01;\
	}\	
	else{\
		XBYTE[0x2030] |= 0x20;\
	}\	
}

/*GPIO5 out 0*/
#define HAL_FlashTriggerStop(isPWM)   {\
	if(isPWM){\
		XBYTE[0x2098] = 0x02;\
	}\
	else{\
		XBYTE[0x2030] &= ~0x20;\
	}\	
}

#elif(GPIO14 == 1)

#define HAL_FlashTriggerPinPwmCfg(flashWidth)  {\	
	HAL_GlobalConfigPG(3,0,0,0x7ff,flashWidth,1) ;\
}

/*GPIO14 out 0*/
#define HAL_FlashTriggerPinGpioCfg() {\
	XBYTE[0x2031] &= ~0x40;\
	XBYTE[0x201b] &= ~0x08; \
	XBYTE[0x2035] |= 0x40;\
}

/*GPIO14 out 0*/
#define HAL_FlashTriggerStart(isPWM)   {\
	if(isPWM){\	
		XBYTE[0x20B8] = 0x01;\
	}\	
	else{\
		XBYTE[0x2035] |= 0x40;\
	}\	
}

/*GPIO14 out 0*/
#define HAL_FlashTriggerStop(isPWM)   {\
	if(isPWM){\
		XBYTE[0x20B8] = 0x02;\
	}\
	else{\
		XBYTE[0x2035] &= ~0x40;\
	}\	
}
#endif


/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* preflash. */
static UINT16 preflash_width;   		/*how long the preflash should be*/
static UINT16 preflash_delay;		/*the H_POS of the preflash*/
static UINT8  preflash_delay_type; 	/*By Lines or time*/
static UINT8  preflash_state;
static UINT16 preflashPulse;
static UINT8 preflashGain;
static UINT32 preflashShutter;
static UINT8 preflashEvIdx;
static UINT8 preflashUsePwm;

/* red-eye flash. */
static UINT8 redeye_npulse; /*=0*/
static UINT8 redeye_duration;

/* snap flash. */
static UINT16 snap_flash_width;
static UINT8 snap_flash_use_pwm;

static UINT16 snap_flash_delay;
static UINT8 snap_flash_delay_type; /*=0*/
static UINT8 snap_flash_frontsync;
static UINT8 snap_flash_rearsync;
static UINT8 snap_flash_gain;
static UINT8 snap_flash_max_gain = SENSOR_GAIN_8X;

/* global variable as fake local variable. */
static UINT8 ret;

static UINT32 frontGShutterBk;
static UINT8 frontGGainBk;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
extern xdata UINT8 G_GAIN;
extern xdata UINT32 G_SHUTTER;
extern xdata UINT8 G_AEReady;
extern xdata UINT8 G_AESetting;
extern UINT8 aeGainExpSetEn;

extern void dbgWdtFeed( UINT32 food);

  typedef struct{
	UINT8 id;
	UINT32 flashTime;
	UINT8 EV;
 }flashEvTbl_t;

 static code flashEvTbl_t flashEvTbl[] = {
 #if 0
	 {0 ,		105  ,97   },
	 {1 ,		108  ,98   },			//-30
	 {2 ,		112  ,99   },
	 {3 ,		115  ,100	},  
	 {4 ,		118  ,101   },
	 {5 ,		121  ,102  },
	 {6 ,		124  ,103  },			//-25
	 {7 ,		128  ,104  },  
	 {8 ,		132  ,105  },
	 {9 ,		136  ,106  },
	 {10	,	140  ,107  },	 
	 {11	,	144  ,108  },			//-20
	 {12	,	148  ,109  },
	 {13	,	152  ,110  },	
	 {14	,	158  ,111  },
	 {15	,	163  ,112  },
	 {16	,	168  ,113  },			//-15
	 {17	,	174  ,114  },
	 {18	,	180  ,115  },
	 {19	,	187  ,116  },
	 {20	,	194  ,117  },
	 {21	,	201  ,118  },			//-10
	 {22	,	207  ,119  	},
	 {23	,	214  ,120  	},
	 {24	,	221  ,121  	},
	 {25	,	231  ,122  	},
	 {26	,	241  ,123  	},		//-5
	 {27	,	251  ,124  	},
	 {28	,	262  ,125  	},
	 {29	,	275  ,126  	},
	 {30	,	292  ,127  	},
	 {31	,	305  ,128  	},		//0
	 {32	,	320  ,130  	},		// 1
	 {33	,	335  ,130  	},		// 2
	 {34	,	348  ,131  	},		// 3
	 {35	,	370  ,132  	},		// 4
	 {36	,	394  ,133  	},		//5
	 {37	,	418  ,134  	},		//6
	 {38	,	452  ,135  	},		//7
	 {39	,	485  ,136  	},		//8
	 {40	,	520  ,137  	},		//9
	 {41	,	560  ,138  	},		//10
	 {42	,	620  ,139  	},		//11
	 {43	,	680  ,140  	},		//12
	 {44	,	750  ,141  	},		//13
	 {45	,	820  ,142  	},  		//14
	 {46,		1100  ,143  },		//15
	 {47,		1300  ,144  },
	 {48,		1600  ,145  },
	 {49,		10000  ,146  },  	//18
	 {50,		1400  ,147  },		//19
	 {51,		2000  ,148  },		//20
	 {52,		10000  ,149  },		//21
	 {53,		300  ,150  },  	//22
	 {54,		480  ,151  },
	 {55,		500  ,152  },
	 {56	,	520  ,153  },	 
	 {57	,	550  ,154  },
	 {58	,	580  ,155  },
	 {59	,	610  ,156  },
	 {60	,	650  ,157  },
	 {61	,	690  ,158  },
	 {62	,	730  ,159  },
	 {63	,	780  ,160  },
	 {64	,	850  ,161  },
	 {65	,	930  ,162  },
	 {66	,	1000 ,163  },
	 {67	,	1130 ,164  },
	 {68	,	1230 ,165  	},
	 {69	,	1300 ,166  	},
	 {70	,	1400 ,167  	},
	 {71	,	1500 ,168  	},
	 {72	,	1750 ,169  	},
	 {73	,	2050 ,170  	},   
	 {74	,	2400 ,171  	},
	 {75	,	3100 ,172  	},
	 {76	,	4200 ,173  	},
	 {77	,	10000,174  	}, 
	 #else
	  {0 ,		100  ,97   },
	 {1 ,		100  ,98   },			//-30
	 {2 ,		102  ,99   },
	 {3 ,		105  ,100	},  
	 {4 ,		108  ,101   },
	 {5 ,		111  ,102  },
	 {6 ,		114  ,103  },			//-25
	 {7 ,		117  ,104  },  
	 {8 ,		120  ,105  },
	 {9 ,		124  ,106  },
	 {10	,	128  ,107  },	 
	 {11	,	132  ,108  },			//-20
	 {12	,	136  ,109  },
	 {13	,	140  ,110  },	
	 {14	,	144  ,111  },
	 {15	,	148  ,112  },
	 {16	,	154  ,113  },			//-15
	 {17	,	160  ,114  },
	 {18	,	165  ,115  },
	 {19	,	170  ,116  },
	 {20	,	175  ,117  },
	 {21	,	184  ,118  },			//-10
	 {22	,	191  ,119  	},
	 {23	,	199  ,120  	},
	 {24	,	205  ,121  	},
	 {25	,	213  ,122  	},
	 {26	,	221  ,123  	},		//-5
	 {27	,	230  ,124  	},
	 {28	,	239  ,125  	},
	 {29	,	248  ,126  	},
	 {30	,	258  ,127  	},
	 {31	,	269  ,128  	},		//0
	 {32	,	284  ,130  	},		// 1
	 {33	,	300  ,130  	},		// 2
	 {34	,	315  ,131  	},		// 3
	 {35	,	330  ,132  	},		// 4
	 {36	,	353  ,133  	},		//5
	 {37	,	375  ,134  	},		//6
	 {38	,	395  ,135  	},		//7
	 {39	,	410  ,136  	},		//8
	 {40	,	445  ,137  	},		//9
	 {41	,	478  ,138  	},		//10
	 {42	,	515  ,139  	},		//11
	 {43	,	560  ,140  	},		//12
	 {44	,	600  ,141  	},		//13
	 {45	,	680  ,142  	},  		//14
	 {46,		770  ,143  },		//15
	 {47,		870  ,144  },
	 {48,		980  ,145  },
	 {49,		1150  ,146  },  	//18
	 {50,		1400  ,147  },		//19
	 {51,		2000  ,148  },		//20
	 {52,		10000  ,149  },		//21
	 {53,		300  ,150  },  	//22
	 {54,		480  ,151  },
	 {55,		500  ,152  },
	 {56	,	520  ,153  },	 
	 {57	,	550  ,154  },
	 {58	,	580  ,155  },
	 {59	,	610  ,156  },
	 {60	,	650  ,157  },
	 {61	,	690  ,158  },
	 {62	,	730  ,159  },
	 {63	,	780  ,160  },
	 {64	,	850  ,161  },
	 {65	,	930  ,162  },
	 {66	,	1000 ,163  },
	 {67	,	1130 ,164  },
	 {68	,	1230 ,165  	},
	 {69	,	1300 ,166  	},
	 {70	,	1400 ,167  	},
	 {71	,	1500 ,168  	},
	 {72	,	1750 ,169  	},
	 {73	,	2050 ,170  	},   
	 {74	,	2400 ,171  	},
	 {75	,	3100 ,172  	},
	 {76	,	4200 ,173  	},
	 {77	,	10000,174  	}, 
	 #endif
 } ;     

        
#define TRIGGER_DELAY_10US 2
void HAL_FlashTriggerDo(UINT8 waitTime,UINT8 isPwm)  
{
	UINT8 start,end;

	/*Get the start time*/
	start = XBYTE[0x20c1];
	if(start > (0xff - TRIGGER_DELAY_10US)){
		start = TRIGGER_DELAY_10US - (0xFF - start) - 1;
	}
	else{
		start = start + TRIGGER_DELAY_10US;
	}
	
	/*Get the end time*/
	if(start > (0xff - waitTime)){
		end = waitTime - (0xFF - start) - 1;
	}
	else{
		end = start + waitTime;
	}

	while(XBYTE[0x20c1] != start);
	/*trigger*/
	HAL_FlashTriggerStart(isPwm);
	//XBYTE[0x2031] |= 0x80;	/*old trigger start*/	

	while(XBYTE[0x20c1] != end);
	
	/*stop*/
	HAL_FlashTriggerStop(isPwm);
	//XBYTE[0x2031] &= ~0x80;	/*old trigger start*/	

}

#if CALCULATE_YUV_LUMA
 UINT8  frontPreflashImgLumaGet(
 	UINT32 yuvAddr ,		/*the YUV image addr*/
 	UINT32 iiTmpAddr,	/*tmp buffer for II*/
	 UINT16 hSize , 		/*the YUV H Size*/
	 UINT16 vSize,		/*the YUV V Size*/
	 UINT16 roiVOff , 		/*the V offset of the YUV img to do luma calculate*/
	 UINT16 roiVSize		/*the V size of the YUV img to do luma calculate*/
 )
 {
	UINT8 sts;
	UINT8 iiTagHsize = 8;
	UINT8 iiTagVsize = 8;
	UINT8 iiResHSize= 64;
	UINT8 iiResVSize = 64;
 	UINT8 buf[sizeof(fd_iijobtable_t)*3];
	fd_iijobtable_t xdata* pt = (fd_iijobtable_t xdata*)buf;
	
	UINT32 iiSrcAddr,iiOutAddr;
	UINT32 iiQueueAddr;

	UINT32 retLuma;
//	UINT8 lowByte,highByte;
	
	//sp1kCalibrationGammaSet(0);/*disable Gamma */
	vSize = vSize;	

	dbgWdtFeed(10000);	

	printf("waiting1..\n");
	/*while(!(XBYTE[REG_II_Status] & 0x02)) ;  	/*Waiting for II idle*/
	
	while(!(XBYTE[REG_II_Status] & 0x01)) ;  	/*Waiting for II idle*/
	
	ENTER_CRITICAL(sts);

	/*Crear II Queue*/
	iiQueueAddr = iiTmpAddr;
	iiSrcAddr = iiQueueAddr + ((UINT32)sizeof(fd_iijobtable_t)*3)/2;
	iiOutAddr = iiSrcAddr + (UINT32)iiResHSize*iiResVSize*2;

	printf("roiVOff=%d,hSize=%d,roiVSize=%d\n",roiVOff,hSize,roiVSize);
	
	HAL_FdIISIIQueue (				
		hSize, 
		0,  0/*roiVOff*/ , hSize, roiVSize, 
		iiResHSize,  iiResVSize, 
		yuvAddr,  iiSrcAddr,  0,//siisrcaddr,
		(UINT8*)pt++,
		0
	);

	/*addrs[0] = aeIIAddrOut;*/
	HAL_FdIIQueue (			
		iiSrcAddr,	//(w)
		iiResHSize,  iiResVSize, 
		&iiTagHsize,  &iiTagVsize, 
		&iiOutAddr, //(w)
		(UINT8*)pt++,
		0, //(w)
		1
	);	
	
	HAL_DramWriteBytesAddr(iiQueueAddr, buf, (UINT32)sizeof(fd_iijobtable_t)*3, 0);

	XBYTE[REG_II_IntrEnable] = 0x01;		/*enable the command queue intr*/
	XBYTE[REG_II_ModuleControl] = 0x00;	/*disable the software reset , enable clock gating*/
	
	//printf("QueueAddr = %lx\n",iiQueueAddr);

	/*II trigger*/
	while(!(XBYTE[REG_II_Status] & 0x01)) ;  	/*Waiting for II idle*/
	
	XBYTE[REG_II_CmdQueueAddr0] = ((unsigned char *)&(iiQueueAddr))[3];
	XBYTE[REG_II_CmdQueueAddr1] = ((unsigned char *)&(iiQueueAddr))[2];
	XBYTE[REG_II_CmdQueueAddr2] = ((unsigned char *)&(iiQueueAddr))[1];
	XBYTE[REG_II_CmdQueueAddr3] = ((unsigned char *)&(iiQueueAddr))[0];
	XBYTE[REG_II_CmdSrc] = 1; 	  	//Queue Trigger mode	
	XBYTE[REG_II_Intr] = 0x03;	        	//Clear done interrupt
	XBYTE[REG_II_CmdTrigger] = 0x01; 	//Trigger
	iiDbgTrig = II_Trigger_YUV;
	//printf("2\n");

	//while((XBYTE[REG_II_Intr] & 0x01)); /*Wating for the all the II commands in the command queue are processed intr */
	
	while(!(XBYTE[REG_II_Status] & 0x01)); /*Wating for II idle */

	XBYTE[REG_II_Intr] = 0x03;	        	//Clear done interrupt

	/*read the result after II*/
	
	/*********

	*****Some thing to be done*****

	***********/
	
	//printf("3\n");
	/*dramFileSave("IIII0000.BIN",iiOutAddr,(UINT32)iiTagHsize*iiTagVsize*4);*/

	iiOutAddr =  iiOutAddr + (UINT32)((UINT32)iiTagHsize*iiTagVsize-1)*2;
	HAL_DramStartAddrSet(iiOutAddr,1);

	HAL_DramWordDataRead(&(((unsigned char *)&(retLuma))[3]),&(((unsigned char *)&(retLuma))[2]));
	/*retLuma = lowByte;
	retLuma |= (UINT16)highByte<<8;
	/*printf("L = %bu , H = %bu\n",lowByte , HighByte);*/
	HAL_DramWordDataRead(&(((unsigned char *)&(retLuma))[1]),&(((unsigned char *)&(retLuma))[0]));
	/*retLuma |= (UINT32)lowByte<<16;
	retLuma |= (UINT32)highByte<<24;*/
	retLuma = retLuma/(iiTagHsize*iiTagVsize);
	printf("\033[32mLuma = %bu\033[0m\n" , (UINT8)retLuma);
	//printf("L = %bu , H = %bu\n",lowByte , highByte);

	EXIT_CRITICAL(sts);

	return (UINT8)retLuma;
 }
#endif

/**
 * @brief	configure the preflash type and duration
 * @param	cfgsel = [in] setting type for preflash.
 * @param	value = [in] setting value for each configuration setting type.
 * @retval	return = NULL
 * @see
 * @author	CS
 * @since	2008-11-04
 * @todo	N/A
 * @bug		N/A
*/
void
frontPreflashCfg(
	UINT8 cfgsel,
	UINT16 value
)
{
	UINT8 regClkDiv;
	switch (cfgsel) {
	default: break;
	case SENSOR_PREFLASH_CFG_WIDTH:
		regClkDiv = XBYTE[0x2021];	/*Get the PWM source clk div , clk = 522 /( DIV+1)*/		
		preflash_width = (value + 200)/100;
		
		if(value <= 300){	/* min is 0.015us , max is 31 us*/
			printf("Pre USE PWM 1\n");	
			preflashUsePwm = 1;
			preflashPulse = (UINT16)(((UINT32)value * 522) /(regClkDiv + 1)/10);	
		}
		else if(value <= 1200){		/*min is 0.0613us , the PWM max pluse width is 124us*/	
			printf("Pre USE PWM 2\n");	
			preflashUsePwm = 2;
			preflashPulse = (UINT16)(((UINT32)value * 522) /(31 + 1)/10);	
		}
		
		HAL_FlashTriggerPinPwmCfg(preflashPulse);
		printf("preflashPulse = %u\n",preflashPulse);
		preflashEvIdx = PREFLASH_INDEX;
		#if 0
		for(i = FLASH_TABLE_MIN_INDEX ; i <= FLASH_TABLE_MAX_INDEX ; i++){
			if(flashEvTbl[i].flashTime == value){
				preflashEvIdx = i;
				break;
			}
		}
		printf("\033[32mpreflashEvIdx = %bu\033[0m\n ",preflashEvIdx);
		#endif
		break;
	case SENSOR_PREFLASH_CFG_H_POS:
		preflash_delay = value + XBYTE[0x2A36]/*V-offset*/;
		printf("preflash_delay = %u\n",preflash_delay);
		break;
	case SENSOR_PREFLASH_CFG_DELAY_TYPE:
		preflash_delay_type = value;
		break;
	}
}

/******************************************
 * @fn        UINT8 frontIgbtFlashCalibTrig(UINT32 trigTime, trigTime,UINT8 saveYUV)
 * @brief        frontIgbtFlashCalibTrig
 * @param     [in] trigTime
 * @param     [in] saveYUV
 * @retval      return = SUCCESS / FAIL
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-09-20 , 10:45:23
 * @todo        N/A
 * @bug         N/A
*******************************************/
UINT8  frontIgbtFlashCalibTrig(UINT32 trigTime,UINT8 saveYUV)
{
	static UINT32 num = 0;
	UINT32 addr = K_SDRAM_CommonEndAddr;
	UINT32 addrPv1,addrPv2,addrPv3,addrPv4;
	UINT16 pvXSize, pvYSize;
	/*UINT8 f1[ 13 ] = "PV000001YUV";
	UINT8 f2[ 13 ] = "PV000002YUV";
	UINT8 f3[ 13 ] = "PV000003YUV";
	UINT8 f4[ 13 ] = "PV000004YUV";*/
	UINT8 f5[ 13 ] = "PV000005YUV";
	UINT8 idx;
	
	UINT8 sts , regClkDivBk;
	UINT8 retLuma;
	
	UINT16 flashPWMPulse , flashWidth;
	UINT16 roiVOffset,roiVSize;
	UINT8 flashUsePWM;

	printf("IGBT calibration\n");

	switch(preflash_delay_type) {
		case SENSOR_FLASH_DELAY_TYPE_H_POS:	
			ENTER_CRITICAL(sts);

			regClkDivBk = XBYTE[0x2021];  /*back up the reg clk div , for cap flash*/
			
			/*calculate the flash parameters*/
			if(trigTime <= 300){	/* min is 0.015us , max is 31 us*/
				printf("USE PWM 1\n");	
				flashUsePWM = 1;
				flashPWMPulse = (UINT16)(((UINT32)trigTime * 522) /(regClkDivBk + 1)/10); /*USE the max redClkDiv to calculate*/
				flashWidth = (trigTime + 200)/100;	 			/*delay for disable the PWM*/

				//HAL_GlobalConfigPG(1,0,0,0x7ff/*0x7ff = 2047 ,It's the max*/,flashPWMPulse,1);  /*Set the function of GPIO5 for PWM , for snap flash pulse control*/
				HAL_FlashTriggerPinPwmCfg(flashPWMPulse);
			}
			else if(trigTime <= 1200){		/*min is 0.0613us , the PWM max pluse width is 124us*/	
				printf("USE PWM 2\n");	
				flashUsePWM = 2;
				flashPWMPulse = (UINT16)(((UINT32)trigTime * 522) /(31 + 1)/10); /*USE the max redClkDiv to calculate*/
				flashWidth = (trigTime + 200)/100;	 			/*delay for disable the PWM*/

				XBYTE[0x2021] = 31;  /*Set to max reg clk div , for cap flash*/
				//HAL_GlobalConfigPG(1,0,0,0x7ff/*0x7ff = 2047 ,It's the max*/,flashPWMPulse,1);  /*Set the function of GPIO5 for PWM , for snap flash pulse control*/
				HAL_FlashTriggerPinPwmCfg(flashPWMPulse);
			}
			else{		
				printf("USE GPIO\n");	
				flashUsePWM = 0;
				flashWidth = trigTime/100;	
				
				/*cfg the trigger GPIO to be generate GPIO function and output 0*/
				#if 0
				XBYTE[0x2030] &= ~0x20;	/*output 0*/
				XBYTE[0x201b] &= ~0x02;	/*cfg for GPIO*/
				XBYTE[0x2034] |= 0x20;	/*Out put enable*/				
				#endif
				HAL_FlashTriggerPinGpioCfg();
			}
			printf("flashWidth =%u\n",flashWidth);
			
			/*the stop piont is not use the line interrupt*/
			/*XBYTE[0x2ACA] = (UINT8) ((preflash_delay + preflash_width) & 0xFF);
			XBYTE[0x2ACB] = (UINT8) (((preflash_delay + preflash_width) >> 8) & 0xFF);*/

			
			/**************************************
			****************capFlash****************
			***************************************/
			frontSignalWait(FRONT_WAIT_VVALID, FRONT_WAIT_RISING, 1);
						
			/*Set the flash start point*/
			XBYTE[0x2AC8] = (UINT8) (preflash_delay & 0xFF);
			XBYTE[0x2AC9] = (UINT8) ((preflash_delay >> 8) & 0xFF);
			
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			
			XBYTE[REG_Front_FintEvt_high] = 0x01;	/*clear the interrupt state*/
			XBYTE[REG_Front_Finten_high] = 0x01;	/*enable the number of ehdi_fs equal to Inthnum0*/

			/*printf("WaittingB...\n");                           /*The printf need more time */
			while(!(XBYTE[0x2AC1] & 0x01));

			HAL_FlashTriggerDo(flashWidth , flashUsePWM);

			XBYTE[REG_Front_Finten_high] = 0x00;	/*disable the number of ehdi_fs equal to Inthnum0*/
			XBYTE[REG_Front_FintEvt_high] = 0x01;	/*clear the interrupt state*/

		
			//sp1kST_TrigSet(1);
			//while(!(XBYTE[0x2AC1] & 0x02));
			//sp1kST_TrigSet(0);

			HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);/*gating*/
			HAL_CdspEofWait(1);
			
			frontSignalWait(FRONT_WAIT_VVALID, FRONT_WAIT_RISING, 2);
			//HAL_GlobalTimerWait(500);
			pvSizeGet( &pvXSize, &pvYSize );
			idx = HAL_CdspWriteBufIdxGet();
			
			if( idx == 0 ) {
				addrPv4 = K_SDRAM_PvBufAddrD;
				addrPv3 = K_SDRAM_PvBufAddrC;
				addrPv2 = K_SDRAM_PvBufAddrB;
				addrPv1 = K_SDRAM_PvBufAddrA;				
			}
			else if( idx == 1 ) {
				addrPv4 = K_SDRAM_PvBufAddrA;
				addrPv3= K_SDRAM_PvBufAddrD;
				addrPv2 = K_SDRAM_PvBufAddrC;
				addrPv1 = K_SDRAM_PvBufAddrB;
			}
			else if( idx == 2 ) {
				addrPv4 = K_SDRAM_PvBufAddrB;
				addrPv3 = K_SDRAM_PvBufAddrA;
				addrPv2 = K_SDRAM_PvBufAddrD;
				addrPv1 = K_SDRAM_PvBufAddrC;				
			}
			else if( idx == 3 ) {
				addrPv4 = K_SDRAM_PvBufAddrC;
				addrPv3 = K_SDRAM_PvBufAddrB;
				addrPv2 = K_SDRAM_PvBufAddrA;
				addrPv1 = K_SDRAM_PvBufAddrD;
			}

			num = trigTime;

			f5[0] = (UINT8)(num/10000000) + '0';
			f5[1] = (UINT8)((num/1000000)%10) + '0';
			f5[2] = (UINT8)((num/100000)%10) + '0';
			f5[3] = (UINT8)((num/10000)%10)  + '0';
			f5[4] = (UINT8)((num/1000)%10)  + '0';
			f5[5] = (UINT8)((num/100)%10) + '0';
			f5[6] = (UINT8)((num/10)%10)  + '0';
			f5[7] = (UINT8)(num%10)  + '0';


			/*dramFileSave(f1, addrPv1, (UINT32)pvXSize * pvYSize * 2 );
			dramFileSave(f2, addrPv2, (UINT32)pvXSize * pvYSize * 2 );
			dramFileSave(f3, addrPv3, (UINT32)pvXSize * pvYSize * 2 );*/
			if(saveYUV){
				dramFileSave(f5, addrPv4, (UINT32)pvXSize * pvYSize * 2 );
				num++;
			}
			
			roiVOffset = preflash_delay -  XBYTE[0x2A36] + 20;   /* delay 20 lines*/
			roiVSize= pvYSize - roiVOffset - 20 ;

			//*preLuma = frontPreflashImgLumaGet(addrPv3 , K_SDRAM_PvFdIIAddr/*addr*/ ,pvXSize ,pvYSize , roiVOffset, roiVSize);
			retLuma = frontPreflashImgLumaGet(addrPv4,K_SDRAM_PvFdIIAddr/*addr*/ ,pvXSize ,pvYSize , roiVOffset, roiVSize);
	
			XBYTE[0x2021] = regClkDivBk;  /*restore the reg clk div , for cap flash*/
			sp1kPreviewSet(0, K_Preview_Width, K_Preview_Height,100, 0);

			EXIT_CRITICAL(sts);
			break;
		case SENSOR_FLASH_DELAY_TYPE_IMMEDIATE_TIMER:
			ENTER_CRITICAL(sts);
			sp1kST_TrigSet(0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			HAL_Front10UsWait((UINT32)preflash_delay);
			sp1kST_TrigSet(1);
			HAL_Front10UsWait((UINT32)preflash_width);
			sp1kST_TrigSet(0);
			EXIT_CRITICAL(sts);
			break;
		default: break;
	}
	return retLuma;
}


/**
 * @brief	preflash triggered
 * @param
 * @retval	return = NULL
 * @see
 * @author	CS
 * @since	2008-11-04
 * @todo	N/A
 * @bug		N/A
*/

void
frontPreflashTrig(
/*void*/
	UINT8 *befPreLuma,
	UINT8 *aftPreLuma
)
{
	#if SAVE_PV_RAW
		UINT32 addr = K_SDRAM_CommonEndAddr;
		UINT16 prevW, prevH;
		UINT8 f[ 13 ] = "00000000RAW";
		UINT8 val;
	#endif

	#if SAVE_PV_YUV
		/*UINT8 f1[ 13 ] = "PV000001YUV";
		UINT8 f2[ 13 ] = "PV000002YUV";
		UINT8 f3[ 13 ] = "PV000003YUV";
		UINT8 f4[ 13 ] = "PV000004YUV";
		UINT8 f5[ 13 ] = "PV000005YUV";*/
	#endif

	UINT32 addr = K_SDRAM_CommonEndAddr;
	UINT32 addrPv1,addrPv2,addrPv3,addrPv4;
	UINT16 pvXSize, pvYSize;
	UINT16 roiVOffset,roiVSize;
	UINT8 idx;
	
	UINT8 sts,trigrUsePwm,regClkDivBk;

	trigrUsePwm = 1;
	printf("Trigger\n");

	if(preflash_width == 0 ||preflash_delay == 0){
		printf("Trig Er1!\n");
		return;
	}

	switch(preflash_delay_type) {
		case SENSOR_FLASH_DELAY_TYPE_H_POS:	
			ENTER_CRITICAL(sts);
			regClkDivBk = XBYTE[0x2021];  /*back up the reg clk div , for cap flash*/
			printf("\033[32mpreflash_width = %u\033[0m\n",preflash_width);
			
			XBYTE[0x2AC8] = (UINT8) (preflash_delay & 0xFF);
			XBYTE[0x2AC9] = (UINT8) ((preflash_delay >> 8) & 0xFF);
			
			/*the stop piont is not use the line interrupt*/
			/*XBYTE[0x2ACA] = (UINT8) ((preflash_delay + preflash_width) & 0xFF);
			XBYTE[0x2ACB] = (UINT8) (((preflash_delay + preflash_width) >> 8) & 0xFF);*/

			#if SAVE_PV_RAW
				frontPreviewSizeGet(0, &prevW, &prevH);

				XBYTE[REG_Cdsp_Cropen] = XBYTE[REG_Cdsp_Cropen] & (~(UINT8)0x20);
				XBYTE[REG_Cdsp_YUVscalesvden] = 0x00;
				XBYTE[REG_Cdsp_HRGBscaleen]=0;

				HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

				// skip AE/AWB data to dram
				HAL_CdspVDramSkipWrGet(&val);
				HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xff);
				HAL_CdspVDramSkipWrSyncSet(1);
				//HAL_CdspVDramSkipImgWrSet(1);
				//HAL_CdspVDramSkipAeWrSet(1);
				//HAL_CdspVDramSkipAwbWrSet(1);
				
				HAL_GlobalCamModeSet(2);

				HAL_DramRawFrmBufAddrSet(addr);
				HAL_DramRawFrmBufSizeSet(prevW/2, prevH);

				cdspSnapDramImgTypeSet(/*CDSP_SNAP_8RAW*/0);

				HAL_CdspRawImgSource(0);

				HAL_CdspVDramSizeClamp(1, prevW, 1, prevH, 1);

				HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_RISING, 1);
				HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_FALLING, 1);

				HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0);
				
				HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_FALLING, 1);
				HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_RISING, 1);
			#endif
			
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

			#if SAVE_PV_RAW
				HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);
			#endif

			if(preflashUsePwm == 2){
				XBYTE[0x2021] = 31;  /*Set to max reg clk div , for cap flash*/
			}
			
			XBYTE[REG_Front_FintEvt_high] = 0x01; 	/*clear the interrupt state*/
			XBYTE[REG_Front_Finten_high] = 0x01; 	/*enable the number of ehdi_fs equal to Inthnum0*/
			
			/*printf("Waitting...\n");*/   /*printf need more time*/
			while(!(XBYTE[0x2AC1] & 0x01));

			HAL_FlashTriggerDo(preflash_width , trigrUsePwm);
			XBYTE[0x2021] = regClkDivBk;		/*restore*/

			/*clean the interrupt flag*/
			XBYTE[REG_Front_Finten_high] = 0x00; 	/*disable the number of ehdi_fs equal to Inthnum0*/
			XBYTE[REG_Front_FintEvt_high] = 0x01; 	/*clear the interrupt state*/

			#if SAVE_PV_RAW
				HAL_CdspEofWait(1);
				HAL_CdspRawImgSource(1);
				dramFileSave(f, addr, (UINT32)prevW * prevH);
				sp1kPreviewSet(0, K_Preview_Width, K_Preview_Height,100, 0); 
				HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,val);
				HAL_CdspVDramSkipWrSyncSet(val>>4);
			#endif

			HAL_FrontParaSet(HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);/*gating*/
			HAL_CdspEofWait(1);
			printf("waitingPVYUV\n");
			frontSignalWait(FRONT_WAIT_VVALID, FRONT_WAIT_RISING, 2);
			/*HAL_GlobalTimerWait(500);*/
			pvSizeGet( &pvXSize, &pvYSize );
			idx = HAL_CdspWriteBufIdxGet();
			
			if( idx == 0 ) {
				addrPv4 = K_SDRAM_PvBufAddrD;
				addrPv3 = K_SDRAM_PvBufAddrC;
				addrPv2 = K_SDRAM_PvBufAddrB;
				addrPv1 = K_SDRAM_PvBufAddrA;				
			}
			else if( idx == 1 ) {
				addrPv4 = K_SDRAM_PvBufAddrA;
				addrPv3= K_SDRAM_PvBufAddrD;
				addrPv2 = K_SDRAM_PvBufAddrC;
				addrPv1 = K_SDRAM_PvBufAddrB;
			}
			else if( idx == 2 ) {
				addrPv4 = K_SDRAM_PvBufAddrB;
				addrPv3 = K_SDRAM_PvBufAddrA;
				addrPv2 = K_SDRAM_PvBufAddrD;
				addrPv1 = K_SDRAM_PvBufAddrC;				
			}
			else if( idx == 3 ) {
				addrPv4 = K_SDRAM_PvBufAddrC;
				addrPv3 = K_SDRAM_PvBufAddrB;
				addrPv2 = K_SDRAM_PvBufAddrA;
				addrPv1 = K_SDRAM_PvBufAddrD;
			}
			
			printf("S\n");
			
			#if SAVE_PV_YUV
			/*dramFileSave(f1, addrPv1, (UINT32)pvXSize * pvYSize * 2 );
			dramFileSave(f2, addrPv2, (UINT32)pvXSize * pvYSize * 2 );*/
			//dramFileSave(f3, addrPv3, (UINT32)pvXSize * pvYSize * 2 ); 
			//dramFileSave(f4, addrPv4, (UINT32)pvXSize * pvYSize * 2 );
			#endif	
			
			roiVOffset = preflash_delay -  XBYTE[0x2A36] + 20;
			roiVSize= pvYSize - roiVOffset - 20 ;
							
			*befPreLuma = frontPreflashImgLumaGet(addrPv3 , K_SDRAM_PvFdIIAddr/*addr*/ ,pvXSize ,pvYSize , roiVOffset, roiVSize);
			*aftPreLuma = frontPreflashImgLumaGet(addrPv4 ,K_SDRAM_PvFdIIAddr/*addr*/ ,pvXSize ,pvYSize , roiVOffset, roiVSize);
			/*dramFileSave(f5, addrPv4, (UINT32)pvXSize * pvYSize * 2 );	*/

			printf("\033[32mbLuma = %bu , aLuma = %bu\033[0m\n",*befPreLuma,*aftPreLuma);
				
			EXIT_CRITICAL(sts);
			break;
		case SENSOR_FLASH_DELAY_TYPE_IMMEDIATE_TIMER:
			ENTER_CRITICAL(sts);
			sp1kST_TrigSet(0);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			HAL_Front10UsWait((UINT32)preflash_delay);
			sp1kST_TrigSet(1);
			HAL_Front10UsWait((UINT32)preflash_width);
			sp1kST_TrigSet(0);
			EXIT_CRITICAL(sts);
			break;
		default: break;
	};
}

/**
 * @brief	strobe flash for red eye reduction
 * @param	npulse = [in] numbers of pulse to be triggered.
 * @param	duration = [in] each pulse duration
 * @retval	return = NULL
 * @see
 * @author	CS
 * @since	2008-11-04
 * @todo	N/A
 * @bug		N/A
*/
void
frontFlashRedEyeCfg(
	UINT8 npulse,
	UINT8 duration
)
{
	redeye_npulse = npulse;
	redeye_duration = duration;
}

/**
 * @brief	trigger red eye flash
 * @param	sync = [in] flash trigger sync to VD or not
 * @retval	return = NULL
 * @see
 * @author	CS
 * @since	2008-11-04
 * @todo	N/A
 * @bug		N/A
*/
void
frontFlashRedEyeTrig(
	UINT8 sync
)
{
	UINT8 sts;
	UINT16 delay;
	UINT8 redeye_loop;

	ENTER_CRITICAL(sts);

	if(sync == 1) frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);

	for(redeye_loop = redeye_npulse; redeye_loop > 0; redeye_loop--) {
		sp1kST_TrigSet(1);
		for(delay = 100; delay > 0 ;delay --);
		sp1kST_TrigSet(0);
		//HAL_Wait(redeye_duration);
		HAL_GlobalTimerWait10us(redeye_duration);
	};

	EXIT_CRITICAL(sts);
}

UINT8 frontSnapFlashCapGainGet(UINT8 flasEvIdx,UINT8 pv2CapGainRatio)
{
	flasEvIdx = flasEvIdx;
	pv2CapGainRatio = pv2CapGainRatio;
	return 0;
}

/******************************************
 * @fn        UINT32 frontSnapFlashTimeGet(UINT8 befPreFlashLuma, UINT8 preFlashLuma, UINT8 tagLuma, UINT8 pv2CapGainRatio)
 * @brief        frontSnapFlashTimeGet
 * @param     [in] befPreFlashLuma
 * @param     [in] preFlashLuma
 * @param     [in] tagLuma
 * @param     [in] pv2CapGainRatio
 * @retval      return = SUCCESS / FAIL
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-09-23 , 15:13:15
 * @todo        N/A
 * @bug         N/A
*******************************************/
UINT32 frontSnapFlashTimeGet(
	UINT8 befPreFlashLuma,
	UINT8 preFlashLuma,
	UINT8 tagLuma,
	UINT8 pv2CapGainRatio
	)
{

	SINT32 preViewLumaAdd,capLumaAdd;
	
	SINT32 log2X, log2Y, ret, round, p, d; 
	UINT16 pvToSnapFrmTimeRatio;
	
	SINT16 val;
	UINT8 sign;

	UINT8 befPreFlashLumaDiv;

	//printf("\033[32mpreflashGain = %bu\033[0m\n",preflashGain);
	preViewLumaAdd = (SINT32)preFlashLuma - (SINT32)befPreFlashLuma;
	//capLumaAdd = (SINT32)tagLuma - (SINT32)befPreFlashLuma;	
	
	pvToSnapFrmTimeRatio = sensorPvToSnapFrmTimeRatio();

	capLumaAdd = (SINT32)tagLuma - (SINT32)(((UINT32)befPreFlashLuma*pvToSnapFrmTimeRatio)>>8);	

	befPreFlashLumaDiv = 2;
	while(capLumaAdd < 20 && preflashGain != 0){
		UINT8 luma ;
		preflashGain --;
		luma = befPreFlashLuma/befPreFlashLumaDiv;
		
		preViewLumaAdd = preViewLumaAdd/befPreFlashLumaDiv;
		capLumaAdd = (SINT32)tagLuma - (SINT32)(((UINT32)luma*pvToSnapFrmTimeRatio)>>8);

		befPreFlashLumaDiv = befPreFlashLumaDiv+2;
	}

	if(preViewLumaAdd == 0){
		preViewLumaAdd = 1;
	}
	/*else if(preViewLumaAdd < 30){
		preViewLumaAdd = preViewLumaAdd+10;
	}*/
	
	printf("pre=%ld,tag=%ld\n",preViewLumaAdd,capLumaAdd);
	ret = log2xyND(preViewLumaAdd,capLumaAdd,&log2X,&log2Y);
	if (ret == LOG2_DOMAIN_ERR) {
		val = preflashEvIdx-preflashEvIdx;
		printf("\033[32m ERROR!\033[0m\n");
	}
	else {	/* ret == 0 */	
		p = log2X * 10;
		d = log2Y;
		sign = p >> 31;
		round = (sign ^ (SINT32)((UINT32)d / 2)) - sign;
		val = (SINT16)((p + round) / d);
		
		printf("\033[32mval_1 = %d \033[0m\n",val);
		val =  preflashEvIdx - val + MAIN_FLASH_AE_OFFSET;
		printf("\033[32mval_2 = %d \033[0m\n",val);

		if(preflashGain ==  SENSOR_GAIN_8X && (FLASH_TABLE_MAX_INDEX - val) >= 10){
			val = val + 10;
			preflashGain = SENSOR_GAIN_4X;
		}
		if(preflashGain == SENSOR_GAIN_4X&& (FLASH_TABLE_MAX_INDEX - val ) >= 10){
			val = val + 10;
			preflashGain = SENSOR_GAIN_2X;
		}
		if(preflashGain == SENSOR_GAIN_2X && (FLASH_TABLE_MAX_INDEX - val) >= 10){
			val = val + 10;
			preflashGain = SENSOR_GAIN_1X;
		}

		if(val < FLASH_TABLE_MIN_INDEX){
			while(preflashGain > SENSOR_GAIN_1X && val < FLASH_TABLE_MIN_INDEX){
				val = val +10;
				preflashGain --;
			}
		}
		else if(val > FLASH_TABLE_MAX_INDEX){
			while(preflashGain < snap_flash_max_gain && val > FLASH_TABLE_MAX_INDEX){
				val = val -10;
				preflashGain ++;
			}
		}	
		if(val < FLASH_TABLE_MIN_INDEX){
			val = FLASH_TABLE_MIN_INDEX;
		}
		else if(val > FLASH_TABLE_MAX_INDEX){
			val = FLASH_TABLE_MAX_INDEX;
		}
	}
	printf("\033[32mval_3 = %u \033[0m\n",(UINT16)val);

		/**********for binning mode**********/
	if(pv2CapGainRatio == 2){
		if(FLASH_TABLE_MAX_INDEX - val >= 10 ){
			val = val + 10;
			snap_flash_gain = preflashGain;
		}
		else{
			if(preflashGain == SENSOR_GAIN_1X){
				snap_flash_gain = SENSOR_GAIN_2X;
			}
			else if(preflashGain == SENSOR_GAIN_2X){
				snap_flash_gain = SENSOR_GAIN_4X;
			}
			else{
				val = FLASH_TABLE_MAX_INDEX;
				snap_flash_gain = SENSOR_GAIN_8X;
			}
		}
	}
	else if(pv2CapGainRatio == 4){
		if(FLASH_TABLE_MAX_INDEX - val >= 20 ){
			val = val + 20;
			snap_flash_gain = preflashGain;
		}
		else if(FLASH_TABLE_MAX_INDEX - val >= 10 ){
			val = val + 10;
			if(preflashGain == SENSOR_GAIN_1X){
				snap_flash_gain = SENSOR_GAIN_2X;
			}
			else if(preflashGain == SENSOR_GAIN_2X){
				snap_flash_gain = SENSOR_GAIN_4X;
			}
			else{
				snap_flash_gain = SENSOR_GAIN_8X;
			}
		}
		else{
			if(preflashGain == SENSOR_GAIN_1X){
				snap_flash_gain = SENSOR_GAIN_4X;
			}
			else if(preflashGain == SENSOR_GAIN_2X){
				snap_flash_gain = SENSOR_GAIN_8X;
			}
			else{
				val = FLASH_TABLE_MAX_INDEX;
				snap_flash_gain = SENSOR_GAIN_8X;
			}
		}
	}
	else{
	
		//printf("\033[32mpreflashGain = %bu\033[0m\n",preflashGain);
		
		snap_flash_gain = preflashGain;
	}
	if(snap_flash_gain > snap_flash_max_gain ){
		snap_flash_gain = snap_flash_max_gain;
		val = FLASH_TABLE_MAX_INDEX;
	}
	/*********for binning mode end*********/
	
	//printf("\033[32msnap_flash_gain = %bu\033[0m\n",snap_flash_gain);
	printf("\033[32mval_4 = %u \033[0m\n",(UINT16)val);
	
	printf("\033[32mmainTime = %lu \033[0m\n",(UINT32)flashEvTbl[val].flashTime);
	return flashEvTbl[val].flashTime;
	
}

/******************************************
 * @fn        void frontPreflashAeCfg(UINT32 shutter, UINT8 gain, UINT8 wait)
 * @brief        frontPreflashAeCfg
 * @param     [in] shutter
 * @param     [in] gain
 * @param     [in] wait
 * @retval      NONE
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-09-14 , 19:56:46
 * @todo        N/A
 * @bug         N/A
*******************************************/
void 
frontPreflashAeCfg(
	UINT32 shutter,
	UINT8 gainIndex,
	UINT8 wait
)
{	
	UINT8 preflashGainIdx = 0;
	UINT8 sensorGainIdx;
	if(gainIndex == SENSOR_GAIN_AUTO){
		preflashGainIdx = sensorFlashGainIdxGet();
		preflashGain = preflashGainIdx;
		sensorGainIdx = sensorGainIdxGet(preflashGain);
	}
	else{
		preflashGain = gainIndex;
		sensorGainIdx = sensorGainIdxGet(gainIndex);
	}
	preflashShutter = shutter;
	
	frontGShutterBk = G_SHUTTER;
	frontGGainBk = G_GAIN;
		
	G_SHUTTER = preflashShutter;
	G_GAIN = sensorGainIdx;

	G_AEReady = AE_READY;
	aeGainExpSetEn = 1;

	dbgWdtFeed(1000);
	while( !( G_AEReady == AE_NOT_READY && G_AESetting == AE_SET_FINISH ) );
	dbgWdtFeed(1000);
	if(wait){
		HAL_FrontSignalWait(HAL_FRONT_WAIT_VVALID, 1, wait);
	}
	printf("\033[32mpreGain = %bu\033[0m\n",preflashGain);
}

/******************************************
 * @fn        void frontSnapFlashAeCfg(UINT32 shutter, UINT8 gainIndex, UINT8 wait)
 * @brief        frontSnapFlashAeCfg
 * @param     [in] shutter , 0 : use preflash shutter , else : use the input 
 * @param     [in] gainIndex , 
 * @param     [in] wait
 * @retval      NONE
 * @see         NULL
 * @author     fan.xiao
 * @since       2011-09-27 , 14:12:51
 * @todo        N/A
 * @bug         N/A
*******************************************/
void 
frontSnapFlashAeCfg(
	UINT32 shutter,
	UINT8 gainIndex,
	UINT8 wait
)
{	
	UINT32 capShutter;
	UINT8 capGain;

	if(wait == 0){		/*dummy flash , so use preview shutter and gain*/
		capShutter = frontGShutterBk;
		capGain = frontGGainBk;
		 G_SHUTTER = frontGShutterBk;
		 G_GAIN = frontGGainBk;
	}
	else{
		if(shutter){
			capShutter = shutter;
		}
	else{
		capShutter = preflashShutter;
	}
	if(gainIndex != SENSOR_GAIN_AUTO){
		capGain = sensorGainIdxGet(gainIndex);
	}
		else{
			capGain = sensorGainIdxGet(snap_flash_gain);
		}
	}

	printf("\033[32m snapGain = %bu\033[0m\n",capGain);
	frontLastShutterGainSet(capShutter,capGain);
}

/**
 * @brief	configure the snap flash type and duration
 * @param	cfgsel = [in] setting type for snap flash.
 * @param	value = [in] setting value for each configuration setting type.
 * @retval	return = NULL
 * @see
 * @author	CS
 * @since	2008-11-04
 * @todo	N/A
 * @bug		N/A
*/
void
frontSnapFlashCfg(
		UINT32 cfgsel,
		UINT32 value
		)
{
	UINT16 snapFlashPulse;
	switch (cfgsel) {
	default: 
		break;
	case SENSOR_SNAP_FLASH_CFG_MODE:
		if (value == SENSOR_SNAP_FLASH_CURTAIN_FRONT_SYNC) {
			snap_flash_frontsync = 1;
			snap_flash_rearsync = 0;
		} else if (value == SENSOR_SNAP_FLASH_CURTAIN_REAR_SYNC) {
			snap_flash_frontsync = 0;
			snap_flash_rearsync = 1;
		} else {
			snap_flash_frontsync = 0;
			snap_flash_rearsync = 0;
		}
		break;
	case SENSOR_SNAP_FLASH_CFG_WIDTH:
		/*calculate the flash parameters*/
		if(value <= 300){	/* min is 0.015us , max is 30 us*/
			printf("USE PWM 1\n");	
			snap_flash_use_pwm = 1;
			
			/*cfg the trigger GPIO to be PWM function*/
			snapFlashPulse = (UINT16)(((UINT32)value * 522) /(XBYTE[0x2021] + 1)/10); /*USE the max redClkDiv to calculate*/

			//HAL_GlobalConfigPG(1,0,0,0x7ff/*0x7ff = 2047 ,It's the max*/,snapFlashPulse,1);	/*Set the function of GPIO5 for PWM , for snap flash pulse control*/
			HAL_FlashTriggerPinPwmCfg(snapFlashPulse);
			
			snap_flash_width = (value + 200)/100;				/*delay for disable the PWM*/
		}
		else if(value <= 1200){		/*the PWM max pluse width is 124*/		
			printf("USE PWM 2\n");	
			snap_flash_use_pwm = 2;

			/*cfg the trigger GPIO to be PWM function*/
			snapFlashPulse = (UINT16)(((UINT32)value * 522) /(31 + 1)/10);	/*USE the max redClkDiv to calculate*/
			//HAL_GlobalConfigPG(1,0,0,0x7ff/*0x7ff = 2047 ,It's the max*/,snapFlashPulse,1);  /*Set the function of GPIO5 for PWM , for snap flash pulse control*/
			HAL_FlashTriggerPinPwmCfg(snapFlashPulse);
			
			snap_flash_width = (value + 200)/100;   /*delay for disable the PWM*/
		}
		else{			
			printf("USE GPIO\n");	
			snap_flash_use_pwm = 0;
			snap_flash_width = value/100;			/*delay for trigger time*/
			
			/*cfg the trigger GPIO to be  GPIO function and output 0*/
			#if 0
			XBYTE[0x2030] &= ~0x20;	/*output 0*/
			XBYTE[0x201b] &= ~0x02;	/*cfg for GPIO*/
			XBYTE[0x2034] |= 0x20;	/*Out put enable*/	
			#endif
			HAL_FlashTriggerPinGpioCfg();
		}
		break;
	case SENSOR_SNAP_FLASH_CFG_H_POS:
		snap_flash_delay = value;
		break;
	case SENSOR_SNAP_FLASH_CFG_DELAY_TYPE:
		snap_flash_delay_type = value;
		break;
	case SENSOR_SNAP_FLASH_CFG_MAX_GAIN:
		snap_flash_max_gain = value;
		break;
	}
}


/**
 * @brief	snap flash triggered
 * @param
 * @retval	return = NULL
 * @see
 * @author	CS
 * @since	2008-11-04
 * @todo	N/A
 * @bug		N/A
*/
void
frontSnapFlashTrig(
	UINT32 exptime
	)
{
	UINT8 sts;
	UINT8 regClkDivBk;
	
	regClkDivBk = XBYTE[0x2021];
	
	if(snap_flash_width == 0 ){
		printf("Trig Er2!\n");
		return;
	}

	switch(snap_flash_delay_type) {
		case SENSOR_FLASH_DELAY_TYPE_H_POS:
			ENTER_CRITICAL(sts);
			
			if(snap_flash_use_pwm == 2){
				XBYTE[0x2021] = 31; 				/* Set the max regClkDiv*/
			}
			if(exptime == 0){
				frontSignalWait(FRONT_WAIT_VVALID, FRONT_WAIT_RISING, 1);
				HAL_FlashTriggerDo(snap_flash_width,snap_flash_use_pwm);
				frontSignalWait(FRONT_WAIT_VVALID, FRONT_WAIT_RISING, 1);
			}
			else{
				frontSignalWait(FRONT_WAIT_VVALID, FRONT_WAIT_FALLING, 1);
				HAL_FlashTriggerDo(snap_flash_width,snap_flash_use_pwm);
			}
			
			XBYTE[0x2021] = regClkDivBk;				/* restore the regClkDiv*/

			frontLastShutterGainSet(frontGShutterBk,frontGGainBk);
			G_SHUTTER = frontGShutterBk;
			G_GAIN = frontGGainBk;
			EXIT_CRITICAL(sts);
			#if 0
			XBYTE[0x2AC8] = (UINT8) (preflash_delay & 0xFF);
			XBYTE[0x2AC9] = (UINT8) ((preflash_delay >> 8) & 0xFF);		
			XBYTE[0x2ACA] = (UINT8) ((snap_flash_delay + snap_flash_width) & 0xFF);
			XBYTE[0x2ACB] = (UINT8) (((snap_flash_delay + snap_flash_width) >> 8) & 0xFF);
			ENTER_CRITICAL(sts);
			frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_RISING, 1);
			XBYTE[0x2AC1] = 0x03;
			while(!(XBYTE[0x2AC1] & 0x01));
			sp1kST_TrigSet(1);
			while(!(XBYTE[0x2AC1] & 0x02));
			sp1kST_TrigSet(0);*/
			EXIT_CRITICAL(sts);
			#endif
			break;
		case SENSOR_FLASH_DELAY_TYPE_IMMEDIATE_TIMER:
			ENTER_CRITICAL(sts);
			HAL_Front10UsWait(snap_flash_delay);

			if(snap_flash_frontsync == 1) {
				sp1kST_TrigSet(1);
				HAL_Front10UsWait(snap_flash_width);
				sp1kST_TrigSet(0);
				HAL_Front10UsWait((UINT32)(exptime -snap_flash_delay - snap_flash_width));
			}
			else {
				HAL_Front10UsWait((UINT32)(exptime -snap_flash_delay - snap_flash_width));
			};

			if(snap_flash_rearsync == 1) {
				sp1kST_TrigSet(1);
				HAL_Front10UsWait(snap_flash_width);
				sp1kST_TrigSet(0);
			};
			break;
		default: break;
	}
}


/**
 * @brief	Strobe flash initialize
 * @param
 * @retval	return = NULL
 * @see
 * @author	CS
 * @since	2008-11-04
 * @todo	N/A
 * @bug		N/A
*/

void
frontStrobeInit(void)
{
	sp1kSTChgEnSet(0);
	sp1kSTChgEnCfg();
	sp1kST_TrigSet(0);
	sp1kST_TrigCfg();
	//sp1kSTChgRdyCfg();

	//XBYTE[0x20a6] &= ~0x02;  /*disable the gpio16 pull high/low*/
	
	//sp1kPullSelSet(4,0x20,0x20);/*Pull high the rdy check GPIO16*/

	/*gpioByteFuncSet(GPIO_BYTE_GEN2,0x01,0x00);*/
	//HAL_GpioByteDirSet(/*GPIO_BYTE_GEN2*/3,0x01,0x00);//gpio 16 -> input
	/*gpioByteOutSet(GPIO_BYTE_GEN2,0x04,0x00);//disable charge */
}

void
frontPreflashTrig_Intr(
	UINT8 en
	)
{
	if(en == 1)
		preflash_state = 1;
	else
		preflash_state = 0;
}

