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
 * @file      stddesc.h
 * @brief     descriptor of usb
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __STDDESC_H__
#define __STDDESC_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "class.h" 
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
/*
  This segment is used to define the variables which should be convert to resource files
*/
#ifdef CVT_TOOL //this macro defined in batch file pre-build complie option
extern UINT8 TAG_CVT_S;//Tag for convert start

extern xdata UINT8 K_DeviceDescriptor[];
extern xdata UINT8 K_ConfigurationDescriptor[];
extern xdata UINT8 K_MSDC_DeviceDescriptor[];
extern xdata UINT8 K_MSDC_ConfigurationDescriptor[];
extern xdata UINT8 K_SIDC_DeviceDescriptor[];
extern code UINT8 K_SIDC_ConfigurationDescriptor[];
extern xdata UINT8 K_DeviceQualifierDescriptor[];

extern UINT8 TAG_CVT_E;//Tag for convert end
#endif//CVT_TOOL
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/



//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------

//patch4.5@richie@audio interface
//#define K_AUDIOINTER	0

#define K_VIDEOFROMATNUMBER 2  //CX++

//Descriptor Content
//---- Device Descriptor ----
#define K_DEVICE_DESCRIPTOR_SIZE        0x0012
xdata    UINT8   K_DeviceDescriptor[] =
{
		//-------------------------------------------------------------------
		//TotalLength = 0x0012 byte
		//-------------------------------------------------------------------

		//Device (0x12 byte)
		0x12,                   //bLength: 0x12 byte
		0x01,                   //bDescriptorType: DEVICE
		0x00, 0x01,             //bcdUSB: version 1.00
		DEVICE_CLASS, // 0x00,  //bDeviceClass: independent interfaces
		DEVICE_SUBCLASS,//0x00, //bDeviceSubClass: 2
		DEVICE_PROTOCOL,//0x00, //bDeviceProtocol: class specific protocols NOT used on device basis
		0x40,                   //bMaxPacketSize0: maximum packet size for endpoint zero
		0xfc, 0x04,             //idVendor: vendor ID
		0x28, 0x16,             //idProduct: product ID
		0x00, 0x01,             //bcdDevice: device release number in BCD
		0x01,                   //iManufacturer: index of string
		0x02,                   //iProduct: index of string 
		0x00,                   //iSerialNumber: index of string  //patch4.3@cytsai@0606
		0x01,                   //bNumConfigurations: 1 configuration
};
#if 0 //mask used res method
//---- Configuration Descriptor ---- //cytsai@0312
//patch4.5@richie@audio interface begin
#define K_ConfigurationDescriptor_VC_SIZE  0x7F

#define K_ConfigurationDescriptor_VS_FORMAT_USB20_SIZE   0x131 //0x1b1 add yuy2 720p
#define K_ConfigurationDescriptor_VS_FORMAT_USB11_SIZE  0X9b
#define K_ConfigurationDescriptor_VS_ALT_BULK_SIZE   0x95 //0x1b1 add yuy2 720p
 
#if (K_AUDIOINTER)
#define K_ConfigurationDescriptor_ADC_SIZE 0x16d

#define K_ConfigurationDescriptor_ADC_HS        K_ConfigurationDescriptor_VC_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_FORMAT_USB20_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_ALT_BULK_SIZE\
                                                                            +K_ConfigurationDescriptor_ADC_SIZE

#define K_ConfigurationDescriptor_ADC_FS        K_ConfigurationDescriptor_VC_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_FORMAT_USB11_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_ALT_BULK_SIZE\
                                                                            +K_ConfigurationDescriptor_ADC_SIZE
                                                                            
#define K_ConfigurationDescriptor_HS                K_ConfigurationDescriptor_VC_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_FORMAT_USB20_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_ALT_BULK_SIZE

#define K_ConfigurationDescriptor_FS                K_ConfigurationDescriptor_VC_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_FORMAT_USB11_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_ALT_BULK_SIZE
#else
#define K_ConfigurationDescriptor_HS                K_ConfigurationDescriptor_VC_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_FORMAT_USB20_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_ALT_BULK_SIZE

#define K_ConfigurationDescriptor_FS                K_ConfigurationDescriptor_VC_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_FORMAT_USB11_SIZE\
                                                                            +K_ConfigurationDescriptor_VS_ALT_BULK_SIZE
#endif
xdata    UINT8   K_ConfigurationDescriptor_VC[] =
{
		//-------------------------------------------------------------------
		//TotalLength = 0x0543 byte
		//-------------------------------------------------------------------
		
		//Configuration (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x02,                   //bDescriptorType: CONFIGURATION        

//patch4.5@richie@audio interface begin
#if (K_AUDIOINTER)
		M_LoByteOfWord(K_ConfigurationDescriptor_ADC_HS),
		M_HiByteOfWord(K_ConfigurationDescriptor_ADC_HS),
		//0xb2,0x03,    //wTotalLength: 0x3b2 byte		
		0x05,                   //bNumInterfaces: 5 interfaces
#else
		M_LoByteOfWord(K_ConfigurationDescriptor_HS),
		M_HiByteOfWord(K_ConfigurationDescriptor_HS),
		//0x45,0x02,
		0x03,                   //bNumInterfaces: 3 interfaces
#endif
//patch4.5@richie@audio interface end

		0x01,                   //bConfigurationValue: configuration 1
		0x00,                   //iConfiguration: index of string
		0x80,                   //bmAttributes: bus powered
		0xFA,                   //MaxPower: 500 mA     

		/*Interface Association Descriptor (0x08 byte)*/
		0x08,                   /*bLength: 0x08 byte*/
		0x0B,                   /*bDescriptorType: INTERFACE ASSOCATION Descriptor*/
		0x00,                   /*bInterfaceNumber: interface 0*/
		0x02,                   /*bInterfaceCount: 2*/
		CC_VIDEO,                   /*bFunctionClass: CC_VIDEO*/
		SC_VIDEO_INTERFACE_COLLETION,  /*bFunctionSubClass: SC_VIDEO_INTERFACE_COLLETION */
		0x00,                   /*bInterfaceProtocol: class specific protocol NOT used on this interface*/
		0x02,                   /*iFunction: 2*/

		//Interface 0, AlternateSetting 0 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x00,                   //bInterfaceNumber: interface 0
		0x00,                   //bAlternateSetting: alternate setting 0
		0x01,                   //bNumEndpoints: 1 endpoint
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO*/
		SC_VIDEOCONTROL,                   /*bInterfaceSubClass: SC_VIDEOCONTROL*/
		0x00,                   /*bInterfaceProtocol: class specific protocol NOT used on this interface*/
		0x02,                   /*iInterface: index of string*/
		/*Class-specific VC Interface Header Descriptor (0x0d byte)*/
		0x0d,                   /*bLength: 0x0d byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x01,                   /*bDescriptorSubType: VC_HEADER subtype*/
		0x00, 0x01,             /*bcdUVC: Revision of class specification that this device is based upon*/
		0x50, 0x00, //  0x33, 0x00(meiyou Extension Unit),   /*wTotalLength: Total size of class-specific descriptors*/
		0x00, 0x6c, 0xdc, 0x02, /*dwClockFrequency: 48MHz*/
		0x01,                   /*bInCollection: Number of streaming interfaces*/
		0x01,                   /*baInterfaceNr(1): Interface number of the first VideoStreaming interface*/

		/*Camera Terminal Descriptor (0x12 byte)*/

		0x12,                   /*bLength: 0x12 byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x02,                   /*bDescriptorSubType: VC_INPUT_TERMINAL subtype*/
		0x01,                   /*bTerminalID: ID of this input terminal*/
		0x01, 0x02,             /*wTerminalType: ITT_CAMERA type.*/
		0x00,                   /*bAssocTerminal: No assocation*/
		0x00,                   /*iTerminal: Unused*/
		0x00, 0x00,             /*wObjectiveFocalLengthMin: No optional zoom supported*/
		0x00, 0x00,             /*wObjectiveFocalLengthMax: No optional zoom supported*/
		0x00, 0x00,             /*wOcularFocalLength: No optional zoom supported*/
		0x03,                   /*bControlSize: The size of the bmControls is 2 bytes*/
		//0x0E, 0x00, 0x04,        /*bmControls: Camera Terminal controls are supported*/
		//0x0A, 0x00, 0x00,        /*bmControls: Camera Terminal controls are supported*/
		0x00, 0x00, 0x00,  

		/*Processing Unit Descriptor (0x0B byte)*/
		0x0B,                   /*bLength: 0x0B byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x05,                   /*bDescriptorSubtype: VC_PROCESSING_UNIT*/
		0x02,                   /*bUnitID: ID of this unit*/
		0x01,                   /*bSourceID: This input pin of this unit is connected to the output pin of unit with ID 0x01*/
		0x00, 0x40,             /*wMaxMultiplier: Maximum digital magnification (4X).*/
		0x02,                   /*bControlSize: Size in bytes of the bmControls field*/
		0x1f, 0x04,             /*bmControls: Processing Unit controls supported.*/
		0x00,                   /*iProcessing: Unused*/

		/*Extension Unit Descriptor (0x1B byte)*/
		0x1D,                   /*bLength: 0x1C byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x06,                   /*bDescriptorSubtype: VC_EXTENSION_UNIT*/
		0x03,                   /*bUnitID: ID of this unit*/
		0x82, 0x06, 0x61, 0x63, /*guidFormat: Globally Unique Identifier for Logitech Extension Unit.*/
		0x70, 0x50, 
		0xAB, 0x49,
		0xB8, 0xCC, 
		0xB3, 0x85,
		0x5E, 0x8D, 0x22, 0x1D,
		0x00,					/*bNumControls*/
		0x01,					/*bNrInPins*/
		0x02,					/*bSourceID*/
		0x04,					/*bControlSize*/
#if(DDX_on) 		
		0x07, 0x00, 0x00, 0x00,  /*bmControls*/
#else
		0x01, 0x00, 0x00, 0x00,  /*bmControls*/
#endif
		0x00,					/*Unused*/

		/*Output Terminal Descriptor (0x09 byte)*/
		0x09,                   /*bLength: 0x09 byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x03,                   /*bDescriptorSubtype: VC_OUTPUT_TERMINAL.*/
		0x04,                   /*bTerminalID: ID of this terminal.*/
		0x01, 0x01,             /*wTerminalType: TT_STREAMING type. This terminal is a USB streaming terminal.*/
		0x00,                   /*bAssocTerminal: No association.*/
		0x03,                   /*bSourceID: The input pin of this unit is connected to the output pin of unit 2.*/
		0x00,                   /*iTerminal: Unused*/
		/*Standard Interrupt Endpoint Descriptor (0x07 byte)*/
		0x07,                   /*bLength: 0x07 byte*/
		0x05,                   /*bDescriptorType: ENDPOINT*/
		0x87,                   /*bEndpointAddress: IN endpoint 7.*/
		0x03,                   /*bmAttributes: Interrupt transfer type.*/
		0x10, 0x00,             /*wMaxPacketSize: 16-byte status packet.*/
		0x01,                   //bInterval: polling interval is 1 ms  

		/*Class-specific Interrupt Endpoint Descriptor (0x05 byte)*/
		0x05,                   /*bLength: 0x09 byte*/
		0x25,                   /*bDescriptorType: CS_ENDPOINT descriptor.*/
		0x03,                   /*bDescriptorSubType: EP_INTERRUPT.*/
		0x10, 0x00,             /*wMaxTransferSize: 16-byte status packet.*/

            /*Standard VS Interface Descriptor (Alternate Setting 0) (0x09 byte)*/
		0x09,                   /*bLength: 0x09 byte*/
		0x04,                   /*bDescriptorType: INTERFACE*/
		0x01,                   /*bInterfaceNumber: interface 1*/
		0x00,                   /*bAlternateSetting: alternate setting 0*/
		0x01,                   /*bNumEndpoints: 0 endpoint*/
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO*/
		SC_VIDEOSTREAMING,                   /*bInterfaceSubClass: SC_VIDEOSTREAMING.*/
		0x00,                   /*bInterfaceProtocol: PC_PROTOCOL_UNDEFINED.*/
		0x00,                   /*iInterface: Unused*/

};

xdata    UINT8   K_ConfigurationDescriptor_VS_FORMAT_USB20[] =
{

		/*Class-specific VS Input Header Descriptor (0x0f/0x0e byte)*/
        #if (K_VIDEOFROMATNUMBER==2)            
		0x0f,                /*bLength: 0x0f byte*/
        #else
              0x0e,               /*bLength: 0x0e byte*/
        #endif       
		0x24,                   /*bDescriptorType: CS_INTERFACE.*/
		0x01,                   /*bDescriptorSubtype: VS_INPUT_HEADER.*/
        #if (K_VIDEOFROMATNUMBER==2)  
		0x02,                    /*bNumFormats: two format descriptors follow.*/
		0x22,0x01,//0x03,0x01,// mask YUY2 720P 0x21,0x01,            /*wTotalLength: Total size of class-specific VideoStreaming interface descriptors.*/
         #else
              0x01,                   /*bNumFormats: one format descriptors follow.*/
		0x97,0x00,//0x63,0x01,            /*wTotalLength: Total size of class-specific VideoStreaming interface descriptors.*/	
        #endif   
		0x81,                   /*bEndpointAddress: Address of the isochronous endpoint used for video data.*/
		0x00,                   /*bmInfo: Dynamic format change doesn't supported.*/
		0x04,                   /*bTerminalLink: This VideoStreaming interface supplies terminal ID 3 (Output Terminal).*/
		0x00,//0x02,                   /*bStillCaptureMethod: Device supports still image capture method 2.*/
		0x00,//0x01,                   /*bTriggerSupport: Hardware trigger supported for still image capture.*/
		0x00,                   /*bTriggerUsage: Hardware trigger should initiate a still image capture.*/
		0x01,                   /*bControlSize: Size of the bmaControls field.*/
		//0x00,                   /*BmaControls(1): Compression quality videoStreaming specific controls is supported.*/
								/* set to 0 to pass WHQL */
         #if (K_VIDEOFROMATNUMBER==2)            
		0x04,//0x00,                   /*BmaControls(1): Compression quality videoStreaming specific controls is supported. MJPEG*/
		0x00,                   /*BmaControls(2): No VideoStreaming specific controls are supported. YUY2*/
        #else
             0x04,//0x00,                   /*BmaControls(1): Compression quality videoStreaming specific controls is supported. MJPEG*/
		//0x00,                   /*BmaControls(2): No VideoStreaming specific controls are supported. YUY2*/
        #endif  
		
		//----------------------------------------
		//	MJPEG subLenth 0x0155Byte
		//----------------------------------------
		/*Class-specific VS Payload Format Descriptor (MJPEG) (0x0b byte)*/
		0x0b,                   			/*bLength: 0x0b byte*/
		CS_INTERFACE,           			/*bDescriptorType: CS_INTERFACE.*/
		VS_FORMAT_MJPEG, 			        /*bDescriptorSubtype: VS_FORMAT_MJPEG.*/
		VC_FORMAT_INDEX_MJPEG_YUV422,				/*bFormatIndex: First format descriptor.*/
		VC_NUMBER_OF_FRAME_MJPEG,		/*bNumFrameDescriptors*/
		0x01,                   			/*bmFlags: Uses fixed size samples.*/
		0x01,//VC_DEFAULT_FRAME_INDEX_MJPEG,    	/*bDefaultFrameIndex: Default frame index.*/
		0x00,                   			/*bAspectRatioX: Non-interlaced stream - not required.*/
		0x00,                   			/*bAspectRatioY: Non-interlaced stream - not required.*/
		0x00,                   			/*bmInterlaceFlags: Non-interlaced stream.*/
		0x00,                   			/*bCopyProtect: No restrictions imposed on the duplication of this video stream.*/

        /*Class-specific VS Video Frame Descriptor (MJPEG:1280x720) (0x1e byte)*/
		0x1e,                   		/*bLength: 0x36 byte*/
		CS_INTERFACE,               	/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_MJPEG,                 /*bDescriptorSubtype: VS_FRAME_MJPEG.*/
		VC_VIDEO_FRAME_SIZE_1280x720,	/*bFrameIndex: 5th frame descriptor.*/
		0x00,                   		/*bmCapabilities: Unused.*/
		0x00, 0x05,             		/*wWidth: Width of frame is 1280 pixels.*/
		0xd0, 0x02,             		/*wHeight: Height of frame is 720 pixels.*/
		0x00, 0x00, 0x77, 0x01, 		/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0x00, 0xca, 0x08, 		/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x60, 0x09, 0x00, 		/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),	/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		M_MidHiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		M_HiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		0x01,                   							/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),


        /*Class-specific VS Video Frame Descriptor (MJPEG:640x480) (0x1e byte)*/
		0x1e,                   		/*bLength: 0x36 byte*/
		CS_INTERFACE,               	/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_MJPEG,                 /*bDescriptorSubtype: VS_FRAME_MJPEG.*/
		VC_VIDEO_FRAME_SIZE_640x480,	/*bFrameIndex: 5th frame descriptor.*/
		0x00,                   		/*bmCapabilities: Unused.*/
		0x80, 0x02,             		/*wWidth: Width of frame is 640 pixels.*/
		0xe0, 0x01,             		/*wHeight: Height of frame is 480 pixels.*/
		0x00, 0x00, 0x77, 0x01, 		/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0x00, 0xca, 0x08, 		/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x60, 0x09, 0x00, 		/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),	/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		M_MidHiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		M_HiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		0x01,                   							/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		
		/*Class-specific VS Video Frame Descriptor (MJPEG:320x240) (0x1e byte)*/
		0x1e,                   		/*bLength: 0x36 byte*/
		CS_INTERFACE,              		/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_MJPEG,            		/*bDescriptorSubtype: VS_FRAME_MJPEG.*/
		VC_VIDEO_FRAME_SIZE_320x240,	/*bFrameIndex: Third frame descriptor.*/
		0x00,                   		/*bmCapabilities: Unused.*/
		0x40, 0x01,             		/*wWidth: Width of frame is 320 pixels.*/
		0xf0, 0x00,             		/*wHeight: Height of frame is 240 pixels.*/
		0x00, 0xc0, 0x5d, 0x00, 		/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0x80, 0x32, 0x02, 		/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x58, 0x02, 0x00, 		/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword      (VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),	/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		M_MidHiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		M_HiByteOfDword      (VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		0x01,            		     /*bFrameIntervalType: 1 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		
        /*Class-specific VS Video Frame Descriptor (MJPEG:160x120) (0x1e byte)*/
		0x1e,                   		/*bLength: 0x36 byte*/
		CS_INTERFACE,              		/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_MJPEG,			 		/*bDescriptorSubtype: VS_FRAME_MJPEG.*/
		VC_VIDEO_FRAME_SIZE_160x120,	/*bFrameIndex: First frame descriptor.*/
		0x00,                   		/*bmCapabilities: Unused.*/
		0xa0, 0x00,             		/*wWidth: Width of frame is 160 pixels.*/
		0x78, 0x00,            		/*wHeight: Height of frame is 120 pixels.*/
		0x00, 0x70, 0x17, 0x00, 		/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0xa0, 0x8c, 0x00, 		/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x96, 0x00, 0x00, 		/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),		/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		M_MidHiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		M_HiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_MJPEG),
		0x01,					/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
					
		/*Class-specific VS Color Matching Descriptor (0x06 byte)*/
		0x06,                   /*bLength: 0x06 byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x0d,                   /*bDescriptorSubtype: VS_COLORFORMAT*/
		0x01,                   /*bColorPrimaries: BT.709, sRGB.*/
		0x01,                   /*bTransferCharacteristics: BT.709*/
		0x04,                   /*bMatrixCoefficients: SMPTE 170M (BT.601)*/
// ---- MJPEG End ----

 #if (K_VIDEOFROMATNUMBER==2)  
		//-----------------------------------------
		//	yuv2 subLenth 0x0165 Byte
		//-----------------------------------------
//yuy2
		/*Class-specific VS Payload Format Descriptor (YUY2) (0x1b byte)*/
		0x1b,                     			/*bLength: 0x0b byte*/
		CS_INTERFACE,           			/*bDescriptorType: CS_INTERFACE.*/
		VS_FORMAT_UNCOMPRESSED,// VS_FORMAT_MJPEG, 			        /*bDescriptorSubtype: VS_FORMAT_MJPEG.*/
		VC_FORMAT_INDEX_UNCOMPRESSED_YUY2,//VC_FORMAT_INDEX_MJPEG_YUV422,				/*bFormatIndex: First format descriptor.*/
		VC_NUMBER_OF_FRAME_UNCOMPRESSED,		/*bNumFrameDescriptors*/
		0x59, 0x55, 0x59, 0x32, 				/*guidFormat: Globally Unique Identifier for YUV2.*/
		0x00, 0x00, 
		0x10, 0x00,
		0x80, 0x00, 
		0x00, 0xaa,
		0x00, 0x38, 0x9b, 0x71, 
		0x10,                   				/*bBitsPerPixel: Decoded frame has 16 bits per pixel*/  ///Ken@20031113
		0x01,//VC_DEFAULT_FRAME_INDEX_UNCOMPRESSED,    	/*bDefaultFrameIndex: Default frame index.*/
		0x00,                   			/*bAspectRatioX: Non-interlaced stream - not required.*/
		0x00,                   			/*bAspectRatioY: Non-interlaced stream - not required.*/
		0x00,                   			/*bmInterlaceFlags: Non-interlaced stream.*/
		0x00,                   			/*bCopyProtect: No restrictions imposed on the duplication of this video stream.*/

#if 1
		/*Class-specific VS Video Frame Descriptor (YUY2:1280x720) (0x1e byte)*/
		0x1e,                   				/*bLength: 0x36 byte*/
		CS_INTERFACE,                   		/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_UNCOMPRESSED,                 	/*bDescriptorSubtype: VS_FRAME_UNCOMPRESSED*/
		VC_VIDEO_FRAME_SIZE_1280x720,//  VC_VIDEO_FRAME_SIZE_1280x720,	/*bFrameIndex: 5th frame descriptor.*/
		0x00,                   				/*bmCapabilities: Unused.*/
		0x00, 0x05,             				/*wWidth: Width of frame is 640 pixels.*/
		0xd0, 0x02,             				/*wHeight: Height of frame is 480 pixels.*/
		0x00, 0x00, 0x77, 0x01, 				/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0x00, 0xca, 0x08, 				/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x60, 0x09, 0x00, 				/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		0x01,                   							/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
#endif	
/*Class-specific VS Video Frame Descriptor (YUY2:640x480) (0x1e byte)*/
		0x1e,                   				/*bLength: 0x36 byte*/
		CS_INTERFACE,                   		/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_UNCOMPRESSED,                 	/*bDescriptorSubtype: VS_FRAME_UNCOMPRESSED*/
		VC_VIDEO_FRAME_SIZE_640x480,	/*bFrameIndex: 5th frame descriptor.*/
		0x00,                   				/*bmCapabilities: Unused.*/
		0x80, 0x02,             				/*wWidth: Width of frame is 640 pixels.*/
		0xe0, 0x01,             				/*wHeight: Height of frame is 480 pixels.*/
		0x00, 0x00, 0x77, 0x01, 				/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0x00, 0xca, 0x08, 				/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x60, 0x09, 0x00, 				/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),		/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		M_MidHiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		M_HiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		0x01,                   							/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		
/*Class-specific VS Video Frame Descriptor (YUY2:320x240) (0x1e byte)*/
		0x1e,                   		/*bLength: 0x36 byte*/
		CS_INTERFACE,               	/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_UNCOMPRESSED,//VS_FRAME_MJPEG,                 /*bDescriptorSubtype: VS_FRAME_MJPEG.*/
		VC_VIDEO_FRAME_SIZE_320x240,//VC_VIDEO_FRAME_SIZE_640x480,	/*bFrameIndex: 5th frame descriptor.*/
		0x00,                   		/*bmCapabilities: Unused.*/
		0x40, 0x01,             		/*wWidth: Width of frame is 320 pixels.*/
		0xf0, 0x00,             		/*wHeight: Height of frame is 480 pixels.*/
		0x00, 0xc0, 0x5d, 0x00, 		/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0x80, 0x32, 0x02, 		/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x58, 0x02, 0x00, 		/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/ 
		M_LoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),	/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		M_MidHiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		M_HiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		0x01,                   							/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		
/*Class-specific VS Video Frame Descriptor (YUY2:160x120) (0x1e byte)*/
		0x1e,                   							/*bLength: 0x36 byte*/
		CS_INTERFACE,              							/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_UNCOMPRESSED,     							/*bDescriptorSubtype: VS_FRAME_UNCOMPRESSED*/
		VC_VIDEO_FRAME_SIZE_160x120,				/*bFrameIndex: First frame descriptor.*/
		0x00,                   							/*bmCapabilities: Unused.*/
		0xa0, 0x00,             							/*wWidth: Width of frame is 160 pixels.*/
		0x78, 0x00,             							/*wHeight: Height of frame is 120 pixels.*/
		0x00, 0x70, 0x17, 0x00, 							/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0xa0, 0x8c, 0x00, 							/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x96, 0x00, 0x00, 							/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),	/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		M_MidHiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		M_HiByteOfDword(VC_VIDEO_DEFAULT_FRAME_INTERVAL_UNCOMPRESSED),
		0x01,                   							/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_30FPS),
	
		/*Class-specific VS Color Matching Descriptor (0x06 byte)*/
		0x06,                   /*bLength: 0x06 byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x0d,                   /*bDescriptorSubtype: VS_COLORFORMAT*/
		0x01,                   /*bColorPrimaries: BT.709, sRGB.*/
		0x01,                   /*bTransferCharacteristics: BT.709*/
		0x04,                   /*bMatrixCoefficients: SMPTE 170M (BT.601)*/
#endif
};

xdata    UINT8   K_ConfigurationDescriptor_VS_FORMAT_USB11[] =
{
		/*Class-specific VS Input Header Descriptor (0x0f/0x0e byte)*/
        #if (K_VIDEOFROMATNUMBER==2)            
		0x0f,                /*bLength: 0x0f byte*/
        #else
              0x0e,               /*bLength: 0x0e byte*/
        #endif       
		0x24,                   /*bDescriptorType: CS_INTERFACE.*/
		0x01,                   /*bDescriptorSubtype: VS_INPUT_HEADER.*/
        #if (K_VIDEOFROMATNUMBER==2)  
		0x02,                    /*bNumFormats: two format descriptors follow.*/
		0x8C,0x00,//0x03,0x01,// mask YUY2 720P 0x21,0x01,            /*wTotalLength: Total size of class-specific VideoStreaming interface descriptors.*/
         #else
              0x01,                   /*bNumFormats: one format descriptors follow.*/
		0x97,0x00,//0x63,0x01,            /*wTotalLength: Total size of class-specific VideoStreaming interface descriptors.*/	
        #endif   
		0x81,                   /*bEndpointAddress: Address of the isochronous endpoint used for video data.*/
		0x00,                   /*bmInfo: Dynamic format change doesn't supported.*/
		0x04,                   /*bTerminalLink: This VideoStreaming interface supplies terminal ID 3 (Output Terminal).*/
		0x00,//0x02,                   /*bStillCaptureMethod: Device supports still image capture method 2.*/
		0x00,//0x01,                   /*bTriggerSupport: Hardware trigger supported for still image capture.*/
		0x00,                   /*bTriggerUsage: Hardware trigger should initiate a still image capture.*/
		0x01,                   /*bControlSize: Size of the bmaControls field.*/
		//0x00,                   /*BmaControls(1): Compression quality videoStreaming specific controls is supported.*/
								/* set to 0 to pass WHQL */
         #if (K_VIDEOFROMATNUMBER==2)            
		0x04,//0x00,                   /*BmaControls(1): Compression quality videoStreaming specific controls is supported. MJPEG*/
		0x00,                   /*BmaControls(2): No VideoStreaming specific controls are supported. YUY2*/
        #else
             0x04,//0x00,                   /*BmaControls(1): Compression quality videoStreaming specific controls is supported. MJPEG*/
		//0x00,                   /*BmaControls(2): No VideoStreaming specific controls are supported. YUY2*/
        #endif  
		
		//----------------------------------------
		//	MJPEG subLenth 0x0155Byte
		//----------------------------------------
		/*Class-specific VS Payload Format Descriptor (MJPEG) (0x0b byte)*/
		0x0b,                   			/*bLength: 0x0b byte*/
		CS_INTERFACE,           			/*bDescriptorType: CS_INTERFACE.*/
		VS_FORMAT_MJPEG, 			        /*bDescriptorSubtype: VS_FORMAT_MJPEG.*/
		VC_FORMAT_INDEX_MJPEG_YUV422,				/*bFormatIndex: First format descriptor.*/
		0X02,//VC_NUMBER_OF_FRAME_MJPEG,		/*bNumFrameDescriptors*/
		0x01,                   			/*bmFlags: Uses fixed size samples.*/
		0x01,//VC_DEFAULT_FRAME_INDEX_MJPEG,    	/*bDefaultFrameIndex: Default frame index.*/
		0x00,                   			/*bAspectRatioX: Non-interlaced stream - not required.*/
		0x00,                   			/*bAspectRatioY: Non-interlaced stream - not required.*/
		0x00,                   			/*bmInterlaceFlags: Non-interlaced stream.*/
		0x00,                   			/*bCopyProtect: No restrictions imposed on the duplication of this video stream.*/

		/*Class-specific VS Video Frame Descriptor (MJPEG:320x240) (0x1e byte)*/
		0x1e,                   		/*bLength: 0x36 byte*/
		CS_INTERFACE,              		/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_MJPEG,            		/*bDescriptorSubtype: VS_FRAME_MJPEG.*/
		VC_VIDEO_FRAME_SIZE_320x240,	/*bFrameIndex: Third frame descriptor.*/
		0x00,                   		/*bmCapabilities: Unused.*/
		0x40, 0x01,             		/*wWidth: Width of frame is 320 pixels.*/
		0xf0, 0x00,             		/*wHeight: Height of frame is 240 pixels.*/
		0x00, 0xc0, 0x5d, 0x00, 		/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0x80, 0x32, 0x02, 		/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x58, 0x02, 0x00, 		/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword      (VC_VIDEO_FRAME_INTERVAL_25FPS),	/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_HiByteOfDword      (VC_VIDEO_FRAME_INTERVAL_25FPS),
		0x01,            		     /*bFrameIntervalType: 1 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		

		/*Class-specific VS Video Frame Descriptor (MJPEG:160x120) (0x1e byte)*/
		0x1e,                   		/*bLength: 0x36 byte*/
		CS_INTERFACE,              		/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_MJPEG,			 		/*bDescriptorSubtype: VS_FRAME_MJPEG.*/
		VC_VIDEO_FRAME_SIZE_160x120,	/*bFrameIndex: First frame descriptor.*/
		0x00,                   		/*bmCapabilities: Unused.*/
		0xa0, 0x00,             		/*wWidth: Width of frame is 160 pixels.*/
		0x78, 0x00,            		/*wHeight: Height of frame is 120 pixels.*/
		0x00, 0x70, 0x17, 0x00, 		/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0xa0, 0x8c, 0x00, 		/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x96, 0x00, 0x00, 		/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),		/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		0x01,					/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		
		/*Class-specific VS Color Matching Descriptor (0x06 byte)*/
		0x06,                   /*bLength: 0x06 byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x0d,                   /*bDescriptorSubtype: VS_COLORFORMAT*/
		0x01,                   /*bColorPrimaries: BT.709, sRGB.*/
		0x01,                   /*bTransferCharacteristics: BT.709*/
		0x04,                   /*bMatrixCoefficients: SMPTE 170M (BT.601)*/
// ---- MJPEG End ----

 #if (K_VIDEOFROMATNUMBER==2)  
		//-----------------------------------------
		//	yuv2 subLenth 0x0165 Byte
		//-----------------------------------------
//yuy2
		/*Class-specific VS Payload Format Descriptor (YUY2) (0x1b byte)*/
		0x1b,                     			/*bLength: 0x0b byte*/
		CS_INTERFACE,           			/*bDescriptorType: CS_INTERFACE.*/
		VS_FORMAT_UNCOMPRESSED,// VS_FORMAT_MJPEG, 			        /*bDescriptorSubtype: VS_FORMAT_MJPEG.*/
		VC_FORMAT_INDEX_UNCOMPRESSED_YUY2,//VC_FORMAT_INDEX_MJPEG_YUV422,				/*bFormatIndex: First format descriptor.*/
		0X01,//VC_NUMBER_OF_FRAME_UNCOMPRESSED,		/*bNumFrameDescriptors*/
		0x59, 0x55, 0x59, 0x32, 				/*guidFormat: Globally Unique Identifier for YUV2.*/
		0x00, 0x00, 
		0x10, 0x00,
		0x80, 0x00, 
		0x00, 0xaa,
		0x00, 0x38, 0x9b, 0x71, 
		0x10,                   				/*bBitsPerPixel: Decoded frame has 16 bits per pixel*/  ///Ken@20031113
		0x01,   	/*bDefaultFrameIndex: Default frame index.*/
		0x00,                   			/*bAspectRatioX: Non-interlaced stream - not required.*/
		0x00,                   			/*bAspectRatioY: Non-interlaced stream - not required.*/
		0x00,                   			/*bmInterlaceFlags: Non-interlaced stream.*/
		0x00,                   			/*bCopyProtect: No restrictions imposed on the duplication of this video stream.*/
 	
/*Class-specific VS Video Frame Descriptor (YUY2:160x120) (0x1e byte)*/
		0x1e,                   							/*bLength: 0x36 byte*/
		CS_INTERFACE,              							/*bDescriptorType: CS_INTERFACE.*/
		VS_FRAME_UNCOMPRESSED,     							/*bDescriptorSubtype: VS_FRAME_UNCOMPRESSED*/
		VC_VIDEO_FRAME_SIZE_160x120,			/*bFrameIndex: First frame descriptor.*/
		0x00,                   							/*bmCapabilities: Unused.*/
		0xa0, 0x00,             							/*wWidth: Width of frame is 160 pixels.*/
		0x78, 0x00,             							/*wHeight: Height of frame is 120 pixels.*/
		0x00, 0x70, 0x17, 0x00, 							/*dwMinBitRate: Min bit rate in bits/s.*/
		0x00, 0xa0, 0x8c, 0x00, 							/*dwMaxBitRate: Max bit rate in bits/s.*/
		0x00, 0x96, 0x00, 0x00, 							/*dwMaxVideoFrameBufSize: Max video or still frame size in bytes.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),	/*dwDefaultFrameInterval:*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		0x01,                   							/*bFrameIntervalType: 7 discrete frame interval.*/
		M_LoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),		/*Frame interval 30fps*/
		M_MidLoByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_MidHiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
		M_HiByteOfDword(VC_VIDEO_FRAME_INTERVAL_25FPS),
	
		/*Class-specific VS Color Matching Descriptor (0x06 byte)*/
		0x06,                   /*bLength: 0x06 byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x0d,                   /*bDescriptorSubtype: VS_COLORFORMAT*/
		0x01,                   /*bColorPrimaries: BT.709, sRGB.*/
		0x01,                   /*bTransferCharacteristics: BT.709*/
		0x04,                   /*bMatrixCoefficients: SMPTE 170M (BT.601)*/
#endif
};

xdata    UINT8   K_ConfigurationDescriptor_VS_ALT_BULK[] =
{
#if 1
      	/*Standard VS Isochronous Video Data Endpoint Descriptor (0x07 byte)*/
		//Endpoint 1 (0x07 byte)
		0x07,                   /*bLength: 0x07 byte*/
		0x05,                   /*bDescriptorType: ENDPOINT descriptor.*/
		0x81,                   /*bEndpointAddress: IN endpoint 1.*/
		0x05,                   /*bmAttributes: Isochronous transfer type.*/
		0x00, 0x00,             /*wMaxPacketSize: 128 bytes.*/
		0x01,                   /*bInterval: Poll at least every 1ms or 125 us.*/
#endif		
		//--------------------------------------------
		//	Interface1 subLenth 0x70Byte
		//--------------------------------------------
		//Interface 1, AlternateSetting 1 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x01,                   //bInterfaceNumber: interface 1
		0x01,                   //bAlternateSetting: alternate setting 1
		0x01,                   /*bNumEndpoints: 1 endpoint*/
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO.*/
		SC_VIDEOSTREAMING,                   /*bInterfaceSubClass: SC_VIDEOSTREAMING.*/
		0x00,                   /*bInterfaceProtocol: PC_PROTOCOL_UNDEFINED.*/
		0x00,                   /*iInterface: Unused.*/

		/*Standard VS Isochronous Video Data Endpoint Descriptor (0x07 byte)*/
		//Endpoint 1 (0x07 byte)
		0x07,                   /*bLength: 0x07 byte*/
		0x05,                   /*bDescriptorType: ENDPOINT descriptor.*/
		0x81,                   /*bEndpointAddress: IN endpoint 1.*/
		0x05,                   /*bmAttributes: Isochronous transfer type.*/
		0x80, 0x00,             /*wMaxPacketSize: 128 bytes.*/
		0x01,                   /*bInterval: Poll at least every 1ms or 125 us.*/

		//Interface 1, AlternateSetting 2 (0x09 byte)
		/*Standard VS Interface Descriptor (Alternate Setting 2) (0x09 byte)*/
		0x09,                   /*bLength: 0x09 byte*/
		0x04,                   /*bDescriptorType: INTERFACE*/
		0x01,                   /*bInterfaceNumber: interface 1*/
		0x02,                   /*bAlternateSetting: alternate setting 2 */
		0x01,                   /*bNumEndpoints: 1 endpoint*/
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO.*/
		SC_VIDEOSTREAMING,                   /*bInterfaceSubClass: SC_VIDEOSTREAMING.*/
		0x00,                   /*bInterfaceProtocol: PC_PROTOCOL_UNDEFINED.*/
		0x00,                   /*iInterface: Unused.*/

		/*Standard VS Isochronous Video Data Endpoint Descriptor (0x07 byte)*/
		//Endpoint 1 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x81,                   //bEndpointAddress: IN endpoint 1
		0x05,                   //bmAttributes: Isochronous
		0x80, 0x01,             //wMaxPacketSize: 384 byte
		0x01,                   //bInterval: polling interval is 1 ms

		//Interface 1, AlternateSetting 3 (0x09 byte)
		/*Standard VS Interface Descriptor (Alternate Setting 2) (0x09 byte)*/
		0x09,                   /*bLength: 0x09 byte*/
		0x04,                   /*bDescriptorType: INTERFACE*/
		0x01,                   /*bInterfaceNumber: interface 1*/
		0x03,                   /*bAlternateSetting: alternate setting 3 */
		0x01,                   /*bNumEndpoints: 1 endpoint*/
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO.*/
		SC_VIDEOSTREAMING,                   /*bInterfaceSubClass: SC_VIDEOSTREAMING.*/
		0x00,                   /*bInterfaceProtocol: PC_PROTOCOL_UNDEFINED.*/
		0x00,                   /*iInterface: Unused.*/

		/*Standard VS Isochronous Video Data Endpoint Descriptor (0x07 byte)*/
		//Endpoint 1 (0x07 byte)
		0x07,                   /*bLength: 0x07 byte*/
		0x05,                   /*bDescriptorType: ENDPOINT descriptor.*/
		0x81,                   /*bEndpointAddress: IN endpoint 1.*/
		0x05,                   /*bmAttributes: Isochronous transfer type.*/
		0x00, 0x02,             /*wMaxPacketSize: 512 bytes.*/
		0x01,                   /*bInterval: Poll at least every 1ms or 125 us.*/

		//Interface 1, AlternateSetting 4 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x01,                   //bInterfaceNumber: interface 1
		0x04,                   //bAlternateSetting: alternate setting 4
		0x01,                   //bNumEndpoints: 1 endpoint
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO.*/
		SC_VIDEOSTREAMING,                   /*bInterfaceSubClass: SC_VIDEOSTREAMING.*/
		0x00,                   //bInterfaceProtocol: class specific protocol NOT used on this interface
		0x00,                   //iInterface: index of string

		/*Standard VS Isochronous Video Data Endpoint Descriptor (0x07 byte)*/
		//Endpoint 1 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x81,                   //bEndpointAddress: IN endpoint 1
		0x05,                   //bmAttributes: Isochronous
		0x80,0x0a,//0x80, 0x02,   //wMaxPacketSize: 640 byte,2 transactions per microframe
		0x01,                   //bInterval: polling interval is 1 ms

		//Interface 1, AlternateSetting 5 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x01,                   //bInterfaceNumber: interface 1
		0x05,                   //bAlternateSetting: alternate setting 5
		0x01,                   //bNumEndpoints: 1 endpoint
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO.*/
		SC_VIDEOSTREAMING,                   /*bInterfaceSubClass: SC_VIDEOSTREAMING.*/
		0x00,                   //bInterfaceProtocol: class specific protocol NOT used on this interface
		0x00,                   //iInterface: index of string

		/*Standard VS Isochronous Video Data Endpoint Descriptor (0x07 byte)*/
		//Endpoint 1 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x81,                   //bEndpointAddress: IN endpoint 1
		0x05,                   //bmAttributes: Isochronous
		0x00,0x0b,//0x00, 0x03,             //wMaxPacketSize: 768 byte
		0x01,                   //bInterval: polling interval is 1 ms

		//Interface 1, AlternateSetting 6 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x01,                   //bInterfaceNumber: interface 1
		0x06,                   //bAlternateSetting: alternate setting 6
		0x01,                   //bNumEndpoints: 1 endpoint
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO.*/
		SC_VIDEOSTREAMING,                   /*bInterfaceSubClass: SC_VIDEOSTREAMING.*/
		0x00,                   //bInterfaceProtocol: class specific protocol NOT used on this interface
		0x00,                   //iInterface: index of string

		/*Standard VS Isochronous Video Data Endpoint Descriptor (0x07 byte)*/
		//Endpoint 1 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x81,                   //bEndpointAddress: IN endpoint 1
		0x05,                   //bmAttributes: Isochronous
		0x80,0x0b,//0x80, 0x03,             //wMaxPacketSize: 896 byte
		0x01,                   //bInterval: polling interval is 1 ms

		//Interface 1, AlternateSetting 7 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x01,                   //bInterfaceNumber: interface 1
		0x07,                   //bAlternateSetting: alternate setting 7
		0x01,                   //bNumEndpoints: 1 endpoint
		CC_VIDEO,                   /*bInterfaceClass: CC_VIDEO.*/
		SC_VIDEOSTREAMING,                   /*bInterfaceSubClass: SC_VIDEOSTREAMING.*/
		0x00,                   //bInterfaceProtocol: class specific protocol NOT used on this interface
		0x00,                   //iInterface: index of string

		/*Standard VS Isochronous Video Data Endpoint Descriptor (0x07 byte)*/
		//Endpoint 1 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x81,                   //bEndpointAddress: IN endpoint 1
		0x05,                   //bmAttributes: Isochronous
		0xfc,0x0b,//0xfc, 0x03,             //wMaxPacketSize: 1020 byte
		0x01,                   //bInterval: polling interval is 1 ms

		//-------------------------------------------------------------------
			//Interface2 SubLength = 0x001e byte
		//-------------------------------------------------------------------
		//Interface 2 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x02,                   //bInterfaceNumber: interface 2
		0x00,                   //bAlternateSetting: alternate setting 0
		0x03,                   //bNumEndpoints: 3 endpoints
		0xFF,                   //bInterfaceClass: Vender Bulk
		0x00,                   //bInterfaceSubClass: UFI
		0x00,					//Bulk Only Rechie
		0x00,                   //iInterface: index of string

		//Endpoint 2 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x82,                   //bEndpointAddress: IN endpoint 2
		0x02,                   //bmAttributes: Bulk
		0x00, 0x02,             //wMaxPacketSize: 512 byte
		0x00,                   //bInterval: ignored

		//Endpoint 3 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x03,                   //bEndpointAddress: OUT endpoint 3
		0x02,                   //bmAttributes: Bulk
		0x00, 0x02,             //wMaxPacketSize: 512 byte
		0x00,                   //bInterval: ignored

		//Endpoint 4 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x84,                   //bEndpointAddress: IN endpoint 4
		0x03,                   //bmAttributes: Interrupt
		0x10, 0x00,             //wMaxPacketSize: 16 byte
		0x01,                   //bInterval: polling interval is 1 ms
};

#if (K_AUDIOINTER)
xdata    UINT8   K_ConfigurationDescriptor_ADC[] =
{
		//-------------------------------------------------------------------
		//		K_ADIO Interface TotalLength = 0x016d byte 
		//-------------------------------------------------------------------
//patch4.5@richie@audio interface begin

//patch4.5@richie@audio interface end

		/* Interface Association Descriptor (0x08 byte)*/
		0x08,							/*bLength: 0x08 byte*/
		0x0B,							/*bDescriptorType: INTERFACE ASSOCATION Descriptor*/
		0x03,                   		/*bInterfaceNumber: interface 2*/
		0x02,                   		/*bInterfaceCount: 2*/
		CC_AUDIO,                   	/*bFunctionClass: CC_AUDIO*/
		0x02,//SC_AUDIO_INTERFACE_COLLETION, 	/*bFunctionSubClass: 2*/
		0x00,                   		/*bInterfaceProtocol: class specific protocol NOT used on this interface*/
		0x00,                   		/*iFunction: 2*/

		//-------------------------------------------------------------------
		//	Interface3 SubLength = 0x2f byte 
		//-------------------------------------------------------------------
	//Interface 3: Audio Control Interface Descriptor
		//Standard AC Interface Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x03,                   //bInterfaceNumber: interface 3
		0x00,                   //bAlternateSetting: alternate setting 0
		0x00,                   //bNumEndpoints: 0 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x01,                   //bInterfaceSubClass: AUDIO_CONTROL
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Class-specific AC Interface Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x24,                   //bDescriptorType: CS_INTERFACE          
		0x01,                   //bDescriptorSubType: HEADER subtype        
		0x00, 0x01,             //bcdADC: revision of class specification - 1.00
		0x26, 0x00,             //wTotalLength: 0x1e byte ????
		0x01,                   //bInCollection: 1 streaming interface
		0x04,                   //baInterfaceNr(1): AudioStreaming interface 3 belongs to the AudioControl interface

		//Input Terminal Descriptor (0x0c byte)
		0x0c,                   //bLength: 0x0c byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x02,                   //bDescriptorSubType: INPUT_TERMINAL subtype
		0x01,                   //bTerminalID: ID of this input terminal is 1
		0x01, 0x02,          //wTerminalType: Microphone
		0x00, //0x04,         //bAssocTerminal: ID of the associated output terminal is 4
		0x01,                   //bNrChannels: 1 channel
		0x00, 0x00,          //wChannelConfig: no position bits
		0x00,                   //iChannelNames: index of string
		0x00,                   //iTerminal: index of string


		//Output Terminal Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x03,                   //bDescriptorSubType: OUTPUT_TERMINAL subtype
		0x03,// 0x04,        //bTerminalID: ID of this output terminal is 4
		0x01, 0x01,          //wTerminalType: USB streaming
		0x00, //0x01,         //bAssocTerminal: ID of the associated input terminal is 1
		0x05, //0x01,        //bSourceID: ID of source terminal is 1 (Microphone)
		0x00,                   //iTerminal: index of string

/* Feature Unit Descriptor - Microphone (0x08 byte)*/
		0x08,			/*bLength: 0x08 byte*/
		0x24,                   /*bDescriptorType: CS_INTERFACE*/
		0x06,                   /*bDescriptorSubType: feature unit subtype*/
		0x05,                   /*bUnitID: ID of this feature unit is 5*/
		0x01,		      /*bSourceID: from input terminal 1*/
		0x01,		      /*bControlSize*/
		0x03,		      /*bmaControls(0)*/
		0x00,		     /*iFeature*/

		//-------------------------------------------------------------------
		//	Interface4 SubLength = 0x0136 byte 
		//-------------------------------------------------------------------
	//Interface 4: Audio Streaming Interface Descriptor
		//Standard AS Interface Descriptor, AlternateSetting 0 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x04,                   //bInterfaceNumber: interface 4
		0x00,                   //bAlternateSetting: alternate setting 0
		0x00,                   //bNumEndpoints: 0 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x02,                   //bInterfaceSubClass: AUDIO_STREAMING
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Standard AS Interface Descriptor, AlternateSetting 1 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x04,                   //bInterfaceNumber: interface 4
		0x01,                   //bAlternateSetting: alternate setting 1
		0x01,                   //bNumEndpoints: 1 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x02,                   //bInterfaceSubClass: AUDIO_STREAMING
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Class-specific AS General Interface Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x03, //0x04,           //bTerminalLink: ID of output terminal is 4 (USB streaming)
		0x01,                   //bDelay: interface delay is 1 frame
		0x01, 0x00,             //wFormatTag: PCM format

		//Type I format Type Descriptor (0x0b byte)
		0x0b,                   //bLength: 0x0b byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x02,                   //bDescriptorSubType: FORMAT_TYPE subtype
		0x01,                   //bFormatType: FORMAT_TYPE_I
		0x01,                   //bNrChannels: 1 channel
		0x02,                   //bSubFrameSize: 2 byte per audio subframe
		0x10,                   //bBitResolution: 16 bits per sample
		0x01,                   //bSamFreqType: 1 frequency supported
		0x00, 0x00, 0x00,       //tSamFreq: 0 Hz

		//Standard AS Endpoint Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x86,                   //bEndpointAddress: IN endpoint 6
		0x05, //0x01,           //bmAttributes: Isochronous        
		0x20,0x00,// 32byte,0x10, 0x00,             //wMaxPacketSize: 16 byte
		0x04,                   //bInterval:  polling interval is 1 ms
		0x00,                   //bRefresh: 0 
		0x00,                   //bSyncAddress: 0

		//Class-specific AS Endpoint Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x25,                   //bDescriptorType: CS_ENDPOINT
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x01, //0x00,           //bmAttributes: no control is supported
		0x00,                   //bLockDelayUnits: 0
		0x00, 0x00,             //wLockDelay: 0

		//Standard AS Interface Descriptor, AlternateSetting 2 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x04,                   //bInterfaceNumber: interface 4
		0x02,                   //bAlternateSetting: alternate setting 2
		0x01,                   //bNumEndpoints: 1 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x02,                   //bInterfaceSubClass: AUDIO_STREAMING
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Class-specific AS General Interface Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x03, //0x04,           //bTerminalLink: ID of output terminal is 4 (USB streaming)
		0x01,                   //bDelay: interface delay is 1 frame
		0x01, 0x00,             //wFormatTag: PCM format

		//Type I format Type Descriptor (0x0b byte)
		0x0b,                   //bLength: 0x0b byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x02,                   //bDescriptorSubType: FORMAT_TYPE subtype
		0x01,                   //bFormatType: FORMAT_TYPE_I
		0x01,                   //bNrChannels: 1 channel
		0x02,                   //bSubFrameSize: 2 byte per audio subframe
		0x10,                   //bBitResolution: 16 bits per sample
		0x01,                   //bSamFreqType: 1 frequency supported
		0x00, 0x00, 0x00,       //tSamFreq: 0 Hz

		//Standard AS Endpoint Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x86,                   //bEndpointAddress: IN endpoint 6
		0x05, //0x01,           //bmAttributes: Isochronous        
		0x40,0x00, //64 0x20, 0x00,             //wMaxPacketSize: 32 byte
		0x04,                   //bInterval:  polling interval is 1 ms
		0x00,                   //bRefresh: 0 
		0x00,                   //bSyncAddress: 0

		//Class-specific AS Endpoint Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x25,                   //bDescriptorType: CS_ENDPOINT
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x01, //0x00,           //bmAttributes: no control is supported
		0x00,                   //bLockDelayUnits: 0
		0x00, 0x00,             //wLockDelay: 0

		//Standard AS Interface Descriptor, AlternateSetting 3 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x04,                   //bInterfaceNumber: interface 4
		0x03,                   //bAlternateSetting: alternate setting 3 
		0x01,                   //bNumEndpoints: 1 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x02,                   //bInterfaceSubClass: AUDIO_STREAMING
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Class-specific AS General Interface Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x03, //0x04,           //bTerminalLink: ID of output terminal is 4 (USB streaming)
		0x01,                   //bDelay: interface delay is 1 frame
		0x01, 0x00,             //wFormatTag: PCM format

		//Type I format Type Descriptor (0x0b byte)
		0x0b,                   //bLength: 0x0b byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x02,                   //bDescriptorSubType: FORMAT_TYPE subtype
		0x01,                   //bFormatType: FORMAT_TYPE_I
		0x01,                   //bNrChannels: 1 channel
		0x02,                   //bSubFrameSize: 2 byte per audio subframe
		0x10,                   //bBitResolution: 16 bits per sample
		0x01,                   //bSamFreqType: 1 frequency supported
		0x00, 0x00, 0x00,       //tSamFreq: 0 Hz

		//Standard AS Endpoint Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x86,                   //bEndpointAddress: IN endpoint 6
		0x05, //0x01,           //bmAttributes: Isochronous        
		0x60,0x00,//96 0x30, 0x00,             //wMaxPacketSize: 48 byte
		0x04,                   //bInterval:  polling interval is 1 ms
		0x00,                   //bRefresh: 0 
		0x00,                   //bSyncAddress: 0

		//Class-specific AS Endpoint Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x25,                   //bDescriptorType: CS_ENDPOINT
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x01,//0x00,            //bmAttributes: no control is supported
		0x00,                   //bLockDelayUnits: 0
		0x00, 0x00,             //wLockDelay: 0

		//Standard AS Interface Descriptor, AlternateSetting 4 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x04,                   //bInterfaceNumber: interface 4
		0x04,                   //bAlternateSetting: alternate setting 4
		0x01,                   //bNumEndpoints: 1 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x02,                   //bInterfaceSubClass: AUDIO_STREAMING
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Class-specific AS General Interface Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x03, //0x04,           //bTerminalLink: ID of output terminal is 4 (USB streaming)
		0x01,                   //bDelay: interface delay is 1 frame
		0x01, 0x00,             //wFormatTag: PCM format

		//Type I format Type Descriptor (0x0b byte)
		0x0b,                   //bLength: 0x0b byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x02,                   //bDescriptorSubType: FORMAT_TYPE subtype
		0x01,                   //bFormatType: FORMAT_TYPE_I
		0x01,                   //bNrChannels: 1 channel
		0x02,                   //bSubFrameSize: 2 byte per audio subframe
		0x10,                   //bBitResolution: 16 bits per sample
		0x01,                   //bSamFreqType: 1 frequency supported
		0x00, 0x00, 0x00,       //tSamFreq: 0 Hz

		//Standard AS Endpoint Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x86,                   //bEndpointAddress: IN endpoint 6
		0x05, //0x01,           //bmAttributes: Isochronous        
		0x80,0x00,//128 0x40, 0x00,             //wMaxPacketSize: 64 byte
		0x04,                   //bInterval:  polling interval is 1 ms
		0x00,                   //bRefresh: 0 
		0x00,                   //bSyncAddress: 0

		//Class-specific AS Endpoint Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x25,                   //bDescriptorType: CS_ENDPOINT
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x01,//0x00,            //bmAttributes: no control is supported
		0x00,                   //bLockDelayUnits: 0
		0x00, 0x00,             //wLockDelay: 0

		//Standard AS Interface Descriptor, AlternateSetting 5 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x04,                   //bInterfaceNumber: interface 4
		0x05,                   //bAlternateSetting: alternate setting 5
		0x01,                   //bNumEndpoints: 1 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x02,                   //bInterfaceSubClass: AUDIO_STREAMING
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Class-specific AS General Interface Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x03, //0x04,           //bTerminalLink: ID of output terminal is 4 (USB streaming)
		0x01,                   //bDelay: interface delay is 1 frame
		0x01, 0x00,             //wFormatTag: PCM format

		//Type I format Type Descriptor (0x0b byte)
		0x0b,                   //bLength: 0x0b byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x02,                   //bDescriptorSubType: FORMAT_TYPE subtype
		0x01,                   //bFormatType: FORMAT_TYPE_I
		0x01,                   //bNrChannels: 1 channel
		0x02,                   //bSubFrameSize: 2 byte per audio subframe
		0x10,                   //bBitResolution: 16 bits per sample
		0x01,                   //bSamFreqType: 1 frequency supported
		0x00, 0x00, 0x00,       //tSamFreq: 0 Hz

		//Standard AS Endpoint Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x86,                   //bEndpointAddress: IN endpoint 6
		0x05, //0x01,           //bmAttributes: Isochronous        
		0xa0,0x00,//160 0x50, 0x00,             //wMaxPacketSize: 80 byte
		0x04,                   //bInterval:  polling interval is 1 ms
		0x00,                   //bRefresh: 0 
		0x00,                   //bSyncAddress: 0

		//Class-specific AS Endpoint Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x25,                   //bDescriptorType: CS_ENDPOINT
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x01, //0x00,           //bmAttributes: no control is supported
		0x00,                   //bLockDelayUnits: 0
		0x00, 0x00,             //wLockDelay: 0

		//Standard AS Interface Descriptor, AlternateSetting 6 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x04,                   //bInterfaceNumber: interface 4
		0x06,                   //bAlternateSetting: alternate setting 6
		0x01,                   //bNumEndpoints: 1 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x02,                   //bInterfaceSubClass: AUDIO_STREAMING
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Class-specific AS General Interface Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x03, //0x04,           //bTerminalLink: ID of output terminal is 4 (USB streaming)
		0x01,                   //bDelay: interface delay is 1 frame
		0x01, 0x00,             //wFormatTag: PCM format

		//Type I format Type Descriptor (0x0b byte)
		0x0b,                   //bLength: 0x0b byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x02,                   //bDescriptorSubType: FORMAT_TYPE subtype
		0x01,                   //bFormatType: FORMAT_TYPE_I
		0x01,                   //bNrChannels: 1 channel
		0x02,                   //bSubFrameSize: 2 byte per audio subframe
		0x10,                   //bBitResolution: 16 bits per sample
		0x01,                   //bSamFreqType: 1 frequency supported
		0x00, 0x00, 0x00,       //tSamFreq: 0 Hz

		//Standard AS Endpoint Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x86,                   //bEndpointAddress: IN endpoint 6
		0x05, //0x01,           //bmAttributes: Isochronous        
		0xc0,0x00,//192 0x60, 0x00,             //wMaxPacketSize: 96 byte
		0x04,                   //bInterval:  polling interval is 1 ms
		0x00,                   //bRefresh: 0 
		0x00,                   //bSyncAddress: 0

		//Class-specific AS Endpoint Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x25,                   //bDescriptorType: CS_ENDPOINT
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x01,//0x00,            //bmAttributes: no control is supported
		0x00,                   //bLockDelayUnits: 0
		0x00, 0x00,             //wLockDelay: 0

		//Standard AS Interface Descriptor, AlternateSetting 7 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x04,                   //bInterfaceNumber: interface 4
		0x07,                   //bAlternateSetting: alternate setting 7
		0x01,                   //bNumEndpoints: 1 endpoint
		0x01,                   //bInterfaceClass: AUDIO
		0x02,                   //bInterfaceSubClass: AUDIO_STREAMING
		0x00,                   //bInterfaceProtocol: 0
		0x00,                   //iInterface: index of string

		//Class-specific AS General Interface Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x03, //0x04,           //bTerminalLink: ID of output terminal is 4 (USB streaming)
		0x01,                   //bDelay: interface delay is 1 frame
		0x01, 0x00,             //wFormatTag: PCM format

		//Type I format Type Descriptor (0x0b byte)
		0x0b,                   //bLength: 0x0b byte
		0x24,                   //bDescriptorType: CS_INTERFACE
		0x02,                   //bDescriptorSubType: FORMAT_TYPE subtype
		0x01,                   //bFormatType: FORMAT_TYPE_I
		0x01,                   //bNrChannels: 1 channel
		0x02,                   //bSubFrameSize: 2 byte per audio subframe
		0x10,                   //bBitResolution: 16 bits per sample
		0x01,                   //bSamFreqType: 1 frequency supported
		0x80, 0xbb, 0x00,       //tSamFreq: 48000 Hz

		//Standard AS Endpoint Descriptor (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x86,                   //bEndpointAddress: IN endpoint 6
		0x05, //0x01,           //bmAttributes: Isochronous        
		0xe0,0x00,//224 0x70, 0x00,             //wMaxPacketSize: 112 byte
		0x04,                   //bInterval:  polling interval is 1 ms
		0x00,                   //bRefresh: 0 
		0x00,                   //bSyncAddress: 0

		//Class-specific AS Endpoint Descriptor (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x25,                   //bDescriptorType: CS_ENDPOINT
		0x01,                   //bDescriptorSubType: GENERAL subtype
		0x01,//0x00,            //bmAttributes: no control is supported
		0x00,                   //bLockDelayUnits: 0
		0x00, 0x00,             //wLockDelay: 0

//patch4.5@richie@audio interface begin

//patch4.5@richie@audio interface end
};
#endif  //if(K_AUDIOINTER)
#endif 
/////////////////////////////////////////////////////////////////////////////
//Descriptor Content of MSDC
//---- Device Descriptor ----
#define K_MSDC_DEVICE_DESCRIPTOR_SIZE        0x0012
xdata    UINT8   K_MSDC_DeviceDescriptor[] =
{
		//-------------------------------------------------------------------
		//TotalLength = 0x0012 byte
		//-------------------------------------------------------------------

		//Device (0x12 byte)
		0x12,                   //bLength: 0x12 byte
		0x01,                   //bDescriptorType: DEVICE
		0x00, 0x01,             //bcdUSB: version 1.00
		0x00,                   //bDeviceClass: independent interfaces
		0x00,                   //bDeviceSubClass: 0
		0x00,                   //bDeviceProtocol: class specific protocols NOT used on device basis
		0x40,                   //bMaxPacketSize0: maximum packet size for endpoint zero
		0xfc, 0x04,             //idVendor: vendor ID
		0x71, 0x01,             //idProduct: product ID (for MSDC)
		0x00, 0x01,             //bcdDevice: device release number in BCD
		0x01,                   //iManufacturer: index of string
		0x02,                   //iProduct: index of string 
		0x03,                   //iSerialNumber: index of string
		0x01,                   //bNumConfigurations: 1 configuration
};

//---- Configuration Descriptor of MSDC ----
#define K_MSDC_CONFIGURATION_DESCRIPTOR_SIZE 0x0027
xdata    UINT8   K_MSDC_ConfigurationDescriptor[] =
{
		//-------------------------------------------------------------------
		//SubLength = 0x0009 byte 
		//-------------------------------------------------------------------

		//Configuration (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x02,                   //bDescriptorType: CONFIGURATION        
		0x27, 0x00,             //wTotalLength:
		0x01,                   //bNumInterfaces: 1 interfaces
		0x01,                   //bConfigurationValue: configuration 1
		0x00,                   //iConfiguration: index of string
		0x80,                   //bmAttributes: bus powered
		0xFA,                   //MaxPower: 500 mA     

		//-------------------------------------------------------------------
		//SubLength = 0x001e byte
		//-------------------------------------------------------------------

		//Interface 1 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x00,                   //bInterfaceNumber: interface 0
		0x00,                   //bAlternateSetting: alternate setting 0
		0x03,                   //bNumEndpoints: 3 endpoints
		0x08,                   //bInterfaceClass: Mass Storage Devices Class
		0x06,                   //bInterfaceSubClass:
		0x50,		        //Bulk Only
		0x00,                   //iInterface: index of string

		//Endpoint 2 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x82,                   //bEndpointAddress: IN endpoint 7
		0x02,                   //bmAttributes: Bulk
		0x00, 0x02,             //wMaxPacketSize: 64 byte
		0x00,                   //bInterval: ignored

		//Endpoint 3 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x03,                   //bEndpointAddress: OUT endpoint 8
		0x02,                   //bmAttributes: Bulk
		0x00, 0x02,             //wMaxPacketSize: 64 byte
		0x00,                   //bInterval: ignored

		//Endpoint 4 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x84,                   //bEndpointAddress: IN endpoint 4
		0x03,                   //bmAttributes: Interrupt
		0x01, 0x00,             //wMaxPacketSize: 1 byte
		0x01,                   //bInterval: polling interval is 1 ms
};

//richie@si0401
/////////////////////////////////////////////////////////////////////////////
//Descriptor Content of SIDC
//---- Device Descriptor ----
#define K_SIDC_DEVICE_DESCRIPTOR_SIZE        0x0012
xdata    UINT8   K_SIDC_DeviceDescriptor[] =
{
		//-------------------------------------------------------------------
		//TotalLength = 0x0012 byte
		//-------------------------------------------------------------------

		//Device (0x12 byte)
		0x12,                   //bLength: 0x12 byte
		0x01,                   //bDescriptorType: DEVICE
		0x00, 0x01,             //bcdUSB: version 1.00
		0x00,                   //bDeviceClass: independent interfaces
		0x00,                   //bDeviceSubClass: 0
		0x00,                   //bDeviceProtocol: class specific protocols NOT used on device basis
		0x40,                   //bMaxPacketSize0: maximum packet size for endpoint zero
		0xfc, 0x04,             //idVendor: vendor ID
		0x72, 0x01,             //idProduct: product ID (for SIDC)
		0x00, 0x01,             //bcdDevice: device release number in BCD
		0x01,                   //iManufacturer: index of string
		0x02,                   //iProduct: index of string 
		0x03,                   //iSerialNumber: index of string
		0x01,                   //bNumConfigurations: 1 configuration
};

//---- Configuration Descriptor of SIDC ----
#define K_SIDC_CONFIGURATION_DESCRIPTOR_SIZE 0x0027
code    UINT8   K_SIDC_ConfigurationDescriptor[] =
{
		//-------------------------------------------------------------------
		//SubLength = 0x0009 byte 
		//-------------------------------------------------------------------

		//Configuration (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x02,                   //bDescriptorType: CONFIGURATION        
		0x27, 0x00,             //wTotalLength:
		0x01,                   //bNumInterfaces: 1 interfaces
		0x01,                   //bConfigurationValue: configuration 1
		0x00,                   //iConfiguration: index of string
		0x80,                   //bmAttributes: bus powered
		0xFA,                   //MaxPower: 500 mA     

		//-------------------------------------------------------------------
		//SubLength = 0x001e byte
		//-------------------------------------------------------------------

		//Interface 1 (0x09 byte)
		0x09,                   //bLength: 0x09 byte
		0x04,                   //bDescriptorType: INTERFACE
		0x00,                   //bInterfaceNumber: interface 1
		0x00,                   //bAlternateSetting: alternate setting 0
		0x03,                   //bNumEndpoints: 3 endpoints
		0x06,                   //bInterfaceClass: Still Image Devices Class
		0x01,                   //bInterfaceSubClass:
		0x01,			//Bulk Only
		0x00,                   //iInterface: index of string

		//Endpoint 2 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x82,                   //bEndpointAddress: IN endpoint 2
		0x02,                   //bmAttributes: Bulk
		0x40, 0x00,             //wMaxPacketSize: 64 byte
		0x00,                   //bInterval: ignored

		//Endpoint 3 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x03,                   //bEndpointAddress: OUT endpoint 8
		0x02,                   //bmAttributes: Bulk
		0x40, 0x00,             //wMaxPacketSize: 64 byte
		0x00,                   //bInterval: ignored

		//Endpoint 4 (0x07 byte)
		0x07,                   //bLength: 0x07 byte
		0x05,                   //bDescriptorType: ENDPOINT
		0x84,                   //bEndpointAddress: IN endpoint 9
		0x03,                   //bmAttributes: Interrupt
		0x40, 0x00,             //wMaxPacketSize: 1 byte
		0x01,                   //bInterval: polling interval is 1 ms
};


#define K_DEVICE_QUALIFIER_DESCRIPTOR_SIZE        0x000A
xdata    UINT8   K_DeviceQualifierDescriptor[] =
{
		//-------------------------------------------------------------------
		//TotalLength = 0x000A byte
		//-------------------------------------------------------------------

		//Device (0x0A byte)
		0x0A,                   //bLength: 0x0A byte
		0x06,                   //bDescriptorType: Device qualifier type
		0x00, 0x02,             //bcdUSB: version 2.00
		DEVICE_CLASS,//  0x00,                   //bDeviceClass: independent interfaces
		DEVICE_SUBCLASS,//0x00,                   //bDeviceSubClass: 0
		DEVICE_PROTOCOL,//0x00,                   //bDeviceProtocol: class specific protocols NOT used on device basis
		0x40,                   //bMaxPacketSize0: maximum packet size for endpoint zero
		0x01,		            //bNumberConfigurations: 1
		0x00,	                //bReserved.
};

#endif /*__STDDESC_H__*/


