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
 * @file      stdreq.c
 * @brief     Module related to standard request
 * @author    Chi-Yeh Tsai
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_gprm.h"
#include "stdreq.h"
#include "main.h"
#include "ctlsetup.h"
#include "stddesc.h"
#include "usbstr.h"
#include "uiflow.h"
//#include "sidcOption.h"
#include "hal.h"
#include "initio.h"
#include "usbopt.h" 
#include "reg_def.h"


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




//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//STDREQ_NoDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void STDREQ_NoDataPhase(void)
 * @brief     process standard request without data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void STDREQ_NoDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//STDREQ_PreInDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void STDREQ_PreInDataPhase(void)
 * @brief     pre-process setting of following in data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void STDREQ_PreInDataPhase(void) USING_1
{
    switch (G_pCtrlCommand->bRequest)
    {
        case K_STDREQCODE_GET_DESCRIPTOR:
            STDREQ_PreGetDescriptor();      //GET_DESCRIPTOR
            break;

        default:
			break;
    }
}

//-----------------------------------------------------------------------------
//STDREQ_PreOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void STDREQ_PreOutDataPhase(void)
 * @brief     pre-process setting of following out data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void STDREQ_PreOutDataPhase(void) USING_1
{
	G_pucCtrlDataPointer = G_ucCtrlData;
}

//-----------------------------------------------------------------------------
//STDREQ_PostInDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void STDREQ_PostInDataPhase(void)
 * @brief     post-process data of complete in data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void STDREQ_PostInDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//STDREQ_PostOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void STDREQ_PostOutDataPhase(void)
 * @brief     post-process data of complete out data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void STDREQ_PostOutDataPhase(void) USING_1
{
}

void ReadUsbDescfromRsv(UINT8 usbspeed,UINT16* sizeAvailable )USING_1
{
        UINT16 DescriptorSize;

        if(usbspeed==1)
        {
       
		UINT32 len = 2048;
		UINT8 xdata *pt = G_FPGAMemPool+10240;
		UINT32 srcAddrByte =( K_SDRAM_UsbAddr<<1)+1024;
		UINT32 dstAddrByte = (K_SDRAM_CodeSize<<1)+(UINT32)pt-0x4000;
		HAL_GprmDramDmaExt_Intr(srcAddrByte,  dstAddrByte, len, 0, 1);
	    	G_pucCtrlDataPointer = pt;
		  // printf("#+++\n");
            //G_pucCtrlDataPointer = &G_FPGAMemPool[10240];
        }
        else
        {
         //printf("#---\n");
            INTR_HAL_DmaSetDramAddr(K_SDRAM_UsbAddr);
            INTR_HAL_DmaSetSramAddr(0x3800);
            INTR_HAL_DmaDo(0,1, 1024, 0, 0);   
            G_pucCtrlDataPointer = G_ucStorData;
        }
        DescriptorSize = (UINT16)G_pucCtrlDataPointer[2] | ((UINT16)G_pucCtrlDataPointer[3] << 8);  
        *sizeAvailable = DescriptorSize;  
}
//-----------------------------------------------------------------------------
//STDREQ_PreGetDescriptor
//-----------------------------------------------------------------------------
/**
 * @fn        void STDREQ_PreGetDescriptor(void)
 * @brief     prepare standard request of get descriptor
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void STDREQ_PreGetDescriptor(void) USING_1
{
	UINT16 sizeAvailable = 0;

	//get descriptor length
	switch (G_pCtrlCommand->wValue & 0xff00)
	{
		case K_DEVICE_DESCRIPTOR:
					//get device descriptor
			if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
				&& (gUsbMode == K_UIOPTION_STORAGE_MSDC) )	
			{
				G_pucCtrlDataPointer = (UINT8 *)&K_MSDC_DeviceDescriptor; // WWWW
				#if 1
				K_MSDC_DeviceDescriptor[3] = 0x01;
				if (G_bUSBSpeed == 0x01){
					K_MSDC_DeviceDescriptor[3] = 0x02;
				}
				#endif
				sizeAvailable = K_MSDC_DEVICE_DESCRIPTOR_SIZE; 
			}
#if USBSIDC_OPTION
			else if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
				&& (gUsbMode == K_UIOPTION_STORAGE_SIDC) )	
			{
				//#if 1
				memcpy(G_ucStorData,K_SIDC_DeviceDescriptor,K_SIDC_DEVICE_DESCRIPTOR_SIZE);
				//printf("G_bUSBSpeed 1:%bx",G_bUSBSpeed);
				G_ucStorData[3] = 0x01;
				if (G_bUSBSpeed == 0x01){
					G_ucStorData[3] = 0x02;
				}
				//#endif
				G_pucCtrlDataPointer = (UINT8 *)&G_ucStorData; // WWWW
				sizeAvailable = K_SIDC_DEVICE_DESCRIPTOR_SIZE; 
			}
#endif
			else
			{
				G_pucCtrlDataPointer = (UINT8 *)&K_DeviceDescriptor; // WWWW
				#if 1
				K_DeviceDescriptor[3] = 0x01;
				if (G_bUSBSpeed == 0x01){
					K_DeviceDescriptor[3] = 0x02;
				}
				#endif
				sizeAvailable = K_DEVICE_DESCRIPTOR_SIZE; 
			}

			break;

		case K_OTHERSPEED_DESCRIPTOR:			
			//if ((XBYTE[REG_Usb_HighSpeed] & 0x01) == 0x01){
			//G_bUSBSpeed = 0x01;
			//}
			//else{
			G_bUSBSpeed = 0x00;
			//}
		case K_CONFIGURATION_DESCRIPTOR:
					//get configuration descriptor
			if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
				&& (gUsbMode == K_UIOPTION_STORAGE_MSDC) )	
			{
				G_pucCtrlDataPointer = (UINT8 *)&K_MSDC_ConfigurationDescriptor; // WWWW
				#if 1
				//printf("1,2,3,4=%bx,%bx,%bx,%bx\n",K_MSDC_ConfigurationDescriptor[22],K_MSDC_ConfigurationDescriptor[23],K_MSDC_ConfigurationDescriptor[29],K_MSDC_ConfigurationDescriptor[30]);
				if (G_bUSBSpeed == 0x01){
					K_MSDC_ConfigurationDescriptor[22] = 0x00;
					K_MSDC_ConfigurationDescriptor[23]	= 0x02;
					K_MSDC_ConfigurationDescriptor[29] = 0x00;
					K_MSDC_ConfigurationDescriptor[30]	= 0x02;
				}
				else {
					K_MSDC_ConfigurationDescriptor[22] = 0x40;
					K_MSDC_ConfigurationDescriptor[23]	= 0x00;
					K_MSDC_ConfigurationDescriptor[29] = 0x40;
					K_MSDC_ConfigurationDescriptor[30]	= 0x00;
				}
				//printf("1,2,3,4=%bx,%bx,%bx,%bx\n",K_MSDC_ConfigurationDescriptor[22],K_MSDC_ConfigurationDescriptor[23],K_MSDC_ConfigurationDescriptor[29],K_MSDC_ConfigurationDescriptor[30]);
				#endif
				sizeAvailable = K_MSDC_CONFIGURATION_DESCRIPTOR_SIZE;
			}
#if USBSIDC_OPTION
			else if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
				&& (gUsbMode == K_UIOPTION_STORAGE_SIDC) )	
			{

				memcpy(G_ucStorData,K_SIDC_ConfigurationDescriptor,K_SIDC_CONFIGURATION_DESCRIPTOR_SIZE);
				//printf("G_bUSBSpeed 2:%bx",G_bUSBSpeed);
				//printf("1,2,3,4=%bx,%bx,%bx,%bx\n",K_SIDC_ConfigurationDescriptor[22],K_SIDC_ConfigurationDescriptor[23],K_SIDC_ConfigurationDescriptor[29],K_SIDC_ConfigurationDescriptor[30]);
				if (G_bUSBSpeed == 0x01){
					G_ucStorData[22] = 0x00;
					G_ucStorData[23]	= 0x02;
					G_ucStorData[29] = 0x00;
					G_ucStorData[30]	= 0x02;
				}
				else {
					G_ucStorData[22] = 0x40;
					G_ucStorData[23]	= 0x00;
					G_ucStorData[29] = 0x40;
					G_ucStorData[30]	= 0x00;
				}
				//printf("1,2,3,4=%bx,%bx,%bx,%bx\n",K_MSDC_ConfigurationDescriptor[22],K_MSDC_ConfigurationDescriptor[23],K_MSDC_ConfigurationDescriptor[29],K_MSDC_ConfigurationDescriptor[30]);

				G_pucCtrlDataPointer = (UINT8 *)&G_ucStorData; // WWWW
				sizeAvailable = K_SIDC_CONFIGURATION_DESCRIPTOR_SIZE;
			}
#endif
			else
			{
                       ReadUsbDescfromRsv(G_bUSBSpeed,&sizeAvailable );
#if 0 //cx mask for used res method
                        memcpy(G_FPGAMemPool,K_ConfigurationDescriptor_VC,K_ConfigurationDescriptor_VC_SIZE);
                        if (G_bUSBSpeed == 0x01)
                            {
                                memcpy(G_FPGAMemPool+K_ConfigurationDescriptor_VC_SIZE,K_ConfigurationDescriptor_VS_FORMAT_USB20,K_ConfigurationDescriptor_VS_FORMAT_USB20_SIZE);
                                 memcpy(G_FPGAMemPool+K_ConfigurationDescriptor_VC_SIZE+K_ConfigurationDescriptor_VS_FORMAT_USB20_SIZE,K_ConfigurationDescriptor_VS_ALT_BULK,K_ConfigurationDescriptor_VS_ALT_BULK_SIZE);
                                sizeAvailable =K_ConfigurationDescriptor_HS;
#if (K_AUDIOINTER)
                                memcpy(G_FPGAMemPool+K_ConfigurationDescriptor_HS,K_ConfigurationDescriptor_ADC,K_ConfigurationDescriptor_ADC_SIZE);
                                sizeAvailable =K_ConfigurationDescriptor_ADC_HS;
#endif
                            }
                       else
                            {
                                memcpy(G_FPGAMemPool+K_ConfigurationDescriptor_VC_SIZE,K_ConfigurationDescriptor_VS_FORMAT_USB11,K_ConfigurationDescriptor_VS_FORMAT_USB11_SIZE);
                                memcpy(G_FPGAMemPool+K_ConfigurationDescriptor_VC_SIZE+K_ConfigurationDescriptor_VS_FORMAT_USB11_SIZE,K_ConfigurationDescriptor_VS_ALT_BULK,K_ConfigurationDescriptor_VS_ALT_BULK_SIZE);
                                sizeAvailable =K_ConfigurationDescriptor_FS;
                                //for configuration total size 
                                G_FPGAMemPool[2] =M_LoByteOfWord(K_ConfigurationDescriptor_FS);//0x1c;//add audio //0xAF; 
                                G_FPGAMemPool[3] =M_HiByteOfWord(K_ConfigurationDescriptor_FS); //0x03;//add audio//0x01; 
                                 //for alt setting  Endpoint wMaxPacketSize
                                    G_FPGAMemPool[K_ConfigurationDescriptor_FS-0x50] = 0x02; //alt setting 4
                                    G_FPGAMemPool[K_ConfigurationDescriptor_FS-0x40] = 0x03; //alt setting 5
                                    G_FPGAMemPool[K_ConfigurationDescriptor_FS-0x30] = 0x03;//alt setting 6
                                    G_FPGAMemPool[K_ConfigurationDescriptor_FS-0x20] = 0x03;//alt setting 7
                                    //for bulk Endpoint wMaxPacketSize
                                    G_FPGAMemPool[K_ConfigurationDescriptor_FS-0x11] = 0x40;
                                    G_FPGAMemPool[K_ConfigurationDescriptor_FS-0x10] = 0x00;
                                    G_FPGAMemPool[K_ConfigurationDescriptor_FS-0x0a] = 0x40;
                                    G_FPGAMemPool[K_ConfigurationDescriptor_FS-0x09] = 0x00;
			
#if (K_AUDIOINTER)
                                memcpy(G_FPGAMemPool+K_ConfigurationDescriptor_FS,K_ConfigurationDescriptor_ADC,K_ConfigurationDescriptor_ADC_SIZE);
                                sizeAvailable =K_ConfigurationDescriptor_ADC_FS;
                                G_FPGAMemPool[2] =M_LoByteOfWord(K_ConfigurationDescriptor_ADC_FS);//0x1c;//add audio //0xAF; 
                                G_FPGAMemPool[3] =M_HiByteOfWord(K_ConfigurationDescriptor_ADC_FS); //0x03;//add audio//0x01; 
#endif                          
                            }
                            G_pucCtrlDataPointer = (UINT8 *)&G_FPGAMemPool; 
#endif			
			}
			if ((G_pCtrlCommand->wValue & 0xff00)== K_OTHERSPEED_DESCRIPTOR) {
				G_pucCtrlDataPointer[1] = 0x07;
			}
			else { /* USB_CONFIGURATION_DESCRIPTOR */
				G_pucCtrlDataPointer[1] = 0x02;
			}
                break;

		case K_STRING_DESCRIPTOR:
			//get string descriptor
			switch (G_pCtrlCommand->wValue & 0x00ff)
			{
				case 0x0000:	
					//get language ID string		
					G_pucCtrlDataPointer = K_LanguageIdString;		 
					sizeAvailable = K_LANGUAGE_ID_STRING_SIZE;
					break;

				case 0x0001:	
					//get manufacturer string		 
					G_pucCtrlDataPointer = K_ManufacturerString;	   
					sizeAvailable = K_MANUFACTURER_STRING_SIZE;
					break;

				case 0x0002:	
					//get product string		
					G_pucCtrlDataPointer = K_ProductString; 	  
					sizeAvailable = K_PRODUCT_STRING_SIZE;
					break;

				case 0x0003:	
					//get serial number string
					G_pucCtrlDataPointer = K_SerialNumberString;
					sizeAvailable = K_SERIAL_NUMBER_STRING_SIZE;
					break;
#if 0
				case 0x0004:	
					//get configuration string
					G_pucCtrlDataPointer = K_ConfigurationString;
					sizeAvailable = K_CONFIGURATION_STRING_SIZE;
					break;

				case 0x0005:	
					//get interface string
					G_pucCtrlDataPointer = K_InterfaceString;
					sizeAvailable = K_INTERFACE_STRING_SIZE;
					break;
#endif

				default:
					G_usCtrlDataLength = 0;
					break;
			}
			break;


		case K_DEVICE_QUALIFIER_DESCRIPTOR:
			//get device qualifier descriptor
			G_pucCtrlDataPointer = K_DeviceQualifierDescriptor; // WWWW
			sizeAvailable = K_DEVICE_QUALIFIER_DESCRIPTOR_SIZE; 

			break;

		case K_INTERFACE_DESCRIPTOR:
		case K_ENDPOINT_DESCRIPTOR:
		case 0x800:
			G_usCtrlDataLength = 0;
			break;


		default:
			//G_pucCtrlDataPointer = NULL;	
			XBYTE[REG_Usb_TransactionStallEn] |= 0x02; //EP0 in stall						
			G_usCtrlDataLength = 0;
			break;
	}

	//min(sizeAvailable, wLength) -> DataLength
	if (sizeAvailable < G_usCtrlDataLength)
		G_usCtrlDataLength = sizeAvailable;
}

