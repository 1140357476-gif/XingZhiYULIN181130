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
 * @file      dcf_api.h
 * @brief     Brief
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __DCF_API_H__
#define __DCF_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SINGLE_DIRECTORY        0

#define K_DCF_MatchIndexName                    255
#define K_DCF_MatchOurFile                      254

//#define K_DCF_MatchDcfDir1Name                  253
//#define K_DCF_MatchDcfDir1Index                 252
#define K_DCF_ALL_DCF_FILE                        251

//version4.0@ada@0513 for DCF
#define K_DCF_OUR_TYPE                          0
#define K_DCF_JPG_TYPE                          1
#define K_DCF_AVI_TYPE                          2
#define K_DCF_WAV_TYPE                          3
#define K_DCF_WGI_TYPE                          4
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct FDBInfo_s{
	UINT32 FdbAddr;
	UINT8 FileAttrib;
	UINT8 FileType;
	UINT16 StartCluster;
	UINT32 FileSize;
	UINT8 FileName[4];
	UINT16 PreIndex;
	UINT16 NextIndex;
} FDBInfo_t, *PFDBInfo_t;

typedef enum {
	NORMAL_SNAPFOLDER = 0,
	TIMELAPSE_VIDEOFOLDER,
}TLSMod_e;

 typedef enum {
	MEDIA_JPG = 0,
	MEDIA_AVI,
	MEDIA_WAV,
	MEDIA_TLS,
 }MediaType_e;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void dcfBaseNameGet(UINT8 *name, UINT16 fileIndex);
//void dcfJpgNameGet(UINT8* name, UINT16 fileIndex);
void dcfMediaNameGet(UINT8 *name,UINT16 fileIndex,UINT8 type);


//void dcfWaveNameGet(UINT8* name, UINT16 fileIndex);
//void dcfAviNameGet(UINT8* name, UINT16 fileIndex);

void dcfDir0NameGet(UINT8* name);
UINT8 dcfDirInitialize(void);
UINT8 dcfFileInitialize(void);

void dcfFileDramFdbParameterClear(UINT8 fileType,UINT16 fileIndex);
UINT8 dcfFileDramFdbParameterRead(UINT16 fileIdx, FDBInfo_t* fileInfo);
void dcfFileDramFdbParameterWrite(UINT8 fileType,UINT16 fileIndex);
void dcfNewFileDramFdbParameterUpdate(UINT8 fileType, UINT32 fileSize);

void dcfFileReadOnlyAttrSet(UINT16 fileIndex,UINT8 fileType,UINT8 readOnlyOn);

void dcfRtcTimeUpdate(void);

UINT8 dcfFolderNext(UINT8 forward);
UINT8 dcfFileIndexUpdate(void);

void dcfFileTypeEnable(UINT8 type, UINT8 en);
void dcfFileBaseNameUpdate(UINT8 *name);

UINT16 dcfFileIndexGet(void);
UINT16 dcfFileCountGet(void);
UINT16 dcfTotalDirectoryGet(void);
UINT32 dcfAllFolderTotalFileGet(void);
UINT16 dcfDir1IndexGet(void);
void dcfProtectAllFile(UINT8 isReadOnly);
void dcfDeleteAllFile(void);
void dcfDeleteFolder(UINT16 FolderIndex);
UINT8 dcfGetFileName(UINT8 *name,UINT16 index,UINT8 *pAttr);
UINT8 dcfDeleteFile(UINT16 index,UINT8 isUpdateDCF,UINT8 filterType);

UINT8 dcfMediaFolderAssign(UINT8 en, UINT16 folderIdx);
UINT8 dcfDir1NameSet(TLSMod_e opt,UINT8 *dirName);
UINT8 dcfTLSAssignFolder();
void dcfDir1NameGet(UINT8* name, UINT16 dirIndex,UINT8 opt);
//UINT8 dcfBaseNameSet(UINT8 * fileName);
void dcfAviNameSet(UINT8* name);
UINT8 dcfFileAttrSet(UINT8 const* oldFileName,UINT8 const* newFileName,UINT8 attr,UINT8 option);
void dcfMmsFileNameGet(UINT8 *fileName,UINT16 fileIndx);
UINT8 dcfEntryMmsDir();
void dcfExitMmsDir();
UINT8 USR_IsOurFileExt(UINT8 *name,MediaType_e opt);
#endif  /* __DCF_API_H__ */
