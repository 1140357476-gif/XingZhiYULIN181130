/**************************************************************************
 *
 *      Copyright (c) 2005-2008 by Sunplus mMedia Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus mMedia
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Technology
 *  Co., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Technology Co., Ltd.
 *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Technology Co., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *
 **************************************************************************/

/**
 * @file		dbg_file_list.h
 * @brief		App for UI setting parameter definition
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
#ifndef __DBG_FILE_LIST_H__
#define __DBG_FILE_LIST_H__
 //=============================================================================
//Header file
//=============================================================================


//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------

typedef enum __FILE_LIST_E{
	__FILE_ID_LIB_3A__ = 0x0100,
	__FILE_ID_LIB_AE_FLOW__ = 0x0101,
	__FILE_ID_LIB_AE_API__ = 0x0102,
	__FILE_ID_LIB_FD_FLOW__ = 0x0103,
	__FILE_ID_LIB_FD_BLINK__ = 0x0104,
	__FILE_ID_LIB_FD_SMILE__ = 0x0105,
	__FILE_ID_LIB_FORNT_LOGIC__ = 0x0106,
	__FILE_ID_LIB_FORNT_VD__ = 0x0107,
	__FILE_ID_LIB_AF_FLOW__ = 0x0108,
	__FILE_ID_LIB_AF_API__ = 0x0109,
	
	__FILE_ID_LIB_AUDIO__ 	= 0x0200,
	
	
	
	__FILE_ID_LIB_CDSP__ = 0x0300,
	__FILE_ID_CDSP_TASK__	 = 0x0301,
	__FILE_ID_CDSP_FILTER__	 = 0x0302,
	__FILE_ID_LIB_CDSP_MISC__ = 0x0303,

	__FILE_ID_CALIBRATION__=0x0310,
	
	__FILE_ID_LIB_CMD__ = 0x0400,
	
	
	
	__FILE_ID_LIB_DISP__ = 0x0500,
	
	
	
	__FILE_ID_LIB_DOS__ = 0x0600,
	__FILE_ID_DOSFILE__ = 0x0601,
	
	
	__FILE_ID_LIB_DOS32__ = 0x0700,
	__FILE_ID_DIR_LOOKUP__ = 0x0701,
	__FILE_ID_DOS_DEL__ = 0x0702,
	__FILE_ID_DOS_FILERW__ = 0x0703,
	__FILE_ID_DOS_FORMAT__ = 0x0704,
	__FILE_ID_DOS_MOUNT__ = 0x0705,
	__FILE_ID_DOS_OPEN__ = 0x0706,
	__FILE_ID_DOS_RW__ = 0x0707,
	__FILE_ID_DOS_SUPPORT__ = 0x0708,
	__FILE_ID_FAT_RW__ = 0x0709,
	__FILE_ID_VFS_API__	= 0x070a,

	__FILE_ID_DOS_USR__ = 0x070b,

	__FILE_ID_LIB_DPOF__ = 0x0800,
	
	
	
	__FILE_ID_LIB_EXIF__ = 0x0900,
	
	
	
	__FILE_ID_LIB_GLOBAL__ = 0x0a00,
	__FILE_ID_INITIO__ = 0x0a01,
	__FILE_ID_GLOBAL_INIT__ = 0x0a02,
	
	
	
	__FILE_ID_LIB_HAL__ = 0x0b00,
	__FILE_ID_HAL_CDSP__ = 0x0b01,
	__FILE_ID_HAL_NAND__ = 0x0b02,
	__FILE_ID_HAL_SD__ = 0x0b03,
	__FILE_ID_HAL_FLASH__ = 0x0b04,
	__FILE_ID_HAL_GPRM__ = 0x0b05,
	__FILE_ID_HAL_CDSP_CORE__ = 0x0b06,
	__FILE_ID_HAL_CDSP_IQ__ = 0x0b07,
	__FILE_ID_HAL_SD_INTR__ = 0x0b08,
	__FILE_ID_HAL_CPU__ = 0x0b09,
	__FILE_ID_HAL_DMAC__ = 0x0b0a,
	__FILE_ID_HAL_MOE__ = 0x0b0b,
	
	__FILE_ID_LIB_INTR__ = 0x0c00,
	
	
	
	__FILE_ID_LIB_MISC__ = 0x0d00,
	__FILE_ID_BP_MAIN__ = 0x0d01,
	__FILE_ID_BP_FIND__ = 0x0d02,
	
	
	__FILE_ID_LIB_OS__ = 0x0e00,
	__FILE_ID_BOOTER__ = 0x0e01,
	
	
	
	__FILE_ID_LIB_OSD__ = 0x0f00,
	__FILE_ID_OSD_MEM__ = 0x0f01,	
	__FILE_ID_OSD_LINE__ = 0x0f02,
	__FILE_ID_OSD_DRAW__ = 0x0f03,
	__FILE_ID_OSD_BUFF__ = 0x0f04,
	__FILE_ID_OSD_INIT__ = 0x0f05,
	
	
	__FILE_ID_LIB_PB__ = 0x1000,
		
	
	
	__FILE_ID_LIB_PV__ = 0x1100,
	__FILE_ID_PV_TASK__ = 0x1101,
	__FILE_ID_PV_PHOTO_FRAME__ = 0x1102,
	__FILE_ID_PV_ZOOM__ = 0x1103,
	 __FILE_ID_PB_PHOTOFRAME__=0X1104,

	
	__FILE_ID_LIB_SNAP__ = 0x1200,
	
	__FILE_ID_SNAP_TASK__	 = 0x1201,
	__FILE_ID_SNAP_DZOOM__ = 0x1202,
	__FILE_ID_SNAP_QTABLE__ = 0x1203,
	__FILE_ID_SNAP_ICATSTUB__ = 0x1204,
	__FILE_ID_SNAP_CORE__ = 0x1205,
	
	__FILE_ID_LIB_STORAGE__  = 0x1300,
	__FILE_ID_SMC_533__ = 0x1301,
	__FILE_ID_SMC_2533__ = 0x1302,
	__FILE_ID_SD_RW__ = 0x1303,
	__FILE_ID_DOSRAM__ = 0x1304,
	__FILE_ID_SMC__ = 0x1305,
	__FILE_ID_SD_533_BGWR__ = 0x1306,	
	__FILE_ID_SD_HW__ = 0x1307,
	__FILE_ID_NAND_AUTOFLOW__ = 0x1308,
	__FILE_ID_SD_BG__=0x1309,

	__FILE_ID_LIB_SYS__ = 0x1400,
	__FILE_ID_MAIN__ = 0x1401,
	__FILE_ID_SYS_SVC_DEF__ = 0x1402,
	__FILE_ID_SYS_MEM_INIT__ = 0x1403,
	
		
	__FILE_ID_LIB_USB__ = 0x1500,
	
	
	
	__FILE_ID_LIB_UTIL__ = 0x1600,
	__FILE_ID_DATESTAMP__ = 0x1601,
	__FILE_ID_FW_UPDATE__ = 0x1602,
	__FILE_ID_UTIL_CODE_SENTRY__ = 0x1603,
	
	__FILE_ID_LIB_VIDEO__ = 0x1700,
	__FILE_ID_VIDEO_CLIP__ = 0x1701,
	__FILE_ID_VIDEO_PB__ = 0x1702,
	__FILE_ID_VIDEO_DTSTMP__ = 0x1703,
	__FILE_ID_VIDEO_BRC__ = 0x1704,
	__FILE_ID_VIDEO_PBINIT__ = 0x1705,
	__FILE_ID_VIDEO_RECINIT__ = 0x1706,
	__FILE_ID_VIDEO_RECABORT__ = 0x1707,
	__FILE_ID_VIDEO_FUNC__ = 0x1708,
	__FILE_ID_VIDEO_MEM__ = 0x1709,
	__FILE_ID_TIMELAPSE__	= 0x1710,
	__FILE_ID_VIDEO_ZOOM__ = 0x1711,
	__FILE_ID_VIDEO_CAPTURE__ =0x1712,
	__FILE_ID_VIDEOPB_DUMMY__= 0x17ff,
	
	__FILE_ID_LIB_HOST__ = 0x2000,
	__FILE_ID_APP_INIT__ = 0x2001,
	__FILE_ID_APP_STS_STILLVIEW__ = 0x2002,
	__FILE_ID_APP_DEV_STORAGE__ = 0x2003,
	__FILE_ID_APP_STS_VIDEOVIEW__ = 0x2004,
	__FILE_ID_APP_DEV_DISP__ = 0x2005,
	__FILE_ID_APP_TP_SCAN__ = 0x2006,
	
	__FILE_ID_SENSOR_DRIVER__ = 0x2100,

	__FILE_ID_MOTION_DET__ = 0x2200,

	__FILE_ID_PANORAMA_API__ = 0x2300,

	__FILE_ID_LIB_MAX__ = 0xff00,
} __FILE_LIST_T;
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Function declare
//-----------------------------------------------------------------------------

#endif /*__DBG_FILE_LIST_H__*/

