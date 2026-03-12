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
 * @file      usbaudc.h
 * @brief     Module related to USB Audio Device Class
 * @author    Chi-Yeh Tsai
 * @since     2010-01-01
 * @date      2010-01-01
*/
#ifndef __USBAUDC_H__
#define __USBAUDC_H__
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
// Audio Interface Class Code
#define K_USBAUDC_CLASS                                 0x01
// Audio Interface Subclass Codes
#define K_USBAUDC_SUBCLASS_UNDEFINED                    0x00
#define K_USBAUDC_SUBCLASS_AUDIOCONTROL                 0x01
#define K_USBAUDC_SUBCLASS_AUDIOSTREAMING               0x02
#define K_USBAUDC_SUBCLASS_MIDISTREAMING                0x03
// Audio Interface Protocol Codes
#define K_USBAUDC_PR_PROTOCOL_UNDEFINED                 0x00
// Audio Class_Specific Descriptor Types
#define K_USBAUDC_CS_UNDEFINED                          0x20
#define K_USBAUDC_CS_DEVICE                             0x21
#define K_USBAUDC_CS_CONFIGURATION                      0x22
#define K_USBAUDC_CS_STRING                             0x23
#define K_USBAUDC_CS_INTERFACE                          0x24
#define K_USBAUDC_CS_ENDPOINT                           0x25
// Audio Class-Specific AC Interafce Descriptor Subtypes
#define K_USBAUDC_AC_DESCRIPTOR_UNDEFINED               0x00
#define K_USBAUDC_HEADER                                0x01        
#define K_USBAUDC_INPUT_TERMINAL                        0x02
#define K_USBAUDC_OUTPUT_TERMINAL                       0x03
#define K_USBAUDC_MIXER_UNIT                            0x04
#define K_USBAUDC_SELECTOR_UNIT                         0x05
#define K_USBAUDC_FEATURE_UNIT                          0x06
#define K_USBAUDC_PROCESSING_UNIT                       0x07
#define K_USBAUDC_EXTENSION_UNIT                        0x08
// Audio Class-Specific AS Interface Descriptor
#define K_USBAUDC_AS_DESCRIPTOR_UNDEFINED               0x00
#define K_USBAUDC_AS_GENERAL                            0x01
#define K_USBAUDC_FORMAT_TYPE                           0x02
#define K_USBAUDC_FORMAT_SPECIFIC                       0x03
// Processing Unit Process Types
#define K_USBAUDC_PROCESS_UNDEFINED                     0x00
#define K_USBAUDC_UPDOWNMIX_PROCESS                     0x01
#define K_USBAUDC_DOLBY_PROLOGIC_PROCESS                0x02
#define K_USBAUDC_3D_STEREO_EXTENDER_PROCESS            0x03
#define K_USBAUDC_REVERBERATION_PROCESS                 0x04
#define K_USBAUDC_CHORUS_PROCESS                        0x05
#define K_USBAUDC_DYN_RANGE_COMP_PROCESS                0x06
// Audio Class-Specific Endpoint Descriptor Subtypes
#define K_USBAUDC_DESCRIPTOR_UNDEFINED                  0x00
#define K_USBAUDC_EP_GENERAL                            0x01
// Audio Class-Specific Request Codes
#define K_USBAUDC_REQUEST_CODE_UNDEFINED                0x00
#define K_USBAUDC_SET_CUR                               0x01
#define K_USBAUDC_GET_CUR                               0x81
#define K_USBAUDC_SET_MIN                               0x02
#define K_USBAUDC_GET_MIN                               0x82
#define K_USBAUDC_SET_MAX                               0x03
#define K_USBAUDC_GET_MAX                               0x83
#define K_USBAUDC_SET_RES                               0x04
#define K_USBAUDC_GET_RES                               0x84
#define K_USBAUDC_SET_MEM                               0x05
#define K_USBAUDC_GET_MEM                               0x85
#define K_USBAUDC_GET_STAT                              0xff
// Terminal Control Selectors
#define K_USBAUDC_TE_CONTROL_UNDEFINED                  0x00
#define K_USBAUDC_COPY_PROTECT_CONTROL                  0x01
// Feature Unit Control Selectors
#define K_USBAUDC_FU_CONTROL_UNDEFINED                  0x00
#define K_USBAUDC_MUTE_CONTROL                          0x01
#define K_USBAUDC_VOLUME_CONTROL                        0x02
#define K_USBAUDC_BASS_CONTROL                          0x03
#define K_USBAUDC_MID_CONTROL                           0x04
#define K_USBAUDC_TREBLE_CONTROL                        0x05
#define K_USBAUDC_GRAPHIC_EQUALIZER_CONTROL             0x06
#define K_USBAUDC_AUTOMATIC_GAIN_CONTROL                0x07
#define K_USBAUDC_DELAY_CONTROL                         0x08
#define K_USBAUDC_BASS_BOOST_CONTROL                    0x09
#define K_USBAUDC_LOUDNESS_CONTROL                      0x0a
//---- Terminal Types -----------------
// USB Terminal Types
#define K_USBAUDC_TERMINAL_UNDEFINED                    0x0100
#define K_USBAUDC_TERMINAL_STREAMING                    0x0101
#define K_USBAUDC_TERMINAL_VENDOR_SPECIFIC              0x01ff
// Input Terminal Types
#define K_USBAUDC_TERMINAL_INPUT_UNDEFINED              0x0200
#define K_USBAUDC_TERMINAL_MICROPHONE                   0x0201
#define K_USBAUDC_TERMINAL_DESKTOP_MICTOPHONE           0x0202
#define K_USBAUDC_TERMINAL_PERSONAL_MICROPHONE          0x0203
#define K_USBAUDC_TERMINAL_OMNIDIRECTION_MICROPHONE     0x0204
#define K_USBAUDC_TERMINAL_MICROPHONE_ARRAY             0x0205
#define K_USBAUDC_TERMINAL_PROCESSING_MICROPHONE_ARRAY  0x0206
// External Terminal Types
#define K_USBAUDC_TERMINAL_EXTERNAL_UNDEFINED           0x0600
#define K_USBAUDC_TERMINAL_ANALOG_CONNECTOR             0x0601
#define K_USBAUDC_TERMINAL_DIGITAL_AUDIO_INTERFACE      0x0602
#define K_USBAUDC_TERMINAL_LINE_CONNECTOR               0x0603
#define K_USBAUDC_TERMINAL_LEGACY_AUDIO_CONNECTOR       0x0604
#define K_USBAUDC_TERMINAL_SPDIF_INTERFACE              0x0605
#define K_USBAUDC_TERMINAL_1394_DA_STREAM               0x0606
#define K_USBAUDC_TERMINAL_1394_DV_STREAM_SOUNDTRACK    0x0607
//---- Format Type Codes --------------
#define K_USBAUDC_FORMAT_TYPE_UNDEFINED                 0x00
#define K_USBAUDC_FORMAT_TYPE_I                         0x01
#define K_USBAUDC_FORMAT_TYPE_II                        0x02
#define K_USBAUDC_FORMAT_TYPE_III                       0x03
 
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
 
void USBAUDC_NoDataPhase(void);
void USBAUDC_PreInDataPhase(void);
void USBAUDC_PreOutDataPhase(void);
void USBAUDC_PostInDataPhase(void);
void USBAUDC_PostOutDataPhase(void);

#endif /*__USBAUDC_H__*/
