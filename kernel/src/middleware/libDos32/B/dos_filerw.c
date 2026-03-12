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
#include "mtd_macro_api.h"
#include "hal_gprm.h"
#include "reg_def.h"

#define __FILE	__FILE_ID_DOS_FILERW__
#define	DEBUG	0

#define FAT_NEW//

extern DOS dos;
xdata UINT32 G_Pre_Cluster;
xdata UINT32 G_MakeFatChain_FileRead = 0;
xdata UINT32 G_FATChainContiniousCnt = 0;

extern void FAT_TableWrite_Con(UINT8 DevId, UINT32 curr, UINT32 next);
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 DOS_FileReadMakeFatChainInit(UINT8 fno)
{
	xdata DISK *disk;
	UINT8  DevId;
	//UINT32 addr,temp;
	DevId = DOS_CurrDriverGet();;
	disk   = (DISK *)&dos.disk[DevId];
	disk->FAT_ChainIndexFno = fno;
	disk->FAT_ChainIndex = 0;
	G_Pre_Cluster = 0;
	G_FATChainContiniousCnt = 0;
	return TRUE; 
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void DOS_FileReadMakeFatEnable(UINT8 enable, UINT8 fd)
{
	if( enable ){
		G_MakeFatChain_FileRead = 1;
	}
	else{
		G_MakeFatChain_FileRead = 0;
	}
	DOS_FileReadMakeFatChainInit(fd);
}

#if 0//No buffer usage reduction
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void DOS_FileReadMakeFatChain(UINT8 fno, UINT32 cluster, UINT16 line)
{
	//xdata FILE *file;
	xdata DISK *disk;
	UINT8  DevId;
	UINT32 addr,temp;
	DevId = DOS_CurrDriverGet();;
	//file   = (FILE *)&dos.file[fno];
	disk   = (DISK *)&dos.disk[DevId];
	if(fno != disk->FAT_ChainIndexFno ) {
		//__ASSERT(__FILE, __LINE__, 1);
		return;
	}
	if( G_Pre_Cluster == cluster ) {
		//__ASSERT(__FILE, __LINE__, 1);
		return;
	}
	temp = dos.TmpBufAddr + ((UINT32)TMP_BUF_SIZE >> 1);
	addr = dos.TmpBufAddr + disk->FAT_ChainIndex;
	//if( cluster == file->CurrCluster ) return;
	if( addr >= temp ){
		__ASSERT(__FILE, __LINE__, 1);
		disk->FAT_ChainIndex = 0;
	}
	if( disk->FsType == 12 ) return;
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
	//write 2 or 4 bytes to DRAM
	while(XBYTE[REG_Dram_IndirectPrefetch]);
	XBYTE[REG_Dram_IndirectRData_Low] = READ8(cluster,3);
	XBYTE[REG_Dram_IndirectRData_High] = READ8(cluster,2);
	disk->FAT_ChainIndex ++;
	if(disk->FsType == 32) {
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(cluster,1);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(cluster,0);
		disk->FAT_ChainIndex ++;
	}
	//printf("%lu line=%u\n",cluster, line);
	G_Pre_Cluster = cluster;
	//printf("line=%u\n",line);
	//dumpData(16, (addr - 2), 1);
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 DOS_FileReadFatChain(UINT8 fno, UINT32 times)
{
	xdata DISK *disk;
	UINT8  DevId;
	UINT32 ret32, addr;
	DevId = DOS_CurrDriverGet();;
	disk   = (DISK *)&dos.disk[DevId];
	//printf("times=%lu\n",times);
	if( disk->FsType == 12 ) return 0;
	if(fno != disk->FAT_ChainIndexFno ) {
		__ASSERT(__FILE, __LINE__, 1);
		return 0;
	}
	if( disk->FAT_ChainIndex == 0 ) {
		__ASSERT(__FILE, __LINE__, 1);
		return 0;
	}
	while(times){
		if(disk->FsType == 32) {
			if( disk->FAT_ChainIndex < 2 ) {
				__ASSERT(__FILE, __LINE__, 1);
				return 0;
			}
			disk->FAT_ChainIndex -= 2;
		}
		else{
			if( disk->FAT_ChainIndex == 0 ) {
				__ASSERT(__FILE, __LINE__, 1);
				return 0;
			}
			disk->FAT_ChainIndex --; 
		} 
		addr = dos.TmpBufAddr + disk->FAT_ChainIndex;
		//dumpData(16, addr, 1);
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
		if(disk->FsType == 32) {
			while(XBYTE[REG_Dram_IndirectPrefetch] & 0x01);
			WRITE8(ret32,1,XBYTE[REG_Dram_IndirectRData_Low]);
			WRITE8(ret32,0,XBYTE[REG_Dram_IndirectRData_High]);
		}
		else {
			WRITE8(ret32,1,0);
			WRITE8(ret32,0,0);
		}
		times--;
		//printf("%lu ",ret32);
	}
	//set back to next addr
	if(disk->FsType == 32) {
		disk->FAT_ChainIndex += 2;
	}
	else{
		disk->FAT_ChainIndex ++; 
	} 
	//printf("\nfat=%lu\n",ret32);
	G_Pre_Cluster = ret32;
	return ret32;
}
#else
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void DOS_FileReadMakeFatChain(UINT8 fno, UINT32 cluster, UINT16 line)
{
	//xdata FILE *file;
	xdata DISK *disk;
	UINT8  DevId;
	UINT32 addr,temp;

	line = line;

	DevId = DOS_CurrDriverGet();;
	//file   = (FILE *)&dos.file[fno];
	disk   = (DISK *)&dos.disk[DevId];
	if(fno != disk->FAT_ChainIndexFno ) {
		//__ASSERT(__FILE, __LINE__, 1);
		return;
	}
	if((G_Pre_Cluster + G_FATChainContiniousCnt + 1) == cluster ){
		G_FATChainContiniousCnt ++;
		//printf("0-%lu %lu\n",G_Pre_Cluster,G_FATChainContiniousCnt);
		return;
	}
	else if(( G_Pre_Cluster == cluster ) || ( cluster == disk->FAT_EOFValue )) {
		//__ASSERT(__FILE, __LINE__, 1);
		return;
	}
	temp = dos.TmpBufAddr + ((UINT32)TMP_BUF_SIZE >> 1);
	addr = dos.TmpBufAddr + disk->FAT_ChainIndex;
	//if( cluster == file->CurrCluster ) return;
	if( addr >= temp ){
		//__ASSERT(__FILE, __LINE__, 1);
		disk->FAT_ChainIndex = 0;
	}
	if( disk->FsType == 12 ) return;
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
	//write 2 or 4 bytes to DRAM
	while(XBYTE[REG_Dram_IndirectPrefetch]);
	XBYTE[REG_Dram_IndirectRData_Low] = READ8(G_Pre_Cluster,3);
	XBYTE[REG_Dram_IndirectRData_High] = READ8(G_Pre_Cluster,2);
	disk->FAT_ChainIndex ++;
	if(disk->FsType == 32) {
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(G_Pre_Cluster,1);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(G_Pre_Cluster,0);
		disk->FAT_ChainIndex ++;
	}
	while(XBYTE[REG_Dram_IndirectPrefetch]);
	XBYTE[REG_Dram_IndirectRData_Low] = READ8(G_FATChainContiniousCnt,3);
	XBYTE[REG_Dram_IndirectRData_High] = READ8(G_FATChainContiniousCnt,2);
	disk->FAT_ChainIndex ++;
	if(disk->FsType == 32) {
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = READ8(G_FATChainContiniousCnt,1);
		XBYTE[REG_Dram_IndirectRData_High] = READ8(G_FATChainContiniousCnt,0);
		disk->FAT_ChainIndex ++;
	}
	//printf("1-%lu %lu %lu %lu\n", cluster, G_Pre_Cluster, G_FATChainContiniousCnt, disk->FAT_ChainIndex);
	G_Pre_Cluster = cluster;
	G_FATChainContiniousCnt = 0;
	//printf("line=%u\n",line);
	//dumpData(32, dos.TmpBufAddr, 1);
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 DOS_FileReadFatChain(UINT8 fno, UINT32 times)
{
	xdata DISK *disk;
	UINT8  DevId;
	UINT32 addr;
	DevId = DOS_CurrDriverGet();;
	disk   = (DISK *)&dos.disk[DevId];
	//printf("times=%lu\n",times);
	if( disk->FsType == 12 ) return 0;
	if(fno != disk->FAT_ChainIndexFno ) {
		__ASSERT(__FILE, __LINE__, 1);
		return 0;
	}
	if( disk->FAT_ChainIndex == 0 ) {
		__ASSERT(__FILE, __LINE__, 1);
		return 0;
	}
	while(times){
		while( 1 ){
			//printf("2-%lu %lu %lu\n",G_Pre_Cluster,G_FATChainContiniousCnt,times);
			times --;
			if( times == 0 ) {
				//printf("3-%lu\n",G_Pre_Cluster);
				//printf("6-%lu %lu %lu\n",G_Pre_Cluster,G_FATChainContiniousCnt,times);
				return (G_Pre_Cluster + G_FATChainContiniousCnt);
			}
			if( G_FATChainContiniousCnt != 0 ) {
				G_FATChainContiniousCnt --;
			}
			else{
				break;
			}
		}
		if(disk->FsType == 32) {
			if( disk->FAT_ChainIndex < 4 ) {
				__ASSERT(__FILE, __LINE__, 1);
				return 0;
			}
			disk->FAT_ChainIndex -= 4;
		}
		else{
			if( disk->FAT_ChainIndex == 0 ) {
				__ASSERT(__FILE, __LINE__, 1);
				return 0;
			}
			disk->FAT_ChainIndex -= 2; 
		} 
		addr = dos.TmpBufAddr + disk->FAT_ChainIndex;
		//dumpData(16, addr, 1);
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		XBYTE[REG_Dram_IndirectPrefetch] = 0x01;	//set prefetch
		//read 2 or 4 bytes from DRAM
		while(XBYTE[REG_Dram_IndirectPrefetch] & 0x01);
		WRITE8(G_Pre_Cluster,3,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(G_Pre_Cluster,2,XBYTE[REG_Dram_IndirectRData_High]);
		//printf("FAT=%lx\n",ret32);
		if(disk->FsType == 32) {
			while(XBYTE[REG_Dram_IndirectPrefetch] & 0x01);
			WRITE8(G_Pre_Cluster,1,XBYTE[REG_Dram_IndirectRData_Low]);
			WRITE8(G_Pre_Cluster,0,XBYTE[REG_Dram_IndirectRData_High]);
		}
		else {
			WRITE8(G_Pre_Cluster,1,0);
			WRITE8(G_Pre_Cluster,0,0);
		}
		while(XBYTE[REG_Dram_IndirectPrefetch] & 0x01);
		WRITE8(G_FATChainContiniousCnt,3,XBYTE[REG_Dram_IndirectRData_Low]);
		WRITE8(G_FATChainContiniousCnt,2,XBYTE[REG_Dram_IndirectRData_High]);
		if(disk->FsType == 32) {
			while(XBYTE[REG_Dram_IndirectPrefetch] & 0x01);
			WRITE8(G_FATChainContiniousCnt,1,XBYTE[REG_Dram_IndirectRData_Low]);
			WRITE8(G_FATChainContiniousCnt,0,XBYTE[REG_Dram_IndirectRData_High]);
		}
		else {
			WRITE8(G_FATChainContiniousCnt,1,0);
			WRITE8(G_FATChainContiniousCnt,0,0);
		}
		//printf("4-%lu %lu %lu\n",G_Pre_Cluster,G_FATChainContiniousCnt,times);
	}
	//set back to next addr
	if(disk->FsType == 32) {
		disk->FAT_ChainIndex += 4;
	}
	else{
		disk->FAT_ChainIndex += 2; 
	} 
	//printf("\nfat=%lu\n",ret32);
	//printf("5-%lu %lu %lu\n",G_Pre_Cluster,G_FATChainContiniousCnt,disk->FAT_ChainIndex);
	return (G_Pre_Cluster + G_FATChainContiniousCnt);
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 DOS_FileRead(UINT8 fno, UINT32 buf, UINT32 cnt, UINT32 targetPos)
{
	//xdata UINT8 *ptr;
	xdata FILE *file;
	xdata DISK *disk;
	UINT8  DevId;
	//UINT8  z2nex;
	//UINT16 start;
	UINT16 offset;
	UINT32 curr_pos;
	UINT32 cluster, sector;
	UINT32 read_num, sector_num, tmp32;
	UINT32 i32,num;
	UINT32 z0,z1,z2,remain;
	//UINT32 t0, t1;
/* timer start tag0 */	
	//XBYTE[0x2038] |= 0x01;
	//printf("DOS_FileRead %bx %lx %lx\n",fno,buf,cnt);
	file   = (FILE *)&dos.file[fno];
	DevId  = file->DevId;
	disk   = (DISK *)&dos.disk[DevId];
	pdiskFAT_BankStartCluster = &disk->FAT_BankStartCluster;
	pdiskFAT_Bank2Cluster = &disk->FAT_Bank2Cluster;

#if DEBUG
	printf("%s %u targetPos=%lu,file->pos=%lu,cn=%lu\n", __FILE__, (unsigned short)__LINE__,targetPos,file->pos,cnt);
#endif
//printf("Pos=%lu,pos=%lu\n",targetPos,file->pos);
	curr_pos = targetPos;//file->pos;
	//exceed file size, error
	if(curr_pos >= file->size) {//fseek exceed fsize, but not data still now...
		return 0;
	}
	// desire to read more the remain size
	if (cnt+curr_pos > file->size ) {
		cnt = file->size - curr_pos;
	}

	cluster = file->CurrCluster;	//file size > 0
	i32 = file->pos >> disk->bitCluster2Byte;
	num = curr_pos >> disk->bitCluster2Byte;
	//HAL_GlobalReadGTimer(&t0);
#if 0 // Old flow
	if( num != i32 ){//if current cluster not target cluster, search target
		//printf(" n=%lu %lu \n",i32,num);
#if DEBUG
		printf("%s %u i32=%lu,num=%lu\n", __FILE__, (unsigned short)__LINE__,i32,num);
#endif
		//printf("%s %u i32=%lu,num=%lu\n", __FILE__, (unsigned short)__LINE__,i32,num);
		if( num < i32 ){// seach backward, search from position 0
			i32 = 0;
			num = curr_pos >> disk->bitCluster2Byte;
			cluster = file->FirstCluster;
		}

		if( num > i32 ){
			num -= i32;
			for(i32=0; i32<num; i32++ ) {
				//printf("%lu->",cluster);
				cluster = FAT_TableRead(DevId, cluster);
				//printf("%lu ",cluster);
				if((cluster & disk->FAT_EOFValue) == disk->FAT_EOFValue) {
					//unknown case
				}
			}
		}
	}
#else

	if( num != i32 ){//if current cluster not target cluster, search target
		// patch for quick search avi idx tbl
		if (!file->TargetCluster) {
			//printf(" n=%lu %lu \n",i32,num);
#if DEBUG
			printf("%s %u i32=%lu,num=%lu\n", __FILE__, (unsigned short)__LINE__,i32,num);
#endif
			if( num < i32 ){// seach backward, search from position 0
				#if 0
				i32 = 0;
				num = curr_pos >> disk->bitCluster2Byte;
				cluster = file->FirstCluster;
				#else
				//printf("%s %u i32=%lu,num=%lu,curr=%lu\n", __FILE__, (unsigned short)__LINE__,i32,num,file->CurrCluster);
				i32 = file->pos >> disk->bitCluster2Byte;
				num = curr_pos >> disk->bitCluster2Byte;
				tmp32 = i32 - num + 1;
				if(G_MakeFatChain_FileRead){
					cluster = DOS_FileReadFatChain(fno, tmp32);
					if( cluster != 0 ){
						num = i32;
						file->CurrCluster = cluster;
					}
					else{
						i32 = 0;
						num = curr_pos >> disk->bitCluster2Byte;
						cluster = file->FirstCluster;
					}
				}
				else{
					i32 = 0;
					num = curr_pos >> disk->bitCluster2Byte;
					cluster = file->FirstCluster;
				}
			}
#endif

			if( num > i32 ){
				num -= i32;
				for(i32=0; i32<num; i32++ ) {
					//printf("%lu->",cluster);
					cluster = FAT_TableRead(DevId, cluster);
					if(G_MakeFatChain_FileRead)
						DOS_FileReadMakeFatChain(fno,cluster,__LINE__);
					//printf("%lu ",cluster);
					if((cluster & disk->FAT_EOFValue) == disk->FAT_EOFValue) {
						//unknown case
					}
				}
			}
		}else {
			/* Set target cluster and refresh FAT table */
			cluster = file->TargetCluster;
			FAT_TableRead(DevId, cluster);
#if DEBUG
			printf("Jump to target cluster %lx\n", cluster);
#endif
		}
	}
	
#endif
	offset  = (curr_pos % disk->Cluster2Byte) >> 9;
	//HAL_GlobalReadGTimer(&t1);
	//printf("%ld\n",t1-t0);
	remain = cnt % 512;
	tmp32 = cnt >> 9;
#if DEBUG
	printf("%s %u CurrCluster=%lu,cluster=%lu,offset=%u\n", __FILE__, (unsigned short)__LINE__,file->CurrCluster,cluster,offset);
#endif
	sector = FAT_Cluster2Sector(DevId, cluster) + offset;
	if( offset ){
		z0 = disk->Cluster2Sector - offset;
		if( z0 > tmp32 ) z0 = tmp32;
		//z2nex = 1;
	}
	else{
		z0 = 0;
		//z2nex = 0;
		/*
		// if offset = 0 and curr_pos != 0 means we have to have next cluster
		if(( file->CurrCluster == file->LastCluster ) && ( curr_pos != 0 )){
			cluster = FAT_TableRead(DevId, cluster);
		}
		*/
	}
//printf("z0=%lu\n",z0);
	read_num = z0 << 9;
	num = cnt;
	num -= read_num;
	z1 = num / disk->Cluster2Byte;
//printf("z1=%lu\n",z1);
	z2 = (num - ( z1 * disk->Cluster2Byte ));
	z2 = z2 >> 9;
	if( remain ) z2 += 1;
#if DEBUG
	printf("%s %u z0=%lu,z1=%lu,z2=%lu,cnt=%lu,cluster=%lu,offset=%u\n", __FILE__, (unsigned short)__LINE__,z0,z1,z2,cnt,cluster,offset);
#endif
/* timer stop tag0 -> 400 us*/	
//printf("z0=%lu,z1=%lu,z2=%lu,cnt=%lu,cluster=%lu,offset=%u\n",z0,z1,z2,cnt,cluster,offset);
	// start non-cluster alignment
	if( z0 ){
#if DEBUG
		printf("%s %u sector=%lu,z0=%lu,buf=%lx\n", __FILE__, (unsigned short)__LINE__,sector,z0,buf);
#endif
		if( 0 == DOS_SectorRead(DevId, sector, z0, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
		buf += read_num;
		curr_pos += read_num;
		//Go to current cluster
		if(( z0 + offset ) >= disk->Cluster2Sector ){
			if((cluster > disk->MaxClusterNum-1) || (cluster < 2)){
				cluster =  EOF_FAT32;
			}
			else{
				cluster = FAT_TableRead(DevId, cluster);
				if(G_MakeFatChain_FileRead)
					DOS_FileReadMakeFatChain(fno,cluster,__LINE__);
			}
		} 
	}
	/*
	if( z0 && z1 ){
		cluster = FAT_TableRead(DevId, cluster);
	}
	*/
	sector_num = disk->Cluster2Sector;
	num = z1;
	/*
	if( num ){ 
		z2nex = 0;
	}
	*/
	while(num) {
#if DEBUG
	printf("%s %u sector=%lu,sector_num=%lu,buf=%lx\n", __FILE__, (unsigned short)__LINE__,sector,sector_num,buf);
#endif
		sector = FAT_Cluster2Sector(DevId, cluster);
		if((cluster > disk->MaxClusterNum-1) || (cluster < 2)){
			i32 =  EOF_FAT32;
		}
		else{
			i32 = FAT_TableRead(DevId, cluster);
			if(G_MakeFatChain_FileRead)
				DOS_FileReadMakeFatChain(fno,i32,__LINE__);
		}
		num --;
		if( i32 == ( cluster + 1 )) {//here will search continious cluster to read, faster
			while(num){
				if((i32 > disk->MaxClusterNum-1) || (i32 < 2)){
					tmp32 =  EOF_FAT32;
				}
				else{
					tmp32 = FAT_TableRead(DevId, i32);
					if(G_MakeFatChain_FileRead)
						DOS_FileReadMakeFatChain(fno,tmp32,__LINE__);
				}
#if DEBUG
				printf("%s %u %lu,%lu,%lu\n", __FILE__, (unsigned short)__LINE__, i32, tmp32, sector_num);
#endif
				if(((tmp32 & disk->FAT_EOFValue) == disk->FAT_EOFValue) || ( tmp32 != (i32 + 1))){
					break;
				}
				num --;
				sector_num += disk->Cluster2Sector;
				i32 = tmp32;
			}
		}
		else{
			sector_num = disk->Cluster2Sector;
		}
#if DEBUG				
		printf("%s %u sector=%lu,sector_num=%lu,z1=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,sector,sector_num,z1,cluster);
#endif
		cluster = i32;
#if DEBUG
		printf("%s %u sector=%lu,sector_num=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,sector,sector_num,cluster);
#endif
		if( 0 == DOS_SectorRead(DevId, sector, sector_num, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
		read_num = sector_num << 9;
		buf += read_num;
		curr_pos += read_num;
	}
	/*
	if( z2 && z2nex ){// check to get next cluster or not base on z0, z1
#if DEBUG
		printf("%s %u sector=%lu,z2=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,sector,z2,cluster);
#endif
		cluster = FAT_TableRead(DevId, cluster);
#if DEBUG
		printf("%s %u sector=%lu,z2=%lu,cluster=%lu\n", __FILE__, (unsigned short)__LINE__,sector,z2,cluster);
#endif
	}
	*/
	if( z2 ){
		sector = FAT_Cluster2Sector(DevId, cluster);
#if DEBUG
		printf("%s %u sector=%lu,z2=%lu,cluster=%lu,remain=%lu\n", __FILE__, (unsigned short)__LINE__,sector,z2,cluster,remain);
#endif
		if( remain ){
			if( z2 > 1 ){
				if( 0 == DOS_SectorRead(DevId, sector, (z2 - 1), buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
				read_num = ((z2 - 1) << 9);
				buf += read_num;
				sector += (z2 - 1);
			}
            else
                read_num = 0; //xian ++, fix bug 29997
			if( 0 == DOS_SectorRead(DevId, sector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
			HAL_GprmDramDma(file->buf*2, buf, remain, 1);
			buf += remain;
		}	
		else{ 
			if( 0 == DOS_SectorRead(DevId, sector, z2, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
			read_num = (z2 << 9);
			buf += read_num;
		}
		//read_num = z2 << 9;
		//buf += read_num;
		curr_pos += read_num;
		if( remain ) curr_pos = curr_pos + remain;
		//Go to current cluster
		if( z2  >= disk->Cluster2Sector ){
			if((cluster > disk->MaxClusterNum-1) || (cluster < 2)){
				cluster =  EOF_FAT32;
			}
			else{
				cluster = FAT_TableRead(DevId, cluster);
				if(G_MakeFatChain_FileRead)
					DOS_FileReadMakeFatChain(fno,cluster,__LINE__);
			}
		} 
	}
	cnt = curr_pos - targetPos;
	file->pos         = curr_pos;
	//file->LastCluster = file->CurrCluster;
	file->CurrCluster = cluster;
	file->TargetCluster = 0;
#if DEBUG
	printf("%s %u cnt=%lu,CurrCluster=%lu,pos=%lu\n", __FILE__, (unsigned short)__LINE__,cnt,file->CurrCluster,file->pos);
#endif
	return cnt;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: DOS_FileWrite                                               *
 *                                                                        *
 *  Purposes: interface of writing files into a device under FAT FS.      *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *    pfile: in/out, file descriptor.                                     *
 *    pbuf: in, pointer to a buffer with content to be written.           *
 *    cnt: in, size during this operation.                                *
 *                                                                        *
 *  Returns: actual size written into a device.                           *
 *                                                                        *
 *  See also: vfsWrite(), fatRead()                                       *
 *                                                                        *
 **************************************************************************/
#if 0
UINT32 DOS_FileWrite(UINT8 fno, UINT32 buf, UINT32 cnt)
{
	xdata FILE *file;
	xdata DISK *disk;
	UINT8  DevId, ret, continiousWr;
	UINT16 offset;
	UINT32 curr_pos, next_pos;
	UINT32 curr, next, size, tmp32;
	UINT32 sector,sector_num;
	UINT32 wrt_num, skip_num, FreeCluster;
	UINT32 z0,z1,z2,remain;
	UINT32 i32,num;

	file   = (FILE *)&dos.file[fno];
	DevId  = file->DevId;
	disk   = (DISK *)&dos.disk[DevId];

	if(file->mode=='r')	{	//can't write
		return 0;
	}
	//Assume it is not write continious
	continiousWr = 0;
	/*
	* Use pointer while mem type is "data" will be faster than struct pointer
	*/
	pdiskFAT_BankStartCluster = &disk->FAT_BankStartCluster;
	pdiskFAT_Bank2Cluster = &disk->FAT_Bank2Cluster;

	ret  = DOS_SUCCESS;
	
	/*
	* Use local variable instead of struct pointer for speed up
	*/
	size = file->size;
	curr_pos = file->pos;
	next_pos = curr_pos + cnt;

	if(!next_pos && !size)
		return 0;

	/*
	* Check the size of file skip and the size of data write
	*/
	skip_num = 0;
	if(curr_pos > size) {	//fseek exceed fsize
		curr    = (size + disk->Cluster2Byte-1) >> disk->bitCluster2Byte;
		wrt_num = ((next_pos + disk->Cluster2Byte-1) >> disk->bitCluster2Byte) - curr;
		skip_num= ((curr_pos + disk->Cluster2Byte-1) >> disk->bitCluster2Byte) - curr;
#if DEBUG
		printf("%s %u wrt=%lu, skip=%lu next_pos=%lu curr=%lu\n", __FILE__, (unsigned short)__LINE__, wrt_num, skip_num,next_pos,curr);
#endif
		if(wrt_num > disk->FreeCluster) {
			printf("DISK_FULL...\n");
			return DOS_DISK_FULL;
		}
	}
	else if( curr_pos < size ){ // fseek backward
		num = curr_pos >> disk->bitCluster2Byte;
		curr = file->FirstCluster;
#if DEBUG
	printf("%s %u num=%lu,curr=%lu\n", __FILE__, (unsigned short)__LINE__, num,curr);
#endif
		for(i32=0; i32<num; i32++ ) {
			//printf("%lu->",cluster);
			curr = FAT_TableRead(DevId, curr);
			//printf("%lu ",cluster);
			if((curr & disk->FAT_EOFValue) == disk->FAT_EOFValue) {
				//unknown case
			}
		}
		file->CurrCluster = curr;
#if DEBUG
	printf("%s %u CurrCluster=%lu,curr_pos=%lu\n", __FILE__, (unsigned short)__LINE__, file->CurrCluster,curr_pos);
#endif
	}
	else { // continious write
#if DEBUG
	printf("%s %u CurrCluster=%lu,curr_pos=%lu\n", __FILE__, (unsigned short)__LINE__, file->CurrCluster,curr_pos);
#endif
		continiousWr = 1;
	}
#if DEBUG
	printf("%s %u CurrCluster=%lu,skip_num=%lu\n", __FILE__, (unsigned short)__LINE__, file->CurrCluster,skip_num);
#endif
	/*
	* allocate & update FAT
	*/
	while(skip_num) {	
		FreeCluster = FAT_FreeClusterGet(DevId, skip_num, &next);
		if( FreeCluster == 0 ) return dosRwErr(__FILE,(unsigned short)__LINE__);
		skip_num -= FreeCluster;
		//size    += (UINT32)FreeCluster << disk->bitCluster2Byte;
		while(FreeCluster) {
			if(curr==0) {
				//FAT_TableWrite(DevId, next, EOF_FAT32);
				file->FirstCluster= next;
				file->CurrCluster = next;
				file->LastCluster = next;
			}
			else {
				FAT_TableWrite(DevId, curr, next);
			}
			curr = next;
			next++;
			FreeCluster--;
		}
		FAT_TableWrite(DevId, curr, disk->FAT_EOFValue);
		file->CurrCluster = curr;
		file->LastCluster = file->CurrCluster;
	}

#if DEBUG	
	printf("%s %u CurrCluster=%lu\n", __FILE__, (unsigned short)__LINE__, file->CurrCluster);
#endif
	curr = file->CurrCluster;
	// Here deal the 1st non-cluster alignment sectors
	offset  = (curr_pos % disk->Cluster2Byte) >> 9;
	remain = cnt % 512;
	tmp32 = cnt >> 9;
	sector = FAT_Cluster2Sector(DevId, curr) + offset;
#if DEBUG
	printf("%s %u CurrCluster=%lu,cluster=%lu,offset=%u\n", __FILE__, (unsigned short)__LINE__,file->CurrCluster,cluster,offset);
#endif
	if( offset ){
		z0 = disk->Cluster2Sector - offset;
		if( z0 > tmp32 ) z0 = tmp32;
	}
	else{
		z0 = 0;
	}
#if DEBUG
	printf("z0=%lu\n",z0);
#endif
	wrt_num = z0 << 9;
	num = cnt;
	num -= wrt_num;
	z1 = num >> disk->bitCluster2Byte;
#if DEBUG
	printf("z1=%lu\n",z1);
#endif
	z2 = (num - ( z1 << disk->bitCluster2Byte ));
	z2 = z2 >> 9;
	if( remain ) z2 += 1;
#if DEBUG
	printf("%s %u z0=%lu,z1=%lu,z2=%lu,cnt=%lu,curr=%lu,offset=%u\n", __FILE__, (unsigned short)__LINE__,z0,z1,z2,cnt,curr,offset);
#endif
	// start non-cluster alignment
	if( z0 ){//When this case --> write back AVI hdr ( fseek backward )
#if DEBUG
		printf("%s %u sector=%lu,z0=%lu,buf=%lx\n", __FILE__, (unsigned short)__LINE__,sector,z0,buf);
#endif
		if( 0 == DOS_SectorWrite(DevId, sector, z0, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
		buf += wrt_num;
		curr_pos += wrt_num;
		//Go to current cluster
		if(( z0 + offset ) >= disk->Cluster2Sector ){
			if((curr > disk->MaxClusterNum-1) || (curr < 2)){
				curr =  EOF_FAT32;
			}
			else{
				curr = FAT_TableRead(DevId, curr);
			}
#if DEBUG
	printf("%s %u curr=%lu\n", __FILE__, (unsigned short)__LINE__, curr);
#endif
			if((curr & disk->FAT_EOFValue) == disk->FAT_EOFValue){
				//Write data over 1 cluster and the curr cluster is EOF
				continiousWr = 1;
				curr = file->CurrCluster;
			}
		} 
	}
	//sector_num = disk->Cluster2Sector;
	num = z1;
	// start cluster alignment
	while( num ) {
		sector = FAT_Cluster2Sector(DevId, curr);
#if DEBUG
	printf("%s %u sector=%lu,curr=%lx\n", __FILE__, (unsigned short)__LINE__, sector, curr);
#endif
		wrt_num = num;
		if(disk->FreeCluster < num) {		//Should be checked by DOS_FileWrite()
			wrt_num = disk->FreeCluster;
		}
		if( continiousWr ){
			FreeCluster = FAT_FreeClusterGet(DevId, wrt_num, &next);// only return continious free block
#if DEBUG
	printf("%s %u FreeCluster=%lu,next=%lx\n", __FILE__, (unsigned short)__LINE__, FreeCluster,next);
#endif
			if( FreeCluster == 0 ) return dosRwErr(__FILE,(unsigned short)__LINE__);
			sector_num = FreeCluster << disk->bitCluster2Sector;
			sector = FAT_Cluster2Sector(DevId, next); //start sector
			while(FreeCluster) {
				if(curr == 0){
					file->FirstCluster= next;
					file->CurrCluster = next;
					file->LastCluster = next;
				}
				else{
					FAT_TableWrite(DevId, curr, next);
				}
				curr = next;
				next++;
				FreeCluster--;
			}
			// FAT EOF
			FAT_TableWrite(DevId, curr, disk->FAT_EOFValue);
			file->CurrCluster = curr;
			file->LastCluster = file->CurrCluster;
		}
		else{ // if write data to exist cluster, fseek backward
			sector_num <<= disk->bitCluster2Sector;
			sector = FAT_Cluster2Sector(DevId, curr); //start sector
			if((curr > disk->MaxClusterNum-1) || (curr < 2)){
				curr =  EOF_FAT32;
			}
			else{
				curr = FAT_TableRead(DevId, curr);
			}
		}
#if DEBUG
	printf("%s %u sector=%lu,sector_num=%lu,wrt_num=%lu,num=%lu,buf=%lx\n", __FILE__, (unsigned short)__LINE__, sector, sector_num, wrt_num, num, buf);
#endif
		if( 0 == DOS_SectorWrite(DevId, sector, sector_num, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
		wrt_num = sector_num << 9;
		buf += wrt_num;
		curr_pos += wrt_num;
		num -= (sector_num >> disk->bitCluster2Sector);
	}
	if( z2 && continiousWr ){//Get 1 free cluster for z2 segment and next for EOF
		FreeCluster = FAT_FreeClusterGet(DevId, 1, &next);// only return continious free block
		if( FreeCluster == 0 ) return dosRwErr(__FILE,(unsigned short)__LINE__);
		if(curr == 0){
			file->FirstCluster= next;
			file->CurrCluster = next;
			file->LastCluster = next;
		}
		FAT_TableWrite(DevId, curr, next);
		curr = next;
		// FAT EOF
		FAT_TableWrite(DevId, curr, disk->FAT_EOFValue);
		file->CurrCluster = curr;
		file->LastCluster = file->CurrCluster;
	}
	if( z2 ){
		sector = FAT_Cluster2Sector(DevId, curr);
#if DEBUG
		printf("%s %u sector=%lu,z2=%lu,curr=%lu,remain=%lu\n", __FILE__, (unsigned short)__LINE__,sector,z2,curr,remain);
#endif
		if( remain ){
			if( z2 > 1 ){
				if( 0 == DOS_SectorWrite(DevId, sector, (z2 - 1), buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
				wrt_num = ((z2 - 1) << 9);
				buf += wrt_num;
				sector += (z2 - 1);
			}
			if( 0 == DOS_SectorRead(DevId, sector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
//dumpData(512, file->buf ,1);
			HAL_GprmDramDma(buf, file->buf*2, remain, 1);
//dumpData(512, file->buf ,1);
			if( 0 == DOS_SectorWrite(DevId, sector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
			buf += remain;
			sector ++;
		}	
		else{ 
			if( 0 == DOS_SectorWrite(DevId, sector, z2, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
			wrt_num = (z2 << 9);
			buf += wrt_num;
			sector += z2;
		}
		curr_pos += wrt_num;
		if( remain ) curr_pos = curr_pos + remain;
	}
	cnt = curr_pos - file->pos;
	file->CurrCluster = curr;
	file->pos = curr_pos;
	file->size= (next_pos >= size) ? next_pos : size;
#if DEBUG
printf("%s %u size=%lu pos=%lu, c=%lu\n", __FILE__, (unsigned short)__LINE__,file->size, file->pos,curr);
#endif
	return cnt;
}

#else
#pragma OT(8, SPEED)
UINT32 DOS_FileWrite(UINT8 fno, UINT32 buf, UINT32 cnt)
{
	xdata FILE *file;
	xdata DISK *disk;
	UINT8  DevId, ret;
	UINT16 start, offset;
	UINT32 curr_pos, next_pos;
	UINT32 curr, next, size, tmp32;
	UINT32 sector,sector_num;
	UINT32 wrt_num, skip_num, FreeCluster;
	UINT8 needMoreFree = 0;
#ifdef FAT_NEW	
	UINT32 addr;
#endif

	file   = (FILE *)&dos.file[fno];
	DevId  = file->DevId;
	disk   = (DISK *)&dos.disk[DevId];
	//printf("DOS_FileWrite %bx %lx %lx\n",fno,cnt,file->pos);

	if(file->mode=='r')	{	//can't write
//printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
		return 0;
	}
	/*
	* Use pointer while mem type is "data" will be faster than struct pointer
	*/
	pdiskFAT_BankStartCluster = &disk->FAT_BankStartCluster;
	pdiskFAT_Bank2Cluster = &disk->FAT_Bank2Cluster;

	ret  = DOS_SUCCESS;
	
	/*
	* Use local variable instead of struct pointer for speed up
	*/
	size = file->size;
	curr_pos = file->pos;
	next_pos = curr_pos + cnt;

	if(!next_pos && !size)
		return 0;

	/*
	* Check the size of file skip and the size of data write
	*/
	skip_num = 0;
	if(curr_pos > size) {	//fseek exceed fsize
		curr    = (size + disk->Cluster2Byte-1) >> disk->bitCluster2Byte;
		wrt_num = ((next_pos + disk->Cluster2Byte-1) >> disk->bitCluster2Byte) - curr;
		skip_num= ((curr_pos + disk->Cluster2Byte-1) >> disk->bitCluster2Byte) - curr;
#if DEBUG
		printf("%s %u wrt=%lu, skip=%lu next_pos=%lu curr=%lu\n", __FILE__, (unsigned short)__LINE__, wrt_num, skip_num,next_pos,curr);
#endif
		if(wrt_num > disk->FreeCluster) {
			printf("DISK_FULL...\n");
			return DOS_DISK_FULL;
		}
	}
	curr = file->CurrCluster;
#if DEBUG
	printf("%s %u CurrCluster=%lu,skip_num=%lu\n", __FILE__, (unsigned short)__LINE__, file->CurrCluster,skip_num);
#endif

	/*
	* allocate & update FAT
	*/
	while(skip_num) {	
		FreeCluster = FAT_FreeClusterGet(DevId, skip_num, &next);
		if( FreeCluster == 0 ) return dosRwErr(__FILE,(unsigned short)__LINE__);
		skip_num -= FreeCluster;
		//size    += (UINT32)FreeCluster << disk->bitCluster2Byte;
		while(FreeCluster) {
			if(curr==0) {
				//FAT_TableWrite(DevId, next, EOF_FAT32);
				file->FirstCluster= next;
				file->CurrCluster = next;
				file->LastCluster = next;
			}
			else {
				FAT_TableWrite(DevId, curr, next);
			}
			curr = next;
			next++;
			FreeCluster--;
		}
		FAT_TableWrite(DevId, curr, disk->FAT_EOFValue);
		file->CurrCluster = curr;
		file->LastCluster = file->CurrCluster;
	}

#if	1
#if DEBUG
	printf("%s %u CurrCluster=%lu\n", __FILE__, (unsigned short)__LINE__, file->CurrCluster);
#endif
	curr = file->CurrCluster;
	
	while(cnt) {
		/*
		* current position calculation (size of sector)
		*/
		offset = (curr_pos >> 9) & (disk->Cluster2Sector-1);
		sector = FAT_Cluster2Sector(DevId, curr);
		sector = sector + offset;
#if DEBUG
		printf("%s %u cnt=%lu curr=%lu pos=%lu offset=%u sector=%lu buf=%08lx\n", \
			__FILE__, (unsigned short)__LINE__,
			cnt, curr, curr_pos,offset,sector, buf);
#endif
		/*
		* This deal the middle part cluster alignment	
		*/
		if((curr_pos & (disk->Cluster2Byte-1))==0 ) {//need more space -> if position is alignment to cluster
			if( curr_pos >= file->size ){
				wrt_num = (cnt + disk->Cluster2Byte-1) >> disk->bitCluster2Byte;// need total cluster number
				if(disk->FreeCluster < wrt_num) {		//Should be checked by DOS_FileWrite()
					wrt_num = disk->FreeCluster;
				}
				FreeCluster = FAT_FreeClusterGet(DevId, wrt_num, &next);// only return continious free block
				if( FreeCluster == 0 ) return dosRwErr(__FILE,(unsigned short)__LINE__);
				needMoreFree = 0;
				if( wrt_num > FreeCluster ){
					needMoreFree = 1;
				}		
#if DEBUG
				printf("%s %u wrt_num=%lu FreeCluster=%lu next=%lu\n", \
					__FILE__, (unsigned short)__LINE__,wrt_num,FreeCluster,next);
#endif
				sector = FAT_Cluster2Sector(DevId, next); //start sector
				sector_num  = (UINT16)FreeCluster << disk->bitCluster2Sector;// total sector number
#if DEBUG
				printf("%s %u sector=%lu n=%lu buf=%lx\n", __FILE__, (unsigned short)__LINE__,\
						sector, sector_num, buf/2);
#endif
				wrt_num = cnt >> 9;//512 bytes
				if(sector_num > wrt_num)
					sector_num = wrt_num;//maximum 1 (cluster sector - 1)
				// Update FAT table for coming writing data
//dbgTimerStart();
#ifdef FAT_NEW				
				while(FreeCluster) {
					if(curr==0) {
						//FAT_TableWrite(DevId, next, EOF_FAT32);
						//printf("<%lu->EOF> ", next);
						file->FirstCluster= next;
						file->CurrCluster = next;
						file->LastCluster = next;
					}
					else {
						FAT_TableWrite(DevId, curr, next);
						//printf("<%lu->%lu> ", curr,next);
					}
					curr = next;
					next++;
					FreeCluster--;
					if (curr) {
						break;
					}
				}
				/* set dram write start address */
				addr = disk->FAT_BufAddr + ((curr - *pdiskFAT_BankStartCluster) << disk->FsShift);
				XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
				XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
				XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
				XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
				
				if (disk->bitCluster2Sector == 0) {
				//if (0) {
					while(FreeCluster) {
							UINT16 DirtySector;
							UINT32 current, bank;

							current = curr;
							if( current < *pdiskFAT_BankStartCluster || \
								(current - *pdiskFAT_BankStartCluster) >= *pdiskFAT_Bank2Cluster ){//xian ++ 20101113
								//current = current + disk->FAT_BankStartCluster;
								bank = current >> disk->FAT_bitBank2Cluster;
								FAT_BankSwitch(DevId, bank, disk->FAT_BufAddr);
								current = current - disk->FAT_BankStartCluster;
								addr = disk->FAT_BufAddr + (current << disk->FsShift);
								//addr = disk->FAT_BufAddr + current; /* disk->FsShift  == 0 */
								XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
								XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
								XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
								XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
							} else {
								current = current - *pdiskFAT_BankStartCluster;
							}
							while(XBYTE[REG_Dram_IndirectPrefetch]);
							XBYTE[REG_Dram_IndirectRData_Low] = READ8(next,3);
							XBYTE[REG_Dram_IndirectRData_High] = READ8(next,2);
							if(disk->FsType==32) {
								while(XBYTE[REG_Dram_IndirectPrefetch]);
								XBYTE[REG_Dram_IndirectRData_Low] = READ8(next,1);
								XBYTE[REG_Dram_IndirectRData_High] = READ8(next,0);
							}
							current <<= disk->FsShift;
							WRITE8(DirtySector,0,READ8(current,1));
							WRITE8(DirtySector,1,READ8(current,2));

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
						//printf("<%lu->%lu> ", curr,next);	
						curr = next;
						next++;
						FreeCluster--;
					}
				} else {
					while(FreeCluster) {
						FAT_TableWrite_Con(DevId, curr, next);
						//FAT_TableWrite(DevId, curr, next);
						//printf("<%lu->%lu> ", curr,next);	
						curr = next;
						next++;
						FreeCluster--;
					}
				}
#else
				while(FreeCluster) {
					if(curr==0) {
						//FAT_TableWrite(DevId, next, EOF_FAT32);
						//printf("<%lu->EOF> ", next);
						file->FirstCluster= next;
						file->CurrCluster = next;
						file->LastCluster = next;
					}
					else {
						FAT_TableWrite(DevId, curr, next);
						//printf("<%lu->%lu> ", curr,next);
					}
					curr = next;
					next++;
					FreeCluster--;
				}				
#endif
//dbgTimerStop();
				// FAT EOF
				FAT_TableWrite(DevId, curr, disk->FAT_EOFValue);
				file->CurrCluster = curr;
				file->LastCluster = file->CurrCluster;
						//printf("<%lu->EOF> ", curr);
				wrt_num = sector_num << 9;// bytes numbers
				if(sector_num) {
					if(buf & 0x01) {	//ODD ADDR
#if DEBUG
						printf("%s %u sector=%lu n=%lu buf=%lx\n", __FILE__, (unsigned short)__LINE__,\
								sector, sector_num, buf/2);
#endif
						if((file->CurrSector != -1) && (file->status =='w')) {
							if( 0 == DOS_SectorWrite(DevId, file->CurrSector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
							file->CurrSector = -1;
						}
						tmp32 = sector_num;
						while(tmp32) {
							HAL_GprmDramDma(buf, file->buf*2, 512, 1);
							if( 0 == DOS_SectorWrite(DevId, sector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
							buf += 512;
							sector++;
							tmp32--;
						}
					}
					else { // WORD address will run here
#if DEBUG
						printf("%s %u sector=%lu n=%lu buf=%lx\n", __FILE__, (unsigned short)__LINE__,\
								sector, sector_num, buf/2);
#endif
						if( 0 == DOS_SectorWrite(DevId, sector, sector_num, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
						buf += wrt_num;
					}
				}
#if DEBUG
printf("%s %u cnt=%lu curr_pos=%lu\n", __FILE__, (unsigned short)__LINE__,cnt,curr_pos);
#endif
				cnt      -= wrt_num;//sector_num << 9;
				curr_pos += wrt_num;
				sector   += sector_num;
				offset    = sector_num & (disk->Cluster2Sector-1);// check if cluster alignment
#if DEBUG
printf("%s %u cnt=%lu curr_pos=%lu\n", __FILE__, (unsigned short)__LINE__,cnt,curr_pos);
#endif
			}
			else{// Here deal seek write, for AVI write back
				sector_num  = (cnt+511) >> 9; //sector alignment
				if(sector_num > disk->Cluster2Sector ){
					sector_num = disk->Cluster2Sector;//maximum 1 (cluster sector - 1)
				}
				sector = FAT_Cluster2Sector(DevId, curr); //start sector
#if DEBUG
				printf("%s %u wrt_num=%lu sector=%lu sector_num=%lu,curr=%lu\n", \
					__FILE__, (unsigned short)__LINE__,wrt_num,sector,sector_num,curr);
#endif
				wrt_num = sector_num << 9; //xian ++, #41693
				next = curr; // dont change curr
				
				while(cnt){ 
#if DEBUG
					printf("%s %u wrt_num=%lu sector=%lu sector_num=%lu,curr=%lu\n", \
						__FILE__, (unsigned short)__LINE__,wrt_num,sector,sector_num,curr);
#endif
					if( 0 == DOS_SectorWrite(DevId, sector, sector_num, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
					#if 0	
					dumpData(512, buf/2, 1);
					#endif
					buf += (sector_num * 512L);
					//sector += sector_num;		//xian --,#41693
					//wrt_num = sector_num << 9;
					if( cnt > wrt_num ){
						cnt -= wrt_num;

						//xian ++, dont change curr, #41693
						if(cnt < wrt_num){
							sector_num  = (cnt+511) >> 9;
							wrt_num = sector_num << 9;
						}
						next = FAT_TableRead(DevId, next); //next cluster
						sector = FAT_Cluster2Sector(DevId, next);	
					}
					else{
						cnt = 0;
					} 

				}
			}
		}

		if( needMoreFree ){
			/* still has data cluster with alignment but not free cluster continious */
#if DEBUG
			printf("%s %u reamain cluster=%lu\n", \
				__FILE__, (unsigned short)__LINE__,(cnt + disk->Cluster2Byte-1) >> disk->bitCluster2Byte);
#endif
			continue;
		}

		// This part deal the start and end part that is not cluster alignment	
		if(cnt==0) break;

		start = curr_pos & 511;// check 512 aglinment
#if DEBUG
printf("%s %u start=%u curr_pos=%lu cnt=%lu\n", __FILE__, (unsigned short)__LINE__,start,curr_pos,cnt);
#endif
		// if still has over cluster size data, should not enter here	
		if(start==0 && cnt>=512)  {//remain sectors, sector alignment but not cluster alignment
			sector_num = disk->Cluster2Sector - offset;
			if(sector_num > (cnt>>9)) {
				sector_num = cnt >> 9;
			}
			wrt_num = sector_num << 9;
			if(buf & 0x01) {	//ODD ADDR
#if DEBUG
printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
#endif
				if((file->CurrSector != -1) && (file->status =='w')) {
					if( 0 == DOS_SectorWrite(DevId, file->CurrSector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
					file->CurrSector = -1;
				}
				tmp32 = sector_num;
				while(tmp32) {
					HAL_GprmDramDma(buf, file->buf*2, 512, 1);
					if( 0 == DOS_SectorWrite(DevId, sector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
					buf += 512;
					sector++;
					tmp32--;
				}
			}
			else {
#if DEBUG
				printf("%s %u sector=%lu n=%lu buf=%lu\n", __FILE__, (unsigned short)__LINE__,\
							sector, sector_num, buf/2);
#endif
				if( 0 == DOS_SectorWrite(DevId, sector, sector_num, buf/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
				buf += wrt_num;
			}
			cnt      -= wrt_num;
			curr_pos += wrt_num;
		}
		//else if(start || cnt<512) {
		else {
			if(file->status =='w') {
				// There is data inside dos buffer still un-write
				if((file->CurrSector != -1) && (file->CurrSector != sector)) {
#if DEBUG
printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
#endif
					if( 0 == DOS_SectorWrite(DevId, file->CurrSector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
				}
			}
			// There is data inside dos buffer still un-write
			if((file->CurrSector != sector) || (file->status !='w')) {
#if DEBUG
printf("%s %u sector=%08lu buf=%lu cnt=%lu\n", __FILE__, (unsigned short)__LINE__,sector,file->buf,cnt);
#endif
				if( 0 == DOS_SectorRead(DevId, sector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
				//cmdMemDump("d 0x94300 +0x40");
			}
			wrt_num = (cnt > 512-start) ? (512-start) : cnt;
			HAL_GprmDramDma(buf, file->buf*2+start, wrt_num, 1);
			file->status = 'w';
			file->CurrSector = sector;
			buf      += wrt_num;
			cnt      -= wrt_num;
			curr_pos += wrt_num;
			if( 0 == DOS_SectorWrite(DevId, file->CurrSector, 1, file->buf)) return dosRwErr(__FILE,(unsigned short)__LINE__);
		}
#if DEBUG
printf("%s %u sector=%08lu curr_pos=%lu cnt=%lu\n", __FILE__, (unsigned short)__LINE__,sector,curr_pos,cnt);
#endif
	}
#endif
	cnt = curr_pos - file->pos;
	file->CurrCluster = curr;
	file->pos = curr_pos;
	file->size= (next_pos >= size) ? next_pos : size;
#if DEBUG
printf("%s %u size=%lu pos=%lu, c=%lu\n", __FILE__, (unsigned short)__LINE__,file->size, file->pos,curr);
#endif
	//file->CurrCluster = next;
	return cnt;
}
#endif

#if 0
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
UINT32 DOS_FreeSpaceGet(UINT8 DevId)
{
	UINT32 size;
	xdata DISK *disk;
	disk   = (DISK *)&dos.disk[DevId];
	//printf("DevId=%bx %lx %bx\n",DevId,disk->FreeCluster,disk->bitCluster2Sector);
	size = disk->FreeCluster << disk->bitCluster2Sector;// to sector number		
	//printf("size=%lx\n",size);
	size = size >> 1;// to K Bytes
	return size;		
}
UINT32 DOS_FileAlignGet(UINT8 fno)
{
	xdata FILE *file;
	xdata DISK *disk;
	UINT32 next;
	UINT32 sector;
	
	file   = (FILE *)&dos.file[fno];
	disk   = (DISK *)&dos.disk[file->DevId];

//	disk->bPrefetch = 1;

	FAT_FreeClusterGet(file->DevId, 0, &next);// only return continious free block

	sector = FAT_Cluster2Sector(file->DevId, next); //start sector	

	printf("next %lx, sector %lx\n", next, sector);
	MTD_SelfShiftL8(sector);
	sector <<= 1;
	return sector;
	
}
#endif

