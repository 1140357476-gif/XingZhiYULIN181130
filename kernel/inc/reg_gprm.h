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
 * @file      reg_gprm.h
 * @brief     GPRM register definition
 * @author    Phil Lin
 * @since     2011-02-23
 * @date      2011-02-23
*/

#ifndef __REG_GPRM_H__
#define __REG_GPRM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	REG_Gprm_DmaIntSts	(0x2F00)
#define	REG_Gprm_DmaIntEn	(0x2F04)
#define	REG_Gprm_SwRst_IntEn	(0x2F10)
#define	REG_Gprm_DmaSwRst	(0x2F11)
#define	REG_Gprm_GateClk	(0x2F14)
		
#define	REG_Gprm_AgbhoffL	(0x2F20)
#define	REG_Gprm_AgbhoffH	(0x2F21)
#define	REG_Gprm_AgbvoffL	(0x2F22)
#define	REG_Gprm_AgbvoffH	(0x2F23)
#define	REG_Gprm_BgbhoffL	(0x2F24)
#define	REG_Gprm_BgbhoffH	(0x2F25)
#define	REG_Gprm_BgbvoffL	(0x2F26)
#define	REG_Gprm_BgbvoffH	(0x2F27)
#define	REG_Gprm_BgbwidthL	(0x2F28)
#define	REG_Gprm_BgbwidthH	(0x2F29)
#define	REG_Gprm_BgbheightL	(0x2F2A)
#define	REG_Gprm_BgbheightH	(0x2F2B)
		
#define	REG_Gprm_OpMode	(0x2F30)
#define	REG_Gprm_RotMode	(0x2F31)
#define	REG_Gprm_SclMode	(0x2F32)
#define	REG_Gprm_SclFctL	(0x2F34)
#define	REG_Gprm_SclFctH	(0x2F35)
#define	REG_Gprm_SclFctIniL	(0x2F36)
#define	REG_Gprm_SclFctIniH	(0x2F37)
#define	REG_Gprm_PstThd	(0x2F38)
#define	REG_Gprm_DatFmt_CpEn	(0x2F3A)
#define	REG_Gprm_OsdKeyEn	(0x2F3B)
#define	REG_Gprm_OsdKeyVal	(0x2F3C)
#define	REG_Gprm_Trg_IntSts	(0x2F3F)
		
#define	REG_Gprm_OsdIdx0	(0x2F40)
#define	REG_Gprm_OsdIdx1	(0x2F41)
#define	REG_Gprm_OsdIdx2	(0x2F42)
#define	REG_Gprm_OsdIdx3	(0x2F43)
#define	REG_Gprm_DecAddrSrc0	(0x2F44)
#define	REG_Gprm_DecAddrSrc1	(0x2F45)
#define	REG_Gprm_DecAddrSrc2	(0x2F46)
#define	REG_Gprm_DecAddrSrc3	(0x2F47)
#define	REG_Gprm_DecAddrDst0	(0x2F48)
#define	REG_Gprm_DecAddrDst1	(0x2F49)
#define	REG_Gprm_DecAddrDst2	(0x2F4A)
#define	REG_Gprm_DecAddrDst3	(0x2F4B)
#define	REG_Gprm_DecSize0	(0x2F4C)
#define	REG_Gprm_DecSize1	(0x2F4D)
#define	REG_Gprm_DecSize2	(0x2F4E)
		
#define	REG_Gprm_Priority	(0x2F50)
#define	REG_Gprm_FillDramEn	(0x2F51)
#define	REG_Gprm_FillDat	(0x2F52)
#define	REG_Gprm_DmaBusy	(0x2F54)
		
#define	REG_Gprm_Dma0SrcAddr0	(0x2F60)
#define	REG_Gprm_Dma0SrcAddr1	(0x2F61)
#define	REG_Gprm_Dma0SrcAddr2	(0x2F62)
#define	REG_Gprm_Dma0SrcAddr3	(0x2F63)
#define	REG_Gprm_Dma0DstAddr0	(0x2F64)
#define	REG_Gprm_Dma0DstAddr1	(0x2F65)
#define	REG_Gprm_Dma0DstAddr2	(0x2F66)
#define	REG_Gprm_Dma0DstAddr3	(0x2F67)
#define	REG_Gprm_Dma0ByteCnt0	(0x2F68)
#define	REG_Gprm_Dma0ByteCnt1	(0x2F69)
#define	REG_Gprm_Dma0ByteCnt2	(0x2F6A)
#define	REG_Gprm_Dma0CntRd0	(0x2F6C)
#define	REG_Gprm_Dma0CntRd1	(0x2F6D)
#define	REG_Gprm_Dma0CntRd2	(0x2F6E)
#define	REG_Gprm_Dma0Start	(0x2F6F)
		
#define	REG_Gprm_Dma1SrcAddr0	(0x2F70)
#define	REG_Gprm_Dma1SrcAddr1	(0x2F71)
#define	REG_Gprm_Dma1SrcAddr2	(0x2F72)
#define	REG_Gprm_Dma1SrcAddr3	(0x2F73)
#define	REG_Gprm_Dma1DstAddr0	(0x2F74)
#define	REG_Gprm_Dma1DstAddr1	(0x2F75)
#define	REG_Gprm_Dma1DstAddr2	(0x2F76)
#define	REG_Gprm_Dma1DstAddr3	(0x2F77)
#define	REG_Gprm_Dma1ByteCnt0	(0x2F78)
#define	REG_Gprm_Dma1ByteCnt1	(0x2F79)
#define	REG_Gprm_Dma1ByteCnt2	(0x2F7A)
#define	REG_Gprm_Dma1CntRd0	(0x2F7C)
#define	REG_Gprm_Dma1CntRd1	(0x2F7D)
#define	REG_Gprm_Dma1CntRd2	(0x2F7E)
#define	REG_Gprm_Dma1Start	(0x2F7F)
		
#define	REG_Gprm_CpuSramEn	(0x2F80)
#define	REG_Gprm_CpuSramAddr	(0x2F81)
#define	REG_Gprm_CpuSramWr0	(0x2F82)
#define	REG_Gprm_CpuSramWr1	(0x2F83)
#define	REG_Gprm_CpuSramWr2	(0x2F84)
#define	REG_Gprm_PaletteSize	(0x2F86)
		
#define	REG_Gprm_ProbeGrp	(0x2FEB)
#define	REG_Gprm_ProbeMode	(0x2FEC)
#define	REG_Gprm_ProbeSel	(0x2FED)
#define	REG_Gprm_Probe0	(0x2FEE)
#define	REG_Gprm_Probe1	(0x2FEF)
		
#define	REG_Gprm_DmaProbeSel	(0x2FF8)
#define	REG_Gprm_DmaProbMode	(0x2FF9)
#define	REG_Gprm_DmaProbe0	(0x2FFA)
#define	REG_Gprm_DmaProbe1	(0x2FFB)
 
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

#endif  /* __REG_GPRM_H__ */
