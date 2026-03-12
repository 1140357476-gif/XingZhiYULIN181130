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
 * @file      Icat_stub.h
 * @brief     Will
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __I_CAT_H__
#define __I_CAT_H__


//------------------------------------------------------------------------------
//Config, should rebuild libsnap, libusb, libsysboot
//ICAT_OPTION is not ready
//------------------------------------------------------------------------------
//#define ICAT_OPTION
//#define AMCAP_OPTION
#if defined(AMCAP_OPTION) || defined(ICAT_OPTION)

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ICAT_REQUEST	0xb0
	
#define ICAT_CAP_FLOW_MODE	(UINT16)0
#define ICAT_RAW_FLOW_MODE	(UINT16)1<<0
#define ICAT_RAW_ONLY_MODE	(UINT16)1<<1
#define ICAT_JPG_UPLD_MODE  (UINT16)1<<2
	
#define ICAT_YUV_UPLD_MODE	(UINT16)1<<3
#define ICAT_YUV_DNLD_MODE  (UINT16)1<<4
#define ICAT_RAW_UPLD_MODE	(UINT16)1<<5
#define ICAT_RAW_DNLD_MODE  (UINT16)1<<6
#define ICAT_RES_UPLD_MODE	(UINT16)1<<7
#define ICAT_RES_DNLD_MODE  (UINT16)1<<8
	
	
/*selected job from AP*/
#define ICAT_JOB_WAIT_AP	0
#define ICAT_JOB_DO			1
#define ICAT_JOB_SKIP		2
#define ICAT_JOB_DONE		3
#define ICAT_JOB_STOP		4 	/*stop all bulk ....*/


/* ICat Command */
#define ICAT_MODE_SWITCH_IDX    	(UINT16)0x0000
#define ICAT_GET_STATUS_IDX     	(UINT16)0x0001
#define ICAT_GET_DEVCAPAC_IDX     	(UINT16)0x0002		/*DEVice CAPACity*/

#define ICAT_CAP_EXPOSURE_IDX   	(UINT16)0x0100		/*exp*/
#define ICAT_CAP_GAIN_IDX       	(UINT16)0x0101		/*agc*/
#define ICAT_CAP_START_IDX      	(UINT16)0x0102
#define ICAT_CAP_STOP_IDX       	(UINT16)0x0103

#define ICAT_CAP_CDSPMISC_IDX      	(UINT16)0x0104
#define ICAT_CAP_AEAWB_IDX       	(UINT16)0x0105



#define ICAT_RAW_GET_DLINFO_IDX 	(UINT16)0x0200		/*RAW Download*/
#define ICAT_RAW_DOWNLOAD_IDX   	(UINT16)0x0201
#define ICAT_RAW_GET_ULINFO_IDX 	(UINT16)0x0202		/*RAW Upload*/
		

#define ICAT_RES_START_IDX      	(UINT16)0x0300
#define ICAT_RES_DOWNLOAD_IDX   	(UINT16)0x0301
#define ICAT_RES_UPLOAD_IDX     	(UINT16)0x0302
#define ICAT_RES_NUM_IDX        	(UINT16)0x0303

#define ICAT_CDSP_PREPRO_IDX		(UINT16)0x0400		/*PreProcess/pretreatment*/
#define ICAT_CDSP_START_IDX     	(UINT16)0x0401
#define ICAT_CDSP_STOP_IDX      	(UINT16)0x0402

#define ICAT_YUV_GET_ULINFO_IDX 	(UINT16)0x0500
#define ICAT_YUV_UPLOAD_IDX     	(UINT16)0x0501 		/*not used*/
#define ICAT_YUV_GET_DLINFO_IDX 	(UINT16)0x0502

#define ICAT_BULK_OUT_IDX       	(UINT16)0x0600      /*BulkInfo; wait FW ready and get size*/
#define ICAT_BULK_IN_IDX        	(UINT16)0x0601      /*BulkInfo*/
#define ICAT_BULK_OUT_END_IDX   	(UINT16)0x0602      /*bulk end*/
#define ICAT_BULK_IN_END_IDX    	(UINT16)0x0603
#define ICAT_BULK_TRIG_IDX      	(UINT16)0x0604      /*start bulk*/

#define ICAT_JPG_GET_ULINFO_IDX		(UINT16)0x0700      /*jpg width/height/size*/

#define ICAT_DBG_GET_ULINFO_IDX		(UINT16)0x0800      /*debug data*/

#define ICAT_3A_GET_CAPACINFO_IDX	(UINT16)0x3A00
#define ICAT_3A_AWBDAT_DOWLOAD_IDX	(UINT16)0x3A01


typedef enum {
	SNAP_ICATE_IDLE = 0,
	SNAP_ICATE_START,
	SNAP_ICATE_SENSOR_FINISH,
	SNAP_ICATE_RAW_READY,
	SNAP_ICATE_CDSP_READY,
	SNAP_ICATE_CDSP_DOING,
	SNAP_ICATE_CDSP_FINISH,

	SNAP_ICATE_END,

	//xian ++
    ICATE_RES_DOWNLOAD,
    ICATE_RES_UPLOAD,
	//xian --

    ICATE_RAW_DOWNLOAD,
    ICATE_YUV_UPLOAD,

    SNAP_ICATE_JPG_FINISH,
    //SNAP_ICAT_CDSP_JPG_READY = SNAP_ICAT_JPG_FINISH, 
    ICAT_JPG_UPLOAD,

	SNAP_ICAT_PREPRO_FINISH,		/*==0xE, bulk*/
	SNAP_ICAT_CDSP_DBG_READY,
	SNAP_ICAT_CDSP_YUV_READY,

	ICAT_RAW_UPLOAD,				/*bulk*/
	ICAT_YUV_DOWNLOAD,				/*bulk*/
	ICAT_DBG_UPLOAD,				/*bulk*/
	
	SP1K_ICAT_STATE_END    
} iCatState_e;


/* Resource ID */
#define	ICAT_RSC_RAWINFO 	0
#define	ICAT_RSC_GAMMATBL  17
#define	ICAT_RSC_LSCTBL    18
#define	ICAT_RSC_BADPIXEL  19
#define	ICAT_RSC_WDRTBL    37
#define	ICAT_RSC_PVCDSP    42
#define	ICAT_RSC_STILLCDSP 43

#define	ICAT_RSC_CDSPPARA 255

#define ICAT_RSC_MILSCCAL  0x42 

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
extern xdata UINT16 iCatFlowMode;
extern xdata UINT8 iCatNextJob;
	
extern xdata UINT32 iCatRawFileSize; // do nothing if zero

extern xdata UINT16 iCatResFileNum;
extern xdata UINT16 iCatResFileId;
extern xdata UINT32 iCatResFileSize;
	
extern xdata UINT32 iCatBulkSize;
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
typedef void ( *Stub )( UINT16, UINT8 * );
void icatVndCmdStub_Intr( UINT16 idx, UINT16 val, UINT16 bufSz, UINT8	*buf );
void icatVndCmdStubDpc( UINT16 idx, UINT16 val, UINT16 bufSz, UINT8  *buf );
//UINT8 icatDownLoadRes(UINT16 idx);
//UINT8 icatResUpload(void);
UINT8 icatIsDPCCmd(UINT16 idx);

void icatSnapInitial(void);
void icatSnapFinish(void);
UINT8 icatIsConnected(void);
UINT8 icatStateGet(void);
UINT8 icatFlowModeGet(void);

void icatSnapYuvBufTypeGet(UINT8 *type);

UINT8 icatDownloadToDRAM(UINT32 BulkDRAMAddr, UINT32 BulkSize);
UINT8 icatUploadFromDRAM(UINT32 BulkDRAMAddr, UINT32 BulkSize);
	
void  icatSetValue16_Intr(UINT8 *buf, UINT16 val);
UINT16 icatGetValue16_Intr(UINT8 *buf);
void  icatSetValue32_Intr(UINT8 *buf, UINT32 val);
UINT32 icatGetValue32_Intr(UINT8 *buf);
	
//void  icatSetValue16(UINT8 *buf, UINT16 val);
//UINT16 icatGetValue16(UINT8 *buf);
//void  icatSetValue32(UINT8 *buf, UINT32 val);
UINT32 icatGetValue32(UINT8 *buf);


void icatBulkOut( UINT32 addr, UINT32 size );
void icatBulkIn( UINT32 addr, UINT32 size );
void icatSnapJpgUpload(UINT32 addr, UINT32 filesize);
void icatSnapYuvUpload(UINT32 addr, UINT32 filesize);
void icatSnapRawUpload(UINT32 addr, UINT32 filesize);

void icatResDownload(void);
void icatIQTunningProcess(void);


#endif
#endif /*__VNDREQ_H__*/



