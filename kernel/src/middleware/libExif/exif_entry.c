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
 * @file		exif_entry.c
 * @brief		exif 2.2 standard
 * @author		hy.ren
 * @since		2008-03-14
 * @date		2008-10-31
 */
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/
#include "exif_entry.h"
#include "utility.h"
#include "common.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_ae_api.h"
#include "sp1k_awb_api.h"
#include "hal_dram.h"
#include "snap_qtable.h"
#include "general.h"
#include "main.h"
#include "pb_general.h"
#include "hal_gprm.h"
#include "dbg_wdt.h"
#include "sp1k_rsvblk_api.h"
#include "hal.h"
#include "initio.h"
#include "sp1k_snap_api.h"
#include "snap_task.h"
#include "dbg_mem.h"
#include "dbg_def.h"
#include "dev_init.h"


#define __FILE __FILE_ID_LIB_EXIF__

#define EXIF_RESDAT 0x48
static code UINT8 JfifHeader[5] = {0x4A,0x46,0x49,0x46,0x00};
static code UINT8 ExifHeader[6] = {0x45,0x78,0x69,0x66,0x00, 0x00};

extern UINT8 xdata G_ucStorData[];
extern xdata UINT16 G_JpgRstMcuNum;
extern code	UINT8 DHT_YDC[];
extern code	UINT8 DHT_YAC[];
extern code	UINT8 DHT_UVDC[];
extern code	UINT8 DHT_UVAC[];
static UINT8 xdata  Jpeg_Format;

xdata UINT8 exifAppN = 0;
ExifData_t *pexif_data;//ExifData_t pexif_data;
//UINT16 xdata g_ExifParam[EXIF_PARAM_MAX];
/**************************************************************************
 * 							 M A C R O S							   *
 **************************************************************************/
#define K_UNKONWN_IMAGE_FORMAT (UINT8)0//xl
#define K_JFIF_IMAGE_FORMAT   (UINT8)1
#define K_EXIF_IMAGE_FORMAT   (UINT8)2
#define FPGA_JPGENC

//#define _ExifDeBugPintf printf
#ifndef _ExifDeBugPintf
#define _ExifDeBugPintf /_printf
#endif
/**************************************************************************
 *  					   G L O B A L    D A T A   					  *
 **************************************************************************/
extern xdata UINT32 G_SHUTTER;

xdata UINT8 exifVlcAtOdd = 0;
xdata UINT8 exifQvVlcAtOdd = 0;
xdata UINT8 exifThbVlcAtOdd = 0;
xdata UINT8 exifFillRotOpt = 0;
xdata UINT16 G_JpgRstMcuNum=0;

xdata UINT32 exifQvYTblAddr;
xdata UINT32 exifQvUVTblAddr;
xdata UINT32 exifThbYTblAddr;
xdata UINT32 exifThbUVTblAddr;
xdata UINT8 isStdHufTab;
#ifdef FPGA_JPGENC
//xdata UINT8 G_awb_cur_mode;
xdata UINT32 G_ulExifThumbAddr;
xdata UINT32 G_ulExifThumbSize;
#else
//extern xdata UINT8 G_awb_cur_mode;//wangjg--
#endif

/*
	0x00:off
	0x01:force on
	0x02:auto
*/
static UINT8 exifFlashMode = 0x00;

/**************************************************************************
 *				E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
extern void jpegYQTableGen(SINT32 quality,	UINT8 *qtable);
extern void jpegUVQTableGen(SINT32 quality, UINT8 *qtable);
/**************************************************************************
 *				E X T E R N A L    R E F E R E N C E S
 **************************************************************************/

dateStc_t* exifRtcPt = 0;
/**************************************************************************
 *				I N T E R N A L    F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
IFD_t* exifGetIFD(ExifIfd exififdtype,ExifTag exifTag);
IFDs_t * exifGetIFDType(ExifIfd type);
void exifDramOddRW(UINT32 addrByte,UINT8 xdata* pt,UINT32 size,UINT8 isRead);
/**************************************************************************
 *				F U N C T I O N   D E F I N I T I O N
 **************************************************************************/

/**************************************************************************
 *  						D A T A    T Y P E S						  *
 **************************************************************************/

ExifFormatTable_t ExifFormatTable[] = {
 		{0,0},
        {EXIF_FORMAT_BYTE,    1},
        {EXIF_FORMAT_ASCII,   1},
        {EXIF_FORMAT_SHORT,   2},
        {EXIF_FORMAT_LONG,    4},
        {EXIF_FORMAT_RATIONAL, 8},
	{EXIF_FORMAT_SBYTE,    1},
	{EXIF_FORMAT_UNDEFINED,  1},
	{EXIF_FORMAT_SSHORT,    2},
        {EXIF_FORMAT_SLONG,  4},
        {EXIF_FORMAT_SRATIONAL,  8},
	{EXIF_FORMAT_FLOAT,    4},
	{EXIF_FORMAT_DOUBLE,   8}
};

#if 0
void exifParamSet(ExifParam_e id,UINT16 value)
{
	if(id > EXIF_PARAM_MAX){
		return;
	}
	g_ExifParam[id] = value;
}

UINT16 exifParamGet(ExifParam_e id)
{
	if(id > EXIF_PARAM_MAX){
		return FAIL;
	}
	return g_ExifParam[id];
}
#endif

/*
	0x00:off
	0x01:force on
	0x02:auto
*/
void exifFlashModeSet(UINT8 flag) {
	exifFlashMode = flag;
}

UINT32 exifGetApp1ItemOffset(EnumExifTag_t tag)
{
	UINT8 i ;
	for(i=0;i<pexif_data->app1.app1Count;i++){
		_ExifDeBugPintf("item=%x\n",pexif_data->app1[i].item.tag);
		if(pexif_data->app1.app1[i].item.tag == tag){
			return pexif_data->app1.app1[i].offset;
		}
	}
	return 0xFFFFFFFF;
}

UINT32 exifGetAppsItemOffset(EnumExifTag_t tag)
{
	UINT8 i ;
	for(i=0;i<pexif_data->appsCount;i++){
		_ExifDeBugPintf("tag oft=%x\n",pexif_data->apps[i].item.tag);
		if(pexif_data->apps[i].item.tag == tag){
			return pexif_data->apps[i].offset;
		}
	}
	return 0xFFFFFFFF;
}


/*brief :get App count and fill apps[] */
void exifParserApp()
{
	ExifItem_t* app = NULL;
	UINT8 *pt = pexif_data->ptData; //point g_ucstore
	UINT16 size = 0;
	UINT8 i;
	UINT8 mod=0;
	UINT32 offset = 0;
	pexif_data->appsCount = 0 ;

	/*attention please ,on my platForm + 1 means + 2*/
	offset += 1;   //skip ff d8
	HAL_DramReadBytesAddr(pexif_data->addrByte + offset, pt, 6, 0);
	for(i=0;i<20;i++){
		app = (ExifItem_t*)(pt+mod);
		/*SPCA1628 have COM segment */
		if((app->tag < APP0 || app->tag >APP15) && app->tag != ECOM ){
			break;
		}
		pexif_data->apps[i].item.tag= app->tag;
		pexif_data->apps[i].item.size = app->size;
		pexif_data->apps[i].offset = offset *2 + mod;
		pexif_data->appsCount ++ ;
		mod += app->size % 2;	//printf("tag=%x,size=%x,offset=%lx\n",pexif_data->apps[i].item.tag,app->size,pexif_data->apps[i].offset);
		offset +=(1+app->size/2);  //skip tag, the style of offet is word,not byte.
		HAL_DramReadBytesAddr(pexif_data->addrByte + offset, pt, 8, 0);
	}
	pexif_data->appsCount= i;
	//printf("appsCnt=%bu\n",i);

}


ExifItemExt_t* exifSearchApp(UINT16 appTag)
{
	UINT8 i;
	for( i=0;i<pexif_data->appsCount;i++){
		ExifItemExt_t *app = pexif_data->apps+i;
		if(app->item.tag == appTag){
			return app;
		}
	}
	return NULL;
}

UINT8 exifPaserThumbTable()
{
	UINT8 thmb_DQT_number = 0;
	UINT8 thmb_DHT_number = 0;
	//UINT8 *startAddr =  pexif_data->app1.ptStart;
	UINT32 offset, size;
	UINT16 jpgMaker;//EnumExifTag_t jpgMaker;
	UINT8 mod;
	UINT32 thbStartOffset;
	UINT8 i=0;

	ExifItemExt_t* app = exifSearchApp(0xFFE1);
	App1_t * app1 = &(pexif_data->app1);
	SOI_t *soi = NULL;
	DQT_t *dqt = NULL;
	DHT_t *dht = NULL;
	DRI_t *dri = NULL;
	SOF_t *sof = NULL;
	SOS_t *sos = NULL;
	UINT8* huffTab[4]={DHT_YDC,DHT_YAC,DHT_UVDC,DHT_UVAC};

	if(app == NULL){//App1 length is 0x5d6,more than 1024 bytes.
		return FALSE;//printf("no app1\n");
	}
	offset = app->offset + 10; // = 4(app1) + 6(exif header)

	pexif_data->app1.app1Count = 0;
	exifGetIFDValue(EXIF_IFD_1 , EXIF_TAG_JPEG_INTERCHANGE_FORMAT ,  1, &thbStartOffset); //	printf("thumbSOI,offset=%lx\n",offset);
	if( FALSE == offset) {
		__ASSERT( __FILE, __LINE__, 1);
		return FALSE;
	}
	exifGetIFDValue(EXIF_IFD_1 , EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH ,  1, &size); //	printf("size=%lx\n",size);
	_ExifDeBugPintf("thumb offset=%lx,sz=%lx\n",offset,size);
	if(FALSE == size){
		return FALSE;
	}

	HAL_DramReadBytesAddr(pexif_data->addrByte + (offset + thbStartOffset)/2, pexif_data->ptData, 1024, 0);
	mod = thbStartOffset % 2;
	offset = 0;
	do{
		jpgMaker = exif_get_short(pexif_data->ptData+ mod + offset,EXIF_BYTE_ORDER_MOTOROLA);
		_ExifDeBugPintf("jpgMaker=%x\n",jpgMaker);
		switch(jpgMaker){
			case ESOI:
				soi = (SOI_t*)(pexif_data->ptData+mod + offset);
				offset = 0;
				pexif_data->app1.app1[pexif_data->app1.app1Count].offset = offset+thbStartOffset;
				pexif_data->app1.app1[pexif_data->app1.app1Count].item.tag = jpgMaker;
				pexif_data->app1.app1Count++;
				offset += 2;
				app1->thumbImage.soi = soi;
				break;
			case EDQT: //max 2
				/*quantization table ...*/
				dqt = (DQT_t*)(pexif_data->ptData+mod + offset); //attention...
				pexif_data->app1.app1[pexif_data->app1.app1Count].offset = offset+thbStartOffset;
				pexif_data->app1.app1[pexif_data->app1.app1Count].item.tag = jpgMaker;
				size = exif_get_short(pexif_data->ptData+ mod + offset + 2,EXIF_BYTE_ORDER_MOTOROLA);
				pexif_data->app1.app1[pexif_data->app1.app1Count].item.size = size;
				pexif_data->app1.app1Count++;
				app1->thumbImage.dqt[thmb_DQT_number]  = dqt;
				thmb_DQT_number+=1;
				offset += (dqt->item.size) + 2; //skip FF d8
			break;
			case EDHT: //max 4
				/*huffman table definition ...*/
				dht = (DHT_t*)(pexif_data->ptData+mod + offset);
				pexif_data->app1.app1[pexif_data->app1.app1Count].offset = offset+thbStartOffset;
				pexif_data->app1.app1[pexif_data->app1.app1Count].item.tag = jpgMaker;
				pexif_data->app1.app1Count++;
				app1->thumbImage.dht[thmb_DHT_number++] = dht;
				offset += (dht->item.size) + 2;
				if(dht->item.size != 0x1A2 && dht->item.size != 0x1F && dht->item.size != 0x5B){
					//printf("dht er\n");
					return FALSE;
				}
			break;
			case EDRI:
				dri = (DRI_t*)(pexif_data->ptData+mod + offset);
				pexif_data->app1.app1[pexif_data->app1.app1Count].offset = offset+thbStartOffset;
				pexif_data->app1.app1[pexif_data->app1.app1Count].item.tag = jpgMaker;
				pexif_data->app1.app1Count++;
				offset += (dri->item.size) + 2;
				app1->thumbImage.dri = dri;
			break;
			case ESOF:
				/*start of frame ...*/
				sof = (SOF_t*)(pexif_data->ptData+mod + offset);
				pexif_data->app1.app1[pexif_data->app1.app1Count].offset = offset+thbStartOffset;
				pexif_data->app1.app1[pexif_data->app1.app1Count].item.tag = jpgMaker;
				pexif_data->app1.app1Count++;
				offset += (sof->item.size) + 2;
				app1->thumbImage.sof = sof ;
			break;

			case ESOS:
				break;

			default:
				i++;
				size = exif_get_short(pexif_data->ptData+ mod + offset + 2,EXIF_BYTE_ORDER_MOTOROLA);
				offset += size + 2;
			break;
		}
	}while(jpgMaker != ESOS  &&( i<=5 ) && pexif_data->app1.app1Count < EXIF_APP1_TAG_MAX_COUNT);

	/*start of scan ...*/
	jpgMaker = exif_get_short(pexif_data->ptData+ mod + offset,EXIF_BYTE_ORDER_MOTOROLA);
	if(ESOS != jpgMaker ){
		_ExifDeBugPintf("file is error ...\n");
		return FALSE;
	}
	sos = (SOS_t*)(pexif_data->ptData+offset+mod);
	pexif_data->app1.app1[pexif_data->app1.app1Count].offset = offset+thbStartOffset;
	pexif_data->app1.app1[pexif_data->app1.app1Count].item.tag = jpgMaker;
	pexif_data->app1.app1Count++;
	app1->thumbImage.sos = sos;

	return TRUE;
}

void exifParserApp0()
{
	ExifItemExt_t* app = exifSearchApp(0xFFE0);//ExifItem_t* app = exifSearchApp(0xFFE0);
	App0_t* app0 = NULL;
	UINT8* pt;
	UINT32 rst=0;
	if(app==NULL){
		_ExifDeBugPintf("no app0\n");
		//4 When JFIF[0] equals ZERO,We can be sure JFIF[0] does Not equals 0x45 when parsing the EXIF image.
		pexif_data->app0.JFIF[0]=0;
		return;
	}
	HAL_DramReadBytesAddr(pexif_data->addrByte + (app->offset+4)/2,
			pexif_data->ptData, 5, 0);

	pt = (UINT8*)pexif_data->ptData;
	if(!memcmp(JfifHeader,pt,5)){
		memcpy(pexif_data->app0.JFIF,pt,5);
		Jpeg_Format  = K_JFIF_IMAGE_FORMAT;
	}else{
		Jpeg_Format = K_UNKONWN_IMAGE_FORMAT;
	}
	//printf("app0 JPG FMT: %bu\n", Jpeg_Format);
}

IFDs_t * exifGetIFDType(ExifIfd type){
	UINT8 i;
	IFDs_t *ifds= &(pexif_data->app1.ifds); //start
	for(i=0;i<pexif_data->app1.ifdCount;i++){
		_ExifDeBugPintf("ifdstype=%bx\n",ifds[i].type);
		if(type == ifds[i].type){
			return ifds+i;
		}
	}
	return NULL;
}

void exifRegIFDList(UINT8* pt,IFDs_t* ifds,ExifIfd ifd,UINT32 offset)
{
	/*SPCA1628 is big endian*/
	UINT16 count = exif_get_short(pt,pexif_data->app1.tiff.ByteOrder);
	ifds->count = count;
	ifds->offset = offset ;
	ifds->ifds.tag = exif_get_short(pt+2, pexif_data->app1.tiff.ByteOrder);
	ifds->ifds.type =exif_get_short(pt+4, pexif_data->app1.tiff.ByteOrder);
	ifds->ifds.count = exif_get_long(pt+6, pexif_data->app1.tiff.ByteOrder);
	ifds->ifds.valueOffset =exif_get_long(pt+10, pexif_data->app1.tiff.ByteOrder);
	ifds->type = ifd;
	#if 0
	printf("  tag=%x,type=%x,count=%lx,value=%lx,ifds->offset=%lx,ifds->type=%bx,ifds->count=%x\n",ifds->ifds.tag,ifds->ifds.type,
		ifds->ifds.count,ifds->ifds.valueOffset,ifds->offset,ifds->type,ifds->count);
	#endif

}

UINT8 exifParserIFD()
{
	ExifItemExt_t* app = exifSearchApp(0xFFE1); //xian ++ 20121116
	App1_t* app1 = &(pexif_data->app1);
	UINT32 offset = 0;
	UINT32 appOffset; //xian ++ 20121116
	UINT16 tagCnt;
	UINT8 i = 0;
	UINT8 timeout=5;
	UINT8 mod=0;
	UINT8 rst;

	offset = exif_longValue_get(pexif_data->app1.tiff.IFD0Offset,pexif_data->app1.tiff.ByteOrder);
	app1->ifdCount = 0;

	/*start from here ... IFD0 ... IFD1 ...*/
	if(0xFFFFFFFF == offset){
		return FAIL;
	}
	appOffset = app->offset + 10; // 10= 4(app1) + 6(exif header)
	mod = appOffset%2;

	_ExifDeBugPintf("appOff:%lx, %lx\n", appOffset, offset);
	while(offset && timeout){
		mod += offset%2;
		HAL_DramReadBytesAddr(pexif_data->addrByte + (appOffset + offset)/2 , pexif_data->ptData, 14, 0);
		tagCnt = exif_get_short (pexif_data->ptData + mod,pexif_data->app1.tiff.ByteOrder);//printf("cnt=%u\n",tagCnt);
		exifRegIFDList(pexif_data->ptData + mod,app1->ifds + app1->ifdCount++,(ExifIfd)i++,offset + 2 + appOffset);
		HAL_DramReadBytesAddr(pexif_data->addrByte + (appOffset + offset)/2	+ (tagCnt * sizeof(IFD_t))/2 + 1, pexif_data->ptData, 4, 0);
		offset = exif_get_long(pexif_data->ptData+mod,pexif_data->app1.tiff.ByteOrder);
		timeout--;
	}
	
	if(timeout == 0){
		return FAIL;
	}
	/*Exif IFD ...*/
	timeout=5;
	rst = exifGetIFDValue(EXIF_IFD_0 , EXIF_TAG_EXIF_IFD_POINTER , 1 ,&offset);
	if((FALSE == rst)||(offset>65536)){
		return FAIL;
	}

	_ExifDeBugPintf("\nexif ifd: %lx\n", offset);
	while(offset && timeout){
		mod = offset%2;
		HAL_DramReadBytesAddr(pexif_data->addrByte + (appOffset + offset)/2, pexif_data->ptData, 14, 0);
		tagCnt = exif_get_short (pexif_data->ptData +mod,pexif_data->app1.tiff.ByteOrder);//printf("cnt=%u\n",tagCnt);
		exifRegIFDList(pexif_data->ptData+mod,app1->ifds + app1->ifdCount++,EXIF_IFD_EXIF,offset+2+appOffset);
		HAL_DramReadBytesAddr(pexif_data->addrByte + (appOffset +  offset)/2	+ (tagCnt * sizeof(IFD_t))/2 + 1, pexif_data->ptData, 4, 0);
		offset = exif_get_long(pexif_data->ptData+mod,pexif_data->app1.tiff.ByteOrder);
		timeout--;
	}
	if(timeout == 0){
		return FAIL;
	}

	#if 1 //xian modified, //hy.ren masked ,20120319 21:02,this code have probelm
	/*interpoperability ...*/
	timeout=5;
	rst = exifGetIFDValue(EXIF_IFD_EXIF , EXIF_TAG_INTEROPERABILITY_IFD_POINTER , 1 ,&offset); //bug here! UINT16 offset
	if((FALSE == rst)||(offset>65536)){
		return FAIL;
	}

	_ExifDeBugPintf("\n\INTER ifd: %lx\n", offset);
	while(offset && timeout){
		mod = offset%2;
		HAL_DramReadBytesAddr(pexif_data->addrByte + (appOffset + offset)/2, pexif_data->ptData, 14, 0);
		tagCnt = exif_get_short (pexif_data->ptData +mod,pexif_data->app1.tiff.ByteOrder);
		exifRegIFDList(pexif_data->ptData+mod,app1->ifds + app1->ifdCount++,EXIF_IFD_INTEROPERABILITY,offset+2+appOffset);
		HAL_DramReadBytesAddr(pexif_data->addrByte + (appOffset + offset)/2	+ (tagCnt * sizeof(IFD_t))/2 + 1, pexif_data->ptData,4, 0);
		offset = exif_get_long(pexif_data->ptData+mod,pexif_data->app1.tiff.ByteOrder);
		timeout--;
	}

	if(timeout == 0){
		return FAIL;

	}

	/*GPS IFD ...*/
	timeout=5;
	exifGetIFDValue(EXIF_IFD_0 , EXIF_TAG_GPS_INFO_IFD_POINTER , 1 , &offset);
	if(FALSE == offset){
		//printf("GPS tag NOT found\n");
		return SUCCESS; //attention ...
	}

	_ExifDeBugPintf("\n\GPS ifd: %lx\n", offset);
	while(offset && timeout){
		mod = offset%2;
		HAL_DramReadBytesAddr(pexif_data->addrByte + (appOffset + offset)/2, pexif_data->ptData, 14, 0);

		tagCnt = exif_get_short (pexif_data->ptData+mod,pexif_data->app1.tiff.ByteOrder);
		exifRegIFDList(pexif_data->ptData+mod,app1->ifds + app1->ifdCount++,EXIF_IFD_GPS,offset+2+appOffset);
		HAL_DramReadBytesAddr(pexif_data->addrByte + (appOffset + offset)/2	+ (tagCnt * sizeof(IFD_t))/2 + 1, pexif_data->ptData, 4, 0);
		offset = exif_get_long(pexif_data->ptData+mod,pexif_data->app1.tiff.ByteOrder);
		timeout--;
	}
	if(timeout == 0){
		return FAIL;

	}
	/*make notes*/
	#endif

	return SUCCESS;
}

UINT8 exifParserApp1()
{
	ExifItemExt_t* app = exifSearchApp(0xFFE1);//ExifItem_t* app = exifSearchApp(0xFFE1);
	UINT8* pt = (UINT8*)app;
	Tiff_t* tiff;
	UINT32 offset;
	App1_t* app1 = &(pexif_data->app1);
	UINT8 ret;
	UINT8 isJfifFmt, isExifFmt;

	if(app == NULL){//App1 length is 0x5d6,more than 1024 bytes.
		return SUCCESS;//printf("no app1\n");
	}

	pexif_data->app1.item = app->item;
	offset = app->offset;

	_ExifDeBugPintf("app1 tag =%x,size =%u\n",app1->item.tag,app1->item.size);
	HAL_DramReadBytesAddr(pexif_data->addrByte + offset/2+sizeof(app1->item)/2,
			pexif_data->ptData, sizeof(Tiff_t), 0);
	_ExifDeBugPintf("offset=%bd,size=%bd\n",sizeof(app1->item)/2 + 1,sizeof(Tiff_t)/2);
	tiff = (Tiff_t*)pexif_data->ptData;
	/* xian -- 20121116
	if(!memcmp(JfifHeader, pexif_data->app0.JFIF, 5)){
		return SUCCESS;
	}*/
	isExifFmt = !memcmp(ExifHeader,tiff->ExifString,6);
	isJfifFmt = !memcmp(JfifHeader, pexif_data->app0.JFIF, 5);	

	/*ruinan.wang  提前 ( from the end of following if)        2014.11.7*/
	memcpy(app1->tiff.ExifString, tiff->ExifString, sizeof(app1->tiff.ExifString));
	app1->tiff.ByteOrder = tiff->ByteOrder;
	app1->tiff.ID = tiff->ID;
	app1->tiff.IFD0Offset = tiff->IFD0Offset;
	ret = exifParserIFD();
						/**************/
					
	if(isJfifFmt || isExifFmt)
	{
		if(isJfifFmt){//by xl.zhang, 2011.9.20
			Jpeg_Format = K_JFIF_IMAGE_FORMAT;
			return SUCCESS; //hao.xu 20140214 
		}
		else{
			Jpeg_Format = K_EXIF_IMAGE_FORMAT;
		}
	}
	else{
		Jpeg_Format = K_UNKONWN_IMAGE_FORMAT;
		__ASSERT(__FILE, __LINE__, 1);
		return FAIL;
	}
	//printf("app1 Jpg fmt: %bu\n", Jpeg_Format);

	if(ret == FAIL){
		return FAIL;
	}

	return SUCCESS;
}

IFD_t* exifGetIFD(ExifIfd exififdtype,ExifTag exifTag)
{
	UINT8 mod;
	UINT32 offset;
	IFD_t *ifd;
	IFDs_t *ifds;
	UINT8 i;

	if((exififdtype < 0) || (exififdtype >= EXIF_IFD_COUNT)){
		return NULL;
	}
	ifds = exifGetIFDType(exififdtype);
	if(NULL == ifds){
		return NULL;
	}
	offset = ifds->offset;
	_ExifDeBugPintf("offset=%lx\n",offset);
	mod = offset % 2;
	HAL_DramReadBytesAddr(pexif_data->addrByte + offset /2, pexif_data->ptData,
								ifds->count * sizeof(IFD_t)+mod,0);
	ifd = (IFD_t*)(pexif_data->ptData+mod);
	_ExifDeBugPintf("cnt=%u\n",ifds->count);
	for(i=0;i<ifds->count;i++){
		_ExifDeBugPintf("tag=%x\n",exif_shortValue_get((ifd+i)->tag,pexif_data->app1.tiff.ByteOrder));
		if(exifTag == exif_shortValue_get((ifd+i)->tag,pexif_data->app1.tiff.ByteOrder)){
			return ifd+i;
		}
	}
	return NULL;
}

UINT32 ExifIFDOffsetGet(ExifIfd exififdtype,ExifTag exifTag)
{
	UINT8 mod;
	UINT32 offset;
	IFD_t *ifd;
	IFDs_t *ifds;
	UINT8 i;

	if((exififdtype < 0) || (exififdtype >= EXIF_IFD_COUNT)){
		return NULL;
	}
	ifds = exifGetIFDType(exififdtype);
	if(NULL == ifds){
		return NULL;
	}
	offset = ifds->offset;// printf("offset=%lx\n",offset);
	mod = offset % 2;

	HAL_DramReadBytesAddr(pexif_data->addrByte + offset /2, pexif_data->ptData,
								ifds->count * sizeof(IFD_t)+mod,0);
	ifd = (IFD_t*)(pexif_data->ptData+mod);

	for(i=0;i<ifds->count;i++){
		_ExifDeBugPintf("tag=%x\n",exif_swap_ushort((ifd+i)->tag));
		if(exifTag == exif_swap_ushort((ifd+i)->tag)){
			return (offset + i*12);
		}
	}

	return 0xFFFFFFFF;

}

UINT32 exifIFDTagOffsetGet(UINT32 addrw,ExifIfd exififdtype,ExifTag exifTag,UINT8 bypass)
{
	IFD_t *ifd;
	static ExifData_t dat1;
	UINT32 valueOffset;

	/* get resource information */
	if(!bypass){
		pexif_data = &dat1;
		pexif_data->ptData = G_ucStorData;
		sp1kNandRsvRead(0x48, addrw<<1);
		pexif_data->addrByte = addrw;
		exifParserApp();
		exifParserApp1();
	}

	if((exififdtype < 0) || (exififdtype >= EXIF_IFD_COUNT)){
		return NULL;
	}
	ifd = exifGetIFD(exififdtype,exifTag);
	if(NULL == ifd){
		return NULL;
	}

	valueOffset = exif_longValue_get(ifd->valueOffset,pexif_data->app1.tiff.ByteOrder);

}

static void exifModifyIfd(ExifIfd ifdtype, ExifTag tag,IFD_t *ifd)
{
	IFD_t *pIfd = NULL;
	UINT32 offset;

	offset = ExifIFDOffsetGet(ifdtype, tag);
	_ExifDeBugPintf("ifd offset =%ld\n",offset);
	HAL_DramReadBytesAddr(pexif_data->addrByte + 6 + offset/2, pexif_data->ptData,8, 0);
	pIfd = (IFD_t*)pexif_data->ptData;
	pIfd->tag = exif_swap_ushort(ifd->tag);
	pIfd->type = exif_swap_ushort(ifd->type);
	pIfd->count = exif_swap_ulong(ifd->count);
	HAL_DramWriteBytesAddr(pexif_data->addrByte + offset/2,pexif_data->ptData,8, 0);
}

/**
 * @fn		 void ExifViViLinkModify(ExifIfd exififdtype, ExifTag  exiftag)
 * @brief	 set customer special information
 * @param	 [in] exififdtype
 * @param	 [in] exiftag
 * @retval	  return = TRUE/FALSE
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
void ExifViViLinkModify(ExifIfd ifdtype, ExifTag tag,UINT8 *comment)
{
	#if 0
	IFD_t *ifd = NULL;
	UINT32 offset;

	offset = ExifIFDOffsetGet(exififdtype, exiftag);
	_ExifDeBugPintf("vi offset =%ld\n",offset);
	HAL_DramReadBytesAddr(pexif_data->addrByte + 6 + offset/2, pexif_data->ptData,
			8, 0);
	ifd = (IFD_t*)pexif_data->ptData;
	ifd->tag = exif_swap_ushort(0x9286);	//userComment
	ifd->type = exif_swap_ushort(0x0007);    //undefine
	ifd->count = exif_swap_ulong(0x00000008);

	HAL_DramWriteBytesAddr(pexif_data->addrByte + offset/2,
		pexif_data->ptData,8, 0);
	#endif
	IFD_t ifd;

	ifd.tag = 0x9286;
	ifd.type = 0x0007;
	ifd.count = 0x00000008;
	exifModifyIfd(ifdtype,tag,&ifd);
	exifSetIFDData(ifdtype,tag,4,
		 	((UINT16)comment[0] << 8) | (UINT16)comment[1],
		 	((UINT16)comment[2] << 8) | (UINT16)comment[3],
		 	((UINT16)comment[4] << 8) | (UINT16)comment[5],
		 	((UINT16)comment[6] << 8) | (UINT16)comment[7]
		 );
}

/**
 * @fn		 UINT8 exifSetIFDData(ExifIfd exififdtype, ExifTag  exiftag,UINT32 va_count,...)
 * @brief	 single picture image decode
 * @param	 [in] exififdtype :
 * @param	 [in] exiftag :
 * @param	 [in] va_count :
 				0: data pointer
 * @retval	  return = TRUE/FALSE
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2010-5-6
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 exifSetIFDData(ExifIfd exififdtype, ExifTag  exiftag,UINT32 va_count,...)
{
	UINT16 tag,type;
	UINT32 count,valuesize,valueOffset;
	UINT8* ptIn=NULL;
	UINT8* pt=NULL;
	UINT8 i,ifdmod,ifdsmod;
	IFD_t *ifd=NULL;
	IFDs_t* ifds;
	UINT16 j=0;

	va_list va_arg;
	va_start(va_arg,va_count);

	ifd = exifGetIFD(exififdtype, exiftag);
	if(NULL == ifd){
		//printf("setIFD[%x] not found!\n",j);
		__ASSERT( __FILE,  __LINE__, 1);
		j++;
		return FALSE;
	}

	tag = exif_shortValue_get(ifd->tag,pexif_data->app1.tiff.ByteOrder);
	type = exif_shortValue_get(ifd->type,pexif_data->app1.tiff.ByteOrder);
	count = exif_longValue_get(ifd->count,pexif_data->app1.tiff.ByteOrder);
	valueOffset = exif_longValue_get(ifd->valueOffset,pexif_data->app1.tiff.ByteOrder);
	valuesize = count * ExifFormatTable[type].size;
	ifdmod = valueOffset%2;
	_ExifDeBugPintf("setIFD:tag=%x,type=%x,count=%lx,vo=%lx,size=%lx\n",tag,type,count,valueOffset,valuesize);
	if(valuesize >4){
			HAL_DramReadBytesAddr(pexif_data->addrByte + 6 + valueOffset/2, pexif_data->ptData,
			valuesize+ifdmod, 0);
		pt = pexif_data->ptData+ifdmod;
/*		pt = pexif_data->ptData;
		exifDramOddRW(pexif_data->addrByte*2 + 12 +valueOffset, pt, valuesize, 1);
*/	}
	else{
		pt = (UINT8*)&(ifd->valueOffset);
	}
	if(0 == va_count){
		ptIn = va_arg(va_arg,UINT8*);
		memcpy(pt,ptIn,count);
	}else{
		if( count<va_count){
			//printf("count:%lx < va_count%:lx\n",count,va_count);
			__ASSERT( __FILE,  __LINE__, 1);
			return FALSE;
		}
		for(i=0;i<va_count;i++){
			switch((UINT8)type){
				case EXIF_FORMAT_RATIONAL:
				case EXIF_FORMAT_SRATIONAL:
					*((UINT32*)pt)++ = va_arg(va_arg,UINT32);
					*((UINT32*)pt)++= va_arg(va_arg,UINT32);
					break;
				case EXIF_FORMAT_LONG:
					*((UINT32*)(pt))++=va_arg(va_arg,UINT32);
					break;
				case EXIF_FORMAT_SHORT:
				case EXIF_FORMAT_SSHORT:
					*((UINT16*)(pt))++ = va_arg(va_arg,UINT16);
					break;
				case EXIF_FORMAT_ASCII:
				case EXIF_FORMAT_SBYTE:
				case EXIF_FORMAT_BYTE:
					*((UINT8*)(pt))++ = va_arg(va_arg,UINT8);
					break;
				case EXIF_FORMAT_FLOAT:
				case EXIF_FORMAT_DOUBLE:
				default:
					break;
			}
		}
	}

	if(valuesize >4){
		HAL_DramWriteBytesAddr(pexif_data->addrByte + 6 + valueOffset/2, pexif_data->ptData,
			valuesize+ifdmod, 0);
//	exifDramOddRW(pexif_data->addrByte*2+12+valueOffset, pt, valuesize, 0);
	}
	else{
		ifds = exifGetIFDType(exififdtype);
		ifdsmod = ifds->offset%2;
//		exifDramOddRW(pexif_data->addrByte*2  + ifds->offset+(pt-(pexif_data->ptData)-ifdsmod),pt,sizeof(valueOffset),0);
		HAL_DramWriteBytesAddr(pexif_data->addrByte + ifds->offset/2, pexif_data->ptData,
								ifds->count * sizeof(IFD_t)+ifdsmod,0);
	}
	va_end(va_arg);

	return TRUE;
}

/**
param ...,such as UINT16* a,UINT16* b;UINT32* c,UINT32* d;
when va_count equals 0,
*/
UINT8 exifGetIFDValue(ExifIfd exififdtype,ExifTag exiftag,UINT32 va_count,...)
{
	UINT16 tag,type;
	UINT32 count,valuesize,valueOffset;
	UINT8* ptIn;
	UINT8* pt;
	UINT8 i,mod;
	IFD_t *ifd;

	va_list va_arg;
	va_start(va_arg,va_count);

	ifd = exifGetIFD(exififdtype,exiftag);
	if(NULL == ifd){
		return FALSE;
	}

	tag = exif_shortValue_get(ifd->tag,pexif_data->app1.tiff.ByteOrder);
	type = exif_shortValue_get(ifd->type, pexif_data->app1.tiff.ByteOrder);
	count = exif_longValue_get(ifd->count, pexif_data->app1.tiff.ByteOrder);
	valueOffset = exif_longValue_get(ifd->valueOffset, pexif_data->app1.tiff.ByteOrder);
	valuesize = count*ExifFormatTable[type].size;
	_ExifDeBugPintf("GetIFD:tag=%x,type=%x,count=%lx,vo=%lx,size=%lx\n",tag,type,count,valueOffset,valuesize);
	mod = valueOffset%2;
	if(valuesize > sizeof(char)*4){
		HAL_DramReadBytesAddr(pexif_data->addrByte + 6+ valueOffset/2, pexif_data->ptData,
			valuesize+mod, 0);
		pt = pexif_data->ptData+mod;
	}
	else{
		pt = (UINT8*)&(ifd->valueOffset);//ifd has been read to  pexif_data->ptData,I can read the valueoffset in it;
	}

	if(0 == va_count){
		ptIn = va_arg(va_arg,UINT8*);
		memcpy(ptIn, pt, count);
	}
	else if(va_count > count){
		//printf("va_count > count\n");
		__ASSERT( __FILE,  __LINE__, 1);
		return FALSE;
	}
	else{
		for(i=0;i<va_count;i++){
			switch(type){
				case EXIF_FORMAT_RATIONAL:
				case EXIF_FORMAT_SRATIONAL:
					*(va_arg(va_arg,UINT32*)) = exif_swap_ulong(*((UINT32*)pt)++);
					*(va_arg(va_arg,UINT32*)) = exif_swap_ulong(*((UINT32*)pt)++);
					break;
				case EXIF_FORMAT_LONG:
				case EXIF_FORMAT_SLONG:
					*(va_arg(va_arg,UINT32*)) = exif_longValue_get(*((UINT32*)pt)++ , pexif_data->app1.tiff.ByteOrder);
					break;
				case EXIF_FORMAT_SHORT:
				case EXIF_FORMAT_SSHORT:
					*(va_arg(va_arg,UINT16*)) = exif_shortValue_get((*((UINT16*)pt)++),pexif_data->app1.tiff.ByteOrder);
					break;
				case EXIF_FORMAT_BYTE:
				case EXIF_FORMAT_SBYTE:
				case EXIF_FORMAT_ASCII:
				case EXIF_FORMAT_UNDEFINED:
				default:
					break;
			}
		}
	}
	va_end(va_arg);

	return TRUE;
}
void exifParserApp5()
{
	ExifItemExt_t* app = exifSearchApp(0xFFE5);
	App5_t* app5 = &(pexif_data->app5);
	if(app == NULL){
		exifAppN = 0;
		_ExifDeBugPintf("app5 not found\n");
		return;
	}
	exifAppN = 1;
	app5->item = app->item;
	app5->offset = app->offset;
}

#if 0
void exifParserApp6()
{
	ExifItemExt_t* app = exifSearchApp(0xFFE6);
	ExifItemExt_t* app6 = &(pexif_data->app5);
	if(app == NULL){
		_ExifDeBugPintf("app6 not found\n");
		return;
	}
	app6->item = app->item;
	app6->offset = app->offset;
}
#endif

/*brief:DQT,DHT,DRI,and SOF come after max app,and before SOS,in any order...*/
void exifParserMainTable()
{
	UINT32 offset = 0;
	UINT32 startAddr ;
	UINT16 size;
	UINT8 mod;
	UINT8 i=0;
	ExifItemExt_t *apps = pexif_data->apps;
	UINT16 jpgMaker;
	App1_t * app1 = &(pexif_data->app1);

	DQT_t *dqt = NULL;
	DHT_t *dht = NULL;
	DRI_t *dri = NULL;
	SOF_t *sof = NULL;
	SOS_t *sos = NULL;
	UINT16 mainDQTNoP=0, mainDHTNoP=0;
	UINT8* huffTab[4]={DHT_YDC,DHT_YAC,DHT_UVDC,DHT_UVAC};
	isStdHufTab = TRUE;

	/* start address after max app ... */
	if(pexif_data->appsCount){
		size = pexif_data->apps[pexif_data->appsCount-1].item.size; //这里需要获取app5的size,so app5 size must filled at first
		offset = (pexif_data->apps + (pexif_data->appsCount -1))->offset; //此处的offset指向最后一个apps的开头位置。
		startAddr = offset + size + 2;
	}
	else{ //xian ++ 20121116 for no APPn
		startAddr = 2;
	}
	_ExifDeBugPintf("cnt=%bu,size=%x\n",pexif_data->appsCount,size);
	mod = startAddr % 2;
	HAL_DramReadBytesAddr(pexif_data->addrByte + startAddr/2,pexif_data->ptData, 1024, 0);
	offset = 0;
	do{
		jpgMaker = exif_get_short(pexif_data->ptData + mod + offset ,EXIF_BYTE_ORDER_MOTOROLA);
		_ExifDeBugPintf("jpgMaker=%x,%lx\n",jpgMaker,offset);
		switch(jpgMaker){
			case EDQT:
				/*quantization table ...*/
				dqt = (DQT_t*)(pexif_data->ptData + mod + offset);
				pexif_data->apps[pexif_data->appsCount].offset = startAddr + offset;
				offset += (dqt->item.size) + 2; //skip ff d8
				pexif_data->apps[pexif_data->appsCount].item= dqt->item;
				pexif_data->appsCount++;
				app1->mainImage.dqt[mainDQTNoP] = dqt;
				mainDQTNoP+=1;
			break;
			case EDHT:
				/*huffman table definition ...*/
				dht = (DHT_t*)(pexif_data->ptData + mod + offset);
				pexif_data->apps[pexif_data->appsCount].offset = offset + startAddr;
				offset += (dht->item.size) + 2;
				pexif_data->apps[pexif_data->appsCount].item = dht->item;
				pexif_data->appsCount++;
				app1->mainImage.dht[mainDHTNoP] = dht;
				/* is standard huffman table */
				if(isStdHufTab &&!(memcmp(dht->Content0,huffTab[mainDHTNoP],sizeof(huffTab[mainDHTNoP])) ==0)){
					isStdHufTab = FALSE;
				}
				mainDHTNoP+=1;
			break;
			case EDRI:
				dri = (DRI_t*)(pexif_data->ptData + mod + offset );
				pexif_data->apps[pexif_data->appsCount].offset = offset+startAddr;
				offset += (dri->item.size) + 2;
				pexif_data->apps[pexif_data->appsCount].item = dri->item;
				pexif_data->appsCount++;
				app1->mainImage.dri = dri;
			break;
			case ESOF:
				/*start of frame ...*/
				sof = (SOF_t*)(pexif_data->ptData + mod + offset);
				pexif_data->apps[pexif_data->appsCount].offset = offset + startAddr;
				offset += (sof->item.size) + 2;
				pexif_data->apps[pexif_data->appsCount].item = sof->item;
				pexif_data->appsCount++;
				app1->mainImage.sof = sof;
			break;
			default :
				i++;
			break;	//Dose not work as wanted ,  how about return??
		}
	}while(jpgMaker != ESOS && (i<=5) );


	/*start of scan ...*/
	jpgMaker = exif_get_short(pexif_data->ptData + mod + offset ,EXIF_BYTE_ORDER_MOTOROLA);
	if(ESOS != jpgMaker ){
		_ExifDeBugPintf("Main file is error ...\n");
		return;
	}
	sos = (SOS_t*)(pexif_data->ptData + mod + offset);
	pexif_data->apps[pexif_data->appsCount].item = sos->item;
	pexif_data->apps[pexif_data->appsCount].offset = offset+startAddr;
	pexif_data->appsCount++;
	app1->mainImage.sos = sos ;
	_ExifDeBugPintf("count=%bd\n",pexif_data->appsCount);
}

void exifGetMainInfo(ExifImage_t *mainImage)
{
	UINT32 sosOffset;
	UINT32 driOffset=0;
	UINT8* qTable;

	exifParserMainTable();
	#if 1
	if( K_EXIF_IMAGE_FORMAT == Jpeg_Format){
		qTable = pexif_data->app1.mainImage.dqt[0]->yQt;
		jpegQTableSet( 0, qTable, qTable + 65, 1 );
	}
	else if(K_JFIF_IMAGE_FORMAT == Jpeg_Format ){
		qTable = pexif_data->app1.mainImage.dqt[0]->yQt;
		jpegQTableSet( 0, qTable, qTable + 69, 1 );	//Should +69 ,not 70
	}
	else{
		//printf("Unknown Fmt\n");
		__ASSERT( __FILE,  __LINE__, 1);
	}
	#else
	if((K_EXIF_IMAGE_FORMAT == Jpeg_Format)||(K_JFIF_IMAGE_FORMAT == Jpeg_Format)){
		jpegQTableSet( 0, pexif_data->app1.mainImage.dqt[0]->yQt, pexif_data->app1.mainImage.dqt[0]->uvQt, 1 );
	}
	else{
		__ASSERT( __FILE,  __LINE__, 1);
	}
	#endif
	sosOffset = exifGetAppsItemOffset(ESOS);
	_ExifDeBugPintf("main SOS = %lx\n",sosOffset);
	//printf("main SOS = %lx\n",sosOffset);
	if(0xFFFFFFFF == sosOffset){
		__ASSERT( __FILE,  __LINE__, 1);
		return;
	}
	if((sosOffset + sizeof(SOS_t)) & 0x00000001){
		exifVlcAtOdd = 1;
	}
	else{
		exifVlcAtOdd = 0;
	}

	mainImage->addr = pexif_data->addrByte + (sosOffset+ 14)/2;
	_ExifDeBugPintf("pexif-addrbyte=%lx,main=%lx\n",pexif_data->addrByte,mainImage->addr);
	mainImage->height = pexif_data->app1.mainImage.sof->Content[1]<<8 |  pexif_data->app1.mainImage.sof->Content[2];
	mainImage->width = pexif_data->app1.mainImage.sof->Content[3]<<8 |  pexif_data->app1.mainImage.sof->Content[4];

	/*Get image type if 422 or 411*/
	if(pexif_data->app1.mainImage.sof->Content[7] == 0x22 ){
		mainImage->dataType = K_File_DataType_YUV420;
	}
	else if(pexif_data->app1.mainImage.sof->Content[7] == 0x21 ){
		mainImage->dataType = K_File_DataType_YUV422;
	}
	else{
		__ASSERT( __FILE,  __LINE__, 1);
	}
	driOffset = exifGetAppsItemOffset(EDRI);
	if(/*pexif_data->app1.mainImage.dri != NULL*/0xFFFFFFFF != driOffset ){
		G_JpgRstMcuNum = (UINT16)pexif_data->app1.mainImage.dri->Content[0]<<8 | (UINT16)pexif_data->app1.mainImage.dri->Content[1];

	}

	/*The JFIF Image could not found the ORIENTATION Tag */
	if(K_EXIF_IMAGE_FORMAT == Jpeg_Format){
		exifGetIFDValue(EXIF_IFD_0,EXIF_TAG_ORIENTATION,1,&(mainImage->orientation));
	}
	else{
		mainImage->orientation = 1; // 46334
	}
}

void exifGetQVInfo(ExifImage_t *qvImage)
{
	UINT8 mod;
	App5_t app5;
	if(!exifAppN){
		qvImage = NULL;
		return;
	}
	qvImage->addr = pexif_data->addrByte + (pexif_data->app5.offset+ 4 + 136)/2;
	exifQvYTblAddr = pexif_data->addrByte + pexif_data->app5.offset/2 + 2;
	if( pexif_data->app5.offset % 2 ){
		exifQvVlcAtOdd = 1;
		mod = 1;
	}
	else{
		exifQvVlcAtOdd = 0;
		mod =0;
	}

	HAL_DramReadBytesAddr(pexif_data->addrByte + pexif_data->app5.offset/2, pexif_data->ptData, sizeof(App5_t)-sizeof(UINT32)+ mod, 0);
	pexif_data->app5.qvImage.ImgWidth = *((UINT16*)(pexif_data->ptData + sizeof(app5.item)+ (sizeof(UINT8)*0x40)*2+sizeof(UINT8)*2+sizeof(UINT16) + mod));
	pexif_data->app5.qvImage.ImgHeight= *((UINT16*)(pexif_data->ptData + sizeof(app5.item)+ (sizeof(UINT8)*0x40)*2+sizeof(UINT8)*2+sizeof(UINT16)*2 + mod));
	qvImage->height= pexif_data->app5.qvImage.ImgHeight;
	qvImage->width = pexif_data->app5.qvImage.ImgWidth;

}

void exifGetThumbInfo(ExifImage_t *thumbImage)
{
	UINT8 rst;
	UINT8* yQtable,*uvQtable;
	UINT32 thmbSOS_Offset, appOffset;
	ExifItemExt_t* app = exifSearchApp(0xFFE1);//ExifItem_t* app = exifSearchApp(0xFFE1);
	App1_t * app1 = &(pexif_data->app1);

	if(pexif_data->appsCount == 0){//no appn
		exifGetMainInfo(thumbImage);
		return;
	}

	if(FALSE == exifPaserThumbTable()){
		exifGetMainInfo(thumbImage);
		return;
	}

	appOffset = app->offset + 10; // = 4(app1) + 6(exif header)

	yQtable = (UINT8*)pexif_data->app1.thumbImage.dqt[0]->yQt;
	if(pexif_data->app1.thumbImage.dqt[0]->item.size == 0x84){//one table
		uvQtable = yQtable + 65;
	}
	else{//two tables
		uvQtable = (UINT8*)pexif_data->app1.thumbImage.dqt[1]->yQt;
	}
	rst = jpegQTableSet( 0, yQtable, uvQtable, 1 );// bug !!!

	thmbSOS_Offset = exifGetApp1ItemOffset(ESOS);	//printf("thmbso=%lx\n",thmbSOS_Offset);
	thumbImage->height= (UINT16)(pexif_data->app1.thumbImage.sof->Content[1])<<8 | pexif_data->app1.thumbImage.sof->Content[2];//WRITE8(thumbImage->width, 0, thumbImage->sof->Content[1]);//WRITE8(thumbImage->width, 1, thumbImage->sof->Content[2]);
	thumbImage->width = (UINT16)(pexif_data->app1.thumbImage.sof->Content[3]<<8 | pexif_data->app1.thumbImage.sof->Content[4]);//WRITE8(thumbImage->height,0, thumbImage->sof->Content[3]);//WRITE8(thumbImage->height,1, thumbImage->sof->Content[4]);
	exifGetIFDValue(EXIF_IFD_0,EXIF_TAG_ORIENTATION,1,&(thumbImage->orientation));
	thumbImage->addr = pexif_data->addrByte  + (appOffset + thmbSOS_Offset + 14 )/2;
	exifGetIFDValue(EXIF_IFD_1, EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH,1,&(thumbImage->vlcSize));
	_ExifDeBugPintf("ThmbAddr=%lx,Thmb VLC size=%lx\n",thumbImage->addr,thumbImage->vlcSize);
	if(thmbSOS_Offset & 0x00000001){
		exifThbVlcAtOdd = 1;
	}
	else{
		exifThbVlcAtOdd = 0;
	}

	/*Get image type if 422 or 411*/
	if(pexif_data->app1.thumbImage.sof->Content[7] == 0x22 ){
		thumbImage->dataType = K_File_DataType_YUV420;
	}
	else if(pexif_data->app1.thumbImage.sof->Content[7] == 0x21 ){
		thumbImage->dataType = K_File_DataType_YUV422;
	}
	else{
		__ASSERT( __FILE,  __LINE__, 1);
	}
}

#if 0
void exifGetJpgExifInfo(ExifImage_t *mainImage,ExifImage_t *qvImage,ExifImage_t *thbImage)
{
	if( mainImage!= NULL ){
		exifGetMainInfo(mainImage);
	}
	if( qvImage!= NULL ){
		exifGetQVInfo(qvImage);
	}
	if( thbImage!= NULL ){
		exifGetThumbInfo(thbImage);
	}
}
#endif
/*
 * attention : variable paramter's first parameter can't equal call back function,
 * otherwish the system will restart ,this is problem,but i can't understand...
 */
/**
 * @fn		  UINT8 exifParserStart(ExifImageInfo_t *image)
 * @brief	  parser exif header
 * @param	  [in] func :  call back function,such as exifGetMainInfo...
 * @param	  [in]
 * @retval	  UINT32
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2010-01-25
 * @todo	  N/A
 * @bug 	  N/A
*/
#if 0
UINT8 exifParserStart(ExifImageInfo_t *image)
{
	UINT8 ret;
	static ExifData_t dat;

	pexif_data = &dat;
	pexif_data->ptData = G_ucStorData;
	pexif_data->addrByte = image->addrW;
	pexif_data->dataSize = image->size;

	G_JpgRstMcuNum=0;
	exifAppN = 0;
	exifParserApp();
	exifParserApp0();
	ret = exifParserApp1();
	if(ret!=SUCCESS){
		return FALSE;
	}
	exifParserApp5();
	if (exifParamGet(EXIF_PARAM_THUB_INFO)){
		exifGetThumbInfo(image->thbImage);
		image->qvImage = NULL;
		image->mainImage = NULL;
	}
	if (exifParamGet(EXIF_PARAM_QV_INFO)){
		exifGetQVInfo(image->qvImage);
		image->thbImage = NULL;
		image->mainImage = NULL;
	}
	if (exifParamGet(EXIF_PARAM_MAIN_INFO)){
		exifGetMainInfo(image->mainImage);
	}
	if(exifParamGet(EXIF_PARAM_FULL_INFO)){
		exifGetJpgExifInfo(image->mainImage,image->qvImage,image->thbImage);
	}
	dat=dat;

	return TRUE;
}
#endif

static ExifData_t dat; // for exifCreatFile also
UINT8 exifParserStart(exifCallBackFun func,ExifImage_t *image,UINT32 addrByte,UINT32 size)
{
	UINT8 ret;

	pexif_data = &dat;
	pexif_data->ptData = G_ucStorData;
	pexif_data->addrByte = addrByte;
	pexif_data->dataSize = size;

	G_JpgRstMcuNum=0;
	exifAppN = 0;
	//Jpeg_Format = 0;
	exifParserApp();//printf("app\n");
	exifParserApp0();//printf("app0\n");
	ret = exifParserApp1();//printf("app1\n");
	if(ret!=SUCCESS){
		return FALSE;
	}
	exifParserApp5();//printf("app5\n");
	//exifParserApp6();
	if(func == NULL){
		return  0;
	}
	func(image);
	dat=dat;

	return TRUE;
}

static void exifModifyApp1(const exifJpgFileInfo_t *jpgInfo) USING_0
{
	UINT16 width;
	UINT16 height;
	UINT32 isoExifValue, flashExifVaule;
	UINT8 meteringMode = aeMeteringModeGet();
	SINT8 evValue = aeExpCompIdxGet();
	UINT8 isoValue;
	UINT8 awbMode = 0;
	UINT32 shutter;
	dateStc_t dateinfo;
	xdata UINT8 Text_NowDateTime[20];
	float Ev;
	float Bv;
	UINT8 tbId;

	typedef struct {
        UINT8 paramId;
        UINT32 exifValue;
	} jpgParm_t;

	jpgParm_t AeParmmeterMode[] = {
		{SP1K_AE_METER_MULTI, 0x04000000},
 		{SP1K_AE_METER_CENTER,0x02000000},
        {SP1K_AE_METER_SPOT,  0x03000000},
	};

	jpgParm_t whiteBlanceTbl[]={
		{SP1K_AWB_MODE_AUTO,0x00000000},
		{SP1K_AWB_MODE_TUNGSTENG,0x00000001},
		{SP1K_AWB_MODE_FLUORESCENT,0x00000001},
		{SP1K_AWB_MODE_DAYLIGHT,0x00000001},
		{SP1K_AWB_MODE_CLOUDY,0x00000001}
	};
	jpgParm_t lightSourceTbl[]={
		{SP1K_AWB_MODE_AUTO,0x00000000},
		{SP1K_AWB_MODE_TUNGSTENG,0x00000003},
		{SP1K_AWB_MODE_FLUORESCENT,0x00000002},
		{SP1K_AWB_MODE_DAYLIGHT,0x00000001},
		{SP1K_AWB_MODE_CLOUDY,0x0000000a}
	};
	awbMode = sp1kAwbModeGet();
	switch(evValue){//20110809
		case 0:
			evValue=0;
			break;
		case -20:
			evValue=-20;//30
			break;
		case -17:
			evValue=-17;
			break;
		case -13:
			evValue=-13;//20
			break;
		case -10:
			evValue=-10;//10
			break;
		case -7:
			evValue=-7;//10
			break;
		case -3:
			evValue=-3;//10
			break;
		case 19:
		case 20:
			evValue=20;
			break;
		case 17:
			evValue=17;
			break;
		case 13:
			evValue=13;
			break;
		case 10:
			evValue=10;
			break;
		case 7:
			evValue=7;
			break;
		case 3:
			evValue=3;
			break;
		default:
			break;
	}

	sp1kAeStatusGet(SP1K_AE_CAPTURE_ISO_VALUE,&isoValue);

	HAL_GlobalReadRTC( &dateinfo);
	HAL_GlobalRTCTransferText( &dateinfo, Text_NowDateTime );
	Text_NowDateTime[19] = 0;
	exifSetIFDData(EXIF_IFD_0, EXIF_TAG_DATE_TIME,0,Text_NowDateTime);
	exifSetIFDData(EXIF_IFD_EXIF,EXIF_TAG_DATE_TIME_ORIGINAL,0,Text_NowDateTime);
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_DIGITIZED,0, Text_NowDateTime);

	exifSetIFDData(EXIF_IFD_0, EXIF_TAG_ORIENTATION,1,exif_swap_ulong( jpgInfo->rotOpt));

	switch (exifFlashMode) {
	case 0: // off
		flashExifVaule = 0x00000000;
		break;

	case 1:	// force on
		flashExifVaule = 0x00000001;
		break;
		
	case 2:	// auto
		flashExifVaule = 0x00000001;
		break;
	}
	exifSetIFDData(EXIF_IFD_EXIF,EXIF_TAG_FLASH1,1,exif_swap_ulong( flashExifVaule/*0x00000001*/ ));  //value,flash did not fired

	width = jpgInfo->mainWidth;
	height = jpgInfo->mainHeight;
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_PIXEL_X_DIMENSION,1, exif_swap_ulong((UINT32)width));
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_PIXEL_Y_DIMENSION,1, exif_swap_ulong((UINT32)height));

	shutter = G_SHUTTER;
	_ExifDeBugPintf("G_SHUTTER=%lx,%lx,%lx\n",G_SHUTTER,shutter,exif_swap_ulong( shutter));
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_TIME, 1, exif_swap_ulong(0x0000000a),exif_swap_ulong(shutter));

	Ev = 0;
	Bv = Ev / ( float ) 10 - ( float ) 5;
	Bv = Bv * 1000;				//Ev - Sv

	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_BRIGHTNESS_VALUE, 1, exif_swap_ulong(Bv), exif_swap_ulong(1000));
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_BIAS_VALUE, 1, exif_swap_ulong(evValue),exif_swap_ulong(10));

	/* metering */
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_METERING_MODE, 1, AeParmmeterMode[meteringMode].exifValue);
	/* awb mode*/
	switch(awbMode){
		case SP1K_AWB_MODE_AUTO:
			tbId = 0;
		break;
		case SP1K_AWB_MODE_TUNGSTENG:
			tbId = 1;
		break;
		case SP1K_AWB_MODE_FLUORESCENT:
			tbId = 2;
		break;
		case SP1K_AWB_MODE_DAYLIGHT:
			tbId = 3;
		break;
		case SP1K_AWB_MODE_CLOUDY:
			tbId = 4;
		break;
		default:
			tbId = 0;
		break;
	}
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_WHITE_BALANCE, 1, exif_swap_ulong(whiteBlanceTbl[tbId].exifValue));
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_LIGHT_SOURCE, 1,exif_swap_ulong(lightSourceTbl[tbId].exifValue));

	if(!jpgInfo->viViLinkEn){
		exifSetIFDData(EXIF_IFD_EXIF,EXIF_TAG_SUBJECT_AREA,4,exif_swap_ushort( width / 2 ),exif_swap_ushort(height/2),exif_swap_ushort(width),exif_swap_ushort(height));
	}
	else{
		/* 0x5669; Vivilnk//0x7669;//0x4c6e;//0x6bD0 */
		ExifViViLinkModify(EXIF_IFD_EXIF,EXIF_TAG_SUBJECT_AREA,jpgInfo->viViLinkBuf);
	}
	exifSetIFDData(EXIF_IFD_EXIF,EXIF_TAG_SUBJECT_AREA,4,exif_swap_ushort( width / 2 ),exif_swap_ushort( height / 2 ),exif_swap_ushort( width ),exif_swap_ushort( height ));
	exifSetIFDData(EXIF_IFD_1, EXIF_TAG_ORIENTATION, 1, exif_swap_ulong( jpgInfo->rotOpt));
	exifSetIFDData(EXIF_IFD_1, EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH, 1, exif_swap_ulong((UINT32)K_THUMBNAIL_STRUCTURE_SIZE + jpgInfo->thbSize -2));
	switch(isoValue){
		case 1:
			isoExifValue = 0x00000064;
		break;
		case 2:
			isoExifValue = 0x000000c8;
		break;
		case 3:
			isoExifValue = 0x00000190;
		break;
		case 4:
			isoExifValue = 0x00000320;
		break;
		default:
		break;
	}
	exifSetIFDData(EXIF_IFD_EXIF, EXIF_TAG_ISO_SPEED_RATINGS, 1, exif_swap_ulong( isoExifValue));
}

void exifDramOddRW(UINT32 addrByte,UINT8 xdata* pt,UINT32 size,UINT8 isRead)
{
	UINT8 sts;
	UINT8 xdata* ptt;
	UINT8 ret;
	UINT8 i;
	UINT32 addr;
	_ExifDeBugPintf("%lx,%lx,%lx,%bu\n",addrByte,(UINT32)pt,size,isRead);
	if(pt>=0x4000){
		addr = (UINT32)(K_SDRAM_CodeSize*2 + pt-0x4000);
		if(isRead){
			HAL_GprmDramDma(addrByte, addr, size, 0);
		}
		else{
			HAL_GprmDramDma( addr, addrByte,size, 0);
		}
	}
	else{
		ENTER_CRITICAL(sts);
		ret = HAL_CpuRamMapSet(addrByte/2, 8UL*1024UL, (void xdata* xdata*)&ptt);//printf("%bu,%x\n",ret,(UINT16)ptt);
			if(addrByte&0x01){
				ptt+=1;
			}
			if(isRead){
				for(i=0;i<size;i++){
					pt[i]=ptt[i];
				}
			}
			else{
				for(i=0;i<size;i++){
					ptt[i]=pt[i];//printf("%bx ",pt[i]);
				}
			}
		HAL_CpuRamMapRcv();
		EXIT_CRITICAL(sts);
	}
}

static void exifModifyOddCom(UINT32 addrByte,UINT8 *comment)
{
	UINT8 i;
	JpgComOdd_t jpgCom;

	jpgCom.Marker[0] = 0XFF;
	jpgCom.Marker[1] = 0XFE;
	jpgCom.Length = sizeof(JpgComOdd_t)-2;

	if(strlen(comment)>16){
		_ExifDeBugPintf("line=%u\n",__LINE__);
	}

	for(i=0;i<16;i++){
		jpgCom.Content[i] = comment[i];
	}
	jpgCom.Content[16] = 0xFF;
	exifDramOddRW(addrByte,(UINT8*)&jpgCom,sizeof(JpgComOdd_t),0);
}

static void exifModifyEvenCom(UINT32 addrByte,UINT8 *comment)
{
	UINT8 i;
	JpgComEven_t jpgCom;

	jpgCom.Marker[0] = 0XFF;
	jpgCom.Marker[1] = 0XFE;
	jpgCom.Length = sizeof(JpgComEven_t)-2;

	if(strlen(comment)>16){
		_ExifDeBugPintf("line=%u\n",__LINE__);
	}

	for(i=0;i<16;i++){
		jpgCom.Content[i] = comment[i];
	}

	exifDramOddRW(addrByte,(UINT8*)&jpgCom,sizeof(JpgComEven_t),0);
}

static void exifModifyApp5( UINT32 addrByte,UINT32 app5Size,exifJpgFileInfo_t *qvInfo)
{
	App5_t app5;

	#if 0
	if( size < ( EXIF_APP5_SIZE + qvInfo->qvSize) ) {
		_ExifDeBugPintf( "Err-qv:%ld-app5:%ld\n", exifQvVlcSize, size );
		//return ;
	}
	#endif
	if(!app5Size){
		return;
	}
	/* ensure even aligment */
	app5Size = app5Size;
	app5.item.tag = APP5;//app5.Length = size - 2;
	app5.item.size = app5Size-2;//printf("app5 size=%lx\n",size-2); //- ff e5

	jpegQTableGen( qvInfo->qvQidx, (UINT8*)app5.qvImage.YQTbl,1);
	jpegQTableGen( qvInfo->qvQidx, (UINT8*)app5.qvImage.UVQTbl,2);

	app5.qvImage.NewMarker[0]= 0;
	app5.qvImage.NewMarker[1] = 0;
	app5.qvImage.Version = 0x01;
	app5.qvImage.ImgWidth = qvInfo->qvWidth;
	app5.qvImage.ImgHeight = qvInfo->qvHeight;

	exifDramOddRW(addrByte,(UINT8*)&app5.item,sizeof(app5.item),0);
	exifDramOddRW(addrByte+4,(UINT8*)&app5.qvImage,EXIF_APP5_SIZE,0);
	addrByte += ( EXIF_APP5_SIZE + sizeof(app5.item));

	/* copy vlc to dram */
	HAL_GprmDramDma((qvInfo->qvVlcAddrW<< 1), addrByte, qvInfo->qvSize, 1);

}

/* App6 tag for debug */
static void exifModifyApp6( UINT32 addrByte,UINT32 size,exifJpgFileInfo_t *dbgInfo)
{
	ExifItem_t app6;

	if(!size){
		return;
	}
	app6.tag = APP6;
	app6.size = size-2;

	exifDramOddRW(addrByte,(UINT8*)&app6,sizeof(app6),0);
	addrByte += sizeof(ExifItem_t);

	/* copy vlc to dram */
	HAL_GprmDramDma((dbgInfo->dbgInfoAddr<< 1), addrByte, dbgInfo->dbgInfoSize, 1);
}

static void exifModifyDQT( UINT8 idx, DQT_t* dqt ) USING_0
{
	jpegQTableGen(idx,dqt->yQt,1);
	jpegQTableGen(idx,dqt->uvQt,2);
}

void exifModifySOF( SOF_t* sof, UINT16 width, UINT16 height,UINT8 fileFormat ) USING_0
{
	/*Set image type if 422 or 411*/
	if( fileFormat == K_File_DataType_YUV420 ) {
		sof->Content[7] = 0x22;
	}
	else {
		sof->Content[7] = 0x21;
	}
	sof->Content[ 1 ] = ( UINT8 ) (( height & 0xff00 ) >> 8);
	sof->Content[ 2 ] = ( UINT8 ) (height & 0x00ff );
	sof->Content[ 3 ] = ( UINT8 ) (( width & 0xff00 ) >> 8);
	sof->Content[ 4 ] = ( UINT8 ) (width & 0x00ff );
}

UINT32 exifQvAvailableSizeGet(UINT32 thbSize)
{
	UINT32 size;

	if( thbSize & 0x00000001){
		size = (K_EXIF_STRUCTURE0_SIZE + thbSize + K_EXIF_STRUCTURE3_SIZE + K_EXIF_STRUCTURE4_SIZE + K_HDR2_SIZE );
	}
	else{
		size = (K_EXIF_STRUCTURE0_SIZE + thbSize + K_EXIF_STRUCTURE2_SIZE + K_EXIF_STRUCTURE4_SIZE + K_HDR2_SIZE );
	}

	return ((UINT32)EXIF_HDR_MAX_SIZE - size);
}

#if 0
void exifUserCommentSet(UINT8 cmCode[8], UINT8 en)
{
	snapQvHSize = wSize;
	snapQvVSize = hSize;
}

void exifSnapInfoSet(snapExifInfo_t snapExif)
{
	UINT8 rotFlipOpt;

	if( snapParamGet(SP1K_SNAP_PARAM_FORMAT) == SP1K_SNAP_FORMAT_422 ) {
		exifFileFormat = K_File_DataType_YUV422;
	}
	else {
		exifFileFormat = K_File_DataType_YUV420;
	}

	exifQvVlcAddr = snapExif.exifQvVlcAddr;
	exifThbVlcAddr = snapExif.exifThbVlcAddr;

	exifQvVlcSize = snapExif.exifQvVlcSize;
	exifThbVlcSize = snapExif.exifThbVlcSize;

	exifMainQTableIdx = snapExif.exifMainQTableIdx;
	exifQvQTableIdx = snapExif.exifQvQTableIdx;
	exifThbQTableIdx = snapExif.exifThbQTableIdx;

	//printf("exifMainQTableIdx=%bd\n",exifMainQTableIdx);
	//printf("exifQvQTableIdx=%bd\n",exifQvQTableIdx);
	//printf("exifThbQTableIdx=%bd\n",exifThbQTableIdx);

	rotFlipOpt = (UINT8)snapParamGet(SP1K_SNAP_PARAM_ROT_FLIP_OPT);

	if( rotFlipOpt & 0x01 == 0x01 ) {
		exifMainXSize = snapExif.exifMainVSize;
		exifMainYSize = snapExif.exifMainHSize;

		exifQvXSize = SNAP_CDSP_QV_VSIZE;
		exifQvYSize = SNAP_CDSP_QV_HSIZE;

		exifThbXSize = SNAP_CDSP_THB_VSIZE;

		if(snapQvVSize * 3 > snapQvHSize *4 ){
			exifThbXSize = ((UINT16)SNAP_CDSP_THB_VSIZE*3/4+7)>>3<<3;
		}

		exifThbYSize = SNAP_CDSP_THB_HSIZE;
	}
	else {
		exifMainXSize = snapExif.exifMainHSize;
		exifMainYSize = snapExif.exifMainVSize;

		exifQvXSize = SNAP_CDSP_QV_HSIZE;
		exifQvYSize = SNAP_CDSP_QV_VSIZE;

		exifThbXSize = SNAP_CDSP_THB_HSIZE;
		exifThbYSize = SNAP_CDSP_THB_VSIZE;

		if(snapQvHSize * 3 > snapQvVSize *4 ){
			exifThbYSize = ((UINT16)SNAP_CDSP_THB_VSIZE*3/4+7)>>3<<3;
		}
	}
	//printf("\033[32mexifQvXSize(%u),exifQvYSize(%u),exifThbXSize(%u),exifThbYSize(%u)\033[0m\n ",exifQvXSize,exifQvYSize,exifThbXSize,exifThbYSize);
	exifRotOpt = 1;

}

void exifVideoCapInfoSet(snapExifInfo_t snapExif)
{
	exifMainQTableIdx = snapExif.exifMainQTableIdx;
	exifThbQTableIdx = snapExif.exifThbQTableIdx;
	exifMainXSize = snapExif.exifMainHSize;
	exifMainYSize = snapExif.exifMainVSize;
	exifThbXSize = 160;
	exifThbYSize = 120;
	exifRotOpt = 1;

}

/*ViVi Link funciton */
void EXIFUserCommentSet(UINT8 cmCode[8], UINT8 en)
{
    UINT8 i;

    if(en){
        for(i = 0; i < 8; i ++){
            g_exifUserCommentCode[i] = cmCode[i];
        }
    }
    g_exifUsrCommentEnable = en;
}
#endif
#if 0
UINT8 exifUserCommentGet(UINT8 cmCode[8])
{
    UINT8 len = 0;

    if(g_exifUsrCommentEnable){
        for(; len < 8; len++){
            *cmCode ++ = g_exifUserCommentCode[len];
        }
    }
    return len;
}
#endif

dateStc_t* exifRtcSet(dateStc_t* ptRtc)
{
	if( (UINT16)ptRtc!=0xffff) {
		exifRtcPt = ptRtc;
	}
	return exifRtcPt;
}

/**
 * @fn		  UINT32 exifCreateFile(const exifJpgFileInfo_t *jpgInfo)
 * @brief	  create JPG Exif header
 * @param	  [in] jpgInfo :jpg file information
 * @param	  [in] opt     :expand parameter,not used currently
 * @retval	  UINT32       :jpg header size
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2010-01-25
 * @todo	  N/A
 * @bug 	  N/A
*/
//xian 20130709, split exifCreateFile() into exifCreateFileStart() and exifCreateFileEnd()
UINT8 exifCreateFileStart(const exifJpgFileInfo_t *jpgInfo)
{
	UINT32 rsvSize;
	UINT32 rsvAddrByte;
#if 0
	UINT32 exifAddrByte;
	UINT32 addrByteThumb;
	UINT32 addrByteCom;
	UINT32 addrByteApp5;
	UINT32 addrByteApp6;
	UINT32 addrByteMain;

	UINT32 sizeApp1;
	UINT32 sizeMain = EXIF_MAIN_TABLE_SIZE;
	UINT32 sizeCom;
	UINT32 sizeApp5;
	UINT32 sizeApp6;
	UINT32 maxApp5Size;
	UINT32 dqtOffset =0;
	UINT32 sofOffset =0;
	UINT32 exifHdrSize = 0;
	static ExifData_t dat;
#endif

	sp1kNandRsvSizeGet(0x48,NULL,&rsvSize);
	rsvAddrByte = (jpgInfo->mainVlcAddrW<<1)-rsvSize;
	pexif_data = &dat;
	pexif_data->ptData = G_ucStorData;
	pexif_data->addrByte = jpgInfo->mainVlcAddrW-rsvSize/2;
	sp1kNandRsvRead(0x48, rsvAddrByte);
	exifParserApp();
	return exifParserApp1();
}
UINT32 exifCreateFileEnd(const exifJpgFileInfo_t *jpgInfo)
{
#if 0 // move to exifCreateFileStart
	UINT32 rsvSize;
	UINT32 rsvAddrByte;
#endif
	UINT32 exifAddrByte;
	UINT32 addrByteThumb;
	UINT32 addrByteCom;
	UINT32 addrByteApp5;
	UINT32 addrByteApp6;
	UINT32 addrByteMain;

	UINT32 sizeApp1;
	UINT32 sizeMain = EXIF_MAIN_TABLE_SIZE;
	UINT32 sizeCom;
	UINT32 sizeApp5;
	UINT32 sizeApp6;
	UINT32 maxApp5Size;
	UINT32 dqtOffset =0;
	UINT32 sofOffset =0;
	UINT32 exifHdrSize = 0;
#if 0 // move to exifCreateFileStart
	ExifData_t dat;

	sp1kNandRsvSizeGet(0x48,NULL,&rsvSize);
	rsvAddrByte = (jpgInfo->mainVlcAddrW<<1)-rsvSize;
	pexif_data = &dat;
	pexif_data->ptData = G_ucStorData;
	pexif_data->addrByte = jpgInfo->mainVlcAddrW-rsvSize/2;
	sp1kNandRsvRead(0x48, rsvAddrByte);
	exifParserApp();
	exifParserApp1();
#endif

	/*fill app1 data */
	exifModifyApp1(jpgInfo);
	exifPaserThumbTable();

	sizeApp1 = pexif_data->app1.item.size;
	_ExifDeBugPintf("sizeAPP1=%ld\n",sizeApp1);
	dqtOffset = exifGetApp1ItemOffset(EDQT);
	sofOffset = exifGetApp1ItemOffset(ESOF);
	_ExifDeBugPintf("thb dqt =%ld,sof=%ld\n",dqtOffset,sofOffset);
	if(0xFFFFFFFF != dqtOffset){
		exifModifyDQT(jpgInfo->thbQidx, pexif_data->app1.thumbImage.dqt[0]);
		exifDramOddRW(pexif_data->addrByte*2 + 12 + dqtOffset, (UINT8*)pexif_data->app1.thumbImage.dqt[0], (UINT32)sizeof(DQT_t), 0);
	}
	else{
		__ASSERT( __FILE,  __LINE__, 1);
	}
	if(0xFFFFFFFF != sofOffset){
		exifModifySOF(pexif_data->app1.thumbImage.sof, jpgInfo->thbWidth, jpgInfo->thbHeight,jpgInfo->fileFormat);
		exifDramOddRW(pexif_data->addrByte*2 + 12 + sofOffset,(UINT8*)pexif_data->app1.thumbImage.sof,(UINT32)sizeof(SOF_t),0);
	}
	else{
		__ASSERT( __FILE,  __LINE__, 1);
	}

	pexif_data->app1.item.size = sizeApp1 + jpgInfo->thbSize;
	HAL_DramWriteBytesAddr(pexif_data->addrByte+2,(UINT8*)&(pexif_data->app1.item.size),2,0);
	/* malloc exif hdr size buf */
	exifHdrSize+= (pexif_data->app1.item.size+4);
	sizeApp6 = (jpgInfo->dbgInfoAddr==0 ? 0:(jpgInfo->dbgInfoSize +4));
	maxApp5Size = EXIF_HDR_MAX_SIZE-sizeApp1-sizeMain-sizeCom-jpgInfo->thbSize -sizeApp6-4; // 4 mean ff d8 ff e1
	if((jpgInfo->qvSize > maxApp5Size)||(jpgInfo->qvVlcAddrW==0)){
		sizeApp5 = 0;
	}
	else{
		sizeApp5 = (EXIF_APP5_SIZE + jpgInfo->qvSize+4);
	}
	if((exifHdrSize+sizeMain+sizeApp5 + sizeApp6)%2){
		/* even aligment */
		sizeCom = sizeof(JpgComOdd_t);
	}
	else{
		sizeCom = sizeof(JpgComEven_t);
	}
	exifHdrSize = exifHdrSize + sizeCom + sizeApp5 + sizeApp6 + sizeMain;
	exifAddrByte = (jpgInfo->mainVlcAddrW <<1)-(exifHdrSize);

	/* copy APP1 lenght without thumbnial vlc data */
	HAL_GprmDramDma((pexif_data->addrByte*2), exifAddrByte,sizeApp1+4,1);

	addrByteThumb = exifAddrByte + sizeApp1 + 4;
	/* DMA copy thumbnial vlc data */
	HAL_GprmDramDma(( jpgInfo->thbVlcAddr<<1), addrByteThumb, jpgInfo->thbSize, 1);

	/* fill comment data */
	addrByteCom = addrByteThumb + jpgInfo->thbSize;
	if(sizeCom%2){
		exifModifyOddCom(addrByteCom,jpgInfo->exifCom);
	}
	else{
		exifModifyEvenCom(addrByteCom,jpgInfo->exifCom);
	}
	/* fill app5 data,in bin file ,no app5 contents */
	addrByteApp5 = addrByteCom + sizeCom;
	exifModifyApp5(addrByteApp5,sizeApp5,jpgInfo);

	/* fill debug App6 data */
	addrByteApp6 = addrByteApp5 + sizeApp5;
	exifModifyApp6(addrByteApp6,sizeApp6,jpgInfo);

	/* fill Main info DQT DHT SOF SOS */
	addrByteMain = addrByteApp6 + sizeApp6;
	_ExifDeBugPintf("main addr = %lx\n",addrByteMain);
	exifParserMainTable();
	exifModifyDQT( jpgInfo->mainQidx, pexif_data->app1.mainImage.dqt[0]);
	exifModifySOF( pexif_data->app1.mainImage.sof, jpgInfo->mainWidth, jpgInfo->mainHeight,jpgInfo->fileFormat);
	dqtOffset = exifGetAppsItemOffset(EDQT);
	sofOffset = exifGetAppsItemOffset(ESOF);

	_ExifDeBugPintf("main dqt=%ld,sof=%ld\n",dqtOffset,sofOffset);
	exifDramOddRW((pexif_data->addrByte<<1)+ dqtOffset ,(UINT8*)pexif_data->app1.mainImage.dqt[0], (UINT32)sizeof(DQT_t),0);
	exifDramOddRW((pexif_data->addrByte<<1)+ sofOffset ,(UINT8*)pexif_data->app1.mainImage.sof, (UINT32)sizeof(SOF_t),0);
	HAL_GprmDramDma((pexif_data->addrByte<<1) + dqtOffset, addrByteMain , sizeMain, 1);

	return exifHdrSize;
}
//return exifHdrSize
UINT32 exifCreateFile(const exifJpgFileInfo_t *jpgInfo)
{
	UINT8 ret;

	ret = exifCreateFileStart(jpgInfo);
	if(ret == SUCCESS){
		return exifCreateFileEnd(jpgInfo);
	}
	else{
		return 0;
	}
}

/**
 * @fn		  UINT32 exifAddMmsHdr(UINT32 thubAddr,UINT32 thubSize,UINT16 thbW,UINT16 thbH,UINT8 qIdx)
 * @brief	  add simple jpg header( used by MMS funciton )
 * @param	  [in] thubAddr :jpg file information
 * @param	  [in] thubSize :expand parameter,not used currently
 * @param	  [in] thbW		:thumbnail widht
 * @param	  [in] thbH     :thumbnail height
 * @param     [in] qIdx     :qtable index
 * @retval	  UINT32        :jpg header size
 * @see 	  NULL
 * @author	  hy.ren
 * @since	  2010-01-25
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32 exifAddMmsHdr(UINT32 thubAddr,UINT32 thubSize,UINT16 thbW,UINT16 thbH,UINT8 qIdx)
{
	UINT8 qtable[130];
	UINT32 rsvSize,rsvOffset;
	UINT32 tmpAddrByte = (thubAddr<<1)-0x600;
	UINT32 dstAddrByte;// = (thubAddr<<1)-offset;//0x253
	UINT32 rsvAlignSize;

	qtable[0] = 0;
	qtable[65] = 1;
	sp1kNandRsvSizeGet(0x49,&rsvOffset,&rsvSize);
	rsvAlignSize = ((rsvSize+1)&~1);
	sp1kNandRsvRead(0x49,tmpAddrByte);
	dstAddrByte = (thubAddr<<1)-rsvAlignSize; //0x254
	HAL_GprmDramDma(tmpAddrByte,dstAddrByte,rsvAlignSize,1);

	HAL_DramStartAddrSet((dstAddrByte>>1)+0x118, 0);
	HAL_DramWordDataWrite(0x08, READ8(&thbH, 0));
	HAL_DramWordDataWrite(READ8(&thbH, 1), READ8(&thbW, 0));
	HAL_DramWordDataWrite(READ8(&thbW, 1), 0x03);
	HAL_DramWordDataWrite(0x01,0x21); //YUY422 for 160*120

	jpegYQTableGen((SINT32)qIdx, qtable +1 );
	jpegUVQTableGen((SINT32)qIdx, qtable +66);
	HAL_DramDmaWrite(qtable, (dstAddrByte>>1)+3, 130, 1);

	/* */
	HAL_GprmDramDma(thubAddr<<1,(thubAddr<<1)-1,thubSize,1);

	return rsvSize;
}

//only modify IFD 12bytes
UINT8 exifRenameIFD(ExifIfd exififdtype, ExifTag  exiftag,  IFD_t *newIFD)
{
	UINT8 ret = FAIL;
	UINT8 mod;
	UINT32 offset;
	UINT16 tag, type;
	UINT32 count, valueOffset, valueSize, newSize;
	IFD_t *ifd;
	IFDs_t *ifds;
	UINT8 i;

	if((exififdtype < 0) || (exififdtype >= EXIF_IFD_COUNT) || (newIFD == NULL)){
		return ret;
	}
	ifds = exifGetIFDType(exififdtype);
	if(NULL == ifds){
		return ret;
	}
	offset = ifds->offset;
	_ExifDeBugPintf("offset=%lx\n",offset);
	mod = offset % 2;
	HAL_DramReadBytesAddr(pexif_data->addrByte + offset /2, pexif_data->ptData,
								ifds->count * sizeof(IFD_t)+mod,0);
	ifd = (IFD_t*)(pexif_data->ptData+mod);
	_ExifDeBugPintf("cnt=%u\n",ifds->count);
	for(i=0;i<ifds->count;i++){
		_ExifDeBugPintf("tag=%x\n",exif_shortValue_get((ifd+i)->tag,pexif_data->app1.tiff.ByteOrder));
		if(exiftag == exif_shortValue_get((ifd+i)->tag,pexif_data->app1.tiff.ByteOrder)){
			ifd += i;

			newSize = newIFD->count* ExifFormatTable[newIFD->type].size;

			tag = exif_shortValue_get(ifd->tag,pexif_data->app1.tiff.ByteOrder);
			type = exif_shortValue_get(ifd->type,pexif_data->app1.tiff.ByteOrder);
			count = exif_longValue_get(ifd->count,pexif_data->app1.tiff.ByteOrder);
			valueOffset = exif_longValue_get(ifd->valueOffset,pexif_data->app1.tiff.ByteOrder);
			valueSize = count * ExifFormatTable[type].size;

			if(valueSize >= newSize){
				ifd->tag = exif_shortValue_get(newIFD->tag,pexif_data->app1.tiff.ByteOrder);
				ifd->type = exif_shortValue_get(newIFD->type,pexif_data->app1.tiff.ByteOrder);
				ifd->count = exif_longValue_get(newIFD->count,pexif_data->app1.tiff.ByteOrder);
				if(newSize <= 4){
					ifd->valueOffset = exif_longValue_get(newIFD->valueOffset,pexif_data->app1.tiff.ByteOrder);
				}

				HAL_DramWriteBytesAddr(pexif_data->addrByte + offset/2, pexif_data->ptData,
										ifds->count * sizeof(IFD_t)+mod,0);
				ret = SUCCESS;
			}
			break;
		}
	}

	return ret;
}

#if 0// sample code
UINT8 gpsMode = 0;
UINT32 exifGPSTagTest(const exifJpgFileInfo_t *jpgInfo)
{
	IFD_t ifd;
	UINT32 hdrSize;

	if(gpsMode == 0){
		hdrSize = exifCreateFile(jpgInfo);
	}
	else{
		exifCreateFileStart(jpgInfo);

		switch (gpsMode){
		case 1:
			printf("mode 1\n");
			exifSetIFDData(EXIF_IFD_GPS, EXIF_TAG_GPS_LATITUDE_REF, 1, 0x53/*'S'*/);
			exifSetIFDData(EXIF_IFD_GPS, EXIF_TAG_GPS_LATITUDE, 3,
				exif_swap_ulong(0x12),exif_swap_ulong(2),
				exif_swap_ulong(0X345),exif_swap_ulong(3),
				exif_swap_ulong(0x0a),exif_swap_ulong(4));
			exifSetIFDData(EXIF_IFD_GPS, EXIF_TAG_GPS_PROCESSING_METHOD, 4, 'J', 'I', 'S', 0x00);
			break;
		case 2:
			printf("mode 2\n");
			ifd.tag = 0xFFFF;
			ifd.type = 4;
			ifd.count = 1;
			ifd.valueOffset = 0x00313233;
			exifRenameIFD(EXIF_IFD_0, EXIF_TAG_GPS_INFO_IFD_POINTER, &ifd);
			break;
		default:
			break;
		}

		hdrSize = exifCreateFileEnd(jpgInfo);

	}
	printf("hdrSize: 0x%lx\n", hdrSize);

	return hdrSize;
}
#endif

