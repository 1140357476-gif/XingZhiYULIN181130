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
 * @file      Usbmsdc2.h
 * @brief     Brief
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __USBMSDC2_H__
#define __USBMSDC2_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 //Class Request
#define K_USBMSDC_BULK_ONLY_MASS_STORAGE_RESET  0xff
#define K_USBMSDC_GET_MAX_LUN                   0xfe

#define K_USBMSDC_CommandPassed                 0x00         
#define K_USBMSDC_CommandFailed                 0x01         
#define K_USBMSDC_PhaseError                    0x02      
//richie@1203 add for read capacity no medium phase
#define K_USBMSDC_NoMedium                      0x03   

#define K_USBMSDC_CBW_SIZE              0x1f
#define K_USBMSDC_CSW_SIZE              0x0d
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
//Command Block Wrapper (CBW)
typedef struct _USBMSDC_CBW
{
        UINT32   dCBWSignature;
        UINT32   dCBWTag;
        UINT32   dCBWDataTransferLength;
        UINT8   bmCBWFlags;
        UINT8   bCBWLUN;
        UINT8   bCBWCBLength;
        UINT8   CBWCB[16];
} USBMSDC_CBW, *PUSBMSDC_CBW;

//Command Status Wrapper (CSW)
typedef struct _USBMSDC_CSW
{
        UINT32   dCSWSignature;
        UINT32   dCSWTag;
        UINT32   dCSWDataResidue;
        UINT8   bCSWStatus;
} USBMSDC_CSW, *PUSBMSDC_CSW; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern  PUSBMSDC_CBW    G_USBMSDC_pCBW;

extern  xdata   UINT32   G_USBMSDC_ulLogicalBlockAddress;
extern  xdata   UINT32   G_USBMSDC_ulSdramBufferAddress;
extern  xdata   UINT32   G_USBMSDC_usSizeProcessed;
extern  xdata   UINT8   G_USBMSDC_ucSenseCodeIdx;
extern  xdata   UINT8   G_USBMSDC_ucCSWStatus;

//richie
extern  xdata   UINT32   G_USBMSDC_ulAllocateLength;
extern  xdata   UINT8   G_USBMSDC_ulLatestCmd;
extern  xdata   UINT8   G_USBMSDC_ubRefreshNeeded;
//patch4.5@richie@eject
extern  xdata   UINT8   G_USBMSDC_ucEject;
extern	xdata   UINT8   G_ucMSDC_MLUN;
extern	xdata	UINT8	G_MLUN_Media_ucEject[];
extern	xdata 	UINT8	G_MLUN_Media_Pos_Next;
//patch5.1@richie@mlun eject begin 
extern  xdata   UINT8   G_USBMSDC_ucCardLock;
extern 	UINT8	ejectPowerOffFlag;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void usbResetEjectFlag(void);
void usbSetEject(UINT8 cardType);
void USBMSDC_BoNoDataPhase(void);
void USBMSDC_BoPreInDataPhase(void);
void USBMSDC_BoPreOutDataPhase(void);
void USBMSDC_BoPostInDataPhase(void);
void USBMSDC_BoPostOutDataPhase(void);
void USBMSDC_BoBulkInData(void);
void USBMSDC_BoBulkOutData(void);
void USBMSDC_BoClearCommandBuffer(void);
UINT8 USBMSDC_BoCommand(UINT8 len);
UINT8 USBMSDC_BoStatus(void);
void USBMSDC_BoSetBulkInPacket(UINT16 len);
UINT8 USBMSDC_BoGetBulkOutPacket(void);
UINT8 USBMSDC_BoGetBulkOutPacketToBlock(void);
UINT8 USBMSDC_BoPreBulkOutDataPhase(void);
UINT8 USBMSDC_BoPostBulkOutDataPhase(void);
void USBMSDC_BoPostInBlock(void);
void USBMSDC_StallBulkInOutPacket(void);
void USBMSDC_ClearStallBulkInOutPacket(void);
//patch4.5@richie@mlun
void MLUN_Initialize(void);
void MLUN_CheckCard(void);
void MLUN_CheckCard_Intr(void);
UINT8 usbSetLunPos(UINT8 cardType,UINT8 pos);
void ejectPwrOffEnable(UINT8 enable);

#endif /*#ifndef __USBMSDC2_H__*/