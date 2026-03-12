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

//return 0 is OK!
//test OK!
UINT8 DOS_FileJoint(UINT8 head, UINT8 tail)
{
	UINT8 DevId;
	xdata FILE *file_h, *file_t;
	xdata x86_FDB *fdb;
	xdata DISK *disk;

	file_h = (FILE *)&dos.file[head];
	file_t = (FILE *)&dos.file[tail];
	DevId  = file_t->DevId;
	if(DevId != file_t->DevId) {
		return DOS_ERR_INVALID_DRV_ID;
	}

	if((file_h->mode == 'r') || (file_t->mode == 'r')) {
		return DOS_ERR_INVALID_OPTION;
	}

	disk = (DISK *)&dos.disk[DevId];
	#if	0
	if(file_h->size & (disk->Cluster2Byte-1)) {
		return DOS_FILE_SIZE_MISMATCH;
	}
	#else
	file_h->size +=  (disk->Cluster2Byte-1);
	file_h->size &= ~(disk->Cluster2Byte-1);
	#endif
#if DEBUG
	printf("%s %u file_h=%lu, file_t=%lu\n", __FILE__, (unsigned short)__LINE__,file_h->size, file_t->size);
#endif


	//joint by connet FAT train
	FAT_TableWrite(DevId, file_h->LastCluster, file_t->FirstCluster);
#if DEBUG
	printf("%s %u %lu--%lu\n", __FILE__, (unsigned short)__LINE__,file_h->FirstCluster, file_h->LastCluster);
	printf("%s %u %lu--%lu\n", __FILE__, (unsigned short)__LINE__,file_t->FirstCluster, file_t->LastCluster);
	printf("%s %u %lu->%lu\n", __FILE__, (unsigned short)__LINE__,file_h->LastCluster, file_t->FirstCluster);
#endif
	//erase the second file
	fdb  = (x86_FDB *)&file_t->fdb;
	fdb->name[0] = 0xE5;
	DIR_AddFDB(tail);
	file_t->DevId = -1;	//free this file handle

	file_h->size += file_t->size;
	DIR_AddFDB(head);
	FAT_TableWriteBack(DevId);

	file_h->LastCluster = file_t->LastCluster;
#if DEBUG
	printf("%s %u %lu--%lu\n", __FILE__, (unsigned short)__LINE__,file_h->FirstCluster, file_h->LastCluster);
#endif
	return DOS_SUCCESS;
}