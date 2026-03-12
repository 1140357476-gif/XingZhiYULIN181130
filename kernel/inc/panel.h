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
#ifndef __PANEL_H__
#define __PANEL_H__

/* TV */
void Tv_Init(UINT8 mode);	//NTSC:720*480(0), PAL:720*576(1)
void Tv_Flip(UINT8 mode);	//dummy function

/* LCD */
/* AUO */
//1.5"
void A015AN02_Init(UINT8 mode);	//1.5", 280*220(8) / 305*220(9)
void A015AN02_Flip(UINT8 mode);
void A015AN04_Init(UINT8 mode);	//1.5", 280*220(8) / 305*220(9)
void A015AN04_Flip(UINT8 mode);
//2.0"
void A020CN01_Init(UINT8 mode);	//2.0"
void A020CN01_Flip(UINT8 mode);
//2.5"
void A025BN01_Init(UINT8 mode);	//2.5",640*240(4,8),320*240(9)
void A025BN01_Flip(UINT8 mode);

/* TPO */
void TD015THEB2_Init(UINT8 mode);	/* 1.5", 320*240(9) */

/* LCM */
void FD54124B_Init(UINT8 mode);			/* 128 * 160 */
void H018IN03_11CSTN_Init(UINT8 mode);	/*  96 *  64 */
void LCM_1B3GB00_Init(UINT8 mode);	/*  240 *  320 */

#endif  /* __PANEL_H__ */

