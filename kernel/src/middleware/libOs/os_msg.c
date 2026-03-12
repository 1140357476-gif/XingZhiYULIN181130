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
#include "general.h"
#include "os_msg.h"
#include "string.h"
#define SYS_MSG_INTERNAL // for libSys internal use
#include "func_msg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MSG_BUF_MAX  10
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
**************************************************************************/
static msg_t xdata msgBuf[MSG_CTRL_TYPE_MAX][MSG_BUF_MAX];

static MainMsgQue_t xdata msgQueCtrl[MSG_CTRL_TYPE_MAX];
//static BIT data isMsgNull[MSG_CTRL_TYPE_MAX];


/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern SYS_MSG_DCL pFuncMsg code funcMsgRcvArray[];

/**
 * @fn        void osMsgInit(void)
 * @brief     osMsgInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void osMsgInit (void)
{
	UINT8 i;
	MainMsgQue_t* pMsg;

	for (i = 0; i < MSG_CTRL_TYPE_MAX; i++) {
		pMsg = &msgQueCtrl[i];
		
		pMsg->QueStart = &msgBuf[i][0];
		pMsg->QueEnd = &msgBuf[i][MSG_BUF_MAX];
		pMsg->QueIn = &msgBuf[i][0];
		pMsg->QueOut = &msgBuf[i][0];
		pMsg->QueEntries = 0;
		pMsg->QueSize = MSG_BUF_MAX;
		pMsg->MsgBuf = &msgBuf[i][0];
		pMsg->bLock = 0;
		
		memset(pMsg->MsgBuf, 0, sizeof(msg_t) * (UINT32)MSG_BUF_MAX);
		//isMsgNull[i] = TRUE;
	}
}

/**
 * @fn        UINT8 osMsgQuery(void)
 * @brief     osMsgQuery
 * @param     NONE
 * @retval    return = TRUE / FALSE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 osMsgQuery (void)
{
	UINT8 i;
	MainMsgQue_t* pMsg;
	UINT8 ret;

	ret = FALSE;
	for (i = 0; i < MSG_CTRL_TYPE_MAX; i++) {
		pMsg = &msgQueCtrl[i];
		if (pMsg->QueEntries != 0) {
			//printf("type:%bu, msg cnt:%u\n", i, pMsg->QueEntries);
			ret = TRUE;
			break;
		}
	}
	
	return ret;
}

/**
 * @fn        UINT8 osMsgPost(UINT16 msg)
 * @brief     osMsgPost
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 osMsgPost (UINT16 msg)
{
	UINT8 status;
	UINT16 msgGrp;
	MainMsgQue_t* pMsg;

	msgGrp = (msg & MSG_CTRL_IDX_GROUP_MASK) >> MSG_CTRL_IDX_GROUP_SHIFT;
	if (msgGrp >= MSG_CTRL_TYPE_MAX) {
		return FAIL;
	}
	
	ENTER_CRITICAL(status);
	
	pMsg = &msgQueCtrl[msgGrp];	
	if (pMsg->QueEntries >= pMsg->QueSize) {
		//printf( "MsgQue over flow!\n" );
		EXIT_CRITICAL(status);
		return FAIL;
	}
	
	//isMsgNull[msgGrp] = FALSE;
	*pMsg->QueIn++ = msg;
	pMsg->QueEntries++;
	if (pMsg->QueIn == pMsg->QueEnd) {
		pMsg->QueIn = pMsg->QueStart;
	}
	
	EXIT_CRITICAL(status);
	return SUCCESS;
}

/**
 * @fn        UINT8 osMsgPostForce(UINT16 msg)
 * @brief     osMsgPostForce
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 osMsgPostForce (UINT16 msg)
{
	UINT8 status;
	UINT16 msgGrp;
	MainMsgQue_t* pMsg;

	msgGrp = (msg & MSG_CTRL_IDX_GROUP_MASK) >> MSG_CTRL_IDX_GROUP_SHIFT;
	if (msgGrp >= MSG_CTRL_TYPE_MAX) {
		return FAIL;
	}
		
	ENTER_CRITICAL(status);
	
	pMsg = &msgQueCtrl[msgGrp];
	if (pMsg->QueEntries >= pMsg->QueSize) {
		//isMsgNull[msgGrp] = FALSE;
		*pMsg->QueIn = msg;
		//printf( "MsgQue over flow!\n" );
		EXIT_CRITICAL(status);
		return FAIL;
	}
	
	//isMsgNull[msgGrp] = FALSE;
	*pMsg->QueIn++ = msg;
	pMsg->QueEntries++;
	if (pMsg->QueIn == pMsg->QueEnd) {
		pMsg->QueIn = pMsg->QueStart;
	}
	
	EXIT_CRITICAL(status);
	return SUCCESS;
}


//-----------------------------------------------------------------------------
//osMsgPostFront
//-----------------------------------------------------------------------------

/**
 * @fn        UINT8 osMsgPostFront(UINT16 msg)
 * @brief     osMsgPostFront
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
UINT8 osMsgPostFront (UINT16 msg)
{
	UINT8 status;
	UINT16 msgGrp;
	MainMsgQue_t* pMsg;

	msgGrp = (msg & MSG_CTRL_IDX_GROUP_MASK) >> MSG_CTRL_IDX_GROUP_SHIFT;
	if (msgGrp >= MSG_CTRL_TYPE_MAX) {
		return FAIL;
	}
	
	ENTER_CRITICAL(status);

	pMsg = &msgQueCtrl[msgGrp];
	if (pMsg->QueEntries >= pMsg->QueSize) {
		//printf( "MsgQue over flow!\n" );
		EXIT_CRITICAL(status);
		return FAIL;
	}
	//isMsgNull[msgGrp] = FALSE;
	if (pMsg->QueOut == pMsg->QueStart) {
		pMsg->QueOut = pMsg->QueEnd;
		*pMsg->QueOut = msg;
	} else {
		*(--pMsg->QueOut) = msg;
	}
	pMsg->QueEntries++;
	
	EXIT_CRITICAL(status);
	return SUCCESS;
}

/**
 * @fn        UINT8 osMsgWait(UINT16 msg)
 * @brief     osMsgWait
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 osMsgWait(UINT16 msg)
{
	while(osMsgSeek(msg) == FALSE);
	return SUCCESS;
}

/**
 * @fn        UINT8 osMsgAccept(UINT16* msg)
 * @brief     osMsgAccept
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 osMsgAccept (UINT16* msg)
{
	UINT8 i, ret;
	MainMsgQue_t* pMsg;
	
	//printf( "\n>>>>>>>>>osMsgAccept<<<<<<<<<<\n\n" );
	ret = FAIL;

	/* msg accept poroity "System" >> "Button" >> "Host" */
	for (i = MSG_CTRL_TYPE_HOST; i < MSG_CTRL_TYPE_MAX; i++) {
		pMsg = &msgQueCtrl[i];
		if (pMsg->QueEntries != 0) {
			if (msg) {
				*msg = *pMsg->QueOut;
			}
			*pMsg->QueOut++ = SP1K_MSG_NULL;
			pMsg->QueEntries--;
			if (!pMsg->QueEntries) {
				//isMsgNull[msgGrp] = TRUE;
			}
			if (pMsg->QueOut == pMsg->QueEnd) {
				pMsg->QueOut = pMsg->QueStart;
			}
			ret = SUCCESS;
			break;
		}
	}
	
	if (ret != SUCCESS) {
		if (msg) {
			*msg = SP1K_MSG_NULL;
		}
	}
	
	return ret;
}

/**
 * @fn        void osMsgFlush(msgCtrlFlush_t msgGrp)
 * @brief     osMsgFlush
 * @param     [in] msgGrp
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
#pragma disable //disable interrupt
void osMsgFlush (UINT8 msgGrp)
{
	UINT8 status;
	MainMsgQue_t* pMsg;
	UINT8 i;

	ENTER_CRITICAL(status);
	
	for (i = 0; i < MSG_CTRL_TYPE_MAX; i++) {
		pMsg = &msgQueCtrl[i];
		
		//In MSG lock mode, this operation will be abort!
		if (pMsg->bLock || (msgGrp & (0x01 << i)) == 0x00) {
			continue;
		}
		
		pMsg->QueIn = pMsg->QueStart;
		pMsg->QueOut = pMsg->QueStart;
		pMsg->QueEntries = 0;
		memset(pMsg->MsgBuf, 0, sizeof(msg_t) * (UINT32)MSG_BUF_MAX);
		//isMsgNull[i] = TRUE;
	}
	
	EXIT_CRITICAL(status);
}

/**
 * @fn        UINT8 osMsgSeek(UINT16 msg)
 * @brief     osMsgSeek
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 osMsgSeek(UINT16 msg)
{
	UINT8 i, j;

	for (i = 0; i < MSG_CTRL_TYPE_MAX; i++) {
		for (j = 0; j < MSG_BUF_MAX; j++) {
			if (msgBuf[i][j] == msg) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

/**
 * @fn        UINT8 osMsgIntrSeek(UINT16 msg)
 * @brief     osMsgSeek
 * @param     [in] msg
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 osMsgIntrSeek(UINT16 msg)
{
	UINT8 i, j;

	for (i = 0; i < MSG_CTRL_TYPE_MAX; i++) {
		for (j = 0; j < MSG_BUF_MAX; j++) {
			if (msgBuf[i][j] == msg) {
				return TRUE;
			}
		}
	}

	return FALSE;
}


/**
 * @fn        void osMsgLock(UINT8 bLock, msgCtrlFlush_t msgGrp)
 * @brief     osMsgLock
 * @param     [in] bLock
 * @param     [in] msgGrp
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void osMsgLock(UINT8 bLock, msgCtrlFlush_t msgGrp)
{
	UINT8 i;
	
	for (i = 0; i < MSG_CTRL_TYPE_MAX; i++) {
		//In MSG lock mode, this operation will be abort!
		if ((msgGrp & (0x01 << i)) != 0x00) {
			msgQueCtrl[i].bLock = bLock;
		}
	}
}

/**
 * @fn        void osMsgRcv(void)
 * @brief     osMsgRcv
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void osMsgRcv(void)
{
	pFuncMsg* pf = &funcMsgRcvArray[0];

	/* call the messsage receive controller */
	if (*pf != NULL) {
		(*pf)();
	}
}

