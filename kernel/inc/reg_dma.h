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
 * @file      reg_dma.h
 * @brief     GPRM register definition
 * @author    Allen Lin
 * @since     2011-02-24
 * @date      2011-02-24
*/

#ifndef __REG_DMA_H__
#define __REG_DMA_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define	REG_Dma_DmaData		(0x2300)
#define	REG_Dma_DmaSrcDst	(0x2301)
#define	REG_Dma_DmaSizeL	(0x2302)
#define	REG_Dma_DmaSizeM	(0x2303)
#define	REG_Dma_DmaSizeH	(0x2304)
#define	REG_Dma_BaseAddrL	(0x2320)
#define	REG_Dma_BaseAddrML	(0x2321)
#define	REG_Dma_BaseAddrMH	(0x2322)
#define	REG_Dma_BaseAddrH	(0x2323)
#define	REG_Dma_DmaIdle		(0x2330)
#define	REG_Dma_DmaBufSts	(0x23A0)
#define	REG_Dma_DmaStart	(0x23B0)
#define	REG_Dma_DmaCmp		(0x23C0)
#define	REG_Dma_DmaCmpEn	(0x23D0)
#define	REG_Dma_Probeswap	(0x23E0)

 
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

#endif  /* __REG_DMA_H__ */
