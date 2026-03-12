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
 * @file		exif_def.c
 * @brief		exif standard 2.2
 * @author		hy.ren
 * @since		2008-03-14
 * @date		2008-10-31
 */
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/

#ifndef __EXIF_DEF__H__
#define __EXIF_DEF__H__

#include "exif_param.h"

typedef struct ExifItem
{
	UINT16 tag;	
	UINT16 size;
}ExifItem_t;

typedef struct ExifItemExt
{
	ExifItem_t item;
	UINT32 offset;
}ExifItemExt_t;

typedef struct IFD1
{
	UINT16 tag;
	UINT16 type;
	UINT32 count;
	UINT32 valueOffset;
}IFD_t;

typedef struct IFDs
{
	IFD_t ifds;
	UINT16 count; //UINT32 count
	ExifIfd type;	 
	UINT32 offset;  //byte
}IFDs_t;

typedef struct SOI
{
	UINT8 tag[2];
}SOI_t;

typedef struct EOI
{
	UINT8 tag[2];
}EOI_t;

typedef struct DQT
{
	//DQT
	ExifItem_t item;
	//yTbl
	UINT8     PqTqY;
	UINT8     yQt[64];  //Y
	//uvTbl
	UINT8     PqTqUV1;
	UINT8     uvQt[64];  //UV
	/*On SPCA1628 only have two qtable ...*/
	//uvTbl
	//UINT8     PqTqUV2;
	//UINT8     Content2[64];  //UV
}DQT_t;

typedef struct DHT
{
	//DHT
	ExifItem_t item;
	
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
}DHT_t;

typedef struct DRI
{
	ExifItem_t item;
	UINT8 Content[4];
}DRI_t;

typedef struct SOF
{
	ExifItem_t item;
	UINT8 Content[15];
}SOF_t;

typedef struct SOS
{
	ExifItem_t item;
	UINT8 Content[10];
}SOS_t;

typedef struct ExifImage
{
	UINT16 width;
	UINT16 height;
	UINT32 addr;
	UINT32 vlcSize;
	UINT16 orientation;
	UINT8 dataType;

	SOI_t *soi;
	DQT_t *dqt[4];
	DRI_t *dri;
	DHT_t *dht[4];
	SOF_t *sof;
	SOS_t *sos;
}ExifImage_t;

typedef struct ExifQVImage
{
     UINT8     YQTbl[0x40];
     UINT8     UVQTbl[0x40];
     UINT8      NewMarker[2];
     UINT16    Version;
     UINT16    ImgWidth;
     UINT16    ImgHeight;
}ExifQVImage_t;

typedef struct Tiff
{	
	UINT8 ExifString[6];
	UINT16 ByteOrder;
	UINT16 ID;
	UINT32 IFD0Offset; //modified 
}Tiff_t;

typedef struct App0
{
	ExifItem_t item;
	UINT8* pt;//data pointer
	UINT8 JFIF[5];//JFIF[6];
}App0_t;

#define EXIF_APP1_TAG_MAX_COUNT 10
typedef struct App1
{
	ExifItem_t item;//maker & size
	//app1
	UINT8* pt;//data pointer
	UINT8* ptStart;
	
	Tiff_t tiff;

	IFDs_t ifds[20];//ifd group pointer
	UINT8 ifdCount;//ifd group count

	ExifItemExt_t app1[EXIF_APP1_TAG_MAX_COUNT];
	UINT8 app1Count;
	
	ExifImage_t mainImage;
	ExifImage_t thumbImage;
	
}App1_t;

typedef struct App5
{
	ExifItem_t item;
	UINT32 offset;
	ExifQVImage_t qvImage;
}App5_t;

typedef struct JpgComOdd
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     Content[17];
}JpgComOdd_t;

typedef struct JpgComEven
{
     UINT8     Marker[2];
     UINT16    Length;
     UINT8     Content[16];
}JpgComEven_t;

typedef struct Exif
{	
	UINT32 addrByte;
	UINT8 xdata *ptData;//g_ucStore
	UINT32 dataSize;
	
	App0_t app0;
	App1_t app1;
	App5_t app5;
	
	ExifItemExt_t apps[20];
	UINT8 appsCount;
	
}ExifData_t;

#define EXIF_HDR_MAX_SIZE (65536)
#define K_THUMBNAIL_STRUCTURE_SIZE      ((UINT32) (0x02 + 0x02 + 0x84 + 0x02 + 0x01a2 + 0x02 + 0x11 + 0x02 + 0x0c + 0x02))
											//        SOI    DQT           DHT             SOF           SOS           EOI
#define EXIF_APP5_SIZE ((UINT32)(0x40+0x40+0x2+0x2+0x2+0x2))
#define EXIF_THUMBNAIL_TABLE_SIZE sizeof(DQT_t)+sizeof(DHT_t)+ sizeof(SOS_t) + sizeof(SOF_t) + sizeof(SOI_t)
#define EXIF_MAIN_TABLE_SIZE   ((UINT32) (0x02 + 0x84 + 0x02 + 0x01a2 + 0x02 + 0x11 + 0x02 + 0x0c))
								//        SOI    DQT           DHT             SOF           SOS   

/*****************************************************************************************/
//structure EXIF header 0,SOI,thumb SOI,thumb EOI
#define K_HDR0_SIZE   ((UINT32)0x00000002)
#define K_HDR1_SIZE   ((UINT32)0x00000383)	//0x2c9+0xb4
//structure EXIF Header2 for APP4
#define K_HDR2_SIZE   ((UINT32)0x0000008c)
//structure EXIF Header3 for COM(Thumbnail odd) & SOF
#define K_HDR3_SIZE   ((UINT32)0x00000013)
//structure EXIF Header4 for DQT
#define K_HDR4_SIZE   ((UINT32)0x00000086)
//structure EXIF Header5 for DHT(DC)
#define K_HDR5_SIZE   ((UINT32)0x000001a4)
//structure EXIF Header6 for SOS
#define K_HDR6_SIZE   ((UINT32)0x0000000e)
//structure EXIF Header7 for COM(Thumbnail even)
#define K_HDR7_SIZE   ((UINT32)0x00000014)

#define K_EXIF_STRUCTURE0_SIZE     ((UINT32) ((K_HDR0_SIZE * 2) + K_HDR1_SIZE + K_HDR3_SIZE + K_HDR4_SIZE + K_HDR5_SIZE + K_HDR6_SIZE))
#define K_EXIF_STRUCTURE2_SIZE     ((UINT32)K_HDR7_SIZE)   //thumbnail even size
#define K_EXIF_STRUCTURE3_SIZE     ((UINT32)K_HDR3_SIZE)   //thumbnail odd size
#define K_EXIF_STRUCTURE4_SIZE     ((UINT32) (K_HDR3_SIZE + K_HDR4_SIZE + K_HDR5_SIZE + K_HDR6_SIZE))  //thumbnail even size

/****************************************************************************************/


#endif

