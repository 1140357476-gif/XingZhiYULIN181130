/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      reg_cpu.h
 * @brief     CPU register definition
 * @author    Phil Lin
 * @since     2011-02-23
 * @date      2011-02-23
*/

#ifndef __REG_CPU_H__
#define __REG_CPU_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	REG_Cpu_MemEn	(0x2C00)
#define	REG_Cpu_RamEn	(0x2C01)
#define	REG_Cpu_P1oeSel	(0x2C02)
#define	REG_Cpu_P3oeSel	(0x2C03)		
#define	REG_Cpu_PwrDown	(0x2C05)
#define	REG_Cpu_IntrSts0	(0x2C06)
#define	REG_Cpu_IntrSts1	(0x2C07)
#define	REG_Cpu_DmaAct	(0x2C08)
#define	REG_Cpu_CpuRdyEn	(0x2C09)
#define	REG_Cpu_DatActCyc	(0x2C0A)
#define	REG_Cpu_CodeActCyc	(0x2C0B)
#define	REG_Cpu_SyncMode	(0x2C0C)
#define	REG_Cpu_LnReadEn	(0x2C0D)
		
#define	REG_Cpu_PioDat	(0x2C10)
#define	REG_Cpu_SramMod	(0x2C11)
#define	REG_Cpu_SramRstIdxL	(0x2C12)
#define	REG_Cpu_SramRstIdxH	(0x2C13)
#define	REG_Cpu_BistSel	(0x2C14)
#define	REG_Cpu_BistCtrl	(0x2C15)
#define	REG_Cpu_IntRomTest	(0x2C16)
		
#define	REG_Cpu_ShadowEn	(0x2C20)
#define	REG_Cpu_ShadowImemL	(0x2C21)
#define	REG_Cpu_ShadowImemH	(0x2C22)
#define	REG_Cpu_ShadowDmemL	(0x2C23)
#define	REG_Cpu_ShadowDmemH	(0x2C24)
#define	REG_Cpu_ShadowDmemSize	(0x2C25)
#define	REG_Cpu_ShadowDramStAddrL	(0x2C26)
#define	REG_Cpu_ShadowDramStAddrH	(0x2C27)
#define	REG_Cpu_ShadowDramStBankL	(0x2C28)
#define	REG_Cpu_ShadowDramStBankH	(0x2C29)
#define	REG_Cpu_ShadowDramEdBankL	(0x2C2A)
#define	REG_Cpu_ShadowDramEdBankH	(0x2C2B)
		
#define	REG_Cpu_DatDramPageL	(0x2C2E)
#define	REG_Cpu_DatDramPageH	(0x2C2F)
#define	REG_Cpu_CpuIntEnL	(0x2C30)
#define	REG_Cpu_CpuIntEnH	(0x2C31)
#define	REG_Cpu_Ram2DramP0L	(0x2C34)
#define	REG_Cpu_Ram2DramP0H	(0x2C35)
#define	REG_Cpu_Ram2DramP1L	(0x2C36)
#define	REG_Cpu_Ram2DramP1H	(0x2C37)
#define	REG_Cpu_Ram2DramP2L	(0x2C38)
#define	REG_Cpu_Ram2DramP2H	(0x2C39)
#define	REG_Cpu_Ram2DramP3L	(0x2C3A)
#define	REG_Cpu_Ram2DramP3H	(0x2C3B)
		
#define	REG_Cpu_LPC0	(0x2C40)
#define	REG_Cpu_LPC1	(0x2C41)
#define	REG_Cpu_LPC2	(0x2C42)
		
#define	REG_Cpu_SramPioWrIdxL	(0x2CA0)
#define	REG_Cpu_SramPioWrIdxH	(0x2CA1)
#define	REG_Cpu_SramPioRdIdxL	(0x2CA2)
#define	REG_Cpu_SramPioRdIdxH	(0x2CA3)
#define	REG_Cpu_SramDmaWrIdxL	(0x2CA4)
#define	REG_Cpu_SramDmaRdIdxH	(0x2CA5)
		
#define	REG_Cpu_RsvL	(0x2CC0)
#define	REG_Cpu_RsvH	(0x2CC1)
		
#define	REG_Cpu_Sprobe	(0x2CF0)
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* __REG_CPU_H__ */
