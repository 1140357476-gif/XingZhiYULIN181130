/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
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
#include "general.h"
#include "sp1k_util_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_front_api.h"
#include "sp1k_snap_api.h"
#include "app_still_view.h"
#include "uiflow.h"
#include "func_ptr.h"
//#include "exifhdr.h"
#include "exiffunc.h"
#include "gpio.h"
#include "gpio_api.h"
#include "util_api.h"
#include "hal.h"
#include "pb_photoFrame.h"
#include "app_still_play.h"
#include "FileIndex.h"
#include "cmd.h"
#include "sp1k_calibration_api.h"
#include "asicreg.h"

#include "sp1k_video_api.h"

#include "sp1k_awb_api.h"//wangjg++

#include "front_logi.h"
#include "gpio.h"
#include "dps_api.h"

#include "app_ui_para.h"
#define D1_TEST 0
#define PV_EDGE_CORING_TUNING 0
#define SN_EDGE_CORING_TUNING 0

#if D1_TEST
#include "os_msg.h"
#include "sp1k_video_api.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#endif

#define EXIF_TEST 0
#if EXIF_TEST
#include "sp1k_exif_api.h"
//extern void TestExifShow();
#endif

#include "os_msg.h"
#include "sp1k_vfs_api.h"
#include "sp1k_pb_api.h"

#include "app_menu_micro.h"
#include "sp1k_disp_api.h"
#include "cardlink.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define GPRM_SRC_V_EXTRA 20

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 #if PV_EDGE_CORING_TUNING
 #define PV_EDGE_CORING_REG_NUM 12
 #define PV_EDGE_CORING_LEVEL 11
 code UINT8 pv_edge_coring_config[PV_EDGE_CORING_REG_NUM][PV_EDGE_CORING_LEVEL] = {
    {	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,	//0x216C
    {	70	,	68	,	66	,	64	,	62	,	60	,	58	,	56	,	54	,	52	,	50	}	,	//0x21D7
    {	15	,	14	,	12	,	11	,	9	,	8	,	6	,	5	,	3	,	2	,	0	}	,	//0x21D6
    {	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,	//0x21D4
    {	24	,	30	,	35	,	41	,	46	,	52	,	58	,	63	,	69	,	74	,	80	}	,	//0x21D5
	{	4	,	8	,	12	,	16	,	20	,	24	,	28	,	32	,	36	,	40	,	44	}	,	//0x21D9
	{	129	,	130	,	131	,	132	,	133	,	134	,	135	,	136	,	137	,	138	,	139	}	,	//0x21DE
    {	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,	//0x21DD
    {	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,	//0x21DC
    {	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,	//0x21DB
    {	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,	//0x21DA
    {	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,	//0x21D0
};

code UINT16 pv_edge_coring_reg[PV_EDGE_CORING_REG_NUM] = {
	0x216C,
	0x21D7,
	0x21D6,
	0x21D4,
	0x21D5,
	0x21D9,
	0x21DE,
	0x21DD,
	0x21DC,
	0x21DB,
	0x21DA,
	0x21D0
};
#endif

#if SN_EDGE_CORING_TUNING
#define SN_EDGE_CORING_REG_NUM 28
#define SN_EDGE_CORING_LEVEL 8
code UINT8 sn_edge_coring_config[SN_EDGE_CORING_REG_NUM][SN_EDGE_CORING_LEVEL] = {
	{	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,	/*0x216C*/
	{	70	,	67	,	64	,	61	,	59	,	56	,	53	,	50	}	,	/*0x21D7*/
	{	15	,	13	,	11	,	9	,	6	,	4	,	2	,	0	}	,	/*0x21D6*/
	{	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,	/*0x21D4*/
	{	24	,	32	,	40	,	48	,	56	,	64	,	72	,	80	}	,	/*0x21D5*/
	{	0	,	4	,	8	,	12	,	16	,	20	,	24	,	28	}	,	/*0x21D9*/
	{	0	,	2	,	4	,	6	,	8	,	10	,	12	,	14	}	,	/*0x21DE*/
	{	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,	/*0x21DD*/
	{	0	,	9	,	10	,	11	,	12	,	13	,	14	,	15	}	,	/*0x21DC*/
	{	0	,	9	,	10	,	11	,	12	,	13	,	14	,	15	}	,	/*0x21DB*/
	{	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,	/*0x21DA*/
	{	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	}	,	/*0x21D0*/
	{	9	,	8	,	7	,	6	,	5	,	4	,	3	,	2	}	,	/*0x22A0*/
	{	8	,	7	,	6	,	5	,	5	,	4	,	3	,	2	}	,	/*0x22A1*/
	{	8	,	7	,	6	,	5	,	5	,	4	,	3	,	2	}	,	/*0x22A2*/
	{	7	,	6	,	5	,	4	,	4	,	3	,	2	,	1	}	,	/*0x22A3*/
	{	7	,	6	,	5	,	4	,	4	,	3	,	2	,	1	}	,	/*0x22A4*/
	{	6	,	5	,	5	,	4	,	3	,	2	,	2	,	1	}	,	/*0x22A5*/
	{	6	,	5	,	5	,	4	,	3	,	2	,	2	,	1	}	,	/*0x22A6*/
	{	5	,	4	,	4	,	3	,	3	,	2	,	2	,	1	}	,	/*0x22A7*/
	{	5	,	4	,	4	,	3	,	3	,	2	,	2	,	1	}	,	/*0x22A8*/
	{	4	,	4	,	3	,	3	,	2	,	2	,	1	,	1	}	,	/*0x22A9*/
	{	4	,	4	,	3	,	3	,	2	,	2	,	1	,	1	}	,	/*0x22AA*/
	{	3	,	3	,	2	,	2	,	2	,	2	,	1	,	1	}	,	/*0x22AB*/
	{	3	,	3	,	2	,	2	,	2	,	2	,	1	,	1	}	,	/*0x22AC*/
	{	2	,	2	,	2	,	2	,	1	,	1	,	1	,	1	}	,	/*0x22AD*/
	{	2	,	2	,	2	,	2	,	1	,	1	,	1	,	1	}	,	/*0x22AE*/
	{	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,	/*0x22AF*/
};

code UINT16 sn_edge_coring_reg[SN_EDGE_CORING_REG_NUM] = {
	0x216C,
	0x21D7,
	0x21D6,
	0x21D4,
	0x21D5,
	0x21D9,
	0x21DE,
	0x21DD,
	0x21DC,
	0x21DB,
	0x21DA,
	0x21D0,
	0x22A0,
	0x22A1,
	0x22A2,
	0x22A3,
	0x22A4,
	0x22A5,
	0x22A6,
	0x22A7,
	0x22A8,
	0x22A9,
	0x22AA,
	0x22AB,
	0x22AC,
	0x22AD,
	0x22AE,
	0x22AF
};
#endif

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
//UINT8 dramFileLoad(UINT8 *name,UINT32 startAddr,	UINT32 *size);
//UINT8 dramFileSave(UINT8 *name,UINT32 startAddr,	UINT32 size);
void dosCpyCmd(UINT8* ptr);

void frontPreflashCfg( UINT8 cfgsel, UINT8 value );
void frontPreflashTrig( void );

extern SIMAGE xdata pbFile;
extern xdata UINT32 G_SHUTTER;
extern xdata UINT8 G_AEReady;
extern xdata UINT8 G_AESetting;
extern xdata UINT8 G_ucStorageType;
extern void sp1kVideoRecBitRateSet(UINT32 bitRate);
extern UINT8 pbCurFoldFileSpaceGet(UINT32 *fileSize,UINT8 fileType);

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
void cmdCustom(UINT8* ptr)
{
	#if 0
	if (strncmp(ptr, "GetSize", 7) == 0) {
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
	}
	#endif
    #if 0
    UINT16 v1, v2;
	if (strncmp(ptr, "thmst", 5) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        v1 = atoi(ptr);

		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        v2 = atoi(ptr);

        v1 = (v1<<8) + v2;
        pbParamSet(PB_PARAM_THUMB_START_X_Y, v1);
    }
    else if (strncmp(ptr, "thmwh", 5) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        v1 = atoi(ptr);

		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        v2 = atoi(ptr);

        v1 = (v1<<8) + v2;
        pbParamSet(PB_PARAM_THUMB_W_H, v1);
    }
    else if (strncmp(ptr, "thmbf", 5) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        v1 = atoi(ptr);
        pbParamSet(PB_PARAM_DISP_BUF_W, v1);

		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
        v2 = atoi(ptr);

        pbParamSet(PB_PARAM_DISP_BUF_H, v2);
    }
    #endif
    
    #if 0
    UINT32 bitRate;
	if (strncmp(ptr, "brc", 3) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

        bitRate = atoi(ptr);
        bitRate = bitRate*8*1024;
        printf("brc: %lu\n", bitRate);
        sp1kVideoRecBitRateSet(bitRate);//default 0 Kbit, no brc
    }
    #endif

#if 0
	if (strncmp(ptr, "spmd", 4) == 0) {
		UINT16 idx;
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			idx=(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}
		
		sp1kPbParamSet(PB_PARAM_SPLAY_MODE, idx);
		printf("mode=%d\n",pbParamGet(PB_PARAM_SPLAY_MODE));
	}
#endif
#if 0
	if (strncmp(ptr, "spe3", 4) == 0) {
		UINT16 idx;
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			idx=(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}
	//	printf("M_idx =%d\n",idx);
		sp1kSpeParamSet(SPE_PARAM_MIRROR_INDEX,idx);

	//	printf("M_idx_=%d\n",sp1kSpeParamGet(SPE_PARAM_MIRROR_INDEX));
		//speParamSetMirrorIdx((UINT16)atoi(ptr));
    }
	else if (strncmp(ptr, "spe2", 4) == 0) {
		UINT16 w,h;
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			w =(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			h =(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}/*
		ptr+=3;
		w=(UINT16)atoi(ptr);
		ptr+=5;
		w=(UINT16)atoi(ptr);*/
	//	printf("w=%d,h=%d\n",w,h);
		sp1kSpeParamSet(SPE_PARAM_H_SIZE_DST,w);
		sp1kSpeParamSet(SPE_PARAM_V_SIZE_DST,h);

	//	printf("w=%d,h=%d\n",
	//		sp1kSpeParamGet(SPE_PARAM_H_SIZE_DST),
	//		sp1kSpeParamGet(SPE_PARAM_V_SIZE_DST));
		//speParamSetDstSize(w,h);
    }
	else if (strncmp(ptr, "spe1", 4) == 0) {
		UINT16 h,v,ho,vo;
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			ho =(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			vo =(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			h =(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			v =(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}
		/*
		ptr+=3;
		w=(UINT16)atoi(ptr);
		ptr+=5;
		w=(UINT16)atoi(ptr);*/
	//	printf("ho=%d,vo=%d,h=%d,v=%d\n",ho,vo,h,v);
		sp1kSpeParamSet(SPE_PARAM_H_OFF,ho);
		sp1kSpeParamSet(SPE_PARAM_V_OFF,vo);
		sp1kSpeParamSet(SPE_PARAM_H_SIZE,h);
		sp1kSpeParamSet(SPE_PARAM_V_SIZE,v);

	//	printf("ho=%d,vo=%d,h=%d,v=%d\n",
	//		sp1kSpeParamGet(SPE_PARAM_H_OFF),
	//		sp1kSpeParamGet(SPE_PARAM_V_OFF),
	//		sp1kSpeParamGet(SPE_PARAM_H_SIZE),
	//		sp1kSpeParamGet(SPE_PARAM_V_SIZE)
	//		);
		//speParamSetDstSize(w,h);
    }
	if (strncmp(ptr, "spe", 3) == 0) {
		UINT16 idx=0;
		PBFile_t fileInfo;
		
		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			idx =(UINT16)atoi(ptr);// strtoul(ptr, 0, 0);
		}
	//	printf("idx=%d\n",idx);
		sp1kPbFileInfoGet(DOS_FileIndexGet(pbFile.dwIndex), &fileInfo);
		sp1kPbDistortView(fileInfo.FileIdx, idx);
		sp1kPbDistortSave(fileInfo.FileIdx, idx);
		}
#endif
    #if 0 //test modify dcf file base name
	if (strncmp(ptr, "dcfnm", 5) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

        sp1kDcfFileBaseNameUpdate(ptr);
    }
    #endif

#if 0
	if (strncmp(ptr, "trig", 3) == 0) {
		// the CWB FID must set in app_init.c
		sp1kAwbScaleSet( SP1K_AWB_PARAM_CWB_FID, 0x43 );

		sp1kCwbTrig();
	}

	if (strncmp(ptr, "cwb", 3) == 0) {
		UINT32 opt;

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			opt = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		if( opt == 0 ) {
			sp1kAwbModeSet( AWB_MODE_Auto );
		}
		else {
			// the CWB FID must set in app_init.c
			sp1kAwbScaleSet( SP1K_AWB_PARAM_CWB_FID, 0x43 );

			sp1kAwbModeSet( SP1K_AWB_MODE_CUSTOM );
		}
	}
#endif

    #if 0
    UINT8 en;

	if (strncmp(ptr, "aud", 3) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

        if(*ptr == '0')
            en = 0;
        else if(*ptr == '1')
            en = 1;
        else
            en = !sp1kVideoRecAudioStsGet();

        printf("audio: %bu\n", en);
        sp1kVideoRecAudioStsSet(en);
    }
    #endif

	#if 0
	UINT8 mode, recMode, pos, fmt;
    UINT8 moonId = 0;
    UINT16 offXDate;
	uiPara_t* puiPara = appUiParaGet();

    #if 0
	if (strncmp(ptr, "hd", 2) == 0) {
        sp1kVideoRecSizeSet(1280);
        return;
    }
    #endif

    #if 1
	if (strncmp(ptr, "disp0", 5) == 0) {
        sp1kDispBufFitModeSet(0);
        printf("FIT_ASPECT_RATIO\n");
        return;
    }
    else if(strncmp(ptr, "disp1", 5) == 0) {
        sp1kDispBufFitModeSet(1);
        printf("STRETCH\n");
        return;
    }
    #endif

    #if 0
    UINT8 parId;
    UINT16 para;
    if(strncmp(ptr, "stpar", 5) == 0){
        cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);
        parId = *ptr - '0';
        
        cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);
        para = atoi(ptr);

//        printf("stamp par set:%u %u\n", parId, para);
        sp1kDateStampParamSet(parId, para);        
        return;
    }
    #endif
    
    #if 0
    if(strncmp(ptr, "stamp0", 6) == 0){
		sp1kVideoRecStampStsSet(0);
        return;
    }
    else if(strncmp(ptr, "sun", 3) == 0){
        moonId = 0x3f; //sun
    }
    else if(strncmp(ptr, "moon", 4) == 0){
        moonId = 0x40; //moon
    }

    if(moonId){
        mode = VD_STAMP_MODE_IMG_DATE;
        recMode = VD_STAMP_REC_MODE_FST_END_FRAME;

        pos = VD_STAMP_LEFT | VD_STAMP_BOTTOM;
        fmt = VD_STAMP_FMT_MMDDYYYY | VD_STAMP_FMT_HHMM;

		printf("md: %bu, %bx, %bx, %bx\n", mode, recMode, pos, fmt);

		sp1kVideoRecStampCfg(mode, pos, recMode);
        sp1kVideoRecStampDateFmtSet(fmt);

        if(puiPara->VideoSize == 0/*MOV_SIZE_VGA*/){
           sp1kVideoRecStampImgResLoad(moonId+2, 288, 32);
        }
        else
            sp1kVideoRecStampImgResLoad(moonId, 192, 24);

		sp1kVideoRecStampStsSet(1);

        return;
    }
    #endif

#if 0
    //stamp mode[1]recMode[1] pos[2] fmt[2]
    if(strncmp(ptr, "stampstr", 8) == 0){
        cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);

        #if 1 //test snap stamp
        if(sp1kSnapParamGet(SP1K_SNAP_PARAM_DATE_STAMP_OPT)==0){
            sp1kSnapParamSet(SP1K_SNAP_PARAM_DATE_STAMP_OPT, 1);
            sp1kSnapParamSet(SP1K_SNAP_PARAM_DATE_STAMP_TYPE, 0x80); //string only
            sp1kSnapParamSet(SP1K_SNAP_PARAM_DS_X_RATIO, 2);
        }
        else{            
            sp1kSnapParamSet(SP1K_SNAP_PARAM_DS_X_RATIO, 2);
            sp1kSnapParamSet(SP1K_SNAP_PARAM_DATE_STAMP_TYPE, 0x40|sp1kSnapParamGet(SP1K_SNAP_PARAM_DATE_STAMP_TYPE));
        }
        #endif
        
        sp1kDateStampCustomStrSet(ptr);
    }
#endif
#if 0 //test video date stamp 
	  /* stampW*stampH<720*48;stampW and stampH both base on MCU;*/
    if (strncmp(ptr, "stamp", 5) == 0) {
#if 0
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

        ptr += 2;

		pos = *ptr - '0' << 4;  //0~1 : up mid, down
		ptr ++;
        pos += (*ptr - '0');    //0~1 : left, mid, right
        ptr += 2;

		fmt = *ptr - '0' << 4;  //0,1,3 : no time, hh:mm, hh:mm:ss
		ptr ++;
        fmt += (*ptr - '0');    //0~2 : yyyy/mm/dd, dd/mm/yyyy, mm/dd/yyyy
#else
      
       /* //mode = VD_STAMP_MODE_IMG_DATE;
        //recMode = VD_STAMP_REC_MODE_FST_END_FRAME;

        pos = VD_STAMP_RIGHT | VD_STAMP_VCENTER;
        fmt = VD_STAMP_FMT_MMDDYYYY | VD_STAMP_FMT_HHMMSS ;

        if(*ptr){
            fmt |= VD_STAMP_FMT_STR_WITH_DATE;
            sp1kDateStampCustomStrSet(ptr);
        }*/
        fmt = VD_STAMP_FMT_DDMMYYYY;
		fmt |= VD_STAMP_FMT_HHMMSS;
		fmt |=VD_STAMP_REE_MODE_TWO_LINE; //Date and time two line show; Note if in two line mode,can not set img and date time 
        mode = VD_STAMP_MODE_DATE ; 
		recMode = VD_STAMP_REC_MODE_ALL_FRAME;
		pos = VD_STAMP_RIGHT | VD_STAMP_BOTTOM;
#endif
		sp1kVideoRecStampCfg(mode, pos, recMode);
		sp1kVideoRecStampDateFmtSet(fmt);
		sp1kVideoRecParamSet(VD_STAMP_DATETIME_DIS_X_RATIO, 100);
		sp1kVideoRecParamSet(VD_STAMP_DISOFFX, 1280); //set date stamp OffsetX
		sp1kVideoRecParamSet(VD_STAMP_DISOFFY, 720);  //set date stamp OffsetY
		
        if(mode == VD_STAMP_MODE_DATE)
		{  if(((fmt & 0x100) == 0x100 )&& ((fmt & 0x10) == 0x10)) //Date and time two line show
		  {
				if(puiPara->VideoSize == MOV_SIZE_QVGA)
				{
					sp1kVideoRecStampImgResLoad(0, 160, 32); //back ground image
				}
				else if(puiPara->VideoSize == MOV_SIZE_HD)
				{ 
					sp1kVideoRecStampImgResLoad(0, 320, 96);
				}
				else if(puiPara->VideoSize == MOV_SIZE_D1)
				{
					sp1kVideoRecStampImgResLoad(0, 240, 64);

				}
				else 
				{
					sp1kVideoRecStampImgResLoad(0, 240, 64); 
				}
		  }
		  else
		  	{
		  		if(puiPara->VideoSize == MOV_SIZE_QVGA)
				{
					sp1kVideoRecStampImgResLoad(0, 240, 16); //back ground image
				}
				else if(puiPara->VideoSize == MOV_SIZE_HD)
				{ 
					sp1kVideoRecStampImgResLoad(0, 720, 48);
				}
				else if(puiPara->VideoSize == MOV_SIZE_D1)
				{
					sp1kVideoRecStampImgResLoad(0, 480, 32);

				}
				else 
				{
					sp1kVideoRecStampImgResLoad(0, 480, 32); 
				}
		  	}
		}
		else if(mode == VD_STAMP_MODE_IMG_DATE) 
		{ //img and Date time
			if(puiPara->VideoSize == MOV_SIZE_QVGA)
			{
				sp1kVideoRecStampImgResLoad(0xba, 192, 16); //back ground image
			}
			else if(puiPara->VideoSize == MOV_SIZE_HD)
			{ 
				sp1kVideoRecStampImgResLoad(0xb8, 720, 48);
			}
			else if(puiPara->VideoSize == MOV_SIZE_D1)
			{
				sp1kVideoRecStampImgResLoad(0xb9, 512, 32);

			}
			else 
			{
				sp1kVideoRecStampImgResLoad(0xb9, 512, 32); 
			}
		}
		else 
		{
			//img
			if(puiPara->VideoSize == MOV_SIZE_QVGA)
			{
				sp1kVideoRecStampImgResLoad(0xba, 192, 16); //back ground image
			}
			else if(puiPara->VideoSize == MOV_SIZE_HD)
			{ 
				sp1kVideoRecStampImgResLoad(0xb8, 720, 48);
			}
			else if(puiPara->VideoSize == MOV_SIZE_D1)
			{
				sp1kVideoRecStampImgResLoad(0xb9,512, 32);

			}
			else 
			{
				sp1kVideoRecStampImgResLoad(0xb9, 512, 32); 
			}
		}
		sp1kVideoRecStampStsSet(1);
	}	
#endif

	#if EXIF_TEST
    struct IFD ifd[] = {
		{IFD0_TAG_IMGDESCRIPTION, 	IFD_DATATYPE_ASCII, 11, 0},
        {IFD0_TAG_MAKE,     IFD_DATATYPE_ASCII, 11, 0},
        {IFD0_TAG_MODEL,    IFD_DATATYPE_ASCII, 11, 0},
        {IFD0_TAG_DATETIME, IFD_DATATYPE_ASCII, 20, 0},

        {EXIF_TAG_FNUMBER,      IFD_DATATYPE_RATIONAL, 1, 0},
        {EXIF_TAG_MAXAPERTURE,  IFD_DATATYPE_RATIONAL, 1, 0},
		{EXIF_TAG_FLASH, 		IFD_DATATYPE_SHORT,    1, 0x00000007},
		{EXIF_TAG_FOCALLENGTH,	IFD_DATATYPE_RATIONAL, 1, 0},
		{EXIF_TAG_WHITEBALANCE,	IFD_DATATYPE_SHORT,    1, 0x00000001},
    };
    UINT8 dataBuf[][20]={
		{"Descript"},
        {"Sunmedia"},
        {"Sun 1528"},
        {"2008:10:25 12:30:59"},

        {0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x04},//big endian, 8/4
        {0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x0a},//big endian, 28/10
		{0},
		{0x00, 0x00, 0x00, 0x55, 0x00, 0x00, 0x00, 0x0c},
		{0}
    };

	UINT8 comment[]= "This is comment";
    UINT8 ifdType;
    struct IFD *pIfd;
    UINT8* pval;
    UINT8 i;

    if (strcmp(ptr, "exifshow") == 0) {
        //TestExifShow();
    }
    else if(strncmp(ptr, "exifset", 7) == 0){
	    cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);
        i = *ptr - '0';

        //if(i > 8)
        //    return;

        pIfd = &ifd[i];
        pval = dataBuf[i];
        if(i >= 4)
            ifdType = IFD_TYPE_EXIFIFD;
        else
            ifdType = IFD_TYPE_ZEROIFD;

        if(pIfd->tag == EXIF_TAG_FLASH || pIfd->tag == EXIF_TAG_WHITEBALANCE)//no data buffer
            pval = NULL;

		if(i==9)
			sp1kEXIFJPGCommentSet(comment, 15);
		else
	        sp1kEXIFTagValueSet(ifdType, pIfd->tag, pIfd->type, pIfd->count, pIfd->valueoffset,
            	pval, 1); //big endian :1
    }
	#endif

	#if D1_TEST
    uiPara_t* puiPara;
    if (strcmp(ptr, "d1") == 0) {
    	puiPara = appUiParaGet();
        puiPara->VideoSize = MOV_SIZE_D1;
        osMsgPost(SP1K_MSG_STATE_INIT);
        printf("set D1\n");
    }
	#endif

	#if PV_EDGE_CORING_TUNING
	UINT8 level, i;
	if (strncmp(ptr, "pvedge", 6) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

		level = *ptr - '0';

		for(i = 0; i < PV_EDGE_CORING_REG_NUM ; i++) {
			XBYTE[pv_edge_coring_reg[i]] = pv_edge_coring_config[i][level];
		};
	}
	#endif

	#if SN_EDGE_CORING_TUNING
	UINT8 level, i;
	if (strncmp(ptr, "snedge", 6) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);

		level = *ptr - '0';

		for(i = 0; i < SN_EDGE_CORING_REG_NUM ; i++) {
			XBYTE[sn_edge_coring_reg[i]] = sn_edge_coring_config[i][level];
		};
	}
	#endif

	if (strcmp(ptr , "string") == 0) {
		printf("Test string\n");//
	}

	#if 0
	else if ( strncmp(ptr, "1", 1) == 0 ) {
		printf( "%s: %ld\n", __FILE__, (UINT32)__LINE__ );

		frontStrobeInit();
		sp1kSTChgEnSet(1);
	}
	#endif

	#if 0
	else if ( strncmp(ptr, "pre", 3) == 0 ) {
		UINT8 y1[ 64 ];
		UINT8 y2[ 64 ];
		UINT8 y3[ 64 ];
		UINT16 i;
		UINT8 regGamma = XBYTE[REG_Cdsp_Lutgammaen];

		UINT16 xSize, ySize;

		// disable Gamma
		XBYTE[ REG_Cdsp_Lutgammaen ] = 0;

		frontPreviewSizeGet( 0, &xSize, &ySize );

		printf( "%s: %ld, sensor %d, %d\n", __FILE__, (UINT32)__LINE__, xSize, ySize );

		#if 0
		G_SHUTTER = 21308;
		#else
		G_SHUTTER = (UINT32)24000000 * 10 / xSize / ( XBYTE[ 0x2249 ] );

		G_SHUTTER = G_SHUTTER >> 1; // for safty
		#endif

		printf( "G_SHUTTER = %ld\n", G_SHUTTER );

		G_AEReady=1;

		while( !( G_AEReady == 0 && G_AESetting == 0 ) );

		// do we wait another VD for AE window update?
		frontSignalWait(FRONT_WAIT_VSYNC, HAL_FRONT_WAIT_RISING, 2);

		HAL_ReadAEWindow(&y1);

		frontPreflashCfg( SENSOR_PREFLASH_CFG_H_POS, ySize >> 1 );
		frontPreflashCfg( SENSOR_PREFLASH_CFG_WIDTH, 4 );
		frontPreflashCfg( SENSOR_PREFLASH_CFG_DELAY_TYPE, SENSOR_FLASH_DELAY_TYPE_H_POS );

		frontPreflashTrig();

		//HAL_CdspAEWinEnSet ( 0x01, 0x1, 0x00, 0x1 ); /* hold window */
		HAL_CdspAEWinHoldSet(1);
		// set ae holh with v sync
		// wait

		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);

		HAL_ReadAEWindow(&y2);

		XBYTE[ REG_Cdsp_Lutgammaen ] = regGamma;

		//HAL_CdspAEWinEnSet ( 0x01, 0x0, 0x00, 0x1 ); /* unhold window */
		HAL_CdspAEWinHoldSet(0);
		
		frontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 2);

		HAL_ReadAEWindow(&y3);

		printf( "===Before===\n" );

		for( i = 0; i < 64; i++ ) {
			printf( "[%02bx]", y1[ i ] );

			if( i % 8 == 7 ) {
				printf( "\n" );
			}
		}

		printf( "===After===\n" );

		for( i = 0; i < 64; i++ ) {
			printf( "[%02bx]", y2[ i ] );

			if( i % 8 == 7 ) {
				printf( "\n" );
			}
		}

		printf( "===Test===\n" );

		for( i = 0; i < 64; i++ ) {
			printf( "[%02bx]", y3[ i ] );

			if( i % 8 == 7 ) {
				printf( "\n" );
			}
		}
	}
	#endif

	#if 0  /* matt wang add. */
	else if ( strncmp(ptr, "20f", 3) == 0 ) {
		sp1kFrontFrameRateSet(0, 20);
	}
	else if ( strncmp(ptr, "30f", 3) == 0 ) {
		sp1kFrontFrameRateSet(0, 30);
	}
	else if ( strncmp(ptr, "60f", 3) == 0 ) {
		sp1kFrontFrameRateSet(0, 60);
	}
	#endif

	#if 1  /* matt wang add. */
	else if ( strncmp(ptr, "obcal", 3) == 0 ) {
		frontOpticalBlackCalibrate(0);
	}
	#endif

	#if 0  /* matt wang add. */
	else if ( strncmp(ptr, "dpin", 3) == 0 ) {
		UINT8 i, ret, byteId, bitMsk;


		printf("chip id=0x%04X\n", pkgChipIdGet());
		for ( i = 0; i< PKG_DEDICATE_PIN_TOT+4; i++ ) {
			ret = pkgDedicateGpioGet(i, &byteId, &bitMsk);
			printf(" %S pin=%2bd byte=%2bd bit=0x%02bX\n", ret==SUCCESS?"OK":"NG", i, byteId, bitMsk);
		}
	}
	#endif

	#if 0  /* matt wang add, test some dedicated pin as gpio. */
	else if ( strncmp(ptr, "dgpio", 3) == 0 ) {
		#define GPIO_PIN_TOT  15

		UINT8 byteId[GPIO_PIN_TOT], bitMsk[GPIO_PIN_TOT], *pinName[GPIO_PIN_TOT];
		UINT8 funcBak[GPIO_PIN_TOT], dirBak[GPIO_PIN_TOT], outBak[GPIO_PIN_TOT], pullBak[7];
		UINT8 i, out = 0xFF;


		/* init gpio. */
		pkgDedicateGpioGet(PKG_DEDICATE_PIN_LCD_SEN, byteId+0, bitMsk+0);
		pinName[0] = "LCD_SEN";
		pkgDedicateGpioGet(PKG_DEDICATE_PIN_SD_CD,   byteId+1, bitMsk+1);
		pinName[1] = "SD_CD";
		pkgDedicateGpioGet(PKG_DEDICATE_PIN_SD_WP,   byteId+2, bitMsk+2);
		pinName[2] = "SD_WP";
		pkgDedicateGpioGet(PKG_DEDICATE_PIN_SPK_EN,  byteId+3, bitMsk+3);
		pinName[3] = "SPK_EN";
		byteId[4] = GPIO_BYTE_DISP0;
		bitMsk[4] = 0x01;
		pinName[4] = "DIGTV0";
		byteId[5] = GPIO_BYTE_DISP0;
		bitMsk[5] = 0x02;
		pinName[5] = "DIGTV1";
		byteId[6] = GPIO_BYTE_DISP0;
		bitMsk[6] = 0x04;
		pinName[6] = "DIGTV2";
		byteId[7] = GPIO_BYTE_DISP0;
		bitMsk[7] = 0x08;
		pinName[7] = "DIGTV3";
		byteId[8] = GPIO_BYTE_DISP0;
		bitMsk[8] = 0x10;
		pinName[8] = "DIGTV4";
		byteId[9] = GPIO_BYTE_DISP0;
		bitMsk[9] = 0x20;
		pinName[9] = "DIGTV5";
		byteId[10] = GPIO_BYTE_DISP0;
		bitMsk[10] = 0x40;
		pinName[10] = "DIGTV6";
		byteId[11] = GPIO_BYTE_DISP0;
		bitMsk[11] = 0x80;
		pinName[11] = "DIGTV7";
		byteId[12] = GPIO_BYTE_DISP1;
		bitMsk[12] = 0x02;
		pinName[12] = "DIGTV9";
		byteId[13] = GPIO_BYTE_DISP1;
		bitMsk[13] = 0x04;
		pinName[13] = "DIGTV10";
		byteId[14] = GPIO_BYTE_DISP1;
		bitMsk[14] = 0x08;
		pinName[14] = "DIGTV11";

		/* save gpio setting. */
		for ( i = 0; i < GPIO_PIN_TOT; i++ ) {
			funcBak[i] = gpioByteFuncGet(byteId[i], bitMsk[i]);
			gpioByteFuncSet(byteId[i], bitMsk[i], 0xFF);
			dirBak[i] = gpioByteDirGet(byteId[i], bitMsk[i]);
			outBak[i] = gpioByteOutGet(byteId[i], bitMsk[i]);
		}
		for ( i = 0; i < 7; i++ ) {
			pullBak[i] = gpioPullEnGet(i, 0xFF);
			gpioPullEnSet(i, 0xFF, 0x00);
		}

		/* set gpio to output. */
		for ( i = 0; i < GPIO_PIN_TOT; i++ ) {
			gpioByteDirSet(byteId[i], bitMsk[i], 0xFF);
		}

		do {
			if ( out ) {
				printf("all gpio output high\n");
			}
			else {
				printf("all gpio output low\n");
			}
			for ( i = 0; i < GPIO_PIN_TOT; i++ ) {
				gpioByteOutSet(byteId[i], bitMsk[i], out);
			}
			out = ~out;
			printf("press Esc to exit.\n\n");
		}
		while ( getchar() != 27
);  /* Esc key? */

		/* set gpio to input. */
		for ( i = 0; i < GPIO_PIN_TOT; i++ ) {
			gpioByteDirSet(byteId[i], bitMsk[i], 0x00);
		}
		do {
			printf("all gpio input.\n");
			for ( i = 0; i < GPIO_PIN_TOT; i++ ) {
				printf(" [%s] = 0x%02bX\n", pinName[i], gpioByteInGet(byteId[i], bitMsk[i]));
			}
			printf("press Esc to exit.\n\n");
		}
		while ( getchar() != 27
);  /* Esc key? */

		/* restore gpio setting. */
		for ( i = 0; i < GPIO_PIN_TOT; i++ ) {
			gpioByteFuncSet(byteId[i], bitMsk[i], funcBak[i]);
			gpioByteDirSet(byteId[i], bitMsk[i], dirBak[i]);
			gpioByteOutSet(byteId[i], bitMsk[i], outBak[i]);
		}
		for ( i = 0; i < 7; i++ ) {
			gpioPullEnSet(i, 0xFF, pullBak[i]);
		}
	}
	#endif

	#if 0
	else if ( strncmp(ptr, "pf", 2) == 0 ) {
		UINT16 d0, d1;
		UINT32 addr;
		UINT32 fileSize;
		UINT8 f[ 13 ] = "00000000YUV";

		d0 = sp1kSnapParamGet( SP1K_SNAP_PARAM_PH_LOW_ADDR );
		d1 = sp1kSnapParamGet( SP1K_SNAP_PARAM_PH_HIGH_ADDR );

		addr = ( (UINT32)d1 << 16 ) | (UINT32)d0;

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		sp1kDramFileLoad( f, addr, &fileSize );

		sp1kSnapParamSet( SP1K_SNAP_PARAM_PH_WIDTH, 320 );
		sp1kSnapParamSet( SP1K_SNAP_PARAM_PH_HEIGHT, 240 );

		sp1kSnapParamSet( SP1K_SNAP_PARAM_PHOTO_FRM_OPT, 1 );
	}
	else if ( strncmp(ptr, "pv", 2) == 0 ) {
		sp1kCalibrationPvRaw();
	}
	else if ( strncmp(ptr, "raw", 3) == 0 ) {
		UINT32 opt;

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			opt = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		sp1kSnapParamSet( SP1K_SNAP_PARAM_SAVE_RAW_OPT, opt );
	}
	else if ( strncmp(ptr, "ob", 2) == 0 ) {
		UINT32 opt;

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			opt = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		sp1kCalibrationObSet( (UINT8) ( opt & 0x01 ) );
	}
	else if ( strncmp(ptr, "gamma", 5) == 0 ) {
		UINT32 opt;

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			opt = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		sp1kCalibrationGammaSet( opt );
	}
	else if ( strncmp(ptr, "awb", 3) == 0 ) {
		UINT32 opt;

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			opt = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		sp1kCalibrationAWBSet( opt );
	}
	else if ( strncmp(ptr, "ae", 2) == 0 ) {
		UINT32 opt;

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			opt = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		sp1kCalibrationAESet( opt );
	}
	else if ( strncmp(ptr, "lc", 2) == 0 ) {
		UINT32 opt;

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			opt = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		sp1kCalibrationLscSet( opt );
	}
	else if ( strncmp(ptr, "yuv", 3) == 0 ) {
		sp1kCalibrationPvYuv();
	}
	else if ( strncmp(ptr, "size", 4) == 0 ) {
		sp1kCalibrationDefaultSizeSet();
	}
	#endif

	#if 0
	else if ( strncmp(ptr, "ttt", 3) == 0 ) {
		UINT32 addr = 0x200000;
		UINT8 t0, t1, t2, t3;
		UINT16 i, value;
		UINT8 f[ 13 ] = "GAMMA000BIN";

		M_DRAM_SetStartAddress(addr, 0);

		t1 = 0xaa;
		t2 = 0x2a;
		t3 = 0x00;

		for( i = 0; i < 0x80; i++ ) {
			value = i * 8 * 2;

			if( value >= 256 ) {
				t0 = 254;
				t1 = t2 = t3 = 0;
			}
			else {
				t0 = (UINT8)value;
			}

			M_DRAM_WriteWord( t0, t1 );
			M_DRAM_WriteWord( t2, t3 );
		}

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;
		sp1kDramFileSave( f, addr, 512 );
	}
	else if ( strncmp(ptr, "gid", 3) == 0 ) {
		UINT32 from, to;

		printf( "%s: %ld\n", __FILE__, (UINT32)__LINE__ );

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			from = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		cmdTokenStrip(&ptr);
		if ( ptr[0] != '\0' ) {
			cmdWhiteSpaceStrip(&ptr);

			to = strtoul(ptr, 0, 0);
		}
		else {
			printf( "Invalid input\n" );
		}

		sp1kCalibrationAETest( from, to, 50 );
	}
	#endif

	else if ( strncmp(ptr, "kaetest", 7) == 0 ) {
		UINT32 addr = 0x200000;
		UINT8 id0, id1, id2, id3;

		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_AE_TEST );

		sp1kCalibrationLoadCriteria( SP1K_CALIBRATION_CRITERIA_FILE_ID );

		sp1kCalibrationAeTestGidStepGet( &id0, &id1, &id2, &id3 );

		printf( "%s: %ld, idx [%bx][%bx][%bx][%bx]\n", __FILE__, (UINT32)__LINE__, id0, id1, id2, id3 );

		sp1kCalibrationAETest( id0, id1 - 1, 60 );
		sp1kCalibrationAETest( id1, id2 - 1, 60 );
		sp1kCalibrationAETest( id2, id3 - 1, 60 );

		sp1kCalibrationAETestSave( id0, id3 - 1 );

		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
	}
	else if ( strncmp(ptr, "kob", 3) == 0 ) {
		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OB );

//		sp1kCalibrationLoadIq( 0x30 );

		// snap

		sp1kCalibrationSaveIq( SP1K_CALIBRATION_OB_FILE_ID );

		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
	}
#if 0
	else if ( strncmp(ptr, "kpvob", 5) == 0 ) {
		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_PV_OB );

//		sp1kCalibrationLoadIq( 0x30 );

		// snap
		sp1kCalibrationPvRaw();

		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
	}
#endif
	else if ( strncmp(ptr, "kawb", 4) == 0 ) {
		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_AWB );

//		sp1kCalibrationLoadIq( 0x30 );

		// snap

		sp1kCalibrationSaveIq( SP1K_CALIBRATION_AWB_FILE_ID );

		sp1kCalibrationModeSet( SP1K_CALIBRATION_MODE_OFF );
	}
/* ------ Note: Demo code zone start flag, don't modify or delete this line!!! ------ */
/* ------ Note: Demo code zone end flag, don't modify or delete this line!!! ------ */

	else if (strcmp(ptr , "exif") == 0) {
		extern xdata UINT32 exifQvVlcAddr;
		extern xdata UINT8 exifQvVlcAtOdd;
		extern xdata UINT16 exifQvXSize;
		extern xdata UINT16 exifQvYSize;

		UINT32 fSize;
		UINT32 addr = 0x200000;
		UINT32 yuvAddr = 0x300000;
		UINT8 f[ 13 ] = "00000000JPG";
		UINT8 dummy[ 20 ];

		struct Exif_Decompress_Parameter MainImgPara;
		struct Exif_Decompress_Parameter ThumbPara;

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		sp1kDramFileLoad( f, addr, &fSize );

		EXIFHDR_Parsing_ExifFile(addr, 0, dummy, &MainImgPara,&ThumbPara);

		sp1kJpegQTableSet( 85, 0, 0, 1 );

		if( exifQvVlcAtOdd == 0 ) {
			HAL_JpegDecode(exifQvVlcAddr, exifQvXSize, exifQvYSize, yuvAddr, 0, 0x04, G_JpgRstMcuNum);
		}
		else {
			HAL_JpegDecode(exifQvVlcAddr, exifQvXSize, exifQvYSize, yuvAddr, 0, 0x14, G_JpgRstMcuNum);
		}

		sp1kDramFileSave( "00000000YUV", yuvAddr, (UINT32)exifQvXSize * exifQvYSize * 2 );
	}
#if 0
	else if (strcmp(ptr , "ans") == 0) {
		UINT32 srcAddr = 0x100000;

		UINT32 tempAddr, yuvAddr, fSize;

		UINT16 srcX, srcY, dstX, dstY;

		UINT8 f[ 13 ] = "00000000YUV";

		srcX = 320;
		srcY = 240;

#if 0
		dstX = 1024;
		dstY = 768;
#else
		dstX = 1872;
		dstY = 1049;

#endif
		tempAddr = srcAddr + (UINT32)srcX * srcY;
		tempAddr = ( tempAddr + 16 ) >> 4 << 4;

		yuvAddr = tempAddr + (UINT32)dstX * dstY;
		yuvAddr = ( yuvAddr + 16 ) >> 4 << 4;

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		sp1kDramFileLoad( f, srcAddr, &fSize );

		HAL_GprmScale( srcAddr, srcX, srcY, yuvAddr, dstX, dstY, tempAddr, 1 );

		f[ 8 ] = 'A';
		f[ 9 ] = 'N';
		f[ 10 ] = 'S';

		sp1kDramFileSave( f, yuvAddr, (UINT32)dstX * dstY * 2 );
	}
	else if (strcmp(ptr , "gprm1") == 0) {
		UINT32 srcAddr = 0x100000;
		UINT32 dstAddr;
		UINT32 tempAddr;
		UINT32 yuvAddr;
		UINT8 f[ 13 ] = "00000000YUV";
		UINT32 fSize;
		UINT16 srcX, srcY, dstX, dstY;
		UINT16 i;
		UINT16 scalingFactor, initScalingFactor;
		UINT16 srcChunk, chunk, m, offset;
		UINT16 max;
		UINT16 yuvPos;

		srcX = 320;
		srcY = 240;

		dstX = 1024;
		dstY = 768;

		chunk = 32;
		srcChunk = (UINT16)((UINT32)chunk * srcY / dstY);

		tempAddr = srcAddr + (UINT32)srcX * srcY;
		tempAddr = ( tempAddr + 15 ) >> 4 << 4;

		dstAddr = tempAddr + (UINT32)chunk * dstX * 2;
		dstAddr = ( dstAddr + 15 ) >> 4 << 4;

		yuvAddr = dstAddr + (UINT32)chunk * dstX * 2;
		yuvAddr = ( yuvAddr + 15 ) >> 4 << 4;

		scalingFactor = (UINT16)(((UINT32)( srcY - 1 )<<16)/(UINT32)( dstY - 1 ));

		max = ( dstY + ( chunk - 1 ) ) / chunk;

		yuvPos = 0;

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		sp1kDramFileLoad( f, srcAddr, &fSize );

		HAL_GprmScaleVFactor( srcAddr, srcX, srcChunk + GPRM_SRC_V_EXTRA, dstAddr, dstX, chunk, tempAddr, scalingFactor, 0x8000, 1 );

		HAL_GprmDramDma( dstAddr << 1, yuvAddr << 1, (UINT32)dstX * chunk * 2, 1);
		yuvPos += chunk;

		for( i = 1; i < max; i++ ){
			UINT32 sAddr;
			UINT16 overlap;

			m = chunk * i + 1 - (UINT16)((UINT32)65536 / (UINT32)scalingFactor );

			overlap = chunk * i - m + 1;

			offset = (UINT32)( m - 1 ) * (UINT32)scalingFactor / (UINT32)65536;
			initScalingFactor = ( (UINT32)( m - 1 ) * (UINT32)scalingFactor % (UINT32)65536 );

			while( initScalingFactor >= 0x8000 ) {
				m = m - 1; // adjust

				overlap = chunk * i - m + 1;

				offset = (UINT32)( m - 1 ) * (UINT32)scalingFactor / (UINT32)65536;
				initScalingFactor = ( (UINT32)( m - 1 ) * (UINT32)scalingFactor % (UINT32)65536 );
			}

			sAddr = srcAddr + (UINT32)offset * srcX;

			printf( "[%d] m %d, offset %d, if 0x%x, sAddr 0x%lx, overlap %d\n", i, m, offset, initScalingFactor, sAddr, overlap );

			if( initScalingFactor >= 0x8000 ) {
				printf( "init overflow[0x%x]\n", initScalingFactor );
			}

			if( offset + srcChunk + GPRM_SRC_V_EXTRA <= srcY ) {
				HAL_GprmScaleVFactor( sAddr, srcX, srcChunk + GPRM_SRC_V_EXTRA, dstAddr, dstX, chunk + overlap, tempAddr, scalingFactor, initScalingFactor + 0x8000, 1 );
			}
			else {
				HAL_GprmScaleVFactor( sAddr, srcX, srcY - offset, dstAddr, dstX, chunk + overlap, tempAddr, scalingFactor, initScalingFactor + 0x8000, 1 );
			}

			HAL_GprmDramDma( ( dstAddr + (UINT32)overlap * dstX ) << 1, ( yuvAddr + (UINT32)yuvPos * dstX ) << 1, (UINT32)dstX * chunk * 2, 1);
			yuvPos += chunk;
		}

		f[ 0 ] = '1';
		sp1kDramFileSave( f, yuvAddr, (UINT32)dstX * dstY * 2 );
	}
	else if (strcmp(ptr , "gprm2") == 0) {
		UINT32 srcAddr = 0x100000;
		UINT32 dstAddr;
		UINT32 tempAddr;
		UINT32 yuvAddr;
		UINT8 f[ 13 ] = "00000000YUV";
		UINT32 fSize;
		UINT16 srcX, srcY, dstX, dstY, chunk, overlap;
		UINT16 i;
		UINT16 max;

		srcX = 320;
		srcY = 240;

#if 0
		dstX = 1024;
		dstY = 768;
#else
		dstX = 1872;
		dstY = 1049;
#endif
		chunk = 32;

		tempAddr = srcAddr + (UINT32)srcX * srcY;
		tempAddr = ( tempAddr + 15 ) >> 4 << 4;

		dstAddr = tempAddr + (UINT32)chunk * dstX * 2;
		dstAddr = ( dstAddr + 15 ) >> 4 << 4;

		yuvAddr = dstAddr + (UINT32)chunk * dstX * 2;
		yuvAddr = ( yuvAddr + 15 ) >> 4 << 4;

		max = ( dstY + ( chunk - 1 ) ) / chunk;

		f[ 11 ] = 0x20;
		f[ 12 ] = 0x00;

		sp1kDramFileLoad( f, srcAddr, &fSize );

		gprmPartialVScaleInit( srcX, srcY, dstX, dstY, chunk );

		for( i = 0; i < max; i++ ) {
			gprmPartialVScale( srcAddr, dstAddr, tempAddr, i, &overlap );

			HAL_GprmDramDma( ( dstAddr + (UINT32)overlap * dstX ) << 1, ( yuvAddr + (UINT32)i * chunk * dstX ) << 1, (UINT32)dstX * chunk * 2, 1);
		}

		f[ 0 ] = '2';
		sp1kDramFileSave( f, yuvAddr, (UINT32)dstX * dstY * 2 );
	}
#endif
	else if (strtoul(ptr , NULL, 0) == 100UL) {
		printf("Test int is %lu\n", strtoul(ptr , NULL, 0));//
	}
	#if 0
	else if (strcmp(ptr , "snap") == 0) {
		static UINT16 times;
		times = 9999;
		appStillViewInit(0);
		sp1kTimer1Config(2000, pFunc_TestSnap, (UINT8*)&times);
		sp1kTimer1Start();
		return;
	}
	#endif
	#if 1
	else if (strcmp(ptr , "mm") == 0) {
		sp1kHalUSBSwPlugOut();
		gUsbMode = K_UIOPTION_STORAGE_MSDC;
		UI_UsbModeSet(K_UISTATUS_USBMODE_MASS);
		sp1kHalUSBSwPlugIn();
		return;
	}
	#endif
	#if 1
	else if (strcmp(ptr , "ms") == 0) {
		sp1kHalUSBSwPlugOut();
		gUsbMode = K_UIOPTION_STORAGE_SIDC;
		UI_UsbModeSet(K_UISTATUS_USBMODE_MASS);
		sp1kHalUSBSwPlugIn();
		return;
	}
	#endif
	#if 1
	else if (strcmp(ptr , "mp") == 0) {
		sp1kHalUSBSwPlugOut();
		gUsbMode = K_UIOPTION_STORAGE_NONE;
		UI_UsbModeSet(K_UISTATUS_USBMODE_PCCAM);
		sp1kHalUSBSwPlugIn();
		return;
	}
	#endif
#if 1
    else if (strncmp(ptr , "sd", 2) == 0) {
        UINT32 addr;
        UINT32 num;

        cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);
        addr = strtoul(ptr, NULL, 0);
        cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);
        num = strtoul(ptr, NULL, 0);
        printf("addr: %lx\n", (UINT32)addr);
        printf("num: %lx\n", (UINT32)num);


        sdSectorW(addr, num, 0x100000);
        
    }
#endif
#if 0
	else if (strncmp(ptr , "dos", 3) == 0) {
        //printf("dos set\n");
        cmdWhiteSpaceStrip(&ptr);
		cmdTokenStrip(&ptr);
        cmdWhiteSpaceStrip(&ptr);

        sp1kDosRsvPartSet(20UL*1024UL*1024UL); // 20M for reserved partition

        if (*ptr == 'A') {
            printf("set to A\n");
            sp1kDosRsvPartActSet(0);
        } else if (*ptr == 'B') {
            printf("set to B\n");
            sp1kDosRsvPartActSet(1);
        }
        
		G_Card_Type = K_MEDIA_NANDF;
		G_ucStorageType = G_Card_Type;
        M_Card_Initialize(1);
	}
#endif
#if 0
    else if (strncmp(ptr, "raw", 3) == 0) {
        printf("read raw\n");
        sfPhySectorR(0, (UINT32)2*1024*1024/512, 512, 0x200000, 0);
        sp1kDramFileSave("SPI_FPGARAW", 0x200000, (UINT32)2*1024*1024);
    }
#endif

	#if 0
		dosCpyCmd(ptr);
		return;
	#endif
}

#if 0 //sample code of dcfMediaFolderAssign() for recording round
void dcfAssignFolderTest(UINT8* ptr)
{
	UINT8 en;
	UINT16 dirCnt;
	
    if (strncmp(ptr, "dcf", 3) == 0) {
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
		en = *ptr - '0';
		
		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
		dirCnt = strtoul(ptr, 0, 0);

		dcfMediaFolderAssign(en, dirCnt);
	}else if (strncmp(ptr, "rnm", 3) == 0) {
		UINT8 *pt1, *pt2;

		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
		pt1 = ptr;

		cmdTokenStrip(&ptr);
		cmdWhiteSpaceStrip(&ptr);
		pt2 = ptr;
		
		*(pt2 - 1) = 0;

		printf("%s->%s\n", pt1, pt2);

		vfsFileAttrSet(pt1, pt2, 0, ATTR_MODIFY_NAME);
	}
}
#endif
