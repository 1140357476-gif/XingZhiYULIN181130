/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		timelapse.c
 * @brief		for timelapses
 * @author		Ted Chiu, Matt Wang
 * @since		2008-03-14
 * @date		2008-10-31
 */
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/
#include "general.h"
#include "common.h"
#include "timelapse.h"
#include "video_clip.h"
#include "doslink.h"
#include "dos32.h"
#include "dosusr.h"
#include "initio.h"
#include "cardlink.h"
#include "sp1k_rsvblk_api.h"
#include "sdramhw.h"
#include "os_msg.h"
#include "fileindex.h"
#include "pb_general.h"
#include "pb_api.h"
#include "hal_dram.h"
#include "video_pb.h"
#include "dcf_api.h"
#include "hal_gprm.h"
#include "vfs_api.h"
#include "dbg_mem.h"
#include "hal_jpeg.h"
/**************************************************************************
 *				C O N S T A N T S
 **************************************************************************/
#define __FILE    __FILE_ID_TIMELAPSE__
#define DBG_SLASH /

#ifndef _TimeLapsePintf
#define _TimeLapsePintf /DBG_SLASH
#endif
#define MOVI_LIST_HDR_SIZE 0xc  //LIST SIZE MOVI
#define SD_Size_4G ((UINT32)0xffffffff)
#define CUSTOM_INFO_OFFSET	0x1e0
#define HDRIDXOFFSET 0x200
#define CLUSTERSIZE DOS_ClusterSizeGet()
/**************************************************************************
 *				M A C R O S 
 **************************************************************************/

/**************************************************************************
 *				D A T A    T Y P E S
 **************************************************************************/
//to reduce parameters, should reverted at the begining of each operation(_Entry, _AddData, _Stop)
typedef struct timelapseAvi_s { 
	UINT32 moviOffset;
	UINT32 hdr_Start_Cluster;
	UINT32 hdr_Last_Cluster;
	UINT32 movi_Start_Cluster;
	UINT32 movi_Last_Cluster; 
	UINT32 idx_Start_Cluster;
	UINT32 idx_Last_Cluster;
	UINT32 idxSize;
	UINT32 hdrIdxOffset;
	UINT32 hdrIdxSkipCluster;
	UINT32 check_Sum;
}timelapseAvi_t;

/**************************************************************************
 *				G L O B A L   D A T A
 **************************************************************************/
UINT16 xdata timelapseParam[TIMELAPSE_PAR_MAX];
UINT32 dbgCnt;
/**************************************************************************
 *				E X T E R N A L    R E F E R E N C E S 
 **************************************************************************/
extern UINT8 DOS_FileCurrInfoGet(UINT8 fno, UINT8 mode, UINT8 *buf);
extern void jpegYQTableGen(SINT32 quality,	UINT8 *qtable);
extern void jpegUVQTableGen(SINT32 quality, UINT8 *qtable);
/**************************************************************************
 *				E X T E R N A L    R E F E R E N C E S 
 **************************************************************************/
extern DOS dos;
/**************************************************************************
 *				I N T E R N A L    F U N C T I O N    D E C L A R A T I O N S 
 **************************************************************************/
void timelapse_ModifyHdr( UINT8 fno,UINT32 dramAddrW,UINT32 hdrCluster,UINT32 movCluster,UINT32 idxCluster,UINT32 cnt);

/**************************************************************************
 *				F U N C T I O N   D E F I N I T I O N
 **************************************************************************/
static UINT8 timelapse_FileRead(UINT8 fno, UINT32 buf, UINT32 cnt, UINT32 pos, UINT32 cluster){
	xdata FILE *file;
	//UINT32 clusterSize = DOS_ClusterSizeGet();
	file  = (FILE *)&dos.file[fno];
	//file->pos = pos;//%clusterSize;
	//file->mode = 'a';
	if(cluster){
		file->pos = pos;
		file->CurrCluster = cluster;
	}
	return DOS_FileRead(fno,buf,cnt,pos);
}

static UINT8 timelapse_FileWrite(UINT8 fno, UINT32 buf, UINT32 cnt, UINT32 pos, UINT32 cluster){
	xdata FILE *file;
	//UINT32 clusterSize = DOS_ClusterSizeGet();
	file  = (FILE *)&dos.file[fno];
	//file->pos = pos;//%clusterSize;
	//file->CurrCluster = file.FirstCluster;
	file->mode = 'a';
	//printf("%lx,%lx\n",cluster,file->CurrCluster);
	if(cluster){
		file->CurrCluster = cluster;
		file->pos = pos;
	}else{
		file->CurrCluster = file->FirstCluster;
		file->pos = pos;
	}
	return DOS_FileWrite(fno,buf,cnt);
}

static void timelapse_ReadDram(UINT32 addr,UINT8* buf,UINT32 size,UINT8 fmt){
	UINT32 i = size/2;
	HAL_DramSetAddr(addr,K_SDRAM_Prefetch);
	if(fmt==0){
		while(i--){	
			HAL_DramWordDataRead(buf++,buf++);
		}
	}else if(fmt==1){
		while(i--){
			HAL_DramWordDataRead(buf+1,buf+0);
			buf+=2;
		}
	}else if(fmt==2){
		i/=2;
		while(i--){	
			HAL_DramWordDataRead(buf+3,buf+2);
			HAL_DramWordDataRead(buf+1,buf+0);
			buf+=4;
		}
	}
}

static void timelapse_WriteDram(UINT32 addr,UINT8* buf,UINT32 size,UINT8 fmt){
	UINT32 i = size/2;
	HAL_DramSetAddr(addr,K_SDRAM_NoPrefetch);
	if(fmt==0){
		while(i--){	
			HAL_DramWordDataWrite(*buf++,*buf++);
		}
	}else if(fmt==1){
		while(i--){	
			HAL_DramWordDataWrite(buf[1],buf[0]);
			buf+=2;
		}
	}else if(fmt==2){
		i/=2;
		while(i--){
			HAL_DramWordDataWrite(buf[3],buf[2]);
			HAL_DramWordDataWrite(buf[1],buf[0]);
			buf+=4;
		}
	}
}

static void timelapse_FillDramLong(UINT32 address, UINT32 value) USING_0
{
	HAL_DramStartAddrSet(address, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite((UINT8)value,(UINT8)(value>>8));
	HAL_DramWordDataWrite((UINT8)(value>>16),(UINT8)(value>>24));
}

UINT8 timelapse_ParamSet(UINT8 paraId, UINT16 value)
{
	if (paraId >= TIMELAPSE_PAR_MAX) {
		return FAIL;
	}
	
	timelapseParam[paraId] = value;
	return SUCCESS;
}

UINT16 timelapse_ParamGet(UINT8 paraId)
{
	if (paraId >= TIMELAPSE_PAR_MAX) {
		return 0;
	}

	return timelapseParam[paraId];
}
static void timelapse_WriteIdxToDram( UINT32 idxAddr)
{
	/*
	0x69,0x64,0x78,0x31,   //idx
	*/
	HAL_DramSetAddr(idxAddr, K_SDRAM_NoPrefetch );
	HAL_DramWriteByte(0x69,0x64);
	HAL_DramWriteByte(0x78,0x31);
	HAL_DramWriteByte(0x00,0x00);
	HAL_DramWriteByte(0x00,0x00);
}

static void timelapse_WriteMoviToDram( UINT32 movAddr )
{
	/*
	0x4C, 0x49, 0x53, 0x54,    //-Id = "LIST"
	0x38, 0xA0, 0x06, 0x00,    //-Size = 0x????????
	0x6D, 0x6F, 0x76, 0x69,   //movi
	*/
	HAL_DramSetAddr(movAddr,K_SDRAM_NoPrefetch);
	HAL_DramWriteByte(0x4C,0x49);
	HAL_DramWriteByte(0x53,0x54);
	HAL_DramWriteByte(0x00,0x00);
	HAL_DramWriteByte(0x00,0x00);
	HAL_DramWriteByte(0x6D,0x6F);
	HAL_DramWriteByte(0x76,0x69);
}

static void timelapseWriteVidIndex2Dram(UINT32 MoviOfset, UINT32 size , UINT32 AVIIdxAddr) USING_0
{

	HAL_DramStartAddrSet(AVIIdxAddr, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite(0x30,0x30);
	HAL_DramWordDataWrite(0x64,0x63);
	HAL_DramWordDataWrite(0x10,0x00);
	HAL_DramWordDataWrite(0x00,0x00);

	HAL_DramWordDataWrite(READ8(MoviOfset, 3),READ8(MoviOfset, 2));
	HAL_DramWordDataWrite(READ8(MoviOfset, 1),READ8(MoviOfset, 0));
	HAL_DramWordDataWrite(READ8(size, 3),READ8(size, 2));
	HAL_DramWordDataWrite(READ8(size, 1),READ8(size, 0));
	
	//IndexNum += 16;
}

static UINT8 timelapse_DiskCheck( UINT8 fno,UINT32 movSize)
{	
	UINT32 clusterSize;
	UINT32 freeSize;
	UINT32 usedSize;
	UINT32 reqSize;

	clusterSize = DOS_ClusterSizeGet();	//Byte
	freeSize = DOS_GetDiskSpace();//Kb
	
	reqSize = (movSize + clusterSize)>>10;//Kb
	usedSize = dos.file[fno].size>>10;//Kb
	usedSize += reqSize;
	
	if (reqSize> freeSize) {
		//osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		printf("Card full!!\n");
		return TIMELAPSE_CARDFULL;
	}

	if(usedSize >=( SD_Size_4G>>10)){
		//osMsgPost(SP1K_MSG_MEDIA_REC_DISK_FULL);
		printf("File size > 4G!!!\n");
		return TIMELAPSE_MAX_FILESIZE;
	}
	
	return TIMELAPSE_SUCCESS;
}

static void timelapse_CheckSum_Get(timelapseAvi_t *avi)
{
	avi->check_Sum = avi->moviOffset + avi->hdr_Start_Cluster + avi->hdr_Last_Cluster
		+ avi->movi_Start_Cluster + avi->movi_Last_Cluster + avi->idx_Start_Cluster
		+avi->idx_Last_Cluster + avi->idxSize + avi->hdrIdxOffset + avi->hdrIdxSkipCluster;
}

static void timelapse_WriteAviInfo( UINT8 fno,UINT32 fileAddr ,timelapseAvi_t aviInfo )
{
	UINT32 offset = 0x150, startPos;
	UINT8 *avibuf = NULL;
	UINT16 bufLenth;
	UINT8 buf[2];
	
	avibuf = (UINT8*)&aviInfo;
	bufLenth = sizeof(aviInfo);
	
	startPos = offset & ~(UINT32)511;    
    offset = offset % 512;

	timelapse_FileRead(fno,fileAddr<<1,0x200,0,aviInfo.hdr_Start_Cluster);
    DOS_FileSeek(fno, startPos, SEEK_SET);           
	
    if(offset & 1){ //first odd byte
        offset >>= 1;
		HAL_DramSetAddr(fileAddr+offset, K_SDRAM_Prefetch );
		HAL_DramWordDataRead(&buf[0], &buf[1]);
       
        buf[1] = *avibuf ++;
        bufLenth --;
        //printf("+%bx,%bx\n", buf[0],buf[1]);

        HAL_DramSetAddr( fileAddr+offset, K_SDRAM_NoPrefetch);
        HAL_DramWordDataWrite(buf[0], buf[1]);
        offset ++;
    }
    else{
        offset >>= 1;
        HAL_DramSetAddr( fileAddr+offset, K_SDRAM_NoPrefetch);
    }

    while(bufLenth>1){
        buf[0] = *avibuf ++;
        buf[1] = *avibuf ++;
        //printf("%bx,%bx\n", buf[0],buf[1]);
        HAL_DramWordDataWrite(buf[0], buf[1]);
        bufLenth -= 2;
        offset ++;
    }

    if(bufLenth){// last odd byte
       HAL_DramSetAddr(fileAddr+offset, K_SDRAM_Prefetch );
       HAL_DramWordDataRead(&buf[0], &buf[1]);

        buf[0] = *avibuf;
        //printf("-%bx,%bx\n", buf[0],buf[1]);
        
       HAL_DramSetAddr( fileAddr+offset, K_SDRAM_NoPrefetch);
       HAL_DramWordDataWrite(buf[0], buf[1]);
    }
                        
    //ret = DOS_FileCurrSectorWrite(fno, fileAddr);  //
	DOS_FileWrite(fno, fileAddr<<1, 0x200);
}

static UINT8 timelapse_Check_Sum(timelapseAvi_t *aviInfo)
{
	UINT32 sum;
	
	sum = aviInfo->moviOffset + aviInfo->hdr_Start_Cluster + aviInfo->hdr_Last_Cluster
		+ aviInfo->movi_Start_Cluster + aviInfo->movi_Last_Cluster + aviInfo->idx_Start_Cluster
		+aviInfo->idx_Last_Cluster + aviInfo->idxSize +aviInfo->hdrIdxOffset
		+ aviInfo->hdrIdxSkipCluster;
	
	//printf("sum1=%ld,sum2=%ld\n",sum,aviInfo->check_Sum);
	if(aviInfo->check_Sum != sum){
		printf("check sum fail!\n");
		return FAIL;
	}
	return SUCCESS;
}

static UINT32 timelapse_ReadAviInfo( UINT8 fno,UINT32 fileAddr,timelapseAvi_t *tlsInfo )
{
	UINT8 ret = SUCCESS;
	UINT8 parity_Check = 0 ;
	UINT32 offset = 0x150;
	UINT8 buf[2];
	UINT8 *avibuf = NULL;
	UINT32 aviLenth;
	
	avibuf = (UINT8 *)tlsInfo;//tmp;
	aviLenth = sizeof(timelapseAvi_t);
    offset = offset % 512;
	DOS_FileSeek(fno,0,SEEK_SET);
	DOS_FileRead(fno, fileAddr<<1,0x200, 0);
	//dumpData(0x200, (UINT8*)fileAddr,1);
    if(offset & 1){ //first odd byte
        offset >>= 1;
        HAL_DramSetAddr(fileAddr+offset, K_SDRAM_Prefetch );
        HAL_DramWordDataRead(&buf[0], &buf[1]);

        *avibuf ++ = buf[1];
        aviLenth --;

        offset ++;
    }
    else{
        offset >>= 1;
        HAL_DramSetAddr(fileAddr+offset, K_SDRAM_Prefetch );
    }

    while(aviLenth>1){
        HAL_DramWordDataRead(&buf[0], &buf[1]);
		//printf("buf[0]=%bx,buf[1]=%bx\n",buf[0],buf[1]);
        *avibuf++ = buf[0];
        *avibuf++ = buf[1];

        aviLenth -= 2;
        offset ++;
    }

    if(aviLenth){// last odd byte
        HAL_DramSetAddr(fileAddr+offset, K_SDRAM_Prefetch );
        HAL_DramWordDataRead(&buf[0], &buf[1]);
        *avibuf ++ = buf[0];
    }
	ret = timelapse_Check_Sum( tlsInfo );
	
	return ret;	
}

static UINT8 timelapse_ExpendSpace(UINT8 fno,UINT32 dataAddr, UINT32 *lstCluster,UINT32 dataLenth)
{
	UINT32 startCluster,currCluser=0,nextCluster,firstCluster;
	UINT32 gainCluster,datClusterCnt,clusterSize;
	UINT32 sector;
	UINT8 i;
	UINT8 sts = SUCCESS;

	clusterSize = DOS_ClusterSizeGet();
	datClusterCnt = dataLenth/clusterSize;
    //printf("need=%ld\n",datClusterCnt);
	while(datClusterCnt){
		 gainCluster = FAT_FreeClusterGet(dos.CurrDrv,datClusterCnt,&nextCluster);
		// printf("get=%ld\n",gainCluster);
		 startCluster = nextCluster;
		 if(currCluser==0){
		 	currCluser =nextCluster++;
			i=1;
			firstCluster = currCluser;
		 }
		 else{
			i=0;
		 }
		 
		/* create FAT list ... */
		for(;i<gainCluster;i++){	
			//printf("%bu,%lx->%lx\n",i,currCluser,nextCluster);
			FAT_TableWrite(dos.CurrDrv, currCluser, nextCluster);
			currCluser = nextCluster++;
		}
		
		FAT_TableWrite(dos.CurrDrv, currCluser, dos.disk.FAT_EOFValue);
		sector = FAT_Cluster2Sector(dos.CurrDrv, startCluster);	
		sts = DOS_SectorWrite(dos.CurrDrv,sector,(UINT32)gainCluster*clusterSize/512,dataAddr);	
		if(sts != TRUE){
			//printf("write error:Line=%d\n",__LINE__);
			return FAIL;
		}
		dataAddr+=(UINT32)gainCluster*clusterSize/2;
		if( gainCluster>=datClusterCnt ){
			break;
		}
		 datClusterCnt -= gainCluster;
	}
	
	nextCluster = FAT_TableRead(dos.CurrDrv,*lstCluster);
	FAT_TableWrite(dos.CurrDrv,*lstCluster,firstCluster);
	FAT_TableWrite(dos.CurrDrv,currCluser,nextCluster);
	
	dos.file[fno].size+=dataLenth;
	*lstCluster = currCluser;//return last cluster

	return SUCCESS;
}

/* return movi last cluster */
static UINT8 timelapse_AppendMoviList( UINT8 fno,UINT32 dataAddr,UINT32 *lstMovCluster,UINT32 dataLenth )
{
	UINT8 ret;

	/* Write 0xdc */
	//timelapse_FillDramLong(dataAddr, 0x63643030);
	timelapse_FillDramLong(dataAddr+ 2, dataLenth-8);//dataLenth -= 8; //not include 00dc ..
	
	ret = timelapse_ExpendSpace(fno,dataAddr,lstMovCluster,dataLenth);
	if(ret!=SUCCESS){
		return FAIL;
	}
	
	return SUCCESS;
}

static UINT8 timelapse_FilterFile(UINT8 *fileName)
{
	/* patch:deal with WGI file,this method is not recommand ,the 
	   best method is DCF can support WGI type */
	if( USR_IsOurFileExt(fileName,MEDIA_AVI) == TRUE){
		//printf("Avi!\n");
		return FAIL;	
	}else if(USR_IsOurFileExt(fileName,MEDIA_WAV) == TRUE){
		//printf("Jpg!\n");
		return FAIL;	
	}else if(USR_IsOurFileExt(fileName,MEDIA_JPG) == TRUE){
		//printf("Wav!\n");
		return FAIL;
	}
	
	return SUCCESS;

}

/* brief: add one frame information */
UINT8 timelapse_AppendFrame(UINT32 startBufAddrW, UINT32 dataLength )
{
	UINT8 fhandle;
	UINT16 fileIndex = 1;
	UINT32 gainCluster = 0;
	UINT32 curr=0;
	UINT32 moviSize;
	UINT32 moviListSize;
	UINT32 clusterSize;
	UINT32 dataOffset = 0x200; //0x200 for read AVI informatin from header
	UINT32 junkSize,junkOffset;
	//UINT32 time0,time1;
	timelapseAvi_t timelapeAVI;
	PBFile_t pFileInfo;
	UINT32 idxSize =0;
	UINT32 tmp;
	UINT8 ret;

	printf("dbg_%ld\n",++dbgCnt);
	//HAL_ReadGTimer(&time0);
	clusterSize = DOS_ClusterSizeGet();
	startBufAddrW-=dataOffset;
	
	dataLength = (dataLength +1)&~1;
	moviSize = (dataLength+clusterSize-1)&~(clusterSize-1);
	junkSize = dataLength%clusterSize;
	junkSize = clusterSize - junkSize;
	junkOffset = dataOffset+dataLength/2;

	if( dcfTLSAssignFolder()){
		//printf("fail:L=%d\n",__LINE__);
		return FAIL;
	}
	if ((pbFileInfoGet(G_USR_FileCount, &pFileInfo) != SUCCESS) ) {
		return FAIL;
	}
	ret = timelapse_FilterFile(pFileInfo.FileName);
	if(ret != SUCCESS){
		return FAIL;
	}
	fhandle = DOS_FileOpen(pFileInfo.FileName, 'r');
	if (fhandle == 0){
		return FAIL;
	}
	
	/*get last avi info from current avi chunk */
	ret = timelapse_ReadAviInfo(fhandle,startBufAddrW,&timelapeAVI);
	if( ret != SUCCESS ){
		DOS_FileClose(fhandle);
		return FAIL;
	}	
	ret = timelapse_DiskCheck(fhandle, moviSize);
	if(ret != TIMELAPSE_SUCCESS){
		DOS_FileClose(fhandle);
		return ret;
	}
	/* fill junk data */
	if(junkSize){
		HAL_GprmDramFill(startBufAddrW+junkOffset, junkSize, 0);
	}
	ret = timelapse_AppendMoviList(fhandle,startBufAddrW+dataOffset,&timelapeAVI.movi_Last_Cluster,moviSize);
	if(ret == FAIL){
		return FAIL;
	}
	/* append movi List */
	timelapse_FileRead(fhandle,startBufAddrW<<1,0x200, 0,timelapeAVI.movi_Start_Cluster);
	timelapse_ReadDram(startBufAddrW+2,(UINT8*)&moviListSize,sizeof(UINT32),2);
	moviListSize += moviSize;
	timelapse_WriteDram(startBufAddrW+2,(UINT8*)&moviListSize,sizeof(UINT32),2);//modify movi list size
	timelapse_FileWrite(fhandle,startBufAddrW<<1,0x200,0,timelapeAVI.movi_Start_Cluster);
	
    /***************************Add file index and header index *************************/
	moviSize-=8;
	timelapeAVI.moviOffset+=8;
	/* modify index list container size */	
	timelapse_FileRead(fhandle,startBufAddrW<<1,0x200, 0,timelapeAVI.idx_Start_Cluster);
	timelapse_ReadDram(startBufAddrW + 2,(UINT8*)&idxSize,sizeof(UINT32),2);
	idxSize+=16;
	timelapse_WriteDram(startBufAddrW + 2, (UINT8*)&idxSize,sizeof(UINT32),2);
	timelapse_FileWrite(fhandle,startBufAddrW<<1,0x200, 0,timelapeAVI.idx_Start_Cluster);

	/* modify index List */
	fastFileSeekEnd(fhandle,timelapeAVI.idx_Last_Cluster);
	HAL_GprmDramFill(startBufAddrW, clusterSize, 0); //attention here
	timelapseWriteVidIndex2Dram(timelapeAVI.moviOffset,  moviSize, startBufAddrW);
	DOS_FileWrite(fhandle,startBufAddrW<<1,16);
	DOS_FileCurrInfoGet(fhandle,0, (UINT8*)&timelapeAVI.idx_Last_Cluster);
	
	/* header index */
	if(((timelapeAVI.hdrIdxOffset+16)/clusterSize)>timelapeAVI.hdrIdxSkipCluster){
		//printf("^_^\n");
		tmp = clusterSize - timelapeAVI.hdrIdxOffset%clusterSize;
		timelapse_FileWrite(fhandle,startBufAddrW<<1,tmp, timelapeAVI.hdrIdxOffset,timelapeAVI.hdr_Last_Cluster);
		ret = timelapse_ExpendSpace(fhandle,startBufAddrW+(tmp>>1),&timelapeAVI.hdr_Last_Cluster,clusterSize);
		if(ret!=SUCCESS){
			return FAIL;
		}
		
		timelapeAVI.hdrIdxSkipCluster++;
		//printf("first=%lx,%lx\n",timelapeAVI.hdr_Last_Cluster,timelapeAVI.hdrIdxSkipCluster);
	}else{
		timelapse_FileWrite(fhandle,startBufAddrW<<1,16, timelapeAVI.hdrIdxOffset,timelapeAVI.hdr_Last_Cluster);
	}
	timelapse_FileRead(fhandle,startBufAddrW<<1,0x200, 0x200,timelapeAVI.hdr_Start_Cluster);
	timelapse_WriteDram(startBufAddrW+2, (UINT8*)&idxSize,sizeof(UINT32),2);
	timelapse_FileWrite(fhandle,startBufAddrW<<1,0x200, 0x200,timelapeAVI.hdr_Start_Cluster);
	
	timelapeAVI.moviOffset += moviSize;
	timelapeAVI.idxSize += 16;
	timelapeAVI.hdrIdxOffset+=16;
	/************************************************************************************/
	
	timelapse_ModifyHdr(fhandle,startBufAddrW,timelapeAVI.hdr_Start_Cluster,timelapeAVI.movi_Start_Cluster,timelapeAVI.idx_Start_Cluster,timelapeAVI.hdrIdxSkipCluster);
	timelapse_CheckSum_Get(&timelapeAVI);
	//printf("sec=%lx,%lx\n",timelapeAVI.hdr_Last_Cluster,timelapeAVI.hdrIdxSkipCluster);
	timelapse_WriteAviInfo(fhandle,startBufAddrW,timelapeAVI);

	//printf("finish\n");
	DOS_FileClose(fhandle);
	//HAL_ReadGTimer(&time1);
	//printf("cost time=%ld\n",time1-time0);
	
	return TIMELAPSE_SUCCESS;
}

static void timelapse_InitHdr(UINT32 hdrAddrW, UINT16 width, UINT16 height)
{
	UINT32 tmp32;
	
	sp1kNandRsvRead(RES_AVIHeader_PCM, hdrAddrW<<1);

	//for test
	tmp32 = 512;
	timelapse_FillDramLong(hdrAddrW+(0x04>>1), tmp32);

	//mainHeader: dwMicroSecPerFrame
	if(!timelapseParam[TIMELAPSE_PAR_FPS]){
		timelapseParam[TIMELAPSE_PAR_FPS] = 2;//default frame = 2
	}
	tmp32 = 1000000/timelapseParam[TIMELAPSE_PAR_FPS];
	timelapse_FillDramLong(hdrAddrW+(0x20>>1), tmp32);
	//streams, mask aud stream
	tmp32 = 1;
	timelapse_FillDramLong(hdrAddrW+(0x38>>1), tmp32);
	//width, height
	timelapse_FillDramLong(hdrAddrW+(0x40>>1), width);
	timelapse_FillDramLong(hdrAddrW+(0x44>>1), height);
	
	//strh: reserved3
	tmp32 = height;
	tmp32 = (tmp32 << 16) + width;
	timelapse_FillDramLong(hdrAddrW+(0xA0>>1), tmp32);

	//strf:widht, height
	timelapse_FillDramLong(hdrAddrW+(0xB0>>1), width);
	timelapse_FillDramLong(hdrAddrW+(0xB4>>1), height);
	//biSizeImage
	tmp32 = (UINT32)width*height*3;
	timelapse_FillDramLong(hdrAddrW+(0xC0>>1), tmp32);

	//mask aud stream
	tmp32 = 0x4B4E554A; //JUNK
	timelapse_FillDramLong(hdrAddrW+(0xD4>>1), tmp32);	

	//modidy audio stream flag
	tmp32 = 0x7364756E;//nuds//0x66736C74;//tlsf
	timelapse_FillDramLong(hdrAddrW+(0xE8>>1), tmp32);	
	
	//custom info
	tmp32 = timelapseParam[TIMELAPSE_PAR_SNAPGAP];
 	HAL_DramSetAddr(hdrAddrW+(CUSTOM_INFO_OFFSET>>1), K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite((UINT8)tmp32,(UINT8)(tmp32>>8));
}

static void timelapse_ModifyHdr( UINT8 fno,UINT32 dramAddrW,UINT32 hdrCluster,UINT32 movCluster,UINT32 idxCluster,UINT32 cnt)
{
	UINT32 fileSize;
	UINT32 frames, scale, rate, fps;
	UINT32 hdrSize;
	UINT32 clusterSize;
	UINT32 movSize,idxSize;
	
	clusterSize = DOS_ClusterSizeGet();
	hdrSize = clusterSize*(cnt+1);

	/* Read movi List size */
	timelapse_FileRead(fno,dramAddrW<<1,0x200,0,movCluster);
	timelapse_ReadDram(dramAddrW + 2,(UINT8*)&movSize,sizeof(UINT32),2);
	movSize+=8;

	/* Read index List size */
	timelapse_FileRead(fno,dramAddrW<<1,0x200,0,idxCluster);
	timelapse_ReadDram(dramAddrW + 2,(UINT8*)&idxSize,sizeof(UINT32),2);

	/* Read AVI header */
	timelapse_FileRead(fno,dramAddrW<<1,1024,0,hdrCluster);

	//printf("mov=%lx,idx=%lx\n",movSize,idxSize);
	fileSize = hdrSize + movSize + (idxSize+8) - 8;// avi.hdrSize + avi.moviSize + (avi.lastIdxSize + 8) - 8;
	timelapse_FillDramLong(dramAddrW+(4>>1), fileSize);
	hdrSize = hdrSize - 0x14;	
	timelapse_FillDramLong(dramAddrW+(0x10>>1),hdrSize);
	
	frames = idxSize / 16; // no audio
	fps = videoPbReadDword(dramAddrW+ (0x20>>1));
	rate = 1000000;//frames * 1000 ;
	scale = fps; //(UINT32)((float)fps * frames / 1000);
		
	timelapse_FillDramLong(dramAddrW+(0x30>>1), frames);
	timelapse_FillDramLong(dramAddrW+(0x80>>1), scale);
	timelapse_FillDramLong(dramAddrW+(0x84>>1), rate);
	timelapse_FillDramLong(dramAddrW+(0x8C>>1), frames);

	/* cluster size - 0x140; real junk include index and cluster alignment	*/
	timelapse_FillDramLong(dramAddrW+(0x13C>>1), hdrSize-0x140+0x14);//videoFillDramLong(dramAddrW+(0x13C>>1), junkSize);
	timelapse_FillDramLong(dramAddrW+(0x204>>1), idxSize);

	timelapse_FileWrite(fno,dramAddrW<<1,1024,0,hdrCluster);
}

static void timelapse_ModifyMovi( UINT8 fd, UINT32 dramAddrW,UINT32 moviSize,UINT32 moviCluster)
{	
	timelapse_FileRead(fd,dramAddrW<<1,0x200,0,moviCluster);
	timelapse_FillDramLong(dramAddrW + 2, moviSize +4 );
	timelapse_FileWrite(fd,dramAddrW<<1,0x200,0,moviCluster);
}

/* create AVI file from dram */
UINT8 timelapse_Entry(UINT32 dramAddrW, UINT16 width, UINT16 height,UINT32 vlcSize)
{
	UINT32 freeSize;
	UINT32 offset;
	timelapseAvi_t avi;
	UINT8 fd;
	UINT8 aviName[12];
	UINT32 sizeByCluster = 0;
	UINT32 clusterSize;
	UINT32 realSize,tmp;
	UINT32 aviSize;
	UINT32 junkSize,junkOffset;

	dbgCnt = 1;
	if( dcfTLSAssignFolder()){
		return FAIL;
		//printf("fail:L=%d\n",__LINE__);
	}
	
	freeSize = DOS_GetDiskSpace();
	if(freeSize < 0x800 ){ // reserved size 0x200000
		return FAIL;
	}
	dcfFileIndexUpdate();
	/* rename AVI name */
	dcfMediaNameGet(aviName, G_USR_FileIndex,MEDIA_TLS);
	fd = DOS_FileOpen(aviName, 'w');
	if (fd == 0){
		return FAIL;
	}
	avi.moviOffset = 4;
	clusterSize = DOS_ClusterSizeGet();

	/* reverse some space for avi header */
	dramAddrW = dramAddrW- ((clusterSize+MOVI_LIST_HDR_SIZE)>>1);

	/* movi list 0x4C 49 53 54  F8 FF 4B 00   6D 6F 76 69 ,20 bytes */
	tmp= (vlcSize + MOVI_LIST_HDR_SIZE + clusterSize-1)&~(clusterSize-1);//cluster aligment
	realSize = tmp - MOVI_LIST_HDR_SIZE -8; //not include 0xdc

	/* load avi header List and init header */
	timelapse_InitHdr(dramAddrW, width, height);
	/* Write header index */
	timelapse_WriteIdxToDram(dramAddrW+(HDRIDXOFFSET>>1));
	timelapse_FillDramLong(dramAddrW +(HDRIDXOFFSET>>1)+ 2, 16);
	timelapseWriteVidIndex2Dram(avi.moviOffset, realSize, dramAddrW +(HDRIDXOFFSET>>1)+ 4);
	
	sizeByCluster += clusterSize;
	/* load movi List */
	offset = clusterSize>>1;
	sizeByCluster +=tmp;
	timelapse_WriteMoviToDram(dramAddrW+offset);
	//timelapse_FillDramLong(dramAddrW + offset + 6, 0x63643030);
	timelapse_FillDramLong(dramAddrW + offset + 6 + 2, realSize);
	
	/* Add movi junk */
	tmp = (vlcSize+1)&~1;
	junkSize = (tmp+12) % clusterSize;
	junkSize = clusterSize - junkSize;
	junkOffset = offset +(tmp+12)/2;
	if(junkSize){
		HAL_GprmDramFill(dramAddrW+junkOffset, junkSize, 0);
	}
	
	/* Write to card */
	DOS_FileWrite(fd,dramAddrW<<1,sizeByCluster);
	avi.hdr_Last_Cluster = dos.file[fd].FirstCluster;
	avi.hdr_Start_Cluster = avi.hdr_Last_Cluster;
	avi.movi_Last_Cluster = dos.file[fd].LastCluster;
	avi.movi_Start_Cluster = FAT_TableRead(dos.CurrDrv,avi.hdr_Last_Cluster);
	avi.hdrIdxOffset = HDRIDXOFFSET+ 8+16;
	avi.hdrIdxSkipCluster = 0;
	
	/* Write index */
	DOS_FileSeek(fd, 0, SEEK_END);
	timelapse_WriteIdxToDram(dramAddrW);
	timelapse_FillDramLong(dramAddrW + 2, 16);
	timelapseWriteVidIndex2Dram(avi.moviOffset, realSize, dramAddrW + 4);
	DOS_FileWrite(fd,dramAddrW<<1,24);
	avi.idx_Last_Cluster = dos.file[fd].LastCluster;
	avi.idx_Start_Cluster = avi.idx_Last_Cluster;
	
	/* modify header */
	avi.moviOffset += realSize;
	avi.idxSize = 16;	

	timelapse_ModifyMovi(fd,dramAddrW,realSize+8,avi.movi_Start_Cluster);
	timelapse_ModifyHdr(fd,dramAddrW,avi.hdr_Start_Cluster,avi.movi_Start_Cluster,avi.idx_Start_Cluster,avi.hdrIdxSkipCluster);
	
	/* Save AVI information */
	timelapse_CheckSum_Get(&avi);
	timelapse_WriteAviInfo(fd,dramAddrW,avi);
	aviSize = vfsFileSizeGet(fd);
	DOS_FileClose(fd);
	
	dcfFileIndexUpdate();
	dcfNewFileDramFdbParameterUpdate(K_DCF_WGI_TYPE ,aviSize);

	return SUCCESS;
}

void timelapse_AddFrameHdr(UINT32 addr,UINT16 width,UINT16 height,UINT8 qIdx)
{
	UINT8 qtable[130];
	UINT32 dramAddr = addr +16;
	UINT32 tmpAddr;
	qtable[0] = 0;
	qtable[65] = 1;

	tmpAddr = addr-0x200;
	sp1kNandRsvRead(0xb,tmpAddr<<1);
	HAL_GprmDramDma(tmpAddr<<1,addr<<1,0x104,1);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaSetDramAddr(addr);
	HAL_DmaDo(0,1,0x0104,0,0);
	XBYTE[0x38E8] = (UINT8)(height>>8);
	XBYTE[0x38E9] = (UINT8)(height);
	
	XBYTE[0x38EA] = (UINT8)(width>>8);
	XBYTE[0x38EB] = (UINT8)(width);
	HAL_DmaSetSramAddr(0x0000);   
	HAL_DmaSetDramAddr(addr);
	HAL_DmaDo(1,0,0x0104,0,0);

	// generation qtable 
	jpegYQTableGen((SINT32)qIdx, qtable +1 );
	jpegUVQTableGen((SINT32)qIdx, qtable +66);
	HAL_DramDmaWrite(qtable, dramAddr, 130, 1);
}
