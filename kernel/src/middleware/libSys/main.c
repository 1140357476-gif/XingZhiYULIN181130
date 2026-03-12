/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

		main.c

Abstract:

		Module related to main routine

Environment:

		Keil C51 Compiler

Revision History:

		08/28/2001		Chi-Yeh	Tsai	created

--*/

//=============================================================================
//Header file
//=============================================================================
#define __FILE   __FILE_ID_MAIN__
#include "general.h"
#include "sys_svc_def.h"
#include "asicreg.h"
#include "main.h"
#include "initio.h"
#include "setmode.h"
//#include "disk.h"
//#include "sensor.h"
#include "timer.h"
#include "bulkout.h"
#include "cardlink.h"
#include "class.h"
#include "sd_def.h"
#include "pv_task.h"
#include "utility.h"
#include "ctlsetup.h"
#include "doslink.h"
#include "uiflow.h"
#include "sp1k_util_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_snap_api.h"
#include "snap_qtable.h"
#include "sp1k_pv_api.h"
//#include "sidcOption.h"
//richie@mi	0409
#include "usbmsdc2.h"
#include "cmd_func_fpga.h"
#include "gpio.h"
//#include "dist_func.h"

//richie@si0418
//#include "sidcinit.h"
//#include "sidcmain.h"

//patch4.3@richie@zo
extern xdata UINT16	G_usZoomOSDTimer;
//patch4.4@richie@dsi begin
#if	( DOSRAM_OPTION	== 1 )
extern	xdata	UINT32	G_ulDRAMSTORFAT1AddrTemp;
#endif



//patch5.1@Add copy to card function begin
#include "copydisk.h"
//patch5.1@Add copy to card function end

#include "sdfunc.h"

#include "os_api.h"
#include "os_msg.h"

//#ifdef ICAT_OPTION//xian ++
#include "icat_stub.h"	//xian ++
//#endif

//#include "cardusr.h"
#include "sp1k_hal_api.h"
//#include "sp1k_awb_api.h"//wangjg++
#include "dpf_scan.h"
#include "hal_dram.h"
#include "dbg_def.h"
#include "cmd.h"
#include "hal_global.h"
#include "sp1k_cdsp_api.h"
#include "cdsp_init.h"
#include "app_init.h"

#include "sp1k_cdsp_api.h"
#include "reg_def.h"
#include "global_init.h"
#include "host_func_opt.h"
#include "app_dev_disp.h"

//=============================================================================
//Symbol
//=============================================================================
UINT8 dosSdBgWriteEnd(void);
void dosSdBgWrite(void);

UINT8 code RsvCodeBuff[0x1];
//Reserved for debug
UINT8 xdata __STACK[0x200] _at_ 0x7800;
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
//bit flag

xdata	UINT8	G_btStopStream = 0;
xdata UINT8 G_calibrationMode = 0;

xdata 	UINT8	G_bUSBSpeed = 0;
//state	and	error code
//type of storage used
xdata	UINT8	G_ucStorageType;
//command and data buffer of control pipe and bulk in/out pipes
xdata	UINT8	G_ucCtrlCommand[K_CtrlCommandSize];
xdata	UINT8	G_ucCtrlData[K_CtrlDataSize];
xdata	UINT8	G_ucStorCommand[K_StorCommandSize];
#if	( SIMULATE == 1	) // WWWW
xdata	UINT8	G_ucStorData[K_DMA_MaxSize];
#else
xdata	UINT8	G_ucStorData[K_DMA_MaxSize]	_at_ 0x3800;	//ada@0118
#endif
/*++
xdata	UINT8	G_ucStor2Command[K_Stor2CommandSize];
xdata	UINT8	G_ucStor2Data[K_Stor2DataSize];
--*/
//pointer to data buffer
UINT8	*G_pucCtrlDataPointer;	//for both CODE	and	DATA area
xdata	UINT8	*G_pucStorDataPointer;
/*++
xdata	UINT8	*G_pucStor2DataPointer;
--*/
//storage interface	N belongs to which USB class

//richie

//patch4.4@richie@usbvd

//richie@ae0305
xdata	UINT8	G_ucStrobeCnt;
xdata	UINT8	G_ucStrobeRdy;

//patch4.4@richie@flip
xdata	UINT8	G_ucFlip;//bit1:H-flip,	bit2:V-flip

//------------------------------------------------------------------------------
//L1 Global	variables
//------------------------------------------------------------------------------
//ISP
xdata	UINT8	G_ISP;
xdata	UINT32	G_ISPDRAMAddr;
xdata	UINT32	G_ISPROMAddr;
xdata	UINT32	G_ISPSecAddr;
xdata	UINT16	G_ISPSecCnt;

//patch4.3@richie@je0531 begin
//data	UINT8	G_ucISPCheck1;
//data	UINT8	G_ucISPCheck2;
//patch4.3@richie@je0531 end

xdata 	UINT32	G_timerCheckTestUnit;
xdata	UINT8	G_testUnitReadyFlag;
xdata 	UINT8	G_ae_ON;
xdata	UINT8	G_ucPccamTuningFlag;
//xdata      UINT8	G_ucProbeControlFlag;  
xdata   UINT8	G_ucCommitControlFlag; 
xdata	UINT8	G_FrameRate;
xdata	UINT8	G_FrameIndex; 
xdata	UINT8	G_FormatIndex; 

#if STILL_SNAP_ON
xdata	UINT8	G_StillFormatIndex;
xdata	UINT8	G_StillFrameIndex; 
xdata   UINT8    G_ucStillImageTriggerFlag;
#endif

xdata	UINT32	G_PixelClk;
xdata	UINT16	G_HTotal;
xdata	UINT16	G_VTotal; 
xdata   UINT8	G_USBSetInterface; 
xdata   UINT8   G_USBAudSetInterface;
//xdata UINT8	G_USBinterin; 
xdata UINT8	G_USBAudioSetInterface; 

//device component
xdata	UINT8	G_SensorID;				//TBD by YGLiu
//patch5.0@richie@sdram	mapping	mark
//xdata	  UINT8	  G_CardType;			  //TBD	by WZH
xdata	UINT8	G_AudioModuleType;		//0:Buildin	audio CODEC, 1:	External AC-97 Codec
xdata	UINT8	G_DisplayType;			//TBD by Prover
//mode setting
//xdata	  UINT8	  G_QtableIndex;		  //0: Q50,	1:Q70, 2:Q80, 3:Q85, 4:Q88,	5:Q90, 6:Q92, 7:Q95	//ada@0225
xdata	UINT8	G_QtableIndex;			//0: Q50, 1:Q70, 2:Q80,	3:Q85, 4:Q88, 5:Q90, 6:Q92,	7:Q95
xdata	UINT8	G_DRAMint;
//user interface
xdata	UINT16	G_ObjectID;				//Current object ID
xdata	UINT8	G_ObjectType;			//Current object type, 0: still	image, 1: videoclip	with audio,	2: videoclip w/o audio 3: audio	file
xdata	UINT16	G_KeyState;				//UI key state,	this variable is set by	the	polling	function or	sent by	the	USB	vendor command
//component	setting
xdata	UINT8	G_FrameRate;
xdata	UINT16	G_Hsize;				//Full image width of the sensor
xdata	UINT16	G_Vsize;				//Full imgage height of	the	sensor
xdata	UINT16	G_MHSize;				//Monitor mode H size
xdata	UINT16	G_MVSize;				//Monitor mode V size
xdata	UINT16	G_DSPHsize;				//Display buffer horizontal	size
xdata	UINT16	G_DSPVsize;				//Display buffer vertical size
//xdata	UINT8	G_3ACount=0;
xdata	UINT8	G_AWBCount=0;
//xdata	UINT8	G_3AFlag;
xdata	UINT8	G_PVZFactor;			//Current zoom ratio in	the	preview	mode, this ratio is	used for later capture
//xdata	UINT8	G_PBZFactor;			//Current zoom ratio in	the	playback mode.
xdata	UINT8	G_PBID;					//DSC playback ID
xdata	UINT8	G_GOSDID;				//Graphic OSD ID
//SDRAM	arrangement
xdata	UINT32	G_AudioAddr;			//Audio	buffer starting	address
xdata	UINT32	G_FreeAddr;				//Fress	SDRAM space	starting address (this space is	used as	temprary buffer	during camera operation)
xdata	UINT32	G_RawImageBufAddr;		//patch4.2@yichang@0513	for	continue snapping

//date/time
xdata	UINT8	G_DateStamp;			//0: Date stamp	function is	off, 1:	date stamp function	is on
xdata	dateStc_t	G_DATEINFO;
//USB
//testing

//For Videoclip
xdata UINT8	G_ABVLCIntFlag;
xdata UINT32	G_AVLCSize;
xdata UINT32	G_BVLCSize;

//patch4.4@richie@sus
xdata UINT8	G_ucSuspend;

//patch4.5@richie@checksum isp begin
#if	0//(ISPCHECKSUM_OPTION)
xdata UINT16 G_usISPCheckSum;
#endif
//patch4.5@richie@checksum isp end

//patch4.5@richie@vender info begin
//patch5.2.1@richie@mmlun begin
#if 0
xdata UINT8	G_ucMSDC_CARD;
xdata UINT8	G_ucMSDC_BUILT_IN;
#endif
//patch5.2.1@richie@mmlun end
/*
xdata	 UINT8	 G_ScsiVendorInformation[8]	=
{
		 'S',  'u',	 'n',  'p',	 'l',  'u',	 's', 0x20,
};

xdata	 UINT8	 G_ScsiProductIdentification[16] =
{
		 'S',  'u',	 'n',  'p',	 'l',  'u',	 's',  ' ',
		 'S',  'P',	 'C',  'A',	 '5',  '3',	 '3', 0x20,
};

xdata	 UINT8	 G_ScsiProductRevisionLevel[4] =
{
		 '1',  '.',	 '0',  '0',
};
*/
//patch4.5@richie@vender info end

//patch3.2@ada@0401	For	file system	refreshing when	back from mass storage
//For system refreshing	when back from mass	storage
xdata UINT8	G_MassStorage_Mode;

//patch4.2@ada@0527	for	key	scan
xdata UINT8	G_MainLoopCount;

//allen added

//sunyong added
xdata UINT8 IOTrap;
//xdata UINT8 G_ShadowData[0x1000]	_at_ 0x6000;

//addby phil @ 08.04.30
UINT32 xdata G_TimerRef;

// wyeo@0521, patch	4.2
#if	(NANDF_OPTION == 1 && CARD_OPTION == 1)

// when	1, use 0x5B	cammand	to test	nand reserve block
// sjould 0	to reduce code size
#define	K_TEST_RESERVE_BLOCK	0

UINT8 TestReserveBlock(void);
#endif

/* for dram assert*/
UINT8 xdata G_DramAssert;

#if SUPPORT_MCU_WTD
UINT8 check_usb_msdc_flag = 0;
#endif



void mainJobDo(void);
void mainTestAp(void);

#if 1
#define _DBG_CLR_WARNING_

extern void jump_main(void);
extern UINT8 code K_CharFont[];
extern UINT8 code K_CharFont1[];
extern UINT8 code GOSD_data0[];
#endif

//Flow 17
#if 0
extern void DIVSETX(UINT32 Divindend);
extern void DIVSETY(UINT32 Divisor);
extern UINT32 DIVDO(void);

//2 Don't chang the sequence of function-calling
#define UserDiv(Divindend, Divisor, Quotient) { \
                                                   DIVSETY(Divisor); \
                                                   DIVSETX(Divindend); \
                                                   Quotient = DIVDO();\
                                              }
#endif

/*
    Add for FPGA verify used.
    Refers to "#define FPGA_MEM_POOL_ADDR   (0x80000UL + 0x8000UL)".
*/
#ifdef FPGA_MEM_POOL_ADDR
UINT8 xdata G_FPGAMemPool[0x3000] _at_ 0xc000;
#endif

extern code UINT16 pkgChipId;
//=============================================================================
//Program
//=============================================================================
/**
 * @brief     main
 * @param     NONE
 * @retval    NONE
 * @see       none
 * @author    Phil Lin
 * @since     2010-01-01
 * @todo      N/A
 * @bug       N/A
*/
void
__main(
	void
)
{
	globalInit();
	
	mainJobDo();
}

/**
 * @brief     mainJobDo
 * @param     NONE
 * @retval    NONE
 * @see       none
 * @author    Phil Lin
 * @since     2010-01-01
 * @todo      N/A
 * @bug       N/A
*/
void
mainJobDo(
	void
)
{
	UINT32 mainCnt = 0;

	#if SUPPORT_MCU_WTD
	static UINT8 flag = 0;
	static UINT32 dleay_cnt = 0;
	#endif
	
	/*
		Note: The uart interrupt always be disabled.
		The cmd line request handled by flag pooling
	*/
	ES0 = 0;
	while (1) {
		mainCnt++;

		/* feed the watch dog */
		dbgWdtFeed(-1);
		
		/* Check if there's any Message in Msg-Que */
		if (osMsgQuery() == TRUE) {
			osMsgRcv();		
		}

		/* Task schedule */
		osSched();

		/* system service */
		sysSvcSched(&mainCnt);

		#if SUPPORT_MCU_WTD
		if (check_usb_msdc_flag) {
			if (++dleay_cnt == 200/*0xA2C2A*/) {
				dleay_cnt = 0;
				appSetWTDHeartbeat(flag);
				flag = !flag;
			}
		}
		#endif
	}
}

