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

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "intr.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define INTR_CALLBACK_ROUTINE    0
#define LCM_TE_EN	0
/* 
 This definition MUST NOT be modified!
 The function "intrCBService" is ONLY for function pointer used.
 */
#define INTERNAL_ONLY   static
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
INTERNAL_ONLY UINT8 intrCBService(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if INTR_CALLBACK_ROUTINE 
pIntrFunc code intrPtrFunc = intrCBService;
#else
pIntrFunc code intrPtrFunc = NULL;
#endif
/**
 * @brief	intrCBService
 * @param	None.
 * @retval	return = TRUE / FALSE.
 * @see
 * @author	Phil Lin
 * @since	2009-12-16
 * @todo	N/A
 * @bug		N/A
*/

INTERNAL_ONLY
UINT8 intrCBService(
    void
) using 1
{
/*
    This function is called by interrupt routine service.

    The return value could determine if the interrupt routine service should 
    be gone thought or not. That means:
    if the return value is TRUE, the interrupt routine will exit after 
    call the function "intrCBService".
    if the return value is FALSE, the interrupt routine will go on after 
    call the function "intrCBService".   
*/

/*
    Note:
    1. MUST NOT call any functions/APIs which not typed of "INTERNAL_ONLY" in this function! 
       The function likes "printf" is also prohibited!
    2. Please use register based code / global software flag / flow control in this function
    3. You could refer to the sample function in this file
*/
		#if LCM_TE_EN	
		if(XBYTE[REG_Disp_LcmAutoCmd30] & 0x08 && XBYTE[REG_Disp_ImgVOddIniEn] & 0x04)
		{

			XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
			XBYTE[0x2013] &= ~0x10;
			
		}
		#if 0  /*Gpio intr process */
 		else if(((XBYTE[0x2052] & 0x02) == 0x02) && //Global timer interrupt event	gpio9
				((XBYTE[0x204A] & 0x02) == 0x02))
		{
	
			XBYTE[0x204A]&= ~0x02;

			XBYTE[0x2013] &= ~0x10;
			XBYTE[0x2013] |= 0x10;
			XBYTE[0x2013] |= 0x10;
			XBYTE[REG_Disp_GpioRIntEn3] |= 0x01;

		}
		#endif
		#endif
		
    return FALSE;   
}



#ifdef INTR_SAMPLE_CODE
/* This function A could be called by "intrCBService" */
INTERNAL_ONLY UINT8 intrSampleA(void)
{
    return intrSampleB();
}

/* This function B could be called by "intrCBService" */
INTERNAL_ONLY UINT8 intrSampleB(void)
{
    return 0;
}

/* 
 This function C could NOT be called by "intrCBService" 
 Because the function C call the function "printf" is not typed of "INTERNAL_ONLY".
 */
INTERNAL_ONLY UINT8 intrSampleC(void)
{
    printf("\n");
    return 0;
}

/* 
 This function D could NOT be called by "intrCBService" 
 Because the sub-routine of the function C call the function "printf" 
 is not typed of "INTERNAL_ONLY".
 */
INTERNAL_ONLY UINT8 intrSampleD(void)
{
    intrSampleA();
    intrSampleB();
    intrSampleC();
    
    return 0;
}

/* 
 This function E could NOT be called by "intrCBService" 
 Because the sub-routine of the function C "osMsgPost" is not typed of "INTERNAL_ONLY".
 */
INTERNAL_ONLY UINT8 intrSampleE(void)
{
    intrSampleA();
    intrSampleB();
    osMsgPost(0);
    return 0;
}
#endif
