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

#ifndef __DPOFCON_H__
#define __DPOFCON_H__

//#include "main.h"
#if (APP_DPOF_INIT)	/*Add by Phil@07.10.02*/
#define K_FIRMWARE_RELEASE_VERSION      ((UINT32)0x00040004)

//------------------------------------------------------------------------------
//Type and Constant Definition
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
code	UINT8 DPOF_Dir0Name[] =   
{
	'M','I','S','C',' ',' ',' ',' ',' ',' ',' ',0x10
};

code	UINT8 DPOF_FileName[] =  
{
	'A','U','T','P','R','I','N','T','M','R','K',0x20
};

#define K_DPOF_HDR_SIZE                 (UINT16)85
#define K_DPOF_HDR_GEN_CRT_START_INDEX  (UINT16)30
#define K_DPOF_HDR_GEN_CRT_SIZE         (UINT16)28
#define K_DPOF_HDR_GEN_DTM_START_INDEX  (UINT16)62

//totally 7+17+28+31+2=85 characters,It will be better to make the length to be even size.
//Check K_FIRMWARE_RELEASE_VERSION in main.h
code UINT8 DPOF_HeaderSectionContent[] = 
{
     '[','H','D','R',']',0x0d,0x0a,     //7 characters
     //version4.0@ada@0513 for DPOF
     'G','E','N',' ','R','E','V',' ','=',' ','0','1','.','0','0',0x0d,0x0a,   //17 characters
     'G','E','N',' ','C','R','T',' ','=',' ','"','S','P','C','A','5','3','3','"',' ','-',(UINT8)((K_FIRMWARE_RELEASE_VERSION & 0xff000000) >> 24) + '0',(UINT8)((K_FIRMWARE_RELEASE_VERSION & 0x00ff0000) >> 16) + '0','.',(UINT8)((K_FIRMWARE_RELEASE_VERSION & 0x0000ff00) >> 8) + '0',(UINT8)(K_FIRMWARE_RELEASE_VERSION & 0x000000ff) + '0',0x0d,0x0a, //28 characters
     'G','E','N',' ','D','T','M',' ','=',' ','2','0','0','1',':','0','1',':','0','1',':','0','0',':','0','0',':','0','0',0x0d,0x0a,   //31 characters
     0x0d,0x0a //2 characters
};

#define K_DPOF_JOBSECTIONTITLE_SIZE  (UINT16)7

code UINT8 DPOF_JobSectionTitle[] = 
{
     '[','J','O','B',']',0x0d,0x0a     //7 characters
};

#define K_DPOF_PRTPID_SIZE    (UINT16)15

code UINT8 DPOF_PrtPid[] = 
{
     'P','R','T',' ','P','I','D',' ','=',' ','0','0','0',0x0d,0x0a  //15 characters
};

#define K_DPOF_PRTQTY_SIZE    (UINT16)15

code UINT8 DPOF_PrtQty[] = 
{
     'P','R','T',' ','Q','T','Y',' ','=',' ','0','0','0',0x0d,0x0a  //15 characters
};


#define K_DPOF_PRTTYPSTD_SIZE      (UINT16)15

code UINT8 DPOF_PrtTypStd[] = 
{
     'P','R','T',' ','T','Y','P',' ','=',' ','S','T','D',0x0d,0x0a  //15 characters
};

#define K_DPOF_PRTTYPIDX_SIZE      (UINT16)15

code UINT8 DPOF_PrtTypIdx[] = 
{
     'P','R','T',' ','T','Y','P',' ','=',' ','I','D','X',0x0d,0x0a  //15 characters
};


#define K_DPOF_IMGFMTJFIF_SIZE     (UINT16)16

code UINT8 DPOF_ImgFmtJfif[] = 
{
     'I','M','G',' ','F','M','T',' ','=',' ','J','F','I','F',0x0d,0x0a  //16 characters
};

#define K_DPOF_IMGFMTEXIF2J_SIZE   (UINT16)20

code UINT8 DPOF_ImgFmtExif2j[] = 
{
     'I','M','G',' ','F','M','T',' ','=',' ','E','X','I','F','2',' ','-','J',0x0d,0x0a  //20 characters
};

#define K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE     (UINT16)45

code UINT8 DPOF_ImgSrcStdSunpXXXXjpg[] = 
{
     '<','I','M','G',' ','S','R','C',' ','=',' ','"','.','.','/','D','C','I','M','/','1','0','0','M','E','D','I','A','/','S','U','N','P','0','0','0','1','.','J','P','G','"','>',0x0d,0x0a  //45 characters
};

#define K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE     (UINT16)43

code UINT8 DPOF_ImgSrcIdxSunpXXXXjpg[] = 
{
     'I','M','G',' ','S','R','C',' ','=',' ','"','.','.','/','D','C','I','M','/','1','0','0','M','E','D','I','A','/','S','U','N','P','0','0','0','1','.','J','P','G','"',0x0d,0x0a  //43 characters
};

#define K_DPOF_CFGDSCSTD_SIZE                     (UINT16)35
#define K_DPOF_CFGDSCSTDSTRING_START_INDEX        (UINT16)11
#define K_DPOF_CFGDSCSTDSTRING_END_INDEX          (UINT16)23
#define K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX     (UINT16)30
  
code UINT8 DPOF_CfgDscStd[] = 
{
     'C','F','G',' ','D','S','C',' ','=',' ','"',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','"',' ','-','A','T','R',' ','D','T','M',0x0d,0x0a  //35 characters
};

#define K_DPOF_CFGDSCIDX_SIZE           (UINT16)26
#define K_DPOF_CFGDSCIDX_START_INDEX    (UINT16)11

code UINT8 DPOF_CfgDscIdx[] = 
{
     'C','F','G',' ','D','S','C',' ','=',' ','"',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','"',0x0d,0x0a  //26 characters
};

#define K_DPOF_CFGTRM_SIZE    (UINT16)11

code UINT8 DPOF_CfgTrm[] = 
{
     'C','F','G',' ','T','R','M',' ','=',' ','"'  //11 characters
};

#define K_DPOF_CFGROT_SIZE    (UINT16)15

code UINT8 DPOF_CfgRot[] = 
{
     'C','F','G',' ','R','O','T',' ','=',' ','D','F','L',0x0d,0x0a  //15 characters
};

#define K_DPOF_SEARCH_SIZE    (UINT16)0x0200

#endif /*APP_DPOF_INIT*/

#endif /*__DPOFCON_H__*/
