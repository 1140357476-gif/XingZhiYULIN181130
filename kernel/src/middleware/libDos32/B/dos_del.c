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
#include "sd_bg.h"
//#include "dosusr.h"

#define __FILE	__FILE_ID_DOS_DEL__
#define	DEBUG	0

extern DOS dos;
extern UINT32 dos_var_start;

//return err_code, 0 is OK!
//TEST OK!
extern UINT32 xdata* data pdiskFAT_BankStartCluster;
extern UINT32 xdata* data pdiskFAT_Bank2Cluster;
#pragma OT(8, SPEED)
UINT32 FAT_TableReadDel(UINT8 FsType, UINT32 xdata *clust)
{
	UINT32 data next;
	UINT32 data curr = (*clust), tmp;
	tmp = *pdiskFAT_BankStartCluster + *pdiskFAT_Bank2Cluster;
	if(FsType == 32) {
		while(1){
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			WRITE8(next,3,XBYTE[REG_Dram_IndirectRData_Low]);
			WRITE8(next,2,XBYTE[REG_Dram_IndirectRData_High]);
			//printf("FAT=%lx\n",ret32);
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			WRITE8(next,1,XBYTE[REG_Dram_IndirectRData_Low]);
			WRITE8(next,0,XBYTE[REG_Dram_IndirectRData_High]);
			if( next == (curr + 1) &&  (next < tmp)){
				curr = next;
				continue;
			}
			*clust = curr;
			return next;
		}
	}
	else {
		while(1){
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			WRITE8(next,3,XBYTE[REG_Dram_IndirectRData_Low]);
			WRITE8(next,2,XBYTE[REG_Dram_IndirectRData_High]);
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			WRITE8(next,1,0);
			WRITE8(next,0,0);
			if( next == (curr + 1) &&  (next < tmp)){
				curr = next;
				continue;
			}
			*clust = curr;
			return next;
		}
	}
}

#pragma OT(8, SPEED)
UINT8 DOS_FileDel(UINT8 *fname)
{
	UINT8  DevId,fno, ret;
	UINT32 next, curr;
	xdata x86_FDB *fdb;
	xdata FILE *file;
	xdata DISK *disk;
	bgRwAbort();
	DevId = DOS_CurrDriverGet();
	disk = (DISK *)&dos.disk[DevId];

	//dbgTimerStart();
	ret = DOS_FILE_NOT_FOUND;
	fno = DOS_FileOpen(fname, 'r');
	printf("del:%s-%bu\n", fname,fno);
	if(fno) {
		file  = (FILE *)&dos.file[fno];
		fdb   = (x86_FDB *)&file->fdb;
		disk  = (DISK *)&dos.disk[DevId];
		DevId = file->DevId;

		fdb->name[0] = 0xE5;
		curr  = file->FirstCluster;					
		if( curr < disk->FAT_FirstFreeCluster ){
			disk->FAT_FirstFreeCluster = curr;//mantis-26586 20080620
		}
		if( disk->FsType == 12 ){
			while(curr) {
				if((curr > disk->MaxClusterNum-1) || (curr < 2)){
					next =  disk->FAT_EOFValue;
				}
				else{
					next = FAT_TableRead(DevId, curr);
				}
				if( next < disk->FAT_FirstFreeCluster ){
					disk->FAT_FirstFreeCluster = next;//mantis-34963 20090108
				}
				//DOS_Cluster2Block_Erase(DevId, curr, next);
				FAT_TableWrite(DevId, curr, 0);
				disk->FreeCluster++;
				if((next & disk->FAT_EOFValue) == disk->FAT_EOFValue)
					break;
				curr = next;
			}
		}
		else{
			//HAL_CpuImemSdSet(LOG_BANK_1, 0x8000); // FAT_TableReadDel inside IMEM
			{
				UINT32 fatcount = 0;
				UINT32 addr = disk->FAT_BufAddr; 
				UINT32 Addr; 
				UINT8 shift = disk->FsShift;
				UINT8 fsType = disk->FsType;
				UINT32 clust = curr;
	
				FAT_TableRead(DevId, curr);//switch FAT bank if the curr not at current bank 
				Addr = disk->FAT_BufAddr + ((curr - disk->FAT_BankStartCluster) << shift);
				XBYTE[REG_Dram_IndirectAddr_B0] = READ8(Addr,3);
				XBYTE[REG_Dram_IndirectAddr_B1] = READ8(Addr,2);
				XBYTE[REG_Dram_IndirectAddr_B2] = READ8(Addr,1);
				XBYTE[REG_Dram_IndirectAddr_B3] = READ8(Addr,0);
				XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
				while(curr) {
					/* find the continued clusters */
					next = FAT_TableReadDel(fsType, &curr); // next maybe over bank
					if( next < disk->FAT_FirstFreeCluster ){
						disk->FAT_FirstFreeCluster = next;//mantis-26586 20080620
					}
					//printf(": %lx, %lx, %lx\n", clust, curr, next);
					
					/* update Free cluster */
					fatcount = curr - clust + 1;
					disk->FreeCluster += fatcount;
	
					/* clear FAT in dram */
					if( fatcount ) {					
						HAL_GprmDramFill(Addr, (fatcount <<  (shift+1)), 0x00); /* the fill size could be more than 64KB */
					}
					
					/* write dirty start and end, write back if cluster jump back */
					FAT_TableWrite(DevId, clust, 0); 
					FAT_TableWrite(DevId, curr, 0);
					
					if((next & disk->FAT_EOFValue) == disk->FAT_EOFValue)
						break;
					
					/* check if the FAT bank switch in FAT table read */
					FAT_TableRead(DevId, next);
					//printf("sc:%lx\n", disk->FAT_BankStartCluster);
					
					/* configure the next FAT delete flow */
					curr = next;
					clust = next;					
					Addr = disk->FAT_BufAddr + ((curr- disk->FAT_BankStartCluster) << shift);
					XBYTE[REG_Dram_IndirectAddr_B0] = READ8(Addr,3);
					XBYTE[REG_Dram_IndirectAddr_B1] = READ8(Addr,2);
					XBYTE[REG_Dram_IndirectAddr_B2] = READ8(Addr,1);
					XBYTE[REG_Dram_IndirectAddr_B3] = READ8(Addr,0);
					XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
				}
			}
		}
		DIR_AddFDB(fno);
		FAT_TableWriteBack(DevId);
		file->DevId = -1;
		FAT_CopyTable(DevId);
		ret = DOS_SUCCESS;
	}
	bgRwResume();
	//dbgTimerStop();
//printf("fr: %lx, %lx\n", disk->FAT_FirstFreeCluster, disk->FreeCluster );
	return ret;
}
#if 0//wangjg++ clear noused function
static UINT32 LastDelSector=-1;
static UINT32 LastDelDrvId=-1;
void DOS_FileDelReset(void)
{
	LastDelSector = -1;
	LastDelDrvId  = -1;
}
#if FALSE
//call DOS_FileDelReset() first, then cont DOS_FileDelX() w/o wrt
//call DOS_FileDelX() w/ wrt for last file
// Speed up delete function, will have 512 bytes buffer to keep 16 FDBs ready for delete
// Once switch to other sector, the current 16 FDB will be deleted.
void DOS_FileDelX(FDB *InputFdb, UINT8 writeImmediate)	//for READ only
{
	UINT8 DevId;
	UINT8 *ptr8;
	UINT32 curr, next;
	xdata DISK *disk;
	DevId = InputFdb->DevId;
	disk  = (DISK *)&dos.disk[DevId];

	if((LastDelSector != InputFdb->FDB_Sector) || (LastDelDrvId != DevId)) {
		if(LastDelSector != -1)	{//wrt back
			if(0 == DOS_SectorWrite(LastDelDrvId, LastDelSector, 1, dos_var_start+FAST_DEL_TMP/2)) dosRwErr(__FILE,(unsigned short)__LINE__);
		}
		if( 0 == DOS_SectorRead(DevId, InputFdb->FDB_Sector, 1, dos_var_start+FAST_DEL_TMP/2)) dosRwErr(__FILE,(unsigned short)__LINE__);
	}

	ptr8  = (dos.share+FAST_DEL_TMP) + ((UINT16)InputFdb->FDB_Idx << 5);
 	*ptr8 = 0xE5;	//erase mark
	curr  = InputFdb->start;
	if( curr < disk->FAT_FirstFreeCluster ){
		disk->FAT_FirstFreeCluster = curr;//mantis-26586 20080620
	}
	while(curr) {
		if((curr > disk->MaxClusterNum-1) || (curr < 2)){
			next =  disk->FAT_EOFValue;
		}
		else {
			next = FAT_TableRead(DevId, curr);
		}
		if( next < disk->FAT_FirstFreeCluster ){
			disk->FAT_FirstFreeCluster = next;//mantis-34963 20090108
		}
		//DOS_Cluster2Block_Erase(DevId, curr, next);
		FAT_TableWrite(DevId, curr, 0);
		disk->FreeCluster++;
		if((next & disk->FAT_EOFValue) == disk->FAT_EOFValue){
			break;
		}
		curr = next;
	}
	if(writeImmediate) {
		FAT_TableWriteBack(DevId);
		if(0 == DOS_SectorWrite(DevId, InputFdb->FDB_Sector, 1, dos_var_start+FAST_DEL_TMP/2)) dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	LastDelSector = InputFdb->FDB_Sector;
	LastDelDrvId  = DevId;
	return;
}
#endif
void DOS_FileDelStop(void)
{
	if(LastDelSector != -1) {
		if(0 == DOS_SectorWrite(LastDelDrvId, LastDelSector, 1, dos_var_start+FAST_DEL_TMP/2)) dosRwErr(__FILE,(unsigned short)__LINE__);
		FAT_TableWriteBack(LastDelDrvId);
	}
	LastDelSector = -1;
	LastDelDrvId  = -1;
}
UINT8 DOS_FileDelBgCacheTrig_Intr(void)
{
	return TRUE;
}
#endif
