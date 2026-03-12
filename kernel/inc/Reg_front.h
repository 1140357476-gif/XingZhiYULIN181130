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
 * @file      reg_front.h
 * @brief     Brief
 * @author    LinJieCheng
 * @since     2010-01-01
 * @date      2010-01-01
*/


#ifndef __REG_FRONT_H__
#define __REG_FRONT_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
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


/**************************************************************************
 *                  R E G I S T E R  A D D R E S S
 **************************************************************************/ 
//I2C Interface Controller
#define REG_Front_Slaveaddr			(0x2900)
#define REG_Front_Transmode			(0x2904)
#define REG_Front_clkcentpos		(0x2905)
#define REG_Front_Subaddren			(0x2908)
#define REG_Front_I2Csyncsel		(0x290C)
#define REG_Front_Freq				(0x2910)
#define REG_Front_Wrcount			(0x2914)
#define REG_Front_Addr0				(0x2930)
#define REG_Front_Data0				(0x2931)
#define REG_Front_Addr1				(0x2934)
#define REG_Front_Data1				(0x2935)
#define REG_Front_Addr2				(0x2938)
#define REG_Front_Data2				(0x2939)
#define REG_Front_Addr3				(0x293C)
#define REG_Front_Data3				(0x293D)
#define REG_Front_Addr4				(0x2940)
#define REG_Front_Data4				(0x2941)
#define REG_Front_Addr5				(0x2944)
#define REG_Front_Data5				(0x2945)
#define REG_Front_Addr6				(0x2948)
#define REG_Front_Data6				(0x2949)
#define REG_Front_Addr7				(0x294C)
#define REG_Front_Data7				(0x294D)
#define REG_Front_Busy				(0x29A0)
#define REG_Front_ACKerr			(0x29A1)
#define REG_Front_Rstsif			(0x29B0)
#define REG_Front_disgatedcclk		(0x29B4)
#define REG_Front_Probemode			(0x29E0)
#define REG_Front_Dummy0			(0x29F0)
#define REG_Front_Dummy1			(0x29F1)
#define REG_Front_Dummy2			(0x29F2)
#define REG_Front_Dummy3			(0x29F3)

//Front Interface Controller
#define REG_Front_Capint			(0x2A00)
#define REG_Front_Vdupdate			(0x2A01)
#define REG_Front_Fldsel			(0x2A04)
#define REG_Front_Snapnum			(0x2A10)

//Valid & Reshape control
#define REG_Front_exthdipol			(0x2A1C)
#define REG_Front_bypashref			(0x2A1D)
#define REG_Front_Hrise_low			(0x2A20)
#define REG_Front_Hrise_high		(0x2A21)
#define REG_Front_Hfall_low			(0x2A22)
#define REG_Front_Hfall_high		(0x2A23)
#define REG_Front_Vrise_low			(0x2A24)
#define REG_Front_Vrise_high		(0x2A25)
#define REG_Front_Vfall_low			(0x2A26)
#define REG_Front_Vfall_high		(0x2A27)
#define REG_Front_Hreshen			(0x2A28)
#define REG_Front_Hsize_low			(0x2A30)
#define REG_Front_Hsize_high		(0x2A31)
#define REG_Front_Hoffset_low		(0x2A32)
#define REG_Front_Hoffset_high		(0x2A33)
#define REG_Front_Vsize_low			(0x2A34)
#define REG_Front_Vsize_high		(0x2A35)
#define REG_Front_Voffset_low		(0x2A36)
#define REG_Front_Voffset_high		(0x2A37)
#define REG_Front_Hsvden			(0x2A38)
#define REG_Front_F2CdspGate		(0x2A3A)

//Timing Generator
#define REG_Front_Linetotal_low		(0x2A40)
#define REG_Front_Linetotal_high	(0x2A41)
#define REG_Front_Lineblank_low		(0x2A42)
#define REG_Front_Lineblank_high	(0x2A43)
#define REG_Front_Frametotal_low	(0x2A44)
#define REG_Front_Frametotal_high	(0x2A45)
#define REG_Front_Frameblank_low	(0x2A46)
#define REG_Front_Frameblank_high	(0x2A47)
#define	REG_Front_Hdvdopos_low		(0x2A48)
#define	REG_Front_Hdvdopos_high		(0x2A49)
#define	REG_Front_Syncgenen			(0x2A4C)

//YUV Interface register
#define	REG_Front_Uvsel				(0x2A50)
#define	REG_Front_Ys128en			(0x2A51)
		  
//Front GPIO
#define	REG_Front_Tgingzero			(0x2A5C)
#define REG_Front_Tggpiosel			(0x2A60)
#define REG_Front_Tggpiooen			(0x2A62)
#define REG_Front_Tggpioo			(0x2A64)
#define REG_Front_Tggpioi			(0x2A66)
#define REG_Front_Rgbin_low			(0x2A6C)
#define REG_Front_Rgbin_high		(0x2A6D)
#define REG_Front_Senrst			(0x2AB0)
#define REG_Front_Disgatedclk		(0x2AB4)
#define REG_Front_Disgatedclk2x		(0x2AB5)
#define REG_Front_Hsync				(0x2AB8)

//Front Interrupt
#define REG_Front_FintEvt_low		(0x2AC0)
#define REG_Front_FintEvt_high		(0x2AC1)
#define REG_Front_VDRIntNum			(0x2AC4)
#define REG_Front_Inthnum0_low		(0x2AC6)
#define REG_Front_Inthnum0_high		(0x2AC7)
#define REG_Front_Inthnum1_low		(0x2AC8)
#define REG_Front_Inthnum1_high		(0x2AC9)
#define REG_Front_Inthnum2_low		(0x2ACA)
#define REG_Front_Inthnum2_high		(0x2ACB)
#define REG_Front_Inthnum3_low		(0x2ACC)
#define REG_Front_Inthnum3_high		(0x2ACD)
#define REG_Front_Finten_low		(0x2AD0)
#define REG_Front_Finten_high		(0x2AD1)

//For Test mode
#define REG_Front_T_Sigmode			(0x2AE0)
#define REG_Front_T_Probemode		(0x2AE4)
#define REG_Front_T_probesel		(0x2AE5)
#define REG_Front_T_Dummy0			(0x2AF0)
#define REG_Front_T_Dummy1			(0x2AF1)
#define REG_Front_T_Dummy2			(0x2AF2)
#define REG_Front_T_Dummy3			(0x2AF3)

#endif	/* __REG_FRONT_H__ */
