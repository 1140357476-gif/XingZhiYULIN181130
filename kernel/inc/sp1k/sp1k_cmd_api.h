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
#ifndef __SP1K_CMD_API_H__
#define __SP1K_CMD_API_H__

/**
 * @file      sp1k_cmd_api.h
 * @brief     command line related API definition
*/

#include "cmd.h"
#include "Uart.h"
#include "sp1k_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
	
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* kai++*/
#ifdef  SP1K_DOXY_GEN
/**
 * @fn void UartBaudrateSet(UINT32 Baudrate);
 * @brief	configure the Baudrate and start Baudrate generator
 *			there are three param to be chosen .
 *			the upmost baudrate is 115200 bps
 * @param[in] Baudrate	0; BAUDRATE_19200
 *						1: BAUDRATE_57600
 *						2: BAUDRATE_115200
 *						default: BAUDRATE_57600
 * @retval none
 */
void UartBaudrateSet(UINT32 Baudrate);


/**
 * @fn void UartModeSet(UINT8 mode);
 * @brief	to set the UART mode,there are three modes to be chosen
 * @param   mode:	   0; UART_MODE_DEBUG; 
 *	                        1; UART_MODE_COMMUNICATE;
 *	                        2; UATR_MODE_DISABLE;            
 * @retval	None.
*/
void UartModeSet(UINT8 mode);


/**
 * @fn void UartSend(char message_s);
 * @brief	 to output a character   
 * @param   message_s:    the type should be in char                   
 * @retval	None.
*/
void UartSend(char message_s);


/**
 * @fn void UartSend_Str(UINT8 *P);
 * @brief	   to output  character string
 *              will automatically end of '\r'
 * @param   *P:  Pointer to the string buff   
 * @retval	None
*/
void UartSend_Str(UINT8 *P);


/**
 * @fn UINT8 * UartDataGet(void);
 * @brief	   to get the comand buff address
 *              the command must be end of '\r'
 * @param   None   
 * @retval    the first command buff address
*/
UINT8 * UartDataGet(void);


/**
 * @fn void UartAckModeSet(UINT8 mode);
 * @brief	to set the ACK mode,there are three modes to be chosen
 * @param   mode: 0; ACK_MODE_NULL;
 *				 1; ACK_MODE_ENTER;
 *				 2; ACK_MODE_CHAR;
 *                          defaulT:null     
 * @retval    None
*/
void UartAckModeSet(UINT8 mode);

#else
#define sp1kUartBaudrateset   UartBaudrateSet
#define sp1kUartModeset         UartModeSet
#define sp1kUartSend               UartSend
#define sp1kUartSend_Str        UartSend_Str
#define sp1kUartDataget          UartDataGet
#define sp1kUartAckset            UartAckModeSet
#endif
/*kai--*/
#define sp1kCmdInit		cmdInit
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

#endif //__SP1K_CMD_API_H__


