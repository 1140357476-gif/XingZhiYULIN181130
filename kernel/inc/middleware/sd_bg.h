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
 * @author   wangjg
 * @since     2011-09-20
 * @date      2011-09-20
*/

#ifndef __SD_BG_H__
#define __SD_BG_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 #define SD_BG_EN 1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 typedef struct 
{
	UINT32 addr;
	UINT32 sector;
	UINT32 size;
	UINT8 status;//[0]valid [1]r/w [2]1st [3]savesize [4]wait
}BgNode_t;
 typedef struct
{
	UINT32 start;
	UINT32 cnt;
}BgClusters_t;
 typedef struct BgFileObj_s
 {
 	
	UINT8 fno;
	UINT8 fname[13];
	UINT32 fsizeMax;//cluster num
	UINT32 fsize;//cluster num
	UINT16 fsizeErr;

	UINT32 listCnt,listMax;
	BgClusters_t xdata* list;//clusters;	
	BgClusters_t xdata* listNode;
	UINT32 listNodeIdx;

	UINT8 mode;//0:getFreeFromFat 1:getFreeFromList
	UINT32 clusterFirst,clusterLast;
	struct BgFileObj_s xdata* nextObj;
 }BgFileObj_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void bgInit();
void bgStop();
void bgRwTrig();
void bgRwResume();
void bgRwAbort();
UINT32 bgFileOpen(BgFileObj_t *file,UINT32 size,UINT8* fname);
UINT32 bgFileWrite(BgFileObj_t *file,UINT32 addrbyte,UINT32 size, UINT32 minFreeList);
UINT8 bgFileClose(BgFileObj_t *file,UINT8* fname,UINT8 isClose);
UINT8 bgFileJoint(BgFileObj_t *bgfile,UINT8* fname,UINT8 isClose);
UINT8 bgSetSpace(UINT8 xdata* buf,UINT32 bufLen,UINT32 fileSize);//kb
UINT32 bgGetFree(BgFileObj_t *file);//cluster num
UINT32 bgSaveSizeGet();
void bgPrintfNodes(BgFileObj_t *bgfile);
#endif  /* __TEMPLATE_HDR_H__ */
