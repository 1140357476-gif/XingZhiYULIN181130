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
 * @file        reg_usb.h
 * @brief      USB register definition
 * @author   Kai.wang
 * @since     2011-02-23
 * @date      2011-02-23
*/

#ifndef __REG_USB_H__
#define __REG_USB_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	REG_Usb_EpAckIntSts           (0x2500)
#define	REG_Usb_EpNakIntSts	          (0x2501)		
#define	REG_Usb_IntSts	              (0x2502)
#define	REG_Usb_StdCmdIntSts          (0x2503)
#define	REG_Usb_EpAckIntEn	          (0x2504)
#define	REG_Usb_EpNakIntEn	          (0x2505)
#define	REG_Usb_IntEn                 (0x2506)
#define	REG_Usb_StdCmdIntEn	          (0x2507)
#define	REG_Usb_UsbClkEn              (0x2508)

#define	REG_Usb_TSelect0              (0x250a)
#define	REG_Usb_TSelect1              (0x250b)
#define	REG_Usb_UsbPrbData0           (0x250c)
#define REG_Usb_UsbPrbData1           (0x250d)
#define	REG_Usb_BISTMode              (0x2510)

#define	REG_Usb_SramBIST              (0x2511)
#define	REG_Usb_CpuWRUsbSram          (0x2512)
#define	REG_Usb_RmWakeEn              (0x2515)
#define	REG_Usb_Bit_UTMIClkSel        (0x2516)
#define	REG_Usb_Status                (0x2517)

#define	REG_Usb_Suspend               (0x2518)
#define	REG_Usb_Phy_SimMode           (0x2519)
#define	REG_Usb_PGMode                (0x251b)
#define	REG_Usb_PhyDbgStsInfo0        (0x251c)
#define	REG_Usb_PhyDbgStsInfo1        (0x251d)
#define	REG_Usb_DpDmSignal            (0x251f)
#define REG_Usb_HighSpeed             (0x2520)
#define REG_Usb_UDCPHYMode            (0x2521)
#define REG_Usb_Linesate              (0x2522)
#define REG_Usb_UsbCfg                (0x2524)
#define REG_Usb_UsbVidAs              (0x2525)
#define REG_Usb_UsbAudAs              (0x2526)
#define REG_Usb_bmRequestType         (0x2528)
#define REG_Usb_bRequest              (0x2529)
#define REG_Usb_wValueL               (0x252A)
#define REG_Usb_wValueH               (0x252B)
#define REG_Usb_wIndexL               (0x252C)
#define REG_Usb_wIndexH               (0x252D)
#define REG_Usb_wLengthL              (0x252E)
#define REG_Usb_wLengthH              (0x252F)
#define REG_Usb_LogicEp1_Disable      (0x2530)
#define REG_Usb_LogicEp2_5_Disable    (0x2531)
#define REG_Usb_LogicEp6_Disable0     (0x2532)
#define REG_Usb_LogicEp6_Disable1     (0x2533)
#define REG_Usb_InterfaceNumMax       (0x2534)
#define REG_Usb_InterfaceOpt          (0x2535)
#define REG_Usb_Alt_If0_NumMax        (0x2538)
#define REG_Usb_Alt_If1_NumMax        (0x2539)
#define REG_Usb_Alt_If2_3_NumMax      (0x253a)
#define REG_Usb_Alt_If4_NumMax        (0x253b)
#define REG_Usb_ICDebug               (0x253c)
#define REG_Usb_ECO1                  (0x253d)
#define REG_Usb_Det_resume_en         (0x253e)
#define REG_Usb_BulkoutNak_gate_en    (0x253f)

#define REG_Usb_Ep0BufData            (0x2540)
#define REG_Usb_BulkInData            (0x2541)
#define REG_Usb_BulkOutData           (0x2542)
#define REG_Usb_IntINData             (0x2543)
#define REG_Usb_Ep7BufData            (0x2544)
#define REG_Usb_TransactionEn         (0x2546)
#define REG_Usb_BulkAutoMode          (0x2547)
#define REG_Usb_IsoPipeEn             (0x2548)
#define REG_Usb_BufClr                (0x254a)
#define REG_Usb_VidAudBufRst          (0x254b)
#define REG_Usb_DMAFIFIOClr           (0x254c)
#define REG_Usb_TransactionStallEn    (0x254d)
#define REG_Usb_HeadLength            (0x254e)
#define REG_Usb_HeadInfo0             (0x254f)
#define REG_Usb_Ep0BufInPtr           (0x2550)
#define REG_Usb_Ep0BufOutPtr          (0x2551)
#define REG_Usb_BulkInBufInPtr0       (0x2552)
#define REG_Usb_BulkInBufInPtr1       (0x2553)
#define REG_Usb_BulkInBufOutPtr0      (0x2554)
#define REG_Usb_BulkInBufOutPtr1      (0x2555)
#define REG_Usb_BulkOutBufInPtr0      (0x2556)
#define REG_Usb_BulkOutBufInPtr1      (0x2557)
#define REG_Usb_BulkOutBufOutPtr0     (0x2558)
#define REG_Usb_BulkOutBufOutPtr1     (0x2559)
#define REG_Usb_IntInBufInPtr         (0x255a)
#define REG_Usb_IntInBufOutPtr        (0x255b)
#define REG_Usb_Ep7BufInPtr           (0x255c)
#define REG_Usb_Ep7BufOutPtr          (0x255d)
#define REG_Usb_Ep0BufSize            (0x2560)
#define REG_Usb_VidIsoBufSize0        (0x2561)
#define REG_Usb_VidIsoBufSize1        (0x2562)
#define REG_Usb_BulkInBufSize0        (0x2563)
#define REG_Usb_BulkInBufSize1        (0x2564)
#define REG_Usb_BulkOutBufSize0       (0x2565)
#define REG_Usb_BulkOutBufSize1       (0x2566)
#define REG_Usb_IntInBufSize          (0x2567)
#define REG_Usb_AudIsoBufSize0        (0x2568)
#define REG_Usb_AudIsoBufSize1        (0x2569)
#define REG_Usb_Ep7BufSize            (0x256a)
#define REG_Usb_UsbMode               (0x256f)
#define REG_Usb_BulkBaseAddr          (0x2570)
#define REG_Usb_VidBaseAddr           (0x2571)
#define REG_Usb_AudBaseAddr           (0x2572)
#define REG_Usb_BulkInAckCnt0         (0x2573)
#define REG_Usb_BulkInAckCnt1         (0x2574)
#define REG_Usb_BulkInAckCnt2         (0x2575)

#define REG_Usb_BulkOutAckCnt0        (0x2576)
#define REG_Usb_BulkOutAckCnt1        (0x2577)
#define REG_Usb_BulkOutAckCnt2        (0x2578)

#define REG_Usb_IntInAckCnt0          (0x2579)
#define REG_Usb_IntInAckCnt1          (0x257a)
#define REG_Usb_IntInAckCnt2          (0x257b)

#define REG_Usb_AckCntRst             (0x257c)
#define REG_Usb_DMAPktCntBkOut0       (0x2580)
#define REG_Usb_DMAPktCntBkOut1       (0x2581)
#define REG_Usb_DMAPktCntBkOut2       (0x2582)

#define REG_Usb_STCDiv0               (0x2584)
#define REG_Usb_STCDiv1               (0x2585)
#define REG_Usb_STCDiv2               (0x2586)
#define REG_Usb_AudIsoTestMode        (0x2588)

#define REG_Usb_Sof_TimeoutCntEn      (0x25a0)
#define REG_Usb_Sof_TimeoutCmp0       (0x25a1)
#define REG_Usb_Sof_TimeoutCmp1       (0x25a2)
#define REG_Usb_Sof_markInterval      (0x25a3)

#define REG_Usb_DMAPktCntBkIn0        (0x25a4)
#define REG_Usb_DMAPktCntBkIn1        (0x25a5)
#define REG_Usb_DMAPktCntBkIn2        (0x25a6)

#define REG_Usb_BulkInAckCnt_match    (0x25a7)
#define REG_Usb_PccamDataFormat       (0x25a9)


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

#endif  /* __REG_USB_H__ */
