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
#ifndef	_SD_DEF_

#define	_SD_DEF_

#define SD_CMD_DEBUG    		0
#define	GET_RSP_FOR_CMD17_CMD18	0	//some card need turn on this opt,
									//Toshiba 16MB, OK! why?
									//PDC16MB EMMC, still NG, dos issue
//#define	SD_48MHZ_OK				1
//#define	SD_RW_PLUS				1
#define	SD_FAST_W				1

#define	CID_OUTPUT_ENABLE		1
#define	CSD_OUTPUT_ENABLE		1
#define	OCR_OUTPUT_ENABLE		1
#define	SCR_OUTPUT_ENABLE		0
#define	SPEED_OUTPUT_ENABLE		1
#define	SD_DAT_PULL_HI			1
#define	SD_CMD_PULL_HI			1

//#define SD_AUTO_CMD 			1

//#define	SD_SECTOR_W_ALWAYS		1
//#define	SD_SECTOR_R_ALWAYS		1
#define	SD_MULTI_RW_MAX	64L //(128L)	//sector, K=sector/2, max=64KB
//#define	SD_SECTOR_R				1
//#define	SD_SECTOR_W				1
//#define	SD_BLOCK_FAST_W			1
//#define	SD_BLOCK_FAST_R			1
#define	SD_BLOCK_AUTO_DUMMY		1
#define	SD_BLOCK_R_DEBUG		0
#define	SD_BLOCK_W_DEBUG		0

#if 0//Enable this to enable Background Write
#define SD_R_INT_EN		1
#define SD_W_INT_EN		1
#define SD_R_BG_EN              0
#define SD_W_BG_EN              1
#else
#define SD_R_INT_EN		0
#define SD_W_INT_EN		0
#define SD_R_BG_EN              0
#define SD_W_BG_EN              0
#endif
#define SD_IDLE_INT_EN  0
#define SD_CRC_INT_EN   0

#define SD_MODE      0
#define MMC_MODE     1

#define SD_1BIT_MODE 0
#define SD_4BIT_MODE 1

#define SD_24M  0
#define SD_12M  1
#define SD_6M   2
#define SD_375K 3
#define SD_48M	4
#define SD_16M  5

#define RSP_TYPE_R1  0
#define RSP_TYPE_R2  1
#define RSP_TYPE_R3  2
#define RSP_TYPE_R6  3


#define SD_CID_CMP_NUM  8    //add

enum {
	SD_BG_FREE,
	SD_BG_READ,
	SD_BG_WRITE,
	SD_BG_DMA_DONE
};

enum {
	SD_SPEC_VER_10=0,
	SD_SPEC_VER_11=1,
	SD_SPEC_VER_20=2
};
#endif
