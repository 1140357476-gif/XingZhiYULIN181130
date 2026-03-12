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
 * @file      uart.h
 * @brief     the head file of uart
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __HAL_UART_H__
#define __HAL_UART_H__
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/******FOR UART FUNCTION****************/
#if 0

#define UART_FUNC 

#endif
/***************************************/ 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (code* pUartHandle)(void); 
typedef enum _BAUDRATE{
	BAUDRATE_19200 = 0x00,
	BAUDRATE_57600 = 0x01,	
	BAUDRATE_115200 = 0x02,
	BAUDRATE_MAX = 0x03,
} _BAUDRATE_t;

typedef enum _UART_MODE{
	UART_MODE_DEBUG = 0x00,
	UART_MODE_COMMUNICATE = 0x01,
	UART_MODE_DISABLE = 0x02,
	UART_MODE_MAX = 0x03,
} _UARTMODE_t;

typedef enum _ACK_MODE{
	ACK_MODE_NULL = 0x00,
	ACK_MODE_ENTER = 0x01,
	ACK_MODE_CHAR = 0x02,
	ACK_MODE_MAX = 0X03,
} _ACKMODE_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void uartIntrRoutine(void);
void UartBaudrateSet(UINT32 Baudrate);
void UartModeSet(UINT8 mode);
void UartSend(char message_s);
void UartSend_Str(UINT8 *P);
void UartAckProcess(char c);
UINT8 * UartDataGet(void);
void UartAckModeSet(UINT8 mode);

#endif /*__HAL_UART_H__*/
