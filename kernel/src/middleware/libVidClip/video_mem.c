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
 * @file	  video_slclip.c
 * @brief	  video  memory initialization related function
 * @author	  rf.wang
 * @since	  2011-07-29
 * @date	  2011-07-29
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
 #include "video_def.h"
 #include "initio.h"
 #include "sp1k_video_api.h"
 //#include "sp1k_video_api.h"
// #include "sys_mem_init.h"

#define __FILE __FILE_ID_VIDEO_MEM__
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
//UINT32 VID_TOTAL_FRAGSIZE = ((UINT32)2*(UINT32)1024 * (UINT32)224);
//UINT32 FRAG_BUF_SIZE = ((UINT32)1024 * (UINT32)224);
//UINT32 FRAGBUFCNT;
//UINT32 SEC_PER_FRAG_BUG	;
UINT8 frameBufNum=2;
UINT8 frameBufNumMode=2;
// extern appVidClipCtrl_t xdata vidClipCtrl;
UINT8 G_VideoVlcBufNum =VID_VLCBUFNUM_TOTAL;
static vidMemCb_t xdata vidMemCb[VID_MEM_IDX_MAX];

//extern memCb_t xdata sgMemCb/*[MEM_IDX_MAX]*/;
 /**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
   /**
 * @fn              vidRecFrameBufNumSet()
 * @brief	    set frame buf num
 * @param[in]    NONE
 * @retval          NONE
 * @see
 */
void vidRecFrameBufNumSet(UINT8 num)
{
 	 frameBufNum = num;
}
  /**
 * @fn               vidRecFrameBufNumGet()
 * @brief	      Get frame buf num
 * @param[in]    NONE
 * @retval          NONE
 * @see
 */
UINT8 vidRecFrameBufNumGet(void)
{
 	return frameBufNum;
}

  /**
 * @fn               vidRecMemAddrSet()
 * @brief	      set addr 
 * @param[in]    NONE
 * @retval          NONE
 * @see
 */
void vidRecMemAddrSet(
	vidMemCb_t* vidpMemCb,
	vidmemCbIdx_t sIdx,
	vidmemCbIdx_t eIdx,
	UINT8 mode,
	UINT8 bAgn
)
{
	UINT8 i;
	//static UINT32 LastAddr=0;
	//UINT32 LastAddr=0;
	
	if (!vidpMemCb || sIdx > eIdx) {
		return;
	}
	
	for (i = sIdx + 1; i <= eIdx; i++) {
		if (mode == VID_MEM_GROWTH_UP) {
			/* growth up */
			vidpMemCb[i].addr = vidpMemCb[i - 1].addr + vidpMemCb[i - 1].size;
		} else {
			/* growth down */
			vidpMemCb[i].addr = vidpMemCb[i - 1].addr - vidpMemCb[i].size;
		}

		/* alignment to 8 words */
		if (bAgn) {
			vidpMemCb[i].addr = (vidpMemCb[i].addr + 7) & ~7;
		}
	}
}

  /**
 * @fn               void vidMemSizeInit()
 * @brief	    video  memory initialization
 * @param[in]    NONE
 * @retval          NONE
 * @see
 */
void vidRecMemSizeInit(void)
{
	UINT8 i;
//	UINT8 vidRecFrameBufNum;
	vidMemCb_t xdata* vidpMemCb = &vidMemCb[0];
	/* video record mode */
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136);
	for(i = 4; i < VID_VLCBUFNUM_TOTAL; i ++){
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A+i,0);
	}
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_QTAB,(UINT32)65*10+4);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_AUDIO,(UINT32)128* (UINT32)1024);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_FRAG,(vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size*2 + 1024ul*32));
	//printf("frag=%ld\n",vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_FRAG));
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_HDR,(UINT32)256);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_StampA,(UINT32) 720* 48);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_StampB,(UINT32) 720* 48);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_StampC,(UINT32) 720* 48);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_StampD,(UINT32) 720* 48);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_Font,(UINT32) 24*48*21);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_Text,(UINT32) 24*48*20*4);	
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_IMG,(UINT32) 720* 48);
	vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_IDX, (UINT32) FRAG_SIZE_IDX/2 * 3);

	 //FRAGBUFCNT=(VID_TOTAL_FRAGSIZE /FRAG_BUF_SIZE);
	 //SEC_PER_FRAG_BUG	=(FRAG_BUF_SIZE/((UINT32)CHUNKSIZE));
}
 /**
 * @fn               vidRecMemAddrInit()
 * @brief	    video  memory addr initialization
 * @param[in]    NONE
 * @retval          NONE
 * @see
 */
void vidRecMemAddrInit(void)
{
  	vidMemCb_t xdata* vidpMemCb = &vidMemCb[0];
	vidpMemCb[VID_MEM_IDX_VIDEO_REC_START].addr = K_SDRAM_CommonEndAddr;
  	vidRecMemAddrSet(vidpMemCb, VID_MEM_IDX_VIDEO_REC_START, VID_MEM_IDX_VIDEO_REC_END, VID_MEM_GROWTH_UP, 1);
  	
}
 
void vidRecMemFrameBufSizeUpdate(void)
{
	UINT8 i;
	UINT16 recHsize,recVsize;
	//UINT8 vidRecFrameBufNum;
	vidMemCb_t xdata* vidpMemCb = &vidMemCb[0];
	
	vidRecMemSizeInit();
	sp1kVideoRecSizeGet(&recHsize,&recVsize);
	if ((recHsize==640 && recVsize==480)||(recHsize==320 && recVsize==240)){		
		#if 0
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/12+136);
		#endif
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/8+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/8+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/8+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,(UINT32)K_VIDEOCLIP_WIDTH_VGA* K_VIDEOCLIP_HEIGHT_VGA/8+136);
	}
	else if (recHsize==720 && recVsize==480){//(recHsize==848 && recVsize==480){ //modify for D1 size
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,(UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/6+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,(UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/6+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,(UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/6+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,(UINT32)K_VIDEOCLIP_WIDTH_D1* K_VIDEOCLIP_HEIGHT_D1/6+136);
	}
	else if (recHsize==1920 && recVsize==1088){
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,(UINT32)K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p /12+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,(UINT32)K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p /12+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,(UINT32)K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p /12+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,(UINT32)K_VIDEOCLIP_WIDTH_1080p * K_VIDEOCLIP_HEIGHT_1080p /12+136);
	}
	else {
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/10+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_B,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/10+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_C,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/10+136);
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_D,(UINT32)K_VIDEOCLIP_WIDTH* K_VIDEOCLIP_HEIGHT/10+136);
	}
	for(i = 4; i < VID_VLCBUFNUM_TOTAL; i ++){
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_VLC_A+i,0);
	}
	if(vidRecFrameBufNumGet()==3){
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_FRAG,(vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size*3 + 1024ul*32));
	}else{
		vidRecMemSizeSet(VID_MEM_IDX_VIDEO_REC_FRAG,(vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A].size*4 + 1024ul*32));
	}
}
   /**
 * @fn       vidRecMemFrameBufAddrUpdate()
 * @brief	    video  memory addr update
 * @param[in]    NONE
 * @retval          NONE
 * @see
 */
void vidRecMemFrameBufAddrUpdate(void)
{  	
  	//memCb_t xdata* pMemCb = &sgMemCb[0];
	vidMemCb_t xdata* vidpMemCb = &vidMemCb[0];
	UINT8 vlcBufNum, vlcBufTotal=VID_VLCBUFNUM_TOTAL, cnt,pvFrameBufNum;
	UINT32 vlcBufSize;
	UINT8 /*vidRecFrameBufNum,*/i;
	UINT32 regularsize=0;
	UINT32 endAddr;
	//UINT16 pvHsizeQ,pvVsizeQ;
	vidRecMemFrameBufSizeUpdate();
	vidpMemCb[VID_MEM_IDX_VIDEO_REC_START].addr = K_SDRAM_CommonEndAddr;
	vlcBufSize=vidRecMemSizeGet(VID_MEM_IDX_VIDEO_REC_VLC_A);
	for(i=VID_MEM_IDX_VIDEO_REC_StampA;i<=VID_MEM_IDX_VIDEO_REC_FRAG;i++){
		regularsize+=vidRecMemSizeGet(i);
	}
	regularsize += 1024; //reserved size for free buffer
	
	pvFrameBufNum=vidRecFrameBufNumGet();
	vidPrintfMem("bufn:%bu\n", pvFrameBufNum);
	if(pvFrameBufNum==4){		
		endAddr = sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_D);
	}
	else if(pvFrameBufNum==3){
		endAddr = sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_C);
	}
	else{
		endAddr = sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_B);
	}
	vlcBufNum=vidRecVlcBufNumGet(vlcBufSize,K_SDRAM_CommonEndAddr+regularsize,endAddr);
	
	for(cnt = 0; cnt <vlcBufNum; cnt ++){
		vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A+cnt].size = vlcBufSize;
	}
	for(; cnt < vlcBufTotal; cnt ++){
		vidpMemCb[VID_MEM_IDX_VIDEO_REC_VLC_A+cnt].size = 0;
	}
	G_VideoVlcBufNum =vlcBufNum;
	printf("vlcNum:%bu, %lx, %lx\n",vlcBufNum, vlcBufSize, K_SDRAM_TotalSize);
	printf("YUV buf mode:%bx\n",pvFrameBufNum);
	//pvSizeGet( &pvHsizeQ,&pvVsizeQ);
	//printf("pvHsize=%d,pvVsize=%d\n",pvHsizeQ,pvVsizeQ);
	vidPrintfMem("vlcNum:%bu, %lx, %lx\n",vlcBufNum, vlcBufSize, K_SDRAM_TotalSize);
	vidPrintfMem("pv:%lx, %lx\n",sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_B),  K_SDRAM_CommonEndAddr);
	//vidpMemCb[VID_MEM_IDX_VIDEO_REC_END].addr = (UINT32)sysMemAddrGet(MEM_IDX_VIDEO_VIEW_PV_D);
	vidRecMemAddrSet(vidpMemCb,VID_MEM_IDX_VIDEO_REC_START, VID_MEM_IDX_VIDEO_REC_END, VID_MEM_GROWTH_UP, 1);

	vidpMemCb[VID_MEM_IDX_VIDEO_REC_END].size = endAddr - vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_END);
	
	for(i=VID_MEM_IDX_VIDEO_REC_StampA;i<=VID_MEM_IDX_VIDEO_REC_VLC_A;i++){
		vidPrintfMem("[%bu] a:%lx, s:%lx\n",i, vidRecMemAddrGet(i),vidRecMemSizeGet(i));
	}
	for(i=VID_MEM_IDX_VIDEO_REC_VLC_TMP;i<=VID_MEM_IDX_VIDEO_REC_END;i++){
		vidPrintfMem("[%bu] a:%lx, s:%lx\n",i, vidRecMemAddrGet(i),vidRecMemSizeGet(i));
	}
}

   /**
 * @fn               vidRecVlcBufNumGet(
 * @brief	    video  vlc num get 
 * @param[in]    NONE
 * @retval          NONE
 * @see
 */
UINT8 vidRecVlcBufNumGet(UINT32 vlcBufSize,UINT32 vlcBufAddrStart,UINT32 vlcBufAddrEnd)
{
	UINT32 vlcBufNum;
	
	vlcBufNum=((vlcBufAddrEnd-vlcBufAddrStart)/vlcBufSize);
	if(vlcBufNum>VID_VLCBUFNUM_TOTAL){
		vlcBufNum=VID_VLCBUFNUM_TOTAL;
	}
	return vlcBufNum;
}

void vidRecMemSizeSet(vidmemCbIdx_t idx, UINT32 size)
{
	if (idx < VID_MEM_IDX_MAX) {
		vidMemCb[idx].size = size;
	}
}

UINT32 vidRecMemSizeGet(vidmemCbIdx_t idx)
{
	if (idx < VID_MEM_IDX_MAX) {
		return vidMemCb[idx].size;
	} else {
		return UNKNOWN;
	}
}
UINT32 vidRecMemAddrGet(vidmemCbIdx_t idx)
{
	if (idx < VID_MEM_IDX_MAX) {
		return vidMemCb[idx].addr;
	} else {
		return UNKNOWN;
	}	
}
UINT32 vidRecMemAddrGet_Intr(vidmemCbIdx_t idx)
{
	if (idx < VID_MEM_IDX_MAX) {
		return vidMemCb[idx].addr;
	} else {
		return UNKNOWN;
	}	
}
UINT8 vidRecSizeGet(void)
{
	UINT16 recHsize,recVsize;
	UINT8 size;
	
	sp1kVideoRecSizeGet(&recHsize,&recVsize);
	
	if ( recHsize == 1920 && recVsize == 1088 ) {
		size = MEM_FB_1080p;
	}
	else if ( recHsize == 1280 && recVsize == 720 ) {
		if ( K_SDRAM_TotalSize < 0x800000 ) {
			size = MEM_FB_HD_960;		
		}
		else {
			size = MEM_FB_HD;	
		}
	}
	else if ( recHsize == 720 && recVsize == 480 ) { //( recHsize == 848 && recVsize == 480 ) {//modify for D1 size
		size = MEM_FB_D1;
	}
	else {
		size = MEM_FB_VGA;
	}
	
	return size;
}
UINT8 vidRecFramebufModeGet(void)
{
	return frameBufNumMode;
}
void vidRecFramebufModeSet(UINT8 mode)
{
	frameBufNumMode=mode;
}

