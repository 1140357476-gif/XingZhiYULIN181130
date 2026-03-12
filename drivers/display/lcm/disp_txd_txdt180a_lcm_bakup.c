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
 * @file		disp_gp_gpm779a0_lcm.c
 * @brief		display lcm giant plus 54124a driver.
 * @author		Matt Wang
 * @since		2008-08-15
 * @date		2008-08-19
 */

/*
 * History
 *
 * 1000, 20080828 Matt Wang
 *    a. Created
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/


#include "general.h"
#include "disp_api.h"

#include "gpio_api.h"
#include "timer.h"
#include "sp1k_util_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_hal_api.h"
#include "gpio.h"
//extern void appTFT_Reset(UINT8 on);
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
 

/* render dimension. */

#define DEV_TYPE  DISP_DEVICE_TYPE_LCM
#define WIDTH     160
#define HEIGHT    128 
#define HSYNCW    1
#define VSYNCW    1
#define HPIXEL    182 //182//214//182
#define VLINE      132//132//164//132
#define PNLH0     10 
#define PNLV0     3
#define PNLH1     (PNLH0 + WIDTH - 1 )
#define PNLV1     (PNLV0 + HEIGHT - 1 ) 

/* clock config. */

#define CLK_SRC        DISP_CLK_SRC_SYSPLL522M
#define CLK_DIVNX    12//  8
#define CLK_DIV1X      3
#define UPS_CLK_PHASE  8
 

/* lcm interface. */

 

#define BUS_TYPE        DISP_LCM_CMD_BUS_I80
#define BIT_WIDTH       DISP_LCM_CMD_8_9_BIT
#define PULSE_ACT       4
#define PULSE_RCV       5
#define CS0_ACT         0
#define A0_CMD          0
#define DATA_FMT        DISP_LCM_DATA_FMT_RGB666

#define RGB666_PACK     0
#define RGB666_FMT_10   0
#define PIXEL_ALIGN     DISP_LCM_PIXEL_ALIGN_BIG
#define COLOR_PACKAGE   DISP_LCM_COLOR_PACKAGE_BGR
#define SWITCH_G        0
#define CHANGE_RB       0
#define ONE_HALF_PIXEL  0
#define DITHER          0
 
/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
/* function type definition. */
#define __STATIC  //static
/* operating function. */

 

#define dispOpDevNameGet     dispPanelOpDevNameGet
#define dispOpCapabilityGet  dispPanelOpCapabilityGet
#define dispOpPowerOn        dispPanelOpPowerOn
#define dispOpPowerOff       dispPanelOpPowerOff 

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

/* operating function. */

__STATIC UINT8 *dispOpDevNameGet(void);
__STATIC void dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc);
__STATIC void dispOpPowerOn(void);
__STATIC void dispOpPowerOff(void);
 

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device name. */
extern void appLcdRst(void) ;   //nogui add
static code UINT8 dispDevName[] = "LCD - TXD1.77 LCM";
void dispSyncInterEnable(void) ;

/* device capability. */

static code dispCapabDesc_t dispCapabDesc = {

         WIDTH,
         HEIGHT,
         PNLH0,
         PNLV0,
         4,
         3,
};


/* lcm command table. */

#if 1 //suny
static code UINT32 dispPwrOnCmdTbl[] = {

	  //DISP_LCM_CMD_INST |0x01,//reset 
         //DISP_LCM_CMD_WAIT |(50),

         DISP_LCM_CMD_INST |0x11,//sleep out
         DISP_LCM_CMD_WAIT |(20),  //1000

 #if 0 //Set tearing effect mode   1- TE ON 0-TE OFF
 /*******************************/
         DISP_LCM_CMD_INST|0x35, 
         DISP_LCM_CMD_PARA | 0x00,//  V syc     
 
         DISP_LCM_CMD_INST|0x0e, 
         DISP_LCM_CMD_PARA | 0x80,// TE on
/*******************************/	
#endif        

         DISP_LCM_CMD_INST|0x3a  ,          //Interface Pixel Format, p162
         DISP_LCM_CMD_PARA | 0x06   ,       // IPF3 VIPF2 VIPF1 VIPF0 D3 IFPF2 IFPF1 IFPF0 66h

	  DISP_LCM_CMD_INST|0x36  ,          //Interface Pixel Format, p162
//         DISP_LCM_CMD_PARA | 0xa8   ,       // IPF3 VIPF2 VIPF1 VIPF0 D3 IFPF2 IFPF1 IFPF0 66h
         DISP_LCM_CMD_PARA | 0x68   ,       // IPF3 VIPF2 VIPF1 VIPF0 D3 IFPF2 IFPF1 IFPF0 66h
         
#if 0 // Frame rate set    1- TE ON 0-TE OFF
         DISP_LCM_CMD_INST|0xb1   ,  
        // DISP_LCM_CMD_PARA | 0x18 ,  
        DISP_LCM_CMD_PARA | 0x08 , 
        DISP_LCM_CMD_PARA | 0x3A,//0x3f, 

#endif

         DISP_LCM_CMD_INST|0xc0   ,  
         DISP_LCM_CMD_PARA | 0x04    ,    
         DISP_LCM_CMD_PARA | 0x00     ,  
            
          DISP_LCM_CMD_INST|0xc1     , 
	   DISP_LCM_CMD_PARA | 0x02   , 


        // DISP_LCM_CMD_INST|0xc2     ,  
         //DISP_LCM_CMD_PARA | 0x05   , 
         //DISP_LCM_CMD_PARA | 0x00  , 

         DISP_LCM_CMD_INST|0xc5     ,       //COM Control 1
         DISP_LCM_CMD_PARA | 0x4A ,//0x28   //comH Vol: nVM* VMH6 VMH5 VMH4 VMH3 VMH2 VMH1 VMH0
	  	 DISP_LCM_CMD_PARA | 0x40,//0x46,//0x38 ,

	  //DISP_LCM_CMD_PARA | 0x4A ,//0x28   //comH Vol: nVM* VMH6 VMH5 VMH4 VMH3 VMH2 VMH1 VMH0
	  //DISP_LCM_CMD_PARA | 0x30 ,
	  
        // DISP_LCM_CMD_INST|0xc6, 
         //DISP_LCM_CMD_PARA | 0x0b,      

//         ld    c,0xc7   
//        call  Write_Command
//        ld    c,0xbc        
//        call  Write_Data
//        call  Delay35ms

        //////////////////data access,data format.
 		DISP_LCM_CMD_INST|0xf2  ,          //
         DISP_LCM_CMD_PARA | 0x01  ,  
         
         
         DISP_LCM_CMD_INST|0xe0  ,          //Gamma Set
         DISP_LCM_CMD_PARA | 0x3f  ,          //Gamma
         DISP_LCM_CMD_PARA | 0x26 ,           //Gamma
         DISP_LCM_CMD_PARA | 0x23  ,          //Gamma
         DISP_LCM_CMD_PARA | 0x2d   ,         //Gamma
         DISP_LCM_CMD_PARA | 0x28    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x0b  ,          //Gamma
         DISP_LCM_CMD_PARA | 0x53  ,          //Gamma
         DISP_LCM_CMD_PARA | 0xe1 ,           //Gamma
         DISP_LCM_CMD_PARA | 0x42   ,         //Gamma
         DISP_LCM_CMD_PARA | 0x19    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x20   ,         //Gamma
         DISP_LCM_CMD_PARA | 0x05   ,         //Gamma
         DISP_LCM_CMD_PARA | 0x00    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x00   ,         //Gamma
         DISP_LCM_CMD_PARA | 0x00      ,      //Gamma
         //DISP_LCM_CMD_PARA | 0x08     ,       //Gamma

         DISP_LCM_CMD_INST|0xe1   ,         //Gamma Set
         DISP_LCM_CMD_PARA | 0x00     ,       //Gamma
         DISP_LCM_CMD_PARA | 0x18     ,       //Gamma
         DISP_LCM_CMD_PARA | 0x1c      ,      //Gamma
         DISP_LCM_CMD_PARA | 0x11    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x19    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x12    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x2b    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x1d   ,         //Gamma         
         DISP_LCM_CMD_PARA | 0x3b   ,         //Gamma
         DISP_LCM_CMD_PARA | 0x05    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x1e    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x39    ,        //Gamma
         DISP_LCM_CMD_PARA | 0x3f    ,       //Gamma
         DISP_LCM_CMD_PARA | 0x3f   ,        //Gamma
         DISP_LCM_CMD_PARA | 0x3f    ,        //Gamma
         //DISP_LCM_CMD_PARA | 0x05  ,          //Gamma        

//        ld    c,0xb4            

//        call  Write_Command

//        ld    c,0x00            

//        call  Write_Data

	   DISP_LCM_CMD_INST |(0x2a),   
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x9F), 
	   
	   DISP_LCM_CMD_INST |(0x2b),  
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x7f), 
	   
	   

	   DISP_LCM_CMD_INST|0x26 ,      
	   DISP_LCM_CMD_PARA | 0x02     ,    

	   DISP_LCM_CMD_INST |(0x29),// Display On
	   DISP_LCM_CMD_INST | 0x2C,  /* ramwr. */ 

	   DISP_LCM_CMD_STOP,      

};


#endif


 

 

static code UINT32 dispPwrOffCmdTbl[] = {

         DISP_LCM_CMD_INST | 0x28,  /* dispoff. */
         //DISP_LCM_CMD_INST | 0x10,  /* slpin. */
         //DISP_LCM_CMD_INST | 0x38,  /* idle mode off. */
         //DISP_LCM_CMD_INST | 0x39,  /* idle mode on. */
        // DISP_LCM_CMD_INST | 0x12,  /* ptlon. */
         DISP_LCM_CMD_STOP, 

};

 static code UINT32 dispPwrOnCmdTbl_2[] =          
{
   	   DISP_LCM_CMD_INST |(0x2a),   
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x9F), 
	   
	   DISP_LCM_CMD_INST |(0x2b),  
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x00),
	   DISP_LCM_CMD_PARA | (0x7f),         

   	 //DISP_LCM_CMD_INST |(0x29),// Display On
        DISP_LCM_CMD_INST | 0x2C,  /* ramwr. */  
   	
   	 DISP_LCM_CMD_STOP,
};
 
void appDispCmdSet(void)	             
{
	dispLcmCmdTableSend(dispPwrOnCmdTbl_2);		
}

 

/**

 

 * @brief  open device (register operating function).

 

 * @param       None.

 

 * @retval         return = NULL or device id.

 

 * @see

 

 * @author        Matt Wang

 

 * @since          2008-06-16

 

 * @todo  N/A

 

 * @bug            N/A

 

*/

 

UINT8

 

dispPanelOpen(

 

         void

 

)

 

{

 

         dispOperatingFunc_t dispOpFunc;

 

 

 

 

 

         dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;

 

         dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;

 

         dispOpFunc.pfOpPowerOn       = dispOpPowerOn;

 

         dispOpFunc.pfOpPowerOff      = dispOpPowerOff;

 

 

 

         return dispDevOpen(&dispOpFunc, sizeof(dispOperatingFunc_t));

 

}

 

 

 

/**

 

 * @brief  operating function to get device name (ascii string).

 

 * @param       None.

 

 * @retval         return = device name string.

 

 * @see

 

 * @author        Matt Wang

 

 * @since          2008-06-16

 

 * @todo  N/A

 

 * @bug            N/A

 

*/

 

__STATIC UINT8 *

 

dispOpDevNameGet(

 

         void

 

)

 

{

 

         return dispDevName;

 

}

 

 

 

/**

 

 * @brief  operating function to get device capability.

 

 * @param       *pcapabDesc = [out] capability describer.

 

 * @param   descSize = [in] describer structure size.

 

 * @retval         None.

 

 * @see

 

 * @author        Matt Wang

 

 * @since          2008-06-16

 

 * @todo  N/A

 

 * @bug            N/A

 

*/

 

__STATIC void

 

dispOpCapabilityGet(

 

         dispCapabDesc_t *pcapabDesc

 

)

 

{

 

         *pcapabDesc = dispCapabDesc;

 

}

 

 

 

/**

 

 * @brief  operating function to power on device.

 

 * @param       None.

 

 * @retval         None.

 

 * @see

 

 * @author        Matt Wang

 

 * @since          2008-06-16

 

 * @todo  N/A

 

 * @bug            N/A

 

*/

 

__STATIC void

 

dispOpPowerOn(

 

         void

 

)

 

{

 	UINT8 i;

         /* set clk and type. */

 	gpioByteFuncSet(GPIO_BYTE_GEN0,0x01,0x01);
	gpioByteDirSet(GPIO_BYTE_GEN0,0x01,0x01);
	
	gpioByteOutSet(GPIO_BYTE_GEN0,0x01,0x00);
	sp1kHalWait(20);
	gpioByteOutSet(GPIO_BYTE_GEN0,0x01,0x01);
	sp1kHalWait(100);
		 
         dispParaSet(DISP_PARA_CLK_CFG, CLK_SRC, CLK_DIVNX, CLK_DIV1X, 0, 0);

 

         dispParaSet(DISP_PARA_UPS_CLK_PHASE, UPS_CLK_PHASE, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0);
 

         /* set vh timing and format */

 

         dispParaSet(DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0);

 

         dispParaSet(DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0);

 

         dispParaSet(DISP_PARA_INTERLACE_CFG, DISP_PROGRESSIVE, 0, 0, 0, 0);

 

         /* set lcm interface. */

 

         dispParaSet(DISP_PARA_LCM_CMD_BUS_TYPE, BUS_TYPE, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_CMD_BIT_WIDTH, BIT_WIDTH, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_CMD_PULSE_WIDTH, PULSE_ACT, PULSE_RCV, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_CMD_ACTIVE_LEVEL, CS0_ACT, A0_CMD, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_DATA_FORMAT, DATA_FMT, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_RGB666_PACK_ENABLE, RGB666_PACK, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_RGB666_FORMAT_10_ENABLE, RGB666_FMT_10, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_PIXEL_ALIGN, PIXEL_ALIGN, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_COLOR_PACKAGE, COLOR_PACKAGE, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_SWITCH_G_ENABLE, SWITCH_G, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_CHANGE_RB_ENABLE, CHANGE_RB, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_ONE_HALF_PIXEL_ENABLE, ONE_HALF_PIXEL, 0, 0, 0, 0);

 

         dispParaSet(DISP_PARA_LCM_DITHER_ENABLE, DITHER, 0, 0, 0, 0);

         /* send lcm command. */

	dispLcmCmdTableSend(dispPwrOnCmdTbl);
	dispLcmCmdTableSend(dispPwrOnCmdTbl);
#if 0  //TE ENABLE    1- TE ON 0-TE OFF
	dispSyncInterEnable();
#endif

   //      printf("Leave LCM Init pl\n");

 

}

 

 

 

/**

 

 * @brief  operating function to power off device.

 

 * @param       None.

 

 * @retval         None.

 

 * @see

 

 * @author        Matt Wang

 

 * @since          2008-06-16

 

 * @todo  N/A

 

 * @bug            N/A

 

*/

 

__STATIC void dispOpPowerOff(void)
{
//	appTFT_Reset(0);
	sp1kHalWait(20);
         /* send lcm command. */
         dispLcmCmdTableSend(dispPwrOffCmdTbl);

}
#if 1//gpio9
void dispSyncInterEnable(void)              //suny 20090324
{
	//TE: gpio17 interrupt
	sp1kGpioByteFuncSet(GPIO_BYTE_GEN1, 0x02, 0x02);//(GPIO_BYTE_GEN2, 0x02, 0x02);   //GPIO17--->gpio9
	//sp1kGpioByteDirSet(GPIO_BYTE_GEN2, 0x01, 0x00);    
	
	//XBYTE[REG_Disp_GpioRIntEn3] &= ~0x01;	
#if 0   //gpio17		
	XBYTE[0x204A] &= ~0x02; //clean gpio17 status
	XBYTE[0x2052] |= 0x02;   //Enable gpio17 interrupt
	XBYTE[0x204A] &= ~0x02; //clean gpio17 status
#else   //gpiuo9
	XBYTE[0x2049] &= ~0x02; //clean gpio9 status
	XBYTE[0x2051] |= 0x02;   //Enable gpio9 interrupt
	XBYTE[0x2049] &= ~0x02; //clean gpio9 status
#endif

	//XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
	//XBYTE[REG_Disp_ImgVOddIniEn] |= 0x04; // enable disp V sync intr
#if 1
	XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
	XBYTE[REG_Disp_ImgVOddIniEn] |= 0x04; // enable disp V sync intr	
#else
	XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x02;
	XBYTE[REG_Disp_ImgVOddIniEn] |= 0x01; // enable disp V sync intr
#endif	
} 

void dispSyncInterDisable(void)              //20091214
{
	//TE: gpio17 interrupt
	sp1kGpioByteFuncSet(GPIO_BYTE_GEN2, 0x02, 0x02);   //GPIO17
	//sp1kGpioByteDirSet(GPIO_BYTE_GEN2, 0x01, 0x00);    
	
	//XBYTE[REG_Disp_GpioRIntEn3] &= ~0x01;	
#if 	0  //gpio17	
	XBYTE[0x204A] &= ~0x02; //clean gpio17 status
	XBYTE[0x2052] &= ~0x02;   //Disable gpio17 interrupt
	XBYTE[0x204A] &= ~0x02; //clean gpio17 status
#else  //gpio9
	XBYTE[0x2049] &= ~0x02; //clean gpio9 status
	XBYTE[0x2051] &= ~0x02;   //Disable gpio9 interrupt
	XBYTE[0x2049] &= ~0x02; //clean gpio9 status
#endif

	//XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
	//XBYTE[REG_Disp_ImgVOddIniEn] |= 0x04; // enable disp V sync intr
#if 1
	XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
	XBYTE[REG_Disp_ImgVOddIniEn] &= ~0x04; //disable disp V sync intr	
#else
	XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x02;
	XBYTE[REG_Disp_ImgVOddIniEn] |= 0x01; // enable disp V sync intr
#endif	
} 

#endif
#if   0  //gpio17
void dispSyncInterEnable(void)              //suny 20090324
{
	//TE: gpio17 interrupt
	sp1kGpioByteFuncSet(GPIO_BYTE_GEN2, 0x02, 0x02);   //GPIO17
	//sp1kGpioByteDirSet(GPIO_BYTE_GEN2, 0x01, 0x00);    
	
	//XBYTE[REG_Disp_GpioRIntEn3] &= ~0x01;	
		
	XBYTE[0x204A] &= ~0x02; //clean gpio17 status
	XBYTE[0x2052] |= 0x02;   //Enable gpio17 interrupt
	XBYTE[0x204A] &= ~0x02; //clean gpio17 status

	//XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
	//XBYTE[REG_Disp_ImgVOddIniEn] |= 0x04; // enable disp V sync intr
#if 1
	XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
	XBYTE[REG_Disp_ImgVOddIniEn] |= 0x04; // enable disp V sync intr	
#else
	XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x02;
	XBYTE[REG_Disp_ImgVOddIniEn] |= 0x01; // enable disp V sync intr
#endif	
} 

void dispSyncInterDisable(void)              //20091214
{
	//TE: gpio17 interrupt
	sp1kGpioByteFuncSet(GPIO_BYTE_GEN2, 0x02, 0x02);   //GPIO17
	//sp1kGpioByteDirSet(GPIO_BYTE_GEN2, 0x01, 0x00);    
	
	//XBYTE[REG_Disp_GpioRIntEn3] &= ~0x01;	
		
	XBYTE[0x204A] &= ~0x02; //clean gpio17 status
	XBYTE[0x2052] &= ~0x02;   //Disable gpio17 interrupt
	XBYTE[0x204A] &= ~0x02; //clean gpio17 status

	//XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
	//XBYTE[REG_Disp_ImgVOddIniEn] |= 0x04; // enable disp V sync intr
#if 1
	XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x08;
	XBYTE[REG_Disp_ImgVOddIniEn] &= ~0x04; //disable disp V sync intr	
#else
	XBYTE[REG_Disp_LcmAutoCmd30]   &= ~0x02;
	XBYTE[REG_Disp_ImgVOddIniEn] |= 0x01; // enable disp V sync intr
#endif	
} 

#endif
void dispVcomSet(UINT16 val)
{
	
}
 
