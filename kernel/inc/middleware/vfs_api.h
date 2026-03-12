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
 * @file      vfs_api.h
 * @brief     Brief
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __VFS_API_H__
#define __VFS_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
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
UINT16 vfsFileOpen(UINT8 *name, UINT8 rwMode, UINT16 numCluster);
UINT8 vfsFileClose(UINT16 fHandle, UINT8* name, UINT8 rwMode, UINT32 fileSize);
UINT8 vfsFileRead(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos);
UINT8 vfsFileWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage);
UINT8 vfsFileDelete(UINT8* name);
UINT8 vfsFileProtectAll(UINT8 isReadOnly);
UINT8 vfsFileDeleteAll(void);//(UINT8 matchCond, UINT32 refCon);
//UINT8 vfsFileAttrSet(UINT8 const* oldFileName,UINT8 const* newFileName,UINT8 attr,UINT8 option);
UINT8 vfsFileJoint(UINT16 head, UINT16 tail);
UINT8 vfsFileSeek(UINT16 fno, UINT32 offset, UINT8 mode);
UINT8 vfsDirRemove(UINT8 *pathName);
UINT32 vfsFileSizeGet(UINT16 fno);

UINT32 vfsClusterSizeGet(void);

UINT8 vfsFileMove(UINT8 *srcName, UINT8 *dstName);
UINT8 vfsFileAttrSet(UINT8 fno,UINT8 option,void* pData);

#endif  /* __VFS_API_H__ */
