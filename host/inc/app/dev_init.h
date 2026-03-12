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
 * @file      dev_init.h
 * @brief     Brief
 * @author    Phil Lin
 * @since     2011-01-01
 * @date      2011-01-01
*/

#ifndef __DEV_INIT_H__
#define __DEV_INIT_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DEV_INIT_DBG_EN		1
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if DEV_INIT_DBG_EN
#define devInitPrintf printf
#else
#define devInitPrintf ;/DBG_SLASH
#endif
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
void devInit(void);
UINT8 appDispInit(void);
UINT8 appFrontInit(void);
void appButtonInit(void);
UINT8 appImgDecode(UINT32 resId);
UINT8 appOpenImgDec(UINT8 mode);
void appGpioInit(void);
void appVersionShow(void);
void hostDevInit(void);
UINT8 appZoomDevInit(void);


#endif  /* __DEV_INIT_H__ */

