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

#ifndef __SP1K_TMR_SVC_H__
#define __SP1K_TMR_SVC_H__

/**
 * @file      sp1k_tmr_svc.h
 * @brief     timer service for Auto Power Off and Power Off status checking
*/

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

/**
 * @fn        void sp1kTmrSvc_APO_EnSet(UINT8 enable)
 * @brief     Auto power off enable set
 * @param     [in] enable
                   1: enabled
                   0: disabled
 * @retval    NONE
*/
void sp1kTmrSvc_APO_EnSet(UINT8 enable);

/**
 * @fn        void sp1kTmrSvc_APO_ReloadSet(UINT32 val)
 * @brief     Auto power off reload time set
 * @param     [in] val :by 10 mini second
 * @retval    NONE
*/
void sp1kTmrSvc_APO_ReloadSet(UINT32 val);

/**
 * @fn        void sp1kTmrSvc_APO_Rst(void)
 * @brief     Reset the auto power off conter
 * @param     NONE
 * @retval    NONE
*/
void sp1kTmrSvc_APO_Rst(void);

/**
 * @fn        void sp1kTmrSvc_PowerOffCfg(UINT8 enable, UINT32 delay)
 * @brief     Configure delay time for power off
 * @param     [in] enable 
                   1: enable the auto power off
                   0: diable the auto power off process
 * @param     [in] delay :by 10 mini second
 * @retval    NONE
*/
void sp1kTmrSvc_PowerOffCfg(UINT8 enable, UINT32 delay);

/**
 * @fn        UINT8 sp1kTmrSvc_PowerOffChk(void)
 * @brief     check if the power off process is running or not
 * @param     NONE
 * @retval    return 
              1: the power off process is running
              0: the power off process is stopped
*/
UINT8 sp1kTmrSvc_PowerOffChk(void);

#endif  /* __SP1K_TMR_SVC_H__ */



