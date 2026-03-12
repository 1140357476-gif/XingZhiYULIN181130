/**************************************************************************
 *         Copyright(c) 2008 by Sunplus mMedia  Technology Co., Ltd.      *
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
 *  Author: Ted Chiu                                                      *
 **************************************************************************/
#include <string.h>
#include <stdio.h>
#include "general.h"
#include "dos32.h"

#define	DEBUG	0

extern DOS dos;

//is new pos > filesize, the pos will update IFF wrt op
//check MSDN for details
//return err code
UINT8 DOS_FileSeek(UINT8 fno, UINT32 offset, UINT8 mode)
{
	UINT8 DevId, ret;
	UINT32 oldPos, cluster, num, i32;
	xdata FILE *file;
	xdata DISK *disk;

#if DEBUG
	printf("DOS_FileSeek %bx %lx %bx\n",fno,offset,mode);
#endif
	ret   = DOS_SUCCESS;
	file  = (FILE *)&dos.file[fno];
	DevId = file->DevId;
	disk  = (DISK *)&dos.disk[DevId];

	if(file->pos > file->size)
		file->pos = file->size;
	oldPos = file->pos;

	//search from the begin of file
	if(mode==SEEK_SET) {
		file->pos = offset;
	}
	//search from curr pos
	else if(mode==SEEK_CUR) {	//forward, if pos > size, ignore if read, rsv disk space if wrt
		file->pos += offset;
		if(file->pos < offset) {	//overflow
			ret = DOS_ERR_INVALID_OPTION;
		}
	}
	else if(mode==-SEEK_CUR) {	//backward
		if(file->pos < offset) {	////underflow
			ret = DOS_ERR_INVALID_OPTION;
		}
		file->pos -= offset;
	}
	//search from the end of file
	else if(mode==SEEK_END) {
		file->pos = file->size + offset;
		if(file->pos < file->size) {	//overflow
			ret = DOS_ERR_INVALID_OPTION;
		}
	}
	else if(mode==-SEEK_END) {	//backward
		if(file->size < offset) {	//underflow
			ret = DOS_ERR_INVALID_OPTION;
		}
		file->pos = file->size - offset;
	}
#if DEBUG
	printf("pos %lx,oldPos=%lx, %bx\n",file->pos,oldPos,ret);
#endif
	if(ret==DOS_SUCCESS) {
		if(file->pos < file->size) {	//if pos > size, take effects when wrt
			if(file->pos <= oldPos) {
				num  = file->pos >> disk->bitCluster2Byte;
				i32 = oldPos >> disk->bitCluster2Byte;
				if( num == 0 ){
					cluster = file->FirstCluster;
				}
				else{ 
					num = i32 - num;
					cluster = file->CurrCluster - num - 1;
					if((cluster > disk->MaxClusterNum-1) || (cluster < 2)){
						cluster =  EOF_FAT32;
					}
					else{
						cluster = FAT_TableRead(DevId, cluster);
					}
				}
#if DEBUG
				printf("i32=%lu num=%lu,curr=%lu\n",i32,num,file->CurrCluster);
#endif
			}
			else{
				//printf("oldPos=%lu new=%lu",oldPos,file->pos);
				i32 = oldPos >> disk->bitCluster2Byte;
				num = file->pos >> disk->bitCluster2Byte;
				//printf(" n=%lu %lu \n",i32,num);
				num -= i32;
				cluster = file->CurrCluster;	//file size > 0
				for(i32=0; i32<num; i32++ ) {
					//printf("%lu->",cluster);
					if((cluster > disk->MaxClusterNum-1) || (cluster < 2)){
						cluster =  EOF_FAT32;
					}
					else{
						cluster = FAT_TableRead(DevId, cluster);
					}
					//printf("%lu ",cluster);
					if((cluster & disk->FAT_EOFValue) == disk->FAT_EOFValue) {
						//unknown case
					}
				}
			}
			file->CurrCluster = cluster;
#if DEBUG
	printf("num %lx cluster=%lx\n",num,cluster);
#endif
#if DEBUG
	printf("offset cluster=%lx\n",cluster);
#endif
		}
		else if(file->mode=='r') {	//pos must small than size, max=size-1
			ret = DOS_ERR_INVALID_OPTION;
		}
	}
	return ret;
}

UINT8 fastFileSeekEnd( UINT8 fno, UINT32 lastCluster)
{
	xdata FILE *file;

	file  = (FILE *)&dos.file[fno];
	file->pos = file->size;
	
	if(file->pos) {	//if pos > size, take effects when wrt
		file->CurrCluster = lastCluster;	
	}
	else{
		file->CurrCluster = file->FirstCluster;
	}
	
	//printf("end cluster=%lx\n",file->CurrCluster);
	return DOS_SUCCESS;
}