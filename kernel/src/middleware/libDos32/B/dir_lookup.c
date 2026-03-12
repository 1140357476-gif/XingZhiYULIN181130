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
#include "hal_dram.h"	
#include "hal_gprm.h"
#include "reg_def.h"


#define __FILE	__FILE_ID_DIR_LOOKUP__
#define	DEBUG	0
extern DOS dos;
extern UINT32 dos_var_start;

void DIR_CreateEmptyFDB(UINT8 DevId, UINT32 UpperEntry, UINT32 CurrEntry)
{
	xdata FILE *file;
	xdata x86_FDB *fdb;
	xdata DISK *disk;
	xdata UINT8  i8, *ptr8;
	UINT16 time, date;
	UINT32 sector;

	HAL_GprmDramFill(dos_var_start, 64, 0x00);	//clear FDB buffer to 0

	file = (FILE *)&dos.file[0];
	fdb = (x86_FDB *)&dos.share;
	disk = (DISK *)&dos.disk[DevId];

	//DOS_FileCreateTimeGet((pDateStc_t)&dos.date);	//already read by DIR_Lookup()
	//dos.date.Year += (2000-1980);		//RTC -> M$ time
	time = (dos.date.Hour << 11) + (dos.date.Minute << 5) + (dos.date.Second >> 1);
	date = (dos.date.Year << 9) + (dos.date.Month << 5) + (dos.date.Day);

	//curr dir entry
	i8 = 0;
	fdb->name[i8++] = '.';
	while(i8 < 11) fdb->name[i8++] = ' ';
	fdb->attr = ATTR_D;
	//fdb->lcase   = 0;	//Reserved for use by Windows NT.
	//fdb->ctimeMs = 0;		/* Creation time, milliseconds */
	fdb->ctime[0] = READ8(time,1);	/* Creation time */
	fdb->ctime[1] = READ8(time,0);
	fdb->time[0]  = READ8(time,1);
	fdb->time[1]  = READ8(time,0);
	fdb->cdate[0] = READ8(date,1);	/* Creation date */
	fdb->cdate[1] = READ8(date,0);
	fdb->adate[0] = READ8(date,1);	/* Last access date */
	fdb->adate[1] = READ8(date,0);
	fdb->date[0]  = READ8(date,1);
	fdb->date[1]  = READ8(date,0);
	fdb->starthi[1]= READ8(CurrEntry,0);	/* High 16 bits of cluster in FAT32 */
	fdb->starthi[0]= READ8(CurrEntry,1);
	fdb->start[1]  = READ8(CurrEntry,2);
	fdb->start[0]  = READ8(CurrEntry,3);
	//*((UINT32 *)fdb->size) = 0L;

	//upper dir entry
	fdb++;
	i8 = 0;
	fdb->name[i8++] = '.';
	fdb->name[i8++] = '.';
	while(i8 < 11) fdb->name[i8++] = ' ';
	fdb->attr = ATTR_D;
	//fdb->lcase   = 0;		//Reserved for use by Windows NT.
	//fdb->ctimeMs = 0;		/* Creation time, milliseconds */
	fdb->ctime[0] = READ8(time,1);	/* Creation time */
	fdb->ctime[1] = READ8(time,0);
	fdb->time[0]  = READ8(time,1);
	fdb->time[1]  = READ8(time,0);
	fdb->cdate[0] = READ8(date,1);	/* Creation date */
	fdb->cdate[1] = READ8(date,0);
	fdb->adate[0] = READ8(date,1);	/* Last access date */
	fdb->adate[1] = READ8(date,0);
	fdb->date[0]  = READ8(date,1);
	fdb->date[1]  = READ8(date,0);
	fdb->starthi[1]= READ8(UpperEntry,0);	/* High 16 bits of cluster in FAT32 */
	fdb->starthi[0]= READ8(UpperEntry,1);
	fdb->start[1]  = READ8(UpperEntry,2);
	fdb->start[0]  = READ8(UpperEntry,3);
	//*((UINT32 *)fdb->size) = 0L;
	
	HAL_GprmDramFill(dos.TmpBufAddr, disk->Cluster2Byte, 0); /* the fill size could be more than 64KB */

	ptr8 = dos.share;
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(dos.TmpBufAddr,3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(dos.TmpBufAddr,2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(dos.TmpBufAddr,1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(dos.TmpBufAddr,0);
	for(i8=0; i8<64/2; i8++) {	//copy 2 entry from previous buffer
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = *ptr8++;
		XBYTE[REG_Dram_IndirectRData_High] = *ptr8++;
	}

	sector = FAT_Cluster2Sector(DevId, CurrEntry);
	if( 0 == DOS_SectorWrite(DevId, sector, disk->Cluster2Sector, dos.TmpBufAddr)) dosRwErr(__FILE, (unsigned short)__LINE__);
}

/**
 * @brief	write curr FDB into disk
 * @param	fno, 0~7, 0 for DIR
 * @retval	1 is success, -1 is find EMPTY Entry. 0=NG
 * @see
 * @author	Ted Chiu
 * @since	08/04/20 13:30
 * @todo
 * @bug
*/
UINT8 DIR_AddFDB(UINT8 fno)
{
	//UINT8 tmp[12];
	xdata UINT8 *ptr;
	xdata FILE *file;
	xdata DISK *disk;
	xdata x86_FDB *fdb;
	UINT8  DevId;
	UINT8 i8;
	UINT16 i16;
	UINT32 i32;
	file   = (FILE *)&dos.file[fno];
	//fname  = file->fdb.name;
	DevId  = file->DevId;
	disk   = (DISK *)&dos.disk[DevId];
	fdb    = (x86_FDB *)&file->fdb;

	/* start cluster */
	i32 = file->FirstCluster;
	fdb->starthi[1] = READ8(i32,0);
	fdb->starthi[0] = READ8(i32,1);
	fdb->start[1]   = READ8(i32,2);
	fdb->start[0]   = READ8(i32,3);

	/* file size */
	i32 = file->size;
	fdb->size[0] = READ8(i32,3);
	fdb->size[1] = READ8(i32,2);
	fdb->size[2] = READ8(i32,1);
	fdb->size[3] = READ8(i32,0);
//printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
	/* date & time */
	DOS_FileCreateTimeGet((pDateStc_t)&dos.date);
	dos.date.Year += (2000-1980);	//RTC -> M$ time << 26;	//bit[31:26]
	i16 = (dos.date.Hour << 11) + (dos.date.Minute << 5) + (dos.date.Second >> 1);
	fdb->time[0]  = READ8(i16,1);
	fdb->time[1]  = READ8(i16,0);
	fdb->ctime[0] = READ8(i16,1);
	fdb->ctime[1] = READ8(i16,0);
	i16 = (dos.date.Year << 9) + (dos.date.Month << 5) + (dos.date.Day);
	fdb->date[0]  = READ8(i16,1);
	fdb->date[1]  = READ8(i16,0);
	fdb->adate[0] = READ8(i16,1);
	fdb->adate[1] = READ8(i16,0);
	fdb->cdate[0] = READ8(i16,1);
	fdb->cdate[1] = READ8(i16,0);
//printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
	if( 0 == DOS_SectorRead(DevId, file->FDB_Sector, 1, dos_var_start)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	//printf("%s %u, FDB_Sector=%lx\n", __FILE__, (unsigned short)__LINE__,file->FDB_Sector);
	i32 = ((UINT32)file->FDB_Idx << 4) + dos_var_start;	//1 FDB = 32 bytes
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(i32,3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(i32,2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(i32,1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(i32,0);
	ptr = (UINT8 *)&file->fdb;
	for(i8=0; i8<32/2; i8++) {
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = *ptr++;
		XBYTE[REG_Dram_IndirectRData_High] =	*ptr++;
	}
	if( 0 == DOS_SectorWrite(DevId, file->FDB_Sector, 1, dos_var_start)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	//printf("%s %u, FDB_Sector=%lx\n", __FILE__, (unsigned short)__LINE__,file->FDB_Sector);

	return DOS_SUCCESS;
}


/**
 * @brief	search file from curr directory
 * @param	fno, 0~7
 * @retval	1 is success, -1 is find EMPTY Entry. 0=NG
 * @see
 * @author	Ted Chiu
 * @since	08/04/20 13:30
 * @todo
 * @bug
*/
UINT8 DIR_Lookup(UINT8 fno)
{
	bit    NeedEmptyEntry;
	UINT32 cluster;
	UINT32 sector, num;
	UINT8  DevId, *fname;
	xdata DISK *disk;
	xdata FILE *file;
	xdata x86_FDB *fdb;
	xdata UINT8 *ptr8;
	UINT16 i16;
	UINT8  find;
	UINT8  LowByte, FindHit;//, HighByte;
	UINT32 FindAddr, FindNum, FindRetIdx, FindRetNum, FindCurr;

	//UINT8 tmp[12];
	UINT8 i8;
#if	DEBUG
	printf("%s %u fno=%bu\n", __FILE__, (unsigned short)__LINE__,  fno);
#endif
	file   = (FILE *)&dos.file[fno];
	fdb    = (x86_FDB *)&file->fdb;
	fname  = fdb->name;
	DevId  = file->DevId;
	disk   = (DISK *)&dos.disk[DevId];
	cluster= disk->CurrDirStartCluster;
#if	DEBUG
	printf("%s %u file=%lx %bu %s\n", __FILE__, (unsigned short)__LINE__, (UINT32)file,file->DevId,fname);
#endif
	//Check if it is root directory
	num    = (cluster < 2) ? (disk->RootEntries>>4) : disk->Cluster2Sector;
	NeedEmptyEntry = (dos.file[fno].mode=='r') ? 0 : 1;	//find empty entry for FileWrite

#if	DEBUG
	printf("DevId=%bu DIR_Lookup:\"",  DevId);
	for(i16=0; i16<11; i16++)
		printf("%c",fname[i16]);
	printf("\"\n");
#endif

	file->FDB_Sector = -1;
	find = FIND_FILE_FAIL;
	do {
		sector = FAT_Cluster2Sector(DevId, cluster);
		if( 0 == DOS_SectorRead(DevId, sector, num, dos.TmpBufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);//read a cluster
		FindNum = num << 4;	//16 FDB per sector
#if	DEBUG
		printf("%s %u cluster=%lu sector=%08lX num=%lu FindNum=%lu\n", __FILE__, (unsigned short)__LINE__, cluster,sector,num,FindNum);
#endif
		FindAddr= dos.TmpBufAddr + 2;	//start from name[4]
		FindHit = 1;
		while(FindHit && find==FIND_FILE_FAIL) {
#if	DEBUG
			printf("%s %u patt=%08lX, FindAddr=%08lX FindNum=%lu\n", __FILE__, (unsigned short)__LINE__, *((UINT32 *)(fname+4)), FindAddr,FindNum);
#endif
			//cmdMemDump("d 0x84000 +0x80");
			HAL_DramPattSearch(PATT_SEARCH_FDB, FindAddr, FindNum, *((UINT32 *)(fname+4)), \
						&FindRetIdx, &FindRetNum, &FindHit);
			if(FindHit) {
#if	DEBUG
				printf("%s %u Idx=%lu Num=%lu a=%0l8X\n", __FILE__, (unsigned short)__LINE__,FindRetIdx,FindRetNum,FindAddr);
#endif
				FindAddr += (FindRetIdx << 4) - 2;	//32 bytes per FDB
#if	DEBUG
				printf("%s %u FindAddr=%08lX\n", __FILE__, (unsigned short)__LINE__,FindAddr);
#endif
				XBYTE[REG_Dram_IndirectAddr_B0] = READ8(FindAddr,3);
				XBYTE[REG_Dram_IndirectAddr_B1] = READ8(FindAddr,2);
				XBYTE[REG_Dram_IndirectAddr_B2] = READ8(FindAddr,1);
				XBYTE[REG_Dram_IndirectAddr_B3] = READ8(FindAddr,0);
				XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
				i8 = 0;
				while(i8 < 12) {
					while(XBYTE[REG_Dram_IndirectPrefetch]);
					if(fname[i8++] != XBYTE[REG_Dram_IndirectRData_Low])
						break;
					if(fname[i8++] != XBYTE[REG_Dram_IndirectRData_High])
						break;
				}
#if	DEBUG
				printf("%s %u i8=%bu %02bX %02bX %02bX\n", __FILE__, (unsigned short)__LINE__,i8, fname[i8-1],XBYTE[REG_Dram_IndirectRData_Low],XBYTE[REG_Dram_IndirectRData_High]);
#endif
				if(i8 >= 11)	{
					FindAddr    -= dos.TmpBufAddr;
					file->FDB_Sector = FindAddr >> 8;	//addr -> sector
					file->FDB_Sector+= sector;
					file->FDB_Idx    = (FindAddr & (256-1)) >> 4;
					FindAddr    += dos.TmpBufAddr;

					FindAddr += 10/2;
					ptr8 = (UINT8 *)fdb + 10;
#if	DEBUG
					printf("%s %u ptr=%p\n", __FILE__, (unsigned short)__LINE__,ptr8);
#endif
					XBYTE[REG_Dram_IndirectAddr_B0] = READ8(FindAddr,3);
					XBYTE[REG_Dram_IndirectAddr_B1] = READ8(FindAddr,2);
					XBYTE[REG_Dram_IndirectAddr_B2] = READ8(FindAddr,1);
					XBYTE[REG_Dram_IndirectAddr_B3] = READ8(FindAddr,0);
					XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
					for(i8=10/2; i8<32/2; i8++) {	//skip first 10 bytes(fname)
						while(XBYTE[REG_Dram_IndirectPrefetch]);
						*ptr8++ = XBYTE[REG_Dram_IndirectRData_Low];
						*ptr8++ = XBYTE[REG_Dram_IndirectRData_High];
						//printf("%s %u ptr=%p\n", __FILE__, (unsigned short)__LINE__,ptr8);
					}
					find = FIND_FILE_OK;
					//printf("<FIND>");
				}
				if(find==FIND_FILE_FAIL && (FindRetNum >1)) {	//meet fname[4-7] but not all characters
					FindRetIdx++;	//skip to next
					FindNum -= FindRetIdx;
					//FindAddr+= FindRetIdx << 4;	//+32*First bytes
					FindAddr += (32+4)/2;	//+32*First bytes
					#if	DEBUG
					printf("%s %u Idx=%lu Num=%lu a=%0l8X\n", __FILE__, (unsigned short)__LINE__,FindRetIdx,FindNum,FindAddr);
					#endif
				}
			}
		}

		if(find==FIND_FILE_FAIL && NeedEmptyEntry) {	//not found, record first empty pos for wrt
			FindAddr = dos.TmpBufAddr;
			#if	DEBUG
			printf("%s %u FindAddr=%lx\n", __FILE__, (unsigned short)__LINE__,FindAddr);
			#endif
			for(i16=0; i16<FindNum; i16++) {
				XBYTE[REG_Dram_IndirectAddr_B0] = READ8(FindAddr,3);
				XBYTE[REG_Dram_IndirectAddr_B1] = READ8(FindAddr,2);
				XBYTE[REG_Dram_IndirectAddr_B2] = READ8(FindAddr,1);
				XBYTE[REG_Dram_IndirectAddr_B3] = READ8(FindAddr,0);
				XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
				while(XBYTE[REG_Dram_IndirectPrefetch]);
				LowByte  = XBYTE[REG_Dram_IndirectRData_Low];
				//HighByte = XBYTE[REG_Dram_IndirectRData_High];
				//Here search available FDB for new created file
				if(LowByte==0xE5 || LowByte==0x00) {
					FindAddr    -= dos.TmpBufAddr;
					file->FDB_Sector = FindAddr >> 8;	//addr -> sector
					file->FDB_Sector+= sector;
					file->FDB_Idx    = (FindAddr & (256-1)) >> 4;
					//printf("Empty s=%08lX i=%bu\n", file->FDB_Sector,file->FDB_Idx);
					NeedEmptyEntry = 0;//found an entry, do not need to create entry
					find = FIND_FILE_EMPTY;
					break;
				}
				FindAddr += 32/2;
			}
		}
		if((cluster > disk->MaxClusterNum-1) || (cluster < 2)){
			FindRetNum =  EOF_FAT32;
		}
		else {
			FindRetNum = FAT_TableRead(DevId, cluster);	//ROOT, return EOF_FAT32
		}
		#if	DEBUG
		printf("%s %u cluster=%lu,FindRetNum=%lx\n", __FILE__, (unsigned short)__LINE__,cluster,FindRetNum);
		#endif
		//if the cluster == FAT_EOFValue, means no more FDB at other cluster
		if((FindRetNum & disk->FAT_EOFValue) == disk->FAT_EOFValue){
			#if	DEBUG
			printf("%s %u FindRetNum=%lx,FAT_EOFValue=%lx\n", __FILE__, (unsigned short)__LINE__,FindRetNum,disk->FAT_EOFValue);
			#endif
			break;
		}
		//if the cluster != FAT_EOFValue, go to next cluster to search new FDB
		cluster = FindRetNum;
		#if	DEBUG
		printf("%s %u cluster=%lu,FindRetNum=%lx\n", __FILE__, (unsigned short)__LINE__,cluster,FindRetNum);
		#endif
	} while(find==FIND_FILE_FAIL);

	#if	DEBUG
	printf("%s %u find=%bx,cluster=%lx\n", __FILE__, (unsigned short)__LINE__,find,cluster);
	#endif
	if(find==FIND_FILE_FAIL) {
		if(NeedEmptyEntry && (disk->FsType==32 || cluster)) {
			FindCurr = FAT_FreeClusterGet(DevId, 1, &FindRetIdx);
			#if	DEBUG
			printf("%s %u FindCurr=%lx,FindRetIdx=%lx\n", __FILE__, (unsigned short)__LINE__,FindCurr,FindRetIdx);
			#endif
			//Search a new cluster for next cluster size/32 FDB
			if(FindCurr) {
				FAT_TableWrite(DevId,cluster,FindRetIdx);
				FAT_TableWrite(DevId,FindRetIdx,EOF_FAT32);
				//Fill all FDB area as data 0
				//DIR_CreateEmptyFDB(DevId, cluster, FindRetIdx);
				//HAL_GprmDramFill(dos.TmpBufAddr, disk->Cluster2Byte, 0x00);	//clear FDB buffer to 0
				HAL_GprmDramFill(dos.TmpBufAddr, disk->Cluster2Byte, 0); /* the fill size could be more than 64KB */

				sector = FAT_Cluster2Sector(DevId, FindRetIdx);
				if( 0 == DOS_SectorWrite(DevId, sector, disk->Cluster2Sector, dos.TmpBufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
				cluster = FindRetIdx;
				file->FDB_Sector = FAT_Cluster2Sector(DevId, cluster);
				file->FDB_Idx    = 0;
				find = FIND_FILE_EMPTY;
				#if	DEBUG
				printf("%s %u FDB_Sector=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,file->FDB_Sector,cluster);
				#endif
			}
		}
	}
	if(find == FIND_FILE_EMPTY) {
		if(fno) {	//file operation
			fdb->attr = ATTR_A;
			FindRetIdx = 0;
			#if	DEBUG
			printf("%s %u FDB_Sector=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,file->FDB_Sector,cluster);
			#endif
		}
		else {	//dir operation, allocate 1 cluster for empty body
			#if	DEBUG
			printf("%s %u FDB_Sector=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,file->FDB_Sector,cluster);
			#endif
			fdb->attr = ATTR_D;
			FindCurr = FAT_FreeClusterGet(DevId, 1, &FindRetIdx);
			if(!FindCurr) {
				return dosRwErr(__FILE,(unsigned short)__LINE__);
			}
			FAT_TableWrite(DevId,FindRetIdx,EOF_FAT32);
		}
		//Open a new file, the First Cluster is 0
		file->FirstCluster = FindRetIdx;
		file->size         = 0;
		#if	DEBUG
		printf("%s %u FDB_Sector=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,file->FDB_Sector,cluster);
		#endif
		DIR_AddFDB(fno);
		if(fno==0){//This is for Directory FDB create
			if (disk->FsType == 32) {
				if (cluster <= 2) cluster = 0;
			}
			DIR_CreateEmptyFDB(DevId, cluster, FindRetIdx);
		}
		FAT_TableWriteBack(DevId);
	}
	//printf("find=%bd s=%08lX i=%bu\n",find,file->FDB_Sector,file->FDB_Idx);
	return find;
}


void DIR_SearchReset(void)
{
	dos.FDB_DataValid  = 0;
	dos.FDB_CurrCluster= -1;
	dos.FDB_CurrIdx    = -1;
}


/**
 * @brief	search file from curr directory
 * @param	DevId, drive ID
 * @param	TarName, name for search, 8.3 format(total 11 bytes). '?' & '*' are valid input
 * @param	RetFdb, return buffer for file info
 * @param	option, FILE_INFO_GET_SIZE, FILE_INFO_GET_TIME
 * @retval	1 is success, -1 is find EMPTY Entry. 0=NG
 * @see
 * @author	Ted Chiu
 * @since	08/04/20 13:30
 * @todo
 * @bug
*/
UINT8 DIR_Search(UINT8 DevId, UINT8 *TarName, FDB *RetFdb, UINT8 option)
{
#if	DEBUG
	UINT16 i16;
#endif
	UINT32 cluster;
	UINT16 SectorNum;
	UINT32 SectorStart;
	xdata UINT8 *ptr8;
	xdata x86_FDB *fdb;
	xdata DISK *disk;
	UINT8  find, ByteL, ByteH, end;
	UINT32 FindAddr, FindNum;//, FindRetIdx, FindRetNum;

	UINT8 i8;
	disk      = (DISK *)&dos.disk[DevId];
	cluster   = dos.FDB_DataValid ? dos.FDB_CurrCluster : disk->CurrDirStartCluster;
	SectorNum = (cluster < 2) ? (disk->RootEntries>>4) : disk->Cluster2Sector;

#if	0
	printf("DevId=%bu DIR_Lookup:\"",  DevId);
	for(i16=0; i16<11; i16++)
		printf("%c",TarName[i16]);
	printf("\"\n");
#endif

	end  = 0;
	find = FIND_FILE_FAIL;
	#if	DEBUG
	printf("%s %u Sector=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,SectorStart,cluster);
	#endif
	do {
		SectorStart = FAT_Cluster2Sector(DevId, cluster);
		FindNum = SectorNum << 4;	//16 FDB per sector
		FindAddr= dos.TmpBufAddr;	//start from name[0]
		if(dos.FDB_DataValid) {
			dos.FDB_CurrIdx++;
			FindNum  -= dos.FDB_CurrIdx;
			FindAddr += dos.FDB_CurrIdx << 4;	//16 FDB per sector
		}
		else {
			#if	DEBUG
			printf("%s %u %bd,%lu,%lx,%lx\n", __FILE__, (unsigned short)__LINE__,SectorStart,SectorNum,dos.TmpBufAddr);
			#endif
			if( 0 == DOS_SectorRead(DevId, SectorStart, SectorNum, dos.TmpBufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
		}
		dos.FDB_DataValid = 1;
		while(FindNum--) {
			XBYTE[REG_Dram_IndirectAddr_B0] = READ8(FindAddr,3);
			XBYTE[REG_Dram_IndirectAddr_B1] = READ8(FindAddr,2);
			XBYTE[REG_Dram_IndirectAddr_B2] = READ8(FindAddr,1);
			XBYTE[REG_Dram_IndirectAddr_B3] = READ8(FindAddr,0);
			XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			ByteL = XBYTE[REG_Dram_IndirectRData_Low];
			ByteH = XBYTE[REG_Dram_IndirectRData_High];
			if(!ByteL) {	//empty
				FindNum = 0;
				break;
			}
			if(ByteL != 0xE5) {	//erased
				i8 = 0;
				while(1) {
					if(TarName[i8] == '*') {
						find = FIND_FILE_OK;
						break;
					}
					if((TarName[i8] != '?') && (TarName[i8] != ByteL))
						break;
					i8++;
					if(i8==11) {
						find = FIND_FILE_OK;
						break;
					}
					if((TarName[i8] != '?') && (TarName[i8] != ByteH))
						break;
					i8++;
					while(XBYTE[REG_Dram_IndirectPrefetch]);
					ByteL = XBYTE[REG_Dram_IndirectRData_Low];
					ByteH = XBYTE[REG_Dram_IndirectRData_High];
				}
			}
			if(find == FIND_FILE_OK) {
				XBYTE[REG_Dram_IndirectAddr_B0] = READ8(FindAddr,3);
				XBYTE[REG_Dram_IndirectAddr_B1] = READ8(FindAddr,2);
				XBYTE[REG_Dram_IndirectAddr_B2] = READ8(FindAddr,1);
				XBYTE[REG_Dram_IndirectAddr_B3] = READ8(FindAddr,0);
				XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
				fdb  = (x86_FDB *)&dos.fdb;
				ptr8 = (UINT8 *)&dos.fdb;
				for(i8=0; i8<32/2; i8++) {
					while(XBYTE[REG_Dram_IndirectPrefetch]);
					*ptr8++ = XBYTE[REG_Dram_IndirectRData_Low];
					*ptr8++ = XBYTE[REG_Dram_IndirectRData_High];
				}
			}
			if((fdb->attr & ATTR_LFN)==ATTR_LFN) {
				find = FIND_FILE_FAIL;	//LFN
			}
			if(find == FIND_FILE_OK) {
				RetFdb->DevId = DevId;
				for(i8=0; i8<11; i8++) {
					RetFdb->name[i8] = fdb->name[i8];
				}
				RetFdb->name[i8] = 0;
				RetFdb->attr = fdb->attr;
				if(option & FILE_INFO_GET_TIME) {
					RetFdb->Year   = (fdb->date[1] >> 1) - (2000-1980);
					RetFdb->Month  =((fdb->date[1] & 0x01)<< 3) + (fdb->date[0] >> 5);
					RetFdb->Day    =  fdb->date[0] & 0x1F;
					RetFdb->Hour   =  fdb->time[1] >> 3;
					RetFdb->Minute = (fdb->time[0] >> 5) + ((fdb->time[1] & 0x07) << 3);
					RetFdb->Second = (fdb->time[0] & 0x1F) << 1;
				}
				if(option & FILE_INFO_GET_SIZE) {
					WRITE8(RetFdb->size,0, fdb->size[3]);
					WRITE8(RetFdb->size,1, fdb->size[2]);
					WRITE8(RetFdb->size,2, fdb->size[1]);
					WRITE8(RetFdb->size,3, fdb->size[0]);
				}
				WRITE8(RetFdb->start,0, fdb->starthi[1]);
				WRITE8(RetFdb->start,1, fdb->starthi[0]);
				WRITE8(RetFdb->start,2, fdb->start[1]);
				WRITE8(RetFdb->start,3, fdb->start[0]);
				FindAddr    -= dos.TmpBufAddr;
				RetFdb->FDB_Sector = FindAddr >> 8;	//addr -> sector
				RetFdb->FDB_Sector+= SectorStart;
				RetFdb->FDB_Idx    = (FindAddr & (256-1)) >> 4;
				dos.FDB_CurrCluster = cluster;
				dos.FDB_CurrIdx     = FindAddr >> 4;
				//printf("c=%-8lu i=%-5u ",dos.FDB_CurrCluster,dos.FDB_CurrIdx);
				break;
			}
			FindAddr += 32/2;
		}
		if(find == FIND_FILE_OK) // found 1 file
			break;
		if((cluster > disk->MaxClusterNum-1) || (cluster < 2)){
			cluster =  EOF_FAT32;
		}
		else {
			cluster = FAT_TableRead(DevId, cluster);	//ROOT, return EOF_FAT32
		}
		dos.FDB_DataValid = 0;
		#if	DEBUG
		printf("%s %u %lx\n", __FILE__, (unsigned short)__LINE__,cluster);
		#endif
		if((cluster & disk->FAT_EOFValue) == disk->FAT_EOFValue)// FDB End of this Directory
			break;
		dos.FDB_CurrCluster = cluster;
		dos.FDB_CurrIdx     = 0;
		#if	DEBUG
		printf("%s %u %lx\n", __FILE__, (unsigned short)__LINE__,cluster);
		#endif
	} while(find==FIND_FILE_FAIL);

	return find;
}
