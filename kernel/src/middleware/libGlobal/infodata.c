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

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#include "infodata.h"
//=============================================================================
//Symbol
//=============================================================================
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#if 0
code UINT8 ProductDescriptor1[] =
{
    //-------------------------------------------------------------------
    //TotalLength = 0x0040 byte
    //-------------------------------------------------------------------

        /*Product (0x40 byte)*/
        0x40,                   /*bLength: 0x40 byte*/
        0x01,                   /*bDescriptorType: Sensor information*/
        0x02,                   /*1:CCD,  2:CMOS*/
        0x04,                   /*1:CIF,  2:VGA,  3:800k,  4:1.0M,  5:1.3M*/
                                /*6:2.0M, 7:3.0M, 8:4.0M*/

                                /*8 Byte  :
                                //byte 1~7 :DSC size index
                                //byte 8   :DSC default Size index
                                /*/
        0x08,                   /*byte1:dsc size index 8 = 1152 x 864*/
        0x00,                   /*byte2:               0 = no use*/    
        0x00,                   /*byte3:               0 = no use*/     
        0x00,                   /*byte4:               0 = no use*/
        0x00,                   /*byte5:               0 = no use*/
        0x00,                   /*byte6:               0 = no use*/
        0x00,                   /*byte7:               0 = no use*/
        0x01,                   /*byte8:dsc size default size : 1 (1152 x 864)*/

                                /*8 Byte  :
                                //byte 1~7 :Video clip size index
                                //byte 8   :Video Clip default Size index
                                /*/
        0x04,                   /*byte1:clip size index 4 = 320 x 240*/
        0x00,                   /*byte2:                0 = no use*/   
        0x00,                   /*byte3:                0 = no use*/     
        0x00,                   /*byte4:                0 = no use*/
        0x00,                   /*byte5:                0 = no use*/
        0x00,                   /*byte6:                0 = no use*/
        0x00,                   /*byte7:                0 = no use*/
        0x01,                   /*byte8:clip size default size : 1 (320 x 240)*/
        
                                /*8 Byte  :
                                //byte 1~7 :pc camera size index
                                //byte 8   :pc camera default Size index
                                /*/
        0x0A,                   /*byte1:pc camera size index 10 = 640 x 480*/
        0x05,                   /*byte2:                     5 = 320 x 240*/ 
        0x05,                   /*byte3:                     5 = 320 x 240*/
        0x00,                   /*byte4:                     0 = no use*/
        0x00,                   /*byte5:                     0 = no use*/
        0x00,                   /*byte6:                     0 = no use*/
        0x00,                   /*byte7:                     0 = no use*/
        0x01,                   /*byte8:pc camera size default size : 1 (320 x 240)*/

    //sensor name description: 20 Byte
    'O','m','n','i','v','i','s','i','o','n',' ','3','.','1','M',' ',  0,  0,  0,  0,

    //idvendor description: 16 Byte
    'S','u','n','p','l','u','s',' ','C','A','1','5','2','8',  0,  0,};

code UINT8 PcCameraSizeMapping1[] =
{
		/*-------------------------------------------------------------------
		//TotalLength = 0x0028 byte
		//-------------------------------------------------------------------*/

		/*Product (0x28 byte)*/
		0x28,                   /*bLength: 0x28 byte*/
		0x03,                   /*bDescriptorType:  DscSizeInformation*/
		0x00,                   /*reserve*/
		0x00,                   /*reserve*/

		0x00,                   /* 160   : width     , DSCSizeIndex = 1*/
		0xa0,                   /**/
		0x00,                   /* 128   : height*/
		0x80,

		0x00,                   /* 160   : width     , DSCSizeIndex = 2*/
		0xa0,                   /**/
		0x00,                   /* 160   : height*/
		0xa0,

		0x00,                   /* 160               , DSCSizeIndex = 3*/
		0xa0,                   /**/
		0x00,                   /* 224*/
		0xe0,

		0x00,                   /* 160               , DSCSizeIndex = 4*/
		0xa0,                   /**/
		0x01,                   /* 400*/
		0x90,

		0x01,                   /* 320               , DSCSizeIndex = 5*/
		0x40,                   /**/
		0x00,                   /* 240*/
		0xF0,

		0x01,                   /* 464               , DSCSizeIndex = 6*/
		0xd0,                   /**/
		0x01,                   /* 336*/
		0x50,

		0x01,                   /* 464               , DSCSizeIndex = 7*/
		0xd0,                   /**/
		0x01,                   /* 352*/
		0x60,

		0x01,                   /* 464               , DSCSizeIndex = 8*/
		0xd0,                   /**/
		0x01,                   /* 432*/
		0xb0,

		0x01,                   /* 464               , DSCSizeIndex = 9*/
		0xd0,                   /**/
		0x01,                   /* 480*/
		0xe0,

		0x02,                   /* 640               , DSCSizeIndex = 10*/
		0x80,                   /**/
		0x01,                   /* 480*/
		0xe0,

		0x01,                   /* 320               , DSCSizeIndex = 11*/
		0x40,                   /**/
		0x00,                   /* 240*/
		0xf0,

		0x01,                   /* 464               , DSCSizeIndex = 12*/
		0xd0,                   /**/
		0x01,                   /* 336*/
		0x50,

		0x01,                   /* 464               , DSCSizeIndex = 13*/
		0xd0,                   /**/
		0x01,                   /* 352*/
		0x60,

		0x01,                   /* 464               , DSCSizeIndex = 14*/
		0xd0,                   /**/
		0x01,                   /* 432*/
		0xb0,

		0x01,                   /* 464               , DSCSizeIndex = 15*/
		0xd0,                   /**/
		0x01,                   /* 480*/
		0xe0
};
#endif
