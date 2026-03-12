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
 * @file      dps_param.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __DPS_PARAM_H__
#define __DPS_PARAM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*---- DPS Parameter Results -------*/
#define	K_DPS_HEX_RESULT_Ok							0x10000000
#define	K_DPS_HEX_RESULT_NotExecuted					0x10010000
#define	K_DPS_HEX_RESULT_OperationNotSupported		0x10020000
#define	K_DPS_HEX_RESULT_OperationNotRecognized		0x10030000

/*---- DPS Parameter Minor Results -------*/
#define	K_DPS_HEX_MINRES_reasonDefault				0x0000
#define	K_DPS_HEX_MINRES_unrecogPara				0x0001
#define	K_DPS_HEX_MINRES_illegalPara				0x0002
#define	K_DPS_HEX_MINRES_missingPara				0x0003
#define	K_DPS_HEX_MINRES_bufferOverflow				0x0004

/*---- DPS Parameter for DPS_ConfigurePrintService -------*/
#define	K_DPS_HEX_PARAVAL_PrintSrvAvail_False		0x30000000
#define	K_DPS_HEX_PARAVAL_PrintSrvAvail_True		0x30010000

/*---- DPS Parameter for DPS_GetCapability -------*/
#define   K_DPS_HEX_PARAVAL_MASK_False			0x00010000

#define	K_DPS_HEX_PARAVAL_Quality_Default			0x50000000
#define	K_DPS_HEX_PARAVAL_Quality_Normal			0x50010000
#define	K_DPS_HEX_PARAVAL_Quality_Draft				0x50020000
#define	K_DPS_HEX_PARAVAL_Quality_Fine				0x50030000

#define	K_DPS_HEX_PARAVAL_PaperSize_Default			0x51000000
#define	K_DPS_HEX_PARAVAL_PaperSize_L				0x51010000
#define	K_DPS_HEX_PARAVAL_PaperSize_2L				0x51020000
#define	K_DPS_HEX_PARAVAL_PaperSize_HagakiPostcard	0x51030000
#define	K_DPS_HEX_PARAVAL_PaperSize_CardSize		0x51040000
#define	K_DPS_HEX_PARAVAL_PaperSize_100mmX150mm		0x51050000
#define	K_DPS_HEX_PARAVAL_PaperSize_4X6in			0x51060000
#define	K_DPS_HEX_PARAVAL_PaperSize_8X10in			0x51070000
#define	K_DPS_HEX_PARAVAL_PaperSize_Letter			0x51080000
#define	K_DPS_HEX_PARAVAL_PaperSize_11X17in			0x510A0000
#define	K_DPS_HEX_PARAVAL_PaperSize_A0				0x51100000
#define	K_DPS_HEX_PARAVAL_PaperSize_A1				0x51110000
#define	K_DPS_HEX_PARAVAL_PaperSize_A2				0x51120000
#define	K_DPS_HEX_PARAVAL_PaperSize_A3				0x51130000
#define	K_DPS_HEX_PARAVAL_PaperSize_A4				0x51140000
#define	K_DPS_HEX_PARAVAL_PaperSize_A5				0x51150000
#define	K_DPS_HEX_PARAVAL_PaperSize_A6				0x51160000
#define	K_DPS_HEX_PARAVAL_PaperSize_A7				0x51170000
#define	K_DPS_HEX_PARAVAL_PaperSize_A8				0x51180000
#define	K_DPS_HEX_PARAVAL_PaperSize_A9				0x51190000
#define	K_DPS_HEX_PARAVAL_PaperSize_B0				0x51200000
#define	K_DPS_HEX_PARAVAL_PaperSize_B1				0x51210000
#define	K_DPS_HEX_PARAVAL_PaperSize_B2				0x51220000
#define	K_DPS_HEX_PARAVAL_PaperSize_B3				0x51230000
#define	K_DPS_HEX_PARAVAL_PaperSize_B4				0x51240000
#define	K_DPS_HEX_PARAVAL_PaperSize_B5				0x51250000
#define	K_DPS_HEX_PARAVAL_PaperSize_B6				0x51260000
#define	K_DPS_HEX_PARAVAL_PaperSize_B7				0x51270000
#define	K_DPS_HEX_PARAVAL_PaperSize_B8				0x51280000
#define	K_DPS_HEX_PARAVAL_PaperSize_B9				0x51290000
#define	K_DPS_HEX_PARAVAL_PaperSize_89mmRoll		0x51810000
#define	K_DPS_HEX_PARAVAL_PaperSize_127mmRoll		0x51820000
#define	K_DPS_HEX_PARAVAL_PaperSize_100mmRoll		0x51860000
#define	K_DPS_HEX_PARAVAL_PaperSize_210mmRoll		0x51940000

#define	K_DPS_HEX_PARAVAL_PaperType_Default			0x52000000
#define	K_DPS_HEX_PARAVAL_PaperType_Plain			0x52010000
#define	K_DPS_HEX_PARAVAL_PaperType_Photo			0x52020000
#define	K_DPS_HEX_PARAVAL_PaperType_FastPhotp		0x52030000

#define	K_DPS_HEX_MINOR_PaperType_stationery			0x0001
#define	K_DPS_HEX_MINOR_PaperType_stationCoated		0x0002
#define	K_DPS_HEX_MINOR_PaperType_stationInkjet		0x0003
#define	K_DPS_HEX_MINOR_PaperType_stationPreprint	0x0004
#define	K_DPS_HEX_MINOR_PaperType_stationLetterhead	0x0005
#define	K_DPS_HEX_MINOR_PaperType_stationPrepunch	0x0006
#define	K_DPS_HEX_MINOR_PaperType_stationFine		0x0007
#define	K_DPS_HEX_MINOR_PaperType_stationHeavywei	0x0008
#define	K_DPS_HEX_MINOR_PaperType_stationLightwei	0x0009
#define	K_DPS_HEX_MINOR_PaperType_transparency		0x000A
#define	K_DPS_HEX_MINOR_PaperType_envelope			0x000B
#define	K_DPS_HEX_MINOR_PaperType_envePlain			0x000C
#define	K_DPS_HEX_MINOR_PaperType_enveWindow		0x000D
#define	K_DPS_HEX_MINOR_PaperType_continuous		0x000E
#define	K_DPS_HEX_MINOR_PaperType_contiLong			0x000F
#define	K_DPS_HEX_MINOR_PaperType_contiShort			0x0010
#define	K_DPS_HEX_MINOR_PaperType_tabStock			0x0011
#define	K_DPS_HEX_MINOR_PaperType_preCutTabs		0x0012
#define	K_DPS_HEX_MINOR_PaperType_fullCutTabs		0x0013
#define	K_DPS_HEX_MINOR_PaperType_multiPartForm		0x0014
#define	K_DPS_HEX_MINOR_PaperType_label				0x0015
#define	K_DPS_HEX_MINOR_PaperType_multiLayer		0x0016
#define	K_DPS_HEX_MINOR_PaperType_screen			0x0017
#define	K_DPS_HEX_MINOR_PaperType_scrPaged			0x0018
#define	K_DPS_HEX_MINOR_PaperType_photographic		0x0019
#define	K_DPS_HEX_MINOR_PaperType_photoGloss		0x001A
#define	K_DPS_HEX_MINOR_PaperType_photoHighGloss	0x001B
#define	K_DPS_HEX_MINOR_PaperType_photoSemiGloss	0x001C
#define	K_DPS_HEX_MINOR_PaperType_photoSatin		0x001D
#define	K_DPS_HEX_MINOR_PaperType_photoMatte		0x001E
#define	K_DPS_HEX_MINOR_PaperType_photFilm			0x001F
#define	K_DPS_HEX_MINOR_PaperType_backPrintFilm		0x0020
#define	K_DPS_HEX_MINOR_PaperType_cardStock			0x0021

#define	K_DPS_HEX_PARAVAL_FileType_Default			0x53000000
#define	K_DPS_HEX_PARAVAL_FileType_Exif_Jpeg			0x53010000
#define	K_DPS_HEX_PARAVAL_FileType_Exif				0x53020000
#define	K_DPS_HEX_PARAVAL_FileType_Jpeg				0x53030000
#define	K_DPS_HEX_PARAVAL_FileType_TifEp				0x53040000
#define	K_DPS_HEX_PARAVAL_FileType_FlashPix			0x53050000
#define	K_DPS_HEX_PARAVAL_FileType_Bmp				0x53060000
#define	K_DPS_HEX_PARAVAL_FileType_Ciff				0x53070000
#define	K_DPS_HEX_PARAVAL_FileType_Gif				0x53080000
#define	K_DPS_HEX_PARAVAL_FileType_Jfif				0x53090000
#define	K_DPS_HEX_PARAVAL_FileType_Pcd				0x530A0000
#define	K_DPS_HEX_PARAVAL_FileType_Pict				0x530B0000
#define	K_DPS_HEX_PARAVAL_FileType_Png				0x530C0000
#define	K_DPS_HEX_PARAVAL_FileType_Tiff				0x530D0000
#define	K_DPS_HEX_PARAVAL_FileType_TifIt				0x530E0000
#define	K_DPS_HEX_PARAVAL_FileType_Jp2				0x530F0000
#define	K_DPS_HEX_PARAVAL_FileType_Jpx				0x53100000
#define	K_DPS_HEX_PARAVAL_FileType_UndefineNonImg	0x53110000
#define	K_DPS_HEX_PARAVAL_FileType_Assocation		0x53120000
#define	K_DPS_HEX_PARAVAL_FileType_Script				0x53130000
#define	K_DPS_HEX_PARAVAL_FileType_Executable		0x53140000
#define	K_DPS_HEX_PARAVAL_FileType_Text				0x53150000
#define	K_DPS_HEX_PARAVAL_FileType_Html				0x53160000
#define	K_DPS_HEX_PARAVAL_FileType_XhtmlPrint		0x53170000
#define	K_DPS_HEX_PARAVAL_FileType_Dpof				0x53180000
#define	K_DPS_HEX_PARAVAL_FileType_Aiff				0x53190000
#define	K_DPS_HEX_PARAVAL_FileType_Wav				0x531A0000
#define	K_DPS_HEX_PARAVAL_FileType_Mp3				0x531B0000
#define	K_DPS_HEX_PARAVAL_FileType_Avi				0x531C0000
#define	K_DPS_HEX_PARAVAL_FileType_Mpeg				0x531D0000
#define	K_DPS_HEX_PARAVAL_FileType_Asf				0x531E0000

#define	K_DPS_HEX_PARAVAL_DatePrint_Default			0x54000000
#define	K_DPS_HEX_PARAVAL_DatePrint_Off				0x54010000
#define	K_DPS_HEX_PARAVAL_DatePrint_On				0x54020000

#define	K_DPS_HEX_PARAVAL_FileNamePrint_Default		0x55000000
#define	K_DPS_HEX_PARAVAL_FileNamePrint_Off			0x55010000
#define	K_DPS_HEX_PARAVAL_FileNamePrint_On			0x55020000

#define	K_DPS_HEX_PARAVAL_ImageOptimize_Default		0x56000000
#define	K_DPS_HEX_PARAVAL_ImageOptimize_Off			0x56010000
#define	K_DPS_HEX_PARAVAL_ImageOptimize_On			0x56020000

#define	K_DPS_HEX_PARAVAL_Layout_Default			0x57000000
#define	K_DPS_HEX_PARAVAL_Layout_1UpLayout			0x57010000
#define	K_DPS_HEX_PARAVAL_Layout_2UpLayout			0x57020000
#define	K_DPS_HEX_PARAVAL_Layout_4UpLayout			0x57040000
#define	K_DPS_HEX_PARAVAL_Layout_6UpLayout			0x57060000
#define	K_DPS_HEX_PARAVAL_Layout_8UpLayout			0x57080000
		/*....*/
#define	K_DPS_HEX_PARAVAL_Layout_250UpLayout		0x57FA0000
#define	K_DPS_HEX_PARAVAL_Layout_IndexPrint			0x57FE0000
#define	K_DPS_HEX_PARAVAL_Layout_1UpBorderLess		0x57FF0000

#define	K_DPS_HEX_PARAVAL_FixedSizes_Default		0x58000000
#define	K_DPS_HEX_PARAVAL_FixedSizes_2HX3Qin		0x58010000
#define	K_DPS_HEX_PARAVAL_FixedSizes_3HX5in			0x58020000
#define	K_DPS_HEX_PARAVAL_FixedSizes_4X6in			0x58030000
#define	K_DPS_HEX_PARAVAL_FixedSizes_5X7in			0x58040000
#define	K_DPS_HEX_PARAVAL_FixedSizes_8X10in			0x58050000
#define	K_DPS_HEX_PARAVAL_FixedSizes_254X178mm		0x58060000
#define	K_DPS_HEX_PARAVAL_FixedSizes_110X74mm		0x58070000
#define	K_DPS_HEX_PARAVAL_FixedSizes_89X55mm		0x58080000
#define	K_DPS_HEX_PARAVAL_FixedSizes_100X148mm		0x58090000
#define	K_DPS_HEX_PARAVAL_FixedSizes_6X8cm			0x580A0000
#define	K_DPS_HEX_PARAVAL_FixedSizes_7X10cm			0x580B0000
#define	K_DPS_HEX_PARAVAL_FixedSizes_9X13cm			0x580C0000
#define	K_DPS_HEX_PARAVAL_FixedSizes_10X15cm		0x580D0000
#define	K_DPS_HEX_PARAVAL_FixedSizes_13X18cm		0x580E0000
#define	K_DPS_HEX_PARAVAL_FixedSizes_15X21cm		0x580F0000
#define	K_DPS_HEX_PARAVAL_FixedSizes_18X24cm		0x58100000
#define	K_DPS_HEX_PARAVAL_FixedSizes_A4				0x58110000
#define	K_DPS_HEX_PARAVAL_FixedSizes_Letter			0x58120000

#define	K_DPS_HEX_PARAVAL_Cropping_Default			0x59000000
#define	K_DPS_HEX_PARAVAL_Cropping_Off				0x59010000
#define	K_DPS_HEX_PARAVAL_Cropping_On				0x59020000

/*---- DPS Parameter for DPS_Get/NotifyDeviceStatus -------*/
#define	K_DPS_HEX_PARAVAL_PrintSrvStatus_Printing	0x70000000
#define	K_DPS_HEX_PARAVAL_PrintSrvStatus_Idle		0x70010000
#define	K_DPS_HEX_PARAVAL_PrintSrvStatus_Paused		0x70020000

#define	K_DPS_HEX_PARAVAL_JobEndReason_JobNotEnded	0x71000000
#define	K_DPS_HEX_PARAVAL_JobEndReason_EndedNormal	0x71010000
#define	K_DPS_HEX_PARAVAL_JobEndReason_AbortImm		0x71020000
#define	K_DPS_HEX_PARAVAL_JobEndReason_AbortNext	0x71030000
#define	K_DPS_HEX_PARAVAL_JobEndReason_OtherReason	0x71040000

#define	K_DPS_HEX_PARAVAL_ErrorStatus_NoError		0x72000000
#define	K_DPS_HEX_PARAVAL_ErrorStatus_Warning		0x72010000
#define	K_DPS_HEX_PARAVAL_ErrorStatus_FatalError	0x72020000

#define	K_DPS_HEX_PARAVAL_ErrorReason_NoReason		0x73000000
#define	K_DPS_HEX_PARAVAL_ErrorReason_Paper			0x73010000
#define	K_DPS_HEX_PARAVAL_ErrorReason_Ink			0x73020000
#define	K_DPS_HEX_PARAVAL_ErrorReason_Hardware		0x73030000
#define	K_DPS_HEX_PARAVAL_ErrorReason_File			0x73040000

#define	K_DPS_HEX_MINOR_Reason_NoReason				0x0000
#define	K_DPS_HEX_MINOR_ReasonPaper_Default			0x0000
#define	K_DPS_HEX_MINOR_ReasonPaper_PaperEmpty		0x0100
#define	K_DPS_HEX_MINOR_ReasonPaper_LoadError		0x0200
#define	K_DPS_HEX_MINOR_ReasonPaper_EjectError		0x0300
#define	K_DPS_HEX_MINOR_ReasonPaper_MediaError		0x0400
#define	K_DPS_HEX_MINOR_ReasonPaper_PaperJam		0x0500
#define	K_DPS_HEX_MINOR_ReasonPaper_FewPaper		0x0600
#define	K_DPS_HEX_MINOR_ReasonPaper_WrongComb		0x0700
#define	K_DPS_HEX_MINOR_ReasonInk_Default			0x0000
#define	K_DPS_HEX_MINOR_ReasonInk_InkEmpty			0x0100
#define	K_DPS_HEX_MINOR_ReasonInk_InkLow			0x0200
#define	K_DPS_HEX_MINOR_ReasonInk_WasteError		0x0300
#define	K_DPS_HEX_MINOR_ReasonHardWare_Default		0x0000
#define	K_DPS_HEX_MINOR_ReasonHardWare_FatalErr		0x0100
#define	K_DPS_HEX_MINOR_ReasonHardWare_ServiceCall	0x0200
#define	K_DPS_HEX_MINOR_ReasonHardWare_PrinterOcc	0x0300
#define	K_DPS_HEX_MINOR_ReasonHardWare_PrinterBusy	0x0400
#define	K_DPS_HEX_MINOR_ReasonHardWare_LeverErr		0x0500
#define	K_DPS_HEX_MINOR_ReasonHardWare_CoverOpen	0x0600
#define	K_DPS_HEX_MINOR_ReasonHardWare_NoHead		0x0700
#define	K_DPS_HEX_MINOR_ReasonHardWare_InkCoverOpen	0x0800
#define	K_DPS_HEX_MINOR_ReasonHardWare_NoInkCart	0x0900
#define	K_DPS_HEX_MINOR_ReasonFile_Default			0x0000
#define	K_DPS_HEX_MINOR_ReasonFile_PrintInfoErr		0x0100
#define	K_DPS_HEX_MINOR_ReasonFile_FileDecodeErr	0x0200

#define	K_DPS_HEX_PARAVAL_DisconEnable_False		0x74000000
#define	K_DPS_HEX_PARAVAL_DisconEnable_True			0x74010000

#define	K_DPS_HEX_PARAVAL_CapChanged_False			0x75000000
#define	K_DPS_HEX_PARAVAL_CapChanged_True			0x75010000

#define	K_DPS_HEX_PARAVAL_NewJobOk_False			0x76000000
#define	K_DPS_HEX_PARAVAL_NewJobOk_True				0x76010000

/*---- DPS Parameter for DPS_AbortJob -------*/
#define	K_DPS_HEX_PARAVAL_AbortStyle_Immediately	0x90000000
#define	K_DPS_HEX_PARAVAL_AbortStyle_AfterCurrPage	0x90010000 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define dps_Check_Result(value)	(value  && (checkValue(value, K_DPS_HEX_RESULT_Ok)))
#define dps_Check_Quality(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_Quality_Default)))
#define dps_Check_PaperSize(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_PaperSize_Default)))
#define dps_Check_PaperType(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_PaperType_Default)))
#define dps_Check_FileType(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_FileType_Default)))
#define dps_Check_DatePrint(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_DatePrint_Default)))
#define dps_Check_FileNamePrint(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_FileNamePrint_Default)))
#define dps_Check_ImageOptimize(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_ImageOptimize_Default)))
#define dps_Check_Layout(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_Layout_Default)))
#define dps_Check_FixedSizes(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_FixedSizes_Default)))
#define dps_Check_Cropping(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_Cropping_Default)))
#define dps_Check_PrintSrvStatus(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_PrintSrvStatus_Printing)))
#define dps_Check_JobEndReason(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_JobEndReason_JobNotEnded)))
#define dps_Check_ErrorStatus(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_ErrorStatus_NoError)))
#define dps_Check_ErrorReason(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_ErrorReason_NoReason)))
#define dps_Check_DisconEnable(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_DisconEnable_False)))
#define dps_Check_CapChanged(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_CapChanged_False)))
#define dps_Check_NewJobOk(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_NewJobOk_False)))
#define dps_Check_AbortStyle(value)	(value  && (checkValue(value, K_DPS_HEX_PARAVAL_AbortStyle_Immediately)))


 
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



#endif
