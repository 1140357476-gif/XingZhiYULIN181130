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
#include "cardopt.h"
#include "hal.h"
#include "smcfunc.h"
#include "smcvar.h"
#include "dramfunc.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "reg_def.h"

#define __FILE	__FILE_ID_DOS_FORMAT__
#define	DEBUG	0
#define DOS_FMT_LABLE 1

#define FAT_LableStrRst() {\
	memset(dosFatLableStr, 0, sizeof(dosFatLableStr));\
	dosFatLableEn = 0;\
}

extern DOS dos;
extern UINT32 dos_var_start;
extern xdata UINT32 SD_PhySectors;
extern xdata UINT32 diskPhySectors;
extern xdata UINT32 DOSRAM_PhySectors;

UINT32 fatFormat(UINT32 devId,UINT32 opt);
UINT8 FAT_LabelAdd(UINT8 DevId);

code chsTbl_t sdChsTbl[] = {
	{   2L*2048L,   2, 16},	//TotalSectors, Heads, sectPerTrack
	{  16L*2048L,   2, 32},
	{  32L*2048L,   4, 32},
	{ 128L*2048L,   8, 32},
	{ 256L*2048L,  16, 32},
	{ 504L*2048L,  16, 63},
	{1008L*2048L,  32, 63},
	{2016L*2048L,  64, 63},
	{2048L*2048L, 128, 63}
};

code boundaryUnit_t boundaryUnit[] = {
	{   8L*2048L, 16,  16},	//TotalSectors,sectPerClust, boundaryUnit
	{  64L*2048L, 32,  32},
	{ 256L*2048L, 32,  64},
	{1024L*2048L, 32, 128},
	{2048L*2048L, 64, 128}
};

static UINT8 dosFatLableStr[13];
static UINT8 dosFatLableEn;

UINT8 FAT_BoundaryUnitGet(UINT8 DevId)
{
	DISK *disk;
	UINT8 i8, j8;
	disk = (DISK *)&dos.disk[DevId];
	j8 = sizeof(boundaryUnit) / sizeof(boundaryUnit_t);
	for(i8=0; i8<j8; i8++) {
		//printf("i=%bu t=%lu s=%lu\n", i8,boundaryUnit[i8].TotalSectors,disk->PhySectors);
		if ( boundaryUnit[i8].TotalSectors >= disk->PhySectors) {
			break;
		}
	}
	if(i8 == j8) {
		i8--;
	}
#if	DEBUG
	printf("j8=%bu i8=%bu size=%u %u u=%bu\n", j8, i8, \
		(UINT16)sizeof(boundaryUnit), (UINT16)sizeof(boundaryUnit_t),boundaryUnit[i8].boundaryUnit);
#endif
	return boundaryUnit[i8].boundaryUnit;
}

void FAT_ChsGet(UINT8 DevId, UINT8 *Heads, UINT8 *SectorPerTrack)
{
	DISK *disk;
	UINT8 i8, j8;
	disk = (DISK *)&dos.disk[DevId];
	j8 = sizeof(sdChsTbl)/sizeof(chsTbl_t);
	for(i8=0; i8<j8; i8++)
	{
		if(sdChsTbl[i8].TotalSectors >= disk->PhySectors) {
			break;
		}
	}
	if(i8 == j8)
	{
		i8--;
	}
	*Heads          = sdChsTbl[i8].nrHead;
	*SectorPerTrack = sdChsTbl[i8].sectPerTrack;
	return;
}

UINT32 FAT_PbsCal16(UINT8 DevId)
{
	UINT32 nrSects;
	UINT32 fatLen;
	UINT16 nrSectsInSysArea;
	UINT32 pbsStartSect;
	UINT8  bu;
	UINT32 i;
	UINT16 MsDosTime, MsDosDate;
	UINT8 nrHeads, sectPerClust,sectPerTrack;
	x86_MBS *mbs;
	x86_PBS *pbs;
	DISK *disk;

	disk = (DISK *)&dos.disk[DevId];
	mbs = (x86_MBS *)&dos.share;
	pbs = (x86_PBS *)(&dos.share+512);

	nrSects = disk->PhySectors;
	/* Step1: determine the value of sectors/cluster */
	if((nrSects > 2048L*2048L)/* || (nrSects <= 2*2048L+256)*/) {
		printf("***nrSects=%lu > 2GB? or < 2.2MB?***\n", nrSects);
		return -1;
	}

	if(nrSects >= 2048L*2048L ) {
		sectPerClust = 128;
	}
	else if (nrSects >= 1024L*2048L ) {
		sectPerClust = 64;
	}
	else if (nrSects >= 128L*2048L ) {
		sectPerClust = 32;
	}
	else if (nrSects >= 64*2048L ) {
		sectPerClust = 16;
	}
	else if (nrSects >= 32*2048L ) {
		sectPerClust = 8;
	}
	else if (nrSects >= 8*2048L ) {
		sectPerClust = 8;
		//printf("%s %u nrSects=%lu sectPerClust=%bu\n", __FILE__, (unsigned short)__LINE__, nrSects,sectPerClust);
	}
	else if (nrSects >= 4*2048L ) {
		sectPerClust = 8;
		//printf("%s %u nrSects=%lu sectPerClust=%bu\n", __FILE__, (unsigned short)__LINE__, nrSects,sectPerClust);
	}
	else if (nrSects >= 2*2048L ) {
		sectPerClust = 4;
		//printf("%s %u nrSects=%lu sectPerClust=%bu\n", __FILE__, (unsigned short)__LINE__, nrSects,sectPerClust);
	}
	else {
		sectPerClust = 2;
	}

	/* Step2: determine the value of FAT bits */
	/* Step3: determine the length of FAT table in unit of sectors */
	if ( nrSects > 16*2048L ) {
		disk->FsType = 16;
		strncpy(&pbs->u.fat16only.fsName[0], "FAT16   ", 8);
	}
	else {
		disk->FsType = 12;
		strncpy(&pbs->u.fat16only.fsName[0], "FAT12   ", 8);
	}

	fatLen = (nrSects / sectPerClust) * disk->FsType;
	fatLen = (fatLen + (512L * 8 - 1)) / (512L * 8);

	/* Step4: determine the size of FAT system area in unit of sectors */
	nrSectsInSysArea = 32 * 512 / 512; /* (FDB X root directory entries) / sector size */
	nrSectsInSysArea = nrSectsInSysArea + (2 * fatLen) + 1; /* 1: reserved sector */
	if(DevId == DRIVE_SD) {
		bu = FAT_BoundaryUnitGet(DevId);
		FAT_ChsGet(DevId, &nrHeads, &sectPerTrack);
	}
	else {
		bu = sectPerClust;
		nrHeads = 0xFF;
		sectPerTrack = sectPerClust;
	}
//printf("%s %u nrSectsInSysArea=%u, bu=%bu\n", __FILE__, (unsigned short)__LINE__, nrSectsInSysArea, bu);
	i = (nrSectsInSysArea + bu) / bu;
//printf("%s %u i=%lu\n", __FILE__, (unsigned short)__LINE__, i);
	//printf_4("nrSectsInSysArea=%d, i=%d, bu=%d\n", nrSectsInSysArea, i, bu);
	pbsStartSect = (i * bu) - nrSectsInSysArea;
	if ( pbsStartSect < bu ) {
		pbsStartSect = pbsStartSect + bu;
	}
	//printf("pbsStartSect=%lx\n", pbsStartSect);
	//pbsStartSect = pbsStartSect + 0x80;//512MB
	//pbsStartSect = pbsStartSect + 0x08;//64MB;
	//printf("pbsStartSect=%lx\n", pbsStartSect);
	nrSects = nrSects - pbsStartSect; /* nrSects = partion size */
//while(1);
	pbs->sectPerClust = sectPerClust;
	pbs->reserved[0] = 0x01;
	pbs->reserved[1] = 0x00;
	pbs->dirEntries[0] = 0x00;
	pbs->dirEntries[1] = 0x02;
	if ( nrSects < 0x0000ffff ) {
		pbs->sects[0] = READ8(nrSects,3);
		pbs->sects[1] = READ8(nrSects,2);
	}
	pbs->media = 0xf8;
	pbs->fatLen[0] = READ8(fatLen,3);
	pbs->fatLen[1] = READ8(fatLen,2);
	//pbs->sectsPerTrack[1] = 0;
	pbs->sectsPerTrack[0] = (sectPerTrack >= 64) ? 63 : sectPerTrack;	/* 6 bits only */
	pbs->heads[0] = nrHeads; /* Pick the max value (who care?) */
	//pbs->heads[1] = 0;
	pbs->hidden[0] = READ8(pbsStartSect,3);
	pbs->hidden[1] = READ8(pbsStartSect,2);
	pbs->hidden[2] = READ8(pbsStartSect,1);
	pbs->hidden[3] = READ8(pbsStartSect,0);
	if ( nrSects > 0x0000ffff ) {
		pbs->totalSects[0] = READ8(nrSects,3);
		pbs->totalSects[1] = READ8(nrSects,2);
		pbs->totalSects[2] = READ8(nrSects,1);
		pbs->totalSects[3] = READ8(nrSects,0);
	}
	pbs->u.fat16only.diskNum   = 0x80;
	//pbs->u.fat16only.reserved1 = 0;
	pbs->u.fat16only.extCode   = 0x29;
	//strncpy(&pbs->u.fat16only.fsName[0], "FAT16   ", 8);

	/* The field of volume ID should be different each time it is formatted */
	DOS_FileCreateTimeGet((pDateStc_t)&dos.date);
	dos.date.Year += (2000-1980);	//RTC -> M$ time << 26;	//bit[31:26]
	MsDosTime = (dos.date.Hour << 11) + (dos.date.Minute << 5) + (dos.date.Second >> 1);
	MsDosDate = (dos.date.Year << 9) + (dos.date.Month << 5) + (dos.date.Day);
	pbs->u.fat16only.volId[0] = READ8(MsDosTime,0);
	pbs->u.fat16only.volId[1] = READ8(MsDosTime,1);
	pbs->u.fat16only.volId[2] = READ8(MsDosDate,0);
	pbs->u.fat16only.volId[3] = READ8(MsDosDate,1);
	return pbsStartSect;
}

UINT32 FAT_PbsCal32(UINT8 DevId)
{
	x86_MBS *mbs;
	x86_PBS *pbs;
	DISK *disk;
	UINT16 MsDosTime, MsDosDate;
	UINT8  sectPerClust;
	UINT32 nrSects;
	UINT32 fatLen, i32;
	UINT32 nrSectsInSysArea;
	UINT32 pbsStartSect;

	disk = (DISK *)&dos.disk[DevId];
	mbs = (x86_MBS *)&dos.share;
	pbs = (x86_PBS *)(&dos.share+512);

#if	0
	struct tm *ptime;
	UINT16 time, date;
#endif

	nrSects = disk->PhySectors;
	/* Step1: determine the value of sectors/cluster */
	sectPerClust = disk->PhySectors >> (11+6);
//printf("%s %u sectPerClust=%bu\n", __FILE__, (unsigned short)__LINE__, sectPerClust);
	i32 = 1;
	while(i32 < sectPerClust) {
		i32 <<= 1L;
	}
	sectPerClust = (i32 > 64L) ? 64 : i32;

	/* Step2: determine the value of FAT bits */
	/* Step3: determine the length of FAT table in unit of sectors */
	fatLen = (nrSects / sectPerClust) * 32;
	fatLen = (fatLen + (512 * 8 - 1)) / (512 * 8);
#if	DEBUG
printf("%s %u startSect=%lu,nrSects=%lu,sectPerClust=%lu\n", __FILE__, (unsigned short)__LINE__,fatLen,nrSects,sectPerClust);
#endif

	/* Step4: determine the size of FAT system area in unit of sectors */
	nrSectsInSysArea = 32; /* Reserved sector */
	nrSectsInSysArea = nrSectsInSysArea + (2*fatLen);
#if 	0
	for(i32=0; (nrSectsInSysArea + 1) > (i32 * sectPerClust); i32++) {
		/* Intended NULL loop body */
	}
	pbsStartSect = (i32 * sectPerClust) - nrSectsInSysArea;
#else // user data from 16384 sector
	pbsStartSect = 16384L - nrSectsInSysArea;
#endif

	if ( pbsStartSect < sectPerClust ) {
		pbsStartSect = pbsStartSect + sectPerClust;
	}
	nrSects = nrSects - pbsStartSect; /* nrSects = partion size */

	pbs->sectPerClust = sectPerClust;
	pbs->reserved[0] = 0x20; /* 32 */
	pbs->reserved[1] = 0x00;
	pbs->fats = 2;
	/* Entries of root directories must be set to zero for FAT32 */
	pbs->dirEntries[0] = 0x00;
	pbs->dirEntries[1] = 0x00;
	if ( nrSects < 0x0000ffff ) {
		pbs->sects[0] = ((nrSects     ) & 0x000000ff);
		pbs->sects[1] = ((nrSects >> 8) & 0x000000ff);
	}
	pbs->media = 0xF8;
	pbs->fatLen[0] = 0;
	pbs->fatLen[1] = 0;
	pbs->sectsPerTrack[0] = (sectPerClust >= 64) ? 63 : sectPerClust;	/* 6 bits only */
	pbs->sectsPerTrack[1] = 0;
	pbs->heads[0] = 255; /* Pick the max value (who care?) */
	pbs->heads[1] = 0;
	pbs->hidden[0] = READ8(pbsStartSect,3);
	pbs->hidden[1] = READ8(pbsStartSect,2);
	pbs->hidden[2] = READ8(pbsStartSect,1);
	pbs->hidden[3] = READ8(pbsStartSect,0);
	if ( nrSects > 0x0000ffff ) {
		pbs->totalSects[0] = READ8(nrSects,3);
		pbs->totalSects[1] = READ8(nrSects,2);
		pbs->totalSects[2] = READ8(nrSects,1);
		pbs->totalSects[3] = READ8(nrSects,0);
	}

	/* The field of volume ID should be different each time it is formatted */
	DOS_FileCreateTimeGet((pDateStc_t)&dos.date);
	dos.date.Year += (2000-1980);	//RTC -> M$ time << 26;	//bit[31:26]
	MsDosTime = (dos.date.Hour << 11) + (dos.date.Minute << 5) + (dos.date.Second >> 1);
	MsDosDate = (dos.date.Year << 9) + (dos.date.Month << 5) + (dos.date.Day);
	pbs->u.fat32only.volId[0] = READ8(MsDosTime,0);
	pbs->u.fat32only.volId[1] = READ8(MsDosTime,1);
	pbs->u.fat32only.volId[2] = READ8(MsDosDate,0);
	pbs->u.fat32only.volId[3] = READ8(MsDosDate,1);

	pbs->u.fat32only.fat32Length[0] = READ8(fatLen,3); /* 0x24: sectors/FAT                         */
	pbs->u.fat32only.fat32Length[1] = READ8(fatLen,2);
	pbs->u.fat32only.fat32Length[2] = READ8(fatLen,1);
	pbs->u.fat32only.fat32Length[3] = READ8(fatLen,0);
	pbs->u.fat32only.flags[0]     = 0x0;	 /* 0x28: bit8: fat mirror, low4: active fat  */
	pbs->u.fat32only.flags[1]     = 0x0;
	pbs->u.fat32only.version[0]   = 0x0;     /* 0x2a: major, minor filesystem version     */
	pbs->u.fat32only.version[1]   = 0x0;
	pbs->u.fat32only.rootClust[0] = 0x2;     /* 0x2c: first cluster in root directory     */
	pbs->u.fat32only.rootClust[1] = 0x0;
	pbs->u.fat32only.rootClust[2] = 0x0;
	pbs->u.fat32only.rootClust[3] = 0x0;
	pbs->u.fat32only.infoSect[0]  = 0x1;     /* 0x30: filesystem info sector              */
	pbs->u.fat32only.infoSect[1]  = 0x0;
	pbs->u.fat32only.backupBoot[0]= 0x0;     /* 0x32: backup boot sector                  */
	pbs->u.fat32only.backupBoot[1]= 0x0;
	pbs->u.fat32only.diskNum =    0x0;       /* 0x40: disk number                         */
	pbs->u.fat32only.reserved2    = 0x0;     /* 0x41:                                     */
	pbs->u.fat32only.extCode      = 0x29;    /* 0x42: always equal to 0x29                */
	strncpy(&pbs->u.fat32only.fsName[0], "FAT32   ", 8); /* 0x52: ASCII string FAT32 with space       */
	disk->FsType = 32;

	return pbsStartSect;
}

UINT8 FAT_MbsCal(UINT8 DevId)
{
	UINT32 startSect, startCylinder, startHead;
	UINT32 endSect, endCylinder, endHead;
	UINT32 pbsStartSect, totalSects;
	UINT16 sectPerTrack;
	UINT8 nrHeads;
	UINT32 idx;
	x86_MBS *mbs;
	x86_PBS *pbs;
	DISK *disk;
	disk = (DISK *)&dos.disk[DevId];
	mbs = (x86_MBS *)&dos.share;
	pbs = (x86_PBS *)(&dos.share+512);

	WRITE8(pbsStartSect,0,pbs->hidden[3]);
	WRITE8(pbsStartSect,1,pbs->hidden[2]);
	WRITE8(pbsStartSect,2,pbs->hidden[1]);
	WRITE8(pbsStartSect,3,pbs->hidden[0]);
	WRITE8(sectPerTrack,0,pbs->sectsPerTrack[1]);
	WRITE8(sectPerTrack,1,pbs->sectsPerTrack[0]);
	nrHeads = pbs->heads[0];
	if(pbs->sects[0] || pbs->sects[1]) {	//FAT16
		WRITE8(totalSects,0,0);
		WRITE8(totalSects,1,0);
		WRITE8(totalSects,2,pbs->sects[1]);
		WRITE8(totalSects,3,pbs->sects[0]);
	}
	else {
		WRITE8(totalSects,0,pbs->totalSects[3]);
		WRITE8(totalSects,1,pbs->totalSects[2]);
		WRITE8(totalSects,2,pbs->totalSects[1]);
		WRITE8(totalSects,3,pbs->totalSects[0]);
	}
	if(DevId == DRIVE_SD) {
		for ( idx = 0; idx < sizeof(sdChsTbl) / sizeof(chsTbl_t); idx++ ) {
			if ( sdChsTbl[idx].TotalSectors >= disk->PhySectors ) {
				break;
			}
		}
		if ( idx == sizeof(sdChsTbl) / sizeof(chsTbl_t) ) {
			idx--;
		}
		nrHeads = sdChsTbl[idx].nrHead;
		sectPerTrack  = sdChsTbl[idx].sectPerTrack;
		startHead     = (pbsStartSect % (nrHeads * sectPerTrack)) / sectPerTrack;
		startSect     = (pbsStartSect % sectPerTrack) + 1;
		startCylinder = (pbsStartSect) / (nrHeads * sectPerTrack);
		endHead       = ((pbsStartSect + totalSects - 1) % (nrHeads * sectPerTrack)) / sectPerTrack;
		endSect       = ((pbsStartSect + totalSects - 1) % sectPerTrack) + 1;
		endCylinder   = ((pbsStartSect + totalSects - 1) / (nrHeads * sectPerTrack));
	}
	else {
		startHead     = (pbsStartSect % (nrHeads * sectPerTrack)) / sectPerTrack;
		startSect     = (pbsStartSect % sectPerTrack) + 1;
		startCylinder = (pbsStartSect) / (nrHeads * sectPerTrack);
		endHead       = ((pbsStartSect + totalSects - 1) % (nrHeads * sectPerTrack)) / sectPerTrack;
		endSect       = ((pbsStartSect + totalSects - 1) % sectPerTrack) + 1;
		endCylinder   = ((pbsStartSect + totalSects - 1) / (nrHeads * sectPerTrack));
	}

	//memset(pmbs, 0, 512);
	mbs->partition1.BootId         = 0x80;
	mbs->partition1.StartHead      = startHead;
	mbs->partition1.StartSector    = (startSect & 0x3f) | ((startCylinder >> 8) << 6);
	mbs->partition1.StartCylinder  = startCylinder & 0xff;
	if ( totalSects < 32680L ) {
		mbs->partition1.SysId      = 1;
	}
	else if ( totalSects < 65536L ) {
		mbs->partition1.SysId      = 4;
	}
	else {
		mbs->partition1.SysId      = 6;
	}
	mbs->partition1.EndHead        = endHead;
	mbs->partition1.EndSector      = (endSect & 0x3f) | ((endCylinder >> 8) << 6);
	mbs->partition1.EndCylinder    = endCylinder & 0xff;
	mbs->partition1.StartLogSector[0] = READ8(pbsStartSect,3);
	mbs->partition1.StartLogSector[1] = READ8(pbsStartSect,2);
	mbs->partition1.StartLogSector[2] = READ8(pbsStartSect,1);
	mbs->partition1.StartLogSector[3] = READ8(pbsStartSect,0);
	mbs->partition1.PartitionSize[0]  = READ8(totalSects,3);
	mbs->partition1.PartitionSize[1]  = READ8(totalSects,2);
	mbs->partition1.PartitionSize[2]  = READ8(totalSects,1);
	mbs->partition1.PartitionSize[3]  = READ8(totalSects,0);
	mbs->Signature[0] = 0x55;
	mbs->Signature[1] = 0xAA;

	return SUCCESS;
}

/**
 * @brief	format a existed driver to FAT16 or FAT32
 * @param	DevId, device ID, 0=SD, 1=NAND, 2=RAM
 * @param	name, 11 bytes for LABEL
 * @param	opt, 32 for FAT32, others for FAT16
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Ted Chiu
 * @since	08/04/16 17:36
 * @todo
 * @bug
*/
//test OK!
UINT8 DOS_Format(UINT8 DevId, UINT8 *name, UINT8 fat, UINT8 erase)
{
	UINT8 ret = TRUE; //xian ++
	UINT32 i32, j32;
	UINT32 startSect;
	UINT32 nrSects;
	UINT8  nrFats;
	UINT16 i16, j16;
	/*
    UINT8 sts = TRUE;
    UINT32 oldPhyStartAddr;
	UINT16 blockAddr = 0;
    UINT32 phyAddr = 0;
    UINT32 logAddr;
	*/
	xdata UINT8 *buf;
	xdata x86_MBS *mbs;
	xdata x86_PBS *pbs;
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];
	mbs = (x86_MBS *)&dos.share;
	pbs = (x86_PBS *)(&dos.share+512);
	/*
	if( DevId == DRIVE_NAND ) {
    	sts = SMC_CardDetect();
		SMC_EraseTotalBlock();
        //Initialization
        logAddr = 0x00000000;
        phyAddr = G_SMC_NewCisBlockAddress;
		if (sts = SMC_FormatCheck())
		{
                G_Card_CurrBank = 0;
                SMC_MakeTable(0);
				sts = SMC_WriteLbaToPba(logAddr, &oldPhyStartAddr, &phyAddr, &blockAddr);
	    		if ( oldPhyStartAddr != 0xffffffff )
	    			SMC_EraseOneBlock(oldPhyStartAddr);
		}	
	}
	*/
	/* Clean format with MBR and PBS */
	HAL_GprmDramFill(dos_var_start, 512+512, 0x00);
//printf("%s %u fat=%bu\n", __FILE__, (unsigned short)__LINE__, fat);
	startSect = -1;
	if(fat==32) {
		if(disk->PhySectors > 65536L) { /* Check for the device size (> 32MB?) */
			startSect = FAT_PbsCal32(DevId);
//printf("%s %u startSect=%lu\n", __FILE__, (unsigned short)__LINE__,startSect);
		}
	}
	if(startSect == -1) {
		startSect = FAT_PbsCal16(DevId);
#if	DEBUG
printf("%s %u startSect=%lu\n", __FILE__, (unsigned short)__LINE__,startSect);
#endif
		if(startSect == -1) {
			startSect = FAT_PbsCal32(DevId);
#if	DEBUG
printf("%s %u startSect=%lu\n", __FILE__, (unsigned short)__LINE__,startSect);
#endif
		}
	}
	/*
	if((pbs->fatLen[0]==0 && pbs->fatLen[1]==0)){
	 	fat = 32;
	}
	else{
	 	fat = 16;
	}
	*/
	printf("fat=%bd\n",disk->FsType);
	//pbs->ignored[0] = 0xE9;
	pbs->ignored[0] = 0xEB;
	//pbs->ignored[1] = 0x00;
	pbs->ignored[2] = 0x90;
	strncpy(&pbs->sysId[0], "SUNP_DSC", 8);
	pbs->fats        = 2;
	pbs->sectSize[0] = 0x00;
	pbs->sectSize[1] = 0x02;
	pbs->Signature[0]= 0x55;    /* signature must reside here(for alignment) */
	pbs->Signature[1]= 0xAA;    /* signature must reside here(for alignment) */
	if(fat==32) {
		if(name==NULL)
			strncpy(&pbs->u.fat32only.volLabel[0], "NO NAME    ", 11);
		else
			strncpy(&pbs->u.fat32only.volLabel[0], name, 11);
	}
	else {
		if(name==NULL)
			strncpy(&pbs->u.fat16only.volLabel[0], "NO NAME    ", 11);
		else
			strncpy(&pbs->u.fat16only.volLabel[0], name, 11);
	}

	FAT_MbsCal(DevId);
    
#if DOS_DEV_SUPPORT_FLASH
	if ( DevId == DRIVE_NAND && (erase & 0x01)){
#if (DOS_RSV_PART == 1)
        /* DO NOT erase all the blocks when there's reserved partition on nand block */
        if (!DOS_RsvPartSizeGet()) {
            SMC_EraseTotalBlock();
        }
#else        
		SMC_EraseTotalBlock();
#endif            
	}
#else
    erase = erase;
#endif //DOS_DEV_SUPPORT_FLASH    

	if( 0 == DOS_SectorWrite(DevId, 0, 1, dos_var_start)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	if( 0 == DOS_SectorWrite(DevId, startSect, 1, dos_var_start+512/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);

	/* Clear FAT table */
	nrSects = 0;
	WRITE8(nrSects,3,pbs->fatLen[0]);
	WRITE8(nrSects,2,pbs->fatLen[1]);
	WRITE8(startSect, 0, pbs->hidden[3]);
	WRITE8(startSect, 1, pbs->hidden[2]);
	WRITE8(startSect, 2, pbs->hidden[1]);
	WRITE8(startSect, 3, pbs->hidden[0]);
	startSect += pbs->reserved[1]*256 + pbs->reserved[0];

#if	DEBUG
	printf("%s %u startSect=%ld\n", __FILE__, (unsigned short)__LINE__,startSect);
	cmdMemDump("0x8000 +512");
	printf("\n");
	cmdMemDump("0x8200 +512");
#endif

#if 0//( DOSRAM_OPTION == 1 )
	if( DevId == DRIVE_DRAM ){
		disk->FAT_BufSize = (nrSects << 9);
		disk->FAT_BufAddr  = (DRAM_StartAddrGet() >> 1) + (startSect << 8);
		//printf("a=%lx,s=%lx,%lx,%lx\n",disk->FAT_BufAddr,disk->FAT_BufSize,nrSects,startSect);
	}
#endif
	nrFats = pbs->fats;
	i32 = disk->FAT_BufAddr;
	j32 = disk->FAT_BufSize;
	while(j32) {
		j16 = (j32 > 32*1024L) ? 32*1024 : j32;
		HAL_GprmDramFill(i32, j16, 0x00);
		j32 -= j16;
		i32 += j16/2;
	}
	buf = (UINT8 *)(&dos.share+512+512);
	HAL_GprmDramFill(dos_var_start+(512+512)/2, 512, 0x00);

	for ( ; nrFats > 0; nrFats-- ) {
		buf[0] = pbs->media;
		buf[1] = 0xFF;
		buf[2] = 0xFF;
		if(disk->FsType == 32) { /* FAT32 */
			//printf("It is FAT32!\n");
			WRITE8(nrSects,0,pbs->u.fat32only.fat32Length[3]);
			WRITE8(nrSects,1,pbs->u.fat32only.fat32Length[2]);
			WRITE8(nrSects,2,pbs->u.fat32only.fat32Length[1]);
			WRITE8(nrSects,3,pbs->u.fat32only.fat32Length[0]);
			buf[3] = 0x0F;	/* Root directory occupies one cluster */
			buf[4] = 0xFF;	buf[5] = 0xFF;	buf[6] = 0xFF;	buf[7] = 0x0F;
			buf[8] = 0xFF;	buf[9] = 0xFF;	buf[10]= 0xFF;	buf[11]= 0x0F;
		}
		else {
			if(disk->FsType == 16) { /* FAT16 */
				buf[3] = 0xFF;
			}
			WRITE8(nrSects,0,0);
			WRITE8(nrSects,1,0);
			WRITE8(nrSects,2,pbs->fatLen[1]);
			WRITE8(nrSects,3,pbs->fatLen[0]);
		}
#if	DEBUG
printf("%s %u startSect=%lu nrSects=%lu\n", __FILE__, (unsigned short)__LINE__,startSect,nrSects);
#endif
		if( 0 == DOS_SectorWrite(DevId, startSect, 1, dos_var_start+(512+512)/2)) return dosRwErr(__FILE,(unsigned short)__LINE__);
		nrSects--;
		startSect++;
#if	DEBUG
printf("%s %u nrSects=%lu\n", __FILE__, (unsigned short)__LINE__,nrSects);
#endif
		i32 = disk->FAT_BufSize >> 9;
		while(nrSects) {
			j32 = (nrSects > i32) ? i32 : nrSects;
#if 0//( DOSRAM_OPTION == 1 )
			if( DevId == DRIVE_DRAM ){
				HAL_GprmDramFill(dos.TmpBufAddr, j32 << 9, 0x00);
				if( 0 == DOS_SectorWrite(DevId, startSect, j32, dos.TmpBufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
			}
			else{
				if( 0 == DOS_SectorWrite(DevId, startSect, j32, disk->FAT_BufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
			}
#else
			if( 0 == DOS_SectorWrite(DevId, startSect, j32, disk->FAT_BufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
#endif
			nrSects      -= j32;
			startSect    += j32;
#if	DEBUG
printf("%s %u nrSects=%lu\n", __FILE__, (unsigned short)__LINE__,nrSects);
#endif
		}
	}

	/* Clear root diretory */
	if(fat==32) { /* FAT32 */
		nrSects = pbs->sectPerClust;
	}
	else {
		WRITE8(i16,1,pbs->dirEntries[0]);
		WRITE8(i16,0,pbs->dirEntries[1]);
		WRITE8(j16,1,pbs->sectSize[0]);
		WRITE8(j16,0,pbs->sectSize[1]);
		nrSects = i16 * 32 / j16;
	}
#if	DEBUG
printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
#endif
#if 0//( DOSRAM_OPTION == 1 )
	if( DevId == DRIVE_DRAM ){
		HAL_GprmDramFill(dos.TmpBufAddr, nrSects << 9, 0x00);
		if( 0 == DOS_SectorWrite(DevId, startSect, nrSects, dos.TmpBufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	else{
		if( 0 == DOS_SectorWrite(DevId, startSect, nrSects, disk->FAT_BufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
#else
	//memset(pbuf, 0, 512);
	if( 0 == DOS_SectorWrite(DevId, startSect, nrSects, disk->FAT_BufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
#if	DEBUG
printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
#endif
#endif

	/* */
	if (dosFatLableEn) {
		FAT_LabelAdd(DevId);
	}

	DOS_UnMount(DevId);
//	return 0;
	return ret; //xian
}
#if DOS_FMT_LABLE
UINT8 DOS_LableStrSet(const UINT8* pLabel)
{
	UINT8 i;

	FAT_LableStrRst();
	if (pLabel == NULL) {
		return SUCCESS;
	}
	
	if (strlen(pLabel) > 11) {
		return FAIL;
	}

	dosFatLableEn = 1;
	for (i = 0; i < 12, *pLabel != 0; i++, pLabel++) {
		dosFatLableStr[i] = *pLabel;
	}

	//printf("dosFatLableStr: %s\n", dosFatLableStr);
	return SUCCESS;
}

UINT8 FAT_LabelAdd(UINT8 DevId)
{
	UINT8 i8;
	FILE* file;
	x86_FDB* fdb;
	DISK* disk;
	UINT32 sector;
	UINT16 time, date;
	UINT8* ptr8;
	UINT8 ret;
#if 0
#if DOS_DEV_SUPPORT_CARD
	if( DevId == K_MEDIA_SDF ){
		ret = DOS_Mount(DRIVE_SD, SD_PhySectors);
		//printf("SD = %lu, ret=%bx\n",SD_PhySectors, ret);
	}
#endif //DOS_DEV_SUPPORT_CARD

#if DOS_DEV_SUPPORT_FLASH
	else if( DevId == K_MEDIA_NANDF ){
		ret = DOS_Mount(DRIVE_NAND, diskPhySectors);
		//printf("Nand = %lu, ret=%bx\n",diskPhySectors, ret);
	}
#endif //DOS_DEV_SUPPORT_FLASH

#if DOS_DEV_SUPPORT_DRAM
#if ( DOSRAM_OPTION == 1 )
	else if( DevId == K_MEDIA_DOSRAM ){
		ret = DOS_Mount(DRIVE_DRAM, DOSRAM_PhySectors);
		//printf("DOSRAM = %lu, ret=%bx\n",DOSRAM_PhySectors, ret);
	}	
#endif
#endif //DOS_DEV_SUPPORT_DRAM
#endif

    ret = DOS_Initialize();

	//printf("DevId: %lx\n", (UINT32)DevId);
	
	HAL_GprmDramFill(dos_var_start, 64, 0x00);	//clear FDB buffer to 0

	file = (FILE *)&dos.file[0];
	fdb = (x86_FDB *)&dos.share;
	disk = (DISK *)&dos.disk[DevId];
	//printf("disk->RootStartCluster: %lx\n", (UINT32)disk->RootStartCluster);

	//DOS_FileCreateTimeGet((pDateStc_t)&dos.date);	//already read by FAT_PbsCal16/32
	//dos.date.Year += (2000-1980); 	//RTC -> M$ time
	time = (dos.date.Hour << 11) + (dos.date.Minute << 5) + (dos.date.Second >> 1);
	date = (dos.date.Year << 9) + (dos.date.Month << 5) + (dos.date.Day);

	//curr dir entry
	i8 = 0;
	while(i8 < 11) fdb->name[i8++] = dosFatLableStr[i8];
	fdb->attr = ATTR_L;
	fdb->lcase	 = 0;	//Reserved for use by Windows NT.
	fdb->ctimeMs = 0;		/* Creation time, milliseconds */
	fdb->ctime[0] = 0;	/* Creation time */
	fdb->ctime[1] = 0;
	fdb->time[0]  = READ8(time,1);
	fdb->time[1]  = READ8(time,0);
	fdb->cdate[0] = 0;	/* Creation date */
	fdb->cdate[1] = 0;
	fdb->adate[0] = 0;	/* Last access date */
	fdb->adate[1] = 0;
	fdb->date[0]  = READ8(date,1);
	fdb->date[1]  = READ8(date,0);
	fdb->starthi[1]= 0; /* High 16 bits of cluster in FAT32 */
	fdb->starthi[0]= 0;
	fdb->start[1]  = 0;
	fdb->start[0]  = 0;
	*((UINT32 *)fdb->size) = 0L;

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

	sector = FAT_Cluster2Sector(DevId, disk->RootStartCluster);
	//printf("disk->RootStartCluster: %lx\n", (UINT32)disk->RootStartCluster);

	if( 0 == DOS_SectorWrite(DevId, sector, 0x20, dos.TmpBufAddr)) dosRwErr(__FILE, (unsigned short)__LINE__);

    FAT_LableStrRst();
    
	return SUCCESS;
}

#endif

//xian ++ 20100825
void DOS_FormatErrProc(UINT8 DevId)
{
	xdata DISK *disk;
	disk = (DISK *)&dos.disk[DevId];

	//maybe used by middleware or app
	disk->FsType = 0; 
	disk->Cluster2Byte = 0;
	disk->FreeCluster = 0;
}
//xian --
