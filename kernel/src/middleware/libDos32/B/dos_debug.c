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

extern UINT32 dos_var_start;
extern DOS dos;

void DOS_InfoShow(UINT8 DevId)
{
	//UINT8 i8;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];

	#if 0
	dumpData(16, ((UINT32)disk >> 1), 1);
	#endif
	if(DevId >= MAX_DEV || disk->FsType==0) {
		printf("ERROR DRIVE ID...\n");
		return;
	}

	printf("FsType              = <FAT%2bu>\n",disk->FsType);
#if 0
	printf("vol name            = <");
	for(i8=0; i8<11; i8++)
		printf("%c",disk->Name[i8]);
	printf(">\n");
#endif
	printf("Total Size          = %lu MB\n",DOS_TotalSize(DevId) >> 10 );
	printf("Free Space          = %lu, %lu KB\n", disk->FreeCluster, (disk->FreeCluster << disk->bitCluster2Sector) >> 1);
	printf("TotalCluster        = %lu\n",disk->TotalCluster);
	printf("ClusterSize         = %lu\n",disk->Cluster2Byte);
#if 0
	printf("PhySectors          = %lu (0x%lX)\n",disk->PhySectors,disk->PhySectors);
	printf("LogSectors          = %lu (0x%lX)\n",disk->LogSectors,disk->LogSectors);
	//printf("HiddenSectors       = %lu (0x%lX)\n",disk->HiddenSectors,disk->HiddenSectors);
	//printf("Cluster2Sector      = %bu\n",disk->Cluster2Sector);
	//printf("bitCluster2Sector   = %bu\n",disk->bitCluster2Sector);
	//printf("Cluster2Fdb         = %u\n",disk->Cluster2Fdb);
#endif
//	printf("RootEntries         = %u\n",disk->RootEntries);
	//printf("RootStartCluster    = %lu\n",disk->RootStartCluster);
//	printf("RootStartSector     = %lu\n",disk->RootStartSector);
	//printf("CurrDirStartCluster = %lu(0x%lX)\n",disk->CurrDirStartCluster,disk->CurrDirStartCluster);
//	printf("DataStartSector     = %lu\n",disk->DataStartSector);
//	printf("FAT_StartSector     = %lu\n",disk->FAT_StartSector);
	//printf("FAT_FirstFreeCluster= %lu(0x%lX)\n",disk->FAT_FirstFreeCluster,disk->FAT_FirstFreeCluster);
#if 1
	printf("FAT_Sectors         = %lu\n",disk->FAT_Sectors);
//	printf("FAT_BufAddr         = %08lX\n",disk->FAT_BufAddr);
//	printf("FAT_BufSize         = %lu(bytes)\n",disk->FAT_BufSize);
	printf("FAT_TotalBank       = %u\n",disk->FAT_TotalBank);
	printf("FAT_CurrBank        = %u\n",disk->FAT_CurrBank);
//	printf("FAT_Bank2Cluster    = %lu\n",disk->FAT_Bank2Cluster);
	//printf("FAT_bitBank2Cluster = %bu\n",disk->FAT_bitBank2Cluster);
//	printf("FAT_Bank2Sector     = %u\n",disk->FAT_Bank2Sector);
	//printf("FAT_bitBank2Sector  = %bu\n",disk->FAT_bitBank2Sector);
	printf("FAT_FirstFreeCluster= %lu(0x%lX)\n",disk->FAT_FirstFreeCluster,disk->FAT_FirstFreeCluster);
//	printf("FAT_ContFreeCluster = %bu\n",disk->FAT_ContFreeCluster);
//	printf("FAT_LocalDirtyLen   = %bu\n",disk->FAT_LocalDirtyLen);
//	printf("FAT_LocalDirtyStart = %d(0x%X)\n",disk->FAT_LocalDirtyStart,disk->FAT_LocalDirtyStart);
	//printf("MaxClusterNum       = %lu\n",disk->MaxClusterNum);
	//printf("FreeCluster         = %lu\n",disk->FreeCluster);
	printf("\n");
#endif
#if 0
	printf("DOS share=%p  size=%u\n", &dos.share, sizeof(dos.share));
	printf("     disk=%p  size=%u\n", &dos.disk, (UINT16)(sizeof(DISK))*(UINT16)MAX_DEV);
	printf("     file=%p  size=%u\n", &dos.file, (UINT16)(sizeof(FILE))*(UINT16)MAX_FILE);
	printf("\n");
#endif
}

/**************************************************************************
 *                                                                        *
 *  Function Name: DOS_FreeSpaceGet                                       *
 *                                                                        *
 *  Purposes: To get the free space of a disk.      *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    DevId: .                                     *
 *                                                                        *
 *  Returns: free KB size of a device.                           *
 *                                                                        *
 *  See also: vfsWrite(), fatRead()                                       *
 *                                                                        *
 **************************************************************************/
UINT32 DOS_TotalSize(UINT8 DevId)
{
	UINT32 size;
	xdata DISK *disk;
	disk   = (DISK *)&dos.disk[DevId];
	//printf("DevId=%bx %lx %bx\n",DevId,disk->FreeCluster,disk->bitCluster2Sector);
	size = disk->PhySectors;//disk->TotalCluster << disk->bitCluster2Sector;// to sector number		
	//printf("size=%lx\n",size);
	size = size >> 1;// to K Bytes
	return size;		
}

UINT8 dosRwErr(UINT16 fileId, UINT16 line)
{
	printf("F(%x), L(%u) RwErr\n", fileId, line);
	return 0;
}

UINT8 DOS_FileInfoGet(UINT8 fno, UINT8 mode, UINT8 *buf)
{
	UINT8 i8, j8, ret;
	pDateStc_t time;
	xdata x86_FDB *fdb;
	xdata FILE *file;
	file  = (FILE *)&dos.file[fno];
	if(file->DevId==DRIVE_NONE)
		return DOS_ERR_INVALID_FILE_HANDLE;
	fdb = (x86_FDB *)&file->fdb;

	i8 = 0;
	j8 = 0;
	ret= DOS_SUCCESS;

	if(mode==FILE_NAME) {
		while(fdb->name[j8] != ' ') {
			buf[i8++] = fdb->name[j8++];
			if(j8==8)
				buf[i8++] = '.';
			if(j8==11) break;
		}
		buf[i8] = 0;
	}
	else if(mode==FILE_ATTR) {
		buf[i8] = fdb->attr;
	}
	else if(mode==FILE_SIZE) {
		while(i8 < 4) {
			buf[i8] = fdb->size[3-i8];
			i8++;
		}
	}
	else if(mode==FILE_START) {
		buf[i8++] = fdb->starthi[1];
		buf[i8++] = fdb->starthi[0];
		buf[i8++] = fdb->start[1];
		buf[i8++] = fdb->start[0];
	}
	else if(mode==FILE_TIME) {
		time = (pDateStc_t)buf;
		time->Year   = (fdb->date[1] >> 1) - (2000-1980);
		time->Month  =((fdb->date[1] & 0x01)<< 3) + (fdb->date[0] >> 5);
		time->Day    =  fdb->date[0] & 0x1F;
		time->Hour   =  fdb->time[1] >> 3;
		time->Minute = (fdb->time[0] >> 5) + ((fdb->time[1] & 0x07) << 3);
		time->Second = (fdb->time[0] & 0x1F) << 1;
	}
	else {
		ret = DOS_ERR_INVALID_OPTION;
	}
	return ret;
}
