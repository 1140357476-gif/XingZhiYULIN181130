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

#ifndef __HAL_DEVICE_H__
#define __HAL_DEVICE_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "solution.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/*
----------------------------------------------------------------------------
                                     DISK
----------------------------------------------------------------------------
*/

#define DISK_DRV_CARD


#ifndef LIB_RELEASE
#if (_HW_SOLUTION_ == _HW_EVB_)
#define DISK_DRV_NAND
#elif (_HW_SOLUTION_ == _HW_RDK_A_)
#define DISK_DRV_SPI
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_A_)
#define DISK_DRV_NAND
#elif (_HW_SOLUTION_ == _HW_CUSTOM_MODEL_B_)
#define DISK_DRV_NAND
#endif
#endif
/*
----------------------------------------------------------------------------
                                     AUDIO
----------------------------------------------------------------------------
*/

//#define EmbCodec                   1
//#define AC97                       0

#define Audio                      1


typedef enum devId_e {
	DEV_ID_CARD = (unsigned char)0x00,
	DEV_ID_DISK = (unsigned char)0x01,
	DEV_ID_PANEL = (unsigned char)0x02,
	DEV_ID_TV = (unsigned char)0x03,
	DEV_ID_KEYBROAD = (unsigned char)0x04,
	DEV_ID_USB = (unsigned char)0x05,
	DEV_ID_FRONT = (unsigned char)0x06,
	DEV_ID_SPEAKER = (unsigned char)0x07,
	DEV_ID_PRINTER = (unsigned char)0x08,
	DEV_ID_MAX = (unsigned char)0x09,
} devId_t;

typedef enum devSts_e {
	DEV_STS_RDY = (unsigned char)0x00,
	DEV_STS_BUSY = (unsigned char)0x01,
	DEV_STS_ERR = (unsigned char)0x02,
	DEV_STS_CONNECTED = (unsigned char)0x0f,
	DEV_STS_DISCONNECTED = (unsigned char)0xf0,
	DEV_STS_UNKNOWN = (unsigned char)0xff,
} devSts_t;

typedef enum devType_e {
	DEV_TYPE_CARD_COM = 0x00, //Default by common type
	DEV_TYPE_CARD_SD = 0x01,
	DEV_TYPE_CARD_MMC = 0x02,

	DEV_TYPE_DISK_COM = 0x10,
	DEV_TYPE_DISK_NAND = 0x11,
	DEV_TYPE_DISK_NOR = 0x12,
	DEV_TYPE_DISK_SPI = 0x13,
	DEV_TYPE_DISK_DRAM = 0x14,

	DEV_TYPE_PANEL_COM = 0x20,
	DEV_TYPE_PANEL_LCD = 0x21,
	DEV_TYPE_PANEL_LCM = 0x22,

	DEV_TYPE_TV_COM = 0x30,
	DEV_TYPE_TV_NTSC = 0x31,
	DEV_TYPE_TV_PAL = 0x32,

	DEV_TYPE_KEYBROAD_COM = 0x40,

	DEV_TYPE_USB_COM = 0x50,
	DEV_TYPE_USB_20 = 0x51,
	DEV_TYPE_USB_11 = 0x52,

	DEV_TYPE_FRONT_COM = 0x60,

	DEV_TYPE_SPEAK_COM = 0x70,

	DEV_TYPE_PRINTER_COM = 0x80,

	DEV_TYPE_UNKNOWN = 0xff, //Init value
} devType_t;

typedef struct devAttr_s {
	UINT8 devId;
	//UINT8 devName[16];
	UINT8 devEn;
	UINT8 devSts;
	UINT8 devType;
	//UINT8 devDrv[5];
} devAttr_t;

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
UINT8 sp1kDevInit(void);
devAttr_t* sp1kDevHdlGet(void);

#include "macro_expand.h"

#include "cardopt.h"
#endif  /* __HAL_DEVICE_H__ */

