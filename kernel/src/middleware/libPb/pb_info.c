/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 
/**
 * @file		pb_info.c
 * @brief		get play back file information
 * @author	Tq.Zhang
 * @since	2010-05-05
 * @date		2010-05-05
*/
/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
//#include "dosusr.h"
#include "initio.h"
#include "pb_general.h"
#include "pb_debug.h"
#include "sp1k_pb_api.h"
#include "dcf_api.h"
#include "FileIndex.h"
#include "Vfs_api.h"    //rhy++ 20110308
#include "doslink.h"
#include "dos32.h"
#include "hal_global.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
 #define FileClusterSize(filesize,clusterSize) ((filesize) = ((filesize) + ((clusterSize) - 1)) &~ ((clusterSize) - 1))
 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
 
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern PBFile_t xdata MediaInfo;

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//pbFileInfoGet
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbFileInfoGet(UINT16 FileIndex, PBFile_t* FileInfo)
 * @brief		get play back file information
 * @param	  [in] FileIndex
 * @param	  [in] FileInfo
 * @retval	return = SUCCESS / FAIL
 * @see		NULL
 * @author	  Tq.Zhang
 * @since	2010-5-5
 * @todo		N/A
 * @bug		N/A
*/
#if STILL_LIB_PB_DMY
UINT8 pbFileInfoGet(UINT16 FileIndex, PBFile_t* FileInfo)USING_0
{
        FileIndex=FileIndex;
        FileInfo=FileInfo;
        return SUCCESS;
}
#else
UINT8 pbFileInfoGet(UINT16 FileIndex, PBFile_t* FileInfo)USING_0
{
	FDBInfo_t fdbInfo;
	UINT8 i;
	UINT8 ret = SUCCESS;
	UINT16 fHandle;
	//pDateStc_t time;
	//printf("file idx is %u\n", FileIndex);
	ret = dcfFileDramFdbParameterRead(FileIndex, &fdbInfo);

	if (ret != SUCCESS) {
		//DBG_pbFuncTrack();
		return ret;
	}

	if (FileInfo == NULL) {
		return ret;
	}
	
	FileInfo->FileIdx = FileIndex;
	FileInfo->FileType = fdbInfo.FileType;
	FileInfo->FileAttrib = fdbInfo.FileAttrib;
	FileInfo->FileStartCluster = fdbInfo.StartCluster;

	FileInfo->FileName[0] = fdbInfo.FileName[0];
	FileInfo->FileName[1] = fdbInfo.FileName[1];
	FileInfo->FileName[2] = fdbInfo.FileName[2];
	FileInfo->FileName[3] = fdbInfo.FileName[3];

	for (i = 7; i > 3; i--) {	 
		FileInfo->FileName[i] = (UINT8) (FileIndex % 10) + '0';;
		FileIndex /= 10;
	}
	
	if (fdbInfo.FileType == SP1K_PB_FILE_TYPE_JPG) {
		FileInfo->FileName[8]  = 'J';
		FileInfo->FileName[9]  = 'P';
		FileInfo->FileName[10] = 'G';
	} else if (fdbInfo.FileType == SP1K_PB_FILE_TYPE_AVI) {
		FileInfo->FileName[8]  = 'A';
		FileInfo->FileName[9]  = 'V';
		FileInfo->FileName[10] = 'I';		
	} else if (fdbInfo.FileType == SP1K_PB_FILE_TYPE_WAV) {
		FileInfo->FileName[8]  = 'W';
		FileInfo->FileName[9]  = 'A';
		FileInfo->FileName[10] = 'V';		
	} else if(fdbInfo.FileType == SP1K_PB_FILE_TYPE_WGI){
		FileInfo->FileName[8]  = 'W';
		FileInfo->FileName[9]  = 'G';
		FileInfo->FileName[10] = 'I';	
	}
	else {
		//printf("Unknown file type!\n");
		DBG_pbFuncTrack();
		ret = FAIL;
	}
	FileInfo->FileName[11] = '\0';

	FileInfo->MediaWidth = MediaInfo.MediaWidth;
	FileInfo->MediaHeight = MediaInfo.MediaHeight;

	fHandle =  vfsFileOpen(FileInfo->FileName, K_DOS_ReadFile, FileInfo->FileStartCluster);
	if (!fHandle) {
		return FAIL;
	}
	DOS_FileInfoGet(fHandle,FILE_TIME,FileInfo->MediaTime);
	vfsFileClose(fHandle,  FileInfo->FileName, K_DOS_ReadFile, 0);

	//time = (pDateStc_t)FileInfo->MediaTime;
	//printf("time1=%bu,%bu:%bu:%bu:%bu:%bu:%bu\n",time->Year,time->Month,time->Day,time->Hour,time->Minute,time->Second);
	
	//printf("FileIdx is %u \n", FileInfo->FileIdx);
	//printf("FileName is %s \n", FileInfo->FileName);
	//printf("FileType is %bu \n", FileInfo->FileType);
	return ret;
}
#endif


UINT8 pbFileInfoGetExt(UINT16 FileIndex, PBFile_t* FileInfo,UINT8 opt)USING_0
{
	FDBInfo_t fdbInfo;
	UINT8 i;
	UINT8 ret = SUCCESS;

	opt = opt;
	
	ret = dcfFileDramFdbParameterRead(FileIndex, &fdbInfo);
	if (ret != SUCCESS) {
		//DBG_pbFuncTrack();
		return ret;
	}

	if (FileInfo == NULL) {
		return ret;
	}
	
	FileInfo->FileIdx = FileIndex;
	FileInfo->FileType = fdbInfo.FileType;
	FileInfo->FileAttrib = fdbInfo.FileAttrib;
	FileInfo->FileStartCluster = fdbInfo.StartCluster;

	FileInfo->FileName[0] = fdbInfo.FileName[0];
	FileInfo->FileName[1] = fdbInfo.FileName[1];
	FileInfo->FileName[2] = fdbInfo.FileName[2];
	FileInfo->FileName[3] = fdbInfo.FileName[3];

	for (i = 7; i > 3; i--) {	 
		FileInfo->FileName[i] = (UINT8) (FileIndex % 10) + '0';;
		FileIndex /= 10;
	}
	
	if (fdbInfo.FileType == SP1K_PB_FILE_TYPE_JPG) {
		FileInfo->FileName[8]  = 'J';
		FileInfo->FileName[9]  = 'P';
		FileInfo->FileName[10] = 'G';
	} else if (fdbInfo.FileType == SP1K_PB_FILE_TYPE_AVI) {
		FileInfo->FileName[8]  = 'A';
		FileInfo->FileName[9]  = 'V';
		FileInfo->FileName[10] = 'I';		
	} else if (fdbInfo.FileType == SP1K_PB_FILE_TYPE_WAV) {
		FileInfo->FileName[8]  = 'W';
		FileInfo->FileName[9]  = 'A';
		FileInfo->FileName[10] = 'V';		
	} else if(fdbInfo.FileType == SP1K_PB_FILE_TYPE_WGI){
		FileInfo->FileName[8]  = 'W';
		FileInfo->FileName[9]  = 'G';
		FileInfo->FileName[10] = 'I';	
	}
	else {
		DBG_pbFuncTrack();
		ret = FAIL;
	}
	FileInfo->FileName[11] = '\0';
	
	FileInfo->MediaWidth = MediaInfo.MediaWidth;
	FileInfo->MediaHeight = MediaInfo.MediaHeight;

	return ret;
}
//-----------------------------------------------------------------------------
//pbFileCntGet
//-----------------------------------------------------------------------------
/**
 * @fn		UINT16 pbFileCntGet(void)
 * @brief		get global file count
 * @param	  NONE
 * @retval	return = SUCCESS / FAIL
 * @see		NULL
 * @author	  Tq.Zhang
 * @since	2010-5-5
 * @todo		N/A
 * @bug		N/A
*/
#if STILL_LIB_PB_DMY
UINT16 pbFileCntGet(void) USING_0
{
	return 0;
}
#else
UINT16 pbFileCntGet(void) USING_0
{
	return dcfFileCountGet();
}
#endif

//-----------------------------------------------------------------------------
//pbCurFoldFileSpaceGet
//-----------------------------------------------------------------------------
/**
 * @fn		UINT8 pbCurFoldFileSpaceGet(UINT8 option,UINT32 *fileSize,UINT8 fileType)
 * @brief		query file cluster size in current folder
 * @param	UINT32 *fileSize,[in] UINT8 fileType: FILE_TYPE_JPG=((UINT8)0x01) ,  
 									            FILE_TYPE_AVI=((UINT8)0x02) ,                
 									            FILE_TYPE_WAV=((UINT8)0x03),
												ALL DCF FILES = 0x04
 											
 * @retval	return = SUCCESS / FAIL
 * @see		NULL
 * @author	hy.ren
 * @since	2011-03-08
 * @todo		N/A
 * @bug		some bug need to be done
*/
#if STILL_LIB_PB_DMY
UINT8 pbCurFoldFileSpaceGet(UINT32 *fileSize,UINT8 fileType)
{
	return SUCCESS;
	fileSize =fileSize;
	fileType = fileType;
}
#else
UINT8 pbCurFoldFileSpaceGet(UINT32 *fileSize,UINT8 fileType)
{
	fileSize = fileSize;
	fileType = fileType;
#if 0 
	FDBInfo_t fdbInfo;
	UINT16 fileIndex;
	UINT8 ret = SUCCESS;
	UINT32 clusterSize;
	UINT32 fileTmpSize=0;
	UINT16 fileCnt;
	UINT16 i;
	
	clusterSize = vfsClusterSizeGet();
	fileCnt = pbFileCntGet();
	for(i=1;i<=fileCnt;i++){
		fileIndex = DOS_FileIndexGet(i);
		ret = dcfFileDramFdbParameterRead(fileIndex, &fdbInfo);
		if (ret != SUCCESS) {
			return FAIL; 
		}
		if(fileType==4){
		}
		else if(fileType!=fdbInfo.FileType){
			continue;
		}

		fileTmpSize = fdbInfo.FileSize;
		FileClusterSize(fileTmpSize, clusterSize);  //cluster size byte ;
		*fileSize+=fileTmpSize/1024;   //kb
		if(fileType!=3)
		{
			ret = pbVoiceMemoCheck(fileIndex); 
			if(ret==TRUE){	
					fileTmpSize = vfsFileSizeGet(0);
					FileClusterSize(fileTmpSize, clusterSize);  //cluster size byte ;
					*fileSize+=fileTmpSize/1024;   //kb	
			}
		}
	}
#endif
	return SUCCESS;
}
/*UINT8 pbCurFoldFileSpaceGet(UINT8 option,UINT32 *fileSize,UINT8 fileType)
{
	FDBInfo_t fdbInfo;
	UINT16 fileIndex;
	UINT8 ret = SUCCESS;
	UINT32 clusterSize;
	UINT32 fileTmpSize=0;
	UINT16 fileCnt;
	UINT16 i;
	
	clusterSize = vfsClusterSizeGet();
	fileCnt = pbFileCntGet();
	//printf("fileCnt=%d\n",fileCnt);
	for(i=1;i<=fileCnt;i++){
		fileIndex = DOS_FileIndexGet(i);
		ret = dcfFileDramFdbParameterRead(fileIndex, &fdbInfo);
		//printf("FileIndex=%d\n",DOS_FileIndexGet(i));
		if (ret != SUCCESS) {
			//printf("%s,%d\n",__FILE__,__LINE__);
			return FAIL; 
		}
		
		if(fileType!=fdbInfo.FileType){
			continue;
		}
		
		if(option){ //calculate cluster size
			fileTmpSize = fdbInfo.FileSize;
			FileClusterSize(fileTmpSize, clusterSize);  //cluster size byte ;
			*fileSize+=fileTmpSize/1024;   //kb
		}
		else{
			*fileSize +=( fdbInfo.FileSize)/1024;
		}
		
		ret = pbVoiceMemoCheck(fileIndex); 
		if(ret==TRUE){
			if(option){
				fileTmpSize = vfsFileSizeGet(0);
				FileClusterSize(fileTmpSize, clusterSize);  //cluster size byte ;
				*fileSize+=fileTmpSize/1024;   //kb
			}
			else{
				*fileSize+=(vfsFileSizeGet(0)/1024);
			}
		}
	}

	return SUCCESS;
}*/
#endif

