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
 * @file		tmr_svc.c
 * @brief		
 * @author		Phil Lin
 * @since		2009-11-25
 * @date		2009-11-25
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "os_msg.h" 
#include "app_msg.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APO_RELOAD_VAL_DEF  6000UL
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 xdata sbAPOEn = 1; // APO: Auto Power Off
static UINT32 xdata sdwAPOTimeVal;
static UINT32 xdata sdwAPOTimeReload = APO_RELOAD_VAL_DEF;

static UINT8 xdata sbPwrOffProc;
static UINT32 xdata sdwPwrOffProcVal = 1;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @brief	tmrSvc_APO_Intr
 * @param	
 * @param	
 * @retval	None.
 * @see
 * @author	Phil Lin
 * @since	2010-01-15
 * @todo	N/A
 * @bug		N/A
*/
/**
 * @fn        void tmrSvc_APO_Intr(void)
 * @brief     tmrSvc_APO_Intr
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
void tmrSvc_APO_Intr(void)
{
	if(sbAPOEn) {
		sdwAPOTimeVal--;
		if (sdwAPOTimeVal == 0 && !prjDbgMode) {
			osMsgPost(SP1K_MSG_POWER_OFF);
            sdwAPOTimeVal = sdwAPOTimeReload;
		}
	}   
}

/**
 * @fn        void tmrSvc_PowerOffProc_Intr(void)
 * @brief     tmrSvc_PowerOffProc_Intr
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
void tmrSvc_PowerOffProc_Intr(void)
{
	if (sbPwrOffProc) {
		if (!(--sdwPwrOffProcVal)) {
			sdwPwrOffProcVal = 1;
			osMsgPost(SP1K_MSG_BAT_EMPTY);
		}
	}    
}

/**
 * @fn        void sp1kTmrSvc_APO_EnSet(UINT8 enable)
 * @brief     Auto power off enable set
 * @param     [in] enable
                   1: enabled
                   0: disabled
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kTmrSvc_APO_EnSet(UINT8 enable)
{
    if (enable) {
        sbAPOEn = 1;
    } else {
        sbAPOEn = 0;
    }
}

/**
 * @fn        void sp1kTmrSvc_APO_ReloadSet(UINT32 val)
 * @brief     Auto power off reload time set
 * @param     [in] val :by 10 mini second
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kTmrSvc_APO_ReloadSet(UINT32 val)
{
    sdwAPOTimeReload = val;
}

/**
 * @fn        void sp1kTmrSvc_APO_Rst(void)
 * @brief     Reset the auto power off conter
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kTmrSvc_APO_Rst(void)
{
    sdwAPOTimeVal = sdwAPOTimeReload;
}

/**
 * @fn        void sp1kTmrSvc_PowerOffCfg(UINT8 enable, UINT32 delay)
 * @brief     Configure delay time for power off
 * @param     [in] enable 
                   1: enable the auto power off
                   0: diable the auto power off process
 * @param     [in] delay :by 10 mini second
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kTmrSvc_PowerOffCfg(UINT8 enable, UINT32 delay)
{
	if (enable) {
		sbPwrOffProc = 1;
		if (!delay) {
			/*if delay == 0, power off immediately*/
			sdwPwrOffProcVal = 1;
		} else {
			sdwPwrOffProcVal = delay;
		}
	} else {
		sbPwrOffProc = 0;
	}    
}

/**
 * @fn        UINT8 sp1kTmrSvc_PowerOffChk(void)
 * @brief     check if the power off process is running or not
 * @param     NONE
 * @retval    return 
              1: the power off process is running
              0: the power off process is stopped
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-3-17
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kTmrSvc_PowerOffChk(void)
{
    return sbPwrOffProc;
}
