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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
	 
#include "solution.h"
#include "initio.h"
#include "os_msg.h"
#include "app_msg.h"
#include "cardlink.h"
#include "main.h"
#include "gpio.h"
#include "gpio_api.h"
#include "key_scan.h"
#include "usbmsdc2.h"
#include "uiflow.h"
//#include "exifhdr.h"
#include "exiffunc.h"
#include "asicreg.h"
#include "cdsp_misc.h"
#include "hal_front.h"
#include "hal_device.h"
#include "sp1k_gpio_api.h"

#include "sp1k_disk_api.h"
#include "sp1k_dos_api.h"
#include "sp1k_aud_api.h"
#include "sp1k_rsvblk_api.h"
#include "sp1k_pb_api.h"
#include "sp1k_osd_api.h"
#include "sp1k_disp_api.h"
#include "sp1k_pv_api.h"
#include "sp1k_snap_api.h"
#include "sp1k_video_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"
#include "sp1k_hal_api.h"
#include "sp1k_vfs_api.h"
#include "sp1k_tmr_svc.h"
#include "sp1k_cdsp_api.h"
	 
#include "app_menu_tab.h"
#include "app_ui_osd.h"
#include "app_osd_icon.h"
#include "app_usb.h"
#include "app_still_play.h"
#include "app_audio_play.h"
#include "app_battery.h"
#include "app_dev_storage.h"
#include "app_dev_usb.h"
#include "app_dev_disp.h"
#include "app_pwr_mgr.h"

#include "app_still_view.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "app_main.h"
#include "app_battery.h"
#include "sp1k_dcf_api.h"
#include "dbg_def.h"

#include "exif_def.h"
#include "exif_entry.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define __FILE	__FILE_ID_APP_DEV_STORAGE__
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
 xdata UINT8 G_NandAlreadyMount = 0;
 
 /* For indicate SD card has been plug in or plug out */
 xdata UINT8 G_SDCardPlugInOut = 0;
 
 UINT8 xdata  storageReadyflag;
 UINT8 xdata G_SDCardPlug = 0xFF;//1:plugged in  0:no card
 static UINT8 cardErrFmt =0;
 static UINT8 cardErrFmtErr =0;
 static UINT8 hotFileDel =0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
extern xdata UINT32 G_Card_TotalCardSize;
extern xdata UINT8 exifVlcAtOdd;
extern xdata UINT16 G_JpgRstMcuNum;
extern xdata UINT8 TestFlag;
extern UINT8 speedSdOpen;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/



//-----------------------------------------------------------------------------
//appSDPlugAction
//-----------------------------------------------------------------------------
/**
 * @brief	SD plug action
 * @param	none
 * @retval	none
 * @see
 * @author	jintao.liu
 * @since	2008-02-23
 * @todo
 * @bug
*/
void appSDPlugAction(void)
{
	UINT8 value;
	devAttr_t* devAttr;
	if(TestFlag)
	{
		putchar('D');
	}

	//sp1kSD_CdCfg();
	value = sp1kSD_CdGet();
	//value = 0 , card plug in
	//value != 0 , card plug out
	if (G_SDCardPlug != value)
	{
//		G_SDCardPlugInOut = 0x01;
		devAttr = sp1kDevHdlGet();

		G_SDCardPlug = value;
		appAutoOffTimeReload();//jintao.liu 2008-4-29 auto power off
		if(value == 0)//card insert
		{
			devAttr[DEV_ID_CARD].devEn = 1;
			devAttr[DEV_ID_CARD].devSts = DEV_STS_CONNECTED;
			devAttr[DEV_ID_CARD].devType = DEV_TYPE_CARD_SD;
			devAttr[DEV_ID_DISK].devEn = 0;
			devAttr[DEV_ID_DISK].devSts = DEV_STS_CONNECTED;
			osMsgPost(SP1K_MSG_CARD_PLUG_IN);
			//printf("SD card Insert!\n");
		}
		else
		{
			devAttr[DEV_ID_CARD].devEn = 0;
			devAttr[DEV_ID_CARD].devSts = DEV_STS_DISCONNECTED;
			devAttr[DEV_ID_DISK].devEn = 1;
			devAttr[DEV_ID_DISK].devSts = DEV_STS_CONNECTED;
			if (sp1kDosRamStsGet() == TRUE) {
				devAttr[DEV_ID_DISK].devType = DEV_TYPE_DISK_DRAM;
			} else {
				devAttr[DEV_ID_DISK].devType = devAttr[DEV_ID_DISK].devType = sp1kDiskTypeGet();//DEV_TYPE_DISK_NAND; for mantis#0039418
			}
			osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
			//printf("SD card Eject!\n");
		}
	}
}

//-----------------------------------------------------------------------------
//appSDPlugStatusGet
//-----------------------------------------------------------------------------
/**
 * @brief	SD plug status get
 * @param	none
 * @retval	  1:card     0:no card
 * @see
 * @author	jintao.liu
 * @since	2008-02-23
 * @todo
 * @bug
*/
//return 1:card inserted
//		 2:no card
UINT8 appSDPlugStatusGet(void)
{
	if(G_SDCardPlug ==0)//card insert
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//-----------------------------------------------------------------------------
//appSDProtectStatusGet
//-----------------------------------------------------------------------------
/**
 * @brief	SD protection status get
 * @param	none
 * @retval	0:no protect     1:Write protect
 * @see
 * @author	jintao.liu
 * @since	2008-02-23
 * @todo
 * @bug
*/
//return  0:no protect     1:Write protect
UINT8 appSDProtectStatusGet(void)
{
	UINT8 ProtStatus;

	if(0 == appSDPlugStatusGet())//No sd
	{
		ProtStatus = 0;
	}
	else
	{
		ProtStatus = sp1kSDWriteProtectDetectGet();
	}

	#if 0 // (_HW_SOLUTION_ == _HW_EVB_)
	ProtStatus = 0;
	#endif

	return ProtStatus;
}

void appSdPowerRecycle(void)
{
//	UINT16 i;
//	UINT8 byteId, bitMsk;

	/* do SD power recycle */
	sp1kHalFlashModeSet(K_MEDIA_NONE, K_MEDIA_NONE, 0x01);  // set media type to gpio.
	

#if APP_PWR_RCC_MODE_INTNAL_MOS
	sp1kHalPwrMosSet(0);
	gpioByteDirSet(GPIO_BYTE_FM0, 0xF0, 0xF0);  // set fmgpio[7:4] as output.
	gpioByteDirSet(GPIO_BYTE_FM1, 0x03, 0x03);  // set fmgpio[9:8] as output
	gpioByteOutSet(GPIO_BYTE_FM0, 0xF0, 0x00);  // set fmgpio[7:4] to low.
	gpioByteOutSet(GPIO_BYTE_FM1, 0x03, 0x00);  // set fmgpio[9:8] to low.
	sp1kHalWait(100); //100ms(200ms on 1528)
	gpioByteDirSet(GPIO_BYTE_FM0, 0xF0, 0x00);  // set fmgpio[7:4] as input.
	gpioByteDirSet(GPIO_BYTE_FM1, 0x03, 0x00);  // set fmgpio[9:8] as input.	
	sp1kHalFlashModeSet(K_MEDIA_SDF,K_MEDIA_NANDF, 0x01);	
	sp1kHalPwrMosSet(1);
	sp1kHalWait(50); //50ms(100ms on 1528)
#else

#if 0
	gpioByteDirSet(GPIO_BYTE_FM1, 0x3F, 0x3E);  // set fmgpio[9:5] as output, and fmgpio[4] as input.
	gpioByteOutSet(GPIO_BYTE_FM1, 0x3E, 0x00);  // set fmgpio[9:5] to low.
#else
	gpioByteDirSet(GPIO_BYTE_FM0, 0xF0, 0xF0);  // set fmgpio[7:4] as output.
	gpioByteDirSet(GPIO_BYTE_FM1, 0x03, 0x03);  // set fmgpio[9:8] as output
	gpioByteOutSet(GPIO_BYTE_FM0, 0xF0, 0x00);  // set fmgpio[7:4] to low.
	gpioByteOutSet(GPIO_BYTE_FM1, 0x03, 0x00);  // set fmgpio[9:8] to low.
#endif
	pkgDedicateGpioGet(PKG_DEDICATE_PIN_MLC_SDPWRCTL, &byteId, &bitMsk);
	gpioByteDirSet(byteId, bitMsk, 0xFF);  // set card detect pin as output.
	gpioByteOutSet(byteId, bitMsk, 0xFF);  // set card detect pin to high.
	sp1kHalWait(100); //100ms(200ms on 1528)
	gpioByteOutSet(byteId, bitMsk, 0x00);  // set card detect pin to low.
	sp1kHalWait(1);;
	gpioByteDirSet(GPIO_BYTE_FM0, 0xF0, 0x00);  // set fmgpio[7:4] as input.
	gpioByteDirSet(GPIO_BYTE_FM1, 0x03, 0x00);  // set fmgpio[9:8] as input.	
	sp1kHalFlashModeSet(K_MEDIA_SDF,K_MEDIA_NANDF, 0x01);	
	sp1kHalWait(50); //50ms(100ms on 1528)	
	sp1kSD_CdCfg();/* set detect pin as input pin again */
#endif
}

//-----------------------------------------------------------------------------
//appSDPlugAction
//-----------------------------------------------------------------------------
/**
 * @brief	SD plug action
 * @param	none
 * @retval	none
 * @see
 * @author	jintao.liu
 * @since	2008-02-23
 * @todo
 * @bug
*/
void appStorageMount (UINT8 type) USING_0
{
	UINT8 sts=FALSE;
	UINT8 tmp = IE, retry;
	devAttr_t* devAttr;

	retry = 0;
	#if !SUPPORT_MCU_WTD
	IE = 0x81;
	#endif
	//sp1kDcfDir1NameSet("ICATH");  
	sp1kDiskMemInit();
	sp1kDiskGlobalInit();
	G_Card_Type = type;
    
#if (DOS_RSV_PART == 1)
    devAttr = sp1kDevHdlGet();
	devAttr += DEV_ID_DISK;
	if (devAttr->devType == DEV_TYPE_DISK_NAND) {
        /* mount the reserved partition: F:\ */
        if (sp1kDosRsvPartSizeGet()) {
            G_Card_Type = K_MEDIA_NANDF;
            G_ucStorageType = G_Card_Type;
            sp1kDosRsvPartActSet(1);
            sts = sp1kVfsInit(1);
            /* if there's error on the reserved partition, 
            format the reserved and common partition */
 			if( sts != TRUE ){
                appDiskPartRecover();
			}
            /* set the write protect attribute for reserved partition */
            if (bUsbPwr) {
                sp1kDosRsvPartWPSet(0);
            } else {
                sp1kDosRsvPartWPSet(1);
            }
        }
    }
#endif

	if (sp1kDosRamStsGet() == TRUE) {
		
#if 1 //mark off here will mount nand
		G_NandAlreadyMount |= 0x01;
		if(type == K_MEDIA_NANDF){
			//Not to mount Nand but DOS DRAM
			type = K_MEDIA_DOSRAM;
			G_Card_Type = type;
		}
#endif
		if((G_NandAlreadyMount & 0x02) != 0x02){
			G_Card_Type = K_MEDIA_DOSRAM;
			G_ucStorageType = 0xff;
			G_NandAlreadyMount |= 0x2;
			sts = sp1kVfsInit(1);
			if( sts != TRUE ){
				sp1kVfsFmt(1);
				sp1kVfsInit(0);
			}
			G_Card_Type = type;
		}
	}

#if 1 //def DOS32_SUPPORT

	/* add by phil for disk driver lib release s*/
	devAttr = sp1kDevHdlGet();
	devAttr += DEV_ID_DISK;
	if (devAttr->devType == DEV_TYPE_DISK_NAND) {
		//First Initial Nand
		if((type != K_MEDIA_NANDF) && ((G_NandAlreadyMount & 0x01) != 0x01)){
			G_Card_Type = K_MEDIA_NANDF;
			G_ucStorageType = G_Card_Type;
			G_NandAlreadyMount |= 0x01;
#if (DOS_RSV_PART == 1)
            if (sp1kDosRsvPartStsGet()) {
                sp1kDosRsvPartActSet(0);
            }
#endif
			sp1kVfsInit(1);
		}
		G_Card_Type = type;
	}
#endif

	G_ucStorageType = G_Card_Type;

_appStorageMount_retry:
	devAttr = sp1kDevHdlGet();
	if( G_Card_Type == K_MEDIA_SDF ){
		if(!speedSdOpen){
			appSdPowerRecycle();
		}
		devAttr += DEV_ID_CARD;
	}
	else {
		devAttr += DEV_ID_DISK;
#if (DOS_RSV_PART == 1)    
        if (devAttr->devType == DEV_TYPE_DISK_NAND) {
            if (sp1kDosRsvPartStsGet()) {
                sp1kDosRsvPartActSet(0);
            }        
        }
#endif    
	}

	sts = sp1kVfsInit(1);
#if 0//ndef DOS32_SUPPORT //eric for mantis #26173
        if( sts != TRUE && (G_DOS_Initial_Status == 3) && (G_Card_TotalCardSize > 2048) && (G_Card_Type == K_MEDIA_SDF)){
               osMsgPost(SP1K_MSG_CARD_NOT_SUPPORT);
			   IE = tmp;
			   return;
        }
#endif
	//sp1kPbFileOrdertableInit(); 	//xian --, move down
	#if !SUPPORT_MCU_WTD
	IE = tmp;
	#endif
	//printf("%s,%d,G_Card_Type=%bx,%bx\n",__FILE__,__LINE__, G_Card_Type,G_DOS_Initial_Status);

	if( sts != TRUE ){
		//osMsgFlush(MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN);
		if (sp1kDosRamStsGet() == TRUE) {
			if ( G_Card_Type == K_MEDIA_DOSRAM && devAttr->devType == DEV_TYPE_DISK_DRAM) {
				//printf("DosRam init fail!\n");
			}
		} else {
			if ( G_Card_Type == K_MEDIA_NANDF && devAttr->devType == DEV_TYPE_DISK_NAND) {
				//printf("Nand init fail!\n");

				//add for mantis #36958 @start
				if(sp1kDiskTypeGet() != DEV_TYPE_DISK_SPI)
				{
					osMsgPost(SP1K_MSG_NAND_ERROR);
				}
				//start for mantis #36958 @end 
				
			} else {
				if(retry == 0 ) {
					retry++;
					goto _appStorageMount_retry;
				}
				//if (!appSDPlugStatusGet()) {
				if (!sp1kSD_CdGet()) {
					//printf("Card init fail!\n");
					osMsgPost(SP1K_MSG_CARD_ERROR);
				} else {
					osMsgPost(SP1K_MSG_CARD_PLUG_OUT);
				}
			}
		}
	}
	else{
		sp1kPbFileOrdertableInit();
		
		if(appSDPlugStatusGet())
		{
			sp1kVideoStorageWriteSpeedTest();
		}
	}

	storageReadyflag = SUCCESS;
}


/**
 * @fn        void appDiskPartRecover(void)
 * @brief     appDiskPartRecover
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-5-26
 * @todo      N/A
 * @bug       N/A
*/
void appDiskPartRecover(void)
{
    sp1kDosRsvPartActSet(1);
    sp1kVfsInit(1);
    DOS_LableStrSet("DISK ROM");
    sp1kVfsFmt(1);
    sp1kVfsInit(1);
    
    sp1kDosRsvPartActSet(0);
    sp1kVfsInit(1);
    sp1kVfsFmt(0);
    sp1kVfsInit(1);
}


#ifdef DISK_DRV_SPI

#include "hal_spi.h"
extern void sfSimpleCmd(UINT8 cmd);
extern UINT8 sfStatusRead(void);
extern void sfStatusWrite(UINT8 sts);

/**
 * @fn		UINT8 appSfWPSet( UINT8 mode )
 * @brief	appSfWPSet
 * @param	mode: UNLOCK, LOCK_0, LOCK_ALL
 * @retva	sts: return stauts register
 * @see		NULL
 * @author	DELEI.GU
 * @since	2012-6-8
 * @todo	N/A
 * @bug		N/A
*/
UINT8 appSfWPSet( UINT8 mode )
{
	UINT8 sts;
	UINT8 val;

	/*
		Customer add the SPI write protect flow here

		Flow:

		1. switch the active storage device to SPI
		2. write the write protect register
		3. restore the active storage device to previous
	*/

#if SUPPORT_SPI_WRITE_CONTROL
		HAL_FlashMode(K_MEDIA_Spi, 0, 0);
#else
		HAL_FlashMode(K_MEDIA_NANDF, K_MEDIA_SDF, 0x01); /* NAND and SPI using the same interface */
#endif



	/* for EON EN25F16 */
	switch (mode) {
		case SPI_STS_LOCK_0: /* low half */
			val = 0x05;
			break;
		case SPI_STS_LOCK_ALL:
			#if SUPPORT_SPI_WRITE_CONTROL
			val = 0x27;
			#else
			val = 0x07;
			#endif
			break;
		case SPI_STS_UNLOCK:
		default:
			val = 0x00;
			break;
	}

	/* read current status */
	sts = sfStatusRead();
	
	/* set status */
	#if SUPPORT_SPI_WRITE_CONTROL
	// sts = (sts & ~0x9c) | (val << 2);
	sts = (sts & ~0xfc) | (val << 2);
	#else
	sts = (sts & ~0x1c) | (val << 2);
	#endif
	
	/* write the SPI register */
	sfSimpleCmd(0x06); /* write enable */
	sfStatusWrite(sts);
	sfSimpleCmd(0x04); /* write disable */

	/* check the status */
	/* check if write in progress*/
	do{
		sts = sfStatusRead();
	}while(sts & 0x01);

	/* check if restore the storage interface to card */
	//printf("media card(%bx,%bx)\n", G_Card_Type,(UINT8)K_MEDIA_CARD);
	if (G_Card_Type == K_MEDIA_CARD) {
		HAL_FlashMode(K_MEDIA_SDF, K_MEDIA_NANDF, 0x01);
	}

	return sts;
}
#endif


/**
 * @fn        UINT8 appBGImgDec(UINT32 resId, Exif_Decompress_Parameter* pImgAttr, UINT32 srcAddr, UINT32 dstAddr, UINT8 bWait)
 * @brief     appBGImgDec
 * @param     [in] resId
 * @param     [in] pImgAttr
 * @param     [in] srcAddr
 * @param     [in] dstAddr
 * @param     [in] bWait
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-5-26
 * @todo      N/A
 * @bug       N/A
*/
//UINT8 appBGImgDec(UINT32 resId, struct Exif_Decompress_Parameter* pImgAttr, UINT32 srcAddr, UINT32 dstAddr, UINT8 bWait)
UINT8 appBGImgDec(UINT32 resId, ExifImage_t* pImgAttr, UINT32 srcAddr, UINT32 dstAddr, UINT8 bWait)
{
	UINT8 opt;
	UINT32 size;
	UINT8 scaleFactor=0;
	
	ExifImage_t mainImage;

	/* load resource file */
	if (!sp1kNandRsvRead(resId, srcAddr << 1)) {
		//printf("load resource file Fail\n");
		ASSERT(0,1);
		return FAIL;
	}

	/* resource file size get */
	if (!sp1kNandRsvSizeGet(resId, NULL, &size)) {
		//printf("resource file size get Fail\n");
		ASSERT(0,1);
		return FAIL;
	}
	
	/* exif header parsing */
	if (!exifParserStart(exifGetMainInfo,&mainImage, srcAddr, size)) {
	//if (!exifParsingFile(srcAddr, size, 0, NULL, &MainImgPara, NULL)) {
		//printf("exifParsingFile Fail\n");
		ASSERT(0,1);
		return FAIL;
	}

	/* check if wait vlc decode finished */
	if (bWait) {
		opt = 0x00;
	} else {
		opt = 0x08; /* hit and run */
	}

	/* check YUV format and if the vlc data is at odd address */
	if (mainImage.dataType== K_File_DataType_YUV420) {
		opt |= 0x05;
	} else if (mainImage.dataType == K_File_DataType_YUV422) {
		opt |= 0x04;
	}
	if (exifVlcAtOdd) {
		exifVlcAtOdd = 0;
		opt |= 0x10;
	}
	if(mainImage.width>320)
	{
		scaleFactor= 320/(mainImage.width>>3);
	}

	/* image decode process */
	sp1kHalJpegDecode(
		mainImage.addr,
		mainImage.width,
		mainImage.height,
		dstAddr,
		scaleFactor,
		opt,
		G_JpgRstMcuNum
	);
	//printf("ph opt=%bx\n",opt);
	
	/* post param */
	if (pImgAttr) {
//		pImgAttr->Width = mainImage.width; 	//mark for 1280*960 photoframe
//		pImgAttr->Height = mainImage.height;
//		pImgAttr->Addr = mainImage.addr;
		pImgAttr->width = mainImage.width;
		pImgAttr->height = mainImage.height;
		pImgAttr->addr = mainImage.addr;
	}
	return SUCCESS;
}

#if _ENABLERLE_

extern UINT32 cmdHwGprmOsdDecodeFuc(UINT32 resID,UINT32 addrSrcByte, UINT32 addrDestByte);

/**
 * @fn        UINT8 appRleBGImgPlay(UINT32 resId, struct Exif_Decompress_Parameter* pImgAttr, UINT8 bWait)
 * @brief     appBGImgDec
 * @param     [in] resId
 * @param     [in] pImgAttr
 * @param     [in] srcAddr
 * @param     [in] dstAddr
 * @param     [in] bWait
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    dongdong.wang
 * @since     2010-5-26
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appRleBGImgPlay(UINT32 resId, ExifImage_t* pImgAttr, UINT8 bWait)
{

	UINT16 decWidth = 320U;
	UINT16 decHeight = 240U;
	UINT8 sts, opt;
	UINT32 size;
	//struct Exif_Decompress_Parameter MainImgPara;
	ExifImage_t mainImage;
	UINT32 addrSrc = ((sysMemAddrGet(MEM_IDX_STILL_PB_END))<<1)+0x4B000;
	UINT32 addrDest = ((sysMemAddrGet(MEM_IDX_STILL_PB_END))<<1)+0x4B000+8192;//2048;
	UINT32 decJpgAddr = sp1kPbDispBuffGet(1);
	
	cmdHwGprmOsdDecodeFuc(resId, addrSrc, addrDest);
	/*dramFileSave("OPEN0001JPG", addrDest>>1, 65536);*/
	
	/* set decode size */
	sp1kPbDecodeSizeSet(decWidth, decHeight);
	sp1kPbDispBuffSwitch();

	/* start-up image decode process */
	/* exif header parsing */
	sp1kNandRsvSizeGet(resId, NULL, &size);
	//sts = exifParsingFile(addrDest>>1, size, 0, NULL, &MainImgPara, NULL);
	sts = exifParserStart(exifGetMainInfo,&mainImage,addrDest>>1, size);
	if (sts != TRUE) {
		//printf("exifParsingFile Fail\n");
		return FAIL;
	}

	/* check if wait vlc decode finished */
	if (bWait) {
		opt = 0x00;
	} else {
		opt = 0x08; /* hit and run */
	}

	/* check YUV format and if the vlc data is at odd address */
	if (mainImage.dataType == K_File_DataType_YUV420) {
		opt |= 0x05;
	} else if (mainImage.dataType == K_File_DataType_YUV422) {
		opt |= 0x04;
	}
	if (exifVlcAtOdd) {
		exifVlcAtOdd = 0;
		opt |= 0x10;
	}
	/* image decode process */
	sp1kHalJpegDecode(
		mainImage.addr,
		mainImage.width,
		mainImage.height,
		decJpgAddr,
		0,
		opt,
		G_JpgRstMcuNum
	);

	/* post param */
	if (pImgAttr) {
		pImgAttr->width = mainImage.width;
		pImgAttr->height = mainImage.height;
	}
	//sp1kPbDecodeSizeGet(&decWidth, &decHeight);
	sp1kDispPbBufSet(sp1kPbDispBuffGet(1), decWidth, decHeight);
	return SUCCESS;	

}


UINT8 appRleBGImgPlayTest(UINT32 resId)
{
	appRleBGImgPlay(resId, NULL, 1);
	return SUCCESS;	
}

#endif


UINT8 appCardErrFmtErrChk(void)
{
	return  cardErrFmtErr;
}
UINT8 appCardErrFmtErrSet(UINT8 sta)
{
	cardErrFmtErr = sta;
	return cardErrFmtErr;
}

/**
 * @fn        UINT8 appCardErrFmtChk(void)
 * @brief     appCardErrFmtChk
 * @param     [in] void
 * @retval    return = cardErrFmt
 * @see       NULL
 * @author   sy
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appCardErrFmtChk(void)
{
	return  cardErrFmt;
}

/**
 * @fn        void appCardErrFmtSta(UINT8 sta)
 * @brief     appCardErrFmtSta
 * @param     [in] sta
 * @retval    return
 * @see       NULL
 * @author   sy
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
void appCardErrFmtSta(UINT8 sta)
{
	cardErrFmt = sta;
	if(sta)osMsgPost(SP1K_MSG_STATE_TO_MENU);
}
/**
 * @fn        UINT8 appFileDelHotKeyChk(void)
 * @brief     appFileDelHotKeyChk
 * @param     [in] void
 * @retval    return = hotFileDel
 * @see       NULL
 * @author   sy
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8 appFileDelHotKeyChk(void)
{
	return  hotFileDel;
}
/**
 * @fn        void appFileDelHotKeySta(UINT8 sta)
 * @brief     appFileDelHotKeySta
 * @param     [in] sta
 * @retval    return 
 * @see       NULL
 * @author   sy
 * @since     2010-8-23
 * @todo      N/A
 * @bug       N/A
*/
void appFileDelHotKeySta(UINT8 sta)
{
	UINT8 LastMode = appPreStateGet(0);
	
	hotFileDel = sta;
	if(sta){
		osMsgPost(SP1K_MSG_STATE_TO_MENU);
	} else if(LastMode == APP_STATE_VIDEO_VIEW || APP_STATE_STILL_VIEW) {
		appPvForceUpdateOn(1);
	}
}


#if SUPPORT_SPI_WRITE_CONTROL
void appSpiWpSet(UINT8 flag) {
	switch (flag) {
		case 0:
			appSetGpio6SpiWP(1);
			appSfWPSet(SPI_STS_UNLOCK);
			break;

		case 1:
			appSfWPSet(SPI_STS_LOCK_ALL);
			appSetGpio6SpiWP(0);
			break;
	}
}
#endif

