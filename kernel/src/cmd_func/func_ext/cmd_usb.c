/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
 
/**
 * @file		cmd_dos.c
 * @brief		
 * @author		Phil Lin
 * @since		2009-11-25
 * @date		2009-11-25
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "cmd.h"
#include "cmd_ext.h"

#include "sp1k_hal_api.h"

#include "uiflow.h"

#if (DOS_USB_OPTION == 1)

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/* printf info control */
#if 1
#define K_SLASH /
#define printf K_SLASH/
#endif

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

/**
 * @fn        void usbCmd(UINT8* ptr)
 * @brief     usbCmd
 * @param     [in] ptr
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-1-6
 * @todo      N/A
 * @bug       N/A
*/
void usbCmd(UINT8* ptr)
{
    UINT32 arg[8] = {0};

    cmdArgGet(&ptr, arg, 8, 1);

	/* "usb mm": usb mode MSDC */
    if (!strncmp((UINT8*)arg[0], "mm", 2)) {
		sp1kHalUSBSwPlugOut();
		gUsbMode = K_UIOPTION_STORAGE_MSDC;
		UI_UsbModeSet(K_UISTATUS_USBMODE_MASS);
		sp1kHalUSBSwPlugIn();

		printf("sw->MSDC\n");
    }
    /* "usb mp": usb mode PCCAM */
    else if (!strncmp((UINT8*)arg[0], "mp", 2)) {
		sp1kHalUSBSwPlugOut();
		gUsbMode = K_UIOPTION_STORAGE_NONE;
		UI_UsbModeSet(K_UISTATUS_USBMODE_PCCAM);
		sp1kHalUSBSwPlugIn();

		printf("sw->PCCAM\n");
    }
    /* "usb ms": usb mode SIDC */
    else if (!strncmp((UINT8*)arg[0], "ms", 2)) {
		sp1kHalUSBSwPlugOut();
		gUsbMode = K_UIOPTION_STORAGE_SIDC;
		UI_UsbModeSet(K_UISTATUS_USBMODE_MASS);
		sp1kHalUSBSwPlugIn();
		
		printf("sw->SIDC\n");
    }
    /* "usb out": usb software plug out */
    else if (!strncmp((UINT8*)arg[0], "out", 3)) {
		sp1kHalUSBSwPlugOut();

		printf("usb out\n");
	}    
    /* "usb in": usb software plug in */
    else if (!strncmp((UINT8*)arg[0], "in", 2)) {
		sp1kHalUSBSwPlugIn();

		printf("usb in\n");
	}    
    /* "usb rst": usb software rst */
    else if (!strncmp((UINT8*)arg[0], "rst", 3)) {
    	UINT32 dly = arg[1] ? arg[1] : 100; 
    	
		sp1kHalUSBSwPlugOut();
		sp1kHalWait(dly);
		sp1kHalUSBSwPlugIn();

		printf("usb rst(%lu)\n", dly);
	}    
    /* "usb fs": usb force full speed */
    else if (!strncmp((UINT8*)arg[0], "fs", 2)) {
		sp1kHalUSBSwPlugOut();
		sp1kHalWait(arg[1] ? arg[1] : 1000);
		XBYTE[0x2517] = 0x02; /* 00:Enable HS, 02:Force FS */
		sp1kHalUSBSwPlugIn();

		printf("usb fs\n");
    }
    /* "usb fs": usb support high speed */
    else if (!strncmp((UINT8*)arg[0], "hs", 2)) {
		sp1kHalUSBSwPlugOut();
		sp1kHalWait(arg[1] ? arg[1] : 1000);
		XBYTE[0x2517] = 0x00; /* 00:Enable HS, 02:Force FS */
		sp1kHalUSBSwPlugIn();

		printf("usb hs\n");
    }
    
}
#endif
