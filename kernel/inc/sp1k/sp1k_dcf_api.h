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
 * @file      sp1k_dcf_api.h
 * @brief     Brief	Spca1628 DCF operation related interface
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __SP1K_DCF_API_H__
#define __SP1K_DCF_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sp1k_def.h"
#include "dcf_api.h"
#include "dosusr.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 typedef enum dcfFileType_e{
	 DCF_FILE_TYPE_ALL = 0,
	 DCF_FILE_TYPE_JPG,
	 DCF_FILE_TYPE_AVI,
	 DCF_FILE_TYPE_WAV,
	 DCF_FILE_TYPE_MAX
 }dcfFileType_t;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
* @fn UINT8 sp1kDcfInit( void )
* @brief   To do DCF initialization
* @retval\n 
*	  TRUE	: initial error.
*\n	  FALSE : initial success								
*/
UINT8 sp1kDcfInit( void );

/**
* @fn void sp1kDcfFileTypeEnable(UINT8 type, UINT8 en)
* @brief   Enable or disable file types. All file types are enable by default.
*\n		  Invoked before dos initialize if needed.
* @param[in] type specify the file type
*\n			DCF_FILE_TYPE_ALL(0) : for all types.
*\n			DCF_FILE_TYPE_JPG : for JPG file type.
*\n			DCF_FILE_TYPE_AVI : for AVI file type.
*\n			DCF_FILE_TYPE_WAV : for WAV file type.
* @param[in] en	 enable or disable
*\n			0 : disable the file type specified by the param of type.
*\n			1 : enable the file type specified by the param of type.
* @retval none
* @see dcfFileType_t
*/
void sp1kDcfFileTypeEnable(UINT8 type, UINT8 en);

/**
* @fn void sp1kDcfFileBaseNameUpdate(UINT8 *name)
* @brief   Modify file base name.
* @param[in] name specify the name base name, should be DCF file name free characters.
* @retval none
* @see none
*/
void sp1kDcfFileBaseNameUpdate(UINT8 *name);

 /**
 * @fn UINT16 sp1kDcfFileCountGet(void)
 * @brief get dcf file count
 * @param none
 * @retval dcf file count 
 * @see none
 */
UINT16 sp1kDcfFileCountGet(void);
 
 /**
 * @fn void sp1kDcfProtectAllFile(UINT8 isReadOnly);
 * @brief	protect all dcf files
 * @param[in]  isReadOnly,set the file attribute is ReadOnly
 * @retval none
 * @see none
 */
void sp1kDcfProtectAllFile(UINT8 isReadOnly);
 
 /**
 * @fn void sp1kDcfDeleteAllFile(void);
 * @brief	delete all dcf files
 * @param void 
 * @retval void 
 * @see none
 */ 
void sp1kDcfDeleteAllFile(void);
 
 /**
 * @fn void sp1kDcfDeleteFolder(UINT16 FolderIndex)
 * @brief delete folder
 * @param[in] folderIndex
 * @retval void 
 * @see none
 */ 
void sp1kDcfDeleteFolder(UINT16 FolderIndex);
 
 /**
 * @fn UINT8 sp1kDcfIsOurBaseName(UINT8* name)
 * @brief  judge current file is ourselves file
 * @param[in] name specify the name base name, should be DCF file name free characters
 * @retval TRUE/FALSE 
 * @see none
 */ 
UINT8 sp1kDcfIsOurBaseName(UINT8* name);
 
 /**
 * @fn UINT8 sp1kDcfMediaFolderAssign(UINT8 en, UINT16 folderIdx);
 * @brief      go to assign folder,if specify folder is not exits,create it.
 * @param[in] folderIdx,specify folder
 * @param[in] en£¬enable or disable
 				0:disable,keep file in default folder
 				1:enable,keep file in assign folder
 * @retval SUCCESS\FAIL 
 * @see none
 */ 
UINT8 sp1kDcfMediaFolderAssign(UINT8 en, UINT16 folderIdx);
 /**
 * @fn UINT8 sp1kDcfDir1NameSet(UINT8 *dirName)
 * @brief      set DCF directory name
 * @param[in]  DCF directory name
 				
 * @retval SUCCESS\FAIL 
 * @see none
 */ 
UINT8 sp1kDcfDir1NameSet(TLSMod_e opt,UINT8 *dirName);
//UINT8 sp1kDcfBaseNameSet(UINT8 * fileName);

/**
 * @fn        UINT8 sp1kdcfFileAttrSet(UINT8 const* oldFileName, UINT8 const* newFileName, UINT8 attr, UINT8 option)
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
UINT8 sp1kDcfFileAttrSet(UINT8 const* oldFileName, UINT8 const* newFileName, UINT8 attr, UINT8 option);

#else
#define sp1kDcfInit			dcfFileInitialize
#define sp1kDcfFileTypeEnable	dcfFileTypeEnable
#define sp1kDcfFileBaseNameUpdate	dcfFileBaseNameUpdate

#define sp1kDcfFileCountGet	dcfFileCountGet
#define sp1kDcfProtectAllFile       dcfProtectAllFile
#define sp1kDcfDeleteAllFile       dcfDeleteAllFile
#define sp1kDcfDeleteFolder   dcfDeleteFolder 
#define sp1kDcfIsOurBaseName USR_IsOurBaseName

#define sp1kDcfMediaFolderAssign dcfMediaFolderAssign

#define sp1kdcfDirNameSet	dcfDir1NameSet
#define sp1kdcfQuickInitSet	dcfQuickInitSet
#define sp1kDcfFileAttrSet	dcfFileAttrSet

//#define sp1kDcfBaseNameSet dcfBaseNameSet
#endif
#endif  /* __SP1K_DCF_API_H__ */
