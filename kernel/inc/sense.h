
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
#ifndef __SENSE_H__
#define __SENSE_H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//Sense Code
typedef struct _SCSIDATA_SENSECODE
{
        UINT8   senseKey;
        UINT8   additionalSenseCode;
        UINT8   additionalSenseCodeQualifier;
} SCSIDATA_SENSECODE, *PSCSIDATA_SENSECODE;

//richie@ref 0326 add one
code    struct  _SCSIDATA_SENSECODE K_SCSISENSE[0x28] =
{
        //  SK   ASC  ASCQ
        { 0x00, 0x00, 0x00 },   //0x00 - NO SENSE
        { 0x01, 0x17, 0x01 },   //0x01 - RECOVERD DATA WITH RETRIES
        { 0x01, 0x18, 0x00 },   //0x02 - RECOVERD DATA WITH ECC
        { 0x02, 0x04, 0x01 },   //0x03 - LOGICAL DRIVE NOT READY - BECOMING READY
        { 0x02, 0x04, 0x02 },   //0x04 - LOGICAL DRIVE NOT READY - INITIALIZATION REQUIRED
        { 0x02, 0x04, 0x04 },   //0x05 - LOGICAL UNIT NOT READY - FORMAT IN PROGRESS
        { 0x02, 0x04, 0xFF },   //0x06 - LOGICAL DRIVE NOT READY - DEVICE IS BUSY
        { 0x02, 0x06, 0x00 },   //0x07 - NO REFERENCE POSITION FOUND
        { 0x02, 0x08, 0x00 },   //0x08 - LOGICAL UNIT COMMUNICATION FAILURE
        { 0x02, 0x08, 0x01 },   //0x09 - LOGICAL UNIT COMMUNICATION TIME-OUT
        { 0x02, 0x08, 0x80 },   //0x0A - LOGICAL UNIT COMMUNICATION OVERRUN
        { 0x02, 0x3A, 0x00 },   //0x0B - MEDIUM NOT PRESENT
        { 0x02, 0x54, 0x00 },   //0x0C - USB TO HOST SYSTEM INTERFACE FAILURE
        { 0x02, 0x80, 0x00 },   //0x0D - INSUFFICIENT RESOURCES
        { 0x02, 0xFF, 0xFF },   //0x0E - UNKNOWN ERROR
        { 0x03, 0x02, 0x00 },   //0x0F - NO SEEK COMPLETE
        { 0x03, 0x03, 0x00 },   //0x10 - WRITE FAULT
        { 0x03, 0x10, 0x00 },   //0x11 - ID CRC ERROR
        { 0x03, 0x11, 0x00 },   //0x12 - UNRECOVERD READ ERROR
        { 0x03, 0x12, 0x00 },   //0x13 - ADDRESS MARK NOT FOUND FOR ID FIELD
        { 0x03, 0x13, 0x00 },   //0x14 - ADDRESS MARK NOT FOUND FOR DATA FIELD
        { 0x03, 0x14, 0x00 },   //0x15 - RECORDED ENTITY NOT FOUND
        { 0x03, 0x30, 0x01 },   //0x16 - CANNOT READ MEDIUM - UNKNOWN FORMAT
        { 0x03, 0x31, 0x01 },   //0x17 - FORMAT COMMAND FAILED
        { 0x04, 0x40, 0x80 },   //0x18 - DIAGNOSTIC FAILURE ON COMPONENT NN (80H-FFH)
        { 0x05, 0x1A, 0x00 },   //0x19 - PARAMETER LIST LENGTH ERROR
        { 0x05, 0x20, 0x00 },   //0x1A - INVALID COMMAND OPERATION CODE
        { 0x05, 0x21, 0x00 },   //0x1B - LOGICAL BLOCK ADDRESS OUT OF RANGE
        { 0x05, 0x24, 0x00 },   //0x1C - INVALID FIELD IN COMMAND PACKET   
        { 0x05, 0x25, 0x00 },   //0x1D - LOGICAL UNIT NOT SUPPORTED
        { 0x05, 0x26, 0x00 },   //0x1E - INVALID FIELD IN PARAMETER LIST
        { 0x05, 0x26, 0x01 },   //0x1F - PARAMETER NOT SUPPORTED 
        { 0x05, 0x26, 0x02 },   //0x20 - PARAMETER VALUE INVALID
        { 0x05, 0x39, 0x00 },   //0x21 - SAVING PARAMETERS NOT SUPPORT  
        { 0x06, 0x28, 0x00 },   //0x22 - NOT READY TO READY TRANSITION - MEDIA CHANGED
        { 0x06, 0x29, 0x00 },   //0x23 - POWER ON RESET OR BUS DEVICE RESET OCCURRED
        { 0x06, 0x2F, 0x00 },   //0x24 - COMMANDS CLEARED BY ANOTHER INITIATOR
        { 0x07, 0x27, 0x00 },   //0x25 - WRITE PROTECTED MEDIA
        { 0x0B, 0x4E, 0x00 },   //0x26 - OVERLAPPED COMMAND ATTEMPED
	//richie@ref 0326
	{ 0x06, 0x06, 0x00 },   //0x27 - REFRESH
};                                 

#endif /*__SENSE_H__*/
