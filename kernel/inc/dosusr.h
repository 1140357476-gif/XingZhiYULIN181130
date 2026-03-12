/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __DOSUSR_H__
#define __DOSUSR_H__

//#include "option.h"
#include "dcf_api.h"

#if 0 //move to dcf_api.h
/*add by phil@08.01.16 s*/
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
/*add by phil@08.01.16 e*/
#endif

//xian ++ 20080717
typedef struct{
    UINT32 startCluster;
    UINT16 ourDirMark;
    UINT8 name[8];
    UINT16 prevCnt; //100~999, should be at the end
    UINT16 nextCnt; //100~999, should be at the end
}FDBMediaFolderInfo_t;
//xian --

//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------
//void dcfBaseNameGet(UINT8 *name, UINT16 fileIndex);
//void dcfJpgNameGet(UINT8* name, UINT16 count);

//#if ( AUDIO_OPTION == 1 )
//patch3.2@ada@0401 For Multi-Frame Play Back
//void dcfWaveNameGet(UINT8* name, UINT16 count);
//#endif

//void dcfAviNameGet(UINT8* name, UINT16 count);


//void dcfDir0NameGet(UINT8* name);
//void dcfDir1NameGet(UINT8* name, UINT16 count);

//patch4.4@ada@DCF begin
UINT8 USR_CheckDcfFileName(UINT8 *fdb);
//patch4.4@ada@DCF end
void dcfQuickInitSet(UINT8 enable);
UINT8 dcfQuickInitStsGet(void);

UINT8 USR_CheckType(UINT8* name);//eric@20080502
UINT8 USR_IsOurBaseName(UINT8* name);

//version4.0@ada@0513 for Multi-Directory
#if (SINGLE_DIRECTORY == 0)
//patch4.3@richie@md0531
//UINT8 USR_ChangeCurrentDir(UINT16 Index);
//UINT8 USR_IsOurDirName(UINT8* name);
#endif

//UINT8 USR_IsJpgExt(UINT8* name);
UINT8 USR_IsSameStr(UINT8*str1, UINT8*str2, UINT16 len);
//UINT8 USR_IsAviExt(UINT8* name);
//UINT8 USR_IsWGIExt(UINT8 * name);
//UINT8 USR_IsWaveExt(UINT8* name);
UINT8 USR_IsOurFile(UINT8* name);

//ada@0322
UINT8 USR_FileExist(UINT8 *name);

//UINT16 USR_GetFileCount(void);
//UINT8 dcfDirInitialize(void);
//UINT32 USR_DiskFreeSizeGet(void);

//patch4.4@ada@MP3 begin

//patch4.4@ada@MP3 end

//version4.0@ada@0513 for Multi-Directory
#if (SINGLE_DIRECTORY == 0)
//UINT8 USR_CreateNewDir1(void);
#endif

//UINT8 dcfFileInitialize(void);
//UINT8 USR_DeleteLastFile(void);

//ada@0321
//version4.0@ada@0513 for Multi-Directory
#if (SINGLE_DIRECTORY)
//UINT16 USR_FindDcfDir1Index(UINT8 *name);
#else
//UINT16 USR_FindDcfDir1Index(void);
#endif

//version4.0@ada@0513 for Multi-Directory
#if (SINGLE_DIRECTORY)
//UINT16 USR_FindDcfDir1Name(UINT8* name);
#endif


UINT8 USR_MatchFdb(UINT8 matchCond, UINT8 *fdb, UINT32 refCon);
UINT8 USR_MatchIndexName(UINT8 *fdb, UINT16 fileIndex);
//UINT8 USR_CheckExtType(UINT8 *name);

//patch3.2@ada@0401 For Multi-Frame Play Back
//UINT8 dcfFileDramFdbParameterRead(UINT16 FileIdx, FDBInfo_t* FileInfo);

//version4.0@ada@0513 for DCF
//void dcfFileDramFdbParameterWrite(UINT8 FileType,UINT16 FileIndex);

//version4.0@ada@0513 for Read-Only
//void dcfFileReadOnlyAttrSet(UINT16 FileIndex,UINT8 FileType,UINT8 ReadOnlyOn);
//UINT8 USR_GetFdb(UINT8 *fdb,UINT32 fdbLogAddr);

//version4.0@ada@0513 for DCF
//void dcfFileDramFdbParameterClear(UINT32 DramAddr);
//void dcfFileDramFdbParameterClear(UINT8 FileType,UINT16 FileIndex);

void USR_ResetDramFdbParameterArea(void);

//patch3.2@ada@0401 For Date & Time Update
//void dcfRtcTimeUpdate(void);
UINT8 USR_CheckExtType(UINT8 *name);

//UINT8 USR_Delete1File(UINT16 fileIndex);
//UINT16 USR_SearchOurFdbToDram(void);					// @WYEO, 102401
//UINT8 USR_UploadFdb(UINT16 fileCount);
//UINT8 USR_Upload1File(UINT16 fileIndex);

//patch4.5@richie@partial file upload
//UINT8 USR_UploadPartialFile(UINT32 offset, UINT32 length, UINT16 fileIndex);

//UINT8 USR_Play1File(UINT16 fileIndex,UINT8 wantAudio);			// @WYEO, 102401
//UINT8 USR_PlayJpg(UINT16 fileIndex, UINT32 ramAddr);
//UINT8 USR_PlayAvi(UINT16 fileIndex, UINT32 vidRamAddr, UINT32 vidRamSize, UINT32 audRamAddr, UINT32 audRamSize);
//UINT8 USR_UploadCheck(UINT8 state3);      //cch@10/30
//UINT8 USR_UploadJpg(UINT16 fileIndex);	// 500 use
//UINT8 USR_UploadAvi(UINT16 fileIndex);	// 500 use

#if 0//def MULTI_FOLDER_AS_ONE  //xian ++ 20080806
UINT8 USR_InitMediaFolderInfo();  //xian ++
UINT8 USR_LastFolder(UINT8 isSkipEmpty);
UINT8 USR_NextFolder(UINT8 forward, UINT8 isSkipEmpty, UINT8 isRepeat);
UINT8 USR_UpdateNewFolderInfo(UINT8 *dirName);
UINT8 USR_UpdateCurFolderInfo(UINT8 mode );
UINT32 USR_GetMediaFileTotal();
UINT32 USR_GetCurFolderFirstFileIdx();
#else
UINT8 USR_UpdateMediaFolderInfo();  //xian ++
//UINT8 dcfFolderNext(UINT8 forward);
//void dcfFileIndexUpdate(void);
#endif

//void dcfFileTypeEnable(UINT8 type, UINT8 en);
//void dcfFileBaseNameUpdate(UINT8 *name);
UINT32 USR_MakeDPSFileHandle(UINT16 fileOrd);
UINT8 USR_ParseDPSFileHandle(UINT32 handle, UINT8* outPathFilename);

void dcfNewFileDramFdbParameterUpdate(UINT8 fileType, UINT32 fileSize);
UINT8 USR_GetMediaFolderInfo(UINT16 dirCnt, FDBMediaFolderInfo_t *info);

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#if 0 //move to dcf_api.h
#define K_DCF_MatchIndexName                    255
#define K_DCF_MatchOurFile                      254

//version4.0@ada@0513 for Multi-Directory
/*
//patch3.2@ada@0401 For DPOF
#define K_DOS_MatchDpofDir1Name                 253
#define K_DOS_MatchDpofDir1Index                252
*/
#define K_DCF_MatchDcfDir1Name                  253
#define K_DCF_MatchDcfDir1Index                 252
#define K_DCF_ALL_DCF_FILE                        251



//version4.0@ada@0513 for DCF
#define K_DCF_OUR_TYPE                          0
#define K_DCF_JPG_TYPE                          1
#define K_DCF_AVI_TYPE                          2
#define K_DCF_WAV_TYPE                          3


//#define K_ReadOnlyOff                           0
//#define K_ReadOnlyOn                            1

//#define K_UPDATE_MODE_ALL       0
//#define K_UPDATE_MODE_ADD_ONE   1
//#define K_UPDATE_MODE_DEL_ONE   2
#endif
//------------------------------------------------------------------------------
//External variable
//------------------------------------------------------------------------------
extern	xdata   UINT16    G_USR_Dir0Cluster;
extern	xdata   UINT16    G_USR_Dir1Cluster;
extern	xdata   UINT16    G_USR_Dir1Count;

//version4.0@ada@0513 for Multi-Directory
#if (SINGLE_DIRECTORY == 0)
//patch4.4@ada@DirectoryIndexOrder begin
//extern    xdata   UINT16  G_USR_Dir1CountIndex;
extern    xdata   UINT16  G_USR_MaxDir1Count;
//extern    xdata   UINT16  G_USR_MaxOurDir1CountIndex;
extern    xdata   UINT16  G_USR_TotalDirectory;
extern    xdata   UINT8   G_USR_OurDirectory;
extern    xdata   UINT16  G_USR_MinDir1Count;
extern    xdata   UINT16  G_USR_MaxOurDir1Count;
extern    xdata   UINT16  G_USR_PrevDir1Count;
//patch4.4@ada@DirectoryIndexOrder end
#endif

//version4.0@ada@0513 for Play Back
extern    xdata   UINT16  G_USR_SearchPreFileIndex;
extern    xdata   UINT16  G_USR_SearchNowFileIndex;
extern    xdata   UINT16  G_USR_SearchNextFileIndex;
extern    xdata   UINT16  G_USR_FirstFileIndex;
extern    xdata   UINT16  G_USR_LastFileIndex;

extern	xdata   UINT16    G_USR_FileIndex;
extern	xdata   UINT8    G_USR_FileType;
extern	xdata   UINT16    G_USR_FileCount;
extern	xdata   UINT16    G_USR_PlayIndex;

//extern    xdata   UINT32   G_USR_NewFileSizeInFdb;

#if (0)	/*Add by Phil@07.10.02*/
//patch4.4@ada@DPOF skip AVI & WAV begin
extern    xdata   UINT16  G_USR_JpgFileCount;
extern    xdata   UINT16  G_USR_FirstJpgFileIndex;
extern    xdata   UINT16  G_USR_LastJpgFileIndex;
//patch4.4@ada@DPOF skip AVI & WAV end
#endif /*APP_DPOF_INIT*/

#endif /*__DOSUSR_H__*/
