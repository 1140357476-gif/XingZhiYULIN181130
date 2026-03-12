/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
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
#ifndef __SP1K_DISP_API_H__
#define __SP1K_DISP_API_H__

/** \file sp1k_disp_api.h
 * @brief		Spca1528 display related interfaces
 */

#include "disp_api.h"
#include "panel.h"
#include "sp1k_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
typedef enum {
	SP1K_DISP_FIT_METHOD_SRC_STRETCH,
	SP1K_DISP_FIT_METHOD_DST_OUTSIDE,
} sp1kDispFitMethod_e;

typedef enum {
	SP1K_DISP_WAIT_HVALID = DISP_WAIT_HVALID,
	SP1K_DISP_WAIT_HSYNC  = DISP_WAIT_HSYNC,
	SP1K_DISP_WAIT_VVALID = DISP_WAIT_VVALID,
	SP1K_DISP_WAIT_VSYNC  = DISP_WAIT_VSYNC,
} sp1kDispWaitSource_e;

typedef enum {
	DISP_DEV_NULL,
	DISP_DEV_PANEL,
	DISP_DEV_NTSC,
	DISP_DEV_PAL,
	DISP_DEV_HDMI,

}sp1kDispDev_e;



typedef enum {
	SP1K_DISP_WAIT_FALLING = DISP_WAIT_FALLING,
	SP1K_DISP_WAIT_RISING  = DISP_WAIT_RISING,
	SP1K_DISP_WAIT_HIGH    = DISP_WAIT_HIGH,
	SP1K_DISP_WAIT_LOW     = DISP_WAIT_LOW,
} sp1kDispWaitMode_e;

/* backward compatible. */
#define SP1K_DISP_SIGNAL_RISING   SP1K_DISP_WAIT_RISING
#define	SP1K_DISP_SIGNAL_HI       SP1K_DISP_WAIT_HIGH
#define	SP1K_DISP_SIGNAL_LO       SP1K_DISP_WAIT_HIGH
#define	SP1K_DISP_SIGNAL_FALLING  SP1K_DISP_WAIT_FALLING

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
 * @fn void sp1kDispImgShow(UINT32 addr, UINT16 width, UINT16 height, UINT16 factor)
 * @brief	 Shows the specified buffer to the selected display device
 * @param[in] addr    The starting of physical buffer address in word unit
 * @param[in] width   Width of  buffer dimension
 * @param[in] height  Height of buffer dimension
 * @param[in] factor  Display zoom factor. The range is from 100(1.0x) to 400(4.0x)
 * @retval none
 * @see sp1kDispImgWinEn(), sp1kDispPbBufSet()
 */
void sp1kDispImgShow(UINT32 addr, UINT16 width, UINT16 height, UINT16 factor);

/**
 * @fn void sp1kDispImgWinEn(UINT8 enable)
 * @brief	Enables or disables display image layer
 * @param[in] enable  Enable/disable
 *\n                           0: disable
 *\n                           1: enable
 * @retval none
 * @see sp1kDispPnlOpen()
 */
void sp1kDispImgWinEn(UINT8 enable);

/**
 * @fn void sp1kDispVsyncWait(UINT8 signal)
 * @brief	Waits until next display v-sync singal arrives
 * @param[in] signal Signal waiting type,. One of the following 4 selectors is available
 *\n                      SP1K_DISP_SIGNAL_RISING: Wait for vsync changing from low to high
 *\n                      SP1K_DISP_SIGNAL_FALLING: Wait for vsync changing from high to low
 *\n                      SP1K_DISP_SIGNAL_HI: Wait for vsync changing to high
 *\n                      SP1K_DISP_SIGNAL_LO: Wait for vsync changing to low
 * @retval none
 * @see sp1kDispVvalidWait()
 */
void sp1kDispVsyncWait(UINT8 signal);

/**
 * @fn void sp1kDispVvalidWait(UINT8 signal)
 * @brief	Waits until next display v-valid singal arrives
 * @param[in] signal Signal waiting type. One of the following 4 selectors is available
 *\n                      SP1K_DISP_SIGNAL_RISING: Wait for v-valid sync changing from low to high
 *\n                      SP1K_DISP_SIGNAL_FALLING: Wait for v-valid sync changing from high to low
 *\n                      SP1K_DISP_SIGNAL_HI: Wait for v-valid sync changing to high
 *\n                      SP1K_DISP_SIGNAL_LO: Wait for v-valid sync changing to low
 * @retval none
 * @see sp1kDispVsyncWait()
 */
void sp1kDispVvalidWait(UINT8 signal);

/**
 * @fn void sp1kDispPbBufSet(UINT32 addr, UINT16 width, UINT16 height)
 * @brief	Specifies a particular image to playbak buffer
 *
 *           The function will not update display controller immediately until dispImgWinEn(1) is called.
 *           Theis function is designed for special usage. sp1kDispImgShow() works in a similar way but everything gets updated immediately
 * @param[in] addr   The starting of image buffer address in word unit
 * @param[in] width  Width of buffer dimension
 * @param[in] height Height of buffer dimension
 * @retval none
 * @see sp1kDispImgShow()
 */
void sp1kDispPbBufSet(UINT32 addr, UINT16 width, UINT16 height);

/**
 * @fn void sp1kDispPnlDimGet(UINT16 *dispH, UINT16 *dispW)
 * @brief	Obtains dimension information of current selected display device
 * @param[out] *dispH  Width of display dimension
 * @param[out] *dispW Height of display dimension
 * @retval none
 */
void sp1kDispPnlDimGet(UINT16 *dispH, UINT16 *dispW);

/**
 * @fn UINT8 sp1kDispSignalWait(UINT8 src, UINT8 mode)
 * @brief	wait for signal event.
 * @param	src = [in] one of sp1kDispWaitSource_e.
 * @param	mode = [in] one of sp1kDispWaitMode_e.
 * @retval	return = SUCCESS or FAIL.
*/
UINT8 sp1kDispSignalWait(UINT8 src, UINT8 mode);

#else
#define sp1kDispImgShow        dispShow
#define sp1kDispImgWinEn       dispImgWinEn
#define sp1kDispPbBufSet       dispPbBufSet
#define sp1kDispPnlDimGet      dispPnlDimGet
#define sp1kDispSignalWait     dispSignalWait
#define sp1kDispFitShow		dispFitShow
/* backward compatible. */
#define sp1kDispVsyncWait(signal)   dispSignalWait(DISP_WAIT_VSYNC, signal)
#define sp1kDispVvalidWait(signal)  dispSignalWait(DISP_WAIT_VVALID, signal)
#endif

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef dispCapabDesc_t  sp1kDispCapabDesc_t;

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/**
 * @fn void sp1kDispPnlOpen(UINT8 panel)
 * @brief	Switches between different display devices.
 *
 * It will turn-off currently used device and turn-on desired device. Please refer to display driver SOP for more infomation
 * @param[in] panel  One of the following selection is available
 *\n                 1: LCD panel
 *\n                 2: NTSC TV-out.
 *\n                 3: PAL TV-out
 *@retval none
 */
void sp1kDispPnlOpen(UINT8 panel);

#define sp1kDispDevActSet      dispDevActSet
#define sp1kDispDevActGet      dispDevActGet
#define sp1kDispCapabilityGet  dispCapabilityGet
#define sp1kDispPowerOn        dispPowerOn
#define sp1kDispPowerOff       dispPowerOff

/**
 * @fn void sp1kDispFitMethodSet(UINT8 method);
 * @brief	set display fitting method.
 * @param	method = one of sp1kDispFitMethod_e.
 * @retval	None.
 */
void sp1kDispFitMethodSet(UINT8 method);

/**
 * @fn UINT8 sp1kDispGammaEnableSet(UINT8 en);
 * @brief	enable/disable gamma.
 * @param	en = [in] 0: disable, 1: enable.
 * @retval	return = SUCCESS or FAIL.
 */
UINT8 sp1kDispGammaEnableSet(UINT8 en);

/**
 * @fn UINT8 sp1kDispGammaTableSet(UINT8 *gammaTbl);
 * @brief	set gamma table.
 * @param	gammaTbl = [in] pointer to gamma table, total 17 entries.
 * @retval	return = SUCCESS or FAIL.
 */
UINT8 sp1kDispGammaTableSet(UINT8 *gammaTbl);

/**
 * @fn void sp1kDispOsdHvGet(UINT32 *h0, UINT32 *v0, UINT32 *h1, UINT32 *v1)
 * @brief	Get the osd H size and V size.
 * @param	h0 = [out] the start position of H
 * @param	v0 = [out] the start position of V
 * @param	h1 = [out] the end position of H
 * @param	v1 = [out] the end position of V
 * @retval	none
*/
void sp1kDispOsdHvGet(UINT32 *h0, UINT32 *v0, UINT32 *h1, UINT32 *v1);

/**
 * @fn void sp1kDispOsdHvSet(UINT32 h0, UINT32 v0, UINT32 h1, UINT32 v1)
 * @brief	Set the osd H size and V size.
 * @param	h0 = [in] the start position of H
 * @param	v0 = [in] the start position of V
 * @param	h1 = [in] the end position of H
 * @param	v1 = [in] the end position of V
 * @retval	none
*/
void sp1kDispOsdHvSet(UINT32 h0, UINT32 v0, UINT32 h1, UINT32 v1);

/**
 * @fn        void sp1kDispImgSclUpd(void)
 * @brief     update image layer scale related parameter
 * @param     NONE
 * @retval    NONE
*/
void sp1kDispImgSclUpd(void);

/**
 * @fn        void sp1kDispOsdSclUpd(void)
 * @brief     update osd layer scale related parameter
 * @param     NONE
 * @retval    NONE
*/
void sp1kDispOsdSclUpd(void);

/**
 * @fn        void sp1kDispOsdEnSet(UINT8 en)
 * @brief     osd layer control
 * @param     [in] en: 1: enabel;  0: disable
 * @retval    NONE
*/
void sp1kDispOsdEnSet(UINT8 en);

#endif  /* __SP1K_DISP_API_H__ */

