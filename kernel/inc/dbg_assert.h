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

#ifndef __DBG_ASSERT_H__
#define __DBG_ASSERT_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
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
UINT8 __ASSERT(UINT16 __FILE_ID, UINT16 __LINE_NUM, UINT8 __EXIT);

UINT8 __INTR_ASSERT(UINT16 __FILE_ID, UINT16 __LINE_NUM, UINT8 __EXIT);

void __PRINTF(UINT16 __FILE_ID, UINT16 __LINE_NUM);
#define __PRINTF_POS() __PRINTF(__FILE,__LINE__)
#define __ASSERT_POS(__EXIF) __ASSERT((__FILE),(__LINE__),(__EXIF))

#endif /*__APP_DEBUG_H__*/


