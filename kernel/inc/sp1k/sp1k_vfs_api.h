/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus mMedia *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno-*
 *  logy Co., Ltd. This software may only be used in accordance with the  *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.         *
 *                                                                        *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus mMedia Technology Co., Ltd.                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __SP1K_VFS_API_H__
#define __SP1K_VFS_API_H__

/** \file sp1k_vfs_api.h
 * @brief		Spca1628 file system related interface
 */

#include "sp1k_def.h"

#include "doslink.h"
#include "Cardui.h"
#include "Cardvar.h"
#include "vfs_api.h"
#include "dcf_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
 * @fn UINT8 sp1kVfsFmt( UINT8 eraseCard )
 * @brief	To format the current storage drive
 * @param[in] eraseCard	    specify to do physical erase or not, especially nand drive
 *\n                              0: Not to do physical format.
 *\n                              1: Do physical format                               
 * @retval\n 
 *      TRUE  : format error.
 *\n    FALSE : format success                               
*/
UINT8 sp1kVfsFmt(UINT8 eraseCard);

/**
 * @fn UINT8 sp1kVfsInit( UINT8 doDOSInitial )
 * @brief	To do file system initialization
 * @param[in] doDOSInitial	To refresh file system buffer
 *\n      0: Not to do dos buffer refresh.
 *\n      1: Do dos buffer refresh, it's better to set doDOSInitial = 1.
 *\n         Because this will read all DOS related info ( such as FAT, directory ..)
 *\n         from storage to buffer.                      
 * @retval\n 
 *      TRUE  : initial error.
 *\n    FALSE : initial success                               
*/
UINT8 sp1kVfsInit( UINT8 doDOSInitial );

/**
 * @fn        UINT16 sp1kVfsFileOpen(UINT8 *name, UINT8 rwMode, UINT16 numCluster)
 * @brief     Open a file.
 * @param     [in] name			The name of the file to read.
 * @param     [in] rwMode 		File access mode, it can be K_DOS_NewFile, K_DOS_ReadFile and K_DOS_NewFile_LinearFAT.
 * @param     [in] numCluster 	Not used.
 * @retval    File handle. The value is bigger than 1 if open file success.
 * @see       NULL
*/
UINT16 sp1kVfsFileOpen(UINT8 *name, UINT8 rwMode, UINT16 numCluster);

/**
 * @fn        UINT8 sp1kVfsFileClose(UINT16 fHandle, UINT8* name, UINT8 rwMode, UINT32 fileSize)
 * @brief     Close a file.
 * @param     [in] fHandle	The handle of the file to be closed.
 * @param     [in] name		Not used
 * @param     [in] rwMode	Not used
 * @param     [in] fileSize	Not used
 * @retval    return = TRUE
 * @see       NULL
*/
UINT8 sp1kVfsFileClose(UINT16 fHandle, UINT8* name, UINT8 rwMode, UINT32 fileSize);

/**
 * @fn        UINT8 sp1kVfsFileRead(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos)
 * @brief     Read block of data from a file.
 * @param     [in] fHandle	The handle of the file to read.
 * @param     [in] dstPath	Not used.
 * @param     [in] readSize	Read size in bytes.
 * @param     [in] ramAddr	Dram address in word units.
 * @param     [in] fpos		Start offset to read.
 * @retval    return = TRUE / FALSE
 * @see       NULL
*/
UINT8 sp1kVfsFileRead(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos);

/**
 * @fn        UINT8 sp1kVfsFileWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage)
 * @brief     Write block of data to a file.
 * @param     [in] fHandle			The handle of the file to write.
 * @param     [in] srcPath			Not used.
 * @param     [in] writeSize			Size in bytes.
 * @param     [in] ramAddr			Dram address in word units.
 * @param     [in] findHeaderPage	Not used.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
*/
UINT8 sp1kVfsFileWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage);

/**
 * @fn        UINT8 sp1kVfsFileDelete(UINT8 *name)
 * @brief     Delete a file
 * @param     [in] name		The name of the file to be deleted.
 * @retval    return = TRUE / FALSE
 * @see       NULL
*/
UINT8 sp1kVfsFileDelete( UINT8 *name );

/**
 * @fn        UINT8 sp1kVfsFileAttrSet(UINT8 const* oldFileName, UINT8 const* newFileName, UINT8 attr, UINT8 option)
 * @brief     Set the attribute of a file.
 * @param     [in] oldFileName	Specify the old filename.
 * @param     [in] newFileName	Specify the new filename if option is ATTR_MODIFY_NAME.
 * @param     [in] attr			Specify the new attribute value. It can be:\n
 *\n				ATTR_R :	read only
 *\n				ATTR_H : hidden
 *\n				ATTR_S : system		
 *\n				ATTR_L : label
 *\n				ATTR_D : dir
 *\n				ATTR_A : archieve
 *\n				ATTR_LFN : ATTR_R+ATTR_H+ATTR_S+ATTR_L
 * @param     [in] option		Operation option. It can be ATTR_MODIFY_NAME, ATTR_MODIFY_ATTR and ATTR_MODIFY_SIZE.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
*/
UINT8 sp1kVfsFileAttrSet(UINT8 const* oldFileName, UINT8 const* newFileName, UINT8 attr, UINT8 option);

#else


#define sp1kVfsFmt			diskFormat
#define sp1kVfsInit			diskInitialize
#define sp1kVfsFileOpen		vfsFileOpen
#define sp1kVfsFileClose	vfsFileClose
#define sp1kVfsFileRead		vfsFileRead
#define sp1kVfsFileWrite	vfsFileWrite
#define sp1kVfsFileDelete	vfsFileDelete
#define sp1kVfsFileAttrSet	vfsFileAttrSet

#endif
#endif 

