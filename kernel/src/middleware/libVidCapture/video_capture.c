/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 /**
 * @file		videoCapture.c
 * @brief		for video capture
 * @author		hy.ren
 * @since		2008-03-14
 * @date		2008-10-31
 */
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/
#include "general.h"
#include "common.h"
#include "initio.h"
#include "reg_jpeg.h"
#include "reg_def.h"
#include "dos32.h"
#include "main.h"
#include "intr.h"
#include "utility.h"

#include "hal.h"
#include "hal_device.h"
#include "asicreg.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal_cdsp_iq.h"
#include "hal_jpeg.h"
#include "hal_front.h"
#include "hal_front_para.h"

#include "func_ptr.h"

#include "dcf_api.h"
#include "dbg_def.h"
#include "dbg_wdt.h"
#include "dbg_assert.h"

#include "video_mem.h"
#include "video_capdef.h"
#include "os_api.h"
#include "snap_qtable.h"
#include "exif_entry.h"
#include "video_cap_mem.h"
#include "sp1k_rsvblk_api.h"
#include "video_dtstmp.h"
#include "dbg_mem.h"
#include "video_capture.h"
#include "os_msg.h"
#include "Sp1k_aud_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_snap_api.h"
#include "video_cap_vlcbuf.h"
#include "video_cap_que.h"
#include "video_cap_hal.h"
#include "video_clip.h"
/**************************************************************************
 *				C O N S T A N T S
 **************************************************************************/
#define __FILE	  __FILE_ID_VIDEO_CAPTURE__

//#define _VidCapturePintf printf
#ifndef _VidCapturePintf
#define _VidCapturePintf /_printf
#endif

#define JPG_HDR_SIZE (UINT32)65536
#define QTABLEVALUE 50
/**************************************************************************
 *				M A C R O S 
 **************************************************************************/

/**************************************************************************
 *				D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *				G L O B A L   D A T A
 **************************************************************************/
UINT16 g_speedyCaptureParam[VD_CAP_PARAM_MAX] = {0};
static appVidCapCtrl_t speedyCaptureCtrl;
UINT8 speedyCaptureDateStamp;
UINT8 fTFrame;
UINT8 IsVlcReady;
/**************************************************************************
 *				E X T E R N A L    R E F E R E N C E S 
**************************************************************************/
/**************************************************************************
 *				E X T E R N A L    R E F E R E N C E S 
 **************************************************************************/
extern UINT8 videoRecInitCdsp(UINT16 width,UINT16 height);
extern void jpegYQTableGen(SINT32 quality,	UINT8 *qtable);
extern void jpegUVQTableGen(SINT32 quality, UINT8 *qtable);
extern void videoStampSet();

/*date stamp function */
extern videoStampCfg_t g_vidDateStamp;
extern UINT8 videoDtFlag[];

/**************************************************************************
 *				I N T E R N A L    F U N C T I O N    D E C L A R A T I O N S 
 **************************************************************************/

/**************************************************************************
 *				F U N C T I O N   D E F I N I T I O N
 **************************************************************************/
void speedyCaptureCallbackReg(VDCapCallBackId_e id,void *pfunc)
{
	switch(id){
		case SPEEDCAP_STILL:
			speedyCaptureCtrl.cbFun.pfnStill = pfunc;
		break;
		case SPEEDCAP_CLIP:
			speedyCaptureCtrl.cbFun.pfnClip = pfunc;
		break;
		case SPEEDCAP_LIGHT_OPEN:
			speedyCaptureCtrl.cbFun.pfnLightOpen = pfunc;
		case SPEEDCAP_LIGHT_CLOSE:
			speedyCaptureCtrl.cbFun.pfnLightClose= pfunc;
		default:
		break;
	}
}

static void speedyCaptureEofIntrEnable(appVidCapCtrl_t *pVidCap)
{
	HAL_DramFrmBufNumSet(2);
	if(pVidCap->dateStampEn) {
		videoStampSet();
	}
	XBYTE[REG_Jpeg_Intr_Event] |= 0x0a; /*clear VLC overflow interrupt*/ 
	XBYTE[REG_Dram_VlcRdy]|=0x03;//clear

	XBYTE[REG_Dram_VlcIntEn] &= ~0x03;
	XBYTE[REG_Jpeg_Intr_Enable]|=0x02; 
}

static void speedyCaptureEofIntrDisable(void)
{
	XBYTE[REG_Dram_VlcIntEn] &= ~0x03;
	XBYTE[REG_Jpeg_Intr_Enable] &= ~0x02;
	
	if(speedyCaptureCtrl.dateStampEn){
	  XBYTE[REG_Jpeg_DisGatedClk] &= ~0x08; 
	  XBYTE[REG_Jpeg_StampEn]=0x00;
	}
}

void speedyCaptureDstEn(UINT8 en)
{
	speedyCaptureDateStamp = en;
}

static UINT8 speedyCaptureInitMjpg(appVidCapCtrl_t *pVidCap)
{
 	UINT32 tmpVLCAddrA = pVidCap->vlcBufQue.dataNod[0].vlcAddr;
	UINT32 tmpVLCAddrB = pVidCap->vlcBufQue.dataNod[1].vlcAddr;
	
	HAL_DramAVLCAddrSet(tmpVLCAddrA );
	HAL_DramBVLCAddrSet(tmpVLCAddrB);		
	HAL_JpegVlcBoundSet(1, 1, pVidCap->vlcBufSize-(JPG_HDR_SIZE+0x253));		
	speedyCaptureEofIntrEnable(pVidCap);
	HAL_JpegMjpgTrig(tmpVLCAddrA, tmpVLCAddrB, 0);

	return TRUE;
}

static void speedyCaptureQtInit(UINT16 qIdx)
{
	UINT8 qtable[ 128 ];

	jpegYQTableGen( (SINT32)qIdx, qtable );
	jpegUVQTableGen( (SINT32)qIdx, qtable + 64 );
	HAL_JpegQTableWrite( qIdx, qtable, qtable + 64, 1 );
}

static void speedyCaptureInitHdr(appVidCapCtrl_t *pVidCap)
{
	UINT32 tmp0;
	UINT8 i;
	UINT32 sizeA = speedyCaptureMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	UINT32 addrA = speedyCaptureMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A)+(UINT32)32768;//32768 = 64*1024/2
	
	/* prepare JPEG header */
	sp1kNandRsvRead(0x0b,addrA<<1);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(addrA);
	HAL_DmaDo(0,1,0x0104,0,0);
	XBYTE[0x38E8] = (UINT8)(pVidCap->height>>8); 
	XBYTE[0x38E9] = (UINT8)(pVidCap->height);
	XBYTE[0x38EA] = (UINT8)(pVidCap->width>>8); 
	XBYTE[0x38EB] = (UINT8)(pVidCap->width);

	tmp0 = addrA;
	for(i = 0; i < pVidCap->capNum; i ++){
		HAL_DmaSetSramAddr(0x0008);          // fill video clip buffer
		HAL_DmaSetDramAddr(tmp0);
		HAL_DmaDo(1,0,0x00FC,0,0); //252
		tmp0 += sizeA;
	}

}

static void speedyCaptureClipInitHdr(appVidCapCtrl_t *pVidCap)
{
	UINT32 tmp0;
	UINT8 i;
	UINT8 qtable[130];
	UINT32 rsvSize,rsvOffset;
	UINT32 rsvAlignSize;
	UINT32 sizeA = speedyCaptureMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	UINT32 addrA = speedyCaptureMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A)+(UINT32)32768;//32768 = 64*1024/2
	UINT32 vlcBufNum;
	
	/* prepare JPEG header */
	qtable[0] = 0;
	qtable[65] = 1;
	vlcBufNum = speedyCaptureFreeBufNumGet();
	sp1kNandRsvSizeGet(0x49,&rsvOffset,&rsvSize);
	rsvAlignSize = ((rsvSize+1)&~1);
	sp1kNandRsvRead(0x49,addrA<<1);
	HAL_DramStartAddrSet(addrA+0x118, 0);
	HAL_DramWordDataWrite(0x08, READ8(&pVidCap->height, 0));
	HAL_DramWordDataWrite(READ8(&pVidCap->height, 1), READ8(&pVidCap->width, 0));
	HAL_DramWordDataWrite(READ8(&pVidCap->width, 1), 0x03);	
	HAL_DramWordDataWrite(0x01,0x21); //YUY422 for 160*120
	jpegYQTableGen((SINT32)QTABLEVALUE, qtable +1 );
	jpegUVQTableGen((SINT32)QTABLEVALUE, qtable +66);
	HAL_DramDmaWrite(qtable, (addrA>>1)+3, 130, 1);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(addrA);
	HAL_DmaDo(0,1,rsvAlignSize,0,0);
	
	tmp0 = addrA;
	for(i = 0; i < vlcBufNum; i ++){
		HAL_DmaSetSramAddr(0x0000);         
		HAL_DmaSetDramAddr(tmp0);
		HAL_DmaDo(1,0,rsvAlignSize,0,0);
		tmp0 += sizeA;
	}
}


static void speedyCaptureGlobalInit(appVidCapCtrl_t *pGVidcap)
{
	UINT8 imgResolution;

	imgResolution = speedyCaptureParamGet(VD_CAP_SIZE);
	switch(imgResolution){
		case VD_CAP_VGA:
			pGVidcap->width = 640;
			pGVidcap->height = 480;
		break;
		case VD_CAP_D1:
			pGVidcap->width = 720;
			pGVidcap->height = 480;
		break;
		case VD_CAP_HD:
			pGVidcap->width = 1280;
			pGVidcap->height = 720;
		break;
		default:
			pGVidcap->width = 640;
			pGVidcap->height = 480;
		break;
	}
	//speedyCaptureCtrl.vlcBufQue = 
	pGVidcap->capNum = speedyCaptureParamGet(VD_CAP_COUNT);
	pGVidcap->vlcBufSize = speedyCaptureMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A)<<1;
	pGVidcap->dateStampEn = speedyCaptureDateStamp;
	pGVidcap->tmrCount = 0;
	pGVidcap->interCnt = 30 / pGVidcap->capNum;
	pGVidcap->sts = SPEEDCAP_STS_IDLE;
	fTFrame=0;
	IsVlcReady= 0;
}

static UINT8 speedyCaptureInit (appVidCapCtrl_t *pGVidCap) USING_0
 {
 	UINT8 mode;
	
	mode = speedyCaptureParamGet(VD_CAP_MOD);
	HAL_CpuImemSdSet(LOG_BANK_6, 0x8000);
	speedyCaptureGlobalInit(pGVidCap);

	if(pGVidCap->dateStampEn){
		videoRecStampInit();
	}

	if(mode == SPEEDCAP_CLIP){
		speedyCaptureClipQueInit(&pGVidCap->vlcBufQue,&pGVidCap->writeBufQue);
		speedyCaptureClipInitHdr(pGVidCap);
	}
	else{
		speedyCaptureStillQueInit(&pGVidCap->vlcBufQue);
		speedyCaptureInitHdr(pGVidCap);
	}
	
	if(!videoRecInitCdsp(pGVidCap->width,pGVidCap->height)){
		__PRINTF_POS();
		return FAIL;
	}
	speedyCaptureQtInit(QTABLEVALUE);	
	if(!speedyCaptureInitMjpg(pGVidCap)){
		__PRINTF_POS();
		return FAIL;
	}
	_VidCapturePintf("@@=%bu\n",XBYTE[0x27B6]);
	_VidCapturePintf("##=%u\n",++VidDebug);
	
	return SUCCESS;
}

void speedyCaptureTimeUpdate(appVidCapCtrl_t *vidCap)
{
	UINT8 i;
	UINT8 idx;
	
	if(vidCap->dateStampEn) {
		for(i=0,idx=g_vidDateStamp.disBufId+1;i<3;i++,idx++){
			if(idx>=4){
				idx=0;
			}
			if(videoDtFlag[idx]==0){
				videoRecStampLoad(idx);	
				break;
			}
		}
	}
}

void speedyCaptureStart()
{
	UINT8 sts;
	UINT8 mod;
	
	speedyCaptureMemFrameBufAddrUpdate();
	sts = speedyCaptureInit(&speedyCaptureCtrl);
	if(sts != SUCCESS){
		ASSERT(0, 1);
	}
	HAL_DramZUpdateClear(0xb0); /* clear CDSP/DISP zf flag */
	mod = speedyCaptureParamGet(VD_CAP_MOD);
	if(mod == SPEEDCAP_CLIP){
		osActiveFuncSet(speedyCaptureCtrl.cbFun.pfnClip);
	}
	else{
		osActiveFuncSet(speedyCaptureCtrl.cbFun.pfnStill);
	}
	speedyCaptureCtrl.sts = SPEEDCAP_STS_RUNNING;
}	

static UINT8 speedyCaptureFillQt(UINT32 dramAddr)
{
	UINT8 qtable[128];

	dramAddr+=12;
	
	jpegYQTableGen( (SINT32)QTABLEVALUE, qtable );
	jpegUVQTableGen( (SINT32)QTABLEVALUE, qtable + 64 );
	HAL_JpegQTableWrite( QTABLEVALUE, qtable, qtable + 64, 1 );
	HAL_DramDmaWrite(qtable, dramAddr, 130, 1);
	
	return SUCCESS;
}

static UINT8 speedyCaptureRecVlcProcess(vlcNode_t *vlcNod)
{
	UINT8 ret;
	UINT32 srcAddr;
	UINT32 hdrSize;
	UINT8 fileName[12];
	UINT32 vlcAddr;

	if(vlcNod==NULL){
		return FAIL;
	}
	if(!vlcNod->vlcSize){
		_VidCapturePintf("0size\n");
		return SUCCESS;
	}	
	hdrSize = 0x253;
	srcAddr = vlcNod->bufAddr +32768;
	vlcAddr = vlcNod->bufAddr +32768 +(0x254>>1);
	
	if(512L > DOS_GetDiskSpace()){// remain 512 k for safety
		osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		return SUCCESS; //que size -1, maybe next is ok
	}

	ret = HAL_GprmDramDma(vlcAddr<<1,(vlcAddr<<1)-1,vlcNod->vlcSize,1);
	if(ret!=SUCCESS){
		ASSERT(0, 1);
		return FAIL;
	}
	ret = dcfFileIndexUpdate();
	if(ret != TRUE ){
		ASSERT(0, 1);
		return FAIL;
	}	
	dcfMediaNameGet(fileName, dcfFileIndexGet(),MEDIA_JPG);
	fileName[11] = 0x00;
	
	dramFileSave( fileName, srcAddr, vlcNod->vlcSize + hdrSize);
	dcfNewFileDramFdbParameterUpdate(K_DCF_JPG_TYPE, vlcNod->vlcSize +hdrSize);

	_VidCapturePintf("OK\n");
	return SUCCESS;
}

static UINT8 speedyCaptureStillVlcProcess(UINT16 width,UINT16 height,vlcNode_t vlcData)
{
	UINT8 ret;
	UINT32 srcAddr;
	UINT32 tmpAddr;
	UINT32 thubAddr;
	UINT32 thubSize;
	UINT32 hdrSize;
	UINT8 opt = 0x04; //K_File_DataType_YUV422
	UINT8 fileName[12];
	exifJpgFileInfo_t jpgInfo;
	UINT8 qTable[128];
	UINT8 i;
	UINT32 vlcSize = vlcData.vlcSize; 
	
	if(!vlcSize){
		_VidCapturePintf("0size\n");
		return SUCCESS;
	}	
	
	srcAddr = vlcData.bufAddr +(UINT32)32768;
	tmpAddr = speedyCaptureMemAddrGet(VID_MEM_IDX_VIDEO_REC_FRAG);
	speedyCaptureFillQt(srcAddr);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(srcAddr);
	HAL_DmaDo(0, 1, 512, 0, 0);
	
	for (i = 0; i < 64; i++) {
		qTable[i] = XBYTE[0x3819 + i];
	}
	for (i = 64; i < 128; i++) {
		qTable[i] = XBYTE[0x381A + i];
	}
	jpegQTableSet( 0, qTable, qTable + 64, 1 );
	ret = HAL_JpegDecode(srcAddr+126,width,height,tmpAddr,2,opt,0); //640*480
	if(ret != SUCCESS ){
		_VidCapturePintf("Jpg Err:%u\n",__LINE__);
		return FAIL;
	}
	
	thubAddr = tmpAddr +(UINT32)160*120;
	jpegQTableSet(QTABLEVALUE, NULL, NULL, 1); 
	HAL_JpgZFactorSet( 100, 100, 160, 120, 160, 120, 0);
	HAL_JpegEncode( tmpAddr, thubAddr, 0,0x20, 1 );
	HAL_JpegReadEncodeSize( &thubSize); 
	
	/* init exif information */
	jpgInfo.mainVlcAddrW = srcAddr+126;
	jpgInfo.mainQidx = QTABLEVALUE;
	jpgInfo.mainWidth = width;
	jpgInfo.mainHeight = height;

	jpgInfo.qvVlcAddrW = 0;

	jpgInfo.thbVlcAddr = thubAddr;
	jpgInfo.thbSize = thubSize;
	jpgInfo.thbWidth = 160;
	jpgInfo.thbHeight = 120;
	jpgInfo.thbQidx = QTABLEVALUE;
	jpgInfo.dbgInfoAddr = 0;
	jpgInfo.rotOpt = 1 ;
	
	hdrSize = exifCreateFile(&jpgInfo);
	if(512L > DOS_GetDiskSpace()){// remain 512 k for safety
		osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		return FAIL;
	}
		
	ret = dcfFileIndexUpdate();
	if(ret != TRUE ){
		ASSERT(0, 1);
		return FAIL;
	}	
	dcfMediaNameGet(fileName, dcfFileIndexGet(),MEDIA_JPG);
	fileName[11] = 0x00;
	dramFileSave( fileName, srcAddr+126 - (hdrSize >> 1), vlcSize + (UINT32)hdrSize);
	dcfNewFileDramFdbParameterUpdate(K_DCF_JPG_TYPE, vlcSize + (UINT32)hdrSize);

	return SUCCESS;
}


UINT8 speedyCaptureAbortMjpg()
{
	ACTIVE_FUNC_CLR();
	HAL_JpegMjpgAbort();
	speedyCaptureEofIntrDisable();
	XBYTE[REG_Dram_VlcRdy] = 3;

	ASSERT(XBYTE[REG_Dram_ClipIdle] !=0, 1);
	
	if(XBYTE[REG_Dram_ClipIdle]==0){
		XBYTE[REG_Jpeg_Swreset]=0x01;    //JPG software reset
		XBYTE[REG_Jpeg_Swreset]=0x00;
	}

	XBYTE[REG_Dram_MpgSb_RegIdx_En] |=0x10; 
	XBYTE[REG_Dram_VlcRegIdx] =0x00; 
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &=0xef; 
	
	XBYTE[REG_Dram_Priority0_lvl2_master] = 0x01; // default value
	XBYTE[REG_Dram_Priority1_lvl2_master] = 0x00;
	HAL_JpegVlcBoundSet(0, 0,0xffffffff);	
	
	return TRUE;

}

UINT8 speedyCaptureStop()
{	
	speedyCaptureCtrl.sts = SPEEDCAP_STS_IDLE;
	speedyCaptureAbortMjpg();
	
	return SUCCESS;
}

UINT8 speedyCapture (void) USING_0
{
	UINT8 i = 1;
	UINT8 ret=SUCCESS;
	UINT8 num;
	vlcNode_t nod;
	vlcNode_t *pNod = (vlcNode_t*)&nod;

	if(speedyCaptureIsStillQueFull(&speedyCaptureCtrl.vlcBufQue)){ // the queue is full
		speedyCaptureStop();
		num = speedyCaptureParamGet(VD_CAP_COUNT);
		if(sp1kSnapParamGet(SP1K_SNAP_PARAM_SHUTTER_SOUND_OPT)){
			sp1kNandRsvRead(0x10, (K_SDRAM_CommonEndAddr<< 1));//load resource
			while(num)
			{
				sp1kAudioPlay(K_SDRAM_CommonEndAddr);//shutter addr
				sp1kHalWait(30);
				num--;
			}
		}
		_VidCapturePintf("size=%bu\n",speedyCaptureCtrl.vlcBufQue.size);
		while(speedyCaptureCtrl.vlcBufQue.size-2){
			pNod = speedyCaptureNodOutQue(&speedyCaptureCtrl.vlcBufQue);	
			ret = speedyCaptureStillVlcProcess(speedyCaptureCtrl.width,speedyCaptureCtrl.height,*pNod);
			if(ret == FAIL){
				break;
			}
			else{
				speedyCaptureCtrl.vlcBufQue.size--;
			}
		}
		if(ret == SUCCESS){
			osMsgPost(SP1K_MSG_MEDIA_VIDO_CAP_FINISH);
		}
		else{
			osMsgPost(SP1K_MSG_MEDIA_VIDO_CAP_ERROR);
		}
	}
	return ret;
}

UINT8 speedyCaptureRec (void) USING_0
{
	UINT8 i = 1;
	UINT8 ret=SUCCESS;
	vlcNode_t nod;
	vlcNode_t *pNod = (vlcNode_t*)&nod;
	UINT8 sts;
	
	speedyCaptureTimeUpdate(&speedyCaptureCtrl);
	if(speedyCaptureIsQueEmpty(&speedyCaptureCtrl.writeBufQue)){
		pNod = speedyCaptureNodOutQue(&speedyCaptureCtrl.writeBufQue);
		ret = speedyCaptureRecVlcProcess(pNod);
		if(ret == SUCCESS){
			ENTER_CRITICAL(sts);
			speedyCaptureCtrl.writeBufQue.size--;
			EXIT_CRITICAL(sts);
		}
	}

	return ret;

}

UINT8 speedyCaptureParamSet(UINT8 paraId, UINT16 value)
{
	if (paraId >= VD_CAP_PARAM_MAX) {
		return FAIL;
	}
	g_speedyCaptureParam[paraId] = value;
	
	return SUCCESS;
}

UINT8 speedyCaptureParamGet(UINT8 paraId)
{
	if (paraId >= VD_CAP_PARAM_MAX) {
		return 0;
	}

	return g_speedyCaptureParam[paraId];
}


UINT8 speedyCaptureStsGet()
{
	return speedyCaptureCtrl.sts;
}

#define speedCaptureStillIntr()\
{\
	if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x03){\
		intrSpeedyCaptureStillBufSet(&speedyCaptureCtrl.vlcBufQue);\
	}\
}

void speedyCaptureClipIntr()
{	
	UINT8 isHit = 0;
	
	++speedyCaptureCtrl.tmrCount;
	if(speedyCaptureCtrl.tmrCount<=2){ //for workround ,fixed ffd9
		if((XBYTE[REG_Dram_VlcRdy]&0x03)==0x03){	//AB ready
			intrSpeedyCaptureClipBufSet(&speedyCaptureCtrl.vlcBufQue,&speedyCaptureCtrl.writeBufQue,0);
		}
	}
	else{
		if((XBYTE[REG_Dram_VlcRdy]&0x03)){	//A ready
			if(speedyCaptureCtrl.tmrCount){
				isHit = !((speedyCaptureCtrl.tmrCount-1)%speedyCaptureCtrl.interCnt);
			}
			if(speedyCaptureCtrl.tmrCount==3){
				isHit = 1;
			}
			if(isHit && (speedyCaptureCtrl.cbFun.pfnLightOpen!=NULL)){
				speedyCaptureCtrl.cbFun.pfnLightOpen();
			}
			else{
				if(speedyCaptureCtrl.cbFun.pfnLightClose!=NULL){
					speedyCaptureCtrl.cbFun.pfnLightClose();
				}
			}
			if(speedyCaptureCtrl.tmrCount>=30){ //xian for safety
				if(speedyCaptureCtrl.dateStampEn){
					videoCaptureStampSetIntr();
				}
				speedyCaptureCtrl.tmrCount=speedyCaptureCtrl.tmrCount - 30;
				//fTFrame = 0; //xian -- not need drop frame
			}
			
			intrSpeedyCaptureClipBufSet(&speedyCaptureCtrl.vlcBufQue,&speedyCaptureCtrl.writeBufQue,isHit);
		}			
	}
}    

void speedyCaptureIntr()
{	
	switch(g_speedyCaptureParam[VD_CAP_MOD]){
		case SPEEDCAP_STILL:
			speedCaptureStillIntr();
		break;
		case SPEEDCAP_CLIP:
			speedyCaptureClipIntr();
		break;
	}
	
}     


