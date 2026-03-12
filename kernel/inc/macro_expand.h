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
 * @file		macro_expand.h
 * @brief		device driver macro expand header file.
 * @author		Matt Wang
 * @since		2008-05-28
 * @date		2008-10-13
 */

#ifndef __MACRO_EXPAND_H_DEV__
#define __MACRO_EXPAND_H_DEV__

#include "host_func_opt.h"

/*
----------------------------------------------------------------------------
                                    DISPLAY
----------------------------------------------------------------------------
*/
/* device #1. */
#define sp1kDispPanelInit()  dispPanelOpen()
UINT8 dispPanelOpen(void);

/* device #2. */
#if FUNC_HOST_TV
#define sp1kDispTvNtscInit()  dispNtscOpen()
UINT8 dispNtscOpen(void);
#else
#define sp1kDispTvNtscInit()
#endif /* FUNC_HOST_TV */

/* device #3. */
#if FUNC_HOST_TV
#define sp1kDispTvPalInit()  dispPalOpen()
UINT8 dispPalOpen(void);
#else
#define sp1kDispTvPalInit()
#endif /* FUNC_HOST_TV */

/* device #4. */
#if FUNC_HOST_HDMI
#define sp1kDispHdmiInit()  hdmiOpen()
UINT8 hdmiOpen(void);
#else
#define sp1kDispHdmiInit()
#endif /* FUNC_HOST_HDMI */

/*
----------------------------------------------------------------------------
                                     FRONT
----------------------------------------------------------------------------
*/
/* device #1. */
#define sp1kFrontSensorInit()  frontSensorOpen()
UINT8 frontSensorOpen(void);

#endif  /* __MACRO_EXPAND_H_DEV__ */

