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
#include "dbg_def.h"
#include "dos32.h"
#include "dosvar.h"
#include "gpio.h"

#define __FILE	__FILE_ID_DOS_OPEN__
#define	DEBUG	0

extern DOS dos;

//return fno, >1 is OK!
UINT8 DOS_FileOpen(UINT8 *fname, UINT8 arg)
{
	UINT8 fno,DevId;
	UINT8 ret;
	UINT8 *path, *name;
	UINT32 max, curr, next;
	xdata FILE *file;
	xdata x86_FDB *fdb;
	xdata DISK *disk;

	//printf("DOS_FileOpen %s\n",fname);
	for(fno=FILE_NORMAL; fno<MAX_FILE; fno++) {
		if(dos.file[fno].DevId == -1)
			break;
	}
#if	DEBUG
	printf("%s %u fno=%bu, %s\n", __FILE__, (unsigned short)__LINE__,  fno, fname);
#endif
	if(fno==MAX_FILE)
		return 0;

	DevId = DOS_Path2Drv(fname);
	file = (FILE *)&dos.file[fno];
	file->mode = arg;
	file->DevId = DevId;
	fdb  = (x86_FDB *)&file->fdb;
	disk = (DISK *)&dos.disk[DevId];
	pdiskFAT_BankStartCluster = &disk->FAT_BankStartCluster;
	pdiskFAT_Bank2Cluster = &disk->FAT_Bank2Cluster;

	name = file->fdb.name;
	path = dos.share;

	DOS_FileNameParser(fname, path, name);
	ret = DOS_SUCCESS;
	if(path != NULL) {
		ret = DIR_Change(path);
	}
	if(ret != DOS_SUCCESS) {
		#if 1
		DOS_CurrDirSet(DevId, DOS_CurrDirGet(DevId));
		#else
		file->DevId = -1;
		return 0;
		#endif
	}
#if	DEBUG
	printf("%s %u fno=%bu\n", __FILE__, (unsigned short)__LINE__,  fno);
#endif
#if	DEBUG
	printf("%s %u file=%lx %bu\n", __FILE__, (unsigned short)__LINE__, (UINT32)file,file->DevId);
#endif
	ret = DIR_Lookup(fno);
#if DEBUG
printf("%s %u ret=%bx,fno=%bx,arg=%bx\n", __FILE__, (unsigned short)__LINE__,ret, fno,arg);
#endif
	if(arg=='r') {
		disk->FAT_WriteOnly1Copy = 0;//Enable write only 1 FAT 
		if(ret != FIND_FILE_OK) {
			file->DevId = -1;
			fno = 0;
		}
	}
	else if(arg=='w') {//if write exist file, will delete it first, no need to delete first
		disk->FAT_WriteOnly1Copy = 1;//Enable write only 1 FAT 
		if(ret == FIND_FILE_OK) {	//already exist
			//free FAT train
#if DEBUG
printf("start Culster=%bx%bx%bx%bx\n",fdb->starthi[1], fdb->starthi[0],fdb->start[1],fdb->start[0]);
#endif
			WRITE8(curr, 0, fdb->starthi[1]);
			WRITE8(curr, 1, fdb->starthi[0]);
			WRITE8(curr, 2, fdb->start[1]);
			WRITE8(curr, 3, fdb->start[0]);
			
			ASSERT(curr, 1); //the start cluster is 0, It might be a file not been writen OK.
		
			if (curr) {
				while(1) {
					if((curr > disk->MaxClusterNum-1) || (curr < 2)){
						next =  EOF_FAT32;
					}
					else{
						next = FAT_TableRead(DevId, curr);
					}
				
					FAT_TableWrite(DevId, curr, 0);
					disk->FreeCluster++;
				//printf("curr=%lu next=%lu\n", curr,next);
					if((next & disk->FAT_EOFValue) == disk->FAT_EOFValue)
						break;
					curr = next;
				}
			}
			file->FirstCluster = 0;
			file->size         = 0;
			DIR_AddFDB(fno);
			FAT_TableWriteBack(DevId);
		}
		//fixed mantis-26368 0n 20080620
		file->fdb.attr = ATTR_A;/* while create file, let it be archive */ 
	}
	WRITE8(file->FirstCluster, 0, fdb->starthi[1]);
	WRITE8(file->FirstCluster, 1, fdb->starthi[0]);
	WRITE8(file->FirstCluster, 2, fdb->start[1]);
	WRITE8(file->FirstCluster, 3, fdb->start[0]);
	WRITE8(file->size, 0, fdb->size[3]);
	WRITE8(file->size, 1, fdb->size[2]);
	WRITE8(file->size, 2, fdb->size[1]);
	WRITE8(file->size, 3, fdb->size[0]);

#if DEBUG
printf("%s %u ret=%bx,fno=%bx\n", __FILE__, (unsigned short)__LINE__,ret, fno);
#endif
	curr = file->FirstCluster;
	//Here do not need to load all FAT, if long AVI file, it will took much time
	//Only append need the last cluster and need to search to the lastest cluster  
	/*  
	max  = (file->size + disk->Cluster2Byte-1) >> (disk->bitCluster2Sector+9);
	while(max) {
		next = FAT_TableRead(DevId, curr);
		if((next & disk->FAT_EOFValue) == disk->FAT_EOFValue) {
			break;
		}
		curr = next;
	}
	file->LastCluster = curr;
	*/
	if(arg=='a') {	//append
		disk->FAT_WriteOnly1Copy = 1;//Enable write only 1 FAT 
		max  = (file->size + disk->Cluster2Byte-1) >> (disk->bitCluster2Sector+9);
		while(max) {
			if((curr > disk->MaxClusterNum-1) || (curr < 2)){
				next =  EOF_FAT32;
			}
			else{
				next = FAT_TableRead(DevId, curr);
			}
			if((next & disk->FAT_EOFValue) == disk->FAT_EOFValue) {
				break;
			}
			curr = next;
		}
		file->LastCluster = curr;
		file->pos         = file->size;
		file->CurrCluster = file->LastCluster;
	}
	else {
		file->pos         = 0;
		file->CurrCluster = file->FirstCluster;
	}
	file->CurrSector = -1;
	file->status = 0;

#if DEBUG
	printf("%s %u fname=%s size=%lu pos=%lu, first=%lu curr=%lu last=%lu\n", __FILE__, (unsigned short)__LINE__\
 			,fname,file->size,file->pos,file->FirstCluster, file->CurrCluster, file->LastCluster);
printf("Open file->pos=%lx,fno=%bx\n",file->pos, fno);
#endif
#if	DEBUG
	printf("%s %u fdb.attr=%bx\n", __FILE__, (unsigned short)__LINE__,  file->fdb.attr);
#endif
//	DOS_FileAlignGet(fno);

	return fno;
}

void DOS_FileClose(UINT8 fno)
{
	UINT8 DevId;
	xdata FILE *file;
	xdata DISK *disk;

	//printf("DOS_FileClose %bx\n",fno);
	if(fno >= MAX_FILE){
		return;
	}
	file  = (FILE *)&dos.file[fno];
	DevId = dos.file[fno].DevId;
	disk = (DISK *)&dos.disk[DevId];
	if(file->mode != 'r' && (!sD_CdGet() || DevId != DRIVE_SD)) {
		if((file ->CurrSector != -1) && file->status=='w') {//possibe remain 1 sector not write
#if DEBUG
			printf("%s %u CurrSector=%08lx buf=%lx\n", __FILE__, (unsigned short)__LINE__,file->CurrSector,file->buf);
#endif
			if( 0 == DOS_SectorWrite(DevId, file->CurrSector, 1, file->buf)) dosRwErr(__FILE,(unsigned short)__LINE__);
		}
		DIR_AddFDB(fno);
		FAT_TableWriteBack(DevId);
		file->status = 0;
		//Add for DCF useage
		USR_DcfFdbAddrSet(fno);
		FAT_CopyTable(DevId);//copy FAT1 to FAT2
	}
	dos.file[fno].DevId = -1;
	return;
}
#if 0
// Quick open if system already knew the FDB, 
// system do not need to paring the FDB.
UINT8 DOS_FileOpenX(FDB *InputFdb)	//for READ only
{
	//UINT8 i8;
	xdata x86_FDB *fdb;
	xdata FILE *file;
	file = (FILE *)&dos.file[FILE_FAST_OPEN];
	fdb  = (x86_FDB *)&file->fdb;
	file->DevId = InputFdb->DevId;
	file->mode  = 'r';
	file->status= 0;
	file->FirstCluster= InputFdb->start;
	file->CurrCluster = file->FirstCluster;
	file->CurrSector  = FAT_Cluster2Sector(InputFdb->DevId, file->CurrCluster);
	file->pos  = 0;
	file->size = InputFdb->size;
	return FILE_FAST_OPEN;
}
#endif
