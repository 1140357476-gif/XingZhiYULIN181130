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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "cmd.h"
#include "Uart.h"
#include <string.h> 
#include "reg_def.h"
#include "Hal_gpio.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define CMD_HISTORIES 5
#define CMD_BUF_SIZE  64 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if 1
#define cmdMainPrintf printf
#else
#define cmdMainPrintf /DBG_SLASH
#endif
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static cmd_t code* code* xdata ppCmds;

UINT8 xdata buffer[CMD_BUF_SIZE];
UINT8 xdata pos;
UINT8 xdata logIn;

UINT8 xdata histBuf[CMD_HISTORIES][CMD_BUF_SIZE];
UINT8 xdata histPos[CMD_HISTORIES];
UINT8 xdata histIns;
UINT8 xdata histOutput;
UINT8 xdata histInsWrap;
UINT8 xdata histOutputWrap;
UINT8 xdata cmdDefaultEn;

//static UINT8 xdata dataAlig[0x05]; // for data link aligment

extern UINT8 xdata Uartbuffer[];
extern cmd_t code* code pCmdArray[];
extern UINT8 xdata Uartmode;	//Kai++ 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        void cmdProcess(UINT8 *cmd, UINT8 repeating)
 * @brief     cmdProcess
 * @param     [in] cmd
 * @param     [in] repeating
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
static void
cmdProcess(
	UINT8 *cmd,
	UINT8 repeating
)
{
	cmd_t code* code* ppCmd = ppCmds;
	cmd_t code* pCmd;
	UINT32 idx = 0;
	UINT32 copy = 0;

	repeating = repeating;
	/*
	 * Strip the white space from the command.
	 */
	if ( Uartmode == UART_MODE_DEBUG) {
		while ( cmd[idx] != '\0' ) {
		if ( (cmd[idx] != ' ') &&
			 (cmd[idx] != '\t') &&
			 (cmd[idx] != '\r') &&
			 (cmd[idx] != '\n') ) {
			break;
		}
		idx++;
	}

	if ( idx > 0 ) {
		/* Reached a non-white space character, compact the string */
		while ( cmd[idx] != '\0' ) {
			cmd[copy++] = cmd[idx++];
		}
		cmd[copy] = '\0';   
	}

	/*
	 * Index points to the end of the string, move backwards.
	 */
	idx = strlen(cmd);

	while ( idx > 0 ) {
		idx--;
		if ( (cmd[idx] == ' ') || 
			 (cmd[idx] == '\t') ||
			 (cmd[idx] == '\r') ||
			 (cmd[idx] == '\n') ) {
			cmd[idx] = '\0';
		}
		else {
			break;
		}
	}

	/*
	 * Find the command.
	 */
	idx = 0;
	
	// add by phil for correcting the call tree for function pointer S	
	if (idx) {
		ppCmd = pCmdArray;
	}
	// add by phil for correcting the call tree for function pointer E	
	
	while ( cmd[idx] != '\0' ) {
		if ( (cmd[idx] == ' ') ||
			 (cmd[idx] == '\t') ||
			 (cmd[idx] == '\r') ||
			 (cmd[idx] == '\n') ) {
			break;
		}
		idx++;
	}
	cmd[idx] = '\0';
	}
	/*to do copy uart data to specified buf+++++*/
	#ifdef  UART_FUNC	
	if ( Uartmode == UART_MODE_COMMUNICATE) {
       	strcpy(Uartbuffer,buffer);
        osMsgPost(SP1K_MSG_UART_CMD);
	}
	#endif
	/* to do copy uart data to specified buf ------*/        

	if ( Uartmode == UART_MODE_DEBUG) {
		while( *ppCmd != NULL) {
			pCmd = *ppCmd;
			while ( pCmd->cmd != NULL ) {
				if ( strcmp(pCmd->cmd, cmd) == 0 ) {
					//switchbank(10);//
					pCmd->phandler(cmd + idx + 1);
					//(ppCmd->phandler)();
					return;
				}
				pCmd++;
			}
			ppCmd++;
		}
	}
	if ( cmdDefaultEn == 0 ) {
		cmdMainPrintf("command '%s' not found!\n", cmd);
	}
	else {
		//cmdCustomDef(cmd);
	}
}


/**
 * @fn        UINT32 cmdIdxIncrease(UINT8 *pcmdIdx)
 * @brief     cmdIdxIncrease
 * @param     [in] pcmdIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
cmdIdxIncrease(
	UINT8 *pcmdIdx
)
{
	UINT8 localIdx;
	UINT8 ret = 0;

	localIdx = *pcmdIdx;
	localIdx++;
	if ( localIdx == CMD_HISTORIES ) {
		localIdx = 0;
		ret = 1;
	}
	*pcmdIdx = localIdx;

	return ret;
}

/**
 * @fn        UINT32 cmdFlushCopy(UINT8 cursorPos, UINT8 *pcmdBuf, UINT8 *pcmdSrc, UINT8 cmdLen)
 * @brief     cmdFlushCopy
 * @param     [in] cursorPos
 * @param     [in] pcmdBuf
 * @param     [in] pcmdSrc
 * @param     [in] cmdLen
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
cmdFlushCopy(
	UINT8 cursorPos,
	UINT8 *pcmdBuf,
	UINT8 *pcmdSrc,
	UINT8 cmdLen
)
{
	if ( cursorPos > 0 ) {
		for ( ; cursorPos > 0; cursorPos-- ) {
			cmdMainPrintf("\b \b");
			buffer[cursorPos] = '\0';
		}
	}
	memcpy(pcmdBuf, pcmdSrc, cmdLen);

	return 0;
}

/**
 * @fn        void cmdMonitor(void* ptr)
 * @brief     cmdMonitor
 * @param     [in] ptr
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
void
cmdMonitor(
	void* ptr
)
{
	UINT8 c;
	UINT8 repeating;
	UINT8 histDownArw;
	static UINT8 upArrowCnt;
	UINT8 temp;
	UINT8 log27BD;
	
	UINT8 gpio4FuncGet, gpio3FuncGet;

	/* 
		芯鼎说GPIO3在某些时候会无触发导致UART RX中断执行一些指令，尤其如果在录像时会导致时机 
		这里判断一下，一般我们如果要用UART RX，UART TX也会使用，所以判断，如果TX或者RX中的任何一个脚位设置为GPIO就不接受RX数据
	*/
	gpio4FuncGet = HAL_GpioByteFuncGet(HAL_GPIO_BYTE_GEN0, (1 << (4 & 0x07)));
	gpio3FuncGet = HAL_GpioByteFuncGet(HAL_GPIO_BYTE_GEN0, (1 << (3 & 0x07)));

	if (gpio4FuncGet || gpio3FuncGet) {
		return;
	}
	
    if (RI) {
        while (!TI);
        RI = 0;
    } else {
        return;
    }
	ptr = ptr;

	/* save parioty setting */
    log27BD = XBYTE[REG_Dram_Priority0_lvl2_master];
    XBYTE[REG_Dram_Priority0_lvl2_master] = 0x02; // cpu first
    
	/*	if ( kbhit() ) {*/
		c = SBUF0;
    
#ifdef UART_FUNC    
	if ( Uartmode == UART_MODE_COMMUNICATE) {
               UartAckProcess(c);
               temp=1;
	}
	else
#endif        
               temp=logIn;
		if ( !temp/*logIn*/ ) {
			if ( c == '\r' ) {
				logIn = TRUE;
				cmdMainPrintf("\n\ncmd>");
			}
		}
		else {
			switch ( c ) {
			case '\b':
			case '\x7f':
				if ( Uartmode==UART_MODE_DEBUG ) {
					if ( pos > 0 ) {
						cmdMainPrintf("\b \b");
						pos--;
					}
					buffer[pos] = '\0';
				}
				break;

			case '\r':  /* Process the command. */
				cmdMainPrintf("\n");
				if ( pos ) {
					/*
					 * Do not place the same last command into the history if the same.
					 */
					if ( strcmp((UINT8 *)histBuf[histIns], buffer) ) {
						if ( cmdIdxIncrease(&histIns) == 1 ) {
							histInsWrap = 1;
						}
						memcpy(histBuf[histIns], buffer, CMD_BUF_SIZE);
						histPos[histIns] = pos;
					}
					histOutput = histIns;
					histOutputWrap = 0;
					upArrowCnt = 0;
					repeating = FALSE;
				} 
				if ( pos ) {
					cmdProcess(buffer, repeating);
					pos = 0;
					memset(buffer, 0, CMD_BUF_SIZE);
					cmdMainPrintf("\n");
				}
				cmdMainPrintf("cmd>");
				break;
			case '<': /* Key: '<' arrow */
				if ( Uartmode==UART_MODE_DEBUG ) {
					if ( histOutputWrap == 1 ) {
						if ( histOutput == histIns ) {
							break;
						}
					}
					if ( histInsWrap == 0 ) {
						if ( histOutput == 0 ) {
							break;
						}
					}
					upArrowCnt++;
					cmdFlushCopy(
						pos,
						buffer,
						(UINT8 *)histBuf[histOutput],
						histPos[histOutput]
					);
					pos = histPos[histOutput];
					buffer[pos + 1] = '\0';
					cmdMainPrintf(buffer);
					if ( histInsWrap == 1 ) {
						if ( histOutput == 0 ) {
							histOutput = CMD_HISTORIES - 1;
							histOutputWrap = 1;
						}
						else {
							histOutput--;
						}
					}
					else {
						if ( histOutput != 0 ) {
							/* Note that when wrap around does not occur, the least
							 * of index is 1 because it is the first one to be
							 * written.
							 */
							histOutput--;
						}
						/* Nothing to do with histOutput == 0,
						 * because there is no more commands.
						 */
					}
				}
				break;
			
			case '>': /* Key: '>' arrow */
				if ( Uartmode==UART_MODE_DEBUG ) {
					if ( upArrowCnt <= 1 ) {
						break;
					}
					upArrowCnt--;
					cmdIdxIncrease(&histOutput);
					histDownArw = histOutput;
					cmdIdxIncrease(&histDownArw);
					cmdFlushCopy(
						pos,
						buffer,
						(UINT8 *)histBuf[histDownArw],
						histPos[histDownArw]
					);
					pos = histPos[histDownArw];
					buffer[pos + 1] = '\0';
					cmdMainPrintf(buffer);
				}
				break;
			
			case '[': /* Non ASCII characters, arrow. */
				if ( Uartmode==UART_MODE_DEBUG ) {
					c = SBUF0;
					switch ( c ) {	
					case 'C': /* Key: right arrow */
						break;
					case 'D': /* Key: left arrow */
						break;
					default:
						break;
					}
				}
				break;
			
			default:
				if ( (pos < (CMD_BUF_SIZE - 1)) && (c >= ' ') && (c <= 'z') ) {
					buffer[pos++] = c;
					buffer[pos] = '\0';
					cmdMainPrintf(buffer + pos - 1);
				}
				if ( c == '\x7e' ) {
					buffer[pos++] = c;
					buffer[pos] = '\0';
					cmdMainPrintf(buffer + pos - 1);
				}
				break;
			}
		} /* else of if !logged_in */
/*	}*/ /* if kbhit_k() */

	 XBYTE[REG_Dram_Priority0_lvl2_master] = log27BD; // restore setting
}
/**
 * @fn        cmd_t cmdRegister(code* ppCmd)
 * @brief     cmdRegister
 * @param     [in] ppCmd
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-12-15
 * @todo      N/A
 * @bug       N/A
*/
void cmdRegister(cmd_t code* code* ppCmd)
{
	ppCmds = ppCmd;
}

/**
 * @fn        void cmdInit(void)
 * @brief     Initial cmd
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
void cmdInit(void)
{
	cmdRegister(pCmdArray);
}

