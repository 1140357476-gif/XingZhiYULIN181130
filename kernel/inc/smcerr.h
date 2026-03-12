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
#ifndef __SMCERR_H__
#define __SMCERR_H__

#define K_SMC_ErrOption		0

extern xdata UINT16 G_SMC_ErrCode;
extern void SMC_SetError(UINT16 errCode);
extern void SMC_ClearError(void);
extern UINT16 SMC_GetError(void);

#if ( K_SMC_ErrOption == 1 )
#define M_SMC_SetError(a)	SMC_SetError(a)
#define M_SMC_ClearError()	SMC_ClearError()
#define M_SMC_GetErrorr()	SMC_GetError()
#else
#define M_SMC_SetError(a)
#define M_SMC_ClearError()
#define M_SMC_GetError()
#endif

#endif /*__SMCERR_H__*/
