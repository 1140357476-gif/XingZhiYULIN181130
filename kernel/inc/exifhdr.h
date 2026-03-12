/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __EXIFHDR_H__
#define __EXIFHDR_H__
//------------------------------------------------------------------------------
//Type and Constant Definition
//------------------------------------------------------------------------------

#include "snap_task.h"

void exifSnapInfoSet(snapExifInfo_t snapExif);


//structure IFD
#define K_IFD_SIZE  ((UINT32)0x0000000c)

struct IFD
{
     UINT16    tag;
     UINT16    type;
     UINT32     count;
     UINT32     valueoffset;
};

//structure EXIF header 0,SOI,thumb SOI,thumb EOI
#define K_HDR0_SIZE   ((UINT32)0x00000002)

struct HDR0
{
     UINT8     Marker[2];
};

//structure EXIF Header1,APP1

//patch5.1@ada@EXIF2.2 upgrade begin
//#define K_HDR1_SIZE   ((UINT32)0x000002c9)
#define K_HDR1_SIZE   ((UINT32)0x00000383)	//0x2c9+0xb4
//patch5.1@ada@EXIF2.2 upgrade end

struct HDR1
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     ExifString[5];
     UINT8     Pad;
     UINT16    Tiff_ByteOrder;
     UINT16    Tiff_ID;
     UINT32     Tiff_IFD0Offset;
     UINT16    IFD0_NumberOfInteroperability;
     struct IFD       IFD0_ImageDescriptionTag;
     struct IFD       IFD0_MakeTag;
     struct IFD       IFD0_ModelTag;
     struct IFD       IFD0_OrientationTag;
     struct IFD       IFD0_XResolutionTag;
     struct IFD       IFD0_YResolutionTag;
     struct IFD       IFD0_ResolutionUnitTag;
     struct IFD       IFD0_DateTimeTag;
     struct IFD       IFD0_YCbCrPositionTag;
     struct IFD       IFD0_ExifIFDPointerTag;
     UINT32     IFD0_NextIFDOffset;
     UINT8     IFD0_ImageDescriptionValue[ 11 ];
     UINT8     IFD0_MakeValue[ 11 ];
     UINT8     IFD0_ModelValue[ 11 ];
     UINT32     IFD0_XResolutionValue[2];
     UINT32     IFD0_YResolutionValue[2];
     UINT8     IFD0_DateTimeValue[20];
     UINT16    Exif_IFDNumber;
     struct IFD       Exif_ExposureTimeTag;
     struct IFD       Exif_FNumberTag;
     struct IFD       Exif_ExposureProgramTag;
     struct IFD       Exif_ISOSpeedRatingsTag;
     struct IFD       Exif_ExifVersionTag;
     struct IFD       Exif_DateTimeOriginalTag;
     struct IFD       Exif_DateTimeDigitilizedTag;
     struct IFD       Exif_ComponentsConfigurationTag;
     struct IFD       Exif_CompressedBitsPerPixelTag;

	 //patch5.1@ada@EXIF2.2 upgrade begin
     struct IFD	      Exif_BrightnessValueTag;
	 //patch5.1@ada@EXIF2.2 upgrade end

     struct IFD       Exif_ExposureBiasTag;
     struct IFD       Exif_MaxApertureTag;
     struct IFD       Exif_MeteringModeTag;
     struct IFD       Exif_LightSourceTag;
     struct IFD       Exif_FlashTag;
     struct IFD       Exif_FocalLengthTag;

	 //patch5.1@ada@EXIF2.2 upgrade begin
     struct IFD	      Exif_SubjectAreaTag;
	 //patch5.1@ada@EXIF2.2 upgrade end

     struct IFD       Exif_FlashPixVersiontTag;
     struct IFD       Exif_ColorSpaceTag;
     struct IFD       Exif_PixelXDimensionTag;
     struct IFD       Exif_PixelYDimensionTag;
     struct IFD       Exif_InteroperabilityIFDPointerTag;
     struct IFD       Exif_FileSourceTag;
     struct IFD       Exif_SceneTypeTag;

	 //patch5.1@ada@EXIF2.2 upgrade begin
     struct IFD	      Exif_CustomRenderedTag;
     struct IFD	      Exif_ExposureModeTag;
     struct IFD	      Exif_WhiteBalanceTag;
     struct IFD	      Exif_DigitalZoomRatioTag;
     struct IFD	      Exif_FocalLengthIn35mmFileTag;
     struct IFD	      Exif_SceneCaptureTypeTag;
     struct IFD	      Exif_GainControlTag;
     struct IFD	      Exif_ContrastTag;
     struct IFD	      Exif_SaturationTag;
     struct IFD	      Exif_SharpnessTag;
     struct IFD	      Exif_SubjectDistanceRangeTag;
	 //patch5.1@ada@EXIF2.2 upgrade end

     UINT32     Exif_NextIFDOffset;
     UINT32     Exif_ExposureTimeValue[2];
     UINT32     Exif_FNumberValue[2];
     UINT8     Exif_DateTimeOriginalValue[20];
     UINT8     Exif_DateTimeDigitilizedValue[20];
     UINT32     Exif_CompressedBitsPerPixelValue[2];

	 //patch5.1@ada@EXIF2.2 upgrade begin
     signed long     Exif_BrightnessValue[2];
	 //patch5.1@ada@EXIF2.2 upgrade end

     UINT32     Exif_ExposureBiasValue[2];
     UINT32     Exif_MaxApertureValue[2];
     UINT32     Exif_FocalLengthValue[2];

	 //patch5.1@ada@EXIF2.2 upgrade begin
     UINT16    Exif_SubjectArea[4];
     UINT32     Exif_DigitalZoomRatio[2];
	 //patch5.1@ada@EXIF2.2 upgrade end

     UINT16    NumberOfInteroperability;
     struct IFD       InteroperabilityIndexTag;
     struct IFD       InteroperabilityVersionTag;
     UINT32     NextInteroperabilityIFDOffset;

     UINT16    IFD1_NumberOfInteroperability;
     struct IFD       IFD1_CompressionTag;
     struct IFD       IFD1_OrientationTag;
     struct IFD       IFD1_XResolutionTag;
     struct IFD       IFD1_YResolutionTag;
     struct IFD       IFD1_ResolutionUnitTag;
     struct IFD       IFD1_JPEGInterChangeFormatTag;
     struct IFD       IFD1_JPEGInterChangeFormatLengthTag;
     struct IFD       IFD1_YCbCrPositioningTag;
     UINT32     IFD1_NextIFDOffset;
     UINT32     IFD1_XResolutionValue[2];
     UINT32     IFD1_YResolutionValue[2];
};

#if 0
//patch5.1@ada@Audio annotation begin
#define K_HDR1_AUDIO_SIZE   ((UINT32)0x00000397)	//0x37d+12+13+1

struct HDR1_AUDIO
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     ExifString[5];
     UINT8     Pad;
     UINT16    Tiff_ByteOrder;
     UINT16    Tiff_ID;
     UINT32     Tiff_IFD0Offset;
     UINT16    IFD0_NumberOfInteroperability;
     struct IFD       IFD0_ImageDescriptionTag;
     struct IFD       IFD0_MakeTag;
     struct IFD       IFD0_ModelTag;
     struct IFD       IFD0_OrientationTag;
     struct IFD       IFD0_XResolutionTag;
     struct IFD       IFD0_YResolutionTag;
     struct IFD       IFD0_ResolutionUnitTag;
     struct IFD       IFD0_DateTimeTag;
     struct IFD       IFD0_YCbCrPositionTag;
     struct IFD       IFD0_ExifIFDPointerTag;
     UINT32     IFD0_NextIFDOffset;
     UINT8     IFD0_ImageDescriptionValue[9];
     UINT8     IFD0_MakeValue[9];
     UINT8     IFD0_ModelValue[10];	//herb change 9 to 10 to meet size
     UINT32     IFD0_XResolutionValue[2];
     UINT32     IFD0_YResolutionValue[2];
     UINT8     IFD0_DateTimeValue[20];
     UINT16    Exif_IFDNumber;
     struct IFD       Exif_ExposureTimeTag;
     struct IFD       Exif_FNumberTag;
     struct IFD       Exif_ExposureProgramTag;
     struct IFD       Exif_ISOSpeedRatingsTag;
     struct IFD       Exif_ExifVersionTag;
     struct IFD       Exif_DateTimeOriginalTag;
     struct IFD       Exif_DateTimeDigitilizedTag;
     struct IFD       Exif_ComponentsConfigurationTag;
     struct IFD       Exif_CompressedBitsPerPixelTag;
     struct IFD	      Exif_BrightnessValueTag;
     struct IFD       Exif_ExposureBiasTag;
     struct IFD       Exif_MaxApertureTag;
     struct IFD       Exif_MeteringModeTag;
     struct IFD       Exif_LightSourceTag;
     struct IFD       Exif_FlashTag;
     struct IFD       Exif_FocalLengthTag;
     struct IFD	      Exif_SubjectAreaTag;
     struct IFD       Exif_FlashPixVersiontTag;
     struct IFD       Exif_ColorSpaceTag;
     struct IFD       Exif_PixelXDimensionTag;
     struct IFD       Exif_PixelYDimensionTag;
     struct IFD	      Exif_RelatedSoundFileTag;	//WAV file index related
     struct IFD       Exif_InteroperabilityIFDPointerTag;
     struct IFD       Exif_FileSourceTag;
     struct IFD       Exif_SceneTypeTag;
     struct IFD	      Exif_CustomRenderedTag;
     struct IFD	      Exif_ExposureModeTag;
     struct IFD	      Exif_WhiteBalanceTag;
     struct IFD	      Exif_DigitalZoomRatioTag;
     struct IFD	      Exif_FocalLengthIn35mmFileTag;
     struct IFD	      Exif_SceneCaptureTypeTag;
     struct IFD	      Exif_GainControlTag;
     struct IFD	      Exif_ContrastTag;
     struct IFD	      Exif_SaturationTag;
     struct IFD	      Exif_SharpnessTag;
     struct IFD	      Exif_SubjectDistanceRangeTag;
     UINT32     Exif_NextIFDOffset;
     UINT32     Exif_ExposureTimeValue[2];
     UINT32     Exif_FNumberValue[2];
     UINT8     Exif_DateTimeOriginalValue[20];
     UINT8     Exif_DateTimeDigitilizedValue[20];
     UINT32     Exif_CompressedBitsPerPixelValue[2];
     signed long     Exif_BrightnessValue[2];
     UINT32     Exif_ExposureBiasValue[2];
     UINT32     Exif_MaxApertureValue[2];
     UINT32     Exif_FocalLengthValue[2];
     UINT16    Exif_SubjectArea[4];
     UINT32     Exif_DigitalZoomRatio[2];
     UINT16    NumberOfInteroperability;
     struct IFD       InteroperabilityIndexTag;
     struct IFD       InteroperabilityVersionTag;
     UINT32     NextInteroperabilityIFDOffset;
     UINT8     Exif_RelatedSoundFileValue[13];	//WAV file related
     UINT16    IFD1_NumberOfInteroperability;
     struct IFD       IFD1_CompressionTag;
     struct IFD       IFD1_OrientationTag;
     struct IFD       IFD1_XResolutionTag;
     struct IFD       IFD1_YResolutionTag;
     struct IFD       IFD1_ResolutionUnitTag;
     struct IFD       IFD1_JPEGInterChangeFormatTag;
     struct IFD       IFD1_JPEGInterChangeFormatLengthTag;
     struct IFD       IFD1_YCbCrPositioningTag;
     UINT32     IFD1_NextIFDOffset;
     UINT32     IFD1_XResolutionValue[2];
     UINT32     IFD1_YResolutionValue[2];
};
//patch5.1@ada@Audio annotation end
#endif

/*Add by phil for app4@07.12.27 s*/

//structure EXIF Header2 for APP4
#define K_HDR2_SIZE   ((UINT32)0x0000008c)

struct HDR2
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     YQTbl[0x40];
     UINT8     UVQTbl[0x40];
     UINT8      NewMarker[2];
     UINT16    Version;
     UINT16    ImgWidth;
     UINT16    ImgHeight;
//     UINT16    Orientation;
};

/*Add by phil for app4@07.12.27 s*/

//structure EXIF Header3 for COM(Thumbnail odd) & SOF
#define K_HDR3_SIZE   ((UINT32)0x00000013)

struct HDR3
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     Content[15];
};

//structure EXIF Header4 for DQT
#define K_HDR4_SIZE   ((UINT32)0x00000086)

struct HDR4
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     PqTq0;
     UINT8     Content0[64];  //Y
     UINT8     PqTq1;
     UINT8     Content1[64];  //UV
};

//structure EXIF Header5 for DHT(DC)
#define K_HDR5_SIZE   ((UINT32)0x000001a4)

struct HDR5
{
     UINT8     Marker[2];
     UINT16    Length;
     //Y
     UINT8     TcTh0;
     UINT8     Content0[28];   //DC
     UINT8     TcTh1;
     UINT8     Content1[178];  //AC
     //UV
     UINT8     TcTh2;
     UINT8     Content2[28];   //DC
     UINT8     TcTh3;
     UINT8     Content3[178];  //AC
};

//structure EXIF Header6 for SOS
#define K_HDR6_SIZE   ((UINT32)0x0000000e)

struct HDR6
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     Content[10];
};

//structure EXIF Header7 for COM(Thumbnail even)
#define K_HDR7_SIZE   ((UINT32)0x00000014)

struct HDR7
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     Content[16];
};

#define K_EXIF_STRUCTURE0_SIZE     ((UINT32) ((K_HDR0_SIZE * 2) + K_HDR1_SIZE + K_HDR3_SIZE + K_HDR4_SIZE + K_HDR5_SIZE + K_HDR6_SIZE))

#if 0
//patch5.1@ada@Audio annotation begin
#define K_EXIF_STRUCTURE0_AUDIO_SIZE     ((UINT32) ((K_HDR0_SIZE * 2) + K_HDR1_AUDIO_SIZE + K_HDR3_SIZE + K_HDR4_SIZE + K_HDR5_SIZE + K_HDR6_SIZE))
//patch5.1@ada@Audio annotation end
#endif

struct Exif_Header0
{
//----- K_STRUCTURE0_SIZE -----
     struct HDR0      SOI;
     struct HDR1      APP1;
     struct HDR0      THUMB_SOI;
     struct HDR4      THUMB_DQT;
     struct HDR5      THUMB_DHT;
     struct HDR3      THUMB_SOF;
     struct HDR6      THUMB_SOS;
};

#if 0
//patch5.1@ada@Audio annotation begin
struct Exif_Header0_Audio
{
     struct HDR0      	SOI;
     struct HDR1_AUDIO  APP1;
     struct HDR0      	THUMB_SOI;
     struct HDR4      	THUMB_DQT;
     struct HDR5      	THUMB_DHT;
     struct HDR3      	THUMB_SOF;
     struct HDR6      	THUMB_SOS;
};
//patch5.1@ada@Audio annotation end
#endif

#define K_EXIF_STRUCTURE1_SIZE		((UINT32))

struct Exif_Header1
{
     struct HDR2 APP5;
};

#define K_EXIF_STRUCTURE2_SIZE     ((UINT32)K_HDR7_SIZE)   //thumbnail even size

struct Exif_Header2
{
//----- K_STRUCTURE1_SIZE -----
     struct HDR7      EVEN_COM;
};

#define K_EXIF_STRUCTURE3_SIZE     ((UINT32)K_HDR3_SIZE)   //thumbnail odd size

struct Exif_Header3
{
//----- K_STRUCTURE2_SIZE -----
     struct HDR3      ODD_COM;
};

#define K_EXIF_STRUCTURE4_SIZE     ((UINT32) (K_HDR3_SIZE + K_HDR4_SIZE + K_HDR5_SIZE + K_HDR6_SIZE))  //thumbnail even size

struct Exif_Header4
{
//----- K_STRUCTURE3_SIZE -----
     struct HDR4      DQT;
     struct HDR5      DHT;
     struct HDR3      SOF;
     struct HDR6      SOS;
};

#define K_TOTAL_EXIF_STRUCTURE_SIZE     ((UINT32) (K_EXIF_STRUCTURE0_SIZE + K_EXIF_STRUCTURE2_SIZE + K_EXIF_STRUCTURE3_SIZE + K_EXIF_STRUCTURE4_SIZE))

//patch5.1@ada@Audio annotation begin
#define K_TOTAL_EXIF_STRUCTURE_AUDIO_SIZE     ((UINT32) (K_EXIF_STRUCTURE0_AUDIO_SIZE + K_EXIF_STRUCTURE2_SIZE + K_EXIF_STRUCTURE3_SIZE + K_EXIF_STRUCTURE4_SIZE))
//patch5.1@ada@Audio annotation end

#define K_THUMBNAIL_STRUCTURE_SIZE      ((UINT32) (0x02 + 0x02 + 0x84 + 0x02 + 0x01a2 + 0x02 + 0x11 + 0x02 + 0x0c + 0x02))
                                        //        SOI    DQT           DHT             SOF           SOS           EOI

struct Exif_Decompress_Parameter
{
     UINT32     Addr;
     UINT16    Height;
     UINT16    Width;
     UINT32     Size;
     UINT8     DataType;
};

//ada@0509 for EXIF
//#define K_JFIF_IMAGE_FORMAT   (UINT8)1
//#define K_EXIF_IMAGE_FORMAT   (UINT8)2
#define K_JFIF_IMAGE_FORMAT   (UINT8)6
#define K_EXIF_IMAGE_FORMAT   (UINT8)4

//patch3.2@ada@0401 For EXIF
#define K_BIG_ENDIAN          (UINT16)0x4d4d
#define K_LITTLE_ENDIAN       (UINT16)0x4949

#define K_SOI_MARKER          (UINT8)0xd8
#define K_APP0_MARKER         (UINT8)0xe0
#define K_APP1_MARKER         (UINT8)0xe1
#define K_APP2_MARKER         (UINT8)0xe2
#define K_APP3_MARKER         (UINT8)0xe3
#define K_APP4_MARKER         (UINT8)0xe4
#define K_APP5_MARKER         (UINT8)0xe5
#define K_APP6_MARKER         (UINT8)0xe6
#define K_APP7_MARKER         (UINT8)0xe7
#define K_APP8_MARKER         (UINT8)0xe8
#define K_APP9_MARKER         (UINT8)0xe9
#define K_APP10_MARKER        (UINT8)0xea
#define K_APP11_MARKER        (UINT8)0xeb
#define K_APP12_MARKER        (UINT8)0xec
#define K_APP13_MARKER        (UINT8)0xed
#define K_APP14_MARKER        (UINT8)0xee
#define K_APP15_MARKER        (UINT8)0xef
#define K_DQT_MARKER          (UINT8)0xdb
#define K_DHT_MARKER          (UINT8)0xc4
#define K_SOF_MARKER          (UINT8)0xc0
#define K_SOS_MARKER          (UINT8)0xda
#define K_EOI_MARKER          (UINT8)0xd9
#define K_COM_MARKER          (UINT8)0xfe
#define K_DRI_MARKER          (UINT8)0xdd

//xian ++ 20081027, for modify ifd
typedef enum {
     IFD_TYPE_ZEROIFD = 1,
     IFD_TYPE_EXIFIFD,
     IFD_TYPE_FIRSTIFD,
     IFD_TYPE_ITOPIFD
}idfTypeID_e;

#define IFD_DATATYPE_BYTE   0x0100
#define IFD_DATATYPE_ASCII  0x0200
#define IFD_DATATYPE_SHORT  0x0300
#define IFD_DATATYPE_LONG   0x0400
#define IFD_DATATYPE_RATIONAL   0x0500
#define IFD_DATATYPE_UNDEFINED  0x0700
#define IFD_DATATYPE_SLONG      0x0900
#define IFD_DATATYPE_SRATIONAL  0x0a00

#define IFD0_TAG_IMGDESCRIPTION		0x0e01
#define IFD0_TAG_MAKE       0x0F01
#define IFD0_TAG_MODEL      0x1001
#define IFD0_TAG_DATETIME   0x3201

#define EXIF_TAG_FNUMBER        0x9d82
#define EXIF_TAG_MAXAPERTURE    0x0592
#define EXIF_TAG_LIGHTSOURCE     0x0892 
#define EXIF_TAG_FLASH			0x0992
#define EXIF_TAG_FOCALLENGTH	0x0a92
#define EXIF_TAG_WHITEBALANCE	0x03a4

#define EXIF_TAG_EXPOSURETIME       0x9a82
#define EXIF_TAG_ISOSPEEDRATINGS    0x2788
//xian --



#endif /*__EXIFHDR_H__*/
