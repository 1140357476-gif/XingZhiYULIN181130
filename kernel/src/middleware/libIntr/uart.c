
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
 * @file      uart.c
 * @brief     Module related to Uart
 * @author    Phil
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "uart.h"
#include "hal_global.h"//for uart baudset
#include "Common.h"
#include "Intr.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define UART_BUF_SIZE 32 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8  xdata Uartmode=0;	 //kai++
UINT8  xdata UartSendEnable=0;	//kai++
UINT8  xdata Uartbuffer[UART_BUF_SIZE];
static UINT8  xdata UartAckmode;
extern UINT8 xdata testBtn;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/



//-----------------------------------------------------------------------------
//uartIntrRoutine
//-----------------------------------------------------------------------------
/**
 * @fn        void uartIntrRoutine(void)
 * @brief     uartIntrRoutine
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void uartIntrRoutine(void)
{	
	/* 
		Disable ES0, so the uart interrupt service handle 
		only once in each main loop 
	*/
	ES0 = 0;
	
	if (RI) {
		RI = 0;
#if 0//CMD_BTN_TEST		
		testBtn = 1;
#endif
	}

	/*
		The TI can't be cleared because the function "putchar"
		will wait TI ready before char sending.
	*/
	//TI = 0;
	
	//ES0 = 1;
}

/*kai++2009.02.26*/
//-------------------------------------------------------------------------------
//UART communication
//-------------------------------------------------------------------------------
#ifdef UART_FUNC

/**
 * @fn        void UartBaudrateSet(UINT32 Baudrate)
 * @brief     set Uart Baudrate. there are three param to be chosen 
 * @param     [in] Baudrate: 
 *				0: BAUDRATE_19200
 *              1: BAUDRATE_57600
 *              2: BAUDRATE_115200
 *				default: BAUDRATE_57600
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void UartBaudrateSet(UINT32 Baudrate)
{
	UINT32 baudrate_temp;
		
	if ( Baudrate < BAUDRATE_MAX ) {
		if ( Baudrate == BAUDRATE_19200 ) {
			baudrate_temp = 19200UL;
		}	
		else if ( Baudrate == BAUDRATE_57600 ) {
			baudrate_temp = 57600UL;
		}	
		else if ( Baudrate == BAUDRATE_115200 ) {
			baudrate_temp = 115200UL;
		}	
		else {
			return;
		}	
	}
	else {
		return;
	}	
	
	TR2= 0;
	T2CON = 0x30;	//set Timer2 at mode 3
	HAL_GlobalBaudRateSet(baudrate_temp);	//set braudrat,reload Rcap2L
	RCAP2H = 0xFF;  
	TR2 = 1;
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------

/**
 * @fn        void UartModeSet(UINT8 mode)
 * @brief     set Uart mode ,there are three modes to be chosen
 * @param     [in] mode:  0; UART_MODE_DEBUG; 
 *	                      1; UART_MODE_COMMUNICATE;
 *	                      2; UATR_MODE_DISABLE;  
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void UartModeSet(UINT8 mode)
{
	if (mode < UART_MODE_MAX)
		Uartmode = mode;
  
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------

/**
 * @fn        void UartSend(char message_s)
 * @brief     to output a character 
 * @param     [in] message_s: the type should be in char
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-18
 * @todo      N/A
 * @bug       N/A
*/
void UartSend(char message_s)
{
	UINT8 i;
	
	ENTER_CRITICAL(i);
	UartSendEnable = 1;
	
	putchar(message_s);

	UartSendEnable = 0;
	EXIT_CRITICAL(i);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------

/**
 * @fn        void UartSend_Str(UINT8 *P)
 * @brief     to output  character string
 * @param     [in] *p:  Pointer to the string buff
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
void UartSend_Str(UINT8 *P)
{
	UINT8 num = 0;
    
	while ( (P[num] !='\r') && (P[num] !='\0') )
   	{
          UartSend(P[num] );
              
	   num++;
		
        } 
   	UartSend('\r');
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------

/**
 * @fn        UINT8 UartDataGet(void)
 * @brief     get the comand buff address
 *            the command must be end of '\r'
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
UINT8 * UartDataGet(void)
{
	UINT8  * add; 
	
	add = Uartbuffer;
	return  (add);
}

//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------

/**
 * @fn        void UartAckModeSet(UINT8 mode)
 * @brief     set the ACK mode,there are three modes to be chosen
 * @param     [in] mode: 
 *					0; ACK_MODE_NULL;
 *					1; ACK_MODE_ENTER;
 *					2; ACK_MODE_CHAR;
 *					default: ACK_MODE_NULL
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
void UartAckModeSet(UINT8 mode)
{
	if ( mode < ACK_MODE_MAX ) 
		UartAckmode = mode;
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------

/**
 * @fn        void UartAckProcess(char c)
 * @brief     UartAckProcess
 * @param     [in] c
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
void UartAckProcess(char c)
{
	switch ( UartAckmode ) {
   	case ACK_MODE_NULL:
		break;
	case ACK_MODE_ENTER:
		UartSend('\r');
		break;
	case ACK_MODE_CHAR:
		UartSend(c);
		break;
	default:
		break;
   	}
}
#endif
/*kai--2009.02.26*/
