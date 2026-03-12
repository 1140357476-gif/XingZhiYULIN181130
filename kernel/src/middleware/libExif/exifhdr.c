/**************************************************************************
 *  																	  *
 *  	   Copyright (c) 2007 by Sunplus mMedia Inc.					  *
 *  																	  *
 *  This software is copyrighted by and is the property of Sunplus  	  *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This	  *
 *  software may only be used in accordance with the corresponding  	  *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.  			  *
 *  																	  *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.  							  *
 *  																	  *
 *  Sunplus mMedia Inc. reserves the right to modify this software  	  *
 *  without notice. 													  *
 *  																	  *
 *  Sunplus mMedia Inc. 												  *
 *  19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *  Hsin-Chu, Taiwan, R.O.C.											  *
 *  																	  *
 *  Author: Wen-Guo Gan 												  *
 *  																	  *
 **************************************************************************/
/**
 * @filename 	exifhdr.c
 * @brief 	The main file of exif
 * @author 	Wen-Guo
 * @since 	2008-1-23
 * @date 	2008-1-23
 */
//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "impt.h"
#include "main.h"
#include "sdramhw.h"
#include "initio.h"
#include "exifhdr.h"
#include "exiffunc.h"
#include "exifvar.h"
#include "doslink.h"
#include "uiflow.h"

//patch5.1@ada@EXIF2.2 upgrade begin
#include "math.h"
//patch5.1@ada@EXIF2.2 upgrade end


#include "sp1k_snap_api.h"
#include "snap_qtable.h"
#include "sp1k_ae_api.h"
#include "awb_api.h"//wangjg++
#include "asicreg.h"
#include "pb_general.h"

#include "hal_supp.h"
#include "hal.h"
#include "sdramhw.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "hal_global.h"
#include "sp1k_snap_api.h"
#include "vfs_api.h"
#include "snap_task.h"

#include "sp1k_exif_api.h"
#include "reg_def.h"

#if !TAG_TODO
#define HAL_RTCTransferText(a, b)
#endif
#include "dbg_wdt.h" 
#include "dbg_mem.h" 
#include "sp1k_ae_api.h"
/**************************************************************************
 *  						 C O N S T A N T S  						  *
 **************************************************************************/
#define EXIF_FIX_HDR_SIZE ( 65536 )

//1 Please refers to "exifcon.c"
#define Length_Text_ImageDescription 11UL//((UINT32) sizeof( verCustom ) )
#define Length_Text_Make 11UL//((UINT32) sizeof( Text_Make ) )
#define Length_Text_Model 11UL//((UINT32) sizeof( Text_Model ) )
#define Length_Text_ThumbOdd_Comment ((UINT32) 0x00000010)
#define Length_Text_ThumbEven_Comment ((UINT32) 0x0000000f)
#define FPGA_JPGENC


/**************************************************************************
 *  							M A C R O S 							  *
 **************************************************************************/
#define ExifDbgLevel 0

/**************************************************************************
 *  						D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *  					   G L O B A L    D A T A   					  *
 **************************************************************************/
//xdata UINT16 G_ucThumbnailWidth;
//xdata UINT16 G_ucThumbnailHeight;
static xdata UINT8 Text_NowDateTime[ 20 ];

//patch3.2@ada@0401 For EXIF
static xdata UINT16 ByteOrder;
static xdata UINT32 ByteOrderOffset;
static xdata UINT32 IFD_ThumbnaiFrameOffset;
static xdata UINT32 IFD_ThumbnaiFrameLength;
static xdata UINT16 Tiff_ID;
static xdata UINT32 Tiff_IFDOffset;
static xdata UINT32 NextIFDOffset;
static xdata UINT16 IFD_Number;
static xdata UINT32 IFD_DateTime_Offset;

//patch5.1@ada@Audio annotation begin
static xdata UINT32 IFD_Exif_Pointer_Offset;
static xdata UINT16 Exif_IFD_Number;
static xdata UINT8 RelatedWavFileName[ 13 ];
//patch5.1@ada@Audio annotation end

static xdata UINT8 Jpeg_Exif_Format;
static xdata UINT32 ExifTmpDramAddr;
static xdata UINT8 Extract_Finished;
static xdata UINT8 Extract_Thumbnail;
static xdata UINT32 HeaderLength;
static xdata UINT8 Parsing_Marker;
static xdata UINT16 APP0Length;
static xdata UINT16 APP1Length;
static xdata UINT16 APPnLength;
static xdata UINT16 ThumbAPPnLength;
static xdata UINT16 TotalThumbAPPnLength;
static xdata UINT16 TotalAPPnLength;
static xdata UINT16 COMLength;
static xdata UINT16 DQTLength;
static xdata UINT16 DHTLength;
static xdata UINT16 SOFLength;
static xdata UINT16 SOSLength;
static xdata UINT16 DRILength;
xdata UINT8 NeedReadMoreHeader;

// wenguo
xdata UINT8 exifFileFormat;
xdata UINT8 exifMainQTableIdx;
xdata UINT8 exifQvQTableIdx;
xdata UINT8 exifThbQTableIdx;
xdata UINT8 exifVlcAtOdd = 0;
xdata UINT8 exifQvVlcAtOdd = 0;
xdata UINT8 exifThbVlcAtOdd = 0;
xdata UINT8 exifFillRotOpt = 0;
xdata UINT8 exifAppN = 0;
xdata UINT16 G_JpgRstMcuNum=0;

xdata UINT16 exifMainXSize;
xdata UINT16 exifMainYSize;

xdata UINT16 exifQvXSize;
xdata UINT16 exifQvYSize;

xdata UINT16 exifThbXSize;
xdata UINT16 exifThbYSize;

xdata UINT32 exifQvVlcAddr;
xdata UINT32 exifThbVlcAddr;

xdata UINT32 exifQvVlcSize;
xdata UINT32 exifThbVlcSize;

xdata UINT32 exifRotOpt;

xdata UINT32 exifQvYTblAddr;
xdata UINT32 exifQvUVTblAddr;
xdata UINT32 exifThbYTblAddr;
xdata UINT32 exifThbUVTblAddr;

static xdata UINT32 exifApp5Size;
static xdata UINT32 exifParsingSize;

/**************************************************************************
 *  			   E X T E R N A L    R E F E R E N C E S   			  *
 **************************************************************************/

//patch5.1@ada@EXIF2.2 upgrade begin
extern unsigned char G_MANUAL_WB_Flag;
extern unsigned char G_MANUAL_SHARP_Flag;
//patch5.1@ada@EXIF2.2 upgrade end

//patch5.1@ada@EXIF2.2 upgrade begin
extern UINT8 Key_FlashLightState;
extern int Tidx;
//unsigned short AE_GetEV( int idx );
//patch5.1@ada@EXIF2.2 upgrade end

//extern xdata UINT16 G_USR_FileIndex;
extern xdata UINT8 G_USR_Company_UpperSIG[];

//extern xdata UINT8 JPEG_Audio;
extern xdata UINT32 G_SHUTTER;

extern code	UINT8 DHT_YDC[];
extern code	UINT8 DHT_YAC[];
extern code	UINT8 DHT_UVDC[];
extern code	UINT8 DHT_UVAC[];
extern code	UINT8 SOFT[];
extern code	UINT8 SOST[];
extern code UINT8 Text_Exif[];
extern code UINT8 verCustom[];

extern code UINT8 Text_Make[];
extern code UINT8 Text_Model[];
extern code UINT8 Text_Comment[];

#ifdef FPGA_JPGENC
//xdata UINT8 G_awb_cur_mode;
xdata UINT32 G_ulExifThumbAddr;
xdata UINT32 G_ulExifThumbSize;
#else
//extern xdata UINT8 G_awb_cur_mode;//wangjg--
#endif
//static UINT8 xdata dataAlig[0x10]; // for data link aligment

UINT8 g_jpgCommentMarkContect[Length_Text_ThumbOdd_Comment] = {0}; //for user modify

UINT8 g_exifUserCommentCode[8] = {0}; //20110809
UINT8 g_exifUsrCommentEnable = 0;
dateStc_t* exifRtcPt = 0;

/**************************************************************************
 *  			 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
static void EXIFHDR_Load_ExifHeader0( UINT32* pSrcDramHdrAddr, UINT32* pDestDramHdrAddr, UINT32 tmbSize );
static void EXIFHDR_Load_ExifHeader2( UINT32* pSrcDramHdrAddr, UINT32* pDestDramHdrAddr, UINT32 tmbSize );
static void EXIFHDR_Modify_SRAM_ExifHeader0( struct Exif_Header0* Hdr0, UINT32 tmbSize );
static void EXIFHDR_Modify_Exif_APP1( struct Exif_Header0* Hdr0, UINT32 tmbSize );
static void EXIFHDR_Modify_DQT( UINT8 idx, struct HDR4* hdr );
static void EXIFHDR_Copy_Exif_Header2( struct Exif_Header2* Hdr1, UINT32* pDestDramHdrAddr, UINT32 tmbSize );
static void EXIFHDR_Copy_Exif_Header3( struct Exif_Header3* Hdr2, UINT32* pDestDramHdrAddr, UINT32 tmbSize );
static void EXIFHDR_Load_ExifHeader2_SRAM2DRAM( struct HDR7* hdr1, UINT32* pDramHdrAddr );
static void EXIFHDR_Load_ExifHeader3( UINT32* pSrcDramHdrAddr, UINT32* pDestDramHdrAddr, UINT32 tmbSize );
static void EXIFHDR_DQT( UINT8 qindex, struct HDR4* table );

static void EXIFHDR_Load_ExifHeaderAppN( UINT32* pDestDramHdrAddr );
UINT8 EXIFHDR_Parsing_APP5( UINT32 DramAddr );

#if 0
static UINT8 exifTimeRec( UINT16 line );
#endif

static void EXIFHDR_Modify_IfdTag( struct Exif_Header0* pHeader0);

#ifndef FPGA_JPGENC
extern UINT8 aeMeteringModeGet( void );
extern SINT8 aeExpCompIdxGet( void );
#else
UINT8 AAA_AEMeteringModeGet( void );
SINT8 AAA_AEExpCompIdxGet( void );

#endif

#define	READ8(a,b)  (*((unsigned char *)&(a)+b)) /* for debug, exif broken */
extern xdata UINT32 snapJpegVlcAddr;/* for debug, exif broken */
extern xdata UINT32 snapMainVlcSize;/* for debug, exif broken */
/**************************************************************************
 *  			 S T A T I C   L O C A L   F U N C T I O N  			  *
 **************************************************************************/
/**************************************************************************
 *  				   G L O B A L   F U N C T I O N					  *
 **************************************************************************/

//-----------------------------------------------------------------------------
//EXIFHDR_Initialize_AllExifHeader
//-----------------------------------------------------------------------------
#if 0
UINT8 aeMeteringModeGet( void )
{
	return 0;
}
#if 0
SINT8
aeExpCompIdxGet(
	void
)
{
	return 0;
}
#endif
//extern UINT8 HAL_DramStartAddrSet(UINT32 DRAMAddr, UINT8 Read);

#endif
void EXIFHDR_Initialize_AllExifHeader( UINT32 exifAddr ) USING_0
{
	//patch5.0@richie@sdram mapping begin
#if 0
	 UINT32 InitialDramHeaderAddr = K_SDRAM_ThumbBufAddr + K_SDRAM_ThumbBufSize;//K_SDRAM_EXIF_BufAddr;
#else
	 UINT32 InitialDramHeaderAddr = exifAddr;//K_SDRAM_EXIF_BufAddr;
#endif
	//patch5.0@richie@sdram mapping end
	DbgPrint( "Init_AllExifHeader\n" );

	EXIFHDR_Initialize_ExifHeader0( &InitialDramHeaderAddr );

	EXIFHDR_Initialize_ExifHeader2( &InitialDramHeaderAddr );
	EXIFHDR_Initialize_ExifHeader3( &InitialDramHeaderAddr );
	EXIFHDR_Initialize_ExifHeader4( &InitialDramHeaderAddr );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Initialize_ExifHeader0
//-----------------------------------------------------------------------------
void EXIFHDR_Initialize_ExifHeader0( UINT32* pDramHdrAddr ) USING_0
{
	struct Exif_Header0 Header0;

	//----- Header 0 -----

	//EXIF SOI
	Header0.SOI.Marker[ 0 ] = 0xff;
	Header0.SOI.Marker[ 1 ] = 0xd8;

	//EXIF APP1
	Header0.APP1.Marker[ 0 ] = 0xff;
	Header0.APP1.Marker[ 1 ] = 0xe1;

	Header0.APP1.Length = 0x0000;

	EXIFHDR_Fill_Text( Text_Exif, Header0.APP1.ExifString, 5 );

	Header0.APP1.Pad = 0x00;

	//patch4.5@ada@improve power to shot performance begin
	Header0.APP1.Tiff_ByteOrder = 0x4949;    //little endian
	//Header0.APP1.Tiff_ByteOrder = EXIFHDR_ByteSwapOfUSHORT(0x4949);    //little endian

	Header0.APP1.Tiff_ID = 0x2a00;
	//Header0.APP1.Tiff_ID = EXIFHDR_ByteSwapOfUSHORT(0x002a);

	Header0.APP1.Tiff_IFD0Offset = 0x08000000;
	//Header0.APP1.Tiff_IFD0Offset = EXIFHDR_ByteSwapOfULONG(0x00000008);

	Header0.APP1.IFD0_NumberOfInteroperability = 0x0a00;
	//Header0.APP1.IFD0_NumberOfInteroperability = M_ByteSwapOfWord(0x000a);
#if 0
	Header0.APP1.IFD0_ImageDescriptionTag.tag = 0x0e01;
	//Header0.APP1.IFD0_ImageDescriptionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x010e);

	Header0.APP1.IFD0_ImageDescriptionTag.type = 0x0200;	//ASCII
	//Header0.APP1.IFD0_ImageDescriptionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0002);	//ASCII

	Header0.APP1.IFD0_ImageDescriptionTag.count = M_ByteSwapOfDword( Length_Text_ImageDescription );
	//Header0.APP1.IFD0_ImageDescriptionTag.count = EXIFHDR_ByteSwapOfULONG(Length_Text_ImageDescription);

	Header0.APP1.IFD0_ImageDescriptionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_ImageDescriptionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_ImageDescriptionTag.valueoffset = //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD0_ImageDescriptionValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));
#else
	if (verCustom[0] == 'X') {
		Header0.APP1.IFD0_ImageDescriptionTag.tag = 0x00; //hidden the tag
		//Header0.APP1.IFD0_ImageDescriptionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0131);		
	} else {
		Header0.APP1.IFD0_ImageDescriptionTag.tag = 0x3101; //for F/W versoin display
	}

	Header0.APP1.IFD0_ImageDescriptionTag.type = 0x0200;	//ASCII
	//Header0.APP1.IFD0_ImageDescriptionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0002);	//ASCII

	Header0.APP1.IFD0_ImageDescriptionTag.count = M_ByteSwapOfDword( Length_Text_ImageDescription );
	//Header0.APP1.IFD0_ImageDescriptionTag.count = EXIFHDR_ByteSwapOfULONG(Length_Text_ImageDescription);

	Header0.APP1.IFD0_ImageDescriptionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_ImageDescriptionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_ImageDescriptionTag.valueoffset = //offset
	//	   EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD0_ImageDescriptionValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));
#endif
	Header0.APP1.IFD0_MakeTag.tag = 0x0f01;
	//Header0.APP1.IFD0_MakeTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x010f);

	Header0.APP1.IFD0_MakeTag.type = 0x0200; //ASCII
	//Header0.APP1.IFD0_MakeTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0002); //ASCII

	Header0.APP1.IFD0_MakeTag.count = M_ByteSwapOfDword( Length_Text_Make );
	//Header0.APP1.IFD0_MakeTag.count = EXIFHDR_ByteSwapOfULONG(Length_Text_Make);

	Header0.APP1.IFD0_MakeTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_MakeValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_MakeTag.valueoffset = //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD0_MakeValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD0_ModelTag.tag = 0x1001;
	//Header0.APP1.IFD0_ModelTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0110);

	Header0.APP1.IFD0_ModelTag.type = 0x0200;     //ASCII
	//Header0.APP1.IFD0_ModelTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0002);     //ASCII

	Header0.APP1.IFD0_ModelTag.count = M_ByteSwapOfDword( Length_Text_Model );
	//Header0.APP1.IFD0_ModelTag.count = EXIFHDR_ByteSwapOfULONG(Length_Text_Model);

	Header0.APP1.IFD0_ModelTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_ModelValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_ModelTag.valueoffset = //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD0_ModelValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD0_OrientationTag.tag = 0x1201;
	//Header0.APP1.IFD0_OrientationTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0112);

	Header0.APP1.IFD0_OrientationTag.type = 0x0300;    //short
	//Header0.APP1.IFD0_OrientationTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003);    //short

	Header0.APP1.IFD0_OrientationTag.count = 0x01000000;
	//Header0.APP1.IFD0_OrientationTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD0_OrientationTag.valueoffset = EXIFHDR_ByteSwapOfULONG( exifRotOpt );   //value
	//Header0.APP1.IFD0_OrientationTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000001);   //value

	Header0.APP1.IFD0_XResolutionTag.tag = 0x1a01;
	//Header0.APP1.IFD0_XResolutionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x011a);

	Header0.APP1.IFD0_XResolutionTag.type = 0x0500;    //rational
	//Header0.APP1.IFD0_XResolutionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);    //rational

	Header0.APP1.IFD0_XResolutionTag.count = 0x01000000;
	//Header0.APP1.IFD0_XResolutionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD0_XResolutionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_XResolutionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_XResolutionTag.valueoffset = //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD0_XResolutionValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD0_YResolutionTag.tag = 0x1b01;
	//Header0.APP1.IFD0_YResolutionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x011b);

	Header0.APP1.IFD0_YResolutionTag.type = 0x0500;    //rational
	//Header0.APP1.IFD0_YResolutionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);    //rational

	Header0.APP1.IFD0_YResolutionTag.count = 0x01000000;
	//Header0.APP1.IFD0_YResolutionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD0_YResolutionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_YResolutionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_YResolutionTag.valueoffset = //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD0_YResolutionValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD0_ResolutionUnitTag.tag = 0x2801;
	//Header0.APP1.IFD0_ResolutionUnitTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0128);

	Header0.APP1.IFD0_ResolutionUnitTag.type = 0x0300; //short
	//Header0.APP1.IFD0_ResolutionUnitTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003); //short

	Header0.APP1.IFD0_ResolutionUnitTag.count = 0x01000000;
	//Header0.APP1.IFD0_ResolutionUnitTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD0_ResolutionUnitTag.valueoffset = 0x02000000;    //value
	//Header0.APP1.IFD0_ResolutionUnitTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000002);	//value

	Header0.APP1.IFD0_DateTimeTag.tag = 0x3201;
	//Header0.APP1.IFD0_DateTimeTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0132);

	Header0.APP1.IFD0_DateTimeTag.type = 0x0200;  //ASCII
	//Header0.APP1.IFD0_DateTimeTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0002);  //ASCII

	Header0.APP1.IFD0_DateTimeTag.count = 0x14000000;
	//Header0.APP1.IFD0_DateTimeTag.count = EXIFHDR_ByteSwapOfULONG(0x00000014);

	Header0.APP1.IFD0_DateTimeTag.valueoffset =    //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_DateTimeValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_DateTimeTag.valueoffset =   //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD0_DateTimeValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD0_YCbCrPositionTag.tag = 0x1302;
	//Header0.APP1.IFD0_YCbCrPositionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0213);

	Header0.APP1.IFD0_YCbCrPositionTag.type = 0x0300;  //short
	//Header0.APP1.IFD0_YCbCrPositionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003);  //short

	Header0.APP1.IFD0_YCbCrPositionTag.count = 0x01000000;
	//Header0.APP1.IFD0_YCbCrPositionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD0_YCbCrPositionTag.valueoffset = 0x02000000;	//value
	//Header0.APP1.IFD0_YCbCrPositionTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000002);    //value

	Header0.APP1.IFD0_ExifIFDPointerTag.tag = 0x6987;
	//Header0.APP1.IFD0_ExifIFDPointerTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x8769);

	Header0.APP1.IFD0_ExifIFDPointerTag.type = 0x0400; //long
	//Header0.APP1.IFD0_ExifIFDPointerTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0004); //long

	Header0.APP1.IFD0_ExifIFDPointerTag.count = 0x01000000;
	//Header0.APP1.IFD0_ExifIFDPointerTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD0_ExifIFDPointerTag.valueoffset =     //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) & Header0.APP1.Exif_IFDNumber - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_ExifIFDPointerTag.valueoffset =    //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) &Header0.APP1.Exif_IFDNumber - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD0_NextIFDOffset =   //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) & Header0.APP1.IFD1_NumberOfInteroperability - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD0_NextIFDOffset =  //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) &Header0.APP1.IFD1_NumberOfInteroperability - (UINT16) &Header0.APP1.Tiff_ByteOrder));
	//patch4.5@ada@improve power to shot performance end

	EXIFHDR_Fill_Text( verCustom, Header0.APP1.IFD0_ImageDescriptionValue, Length_Text_ImageDescription );

	EXIFHDR_Fill_Text( Text_Make, Header0.APP1.IFD0_MakeValue, Length_Text_Make );

	EXIFHDR_Fill_Text( Text_Model, Header0.APP1.IFD0_ModelValue, Length_Text_Model );

	//patch4.5@ada@improve power to shot performance begin
	Header0.APP1.IFD0_XResolutionValue[ 0 ] = 0x48000000;     //72 dpi
	//Header0.APP1.IFD0_XResolutionValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000048);     //72 dpi

	Header0.APP1.IFD0_XResolutionValue[ 1 ] = 0x01000000;
	//Header0.APP1.IFD0_XResolutionValue[1] = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD0_YResolutionValue[ 0 ] = 0x48000000;     //72 dpi
	//Header0.APP1.IFD0_YResolutionValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000048);     //72 dpi

	Header0.APP1.IFD0_YResolutionValue[ 1 ] = 0x01000000;
	//Header0.APP1.IFD0_YResolutionValue[1] = EXIFHDR_ByteSwapOfULONG(0x00000001);

	EXIFHDR_Fill_Text( Text_NowDateTime, Header0.APP1.IFD0_DateTimeValue, 20 );

	//patch5.1@ada@EXIF2.2 upgrade begin
	Header0.APP1.Exif_IFDNumber = 0x2300;
	//Header0.APP1.Exif_IFDNumber = 0x1600;
	//Header0.APP1.Exif_IFDNumber = EXIFHDR_ByteSwapOfUSHORT(0x0016);
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.Exif_ExposureTimeTag.tag = 0x9a82;
	//Header0.APP1.Exif_ExposureTimeTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x829a);

	Header0.APP1.Exif_ExposureTimeTag.type = 0x0500;   //rational
	//Header0.APP1.Exif_ExposureTimeTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);   //rational

	Header0.APP1.Exif_ExposureTimeTag.count = 0x01000000;
	//Header0.APP1.Exif_ExposureTimeTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_ExposureTimeTag.valueoffset = 	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_ExposureTimeValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_ExposureTimeTag.valueoffset =    //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.Exif_ExposureTimeValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.Exif_FNumberTag.tag = 0x9d82;
	//Header0.APP1.Exif_FNumberTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x829d);

	Header0.APP1.Exif_FNumberTag.type = 0x0500;   //rational
	//Header0.APP1.Exif_FNumberTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);   //rational

	Header0.APP1.Exif_FNumberTag.count = 0x01000000;
	//Header0.APP1.Exif_FNumberTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_FNumberTag.valueoffset =     //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_FNumberValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_FNumberTag.valueoffset =	//offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.Exif_FNumberValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.Exif_ExposureProgramTag.tag = 0x2288;
	//Header0.APP1.Exif_ExposureProgramTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x8822);

	Header0.APP1.Exif_ExposureProgramTag.type = 0x0300; 	//short
	//Header0.APP1.Exif_ExposureProgramTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003); 	//short

	Header0.APP1.Exif_ExposureProgramTag.count = 0x01000000;
	//Header0.APP1.Exif_ExposureProgramTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_ExposureProgramTag.valueoffset = 0x02000000;  //value,normal program
	//Header0.APP1.Exif_ExposureProgramTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000002);  //value,normal program

	Header0.APP1.Exif_ISOSpeedRatingsTag.tag = 0x2788;
	//Header0.APP1.Exif_ISOSpeedRatingsTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x8827);

	Header0.APP1.Exif_ISOSpeedRatingsTag.type = 0x0300; 	//short
	//Header0.APP1.Exif_ISOSpeedRatingsTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003); 	//short

	Header0.APP1.Exif_ISOSpeedRatingsTag.count = 0x01000000;
	//Header0.APP1.Exif_ISOSpeedRatingsTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_ISOSpeedRatingsTag.valueoffset = 0x64000000;  //value
	//Header0.APP1.Exif_ISOSpeedRatingsTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000064);  //value

	Header0.APP1.Exif_ExifVersionTag.tag = 0x0090;
	//Header0.APP1.Exif_ExifVersionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9000);

	Header0.APP1.Exif_ExifVersionTag.type = 0x0700; 		 //undefined
	//Header0.APP1.Exif_ExifVersionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0007); 		 //undefined

	Header0.APP1.Exif_ExifVersionTag.count = 0x04000000;
	//Header0.APP1.Exif_ExifVersionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000004);

	//patch5.1@ada@EXIF2.2 upgrade begin
	Header0.APP1.Exif_ExifVersionTag.valueoffset = 0x30323230;  //value."0220"
	//Header0.APP1.Exif_ExifVersionTag.valueoffset = 0x30323130;  //value."0210"
	//Header0.APP1.Exif_ExifVersionTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x30313230);  //value."0210"
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.Exif_DateTimeOriginalTag.tag = 0x0390;
	//Header0.APP1.Exif_DateTimeOriginalTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9003);

	Header0.APP1.Exif_DateTimeOriginalTag.type = 0x0200;	//ASCII
	//Header0.APP1.Exif_DateTimeOriginalTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0002);	//ASCII

	Header0.APP1.Exif_DateTimeOriginalTag.count = 0x14000000;
	//Header0.APP1.Exif_DateTimeOriginalTag.count = EXIFHDR_ByteSwapOfULONG(0x00000014);

	Header0.APP1.Exif_DateTimeOriginalTag.valueoffset = 	 //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_DateTimeOriginalValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_DateTimeOriginalTag.valueoffset =     //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.Exif_DateTimeOriginalValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.Exif_DateTimeDigitilizedTag.tag = 0x0490;
	//Header0.APP1.Exif_DateTimeDigitilizedTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9004);

	Header0.APP1.Exif_DateTimeDigitilizedTag.type = 0x0200;    //ASCII
	//Header0.APP1.Exif_DateTimeDigitilizedTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0002);    //ASCII

	Header0.APP1.Exif_DateTimeDigitilizedTag.count = 0x14000000;
	//Header0.APP1.Exif_DateTimeDigitilizedTag.count = EXIFHDR_ByteSwapOfULONG(0x00000014);

	Header0.APP1.Exif_DateTimeDigitilizedTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_DateTimeDigitilizedValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_DateTimeDigitilizedTag.valueoffset =	 //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.Exif_DateTimeDigitilizedValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.Exif_ComponentsConfigurationTag.tag = 0x0191;
	//Header0.APP1.Exif_ComponentsConfigurationTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9101);

	Header0.APP1.Exif_ComponentsConfigurationTag.type = 0x0700; 	//undefined
	//Header0.APP1.Exif_ComponentsConfigurationTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0007); 	//undefined

	Header0.APP1.Exif_ComponentsConfigurationTag.count = 0x04000000;
	//Header0.APP1.Exif_ComponentsConfigurationTag.count = EXIFHDR_ByteSwapOfULONG(0x00000004);

	Header0.APP1.Exif_ComponentsConfigurationTag.valueoffset = 0x01020300;  //value."1230"
	//Header0.APP1.Exif_ComponentsConfigurationTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00030201);  //value."1230"

	Header0.APP1.Exif_CompressedBitsPerPixelTag.tag = 0x0291;
	//Header0.APP1.Exif_CompressedBitsPerPixelTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9102);

	Header0.APP1.Exif_CompressedBitsPerPixelTag.type = 0x0500;    //rational
	//Header0.APP1.Exif_CompressedBitsPerPixelTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);    //rational

	Header0.APP1.Exif_CompressedBitsPerPixelTag.count = 0x01000000;
	//Header0.APP1.Exif_CompressedBitsPerPixelTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_CompressedBitsPerPixelTag.valueoffset =      //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_CompressedBitsPerPixelValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_CompressedBitsPerPixelTag.valueoffset = 	//offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.Exif_CompressedBitsPerPixelValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	//patch5.1@ada@EXIF2.2 upgrade begin
	Header0.APP1.Exif_BrightnessValueTag.tag = 0x0392;	//BrightnessValue
	Header0.APP1.Exif_BrightnessValueTag.type = 0x0a00;    //srational
	Header0.APP1.Exif_BrightnessValueTag.count = 0x01000000;
	Header0.APP1.Exif_BrightnessValueTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_BrightnessValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.Exif_ExposureBiasTag.tag = 0x0492;
	//Header0.APP1.Exif_ExposureBiasTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9204);

	Header0.APP1.Exif_ExposureBiasTag.type = 0x0a00;	//srational
	//Header0.APP1.Exif_ExposureBiasTag.type = EXIFHDR_ByteSwapOfUSHORT(0x000a);	//srational

	Header0.APP1.Exif_ExposureBiasTag.count = 0x01000000;
	//Header0.APP1.Exif_ExposureBiasTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_ExposureBiasTag.valueoffset = 	 //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_ExposureBiasValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_ExposureBiasTag.valueoffset =     //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.Exif_ExposureBiasValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.Exif_MaxApertureTag.tag = 0x0592;
	//Header0.APP1.Exif_MaxApertureTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9205);

	Header0.APP1.Exif_MaxApertureTag.type = 0x0500;    //rational
	//Header0.APP1.Exif_MaxApertureTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);    //rational

	Header0.APP1.Exif_MaxApertureTag.count = 0x01000000;
	//Header0.APP1.Exif_MaxApertureTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_MaxApertureTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_MaxApertureValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_MaxApertureTag.valueoffset =	 //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.Exif_MaxApertureValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.Exif_MeteringModeTag.tag = 0x0792;
	//Header0.APP1.Exif_MeteringModeTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9207);

	Header0.APP1.Exif_MeteringModeTag.type = 0x0300;	//short
	//Header0.APP1.Exif_MeteringModeTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003);	//short

	Header0.APP1.Exif_MeteringModeTag.count = 0x01000000;
	//Header0.APP1.Exif_MeteringModeTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_MeteringModeTag.valueoffset = 0x02000000;  //value,center-weighted average
	//Header0.APP1.Exif_MeteringModeTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000002);  //value,center-weighted average

	Header0.APP1.Exif_LightSourceTag.tag = /*0x00*/0x0892;//20110809
	//Header0.APP1.Exif_LightSourceTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9208);

	Header0.APP1.Exif_LightSourceTag.type = 0x0300;    //short
	//Header0.APP1.Exif_LightSourceTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003);    //short

	Header0.APP1.Exif_LightSourceTag.count = 0x01000000;
	//Header0.APP1.Exif_LightSourceTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_LightSourceTag.valueoffset = 0x00000000;  //value,unknown
	//Header0.APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000000);  //value,unknown

	Header0.APP1.Exif_FlashTag.tag = 0x0992;
	//Header0.APP1.Exif_FlashTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x9209);

	Header0.APP1.Exif_FlashTag.type = 0x0300;    //short
	//Header0.APP1.Exif_FlashTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003);    //short

	Header0.APP1.Exif_FlashTag.count = 0x01000000;
	//Header0.APP1.Exif_FlashTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_FlashTag.valueoffset = 0x00000000;  //value,flash did not fired
	//Header0.APP1.Exif_FlashTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000000);  //value,flash did not fired

	Header0.APP1.Exif_FocalLengthTag.tag = 0x0a92;
	//Header0.APP1.Exif_FocalLengthTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x920a);

	Header0.APP1.Exif_FocalLengthTag.type = 0x0500;    //rational
	//Header0.APP1.Exif_FocalLengthTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);    //rational

	Header0.APP1.Exif_FocalLengthTag.count = 0x01000000;
	//Header0.APP1.Exif_FocalLengthTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_FocalLengthTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_FocalLengthValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_FocalLengthTag.valueoffset =	 //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.Exif_FocalLengthValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	//patch5.1@ada@EXIF2.2 upgrade begin
    if(!g_exifUsrCommentEnable){	//20110809
            Header0.APP1.Exif_SubjectAreaTag.tag = 0x1492;	//subject area
            Header0.APP1.Exif_SubjectAreaTag.type = 0x0300;    //short
            Header0.APP1.Exif_SubjectAreaTag.count = 0x04000000;
    }
    else{
            Header0.APP1.Exif_SubjectAreaTag.tag = 0x8692;	//userComment
            Header0.APP1.Exif_SubjectAreaTag.type = 0x0700;    //undefine
            Header0.APP1.Exif_SubjectAreaTag.count = 0x08000000;
    }

	Header0.APP1.Exif_SubjectAreaTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_SubjectArea - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.Exif_FlashPixVersiontTag.tag = 0x00a0;
	//Header0.APP1.Exif_FlashPixVersiontTag.tag = EXIFHDR_ByteSwapOfUSHORT(0xa000);

	Header0.APP1.Exif_FlashPixVersiontTag.type = 0x0700;	//undefined
	//Header0.APP1.Exif_FlashPixVersiontTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0007);	//undefined

	Header0.APP1.Exif_FlashPixVersiontTag.count = 0x04000000;
	//Header0.APP1.Exif_FlashPixVersiontTag.count = EXIFHDR_ByteSwapOfULONG(0x00000004);

	Header0.APP1.Exif_FlashPixVersiontTag.valueoffset = 0x30313030;   //value
	//Header0.APP1.Exif_FlashPixVersiontTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x30303130);   //value

	Header0.APP1.Exif_ColorSpaceTag.tag = 0x01a0;
	//Header0.APP1.Exif_ColorSpaceTag.tag = EXIFHDR_ByteSwapOfUSHORT(0xa001);

	Header0.APP1.Exif_ColorSpaceTag.type = 0x0300;    //short
	//Header0.APP1.Exif_ColorSpaceTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003);    //short

	Header0.APP1.Exif_ColorSpaceTag.count = 0x01000000;
	//Header0.APP1.Exif_ColorSpaceTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_ColorSpaceTag.valueoffset = 0x01000000;   //value,sRGB
	//Header0.APP1.Exif_ColorSpaceTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000001);   //value,sRGB

	Header0.APP1.Exif_PixelXDimensionTag.tag = 0x02a0;
	//Header0.APP1.Exif_PixelXDimensionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0xa002);

	Header0.APP1.Exif_PixelXDimensionTag.type = 0x0400;    //long
	//Header0.APP1.Exif_PixelXDimensionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0004);    //long

	Header0.APP1.Exif_PixelXDimensionTag.count = 0x01000000;
	//Header0.APP1.Exif_PixelXDimensionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_PixelXDimensionTag.valueoffset = 0x80020000;   //value,640
	//Header0.APP1.Exif_PixelXDimensionTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000280);   //value,640

	Header0.APP1.Exif_PixelYDimensionTag.tag = 0x03a0;
	//Header0.APP1.Exif_PixelYDimensionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0xa003);

	Header0.APP1.Exif_PixelYDimensionTag.type = 0x0400;    //long
	//Header0.APP1.Exif_PixelYDimensionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0004);    //long

	Header0.APP1.Exif_PixelYDimensionTag.count = 0x01000000;
	//Header0.APP1.Exif_PixelYDimensionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_PixelYDimensionTag.valueoffset = 0xe001;   //value,480
	//Header0.APP1.Exif_PixelYDimensionTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x000001e0);   //value,480

	Header0.APP1.Exif_InteroperabilityIFDPointerTag.tag = 0x05a0;
	//Header0.APP1.Exif_InteroperabilityIFDPointerTag.tag = EXIFHDR_ByteSwapOfUSHORT(0xa005);

	Header0.APP1.Exif_InteroperabilityIFDPointerTag.type = 0x0400;    //ASCII
	//Header0.APP1.Exif_InteroperabilityIFDPointerTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0004);    //ASCII

	Header0.APP1.Exif_InteroperabilityIFDPointerTag.count = 0x01000000;
	//Header0.APP1.Exif_InteroperabilityIFDPointerTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_InteroperabilityIFDPointerTag.valueoffset =    //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) & Header0.APP1.NumberOfInteroperability - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.Exif_InteroperabilityIFDPointerTag.valueoffset =   //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) &Header0.APP1.NumberOfInteroperability - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.Exif_FileSourceTag.tag = 0x00a3;
	//Header0.APP1.Exif_FileSourceTag.tag = EXIFHDR_ByteSwapOfUSHORT(0xa300);

	Header0.APP1.Exif_FileSourceTag.type = 0x0700;    //undefined
	//Header0.APP1.Exif_FileSourceTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0007);    //undefined

	Header0.APP1.Exif_FileSourceTag.count = 0x01000000;
	//Header0.APP1.Exif_FileSourceTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_FileSourceTag.valueoffset = 0x03000000;   //value,DSC
	//Header0.APP1.Exif_FileSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000003);   //value,DSC

	Header0.APP1.Exif_SceneTypeTag.tag = 0x01a3;
	//Header0.APP1.Exif_SceneTypeTag.tag = EXIFHDR_ByteSwapOfUSHORT(0xa301);

	Header0.APP1.Exif_SceneTypeTag.type = 0x0700;    //undefined
	//Header0.APP1.Exif_SceneTypeTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0007);    //undefined

	Header0.APP1.Exif_SceneTypeTag.count = 0x01000000;
	//Header0.APP1.Exif_SceneTypeTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	//patch5.1@ada@Fix EXIF bug begin
	Header0.APP1.Exif_SceneTypeTag.valueoffset = 0x01000000;   //value,a directly photographed image
	//patch5.1@ada@Fix EXIF bug end

	//patch5.1@ada@EXIF2.2 upgrade begin
	Header0.APP1.Exif_CustomRenderedTag.tag = 0x01a4;	//CustomRendered
	Header0.APP1.Exif_CustomRenderedTag.type = 0x0300;    //short
	Header0.APP1.Exif_CustomRenderedTag.count = 0x01000000;
	Header0.APP1.Exif_CustomRenderedTag.valueoffset = 0x00000000;   //value, normal process

	Header0.APP1.Exif_ExposureModeTag.tag = 0x02a4;	//ExposureMode
	Header0.APP1.Exif_ExposureModeTag.type = 0x0300;	//short
	Header0.APP1.Exif_ExposureModeTag.count = 0x01000000;
	Header0.APP1.Exif_ExposureModeTag.valueoffset = 0x00000000;   //value, auto exposure

	Header0.APP1.Exif_WhiteBalanceTag.tag = 0x03a4;	//WhiteBalance
	Header0.APP1.Exif_WhiteBalanceTag.type = 0x0300;	//short
	Header0.APP1.Exif_WhiteBalanceTag.count = 0x01000000;
	Header0.APP1.Exif_WhiteBalanceTag.valueoffset = 0x00000000;   //value, auto whitebalance

	Header0.APP1.Exif_DigitalZoomRatioTag.tag = 0x04a4;	//DigitalZommRatio
	Header0.APP1.Exif_DigitalZoomRatioTag.type = 0x0500;	//rational
	Header0.APP1.Exif_DigitalZoomRatioTag.count = 0x01000000;
	Header0.APP1.Exif_DigitalZoomRatioTag.valueoffset = 	 //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_DigitalZoomRatio - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_FocalLengthIn35mmFileTag.tag = 0x05a4;	//FocalLengthIn35mmFilm
	Header0.APP1.Exif_FocalLengthIn35mmFileTag.type = 0x0300;    //short
	Header0.APP1.Exif_FocalLengthIn35mmFileTag.count = 0x01000000;
	Header0.APP1.Exif_FocalLengthIn35mmFileTag.valueoffset = 0x00000000;   //value, unknown focalength

	Header0.APP1.Exif_SceneCaptureTypeTag.tag = 0x06a4;	//SceneCaptureType
	Header0.APP1.Exif_SceneCaptureTypeTag.type = 0x0300;	//short
	Header0.APP1.Exif_SceneCaptureTypeTag.count = 0x01000000;
	Header0.APP1.Exif_SceneCaptureTypeTag.valueoffset = 0x00000000;   //value, standard

	Header0.APP1.Exif_GainControlTag.tag = 0x07a4;	//GainControl
	Header0.APP1.Exif_GainControlTag.type = 0x0300;    //short
	Header0.APP1.Exif_GainControlTag.count = 0x01000000;
	Header0.APP1.Exif_GainControlTag.valueoffset = 0x00000000;   //value, none

	Header0.APP1.Exif_ContrastTag.tag = 0x08a4;	//Contrast
	Header0.APP1.Exif_ContrastTag.type = 0x0300;	//short
	Header0.APP1.Exif_ContrastTag.count = 0x01000000;
	Header0.APP1.Exif_ContrastTag.valueoffset = 0x00000000;   //value, normal

	Header0.APP1.Exif_SaturationTag.tag = 0x09a4;	//Saturation
	Header0.APP1.Exif_SaturationTag.type = 0x0300;    //short
	Header0.APP1.Exif_SaturationTag.count = 0x01000000;
	Header0.APP1.Exif_SaturationTag.valueoffset = 0x00000000;   //value, normal

	Header0.APP1.Exif_SharpnessTag.tag = 0x0aa4;	//Sharpness
	Header0.APP1.Exif_SharpnessTag.type = 0x0300;    //short
	Header0.APP1.Exif_SharpnessTag.count = 0x01000000;
	Header0.APP1.Exif_SharpnessTag.valueoffset = 0x00000000;   //value, normal

	Header0.APP1.Exif_SubjectDistanceRangeTag.tag = 0x0ca4;	//SubjectDistanceRange
	Header0.APP1.Exif_SubjectDistanceRangeTag.type = 0x0300;	//short
	Header0.APP1.Exif_SubjectDistanceRangeTag.count = 0x01000000;
	Header0.APP1.Exif_SubjectDistanceRangeTag.valueoffset = 0x00000000;   //value, unknown
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.Exif_NextIFDOffset = 0x00000000;
	//Header0.APP1.Exif_NextIFDOffset = EXIFHDR_ByteSwapOfULONG(0x00000000);

	Header0.APP1.Exif_ExposureTimeValue[ 0 ] = 0x01000000; //1/35
	//Header0.APP1.Exif_ExposureTimeValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000001); //1/35

	Header0.APP1.Exif_ExposureTimeValue[ 1 ] = 0x23000000;
	//Header0.APP1.Exif_ExposureTimeValue[1] = EXIFHDR_ByteSwapOfULONG(0x00000023);

	// 1528 comment: F number information is here
	// Fill customer's F number here
	// The Exif_FNumberValue[ 0 ] is numerator
	// The Exif_FNumberValue 1 ] is nominator
	Header0.APP1.Exif_FNumberValue[0] = 0x20000000; //32/10
	//Header0.APP1.Exif_FNumberValue[0] = 0x1c000000; //28/10
	//Header0.APP1.Exif_FNumberValue[0] = 0x14000000; //20/10
	//Header0.APP1.Exif_FNumberValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000014); //20/10
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.Exif_FNumberValue[ 1 ] = 0x0a000000;
	//Header0.APP1.Exif_FNumberValue[1] = EXIFHDR_ByteSwapOfULONG(0x0000000a);

	EXIFHDR_Fill_Text( Text_NowDateTime, Header0.APP1.Exif_DateTimeOriginalValue, 20 );

	EXIFHDR_Fill_Text( Text_NowDateTime, Header0.APP1.Exif_DateTimeDigitilizedValue, 20 );

	Header0.APP1.Exif_CompressedBitsPerPixelValue[ 0 ] = 0x02000000; //2/1
	//Header0.APP1.Exif_CompressedBitsPerPixelValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000002); //2/1

	Header0.APP1.Exif_CompressedBitsPerPixelValue[ 1 ] = 0x01000000;
	//Header0.APP1.Exif_CompressedBitsPerPixelValue[1] = EXIFHDR_ByteSwapOfULONG(0x00000001);

	//patch5.1@ada@EXIF2.2 upgrade begin
	Header0.APP1.Exif_BrightnessValue[ 0 ] = 0x00000000;
	Header0.APP1.Exif_BrightnessValue[ 1 ] = 0xffffffff;
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.Exif_ExposureBiasValue[ 0 ] = 0x00000000;
	//Header0.APP1.Exif_ExposureBiasValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000000);

	Header0.APP1.Exif_ExposureBiasValue[ 1 ] = 0x0a000000;
	//Header0.APP1.Exif_ExposureBiasValue[1] = EXIFHDR_ByteSwapOfULONG(0x0000000a);

	Header0.APP1.Exif_MaxApertureValue[ 0 ] = 0x02000000;
	//Header0.APP1.Exif_MaxApertureValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000002);

	Header0.APP1.Exif_MaxApertureValue[ 1 ] = 0x01000000;
	//Header0.APP1.Exif_MaxApertureValue[1] = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.Exif_FocalLengthValue[ 0 ] = 0x51000000;
	//Header0.APP1.Exif_FocalLengthValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000051);

	Header0.APP1.Exif_FocalLengthValue[ 1 ] = 0x0a000000;
	//Header0.APP1.Exif_FocalLengthValue[1] = EXIFHDR_ByteSwapOfULONG(0x0000000a);

	//patch5.1@ada@EXIF2.2 upgrade begin
	Header0.APP1.Exif_SubjectArea[ 0 ] = 0x0000;
	Header0.APP1.Exif_SubjectArea[ 1 ] = 0x0000;
	Header0.APP1.Exif_SubjectArea[ 2 ] = 0x0000;
	Header0.APP1.Exif_SubjectArea[ 3 ] = 0x0000;
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.NumberOfInteroperability = 0x0200;
	//Header0.APP1.NumberOfInteroperability = EXIFHDR_ByteSwapOfUSHORT(0x0002);

	Header0.APP1.InteroperabilityIndexTag.tag = 0x0100;
	//Header0.APP1.InteroperabilityIndexTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0001);

	Header0.APP1.InteroperabilityIndexTag.type = 0x0200;
	//Header0.APP1.InteroperabilityIndexTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0002);

	Header0.APP1.InteroperabilityIndexTag.count = 0x04000000;
	//Header0.APP1.InteroperabilityIndexTag.count = EXIFHDR_ByteSwapOfULONG(0x00000004);

	Header0.APP1.InteroperabilityIndexTag.valueoffset = 0x52393800;   //"R98"
	//Header0.APP1.InteroperabilityIndexTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00383952);   //"R98"

	Header0.APP1.InteroperabilityVersionTag.tag = 0x0200;
	//Header0.APP1.InteroperabilityVersionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0002);

	Header0.APP1.InteroperabilityVersionTag.type = 0x0700;
	//Header0.APP1.InteroperabilityVersionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0007);

	Header0.APP1.InteroperabilityVersionTag.count = 0x04000000;
	//Header0.APP1.InteroperabilityVersionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000004);

	Header0.APP1.InteroperabilityVersionTag.valueoffset = 0x30313030;
	//Header0.APP1.InteroperabilityVersionTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x30303130);

	Header0.APP1.NextInteroperabilityIFDOffset = 0x00000000;
	//Header0.APP1.NextInteroperabilityIFDOffset = EXIFHDR_ByteSwapOfULONG(0x00000000);

	//patch5.1@ada@EXIF2.2 upgrade begin
	Header0.APP1.Exif_DigitalZoomRatio[ 0 ] = 0x00000000;
	Header0.APP1.Exif_DigitalZoomRatio[ 1 ] = 0x01000000;
	//patch5.1@ada@EXIF2.2 upgrade end

	Header0.APP1.IFD1_NumberOfInteroperability = 0x0800;
	//Header0.APP1.IFD1_NumberOfInteroperability = EXIFHDR_ByteSwapOfUSHORT(0x0008);

	Header0.APP1.IFD1_CompressionTag.tag = 0x0301;
	//Header0.APP1.IFD1_CompressionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0103);

	Header0.APP1.IFD1_CompressionTag.type = 0x0300; 	//short
	//Header0.APP1.IFD1_CompressionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003); 	//short

	Header0.APP1.IFD1_CompressionTag.count = 0x01000000;
	//Header0.APP1.IFD1_CompressionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_CompressionTag.valueoffset = 0x06000000;   //value,thumbnail is JPEG compressed
	//Header0.APP1.IFD1_CompressionTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000006);   //value,thumbnail is JPEG compressed

	Header0.APP1.IFD1_OrientationTag.tag = 0x1201;
	//Header0.APP1.IFD1_OrientationTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0112);

	Header0.APP1.IFD1_OrientationTag.type = 0x0300;    //short
	//Header0.APP1.IFD1_OrientationTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003);    //short

	Header0.APP1.IFD1_OrientationTag.count = 0x01000000;
	//Header0.APP1.IFD1_OrientationTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_OrientationTag.valueoffset = EXIFHDR_ByteSwapOfULONG( exifRotOpt );   //value
	//Header0.APP1.IFD1_OrientationTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000001);   //value

	Header0.APP1.IFD1_XResolutionTag.tag = 0x1a01;
	//Header0.APP1.IFD1_XResolutionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x011a);

	Header0.APP1.IFD1_XResolutionTag.type = 0x0500;    //rational
	//Header0.APP1.IFD1_XResolutionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);    //rational

	Header0.APP1.IFD1_XResolutionTag.count = 0x01000000;
	//Header0.APP1.IFD1_XResolutionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_XResolutionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD1_XResolutionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD1_XResolutionTag.valueoffset = //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD1_XResolutionValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD1_YResolutionTag.tag = 0x1b01;
	//Header0.APP1.IFD1_YResolutionTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x011b);

	Header0.APP1.IFD1_YResolutionTag.type = 0x0500;    //rational
	//Header0.APP1.IFD1_YResolutionTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0005);    //rational

	Header0.APP1.IFD1_YResolutionTag.count = 0x01000000;
	//Header0.APP1.IFD1_YResolutionTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_YResolutionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD1_YResolutionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD1_YResolutionTag.valueoffset = //offset
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.APP1.IFD1_YResolutionValue - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD1_ResolutionUnitTag.tag = 0x2801;
	//Header0.APP1.IFD1_ResolutionUnitTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0128);

	Header0.APP1.IFD1_ResolutionUnitTag.type = 0x0300; //short
	//Header0.APP1.IFD1_ResolutionUnitTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003); //short

	Header0.APP1.IFD1_ResolutionUnitTag.count = 0x01000000;
	//Header0.APP1.IFD1_ResolutionUnitTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_ResolutionUnitTag.valueoffset = 0x02000000;    //value
	//Header0.APP1.IFD1_ResolutionUnitTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000002);	//value

	Header0.APP1.IFD1_JPEGInterChangeFormatTag.tag = 0x0102;
	//Header0.APP1.IFD1_JPEGInterChangeFormatTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0201);

	Header0.APP1.IFD1_JPEGInterChangeFormatTag.type = 0x0400; //long
	//Header0.APP1.IFD1_JPEGInterChangeFormatTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0004); //long

	Header0.APP1.IFD1_JPEGInterChangeFormatTag.count = 0x01000000;
	//Header0.APP1.IFD1_JPEGInterChangeFormatTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_JPEGInterChangeFormatTag.valueoffset = M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.THUMB_SOI.Marker - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );
	//Header0.APP1.IFD1_JPEGInterChangeFormatTag.valueoffset =
	//     EXIFHDR_ByteSwapOfULONG((UINT32) ((UINT16) Header0.THUMB_SOI.Marker - (UINT16) &Header0.APP1.Tiff_ByteOrder));

	Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.tag = 0x0202;
	//Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0202);

	Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.type = 0x0400; //long
	//Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0004); //long

	Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.count = 0x01000000;
	//Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.valueoffset = M_ByteSwapOfDword( K_THUMBNAIL_STRUCTURE_SIZE );    //value
	//Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.valueoffset = EXIFHDR_ByteSwapOfULONG(K_THUMBNAIL_STRUCTURE_SIZE);    //value

	Header0.APP1.IFD1_YCbCrPositioningTag.tag = 0x1302;
	//Header0.APP1.IFD1_YCbCrPositioningTag.tag = EXIFHDR_ByteSwapOfUSHORT(0x0213);

	Header0.APP1.IFD1_YCbCrPositioningTag.type = 0x0300;  //short
	//Header0.APP1.IFD1_YCbCrPositioningTag.type = EXIFHDR_ByteSwapOfUSHORT(0x0003);  //short

	Header0.APP1.IFD1_YCbCrPositioningTag.count = 0x01000000;
	//Header0.APP1.IFD1_YCbCrPositioningTag.count = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_YCbCrPositioningTag.valueoffset = 0x02000000;    //value
	//Header0.APP1.IFD1_YCbCrPositioningTag.valueoffset = EXIFHDR_ByteSwapOfULONG(0x00000002);    //value

	Header0.APP1.IFD1_NextIFDOffset = 0x00000000;
	//Header0.APP1.IFD1_NextIFDOffset = EXIFHDR_ByteSwapOfULONG(0x00000000);

	Header0.APP1.IFD1_XResolutionValue[ 0 ] = 0x48000000;     //72 dpi
	//Header0.APP1.IFD1_XResolutionValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000048);     //72 dpi

	Header0.APP1.IFD1_XResolutionValue[ 1 ] = 0x01000000;
	//Header0.APP1.IFD1_XResolutionValue[1] = EXIFHDR_ByteSwapOfULONG(0x00000001);

	Header0.APP1.IFD1_YResolutionValue[ 0 ] = 0x48000000;     //72 dpi
	//Header0.APP1.IFD1_YResolutionValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000048);     //72 dpi

	Header0.APP1.IFD1_YResolutionValue[ 1 ] = 0x01000000;
	//Header0.APP1.IFD1_YResolutionValue[1] = EXIFHDR_ByteSwapOfULONG(0x00000001);
	//patch4.5@ada@improve power to shot performance end

	//Thumbnail SOI
	Header0.THUMB_SOI.Marker[ 0 ] = 0xff;
	Header0.THUMB_SOI.Marker[ 1 ] = 0xd8;

	//Thumbnail DQT
	//     EXIFHDR_DQT(G_QtableIndex,&Header0.THUMB_DQT);  //ada@0225
	EXIFHDR_DQT( exifThbQTableIdx, &Header0.THUMB_DQT );

	//Thumbnail DHT
	EXIFHDR_DHT( &Header0.THUMB_DHT );

	//Thumbnail SOF
	EXIFHDR_SOF( &Header0.THUMB_SOF, exifThbXSize, exifThbYSize );

	//Thumbnail SOS
	EXIFHDR_SOS( &Header0.THUMB_SOS );

	EXIFHDR_Load_ExifHeader0_SRAM2DRAM( &Header0.SOI, pDramHdrAddr );
}

#if 0
//patch5.1@ada@Audio annotation begin
//-----------------------------------------------------------------------------
//EXIFHDR_Initialize_ExifHeader0_Audio
//-----------------------------------------------------------------------------
void EXIFHDR_Initialize_ExifHeader0_Audio( UINT32* pDramHdrAddr ) USING_0
{
	struct Exif_Header0_Audio Header0;

	//----- Header 0 -----

	//EXIF SOI
	Header0.SOI.Marker[ 0 ] = 0xff;
	Header0.SOI.Marker[ 1 ] = 0xd8;

	//EXIF APP1
	Header0.APP1.Marker[ 0 ] = 0xff;
	Header0.APP1.Marker[ 1 ] = 0xe1;

	Header0.APP1.Length = 0x0000;

	EXIFHDR_Fill_Text( Text_Exif, Header0.APP1.ExifString, 5 );

	Header0.APP1.Pad = 0x00;

	Header0.APP1.Tiff_ByteOrder = 0x4949;    //little endian

	Header0.APP1.Tiff_ID = 0x2a00;

	Header0.APP1.Tiff_IFD0Offset = 0x08000000;

	Header0.APP1.IFD0_NumberOfInteroperability = 0x0a00;

	Header0.APP1.IFD0_ImageDescriptionTag.tag = 0x0e01;

	Header0.APP1.IFD0_ImageDescriptionTag.type = 0x0200;	//ASCII

	Header0.APP1.IFD0_ImageDescriptionTag.count = M_ByteSwapOfDword( Length_Text_ImageDescription );

	Header0.APP1.IFD0_ImageDescriptionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_ImageDescriptionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD0_MakeTag.tag = 0x0f01;

	Header0.APP1.IFD0_MakeTag.type = 0x0200; //ASCII

	Header0.APP1.IFD0_MakeTag.count = M_ByteSwapOfDword( Length_Text_Make );

	Header0.APP1.IFD0_MakeTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_MakeValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD0_ModelTag.tag = 0x1001;

	Header0.APP1.IFD0_ModelTag.type = 0x0200;     //ASCII

	Header0.APP1.IFD0_ModelTag.count = M_ByteSwapOfDword( Length_Text_Model );

	Header0.APP1.IFD0_ModelTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_ModelValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD0_OrientationTag.tag = 0x1201;

	Header0.APP1.IFD0_OrientationTag.type = 0x0300;    //short

	Header0.APP1.IFD0_OrientationTag.count = 0x01000000;

	Header0.APP1.IFD0_OrientationTag.valueoffset = 0x01000000;   //value

	Header0.APP1.IFD0_XResolutionTag.tag = 0x1a01;

	Header0.APP1.IFD0_XResolutionTag.type = 0x0500;    //rational

	Header0.APP1.IFD0_XResolutionTag.count = 0x01000000;

	Header0.APP1.IFD0_XResolutionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_XResolutionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD0_YResolutionTag.tag = 0x1b01;

	Header0.APP1.IFD0_YResolutionTag.type = 0x0500;    //rational

	Header0.APP1.IFD0_YResolutionTag.count = 0x01000000;

	Header0.APP1.IFD0_YResolutionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_YResolutionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );


	Header0.APP1.IFD0_ResolutionUnitTag.tag = 0x2801;

	Header0.APP1.IFD0_ResolutionUnitTag.type = 0x0300; //short

	Header0.APP1.IFD0_ResolutionUnitTag.count = 0x01000000;

	Header0.APP1.IFD0_ResolutionUnitTag.valueoffset = 0x02000000;    //value

	Header0.APP1.IFD0_DateTimeTag.tag = 0x3201;

	Header0.APP1.IFD0_DateTimeTag.type = 0x0200;  //ASCII

	Header0.APP1.IFD0_DateTimeTag.count = 0x14000000;

	Header0.APP1.IFD0_DateTimeTag.valueoffset =    //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD0_DateTimeValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD0_YCbCrPositionTag.tag = 0x1302;

	Header0.APP1.IFD0_YCbCrPositionTag.type = 0x0300;  //short

	Header0.APP1.IFD0_YCbCrPositionTag.count = 0x01000000;

	Header0.APP1.IFD0_YCbCrPositionTag.valueoffset = 0x02000000;	//value

	Header0.APP1.IFD0_ExifIFDPointerTag.tag = 0x6987;

	Header0.APP1.IFD0_ExifIFDPointerTag.type = 0x0400; //long

	Header0.APP1.IFD0_ExifIFDPointerTag.count = 0x01000000;

	Header0.APP1.IFD0_ExifIFDPointerTag.valueoffset =     //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) & Header0.APP1.Exif_IFDNumber - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD0_NextIFDOffset =   //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) & Header0.APP1.IFD1_NumberOfInteroperability - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	EXIFHDR_Fill_Text( verCustom, Header0.APP1.IFD0_ImageDescriptionValue, Length_Text_ImageDescription );

	EXIFHDR_Fill_Text( Text_Make, Header0.APP1.IFD0_MakeValue, Length_Text_Make );

	EXIFHDR_Fill_Text( Text_Model, Header0.APP1.IFD0_ModelValue, Length_Text_Model );

	Header0.APP1.IFD0_XResolutionValue[ 0 ] = 0x48000000;     //72 dpi

	Header0.APP1.IFD0_XResolutionValue[ 1 ] = 0x01000000;

	Header0.APP1.IFD0_YResolutionValue[ 0 ] = 0x48000000;     //72 dpi

	Header0.APP1.IFD0_YResolutionValue[ 1 ] = 0x01000000;

	EXIFHDR_Fill_Text( Text_NowDateTime, Header0.APP1.IFD0_DateTimeValue, 20 );

	Header0.APP1.Exif_IFDNumber = 0x2400;

	Header0.APP1.Exif_ExposureTimeTag.tag = 0x9a82;

	Header0.APP1.Exif_ExposureTimeTag.type = 0x0500;   //rational

	Header0.APP1.Exif_ExposureTimeTag.count = 0x01000000;

	Header0.APP1.Exif_ExposureTimeTag.valueoffset = 	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_ExposureTimeValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_FNumberTag.tag = 0x9d82;

	Header0.APP1.Exif_FNumberTag.type = 0x0500;   //rational

	Header0.APP1.Exif_FNumberTag.count = 0x01000000;

	Header0.APP1.Exif_FNumberTag.valueoffset =     //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_FNumberValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_ExposureProgramTag.tag = 0x2288;

	Header0.APP1.Exif_ExposureProgramTag.type = 0x0300; 	//short

	Header0.APP1.Exif_ExposureProgramTag.count = 0x01000000;

	Header0.APP1.Exif_ExposureProgramTag.valueoffset = 0x02000000;  //value,normal program

	Header0.APP1.Exif_ISOSpeedRatingsTag.tag = 0x2788;

	Header0.APP1.Exif_ISOSpeedRatingsTag.type = 0x0300; 	//short

	Header0.APP1.Exif_ISOSpeedRatingsTag.count = 0x01000000;

	Header0.APP1.Exif_ISOSpeedRatingsTag.valueoffset = 0x64000000;  //value

	Header0.APP1.Exif_ExifVersionTag.tag = 0x0090;

	Header0.APP1.Exif_ExifVersionTag.type = 0x0700; 		 //undefined

	Header0.APP1.Exif_ExifVersionTag.count = 0x04000000;

	Header0.APP1.Exif_ExifVersionTag.valueoffset = 0x30323230;  //value."0220"

	Header0.APP1.Exif_DateTimeOriginalTag.tag = 0x0390;

	Header0.APP1.Exif_DateTimeOriginalTag.type = 0x0200;	//ASCII

	Header0.APP1.Exif_DateTimeOriginalTag.count = 0x14000000;

	Header0.APP1.Exif_DateTimeOriginalTag.valueoffset = 	 //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_DateTimeOriginalValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_DateTimeDigitilizedTag.tag = 0x0490;

	Header0.APP1.Exif_DateTimeDigitilizedTag.type = 0x0200;    //ASCII

	Header0.APP1.Exif_DateTimeDigitilizedTag.count = 0x14000000;

	Header0.APP1.Exif_DateTimeDigitilizedTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_DateTimeDigitilizedValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_ComponentsConfigurationTag.tag = 0x0191;

	Header0.APP1.Exif_ComponentsConfigurationTag.type = 0x0700; 	//undefined

	Header0.APP1.Exif_ComponentsConfigurationTag.count = 0x04000000;

	Header0.APP1.Exif_ComponentsConfigurationTag.valueoffset = 0x01020300;  //value."1230"

	Header0.APP1.Exif_CompressedBitsPerPixelTag.tag = 0x0291;

	Header0.APP1.Exif_CompressedBitsPerPixelTag.type = 0x0500;    //rational

	Header0.APP1.Exif_CompressedBitsPerPixelTag.count = 0x01000000;

	Header0.APP1.Exif_CompressedBitsPerPixelTag.valueoffset =      //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_CompressedBitsPerPixelValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_BrightnessValueTag.tag = 0x0392;	//BrightnessValue
	Header0.APP1.Exif_BrightnessValueTag.type = 0x0a00;    //srational
	Header0.APP1.Exif_BrightnessValueTag.count = 0x01000000;
	Header0.APP1.Exif_BrightnessValueTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_BrightnessValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_ExposureBiasTag.tag = 0x0492;

	Header0.APP1.Exif_ExposureBiasTag.type = 0x0a00;	//srational

	Header0.APP1.Exif_ExposureBiasTag.count = 0x01000000;

	Header0.APP1.Exif_ExposureBiasTag.valueoffset = 	 //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_ExposureBiasValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_MaxApertureTag.tag = 0x0592;

	Header0.APP1.Exif_MaxApertureTag.type = 0x0500;    //rational

	Header0.APP1.Exif_MaxApertureTag.count = 0x01000000;

	Header0.APP1.Exif_MaxApertureTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_MaxApertureValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_MeteringModeTag.tag = 0x0792;

	Header0.APP1.Exif_MeteringModeTag.type = 0x0300;	//short

	Header0.APP1.Exif_MeteringModeTag.count = 0x01000000;

	Header0.APP1.Exif_MeteringModeTag.valueoffset = 0x02000000;  //value,center-weighted average

	Header0.APP1.Exif_LightSourceTag.tag = 0x0892;

	Header0.APP1.Exif_LightSourceTag.type = 0x0300;    //short

	Header0.APP1.Exif_LightSourceTag.count = 0x01000000;

	Header0.APP1.Exif_LightSourceTag.valueoffset = 0x00000000;  //value,unknown

	Header0.APP1.Exif_FlashTag.tag = 0x0992;

	Header0.APP1.Exif_FlashTag.type = 0x0300;    //short

	Header0.APP1.Exif_FlashTag.count = 0x01000000;

	Header0.APP1.Exif_FlashTag.valueoffset = 0x00000000;  //value,flash did not fired

	Header0.APP1.Exif_FocalLengthTag.tag = 0x0a92;

	Header0.APP1.Exif_FocalLengthTag.type = 0x0500;    //rational

	Header0.APP1.Exif_FocalLengthTag.count = 0x01000000;

	Header0.APP1.Exif_FocalLengthTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_FocalLengthValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_SubjectAreaTag.tag = 0x1492;	//subject area
	Header0.APP1.Exif_SubjectAreaTag.type = 0x0300;    //short
	Header0.APP1.Exif_SubjectAreaTag.count = 0x04000000;
	Header0.APP1.Exif_SubjectAreaTag.valueoffset =  	//offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_SubjectArea - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_FlashPixVersiontTag.tag = 0x00a0;

	Header0.APP1.Exif_FlashPixVersiontTag.type = 0x0700;	//undefined

	Header0.APP1.Exif_FlashPixVersiontTag.count = 0x04000000;

	Header0.APP1.Exif_FlashPixVersiontTag.valueoffset = 0x30313030;   //value

	Header0.APP1.Exif_ColorSpaceTag.tag = 0x01a0;

	Header0.APP1.Exif_ColorSpaceTag.type = 0x0300;    //short

	Header0.APP1.Exif_ColorSpaceTag.count = 0x01000000;

	Header0.APP1.Exif_ColorSpaceTag.valueoffset = 0x01000000;   //value,sRGB

	Header0.APP1.Exif_PixelXDimensionTag.tag = 0x02a0;

	Header0.APP1.Exif_PixelXDimensionTag.type = 0x0400;    //long

	Header0.APP1.Exif_PixelXDimensionTag.count = 0x01000000;

	Header0.APP1.Exif_PixelXDimensionTag.valueoffset = 0x80020000;   //value,640

	Header0.APP1.Exif_PixelYDimensionTag.tag = 0x03a0;

	Header0.APP1.Exif_PixelYDimensionTag.type = 0x0400;    //long

	Header0.APP1.Exif_PixelYDimensionTag.count = 0x01000000;

	Header0.APP1.Exif_PixelYDimensionTag.valueoffset = 0xe001;   //value,480

	Header0.APP1.Exif_RelatedSoundFileTag.tag = 0x04a0;

	Header0.APP1.Exif_RelatedSoundFileTag.type = 0x0200;	//ASCII

	Header0.APP1.Exif_RelatedSoundFileTag.count = 0x0d000000;

	Header0.APP1.Exif_RelatedSoundFileTag.valueoffset = 	 //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_RelatedSoundFileValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_InteroperabilityIFDPointerTag.tag = 0x05a0;

	Header0.APP1.Exif_InteroperabilityIFDPointerTag.type = 0x0400;    //ASCII

	Header0.APP1.Exif_InteroperabilityIFDPointerTag.count = 0x01000000;

	Header0.APP1.Exif_InteroperabilityIFDPointerTag.valueoffset =    //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) & Header0.APP1.NumberOfInteroperability - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_FileSourceTag.tag = 0x00a3;

	Header0.APP1.Exif_FileSourceTag.type = 0x0700;    //undefined

	Header0.APP1.Exif_FileSourceTag.count = 0x01000000;

	Header0.APP1.Exif_FileSourceTag.valueoffset = 0x03000000;   //value,DSC

	Header0.APP1.Exif_SceneTypeTag.tag = 0x01a3;

	Header0.APP1.Exif_SceneTypeTag.type = 0x0700;    //undefined

	Header0.APP1.Exif_SceneTypeTag.count = 0x01000000;

	Header0.APP1.Exif_SceneTypeTag.valueoffset = 0x01000000;   //value,a directly photographed image

	Header0.APP1.Exif_CustomRenderedTag.tag = 0x01a4;	//CustomRendered
	Header0.APP1.Exif_CustomRenderedTag.type = 0x0300;    //short
	Header0.APP1.Exif_CustomRenderedTag.count = 0x01000000;
	Header0.APP1.Exif_CustomRenderedTag.valueoffset = 0x00000000;   //value, normal process

	Header0.APP1.Exif_ExposureModeTag.tag = 0x02a4;	//ExposureMode
	Header0.APP1.Exif_ExposureModeTag.type = 0x0300;	//short
	Header0.APP1.Exif_ExposureModeTag.count = 0x01000000;
	Header0.APP1.Exif_ExposureModeTag.valueoffset = 0x00000000;   //value, auto exposure

	Header0.APP1.Exif_WhiteBalanceTag.tag = 0x03a4;	//WhiteBalance
	Header0.APP1.Exif_WhiteBalanceTag.type = 0x0300;	//short
	Header0.APP1.Exif_WhiteBalanceTag.count = 0x01000000;
	Header0.APP1.Exif_WhiteBalanceTag.valueoffset = 0x00000000;   //value, auto whitebalance

	Header0.APP1.Exif_DigitalZoomRatioTag.tag = 0x04a4;	//DigitalZommRatio
	Header0.APP1.Exif_DigitalZoomRatioTag.type = 0x0500;	//rational
	Header0.APP1.Exif_DigitalZoomRatioTag.count = 0x01000000;
	Header0.APP1.Exif_DigitalZoomRatioTag.valueoffset = 	 //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.Exif_DigitalZoomRatio - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.Exif_FocalLengthIn35mmFileTag.tag = 0x05a4;	//FocalLengthIn35mmFilm
	Header0.APP1.Exif_FocalLengthIn35mmFileTag.type = 0x0300;    //short
	Header0.APP1.Exif_FocalLengthIn35mmFileTag.count = 0x01000000;
	Header0.APP1.Exif_FocalLengthIn35mmFileTag.valueoffset = 0x00000000;   //value, unknown focalength

	Header0.APP1.Exif_SceneCaptureTypeTag.tag = 0x06a4;	//SceneCaptureType
	Header0.APP1.Exif_SceneCaptureTypeTag.type = 0x0300;	//short
	Header0.APP1.Exif_SceneCaptureTypeTag.count = 0x01000000;
	Header0.APP1.Exif_SceneCaptureTypeTag.valueoffset = 0x00000000;   //value, standard

	Header0.APP1.Exif_GainControlTag.tag = 0x07a4;	//GainControl
	Header0.APP1.Exif_GainControlTag.type = 0x0300;    //short
	Header0.APP1.Exif_GainControlTag.count = 0x01000000;
	Header0.APP1.Exif_GainControlTag.valueoffset = 0x00000000;   //value, none

	Header0.APP1.Exif_ContrastTag.tag = 0x08a4;	//Contrast
	Header0.APP1.Exif_ContrastTag.type = 0x0300;	//short
	Header0.APP1.Exif_ContrastTag.count = 0x01000000;
	Header0.APP1.Exif_ContrastTag.valueoffset = 0x00000000;   //value, normal

	Header0.APP1.Exif_SaturationTag.tag = 0x09a4;	//Saturation
	Header0.APP1.Exif_SaturationTag.type = 0x0300;    //short
	Header0.APP1.Exif_SaturationTag.count = 0x01000000;
	Header0.APP1.Exif_SaturationTag.valueoffset = 0x00000000;   //value, normal

	Header0.APP1.Exif_SharpnessTag.tag = 0x0aa4;	//Sharpness
	Header0.APP1.Exif_SharpnessTag.type = 0x0300;    //short
	Header0.APP1.Exif_SharpnessTag.count = 0x01000000;
	Header0.APP1.Exif_SharpnessTag.valueoffset = 0x00000000;   //value, normal

	Header0.APP1.Exif_SubjectDistanceRangeTag.tag = 0x0ca4;	//SubjectDistanceRange
	Header0.APP1.Exif_SubjectDistanceRangeTag.type = 0x0300;	//short
	Header0.APP1.Exif_SubjectDistanceRangeTag.count = 0x01000000;
	Header0.APP1.Exif_SubjectDistanceRangeTag.valueoffset = 0x00000000;   //value, unknown

	Header0.APP1.Exif_NextIFDOffset = 0x00000000;

	Header0.APP1.Exif_ExposureTimeValue[ 0 ] = 0x01000000; //1/35

	Header0.APP1.Exif_ExposureTimeValue[ 1 ] = 0x23000000;

	Header0.APP1.Exif_FNumberValue[ 0 ] = 0x1c000000; //28/10

	Header0.APP1.Exif_FNumberValue[ 1 ] = 0x0a000000;

	EXIFHDR_Fill_Text( Text_NowDateTime, Header0.APP1.Exif_DateTimeOriginalValue, 20 );

	EXIFHDR_Fill_Text( Text_NowDateTime, Header0.APP1.Exif_DateTimeDigitilizedValue, 20 );

	Header0.APP1.Exif_CompressedBitsPerPixelValue[ 0 ] = 0x02000000; //2/1

	Header0.APP1.Exif_CompressedBitsPerPixelValue[ 1 ] = 0x01000000;

	Header0.APP1.Exif_BrightnessValue[ 0 ] = 0x00000000;
	Header0.APP1.Exif_BrightnessValue[ 1 ] = 0xffffffff;

	Header0.APP1.Exif_ExposureBiasValue[ 0 ] = 0x00000000;

	Header0.APP1.Exif_ExposureBiasValue[ 1 ] = 0x0a000000;

	Header0.APP1.Exif_MaxApertureValue[ 0 ] = 0x02000000;

	Header0.APP1.Exif_MaxApertureValue[ 1 ] = 0x01000000;

	Header0.APP1.Exif_FocalLengthValue[ 0 ] = 0x51000000;

	Header0.APP1.Exif_FocalLengthValue[ 1 ] = 0x0a000000;

	EXIFHDR_Fill_Text( Text_RelatedSoundFile, Header0.APP1.Exif_RelatedSoundFileValue, Length_Text_RelatedSoundFile );

	Header0.APP1.Exif_SubjectArea[ 0 ] = 0x0000;
	Header0.APP1.Exif_SubjectArea[ 1 ] = 0x0000;
	Header0.APP1.Exif_SubjectArea[ 2 ] = 0x0000;
	Header0.APP1.Exif_SubjectArea[ 3 ] = 0x0000;

	Header0.APP1.NumberOfInteroperability = 0x0200;

	Header0.APP1.InteroperabilityIndexTag.tag = 0x0100;

	Header0.APP1.InteroperabilityIndexTag.type = 0x0200;

	Header0.APP1.InteroperabilityIndexTag.count = 0x04000000;

	Header0.APP1.InteroperabilityIndexTag.valueoffset = 0x52393800;   //"R98"

	Header0.APP1.InteroperabilityVersionTag.tag = 0x0200;

	Header0.APP1.InteroperabilityVersionTag.type = 0x0700;

	Header0.APP1.InteroperabilityVersionTag.count = 0x04000000;

	Header0.APP1.InteroperabilityVersionTag.valueoffset = 0x30313030;

	Header0.APP1.NextInteroperabilityIFDOffset = 0x00000000;

	Header0.APP1.Exif_DigitalZoomRatio[ 0 ] = 0x00000000;
	Header0.APP1.Exif_DigitalZoomRatio[ 1 ] = 0x01000000;

	Header0.APP1.IFD1_NumberOfInteroperability = 0x0800;

	Header0.APP1.IFD1_CompressionTag.tag = 0x0301;

	Header0.APP1.IFD1_CompressionTag.type = 0x0300; 	//short

	Header0.APP1.IFD1_CompressionTag.count = 0x01000000;

	Header0.APP1.IFD1_CompressionTag.valueoffset = 0x06000000;   //value,thumbnail is JPEG compressed

	Header0.APP1.IFD1_OrientationTag.tag = 0x1201;

	Header0.APP1.IFD1_OrientationTag.type = 0x0300;    //short

	Header0.APP1.IFD1_OrientationTag.count = 0x01000000;

	Header0.APP1.IFD1_OrientationTag.valueoffset = 0x01000000;   //value

	Header0.APP1.IFD1_XResolutionTag.tag = 0x1a01;

	Header0.APP1.IFD1_XResolutionTag.type = 0x0500;    //rational

	Header0.APP1.IFD1_XResolutionTag.count = 0x01000000;

	Header0.APP1.IFD1_XResolutionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD1_XResolutionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD1_YResolutionTag.tag = 0x1b01;

	Header0.APP1.IFD1_YResolutionTag.type = 0x0500;    //rational

	Header0.APP1.IFD1_YResolutionTag.count = 0x01000000;

	Header0.APP1.IFD1_YResolutionTag.valueoffset =  //offset
	M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.APP1.IFD1_YResolutionValue - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD1_ResolutionUnitTag.tag = 0x2801;

	Header0.APP1.IFD1_ResolutionUnitTag.type = 0x0300; //short

	Header0.APP1.IFD1_ResolutionUnitTag.count = 0x01000000;

	Header0.APP1.IFD1_ResolutionUnitTag.valueoffset = 0x02000000;    //value

	Header0.APP1.IFD1_JPEGInterChangeFormatTag.tag = 0x0102;

	Header0.APP1.IFD1_JPEGInterChangeFormatTag.type = 0x0400; //long

	Header0.APP1.IFD1_JPEGInterChangeFormatTag.count = 0x01000000;

	Header0.APP1.IFD1_JPEGInterChangeFormatTag.valueoffset = M_ByteSwapOfDword( ( UINT32 ) ( ( UINT16 ) Header0.THUMB_SOI.Marker - ( UINT16 ) & Header0.APP1.Tiff_ByteOrder ) );

	Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.tag = 0x0202;

	Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.type = 0x0400; //long

	Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.count = 0x01000000;

	Header0.APP1.IFD1_JPEGInterChangeFormatLengthTag.valueoffset = M_ByteSwapOfDword( K_THUMBNAIL_STRUCTURE_SIZE );    //value

	Header0.APP1.IFD1_YCbCrPositioningTag.tag = 0x1302;

	Header0.APP1.IFD1_YCbCrPositioningTag.type = 0x0300;  //short

	Header0.APP1.IFD1_YCbCrPositioningTag.count = 0x01000000;

	Header0.APP1.IFD1_YCbCrPositioningTag.valueoffset = 0x02000000;    //value

	Header0.APP1.IFD1_NextIFDOffset = 0x00000000;

	Header0.APP1.IFD1_XResolutionValue[ 0 ] = 0x48000000;     //72 dpi

	Header0.APP1.IFD1_XResolutionValue[ 1 ] = 0x01000000;

	Header0.APP1.IFD1_YResolutionValue[ 0 ] = 0x48000000;     //72 dpi

	Header0.APP1.IFD1_YResolutionValue[ 1 ] = 0x01000000;

	Header0.THUMB_SOI.Marker[ 0 ] = 0xff;
	Header0.THUMB_SOI.Marker[ 1 ] = 0xd8;

	EXIFHDR_DQT( exifThbQTableIdx, &Header0.THUMB_DQT );

	EXIFHDR_DHT( &Header0.THUMB_DHT );

	EXIFHDR_SOF( &Header0.THUMB_SOF, exifThbXSize, exifThbYSize );

	EXIFHDR_SOS( &Header0.THUMB_SOS );

	EXIFHDR_Load_ExifHeader0_SRAM2DRAM( &Header0.SOI, pDramHdrAddr );
}
//patch5.1@ada@Audio annotation end
#endif

//-----------------------------------------------------------------------------
//EXIFHDR_Initialize_ExifHeader2
//-----------------------------------------------------------------------------
void EXIFHDR_Initialize_ExifHeader2( UINT32* pDramHdrAddr ) USING_0
{
	struct Exif_Header2 Header1;

	//----- Header 1 -----
	//EXIF COM
	EXIFHDR_ThumbEven_COM( &Header1 );

	EXIFHDR_Load_ExifHeader2_SRAM2DRAM( &Header1.EVEN_COM, pDramHdrAddr );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Initialize_ExifHeader3
//-----------------------------------------------------------------------------
void EXIFHDR_Initialize_ExifHeader3( UINT32* pDramHdrAddr ) USING_0
{
	struct Exif_Header3 Header2;

	//----- Header 2 -----
	//EXIF COM
	EXIFHDR_ThumbOdd_COM( &Header2 );

	EXIFHDR_Load_ExifHeader3_SRAM2DRAM( &Header2.ODD_COM, pDramHdrAddr );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Initialize_ExifHeader4
//-----------------------------------------------------------------------------
void EXIFHDR_Initialize_ExifHeader4( UINT32* pDramHdrAddr ) USING_0
{
	struct Exif_Header4 Header3;

	//----- Header3 -----

	//EXIF DQT
	EXIFHDR_DQT( exifMainQTableIdx, &Header3.DQT );

	//EXIF DHT
	EXIFHDR_DHT( &Header3.DHT );

#if 0
	 EXIFHDR_SOF(&Header3.SOF,width,height);
#else
	 EXIFHDR_SOF(&Header3.SOF, exifMainXSize, exifMainYSize);
#endif

	//EXIF SOS
	EXIFHDR_SOS( &Header3.SOS );

	//----Copy all header to DRAM(from K_SDRAM_EXIF_AreaAddr,23K bytes space)
	EXIFHDR_Load_ExifHeader4_SRAM2DRAM( &Header3.DQT, pDramHdrAddr );
}

//-----------------------------------------------------------------------------
//EXIFHDR_ThumbEven_COM
//-----------------------------------------------------------------------------
void EXIFHDR_ThumbEven_COM( struct Exif_Header2* hdr1 ) USING_0
{
	hdr1->EVEN_COM.Marker[ 0 ] = 0xff;
	hdr1->EVEN_COM.Marker[ 1 ] = 0xfe;

	hdr1->EVEN_COM.Length = ( 0x0002 + ( UINT16 ) ( Length_Text_ThumbOdd_Comment & 0x0000ffff ) );

	if(g_jpgCommentMarkContect[0])
		EXIFHDR_Fill_Text( g_jpgCommentMarkContect, hdr1->EVEN_COM.Content, Length_Text_ThumbOdd_Comment );
	else
		EXIFHDR_Fill_Text( Text_Comment, hdr1->EVEN_COM.Content, Length_Text_ThumbOdd_Comment );
}

//-----------------------------------------------------------------------------
//EXIFHDR_ThumbOdd_COM
//-----------------------------------------------------------------------------
void EXIFHDR_ThumbOdd_COM( struct Exif_Header3* hdr2 ) USING_0
{
	hdr2->ODD_COM.Marker[ 0 ] = 0xff;
	hdr2->ODD_COM.Marker[ 1 ] = 0xfe;

	hdr2->ODD_COM.Length = ( 0x0002 + ( UINT16 ) ( Length_Text_ThumbEven_Comment & 0x0000ffff ) );

	if(g_jpgCommentMarkContect[0])
		EXIFHDR_Fill_Text( g_jpgCommentMarkContect, hdr2->ODD_COM.Content, Length_Text_ThumbEven_Comment );
	else	
		EXIFHDR_Fill_Text( Text_Comment, hdr2->ODD_COM.Content, Length_Text_ThumbEven_Comment );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Fill_Text
//-----------------------------------------------------------------------------
void EXIFHDR_Fill_Text( UINT8* Srctable, UINT8* Desttable, UINT32 i ) USING_0
{
	UINT8 j;

	for( j = 0; j < i; j++ ) {
		*( Desttable + j ) = *( Srctable + j );
	}
}

//-----------------------------------------------------------------------------
//EXIFHDR_DQT
//-----------------------------------------------------------------------------
static void EXIFHDR_DQT( UINT8 qindex, struct HDR4* table ) USING_0
{
	//     UINT8 *pQTable;

	qindex = qindex;

	table->Marker[ 0 ] = 0xff;
	table->Marker[ 1 ] = 0xdb;

	table->Length = ( UINT16 ) ( ( K_HDR4_SIZE - 2 ) & 0x0000ffff );

	table->PqTq0 = 0x00;

	table->PqTq1 = 0x01;

#if 0 // exif load will fill Q table
	jpegQTableGen( qindex, &table->Content0[ 0 ], 1 );
	jpegQTableGen( qindex, &table->Content1[ 0 ], 2 );
#endif

#if 0
		pQTable = File_GetQTable(qindex);

		File_JpgConvertToZizag(pQTable,&table->Content0[0]);
	 File_JpgConvertToZizag((pQTable+64),&table->Content1[0]);
#endif
}

//-----------------------------------------------------------------------------
//EXIFHDR_DHT
//-----------------------------------------------------------------------------
void EXIFHDR_DHT( struct HDR5* table ) USING_0
{
	UINT8 i;

	table->Marker[ 0 ] = 0xff;
	table->Marker[ 1 ] = 0xc4;

	table->Length = ( UINT16 ) ( ( K_HDR5_SIZE - 2 ) & 0x0000ffff );

	table->TcTh0 = 0x00;   //YDC
	table->TcTh1 = 0x10;   //YAC
	table->TcTh2 = 0x01;   //UVDC
	table->TcTh3 = 0x11;   //UVAC

	for( i = 0; i < 28; i++ ) {
		table->Content0[ i ] = DHT_YDC[ i ];
		table->Content2[ i ] = DHT_UVDC[ i ];
	}

	for( i = 0; i < 178; i++ ) {
		table->Content1[ i ] = DHT_YAC[ i ];
		table->Content3[ i ] = DHT_UVAC[ i ];
	}
}

//-----------------------------------------------------------------------------
//EXIFHDR_SOF
//-----------------------------------------------------------------------------
void EXIFHDR_SOF( struct HDR3* softable, UINT16 width, UINT16 height ) USING_0
{
	UINT8 i;

	softable->Marker[ 0 ] = 0xff;
	softable->Marker[ 1 ] = 0xc0;

	softable->Length = 0x0011;

	for( i = 0; i < 15; i++ ) {
		softable->Content[ i ] = SOFT[ i ];
	}

	if( exifFileFormat == K_File_DataType_YUV420 ) {
		softable->Content[ 7 ] = 0x22;
	}
	else {
		softable->Content[ 7 ] = 0x21;
	}

	softable->Content[ 1 ] = ( UINT8 ) ( ( height & 0xff00 ) >> 8 );
	softable->Content[ 2 ] = ( UINT8 ) ( height & 0x00ff );
	softable->Content[ 3 ] = ( UINT8 ) ( ( width & 0xff00 ) >> 8 );
	softable->Content[ 4 ] = ( UINT8 ) ( width & 0x00ff );
}

//-----------------------------------------------------------------------------
//EXIFHDR_SOS
//-----------------------------------------------------------------------------
void EXIFHDR_SOS( struct HDR6* sostable ) USING_0
{
	UINT8 i;

	sostable->Marker[ 0 ] = 0xff;
	sostable->Marker[ 1 ] = 0xda;

	sostable->Length = 0x000c;

	for( i = 0; i < 10; i++ ) {
		sostable->Content[ i ] = SOST[ i ];
	}
}

//-----------------------------------------------------------------------------
//exifParsingFile
//-----------------------------------------------------------------------------
UINT8 exifParsingFile( UINT32 DramAddr, UINT32 srcSize, BIT ShowThumb, UINT8* DateTime, struct Exif_Decompress_Parameter* ImagePara, struct Exif_Decompress_Parameter* ThumbPara ) USING_0
{
	UINT8 sts = TRUE;

	//patch5.1@ada@Fix EXIF header parsing bug begin
	UINT16 tmplength;
	UINT8 Marker_Find = 0;
	//patch5.1@ada@Fix EXIF header parsing bug end

	Extract_Finished = 0;
	Extract_Thumbnail = 0;
	ByteOrder = 0;
	ByteOrderOffset = 0;
	IFD_ThumbnaiFrameOffset = 0;
	IFD_ThumbnaiFrameLength = 0;
	IFD_Number = 0;
	Tiff_ID = 0;
	Tiff_IFDOffset = 0;
	NextIFDOffset = 0;
	IFD_DateTime_Offset = 0;
	Jpeg_Exif_Format = 0;
	HeaderLength = 0;
	ExifTmpDramAddr = 0;
	Parsing_Marker = 0;
	APP0Length = 0;
	APP1Length = 0;
	APPnLength = 0;
	ThumbAPPnLength = 0;
	TotalThumbAPPnLength = 0;
	TotalAPPnLength = 0;
	COMLength = 0;
	DQTLength = 0;
	DHTLength = 0;
	SOFLength = 0;
	SOSLength = 0;
	DRILength = 0;
	G_JpgRstMcuNum = 0;

	exifAppN = 0;

	exifParsingSize = srcSize;

    if(!exifFillRotOpt)
        exifRotOpt = 1;
        
	while( ( !Extract_Finished ) && ( sts == TRUE ) ) {
		ExifTmpDramAddr = ( DramAddr + ( HeaderLength >> 1 ) );
    
        if(HeaderLength+8 > exifParsingSize){//xian ++ 20090923
        	sts = FALSE;
        	break;    
        }

		if( HeaderLength & 0x00000001 ) {
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 8 );

			//patch5.1@ada@Fix EXIF header parsing bug begin
			//Parsing_Marker = G_ucStorData[2];
			if( G_ucStorData[ 1 ] == 0xff ) {
				Parsing_Marker = G_ucStorData[ 2 ];
			}
			else {
				sts = FALSE;
				break;
			}
			//patch5.1@ada@Fix EXIF header parsing bug end

		}
		else {
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 8 );

			//patch5.1@ada@Fix EXIF header parsing bug begin
			//Parsing_Marker = G_ucStorData[1];
			if( G_ucStorData[ 0 ] == 0xff ) {
				Parsing_Marker = G_ucStorData[ 1 ];
			}
			else {
				sts = FALSE;
				break;
			}
			//patch5.1@ada@Fix EXIF header parsing bug end

		}
#if (ExifDbgLevel>1)
	printf("Len: %lx, Mark: %bx\n",HeaderLength, Parsing_Marker);
#endif
		switch( Parsing_Marker ) {
		case K_SOI_MARKER:
			//Check SOI
#if (ExifDbgLevel>1)
	printf("SOI is found!\n");
#endif
			sts = EXIFHDR_Parsing_SOI();
#if (ExifDbgLevel>1)
	printf("sts: %bu\n",sts);
#endif
			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_APP0_MARKER:
			//Check APP0
#if (ExifDbgLevel>1)
	printf("APP0 is found!\n");
#endif
			sts = EXIFHDR_Parsing_APP0( DramAddr, ShowThumb );
#if (ExifDbgLevel>1)
	printf("sts: %bu\n",sts);
#endif
			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_APP1_MARKER:
			//Check APP1
#if (ExifDbgLevel>0)
	printf("APP1 is found!\n");
#endif
			//patch5.1@ada@Fix EXIF header parsing bug begin
			if( Jpeg_Exif_Format == K_EXIF_IMAGE_FORMAT ) {
				tmplength = 2 + ( ( ( ( UINT16 ) G_ucStorData[ 2 + ( UINT8 ) ( HeaderLength & 0x00000001 ) ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + ( UINT8 ) ( HeaderLength & 0x00000001 ) ] ) );
				HeaderLength += ( UINT32 ) tmplength;
				sts = TRUE;
			}
			else {
				sts = EXIFHDR_Parsing_APP1( DramAddr, ShowThumb, DateTime );
			}
#if (ExifDbgLevel>0)
	printf("sts: %bu\n",sts);
#endif
			//sts = EXIFHDR_Parsing_APP1(DramAddr,ShowThumb,DateTime);

			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_APP5_MARKER:
#if (ExifDbgLevel>0)
	printf("APP5 is found!\n");
#endif
			sts = EXIFHDR_Parsing_APP5( DramAddr );
#if (ExifDbgLevel>0)
	printf("sts: %bu\n",sts);
#endif
			Marker_Find = 0;
            break;
            
		case K_APP2_MARKER:
			//Check APPn
		case K_APP3_MARKER:
		case K_APP4_MARKER:
		//case K_APP5_MARKER:
		case K_APP6_MARKER:
		case K_APP7_MARKER:
		case K_APP8_MARKER:
		case K_APP9_MARKER:
		case K_APP10_MARKER:
		case K_APP11_MARKER:
		case K_APP12_MARKER:
		case K_APP13_MARKER:
		case K_APP14_MARKER:
		case K_APP15_MARKER:
#if (ExifDbgLevel>0)
	printf("APPn is found!\n");
#endif
			sts = EXIFHDR_Parsing_APPn( DramAddr );
#if (ExifDbgLevel>0)
	printf("sts: %bu\n",sts);
#endif
			//exifAppN = 1;

			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_DQT_MARKER:
			//Check DQT
#if (ExifDbgLevel>0)
	printf("DQT is found!\n");
#endif
			//version4.0@ada@0513 for EXIF
			//sts = EXIFHDR_Parsing_DQT(ShowThumb);
			sts = EXIFHDR_Parsing_DQT();
#if (ExifDbgLevel>0)
	printf("sts: %bu\n",sts);
#endif
			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_DHT_MARKER:
			//Check DHT
#if (ExifDbgLevel>0)
	printf("DHT is found!\n");
#endif
			sts = EXIFHDR_Parsing_DHT();
#if (ExifDbgLevel>0)
	printf("sts: %bu\n",sts);
#endif			
			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_SOF_MARKER:
			//Check SOF
#if (ExifDbgLevel>0)
	printf("SOF is found!\n");
#endif
			sts = EXIFHDR_Parsing_SOF( ThumbPara, ImagePara );
#if (ExifDbgLevel>0)
	printf("sts: %bu\n",sts);
#endif			
			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_SOS_MARKER:
			//Check SOS
#if (ExifDbgLevel>0)
	printf("SOS is found!\n");
#endif			
			//patch4.5@ada@Fix EXIF bug begin
			sts = EXIFHDR_Parsing_SOS( DramAddr, ShowThumb, ThumbPara, ImagePara );
#if (ExifDbgLevel>0)
	printf("sts: %bu\n",sts);
#endif			
			//sts = EXIFHDR_Parsing_SOS(DramAddr,ThumbPara,ImagePara);
			//patch4.5@ada@Fix EXIF bug end

			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_EOI_MARKER:
			//Check EOI
#if (ExifDbgLevel>2)
	printf("EOI is found!\n");
#endif	
			sts = EXIFHDR_Parsing_EOI( DramAddr, ShowThumb );
#if (ExifDbgLevel>2)
	printf("sts: %bu\n",sts);
#endif				
			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_COM_MARKER:
			//Check COM
#if (ExifDbgLevel>1)
	printf("COM is found!\n");
#endif	
			sts = EXIFHDR_Parsing_COM();
#if (ExifDbgLevel>1)
	printf("sts: %bu\n",sts);
#endif				
			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		case K_DRI_MARKER:
			//Check DRI
#if (ExifDbgLevel>1)
	printf("DRI is found!\n");
#endif	
			sts = EXIFHDR_Parsing_DRI();
#if (ExifDbgLevel>1)
	printf("sts: %bu\n",sts);
#endif				
			//patch5.1@ada@Fix EXIF header parsing bug begin
			Marker_Find = 0;
			//patch5.1@ada@Fix EXIF header parsing bug end
			break;

		default:
			//Other marker
			//patch5.1@ada@Fix EXIF header parsing bug begin
			//Jpeg_Exif_Format = 0;
			//Extract_Finished = 1;
			//sts = FALSE;
#if (ExifDbgLevel>1)
	printf("Unknown marker field is found!\n");
#endif	
			if( Parsing_Marker == 0xff ) {
					HeaderLength++;
			}
			else {
				Jpeg_Exif_Format = 0;
				Extract_Finished = 1;
				sts = FALSE;
			}
			//patch5.1@ada@Fix EXIF header parsing bug begin
			break;
		}
	}

	if(sts == FALSE){//xian ++ 20110513, for #41585
		printf("exif err: 0x%lx, Mark: 0x%bx\n",HeaderLength, Parsing_Marker);
		printf("Addr: %lx, %lx\n", DramAddr, ExifTmpDramAddr);
		for(tmplength = 0; tmplength < 16; tmplength++){
				printf("%bx ", G_ucStorData[tmplength]);
		}
		printf("\n");

		#if 0
		dbgWdtFeed(100000);
		dumpData(srcSize, DramAddr, 1);
		dbgWdtFeed(10000);
		#endif
	}
	
	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_SOI
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_SOI( void ) USING_0
{
	UINT8 sts = TRUE;

	if( Jpeg_Exif_Format == K_EXIF_IMAGE_FORMAT )     //EXIF file format
	{
		//richie@si0419
		////printf("HeaderLength=%lx\n",HeaderLength);
		G_ulExifThumbAddr = HeaderLength;

		Extract_Thumbnail++;
	}
	else {
		Extract_Thumbnail = 0;
	}

	HeaderLength += 2;

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_APP0
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_APP0( UINT32 DramAddr, BIT ShowThumb ) USING_0
{
	UINT8 sts = TRUE;

	SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 10 );

	sts = EXIFHDR_Parsing_APP0_Sub( DramAddr, ShowThumb, ( UINT16 ) ( HeaderLength & 0x00000001 ) );

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_APP0_Sub
//-----------------------------------------------------------------------------
UINT8 EXIFHDR_Parsing_APP0_Sub( UINT32 DramAddr, BIT ShowThumb, UINT16 EvenOddOffset ) USING_0
{
	UINT8 sts = TRUE;

	if( Jpeg_Exif_Format == 0 ) {
		//Check "JFIF"
		if( ( G_ucStorData[ 4 + EvenOddOffset ] == 0x4a ) && ( G_ucStorData[ 5 + EvenOddOffset ] == 0x46 ) && ( G_ucStorData[ 6 + EvenOddOffset ] == 0x49 ) && ( G_ucStorData[ 7 + EvenOddOffset ] == 0x46 ) ) {
			Jpeg_Exif_Format = K_JFIF_IMAGE_FORMAT;   //JFIF file

			//printf("This file is JFIF format!\n");

			if( ShowThumb ) {
				Extract_Finished = 1;
				sts = FALSE;
			}
			else {
				APP0Length = ( ( ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + EvenOddOffset ] ) );
				HeaderLength += ( 2 + ( UINT32 ) APP0Length );
				ExifTmpDramAddr = DramAddr + ( HeaderLength >> 1 );
			}
		}
		else {
			Extract_Finished = 1;
			sts = FALSE;
			//printf("This is not an JPEG file!\n");
		}
	}
	else//if EXIF header contains APP0
	{
		ThumbAPPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + EvenOddOffset ] ) );
		TotalThumbAPPnLength += ( 2 + ThumbAPPnLength );
		HeaderLength += ( 2 + ( UINT32 ) ThumbAPPnLength );
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_APP1
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_APP1( UINT32 DramAddr, BIT ShowThumb, UINT8* DateTime ) USING_0
{
	UINT8 sts = TRUE;

	//patch4.5.1@ada@Fix EXIF bugs begin
	sts = EXIFHDR_Parsing_APP1_Sub1( DramAddr, ShowThumb, DateTime, ( UINT8 ) ( HeaderLength & 0x00000001 ) );
	//sts = EXIFHDR_Passing_APP1_Sub1(DramAddr,ShowThumb,DateTime,(UINT16)(HeaderLength & 0x00000001));
	//patch4.5.1@ada@Fix EXIF bugs end

	if( sts ) {
		//patch4.5.2@ada@Fix EXIF bug begin
		if( Jpeg_Exif_Format == K_EXIF_IMAGE_FORMAT ) {
			HeaderLength = IFD_ThumbnaiFrameOffset + ByteOrderOffset;
		}
		//patch4.5.2@ada@Fix EXIF bug end
	}
	else {
		Jpeg_Exif_Format = 0;
		Extract_Finished = 1;
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_APP1_Sub1
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_APP1_Sub1( UINT32 DramAddr, BIT ShowThumb, UINT8* DateTime, UINT8 EvenOddOffset ) USING_0
{
	UINT8 sts = TRUE;
	UINT16 i;

	//patch5.1@ada@Fix EXIF IFD Offset bug begin
	UINT32 IFD_Offset_Difference = 0;
	//patch5.1@ada@Fix EXIF IFD Offset bug end

	if( Jpeg_Exif_Format == 0 ) {
		if( exifFillRotOpt == 0 ) {
			SDRAM_Dram2Sram( DramAddr + ( HeaderLength >> 1 ) + ( 64 >> 1 ), G_ucDataBuffer, 4 );

			*( (UINT8 *)(&exifRotOpt) + 0 ) = G_ucStorData[ 3 ];
			*( (UINT8 *)(&exifRotOpt) + 1 ) = G_ucStorData[ 2 ];
			*( (UINT8 *)(&exifRotOpt) + 2 ) = G_ucStorData[ 1 ];
			*( (UINT8 *)(&exifRotOpt) + 3 ) = G_ucStorData[ 0 ];
		}
		else {
			G_ucStorData[ 3 ] = *( (UINT8 *)(&exifRotOpt) + 0 );
			G_ucStorData[ 2 ] = *( (UINT8 *)(&exifRotOpt) + 1 );
			G_ucStorData[ 1 ] = *( (UINT8 *)(&exifRotOpt) + 2 );
			G_ucStorData[ 0 ] = *( (UINT8 *)(&exifRotOpt) + 3 );

			M_DRAM_SetStartAddress( DramAddr + ( HeaderLength >> 1 ) + ( 64 >> 1 ), 0 );
			M_DRAM_WriteWord( G_ucStorData[ 0 ], G_ucStorData[ 1 ] );
			M_DRAM_WriteWord( G_ucStorData[ 2 ], G_ucStorData[ 3 ] );

			SDRAM_Dram2Sram( DramAddr + ( HeaderLength >> 1 ) + ( 802 >> 1 ), G_ucDataBuffer, 6 );

			G_ucStorData[ 5 ] = G_ucStorData[ 5 ];
			G_ucStorData[ 4 ] = *( (UINT8 *)(&exifRotOpt) + 0 );
			G_ucStorData[ 3 ] = *( (UINT8 *)(&exifRotOpt) + 1 );
			G_ucStorData[ 2 ] = *( (UINT8 *)(&exifRotOpt) + 2 );
			G_ucStorData[ 1 ] = *( (UINT8 *)(&exifRotOpt) + 3 );
			G_ucStorData[ 0 ] = G_ucStorData[ 0 ];

			M_DRAM_SetStartAddress( DramAddr + ( HeaderLength >> 1 ) + ( 802 >> 1 ), 0 );

			M_DRAM_WriteWord( G_ucStorData[ 0 ], G_ucStorData[ 1 ]  );
			M_DRAM_WriteWord( G_ucStorData[ 2 ], G_ucStorData[ 3 ]  );
			M_DRAM_WriteWord( G_ucStorData[ 4 ], G_ucStorData[ 5 ]  );
		}

		SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 22 );

		for( i = ( 4 + EvenOddOffset ); i < ( 9 + EvenOddOffset ); i++ )  //Check "Exif"
		{
			if( G_ucStorData[ i ] != Text_Exif[ i - ( 4 + EvenOddOffset ) ] ) {
				Extract_Finished = 1;
				sts = FALSE;
				//printf("This is not an EXIF file");
				break;
			}
		}//End for loop

		if( ( sts == FALSE ) || ( G_ucStorData[ 9 + EvenOddOffset ] != 0x00 ) ) {
			sts = FALSE;
		}

		if( sts ) {
			APP1Length = ( ( ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + EvenOddOffset ] ) );

			if( ( ShowThumb ) && ( NeedReadMoreHeader == 0 ) && ( ( 2 + APP1Length + 2 ) >= exifParsingSize ) ) {
				NeedReadMoreHeader = 1;
				sts = FALSE;
			}
			else {
				ByteOrder = ( ( ( ( UINT16 ) G_ucStorData[ 10 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 11 + EvenOddOffset ] ) );	//0x4949 or 0x4d4d
				ByteOrderOffset = HeaderLength + ( UINT32 ) 10;

				if( ByteOrder == K_LITTLE_ENDIAN ) {
					//printf("It's little-endian file!\n");
					sts = TRUE;
				}
				else {
					if( ByteOrder == K_BIG_ENDIAN ) {
						//printf("It's big-endian file!\n");
						sts = TRUE;
					}
					else {
						//printf("Not either little-endian or big-endian file\n");
						sts = FALSE;
					}
				}

				if( sts ) {
					//Check Tiff_ID
					Tiff_ID = EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 12 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 13 + EvenOddOffset ] ) ) );

					if( Tiff_ID == 0x002a ) {
						//Check Tiff_IFDOffset
						Tiff_IFDOffset = EXIFHDR_NeedSwitchULONGOrder( ByteOrder, ( ( ( ( UINT32 ) G_ucStorData[ 14 + EvenOddOffset ] ) << 24 ) | ( ( ( UINT32 ) G_ucStorData[ 15 + EvenOddOffset ] ) << 16 ) | ( ( ( UINT32 ) G_ucStorData[ 16 + EvenOddOffset ] ) << 8 ) | ( ( UINT32 ) G_ucStorData[ 17 + EvenOddOffset ] ) ) );

						//patch5.1@ada@Fix EXIF IFD Offset bug begin
						IFD_Offset_Difference = Tiff_IFDOffset - 0x00000008;
						if (IFD_Offset_Difference>0){// tq, for mantis bug 39161
							SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, (UINT16)IFD_Offset_Difference+22);
						}

						//if (Tiff_IFDOffset == 0x00000008)
						if( ( Tiff_IFDOffset & 0x00000001 ) == 0 )
							//patch5.1@ada@Fix EXIF IFD Offset bug end
						{
							Jpeg_Exif_Format = K_EXIF_IMAGE_FORMAT;    //EXIF file
							Extract_Thumbnail = 1;
							//printf("This file is EXIF format!\n");

							//Extract IFD0

							//patch5.1@ada@Fix EXIF IFD Offset bug begin
							//IFD_Number = EXIFHDR_NeedSwitchUSHORTOrder(ByteOrder,((((UINT16)G_ucStorData[18 + EvenOddOffset]) << 8) | ((UINT16)G_ucStorData[19 + EvenOddOffset])));
							IFD_Number = EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 18 + ( UINT8 ) ( IFD_Offset_Difference ) + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 19 + ( UINT8 ) ( IFD_Offset_Difference ) + EvenOddOffset ] ) ) );
							//patch5.1@ada@Fix EXIF IFD Offset bug end

							//patch5.1@ada@Fix EXIF IFD Offset bug begin
							//HeaderLength += (UINT32)20;
							HeaderLength += ( ( ( UINT32 ) 20 ) + IFD_Offset_Difference );
							//patch5.1@ada@Fix EXIF IFD Offset bug end

							//Still even size
							ExifTmpDramAddr = DramAddr + ( HeaderLength >> 1 );

							//Find IFD0_DateTimeTag if need
							for( i = 0; i < IFD_Number; i++ ) {
								EXIFHDR_Parsing_APP1_Sub2( DramAddr, DateTime, ( UINT16 ) ( HeaderLength & 0x00000001 ) );
							}//End for loop

							//patch5.1@ada@Audio annotation begin
							//printf("ExifTmpDramAddr = %lx\n", ExifTmpDramAddr);
							//printf("HeaderLength = %lx\n", HeaderLength);

							//Mark by Phil@08.06.23 s
							/*
							if( G_Playback_Mode == K_PLAYBACK_MODE_ONE ) {
								sts = EXIFHDR_Passing_APP1_ExifTag( DramAddr );

								if( sts ) {
									JPEG_Audio = 1;
								}
								else {
									JPEG_Audio = 0;
								}
							}*/
							//patch5.1@ada@Audio annotation end
							//Mark by Phil@08.06.23 s

							sts = EXIFHDR_Parsing_APP1_Sub3( DramAddr, ( UINT16 ) ( HeaderLength & 0x00000001 ) );
						}
						else//Tiff_IFDOffset != 0x00000008
						{
							sts = FALSE;
							//printf("Tiff_IFDOffset is not found!\n");
						}//End check Tiff_IFDOffset

					}
					else//Tiff_IFD != 0x002a
					{
						sts = FALSE;
						//printf("Tiff_IFD is not found!\n");
					}//End check Tiff_IFD

				}//End check "Exif"
			}
		}
	}
	else//if JFIF header contains APP1
	{
		APPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + EvenOddOffset ] ) );
		//printf("APPn length = %x\n",APPnLength);
		TotalAPPnLength += ( 2 + APPnLength );
		HeaderLength += ( 2 + ( UINT32 ) APPnLength );
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_APP1_Sub2
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_APP1_Sub2( UINT32 DramAddr, UINT8* DateTime, UINT16 EvenOddOffset ) USING_0
{
	UINT8 sts = FALSE;
	UINT8 j;

	//Find IFD0_DateTimeTag if need
	SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, ( UINT16 ) ( K_IFD_SIZE + 2 ) );

	if( EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 0 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) ) ) == 0x0132 ) {
		IFD_DateTime_Offset = EXIFHDR_NeedSwitchULONGOrder( ByteOrder, ( ( ( ( UINT32 ) G_ucStorData[ 8 + EvenOddOffset ] ) << 24 ) | ( ( ( UINT32 ) G_ucStorData[ 9 + EvenOddOffset ] ) << 16 ) | ( ( ( UINT32 ) G_ucStorData[ 10 + EvenOddOffset ] ) << 8 ) | ( ( UINT32 ) G_ucStorData[ 11 + EvenOddOffset ] ) ) ) + ByteOrderOffset;
		//*IFD_DateTime_Found = 1;

		ExifTmpDramAddr = DramAddr + ( IFD_DateTime_Offset >> 1 );

		if( IFD_DateTime_Offset & 0x00000001 ) {
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 22 );

			for( j = 0; j < 20; j++ ) {
				DateTime[ j ] = G_ucStorData[ j + 1 ];
			}
			//printf("DateTime = %s\n",DateTime);
		}
		else//even offset
		{
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 20 );

			for( j = 0; j < 20; j++ ) {
				DateTime[ j ] = G_ucStorData[ j ];
			}
			//printf("DateTime = %s\n",DateTime);
		}

		sts = TRUE;
	}
	//patch5.1@ada@Audio annotation begin
	else if( EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 0 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) ) ) == 0x8769 ) {
		IFD_Exif_Pointer_Offset = EXIFHDR_NeedSwitchULONGOrder( ByteOrder, ( ( ( ( UINT32 ) G_ucStorData[ 8 + EvenOddOffset ] ) << 24 ) | ( ( ( UINT32 ) G_ucStorData[ 9 + EvenOddOffset ] ) << 16 ) | ( ( ( UINT32 ) G_ucStorData[ 10 + EvenOddOffset ] ) << 8 ) | ( ( UINT32 ) G_ucStorData[ 11 + EvenOddOffset ] ) ) ) + ByteOrderOffset;

		ExifTmpDramAddr = DramAddr + ( IFD_Exif_Pointer_Offset >> 1 );

		if( IFD_Exif_Pointer_Offset & 0x00000001 ) {
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 4 );
			Exif_IFD_Number = EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) ) );
		}
		else {
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 2 );
			Exif_IFD_Number = EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 0 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) ) );
		}
		//printf("Exif_IFD_Number = %x\n", Exif_IFD_Number);

	}
	//patch5.1@ada@Audio annotation end

	HeaderLength += ( UINT32 ) K_IFD_SIZE;
	ExifTmpDramAddr = DramAddr + ( HeaderLength >> 1 );

	return sts;
}

//patch5.1@ada@Audio annotation begin
//-----------------------------------------------------------------------------
//EXIFHDR_Passing_APP1_ExifTag
//-----------------------------------------------------------------------------
UINT8 EXIFHDR_Passing_APP1_ExifTag( UINT32 DramAddr ) USING_0
{
	UINT8 sts = FALSE;
	UINT8 i, j, EvenOddOffset;
	UINT32 RelatedIndex_Offset;
	UINT32 ExifDramAddr;
	//UINT32 Exiftemp;

	ExifDramAddr = DramAddr + ( IFD_Exif_Pointer_Offset >> 1 ) + 1;
	//Exiftemp = ExifDramAddr;
	if( IFD_Exif_Pointer_Offset & 0x00000001 ) {
		EvenOddOffset = 1;
	}
	else {
		EvenOddOffset = 0;
	}
	//printf("EvenOddOffset = %bx\n", EvenOddOffset);

	for( i = 0; i < Exif_IFD_Number; i++ ) {
		SDRAM_Dram2Sram( ExifDramAddr, G_ucDataBuffer, ( UINT16 ) ( K_IFD_SIZE + 2 ) );
		//printf("%bd\n", i);
		//printf("%bx, %bx, %bx, %bx, %bx\n", G_ucDataBuffer[0], G_ucDataBuffer[1], G_ucDataBuffer[2], G_ucDataBuffer[3], G_ucDataBuffer[4]);
		//printf("%bx, %bx, %bx, %bx, %bx\n", G_ucDataBuffer[5], G_ucDataBuffer[6], G_ucDataBuffer[7], G_ucDataBuffer[8], G_ucDataBuffer[9]);
		//printf("%bx, %bx, %bx, %bx\n", G_ucDataBuffer[10], G_ucDataBuffer[11], G_ucDataBuffer[12], G_ucDataBuffer[13]);

		if( EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 0 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) ) ) == 0xa004 )	//related file tag
		{
			RelatedIndex_Offset = EXIFHDR_NeedSwitchULONGOrder( ByteOrder, ( ( ( ( UINT32 ) G_ucStorData[ 8 + EvenOddOffset ] ) << 24 ) | ( ( ( UINT32 ) G_ucStorData[ 9 + EvenOddOffset ] ) << 16 ) | ( ( ( UINT32 ) G_ucStorData[ 10 + EvenOddOffset ] ) << 8 ) | ( ( UINT32 ) G_ucStorData[ 11 + EvenOddOffset ] ) ) ) + ByteOrderOffset;

			ExifDramAddr = DramAddr + ( RelatedIndex_Offset >> 1 );

			if( RelatedIndex_Offset & 0x00000001 ) {
				SDRAM_Dram2Sram( ExifDramAddr, G_ucDataBuffer, 16 );

				for( j = 0; j <= 12; j++ ) {
					RelatedWavFileName[ j ] = G_ucDataBuffer[ j + 1 ];
				}
			}
			else {
				SDRAM_Dram2Sram( ExifDramAddr, G_ucDataBuffer, 14 );

				for( j = 0; j <= 12; j++ ) {
					RelatedWavFileName[ j ] = G_ucDataBuffer[ j ];
				}
			}
			//printf("%s\n", RelatedWavFileName);
			return sts = TRUE;
		}

		//Exiftemp += (UINT32)K_IFD_SIZE;
		//ExifDramAddr = DramAddr + (Exiftemp>>1);
		ExifDramAddr = ExifDramAddr + ( UINT32 ) ( K_IFD_SIZE >> 1 );
	}

	return sts = FALSE;
}
//patch5.1@ada@Audio annotation end

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_APP1_Sub3
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_APP1_Sub3( UINT32 DramAddr, UINT16 EvenOddOffset ) USING_0
{
	UINT8 sts = TRUE;
	UINT16 i;
	UINT8 search = 1;

	SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 6 );
	NextIFDOffset = EXIFHDR_NeedSwitchULONGOrder( ByteOrder, ( ( ( ( UINT32 ) G_ucStorData[ 0 + EvenOddOffset ] ) << 24 ) | ( ( ( UINT32 ) G_ucStorData[ 1 + EvenOddOffset ] ) << 16 ) | ( ( ( UINT32 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT32 ) G_ucStorData[ 3 + EvenOddOffset ] ) ) ) + ByteOrderOffset;

	if( NextIFDOffset > 0 ) {
		ExifTmpDramAddr = DramAddr + ( NextIFDOffset >> 1 );

		if( NextIFDOffset & 0x00000001 ) {
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 4 );
			IFD_Number = EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) ) );
		}
		else {
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 2 );
			IFD_Number = EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 0 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) ) );
		}

		ExifTmpDramAddr += 1;
		//patch4.3@richie@0610
		for( i = 0; ( ( i < IFD_Number ) && ( search > 0 ) ); i++ ) {
			EXIFHDR_Parsing_APP1_Sub4( ( UINT16 ) ( NextIFDOffset & 0x00000001 ), &search );
		}//End for loop

		if( ( IFD_ThumbnaiFrameOffset == 0 ) || ( IFD_ThumbnaiFrameLength == 0 ) ) {
			Jpeg_Exif_Format = 0;
			Extract_Finished = 1;
			sts = FALSE;
			//printf("Thumbnail information is not found!\n");
		}
	}
	else//NextIFDOffset = 0
	{
		Jpeg_Exif_Format = 0;
		Extract_Finished = 1;
		sts = FALSE;
		//printf("NextIFDOffset is not found!\n");
	}//End check NextIFDOffset

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_APP1_Sub4
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_APP1_Sub4( UINT16 EvenOddOffset, UINT8* search ) USING_0
{
	UINT8 sts = TRUE;

	SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, ( UINT16 ) K_IFD_SIZE + 2 * EvenOddOffset );

	if( EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 0 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) ) ) == 0x0201 ) {
		IFD_ThumbnaiFrameOffset = EXIFHDR_NeedSwitchULONGOrder( ByteOrder, ( ( ( ( UINT32 ) G_ucStorData[ 8 + EvenOddOffset ] ) << 24 ) | ( ( ( UINT32 ) G_ucStorData[ 9 + EvenOddOffset ] ) << 16 ) | ( ( ( UINT32 ) G_ucStorData[ 10 + EvenOddOffset ] ) << 8 ) | ( ( UINT32 ) G_ucStorData[ 11 + EvenOddOffset ] ) ) );

		ExifTmpDramAddr += ( ( UINT32 ) K_IFD_SIZE >> 1 );

		SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, ( UINT16 ) K_IFD_SIZE + 2 * EvenOddOffset );

		if( EXIFHDR_NeedSwitchUSHORTOrder( ByteOrder, ( ( ( ( UINT16 ) G_ucStorData[ 0 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 1 + EvenOddOffset ] ) ) ) == 0x0202 ) {
			IFD_ThumbnaiFrameLength = EXIFHDR_NeedSwitchULONGOrder( ByteOrder, ( ( ( ( UINT32 ) G_ucStorData[ 8 + EvenOddOffset ] ) << 24 ) | ( ( ( UINT32 ) G_ucStorData[ 9 + EvenOddOffset ] ) << 16 ) | ( ( ( UINT32 ) G_ucStorData[ 10 + EvenOddOffset ] ) << 8 ) | ( ( UINT32 ) G_ucStorData[ 11 + EvenOddOffset ] ) ) );
		}
		else {
			Jpeg_Exif_Format = 0;
			Extract_Finished = 1;
			sts = FALSE;
			//printf("IFD_ThumbnaiFrameLength is not found!\n");
		}

		*search = 0;
	}
	else//Still not found IFD1_JPEGInterChangeFormatTag
	{
		ExifTmpDramAddr += ( UINT32 ) ( K_IFD_SIZE >> 1 );
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_APPn
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_APP5( UINT32 DramAddr ) USING_0
{
	UINT8 sts = TRUE;

	//patch4.5.2@ada@Fix coding bug begin
	if( HeaderLength & 0x00000001 )
		//if (HeaderLength & 0x0001)
		//patch4.5.2@ada@Fix coding bug end
	{
		// TODO: take care of vlc at add address
		exifQvVlcAtOdd = 1;

		// set Qv/Thb qtable
		exifQvYTblAddr = ( DramAddr + ( HeaderLength >> 1 ) + ( 4 >> 1 ) );
		exifQvUVTblAddr = ( DramAddr + ( HeaderLength >> 1 ) + ( 68 >> 1 ) );

		// set QV/thb addr
		exifQvVlcAddr = ( DramAddr + ( HeaderLength >> 1 ) + ( K_HDR2_SIZE >> 1 ) );

		// set QV/Thb size
		SDRAM_Dram2Sram( ExifTmpDramAddr + ( 136 >> 1 ), G_ucDataBuffer, 6 );

		exifQvXSize = ( ( ( ( UINT16 ) G_ucStorData[ 1 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 2 ] ) );
		exifQvYSize = ( ( ( ( UINT16 ) G_ucStorData[ 3 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 4 ] ) );

		SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 6 );
		if( Extract_Thumbnail == 0 ) {
			APPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 3 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 4 ] ) );
			TotalAPPnLength += ( 2 + APPnLength );
			HeaderLength += ( 2 + ( UINT32 ) APPnLength );
		}
		else {
			ThumbAPPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 3 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 4 ] ) );
			TotalThumbAPPnLength += ( 2 + ThumbAPPnLength );
			HeaderLength += ( 2 + ( UINT32 ) ThumbAPPnLength );
		}
	}
	else//even offset
	{
		exifQvVlcAtOdd = 0;

		// set Qv/Thb qtable
		exifQvYTblAddr = ( DramAddr + ( HeaderLength >> 1 ) + ( 4 >> 1 ) );
		exifQvUVTblAddr = ( DramAddr + ( HeaderLength >> 1 ) + ( 68 >> 1 ) );

		// set QV/thb addr
		exifQvVlcAddr = ( DramAddr + ( HeaderLength >> 1 ) + ( K_HDR2_SIZE >> 1 ) );

		// set QV/Thb size
		SDRAM_Dram2Sram( ExifTmpDramAddr + ( 136 >> 1 ), G_ucDataBuffer, 4 );

		exifQvXSize = ( ( ( ( UINT16 ) G_ucStorData[ 0 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 1 ] ) );
		exifQvYSize = ( ( ( ( UINT16 ) G_ucStorData[ 2 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 ] ) );

		SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 4 );

		if( Extract_Thumbnail == 0 ) {
			APPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 ] ) );
			TotalAPPnLength += ( 2 + APPnLength );
			HeaderLength += ( 2 + ( UINT32 ) APPnLength );
		}
		else {
			ThumbAPPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 ] ) );
			TotalThumbAPPnLength += ( 2 + ThumbAPPnLength );
			HeaderLength += ( 2 + ( UINT32 ) ThumbAPPnLength );
		}
	}
    if((exifQvXSize==320 && exifQvYSize==240) || (exifQvXSize ==240 && exifQvYSize == 320)
       || (exifQvXSize==640 && exifQvYSize==480) ){
        exifAppN = 1;
    }
	
	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_DQT
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
//version4.0@ada@0513 for EXIF
UINT8 EXIFHDR_Parsing_DQT( void ) USING_0
{
	UINT8 sts = TRUE;

	//patch4.5.2@ada@Fix coding bug begin
	if( HeaderLength & 0x00000001 )
		//if (HeaderLength & 0x0001)
		//patch4.5.2@ada@Fix coding bug end
	{
		sts = EXIFHDR_Parsing_DQT_Sub1( 1 );
	}
	else//Even size
	{
		sts = EXIFHDR_Parsing_DQT_Sub1( 0 );
	}

	if( ( sts ) && ( Extract_Thumbnail > 0 ) ) {
		Extract_Thumbnail++;
	}

	if( sts == FALSE ) {
		Jpeg_Exif_Format = 0;
		Extract_Finished = 1;
		//printf("Thumbnail Qtable format not support!\n");
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_DQT_Sub1
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_DQT_Sub1( UINT16 EvenOddOffset ) USING_0
{
	UINT8 sts = TRUE;
	UINT8* qTable;
	UINT16 Tmp;

	//patch4.5.1@ada@Fix EXIF bugs begin
	UINT8 i;
	//patch4.5.1@ada@Fix EXIF bugs end

	DQTLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + EvenOddOffset ] ) );

	if( DQTLength == 0x84 )   //DQT 1 field
	{
		SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 0x88 );

		if( ( G_ucStorData[ 4 + EvenOddOffset ] == 0 ) && ( G_ucStorData[ 69 + EvenOddOffset ] == 1 ) ) 	//PqTq0,PqTq1
		{
			//printf("Start to fill in Q table Y component!\n");
			//For Y component
			qTable = &G_ucStorData[ 5 + EvenOddOffset ];
			//HAL_WriteZigZagQTable(0,qTable);
			jpegQTableSet( 0, qTable, qTable + 65, 1 );
			//printf("Finished to fill in Q table Y component!\n");
			//printf("Start to fill in Q table UV component!\n");
			//For UV Component
			//qTable = &G_ucStorData[70 + EvenOddOffset];
			//HAL_WriteZigZagQTable(1,qTable);

			HeaderLength += ( 2 + ( UINT32 ) DQTLength );
			//printf("Finished to fill in Q table UV component!\n");
		}
		else {
			sts = FALSE;
			//printf("DQT error 1\n");
		}
	}
	else//Other Qtable format
	{
		if( DQTLength == 0xc5 )   //DQT 1 field
		{
			SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 0xca );

			//patch4.5.1@ada@Fix EXIF bugs begin
			if( ( G_ucStorData[ 4 + EvenOddOffset ] == 0 ) && ( G_ucStorData[ 69 + EvenOddOffset ] == 1 ) && ( ( G_ucStorData[ 134 + EvenOddOffset ] == 1 ) || ( G_ucStorData[ 134 + EvenOddOffset ] == 2 ) ) ) 	//PqTq0,PqTq1
				//if ((G_ucStorData[4 + EvenOddOffset] == 0) && (G_ucStorData[69 + EvenOddOffset] == 1) && (G_ucStorData[134 + EvenOddOffset] == 2))	 //PqTq0,PqTq1
				//patch4.5.1@ada@Fix EXIF bugs end
			{
				////printf("Start to fill in Q table Y component!\n");
				//For Y component
				qTable = &G_ucStorData[ 5 + EvenOddOffset ];
				//HAL_WriteZigZagQTable(0,qTable);
				jpegQTableSet( 0, qTable, qTable + 65, 1 );
				//printf("Finished to fill in Q table Y component!\n");
				//printf("Start to fill in Q table UV component!\n");
				//For UV component
				//qTable = &G_ucStorData[70 + EvenOddOffset];
				//HAL_WriteZigZagQTable(1,qTable);

				HeaderLength += ( 2 + ( UINT32 ) DQTLength );
				//printf("Finished to fill in Q table UV component!\n");
			}
			else {
				sts = FALSE;
				//printf("DQT error 2\n");
			}
		}
		else {
			if( DQTLength == 0x43 )   //DQT 2 Field
			{
				//patch4.5.1@ada@Fix EXIF bugs begin
				SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 0x90 );
				//SDRAM_Dram2Sram(ExifTmpDramAddr,G_ucDataBuffer,0x8c);
				//patch4.5.1@ada@Fix EXIF bugs end

				if( ( G_ucStorData[ 4 + EvenOddOffset ] == 0 ) && ( G_ucStorData[ 73 + EvenOddOffset ] == 1 ) )   //PqTq0,PqTq1
				{
					//For Y component
					qTable = &G_ucStorData[ 5 + EvenOddOffset ];
					//HAL_WriteZigZagQTable(0,qTable);
					jpegQTableSet( 0, qTable, qTable + 69, 1 );
					//printf("Finished to fill in Q table Y component!\n");
					//printf("Start to fill in Q table UV component!\n");
					//For UV Component
					//qTable = &G_ucStorData[74 + EvenOddOffset];
					//HAL_WriteZigZagQTable(1,qTable);

					//patch4.5.1@ada@Fix EXIF bugs begin
					if( G_ucStorData[ 142 + EvenOddOffset ] == 2 )	//3 fields
					{
						for( i = 0; i < 65; i++ ) {
							if( G_ucStorData[ 143 + EvenOddOffset ] != G_ucStorData[ 74 + EvenOddOffset ] ) {
								//printf("DQT error 5\n");
								sts = FALSE;
								return sts;
							}
						}

						Tmp = ( 4 + ( DQTLength * 3 ) );
						DQTLength = Tmp;
						HeaderLength += ( 2 + ( UINT32 ) DQTLength );
					}
					else	//2 fields
					{
						Tmp = ( 2 + ( DQTLength << 1 ) );
						DQTLength = Tmp;
						HeaderLength += ( 2 + ( UINT32 ) DQTLength );
					}

					//Tmp = (2 + (DQTLength << 1));
					//DQTLength = Tmp;
					//HeaderLength += (2 + (UINT32)DQTLength);

					//patch4.5.1@ada@Fix EXIF bugs end

					//printf("Finished to fill in Q table UV component!\n");

				}
				else//Other Qtable format,not support
				{
					sts = FALSE;
					//printf("DQT error 3\n");
				}
			}
			else {
				sts = FALSE;
				//printf("DQT error 4\n");
			}
		}
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_DQT_Sub2
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
//version4.0@ada@0513 for EXIF
/*UINT8 EXIFHDR_Parsing_DQT_Sub2(UINT16 EvenOddOffset) USING_0
{
	 UINT8 sts = TRUE;
	 UINT16 Tmp;

	 DQTLength = ((((UINT16)G_ucStorData[2 + EvenOddOffset]) << 8) | ((UINT16)G_ucStorData[3 +EvenOddOffset]));

	 if ((DQTLength == 0x84)  || (DQTLength == 0xc5))//Thumbnail DQT 1 field
	 {
		  HeaderLength += (2 + (UINT32)DQTLength);
	 }
	 else//Other Qtable format
	 {
		  if (DQTLength == 0x43)   //DQT 2 field
		  {
			   Tmp = (2 + (DQTLength << 1));
			   DQTLength = Tmp;
			   HeaderLength += (2 + (UINT32)DQTLength);
		  }
		  else//Other Qtable format not support
		  {
			   sts = FALSE;
			   //printf("DQT error 5\n");
		  }
	 }

	 return sts;
}*/

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_DHT
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_DHT( void ) USING_0
{
	UINT8 sts = TRUE;

	//patch4.5.2@ada@Fix coding bug begin
	if( HeaderLength & 0x00000001 )
		//if (HeaderLength & 0x0001)
		//patch4.5.2@ada@Fix coding bug end
	{
		sts = EXIFHDR_Parsing_DHT_Sub( 1 );
	}
	else//even offset
	{
		sts = EXIFHDR_Parsing_DHT_Sub( 0 );
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_DHT_Sub
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_DHT_Sub( UINT16 EvenOddOffset ) USING_0
{
	UINT8 sts = TRUE;
	UINT16 i;

	DHTLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + EvenOddOffset ] ) );

	SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 512 );	//ada@0327

	if( DHTLength == 0x01a2 ) {   //DHT 1 field
		if( G_ucStorData[ 4 + EvenOddOffset ] == 0x00 ) {
			for( i = ( 5 + EvenOddOffset ); i < ( 33 + EvenOddOffset ); i++ ) {
				if( G_ucStorData[ i ] != DHT_YDC[ i - ( 5 + EvenOddOffset ) ] ) {
					//printf("DHT error 1-1\n");
					sts = FALSE;
					break;
				}
			}
		}
		else {
			//printf("DHT error 1-2\n");
			sts = FALSE;
		}

		if( sts ) {
			if( G_ucStorData[ 33 + EvenOddOffset ] == 0x10 ) {
				for( i = ( 34 + EvenOddOffset ); i < ( 212 + EvenOddOffset ); i++ ) {
					if( G_ucStorData[ i ] != DHT_YAC[ i - ( 34 + EvenOddOffset ) ] ) {
						//printf("DHT error 2-1\n");
						sts = FALSE;
						break;
					}
				}

				if( sts ) {
					if( G_ucStorData[ 212 + EvenOddOffset ] == 0x01 ) {
						for( i = ( 213 + EvenOddOffset ); i < ( 241 + EvenOddOffset ); i++ ) {
							if( G_ucStorData[ i ] != DHT_UVDC[ i - ( 213 + EvenOddOffset ) ] ) {
								//printf("DHT error 2-2\n");
								sts = FALSE;
								break;
							}
						}
					}
				}
			}
			else {
				if( G_ucStorData[ 33 + EvenOddOffset ] == 0x01 ) {
					for( i = ( 34 + EvenOddOffset ); i < ( 62 + EvenOddOffset ); i++ ) {
						if( G_ucStorData[ i ] != DHT_UVDC[ i - ( 34 + EvenOddOffset ) ] ) {
							//printf("DHT error 2-3\n");
							sts = FALSE;
							break;
						}
					}

					if( sts ) {
						if( G_ucStorData[ 62 + EvenOddOffset ] == 0x10 ) {
							for( i = ( 63 + EvenOddOffset ); i < ( 241 + EvenOddOffset ); i++ ) {
								if( G_ucStorData[ i ] != DHT_YAC[ i - ( 63 + EvenOddOffset ) ] ) {
									//printf("DHT error 2-4\n");
									sts = FALSE;
									break;
								}
							}
						}
					}
				}
				else {
					//printf("DHT error 2-5\n");
					sts = FALSE;
				}
			}
		}

		if( ( sts ) && ( G_ucStorData[ 241 + EvenOddOffset ] == 0x11 ) ) {
			for( i = ( 242 + EvenOddOffset ); i < ( 420 + EvenOddOffset ); i++ ) {
				if( G_ucStorData[ i ] != DHT_UVAC[ i - ( 242 + EvenOddOffset ) ] ) {
					//printf("DHT error 3-1\n");
					sts = FALSE;
					break;
				}
			}
		}
		else {
			//printf("DHT error 3-2\n");
			sts = FALSE;
		}

		if( sts ) {
			HeaderLength += ( 2 + ( UINT32 ) DHTLength );

			if( Extract_Thumbnail > 0 ) {
				Extract_Thumbnail++;   //Thumbnail DHT is extracted
			}
		}
	}
	else { //Other DHT format
		if( DHTLength == 0x001f ) {     //DHT 4 field
			if( G_ucStorData[ 4 + EvenOddOffset ] == 0x00 ) {
				for( i = ( 5 + EvenOddOffset ); i < ( 33 + EvenOddOffset ); i++ ) {
					if( G_ucStorData[ i ] != DHT_YDC[ i - ( 5 + EvenOddOffset ) ] ) {
						//printf("DHT error 4-1\n");
						sts = FALSE;
						break;
					}
				}
			}
			else {
				//printf("DHT error 4-2\n");
				sts = FALSE;
			}

			if( sts ) {
				if( ( G_ucStorData[ 36 + EvenOddOffset ] == 0xb5 ) && ( G_ucStorData[ 37 + EvenOddOffset ] == 0x10 ) ) {
					for( i = ( 38 + EvenOddOffset ); i < ( 216 + EvenOddOffset ); i++ ) {
						if( G_ucStorData[ i ] != DHT_YAC[ i - ( 38 + EvenOddOffset ) ] ) {
							//printf("DHT error 5-1\n");
							sts = FALSE;
							break;
						}
					}

					if( sts ) {
						if( ( G_ucStorData[ 219 + EvenOddOffset ] == 0x1f ) && ( G_ucStorData[ 220 + EvenOddOffset ] == 0x01 ) ) {
							for( i = ( 221 + EvenOddOffset ); i < ( 249 + EvenOddOffset ); i++ ) {
								if( G_ucStorData[ i ] != DHT_UVDC[ i - ( 221 + EvenOddOffset ) ] ) {
									//printf("DHT error 5-2\n");
									sts = FALSE;
									break;
								}
							}
						}
					}
				}
				else {
					if( ( G_ucStorData[ 36 + EvenOddOffset ] == 0x1f ) && ( G_ucStorData[ 37 + EvenOddOffset ] == 0x01 ) ) {
						for( i = ( 38 + EvenOddOffset ); i < ( 66 + EvenOddOffset ); i++ ) {
							if( G_ucStorData[ i ] != DHT_UVDC[ i - ( 38 + EvenOddOffset ) ] ) {
								//printf("DHT error 5-3\n");
								sts = FALSE;
								break;
							}
						}

						if( sts ) {
							if( ( G_ucStorData[ 69 + EvenOddOffset ] == 0xb5 ) && ( G_ucStorData[ 70 + EvenOddOffset ] == 0x10 ) ) {
								for( i = ( 71 + EvenOddOffset ); i < ( 249 + EvenOddOffset ); i++ ) {
									if( G_ucStorData[ i ] != DHT_YAC[ i - ( 71 + EvenOddOffset ) ] ) {
										//printf("DHT error 5-4\n");
										sts = FALSE;
										break;
									}
								}
							}
						}
					}
				}
			}

			if( ( sts ) && ( G_ucStorData[ 252 + EvenOddOffset ] == 0xb5 ) && ( G_ucStorData[ 253 + EvenOddOffset ] == 0x11 ) ) {
				for( i = ( 254 + EvenOddOffset ); i < ( 432 + EvenOddOffset ); i++ ) {
					if( G_ucStorData[ i ] != DHT_UVAC[ i - ( 254 + EvenOddOffset ) ] ) {
						//printf("DHT error 8\n");
						sts = FALSE;
						break;
					}
				}
			}
			else {
				//printf("DHT error 8-1\n");
				sts = FALSE;
			}

			if( sts ) {
				DHTLength = ( 2 + 2 + 2 + ( DHTLength << 1 ) + ( 0x00b5 << 1 ) );
				HeaderLength += ( 2 + ( UINT32 ) DHTLength );

				if( Extract_Thumbnail > 0 ) {
					Extract_Thumbnail++;   //Thumbnail DHT is extracted
				}
			}
		}
		else//Other Huffman table not support
		{
			sts = FALSE;
			//printf("DHT error 9\n");
		}
	}

	if( sts == FALSE ) {
		Jpeg_Exif_Format = 0;
		Extract_Finished = 1;
		//printf("Other Huffman table format not support!\n");
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_SOF
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_SOF( struct Exif_Decompress_Parameter* ThumbPara, struct Exif_Decompress_Parameter* ImagePara ) USING_0
{
	UINT8 sts = TRUE;

	SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 20 );

	//patch4.5.2@ada@Fix coding bug begin
	if( HeaderLength & 0x00000001 )
		//if (HeaderLength & 0x0001)
		//patch4.5.2@ada@Fix coding bug end
	{
		sts = EXIFHDR_Parsing_SOF_Sub( ThumbPara, ImagePara, 1 );
	}
	else {
		sts = EXIFHDR_Parsing_SOF_Sub( ThumbPara, ImagePara, 0 );
	}

	if( sts == FALSE ) {
		Jpeg_Exif_Format = 0;
		Extract_Finished = 1;
		//printf("Other SOF format not support!\n");
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_SOF_Sub
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_SOF_Sub( struct Exif_Decompress_Parameter* ThumbPara, struct Exif_Decompress_Parameter* ImagePara, UINT16 EvenOddOffset ) USING_0
{
	UINT8 sts = TRUE;

	SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 20 );

	SOFLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + EvenOddOffset ] ) );

	if( SOFLength == 0x11 ) {
		//Get image height and width
		if( Extract_Thumbnail > 0 )   //Thumbnail SOF field
		{
			ThumbPara->Height = ( ( ( ( UINT16 ) G_ucStorData[ 5 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 6 + EvenOddOffset ] ) );
			ThumbPara->Width = ( ( ( ( UINT16 ) G_ucStorData[ 7 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 8 + EvenOddOffset ] ) );
			//printf("Thumbnail width = %x\n",ThumbPara->Width);
			//printf("Thumbnail height = %x\n",ThumbPara->Height);

			//Get image type if 422 or 411
			if( G_ucStorData[ 11 + EvenOddOffset ] == 0x22 ) {
				ThumbPara->DataType = K_File_DataType_YUV420;
				//printf("This thumbnail is YUV411 format!\n");
				HeaderLength += ( 2 + ( UINT32 ) SOFLength );
				Extract_Thumbnail++;
			}
			else {
				if( G_ucStorData[ 11 + EvenOddOffset ] == 0x21 ) {
					ThumbPara->DataType = K_File_DataType_YUV422;
					//printf("This thumbnail is YUV422 format!\n");
					HeaderLength += ( 2 + ( UINT32 ) SOFLength );
					Extract_Thumbnail++;
				}
				else {
					sts = FALSE;
					//printf("SOF error 1\n");
				}
			}
		}
		else//Image SOF
		{
			ImagePara->Height = ( ( ( ( UINT16 ) G_ucStorData[ 5 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 6 + EvenOddOffset ] ) );
			ImagePara->Width = ( ( ( ( UINT16 ) G_ucStorData[ 7 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 8 + EvenOddOffset ] ) );
			//printf("Image width = %x\n",ImagePara->Width);
			//printf("Image height = %x\n",ImagePara->Height);

			//Get image type if 422 or 411
			if( G_ucStorData[ 11 + EvenOddOffset ] == 0x22 ) {
				ImagePara->DataType = K_File_DataType_YUV420;
				//printf("This file is YUV411 format!\n");
				HeaderLength += ( 2 + ( UINT32 ) SOFLength );
			}
			else {
				if( G_ucStorData[ 11 + EvenOddOffset ] == 0x21 ) {
					ImagePara->DataType = K_File_DataType_YUV422;
					//printf("This file is YUV422 format!\n");
					HeaderLength += ( 2 + ( UINT32 ) SOFLength );
				}
				else {
					sts = FALSE;
					//printf("SOF error 2\n");
				}
			}
		}
	}
	else {
		sts = FALSE;
		//printf("SOF error 3\n");
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_DRI
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_DRI( void ) USING_0
{
	UINT8 sts = TRUE;

	//patch4.5.2@ada@Fix coding bug begin
	if( HeaderLength & 0x00000001 )
		//if (HeaderLength & 0x0001)
		//patch4.5.2@ada@Fix coding bug end
	{
		DRILength = ( ( ( ( UINT16 ) G_ucStorData[ 3 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 4 ] ) );
		G_JpgRstMcuNum = ( ( ( ( UINT16 ) G_ucStorData[ 5 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 6 ] ) );		
	}
	else//even offset
	{
		DRILength = ( ( ( ( UINT16 ) G_ucStorData[ 2 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 ] ) );
		G_JpgRstMcuNum = ( ( ( ( UINT16 ) G_ucStorData[ 4 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 5 ] ) );
	}

	HeaderLength += ( 2 + ( UINT32 ) DRILength );

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_SOS
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF

//patch4.5@ada@Fix EXIF bug begin
UINT8 EXIFHDR_Parsing_SOS( UINT32 DramAddr, BIT ShowThumb, struct Exif_Decompress_Parameter* ThumbPara, struct Exif_Decompress_Parameter* ImagePara ) USING_0
//UINT8 EXIFHDR_Parsing_SOS(UINT32 DramAddr,struct Exif_Decompress_Parameter *ThumbPara,struct Exif_Decompress_Parameter *ImagePara) USING_0
{
	UINT8 sts = TRUE;

	//patch4.5.2@ada@Fix coding bug begin
	if( HeaderLength & 0x00000001 )
		//if (HeaderLength & 0x0001)
		//patch4.5.2@ada@Fix coding bug end
	{
		sts = EXIFHDR_Parsing_SOS_Sub( DramAddr, ShowThumb, ThumbPara, ImagePara, 1 );
		//EXIFHDR_Parsing_SOS_Sub(DramAddr,ThumbPara,ImagePara,1);
	}
	else {
		sts = EXIFHDR_Parsing_SOS_Sub( DramAddr, ShowThumb, ThumbPara, ImagePara, 0 );
		//EXIFHDR_Parsing_SOS_Sub(DramAddr,ThumbPara,ImagePara,0);
	}

	return sts;
}
//patch4.5@ada@Fix EXIF bug end

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_SOS_Sub
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF

//patch4.5@ada@Fix EXIF bug begin
UINT8 EXIFHDR_Parsing_SOS_Sub( UINT32 DramAddr, BIT ShowThumb, struct Exif_Decompress_Parameter* ThumbPara, struct Exif_Decompress_Parameter* ImagePara, UINT16 EvenOddOffset ) USING_0
//UINT8 EXIFHDR_Parsing_SOS_Sub(UINT32 DramAddr,struct Exif_Decompress_Parameter *ThumbPara,struct Exif_Decompress_Parameter *ImagePara,UINT16 EvenOddOffset) USING_0
//patch4.5@ada@Fix EXIF bug end
{
	UINT8 sts = TRUE;
//	UINT8 t1, t2;

	//patch4.5@ada@Fix EXIF bug begin
	UINT8 tmp1, tmp2;
	//patch4.5@ada@Fix EXIF bug end
	UINT32 ImgPixel;

	SOSLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 + EvenOddOffset ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 + EvenOddOffset ] ) );

	HeaderLength += ( 2 + ( UINT32 ) SOSLength );

	if( Extract_Thumbnail > 0 ) {
		//richie@si0419
		//printf("HeaderLength=%lx\n",HeaderLength);
		//printf("IFD_ThumbnaiFrameLength=%lx\n",IFD_ThumbnaiFrameLength);
		G_ulExifThumbSize = IFD_ThumbnaiFrameLength;

		//Calculate thumbnail size  				 SOI DQT					DHT 				   SOF  				  SOS   				 EOI
		ThumbPara->Size = IFD_ThumbnaiFrameLength - ( 2 + 2 + ( UINT32 ) DQTLength + 2 + ( UINT32 ) DHTLength + 2 + ( UINT32 ) SOFLength + 2 + ( UINT32 ) SOSLength + 2 );

		if( DRILength > 0 ) {
			ThumbPara->Size -= ( 2 + ( UINT32 ) DRILength );
			DRILength = 0; //Reset for main frame image
		}

		if( ThumbAPPnLength > 0 ) {
			ThumbPara->Size -= ( 2 + ( UINT32 ) ThumbAPPnLength );
		}

		//Calculate thumbnail start address in SDRAM
		ThumbPara->Addr = ( DramAddr + ( HeaderLength >> 1 ) );
		//printf("Thumbnail Size = %lx\n",ThumbPara->Size);
		//printf("Thumbnail start address in SDRAM = %lx\n",ThumbPara->Addr);

		//patch4.5.2@ada@Fix coding bug begin
		if( HeaderLength & 0x00000001 )
			//if (HeaderLength & 0x0001)
			//patch4.5.2@ada@Fix coding bug end
		{
			exifThbVlcAtOdd = 1;
		}
		else {
			exifThbVlcAtOdd = 0;
		}

		//Calculate thumbnail EOI address

		//patch4.5@ada@Fix EXIF bug begin

		//HeaderLength += ThumbPara->Size;
		//printf("HeaderLength = %lx\n",HeaderLength);
		//Extract_Thumbnail++;

		if( ShowThumb ) {
			Extract_Thumbnail = 0;
			Extract_Finished = 1;
			sts = TRUE;
		}
		else {
			Extract_Thumbnail = 0;
			HAL_DramStartAddrSet( ( DramAddr + 2 ), K_SDRAM_Prefetch );
			HAL_DramWordDataRead( &tmp1, &tmp2 );
			HeaderLength = ( UINT32 ) ( ( ( ( ( UINT16 ) tmp1 ) << 8 ) | ( ( UINT16 ) tmp2 ) ) + 4 );
			//printf("HeaderLength = %lx\n",HeaderLength);
		}
		//patch4.5@ada@Fix EXIF bug end

	}
	else//Image SOS
	{
		//Calculate image size  			SOI APPn					 DQT					DHT 					SOF 				   SOS  				  EOI
		ImagePara->Size = exifParsingSize - ( 2 + ( UINT32 ) TotalAPPnLength + 2 + ( UINT32 ) DQTLength + 2 + ( UINT32 ) DHTLength + 2 + ( UINT32 ) SOFLength + 2 + ( UINT32 ) SOSLength + 2 );

		if( APP0Length > 0 ) {
			ImagePara->Size -= ( 2 + ( UINT32 ) APP0Length );
		}

		if( APP1Length > 0 ) {
			ImagePara->Size -= ( 2 + ( UINT32 ) APP1Length );
		}

		if( COMLength > 0 ) {
			ImagePara->Size -= ( 2 + ( UINT32 ) COMLength );
		}

		if( DRILength > 0 ) {
			ImagePara->Size -= ( 2 + ( UINT32 ) DRILength );
		}

		//Calculate image start address in SDRAM
		ImagePara->Addr = DramAddr + ( HeaderLength >> 1 );
		//printf("VLC stream size = %lx\n",ImagePara->Size);
		//printf("Image start address in SDRAM = %lx\n",ImagePara->Addr);

		//patch4.5.2@ada@Fix coding bug begin
		if( HeaderLength & 0x00000001 )
			//if (HeaderLength & 0x0001)
			//patch4.5.2@ada@Fix coding bug end
		{
			ImgPixel = (UINT32)ImagePara->Height * ImagePara->Width;
			//printf("H=%x,W=%x,Size=%lx,P=%lx\n",ImagePara->Height,ImagePara->Width,ImagePara->Size,ImgPixel);
			if (ImgPixel > 0x800000){
				return FALSE;
			}
			exifVlcAtOdd = 1;
		}
		else {
			exifVlcAtOdd = 0;
		}

		//Calculate image EOI address
		HeaderLength += ImagePara->Size;
		Extract_Finished = 1;
		sts = TRUE;
	}

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_EOI
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_EOI( UINT32 DramAddr, BIT ShowThumb ) USING_0
{
	UINT8 sts = TRUE;
	UINT8 tmp1, tmp2;

	if( Jpeg_Exif_Format == K_EXIF_IMAGE_FORMAT )     //EXIF file format
	{
		if( Extract_Thumbnail > 0 ) {
			Extract_Thumbnail = 0;

			if( ShowThumb ) {
				Extract_Finished = 1;    //Extraction stops here
				sts = TRUE;
			}

			//printf("Thumbnail EOI is found!\n");
		}
		else {
			Extract_Finished = 1;
			sts = FALSE;
		}
	}
	else //JPEG file format
	{
		Extract_Finished = 1;
		sts = FALSE;
	}

	HAL_DramStartAddrSet( ( DramAddr + 2 ), K_SDRAM_Prefetch );

	HAL_DramWordDataRead( &tmp1, &tmp2 );

	HeaderLength = ( UINT32 ) ( ( ( ( ( UINT16 ) tmp1 ) << 8 ) | ( ( UINT16 ) tmp2 ) ) + 4 );

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Parsing_COM
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For EXIF
UINT8 EXIFHDR_Parsing_COM( void ) USING_0
{
	UINT8 sts = TRUE;

	//patch4.5.2@ada@Fix coding bug begin
	if( HeaderLength & 0x00000001 )
		//if (HeaderLength & 0x0001)
		//patch4.5.2@ada@Fix coding bug end
	{
		COMLength = ( ( ( ( UINT16 ) G_ucStorData[ 3 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 4 ] ) );
	}
	else//even offset
	{
		COMLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 ] ) );
	}

	HeaderLength += ( 2 + ( UINT32 ) COMLength );

	return sts;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader0_SRAM2DRAM
//-----------------------------------------------------------------------------
void EXIFHDR_Load_ExifHeader0_SRAM2DRAM( struct HDR0* hdr0, UINT32* pDramHdrAddr ) USING_0
{
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = K_EXIF_STRUCTURE0_SIZE;   //Header0 is even size
	UINT32 temp;

	//----- Load EXIF Header0 from SRAM buffer pSramHeadStart to DRAM *pDramHdrAddr -----
	//printf("Header0 start address in DRAM = %lx\n",*pDramHdrAddr);

	pSramHeadStart = &hdr0->Marker[ 0 ];

	if( TempSramHdrSize <= 1024 ) {
		HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
		HAL_DmaSetSramAddr( 0x0000 );
		HAL_DmaSetDramAddr( *pDramHdrAddr );
		HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
		HAL_CpuSramModeSet( 0x00 );

		*pDramHdrAddr += ( TempSramHdrSize >> 1 );
	}
	else {
		while( TempSramHdrSize >= 1024 ) {
			HAL_SRAMLow2High( 1024, pSramHeadStart );
			HAL_DmaSetSramAddr( 0x0000 );
			HAL_DmaSetDramAddr( *pDramHdrAddr );
			HAL_DmaDo( 1, 0, 1024, 0, 0 );
			HAL_CpuSramModeSet( 0x00 );

			TempSramHdrSize -= 1024;
			pSramHeadStart += 1024;
			*pDramHdrAddr += 512;
		}

		if( TempSramHdrSize > 0 ) {
			HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
			HAL_DmaSetSramAddr( 0x0000 );
			HAL_DmaSetDramAddr( *pDramHdrAddr );
			HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
			HAL_CpuSramModeSet( 0x00 );

			//*pDramHdrAddr += (TempSramHdrSize >> 1);
			temp = ( TempSramHdrSize >> 1 );
			*pDramHdrAddr += temp;
		}
	}

	////printf("Header0 end address in DRAM = %lx\n",*pDramHdrAddr - 1);

}

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader2_SRAM2DRAM
//-----------------------------------------------------------------------------
static void EXIFHDR_Load_ExifHeader2_SRAM2DRAM( struct HDR7* hdr1, UINT32* pDramHdrAddr ) USING_0
{
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = K_EXIF_STRUCTURE2_SIZE;   //Header1 is even size and < 1024
	UINT32 temp;

	//----- Load EXIF Header1 from SRAM buffer pSramHeadStart to DRAM *pDramHdrAddr -----
	//printf("Header1 start address in DRAM = %lx\n",*pDramHdrAddr);

	pSramHeadStart = &hdr1->Marker[ 0 ];

	HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pDramHdrAddr );
	HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
	HAL_CpuSramModeSet( 0x00 );

	//     *pDramHdrAddr += (TempSramHdrSize >> 1);
	temp = ( TempSramHdrSize >> 1 );
	*pDramHdrAddr += temp;
	//printf("Header1 end address in DRAM = %lx\n",*pDramHdrAddr - 1);
}

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader3_SRAM2DRAM
//-----------------------------------------------------------------------------
void EXIFHDR_Load_ExifHeader3_SRAM2DRAM( struct HDR3* hdr2, UINT32* pDramHdrAddr ) USING_0
{
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = ( K_EXIF_STRUCTURE3_SIZE - 1 );  //Header2 is odd size and < 1024
	UINT32 temp;

	//----- Load EXIF Header2 from SRAM buffer pSramHeadStart to DRAM *pDramHdrAddr -----
	//printf("Header2 start address in DRAM = %lx\n",*pDramHdrAddr);

	pSramHeadStart = &hdr2->Marker[ 0 ];

	HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pDramHdrAddr );
	HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
	HAL_CpuSramModeSet( 0x00 );

	//The last byte of header2
	temp = ( TempSramHdrSize >> 1 );
	*pDramHdrAddr += temp;

	HAL_DramStartAddrSet( *pDramHdrAddr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite( *( pSramHeadStart + TempSramHdrSize ), 0xff );
	//*pDramHdrAddr += ((TempSramHdrSize >> 1) + 1);
	//HAL_DramWordDataWrite(*pDramHdrAddr,*(pSramHeadStart + TempSramHdrSize),0xff);

	//printf("Header2 end address in DRAM = %lx\n",*pDramHdrAddr);
}

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader4_SRAM2DRAM
//-----------------------------------------------------------------------------
void EXIFHDR_Load_ExifHeader4_SRAM2DRAM( struct HDR4* hdr3, UINT32* pDramHdrAddr ) USING_0
{
	UINT8 t1, t2;
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = ( K_EXIF_STRUCTURE4_SIZE - 1 );  //Header3 is odd size and < 1024
	UINT32 temp;

	//----- Load EXIF Header3 from SRAM buffer pSramHeadStart to DRAM *pDramHdrAddr -----
	////printf("Header3 start address in DRAM = %lx\n",*pDramHdrAddr);

	//The first byte of header3
	pSramHeadStart = &hdr3->Marker[ 0 ];

	//set SDRAM buffer start address
	HAL_DramStartAddrSet( *pDramHdrAddr, K_SDRAM_Prefetch );
	HAL_DramWordDataRead( &t1, &t2 );

	//set SDRAM buffer start address
	HAL_DramStartAddrSet( *pDramHdrAddr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite( t1, *pSramHeadStart );

	//set SDRAM buffer start address
	HAL_DramStartAddrSet( *pDramHdrAddr, K_SDRAM_Prefetch );
	HAL_DramWordDataRead( &t1, &t2 );
	////printf("The first word in SDRAM address [%lx] = (%bx,%bx)\n",*pDramHdrAddr,t1,t2);

	*pDramHdrAddr += 1;
	pSramHeadStart = &hdr3->Marker[ 1 ];

	HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pDramHdrAddr );
	HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
	HAL_CpuSramModeSet( 0x00 );

	//     *pDramHdrAddr += (TempSramHdrSize >> 1);
	temp = ( TempSramHdrSize >> 1 );

	*pDramHdrAddr += temp;
	////printf("The end address of all header in DRAM = %lx\n",(*pDramHdrAddr - 1));
}

//-----------------------------------------------------------------------------
//EXIFHDR_ByteSwapOfUSHORT
//-----------------------------------------------------------------------------
UINT16 EXIFHDR_ByteSwapOfUSHORT( UINT16 x ) USING_0
{
	x = ( ( ( x & 0xff00 ) >> 8 ) | ( ( x & 0x00ff ) << 8 ) );
	return x;
}

//-----------------------------------------------------------------------------
//EXIFHDR_ByteSwapOfULONG
//-----------------------------------------------------------------------------
UINT32 EXIFHDR_ByteSwapOfULONG( UINT32 x ) USING_0 {
	x = ( ( ( x & 0xff000000 ) >> 24 ) | ( ( x & 0x00ff0000 ) >> 8 ) | ( ( x & 0x0000ff00 ) << 8 ) | ( ( x & 0x000000ff ) << 24 ) );
	return x;
}

//-----------------------------------------------------------------------------
//EXIFHDR_LoadExifHeader
//-----------------------------------------------------------------------------
UINT32 EXIFHDR_LoadExifHeader( UINT32 vlcStart, UINT32 exifAddr, UINT32 tmbSize ) USING_0
{
	UINT32 hdrSize, size;
	UINT32 TempHdrAddr;

	//patch5.0@richie@sdram mapping begin
	UINT32 InitialDramHeaderAddr = exifAddr;//K_SDRAM_EXIF_BufAddr;
	//patch5.0@richie@sdram mapping end

	////printf("Thumbnail size = %lx\n",tmbSize);

	//patch5.1@ada@Audio annotation begin

	hdrSize = EXIF_FIX_HDR_SIZE;

	if( tmbSize & 0x00000001 ) {
		size = ( K_EXIF_STRUCTURE0_SIZE + tmbSize + K_EXIF_STRUCTURE3_SIZE + K_EXIF_STRUCTURE4_SIZE );
	}
	else {
		size = ( K_EXIF_STRUCTURE0_SIZE + tmbSize + K_EXIF_STRUCTURE2_SIZE + K_EXIF_STRUCTURE4_SIZE );
	}

	exifApp5Size = hdrSize - size;

	if( exifApp5Size < ( (UINT32)K_HDR2_SIZE + exifQvVlcSize ) ) {
		printf( "qv size too large[%ld], app5 size %ld\n", exifQvVlcSize, exifApp5Size );
	}

	TempHdrAddr = vlcStart - ( hdrSize >> 1 );

	EXIFHDR_Load_ExifHeader0( &InitialDramHeaderAddr, &TempHdrAddr, tmbSize );

	if( tmbSize & 0x00000001 )  //odd size
	{
		InitialDramHeaderAddr += ( K_EXIF_STRUCTURE2_SIZE / 2 );
		EXIFHDR_Load_ExifHeader3( &InitialDramHeaderAddr, &TempHdrAddr, tmbSize );
	}
	else {
		EXIFHDR_Load_ExifHeader2( &InitialDramHeaderAddr, &TempHdrAddr, tmbSize );
		InitialDramHeaderAddr += ( K_EXIF_STRUCTURE3_SIZE / 2 );
	}

	EXIFHDR_Load_ExifHeaderAppN( &TempHdrAddr );

	EXIFHDR_Load_ExifHeader4( &InitialDramHeaderAddr, &TempHdrAddr );

	////printf("EXIF header size = %lx\n",Hdrsize);

	return hdrSize;
}

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader0
//-----------------------------------------------------------------------------
static void EXIFHDR_Load_ExifHeader0( UINT32* pSrcDramHdrAddr, UINT32* pDestDramHdrAddr, UINT32 tmbSize ) USING_0
{
	struct Exif_Header0 Header0;
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = K_EXIF_STRUCTURE0_SIZE;

	//----- Load EXIF Header from DRAM *pDramHdrAddr to SRAM buffer pSramHeadStart-----
	pSramHeadStart = &Header0.SOI.Marker[ 0 ];

	if( TempSramHdrSize <= 1024 ) {
		HAL_DmaSetSramAddr( 0x0000 );
		HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
		HAL_DmaDo( 0, 1, TempSramHdrSize, 0, 0 );
		HAL_SRAMHigh2Low( TempSramHdrSize, pSramHeadStart );
		HAL_CpuSramModeSet( 0x00 );

		*pSrcDramHdrAddr += ( TempSramHdrSize >> 1 );
	}
	else {
		while( TempSramHdrSize >= 1024 ) {
			HAL_DmaSetSramAddr( 0x0000 );
			HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
			HAL_DmaDo( 0, 1, 1024, 0, 0 );
			HAL_SRAMHigh2Low( 1024, pSramHeadStart );
			HAL_CpuSramModeSet( 0x00 );

			TempSramHdrSize -= 1024;
			*pSrcDramHdrAddr += 512;
			pSramHeadStart += 1024;
		}

		if( TempSramHdrSize > 0 ) {
			HAL_DmaSetSramAddr( 0x0000 );
			HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
			HAL_DmaDo( 0, 1, TempSramHdrSize, 0, 0 );
			HAL_SRAMHigh2Low( TempSramHdrSize, pSramHeadStart );
			HAL_CpuSramModeSet( 0x00 );

			*pSrcDramHdrAddr += ( TempSramHdrSize >> 1 );
		}
	}

	EXIFHDR_Modify_SRAM_ExifHeader0( &Header0, tmbSize );
	EXIFHDR_Copy_Exif_Header0( &Header0, pDestDramHdrAddr );
}

#if 0
//patch5.1@ada@Audio annotation begin
//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader0_Audio
//-----------------------------------------------------------------------------
static void EXIFHDR_Load_ExifHeader0_Audio( UINT32* pSrcDramHdrAddr, UINT32* pDestDramHdrAddr, UINT32 tmbSize ) USING_0
{
	struct Exif_Header0_Audio Header0;
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = K_EXIF_STRUCTURE0_AUDIO_SIZE;

	//----- Load EXIF Header from DRAM *pDramHdrAddr to SRAM buffer pSramHeadStart-----
	pSramHeadStart = &Header0.SOI.Marker[ 0 ];

	if( TempSramHdrSize <= 1024 ) {
		HAL_DmaSetSramAddr( 0x0c00 );
		HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
		HAL_DmaDo( 0, 1, TempSramHdrSize, 0, 0 );
		HAL_SRAMHigh2Low( TempSramHdrSize, pSramHeadStart );
		HAL_CpuSramModeSet( 0x00 );

		*pSrcDramHdrAddr += ( TempSramHdrSize >> 1 );
	}
	else {
		while( TempSramHdrSize >= 1024 ) {
			HAL_DmaSetSramAddr( 0x0c00 );
			HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
			HAL_DmaDo( 0, 1, 1024, 0, 0 );
			HAL_SRAMHigh2Low( 1024, pSramHeadStart );
			HAL_CpuSramModeSet( 0x00 );

			TempSramHdrSize -= 1024;
			*pSrcDramHdrAddr += 512;
			pSramHeadStart += 1024;
		}

		if( TempSramHdrSize > 0 ) {
			HAL_DmaSetSramAddr( 0x0c00 );
			HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
			HAL_DmaDo( 0, 1, TempSramHdrSize, 0, 0 );
			HAL_SRAMHigh2Low( TempSramHdrSize, pSramHeadStart );
			HAL_CpuSramModeSet( 0x00 );

			*pSrcDramHdrAddr += ( TempSramHdrSize >> 1 );
		}
	}

	EXIFHDR_Modify_SRAM_ExifHeader0_Audio( &Header0, tmbSize );
	EXIFHDR_Copy_Exif_Header0_Audio( &Header0, pDestDramHdrAddr );
}
//patch5.1@ada@Audio annotation end
#endif

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader2
//-----------------------------------------------------------------------------
static void EXIFHDR_Load_ExifHeader2( UINT32* pSrcDramHdrAddr, UINT32* pDestDramHdrAddr, UINT32 tmbSize ) USING_0
{
	struct Exif_Header2 Header1;
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = K_EXIF_STRUCTURE2_SIZE;

	//----- Load EXIF Header1 from DRAM *pDramHdrAddr to SRAM buffer pSramHeadStart-----
	pSramHeadStart = &Header1.EVEN_COM.Marker[ 0 ];

	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
	HAL_DmaDo( 0, 1, TempSramHdrSize, 0, 0 );
	HAL_SRAMHigh2Low( TempSramHdrSize, pSramHeadStart );
	HAL_CpuSramModeSet( 0x00 );

	*pSrcDramHdrAddr += ( TempSramHdrSize >> 1 );

	EXIFHDR_Copy_Exif_Header2( &Header1, pDestDramHdrAddr, tmbSize );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader3
//-----------------------------------------------------------------------------
static void EXIFHDR_Load_ExifHeader3( UINT32* pSrcDramHdrAddr, UINT32* pDestDramHdrAddr, UINT32 tmbSize ) USING_0
{
	struct Exif_Header3 Header2;
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = ( K_EXIF_STRUCTURE3_SIZE - 1 );
	UINT8 t1, t2;

	//----- Load EXIF Header2 from DRAM *pDramHdrAddr to SRAM buffer pSramHeadStart-----
	pSramHeadStart = &Header2.ODD_COM.Marker[ 0 ];

	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
	HAL_DmaDo( 0, 1, TempSramHdrSize, 0, 0 );
	HAL_SRAMHigh2Low( TempSramHdrSize, pSramHeadStart );
	HAL_CpuSramModeSet( 0x00 );

	//Copy the last byte of header2
	*pSrcDramHdrAddr += ( TempSramHdrSize >> 1 );

	//set SDRAM buffer start address
	HAL_DramStartAddrSet( *pSrcDramHdrAddr, K_SDRAM_Prefetch );
	HAL_DramWordDataRead( &t1, &t2 );
	pSramHeadStart += TempSramHdrSize;
	*pSramHeadStart = t1;

	EXIFHDR_Copy_Exif_Header3( &Header2, pDestDramHdrAddr, tmbSize );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeader4
//-----------------------------------------------------------------------------
void EXIFHDR_Load_ExifHeader4( UINT32* pSrcDramHdrAddr, UINT32* pDestDramHdrAddr ) USING_0
{
	struct Exif_Header4 Header3;
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = ( K_EXIF_STRUCTURE4_SIZE - 1 );  //K_EXIF_STRUCTURE4_SIZE < 1024
	UINT8 t1, t2;

	//----- Load EXIF Header1 from DRAM *pDramHdrAddr to SRAM buffer pSramHeadStart-----
	//Copy the first byte of header3
	pSramHeadStart = &Header3.DQT.Marker[ 0 ];

	//set SDRAM buffer start address
	HAL_DramStartAddrSet( *pSrcDramHdrAddr, K_SDRAM_Prefetch );
	HAL_DramWordDataRead( &t1, &t2 );
	*pSramHeadStart = t2;

	*pSrcDramHdrAddr += 1;
	pSramHeadStart = &Header3.DQT.Marker[ 1 ];

	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pSrcDramHdrAddr );
	HAL_DmaDo( 0, 1, TempSramHdrSize, 0, 0 );
	HAL_SRAMHigh2Low( TempSramHdrSize, pSramHeadStart );
	HAL_CpuSramModeSet( 0x00 );

	EXIFHDR_Modify_SRAM_ExifHeader4( &Header3 );
	EXIFHDR_Copy_Exif_Header4( &Header3, pDestDramHdrAddr );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Load_ExifHeaderAppN
//-----------------------------------------------------------------------------
static void EXIFHDR_Load_ExifHeaderAppN( UINT32* pDestDramHdrAddr ) USING_0
{
	struct Exif_Header1 hdr;

	hdr.APP5.Marker[ 0 ] = 0xff;
	hdr.APP5.Marker[ 1 ] = K_APP5_MARKER;

	hdr.APP5.Length = exifApp5Size - 2;

	jpegQTableGen( exifQvQTableIdx, hdr.APP5.YQTbl, 1 );
	jpegQTableGen( exifQvQTableIdx, hdr.APP5.UVQTbl, 2 );

	hdr.APP5.NewMarker[ 0 ] = 0;
	hdr.APP5.NewMarker[ 1 ] = 0;
	hdr.APP5.Version = 0x01;
	hdr.APP5.ImgWidth = exifQvXSize;
	hdr.APP5.ImgHeight = exifQvYSize;
//	hdr.APP5.Orientation = 0x00;

	HAL_SRAMLow2High( K_HDR2_SIZE, hdr.APP5.Marker );
	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pDestDramHdrAddr );
	HAL_DmaDo( 1, 0, K_HDR2_SIZE, 0, 0 );
	HAL_CpuSramModeSet( 0x00 );

	*pDestDramHdrAddr += ( K_HDR2_SIZE >> 1 );

	// copy vlc to dram
	HAL_GprmDramDma( ( exifQvVlcAddr << 1 ), ( ( *pDestDramHdrAddr ) << 1 ), exifQvVlcSize, 1 );

	*pDestDramHdrAddr += ( exifQvVlcSize >> 1 );

	if( ( exifQvVlcSize & 0x01 ) == 0x01 ) {
		UINT8 t1, t2;

		// fill one more 0xff
		HAL_DramStartAddrSet( *pDestDramHdrAddr, 1 );
		HAL_DramWordDataRead( &t1, &t2 );

		t2 = 0xff;
		HAL_DramStartAddrSet( *pDestDramHdrAddr, 0 );
		HAL_DramWordDataWrite( t1, t2 );

		*pDestDramHdrAddr += 1;
	}

	HAL_GprmDramFill( *pDestDramHdrAddr, exifApp5Size - K_HDR2_SIZE - exifQvVlcSize + 1 , 0xff);

	if( ( exifQvVlcSize & 0x01 ) == 0x01 ) {
		*pDestDramHdrAddr += ( ( ( exifApp5Size - K_HDR2_SIZE - exifQvVlcSize ) >> 1 ) - 1 );
	}
	else {
		*pDestDramHdrAddr += ( ( exifApp5Size - K_HDR2_SIZE - exifQvVlcSize ) >> 1 );
	}
}

//-----------------------------------------------------------------------------
//EXIFHDR_Modify_SRAM_ExifHeader0
//-----------------------------------------------------------------------------
static void EXIFHDR_Modify_SRAM_ExifHeader0( struct Exif_Header0* Hdr0, UINT32 tmbSize ) USING_0
{
	//----- All other items are fixed,only the following ones need to be modified -----
	EXIFHDR_Modify_Exif_APP1( Hdr0, tmbSize );

	EXIFHDR_Modify_DQT( exifThbQTableIdx, &Hdr0->THUMB_DQT );

	EXIFHDR_Modify_SOF( &Hdr0->THUMB_SOF, exifThbXSize, exifThbYSize );
}

#if 0
//patch5.1@ada@Audio annotation begin
//-----------------------------------------------------------------------------
//EXIFHDR_Modify_SRAM_ExifHeader0_Audio
//-----------------------------------------------------------------------------
static void EXIFHDR_Modify_SRAM_ExifHeader0_Audio( struct Exif_Header0_Audio* Hdr0, UINT32 tmbSize ) USING_0 {
	//----- All other items are fixed,only the following ones need to be modified -----
	EXIFHDR_Modify_Exif_APP1_Audio( Hdr0, tmbSize );

	EXIFHDR_Modify_DQT( exifThbQTableIdx, &Hdr0->THUMB_DQT );

	EXIFHDR_Modify_SOF( &Hdr0->THUMB_SOF, exifThbXSize, exifThbYSize );
}
//patch5.1@ada@Audio annotation end
#endif

//-----------------------------------------------------------------------------
//EXIFHDR_Modify_SRAM_ExifHeader4
//-----------------------------------------------------------------------------
void EXIFHDR_Modify_SRAM_ExifHeader4( struct Exif_Header4* Hdr3 ) USING_0
{
	EXIFHDR_Modify_DQT( exifMainQTableIdx, &Hdr3->DQT );

	EXIFHDR_Modify_SOF( &Hdr3->SOF, exifMainXSize, exifMainYSize );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Modify_Exif_APP1
//-----------------------------------------------------------------------------
static void EXIFHDR_Modify_Exif_APP1( struct Exif_Header0* Hdr0, UINT32 tmbSize ) USING_0
{
	UINT16 width;
	UINT16 height;
	UINT32 Hdrsize;
	UINT8 meteringMode = aeMeteringModeGet();
	SINT8 evValue = aeExpCompIdxGet();
	UINT8 isoValue;
	dateStc_t dateinfo;
	//patch5.1@ada@EXIF2.2 upgrade begin
	float Ev;
	float Bv;
	//patch5.1@ada@EXIF2.2 upgrade end

	UINT8 awbMode = sp1kAwbModeGet();//wangjg++
	sp1kAeStatusGet(SP1K_AE_CAPTURE_ISO_VALUE,&isoValue);
	//Modify APP1 entries

	//Header0 size - SOI marker size - APP1 marker size + (thumbnail size + EOI marker size)
	switch(evValue)//20110809
	{
		case 0:
			evValue=0;
			break;
		case -20:
			evValue=-20;
			break;
		case -17:
			evValue=-17;
			break;
		case -13:
			evValue=-13;
			break;
		case -10:
			evValue=-10;
			break;
		case -7:
			evValue=-7;
			break;
		case -3:
			evValue=-3;
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
	Hdrsize = ( K_EXIF_STRUCTURE0_SIZE - 2 - 2 ) + ( tmbSize );
	if(exifRtcPt){
		dateinfo = *exifRtcPt;
	}else{
	HAL_GlobalReadRTC( &dateinfo );
	}
	HAL_GlobalRTCTransferText( &dateinfo, Text_NowDateTime );   //modify date and time

	// 1528 comment: Date time information is here
	// If do not show date time, fill IFD0_DateTimeValue, Exif_DateTimeOriginalValue
	// Exif_DateTimeDigitilizedValue with 0x00
	EXIFHDR_Fill_Text( Text_NowDateTime, Hdr0->APP1.IFD0_DateTimeValue, 20 );
	EXIFHDR_Fill_Text( Text_NowDateTime, Hdr0->APP1.Exif_DateTimeOriginalValue, 20 );
	EXIFHDR_Fill_Text( Text_NowDateTime, Hdr0->APP1.Exif_DateTimeDigitilizedValue, 20 );

	Hdr0->APP1.IFD0_DateTimeValue[ 4 ] = Hdr0->APP1.IFD0_DateTimeValue[ 7 ] = Hdr0->APP1.IFD0_DateTimeValue[ 13 ] = Hdr0->APP1.IFD0_DateTimeValue[ 16 ] = ':';
	Hdr0->APP1.IFD0_DateTimeValue[ 10 ] = ' ';
	Hdr0->APP1.IFD0_DateTimeValue[ 19 ] = 0x00;
	Hdr0->APP1.Exif_DateTimeOriginalValue[ 4 ] = Hdr0->APP1.Exif_DateTimeOriginalValue[ 7 ] = Hdr0->APP1.Exif_DateTimeOriginalValue[ 13 ] = Hdr0->APP1.Exif_DateTimeOriginalValue[ 16 ] = ':';
	Hdr0->APP1.Exif_DateTimeOriginalValue[ 10 ] = ' ';
	Hdr0->APP1.Exif_DateTimeOriginalValue[ 19 ] = 0x00;
	Hdr0->APP1.Exif_DateTimeDigitilizedValue[ 4 ] = Hdr0->APP1.Exif_DateTimeDigitilizedValue[ 7 ] = Hdr0->APP1.Exif_DateTimeDigitilizedValue[ 13 ] = Hdr0->APP1.Exif_DateTimeDigitilizedValue[ 16 ] = ':';
	Hdr0->APP1.Exif_DateTimeDigitilizedValue[ 10 ] = ' ';
	Hdr0->APP1.Exif_DateTimeDigitilizedValue[ 19 ] = 0x00;

	Hdr0->APP1.Length = ( UINT16 ) ( Hdrsize & 0x0000ffff );

	Hdr0->APP1.IFD0_OrientationTag.valueoffset = EXIFHDR_ByteSwapOfULONG( exifRotOpt );   //value

	Hdr0->APP1.Exif_ComponentsConfigurationTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00030201 );  //value."1230"

	Hdr0->APP1.Exif_FlashTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );  //value,flash did not fired

	width = exifMainXSize;
	height = exifMainYSize;

	Hdr0->APP1.Exif_PixelXDimensionTag.valueoffset = EXIFHDR_ByteSwapOfULONG( ( UINT32 ) width );
	Hdr0->APP1.Exif_PixelYDimensionTag.valueoffset = EXIFHDR_ByteSwapOfULONG( ( UINT32 ) height );

	//patch5.1@ada@EXIF2.2 upgrade begin
	//Hdr0->APP1.Exif_ExposureTimeValue[0] = EXIFHDR_ByteSwapOfULONG(0x00000002);
	//Hdr0->APP1.Exif_ExposureTimeValue[1] = EXIFHDR_ByteSwapOfULONG(0x00000024);
	Hdr0->APP1.Exif_ExposureTimeValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( 0x0000000a );

	Hdr0->APP1.Exif_ExposureTimeValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( G_SHUTTER );

	//Ev = 2 + log10(Shutter/0x0a)/log10(2);	//Av + Tv
	//Ev = AE_GetEV( Tidx );
	Ev = 0;
	Bv = Ev / ( float ) 10 - ( float ) 5;
	Bv = Bv * 1000;				//Ev - Sv

	Hdr0->APP1.Exif_BrightnessValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( Bv );
	Hdr0->APP1.Exif_BrightnessValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( 1000 );
	//patch5.1@ada@EXIF2.2 upgrade end

	Hdr0->APP1.Exif_ExposureBiasValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( evValue );
	Hdr0->APP1.Exif_ExposureBiasValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( 10 );

	// 1528 comment: max aperture information is here
	// Fill customer's aperture here
	// The Exif_MaxApertureValue[ 0 ] is numerator
	// The Exif_MaxApertureValue[ 1 ] is nominator
	Hdr0->APP1.Exif_MaxApertureValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( 0x00000003 );
	Hdr0->APP1.Exif_MaxApertureValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( 0x00000002 );

	// metering

	if( meteringMode == SP1K_AE_METER_CENTER ) {
		Hdr0->APP1.Exif_MeteringModeTag.valueoffset = 0x02000000;
	}
	else if( meteringMode == SP1K_AE_METER_SPOT ){
		Hdr0->APP1.Exif_MeteringModeTag.valueoffset = 0x03000000;
	}
	else if( meteringMode == SP1K_AE_METER_MULTI ){
		Hdr0->APP1.Exif_MeteringModeTag.valueoffset = 0x04000000;
	}
	else {
		Hdr0->APP1.Exif_MeteringModeTag.valueoffset = 0x04000000;
	}

#if 0
	//patch5.1@ada@EXIF2.2 upgrade begin
#if 0  // TODO: Need to specifiy G_MANUAL_WB_Flag from 3A flow
	if( G_MANUAL_WB_Flag == 1 ) {
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );
	}
	else if( G_MANUAL_WB_Flag == 2 ) {
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x0000000b );
	}
	else if( G_MANUAL_WB_Flag == 3 ) {
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000003 );
	}
	else if( G_MANUAL_WB_Flag == 4 ) {
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000002 );
	}
	else {
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
	}
#else
	Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
#endif
#endif

	if( awbMode/*G_awb_cur_mode*/ == AWB_MODE_Auto ) {
		Hdr0->APP1.Exif_WhiteBalanceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
	}
	else if( awbMode/*G_awb_cur_mode*/ == AWB_MODE_Tungsten ) {
		Hdr0->APP1.Exif_WhiteBalanceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000003 );
	}
	else if( awbMode/*G_awb_cur_mode*/ == AWB_MODE_Fluorescent ) {
		Hdr0->APP1.Exif_WhiteBalanceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000002 );
	}
	else if( awbMode/*G_awb_cur_mode*/ == AWB_MODE_Sunny ) {
		Hdr0->APP1.Exif_WhiteBalanceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );
	}
	else if( awbMode/*G_awb_cur_mode*/ == AWB_MODE_Cloudy ) {
		Hdr0->APP1.Exif_WhiteBalanceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x0000000a );
	}
	else {
		Hdr0->APP1.Exif_WhiteBalanceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
		Hdr0->APP1.Exif_LightSourceTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
	}

#if 0  // TODO: Need to specifiy G_MANUAL_SHARP_Flag
	if( G_FlashStatus == 1 ) {
		Hdr0->APP1.Exif_FlashTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000009 );
	}
	else if( G_FlashStatus == 0 ) {
		Hdr0->APP1.Exif_FlashTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
	}
#else
	Hdr0->APP1.Exif_FlashTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
#endif

	// 1528 comment: focal length information is here
	// Fill customer's focal length here
	// The Exif_FocalLengthValue[ 0 ] is numerator
	// The Exif_FocalLengthValue[ 1 ] is nominator
	Hdr0->APP1.Exif_FocalLengthValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( 0x00000052 );
	Hdr0->APP1.Exif_FocalLengthValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( 0x0000000b );

	//patch5.1@ada@EXIF2.2 upgrade begin
    if(!g_exifUsrCommentEnable){	//20110809
        Hdr0->APP1.Exif_SubjectArea[ 0 ] = EXIFHDR_ByteSwapOfUSHORT( width / 2 );
        Hdr0->APP1.Exif_SubjectArea[ 1 ] = EXIFHDR_ByteSwapOfUSHORT( height / 2 );
        Hdr0->APP1.Exif_SubjectArea[ 2 ] = EXIFHDR_ByteSwapOfUSHORT( width );
        Hdr0->APP1.Exif_SubjectArea[ 3 ] = EXIFHDR_ByteSwapOfUSHORT( height );
    }
    else{
        Hdr0->APP1.Exif_SubjectArea[ 0 ] = ((UINT16)g_exifUserCommentCode[0] << 8) | (UINT16)g_exifUserCommentCode[1]; //0x5669; //Vivilnk
        Hdr0->APP1.Exif_SubjectArea[ 1 ] = ((UINT16)g_exifUserCommentCode[2] << 8) | (UINT16)g_exifUserCommentCode[3]; //0x7669;
        Hdr0->APP1.Exif_SubjectArea[ 2 ] = ((UINT16)g_exifUserCommentCode[4] << 8) | (UINT16)g_exifUserCommentCode[5]; //0x4c6e;
        Hdr0->APP1.Exif_SubjectArea[ 3 ] = ((UINT16)g_exifUserCommentCode[6] << 8) | (UINT16)g_exifUserCommentCode[7]; //0x6b00;
    }

#if 0  // TODO: Need to specifiy G_MANUAL_SHARP_Flag
	if( G_MANUAL_SHARP_Flag == 0 ) {
		Hdr0->APP1.Exif_SharpnessTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
	}
	else if( G_MANUAL_SHARP_Flag == 1 ) {
		Hdr0->APP1.Exif_SharpnessTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );
	}
	else if( G_MANUAL_SHARP_Flag == 2 ) {
		Hdr0->APP1.Exif_SharpnessTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000002 );
	}
#else
	Hdr0->APP1.Exif_SharpnessTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );
#endif
	Hdr0->APP1.IFD1_OrientationTag.valueoffset = EXIFHDR_ByteSwapOfULONG( exifRotOpt );

	Hdr0->APP1.IFD1_JPEGInterChangeFormatLengthTag.valueoffset = EXIFHDR_ByteSwapOfULONG( K_THUMBNAIL_STRUCTURE_SIZE + tmbSize - 2 );

	//patch
	if(1 == isoValue ){
		Hdr0->APP1.Exif_ISOSpeedRatingsTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000064);
	}else if(2 == isoValue ){
		Hdr0->APP1.Exif_ISOSpeedRatingsTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x000000c8);
	}else if(3 == isoValue ){
		Hdr0->APP1.Exif_ISOSpeedRatingsTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000190);
	}else if(4 == isoValue ){
		Hdr0->APP1.Exif_ISOSpeedRatingsTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000320);
	}
		
    EXIFHDR_Modify_IfdTag(Hdr0); //xian ++ 20081027
}

#if 0
//patch5.1@ada@Audio annotation begin
//-----------------------------------------------------------------------------
//EXIFHDR_Modify_Exif_APP1_Audio
//-----------------------------------------------------------------------------
static void EXIFHDR_Modify_Exif_APP1_Audio( struct Exif_Header0_Audio* Hdr0, UINT32 tmbSize ) USING_0
{
	UINT16 width;
	UINT16 height;
	UINT32 Hdrsize;

	UINT16 fileIndex = G_USR_FileIndex + 1;
	UINT8 name[ 13 ];
	UINT8 i;

	//Modify APP1 entries

	//Header0 size - SOI marker size - APP1 marker size + (thumbnail size + EOI marker size)
	Hdrsize = ( K_EXIF_STRUCTURE0_AUDIO_SIZE - 2 - 2 ) + ( tmbSize );
	//printf("header0 size Hdrsize = %lx\n", Hdrsize);

	HAL_GlobalReadRTC( &G_DATEINFO );
	HAL_RTCTransferText( &G_DATEINFO, Text_NowDateTime );   //modify date and time

	EXIFHDR_Fill_Text( Text_NowDateTime, Hdr0->APP1.IFD0_DateTimeValue, 20 );
	EXIFHDR_Fill_Text( Text_NowDateTime, Hdr0->APP1.Exif_DateTimeOriginalValue, 20 );
	EXIFHDR_Fill_Text( Text_NowDateTime, Hdr0->APP1.Exif_DateTimeDigitilizedValue, 20 );

	Hdr0->APP1.Length = ( UINT16 ) ( Hdrsize & 0x0000ffff );

	Hdr0->APP1.IFD0_OrientationTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );   //value

	Hdr0->APP1.Exif_ComponentsConfigurationTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00030201 );  //value."1230"

	Hdr0->APP1.Exif_FlashTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000000 );  //value,flash did not fired

	//patch5.1@ada@Audio annotation begin
	for( i = 0; i < 4; i++ )
		name[ i ] = G_USR_Company_UpperSIG[ i ];
	for( i = 7; i > 3; i-- ) {
		name[ i ] = ( UINT8 ) ( fileIndex % 10 ) + '0';
		fileIndex /= 10;
	}
	name[ 8 ] = '.';
	name[ 9 ] = 'W';
	name[ 10 ] = 'A';
	name[ 11 ] = 'V';
	name[ 12 ] = 0x00;
	EXIFHDR_Fill_Text( name, Hdr0->APP1.Exif_RelatedSoundFileValue, 13 );
	//patch5.1@ada@Audio annotation end

	width = exifMainXSize;
	height = exifMainYSize;

	Hdr0->APP1.Exif_PixelXDimensionTag.valueoffset = EXIFHDR_ByteSwapOfULONG( ( UINT32 ) width );
	Hdr0->APP1.Exif_PixelYDimensionTag.valueoffset = EXIFHDR_ByteSwapOfULONG( ( UINT32 ) height );

	Hdr0->APP1.Exif_ExposureTimeValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( 0x00000002 );
	Hdr0->APP1.Exif_ExposureTimeValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( 0x00000024 );

	Hdr0->APP1.Exif_ExposureBiasValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( 0x00000001 );
	Hdr0->APP1.Exif_ExposureBiasValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( 0x0000000b );

	Hdr0->APP1.Exif_MaxApertureValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( 0x00000003 );
	Hdr0->APP1.Exif_MaxApertureValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( 0x00000002 );

	Hdr0->APP1.Exif_FocalLengthValue[ 0 ] = EXIFHDR_ByteSwapOfULONG( 0x00000052 );
	Hdr0->APP1.Exif_FocalLengthValue[ 1 ] = EXIFHDR_ByteSwapOfULONG( 0x0000000b );

	Hdr0->APP1.IFD1_OrientationTag.valueoffset = EXIFHDR_ByteSwapOfULONG( 0x00000001 );

	Hdr0->APP1.IFD1_JPEGInterChangeFormatLengthTag.valueoffset = EXIFHDR_ByteSwapOfULONG( K_THUMBNAIL_STRUCTURE_SIZE + tmbSize - 2 );
}
//patch5.1@ada@Audio annotation end
#endif

//-----------------------------------------------------------------------------
//EXIFHDR_Modify_DQT
//-----------------------------------------------------------------------------
static void EXIFHDR_Modify_DQT( UINT8 idx, struct HDR4* hdr ) USING_0 {
	jpegQTableGen( idx, hdr->Content0, 1 );
	jpegQTableGen( idx, hdr->Content1, 2 );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Modify_SOF
//-----------------------------------------------------------------------------
void EXIFHDR_Modify_SOF( struct HDR3* hdr, UINT16 width, UINT16 height ) USING_0
{
	//Set image type if 422 or 411.
	if( exifFileFormat == K_File_DataType_YUV420 ) {
		hdr->Content[ 7 ] = 0x22;
	}
	else {
		hdr->Content[ 7 ] = 0x21;
	}

	hdr->Content[ 1 ] = ( UINT8 ) ( ( height & 0xff00 ) >> 8 );
	hdr->Content[ 2 ] = ( UINT8 ) ( height & 0x00ff );
	hdr->Content[ 3 ] = ( UINT8 ) ( ( width & 0xff00 ) >> 8 );
	hdr->Content[ 4 ] = ( UINT8 ) ( width & 0x00ff );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Copy_Exif_Header0
//-----------------------------------------------------------------------------
void EXIFHDR_Copy_Exif_Header0( struct Exif_Header0* Hdr0, UINT32* pDestDramHdrAddr ) USING_0
{
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = K_EXIF_STRUCTURE0_SIZE;  //K_EXIF_STRUCTURE0_SIZE must be even

	//----- Load EXIF Header from SRAM buffer *pSramHeadStart to DRAM *pDestDramHdrAddr -----
	pSramHeadStart = &Hdr0->SOI.Marker[ 0 ];

	if( TempSramHdrSize <= 1024 ) {
		HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
		HAL_DmaSetSramAddr( 0x0000 );
		HAL_DmaSetDramAddr( *pDestDramHdrAddr );
		HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
		HAL_CpuSramModeSet( 0x00 );

		*pDestDramHdrAddr += ( TempSramHdrSize >> 1 );
	}
	else {
		while( TempSramHdrSize >= 1024 ) {
			HAL_SRAMLow2High( 1024, pSramHeadStart );
			HAL_DmaSetSramAddr( 0x0000 );
			HAL_DmaSetDramAddr( *pDestDramHdrAddr );
			HAL_DmaDo( 1, 0, 1024, 0, 0 );
			HAL_CpuSramModeSet( 0x00 );

			TempSramHdrSize -= 1024;
			pSramHeadStart += 1024;
			*pDestDramHdrAddr += 512;
		}

		if( TempSramHdrSize > 0 ) {
			HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
			HAL_DmaSetSramAddr( 0x0000 );
			HAL_DmaSetDramAddr( *pDestDramHdrAddr );
			HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
			HAL_CpuSramModeSet( 0x00 );

			*pDestDramHdrAddr += ( TempSramHdrSize >> 1 );
		}
	}
}

#if 0
//patch5.1@ada@Audio annotation begin
//-----------------------------------------------------------------------------
//EXIFHDR_Copy_Exif_Header0_Audio
//-----------------------------------------------------------------------------
void EXIFHDR_Copy_Exif_Header0_Audio( struct Exif_Header0_Audio* Hdr0, UINT32* pDestDramHdrAddr ) USING_0
{
	UINT8* pSramHeadStart;
	UINT32 TempSramHdrSize = K_EXIF_STRUCTURE0_AUDIO_SIZE;  //K_EXIF_STRUCTURE0_SIZE must be even

	//----- Load EXIF Header from SRAM buffer *pSramHeadStart to DRAM *pDestDramHdrAddr -----
	pSramHeadStart = &Hdr0->SOI.Marker[ 0 ];

	if( TempSramHdrSize <= 1024 ) {
		HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
		HAL_DmaSetSramAddr( 0x0c00 );
		HAL_DmaSetDramAddr( *pDestDramHdrAddr );
		HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
		HAL_CpuSramModeSet( 0x00 );

		*pDestDramHdrAddr += ( TempSramHdrSize >> 1 );
	}
	else {
		while( TempSramHdrSize >= 1024 ) {
			HAL_SRAMLow2High( 1024, pSramHeadStart );
			HAL_DmaSetSramAddr( 0x0c00 );
			HAL_DmaSetDramAddr( *pDestDramHdrAddr );
			HAL_DmaDo( 1, 0, 1024, 0, 0 );
			HAL_CpuSramModeSet( 0x00 );

			TempSramHdrSize -= 1024;
			pSramHeadStart += 1024;
			*pDestDramHdrAddr += 512;
		}

		if( TempSramHdrSize > 0 ) {
			HAL_SRAMLow2High( TempSramHdrSize, pSramHeadStart );
			HAL_DmaSetSramAddr( 0x0c00 );
			HAL_DmaSetDramAddr( *pDestDramHdrAddr );
			HAL_DmaDo( 1, 0, TempSramHdrSize, 0, 0 );
			HAL_CpuSramModeSet( 0x00 );

			*pDestDramHdrAddr += ( TempSramHdrSize >> 1 );
		}
	}
	//printf("*pDestDramHdrAddr = %lx after load header0\n", *pDestDramHdrAddr);

}
//patch5.1@ada@Audio annotation end
#endif

//-----------------------------------------------------------------------------
//EXIFHDR_Copy_Exif_Header2
//-----------------------------------------------------------------------------
static void EXIFHDR_Copy_Exif_Header2( struct Exif_Header2* Hdr1, UINT32* pDestDramHdrAddr, UINT32 tmbSize ) USING_0
{
	UINT8* pSramHeadStart;
	UINT32 TmpTmbSize = ( tmbSize + 1 );

	//patch5.0@richie@sdram mapping begin
	UINT32 TmpSrcTmbAddr = exifThbVlcAddr;
	//patch5.0@richie@sdram mapping end

	UINT32 TmpDestTmbAddr = *pDestDramHdrAddr;

	//----- Copy Thumbnail from G_TmbVLCAddr to *pDestDramHdrAddr -----
	////printf("Thumbnail start address in SDRAM = %lx\n",*pDestDramHdrAddr);

	if( TmpTmbSize <= 1024 ) {
		HAL_GprmDramDma( ( TmpSrcTmbAddr * 2 ), ( TmpDestTmbAddr * 2 ), TmpTmbSize, 1 );
	}
	else {
		while( TmpTmbSize >= 1024 ) {
			HAL_GprmDramDma( ( TmpSrcTmbAddr * 2 ), ( TmpDestTmbAddr * 2 ), 1024, 1 );
			TmpTmbSize -= 1024;
			TmpSrcTmbAddr += 512;
			TmpDestTmbAddr += 512;
		}

		if( TmpTmbSize > 0 ) {
			HAL_GprmDramDma( ( TmpSrcTmbAddr * 2 ), ( TmpDestTmbAddr * 2 ), TmpTmbSize, 1 );
		}
	}

	*pDestDramHdrAddr += ( tmbSize >> 1 );

	pSramHeadStart = &Hdr1->EVEN_COM.Marker[ 0 ];

	HAL_SRAMLow2High( K_EXIF_STRUCTURE2_SIZE, pSramHeadStart );

	/* for debug, exif broken */
	//G_ucStorData[3]=0x00;
	G_ucStorData[4]=READ8(snapJpegVlcAddr,3);
	G_ucStorData[5]=READ8(snapJpegVlcAddr,2);
	G_ucStorData[6]=READ8(snapJpegVlcAddr,1);
	G_ucStorData[7]=READ8(snapJpegVlcAddr,0);
	
	G_ucStorData[8]=READ8(snapMainVlcSize,3);
	G_ucStorData[9]=READ8(snapMainVlcSize,2);
	G_ucStorData[10]=READ8(snapMainVlcSize,1);
	G_ucStorData[11]=READ8(snapMainVlcSize,0);

	//G_ucStorData[12]=READ8(snapMainVlcSize,3);
	//G_ucStorData[13]=READ8(snapMainVlcSize,2);
	//G_ucStorData[14]=READ8(snapMainVlcSize,1);
	//G_ucStorData[15]=READ8(snapMainVlcSize,0);
	
	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pDestDramHdrAddr );
	HAL_DmaDo( 1, 0, K_EXIF_STRUCTURE2_SIZE, 0, 0 );
	HAL_CpuSramModeSet( 0x00 );

	*pDestDramHdrAddr += ( K_EXIF_STRUCTURE2_SIZE >> 1 );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Copy_Exif_Header3
//-----------------------------------------------------------------------------
static void EXIFHDR_Copy_Exif_Header3( struct Exif_Header3* Hdr2, UINT32* pDestDramHdrAddr, UINT32 tmbSize ) USING_0
{
	UINT8* pSramHeadStart;
	UINT8 t1, t2;
	UINT32 TmpTmbSize = tmbSize;

	//patch5.0@richie@sdram mapping begin
	UINT32 TmpSrcTmbAddr = exifThbVlcAddr;
	UINT32 TmpDestTmbAddr = *pDestDramHdrAddr;
	//patch5.0@richie@sdram mapping end

	//Copy Thumbnail from G_TmbVLCAddr to *pHdrAddr -----
	////printf("Thumbnail start address in SDRAM = %lx\n",*pDestDramHdrAddr);


	if( TmpTmbSize <= 1024 ) {
		HAL_GprmDramDma( ( TmpSrcTmbAddr * 2 ), ( TmpDestTmbAddr * 2 ), TmpTmbSize, 1 );
	}
	else {
		while( TmpTmbSize >= 1024 ) {
			HAL_GprmDramDma( ( TmpSrcTmbAddr * 2 ), ( TmpDestTmbAddr * 2 ), 1024, 1 );
			TmpTmbSize -= 1024;
			TmpSrcTmbAddr += 512;
			TmpDestTmbAddr += 512;
		}

		if( TmpTmbSize > 0 ) {
			HAL_GprmDramDma( ( TmpSrcTmbAddr * 2 ), ( TmpDestTmbAddr * 2 ), TmpTmbSize, 1 );
		}
	}

	//Load EXIF Header from SRAM buffer pHeadstart to DRAM *pHdrAddr -----
	*pDestDramHdrAddr += ( ( tmbSize - 1 ) >> 1 );

	//set SDRAM buffer start address
	HAL_DramStartAddrSet( *pDestDramHdrAddr, K_SDRAM_Prefetch );
	HAL_DramWordDataRead( &t1, &t2 );
	t2 = Hdr2->ODD_COM.Marker[ 0 ];
	HAL_DramStartAddrSet( *pDestDramHdrAddr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite( t1, t2 );

	*pDestDramHdrAddr += 1;

	pSramHeadStart = &Hdr2->ODD_COM.Marker[ 1 ];

	HAL_SRAMLow2High( ( K_EXIF_STRUCTURE3_SIZE - 1 ), pSramHeadStart );

	//G_ucStorData[3]=0x00;
	G_ucStorData[4]=READ8(snapJpegVlcAddr,3);
	G_ucStorData[5]=READ8(snapJpegVlcAddr,2);
	G_ucStorData[6]=READ8(snapJpegVlcAddr,1);
	G_ucStorData[7]=READ8(snapJpegVlcAddr,0);
	
	G_ucStorData[8]=READ8(snapMainVlcSize,3);
	G_ucStorData[9]=READ8(snapMainVlcSize,2);
	G_ucStorData[10]=READ8(snapMainVlcSize,1);
	G_ucStorData[11]=READ8(snapMainVlcSize,0);

	//G_ucStorData[12]=READ8(snapMainVlcSize,3);
	//G_ucStorData[13]=READ8(snapMainVlcSize,2);
	//G_ucStorData[14]=READ8(snapMainVlcSize,1);
	//G_ucStorData[15]=READ8(snapMainVlcSize,0);
	
	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pDestDramHdrAddr );


	
	HAL_DmaDo( 1, 0, ( K_EXIF_STRUCTURE3_SIZE - 1 ), 0, 0 );
	HAL_CpuSramModeSet( 0x00 );

	*pDestDramHdrAddr += ( ( K_EXIF_STRUCTURE3_SIZE - 1 ) >> 1 );
}

//-----------------------------------------------------------------------------
//EXIFHDR_Copy_Exif_Header4
//-----------------------------------------------------------------------------
void EXIFHDR_Copy_Exif_Header4( struct Exif_Header4* Hdr3, UINT32* pDestDramHdrAddr ) USING_0
{
	UINT8* pSramHeadStart;
	UINT8 t1, t2;

	//Copy the first byte of header1 to connect to the thumbnail -----
	//set SDRAM buffer start address
	HAL_DramStartAddrSet( *pDestDramHdrAddr, K_SDRAM_Prefetch );
	HAL_DramWordDataRead( &t1, &t2 );
	t2 = Hdr3->DQT.Marker[ 0 ];
	HAL_DramStartAddrSet( *pDestDramHdrAddr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite( t1, t2 );

	//Load EXIF Header3 from SRAM buffer pSramHeadStart to DRAM *pDestDramHdrAddr -----
	*pDestDramHdrAddr += 1;
	pSramHeadStart = &Hdr3->DQT.Marker[ 1 ];

	HAL_SRAMLow2High( ( K_EXIF_STRUCTURE4_SIZE - 1 ), pSramHeadStart );
	HAL_DmaSetSramAddr( 0x0000 );
	HAL_DmaSetDramAddr( *pDestDramHdrAddr );
	HAL_DmaDo( 1, 0, ( K_EXIF_STRUCTURE4_SIZE - 1 ), 0, 0 );
	HAL_CpuSramModeSet( 0x00 );

	*pDestDramHdrAddr += ( ( K_EXIF_STRUCTURE4_SIZE - 1 ) >> 1 );
}

//-----------------------------------------------------------------------------
//EXIFHDR_NeedSwitchUSHORTOrder
//-----------------------------------------------------------------------------
UINT16 EXIFHDR_NeedSwitchUSHORTOrder( UINT16 byteorder, UINT16 value ) USING_0
{
	if( byteorder == 0x4949 ) {
		return EXIFHDR_ByteSwapOfUSHORT( value );
	}
	else {
		return value;
	}
}

//-----------------------------------------------------------------------------
//EXIFHDR_NeedSwitchULONGOrder
//-----------------------------------------------------------------------------
UINT32 EXIFHDR_NeedSwitchULONGOrder( UINT16 byteorder, UINT32 value ) USING_0
{
	if( byteorder == 0x4949 ) {
		return EXIFHDR_ByteSwapOfULONG( value );
	}
	else {
		return value;
	}
}

//-----------------------------------------------------------------------------
// host function
//-----------------------------------------------------------------------------
UINT8
EXIFHDR_Parsing_ExifFile(
	UINT32 DramAddr,
	BIT ShowThumb,
	UINT8* DateTime,
	struct Exif_Decompress_Parameter* ImagePara,
	struct Exif_Decompress_Parameter* ThumbPara
)
{
	return exifParsingFile( DramAddr, vfsFileSizeGet(0), ShowThumb, DateTime, ImagePara, ThumbPara );
}


//xian ++ 20081027
typedef struct{
    UINT16 tag;
    UINT8  maxBytes; //shuold < 256
} ifdTagInfo_t;

#define INVALID_TAG 0xffff
//the follow tag can be modified
code ifdTagInfo_t zeroIfdModifyTagList[] = {
	{IFD0_TAG_IMGDESCRIPTION,	11},
    {IFD0_TAG_MAKE,     11},
    {IFD0_TAG_MODEL,    11},
    {IFD0_TAG_DATETIME, 20},
    {INVALID_TAG,       0}
};
code ifdTagInfo_t exifIfdModifyTagList[] = {
    {EXIF_TAG_FNUMBER,      8},    
    {EXIF_TAG_MAXAPERTURE,  8},    
    {EXIF_TAG_LIGHTSOURCE, 0}, 
    {EXIF_TAG_FLASH,		0},
    {EXIF_TAG_FOCALLENGTH,	8},
    {EXIF_TAG_WHITEBALANCE, 0},
    {EXIF_TAG_EXPOSURETIME, 8},
    {EXIF_TAG_ISOSPEEDRATINGS, 0},
    {INVALID_TAG,           0}
};
	
typedef struct{
    struct IFD  IFD0_ImgDescriptionTag;
    UINT8       IFD0_ImgDescriptionValue[ 11 ];	
    struct IFD  IFD0_MakeTag;
    UINT8       IFD0_MakeValue[ 11 ];
    struct IFD  IFD0_ModelTag;
    UINT8       IFD0_ModelValue[ 11 ];
    struct IFD  IFD0_DateTimeTag;
    UINT8       IFD0_DateTimeValue[20];
} zeroIfdTagValBuf_t;
typedef struct{
    struct IFD  Exif_FNumberTag;
    UINT8       Exif_FNumberValue[8];
    struct IFD  Exif_MaxApertureTag;
    UINT8       Exif_MaxApertureValue[8];
     struct IFD  Exif_LightSourceTag;  
    struct IFD  Exif_FlashTag;
    //UINT8       Exif_FlashValue[0];
    struct IFD  Exif_FocalLengthTag;
    UINT8       Exif_FocalLengthValue[8];
    struct IFD  Exif_WhiteBalanceTag;
    //UINT8       Exif_WhiteBalanceValue[0];
    
    struct IFD  Exif_ExposureTimeTag;
    UINT8       Exif_ExposureTimeValue[8];
	struct IFD  Exif_ISOSpeedRatingsTag;
    //UINT8       Exif_ISOSpeedRatingsVaue[0];
       
} exifIfdTagValBuf_t;

zeroIfdTagValBuf_t g_zeroTagValList={0};
exifIfdTagValBuf_t g_exifTagValList={0};
//-----------------------------------------------------------------------------
//Get data type size 
//-----------------------------------------------------------------------------
static UINT8 ifdDataTypeSizeGet(UINT16 idfDataType)
{
    UINT8 sz = 0;
    
    switch(idfDataType){
        case IFD_DATATYPE_BYTE:            
        case IFD_DATATYPE_ASCII:
        case IFD_DATATYPE_UNDEFINED:
            sz = 1;
            break;

        case IFD_DATATYPE_SHORT:
            sz = 2;
            break;
            
        case IFD_DATATYPE_SLONG:
        case IFD_DATATYPE_LONG:
            sz = 4;
            break;
            
        case IFD_DATATYPE_RATIONAL:
        case IFD_DATATYPE_SRATIONAL:
            sz = 8;
            break;
    }
    return sz;
}

//-----------------------------------------------------------------------------
//swap byte order of *pvalue
//-----------------------------------------------------------------------------
static void ifdValueByteSwap(UINT8 typeSz, UINT32 ifdCnt,UINT8 *pvalue)
{
    UINT32 cnt;
    UINT16 *p16 = (UINT16*)pvalue;
    UINT32 *p32 = (UINT32*)pvalue;

    if(!pvalue)
        return;
    
    switch(typeSz){
        case 2:
            for(cnt = 0; cnt < ifdCnt; cnt++){
                *p16 = EXIFHDR_ByteSwapOfUSHORT(*p16);
                //printf("*p16:%x\n", *p16);
                p16 ++;
            }
            break;

        case 8:
            ifdCnt += ifdCnt;
        case 4:
            for(cnt = 0; cnt < ifdCnt; cnt++){
                *p32 = EXIFHDR_ByteSwapOfULONG(*p32);
                //printf("*p32:%lx\n", *p32);
                p32 ++;
            }
            
            break;
    }
}

//-----------------------------------------------------------------------------
//Get data bytes of a tag 
//return 0xffffffff: invalid
//comment: the byteorder of ifdcnt is defined by exif header!!!!
//-----------------------------------------------------------------------------
static UINT32 
ifdValBytesGet(
    UINT16 idfDataType, 
    UINT32 ifdcnt
)
{
    UINT32 valBytes = EXIFHDR_ByteSwapOfULONG(ifdcnt);
        
    switch(idfDataType){
        case IFD_DATATYPE_BYTE:            
        case IFD_DATATYPE_ASCII:
        case IFD_DATATYPE_UNDEFINED:
            break;

        case IFD_DATATYPE_SHORT:
            valBytes <<= 1;
            break;
            
        case IFD_DATATYPE_SLONG:
        case IFD_DATATYPE_LONG:
            valBytes <<= 2;
            
            break;
        case IFD_DATATYPE_RATIONAL:
        case IFD_DATATYPE_SRATIONAL:
            valBytes <<= 3;
            break;
            
        default:
            valBytes = 0xffffffff; //error
    }
    //printf("bytes:%lu\n", valBytes);
    return valBytes;
}

//-----------------------------------------------------------------------------
//Search tagId in the TagList and get the offset of data in valList 
//return 0 : not found, 1~.. idx in list
//return *pIfdOff :  IFD offset   
//-----------------------------------------------------------------------------
static UINT8 
ifdTagListSrch(
    UINT16 tagId, 
    ifdTagInfo_t *pList, 
    UINT32 *pIfdOff
)
{
    UINT8 find = 1;
    UINT32 offset = 0;

    while(pList->tag != INVALID_TAG && pList->tag != tagId){
        find ++;
        offset += (UINT32)pList->maxBytes + sizeof(struct IFD);
        pList ++;
    }
    *pIfdOff = offset;
    if(pList->tag == INVALID_TAG){//not found
        find = 0;
    }

    //printf("tag=%u, find=%bu, offset=%lu\n", tagId, find, offset);
    return find;
}

//-----------------------------------------------------------------------------
//Search tagId in IFD of exif header to get data offset
//return 0 : not found, 1~.. idx in IFD Header
//return *pTagValOff :  IFD offset   
//-----------------------------------------------------------------------------
static UINT8 
ifdExifHdrSrch(
	UINT16 tagId,
	struct IFD *pIfd,
	UINT8 ifdTotal
)
{
    UINT8 idx;

    for(idx = 1; idx <= ifdTotal; idx++){
        if(pIfd->tag == tagId){
            break;
        }
        pIfd ++;
    }    
    if(idx > ifdTotal){//not found
        idx = 0;
    }
    
    //printf("Header: tag=%x, idx=%bu\n", tagId, idx);
    return idx;
}
    
//-----------------------------------------------------------------------------
//Save Tag to valList specified by pDstValList
//return SUCCESS/FAIL
//comment : byteorder is same as exif defined!!
//-----------------------------------------------------------------------------
static UINT8
ifdTagValueSave(
	UINT16 tagId,
	UINT16 idfDataType,
	UINT32 ifdCnt,
	UINT32 valueOffset,
	UINT8 *pvalue,
	ifdTagInfo_t *pTagList,
	UINT8 *pDstValList
)
{
    struct IFD *pDstIFD;
    UINT8 *pDstVal;
    UINT32 bytes, i;
    UINT8 find;
    UINT32 offset;

    //search
    find = ifdTagListSrch(tagId, pTagList, &offset); 

    //save
    if(find){        
        pDstIFD = (struct IFD *)(pDstValList + offset);
        pDstVal = (UINT8*)(pDstIFD + 1);

        pDstIFD->tag = tagId;
        pDstIFD->type = idfDataType;
        pDstIFD->count = ifdCnt;
        pDstIFD->valueoffset = valueOffset;

        pTagList = pTagList + find - 1;        
        bytes = ifdValBytesGet(idfDataType, ifdCnt);

        if(pvalue && pTagList->maxBytes && bytes > 4 && bytes <= pTagList->maxBytes){//maxBytes == 0 means data is in the dataoffset
            for(i = 0; i < bytes; i ++)
                pDstVal[i] = pvalue[i];
        }
        else if(pvalue || pTagList->maxBytes || bytes >= 4){
            pDstIFD->tag = INVALID_TAG; //clean tag
            printf("tag err\n");
            return FAIL;
        }

        return SUCCESS;
    }
    else{
        printf("unsupported tag:%x\n", tagId);
        return FAIL;
    }
}

//-----------------------------------------------------------------------------
//Save Tag to valList specified by ifdType
//param: ifdType - specified as IFD0, EXIF IFD, IFD1 or Interoperability IFD 
//       tagId   - specify IFD's tag id
//       idfDataType - specify IFD's data type as IFD_DATATYPE_BYTE ~ IFD_DATATYPE_SRATIONAL
//       ifdCnt  - specify IFD's value count 
//       valueOffset - specify IFD's new value, it is useless as offset.
//       pvalue  - specify IFD's value
//       bigEndian - specify the byteorder of ifdCnt, valueOffset and pvalue. 1:big endian, 0:little endian
//return SUCCESS/FAIL
//-----------------------------------------------------------------------------
UINT8
EXIFTagValueSet(
	UINT8 ifdType,
	UINT16 tagId,
	UINT16 idfDataType,
	UINT32 ifdCnt,
	UINT32 valueOffset,
	UINT8 *pvalue,
	UINT8 bigEndian 
)
{
    void * pValList = NULL;
    ifdTagInfo_t * pTagList = NULL;
    UINT8 ret = FAIL;
    UINT8 typeSz;
        
    switch(ifdType){
        case IFD_TYPE_ZEROIFD:
            pValList = (void*)&g_zeroTagValList;
            pTagList = &zeroIfdModifyTagList;
            break;
        case IFD_TYPE_EXIFIFD:
            pValList = (void*)&g_exifTagValList;
            pTagList = &exifIfdModifyTagList;
            break;
        //case IFD_TYPE_FIRSTIFD:
            //break;
        //case IFD_TYPE_ITOPIFD:
            //break;
    }

    //1528 is big endian, but exif is little endian
    if(bigEndian){//convert to exif byte order
        if(pvalue){
            typeSz = ifdDataTypeSizeGet(idfDataType);
            ifdValueByteSwap(typeSz, ifdCnt, pvalue);
        }

        ifdCnt = EXIFHDR_ByteSwapOfULONG(ifdCnt);
        valueOffset = EXIFHDR_ByteSwapOfULONG(valueOffset);
        printf("cnt:0x%lx, %lx\n", ifdCnt, valueOffset);
    }
    
    if(pValList && pTagList){ 
        ret = ifdTagValueSave(tagId, idfDataType, ifdCnt, valueOffset,
            pvalue, pTagList, pValList);
    }
    
    return ret;
}

//-----------------------------------------------------------------------------
//Search Value List and update IFD in the exif header
//param: pTagList - specify Tag List, 
//       pSrcValList - specify Value List to be updated
//       pHeader0 - specify exif header
//       pDstIfd  - specify IFD (IFD0, EXIF IFD...) 
//       ifdTotal - ifd total to be searched
//return none
//-----------------------------------------------------------------------------
static void
ifdTagValueModify(
	ifdTagInfo_t *pTagList,
	UINT8 *pSrcValList,
    struct Exif_Header0 *pHeader0,
    struct IFD *pDstIfd,
    UINT8 ifdTotal
)
{
    struct IFD *pSrcIfd;
    UINT8 *pSrcVal;

    struct IFD *pCurIfd;
    UINT8 *pCurVal;
    UINT8 dstFind;
    UINT8 remain; //search
    
    UINT8 i;
    UINT8 bytes;

    remain = ifdTotal;
    pCurIfd = pDstIfd;
    pSrcIfd = (struct IFD *)pSrcValList;
    while(pTagList->tag != INVALID_TAG){
        pSrcVal = (UINT8*)(pSrcIfd + 1);
        if(pTagList->tag == pSrcIfd->tag){//find tag to modify
            //search header            
            dstFind = ifdExifHdrSrch(pTagList->tag, pCurIfd, remain);
            if(!dstFind){//search from beginning
                remain = ifdTotal;
                pCurIfd = pDstIfd;
                dstFind = ifdExifHdrSrch(pTagList->tag, pCurIfd, remain);
            }
            if(dstFind){//only modify dataOffset or data buffer
                pCurIfd = pCurIfd + dstFind - 1;            
                if(pSrcIfd->type == pCurIfd->type && pCurIfd->count == pSrcIfd->count){//dst found
                    bytes = ifdValBytesGet(pSrcIfd->type, pSrcIfd->count);                
                    if(bytes > 4 && bytes <= pTagList->maxBytes){
                        pCurVal = (UINT8*)pHeader0 + EXIFHDR_ByteSwapOfULONG(pCurIfd->valueoffset) + 12;
                        for(i = 0; i < bytes; i++)
                            pCurVal[i] = pSrcVal[i];
                    }
                    else{
                        pCurIfd->valueoffset = pSrcIfd->valueoffset;
                    }
                }

                pCurIfd += 1;
                remain = ifdTotal - dstFind;
            }
                
        }

        pSrcIfd = (struct IFD *)(pSrcVal + pTagList->maxBytes);        
        pTagList ++;
    }
}

//-----------------------------------------------------------------------------
//Modify IFD Tag set by EXIFTagValueSet() 
//-----------------------------------------------------------------------------
static void EXIFHDR_Modify_IfdTag( struct Exif_Header0* pHeader0)
{
    struct IFD *pIfd;
    UINT8 total; //<255 in fact

    //IFD0
    total = EXIFHDR_ByteSwapOfUSHORT(pHeader0->APP1.IFD0_NumberOfInteroperability);
    //printf("total IFD0 : %bu\n", total);
    pIfd = (struct IFD *)((UINT8*)&(pHeader0->APP1.IFD0_NumberOfInteroperability) + 2);//val 2: idf num        
    ifdTagValueModify(&zeroIfdModifyTagList, (UINT8*)&g_zeroTagValList, pHeader0, pIfd, total);

    //EIXf
    total = EXIFHDR_ByteSwapOfUSHORT(pHeader0->APP1.Exif_IFDNumber);
    //printf("total Exif : %bu\n", total);
    pIfd = (struct IFD *)((UINT8*)&(pHeader0->APP1.Exif_IFDNumber) + 2);//val 2: idf num        
    ifdTagValueModify(&exifIfdModifyTagList, (UINT8*)&g_exifTagValList, pHeader0, pIfd, total);

}

//-----------------------------------------------------------------------------
//Save Tag to valList specified by ifdType
//param: txt - specify comment text 
//           len  - text len, should not exceed Length_Text_ThumbOdd_Comment
//return SUCCESS/FAIL
//-----------------------------------------------------------------------------
void EXIFJPGCommentSet(UINT8 *txt, UINT8 len)
{
	UINT8 j;

	if(len > Length_Text_ThumbOdd_Comment)
		len = Length_Text_ThumbOdd_Comment;
	for( j = 0; j < len; j++ ) {
		*( g_jpgCommentMarkContect + j ) = *( txt + j );
	}
	for( ; j < len; j++ ) {
		*( g_jpgCommentMarkContect + j ) = 0;
	}
}

UINT32 exifQvAvailableSizeGet(
	UINT32 thbSize
)
{
	UINT32 size;

	if( thbSize & 0x00000001 ) {
		size = ( K_EXIF_STRUCTURE0_SIZE + thbSize + K_EXIF_STRUCTURE3_SIZE + K_EXIF_STRUCTURE4_SIZE + K_HDR2_SIZE );
	}
	else {
		size = ( K_EXIF_STRUCTURE0_SIZE + thbSize + K_EXIF_STRUCTURE2_SIZE + K_EXIF_STRUCTURE4_SIZE + K_HDR2_SIZE );
	}

	return ( (UINT32)EXIF_FIX_HDR_SIZE - size );
}

void exifSnapInfoSet(
	snapExifInfo_t snapExif
)
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
		exifThbYSize = SNAP_CDSP_THB_HSIZE;
	}
	else {
		exifMainXSize = snapExif.exifMainHSize;
		exifMainYSize = snapExif.exifMainVSize;

		exifQvXSize = SNAP_CDSP_QV_HSIZE;
		exifQvYSize = SNAP_CDSP_QV_VSIZE;

		exifThbXSize = SNAP_CDSP_THB_HSIZE;
		exifThbYSize = SNAP_CDSP_THB_VSIZE;
	}
	exifRotOpt = 1;
}

#if 0 //test code
//-----------------------------------------------------------------------------
//Show one tag and its value
//-----------------------------------------------------------------------------
void TestShowTagValue(struct IFD *pIfd, UINT8 *pval)
{
    UINT8 bytes, i;

    if(pIfd){
        printf("IFD: %x, %x, %lx, %lx\n", 
            pIfd->tag, pIfd->type, pIfd->count, pIfd->valueoffset);
    }

    if(pval){
        if(IFD_DATATYPE_ASCII==pIfd->type)    
            printf("%s\n", pval);
        else{
            bytes = ifdValBytesGet(pIfd->type, pIfd->count);
            if(bytes > 4){
                for(i = 0; i < bytes; i++)
                    printf("%bx, ", pval[i]);
                printf("\n");
            }
        }
    }
}
//-----------------------------------------------------------------------------
//Show value list
//-----------------------------------------------------------------------------
void TestShowValueList(ifdTagInfo_t *pTagList, void *pValList)
{
    struct IFD *pIfd;
    UINT8 *pVal;
    UINT8 idx;
    UINT32 dataBytes;
    
    pIfd = (struct IFD *)pValList;
    
    idx = 0;
    while(pTagList->tag != INVALID_TAG){
        if(pTagList->tag == pIfd->tag){//found tag
            dataBytes = ifdValBytesGet(pIfd->type, pIfd->count);
            if(dataBytes > 4 && dataBytes <= pTagList->maxBytes){
                pVal = (UINT8*)(pIfd + 1);
                TestShowTagValue(pIfd, pVal);
            }
            else if(!pTagList->maxBytes && dataBytes < 4){
                printf("Tag: 0x%x, 0x%x, 0x%lx, 0x%lx -- no data buf\n", pIfd->tag,
                    pIfd->type, pIfd->count, pIfd->valueoffset);
            }
            else{
                printf("Tag: 0x%x, 0x%x, 0x%lx, 0x%lx -- size error(max:%lx)\n", pIfd->tag,
                    pIfd->type, pIfd->count, pIfd->valueoffset,pTagList->maxBytes);
            }
        }
        else{
            printf("Tag: 0x%x, 0x%x, 0x%lx, 0x%lx-- tag invalid\n", pIfd->tag,
                pIfd->type, pIfd->count, pIfd->valueoffset);
        }

        pIfd = (struct IFD *)((UINT8*)(pIfd + 1) + pTagList->maxBytes);
        pTagList ++;        
    }

}
//-----------------------------------------------------------------------------
//Show IFD0 and exif IFD settings
//-----------------------------------------------------------------------------
void TestExifShow()
{
    TestShowValueList(zeroIfdModifyTagList,(void *)&g_zeroTagValList);
    TestShowValueList(exifIfdModifyTagList,(void *)&g_exifTagValList);
}
//-----------------------------------------------------------------------------
//Show Tag in Exif header
//-----------------------------------------------------------------------------
void TestShowExifHdr(struct Exif_Header0 *pHeader0)
{
    struct IFD *pIfd;
    UINT8 *pVal;
    UINT32 offset;
    UINT8 total;
    UINT8 i;

printf("1-----------\n");
    total = EXIFHDR_ByteSwapOfUSHORT(pHeader0->APP1.IFD0_NumberOfInteroperability);
    printf("total IFD0 : %bu\n", total);
    pIfd = (struct IFD *)((UINT8*)&(pHeader0->APP1.IFD0_NumberOfInteroperability) + 2);//val 2: idf0 num        
    for(i = 0; i < total; i ++, pIfd++){
        offset = EXIFHDR_ByteSwapOfULONG(pIfd->valueoffset);
        printf("offset=%lx\n", offset);
        pVal = (UINT8*)pHeader0 + offset + 12 ; //12: make 4, len 2, exif 6
        TestShowTagValue(pIfd, pVal);
    } 
    
    total = EXIFHDR_ByteSwapOfUSHORT(pHeader0->APP1.Exif_IFDNumber);
    printf("total Exif : %bu\n", total);
    pIfd = (struct IFD *)((UINT8*)&(pHeader0->APP1.Exif_IFDNumber) + 2);//val 2: idf num        
    for(i = 0; i < total; i ++, pIfd++){
        offset = EXIFHDR_ByteSwapOfULONG(pIfd->valueoffset);
        printf("offset=%lx\n", offset);
        pVal = (UINT8*)pHeader0 + offset + 12 ; //12: make 4, len 2, exif 6
        TestShowTagValue(pIfd, pVal);
    } 
printf("2-----------\n\n");
}
#endif
//xian --

UINT8 EXIFHDR_Parsing_APPn( UINT32 DramAddr ) USING_0
{
	UINT8 sts = TRUE;

	//patch4.5.2@ada@Fix coding bug begin
	if( HeaderLength & 0x00000001 )
		//if (HeaderLength & 0x0001)
		//patch4.5.2@ada@Fix coding bug end
	{
		SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 6 );
		if( Extract_Thumbnail == 0 ) {
			APPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 3 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 4 ] ) );
			TotalAPPnLength += ( 2 + APPnLength );
			HeaderLength += ( 2 + ( UINT32 ) APPnLength );
		}
		else {
			ThumbAPPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 3 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 4 ] ) );
			TotalThumbAPPnLength += ( 2 + ThumbAPPnLength );
			HeaderLength += ( 2 + ( UINT32 ) ThumbAPPnLength );
		}
	}
	else//even offset
	{
		SDRAM_Dram2Sram( ExifTmpDramAddr, G_ucDataBuffer, 4 );

		if( Extract_Thumbnail == 0 ) {
			APPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 ] ) );
			TotalAPPnLength += ( 2 + APPnLength );
			HeaderLength += ( 2 + ( UINT32 ) APPnLength );
		}
		else {
			ThumbAPPnLength = ( ( ( ( UINT16 ) G_ucStorData[ 2 ] ) << 8 ) | ( ( UINT16 ) G_ucStorData[ 3 ] ) );
			TotalThumbAPPnLength += ( 2 + ThumbAPPnLength );
			HeaderLength += ( 2 + ( UINT32 ) ThumbAPPnLength );
		}
	}

	return sts;
	DramAddr = DramAddr;
}


//wangjg++

UINT32 exifParamSet(exif_param_t param,UINT32 val,UINT8 isSet)
{
#if 1 /* for debug, exif broken */
	param=param;
	val=val;
	isSet=isSet;
	return 0;	
#else
	code UINT8 xdata* p8[]=
	{
		&exifFileFormat,
		&exifMainQTableIdx,
		&exifQvQTableIdx,
		&exifThbQTableIdx,
		&exifVlcAtOdd,
		&exifQvVlcAtOdd,
		&exifThbVlcAtOdd ,
		&exifFillRotOpt,
		&exifAppN,
	};
	code UINT16 xdata* p16[]=
	{
		&exifMainXSize,
		&exifMainYSize,

		&exifQvXSize,
		&exifQvYSize,

		&exifThbXSize,
		&exifThbYSize
	};
	code UINT32 xdata* p32[]=
	{
		&exifQvVlcAddr,
		&exifThbVlcAddr,

		&exifQvVlcSize,
		&exifThbVlcSize,

		&exifQvYTblAddr,
		&exifQvUVTblAddr,
		&exifThbYTblAddr,
		&exifThbUVTblAddr,
		&exifRotOpt,
	} ;
	UINT8 fmt = (UINT8)param;
	
	if(isSet)
	{
		if(fmt>EXIF_PARA_32)	{
			*(p32[fmt-EXIF_PARA_32]) = val;
		}else if(fmt>EXIF_PARA_16){
			*(p16[fmt-EXIF_PARA_16]) = val;
		}else{
			*(p8[fmt]) = val;
		}
	}else{
		if(fmt>EXIF_PARA_32)	{
			return p32[fmt-EXIF_PARA_32];
		}else if(fmt>EXIF_PARA_16){
			return p32[fmt-EXIF_PARA_16];
		}else{
			return p32[fmt] ;
		}
	}
#endif	

}

//20110809
void EXIFUserCommentSet(UINT8 cmCode[8], UINT8 en)
{
    UINT8 i;
    
    if(en){
        for(i = 0; i < 8; i ++){
            g_exifUserCommentCode[i] = cmCode[i];   
        }
    }
    g_exifUsrCommentEnable = en;
    //printf("-%bu\n", g_exifUsrCommentEnable);
}

//return len
UINT8 EXIFUserCommentGet(UINT8 cmCode[8])
{
    UINT8 len = 0;

    if(g_exifUsrCommentEnable){
        for(; len < 8; len++){
            *cmCode ++ = g_exifUserCommentCode[len];  
        }
    }
    return len;
}

dateStc_t* exifRtcSet(dateStc_t* ptRtc)
{
	if( (UINT16)ptRtc!=0xffff) {
		exifRtcPt = ptRtc;
	}
	return exifRtcPt;
	
}
