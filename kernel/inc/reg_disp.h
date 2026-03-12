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
 * @file      reg_disp.h
 * @brief     DISP register definition
 * @author    Phil Lin
 * @since     2011-02-23
 * @date      2011-02-23
*/

#ifndef __REG_DISP_H__
#define __REG_DISP_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	REG_Disp_VsVdIntSts	(0x2D00)
#define	REG_Disp_VsVdIntEn	(0x2D02)
		
#define	REG_Disp_SwRst	(0x2D05)
#define	REG_Disp_UnderFlow	(0x2D06)
#define	REG_Disp_DispEn_Flip	(0x2D07)

#define	REG_Disp_VlineL	(0x2D08)
#define	REG_Disp_VlineH	(0x2D09)
#define	REG_Disp_HpixelL	(0x2D0A)
#define	REG_Disp_HpixelH	(0x2D0B)

#define	REG_Disp_Vsyncos	(0x2D0C)
#define	REG_Disp_Hsyncos	(0x2D0D)
#define	REG_Disp_Vsyncw	(0x2D0E)
#define	REG_Disp_Hsyncw	(0x2D0F)

#define	REG_Disp_PnlH0L	(0x2D10)
#define	REG_Disp_PnlH0H	(0x2D11)
#define	REG_Disp_PnlH1L	(0x2D12)
#define	REG_Disp_PnlH1H	(0x2D13)
#define	REG_Disp_PnlV0L	(0x2D14)
#define	REG_Disp_PnlV0H	(0x2D15)
#define	REG_Disp_PnlV1L	(0x2D16)
#define	REG_Disp_PnlV1H	(0x2D17)

#define	REG_Disp_PvX0L	(0x2D18)
#define	REG_Disp_PvX0H	(0x2D19)
#define	REG_Disp_PvX1L	(0x2D1A)
#define	REG_Disp_PvX1H	(0x2D1B)
#define	REG_Disp_PvY0L	(0x2D1C)
#define	REG_Disp_PvY0H	(0x2D1D)
#define	REG_Disp_PvY1L	(0x2D1E)
#define	REG_Disp_PvY1H	(0x2D1F)
#define	REG_Disp_PvH0L	(0x2D20)
#define	REG_Disp_PvH0H	(0x2D21)
#define	REG_Disp_PvH1L	(0x2D22)
#define	REG_Disp_PvH1H	(0x2D23)
#define	REG_Disp_PvV0L	(0x2D24)
#define	REG_Disp_PvV0H	(0x2D25)
#define	REG_Disp_PvV1L	(0x2D26)
#define	REG_Disp_PvV1H	(0x2D27)

#define	REG_Disp_OsdX0L	(0x2D28)
#define	REG_Disp_OsdX0H	(0x2D29)
#define	REG_Disp_OsdX1L	(0x2D2A)
#define	REG_Disp_OsdX1H	(0x2D2B)
#define	REG_Disp_OsdY0L	(0x2D2C)
#define	REG_Disp_OsdY0H	(0x2D2D)
#define	REG_Disp_OsdY1L	(0x2D2E)
#define	REG_Disp_OsdY1H	(0x2D2F)
#define	REG_Disp_OsdH0L	(0x2D30)
#define	REG_Disp_OsdH0H	(0x2D31)
#define	REG_Disp_OsdH1L	(0x2D32)
#define	REG_Disp_OsdH1H	(0x2D33)
#define	REG_Disp_OsdV0L	(0x2D34)
#define	REG_Disp_OsdV0H	(0x2D35)
#define	REG_Disp_OsdV1L	(0x2D36)
#define	REG_Disp_OsdV1H	(0x2D37)

#define	REG_Disp_ImgOsdReq	(0x2D38)
#define	REG_Disp_ReqBlkPedL	(0x2D39)
#define	REG_Disp_ReqBlkPedH	(0x2D3A)
#define	REG_Disp_SclMode	(0x2D3B)

#define	REG_Disp_PvHSclIniL	(0x2D3C)
#define	REG_Disp_PvHSclIniH	(0x2D3D)
#define	REG_Disp_PvVSclIniL	(0x2D3E)
#define	REG_Disp_PvVSclIniH	(0x2D3F)
#define	REG_Disp_PvHSclFctL	(0x2D40)
#define	REG_Disp_PvHSclFctH	(0x2D41)
#define	REG_Disp_PvVSclFctL	(0x2D42)
#define	REG_Disp_PvVSclFctH	(0x2D43)

#define	REG_Disp_OsdHSclIniL	(0x2D44)
#define	REG_Disp_OsdHSclIniH	(0x2D45)
#define	REG_Disp_OsdVSclIniL	(0x2D46)
#define	REG_Disp_OsdVSclIniH	(0x2D47)
#define	REG_Disp_OsdHSclFctL	(0x2D48)
#define	REG_Disp_OsdHSclFctH	(0x2D49)
#define	REG_Disp_OsdVSclFctL	(0x2D4A)
#define	REG_Disp_OsdVSclFctH	(0x2D4B)
#define	REG_Disp_OsdilaceIni	(0x2D4C)
#define	REG_Disp_OsdvoddIniL	(0x2D4D)
#define	REG_Disp_OsdvoddIniH	(0x2D4E)
		
#define	REG_Disp_ExtendMode	(0x2D50)
#define	REG_Disp_BgColorR	(0x2D51)
#define	REG_Disp_BgColorG	(0x2D52)
#define	REG_Disp_BgColorB	(0x2D53)
#define	REG_Disp_ImgKeyEn	(0x2D54)
#define	REG_Disp_ImgKeyYH	(0x2D55)
#define	REG_Disp_ImgKeyYL	(0x2D56)
		
#define	REG_Disp_OsdAlpha0	(0x2D58)
#define	REG_Disp_OsdAlpha1	(0x2D59)
#define	REG_Disp_OsdAlpha2	(0x2D5A)
#define	REG_Disp_OsdAlpha3	(0x2D5B)

#define	REG_Disp_EdgeTypeGain	(0x2D5C)
#define	REG_Disp_CorClpType	(0x2D5D)
		
#define	REG_Disp_GammaEnSel	(0x2D5F)
#define	REG_Disp_GammaLUT0	(0x2D60)
#define	REG_Disp_GammaLUT1	(0x2D61)
#define	REG_Disp_GammaLUT2	(0x2D62)
#define	REG_Disp_GammaLUT3	(0x2D63)
#define	REG_Disp_GammaLUT4	(0x2D64)
#define	REG_Disp_GammaLUT5	(0x2D65)
#define	REG_Disp_GammaLUT6	(0x2D66)
#define	REG_Disp_GammaLUT7	(0x2D67)
#define	REG_Disp_GammaLUT8	(0x2D68)
#define	REG_Disp_GammaLUT9	(0x2D69)
#define	REG_Disp_GammaLUTa	(0x2D6A)
#define	REG_Disp_GammaLUTb	(0x2D6B)
#define	REG_Disp_GammaLUTc	(0x2D6C)
#define	REG_Disp_GammaLUTd	(0x2D6D)
#define	REG_Disp_GammaLUTe	(0x2D6E)
#define	REG_Disp_GammaLUTf	(0x2D6F)
#define	REG_Disp_GammaLUT10	(0x2D70)

#define	REG_Disp_ColMtxEnFmt	(0x2D71)
#define	REG_Disp_ColA00L	(0x2D72)
#define	REG_Disp_ColA01L	(0x2D73)
#define	REG_Disp_ColA02L	(0x2D74)
#define	REG_Disp_ColA10L	(0x2D75)
#define	REG_Disp_ColA11L	(0x2D76)
#define	REG_Disp_ColA12L	(0x2D77)
#define	REG_Disp_ColA20L	(0x2D78)
#define	REG_Disp_ColA21L	(0x2D79)
#define	REG_Disp_ColA22L	(0x2D7A)
#define	REG_Disp_ColA00H01H	(0x2D7B)
#define	REG_Disp_ColA02H10H	(0x2D7C)
#define	REG_Disp_ColA11H12H	(0x2D7D)
#define	REG_Disp_ColA20H21H	(0x2D7E)
#define	REG_Disp_ColA22H	(0x2D7F)

#define	REG_Disp_GpioSel0	(0x2D80)
#define	REG_Disp_GpioSel1	(0x2D81)
#define	REG_Disp_GpioSel2	(0x2D82)
#define	REG_Disp_GpioSel3	(0x2D83)

#define	REG_Disp_GpioOutVal0	(0x2D84)
#define	REG_Disp_GpioOutVal1	(0x2D85)
#define	REG_Disp_GpioOutVal2	(0x2D86)
#define	REG_Disp_GpioOutVal3	(0x2D87)

#define	REG_Disp_GpioOutEn0	(0x2D88)
#define	REG_Disp_GpioOutEn1	(0x2D89)
#define	REG_Disp_GpioOutEn2	(0x2D8A)
#define	REG_Disp_GpioOutEn3	(0x2D8B)

#define	REG_Disp_GpioInVal0	(0x2D8C)
#define	REG_Disp_GpioInVal1	(0x2D8D)
#define	REG_Disp_GpioInVal2	(0x2D8E)
#define	REG_Disp_GpioInVal3	(0x2D8F)

#define	REG_Disp_GpioRIntEn0	(0x2D90)
#define	REG_Disp_GpioRIntEn1	(0x2D91)
#define	REG_Disp_GpioRIntEn2	(0x2D92)
#define	REG_Disp_GpioRIntEn3	(0x2D93)
#define	REG_Disp_GpioFIntEn0	(0x2D94)
#define	REG_Disp_GpioFIntEn1	(0x2D95)
#define	REG_Disp_GpioFIntEn2	(0x2D96)
#define	REG_Disp_GpioFIntEn3	(0x2D97)

#define	REG_Disp_GpioRIntSts0	(0x2D98)
#define	REG_Disp_GpioRIntSts1	(0x2D99)
#define	REG_Disp_GpioRIntSts2	(0x2D9A)
#define	REG_Disp_GpioRIntSts3	(0x2D9B)
#define	REG_Disp_GpioFIntSts0	(0x2D9C)
#define	REG_Disp_GpioFIntSts1	(0x2D9D)
#define	REG_Disp_GpioFIntSts2	(0x2D9E)
#define	REG_Disp_GpioFIntSts3	(0x2D9F)

#define	REG_Disp_PnlSel	(0x2DA0)
#define	REG_Disp_PhsSel_Polar	(0x2DA1)
#define	REG_Disp_PnlRound_RB	(0x2DA3)
#define	REG_Disp_UpshdSel	(0x2DA6)
#define	REG_Disp_RgbEvenOdd	(0x2DA7)
#define	REG_Disp_LvEn_Rgb888	(0x2DA8)
#define	REG_Disp_UpsRgbOnly	(0x2DA9)

#define	REG_Disp_CcirOSeq	(0x2DAA)
#define	REG_Disp_FieldEnSwap	(0x2DAB)
#define	REG_Disp_PalmEn_Filter	(0x2DAC)
#define	REG_Disp_SatuRateL	(0x2DAD)
#define	REG_Disp_SatuRateH	(0x2DAE)
#define	REG_Disp_HueAdjust	(0x2DAF)

#define	REG_Disp_LcmMode_BusW	(0x2DB0)
#define	REG_Disp_LcmDatCyc_Type	(0x2DB1)
#define	REG_Disp_LcmVSUpd_DmaEn	(0x2DB2)
#define	REG_Disp_LcmCS_RS	(0x2DB3)
#define	REG_Disp_LcmDatFmt	(0x2DB4)
#define	REG_Disp_LcmCmdCtrl	(0x2DB5)
#define	REG_Disp_LcmPioTrg_Busy	(0x2DB6)

#define	REG_Disp_LcmAutoCmd00	(0x2DB7)
#define	REG_Disp_LcmAutoCmd01	(0x2DB8)
#define	REG_Disp_LcmAutoCmd02	(0x2DB9)
#define	REG_Disp_LcmAutoCmd10	(0x2DBA)
#define	REG_Disp_LcmAutoCmd11	(0x2DBB)
#define	REG_Disp_LcmAutoCmd12	(0x2DBC)
#define	REG_Disp_LcmAutoCmd20	(0x2DBD)
#define	REG_Disp_LcmAutoCmd21	(0x2DBE)
#define	REG_Disp_LcmAutoCmd22	(0x2DBF)
#define	REG_Disp_LcmAutoCmd30	(0x2DC0)
#define	REG_Disp_LcmAutoCmd31	(0x2DC1)
#define	REG_Disp_LcmAutoCmd32	(0x2DC2)
#define	REG_Disp_LcmAutoCmd40	(0x2DC3)
#define	REG_Disp_LcmAutoCmd41	(0x2DC4)
#define	REG_Disp_LcmAutoCmd42	(0x2DC5)
#define	REG_Disp_LcmAutoCmd50	(0x2DC6)
#define	REG_Disp_LcmAutoCmd51	(0x2DC7)
#define	REG_Disp_LcmAutoCmd52	(0x2DC8)
#define	REG_Disp_LcmAutoCmd60	(0x2DC9)
#define	REG_Disp_LcmAutoCmd61	(0x2DCA)
#define	REG_Disp_LcmAutoCmd62	(0x2DCB)
#define	REG_Disp_LcmAutoCmd70	(0x2DCC)
#define	REG_Disp_LcmAutoCmd71	(0x2DCD)
#define	REG_Disp_LcmAutoCmd72	(0x2DCE)
		
#define	REG_Disp_ImgVOddIniEn	(0x2DD0)
#define	REG_Disp_ImgVOddIniL	(0x2DD1)
#define	REG_Disp_ImgVOddIniH	(0x2DD2)
		
#define	REG_Disp_PnlEFAvgR	(0x2DD8)
#define	REG_Disp_PnlEFAvgG	(0x2DD9)
#define	REG_Disp_PnlEFAvgB	(0x2DDA)
#define	REG_Disp_PnlOFAvgR	(0x2DDB)
#define	REG_Disp_PnlOFAvgG	(0x2DDC)
#define	REG_Disp_PnlOFAvgB	(0x2DDD)
#define	REG_Disp_PnlAvgHf	(0x2DDE)
		
#define	REG_Disp_UpdPvOsd	(0x2DE0)
#define	REG_Disp_DramUpdEn	(0x2DE1)
#define	REG_Disp_UpdRegSel	(0x2DE2)
#define	REG_Disp_UpdRegDatL	(0x2DE3)
#define	REG_Disp_UpdRegDatH	(0x2DE4)
		
#define	REG_Disp_ColB00H	(0x2DE6)
#define	REG_Disp_ColB01H	(0x2DE7)
#define	REG_Disp_ColB02H	(0x2DE8)
#define	REG_Disp_ColB00L01L	(0x2DE9)
#define	REG_Disp_ColB02L	(0x2DEA)

#define	REG_Disp_BistEn	(0x2DEB)
#define	REG_Disp_CpuSramEn	(0x2DEC)
#define	REG_Disp_CpuSramAddr	(0x2DED)
#define	REG_Disp_CpuSramDatL	(0x2DEE)
#define	REG_Disp_CpuSramDatH	(0x2DEF)
#define	REG_Disp_ImgTestMod	(0x2DF0)
#define	REG_Disp_TvTestDat0	(0x2DF1)
#define	REG_Disp_TvTestDat1	(0x2DF2)
#define	REG_Disp_TvTestDat2	(0x2DF3)

#define	REG_Disp_DispSignal	(0x2DF4)

#define	REG_Disp_RandDat	(0x2DF5)
#define	REG_Disp_PrbGrpSel	(0x2DF6)
#define	REG_Disp_PrbSel	(0x2DF7)
#define	REG_Disp_Eprobe1	(0x2DF8)
#define	REG_Disp_Eprobe2	(0x2DF9)
#define	REG_Disp_Rsv0	(0x2DFA)
#define	REG_Disp_Rsv1	(0x2DFB)
#define	REG_Disp_Rsv2	(0x2DFC)
#define	REG_Disp_Rsv3	(0x2DFD)
 
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

#endif  /* __REG_DISP_H__ */
