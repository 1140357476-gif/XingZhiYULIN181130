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
#ifndef __QUANTDEF_H__
#define __QUANTDEF_H__

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define K_QWriteIndex           0
#define K_QWriteTable           1

#define K_QReadIndex            0
#define K_QReadTableRaster      1
#define K_QReadTableZigzag      2

#define K_Q50                   50
#define K_Q70                   70
#define K_Q80                   80
#define K_Q85                   85
#define K_Q88                   88
#define K_Q90                   90
#define K_Q92                   92
#define K_Q95                   95
//#define K_QUserRaster           0xf0
//#define K_QUserZigzag           0xf1
#define K_Q1                    1
//#define K_Qxx                   K_Q85
#define K_Qxx                   35
#define K_Q_OV                  35


#if 0
extern	code UINT8 QTable50[];
extern	code UINT8 QTable70[];
extern	code UINT8 QTable80[];
extern	code UINT8 QTable85[];
extern	code UINT8 QTable88[];
extern	code UINT8 QTable90[];
extern	code UINT8 QTable92[];
extern	code UINT8 QTable95[];

//extern	code UINT8 zigzagTable[];
extern    code UINT8 QTableGOSD[];
#endif

#endif /*__QUANTDEF_H__*/
