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
 * @file      ICat_stub.c
 * @brief     
 * @author    Xianxing
 * @since     2010-01-01
 * @date      2010-01-01
*/



/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "icat_stub.h"

#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)

#include "vndreq.h"
#include "main.h"
#include "initio.h"
#include "front_logi.h" 
#include "asicreg.h"	 
#include "sp1k_snap_api.h"
#include "sp1k_rsvblk_api.h"
#include "awb_api.h"//wangjg++
#include "smcfunc.h"	 
#include "Setmode.h"    //will add
#include "hal.h"
#include "hal_usb.h"
#include "hal_cdsp_iq.h"
#include "hal_gprm.h"
#include "snap_task.h"
#include "osd_mem.h"
#include "ae_api.h"
#include "cdsp_misc.h"
#include "cdsp_task.h"
#include "hal_usb.h"
#include "sp1k_hal_api.h" 
#include "dbg_wdt.h"
#include "icat_debug.h"
#include "front_logi.h"
#include "uiflow.h"
#include "snap_task.h"
#include "rsc_id.h"
#include "sp1k_ae_api.h"
#include "utility.h"
#include "reg_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_SNAP_ICATSTUB__
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
xdata UINT16 IcatVCIndex = 0;

//xian ++
xdata UINT16 iCatFlowMode = 0;
xdata UINT8 iCatNextJob = 0;
	
xdata UINT32 iCatRawFileSize = 0; // do nothing if zero

xdata UINT16 iCatResFileNum;
xdata UINT16 iCatResFileId;
xdata UINT32 iCatResFileSize = 0;
	
xdata UINT32 iCatBulkSize = 0;
//xian --
xdata UINT8 sp1kSnapIcatBulkTrig = 0;
xdata UINT8 sp1kSnapIcatBulkEnd = 0;


xdata UINT8 sp1kSnapIcatMode=0;//eric
xdata UINT8 sp1kSnapIcatState = 0;
xdata UINT8 sp1kSnapIcatGo = 0;
xdata UINT8 sp1kSnapIcatCDSPParam = 0;
//xdata UINT32 sp1kSnapIcatCdspParamAddr;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdata UINT32 snapRawDataAddr;

extern xdata UINT16 snapRawHSize;
extern xdata UINT16 snapRawVSize;
extern xdata UINT16 snapSensorCropHSize;
extern xdata UINT16 snapSensorCropVSize;
extern xdata UINT16 snapYuvHSize;
extern xdata UINT16 snapYuvVSize;
extern xdata UINT16 snapHSize;
extern xdata UINT16 snapVSize;

extern xdata UINT8 G_AEReady;
extern xdata UINT8 G_AESetting;

extern xdata UINT32 G_BulkSize;
extern xdata UINT32 G_BulkDRAMAddr;
extern xdata UINT32 G_BulkFlashType;
extern xdata UINT32 G_BulkSRAMAddr;
extern xdata UINT32	G_LastBulkSize;

extern xdata UINT8 G_UI_USBConnect; 
extern xdata UINT8 G_ucPccamTuningFlag;
extern xdata UINT16 G_wValueLock;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void 
icatVndCmdStub_Intr( 
	UINT16 idx, 
	UINT16 val, 
	UINT16 bufSz, 
	UINT8  *buf 
)
{
	#define SetValue16 icatSetValue16_Intr
	#define GetValue16 icatGetValue16_Intr
	#define SetValue32 icatSetValue32_Intr
	#define GetValue32 icatGetValue32_Intr

	UINT8 bValidCmd = 1;
	//UINT16 snapW, snapH;
    UINT32 fileSize;

    bufSz = bufSz; // for compile warning

//	Stub stub = 0;
	IcatVCIndex = idx;

//	if(idx != 0x601 && idx != 0x603 && idx != 0x602 && idx != 0x604 && idx != 0x600)	//xian ++ 20080521, add 0x604
//		printf( "--f: VndCmdStub 0x%x, 0x%x, 0x%x, 0x%bx\n", idx, val, bufSz, *buf);

	//icatPrintf("cmd=%x\n",idx);

	switch ( idx ){
	
//--------------------- Status Command ---------------------
		case ICAT_MODE_SWITCH_IDX: 	//mode sw
			icatPrintfCmdFlow("ICat mode switch \n");
			SetValue16(buf, SUCCESS);

			sp1kSnapIcatMode = 1;
			sp1kSnapIcatState = 0;
			break;

		case ICAT_GET_STATUS_IDX: 	//get status
			icatPrintfCmdFlow("ICat Mode=0x%bx, Sts=0x%bx\n", sp1kSnapIcatMode, sp1kSnapIcatState);
			if(sp1kSnapIcatMode == 1){
				SetValue16(buf, SUCCESS);
				SetValue16(buf+2, sp1kSnapIcatState);
			}
			else{
				SetValue16(buf, -1);
			}
			break;
//--------------------- Capture Command ---------------------
		case ICAT_CAP_EXPOSURE_IDX:	//set exposure
			icatPrintfCmdFlow("ICat Set exp = 0x%lx\n", GetValue32(buf));
			frontIntrExposureTimeSet(0, GetValue32(buf), 1, 0);
			break;

		case ICAT_CAP_GAIN_IDX:	//set gain
			icatPrintfCmdFlow("ICat Set gain = 0x%lx\n", GetValue32(buf));
			frontIntrGainSet(0, GetValue32(buf), 1, 0);
			break;
/*
		case 0x0102:	//start capture
			icatPrintf("ICat Start capture...\n");
			G_bRequestLock = 0;
			sp1kSnapTask();
			break;

		case 0x0103:	//stop capture
			icatPrintf("ICat Stop capture... \n");
			break;
*/

		case ICAT_CAP_CDSPMISC_IDX:
			G_ucPccamTuningFlag = 2;
			G_wIndexLock = idx;
			G_wValueLock = val;		
			break;
			
		case ICAT_CAP_AEAWB_IDX:
			switch((val&0xff00)>>8)
			{
				case 0x01:
					//icatPrintf("@@@ gID @@@\n");
					G_ucCtrlData[0] = aeIntrGIdGet();
					break;
				case 0x02:
					//icatPrintf("@@@ gain/offset @@@\n");
					// R,Gr,B,Gb gain
					G_ucCtrlData[0] = XBYTE[0x2154];
					G_ucCtrlData[1] = XBYTE[0x2155];
					G_ucCtrlData[2] = XBYTE[0x2156];
					G_ucCtrlData[3] = XBYTE[0x2157];
					G_ucCtrlData[4] = XBYTE[0x2158];
					G_ucCtrlData[5] = XBYTE[0x2159];
					G_ucCtrlData[6] = XBYTE[0x215A];
					G_ucCtrlData[7] = XBYTE[0x215B];
					// R,Gr,B,Gb Offset
					G_ucCtrlData[8] = XBYTE[0x2118];
					G_ucCtrlData[9] = XBYTE[0x2119];
					G_ucCtrlData[10] = XBYTE[0x211A];
					G_ucCtrlData[11] = XBYTE[0x211B];
					G_ucCtrlData[12] = XBYTE[0x211C];
					G_ucCtrlData[13] = XBYTE[0x211D];
					G_ucCtrlData[14] = XBYTE[0x211E];
					G_ucCtrlData[15] = XBYTE[0x211F];
					break;
			}
			break;

//--------------------- Raw Data ---------------------
		case ICAT_RAW_GET_DLINFO_IDX:	//get raw info
			SetValue16(buf, SUCCESS);
			SetValue16(buf+2, 0);		//format
			//SetValue16(buf+4, 0xA20); 	//width, test
			//SetValue16(buf+6, 0x798);	//height
			//frontSnapSizeGet(0, &snapW, &snapH);
			//snapW=2592;
			//snapH=1944;
			
			SetValue16(buf+4, snapRawHSize);
			SetValue16(buf+6, snapRawVSize);

			#if 0
				icatPrintf("Get Raw Info : \n");
				for(i = 0; i < 16; i++) {
					printf("0x%bx, ", buf[i]);
				}
				printf("\n");
			#endif

			//printf("iCat Raw hsize=0x%x,vsize=0x%x\n", snapRawHSize,snapRawVSize);
			
			break;

		case ICAT_RAW_DOWNLOAD_IDX:	//download raw file size
			iCatRawFileSize = GetValue32(buf);
			icatPrintfCmdFlow("i iCat RawSize = %ld\n", iCatRawFileSize);

			if(iCatRawFileSize == 0){
				iCatNextJob = ICAT_JOB_SKIP;
			}
			else{
				iCatNextJob = ICAT_JOB_DO;
			}

			break;
		case ICAT_RAW_GET_ULINFO_IDX:	//Upload raw file size
			SetValue16(buf, SUCCESS);
			//tValue16(buf+2, 8); /*format*/			
			buf[2]=8;   /* bits/pixel */
			//f[3]=0;   /* bayer pattern */
			SetValue16(buf+4, snapRawHSize); /*width*/
			SetValue16(buf+6, snapRawVSize); /*height*/
			
			//icatPrintf("snap h=%d,v=%d\n",snapRawHSize,snapRawVSize);	
			//halCdspWbGainGet(wb_gain+0, wb_gain+1, wb_gain+2, wb_gain+3);
			//nValue = (wb_gain[0]<<6)>>10;
			//SetValue16(pdataBuf+8, (UINT16)nValue); /*r gain*/
			//nValue = (wb_gain[1]<<6)>>10;
			//SetValue16(pdataBuf+10, (UINT16)nValue); /*gr gain*/
			//nValue = (wb_gain[2]<<6)>>10;
			//SetValue16(pdataBuf+12, (UINT16)nValue); /*b gain*/
			//nValue = (wb_gain[3]<<6)>>10;
			//SetValue16(pdataBuf+14, (UINT16)nValue); /*gb gain*/	
			if(snapRawHSize == 0){
				iCatNextJob = ICAT_JOB_SKIP;
			}
			else{
				iCatNextJob = ICAT_JOB_DO;
			}
			
			break;
//--------------------- Res File ---------------------
		case ICAT_RES_DOWNLOAD_IDX: //DownloadRes(size)
			iCatResFileId = val;
			iCatResFileSize = GetValue32(buf);
			icatPrintfCmdFlow("Res Id=0x%x,size=%ld\n", iCatResFileId, iCatResFileSize);

			if(iCatResFileSize == 0){
				iCatNextJob = ICAT_JOB_SKIP;
			}
			else{
				iCatNextJob = ICAT_JOB_DO;
			}

			break;

		case ICAT_RES_UPLOAD_IDX: //UploadRes(size)	should 2 times
			iCatResFileId = val;
			SetValue16(buf, SUCCESS);
			SetValue32(buf+2, iCatBulkSize);		//size
			#if 0
				UINT32 i;
				icatPrintf("Upld Res Info : \n");
				for(i = 0; i < 16; i++) {
					icatPrintf("0x%2bx, ", buf[i]);
				}
				icatPrintf("\n");			
			#endif
			
			iCatNextJob = ICAT_JOB_DO;
			break;

		case ICAT_RES_NUM_IDX: //Set res num
			iCatResFileNum = GetValue16(buf);
			icatPrintfCmdFlow("icatResFileNum = 0x%x\n", iCatResFileNum);
			
			if(iCatResFileNum == 0){
				iCatNextJob = ICAT_JOB_SKIP;
			}
			break;

//--------------------- CDSP FLOW ---------------------
		case ICAT_CDSP_PREPRO_IDX:	/* Ready to get Raw Data */
			SetValue16(buf, SUCCESS);
			//icatPrintf("ready to get raw\n");
			
			//iCatNextJob = ICAT_JOB_DO; 	//xian ++ 20080521

			//iCatResFileNum=0;
			break;

		case ICAT_CDSP_START_IDX:	//Stop CDSP
			SetValue16(buf, SUCCESS);
			//icatPrintf("get raw suc\n");
			break;

//--------------------- YUV Data ---------------------
		case ICAT_YUV_GET_ULINFO_IDX: //Get info
			SetValue16(buf, SUCCESS);
			SetValue16(buf+2, 0);		//format

			//snapW=2592;
			//snapH=1944;
			//SetValue16(buf+4, 1296*2); 	//width, test, xian 20080521, *2 for new i-cat 1.2.x
			//SetValue16(buf+6, 1944);	//height
			//frontSnapSizeGet(0, &snapW, &snapH);
			//icatPrintf("w=%u, h=%u\n", snapW, snapH);
			
			SetValue16(buf+4, snapYuvHSize);
			SetValue16(buf+6, snapYuvVSize);

			if(fileSize == 0){
				iCatNextJob = ICAT_JOB_SKIP;
			}
			else{
				iCatNextJob = ICAT_JOB_DO;
			}

			#if 0
				printf("Get Yuv Info : \n");
				for(i = 0; i < 16; i++) {
					printf("0x%2bx, ", buf[i]);
				}
				printf("\n");
			#endif

			break;

//		case 0x0502: //UploadRes(size)
//			icatPrintf("ICat Dnd resSize = 0x%lx\n", GetValue32(buf));

			//break;

//--------------------- Bulk Trigger ---------------------
		case ICAT_BULK_OUT_IDX:  	//bulk out start
		case ICAT_BULK_IN_IDX:	//bulk in
			//icatPrintf("Bulk Size = 0x%lx\n", iCatBulkSize);
			SetValue16(buf, SUCCESS);
			SetValue32(buf+2, iCatBulkSize);		//format
			
			#if 0
				printf("Get Bulk Size : \n");
				for(i = 0; i < 16; i++) {
					printf("0x%2bx, ", buf[i]);
				}
				printf("\n");
			#endif
			
			break;

		case ICAT_BULK_TRIG_IDX:	//xian ++ 20080521, add 0x604
			//icatPrintf("trig\n");
			sp1kSnapIcatBulkTrig = 1;
			break;
		case ICAT_BULK_OUT_END_IDX:	//bulk out end
		case ICAT_BULK_IN_END_IDX:	//bulk in end
			//icatPrintf("end\n");
			sp1kSnapIcatBulkEnd =1;
			break;

//--------------------- JPG Data ---------------------
		case ICAT_JPG_GET_ULINFO_IDX: //Get info
		    fileSize = snapJpgFileSizeGet_Intr();
		    
            if(fileSize){
			    SetValue16(buf, SUCCESS);
			}
            else{
			    SetValue16(buf, FAIL);                
			}
			    
			SetValue32(buf+2, fileSize);		//size

			if(fileSize == 0){
				iCatNextJob = ICAT_JOB_SKIP;
			}
			else{
				iCatNextJob = ICAT_JOB_DO;
			}

            break;


//--------------------- Invalid ---------------------
		default :
			bValidCmd = 0;
	}

  	if(!bValidCmd){
		//icatPrintf( "INVALID INDEX %x\n", idx );
	}
	return;

#undef SetValue16
#undef GetValue16
#undef SetValue32
#undef GetValue32
}

//invoked in main loop
void 
icatVndCmdStubDpc( 
	UINT16 idx, 
	UINT16 val, 
	UINT16 bufSz, 
	UINT8  *buf 
)
{
	UINT8 bValidCmd = 1;

	val = val; /* for compile warning*/
	
	bufSz = bufSz; /* for compile warning*/
	//Stub stub = 0;
	IcatVCIndex = idx;

	//icatPrintf( "--f: VndCmdStubDPC %x, %x, %bx\n", idx, bufSz , *buf);

	switch ( idx ){
//--------------------- Capture Command ---------------------
/*		case 0x0100:	//set exposure
			printf("ICat Set exp = 0x%lx\n", GetValue32(buf));
			frontExposureTimeSet(0, GetValue32(buf), 0, 1);

			break;

		case 0x0101:	//set gain
			printf("ICat Set gain = 0x%lx\n", GetValue32(buf));
			frontGainSet(0, GetValue32(buf), 0, 1);

			break;
*/
		case ICAT_CAP_START_IDX:	//start capture
			//printf("ICat Start capture...\n");
//			G_bRequestLock = 0;

			// load shutter sound
			//sp1kNandRsvRead(0x10, (K_SDRAM_CommonEndAddr<< 1));//will mark

			//init global

			iCatFlowMode = icatGetValue32(buf);
			
			iCatNextJob = ICAT_JOB_WAIT_AP;
			iCatBulkSize = 0;

			icatPrintfCmdFlow("iCatFlowMode=0x%x\n",iCatFlowMode);
			if((iCatFlowMode&ICAT_RAW_ONLY_MODE)||(iCatFlowMode&ICAT_RAW_UPLD_MODE)){				
				sp1kSnapParamSet(SP1K_SNAP_PARAM_DEBUG_RAW_ONLY,1);				
			}
			else{
				sp1kSnapParamSet(SP1K_SNAP_PARAM_DEBUG_RAW_ONLY,0);
			}
			
			
			//appVoicePlay(SP1K_MSG_UNKNOWN_HOST, VOICE_TYPE_SHUTTER, VOICE_PLAYLEVLE_HIGH);
			//sp1kHalSpeakerSet(0);
			sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,0);
			sp1kSnapTask();
			sp1kSnapParamSet( SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT,1);
			
			break;

		case ICAT_CAP_STOP_IDX:	//stop capture
			//icatPrintf("ICat Stop capture...\n");
			break;

		#if 0
		case ICAT_CAP_CDSPMISC_IDX:
			G_wIndexLock = idx;
			G_wValueLock = val;		
			icatIQTunningProcess();
			break;
		#endif

//--------------------- Res File ---------------------

		case ICAT_RES_START_IDX: //Trigger flow
			//icatPrintf("val = 0x%x\n", val);

			if(val == 0){
				//icatPrintf("ICat Res -> Nand...\n");
				iCatFlowMode=ICAT_RES_DNLD_MODE;
				iCatNextJob = ICAT_JOB_WAIT_AP;
				icatResDownload();
			}
			//else if(val == 1){
			//	icatPrintf("ICat Res <- Nand...\n");
			//	icatResUpload();
			//}
			//else if(val == 2){
			//	icatPrintf("ICat Res -> Register...\n");
			//	icatDownLoadRes(2);
			//}
			else{
				iCatNextJob = ICAT_JOB_SKIP;
				//printf("unknow res type!!\n");
			}


			break;

		#if 0

			case 0x0303: //DownloadRes
				printf("val = 0x%x\n", val);

				if(val == 0){
					printf("ICat Res -> Nand...\n");
					icatDownLoadRes(0);
				}
				break;


			case 0x0304: //UploadRes
				printf("ICat UploadRes...\n");
				icatResUpload();
				break;
			case 0x0305: //For set parameter
				printf("ICat Set Parameter...\n");
				icatDownLoadRes(0x305);
				break;
		#endif

		default :
			bValidCmd = 0;
	}

  	if(!bValidCmd){
		//icatPrintf( "INVALID INDEX %x\n", idx );
	}

}

void 
icatResDownload(
	void
)
{
	UINT8 lsb,msb;
	
	icatPrintfDownload("res download\n");

	sp1kSnapIcatCDSPParam = 1;
	
	if( sp1kSnapIcatMode == 1 ) {
		
		sp1kSnapIcatState=ICATE_RES_DOWNLOAD;	/*will: new icat flow*/

		icatPrintfDownload("iCatFlowMode=0x%x\n",iCatFlowMode);
		icatPrintfDownload("iCatNextJob=0x%bx\n",iCatNextJob);
		
		//download res data
		while( (iCatFlowMode & ICAT_RES_DNLD_MODE) && (iCatNextJob != ICAT_JOB_STOP) ){
			if( iCatNextJob == ICAT_JOB_DO ) { //test res bulk out
				iCatNextJob = ICAT_JOB_DONE;

				
				switch (iCatResFileId){
					case ICAT_RSC_RAWINFO:
						icatPrintfDownload("dnl ifo\n");
						
						icatBulkOut(K_SDRAM_IcatCdspParamBuf, iCatResFileSize);
						M_DRAM_SetStartAddress(K_SDRAM_IcatCdspParamBuf, 1);
						
						M_DRAM_ReadWord(lsb, msb);
						snapRawHSize=(UINT16)lsb+(UINT16)msb*256;
						M_DRAM_ReadWord(lsb, msb);
						snapRawVSize=(UINT16)lsb+(UINT16)msb*256;
						iCatRawFileSize=(UINT32)snapRawHSize*(UINT32)snapRawVSize;

						snapSensorCropHSize=snapYuvHSize=snapHSize=snapRawHSize;
						snapSensorCropVSize=snapYuvVSize=snapVSize=snapRawVSize; 									

						chgRawSizeSet(snapRawHSize,snapRawVSize);
						//snapMemInit(); 
						icatPrintfDownload("snap h=%d,v=%d\n",snapRawHSize,snapRawVSize);
						icatPrintfDownload("yuv size h=%d,v=%d\n",snapYuvHSize,snapYuvVSize);
						icatPrintfDownload("raw size=%ld\n",iCatRawFileSize);
					break;
					case ICAT_RSC_PVCDSP:
					case ICAT_RSC_STILLCDSP:
					case ICAT_RSC_CDSPPARA:
						icatPrintfDownload("dnl para\n");
						icatPrintfDownload("rsc addr=0x%lx\n",K_SDRAM_IcatCdspParamBuf);						
						icatBulkOut(K_SDRAM_IcatCdspParamBuf, iCatResFileSize);
						cdspBinRead( "", K_SDRAM_IcatCdspParamBuf );												
					break;
					case ICAT_RSC_GAMMATBL:						
						icatBulkOut(K_SDRAM_IcatCdspParamBuf, iCatResFileSize);		
						icatPrintfDownload("gamma size=%ld\n",iCatResFileSize);
						gammaBinRead( "", K_SDRAM_IcatCdspParamBuf);											
					break;					
					case ICAT_RSC_LSCTBL:						
						icatBulkOut(K_SDRAM_LSCTableBufAddr, iCatResFileSize);	
						icatPrintfDownload("lsc size=%ld\n",iCatResFileSize);
						lscBinRead( "", K_SDRAM_LSCTableBufAddr);						
					break;
					case ICAT_RSC_WDRTBL:
						icatBulkOut(K_SDRAM_IcatCdspParamBuf, iCatResFileSize);	
						icatPrintfDownload("wdr size=%ld\n",iCatResFileSize);
						wdrBinRead( "", K_SDRAM_IcatCdspParamBuf);											
					break;	
					case ICAT_RSC_MILSCCAL:
						icatPrintfDownload("misenlsc size=%ld\n",iCatResFileSize);
						icatBulkOut(K_SDRAM_IcatCdspParamBuf, iCatResFileSize);	
						sp1kNandRsvWrite(SP1K_RSC_ID_MILSCCAL, K_SDRAM_IcatCdspParamBuf<<1);
						
					break;					
					case ICAT_RSC_BADPIXEL:						
					default:
					break;						
				}
				
	
				iCatResFileNum--;
				if (iCatResFileNum==0){ /*end*/
					break; 
				}			
				
			}
			else if( iCatNextJob == ICAT_JOB_SKIP ){
				iCatNextJob = ICAT_JOB_DONE;
				break; //end
			}
						
		}
		//printf("dnl rsc suc\n");
		sp1kSnapIcatState = SNAP_ICATE_SENSOR_FINISH;
	
		iCatNextJob = ICAT_JOB_WAIT_AP;

		//if (((iCatFlowMode & ICAT_RAW_ONLY_MODE)==0)&&((iCatFlowMode & ICAT_RAW_UPLD_MODE)==0)){ /* not capture raw only*/			
		if (iCatFlowMode & ICAT_RAW_FLOW_MODE){ /* not capture raw only*/			
			snapMemInit();
		}
		
		//download raw data	
		while( (iCatFlowMode & ICAT_RAW_FLOW_MODE) && (iCatNextJob != ICAT_JOB_STOP) ){
			if( iCatNextJob == ICAT_JOB_DO ) { //test res bulk out
				iCatNextJob = ICAT_JOB_DONE;				
	
				icatBulkOut(snapRawDataAddr, iCatRawFileSize);
				
				icatPrintfDownload("raw data=%ld\n",iCatRawFileSize);
				//XBYTE[ 0x2110 ] = 0x02;
				//dramFileSave( "ICATRAW0RAW", snapRawDataAddr, iCatRawFileSize );
				#if 0

					UINT32 fileSize;
					UINT8 f[ 13 ] = "BLK00000RAW";
	
					f[ 11 ] = 0x20;
					f[ 12 ] = 0x00;
	
					dramFileLoad( f, snapRawDataAddr, &fileSize );

				#endif
				break; //end
			}
			else if( iCatNextJob == ICAT_JOB_SKIP ){
				iCatNextJob = ICAT_JOB_DONE;
				break; //end
			}
		}
	
		//xian ++ 20081117
		//printf("dnl raw suc\n");

		#if 0
		while( (iCatFlowMode & ICAT_RAW_ONLY_MODE) && (iCatNextJob != ICAT_JOB_STOP) ){
		//while( (iCatFlowMode & ICAT_RAW_UPLD_MODE) && (iCatNextJob != ICAT_JOB_STOP) ){
			//printf("raw size=%ld\n",iCatRawFileSize);
			//printf("iCatNextJob=%bd\n",iCatNextJob);
			if( iCatNextJob == ICAT_JOB_DO ) { //test res bulk out
				iCatNextJob = ICAT_JOB_DONE;
				icatBulkIn(snapRawDataAddr, iCatRawFileSize);
				break;
			}
			else if( iCatNextJob == ICAT_JOB_SKIP ){
				iCatNextJob = ICAT_JOB_DONE;
				break; //end
			}
		}
		//xian --
		#endif 
		
		sp1kSnapIcatCDSPParam = 0;
	
		sp1kSnapIcatGo = 0;
	
		//iCatNextJob = ICAT_JOB_WAIT_AP; //xian ++ 20080521
		sp1kSnapIcatState = SNAP_ICATE_RAW_READY;
	
		//xian ++ 20080521, wait CDSP Start Command
		//while ( iCatNextJob != ICAT_JOB_DO ) {};
		iCatNextJob = ICAT_JOB_DONE;
		//xian --
	
		//snapRawSave();	/*check if raw data is right*/
	}

}

//-----------------------------------------------------------------------------
//L1_DownloadToDRAM
//-----------------------------------------------------------------------------
/*
routine description:
	Download File to DRAM
arguments:
return value:
        0x00   - success
        others - error
*/
UINT8 
icatDownloadToDRAM(
	UINT32 BulkDRAMAddr, 
	UINT32 BulkSize
)
{
	//icatPrintf("BulkOut DRAMAddr=%lx,BulkSize=%lx\n",BulkDRAMAddr,(UINT32)BulkSize);
    G_BulkDRAMAddr = BulkDRAMAddr;
    G_ISPDRAMAddr = BulkDRAMAddr;
    G_BulkSize = BulkSize;
	G_LastBulkSize = BulkSize;
    G_BulkFlashType = 0x00;
	//icatPrintf("L1_DownloadToDRAM %lx %lx\n",G_ISPDRAMAddr, G_BulkSize);
	//richie@0125
	while ((G_usbState & 0x01) != 0x01);
	G_usbState &= 0xfe;
	//printf("L1_DownloadToDRAM %lx %lx\n",BulkDRAMAddr, BulkSize);
    HAL_UsbBulkEnable(0);
	HAL_UsbBulkFlashAddrSet(0,G_BulkDRAMAddr);
	HAL_UsbBulkOut(&G_BulkSize);
	//G_BulkDRAMAddr = HAL_UsbBulkFlashAddrGet(0);

    return HALK_SUCCESS;
}

//-----------------------------------------------------------------------------
//L1_UploadFromDRAM
//-----------------------------------------------------------------------------
/*
routine description:
	Upload File From DRAM
arguments:
return value:
        0x00   - success
        others - error
*/
UINT8
icatUploadFromDRAM(
	UINT32 BulkDRAMAddr, 
	UINT32 BulkSize
)
{
	UINT16 BulkInSize;

		BulkInSize = 0x40;

		if (G_bUSBSpeed == 0x01){
			BulkInSize = 0x200;
		}

        G_BulkDRAMAddr = BulkDRAMAddr;
        G_BulkSize = BulkSize;
        G_BulkFlashType = 0x00;

		while ((G_usbState & 0x01) != 0x01);
		G_usbState &= 0xfe;

	//icatPrintf("BulkIn DRAMAddr=%lx,BulkSize=%lx\n",BulkDRAMAddr,(UINT32)BulkSize);
	HAL_UsbBulkEnable(1);
    HAL_UsbBulkFlashAddrSet(0,G_BulkDRAMAddr);
	HAL_UsbBulkIn(&G_BulkSize);
	//G_BulkDRAMAddr = HAL_UsbBulkFlashAddrGet(0);	

	//richie@0125
        XBYTE[REG_Usb_TransactionEn] = K_USB_AP_IN_OPEN;//0x08;
        return HALK_SUCCESS;
}


void
icatSnapInitial(
	void
)
{
	if( sp1kSnapIcatMode == 1 ) {
		sp1kSnapIcatGo = 0;
		sp1kSnapIcatCDSPParam = 0;
	
		// predefine
		sp1kSnapParamSet( SP1K_SNAP_PARAM_QUALITY, SP1K_SNAP_QUALITY_NORMAL );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_FORMAT, SP1K_SNAP_FORMAT_422 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_BURST, 0 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_QV, 1 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_PRE_BRC, 0 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_POST_BRC, 0 );
		//sp1kSnapParamSet( SP1K_SNAP_PARAM_COMPRESS_RATIO, 10 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_JPG_OPT, 1 );
		sp1kJpegQTableSet( 85, 0, 0, 1 );
		icatPrintf("iCatFlow=0x%x\n",iCatFlowMode);
		//if(iCatFlowMode&ICAT_RAW_ONLY_MODE){
		//if((iCatFlowMode&ICAT_RAW_ONLY_MODE)||(iCatFlowMode&ICAT_RAW_UPLD_MODE)){
		if((iCatFlowMode&ICAT_RAW_FLOW_MODE)==0){
			sp1kSnapParamSet( SP1K_SNAP_PARAM_REDO_PREPROCESS, SP1K_SNAP_PRE_REDO_DISABLE );	/* don't do raw to raw process */
		}
		else{
			sp1kSnapParamSet( SP1K_SNAP_PARAM_REDO_PREPROCESS, SP1K_SNAP_PRE_REDO );	/* do raw to raw process */
		}
		
		// full size mode
		//snapHSize = snapYuvHSize = snapRawHSize = snapSensorCropHSize = snapSensorHSize;
		//snapVSize = snapYuvVSize = snapRawVSize = snapSensorCropVSize = snapSensorVSize;

		iCatRawFileSize=(UINT32)snapHSize*(UINT32)snapVSize;
		//sp1kSnapIcatYuvABBufLines = snapYuvLines;
		sp1kSnapIcatState = SNAP_ICATE_START;
	}

}

void
icatSnapFinish(
	void
)
{
	//xian ++
	//printf("end of snapTask\n");
	frontSnapSizeGet(0,&snapRawHSize,&snapRawVSize);

	/* restore size for memory allocation (snapConfigSize)*/
	snapSensorCropHSize=snapYuvHSize=snapHSize=snapRawHSize;
	snapSensorCropVSize=snapYuvVSize=snapVSize=snapRawVSize;									


	//if (iCatFlowMode&ICAT_RAW_UPLD_MODE){
	//	sp1kSnapIcatState = SNAP_ICATE_RAW_READY;
	//}
	//else{
	sp1kSnapIcatState = SNAP_ICATE_END;
	//}

}

UINT8
icatIsConnected(
	void
)
{
	if(G_UI_USBConnect == K_UI_USB_DISCONNECT){ 
		sp1kSnapIcatMode = 0;
	}
	return sp1kSnapIcatMode;
}

UINT8 
icatStateGet(
	void
)
{
	return sp1kSnapIcatState;
}


UINT8 
icatFlowModeGet(
	void
)
{
	return iCatFlowMode;
}

void 
icatSnapYuvBufTypeGet(
	UINT8 *type
)
{
	if( sp1kSnapIcatMode == 1 ) {
		if (iCatFlowMode & ICAT_YUV_UPLD_MODE){
			*type = SNAP_ONE_YUVBUF;
		}
		else{		
			*type = SNAP_TWO_YUVBUF;
		}
	}
	icatPrintf("type=%bx\n",*type);	
}

//no data cmds are DPC cmd
UINT8 icatIsDPCCmd(UINT16 idx)
{
    UINT8 ret; //= FALSE;

    switch (idx){
        case ICAT_CAP_START_IDX:
        case ICAT_CAP_STOP_IDX:
        case ICAT_RES_START_IDX:
        //case ICAT_CAP_CDSPMISC_IDX:
            ret = TRUE;
            break;
    	default:
	    	ret = FALSE;
    		break;
    }
    
    return ret;    
}

void
icatBulkOut(
	UINT32 addr,
	UINT32 size
)
{
	#if 0
		addr=addr=size;
	#else  
		UINT32 cnt = 0;
		UINT32 chunkSize = (UINT32)64 * 1024;

		icatPrintf("size=%ld\n",size);
		
		XBYTE[REG_Usb_EpAckIntEn] = XBYTE[REG_Usb_EpAckIntEn] & 0XEF;	// disable bulk out ack intr
		while( size ) {
			
			if( size < chunkSize){
				chunkSize = size;
			}
			size -= chunkSize;

			iCatBulkSize = (UINT32)64 * 1024; //xian ++ 20080527
				
			//icatPrintf("%ld\n",size);
			//xian ++ 20080521
			//icatPrintf("1bulk state=%bd\n",sp1kSnapIcatBulkTrig);
			//icatPrintf("size=%ld,BulkSize=%ld\n",size,iCatBulkSize);
			while( !sp1kSnapIcatBulkTrig ){
				//printf("2bulk state=%bd\n",sp1kSnapIcatBulkTrig);
				
			};/*wait trig cmd 0x604*/
			//xian --
			sp1kSnapIcatBulkTrig = 0; 

			//icatPrintf("trig\n");
				
			iCatBulkSize = 0; //xian ++ 20080527
			//icatPrintf("wait end\n");
			//sp1kSnapIcatBulkTrig = 0; //begin bulk, set by ap
			//icatPrintf("3bulk end state=%bd\n",sp1kSnapIcatBulkEnd);
			icatDownloadToDRAM(addr + ( cnt >> 1 ), chunkSize);
				
			//cnt += ((UINT32)64 * 1024);
			cnt += chunkSize;
			//icatPrintf("2cnt=%ld\n",cnt);
			while( !sp1kSnapIcatBulkEnd  ){ /*wait trig cmd 0x602*/
				//printf("4bulk end state=%bd\n",sp1kSnapIcatBulkEnd);
					
				//HAL_UsbBulkFlashAddrSet(0,G_BulkDRAMAddr);
				//HAL_UsbBulkOut(&G_BulkSize);
				//G_BulkDRAMAddr = HAL_UsbBulkFlashAddrGet(0);
			}
			sp1kSnapIcatBulkEnd=0;
			//icatPrintf("get end1\n");
		}
	#endif	
	}
	
void
icatBulkIn(
	UINT32 addr,
	UINT32 size
)
{    
	#if 0
    	addr=size;
	#else
		UINT32 cnt = 0;
		UINT32 chunkSize = (UINT32)64 * 1024;

	    XBYTE[REG_Usb_EpAckIntEn] = XBYTE[REG_Usb_EpAckIntEn] & 0XF7;   // disable bulk in ack intr
		while( size ) {
	        chunkSize = (UINT32)64 * 1024;
			if( size < chunkSize){
				chunkSize = size;
			}
			size -= chunkSize;

			iCatBulkSize = chunkSize; //xian ++, AP needed

			sp1kSnapIcatBulkTrig = 0;
	        dbgWdtFeed(20000);
			while(!sp1kSnapIcatBulkTrig){ // wait AP get size
			}

			iCatBulkSize = 0; // AP will polling, xian move 20080527
	        //dbgWdtFeed(20000);
	        //icatPrintf(" 604 ok,start bulk in\n");
			sp1kSnapIcatBulkTrig = 0; //begin bulk, set by ap
			//L1_UploadFromDRAM(addr + ( cnt >> 1 ), chunkSize);
	        //sidcDram2Usb(addr + ( cnt >> 1 ), chunkSize); //will mark //	replace HAL_BulkInOut();
			//move up 20080527		iCatBulkSize = 0; // AP will polling
	        //cx++ for bulk in 
	        HAL_UsbBulkEnable(1);
	        HAL_UsbBulkFlashAddrSet(0,addr + ( cnt >> 1 ));
	        //  icatPrintf("chunkSize=%lx\n",chunkSize);
			HAL_UsbBulkIn(&chunkSize);
			
			//CAT_Snap_DramToUsb(addr + ( cnt >> 1 ), chunkSize);
	        /* cx++ end */
	        //icatPrintf(" bulk in ok,wait 603\n");
			cnt += ((UINT32)64 * 1024);
	        dbgWdtFeed(20000);
			while( !sp1kSnapIcatBulkEnd  ){
				//HAL_UsbBulkFlashAddrSet(0,G_BulkDRAMAddr);
				//HAL_UsbBulkIn(&G_BulkSize);
				//G_BulkDRAMAddr = HAL_UsbBulkFlashAddrGet(0);
			}
		}

		return;
	#endif	
}	
	
 
void 
icatSnapJpgUpload(
	UINT32 addr, 
	UINT32 filesize
)
{
	if(sp1kSnapIcatMode == 1 && (iCatFlowMode & ICAT_JPG_UPLD_MODE)){				
		//dbgWdtFeed(1000);
		
		iCatNextJob = ICAT_JOB_WAIT_AP;
		sp1kSnapIcatState = SNAP_ICATE_JPG_FINISH;
		dbgWdtFeed(20000);
		icatPrintf("upload\n");
		
		while(iCatNextJob == ICAT_JOB_WAIT_AP);
		
			if(iCatNextJob == ICAT_JOB_DO){
	            //dbgWdtFeed(10000);	          
				sp1kSnapIcatState = ICAT_JPG_UPLOAD;
				icatBulkIn(addr, filesize);
			}	
	}
	
}

void 
icatSnapYuvUpload(
	UINT32 addr, 
	UINT32 filesize
)
{

	if(sp1kSnapIcatMode == 1 && (iCatFlowMode & ICAT_YUV_UPLD_MODE)){ 		
	
		iCatNextJob = ICAT_JOB_WAIT_AP;
		sp1kSnapIcatState = SNAP_ICAT_CDSP_YUV_READY;
		dbgWdtFeed(20000);
		icatPrintf("upload\n");
		
		while(iCatNextJob == ICAT_JOB_WAIT_AP);
		
		//  dbgWdtFeed(10000);
		if(iCatNextJob == ICAT_JOB_DO){				
	        //dbgWdtFeed(10000);
	      	//icatPrintf("bulk in\n");
			sp1kSnapIcatState = ICATE_YUV_UPLOAD;
			icatBulkIn(addr, filesize);
		}
	}
}

void 
icatSnapRawUpload(
	UINT32 addr, 
	UINT32 filesize
)
{

	icatPrintf("raw upload\n");
	if(sp1kSnapIcatMode == 1 && (iCatFlowMode & ICAT_RAW_UPLD_MODE)){				
		//dbgWdtFeed(1000);
		
		iCatNextJob = ICAT_JOB_WAIT_AP;
		sp1kSnapIcatState = SNAP_ICATE_RAW_READY;
		dbgWdtFeed(20000);
		
		while(iCatNextJob == ICAT_JOB_WAIT_AP);
		
		if(iCatNextJob == ICAT_JOB_DO){
			//dbgWdtFeed(10000);	     
			icatPrintf("raw size=%ld\n",filesize);
			sp1kSnapIcatState = ICAT_RAW_UPLOAD;
			icatBulkIn(addr, filesize);
		}	
	}
	//icatPrintf("raw finish\n");
}

void 
icatSetValue16_Intr(
	UINT8 *buf, 
	UINT16 val
)
{
	buf[0] = (UINT8)val;
	buf[1] = (UINT8)(val >> 8);
}

UINT16 
icatGetValue16_Intr(
	UINT8 *buf
)
{
	UINT16 val;

	val = (((UINT16)buf[1]) << 8) + buf[0];

	return val;
}

void 
icatSetValue32_Intr(
	UINT8 *buf, 
	UINT32 val
)
{
	UINT8 *pt = (UINT8*)&val;

	buf[0] = pt[3];
	buf[1] = pt[2];
	buf[2] = pt[1];
	buf[3] = pt[0];
}

UINT32 
icatGetValue32_Intr(
	UINT8 *buf
)
{
	UINT32 val;

	val = (((UINT32)buf[3]) << 24) + (((UINT32)buf[2]) << 16) + (((UINT32)buf[1]) << 8) + buf[0];

	return val;
}

void icatIQTunningProcess(void)
{
	UINT8 sys, curAeIdx, currAeTbl;
	
	switch((G_wValueLock&0xff00)>>8)
	{
		case 0x01: // AF on/off
			break;

		case 0x02: // AE on/off
			ENTER_CRITICAL(sys);
			if(G_pucCtrlDataPointer[0]==0)
				sp1kAeSetModeEn(0);
			else
				sp1kAeSetModeEn(1);				
			EXIT_CRITICAL(sys);
			break;

		case 0x03: // AE Set Index
			sp1kAeStatusGet(SP1K_AE_G_AETblSel, &currAeTbl);
			sp1kAeParamSet(SP1K_AE_gId,(UINT16)G_pucCtrlDataPointer[0]);
			aeConfigForIQTunning((UINT16)G_pucCtrlDataPointer[0],currAeTbl);
			break;

		case 0x04: // AWB on/off
			ENTER_CRITICAL(sys);
			if(G_pucCtrlDataPointer[0]==0)
				sp1kAwbDisable();
			else
				sp1kAwbEnable();			
			EXIT_CRITICAL(sys);		
			break;

		case 0x05: // AWB Set
			XBYTE[REG_Cdsp_gain_en] = 0x01;
			if(G_pucCtrlDataPointer[0]==0)
			{ 
				XBYTE[REG_Cdsp_prgain_low] = G_pucCtrlDataPointer[1];
				XBYTE[REG_Cdsp_prgain_high] = G_pucCtrlDataPointer[2];
				XBYTE[REG_Cdsp_pgrgain_low] = G_pucCtrlDataPointer[3];
				XBYTE[REG_Cdsp_pgrgain_high] = G_pucCtrlDataPointer[4];
				XBYTE[REG_Cdsp_pbgain_low] = G_pucCtrlDataPointer[5];
				XBYTE[REG_Cdsp_pbgain_high] = G_pucCtrlDataPointer[6];
				XBYTE[REG_Cdsp_pgbgain_low] = G_pucCtrlDataPointer[7];
				XBYTE[REG_Cdsp_pgbgain_high] = G_pucCtrlDataPointer[8];
			}
			else if(G_pucCtrlDataPointer[0]==1)
			{
				XBYTE[REG_Cdsp_proffset_low] = G_pucCtrlDataPointer[1];
				XBYTE[REG_Cdsp_proffset_high] = G_pucCtrlDataPointer[2];
				XBYTE[REG_Cdsp_pgroffset_low] = G_pucCtrlDataPointer[3];
				XBYTE[REG_Cdsp_pgroffset_high] = G_pucCtrlDataPointer[4];
				XBYTE[REG_Cdsp_pboffset_low] = G_pucCtrlDataPointer[5];
				XBYTE[REG_Cdsp_pboffset_high] = G_pucCtrlDataPointer[6];
				XBYTE[REG_Cdsp_pgboffset_low] = G_pucCtrlDataPointer[7];
				XBYTE[REG_Cdsp_pgboffset_high] = G_pucCtrlDataPointer[8];
			}
			break;

		case 0x06:
			HAL_CdspGammaEnSet(0);
			XBYTE[REG_Cdsp_Colcorren] = 0x00; // color matrix disable
			HAL_CdspYEdgeEnSet(0);
			HAL_CdspLEdgeEnSet(0);
			break;

		case 0x07:
			sp1kAeStatusGet(SP1K_AE_gId, &curAeIdx);
			if( G_ucCtrlData[0] == 1 ) {
				aeConfigForIQTunning(curAeIdx, 50);
				aeFrameRateChangeSet(1);
			}
			else {
				aeConfigForIQTunning(curAeIdx, 60);
				aeFrameRateChangeSet(0);
			}
			break;
	}
}


UINT32 
icatGetValue32(
	UINT8 *buf
) 
{
	UINT32 val;

	val = (((UINT32)buf[3]) << 24) + (((UINT32)buf[2]) << 16) + (((UINT32)buf[1]) << 8) + buf[0];

	return val;
}

/****************************/
/*  unused function			*/
/****************************/

#if 0
//xian : should set "sp1kSnapIcatState" after invoked if used in capture flow
UINT8 icatDownLoadRes(UINT16 idx)
{
	osdMem_t* pMem;
	osdMemCbGet(&pMem);

#ifndef AMCAP_OPTION
	//printf("ICat_DownLdRes\n");
	iCatNextJob = ICAT_JOB_STOP;
	sp1kSnapIcatState = ICATE_RES_DOWNLOAD;  //xian ++, AP should wiat this state

	/* 3A off */
	aeModeEnableSet( 0 );
	sp1kAwbDisable();
	//G_AWBEN = 0;
	//XBYTE[ REG_Cdsp_gain_en ] = 0x00;
	/*************/
	
	while( !( G_AEReady == 0 && G_AESetting == 0 ) );


	while( iCatNextJob != ICAT_JOB_DO ){
		/*//for escape
		if(iCatNextJob == ICAT_JOB_STOP){
			return TRUE;
		}
		*/
	}
	
	iCatNextJob = ICAT_JOB_DONE;
	while( 1 ){
		iCatBulkSize = iCatResFileSize; //xian ++ 20080527

		//xian ++ 20080521
		sp1kSnapIcatBulkTrig = 0; //wait trig
		while( !sp1kSnapIcatBulkTrig  ){};
		//xian --

		iCatBulkSize = 0; //xian ++ 20080527

		//printf("Dnld res id=%x,size=0x%lx,Num=%lx\n", iCatResFileId, iCatResFileSize,(UINT32)iCatResFileNum);
		iCatResFileNum --;
		sp1kSnapIcatBulkTrig = 0; //begin bulk, set by ap
		icatDownloadToDRAM(pMem->char8.addr, iCatResFileSize);
		//while( !sp1kSnapIcatBulkTrig  ){
		//	HAL_UsbBulkFlashAddrSet(0,G_BulkDRAMAddr);
		//	HAL_UsbBulkOut(&G_BulkSize);
		//	G_BulkDRAMAddr = HAL_UsbBulkFlashAddrGet(0);
		//}
		if( idx == 2 ) {
			if( iCatResFileId == 0x09 ) {
				cdspBinRead( "", pMem->char8.addr );

				cdspInit();

				// PV only L edge
			    //HAL_CdspEdgeCoringMedianEnSet( 0, 1, 0, 0);
	   			HAL_CdspYEdgeEnSet(0);
				HAL_CdspLEdgeEnSet(1);
				HAL_CdspYCoringEnSet(0);
				HAL_CdspUVMedianEnSet(0); 				

				HAL_GprmDramDma( pMem->char8.addr << 1, K_SDRAM_IcatCdspParamBuf << 1, iCatResFileSize, 1 );
			}
			else if( iCatResFileId == 0x08 ) {
				gammaBinRead( "", pMem->char8.addr );

				HAL_GprmDramDma( pMem->char8.addr<< 1, K_SDRAM_IcatGammaBuf << 1, iCatResFileSize, 1 );
			}
			else if( iCatResFileId == 0x07 ) {
#if TAG_TODO
				lscBinRead( "", pMem->char8.addr );
#endif
				HAL_GprmDramDma( pMem->char8.addr << 1, K_SDRAM_IcatLcBuf << 1, iCatResFileSize, 1 );
			}
		}
		else{
			//printf("write nand %lx %lx\n", (UINT32)iCatResFileId, (UINT32)iCatResFileSize);
			sp1kNandRsvWrite(iCatResFileId, (pMem->char8.addr<< 1));
		}
		
		if( iCatResFileNum == 0 ){
			iCatNextJob = ICAT_JOB_DONE;
			break;
		}
		while( iCatNextJob != ICAT_JOB_DO ){
			/*//for escape
			if(iCatNextJob == ICAT_JOB_STOP){
				return TRUE;
			}
			*/
		}
		iCatNextJob = ICAT_JOB_DONE;
	}

	sp1kSnapIcatState = SNAP_ICATE_IDLE; //xian ++
#else
    idx = idx; // for compile warning
#endif	
	return TRUE;
}


UINT8 
icatResUpload(
	void
)
{

	#ifndef AMCAP_OPTION
		UINT8 status;
	#endif
	
	osdMem_t* pMem;
	osdMemCbGet(&pMem);


	#ifndef AMCAP_OPTION
	
		iCatNextJob = ICAT_JOB_STOP;
		iCatBulkSize = 0;
		//	iCatNextJob = ICAT_JOB_DONE;
		sp1kSnapIcatState = ICATE_RES_UPLOAD;  //xian ++, AP should wiat this state

		while( 1 ){
			while( iCatNextJob != ICAT_JOB_DO );
			ENTER_CRITICAL( status );
			iCatResFileNum --;
			//printf("K_SDRAM_OsdFontBufAddr=%lx\n", K_SDRAM_OsdFontBufAddr);
			sp1kNandRsvSizeGet(iCatResFileId, NULL, &iCatResFileSize);
			//printf("UpLd res id=%x,size=0x%lx,Num=%lx\n", iCatResFileId, iCatResFileSize,(UINT32)iCatResFileNum);
			//sp1kNandRsvRead(iCatResFileId, (K_SDRAM_OsdFontBufAddr << 1));//will mark
			#if 0
			dumpData(iCatResFileSize, K_SDRAM_OsdFontBufAddr, 1);
			#endif
			iCatBulkSize = iCatResFileSize; // AP will polling
			sp1kSnapIcatBulkTrig = 0; //begin bulk, set by ap
			EXIT_CRITICAL( status );
			while(!sp1kSnapIcatBulkTrig);
			sp1kSnapIcatBulkTrig = 0; //begin bulk, set by ap

			icatUploadFromDRAM(pMem->char8.addr, iCatResFileSize);
			//while( !sp1kSnapIcatBulkTrig  ){
			//	HAL_UsbBulkFlashAddrSet(0,G_BulkDRAMAddr);
			//	HAL_UsbBulkIn(&G_BulkSize);
			//	G_BulkDRAMAddr = HAL_UsbBulkFlashAddrGet(0);		}
			if( iCatResFileNum == 0 ){
				iCatNextJob = ICAT_JOB_DONE;
				//return TRUE;
				break; //xian ++
			}
			iCatBulkSize = 0; // AP will polling
			iCatNextJob = ICAT_JOB_DONE;
		}

		sp1kSnapIcatState = SNAP_ICATE_IDLE; //xian ++
	#endif
	
	return TRUE;
}

void 
icatSetValue16(
	UINT8 *buf, 
	UINT16 val
)	
{
	buf[0] = (UINT8)val;
	buf[1] = (UINT8)(val >> 8);
}

UINT16 
icatGetValue16(
	UINT8 *buf
) 
{
	UINT16 val;

	val = (((UINT16)buf[1]) << 8) + buf[0];

	return val;
}

void 
icatSetValue32(
	UINT8 *buf, 
	UINT32 val
)	 
{
	UINT8 *pt = (UINT8*)&val;

	buf[0] = pt[3];
	buf[1] = pt[2];
	buf[2] = pt[1];
	buf[3] = pt[0];
}


#endif

#endif//ICAT_OPTION

