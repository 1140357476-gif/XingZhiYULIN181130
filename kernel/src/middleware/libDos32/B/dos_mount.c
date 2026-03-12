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
#include "initio.h"
#include "dramfunc.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "dbg_mem.h"
#include "reg_def.h"


#define __FILE	__FILE_ID_DOS_MOUNT__
#define	DEBUG	0

void cmdMemDump(UINT8 *param);

extern UINT32 dos_var_start;

DOS dos _at_ 0x8000;

/**
 * @brief	mount a new driver to DOS system
 * @param	DevId, device ID, 0=SD, 1=NAND, 2=RAM
 * @param	TotalSectors, device capacity by sectors (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
//test OK!
UINT8 DOS_Mount(UINT8 DevId, UINT32 TotalSectors)
{
	x86_MBS *mbs;
	x86_PBS *pbs;
	DISK *disk;
	x86_PartitionTable *partition;
	UINT8 i8, j8, mode;
	UINT32 BufSize;
	UINT32 MaxCluster, MaxSector;
	UINT32 total, first;
	UINT32 addr;
	UINT8  hit;
	UINT16 i16;
    UINT32 size;

#if	DEBUG
	printf("%s %u sizeof(dos)=%u\n", __FILE__, (unsigned short)__LINE__, sizeof(dos));
#endif
	i8 = DOS_DevPreCheck(DevId);
	if(i8 != DOS_ERR_NONE){
		__ASSERT(__FILE, __LINE__, 1);
		return i8;
	}
	disk = (DISK *)&dos.disk[DevId];
	pdiskFAT_BankStartCluster = &disk->FAT_BankStartCluster;
	pdiskFAT_Bank2Cluster = &disk->FAT_Bank2Cluster;
	disk->PhySectors = TotalSectors;
	// read PBS or MBS
	addr = 0;
	if( 0 == DOS_SectorRead(DevId, addr, 1, dos_var_start)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	//cmdMemDump("0x8000 +512");
	#if 0
	dumpData(512, dos_var_start, 1);
	#endif
/*
	0x04, 16-bit FAT primary partition or logical drive.
			The number of sectors is between 32680 and 65535.
 	0x05, Extended partition.
 	0x06, BIGDOS FAT primary partition or logical drive.
	 * 0x08: FAT32
	0x0B, Primary Fat32 partition, using interrupt 13 (INT 13) extensions.
	0x0C, Extended Fat32 partition, using INT 13 extensions.
	 * 0x0d: LBA mode (FAT16)
	0x0E, Extended Fat16 partition, using INT 13 extensions.
	0x0F, Primary Fat16 partition, using INT 13 extensions.
*/
	mbs = (x86_MBS *)&dos.share;
	pbs = (x86_PBS *)&dos.share;
	#if	DEBUG
	printf("mbs->Signature[0]=%bx mbs->Signature[0]=%bx\n",mbs->Signature[0],mbs->Signature[1]);
	#endif
	if(mbs->Signature[0] != 0x55 || mbs->Signature[1] != 0xAA){
		__ASSERT(__FILE, __LINE__, 1);
		return DOS_ERR_INVALID_MBS;
	}
	#if 0
	partition = &mbs->partition1;
	for(i8=0; i8<4; i8++,partition++) {
		#if	1
		printf("partition[%bu]=%p SysId=%02bX, BootId=%02bX\n", \
			i8, partition, partition->SysId,partition->BootId);
		#endif
		switch(partition->SysId)
		{
			case 0x01:	//FAT12 primary partition or logical drive (fewer than 32,680 sectors in the volume)
			case 0x04:	//FAT16 partition or logical drive (32,680¡V65,535 sectors or 16 MB¡V33 MB)
			case 0x05:	//MS-DOS extended
			case 0x06:	//BIGDOS FAT16 partition or logical drive (33 MB¡V4 GB)
			case 0x08: 	//FAT32
			case 0x0B:	//FAT32 partition or logical drive
			case 0x0C:	//FAT32 partition or logical drive using BIOS INT 13h extensions
			case 0x0D:	//LBA mode (FAT16)
			case 0x0E:	//BIGDOS FAT16 partition or logical drive using BIOS INT 13h extensions
			case 0x0F:	//Extended partition using BIOS INT 13h extensions
			case 0x82:      //Linux swap
			case 0x83:      //Linux native
			case 0x85:      //Linux extended
				//eric mark off, some system do not apply this
				WRITE8(addr, 0, partition->StartLogSector[3]);
				WRITE8(addr, 1, partition->StartLogSector[2]);
				WRITE8(addr, 2, partition->StartLogSector[1]);
				WRITE8(addr, 3, partition->StartLogSector[0]);
				if((partition->BootId==0) || (partition->BootId == 0x80)) {
					#if	1
					printf("addr=%08lX\n", addr);
					#endif
					if(addr < TotalSectors) {// error handle
						if( 0 == DOS_SectorRead(DevId,addr,1,dos_var_start+(512/2))) return dosRwErr(__FILE,(unsigned short)__LINE__);
						#if	DEBUG
						dumpData(512, (dos_var_start+(512/2)), 1);
						#endif
						// second sector is pbs, sector 0 is mbs
						pbs = (x86_PBS *)(&dos.share+512);
						//cmdMemDump("0x8200 +512");
						if((pbs->Signature[0] != 0x55) || (pbs->Signature[1] != 0xAA)) {
							pbs = (x86_PBS *)&dos.share;
							//addr = 0;
							// error, pbs is sector 0
						}
					}
					else {
						__ASSERT(__FILE, __LINE__, 1);
						return DOS_ERR_INVALID_MBS;
					}
				}
				else{
					#if	1
					printf("addr=%08lX\n", addr);
					#endif
					addr = 0;
				}
				break;
			default:
				break;
		}
		if(addr != 0)
			break;
	}
	if(i8==4) {
		__ASSERT(__FILE, __LINE__, 1);
		if( 0 == DOS_SectorRead(DevId,addr,1,dos_var_start+(512/2))) return dosRwErr(__FILE,(unsigned short)__LINE__);
		#if	DEBUG
		dumpData(512, (dos_var_start+(512/2)), 1);
		#endif
		// second sector is pbs, sector 0 is mbs
		pbs = (x86_PBS *)(&dos.share+512);
		//cmdMemDump("0x8200 +512");
		if((pbs->Signature[0] != 0x55) || (pbs->Signature[1] != 0xAA)) {
			pbs = (x86_PBS *)&dos.share;
			//addr = 0;
			// error, pbs is sector 0
		}
		//return DOS_ERR_INVALID_MBS;
	}
	#else
	/*
	case 0x01:	//FAT12 primary partition or logical drive (fewer than 32,680 sectors in the volume)
	case 0x04:	//FAT16 partition or logical drive (32,680¡V65,535 sectors or 16 MB¡V33 MB)
	case 0x05:	//MS-DOS extended
	case 0x06:	//BIGDOS FAT16 partition or logical drive (33 MB¡V4 GB)
	case 0x08: 	//FAT32
	case 0x0B:	//FAT32 partition or logical drive
	case 0x0C:	//FAT32 partition or logical drive using BIOS INT 13h extensions
	case 0x0D:	//LBA mode (FAT16)
	case 0x0E:	//BIGDOS FAT16 partition or logical drive using BIOS INT 13h extensions
	case 0x0F:	//Extended partition using BIOS INT 13h extensions
	case 0x82:      //Linux swap
	case 0x83:      //Linux native
	case 0x85:      //Linux extended
	*/
	partition = &mbs->partition1;
	#if	DEBUG
	printf("partition[%bu]=%p SysId=%02bX, BootId=%02bX\n", \
		i8, partition, partition->SysId,partition->BootId);
	#endif
	for ( i8 = 0; i8 < 4; i8++ ) {
		if ( partition->SysId == 0 ) {
			partition ++;
		}
		else {
			#if	DEBUG
			printf("addr=%08lX\n", addr);
			#endif
			break;
		}
	}
	if(i8==4) {
		__ASSERT(__FILE, __LINE__, 1);
		/* phil mark for the situation that there's no MBS but PBS only */
		//return DOS_ERR_INVALID_MBS;
	}

	WRITE8(size, 0, partition->PartitionSize[3]);
	WRITE8(size, 1, partition->PartitionSize[2]);
	WRITE8(size, 2, partition->PartitionSize[1]);
	WRITE8(size, 3, partition->PartitionSize[0]);

    if (size > TotalSectors) {
        ASSERT(0, 1);
        /* the MSB may be error, but we still have to check the active partition is OK not not */
        //return DOS_ERR_INVALID_MBS;
    }
    
	//eric mark off, some system do not apply this
	WRITE8(addr, 0, partition->StartLogSector[3]);
	WRITE8(addr, 1, partition->StartLogSector[2]);
	WRITE8(addr, 2, partition->StartLogSector[1]);
	WRITE8(addr, 3, partition->StartLogSector[0]);
	#if	DEBUG
	printf("addr=%08lX\n", addr);
	#endif
	if(addr > TotalSectors) {// error handle
		addr = 0;//Assume sector 0 is not MBS but PBS
	}
	else{
		//To get PBS data
		if( 0 == DOS_SectorRead(DevId,addr,1,dos_var_start+(512/2))) return dosRwErr(__FILE,(unsigned short)__LINE__);
		#if	DEBUG
		dumpData(512, (dos_var_start+(512/2)), 1);
		#endif
		// second sector is pbs, sector 0 is mbs
		pbs = (x86_PBS *)(&dos.share+512);
		if((pbs->Signature[0] != 0x55) || (pbs->Signature[1] != 0xAA)) {
			__ASSERT(__FILE, __LINE__, 1);
			return DOS_ERR_INVALID_PBS;
		}
	}
	#endif

	if((pbs->fats != 2) && (DevId != DRIVE_DRAM)){
		__ASSERT(__FILE, __LINE__, 1);
		return DOS_ERR_INVALID_FAT_COPIES_NUM;
	}
	#if	DEBUG
	printf("pbs->fats=%bx\n",pbs->fats);
	#endif
	disk->FAT_CopyNum = pbs->fats;
	if((pbs->sectSize[1] != 2) || (pbs->sectSize[0] != 0)){
		__ASSERT(__FILE, __LINE__, 1);
		return DOS_ERR_INVALID_SECTOR_SIZE;
	}
	#if	DEBUG
	printf("pbs->sectSize[0]=%bx,pbs->sectSize[1]=%bx,,pbs->sectSize[2]=%bx\n",pbs->sectSize[0],pbs->sectSize[1],pbs->sectSize[2]);
	#endif
	for(i8=0,j8=0; i8<8; i8++) {
		if(pbs->sectPerClust & (1<<i8)) {
			disk->bitCluster2Sector = i8;
			j8++;
		}
	}
	if(j8 != 1) {
		__ASSERT(__FILE, __LINE__, 1);
		return DOS_ERR_INVALID_CLUSTER_SIZE;
	}

	#if	DEBUG
	printf("j8=%bx\n",j8);
	#endif
	disk->Cluster2Sector = pbs->sectPerClust;
	disk->Cluster2Byte   = (UINT32)disk->Cluster2Sector << 9;
	disk->bitCluster2Byte= disk->bitCluster2Sector + 9;
	disk->Cluster2Fdb    = disk->Cluster2Byte >> 5;
	#if	DEBUG
	printf("Cluster2Byte=%lx\n",disk->Cluster2Byte);
	#endif

	WRITE8(disk->HiddenSectors,0,pbs->hidden[3]);
	WRITE8(disk->HiddenSectors,1,pbs->hidden[2]);
	WRITE8(disk->HiddenSectors,2,pbs->hidden[1]);
	WRITE8(disk->HiddenSectors,3,pbs->hidden[0]);
	WRITE8(disk->FAT_StartSector,0,0);
	WRITE8(disk->FAT_StartSector,1,0);
	WRITE8(disk->FAT_StartSector,2,pbs->reserved[1]);
	WRITE8(disk->FAT_StartSector,3,pbs->reserved[0]);
	if (disk->HiddenSectors) {
		disk->FAT_StartSector += disk->HiddenSectors;
	} 
	else {
		disk->FAT_StartSector += addr;
		//printf("fixed sector : %lu\n", disk->FAT_StartSector);
	}

	disk->FAT_BufSize = FAT_BUF_SIZE_SD*2; //xian ++ 20100919 default value, #40742 
	if(pbs->fatLen[0] || pbs->fatLen[1]) {	//FAT12/16
		if(strncmp(pbs->u.fat16only.fsName,"FAT16   ", 8) == 0){
			disk->FsShift = 0;
			disk->FsType = 16;
			disk->FAT_EOFValue = EOF_FAT16;
		}
		else if(strncmp(pbs->u.fat16only.fsName,"FAT12   ", 8) == 0) {
			disk->FsShift = 0;
			disk->FsType = 12;
			disk->FAT_EOFValue = EOF_FAT12;
			//This is try to let FAt buffer size is FAT alignment, especially FAT12
			disk->FAT_BufSize = (disk->FAT_BufSize / disk->FsType) * disk->FsType;
		}
		else{
			__ASSERT(__FILE, __LINE__, 1);
			return DOS_ERR_INVALID_FAT16;
		}

		#if	DEBUG
		printf("fsName=%s\n",pbs->u.fat16only.fsName);
		#endif
		WRITE8(disk->FAT_Sectors,0,0);
		WRITE8(disk->FAT_Sectors,1,0);
		WRITE8(disk->FAT_Sectors,2,pbs->fatLen[1]);
		WRITE8(disk->FAT_Sectors,3,pbs->fatLen[0]);

		WRITE8(disk->RootEntries,1,pbs->dirEntries[0]);
		WRITE8(disk->RootEntries,0,pbs->dirEntries[1]);

		//disk->FsShift = 0;
		//disk->FsType = 16;
		disk->RootStartCluster= 0;
		disk->RootStartSector = disk->FAT_StartSector + (disk->FAT_Sectors<<1);	//2 copies of FAT
		i8 = (disk->RootEntries + 15) >> 4;
		disk->DataStartSector = disk->RootStartSector + i8;
		strncpy(disk->Name, pbs->u.fat16only.volLabel, 11);
		//disk->FAT_EOFValue = EOF_FAT16;
	}
	else {
		if(strncmp(pbs->u.fat32only.fsName,"FAT32   ", 8) != 0) {
			//pbs->u.fat32only.fsName[8] = 0;
			//printf("%s %d FS=%s\n", __FILE__, __LINE__,pbs->u.fat32only.fsName);
			__ASSERT(__FILE, __LINE__, 1);
			return DOS_ERR_INVALID_FAT32;
		}
		#if	DEBUG
		printf("fsName=%s\n",pbs->u.fat32only.fsName);
		#endif
		if(pbs->dirEntries[0] || pbs->dirEntries[1]){
			__ASSERT(__FILE, __LINE__, 1);
			return DOS_ERR_INVALID_ROOT_NUM;
		}

		WRITE8(disk->FAT_Sectors,0,pbs->u.fat32only.fat32Length[3]);
		WRITE8(disk->FAT_Sectors,1,pbs->u.fat32only.fat32Length[2]);
		WRITE8(disk->FAT_Sectors,2,pbs->u.fat32only.fat32Length[1]);
		WRITE8(disk->FAT_Sectors,3,pbs->u.fat32only.fat32Length[0]);

		disk->FsShift = 1;
		disk->FsType = 32;
		/* For FAT32, there is no limitation of root dir entries. */

 		disk->RootEntries = disk[DevId].Cluster2Fdb;
		WRITE8(disk->RootStartCluster,0, pbs->u.fat32only.rootClust[3]);
		WRITE8(disk->RootStartCluster,1, pbs->u.fat32only.rootClust[2]);
		WRITE8(disk->RootStartCluster,2, pbs->u.fat32only.rootClust[1]);
		WRITE8(disk->RootStartCluster,3, pbs->u.fat32only.rootClust[0]);

		disk->DataStartSector = disk->FAT_StartSector + (disk->FAT_Sectors<<1);
		disk->RootStartSector = disk->DataStartSector + \
				((disk->RootStartCluster - 2) << disk->bitCluster2Sector);
		strncpy(disk->Name, pbs->u.fat32only.volLabel, 11);
		disk->FAT_EOFValue = EOF_FAT32;
	}
	disk->CurrDirStartCluster = disk->RootStartCluster;	//default to ROOT
	disk->FDB_CurrCluster = -1;	//invalid
	if(pbs->sects[0] || pbs->sects[1]) {
		WRITE8(disk->LogSectors,0,0);
		WRITE8(disk->LogSectors,1,0);
		WRITE8(disk->LogSectors,2,pbs->sects[1]);
		WRITE8(disk->LogSectors,3,pbs->sects[0]);
	}
	else {
		WRITE8(disk->LogSectors,0,pbs->totalSects[3]);
		WRITE8(disk->LogSectors,1,pbs->totalSects[2]);
		WRITE8(disk->LogSectors,2,pbs->totalSects[1]);
		WRITE8(disk->LogSectors,3,pbs->totalSects[0]);
	}
	#if	DEBUG
	printf("Log=%lx,%lx,%lx\n",disk->LogSectors,disk->DataStartSector,disk->HiddenSectors);
	#endif
	disk->TotalCluster = (disk->LogSectors - (disk->DataStartSector-disk->HiddenSectors)) >> disk->bitCluster2Sector;
	disk->MaxClusterNum= disk->TotalCluster + 2;
	#if 0
	if(disk->TotalCluster < 4085) {
		printf("TotalCluster=%lu, FAT12?\n",disk->TotalCluster);
		//return DOS_ERR_INVALID_FAT;
	}
	#endif
	if(disk->TotalCluster < 65525 && disk->FsType==32) {
		printf("TotalCluster=%lu, FAT16? FAT32?\n",disk->TotalCluster);
		__ASSERT(__FILE, __LINE__, 1);
		return DOS_ERR_INVALID_FAT;
	}

#if 0//( DOSRAM_OPTION == 1 )
	if( DevId == DRIVE_DRAM ){
		disk->FAT_BufSize = (disk->FAT_Sectors << 9);// drive 2 ( DRAM ) FAT buffer
		disk->FAT_BufAddr  = (DRAM_StartAddrGet() >> 1) + (disk->FAT_StartSector << 8);
	}
#endif
	#if	DEBUG
	printf("TotalCluster=%lx\n",disk->TotalCluster);
	#endif
	BufSize = disk->FAT_Sectors << 9;
	disk->FAT_TotalBank    = (BufSize + disk->FAT_BufSize-1) / disk->FAT_BufSize;
	if( disk->FsType == 12 ){
		//printf("FAT_TotalBank=%x,%lx,%lx\n",disk->FAT_TotalBank,BufSize,disk->FAT_BufSize);
		disk->FAT_Bank2Cluster = ( disk->FAT_BufSize / disk->FsType ) * 8;
	}
	else{
		disk->FAT_Bank2Cluster = (disk->FAT_BufSize / (disk->FsType / 8) );
	}

	for(i8=0; i8<32; i8++) {
		if(disk->FAT_Bank2Cluster & (1L << i8))
			break;
	}
	disk->FAT_bitBank2Cluster = i8;
	disk->FAT_Bank2Sector = disk->FAT_BufSize >> 9;
	for(i8=0; i8<32; i8++) {
		if(disk->FAT_Bank2Sector & (1L << i8))
			break;
	}
	disk->FAT_bitBank2Sector  = i8;

	disk->FAT_FirstFreeCluster= 0;
	disk->FAT_BankStartCluster= 0;
	disk->FAT_LocalDirtyLen   = 0;
	disk->FAT_LocalDirtyStart =-1;
	disk->FreeCluster  = 0;
	disk->FAT_CurrBank = 0;

	#if 0
	if(disk->FsType == 12) {
		MaxCluster = disk->TotalCluster + 2;
		MaxSector  = disk->FAT_Sectors;
		//printf("TotalCluster=%lu,=%lu,=%lu\n",disk->TotalCluster,disk->MaxClusterNum,MaxCluster);
		//printf("FAT_Bank2Cluster=%lu\n",disk->FAT_Bank2Cluster);
	}
	else
	#endif
	{
		if(disk->MaxClusterNum <= disk->FAT_Bank2Cluster ){
			MaxCluster = disk->MaxClusterNum;
		}
		else{
			MaxCluster = disk->MaxClusterNum % (disk->FAT_Bank2Cluster);
		}
		if(disk->FAT_Sectors <= disk->FAT_Bank2Sector ){
			MaxSector  = disk->FAT_Sectors;
		}
		else{
			MaxSector  = disk->FAT_Sectors % (disk->FAT_Bank2Sector);
		}
	}
//	printf("MaxSector=%lx,%lx,%x,%lx\n",MaxSector,disk->FAT_Sectors,disk->FAT_Bank2Sector,disk->FAT_BufSize);
	if(disk->FsType == 32) {
		mode = PATT_SEARCH_FAT32_FREE_TOTAL;
		#if	SEARCH_8N_16N_PLUS_1==0
		if((MaxCluster & 0x07)==1)	//8N+1 for FAT32
			MaxCluster--;	//IC's bug
		#endif
	}
	else if(disk->FsType == 16) {
		mode = PATT_SEARCH_FAT16_FREE_TOTAL;
		#if	SEARCH_8N_16N_PLUS_1==0
		if((MaxCluster & 0x0F)==1)	//16N+1 for FAT16
			MaxCluster--;	//IC's bug
		#endif
	}
	else {
	}

	/* Read the FAT table & calculate the free space, use HW pattern search */
	i16 = disk->FAT_TotalBank;
	#if	DEBUG
	printf("FAT_StartSector=%lx,FAT_TotalBank=%x\n",disk->FAT_StartSector,disk->FAT_TotalBank);
	#endif
	while(i16) {
		i16--;
		addr = disk->FAT_StartSector + (i16 * disk->FAT_Bank2Sector);
		#if	DEBUG
		printf("addr=%lx,i16=%x,MaxSector=%lx,StartSector=%lx\n",addr,i16,MaxSector,disk->FAT_StartSector);
		#endif
		if( MaxSector ){
			if( 0 == DOS_SectorRead(DevId, addr, MaxSector, disk->FAT_BufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
			if(disk->FsType == 12) {
				FAT12_PattSearch(mode, disk->FAT_BufAddr, MaxCluster, 0, \
						&first, &total, &hit);
			}
			else{
				HAL_DramPattSearch(mode, disk->FAT_BufAddr, MaxCluster, 0, \
					&first, &total, &hit);
			}
	#if DEBUG
	printf("%s %u MaxCluster=%lu,first=%lu,total=%lu,hit=%bu,\n", __FILE__, (unsigned short)__LINE__,\
		MaxCluster,first,total,hit);
	#endif
			if(hit) {
				disk->FreeCluster += total;
				if(disk->FsType != 12) {//xian ++, 20110401, only one bank for fat12
					first += (UINT32)i16 << disk->FAT_bitBank2Cluster;
				}
				disk->FAT_FirstFreeCluster = first;
				disk->FAT_ContFreeCluster  = hit;
			}
		}
		MaxCluster = disk->FAT_Bank2Cluster;
		MaxSector  = disk->FAT_Bank2Sector;
	}

	if (disk->FAT_FirstFreeCluster < ((disk->FsType == 32) ? 2 : 1) && disk->FreeCluster) {
		ASSERT(0, 1); //The FAT start two(FAT16) or three(FAT32) could NOT be 0. 
		return DOS_FAIL;
	}

	DOS_CurrDriverSet(DevId);
	return 1;
}

/**
 * @brief	unmount a existed driver from DOS system
 * @param	DevId, device ID, 0=SD, 1=NAND, 2=RAM
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Ted Chiu
 * @since	08/04/16 17:37
 * @todo
 * @bug
*/
UINT8 DOS_UnMount(UINT8 DevId)
{
	DISK *disk;
	if(DevId >= MAX_DEV)
		return DOS_ERR_INVALID_DRV_ID;
	disk = (DISK *)&dos.disk[DevId];
	disk->FsType = 0;
	if(dos.CurrDrv == DevId)
		dos.CurrDrv = DRIVE_NONE;
	return 0;
}

#if 0
/**
 * @brief	get a bank number for select driver
 * @param	DevId, device ID, 0=SD, 1=NAND, 2=RAM
 * @retval	bank number, 1 is means bank function is OFF.
 * @see
 * @author	Ted Chiu
 * @since	08/04/16 17:38
 * @todo
 * @bug
*/
UINT32 DOS_BankNumGet(UINT32 DevId)
{
	UINT32 ret=0;
	DISK *disk;
	disk = (DISK *)&dos.disk[DevId];
	if(disk->FsType)
		ret = disk->FAT_TotalBank;
	return ret;
}
#endif

UINT8 DOS_DevPreCheck(UINT8 DevId)
{
	if(DevId >= MAX_DEV)
		return DOS_ERR_INVALID_DRV_ID;
	return DOS_ERR_NONE;
}

void DOS_CurrDriverSet(UINT8 DevId)
{
	DISK *disk;
	if(DevId < MAX_DEV) {
		disk = (DISK *)&dos.disk[DevId];
		if(disk->FsType) {
			dos.CurrDrv = DevId;
		}
	}
}


UINT8 DOS_CurrDriverGet(void)
{
	return dos.CurrDrv;
}

/**
 * @brief	FAT12 pattern search
 * @param	DevId, device ID, 0=SD, 1=NAND, 2=RAM
 * @param	want, free clusters number, max=255
 * @param	next, 1st free clusters number
 * @retval	free cluster number, 0 is FULL
 * @see
 * @since	08/04/19 10:59
 * @todo
 * @bug
*/
void FAT12_PattSearch(UINT8 mode, UINT32 addr, UINT32 length, UINT32 patt,
					   UINT32 *first, UINT32 *total, UINT8 *hit)
{
	UINT32 ret32,cluster,curr,tmp;
	UINT32 First,Total, baseAddr;
	UINT8 Hit, continious = 0;

	First = 0;
	Total = 0;
	Hit = 0;
	cluster = 0;
	mode = mode;
	curr = 0;

	baseAddr = addr;
#if DEBUG
dumpData(16, (addr - 1) ,1);
printf("addr=%lx length=%lu curr=%lu,First=%lx,Total=%lx\n",addr,length,curr,First,Total);
#endif
	while( curr < length ){
		tmp = curr % 4;
		addr = baseAddr + ((curr >> 2) * 3) + (tmp >> 1);
#if DEBUG
printf("%s %u addr=%lx length=%lu curr=%lu,First=%lx,Total=%lx\n", __FILE__, (unsigned short)__LINE__, addr,length,curr,First,Total);
#endif
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
		//read 2 or 4 bytes from DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		WRITE8(ret32,0,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(ret32,1,XBYTE[REG_Dram_IndirectRData_High]);
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		WRITE8(ret32,2,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(ret32,3,XBYTE[REG_Dram_IndirectRData_High]);
		if( tmp == 0 ){
			//        0  1  2  3
			//ret32 = FF XF XX XX
			WRITE8(cluster, 3, READ8(ret32,0));
			WRITE8(cluster, 2, READ8(ret32,1) & 0x0F);
		}
		else if( tmp == 1 ){
			//        0  1  2  3
			//ret32 = XX FX FF XX
			WRITE8(cluster, 2, READ8(ret32,2));
			WRITE8(cluster, 3, READ8(ret32,1));
			cluster = cluster >> 4;
		}
		else if( tmp == 2 ){
			//        0  1  2  3
			//ret32 = XX FF XF XX
			WRITE8(cluster, 3, READ8(ret32,1));
			WRITE8(cluster, 2, READ8(ret32,2) & 0x0F);
		}
		else if( tmp == 3 ){
			//        0  1  2  3
			//ret32 = XX XX FX FF
			WRITE8(cluster, 2, READ8(ret32,3));
			WRITE8(cluster, 3, READ8(ret32,2));
			cluster = cluster >> 4;
		}
		//cluster &= 0x0fff;
#if DEBUG
//if(Total < 20)
printf("%s %u clusterr=%lx,patt=%lx,tmp=%lx,curr=%lx\n", __FILE__, (unsigned short)__LINE__, cluster, patt,tmp,curr);
#endif
		if( patt == (cluster & 0xfff)){
//printf("curr=%lu,First=%lu,Total=%lu,%lx\n",curr,First,Total,patt);
			if( Hit == 0 ){
				continious = 1;
				First = curr;
				Hit = 1;
#if DEBUG
printf("%s %u First=%lx,cluster=%lx\n", __FILE__, (unsigned short)__LINE__, First,cluster);
#endif
			}
			else{
				if( Hit < 0xff ){
					if( continious != 2 ){
						Hit ++;
						if( Hit == 0xff && total == 0 ) break;
					}
					else if( total == 0 ){
						break;	
					}
				}
			}
			Total ++;
#if DEBUG
printf("%s %u addr=%lx length=%lu curr=%lu,First=%lx,Total=%lx\n", __FILE__, (unsigned short)__LINE__, addr,length,curr,First,Total);
#endif
		}
		else if( continious == 1 ){
//printf("%lx,%bx\n",Total,Hit);
			continious = 2;
			//break;
		}
		curr ++;
	}
	(* first) = First;
	if( total != 0 ) (* total ) = Total;
	(* hit) = Hit;
#if DEBUG
printf("%s %u addr=%lx length=%lu curr=%lu,*first=%lx,*total=%lx\n", __FILE__, (unsigned short)__LINE__, addr,length,curr,*first,*total);
#endif
}

void DOS_CurrDriverSet_Intr(UINT8 DevId)
{
	DISK *disk;
	if(DevId < MAX_DEV) {
		disk = (DISK *)&dos.disk[DevId];
		if(disk->FsType) {
			dos.CurrDrv = DevId;
		}
	}
}

