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
 *                                                                         *
 **************************************************************************/

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "intr.h"
#include "hal.h"
#include "main.h"
#include "ctlsetup.h"
#include "ctlout.h"
#include "ctlin.h"
#include "class.h"
#include "bulkout.h"
#include "bulkin.h"
#include "front_logi.h"
#include "intin.h"
#include "usbmsdc2.h"
#include "uiflow.h"
#include "utility.h"
#include "sp1k_util_api.h"
#include "video_clip.h"
#include "initio.h"
#include "Uart.h"
#include "gpio.h"
#include "setmode.h"
#include "hal_moe.h"
#include "cmd_func_FPGA.h"
#include "sp1k_fd_api.h"
#include "dist_func.h"
#include "hal_global.h"
#include "ae_api.h"
#include "audio_rw.h"
#include "hal_audio.h"
#include "dbg_def.h"
#include "video_debug.h"
#include "hal_usb.h"
#include "hal_fd.h"

#include "ae_def.h"
#include "ae_debug.h"
#include "os_api.h"
#include "app_video_view.h"
#include "cdsp_task.h"
#include "reg_def.h"
#include "gpio_api.h"
#include "awb_api.h"
#include "video_mem.h"
#include "sd_bg.h"
#include "gui_tp.h"
#include "hal_disp_tp.h"
#include "video_capture.h"

#include "af_api.h"

#include "hal_dram.h"
#include "app_msg.h"
#include "video_zoom.h"
#include "pv_task.h"
#include "panorama_def.h"
#include "panorama_api.h"



/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/


/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/
data  BIT  G_btBulkIntr;
data  BIT  G_btEP0Intr;

UINT16 xdata IntSts;
UINT8 xdata Gt0Cnt;
UINT8 xdata Gt1Cnt;
UINT8 xdata Gt2Cnt;
UINT16 tvTime=1093;
xdata UINT8 G_USBNewConfigureOK = 0x00;

xdata UINT8 G_VDRFlag;
//extern xdata videoStampCfg_t g_vidDateStamp;//wangrf++
extern xdata UINT8 G_ucSuspend;
extern xdata UINT8 G_Card_Type;
extern xdata UINT8	G_AEFlag;

extern xdata UINT16 audio_curr_energy;
extern xdata UINT16 high_thr_cnt,silent_cnt;
extern xdata UINT8 frame_count, pre_frame_count;
extern xdata UINT8 init_frame_cnt;

//extern xdata UINT8 gpioByteIdSpkEn, gpioBitMskSpkEn;
extern xdata UINT8 Uartmode; //
extern xdata UINT8 UartSendEnable;//
//For high gain noise - Jim@0512
//extern UINT8   G_luma;
//extern code UINT16 cmt[3][9];

extern xdata UINT8 vidDateStampSts; //xian ++ 20081029
extern xdata UINT8 photoEnable;

xdata UINT8 G_USBReset = 0x00;
//xdata UINT8 G_USBCharge = 0x00;
xdata UINT8 AdapterCharge=1; //  1: USB contect power adapter charge   0:USB contect PC
extern xdata UINT8 G_USBSetInterface;
extern xdata UINT8 G_UsbPccamStart;
extern xdata UINT8 G_UsbPccamStop;
extern xdata UINT8 G_usbPvFrameCount;
extern xdata UINT8 G_USBAudSetInterface;
//extern xdata UINT8 G_USBinterin;//cx++ for intr in test
extern appAudRecCtrl_t xdata audRecCtrl;
extern xdata UINT16 G_WindowSize;
extern xdata UINT8 G_Sound_Ctrl;

xdata UINT8 G_CodeSentry;
xdata UINT8 G_CodeSentry_270A;
xdata UINT8 G_CodeSentry_270B;
xdata UINT8 G_DgpioValue=0xff;
extern code UINT16 pkgChipId;
extern UINT8 G_tpPenState;
extern UINT8 G_tpSarDelayFlag;

#if STILL_SNAP_ON
extern xdata       UINT8	G_SnapFlag;
#endif

extern UINT8 G_FrameIntervalOption;

#if PANORAMA_EN
/* Panorama func */
extern xdata UINT8 G_MEIntrSts;//0:initial, 1:ME start, 2: ME Done
extern xdata UINT8 G_MEIntrBufIdx;
extern xdata UINT8 G_PanoramaBufSts;//0:not chang, 1:chang
extern xdata UINT8 G_PanoramaEn;//0:PANORAMA_DISABLE, 1:PANORAMA_ENABLE
#endif


//extern UINT32 totalFrameCnt;
//extern UINT8 xdata G_FPGAMemPool[0x3000];

/*#if (VIDEO_FRONT_CDSP_JPG_DBG == 1)
UINT8 videoStatisticsEn;
//UINT8 videoCdspRegSave;
//UINT32 videoCdspChangeCnt;
UINT32 videoJpgTriggerCnt;
UINT32 videoFrontInputCnt;
#endif
*/
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
void INTR_SDBgRw(void);
extern void HAL_ColorSatReduce_Intr(UINT8);
extern void INTR_calibrationPrvRawAddrSet(void);
#ifdef VIDEO_REC_BRC_EN
extern void INTR_videoClipBrcVlcOvfSetQtable(void);
#endif
//extern void videoDateStamp(void); //xian ++ 20081029
extern UINT8 HAL_GlobalICVerGet_intr(void);
extern xdata UINT8 G_USBSpeedStartCheck;
extern UINT8 VideoRecBegin;
extern UINT8 VideoRecFrameAB;
extern UINT16 g_vidParam[VD_PARAM_MAX];
extern pIntrFunc code intrPtrFunc;

extern xdata UINT8 FWGain[512];
extern xdata UINT8 HWGain[512];

extern xdata 	UINT8	G_bUSBSpeed;
extern xdata	UINT8	G_usbState;
extern xdata	UINT8	G_ucPktStorIntrf;

extern xdata UINT8 vdCount;
extern xdata UINT8 aeVdCount;

extern xdata UINT8 vdProcEn;
extern xdata UINT8 G_ExpGainSetPos;

extern UINT8 aeSyncStatus;

extern xdata UINT8 csDbgData[];

extern intrSmoothZoom xdata intrSmoothZoomCtrl;

extern UINT8 aeProc;

//UINT8 zfTgSkipFrameCnt;

void INTR_SmoothZoomStopCheck(void);
void INTR_ZoomZfRegSet(UINT8 videoMode);
void INTR_ZoomUpdateSet(	void);
void INTR_HQDZoomFrontGateDisableSet(void);


extern appVideoZoom_NBlocking_t xdata VideoZoomNBCtrl;
extern UINT8 IntrvideoRecStatusGet(void);
void INTR_VideoZoom (void);

extern appPvZoom_NBlocking_t xdata pvZoomNBCtrl;

extern void INTR_PvZoom(void);


UINT32 intrTime;
#define INTR_TIMR_DECTED 0
//UINT32 awbErrCnt=0;
//-----------------------------------------------------------------------------
//INTR_Int0
//-----------------------------------------------------------------------------
void INTR_Int0(void)
/*++

Routine Description:

        service routine of interrupt 0

Arguments:

        none

Return Value:

        none

--*/

{
     //UINT32 tt0,tt1;
//	 UINT16 ysize;
//	 UINT16 disHOff,disVOff;

	//UINT32 awbTime;
	//UINT32 xdata *pt;
#if !TAG_TODO
	UINT8 log2500, log2501, log2502, log2503;
	#if 0 /* mark for 1628 FPGA */
	UINT8 alctmp;
	#endif
    UINT8 sts;
#endif //TAG_TODO
	UINT8 idx;
	IntSts = ((UINT16)XBYTE[REG_Cpu_IntrSts1]<<8) | (UINT16)XBYTE[REG_Cpu_IntrSts0];
	INTR_DisableInterrupt();

	INTR_Enter();

#if !TAG_TODO
    if (intrPtrFunc != NULL) {
        sts = (*intrPtrFunc)();
        if (sts == TRUE) {
            goto INTR_EXIT;
        }
    }
	#if INTR_TIMR_DECTED
	WRITE8(tt0, 0, XBYTE[0x20C4]);
	WRITE8(tt0, 1, XBYTE[0x20C3]);
	WRITE8(tt0, 2, XBYTE[0x20C2]);
	WRITE8(tt0, 3, XBYTE[0x20C1]);
	#endif
	log2500 = XBYTE[REG_Usb_EpAckIntSts];        //save 0x25c0
	log2501 = XBYTE[REG_Usb_EpNakIntSts];        //save 0x25c1
	log2502 = XBYTE[REG_Usb_IntSts];        //save 0x25c2
	log2503 = XBYTE[REG_Usb_StdCmdIntSts];

	G_btEP0Intr = 0;
#if 1
		if ((IntSts & INTR_SRC_TYPE_TVENC) == INTR_SRC_TYPE_TVENC){
			if((XBYTE[REG_Disp_VsVdIntSts]&0x02) && (XBYTE[REG_Disp_VsVdIntEn]&0x02) && (HAL_GlobalICVerGet_intr()!=0x01))
			{
				XBYTE[REG_Disp_VsVdIntSts] |= 0x02;
			}
		}
#endif


//1 USB

	if ((IntSts & INTR_SRC_TYPE_USB) == INTR_SRC_TYPE_USB) {
		if (log2500 & 0x04)        //check ACK of EP0 Out packet
	  	{
			XBYTE[REG_Usb_EpAckIntSts] = 0x04;           //clear Ep0OutAck
			//INTR_putchar('o');
			CTLOUT_Packet();                //Ep0OutAck
			G_btBulkIntr = 0;
			G_btEP0Intr = 1;
		}
	  	else if (log2500 & 0x02)        //check ACK of EP0 In packet
	  	{
	  		XBYTE[REG_Usb_EpAckIntSts] = 0x02;           //clear Ep0InAck
			//INTR_putchar('i');
	  		CTLIN_Packet();                 //Ep0InAck
			G_btBulkIntr = 0;
			G_btEP0Intr = 1;
	  	}
	  	else if (log2500 & 0x01)             //check ACK of EP0 Setup packet
	  	{
			XBYTE[REG_Usb_EpAckIntSts] = 0x01;           //clear Ep0SetupAck
			//INTR_putchar('s');
			G_USBReset = 0x00;
			AdapterCharge = 0;

			G_bUSBSpeed = 0;
			if ((XBYTE[REG_Usb_HighSpeed]&0x01) == 0x01){
			/* high speed */
			G_bUSBSpeed = 1;
			//printf("h");
			}

			if (G_UI_USBConnect != K_UI_USB_CONNECT){
				G_UI_USBConnect = K_UI_USB_CONNECT;
			}
			CTLSETUP_Packet();              //Ep0SetupAck
			G_usbState &= 0xfe;
			G_btBulkIntr = 0;
			G_btEP0Intr = 1;
		}
	  	else if ((log2500 & K_USB_CLASS_IN_ACK_MASK)        //check ACK of Bulk In packet
			&& (XBYTE[REG_Usb_EpAckIntEn] & K_USB_CLASS_IN_ACK_MASK))
		{
	  		XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_IN_ACK_RESET;           //clear BulkInAck
			G_USBReset = 0x00;
			//INTR_putchar('y');
	  		G_ucPktStorIntrf = K_StorageInterface1;
	  		BULKIN_Packet();               //BulkInAck
			G_btBulkIntr = 1;
		}
	  	else if ((log2500 & K_USB_CLASS_OUT_ACK_MASK)        //check ACK of Bulk Out packet
			&& (XBYTE[REG_Usb_EpAckIntEn] & K_USB_CLASS_OUT_ACK_MASK))
	  	{
			XBYTE[REG_Usb_EpAckIntSts] = K_USB_CLASS_OUT_ACK_RESET;           //clear BulkOutAck
			G_USBReset = 0x00;
			//INTR_putchar('x');
			XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
			XBYTE[REG_Front_Finten_low] = 0x00;

			G_ucPktStorIntrf = K_StorageInterface1;
			BULKOUT_Packet();               //BulkOutAck
			G_btBulkIntr = 1;
		}
	  	else if (log2500 & 0x20)        //check ACK of Interrupt In packet
	  	{
	  	     // INTR_putchar('I');
			XBYTE[REG_Usb_EpAckIntSts] = 0x20;           //clear IntInAck
			G_ucPktStorIntrf = K_StorageInterface1;
			INTIN_Packet();                 //IntInAck
			G_btBulkIntr = 0;
			//G_USBinterin = 1;//cx++ for intr in
	  	}
		/*else if (log2501 & 0x20)        //cx++ check nak of Interrupt In packet
	  	{
	  	       INTR_putchar('N');
			XBYTE[REG_Usb_EpNakIntSts] = 0x20;           //clear IntInnck
			G_USBinterin = 1;//cx++ for intr in
			G_btBulkIntr = 0;
	  	}*/
	 	else if (log2502 & 0x20)        //check video interface alternate setting changed device Interrupt In packet
	 	{
	 		if(XBYTE[REG_Usb_UsbVidAs]!=0)  //cx++
			{
				G_UsbPccamStart = 1;
				//G_USBSetInterface = 1;  //start pccam
			}else
			{
                G_UsbPccamStop = 1;
				//G_USBSetInterface = 2; //stop pccam
			}
	   		XBYTE[REG_Usb_IntSts] = 0x20;           //clear VidASChg
	 	}
	 	else if (log2502 & 0x40)        //check audio interface alternate setting changed device Interrupt In packet
	 	{
			if(XBYTE[REG_Usb_UsbAudAs]!=0)  //cx++
			{
				G_USBAudSetInterface = 1;  //start audio
			}else
 			{
				G_USBAudSetInterface = 2; //stop audio
			}
			XBYTE[REG_Usb_IntSts] = 0x40;           //clear AudASChg
	 	}
		else if ((log2502 & 0x01) && (XBYTE[REG_Usb_IntEn] & 0x01)) //check suspend intr
		{
			XBYTE[REG_Usb_IntSts] = 0x01;           //clear suspend
			INTR_putchar('R');
			//INTR_putchar('W');
			//INTR_putchar('#');
			//INTR_putchar('\n');
			G_USBReset = 0x01;
			//G_USBCharge = 0x01; 	//add for USB battery charge
			//AdapterCharge = 0;
		//	XBYTE[REG_Dma_DmaIdle] = 0x01;	//RST DMA
		//	XBYTE[REG_Dma_DmaIdle] = 0x00;
			XBYTE[REG_Usb_BufClr] = 0x1F;
		//	XBYTE[REG_Fm_Mediatype_rst] |= 0x40;	//RST SD
			XBYTE[REG_Usb_Suspend] &= 0x01;  // 20100802 shengkai added for USB EL-9 test
			G_ucBulkPhase = K_CommandPhase;
		}

		else if (log2502 & 0x02) //check suspend intr
		{
			XBYTE[REG_Usb_IntSts] = 0x02;           //clear suspend
			G_ucSuspend = 1;
		}
		else if (log2502 & 0x04) //check resume intr
		{
			XBYTE[REG_Usb_IntSts] = 0x04;           //clear resume
			G_ucSuspend = 0;
		}
		else if (log2502 & 0x10) //check configuration change intr
		{
			XBYTE[REG_Usb_IntSts] = 0x17;           //clear suspend/resume/configuration change
			//XBYTE[REG_Usb_IntEn] &= ~0x10;
			XBYTE[REG_Usb_IntEn] |= 0x06;
	   		G_USBNewConfigureOK = 1;
			G_USBSpeedStartCheck = 0;
			INTR_putchar('C');
		}
		/* 20100802 shengkai added for USB EL-9 test */
		else if (log2503 & 0x20)
		{
			/* only when wValue = 0x0002 & wIndex = 0x0300 we set the register */
			if ( XBYTE[REG_Usb_wValueL]==0x02 && XBYTE[REG_Usb_wValueH]==0x00&
				 XBYTE[REG_Usb_wIndexL]==0x00 && XBYTE[REG_Usb_wIndexH]==0x03 )
				{
					XBYTE[REG_Usb_Suspend] |= 0x02;
				}
			XBYTE[REG_Usb_StdCmdIntSts]|= 0x20; //clear Set_Feature intr
		}
	}

//1 DRAM

	else if ((IntSts & INTR_SRC_TYPE_DRAM) == INTR_SRC_TYPE_DRAM) {
		if ((XBYTE[REG_Dram_Cdsp1Eof_Sts]&0x20) && (XBYTE[REG_Dram_Cdsp1Eof_Inten]&0x20)) {
			XBYTE[REG_Dram_Cdsp1Eof_Sts] |= 0x20;


			/* check and set zoom factor, zfupdate */
			if ( intrSmoothZoomCtrl.zoomFrameCnt != 0 ) {
				intrSmoothZoomCtrl.zoomFrameCnt--;
			}

			/* cdsp sof check and set zoom factor, zfupdate */
			if ( intrSmoothZoomCtrl.zoomFactUpdate == INTR_SMOOTH_ZOOM_MACHINE_STATUS_START ) {
				if ( intrSmoothZoomCtrl.zoomFrameCnt == 0 ) {
					intrSmoothZoomCtrl.zoomFactUpdate = INTR_SMOOTH_ZOOM_MACHINE_STATUS_NEXT_DELAY_FINISH;
				}
			}

			if ( intrSmoothZoomCtrl.zoomFactUpdate == INTR_SMOOTH_ZOOM_MACHINE_STATUS_NEXT_DELAY_FINISH ) {
				INTR_ZoomUpdateSet();
			}
			else {
				if ( intrSmoothZoomCtrl.zoomFactUpdate == INTR_SMOOTH_ZOOM_MACHINE_STATUS_W_REG_FINISH ) {
					intrSmoothZoomCtrl.zoomFactUpdate = INTR_SMOOTH_ZOOM_MACHINE_STATUS_W_REG_FINISH_NEXT_FRAME;
				}
			}
		}

		if (XBYTE[REG_Dram_Cdsp1Eof_Sts]&0x02 && XBYTE[REG_Dram_Cdsp1Eof_Inten]&0x02) {
			#if 1
			if (vidDateStampSts) {
				//pvPhofrmBlendIntr(K_SDRAM_ImagePlaybackBufAddr, 320, 240);
				XBYTE[REG_Dram_Cdsp1Eof_Sts] |= 0x02;

			}
			if (photoEnable) {
				pvPhofrmBlendIntr(K_SDRAM_PvPFYUVAddr, 320, 240);
				XBYTE[REG_Dram_Cdsp1Eof_Sts] |= 0x02;
			}
			INTR_calibrationPrvRawAddrSet();
			#endif
			XBYTE[REG_Dram_Cdsp1Eof_Sts] |= 0x02;
		}

		#if STILL_SNAP_ON//cx add for still snap
		if (XBYTE[0x2700]&0x40 && XBYTE[0x2708]&0x40) {
			INTR_putchar('#');
			XBYTE[0x2700] |= 0x40; //clear
			XBYTE[0x2708]&= 0xbf;  //disable dram usbrdyend_sts_en
			G_SnapFlag = 2;
			XBYTE[0x2548]&=0xfe;	//disable video ISO pipe
		 }
		 #endif
		//#if !VIDREC_JPEG_EOF_EN

		if(XBYTE[REG_Dram_VlcIntEn] & 0x03 & XBYTE[REG_Dram_VlcRdy]) {

#if (VIDEO_FRONT_CDSP_JPG_DBG == 1)
			if (videoStatisticsEn) {
				videoJpgTriggerCnt++;
			}
#endif
			videoRecIntr();

			#if 0
			//INTR_putchar('c');
			if (VideoRecBegin){
				INTR_videoRecVlcBufSet();
				//INTR_putchar('0');
			}else{
				if ((XBYTE[REG_Dram_VlcRdy]&0x03)==0x03){
					 //wangrf++
					ysize=(g_vidDateStamp.stampH>>4)& 0x01FF;
					disHOff=g_vidDateStamp.disHOff>>4;
					disVOff=g_vidDateStamp.disVOff>>4;

					XBYTE[REG_Jpeg_Stamhoffset_Low] = *((UINT8*)&disHOff+1); //stamp h offset
					XBYTE[REG_Jpeg_Stamhoffset_High] = *((UINT8*)&disHOff+0);
					XBYTE[REG_Jpeg_Stamvoffset_Low] = *((UINT8*)&disVOff+1);//stamp v offset
					XBYTE[REG_Jpeg_Stamvoffset_High] = *((UINT8*)&disVOff+0);

					XBYTE[REG_Jpeg_Stamvsize_Low] = *((UINT8*)&ysize+1);//stamp v size
					XBYTE[REG_Jpeg_Stamvsize_High] = *((UINT8*)&ysize+0);
					//wangrf++ end
					XBYTE[REG_Dram_PauseReq_Low] |= 0x20;
					XBYTE[REG_Jpeg_YUV_Type] &= 0xFC;
					XBYTE[REG_Jpeg_YUV_Type] |= 0x01;
					XBYTE[REG_Dram_PauseReq_Low] &= ~0x20;
					XBYTE[REG_Dram_VlcIntEn]|=0x03;
					INTR_videoRecVlcBufSet();
					VideoRecBegin = 1;
					//INTR_putchar('2');
				}
				else
				{
					XBYTE[REG_Dram_VlcIntEn]&=~(XBYTE[REG_Dram_VlcRdy]&0x03);
					//INTR_putchar('1');
				}
			}
			#endif

		}

		//#endif
		/* for Code sentry use */
		if (XBYTE[REG_Dram_Cdsp1Eof_Sts]&0x80 && XBYTE[REG_Dram_Cdsp1Eof_Inten]&0x80) {
			G_CodeSentry_270A = XBYTE[REG_Dram_AddressErrms_Low];
			G_CodeSentry_270B = XBYTE[REG_Dram_AddressErrms_High];
			XBYTE[REG_Dram_Cdsp1Eof_Sts] |= 0x80;
			G_CodeSentry = 0x01;
			INTR_putchar('@');
			#if 1 //wangjg++ for MOE/II/FD dbg
			intrPutUint8(G_CodeSentry_270B);
			intrPutUint8(G_CodeSentry_270A);
			intrPutChar(',');
			#if 0 //wangjg++ codesentry disable hal engine clock
			XBYTE[0x2013]&=~0x20;//disable jpg
			XBYTE[0x2014]&=~0x2c;//disable audio,audio adc,gprm
			XBYTE[0x2015]&=~0x93;//disable audio,cdsp,dma mclk cdsp pclk
			HAL_MoeDbgIntr();

			while(1);//wangjg++ codesentry disable cpu
			#endif
			if(0x1000==((UINT16)((G_CodeSentry_270B & 0x1F)<<8)  | (UINT16)G_CodeSentry_270A))
			{
				/*
				xdata UINT32 xdata* pt32= csDbgData;
				csDbgData[0] = XBYTE[0x2e3b];//moe dst addr
				csDbgData[1] = XBYTE[0x2e3a];
				csDbgData[2] = XBYTE[0x2e39];
				csDbgData[3] = XBYTE[0x2e38];

				csDbgData[4] = XBYTE[REG_II_DstAddr3];//II Scale/II/SII Generation dst addr
				csDbgData[5] = XBYTE[REG_II_DstAddr2];
				csDbgData[6] = XBYTE[REG_II_DstAddr1];
				csDbgData[7] = XBYTE[REG_II_DstAddr0];

				csDbgData[8] = XBYTE[REG_FD_OutStartAddr3];//FD Out addr
				csDbgData[9] = XBYTE[REG_FD_OutStartAddr2];
				csDbgData[10] = XBYTE[REG_FD_OutStartAddr1];
				csDbgData[11] = XBYTE[REG_FD_OutStartAddr0];


				csDbgData[12] = XBYTE[0x2e17];//moe queue idx
				csDbgData[13] = XBYTE[0x2e16];//moe queue idx
				csDbgData[14] = XBYTE[0x2e15];//moe queue idx
				csDbgData[15] = XBYTE[0x2e14];//moe queue idx

				csDbgData[16] = XBYTE[0x2e57];//II queue idx
				csDbgData[17] = XBYTE[0x2e56];//II queue idx
				csDbgData[18] = XBYTE[0x2e55];//II queue idx
				csDbgData[19] = XBYTE[0x2e54];//II queue idxcsDbgData[16] = XBYTE[0x2e54];//II queue idx

				intrPutChar('\n');
				intrPutUint32(pt32[0]);
				intrPutChar(',');
				intrPutUint32(pt32[1]);
				intrPutChar(',');
				intrPutUint32(pt32[2]);
				intrPutChar(',');
				intrPutUint32(pt32[3]);
				intrPutChar(',');
				intrPutUint32(pt32[4]);
				intrPutChar('\n');*/

				HAL_MoeDbgIntr();
				//printf("OUT:MOE(%lx),II(%lx),FD(%lx),(%lx,%bx),(%bx)\n",pt32[0],pt32[1],pt32[2],pt32[3],pt32[4],vdProcEn);
			}
			#endif
		}

	}

//1 JPEG

	else if ((IntSts & INTR_SRC_TYPE_JPG ) == INTR_SRC_TYPE_JPG) {
		if ((XBYTE[REG_Jpeg_Intr_Event] & 0x20)
			&& (XBYTE[REG_Jpeg_Intr_Enable] & 0x20)){/* ME Start */
			XBYTE[REG_Jpeg_Intr_Event] = 0x20;//claer ME Start Sts

			#if PANORAMA_EN
			if ( G_PanoramaEn ) {
				G_MEIntrSts = 1;
				//intrPutChar('S');
			}
			#endif
		}
		else if ((XBYTE[REG_Jpeg_Intr_Event] & 0x40)
			&& (XBYTE[REG_Jpeg_Intr_Enable] & 0x40)){/* ME Dobe */
			XBYTE[REG_Jpeg_Intr_Event] = 0x40;//claer ME Done Sts
			
			#if PANORAMA_EN
			if ( G_PanoramaEn ) {
				if ( G_MEIntrSts == 1 ) {
					G_MEIntrSts = 2;
					//intrPutChar('E');
					osMsgPost(SP1K_MSG_GMV_BY_FRAME_CALCULUS);
					
					/* switch ME buffer, 1st Path YUV buffer */
					if ( G_PanoramaBufSts ) {
						UINT8 sts;
						sts = INTR_panoramaBufSwitch();
						
						if ( sts == SUCCESS ) {
							//intrPutChar('C');
							G_PanoramaBufSts = 0;
						}
						else {
							//intrPutUint8(sts);
						}
					}
				}
			}
			else 
			#endif
			{
				XBYTE[REG_Jpeg_Intr_Event] = 0x60;
				//INTR_DistGmvSet();
				intrPutChar('P');
			}
		}	
		//#ifdef VIDEO_REC_BRC_EN
		else if ((XBYTE[REG_Jpeg_Intr_Event] & 0x08)
			&& (XBYTE[REG_Jpeg_Intr_Enable] & 0x08)){
				//#if VD_REC_DEBUG
				//vdTimeEvt_Intr(VD_EVT_BOUNDARY_OVERFLOW);
				//#endif
				//XBYTE[REG_Jpeg_Intr_Event] = 0x0A;// clear Overflow & MJPG eof  interupt
				INTR_videoClipBrcVlcOvfSetQtable();
				INTR_putchar('O');
			}
		//#endif
		//#if VIDREC_JPEG_EOF_EN
		else if((XBYTE[REG_Jpeg_Intr_Event]&0x02)&&(XBYTE[REG_Jpeg_Intr_Enable]&0x02)){
			XBYTE[REG_Jpeg_Intr_Event] |=0x02;
			if(g_vidParam[VD_CAPTURE]){
				speedyCaptureIntr();
			}
			else{
				if(XBYTE[0x2000] == 0x04) {
					G_usbPvFrameCount++;
				}
				else {
					videoRecIntr();
				}

			}
			#if 0
			if (VideoRecBegin){
				if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x03){
					INTR_videoRecVlcBufSet();
				}
				//INTR_putchar('0');
			}else{
				if ((XBYTE[REG_Dram_VlcRdy]&0x03)==0x03){
					 //wangrf++
					ysize=(g_vidDateStamp.stampH>>4)& 0x01FF;
					disHOff=g_vidDateStamp.disHOff>>4;
					disVOff=g_vidDateStamp.disVOff>>4;

					XBYTE[REG_Jpeg_Stamhoffset_Low] = *((UINT8*)&disHOff+1); //stamp h offset
					XBYTE[REG_Jpeg_Stamhoffset_High] = *((UINT8*)&disHOff+0);
					XBYTE[REG_Jpeg_Stamvoffset_Low] = *((UINT8*)&disVOff+1);//stamp v offset
					XBYTE[REG_Jpeg_Stamvoffset_High] = *((UINT8*)&disVOff+0);

					XBYTE[REG_Jpeg_Stamvsize_Low] = *((UINT8*)&ysize+1);//stamp v size
					XBYTE[REG_Jpeg_Stamvsize_High] = *((UINT8*)&ysize+0);
					//wangrf++ end
					XBYTE[REG_Dram_PauseReq_Low] |= 0x20;
					XBYTE[REG_Jpeg_YUV_Type] &= 0xFC;
					XBYTE[REG_Jpeg_YUV_Type] |= 0x01;
					XBYTE[REG_Dram_PauseReq_Low] &= ~0x20;
					//XBYTE[REG_Dram_VlcIntEn]|=0x03;
					INTR_videoRecVlcBufSet();
					VideoRecBegin = 1;
					//INTR_putchar('2');
				}
				else
				{
					//XBYTE[REG_Dram_VlcIntEn]&=~(XBYTE[REG_Dram_VlcRdy]&0x03);
					//INTR_putchar('1');
				}
			}
			#endif
			#if 0
			 if (VideoRecBegin==1 && (XBYTE[REG_Jpeg_YUV_Type]&0x01)!=0x01 && g_vidParam[VD_REC_YUV_FORMAT]==VD_REC_YUV_422_420){//tq.zhang video yuv420
				if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x00 ){

				}
				else if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x01 ){

				}
				else if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x02 ){

				}
				else{

					ysize=(g_vidDateStamp.stampH>>4)& 0x01FF;
					disHOff=g_vidDateStamp.disHOff>>4;
					disVOff=g_vidDateStamp.disVOff>>4;
					XBYTE[REG_Jpeg_Stamhoffset_Low] = *((UINT8*)&disHOff+1); //stamp h offset
					XBYTE[REG_Jpeg_Stamhoffset_High] = *((UINT8*)&disHOff+0);
					XBYTE[REG_Jpeg_Stamvoffset_Low] = *((UINT8*)&disVOff+1);//stamp v offset
					XBYTE[REG_Jpeg_Stamvoffset_High] = *((UINT8*)&disVOff+0);
					XBYTE[REG_Jpeg_Stamvsize_Low] = *((UINT8*)&ysize+1);//stamp v size
					XBYTE[REG_Jpeg_Stamvsize_High] = *((UINT8*)&ysize+0);
					XBYTE[REG_Dram_PauseReq_Low] |= 0x20;
					XBYTE[REG_Jpeg_YUV_Type] &= 0xFC;
					XBYTE[REG_Jpeg_YUV_Type] |= 0x01;
					XBYTE[REG_Dram_PauseReq_Low] &= ~0x20;
					INTR_videoRecVlcBufSet();
					VideoRecBegin=2;
					}
           		}
			else{
				if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x00 ){

				}
				else if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x01 ){

				}
				else if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x03){

					INTR_videoRecVlcBufSet();
				}
			}
			#endif
		}
	 	//#endif
	}
//1 CDSP
	else if ((IntSts & INTR_SRC_TYPE_CDSP) == INTR_SRC_TYPE_CDSP) {

		#if 0
			if (((XBYTE[REG_Cdsp_Cdspinten_lo] & 0x04) == 0x04) &&((XBYTE[REG_Cdsp_evnt1] & 0x04) == 0x04))	{
				XBYTE[REG_Cdsp_evnt1] = 0x04;
				G_3ACount = G_3ACount+1;
				if(G_3ACount == G_3AFreq) {
					//XBYTE[0x2030]|=0x01;
					//printf("3 AE done enter\n");
					G_3AFlag = 1;
					G_3ACount=0;
					//XBYTE[0x2030]&=(~0x01);
				}
				G_AEFlag = 1;
			}

			if ((totalFrameCnt>0)&&(totalFrameCnt<600)){	/*record output hsize every frame*/
				G_FPGAMemPool[totalFrameCnt*2]=XBYTE[0x22b8];
				G_FPGAMemPool[totalFrameCnt*2+1]=XBYTE[0x22b9];
			}
			XBYTE[REG_Cdsp_evnt1] = 0x01;
		#endif
	}

//1 FRONT
	else if ((IntSts & INTR_SRC_TYPE_FRONT) == INTR_SRC_TYPE_FRONT) {

		aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);	/* status 0 */

		if ( (XBYTE[REG_Front_Finten_low] & 0x01) &&	 //VD rising edge interrupt enable
				(XBYTE[0x2AC0] & 0x01) ) {	//VD rising edge interrupt

			XBYTE[0x2AC0] = 0x01;	/* clear interrupt event. */
			vdProcEn|=0x80;

			aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x10);	/* status 1 */

			vdCount++;
			
			if ((vdCount & 0x03) != (XBYTE[REG_Dram_MpgSb_CdspWRegIdx] & 0x03) ) {
				vdCount = (vdCount & ~0x03) | (XBYTE[REG_Dram_MpgSb_CdspWRegIdx] & 0x03);
				EGTaeIntrPutChar('!');
			}

			GIDaeIntrPutChar('[');
			GIDaeIntrPutUint8(vdCount);
			GIDaeIntrPutChar(']');

			#if (II_AE_MOTION_DET_EN == 1)
			if (aeGainExpSetEn && (G_ExpGainSetPos == AE_EXP_GAIN_NOT_ACTION_FOR_YUV)) {
				idx = vdCount&0x03;

				if (idx == 2 && aeSyncStatus == AE_STATUS_PROC_TRIGGER) {
					aeSyncStatus = AE_STATUS_SET_EXPOSURE;
					aeIITrigApi();
					EGTaeIntrPutChar('@');
				}
				else if (idx == 3 && aeSyncStatus == AE_STATUS_SET_EXPOSURE) {
					aeSyncStatus = AE_STATUS_SET_GAIN;
					aeIntrProcApi();
					EGTaeIntrPutChar('#');
				}
				else if (idx == 0 && aeSyncStatus == AE_STATUS_SET_GAIN) {
					aeSyncStatus = AE_STATUS_EFFECT_FRAME;
					EGTaeIntrPutChar('%');
				}
				else if (idx == 1 && aeSyncStatus == AE_STATUS_EFFECT_FRAME) {
					aeSyncStatus = AE_STATUS_PROC_TRIGGER;
					EGTaeIntrPutChar('\n');
				}
			}
			#else
			if (aeGainExpSetEn && (G_ExpGainSetPos == AE_EXP_GAIN_SET_POS_VD_RISING)) {
				idx = vdCount&0x03;

				if (idx == 2 && aeSyncStatus == AE_STATUS_PROC_TRIGGER) {
					aeSyncStatus = AE_STATUS_SET_EXPOSURE;
					aeIITrigApi();
					EGTaeIntrPutChar('@');
				}
				else if (idx == 3 && aeSyncStatus == AE_STATUS_SET_EXPOSURE) {
					aeSyncStatus = AE_STATUS_SET_GAIN;
					aeCostGpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
					aeIntrProcApi();
					aeCostGpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
					aeExposureGainSet(vdCount);
					EGTaeIntrPutChar('#');
				}
				else if (idx == 0 && aeSyncStatus == AE_STATUS_SET_GAIN) {
					aeSyncStatus = AE_STATUS_EFFECT_FRAME;
					aeExposureGainSet(vdCount);
					EGTaeIntrPutChar('%');
				}
				else if (idx == 1 && aeSyncStatus == AE_STATUS_EFFECT_FRAME) {
					aeSyncStatus = AE_STATUS_PROC_TRIGGER;
					EGTaeIntrPutChar('\n');
				}
			}
			awbGainUpdateSyncVdIntr();
			#endif
			if(vdProcEn & VD_PROC_DIST){
				//distIntr();
			}
		}
		else if ( (XBYTE[REG_Front_Finten_low] & 0x20) &&   //VVALID rising edge interrupt enable
					(XBYTE[0x2AC0] & 0x20) ) {	//VVALID rising edge interrupt event

			//XBYTE[0x2AC0] = 0x20;	/* clear interrupt event. */
			XBYTE[0x2AC0] = 0x60;	/* clear VVALID rising and VVALID falling interrupt event, when VVALID rising and VVALID falling event trigger at the same time */

			aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x30);	/* status 3 */

			if ( intrSmoothZoomCtrl.zoomFactUpdate == INTR_SMOOTH_ZOOM_MACHINE_STATUS_W_REG_FINISH_NEXT_FRAME ) {
				intrSmoothZoomCtrl.zoomFactUpdate = INTR_SMOOTH_ZOOM_MACHINE_STATUS_STOP;
				//intrPutChar('S');
			}

			if ( intrSmoothZoomCtrl.zoomFactUpdate == INTR_SMOOTH_ZOOM_MACHINE_STATUS_W_REG_FINISH ) {
				if (intrSmoothZoomCtrl.frontGateDisable == INTR_SMOOTH_SENSOR_ZOOM_FLOW_GATING_DISABLE) {
					INTR_HQDZoomFrontGateDisableSet();
					intrSmoothZoomCtrl.frontGateDisable = INTR_SMOOTH_SENSOR_ZOOM_FLOW_END;
				}
				else if (intrSmoothZoomCtrl.frontGateDisable == INTR_SMOOTH_SENSOR_ZOOM_FLOW_START) {
					/* next frame status */
					intrSmoothZoomCtrl.frontGateDisable = INTR_SMOOTH_SENSOR_ZOOM_FLOW_GATING_DISABLE;
				}
			}

			/* check seek msg */
			INTR_SmoothZoomStopCheck();

			#if (II_AE_MOTION_DET_EN == 1)
			if (aeGainExpSetEn && (G_ExpGainSetPos == AE_EXP_GAIN_NOT_ACTION_FOR_YUV)) {
				idx = vdCount&0x03;

				if (idx == 2 && aeSyncStatus == AE_STATUS_PROC_TRIGGER) {
					aeSyncStatus = AE_STATUS_SET_EXPOSURE;
					aeIITrigApi();
					EGTaeIntrPutChar('@');
				}
				else if (idx == 3 && aeSyncStatus == AE_STATUS_SET_EXPOSURE) {
					aeSyncStatus = AE_STATUS_SET_GAIN;
					aeIntrProcApi();
					EGTaeIntrPutChar('#');
				}
				else if (idx == 0 && aeSyncStatus == AE_STATUS_SET_GAIN) {
					aeSyncStatus = AE_STATUS_EFFECT_FRAME;
					EGTaeIntrPutChar('%');
				}
				else if (idx == 1 && aeSyncStatus == AE_STATUS_EFFECT_FRAME) {
					aeSyncStatus = AE_STATUS_PROC_TRIGGER;
					EGTaeIntrPutChar('\n');
				}
			}
			#else
			if (aeGainExpSetEn && (G_ExpGainSetPos == AE_EXP_GAIN_SET_POS_VVALID_RISING)) {
				idx = vdCount&0x03;

				if (idx == 2 && aeSyncStatus == AE_STATUS_PROC_TRIGGER) {
					aeSyncStatus = AE_STATUS_SET_EXPOSURE;
					aeIITrigApi();
					EGTaeIntrPutChar('@');
				}
 				else if (idx == 3 && aeSyncStatus == AE_STATUS_SET_EXPOSURE) {
 					aeSyncStatus = AE_STATUS_SET_GAIN;
					aeCostGpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
					aeIntrProcApi();
					aeCostGpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);

					INTR_PvDynamicTempoalDenoiseCheck();

					aeExposureGainSet(vdCount);
					EGTaeIntrPutChar('#');
				}
				else if (idx == 0 && aeSyncStatus == AE_STATUS_SET_GAIN) {
					aeSyncStatus = AE_STATUS_EFFECT_FRAME;
					aeExposureGainSet(vdCount);
					EGTaeIntrPutChar('%');
				}
				else if (idx == 1 && aeSyncStatus == AE_STATUS_EFFECT_FRAME) {
					aeSyncStatus = AE_STATUS_PROC_TRIGGER;
					EGTaeIntrPutChar('\n');
				}
			}
			#endif

			#if (AF_FIXTURE_EN == 1)
			afIntrWinValueGet();
			#endif

		}
		else if ( (XBYTE[REG_Front_Finten_low] & 0x40) &&   //VVALID falling edge interrupt enable
					(XBYTE[0x2AC0] & 0x40) ) {	//VVALID falling edge interrupt event

			//UINT16 addr;
			XBYTE[0x2AC0] = 0x40;	/* clear interrupt event. */

			aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x20);	/* status 2 */

/*			#if (VIDEO_FRONT_CDSP_JPG_DBG == 1)
			if (videoStatisticsEn) {
				videoFrontInputCnt++;
			}
			#endif
*/
			/*
			if ((totalFrameCnt>0)&&(totalFrameCnt<1000)){

				if (totalFrameCnt%4==0){
					XBYTE[0x227a]+=4;
				}
			}
			*/

			#if 0
			if ((XBYTE[REG_Cdsp_evnt1]&0x80)==0x80){  /* It would cause snap issue, needs to be checked */
				XBYTE[REG_Cdsp_pSkipwr]=0x02;
				XBYTE[REG_Cdsp_pSkipwr]&=0xfd;
				XBYTE[REG_Cdsp_evnt1]=0x80;
				//HAL_GlobalReadGTimer( &awbTime );
				//pt = (UINT32 xdata *)G_FPGAMemPool;
				//if(awbErrCnt<500){
				//	*(pt+awbErrCnt)=awbTime;
				//	awbErrCnt++;
				//}
			}
			#endif

			INTR_PvDynamicTempoalDenoiseGateSet();

			if (aeSyncStatus == AE_STATUS_EFFECT_FRAME) {
				INTR_PvDynamicTempoalDenoisePclkSet();

				INTR_PvDynamicTempoalDenoiseRegSet();
			}

			INTR_PvDynamicEnhanCheck();

			if(vdProcEn & VD_PROC_DIST){
				distIntr();
			}
		}
	}

//1 AUDIO

	else if ((IntSts & INTR_SRC_TYPE_AUD) == INTR_SRC_TYPE_AUD){
	/* alc , enable: XBYTE[0x2609] */
		if (((XBYTE[REG_Audio_SarAdcIntsts] & 0x01) == 0x01) && //TP interrupt event
			((XBYTE[REG_Audio_SarAdcIntEn] & 0x01) == 0x01))	//TP interrupt enable
		{
			XBYTE[REG_Audio_SarAdcIntsts] = 0x01;

			//HAL_GlobalTimerWait(5);
			//XBYTE[REG_Audio_SarAdcIntsts]=0x01;//clear tp evt intr  state
			intrPutChar('e');
			intrPutChar('\n');
			HAL_TpIntrEvt();//G_penstate=pendown

		}
	      /*for sar  delay  int*/
		else if (((XBYTE[REG_Audio_SarAdcIntsts] & 0x04) == 0x04) &&
			((XBYTE[REG_Audio_SarAdcIntEn] & 0x04) == 0x04))
		{
			XBYTE[REG_Audio_SarAdcIntsts] = 0x04;

			XBYTE[0x260b]&=0xfb;//disable sar delay int
			intrPutChar('s');
			intrPutChar('\n');
		}
		/*for tp  pen down */
		else if (((XBYTE[REG_Audio_SarAdcIntsts] & 0x08) == 0x08) &&
				((XBYTE[REG_Audio_SarAdcIntEn] & 0x08) == 0x08))
			{
				XBYTE[REG_Audio_SarAdcIntsts] = 0x08;

				XBYTE[0x260b]&=0xf7;//disable pendnint
				XBYTE[0x2648]&=0xfd;//close tp_sw

				G_tpPenState=TP_PENDOWN;
				sp1kBtnDisableAllIntr();
				intrPutChar('d');
				intrPutChar('\n');
			}
			/*for tp  pen up */
			else if (((XBYTE[REG_Audio_SarAdcIntsts] & 0x10) == 0x10) &&
				((XBYTE[REG_Audio_SarAdcIntEn] & 0x10) == 0x10))
			{
				XBYTE[REG_Audio_SarAdcIntsts] = 0x10;
				XBYTE[0x260b]&=0xef;//disable penupint
				G_tpPenState=TP_PENUP;
				sp1kBtnEnableAllIntr();
				intrPutChar('u');
				intrPutChar('\n');
			}


		#if 1 /* mark for 1628 FPGA */
		else if (((XBYTE[REG_Audio_VolumeIntSts]&0x04) == 0x04) && //audio frame interrupt event
			((XBYTE[REG_Audio_VolumeIntEn]&0x04)==0x04)) //audio frame interrupt enable
		{
			UINT32 temp,audbuffaddr = K_SDRAM_AviCapAudioBufAddr;
			UINT8 i,winnum = 16;
			XBYTE[REG_Audio_VolumeIntSts] = 0x04; //clear audio fram interrrupt sts

			if(HAL_GlobalICVerGet_intr()==0x00) {
				if ((XBYTE[REG_Moe_CmdStatus]&0x01) == 1)//HAL_MoeIsBusy(1) == 0x00 )
				{
					if(G_Sound_Ctrl) {
						winnum = 2;
						audbuffaddr = K_SDRAM_SoundCtrlAddr;
					}
					frame_count = XBYTE[REG_Audio_FrameCnt];
					i = frame_count-1;
					while( i>winnum-1 ) {
						i= (i-winnum);
					}
					//when window_size modify,this follow value(i<<11) should be modify synchronously!!!
					temp = (UINT32)(i*G_WindowSize);//  i*window_size,

					//AgcvalGet_Intr(((audRecCtrl.AudioAddr)+temp), K_SDRAM_AlcMoeCommandBufAddr, &audio_curr_energy);
					HAL_AudAgcValGet((audbuffaddr+temp),sysMemAddrGet_Intr(MEM_IDX_ALC_MOE_CMD),HAL_AUD_SAMPLE_16BIT,&audio_curr_energy);
					//AgcvalGet(((audRecCtrl.AudioAddr)+temp),0x118b88UL+100030UL,0x0EA6,0x118b88UL+100000UL,1,&audio_curr_energy);
					//CPrintf3("agcengy:%u,scr=%lx,dst=%lx",audio_curr_energy,(K_SDRAM_AviCapAudioBufAddr+temp),sysMemAddrGet_Intr(MEM_IDX_ALC_MOE_CMD));
					audioAlcCal_Intr();
				}
			}
			else {
				HAL_AudAgcValGet((K_SDRAM_AviCapAudioBufAddr+temp),sysMemAddrGet_Intr(MEM_IDX_ALC_MOE_CMD),HAL_AUD_SAMPLE_16BIT,&audio_curr_energy);
				audioAlcCal_Intr();
			}

		}
		//sunyong add for shutter sound begin
		#if 1  //marked for FPGA test alc  tq@20100311
		else if(((XBYTE[REG_Audio_DataThdIntSts] & 0x02) == 0x02) && ((XBYTE[REG_Audio_DataThdIntEn] & 0x02 ) == 0x02))
		{
			XBYTE[REG_Audio_DataThdIntSts] |= 0x02;  /* clear audlessintsts. */
			XBYTE[REG_Audio_DataThdIntEn] &= ~0x02;  /* disable audlessinten. */
			//XBYTE[REG_Audio_AudEn] &= ~0x02;  /* stop audply. */
			XBYTE[REG_Audio_AudPause] = 0x01; /*puse audply*/
			#if 0  /* [0035142] mattwang remove. */
			HAL_GpioByteOutSet(HAL_GPIO_BYTE_GEN1, 0x10, 0x00);	//pull low gpio12
			#else
			//HAL_GpioByteOutSet_Intr(gpioByteIdSpkEn, gpioBitMskSpkEn, 0x00);	//pull low gpio12
			#endif
			//printf("2608:%bx\n", XBYTE[REG_Audio_DataThdIntEn] );
		}
             #endif
		#endif
		//sunyong add for shutter sound end
		/*
		if (((XBYTE[0x2Ad0] & 0x07) == 0x07) && //Inthnum0
		    ((XBYTE[0x2Ac0] & 0x07) == 0x07))
		{
			//printf("VD rising enter\n");
			XBYTE[0x2030]|=0x08;
			XBYTE[0x2Ac0] = 0x07;
			XBYTE[0x2030]&=(~0x08);
		}*/
	}

//1 TV ENCODE
#if 0
	else if ((IntSts & INTR_SRC_TYPE_TVENC) == INTR_SRC_TYPE_TVENC){
		if((XBYTE[REG_Disp_VsVdIntSts]&0x08) && (XBYTE[REG_Disp_VsVdIntEn]&0x08) && (HAL_GlobalICVerGet_intr()!=0x01))
		{
			if(!((XBYTE[REG_Disp_DispSignal]&0x01)||(XBYTE[REG_Disp_DispSignal]&0x02)))
			{
				XBYTE[REG_Disp_VsVdIntSts] |= 0x08;
				if( ((pkgChipIdGet_Intr()==0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalICVerGet_intr()==0x00) )
				{
					XBYTE[REG_Disp_GpioSel0] = 0xFF;
					XBYTE[REG_Disp_GpioSel0] = 0xFF;
					G_DgpioValue = XBYTE[REG_Disp_GpioInVal0];
					G_DgpioValue = XBYTE[REG_Disp_GpioInVal0];//for delay
					XBYTE[REG_Disp_GpioSel0] = 0x00;
				}
			}
		}
	}
#endif
//1 FM GPIO

	else if ((IntSts & INTR_SRC_TYPE_FMGPIO) == INTR_SRC_TYPE_FMGPIO){
		if((XBYTE[REG_Fm_evt] & (0x04)) && (XBYTE[REG_Fm_intEn] & (0x04))) {
			XBYTE[REG_Fm_evt] |= (0x04);
			//intrPutChar('S');
			//intrPutChar('\n');
			//INTR_SDBgRw();
			bgRwTrig();
		}
	}

//1 GLOBAL

	else if ((IntSts & INTR_SRC_TYPE_GLOBAL) == INTR_SRC_TYPE_GLOBAL){
#if 1 /* mark for 1628 FPGA */
	/* TP interrupt */
		UINT8 val;

		
		//if (pkgChipIdGet_Intr() != 0x1527){
				//	 val = 0x01 ; //GPIO8
				//} else {
					 val = 0x08;  //GPIO3
				//}
		
		#if 0
				if (((XBYTE[0x207B] & val) == val) && //GPIO8 interrupt event
						((XBYTE[0x2045] & val) == val)) //GPIO8 interrupt enable
		#else
				if (((XBYTE[0x207A] & val) == val) && //GPIO3 interrupt event
						((XBYTE[0x2044] & val) == val)) //GPIO3 interrupt enable
		#endif
				{
			#if 0
					XBYTE[0x207B] &= (~val);
			#else
					XBYTE[0x207A] &= (~val);
			#endif
					HAL_UsbSwPlugOut_Intr();
					G_UI_USBConnect = K_UI_USB_DISCONNECT;
					G_ucBulkPhase = K_CommandPhase;
					AdapterCharge = 1; //recover default value!
					XBYTE[REG_Usb_BufClr] = 0xFF;
					INTR_putchar('G');
				}
		
#endif
		if (((XBYTE[0x20C0] & 0x02) == 0x02) && //Global timer0 interrupt event
				((XBYTE[0x20D0] & 0x02) == 0x02))	//Global timer interrupt enable
		{
			//XBYTE[0x20C0] &= 0xFD; //timer 0 for WDT used
			Gt0Cnt++;
		}
		else if (((XBYTE[0x20C0] & 0x04) == 0x04) && //Global timer1 interrupt event
				((XBYTE[0x20D0] & 0x04) == 0x04))	//Global timer interrupt enable
		{
			XBYTE[0x20C0] &= 0xFB;
			Gt1Cnt++;
		}
		else if (((XBYTE[0x20C0] & 0x08) == 0x08) && //Global timer2 interrupt event
				((XBYTE[0x20D0] & 0x08) == 0x08))	//Global timer interrupt enable
		{
			XBYTE[0x20C0] &= 0xF7;
			HAL_GlobalTimer2Handle_intr();
		}
		else if(( (XBYTE[0x20d0] &0x20)==0x20)){
			videoRecRtcIntr();

			  //INTR_putchar('H');
			#if 0
		  	UINT8 first0, first1,first2,first3,first4,first5;
		  	UINT32 _hight,time;
		 	UINT16 _low;
		   	HAL_GlobalWriteRTCData(0xc0, 0); /*clear alarm*/
			HAL_GlobalWriteRTCData(0xB1, 0x01);
			HAL_GlobalTimerWait10us(5);
			HAL_GlobalReadRTCData(0x25, &first5);
			HAL_GlobalReadRTCData(0x24, &first4);
			HAL_GlobalReadRTCData(0x23, &first3);
			HAL_GlobalReadRTCData(0x22, &first2);
			HAL_GlobalReadRTCData(0x21, &first1);
			HAL_GlobalReadRTCData(0x20, &first0);
			WRITE8(_low,0,first1);
			WRITE8(_low,1,first0);
			WRITE8(_hight,0,first5);
			WRITE8(_hight,1,first4);
			WRITE8(_hight,2,first3);
			WRITE8(_hight,3,first2);

			 if(_low >=(65536 - tvTime) ){
			    _hight += 1;
			 }
		    _low += tvTime;
		    first0 = READ8(_low,1);
		    first1 = READ8(_low,0);
		    first2 = READ8(_hight,3);
		    first3 = READ8(_hight,2);
		    first4 = READ8(_hight,1);
		    first5= READ8(_hight,0);
			HAL_GlobalWriteRTCData(0x20, first0);
			HAL_GlobalWriteRTCData(0x21, first1);
			HAL_GlobalWriteRTCData(0x22, first2);
			HAL_GlobalWriteRTCData(0x23, first3);
			HAL_GlobalWriteRTCData(0x24, first4);
			HAL_GlobalWriteRTCData(0x25, first5);
			#endif
		}
	}

//1 FD
	else if ((IntSts & INTR_SRC_TYPE_FD) == INTR_SRC_TYPE_FD) {
		//printf("FD interrupt enter \n");
		//INTR_putchar('~');
		if ((XBYTE[REG_FD_Intr] & 0x01) && XBYTE[REG_FD_IntrEnable] & 0x01)	{
			#if (FD_ENABLE == 1)
			XBYTE[REG_FD_Intr] |= 0x03;
			fdIntr();
			#endif
		}else if((XBYTE[REG_II_Intr] & 0x01)  && (XBYTE[REG_II_IntrEnable] & 0x01) ){
			XBYTE[REG_II_Intr] |= 0x03;
			//INTR_putchar('^');
			if (iiDbgTrig == II_Trigger_Ae) {
				aeMoeTrigApi();
			}
			#if (FD_ENABLE == 1)
			else if (iiDbgTrig == II_Trigger_Fd) {
				fdIntr();
			}
			#endif
		}
		else
		{
			XBYTE[REG_II_Intr] = 0xff;
			#if (FD_ENABLE == 1)
			XBYTE[REG_FD_Intr] = 0xff;
			#endif
		}
	}
#if MOE_INTR_PROC_Enable
	else if(XBYTE[REG_Moe_Intr] & XBYTE[REG_Moe_IntrEnable])
	{
		//printf("-moe intr-%bx,en:%bx\n",XBYTE[0x2e00],XBYTE[0x2e01]);
		XBYTE[REG_Moe_Intr]=0xff;
	}
#endif

INTR_EXIT:
#endif //TAG_TODO
#if INTR_TIMR_DECTED
	WRITE8(tt1, 0, XBYTE[0x20C4]);
	WRITE8(tt1, 1, XBYTE[0x20C3]);
	WRITE8(tt1, 2, XBYTE[0x20C2]);
	WRITE8(tt1, 3, XBYTE[0x20C1]);
	if(tt1>tt0){
		tt1-=tt0;
		if(intrTime<tt1)
			intrTime=tt1;
	}
#endif
	INTR_Leave();
	INTR_EnableInterrupt();
}

//-----------------------------------------------------------------------------
//INTR_Enter
//-----------------------------------------------------------------------------
void INTR_Enter(void) USING_1
/*++

Routine Description:

        pre-processing in service routine of interrupt 0

Arguments:

        none

Return Value:

        none

--*/
{
	if(!(XBYTE[REG_Disp_DispSignal]&0x01))
	{
		if( ((pkgChipIdGet()== 0x1627)||(pkgChipIdGet()== 0x1626))&&(HAL_GlobalICVerGet_intr()==0x00) )
		{
			XBYTE[REG_Disp_GpioSel0] = 0xFF;
			XBYTE[REG_Disp_GpioSel0] = 0xFF;
			G_DgpioValue = XBYTE[REG_Disp_GpioInVal0];
			G_DgpioValue = XBYTE[REG_Disp_GpioInVal0];
			XBYTE[REG_Disp_GpioSel0] = 0x00;
		}
	}
}

//-----------------------------------------------------------------------------
//INTR_Leave
//-----------------------------------------------------------------------------
void INTR_Leave(void) USING_1
/*++

Routine Description:

        post-processing in service routine of interrupt 0

Arguments:

        none

Return Value:

        none

--*/
{
//        POWER_UsbConnectStatusChangeInt();
}

//-----------------------------------------------------------------------------
//INTR_DisableInterrupt
//-----------------------------------------------------------------------------
void INTR_DisableInterrupt(void) USING_1
/*++

Routine Description:

        disable relative trigger of interrupt 0

Arguments:

        none

Return Value:

        none

--*/
{
	EX0 = 0;

	WRITE8(IntSts, 0, XBYTE[REG_Cpu_IntrSts1]);
	WRITE8(IntSts, 1, XBYTE[REG_Cpu_IntrSts0]);

	if ((IntSts & INTR_SRC_TYPE_USB) == INTR_SRC_TYPE_USB){
		XBYTE[REG_Usb_IntEn] &= 0x9f;          //disable change event interrupt of
		                        //      video interface alternate setting
		                        //      audio interface alternate setting

		XBYTE[REG_Usb_TransactionEn] &= 0x01;//0x81;        //NAK of (automatic done by hardware)
		                        //      EP0 Out packet
		                        //      EP0 In packet
		                        //      Bulk In packet
		                        //      Bulk Out packet
		                        //      Int In packet
		                        //      EP7 Int In packet

		XBYTE[REG_Usb_EpAckIntEn] &= 0xb8;//0xf8;          //disable ACK interrupt of
		                        //      EP0 Setup packet OK
		                        //      EP0 Out packet OK
		                        //      EP0 In packet OK
		                        //      USB reset event
	}
}

//-----------------------------------------------------------------------------
//INTR_EnableInterrupt
//-----------------------------------------------------------------------------
void INTR_EnableInterrupt(void) USING_1
/*++

Routine Description:

        enable relative trigger of interrupt 0

Arguments:

        none

Return Value:

        none

--*/
{

#if !TAG_TODO
	if ((IntSts & 0x0011) == 0x0011) {
		XBYTE[REG_Usb_IntEn] |= 0x60;          //enable change event interrupt of
		                                //      video interface alternate setting
		                                //      audio interface alternate setting
/*
		if (XBYTE[REG_Usb_EpAckIntSts] & 0x01)
		{
			//printf("p");
			putchar('p');
			XBYTE[REG_Usb_EpAckIntSts] = 0x06;   //clear EP0InAck
		}
		else*/
		if (G_btEP0Intr == 1)
		{
			G_btEP0Intr = 0;
			if (G_ucCtrlPhase==K_OutDataPhase)
			{
				//printf("a");
				XBYTE[REG_Usb_TransactionEn] |= 0x04;
			}
			else if ((G_ucCtrlPhase == K_InDataPhase) || (G_ucCtrlPhase == K_InResponsePhase))
			{
				//printf("b");
		      		//XBYTE[REG_Usb_TransactionEn] &= 0xfb;  // Disable EP0 Out
			  	XBYTE[REG_Usb_TransactionEn] = 0x02;  // Enable Ep0 In
			}
			else if (G_ucCtrlPhase == K_OutResponsePhase)
			{
				//printf("c");
				#if 0
		   		XBYTE[REG_Usb_TransactionEn] &= 0xfd;  // Disable EP0 Out
			  	XBYTE[REG_Usb_TransactionEn] |= 0x04;  // Enable Ep0 Out
			  	#else
			  	XBYTE[REG_Usb_TransactionEn] = 0x04;  // Enable Ep0 Out
			  	#endif
			}
			else if (G_ucCtrlPhase == K_CommandPhase)
			{
				//printf("d");
				XBYTE[REG_Usb_TransactionEn] &= 0xf9;  // Disable EP0 IN/Out
		    	if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
			   		&& (gUsbMode == K_UIOPTION_STORAGE_MSDC ||
			   		gUsbMode == K_UIOPTION_STORAGE_SIDC ) )
				{
					XBYTE[REG_Usb_TransactionEn] = 0x10;
				}
			}
			else {
				//printf("e");
		    	if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
		   		&& (gUsbMode == K_UIOPTION_STORAGE_MSDC ||
		   		gUsbMode == K_UIOPTION_STORAGE_SIDC) )
				{
					XBYTE[REG_Usb_TransactionEn] |= 0x1e;
				}
				else
				{
					XBYTE[REG_Usb_TransactionEn] |= 0x06;
				}
		    }
		}

		if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
				&& (gUsbMode == K_UIOPTION_STORAGE_MSDC ||
		   		gUsbMode == K_UIOPTION_STORAGE_SIDC ) )
		{
			XBYTE[REG_Usb_EpAckIntEn] |= 0x1F;      //enable ACK interrupt of
		                                //      EP0 Setup packet OK
		                                //      EP0 Out packet OK
		                                //      EP0 In packet OK
			/* 20100802 shengkai added for USB EL-9 test */
			XBYTE[REG_Usb_StdCmdIntEn] |= 0x20;
		}
		else {
			XBYTE[REG_Usb_EpAckIntEn] |= 0x07;
			/* 20100802 shengkai added for USB EL-9 test */
			XBYTE[REG_Usb_StdCmdIntEn] |= 0x20;
		}

		if (G_btBulkIntr == 1)
		{
			//P1 = G_ucBulkPhase;
			//printf("BP1=%bx\n",G_ucBulkPhase);
		    switch (G_ucBulkPhase)
		    {
		  	    case K_CommandPhase:
			    case K_OutDataPhase:
		    	//case K_OutDataPhaseWrite:
		    	case K_OutResponsePhase:
					XBYTE[REG_Usb_BufClr] =	K_USB_CLASS_OUT_OPEN;
		        	XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_OUT_OPEN;	//0x06;   //ACK of
		            		                               	//      Bulk Out packet
		                                            		//      Int In packet
		            XBYTE[REG_Usb_EpAckIntEn] = 0x11;//0x06;   //enable ACK interrupt of
		                                           //      Bulk Out packet OK
		                                           //      Int In packet OK
		        	break;

		        case K_InDataPhase:
				//richie@mi 0409 mark
		        //case K_InDataPhaseRead:
		        case K_InResponsePhase:
		        case K_InCompletionPhase:
		            XBYTE[REG_Usb_TransactionEn] = K_USB_CLASS_IN_OPEN;//0x05;   //ACK of
		 	                                            //      Bulk In packet
		     	                                        //      Int In packet
		            XBYTE[REG_Usb_EpAckIntEn] = 0x08;//0x05;   //enable ACK interrupt of
		                                           //      Bulk In packet OK
		                                           //      Int In packet OK
		        	break;

		        case K_InDataPhaseRead:
		            XBYTE[REG_Usb_TransactionEn] &= 0xf9;//0x05;   //ACK of
					break;

		        case K_OutDataPhaseWrite:
			        break;

		        default:
		            break;
		    }
		    G_btBulkIntr = 0;
		}
	}
#endif //TAG_TODO
	EX0 = 1;
}

#define XON  0x11
#define XOFF 0x13
void INTR_putUINT32(UINT32 val)
{
	intrPutUint16((val)>>16);
	intrPutUint16((val)&0xffff);
}
/*
 * putchar (full version):  expands '\n' into CR LF and handles
 *                          XON/XOFF (Ctrl+S/Ctrl+Q) protocol
 */
char INTR_putchar (char c)
{
	if (c == '\n')  {
		if (RI)  {
			if (SBUF0 == XOFF)  {
				do  {
					RI = 0;
					while (!RI);
				} while (SBUF0 != XON);
				RI = 0;
			}
		}
		while (!TI);
		TI = 0;
		SBUF0 = 0x0d;                         /* output CR  */
	}
	if (RI)  {
		if (SBUF0 == XOFF)  {
			do  {
				RI = 0;
				while (!RI);
			} while (SBUF0 != XON);
			RI = 0;
		}
	}
	while (!TI);
	TI = 0;

	return (SBUF0 = c);
}

#if 0 /* mark for 1628 FPGA */
void
INTR_SensorExposureGain(
	void
)
{
	static UINT8 shutter=0;


	#if 1
	if((G_AEReady==1)&&(shutter==0)){
		G_AESetting = 1;
		//XBYTE[0x2030]|=0x01;
		/*do shutter setting;*/
		if(G_SHUTTER != PreShutter) {
			frontExposureTimeSet(0,G_SHUTTER,1,0);
			PreShutter=G_SHUTTER;
		}

		if(G_GainAfterExp == 0x01) {
			shutter=1;
			//XBYTE[0x2030]&=(~0x01);
		}
		else {
			if(PreGain != G_GAIN) {
				frontGainSet(0,G_GAIN,1,0);
				PreGain=G_GAIN;
			};

			shutter=0;
			G_AESetting = 0;
		}
		G_AEReady=0;
	}
	else if(shutter==1) {
		//XBYTE[0x2030]|=0x01;
		/*do gain setting;*/
		if((XBYTE[0x2AB8] & 0x08) == 0) {
			if(PreGain != G_GAIN) {
				frontGainSet(0,G_GAIN,1,0);
				PreGain=G_GAIN;
			};
			shutter=0;
			G_AESetting = 0;
		}
		else {
			G_3ACount = 0;
		};
		//XBYTE[0x2030]&=(~0x01);
	}
	#endif
}
#endif
/*
 * putchar (full version):  expands '\n' into CR LF and handles
 *                          XON/XOFF (Ctrl+S/Ctrl+Q) protocol
 */

/*kai++2009.02.26*/
 #ifdef  UART_FUNC
char putchar (char c)
{
	/*to do track UART enable flag+++*/
	if ( (Uartmode !=UART_MODE_DEBUG) &&
		((Uartmode !=UART_MODE_COMMUNICATE) || (UartSendEnable !=1)) )

		return c;
	/*to do track UART enable flag----*/
	else {
		if (c == '\n')  {
			if (RI)  {
				if (SBUF0 == XOFF)  {
					do  {
						RI = 0;
						while (!RI);
					} while (SBUF0 != XON);
					RI = 0;
				}
			}
			while (!TI);
			TI = 0;
			SBUF0 = 0x0d;                         /* output CR  */
		}
		if (RI)  {
			if (SBUF0 == XOFF)  {
				do  {
					RI = 0;
					while (!RI);
				} while (SBUF0 != XON);
				RI = 0;
			}
		}
		while (!TI);
		TI = 0;

		return (SBUF0 = c);
	}
}
 #endif

/**
* @fn 	   void INTR_PvZoom(void)
* @brief		 INTR stillview zoom flow
* @param	 NONE
* @retval  NONE
* @todo
* @author  LinJieCheng
* @since	 2010-10-07
*/
void
INTR_PvZoom(
	void
)
{
	UINT8 ZfStatus;

	ZfStatus = HAL_IntrDramZUpdateWait(HAL_DRAM_ZUPDATE_PV);

	if (ZfStatus == FAIL) {
		return;
	}
	else {

		if (pvZoomNBCtrl.zoomAction != 0) {

			INTR_ZoomZfRegSet(0);

			#if 0
			INTR_ZoomClampSet();
			#endif

			HAL_IntrDramZUpdateClear(0x30); /* clear CDSP/DISP zf flag */

			//if ((XBYTE[0x27B1]&0x08) == 0x08) {
			//  zfTgSkipFrameCnt++;
			//}

			HAL_IntrDramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);

			intrSmoothZoomCtrl.zoomFactUpdate = INTR_SMOOTH_ZOOM_MACHINE_STATUS_W_REG_FINISH;

			/* set next trigger time need wait frame number */
			if (intrSmoothZoomCtrl.zoomRealTimeUpdate == 1) {
				intrSmoothZoomCtrl.zoomFrameCnt = 1;
				intrSmoothZoomCtrl.zoomRealTimeUpdate = 0;
			}
			else {
				intrSmoothZoomCtrl.zoomFrameCnt = intrSmoothZoomCtrl.zoomFrameUpdateInterval;
			}

			//osMsgPost(SP1K_MSG_DZOOM_OSD_UPDATE);
			if ( FALSE == osMsgIntrSeek(SP1K_MSG_DZOOM_OSD_UPDATE) ) {
          		osMsgPost(SP1K_MSG_DZOOM_OSD_UPDATE);
       		 }
		}
		else {
		}

	}
}

/**
* @fn 	 void INTR_VideoZoom(void)
* @brief	 Smooth Zoom Zf Reg Set
* @param	 NONE
* @retval    NONE
* @todo	   NONE
* @author    LinJieCheng
* @since	 2007-12-18
*/
void
INTR_VideoZoom (
void
)
{
	UINT8 ZfStatus;
	UINT8 videoIdle;


	videoIdle = IntrvideoRecStatusGet();

	/* wait for zf done and write new factors to registers then*/
	if ( videoIdle != VID_REC_IDLE ) {
		ZfStatus = HAL_IntrDramZUpdateWait(HAL_DRAM_ZUPDATE_VIDEO);
	}
	else {
		ZfStatus = HAL_IntrDramZUpdateWait(HAL_DRAM_ZUPDATE_PV);
	}

	if ( ZfStatus == FAIL ) {
		return;
	}
	else {

		if (VideoZoomNBCtrl.zoomAction != 0) {

			INTR_ZoomZfRegSet(1);

			#if 0
			INTR_ZoomClampSet();
			#endif


			if(videoIdle != VID_REC_IDLE){
				HAL_IntrDramZUpdateClear(0xb0); /* clear CDSP/DISP/MJPG zf flag */
			}
			else{
				HAL_IntrDramZUpdateClear(0x30); /* clear CDSP/DISP zf flag */
			}

			//if ((XBYTE[0x27B1]&0x08) == 0x08) {
			//  zfTgSkipFrameCnt++;
			//}

			HAL_IntrDramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PV);

			intrSmoothZoomCtrl.zoomFactUpdate = INTR_SMOOTH_ZOOM_MACHINE_STATUS_W_REG_FINISH;

			/* set next trigger time need wait frame number */
			if (intrSmoothZoomCtrl.zoomRealTimeUpdate == 1) {
				intrSmoothZoomCtrl.zoomFrameCnt = 1;
				intrSmoothZoomCtrl.zoomRealTimeUpdate = 0;
			}
			else {
				intrSmoothZoomCtrl.zoomFrameCnt = intrSmoothZoomCtrl.zoomFrameUpdateInterval;
			}
			if ( FALSE == osMsgIntrSeek(SP1K_MSG_DZOOM_OSD_UPDATE) ) {
          		osMsgPost(SP1K_MSG_DZOOM_OSD_UPDATE);
       		 }
		}
		else {
		}

	}
}

/**
* @fn 	 void INTR_HQDZoomFrontGateDisableSet(void)
* @brief	 HQ Smooth Zoom front gating disable Set
* @param	 NONE
* @retval    NONE
* @todo	   NONE
* @author    LinJieCheng
* @since	 2007-12-18
*/
void
INTR_HQDZoomFrontGateDisableSet(
void
)
{
	SINT16 paraAry;

	paraAry = 0;
	HAL_IntrFrontParaFrontToCdspGatingEnableSet(&paraAry,1);
}

/**
* @fn 	 void INTR_ZoomUpdateSet(void)
* @brief	 Smooth Zoom Update Set
* @param	 NONE
* @retval    NONE
* @todo	   NONE
* @author    LinJieCheng
* @since	 2007-12-18
*/
void
INTR_ZoomUpdateSet(
	void
)
{

	UINT8 CurrState;

	CurrState = osSysModeGet();

	switch( CurrState ) {
		case OS_SYS_MODE_STILL_PV:
			INTR_PvZoom();
			break;
		case OS_SYS_MODE_VIDEO_PV:
		case OS_SYS_MODE_VIDEO_REC:
			INTR_VideoZoom();
			break;
	}

}

/**
* @fn		void INTR_ZoomZfRegSet(UINT8 videoMode)
* @brief   Smooth Zoom Zf Reg Set
* @param   [in]videoMode :
*						 0:still preview
*						 1:video preview
* @retval	  NONE
* @todo	  NONE
* @author	  LinJieCheng
* @since   2007-12-18
*/
void
INTR_ZoomZfRegSet(
	UINT8 videoMode
)
{
	UINT16 xdata* pRegIdx;
	UINT8 xdata* pRegVal;
	UINT32 offSet;
	UINT16 offSet_16,msgGrp;
	SINT16 idx;
	UINT8 QueueEntries;
	extern UINT8 dzoomBuf[];
	msgGrp = ((videoMode) ? ZOOM_CTRL_TYPE_VIDEO : ZOOM_CTRL_TYPE_STILL);

	INTR_zoomSmoothQueueEntriesGet(msgGrp, &QueueEntries);

	//intrPutChar('\n');
	//intrPutUint8(QueueEntries);

	if (QueueEntries != 0) {

		zoomSmoothQueueAccept(msgGrp, &offSet_16);

		//intrPutChar('-');
		//intrPutChar('>');
		//intrPutUint16(offSet_16);

		offSet = offSet_16;

		pRegIdx = (UINT16*)&dzoomBuf[0];
		pRegVal = &dzoomBuf[intrSmoothZoomCtrl.zoomRegSize + offSet];

		switch (msgGrp) {
			case ZOOM_CTRL_TYPE_STILL:
				while(1) {
					if (*pRegIdx == 0xffff) {
						break;
					}
					else if ( *pRegIdx == REG_Cdsp_Clamphsize_low ) {
						XBYTE[*pRegIdx] = *pRegVal;
						XBYTE[REG_Cdsp_3Dsnapsubwidth_low] = *pRegVal;
						WRITEP8(&intrSmoothZoomCtrl.zoomClampH,1,*pRegVal);
					}
					else if ( *pRegIdx == REG_Cdsp_Clamphsize_high) {
						XBYTE[*pRegIdx] = *pRegVal;
						XBYTE[REG_Cdsp_3Dsnapsubwidth_high] = *pRegVal;
						WRITEP8(&intrSmoothZoomCtrl.zoomClampH,0,*pRegVal);
					}
					else if ( *pRegIdx == REG_Cdsp_Clampvsize_low ) {
						XBYTE[*pRegIdx] = *pRegVal;
						WRITEP8(&intrSmoothZoomCtrl.zoomClampV,1,*pRegVal);
					}
					else if ( *pRegIdx == REG_Cdsp_Clampvsize_high ) {
						XBYTE[*pRegIdx] = *pRegVal;
						WRITEP8(&intrSmoothZoomCtrl.zoomClampV,0,*pRegVal);
					}
					else {
						//printf("0x%x=0x%bx\n",*pRegIdx,*pRegVal);
						XBYTE[*pRegIdx] = *pRegVal;
					}

					pRegIdx++;
					pRegVal++;
				}

				switch (pvZoomNBCtrl.zoomAction) {
					default:
						//INTR_ASSERT(0,0);
						break;
					case 1://dzoom in
						idx = pvZoomNBCtrl.curLog + pvZoomNBCtrl.logStep;
						if ( idx <= pvZoomNBCtrl.maxLog ) {
							pvZoomNBCtrl.curLog = (UINT16)idx;
						}
						else {
							pvZoomNBCtrl.curLog = pvZoomNBCtrl.maxLog;
						}
						break;
					case 2://dzoom out
						idx = pvZoomNBCtrl.curLog - pvZoomNBCtrl.logStep;

						if ( idx >= pvZoomNBCtrl.minLog ) {
							pvZoomNBCtrl.curLog = (UINT16)idx;
						}
						else {
							pvZoomNBCtrl.curLog = pvZoomNBCtrl.minLog;
						}
						break;
				}

				/* HQ Dzoom sensor mode switch front2cdsp gating enable */
				if (pvZoomNBCtrl.hQDzoomStep != 0) {
					if (((pvZoomNBCtrl.curLog >= pvZoomNBCtrl.hQDzoomStep) && (pvZoomNBCtrl.hQDzoomStatus == 0)) ||
						((pvZoomNBCtrl.curLog < pvZoomNBCtrl.hQDzoomStep) && (pvZoomNBCtrl.hQDzoomStatus == 1))){
						pvZoomNBCtrl.hQDzoomStatus ^= 0x01;
						frontIntrPreviewSensorZoomModeSet(frontIntrDevActGet(),((pvZoomNBCtrl.hQDzoomStatus)?pvZoomNBCtrl.hQDzoomFct:100));
						intrSmoothZoomCtrl.frontGateDisable = INTR_SMOOTH_SENSOR_ZOOM_FLOW_START;
					}
				}
				break;
			case ZOOM_CTRL_TYPE_VIDEO:
				while(1) {
					if (*pRegIdx == 0xffff) {
						break;
					}
					else if ( *pRegIdx == REG_Cdsp_Clamphsize_low ) {
						XBYTE[*pRegIdx] = *pRegVal;
						XBYTE[REG_Cdsp_3Dsnapsubwidth_low] = *pRegVal;
						WRITEP8(&intrSmoothZoomCtrl.zoomClampH,1,*pRegVal);
					}
					else if ( *pRegIdx == REG_Cdsp_Clamphsize_high) {
						XBYTE[*pRegIdx] = *pRegVal;
						XBYTE[REG_Cdsp_3Dsnapsubwidth_high] = *pRegVal;
						WRITEP8(&intrSmoothZoomCtrl.zoomClampH,0,*pRegVal);
					}
					else if ( *pRegIdx == REG_Cdsp_Clampvsize_low ) {
						XBYTE[*pRegIdx] = *pRegVal;
						WRITEP8(&intrSmoothZoomCtrl.zoomClampV,1,*pRegVal);
					}
					else if ( *pRegIdx == REG_Cdsp_Clampvsize_high ) {
						XBYTE[*pRegIdx] = *pRegVal;
						WRITEP8(&intrSmoothZoomCtrl.zoomClampV,0,*pRegVal);
					}
					else {
						//printf("0x%x=0x%bx\n",*pRegIdx,*pRegVal);
						XBYTE[*pRegIdx] = *pRegVal;
					}

					pRegIdx++;
					pRegVal++;
				}

				switch (VideoZoomNBCtrl.zoomAction) {
					default:
						//INTR_ASSERT(0,0);
						break;
					case 1://dzoom in
						idx = VideoZoomNBCtrl.curLog + VideoZoomNBCtrl.logStep;
						if ( idx <= VideoZoomNBCtrl.maxLog ) {
							VideoZoomNBCtrl.curLog = (UINT16)idx;
						}
						else {
							VideoZoomNBCtrl.curLog = VideoZoomNBCtrl.maxLog;
						}
						break;
					case 2://dzoom out
						idx = VideoZoomNBCtrl.curLog - VideoZoomNBCtrl.logStep;

						if ( idx >= VideoZoomNBCtrl.minLog ) {
							VideoZoomNBCtrl.curLog = (UINT16)idx;
						}
						else {
							VideoZoomNBCtrl.curLog = VideoZoomNBCtrl.minLog;
						}
						break;
				}

				/* HQ Dzoom sensor mode switch front2cdsp gating enable */
				if (VideoZoomNBCtrl.hQDzoomStep != 0) {
					if (((VideoZoomNBCtrl.curLog >= VideoZoomNBCtrl.hQDzoomStep) && (VideoZoomNBCtrl.hQDzoomStatus == 0)) ||
						((VideoZoomNBCtrl.curLog < VideoZoomNBCtrl.hQDzoomStep) && (VideoZoomNBCtrl.hQDzoomStatus == 1))){
						VideoZoomNBCtrl.hQDzoomStatus ^= 0x01;
						frontIntrPreviewSensorZoomModeSet(frontIntrDevActGet(),((VideoZoomNBCtrl.hQDzoomStatus)?VideoZoomNBCtrl.hQDzoomFct:100));
						intrSmoothZoomCtrl.frontGateDisable = INTR_SMOOTH_SENSOR_ZOOM_FLOW_START;
					}
				}
				break;
		}
	}

	//intrPutChar('\n');

}

/**
* @fn		void INTR_SmoothZoomStopCheck(void)
* @brief   Smooth Zoom Stop Check
* @param   NONE
* @retval	  NONE
* @todo	  NONE
* @author	  LinJieCheng
* @since   2007-12-18
*/
void
INTR_SmoothZoomStopCheck(
	void
)
{
	UINT8 CurrState,msgGrp,QueNum;

	/* check seek msg */
	if ( intrSmoothZoomCtrl.zoomSmoothEn == INTR_SMOOTH_ZOOM_CTRL_STATUS_ENABLE ) {

		CurrState = osSysModeGet();

		switch( CurrState ) {
			case OS_SYS_MODE_STILL_PV:
				msgGrp = ZOOM_CTRL_TYPE_STILL;
				break;
			case OS_SYS_MODE_VIDEO_PV:
			case OS_SYS_MODE_VIDEO_REC:
				msgGrp = ZOOM_CTRL_TYPE_VIDEO;
				break;
		}

		INTR_zoomSmoothQueueEntriesGet(msgGrp, &QueNum);

		if ( intrSmoothZoomCtrl.zoomReleaseKeySts == INTR_SMOOTH_ZOOM_RELEASE_KEY ) {
			//intrPutChar('#');
			if (intrSmoothZoomCtrl.zoomFactUpdate == INTR_SMOOTH_ZOOM_MACHINE_STATUS_STOP) {
				intrSmoothZoomCtrl.zoomSmoothEn = INTR_SMOOTH_ZOOM_CTRL_STATUS_DISABLE;
				intrSmoothZoomCtrl.zoomReleaseKeySts = INTR_SMOOTH_ZOOM_NOT_RELEASE_KEY;
				//intrPutChar('@');
			}
		}
		else {
			if ((intrSmoothZoomCtrl.zoomFactUpdate == INTR_SMOOTH_ZOOM_MACHINE_STATUS_STOP) && (QueNum != 0)) {
				intrSmoothZoomCtrl.zoomFactUpdate = INTR_SMOOTH_ZOOM_MACHINE_STATUS_START;
				//intrPutChar('$');
			}
		}
	}
	else {

		if (intrSmoothZoomCtrl.zoom3AUpdateDlyFrame != 0) {
			intrSmoothZoomCtrl.zoom3AUpdateDlyFrame--;
			if (intrSmoothZoomCtrl.zoom3AUpdateDlyFrame == 0) {

				CurrState = osSysModeGet();

				switch( CurrState ) {
					case OS_SYS_MODE_STILL_PV:
						pvZoomNBCtrl.zoomAction = 0;
						break;
					case OS_SYS_MODE_VIDEO_PV:
					case OS_SYS_MODE_VIDEO_REC:
						pvZoomNBCtrl.zoomAction = 0;
						break;
				}
				//osMsgFlush(MSG_CTRL_FLUSH_ALL);
				if ( FALSE == osMsgIntrSeek(SP1K_MSG_DZOOM_OSD_UPDATE) ) {
          			osMsgPost(SP1K_MSG_DZOOM_OSD_UPDATE);
       			 }
				//osMsgPost(SP1K_MSG_DZOOM_OSD_UPDATE);
				//osMsgPostForce(SP1K_MSG_DZOOM_3A_UPDATE);
				osMsgPost(SP1K_MSG_DZOOM_3A_UPDATE);
			}
		}
	}
}

