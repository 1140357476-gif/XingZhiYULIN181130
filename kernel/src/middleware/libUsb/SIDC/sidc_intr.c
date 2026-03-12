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
 * @file      sidc_intr.c
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sidc_func.h" 
#include "dbg_def.h"
#if USBSIDC_OPTION
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
void sidcIntrData()
{
	sidc_container_t* pData = sidcData->hData.container;//sidcData->outContainer;	
	switch (pData->usCode)
	{
		case K_PIMAOPE_SendObjectInfo:          //0x0c10   
			//sidcFlag = SIDC_MainLoop;
			pimaSendObjectInfo();
			break;
		case K_PIMAOPE_SendObject:              //0x0d10
			//sidcFlag = SIDC_MainLoop;
			pimaSendObject();
			break;
		default:
			//ret = FALSE;
			//sidcFlag = SIDC_Null;
			sidcIntrResponse(K_PIMARES_OperationNotSupported,0,0);
			break;
	}
	//sidcPrintfErr("sidcFlag:%bx\n",sidcFlag);
	//if(ret == FAIL)
	//	sidcIntrResponse(K_PIMARES_OperationNotSupported,0,0);
}
/**
 * @fn        void sidcIntrCommand(void)
 * @brief     sidcIntrCommand
 * @param     NONE
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
void sidcIntrCommand()
{
	sidc_container_t* pCommand = G_ucStorCommand;
	UINT8 ret = TRUE;	

	//sidcPrintfCmd("cmd:%x\n-len:0x%lx\n-ID:%lx\n-p1:0x%lx\n-p2:0x%lx\n-p3:0x%lx\n",(pCommand->usCode),M_ByteSwapOfDword(pCommand->ulContainerLength),M_ByteSwapOfDword(pCommand->ulTransactionId),M_ByteSwapOfDword(pCommand->ulParameter[0]),M_ByteSwapOfDword(pCommand->ulParameter[1]),M_ByteSwapOfDword(pCommand->ulParameter[2]));
	intrPutUint16(pCommand->usCode);
	intrPutChar('\n');
	switch (pCommand->usCode)
	{
  		case K_PIMAOPE_GetDeviceInfo:           //0x0110
			ret = pimaGetDeviceInfo();
			break;
		case K_PIMAOPE_OpenSession:             //0x0210
			ret = pimaOpenSession();		
			break;
		case K_PIMAOPE_CloseSession:            //0x0310
			ret = pimaCloseSession();
			break;
		case K_PIMAOPE_GetStorageIDs:           //0x0410
			ret = pimaGetStorageIDs();
			break;

		case K_PIMAOPE_GetStorageInfo:          //0x0510
			ret = pimaGetStorageInfo();
			break;

		case K_PIMAOPE_GetNumObjects:           //0x0610   
			ret = pimaGetNumObjects();
			break;

		case K_PIMAOPE_GetObjectHandles:        //0x0710   
		case K_PIMAOPE_GetObjectInfo:           //0x0810  
		case K_PIMAOPE_GetObject:               //0x0910   				
		case K_PIMAOPE_GetThumb:                //0x0a10			
//		case K_PIMAOPE_DeleteObject:            //0x0b10
		//case K_PIMAOPE_SendObjectInfo:          //0x0c10   
		//case K_PIMAOPE_SendObject:              //0x0d10
		case K_PIMAOPE_GetPartialObject:        //0x1b10   
			sidcFlag = SIDC_MainLoop;
			break;
		case K_PIMAOPE_SendObjectInfo:          //0x0c10         
		case K_PIMAOPE_SendObject:              //0x0d10
			//ret = sidcIntrBulkOut((UINT16)sidcData->outContainer,0);
			//if(pCommand->usContainerType == K_PIMADATA_ContainerType_DataBlock)
			//	sidcFlag = SIDC_MainLoop;
			sidcFlag = SIDC_Null;
			break;
		/*	
		case K_PIMAOPE_SendObjectInfo:          //0x0c10         
			ret = pimaSendObjectInfo();
		break;
		case K_PIMAOPE_SendObject:              //0x0d10
			ret = pimaSendObject();
			break;
		*/
		//case K_PIMAOPE_InitiateCapture:         //0x0e10
		//case K_PIMAOPE_FormatStore:             //0x0f10
		//case K_PIMAOPE_ResetDevice:             //0x1010
		//case K_PIMAOPE_SelfTest:                //0x1110
		//case K_PIMAOPE_SetObjectProtection:     //0x1210   
		//case K_PIMAOPE_PowerDown:               //0x1310     
		//	sidcSvcProc |= 0x01;
		//	break;
		/*
		case K_PIMAOPE_GetDevicePropDesc:       //0x1410
			pimaGetDevicePropDesc();
			break;

		case K_PIMAOPE_GetDevicePropValue:      //0x1510   
			pimaGetDevicePropValue();		
			break;

		case K_PIMAOPE_SetDevicePropValue:      //0x1610   
			pimaSetDevicePropValue();
			break;

		case K_PIMAOPE_ResetDevicePropValue:    //0x1710   
			pimaResetDevicePropValue();
			break;
		*/
		//case K_PIMAOPE_TerminateOpenCapture:    //0x1810  
		//case K_PIMAOPE_MoveObject:              //0x1910   
		//case K_PIMAOPE_CopyObject:              //0x1a10   
		//case K_PIMAOPE_GetPartialObject:        //0x1b10   
		//case K_PIMAOPE_InitiateOpenCapture:     //0x1c10   
		//	sidcFlag = SIDC_MainLoop;
		//	break;
			
		default:
			ret = FALSE;
			break;	    
    }
	if(ret == FALSE)
		sidcIntrResponse(K_PIMARES_OperationNotSupported,0,0);
}
#endif
void sidcIntrBulkOutProc()
{
#if USBSIDC_OPTION
	UINT32 size;
	sidc_container_t* pContainer = sidcData->hData.container;//sidcData->outContainer;

	//sidcPrintfFunc("\n");
	intrPutChar('\n');
	size = sidcIntrDataOut();	

	//intrPutChar('S');
	//intrPutUint32(size);
	//intrPutChar('\n');
	if(size == 0){
		sidcPrintfErr("err!\n");
		return;
	}else if(pContainer->usContainerType==K_PIMADATA_ContainerType_CommandBlock){
		memcpy(G_ucStorCommand,pContainer,size);
		sidcIntrCommand();		
	}else if(pContainer->usContainerType==K_PIMADATA_ContainerType_DataBlock){
		sidcIntrData();
	}else{
		intrPutChar('E');
	}
#endif
}

