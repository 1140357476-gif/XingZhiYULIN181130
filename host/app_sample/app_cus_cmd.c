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
 * @file      app_cus_cmd.c
 * @brief     customized command line for host
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "app_ui_para.h"
#include "sp1k_video_api.h"
#include "video_dtstmp.h"
#include "cmd.h"
#include "app_msg.h"
#include "sys_mem_init.h"
#include "sp1k_util_api.h"
#include "app_menu_micro.h"
#include "pv_task.h"
#include "video_slclip.h"
#include "dcf_api.h"
#include "fileIndex.h"
#include "sp1k_exif_api.h"
#include "Sp1k_pb_api.h"
#include "Sp1k_hal_api.h"
#include "hal_disp.h"
#include "hal_dram.h"
#include "sp1k_pb_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_timelapse.h"
#include "dcf_api.h"
#include "sp1k_pv_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_video_capture.h"
#include "hal_disp_tp.h"
#include "gui_tp.h"
#include "app_tp_scan.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 extern UINT16 g_vidParam[VD_PARAM_MAX];
 extern UINT8 sp1kHDMI_PlugGet(void);
 extern UINT8 appDispHdmiOpen(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if 0
void videostamptest(UINT8* ptr)
{
	UINT8 mode, recMode, pos, fmt;
    UINT8 moonId = 0;
    UINT16 offXDate;
	uiPara_t* puiPara = appUiParaGet();
    if (strncmp(ptr, "stamp", 5) == 0) {
        cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);

        mode = *ptr - '0'; //0~2 : date, img, img_date
        ptr ++;
        recMode = (*ptr - '0');    //0~3 : all, first, end, first_end
        ptr += 2;
        offXDate =*ptr - '0';
        ptr ++;
        offXDate = offXDate*10 + *ptr - '0';
        //printf("offXDate:%u\n", offXDate);
        ptr ++;

		ptr ++;
       	pos =*ptr - '0';
        ptr ++;
        pos = pos*16 + *ptr - '0';		//00:up_left, 22:bot_right
        ptr ++;

        //mode = VD_STAMP_MODE_IMG_DATE;
        //recMode = VD_STAMP_REC_MODE_FST_END_FRAME;

        //pos = VD_STAMP_RIGHT | VD_STAMP_BOTTOM;
        fmt = VD_STAMP_FMT_MMDDYYYY | VD_STAMP_FMT_HHMMSS ;

        if(*ptr){
            fmt |= VD_STAMP_FMT_STR_WITH_DATE;
            sp1kDateStampCustomStrSet(ptr);
        }

        sp1kVideoRecStampCfg(mode, pos, recMode);
        sp1kVideoRecStampDateFmtSet(fmt);
        sp1kVideoRecParamSet(VD_STAMP_DATETIME_DIS_X_RATIO, offXDate);
        if(mode == VD_STAMP_MODE_IMG)
        	sp1kVideoRecStampImgResLoad(0x2b, 32, 32);
        else if(mode == VD_STAMP_MODE_IMG_DATE){
			//company logo
            if(puiPara->VideoSize == MOV_SIZE_QVGA)
    			sp1kVideoRecStampImgResLoad(0, 320, 16); //back ground image
    		else if(puiPara->VideoSize == MOV_SIZE_HD)
			{
			    if(K_SDRAM_TotalSize>=0x800000)
					sp1kVideoRecStampImgResLoad(0xb8, 640, 48); //back ground image
				else
					sp1kVideoRecStampImgResLoad(0xb8, 640, 48);
			}
    		else if(puiPara->VideoSize == MOV_SIZE_D1)
    			sp1kVideoRecStampImgResLoad(0xb9, 480, 32);
			else
    		    sp1kVideoRecStampImgResLoad(0xb9, 480, 32);
        }
		else
			{
			     sp1kVideoRecStampImgResLoad(0, 640, 32);
			}

        sp1kVideoRecStampStsSet(1);

        return;
    }

}
#endif

#if 0
/*motion detect test*/
void cmdMDTest(UINT8* ptr)
{
	UINT32 arg[8] = {0};
	cmdArgGet(&ptr, arg, 8, 1);

	if(sysMemTotSizeGet() <= DEV_MEM_SIZE_64M)
	{
		printf("Dram too samll!");
		return;
	}

	if (strncmp((UINT8*)arg[0], "mdon", 4) == 0)
	{
		sp1kMDEnSet(1);
		sp1kMDLearningStart(1);
		sp1kMDSenSet(0);
		printf("MD!\n");
	}
	else if (strncmp((UINT8*)arg[0], "mdoff", 5) == 0)
	{
		sp1kMDEnSet(0);
	}

	else if (strncmp((UINT8*)arg[0], "senset", 5) == 0)
	{

		sp1kMDEnSet(1);
		sp1kMDLearningStart(1);
		sp1kMDSenSet(arg[1]);
	}
}

#endif
#if 0
videoSeamlessLPRectest(UINT8* ptr)
{
   UINT32 arg[6];
   UINT32 filesize,rectime;
  UINT16 slrecmode;
//   UINT8 i;
   UINT16 pvHSize,pvVSize;
   UINT16 recwidth,recheight;
   if (strncmp(ptr, "slrec", 5) == 0)
   	{
   	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	cmdArgValueGet(ptr, arg, 6);
	filesize=(UINT32)arg[0]*arg[1]*arg[2]*arg[3];
	slrecmode=(UINT16)arg[5];
	rectime=(UINT32)arg[4];
	printf("arg=%ld\n",filesize);
        printf("recmode=%d\n",slrecmode);
       printf("rectime=%ld\n",(UINT32)arg[4]);
	sp1kVideoSeamlessLPRecStsSet(1);
	sp1kVideoRecParamSet(VD_SLREC_MODE, slrecmode); //set date stamp OffsetX

	if(g_vidParam[VD_SLREC_MODE]==0){
		sp1kVideoSeamlessLPRecFilesizeSet(filesize);
	}
	else{
		sp1kVideoSeamlessLPRecTimeSet(rectime);
	}
	#if 0
	pvSizeGet(& pvHSize, & pvVSize);
	//sysMemFrameBufAddrUpdate(UINT8 size)
	if(pvHSize<=640){
		sysMemFrameBufAddrUpdate(MEM_FB_VGA);
	}
	else if(pvHSize == 960 && pvVSize == 540){
		sysMemFrameBufAddrUpdate(MEM_FB_HD_960);
	}
	else if((pvHSize>640)&&(pvHSize<1280)){
		sysMemFrameBufAddrUpdate(MEM_FB_D1);
	}
	else {
		sysMemFrameBufAddrUpdate(MEM_FB_HD);
		printf("here\n");
	}
		/* if((puiPara->VideoSize==MOV_SIZE_1080P)){
			 sysMemFrameBufAddrUpdate(MEM_FB_1080p);
		}*/
	sp1kVideoRecSizeGet(&recwidth,&recheight);
	if(recwidth==1920 && recheight==1088 ){
		sysMemFrameBufAddrUpdate(MEM_FB_1080p);
	}
#endif
//appVideoViewInit(SP1K_VIDEO_VIEW_INIT_FULL);
   	}
}
#endif
#if 0
videoSeamlessLPRectest(UINT8* ptr,UINT32 arg[])
{
   //UINT32 arg[6];
   extern UINT32 g_brcTargetSize;
   UINT32 filesize = arg[1]*arg[2]*arg[3];//*arg[4];
   UINT32 rectime = arg[1];
  UINT16 slrecmode = arg[0];
  if(arg[4]==1024)
  	{
  	g_brcTargetSize = arg[4]*arg[5];
  	}
	//UINT8 name[12];
	//dcfGetFileName(name,DOS_FileIndexGet(1));
	//dcfGetFileName(name,DOS_FileIndexGet(2));
	//dcfGetFileName(name,DOS_FileIndexGet(3));
	//dcfDeleteFile(DOS_FileIndexGet(1),0);
	//dcfDeleteFile(DOS_FileIndexGet(2),1);
	//dcfDeleteFile(DOS_FileIndexGet(123),1);
	printf("%u,%lu,%lu,%lu\n",slrecmode,filesize,rectime,g_brcTargetSize);
	sp1kVideoSeamlessLPRecStsSet(1);
	sp1kVideoRecParamSet(VD_SLREC_MODE, slrecmode); //set date stamp OffsetX

	if(g_vidParam[VD_SLREC_MODE]==0){
		sp1kVideoSeamlessLPRecFilesizeSet(filesize);
	}
	else{
		sp1kVideoSeamlessLPRecTimeSet(rectime);
	}
	#if 0
	pvSizeGet(& pvHSize, & pvVSize);
	//sysMemFrameBufAddrUpdate(UINT8 size)
	if(pvHSize<=640){
		sysMemFrameBufAddrUpdate(MEM_FB_VGA);
	}
	else if(pvHSize == 960 && pvVSize == 540){
		sysMemFrameBufAddrUpdate(MEM_FB_HD_960);
	}
	else if((pvHSize>640)&&(pvHSize<1280)){
		sysMemFrameBufAddrUpdate(MEM_FB_D1);
	}
	else {
		sysMemFrameBufAddrUpdate(MEM_FB_HD);
		printf("here\n");
	}
		/* if((puiPara->VideoSize==MOV_SIZE_1080P)){
			 sysMemFrameBufAddrUpdate(MEM_FB_1080p);
		}*/
	sp1kVideoRecSizeGet(&recwidth,&recheight);
	if(recwidth==1920 && recheight==1088 ){
		sysMemFrameBufAddrUpdate(MEM_FB_1080p);
	}
#endif
//appVideoViewInit(SP1K_VIDEO_VIEW_INIT_FULL);
   //	}
}
#endif

#if 0
void cmdViviLink(UINT8 *ptr)
{
    UINT32 ret32;
    PBFile_t pFileInfo;
    UINT8 viviCode[8] = {'V', 'i', 'v', 'i', 'L', 'n', 'k', 0};

	if(strncmp(ptr, "pbvi", 4) == 0){//modify file
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

		ret32 = atoi(ptr);

		sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(ret32), &pFileInfo);
		//for(ret = 0; ret < 12; ret++){
		//	  printf("%c", pFileInfo.FileName[ret]);
		//}
		//printf("\n");


		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

		viviCode[7] = *ptr - '0';
		sp1kPbEditUserCommentCodeModify(&pFileInfo, viviCode, 8);
	}
	else if(strncmp(ptr, "snvi", 4) == 0){// when snap or edit jpeg file
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

		viviCode[7] = *ptr - '0';
		//printf("%bx\n", viviCode[7]);
		sp1kEXIFUserCommentSet(viviCode, 1);
	}
	else if(strncmp(ptr, "vdvi", 4) == 0){// when video rec
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

		viviCode[7] = *ptr - '0';
		sp1kVideoRecParamSet(VD_USER_COMMENT_CODE0, ((UINT16)viviCode[0] << 8) | (UINT16)viviCode[1]);
		sp1kVideoRecParamSet(VD_USER_COMMENT_CODE1, ((UINT16)viviCode[2] << 8) | (UINT16)viviCode[3]);
		sp1kVideoRecParamSet(VD_USER_COMMENT_CODE2, ((UINT16)viviCode[4] << 8) | (UINT16)viviCode[5]);
		sp1kVideoRecParamSet(VD_USER_COMMENT_CODE3, ((UINT16)viviCode[6] << 8) | (UINT16)viviCode[7]);
	}
	else if(strncmp(ptr, "disvi", 5) == 0){
		sp1kEXIFUserCommentSet(0, 0);

		sp1kVideoRecParamSet(VD_USER_COMMENT_CODE0, 0);
		sp1kVideoRecParamSet(VD_USER_COMMENT_CODE1, 0);
		sp1kVideoRecParamSet(VD_USER_COMMENT_CODE2, 0);
		sp1kVideoRecParamSet(VD_USER_COMMENT_CODE3, 0);
	}
}
#endif

#if 0
void cmdPbCropTest( UINT8* ptr )
{
    PBFile_t pFileInfo;
	UINT16 fileIdx;
	UINT32 ImgX0, ImgX1, ImgY0, ImgY1;
	UINT32 addr;
	UINT16 pw,ph;
	PbCropImage_t crop;
	if(strncmp(ptr, "cropsave", 8) == 0){
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
		fileIdx = atoi(ptr);

		sp1kPbFileInfoGet(sp1kPbFileIndexByOrderGet(fileIdx), &pFileInfo);
		HAL_DispImgXyGet(&ImgX0, &ImgY0, &ImgX1, &ImgY1);
		HAL_DispPbFrameBufferGet(&addr,&pw,&ph);

		crop.startX = ImgX0;
		crop.startY = ImgY0;
		crop.srcWidth = pw;
		crop.srcHeight = ph;
		crop.recWidth = ImgX1 - ImgX0;
		crop.recHeight = ImgY1 - ImgY0;
		crop.srcAddrW = addr;

		printf("pw=%d,ph=%d\n",pw,ph);
		printf("startX=%d,startY=%d,recW=%d,recH=%d,addr=%lx\n",crop.startX,crop.startY,crop.recWidth,crop.recHeight,crop.srcAddrW);
		pbEdit_ImgDec(PB_CROP, &crop);
		pbEdit_Save(0x1000, crop);
	}
}
#endif
void video_test(UINT8* pt);
#if 0
void bufModeTest(UINT8* ptr){
	UINT32 arg;
	//cmdTokenStrip(&ptr);
	//cmdWhiteSpaceStrip(&ptr);
	cmdArgValueGet(ptr, &arg, 1);
	printf("**arg=%ld\n",arg);
	if(arg==4){
		printf("^^^4\n");
		sp1kVideoRecFrameBufNumSet(4);
	}else if(arg==3){
		printf("^^^3\n");
		sp1kVideoRecFrameBufNumSet(3);
	}
	appVideoViewInit(SP1K_VIDEO_VIEW_INIT_FULL);
}
#endif
//#include "sp1k_rsvblk_api.h"
//extern void sfBitModeSet(UINT8 bitMode);
//extern UINT8 gpsMode;
#if 0 // for uvc extension unit cmd test
#include "sp1k_uvc_api.h"
static UINT8 curVal = 0;
UINT16 uvcPreIn(pusbVNDCb_t pCb) USING_1
{
	UINT8 *pt8;
	UINT16 sizeAvailable = 0;
	UINT32 i = 0;

	if(pCb->pData){
		pt8 = pCb->pData;
	}
	switch( (pCb->cmd.wValue&0xff00)>>8 ){
		case  7:
			switch( pCb->cmd.bRequest )
			{
				case GET_CUR:
					pt8[0] = curVal;
					sizeAvailable = 1;
					break;
				case GET_MIN:
					pt8[0] = 0x00;
					sizeAvailable = 1;
					break;
				case GET_MAX:
					pt8[0] = 0x05;
					sizeAvailable = 1;
					break;
				case GET_RES:
					pt8[0] = 0x01;
					sizeAvailable = 1;
					break;
				case GET_INFO:
					pt8[0] = 3;
					sizeAvailable = 1;
					break;
				case GET_DEF:
					pt8[0] = 0x00;
					pt8[1] = 0x00;
					sizeAvailable = 1;
					break;
				case GET_LEN:
					pt8[0] = 0x01;
					pt8[1] = 0x00;
					sizeAvailable = 2;
					break;
			}
			break;
		case 6:
			switch( pCb->cmd.bRequest )
			{
				case GET_CUR:
					for (i = 0; i < 8; i++){
						pt8[i] = 'A' + i;
					}
					sizeAvailable = 8;
					break;
				case GET_MIN:
					pt8[0] = 0x00;
					sizeAvailable = 1;
					break;
				case GET_MAX:
					pt8[0] = 0xFF;
					sizeAvailable = 1;
					break;
				case GET_RES:
					pt8[0] = 0x01;
					sizeAvailable = 1;
					break;
				case GET_INFO:
					pt8[0] = 0x03;
					sizeAvailable = 1;
					break;
				case GET_DEF:
					pt8[0] = 0x00;
					sizeAvailable = 8;
					break;
				case GET_LEN:
					pt8[0] = 0x08;
					pt8[1] = 0x00;
					sizeAvailable = 2;
					break;
			}
			break;
		default:
			printf("not supported value\n");
			break;
	}
	printf("PreIn cmd : t=0x%bx, r=0x%bx, v=0x%x, i=0x%x, l=0x%x, sz=0x%x\n",
		pCb->cmd.bmRequestType, pCb->cmd.bRequest, pCb->cmd.wValue, pCb->cmd.wIndex, pCb->cmd.wLength, sizeAvailable);
	return sizeAvailable;
}

UINT16 uvcPostOut(pusbVNDCb_t pCb) USING_1
{
	UINT8 *pt8;
	UINT16 sizeAvailable = 0;
	printf("PostOut cmd : t=0x%bx, r=0x%bx, v=0x%x, i=0x%x, l=0x%x\n",
		pCb->cmd.bmRequestType, pCb->cmd.bRequest, pCb->cmd.wValue, pCb->cmd.wIndex, pCb->cmd.wLength);

	if(pCb->pData){
		pt8 = pCb->pData;
		printf("data: 0x%bx, 0x%bx, 0x%bx, 0x%bx\n",
			*pt8, *(pt8+1), *(pt8+2), *(pt8+3));
	}

	switch( (pCb->cmd.wValue&0xff00)>>8 ){
		case  7:
			switch( pCb->cmd.bRequest )
			{
				case SET_CUR:
					curVal = pt8[0];
					sizeAvailable = 1;
					break;
			}
			break;

		default:
			printf("not supported value\n");
			break;
	}
	return 0;
}

usbcbVNDRFunc_t code usbcb[] =
{
	uvcPreIn,
	uvcPostOut
};
#endif

void cmdCustom(UINT8* ptr)
{
#if 0 // for uvc extension unit cmd test
	if(strncmp(ptr, "uvc", 3) == 0)
	{
		sp1kUvcUserExtUnitPreInCallBackSet(uvcPreIn);
		sp1kUvcUserExtUnitPostOutCallBackSet(uvcPostOut);
	}
#endif

#if 0
	if(strncmp(ptr, "slow", 4) == 0)
	{
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
		//printf("slow %bx\n", atoi(ptr));
		//sp1kVideoPbSlowFWSpeedSet(atoi(ptr));
		sp1kVideoPbSlowFWSpeedSet(*ptr - '0');
	}
#endif

	if(strncmp(ptr, "snap", 4) == 0)
	{
		printf("hw snap\n");
		sp1kUsbIntrDataTx(0,1);
	}
	
#if 0
	if(strncmp(ptr, "hdin", 4) == 0)
	{
		printf("hd in\n");

		osMsgPost(SP1K_MSG_HDMI_PLUG_IN);
	}
	if(strncmp(ptr, "hdot", 4) == 0)
	{
		printf("hd out\n");
		osMsgPost(SP1K_MSG_HDMI_PLUG_OUT);
	}

	if(strncmp(ptr, "gps", 3) == 0)
	{
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
		gpsMode = *ptr - '0';
		printf("gpsMode:%bx\n", gpsMode);
	}
#endif

	#if 0
	UINT32 AddrWord = sysMemTotSizeGet() - 0x4000;
	UINT32 idx,offset,size,cnt;
	UINT8 LSB,MSB,bitmode,sts = FAIL;
	UINT8 str1[] = "bit mode is";
	//UINT8 str2[] = "GD_SPI FLASH";
	//UINT8 str3[] = "SPI Command Err!";
	idx = 0x802;
	if(strncmp(ptr, "spi1bit", 7) == 0)
	{
		sfBitModeSet(0);
		bitmode = 1;
	}
	else if(strncmp(ptr, "spi2bit", 7) == 0)
	{
		sfBitModeSet(1);
		bitmode = 2;
	}
	else if(strncmp(ptr, "spi4bit", 7) == 0)
	{
		sfBitModeSet(2);
		bitmode = 4;
	}
	else
	{
		printf("\nSPI Command Err!\n");
		return;
	}
	sp1kNandRsvSizeGet(idx, &offset, &size);
	printf("Idx=0x%lx, offset=0x%lx, size=0x%lx\n",idx,offset,size);
	sts = sp1kNandRsvRead(idx, AddrWord << 1);
	if(sts == TRUE)
	{
		printf("Continue...\n");

		HAL_DramStartAddrSet(AddrWord, 1);
		for(cnt = 0; cnt < 6; cnt++)
		{
			HAL_DramWordDataRead(&LSB,&MSB);
			putchar(LSB);
			putchar(MSB);
		}

		HAL_DramStartAddrSet(AddrWord+6, 1);
		for(cnt = 0; cnt < 0x80; cnt++)
		{
			HAL_DramWordDataRead(&LSB,&MSB);
			if((LSB != 0x55) || (MSB != 0xAA))
			{
				sts = FALSE;
				break;
			}
		}
	}
	else
		printf("GD_SPI FLASH");
	if(sts == TRUE)
		printf(" %bu%s OK!\n",bitmode,str1);
	else
		printf(" %bu%s Err!\n",bitmode,str1);

	#endif
	#if 0
	if(strncmp(ptr, "cap", 4) == 0){

		sp1kVideoRecParamSet(VD_CAPTURE,1);
		sp1kVideoCapParamSet(SP1K_VD_CAP_COUNT,3);
		sp1kVideoCapParamSet (SP1K_VD_CAP_SIZE,VD_CAP_VGA);
		sp1kVideoCapParamSet(SP1K_VD_CAP_MOD,SP1K_SPEEDCAP_CLIP);
		speedyCaptureStart();
	}
	#endif
	#if 0
	UINT8 hdmi_in= 1;

	if(strncmp(ptr, "hdmi", 4) == 0)
	{
		if(sp1kHDMI_PlugGet() == hdmi_in)
		{
			osMsgPost(SP1K_MSG_STATE_TO_STILL_VIEW);
			appDispHdmiOpen();
			osMsgPost(SP1K_MSG_STATE_INIT);
			sp1kHalCtrlScreen(3);

			osMsgPost(SP1K_MSG_STATE_TO_VIDEO_VIEW);
			sp1kSuperPreviewSet(1,1);
		}
	}
	#endif
	#if 0
	if(strncmp(ptr, "mms", 3) == 0){
		sp1kSnapParamSet(SP1K_SANP_PARAM_MMS_STS, 1);
		sp1kVideoRecParamSet(VD_MMS,1);
	}

	#endif
	#if 0
	UINT32 rate;
	if(strncmp(ptr, "slow", 4) == 0){
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        rate = atoi(ptr);
		sp1kVideoRecParamSet(VD_SLOW_REC_MOD,1);
	    sp1kVideoRecParamSet( VD_SLOW_REC_FPS,rate);
	}else if(strncmp(ptr,"pbSlow",6) == 0){
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

        rate = atoi(ptr);
		sp1kVideoRecParamSet(VD_SLOW_PLAY_RATE,rate);
	}
	#endif

	#if 0  //timelapse
	/*UINT16 v1, v2;
	UINT16 v1;

	UINT32 fps;
	if(strncmp(ptr, "fps", 3) == 0){
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        fps = atoi(ptr);
		timelapse_ParamSet(TIMELAPSE_PAR_FPS,fps);
		return ;
	}
	else if(strncmp(ptr, "gap", 3) == 0){
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        v1 = atoi(ptr);
		//printf("gap=%d\n",v1);
		timelapse_ParamSet(TIMELAPSE_PAR_SNAPGAP, v1);
	}*/
	UINT16 fps;
	if(strncmp(ptr, "fps", 3) == 0){
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        fps = atoi(ptr);
		sp1kTimelapse_ParamSet(TIMELAPSE_PAR_FPS,fps);
	}else if(strncmp(ptr, "lps1", 4) == 0){
		dcfDir1NameSet(TIMELAPSE_VIDEOFOLDER,"100TLVID");
		sp1kSnapParamSet(SP1K_SNAP_TIMESLAPE_STS, 1);
		dcfAviNameSet("WGI");
	}//else if(strncmp(ptr, "tlsp", 4)== 0){
	//	sp1kSnapParamSet(SP1K_SNAP_TIMESLAPE_PAUSE,1);
	//}else if(strncmp(ptr, "tlsr", 4 )== 0){
	//	sp1kSnapParamSet(SP1K_SNAP_TIMESLAPE_PAUSE,0);
	//}

	#endif
	#if 0
		video_test(ptr);
	#endif
	#if 0
	UINT32 addr, size;
	UINT16 len;

	if(strncmp(ptr, "cu", 2) == 0){//modify file
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

		len = atoi(ptr); // include itself

		addr = vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_END);
		size = vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_END);
		printf("free:0x%lx, 0x%lx, %u\n", addr, size, len);
		if(size > len){
			// the data len include itself length is saved in the begining of dram
			sp1kHalDRAMSetStartAddr(addr, 0);
			sp1kHalDRAMWriteWord(READ8(len,1), READ8(len,0));

			sp1kVideoCustomUpdate(addr);
		}
	}
	#endif
#if 0//FUNC_HOST_TOUCH_PANEL
	else if(strncmp(ptr , "calb", 4) == 0)
	{
	    tpCalibrationEnter();
  	}
  	else if(strcmp(ptr , "tpwt") == 0)
  	{
		tpCalDatSave(0xfb, K_SDRAM_CommonEndAddr);
		printf("wt ok\n");
	}
	else if(strcmp(ptr , "recon") == 0)
	{
		HAL_TpReCfg();
  	}
	else if(strcmp(ptr , "chek") == 0)
	{
	    HAL_TpPenupChek();
  	}
#endif
	#if 0
	UINT32 arg[8];
	UINT8 ret = 0;
	extern UINT8 G_VideoVlcBufNum;
	cmdArgGet(&ptr, arg, 8, 1);
	#else
	ptr = ptr;
	#endif

	#if 0
	sp1kpbCurFoldFileSpaceGet(&fileSize,0x01);
	printf("Total filesize=%ld\n",fileSize);
	 fileSize =0;
	sp1kpbCurFoldFileSpaceGet(&fileSize,0x02);
	printf("Total filesize=%ld\n",fileSize);
	 fileSize =0;
	sp1kpbCurFoldFileSpaceGet(&fileSize,0x03);
	printf("Total filesize=%ld\n",fileSize);
	 fileSize =0;
	sp1kpbCurFoldFileSpaceGet(&fileSize,0x04);
	printf("Total filesize=%ld\n",fileSize);
	#endif

	#if 0
	cmdMDTest(ptr);
	#endif

	#if 0 //video stamp test
	videostamptest(ptr);
	#endif
	#if 0
	printf("--%bu\n",G_VideoVlcBufNum);
	videoSeamlessLPRectest(ptr,arg);
	#endif

	#if 0
	cmdViviLink(ptr);
	#endif

	#if 0
	cmdPbCropTest(ptr);
	#endif
	#if 0
	bufModeTest(ptr);
	#endif
}
