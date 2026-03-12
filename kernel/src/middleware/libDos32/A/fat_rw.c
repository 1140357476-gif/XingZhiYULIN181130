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
#include "dbg_def.h"
#include "smcfunc.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "reg_def.h"

#define __FILE	__FILE_ID_FAT_RW__
#define	DEBUG	0
extern DOS dos;
extern unsigned int USERDIV(unsigned int divX, unsigned int divY);
UINT32 xdata* data pdiskFAT_BankStartCluster;
UINT32 xdata* data pdiskFAT_Bank2Cluster;
UINT16 xdata startFATBank = 0xffff;
UINT16 xdata endFATBank = 0xffff;


/* This function intends to deal with clusts other than ROOT dir! */
UINT32 FAT_Cluster2Sector(UINT8 DevId, UINT32 Cluster)
{
	UINT32 Sector;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];
	if(Cluster >= 2) {
		Sector = disk->DataStartSector + ((Cluster-2) << disk->bitCluster2Sector);
		//printf("c=%08lx s=%08lx d=%08lx b=%bu\n", Cluster,Sector,disk->DataStartSector,disk->bitCluster2Sector);
	}
	else {
		Sector = disk->RootStartSector;
	}
	return Sector;
}
#if 0
UINT32 FAT_Sector2Cluster(UINT8 DevId, UINT32 Sector)
{
	UINT32 Cluster;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];

	if(Sector >= disk->DataStartSector) {
		Cluster  = (Sector - disk->DataStartSector) >> disk->bitCluster2Sector;
		Cluster += 2;
	}
	else {
		Cluster = 0;
	}
	return Cluster;
}
#endif
void FAT_CopyTable(UINT8 DevId)
{
	UINT32 i16;
	UINT32 Addr, Num;
	UINT32 addr;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];
	Addr = dos.TmpBufAddr;
#if DEBUG
printf("Copy FAT %x,%x\n",startFATBank, endFATBank);
#endif
	if( disk->FAT_WriteOnly1Copy == 0){ 
		startFATBank = 0xffff;
		endFATBank = 0xffff;
		return;
	}
	disk->FAT_WriteOnly1Copy = 0;
	if( startFATBank == 0xffff && endFATBank == 0xffff ){
		return;
	}
	for(i16 = startFATBank; i16 <= endFATBank; i16++) {
		if(i16 == (disk->FAT_TotalBank-1)) {
			if(disk->FsType == 12) {
				Num = disk->FAT_Sectors;
			}
			else{
				//num = disk->FAT_Sectors & (disk->FAT_Bank2Sector-1);
				if(disk->FAT_Sectors <= disk->FAT_Bank2Sector ){
					Num = disk->FAT_Sectors;
				}
				else{
					Num = disk->FAT_Sectors % (disk->FAT_Bank2Sector);
				}
			}
		}
		else {
			Num = disk->FAT_Bank2Sector;
		}
		addr = disk->FAT_StartSector + (i16 << disk->FAT_bitBank2Sector);
#if DEBUG
printf("%lx,%lx,%lx,%lx\n",i16, addr, Num, Addr);
#endif
		if( 0 == DOS_SectorRead(DevId, addr, Num, Addr)) dosRwErr(__FILE,(unsigned short)__LINE__);
		if( 0 == DOS_SectorWrite(DevId,(addr + disk->FAT_Sectors), Num, Addr)) dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	startFATBank = 0xffff;
	endFATBank = 0xffff;
}
void FAT_TableWriteBack(UINT8 DevId)
{
	UINT8 i8, copy;
	UINT32 start;
	UINT32 buf;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];
	if(disk->FAT_LocalDirtyLen==0)
		return;

	if(( startFATBank == 0xffff ) && ( endFATBank == 0xffff )){
		startFATBank = disk->FAT_CurrBank;
		endFATBank = disk->FAT_CurrBank;
	}
	else if(disk->FAT_CurrBank < startFATBank ){
		startFATBank = disk->FAT_CurrBank;		
	}
	else if(disk->FAT_CurrBank > endFATBank ){
		endFATBank = disk->FAT_CurrBank;		
	}
	buf   = disk->FAT_BufAddr + (disk->FAT_LocalDirtyStart << 8);	//word addr
	start = disk->FAT_CurrBank << disk->FAT_bitBank2Sector;
	start+= disk->FAT_LocalDirtyStart + disk->FAT_StartSector;

	//printf("Bank=%x,Start=%x,Len=%x,StartSector=%lx\n",disk->FAT_CurrBank,disk->FAT_LocalDirtyStart,disk->FAT_LocalDirtyLen,disk->FAT_StartSector);
	if( disk->FAT_WriteOnly1Copy ){
		copy = 1;
	}
	else{
		copy = disk->FAT_CopyNum;
	}
	// Write FAT copy
#if 0//( DOSRAM_OPTION == 1 )
	if( DevId == DRIVE_DRAM ){//DOSRAM don't need write back
		start += disk->FAT_Sectors;
		if( 0 == DOS_SectorWrite(DevId, start, disk->FAT_LocalDirtyLen, buf)) dosRwErr(__FILE,(unsigned short)__LINE__);
		disk->FAT_LocalDirtyLen   = 0;
		disk->FAT_LocalDirtyStart = -1;
		return;
	}
#endif
	for(i8=0; i8< copy; i8++) {
#if DEBUG
	printf("%s %u i8=%bx,buf=%lx,start=%lx,dirty=%x\n", __FILE__, (unsigned short)__LINE__,i8,buf,start,disk->FAT_LocalDirtyLen);
#endif
		if( 0 == DOS_SectorWrite(DevId, start, disk->FAT_LocalDirtyLen, buf)) dosRwErr(__FILE,(unsigned short)__LINE__);
		start += disk->FAT_Sectors;
	}

	DOS_CacheFlush();
	disk->FAT_LocalDirtyLen   = 0;
	disk->FAT_LocalDirtyStart = -1;
}
// The FAT bank switch only happen for FAT32, the buffer size just match 128KB (FAT16)
void FAT_BankSwitch(UINT8 DevId, UINT32 bank, UINT32 buf)
{
	UINT32 addr,num;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];
	if(disk->FsType == 12) {
		disk->FAT_CurrBank = 0;
		FAT_TableWriteBack(DevId);
		disk->FAT_BankStartCluster = 0;
		return;
	}
	if(bank != disk->FAT_CurrBank) {
#if DEBUG
		printf("Bank=%lx,Curr=%x\n",bank , disk->FAT_CurrBank);
#endif
		//xian ++ for debug
		if(bank >= disk->FAT_TotalBank){
			printf("E%u:%u,%bu,%lu -> %lu\n", (UINT16)__LINE__, 
				disk->FAT_CurrBank, disk->FAT_bitBank2Cluster, *pdiskFAT_BankStartCluster, 
				bank);
			//return;
		}
		//xian --
		
		if( buf == disk->FAT_BufAddr ){ 
			FAT_TableWriteBack(DevId);
		}
		addr = disk->FAT_StartSector + (bank << disk->FAT_bitBank2Sector);
		if(bank==disk->FAT_TotalBank-1) {
			//num = disk->FAT_Sectors & (disk->FAT_Bank2Sector-1);
			if(disk->FAT_Sectors <= disk->FAT_Bank2Sector ){
				num = disk->FAT_Sectors;
			}
			else{
				num = disk->FAT_Sectors % (disk->FAT_Bank2Sector);
			}
		}
		else {
			num = disk->FAT_Bank2Sector;
		}
		if( 0 == DOS_SectorRead(DevId, addr, num, buf)) dosRwErr(__FILE,(unsigned short)__LINE__);
		if( buf == disk->FAT_BufAddr ){ 
			disk->FAT_CurrBank = bank;
			disk->FAT_BankStartCluster = bank << disk->FAT_bitBank2Cluster;
		}
	}
	return;
}
#pragma OT(8, SPEED)
UINT32 FAT_TableRead(UINT8 DevId, UINT32 cluster)
{
	UINT32 ret32;
	UINT32 bank, addr;
	UINT8 tmp;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];
	//XBYTE[0x2030] &= 0xfe;
	
	if( cluster < *pdiskFAT_BankStartCluster || \
		(cluster - *pdiskFAT_BankStartCluster) >= *pdiskFAT_Bank2Cluster ){ // xian ++ 20101113
#if DEBUG
		printf("StartCluster=%lu,cluster=%lu\n",disk->FAT_BankStartCluster, cluster);
#endif
		//cluster = cluster + disk->FAT_BankStartCluster;
		bank = cluster >> disk->FAT_bitBank2Cluster;
#if DEBUG
		printf("B=%bx,C=%lx,B=%lu,M=%lx\n",disk->FAT_bitBank2Cluster,cluster,bank,disk->MaxClusterNum);
#endif
		FAT_BankSwitch(DevId, bank, disk->FAT_BufAddr);
		cluster = cluster - disk->FAT_BankStartCluster;
#if 0
		printf("S=%lu,C=%lx,%bx,B=%lu\n",disk->FAT_BankStartCluster,cluster,disk->FAT_bitBank2Cluster,bank);
#endif
	} else {//xian ++ 20101113
		cluster = cluster - *pdiskFAT_BankStartCluster;
	}

	//set base word addr
	if( disk->FsType != 12 ){
		cluster <<= disk->FsShift;
		addr = disk->FAT_BufAddr + cluster;
#if DEBUG
		printf("addr=%lx,FAT_BufAddr=%lx,cluster=%lu\n",addr , disk->FAT_BufAddr, cluster);
	//dumpData(32, addr, 1);
#endif
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
		//read 2 or 4 bytes from DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch] & 0x01);
		WRITE8(ret32,3,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(ret32,2,XBYTE[REG_Dram_IndirectRData_High]);
		//printf("FAT=%lx\n",ret32);
		if(disk->FsType==32) {
			while(XBYTE[REG_Dram_IndirectPrefetch] & 0x01);
			WRITE8(ret32,1,XBYTE[REG_Dram_IndirectRData_Low]);
			WRITE8(ret32,0,XBYTE[REG_Dram_IndirectRData_High]);
		}
		else {
			WRITE8(ret32,1,0);
			WRITE8(ret32,0,0);
		}
#if DEBUG
		printf("FAT=%lx\n",ret32);
#endif
		//XBYTE[0x2030] |= 0x01;
		return ret32;
	}
	else{
#if DEBUG
printf("%s %u cluster=%lx\n", __FILE__, (unsigned short)__LINE__,cluster);
#endif
		tmp = cluster % 4;
		addr = disk->FAT_BufAddr + ((cluster >> 2) * 3) + (tmp >> 1);
#if DEBUG
dumpData(16, disk->FAT_BufAddr ,1);
printf("%s %u cluster=%lx,addr=%lx\n", __FILE__, (unsigned short)__LINE__,cluster, addr);
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
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
			WRITE8(cluster, 3, READ8(ret32,0));
			WRITE8(cluster, 2, READ8(ret32,1) & 0x0F);
		}
		else if( tmp == 1 ){
			//        0  1  2  3
			//ret32 = XX FX FF XX
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
			WRITE8(cluster, 2, READ8(ret32,2));
			WRITE8(cluster, 3, READ8(ret32,1));
			cluster = cluster >> 4;
		}
		else if( tmp == 2 ){
			//        0  1  2  3
			//ret32 = XX FF XF XX
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
			WRITE8(cluster, 3, READ8(ret32,1));
			WRITE8(cluster, 2, READ8(ret32,2) & 0x0F);
		}
		else if( tmp == 3 ){
			//        0  1  2  3
			//ret32 = XX XX FX FF
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
			WRITE8(cluster, 2, READ8(ret32,3));
			WRITE8(cluster, 3, READ8(ret32,2));
			cluster = cluster >> 4;
		}
		cluster = cluster & 0x0fff;
#if DEBUG
printf("%s %u cluster=%lx\n", __FILE__, (unsigned short)__LINE__, cluster);
#endif
		return cluster;
	}
}
#pragma OT(8, SPEED)
void FAT_TableWrite(UINT8 DevId, UINT32 curr, UINT32 next)
{
	UINT16 DirtySector;
	UINT32 bank, addr, val;
	UINT8 tmp;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];

#if DEBUG
printf("%s %u c=%lx n=%lx, %lx\n", __FILE__, (unsigned short)__LINE__,curr, next, *pdiskFAT_BankStartCluster);
#endif
	if( curr < *pdiskFAT_BankStartCluster || \
		(curr - *pdiskFAT_BankStartCluster) >= *pdiskFAT_Bank2Cluster ){ //xian ++ 20101113
		//curr = curr + disk->FAT_BankStartCluster;
		bank = curr >> disk->FAT_bitBank2Cluster;
		FAT_BankSwitch(DevId, bank, disk->FAT_BufAddr);
#if DEBUG
		printf("%s %u S=%lu,C=%lx,%bx,B=%lu\n", __FILE__, (unsigned short)__LINE__,disk->FAT_BankStartCluster,curr,disk->FAT_bitBank2Cluster,bank);
#endif
		curr = curr - disk->FAT_BankStartCluster;
	} else {
		curr = curr - *pdiskFAT_BankStartCluster;
	}

//printf("W %lu,%lu,%lu\n",*pdiskFAT_BankStartCluster,*pdiskFAT_Bank2Cluster,curr);
#if DEBUG
printf("%s %u c=%lx n=%lx\n", __FILE__, (unsigned short)__LINE__,curr, next);
#endif
	//set base word addr
	if( disk->FsType != 12 ){
		curr <<= disk->FsShift;
		addr = disk->FAT_BufAddr + curr;
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		//write 2 or 4 bytes to DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(next,3);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(next,2);
		if(disk->FsType==32) {
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			XBYTE[REG_Dram_IndirectRData_Low] = READ8(next,1);
			XBYTE[REG_Dram_IndirectRData_High] = READ8(next,0);
		}
	}
	else{		
#if DEBUG
printf("%s %u c=%lx n=%lx\n", __FILE__, (unsigned short)__LINE__,curr, next);
#endif
		tmp = curr % 4;
		addr = disk->FAT_BufAddr + ((curr >> 2) * 3) + (tmp >> 1);
#if DEBUG
dumpData(16, disk->FAT_BufAddr ,1);
printf("%s %u c=%lx addr=%lx\n", __FILE__, (unsigned short)__LINE__,curr, addr);
#endif
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
		//read 2 or 4 bytes from DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		WRITE8(val,0,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(val,1,XBYTE[REG_Dram_IndirectRData_High]);
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		WRITE8(val,2,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(val,3,XBYTE[REG_Dram_IndirectRData_High]);
#if DEBUG
dumpData(16, addr ,1);
#endif
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
		if( tmp == 0 ){
			//       0  1  2  3
			//val = FF XF XX XX
			//next= 00 00 00 05
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
			WRITE8(val, 0, READ8(next,3));
			tmp = (READ8(val,1) & 0xF0) | (READ8(next,2) & 0x0F);
			WRITE8(val, 1, tmp);
		}
		else if( tmp == 1 ){
			//       0  1  2  3
			//val = XX FX FF XX
			//next= 00 00 00 60
			next = next << 4;
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
			WRITE8(val, 2, READ8(next,2));
			tmp = (READ8(val,1) & 0x0F) | (READ8(next,3) & 0xF0);
			WRITE8(val, 1, tmp);
		}
		else if( tmp == 2 ){
			//       0  1  2  3
			//val = XX FF XF XX
			//next= 00 00 00 07
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
			WRITE8(val, 1, READ8(next,3));
			tmp = (READ8(val,2) & 0xF0) | (READ8(next,2) & 0x0F);
			WRITE8(val, 2, tmp);
		}
		else if( tmp == 3 ){
			//       0  1  2  3
			//val = XX XX FX FF
			//next= 00 00 00 80
			next = next << 4;
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
			WRITE8(val, 3, READ8(next,2));
			tmp = (READ8(val,2) & 0x0F) | (READ8(next,3) & 0xF0);
			WRITE8(val, 2, tmp);
		}
#if DEBUG
printf("%s %u val=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(val,0),READ8(val,1),READ8(val,2),READ8(val,3));
#endif
#if DEBUG
printf("%s %u next=%bx %bx %bx %bx\n", __FILE__, (unsigned short)__LINE__,READ8(next,0),READ8(next,1),READ8(next,2),READ8(next,3));
#endif
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		//write 2 or 4 bytes to DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(val,0);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(val,1);
		while(XBYTE[REG_Dram_IndirectPrefetch]);	   
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(val,2);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(val,3);
		curr = (curr >> 2) * 3;// clusters to 512 bytes for dirty FAT
#if DEBUG
//dumpData(16, disk->FAT_BufAddr ,1);
printf("%s %u curr=%lx\n", __FILE__, (unsigned short)__LINE__,curr);
#endif
	}
	#if 0
	DirtySector = curr >> 8;	//1 sector = 512 bytes = 256 words
	#else
	WRITE8(DirtySector,0,READ8(curr,1));
	WRITE8(DirtySector,1,READ8(curr,2));
	#endif
#if DEBUG
	printf("%s %u DirtySector=%x,curr=%lx\n", __FILE__, (unsigned short)__LINE__,DirtySector, curr);
#endif
	if(disk->FAT_LocalDirtyLen==0) {
#if DEBUG
	printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
#endif
		disk->FAT_LocalDirtyLen   = 1;
		disk->FAT_LocalDirtyStart = DirtySector;
	}
	else if(disk->FAT_LocalDirtyStart > DirtySector) {	/* Move backward is means wrt to the others file */
#if DEBUG
	printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
#endif
		FAT_TableWriteBack(DevId);
		disk->FAT_LocalDirtyLen   = 1;
		disk->FAT_LocalDirtyStart = DirtySector;
	}
	else if(DirtySector >= disk->FAT_LocalDirtyStart + disk->FAT_LocalDirtyLen) { /* Was "if" */
#if DEBUG
	printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
#endif
		disk->FAT_LocalDirtyLen = DirtySector - disk->FAT_LocalDirtyStart + 1;
	}
}
#pragma OT(8, SPEED)
void FAT_TableWrite_Con(UINT8 DevId, UINT32 curr, UINT32 next)
{
	UINT16 DirtySector;
	UINT32 bank, addr, val;
	UINT8 tmp;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];

	if( curr < *pdiskFAT_BankStartCluster || \
		(curr - *pdiskFAT_BankStartCluster) >= *pdiskFAT_Bank2Cluster ){ //xian ++ 20101113
		//curr = curr + disk->FAT_BankStartCluster;
		bank = curr >> disk->FAT_bitBank2Cluster;
		printf("bank: %lx\n", (UINT32)bank);
		FAT_BankSwitch(DevId, bank, disk->FAT_BufAddr);
		curr = curr - disk->FAT_BankStartCluster;

		addr = disk->FAT_BufAddr + (curr << disk->FsShift);
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		printf("addr: %lx\n", (UINT32)addr);
	} else { //xian ++ 20101113
		curr = curr - *pdiskFAT_BankStartCluster;
	}

	//set base word addr
	if( disk->FsType != 12 ) {
		curr <<= disk->FsShift;
#if 0		
		addr = disk->FAT_BufAddr + curr;

		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
#endif
		//write 2 or 4 bytes to DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(next,3);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(next,2);
		if(disk->FsType==32) {
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			XBYTE[REG_Dram_IndirectRData_Low] = READ8(next,1);
			XBYTE[REG_Dram_IndirectRData_High] = READ8(next,0);
		}
	}
	else{		
		tmp = curr % 4;
		addr = disk->FAT_BufAddr + ((curr >> 2) * 3) + (tmp >> 1);
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
		//read 2 or 4 bytes from DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		WRITE8(val,0,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(val,1,XBYTE[REG_Dram_IndirectRData_High]);
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		WRITE8(val,2,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(val,3,XBYTE[REG_Dram_IndirectRData_High]);
		if( tmp == 0 ){
			//       0  1  2  3
			//val = FF XF XX XX
			//next= 00 00 00 05
			WRITE8(val, 0, READ8(next,3));
			tmp = (READ8(val,1) & 0xF0) | (READ8(next,2) & 0x0F);
			WRITE8(val, 1, tmp);
		}
		else if( tmp == 1 ){
			//       0  1  2  3
			//val = XX FX FF XX
			//next= 00 00 00 60
			next = next << 4;
			WRITE8(val, 2, READ8(next,2));
			tmp = (READ8(val,1) & 0x0F) | (READ8(next,3) & 0xF0);
			WRITE8(val, 1, tmp);
		}
		else if( tmp == 2 ){
			//       0  1  2  3
			//val = XX FF XF XX
			//next= 00 00 00 07
			WRITE8(val, 1, READ8(next,3));
			tmp = (READ8(val,2) & 0xF0) | (READ8(next,2) & 0x0F);
			WRITE8(val, 2, tmp);
		}
		else if( tmp == 3 ){
			//       0  1  2  3
			//val = XX XX FX FF
			//next= 00 00 00 80
			next = next << 4;
			WRITE8(val, 3, READ8(next,2));
			tmp = (READ8(val,2) & 0x0F) | (READ8(next,3) & 0xF0);
			WRITE8(val, 2, tmp);
		}
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		//write 2 or 4 bytes to DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(val,0);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(val,1);
		while(XBYTE[REG_Dram_IndirectPrefetch]);	   
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(val,2);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(val,3);
		curr = (curr >> 2) * 3;// clusters to 512 bytes for dirty FAT
	}
	
	#if 0
	DirtySector = curr >> 8;	//1 sector = 512 bytes = 256 words
	#else
	WRITE8(DirtySector,0,READ8(curr,1));
	WRITE8(DirtySector,1,READ8(curr,2));
	#endif
	
	if(disk->FAT_LocalDirtyLen==0) {
		disk->FAT_LocalDirtyLen   = 1;
		disk->FAT_LocalDirtyStart = DirtySector;
	}
	else if(disk->FAT_LocalDirtyStart > DirtySector) {	/* Move backward is means wrt to the others file */
		FAT_TableWriteBack(DevId);
		disk->FAT_LocalDirtyLen   = 1;
		disk->FAT_LocalDirtyStart = DirtySector;
	}
	else if(DirtySector >= disk->FAT_LocalDirtyStart + disk->FAT_LocalDirtyLen) { /* Was "if" */
		disk->FAT_LocalDirtyLen = DirtySector - disk->FAT_LocalDirtyStart + 1;
	}
}
/**
 * @brief	get cont. free cluster
 * @param	DevId, device ID, 0=SD, 1=NAND, 2=RAM
 * @param	want, free clusters number, max=255
 * @param	next, 1st free clusters number
 * @retval	free cluster number, 0 is FULL
 * @see
 * @author	Ted Chiu
 * @since	08/04/19 10:59
 * @todo
 * @bug
*/
#if 1
#pragma OT(8, SPEED)
UINT32 FAT_FreeClusterGet(UINT8 DevId, UINT32 want, UINT32 *next)
{
	UINT16 i16;
	UINT32 Bank;
	UINT8  Mode, Free;
	UINT32 Addr, Num, First, Total;
	UINT32 offset;
	UINT32 BankStartCluster;
	xdata  DISK *disk;

	disk = (DISK *)&dos.disk[DevId];
	#if	0	//FreeCluster < want?
	if(disk->FreeCluster < want) {		//Should be checked by DOS_FileWrite()
		want = disk->FreeCluster;
	}
	#endif

	if(disk->FsType == 12) {
		Bank = 0;
		//4 clusters is 3 word periodically
		offset = (disk->FAT_FirstFreeCluster / 4);
		offset = offset * 3;
	}
	else{
		Bank   = disk->FAT_FirstFreeCluster >> disk->FAT_bitBank2Cluster;
		offset = disk->FAT_FirstFreeCluster & (disk->FAT_Bank2Cluster-1);
	}
	FAT_BankSwitch(DevId,Bank,disk->FAT_BufAddr);
	BankStartCluster = disk->FAT_BankStartCluster;
	if(disk->FsType==32) {
		Mode = PATT_SEARCH_FAT32_FREE;	//1 unit = 4 bytes
		Addr = disk->FAT_BufAddr + (offset << 1);
	}
	else {
		Mode = PATT_SEARCH_FAT16_FREE;
		Addr = disk->FAT_BufAddr + offset;
#if DEBUG
printf("Addr=%lx offset=%lu FreeCluster=%lu\n", Addr, offset,disk->FAT_FirstFreeCluster);
#endif
	}

#if DEBUG
	printf("%s %u Bank=%lu,offset=%lu,Addr=%lx\n", __FILE__, (unsigned short)__LINE__,Bank,offset,Addr);
#endif
	for(i16=0; i16<disk->FAT_TotalBank; i16++) {
		if(Bank==disk->FAT_TotalBank-1) {
			if(disk->FsType == 12) {
				Num = disk->FAT_Bank2Cluster;
			}
			else{
				//Num = disk->MaxClusterNum & (disk->FAT_Bank2Cluster-1);
				if(disk->MaxClusterNum <= disk->FAT_Bank2Cluster ){
					Num = disk->MaxClusterNum;
				}
				else{
					Num = disk->MaxClusterNum % (disk->FAT_Bank2Cluster);
				}
			}
		}
		else {
			Num = disk->FAT_Bank2Cluster;
		}
		Num -= offset;
#if DEBUG
printf("%s %u i=%u n=%lu b=%u off=%lu\n", __FILE__, (unsigned short)__LINE__, i16, Num, Bank,offset);
#endif
		/* IC's bug...work-around by length or HAL_DramPattSearch() */
		#if SEARCH_8N_16N_PLUS_1==0
		if((Num & 0x07)==1) Num--;
		#endif
		if(Num) {
			if(disk->FsType == 12) {
				FAT12_PattSearch(Mode, Addr, Num, 0, &First, NULL, &Free);
			}
			else{
				HAL_DramPattSearch(Mode, Addr, Num, 0, &First, &Total, &Free);
			}
#if DEBUG
printf("First=%lu Total=%lu Free=%bu\n", First, Total,Free);
#endif
			if(Free) break;
		}
		Bank++;
		if(Bank == disk->FAT_TotalBank){ //xian ++ 20100911, 
			Bank = 0;
		}
		offset = 0;
		Addr   = dos.TmpBufAddr;
		FAT_BankSwitch(DevId,Bank,Addr);
		BankStartCluster = Bank << disk->FAT_bitBank2Cluster; 
//dumpData(256, disk->FAT_BufAddr ,1);
//dumpData(256, Addr ,1);
	}
	if(!Free){
		printf("E%u:%lu,%lu,%lu,%lu\n", (UINT16)__LINE__, disk->FAT_FirstFreeCluster, Bank, First, offset);
	}
	if (want) {
		disk->FAT_ContFreeCluster = Free;
		if(disk->FsType == 12) {
			disk->FAT_FirstFreeCluster = First + disk->FAT_FirstFreeCluster - (disk->FAT_FirstFreeCluster % 4);
		}
		else{
			disk->FAT_FirstFreeCluster= First + BankStartCluster + offset;
		}
#if DEBUG
		printf("%s %u 1st=%lu s=%lu,%lu,%lu\n", __FILE__, (unsigned short)__LINE__,\
			disk->FAT_FirstFreeCluster,BankStartCluster, First, offset);
#endif

		*next = disk->FAT_FirstFreeCluster;
		if(want > Free)
			want = Free;
		disk->FreeCluster -= want;
	} else {
		if(disk->FsType == 12) {
			*next = First + disk->FAT_FirstFreeCluster - (disk->FAT_FirstFreeCluster % 4);
		} else {
			*next = First + disk->FAT_BankStartCluster + offset;
		}	
	}
	
	return want;
}
#else
UINT32 FAT_FreeClusterGet(UINT8 DevId, UINT32 want, UINT32 *next)
{
	UINT16 i16;
	UINT32 Bank;
	UINT8  Mode, Free;
	UINT32 Addr, Num, First, Total;
	UINT32 offset;
	xdata  DISK *disk;

	disk = (DISK *)&dos.disk[DevId];
	#if	0	//FreeCluster < want?
	if(disk->FreeCluster < want) {		//Should be checked by DOS_FileWrite()
		want = disk->FreeCluster;
	}
	#endif

	if(disk->FsType == 12) {
		Bank = 0;
		//4 clusters is 3 word periodically
		offset = (disk->FAT_FirstFreeCluster / 4);
		offset = offset * 3;
	}
	else{
		Bank   = disk->FAT_FirstFreeCluster >> disk->FAT_bitBank2Cluster;
		offset = disk->FAT_FirstFreeCluster & (disk->FAT_Bank2Cluster-1);
	}
	FAT_BankSwitch(DevId,Bank,disk->FAT_BufAddr);
	if(disk->FsType==32) {
		Mode = PATT_SEARCH_FAT32_FREE;	//1 unit = 4 bytes
		Addr = disk->FAT_BufAddr + (offset << 1);
	}
	else {
		Mode = PATT_SEARCH_FAT16_FREE;
		Addr = disk->FAT_BufAddr + offset;
#if DEBUG
printf("Addr=%lx offset=%lu FreeCluster=%lu\n", Addr, offset,disk->FAT_FirstFreeCluster);
#endif
	}

#if DEBUG
	printf("%s %u Bank=%lu,offset=%lu,Addr=%lx\n", __FILE__, (unsigned short)__LINE__,Bank,offset,Addr);
#endif
	for(i16=0; i16<disk->FAT_TotalBank; i16++) {
		if(Bank==disk->FAT_TotalBank-1) {
			if(disk->FsType == 12) {
				Num = disk->FAT_Bank2Cluster;
			}
			else{
				//Num = disk->MaxClusterNum & (disk->FAT_Bank2Cluster-1);
				if(disk->MaxClusterNum <= disk->FAT_Bank2Cluster ){
					Num = disk->MaxClusterNum;
				}
				else{
					Num = disk->MaxClusterNum % (disk->FAT_Bank2Cluster);
				}
			}
		}
		else {
			Num = disk->FAT_Bank2Cluster;
		}
		Num -= offset;
#if DEBUG
printf("%s %u i=%u n=%lu b=%u off=%lu\n", __FILE__, (unsigned short)__LINE__, i16, Num, Bank,offset);
#endif
		/* IC's bug...work-around by length or HAL_DramPattSearch() */
		#if SEARCH_8N_16N_PLUS_1==0
		if((Num & 0x07)==1) Num--;
		#endif
		if(Num) {
			if(disk->FsType == 12) {
				FAT12_PattSearch(Mode, Addr, Num, 0, &First, NULL, &Free);
			}
			else{
				HAL_DramPattSearch(Mode, Addr, Num, 0, &First, &Total, &Free);
			}
#if DEBUG
printf("First=%lu Total=%lu Free=%bu\n", First, Total,Free);
#endif
			if(Free) break;
		}
		Bank++;
		offset = 0;
		Addr   = disk->FAT_BufAddr;
		FAT_BankSwitch(DevId,Bank,disk->FAT_BufAddr);
	}
	if (want) {
		disk->FAT_ContFreeCluster = Free;
		if(disk->FsType == 12) {
			disk->FAT_FirstFreeCluster = First + disk->FAT_FirstFreeCluster - (disk->FAT_FirstFreeCluster % 4);
		}
		else{
			disk->FAT_FirstFreeCluster= First + disk->FAT_BankStartCluster + offset;
		}
#if DEBUG
		printf("%s %u 1st=%lu s=%lu,\n", __FILE__, (unsigned short)__LINE__,\
			disk->FAT_FirstFreeCluster,disk->FAT_BankStartCluster);
#endif

		*next = disk->FAT_FirstFreeCluster;
		if(want > Free)
			want = Free;
		disk->FreeCluster -= want;
	} else {
		if(disk->FsType == 12) {
			*next = First + disk->FAT_FirstFreeCluster - (disk->FAT_FirstFreeCluster % 4);
		} else {
			*next = First + disk->FAT_BankStartCluster + offset;
		}	
	}
	
	return want;
}
#endif
