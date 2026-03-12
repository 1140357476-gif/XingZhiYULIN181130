/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __USBSTR_H__
#define __USBSTR_H__

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#define K_LANGUAGE_ID_STRING_SIZE       0x0004              
xdata    UINT8   K_LanguageIdString[] =
{
        //----------
        //0x04 Bytes
        //----------
        0x04,           //bLength
        0x03,           //bDescriptorType: STRING
        0x09, 0x04,     //bString: English (US)
};

#define K_MANUFACTURER_STRING_SIZE      0x0024
xdata    UINT8   K_ManufacturerString[] =
{
        //----------
        //0x24 Bytes
        //----------
        0x24,           //bLength
        0x03,           //bDescriptorType: STRING
         'I', 0x00,     //bString: Sunplus Co Ltd
         'c', 0x00,
         'a', 0x00,
         't', 0x00,
         'c', 0x00,
         'h', 0x00,
         't', 0x00,
	 'e', 0x00,
	 'k', 0x00,
         ' ', 0x00,
         'C', 0x00,
         'o', 0x00,
         ' ', 0x00,
         'L', 0x00,
         't', 0x00,
         'd', 0x00,
         ' ', 0x00,
};

#define K_PRODUCT_STRING_SIZE           0x002a
xdata    UINT8   K_ProductString[] =
{
        //----------
        //0x28 Bytes
        //----------
        0x2a,           //bLength
        0x03,           //bDescriptorType: STRING
         'I', 0x00,     //
         'c', 0x00,
         'a', 0x00,
         't', 0x00,
         'c', 0x00,
         'h', 0x00,
         't', 0x00,
         'e', 0x00,
         'k', 0x00,
         ' ', 0x00,
         'S', 0x00,
         'P', 0x00,
         'C', 0x00,
         'A', 0x00,
         '1', 0x00,
         '6', 0x00,
         '2', 0x00,
         '8', 0x00,
         ' ', 0x00,
         ' ', 0x00,
};

#define K_SERIAL_NUMBER_STRING_SIZE     0x001E
xdata    UINT8   K_SerialNumberString[] =
{
        //----------
        //0x1e Bytes
        //----------
        0x1E,           //bLength
        0x03,           //bDescriptorType: STRING
         '2', 0x00,
         '0', 0x00,
         '1', 0x00,
         '0', 0x00,
         '0', 0x00,     //bString: 01.00.00
         '8', 0x00,     
         '0', 0x00,
         '5', 0x00,
         ' ',  0x00,
         '0', 0x00,
         '1', 0x00,
         '.', 0x00,
         '0', 0x00,
         '0', 0x00,    
};

#if 0

#define K_CONFIGURATION_STRING_SIZE     0x0020
code    UINT8   K_ConfigurationString[] =
{
        //----------
        //0x20 Bytes
        //----------
        0x20,           //bLength
        0x03,           //bDescriptorType: STRING
         'c', 0x00,     //bString: configuration
         'o', 0x00,
         'n', 0x00,
         'f', 0x00,
         'i', 0x00,
         'g', 0x00,
         'u', 0x00,
         'r', 0x00,
         'a', 0x00,
         't', 0x00,
         'i', 0x00,
         'o', 0x00,
         'n', 0x00,
         ' ', 0x00,
         ' ', 0x00,
};

#define K_INTERFACE_STRING_SIZE         0x0020
code    UINT8   K_InterfaceString[] =
{
        //----------
        //0x20 Bytes
        //----------
        0x20,           //bLength
        0x03,           //bDescriptorType: STRING
         'i', 0x00,     //bString: interface
         'n', 0x00,
         't', 0x00,
         'e', 0x00,
         'r', 0x00,
         'f', 0x00,
         'a', 0x00,
         'c', 0x00,
         'e', 0x00,
         ' ', 0x00,
         ' ', 0x00,
         ' ', 0x00,
         ' ', 0x00,
         ' ', 0x00,
         ' ', 0x00,
};

#endif 
#endif /*__USBSTR_H__*/
