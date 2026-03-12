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
 *  Author: Eric Su                                                      *
 **************************************************************************/
#include <string.h>
#include <stdio.h>
#include "general.h"
#include "dbg_def.h"
//#include "dosusr.h"
#include "dos32.h"
#include "initio.h"
#include "cardlink.h"
#include "hal.h"
#include "sdfunc.h"
#include "smcfunc.h"
#include "smcvar.h"
#include "main.h"
#include "uiFlow.h"
#include "gpio.h"
#include "asicreg.h"
#include "hal.h"
#include "sp1k_hal_api.h"
#include "dramfunc.h"
#include "dosvar.h"
#include "mtd_macro_api.h"
#include "mtd_general.h"
#include "sdramhw.h"
#include "hal_gprm.h"
#include "hal_disp.h"
//#include "osd_api.h"
#include "utility.h"
#include "dcf_api.h"
#include "reg_dram.h"
#include "dbg_def.h"

#define __FILE	__FILE_ID_DOS_SUPPORT__
#define	DEBUG	0

#ifndef FPGA_MEM_POOL_ADDR
static UINT8 xdata dosRwDbg _at_ 0x5000;
#else
static UINT8 xdata dosRwDbg;
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
xdata UINT8 G_Dos_Create_Time_By_Rtc = 1;
xdata UINT8 G_DosRamEn;
xdata UINT32 dos_var_start = 0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern DOS dos;
extern xdata UINT8 G_Card_Type;
extern xdata UINT32 SD_PhySectors;
extern xdata UINT32 diskPhySectors;
extern xdata UINT32 DOSRAM_PhySectors;
//extern xdata UINT32 dos_var_start;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//UINT8 fwUpdateDo(UINT32);

/*
	start, word addr
	size, UNIT=word
*/
UINT8 DOS_BuffSet(UINT32 start, UINT32 size)
{
	//UINT16 idx;
	UINT8 i8;

	size = size;

	if( dos_var_start != 0 ){//Only initial Once ! 
		return TRUE;
	}

	dos_var_start = K_SDRAM_ShadowAddr + (0x4000UL >> 1);

	dos.CurrDrv = DRIVE_NONE;
	dos.disk[DRIVE_SD].FAT_BufSize = FAT_BUF_SIZE_SD*2;// drive 0 ( SD ) FAT buffer, 128KB
	dos.disk[DRIVE_NAND].FAT_BufSize = FAT_BUF_SIZE_NAND*2;// drive 1 ( Nand ) FAT buffer, 32KB
#if ( DOSRAM_OPTION == 1 )
	dos.disk[DRIVE_DRAM].FAT_BufSize = FAT_BUF_SIZE_DRAM*2;// drive 2 ( DRAM ) FAT buffer, 32KB
#endif
#if (DOS_RSV_PART == 1)
    dos.disk[DRIVE_NAND_RSV].FAT_BufSize = dos.disk[DRIVE_DRAM].FAT_BufSize;
#endif

	//dos.disk[DRIVE_SD].FAT_BufAddr = dos_var_start + VAR_BUF_SIZE;
	dos.disk[DRIVE_SD].FAT_BufAddr = start;
	dos.disk[DRIVE_NAND].FAT_BufAddr  = dos.disk[DRIVE_SD].FAT_BufAddr + FAT_BUF_SIZE_SD;
#if ( DOSRAM_OPTION == 1 )
	dos.disk[DRIVE_DRAM].FAT_BufAddr  = dos.disk[DRIVE_NAND].FAT_BufAddr + FAT_BUF_SIZE_NAND;
	dos.TmpBufAddr  = dos.disk[DRIVE_DRAM].FAT_BufAddr + FAT_BUF_SIZE_DRAM;//64 KB ( maximum cluster size )
#else
	dos.TmpBufAddr  = dos.disk[DRIVE_NAND].FAT_BufAddr + FAT_BUF_SIZE_NAND;//64 KB ( maximum cluster size )
#endif

#if (DOS_RSV_PART == 1)
    dos.disk[DRIVE_NAND_RSV].FAT_BufAddr = dos.disk[DRIVE_DRAM].FAT_BufAddr;    
#endif
	for(i8=0; i8<MAX_FILE; i8++) 
	{ // One file reserve 512 bytes, now is 8 file
		dos.file[i8].DevId = -1;
		dos.file[i8].buf = dos.TmpBufAddr + TMP_BUF_SIZE/2 + i8*512L/2;
	}
	return 0;
}

/**
 * @fn        UINT8 DOS_DosRamCfg(UINT8 enable)
 * @brief     Enable or disable DOS Ram.
 * @param     [in] enable	1: enable, 0:disable
 * @retval    return = SUCCESS
 * @see       NULL
*/
UINT8 DOS_DosRamCfg(UINT8 enable)
{
	if (enable) {
		G_DosRamEn = 1;
	} else {
		G_DosRamEn = 0;
	}

	return 0;
}

/**
 * @fn        UINT8 DOS_DosRamStsGet(void)
 * @brief     Get DOS Ram status
 * @param     NONE
 * @retval    return		TRUE : enable, FALSE: disable.
 * @see       NULL
*/
UINT8 DOS_DosRamStsGet(void)
{
	if (G_DosRamEn) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
 * @fn        UINT32 DOS_GetDiskSpace(void)
 * @brief     Get free space by KB
 * @param     NONE
 * @retval    return Free space of disk in KByte units.
 * @see       NULL
*/
UINT32 DOS_GetDiskSpace(void)
{
	UINT32 size = 0;
	UINT8 DevId = DRIVE_SD;
	xdata DISK *disk;

	//printf( "%s: %ld\n", __FILE__, (UINT32)__LINE__ );

	if( G_Card_Type == K_MEDIA_NONE ){
		return 0;
	}
	else if( G_Card_Type == K_MEDIA_NANDF ){
#if (DOS_RSV_PART == 1)
        if (DOS_RsvPartStsGet() && DOS_RsvPartActGet()) {
            DevId = DRIVE_NAND_RSV;
        } else {
            DevId = DRIVE_NAND;
        }
#else
		DevId = DRIVE_NAND;
#endif 
	}
	else if( G_Card_Type == K_MEDIA_SDF ){
		DevId = DRIVE_SD;
	}
	#if ( DOSRAM_OPTION == 1 )
	else if( G_Card_Type == K_MEDIA_DOSRAM ){
		DevId = DRIVE_DRAM;
	}
	#endif

	disk   = (DISK *)&dos.disk[DevId];
	if(disk->FsType != 0){ // #40938
		//printf("DevId=%bx %lx %bx\n",DevId,disk->FreeCluster,disk->bitCluster2Sector);
		size = disk->FreeCluster << disk->bitCluster2Sector;// to sector number
		//printf("size=%lx\n",size);
		size = size >> 1;// to K Bytes
	}

	//printf( "%s: %ld\n", __FILE__, (UINT32)__LINE__ );

	return size;
}

/**
 * @brief	Get free space by KB
 * @param	DevId, device ID, 0=SD, 1=NAND, 2=RAM
 * @param	TotalSectors, device capacity by sectors (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT32 DOS_FreeDiskSpaceByCluster(void)
{
	UINT8 DevId = DRIVE_SD;
	xdata DISK *disk;
	if( G_Card_Type == K_MEDIA_NANDF ){
#if (DOS_RSV_PART == 1)
        if (DOS_RsvPartStsGet() && DOS_RsvPartActGet()) {
            DevId = DRIVE_NAND_RSV;
        } else {
            DevId = DRIVE_NAND;
        }
#else
        DevId = DRIVE_NAND;
#endif 
	}
	else if( G_Card_Type == K_MEDIA_SDF ){
		DevId = DRIVE_SD;
	}
	#if ( DOSRAM_OPTION == 1 )
	else if( G_Card_Type == K_MEDIA_DOSRAM ){
		DevId = DRIVE_DRAM;
	}
	#endif
	disk   = (DISK *)&dos.disk[DevId];
	G_DOS_TotalFreeCluster = disk->FreeCluster;
	//printf("Free cluster = %lx\n",disk->FreeCluster);
	return disk->FreeCluster;
}
/**
 * @fn        UINT32 DOS_ClusterSizeGet(void)
 * @brief     Get the cluster size.
 * @param     NONE
 * @retval    return cluster size in byte units.
 * @see       NULL
*/
UINT32 DOS_ClusterSizeGet(void)
{
	UINT8 DevId = DRIVE_SD;
	xdata DISK *disk;
	if( G_Card_Type == K_MEDIA_NANDF ){
#if (DOS_RSV_PART == 1)
        if (DOS_RsvPartStsGet() && DOS_RsvPartActGet()) {
            DevId = DRIVE_NAND_RSV;
        } else {
            DevId = DRIVE_NAND;
        }
#else
        DevId = DRIVE_NAND;
#endif 
	}
	else if( G_Card_Type == K_MEDIA_SDF ){
		DevId = DRIVE_SD;
	}
	#if ( DOSRAM_OPTION == 1 )
	else if( G_Card_Type == K_MEDIA_DOSRAM ){
		DevId = DRIVE_DRAM;
	}
	#endif
	disk   = (DISK *)&dos.disk[DevId];
	G_DOS_ClusterSize = disk->Cluster2Byte;
	//printf("cluster = %x\n",G_DOS_ClusterSize);
	return (UINT32)G_DOS_ClusterSize;
}
/**
 * @brief	initial DOS system
 * @param	DevId, device ID, 0=SD, 1=NAND, 2=RAM
 * @param	TotalSectors, device capacity by sectors (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
//-----------------------------------------------------------------------------
//DOS_Initialize
//-----------------------------------------------------------------------------
UINT8 DOS_Initialize(void)
{
	UINT8 ret = TRUE;
	//UINT32 t0,t1;

	/* 80UL << 9: 80KB for smc disk table */
	DOS_BuffSet(K_SDRAM_DOS_WorkAddr + (80UL << 9), K_SDRAM_DOS_WorkSize + K_SDRAM_DOS_Fat1BufSize);
	//DOS_BuffSet(K_SDRAM_DOS_WorkAddr, K_SDRAM_DOS_WorkSize + K_SDRAM_DOS_Fat1BufSize);

#if DOS_DEV_SUPPORT_CARD
	if( G_Card_Type == K_MEDIA_SDF || G_Card_Type == K_MEDIA_EMMC){
		ret = DOS_Mount(DRIVE_SD, SD_PhySectors);
		//printf("SD = %lu, ret=%bx\n",SD_PhySectors, ret);
	}
#endif //DOS_DEV_SUPPORT_CARD

#if DOS_DEV_SUPPORT_FLASH
	else if( G_Card_Type == K_MEDIA_NANDF ){
#if (DOS_RSV_PART == 1)
        if (DOS_RsvPartStsGet() && DOS_RsvPartActGet()) {
            ret = DOS_Mount(DRIVE_NAND_RSV, diskPhySectors);
        } else {
            ret = DOS_Mount(DRIVE_NAND, diskPhySectors);
        }
#else
		ret = DOS_Mount(DRIVE_NAND, diskPhySectors);
#endif
		//printf("Nand = %lu, ret=%bx\n",diskPhySectors, ret);
	}
#endif //DOS_DEV_SUPPORT_FLASH  

#if DOS_DEV_SUPPORT_DRAM
	#if ( DOSRAM_OPTION == 1 )
	else if( G_Card_Type == K_MEDIA_DOSRAM ){
		ret = DOS_Mount(DRIVE_DRAM, DOSRAM_PhySectors);
		//printf("DOSRAM = %lu, ret=%bx\n",DOSRAM_PhySectors, ret);
	}
	#endif
#endif //DOS_DEV_SUPPORT_DRAM

	else{
		ret = 0;
	}
	DOS_ClusterSizeGet();
	if( ret != TRUE ){
		return FALSE;
	}
	//#if CARD_MOUNT_ISP
	//if( FAIL == fwUpdateDo(G_ISP_Addr)){
	//}
	//#endif
	if(FAIL == fwUpdateDo()){
	}
	
	//HAL_GlobalReadGTimer(&t0);
#if (DOS_RSV_PART == 1)
    if( G_Card_Type == K_MEDIA_NANDF ) {
        /* Do not create DCIM fold for reserved partition */
        if (!DOS_RsvPartStsGet() || !DOS_RsvPartActGet()) {
            ret = dcfDirInitialize();
        }
    } else {
        ret = dcfDirInitialize();
    }

#else
	ret = dcfDirInitialize();
#endif
	//HAL_GlobalReadGTimer(&t1);
	//printf("DirInit=%lu\n",t1-t0);
	return ret;
}
#if 0 //move to util_card.c
/**
 * @brief	Create File from data at Dram
 * @param	name, start sectors (1 sector = 512 Bytes), size
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 dramFileSave(UINT8 *name,UINT32 startAddr,UINT32 size)
{
	UINT32 cnt = 0;
	UINT8 fd;
	//BIT es0;
#if 0	//xian mask 20081203
	UINT8 dirName[9] = "100MEDIA";

	if( G_USR_FileIndex > 9999 ){
		if( USR_CheckMediaFolder(dirName) != TRUE ) return FALSE;
		if( DIR_Create(dirName) != DOS_SUCCESS ) return FALSE;
#ifdef MULTI_FOLDER_AS_ONE  //xian ++ 20080806
        USR_UpdateNewFolderInfo(dirName);
#endif
		if( DIR_Change(dirName) != DOS_SUCCESS ) return FALSE;
		dcfFileInitialize();
		dcfBaseNameGet(name,G_USR_FileIndex);
	}
#else
#if 0 //xian --, 20090116
	if( G_USR_FileIndex > 9999 ){ // just keep the same with the old code
        dcfFileIndexUpdate();
        dcfBaseNameGet(name,G_USR_FileIndex);
    }
#endif    
#endif
	startAddr = startAddr << 1;
	fd = DOS_FileOpen(name, 'w');
	if(fd == 0) {
		//printf("open err...\n");
		DOS_FileClose(fd);
		return FALSE;
	}
//XBYTE[0x2038] |= 0x01;
//dbgTimerStart();
//XBYTE[0x2030] |= 0x01;
	cnt = DOS_FileWrite(fd, startAddr, size);
//XBYTE[0x2030] &= ~0x01;
//dbgTimerStop();
	DOS_FileClose(fd);

	//dumpData(128, G_SMC_EntryTable, 0);
	//dumpData(512, G_SMC_AllocatTable, 0);

	if(cnt != size ){
		//printf("cnt=%lx,size=%lx,%bx\n",cnt,size,FALSE);
		return FALSE;
	}
	else{
		//printf("cnt=%lx,size=%lx,%bx\n",cnt,size,TRUE);
		return TRUE;
	}
}


/**
 * @brief	Read File data to Dram
 * @param	name, start sectors (1 sector = 512 Bytes)
 * @retval	size.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 dramFileLoad(UINT8 *name,UINT32 startAddr,UINT32 *size)
{
	UINT32 cnt = 0;

	UINT8 fd;
	//BIT es0;
	//UINT32 t0,t1;

	//HAL_GlobalReadGTimer(&t0);
	startAddr = startAddr << 1;

	fd = DOS_FileOpen(name, 'r');
	//HAL_GlobalReadGTimer(&t1);
//printf("t1=%lu\n",t1-t0);
	if (fd != 0)
	{
		//HAL_GlobalReadGTimer(&t0);
		DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)size);
//XBYTE[0x2038] |= 0x01;
//dbgTimerStart();
		cnt = DOS_FileRead(fd, startAddr, *size, 0);
//dbgTimerStop();
		//printf("%lx,startAddr=%lx,size=%lx\n",cnt,startAddr,*size);
		//HAL_GlobalReadGTimer(&t1);
//printf("t2=%lu\n",t1-t0);
		//HAL_GlobalReadGTimer(&t0);
		DOS_FileClose(fd);
		//HAL_GlobalReadGTimer(&t1);
//printf("t3=%lu\n",t1-t0);
	}
	else
	{
		//printf("%s not Found",name);
		DOS_FileClose(fd);
		return FALSE;
	}

	if(cnt != *size ){
//printf("cnt=%lx,size=%lx,%bx\n",cnt,*size,FALSE);
		return FALSE;
	}
	else{
//printf("cnt=%lx,size=%lx,%bx\n",cnt,*size,TRUE);
		return TRUE;
	}

}
#endif

#if 0 //move to vfs_api.c
/**
 * @brief	delete 1 file
 * @param	name, start sectors (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 vfsFileDelete(UINT8 *name)
{
	UINT8 ret = FALSE;
#if (FPGA_VERIFY != 1)
	logBank_t bank;
	UINT16 addr;

	HAL_CpuImemSdGet(&bank, &addr);
	HAL_CpuImemSdSet(LOG_BANK_1, 0x8000); // DOS_FileDel inside IMEM
#endif

	ret = DOS_FileDel(name);

#if (FPGA_VERIFY != 1)
	HAL_CpuImemSdSet(bank, addr); //return original IMEM
#endif	

	//dumpData(128, G_SMC_EntryTable, 0);
	//dumpData(512, G_SMC_AllocatTable, 0);
	if( ret == DOS_SUCCESS ){
		ret = TRUE;
	}
	return ret;
}
#endif
#if 0 //same with vfsFileDelete
UINT8 File_Delete1File(UINT8 matchCond, UINT32 refCon)
{
	UINT8 ret = FALSE;
    
#if (FPGA_VERIFY != 1)
	logBank_t bank;
	UINT16 addr;

	HAL_CpuImemSdGet(&bank, &addr);
	HAL_CpuImemSdSet(LOG_BANK_1, 0x8000); // DOS_FileDel inside IMEM
#endif	

	ret = DOS_FileDel((UINT8 *)refCon);

#if (FPGA_VERIFY != 1)
	HAL_CpuImemSdSet(bank, addr); //return original IMEM
#endif    
	if( ret == DOS_SUCCESS ){
		ret = TRUE;
	}
	return ret;
	matchCond=matchCond;
}
#endif
#if 0 //move to vfs_api.c
/**
 * @brief	delete all file
 * @param	name, start sectors (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 vfsFileDeleteAll(UINT8 matchCond, UINT32 refCon)
{

	/*						   |------------|
							   |		|
	DIR_SearchReset() ->  -> DOS_FileDelReset() ->  DIR_Search() -> DOS_FileDelX(fdb, 0) -> DOS_FileDelStop()

	*/
	UINT8 DevId, ret;
	//UINT32 t0,t1;
#if (FPGA_VERIFY != 1)
	logBank_t bank;
	UINT16 addr;
#endif

	FDB fdb;
	DevId = DOS_CurrDriverGet();
	//printf("ID=%bx\n",DevId);
#if (FPGA_VERIFY != 1)	
	HAL_CpuImemSdGet(&bank, &addr);
	HAL_CpuImemSdSet(LOG_BANK_1, 0x8000); // DOS_FileDel inside IMEM
#endif	
	//sp1kHalReadGTimer(&t0);
	DIR_SearchReset();
	DOS_FileDelReset();
	while(1) {
		ret = DIR_Search(DevId, "*", (FDB *)&fdb, FILE_INFO_GET_SIZE);
		if(ret==FIND_FILE_FAIL)	//can't find more file
			break;
		if(ret==FIND_FILE_OK) {
			//printf("name=%s\n",fdb.name);
			ret = USR_MatchFdb(matchCond, (UINT8 *)&(fdb.name), refCon);
			if (ret && !(fdb.attr & 0x03)){//Write Protect
				if( fdb.size < 0x100000 ){ //if file size < 1MB, use DOS_FileDelX flow
					DOS_FileDelX(&fdb, 0);
    					continue;
				}
				DOS_FileDelStop();
				ret = DOS_FileDel(fdb.name);
				DOS_FileDelReset();
			}
			/*
			else{
				printf("not del %s\n",fdb);
			}
			*/
		}
	}
	DOS_FileDelStop();
	//sp1kHalReadGTimer(&t1);
#if (FPGA_VERIFY != 1)		
	HAL_CpuImemSdSet(bank, addr); //return original IMEM
#endif 	
	//printf("del=%lu\n",t1-t0);
	return TRUE;
}

/**
 * @brief	Open a file
 * @param	name, rwMode, numCluster (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT16 vfsFileOpen(UINT8 *name, UINT8 rwMode, UINT16 numCluster)
{
	UINT8 arg = 'r';
	UINT32 size;
	UINT8 fd;
//	UINT8 dirName[9] = "100MEDIA";

	if(( rwMode == K_DOS_NewFile ) || ( rwMode == K_DOS_NewFile_LinearFAT )){
		arg = 'w';
#if 0 //xian mask 20081203
		if( G_USR_FileIndex > 9999 ){
			if( USR_CheckMediaFolder(dirName) != TRUE ) return FALSE;
			//printf("1 dirName=%s\n",dirName);
			if( DIR_Create(dirName) != DOS_SUCCESS ) return FALSE;
#ifdef MULTI_FOLDER_AS_ONE  //xian ++ 20080806
        USR_UpdateNewFolderInfo(dirName);
#endif
			//printf("2 dirName=%s\n",dirName);
			if( DIR_Change(dirName) != DOS_SUCCESS ) return FALSE;
			dcfFileInitialize();
			dcfBaseNameGet(name,G_USR_FileIndex);
			//printf("name=%s\n",name);
		}
#else
#if 0 //xian --, 20090116
    	if( G_USR_FileIndex > 9999 ){ // just keep the same with the old code
            dcfFileIndexUpdate();
            dcfBaseNameGet(name,G_USR_FileIndex);
        }
#endif        
#endif
	}

	fd = DOS_FileOpen(name, arg);
	DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
	G_DOS_FileSize = size;
	return fd;
	//useless
	numCluster = numCluster;
}
/**
 * @brief	Open a file
 * @param	name, rwMode, numCluster (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/

UINT8 File_LRead(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos)
{
	UINT32 cnt;
	//UINT32 t0, t1;
	xdata FILE *file;
	file   = (FILE *)&dos.file[fHandle];
	//HAL_StartGTimer();
	//HAL_GlobalReadGTimer(&t0);
	if( fpos != 0 ){
		//printf("pos=%lx\n",fpos);
	}
	//printf("pos=%lx\n",fpos);
	//file->pos = fpos;
	//printf("ramAddr=%lx,fHandle=%x\n",ramAddr,fHandle);
	ramAddr = ramAddr << 1;
	//printf("ramAddr=%lx\n",ramAddr);
	//DOS_FileInfoGet(fHandle, FILE_SIZE, (UINT8 *)G_DOS_FileSize);
	cnt = DOS_FileRead(fHandle, ramAddr, readSize, fpos);
	#if 0
	dumpData(128, (ramAddr >> 1), 1);
	#endif
	//printf("%lx,readSize=%lx\n",cnt,readSize);
	//HAL_GlobalReadGTimer(&t1);
	//HAL_StopGTimer();
	//printf("File_LRead %ld\n",t1-t0);
	if(cnt != readSize ){
//printf("cnt=%lx,size=%lx,%bx\n",cnt,*size,FALSE);
		return FALSE;
	}
	else{
//printf("cnt=%lx,size=%lx,%bx\n",cnt,*size,TRUE);
		return TRUE;
	}
	//useless
	dstPath= dstPath;
}

UINT8 vfsFileRead(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos)
{
	return File_LRead( fHandle,  dstPath,  readSize,  ramAddr,  fpos);
}

/**
 * @brief	Write file
 * @param	name, rwMode, numCluster (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 File_LWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage)
{
	UINT32 cnt;
#if DEBUG
	UINT32 tmr0, tmr1;

	if (dosRwDbg) {
		HAL_GlobalReadGTimer(&tmr0);
	}
#endif
	ramAddr = ramAddr << 1;
	cnt = DOS_FileWrite(fHandle, ramAddr, writeSize);

#if DEBUG
    if (dosRwDbg) {
		HAL_GlobalReadGTimer(&tmr1);
		printf("f%lu\n", tmr1 - tmr0);
	}
#endif

	if(cnt != writeSize ){
		//printf("cnt=%lx,size=%lx,%bx\n",cnt,writeSize,FALSE);
		return FALSE;
	}
	else{
		//printf("cnt=%lx,size=%lx,%bx\n",cnt,writeSize,TRUE);
		return TRUE;
	}
	srcPath = srcPath;
	findHeaderPage = findHeaderPage;
}
UINT8 vfsFileWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage)
{
	return File_LWrite( fHandle,  srcPath,  writeSize,  ramAddr,  findHeaderPage);
}
UINT8 File_LWriteSD(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage)
{
	return File_LWrite( fHandle,  srcPath,  writeSize,  ramAddr,  findHeaderPage);
}
/**
 * @brief	Open a file
 * @param	name, rwMode, numCluster (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 vfsFileClose(UINT16 fHandle, UINT8* name, UINT8 rwMode, UINT32 fileSize)
{
	DOS_FileClose(fHandle);
	return TRUE;
	name = name;
	fileSize = fileSize;
	rwMode = rwMode;
}
#endif

/**
 * @brief	Offset file position
 * @param	fd, offset (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 File_Offset(UINT16 fHandle, UINT32 offset)
{
	xdata FILE *file;
	file   = (FILE *)&dos.file[fHandle];
	//printf("File_Offset %x %lx\n",fHandle,offset);

	file->pos = offset;
	return TRUE;
}

/**
 * @brief	Read sector data directly from storage
 * @param	logAddr, size (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 DOS_ReadMoreSector(UINT8 dstPath, UINT32	logAddr, UINT32 ramAddr,UINT32 size)
{
	UINT8 drivId = DOS_CurrDriverGet();
	UINT8 sts = TRUE;
	UINT32 k = k;
	switch(drivId){
#if DOS_DEV_SUPPORT_CARD        
		case DRIVE_SD :
			HAL_FlashMode(K_MEDIA_Sdf, K_MEDIA_NANDF, 1);
			//logAddr = logAddr >> 9;
			sts = SD_LCard2UsbMore(logAddr, size >> 9);
			break;
#endif //DOS_DEV_SUPPORT_CARD

#if DOS_DEV_SUPPORT_FLASH
		case DRIVE_NAND :
#if DEBUG
	printf("%s(%ld) drivId=%bx,logAddr=%lx,size=%lx,ramAddr=%lx\n",__FILE__,(UINT32)__LINE__,drivId,logAddr,size,ramAddr);
#endif
#if 0//ndef MTD_NEW
			sts = TRUE;
			for(k = 0; k < size && sts == TRUE; k += K_Card_PageSize){
				sts = SMC_LCard2Usb((logAddr << 9) + k, 0);
			}
#else
#if (DOS_RSV_PART == 1)
            if (DOS_RsvPartStsGet()) {
                logAddr += DOS_PartActOffsetGet();
            }
#endif
			//sts = SMC_LCard2Dram(logAddr, 0, size);
			sts = SMC_LCard2Usb((logAddr << 9), size);
#endif
			break;
#endif //DOS_DEV_SUPPORT_FLASH

#if DOS_DEV_SUPPORT_DRAM
#if ( DOSRAM_OPTION == 1 )
		case DRIVE_DRAM :
			DRAM_LCard2UsbMore(logAddr, size >> 9);
			break;
#endif
#if (DOS_RSV_PART == 1)
        case DRIVE_NAND_RSV:
            /*if (DOS_RsvPartStsGet() && DOS_RsvPartActGet()) {
                logAddr += DOS_PartActOffsetGet();
                //printf("More R offset: %lx\n", (UINT32)DOS_PartActOffsetGet());
            }*/
            
			//sts = SMC_LCard2Dram(logAddr, 0, size);
			sts = SMC_LCard2Usb((logAddr << 9), size);            
            break;
#endif
#endif //DOS_DEV_SUPPORT_DRAM
		default :
			break;
	}
	return sts;
	dstPath = dstPath;
	ramAddr = ramAddr;
}

/**
 * @brief	Write sector data directly from storage
 * @param	logAddr, size (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 DOS_WriteMoreSector(UINT8 srcPath, UINT32 logAddr, UINT32	ramAddr, UINT32 size)
{
	UINT8 drivId = DOS_CurrDriverGet();
	UINT8	sts	= FALSE;
	switch(drivId){
#if DOS_DEV_SUPPORT_CARD
		case DRIVE_SD :
			HAL_FlashMode(K_MEDIA_Sdf, K_MEDIA_NANDF, 1);
			//logAddr = logAddr >> 9;
			sts = SD_LUsb2CardMore(logAddr, size >> 9);
			break;
#endif //DOS_DEV_SUPPORT_CARD            

#if DOS_DEV_SUPPORT_FLASH
		case DRIVE_NAND :
#if DEBUG
	printf("%s(%ld) drivId=%bx,logAddr=%lx,size=%lx,ramAddr=%lx\n",__FILE__,(UINT32)__LINE__,drivId,logAddr,size,ramAddr);
#endif
#if (DOS_RSV_PART == 1)
            if (DOS_RsvPartStsGet()) {
                logAddr += DOS_PartActOffsetGet();
            }
#endif
			sts = SMC_LUsb2Card((logAddr << 9), size);
			break;
#endif //DOS_DEV_SUPPORT_FLASH            

#if DOS_DEV_SUPPORT_DRAM
#if ( DOSRAM_OPTION == 1 )
		case DRIVE_DRAM :
			DRAM_LUsb2CardMore(logAddr, size >> 9);
			break;
#endif
#if (DOS_RSV_PART == 1)
        case DRIVE_NAND_RSV:
            /*if (DOS_RsvPartStsGet() && DOS_RsvPartActGet()) {
                logAddr += DOS_PartActOffsetGet();
                //printf("More W offset: %lx\n", (UINT32)DOS_PartActOffsetGet());
            }*/
                
            sts = SMC_LUsb2Card((logAddr << 9), size);
            break;
#endif                    
#endif //DOS_DEV_SUPPORT_DRAM
		default :
			break;
	}
	return sts;
	srcPath = srcPath;
	ramAddr = ramAddr;
}

UINT8 DOS_ReadSector(UINT8 dstPath, UINT32 logAddr, UINT32 ramAddr)
{
	UINT16 i;
	UINT8 drivId = DOS_CurrDriverGet();
	logAddr = logAddr >> 9;
	//printf("1 logAddr[%lu]\n",logAddr);
	if ( ramAddr == 0 ){
		if( 0 == DOS_SectorRead(drivId, logAddr, 1, dos.TmpBufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
		M_DRAM_SetStartAddress(dos.TmpBufAddr, TRUE);
		for( i = 0; i < 512 ; i = i + 2 ){
			M_DRAM_ReadWord(G_ucStorData[i + 0],G_ucStorData[i + 1]);
		}
		#if 0
		dumpData(64, G_ucStorData, 0);
		#endif
	}
	else{
		if( 0 == DOS_SectorRead(drivId, logAddr, 1, ramAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	return TRUE;
	dstPath = dstPath;
	ramAddr = ramAddr;
}

UINT8 DOS_WriteSector(UINT8 srcPath, UINT32 logAddr, UINT32 ramAddr)
{
	UINT16 i;
	UINT8 drivId = DOS_CurrDriverGet();
	logAddr = logAddr >> 9;
	//printf("2 logAddr[%lu]\n",logAddr);
	if ( ramAddr == 0 ){
		M_DRAM_SetStartAddress(dos.TmpBufAddr, FALSE);
		for( i = 0; i < 512 ; i = i + 2 ){
			M_DRAM_WriteWord(G_ucStorData[i + 0],G_ucStorData[i + 1]);
		}
		if( 0 == DOS_SectorWrite(drivId, logAddr, 1, dos.TmpBufAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	else{
		if( 0 == DOS_SectorWrite(drivId, logAddr, 1, ramAddr)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	return TRUE;
	srcPath = srcPath;
	ramAddr = ramAddr;
}
#if 1
UINT32 DOS_FileCurrClusterGet(UINT8 fno)
{
	xdata FILE *file;
	UINT32 next;

	if (fno >= MAX_FILE) {
		return -1;
	}

	file = (FILE *)&dos.file[fno];

	FAT_FreeClusterGet(file->DevId, 0, &next);// only return continious free block

	return next;
}

UINT8 DOS_FileTagClusterSet(UINT8 fno, UINT32 cluster)
{
	xdata FILE *file;

	if (fno >= MAX_FILE) {
		return FAIL;
	}
	file   = (FILE *)&dos.file[fno];

	file->TargetCluster = cluster;

	return SUCCESS;
}
#endif
void DOS_FileCreateTimeByRtc( UINT8 withRtc )
{
	G_Dos_Create_Time_By_Rtc = withRtc;
}

UINT8 DOS_FileCreateTimeSet( dateStc_t *pdate )
{
	dos.date.Year = pdate->Year;
 	dos.date.Month = pdate->Month;
	dos.date.Day = pdate->Day;
	dos.date.Hour = pdate->Hour;
	dos.date.Minute = pdate->Minute;
	dos.date.Second = pdate->Second;

	return SUCCESS;
}

UINT8 DOS_FileCreateTimeGet( dateStc_t *pdate )
{

	if( G_Dos_Create_Time_By_Rtc ){
		HAL_GlobalReadRTC((pDateStc_t)pdate);
	}

	return SUCCESS;
}
#if 0 //move to vfs_api.c
/**
 * @brief		FILE_AttrSet
 * @param	fno, option, pData
 * @retval	SUCCESS / FAIL
 * @see
 * @author	Phil.lin
 * @since		08/08/21
 * @todo
 * @bug
*/
UINT8 FILE_AttrSet(
	UINT8 fno,
	UINT8 option,
	void* pData
)
{
	UINT8 ret;
	UINT8 DevId;
	//UINT8 *path, *name;
	//UINT32 max, curr, next;
	xdata FILE *file;
	xdata x86_FDB *fdb;//, tmpFdb;
	//xdata DISK *disk;
	UINT8 i8;
	UINT32 i32;
	UINT8 xdata* ptr;

	if (!fno) {
		return FAIL;
	}

	file = (FILE *)&dos.file[fno];
	fdb  = (x86_FDB *)&file->fdb;
	DevId  = file->DevId;

	//printf("attr is %bx\n", file->fdb.attr);

	/* modify file attrib */
	if (option & ATTR_MODIFY_NAME) {
		memcpy(fdb->name, (UINT8*)pData, sizeof(fdb->name));
	} else if (option & ATTR_MODIFY_ATTR) {
		fdb->attr = *(UINT8*)pData;
	}

	/* fdb write back */
	if( 0 == DOS_SectorRead(DevId, file->FDB_Sector, 1, dos_var_start)) return dosRwErr(__FILE,(unsigned short)__LINE__);
	//printf("%s %u, FDB_Sector=%lx\n", __FILE__, (unsigned short)__LINE__,file->FDB_Sector);
	i32 = ((UINT32)file->FDB_Idx << 4) + dos_var_start;	//1 FDB = 32 bytes
	//printf("i32 %lx \n", i32);
	//printf("file->FDB_Idx %bx \n", file->FDB_Idx);
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(i32,3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(i32,2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(i32,1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(i32,0);
	ptr = (UINT8 *)&file->fdb;
	for(i8=0; i8<32/2; i8++) {
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = *ptr++;
		XBYTE[REG_Dram_IndirectRData_High] = *ptr++;
	}

	if( 0 == DOS_SectorWrite(DevId, file->FDB_Sector, 1, dos_var_start)) return dosRwErr(__FILE,(unsigned short)__LINE__);

	//G_DOS_StartClusterNumber = file->FDB_Sector; //UINT32->UINT16?
	G_DOS_FdbAddress = ((UINT32)file->FDB_Sector << 9) + ((UINT32)file->FDB_Idx << 5);////mantis-26979
	//printf("file->FDB_Sector: %lx\n", file->FDB_Sector);
	//printf("file->FDB_Idx %bx \n", file->FDB_Idx);
	//printf("G_DOS_FdbAddress: %lx\n", G_DOS_FdbAddress);
	ret = USR_CheckType((UINT8 *)fdb);
	return SUCCESS;
}
/**
 * @brief		vfsFileAttrSet
 * @param	oldFileName, newFileName, Attr, option
 * @retval	SUCCESS / FAIL
 * @see
 * @author	Phil.lin
 * @since		08/08/21
 * @todo
 * @bug
*/
UINT8 vfsFileAttrSet(
	UINT8 const* oldFileName,
	UINT8 const* newFileName,
	UINT8 attr,
	UINT8 option
)
{
	UINT8 fHdl;
	UINT8 ret = SUCCESS;
	xdata FILE *file;
	xdata x86_FDB *fdb;

	if (oldFileName == NULL && newFileName == NULL) {
		return FAIL;
	}

	//printf("old name %s, new name %s, attr %bx, option %bx\n", oldFileName, newFileName, attr, option);

	fHdl = DOS_FileOpen(oldFileName, 'r');
	if (!fHdl) {
		return FAIL;
	}

	/* modify file name */
	if (option & ATTR_MODIFY_NAME) {
		if (newFileName) {
			ret |= FILE_AttrSet(fHdl, ATTR_MODIFY_NAME, (void*)newFileName);
		}
	}

	/* modify file attrib */
	if (option & ATTR_MODIFY_ATTR) {
		ret |= FILE_AttrSet(fHdl, ATTR_MODIFY_ATTR, (void*)&attr);
	}

	file = (FILE *)&dos.file[fHdl];
	fdb  = (x86_FDB *)&file->fdb;

	DOS_FileClose(fHdl);

	return ret;
}
#endif

/**
 * @brief	DOS_BlockAlign_Get
 * @param	file number id
 * @retval	size of block align.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
void DOS_CacheFlush( void )
{
	UINT8 driVid = DOS_CurrDriverGet();
    
#if DOS_DEV_SUPPORT_FLASH
	if( driVid == DRIVE_NAND ){
		if( flashCacheCtrlSts() ){
			flashCacheCtrl(0);
			flashCacheCtrl(1);
		}
	}
#endif //DOS_DEV_SUPPORT_FLASH

}

#if 0 //xian move to dos_utility.c
#if CARD_MOUNT_ISP
UINT32
fwUpdateAddrGet(
	void
)
{
	UINT32 rsvA,rsvB, rsvSize, tmp;
	tmp = 0;
	rsvA = ((UINT32)G_ucStorData[16] << 24) + ((UINT32)G_ucStorData[17] << 16) + \
	       ((UINT32)G_ucStorData[18] << 8) + ((UINT32)G_ucStorData[19] << 0);
	tmp += (rsvA * 10);
	rsvB = ((UINT32)G_ucStorData[20] << 24) + ((UINT32)G_ucStorData[21] << 16) + \
	       ((UINT32)G_ucStorData[22] << 8) + ((UINT32)G_ucStorData[23] << 0);
	tmp += (rsvB * 10);
	tmp += 10;// FW ID
	rsvSize = ((UINT32)G_ucStorData[24] << 24) + ((UINT32)G_ucStorData[25] << 16) + \
	       ((UINT32)G_ucStorData[26] << 8) + ((UINT32)G_ucStorData[27] << 0);
	tmp += rsvSize;//rsvA size
	rsvSize = ((UINT32)G_ucStorData[28] << 24) + ((UINT32)G_ucStorData[29] << 16) + \
	       ((UINT32)G_ucStorData[30] << 8) + ((UINT32)G_ucStorData[31] << 0);
	tmp += rsvSize;//rsvB size
	tmp += 32;//Hdr size
	tmp = (tmp + 1) >> 1 << 1;
	return tmp;
}

#if CARD_MOUNT_ISP
void
fwUpdateAdcGet(
	UINT8 index,
	UINT16* value
)
{
	UINT8 i = 0;
	ET0 = 0;
	XBYTE[REG_Audio_SarChnlSel] = index;
	HAL_Wait(1);//delay several cycle
	XBYTE[REG_Audio_SnapSarAdcVal0] = 0x01;
	*((UINT8*)value + 1) = XBYTE[REG_Audio_SnapSarAdcVal0];
	*((UINT8*)value + 0) = (XBYTE[REG_Audio_SnapSarAdcVal1]^0x02);
	ET0 = 1;
}

UINT8
fwUpdateConfirmOsd(
	UINT32 addr
)
{
	/* disable image layers. */
	UINT8 OsdMaxX, OsdMaxY;
	UINT32 i, j;
	UINT16 value0, value1;
	//UINT8 value;
	osdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	HAL_DispParaSet(HAL_DISP_PARA_IMG_LAYER_ENABLE, 0, 0, 0, 0, 0);
	XBYTE[REG_Disp_OsdH0L] &= ~0x01;
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
	HAL_DispParaSet(HAL_DISP_PARA_GLOB_BLEND_FACTOR, HAL_DISP_GLOB_BLEND_SRC_BLACK, 4+11, 0, 0, 0);
	HAL_DispParaSet(HAL_DISP_PARA_OSD_LAYER_ENABLE, 1, 0, 0, 0, 0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_OSD);
	HAL_DispParaSet(HAL_DISP_PARA_BG_COLOR, 0, 0, 200, 0, 0);  /* blue. */
	HAL_DispParaSet(HAL_DISP_PARA_OSD_WINDOW_OFFSET, 0, 0, 0, 0, 0);
	osdClear(0, (1<<4)+8);
	//osdStrDraw(OsdMaxX/2 - 4,OsdMaxY/2,"Do ISP ???", (1<<4)+0);
	osdStrDraw(0,0,"Do ISP ???", (3<<4)+0);
	osdStrDraw(0,1,"Press any key", (1<<4)+0);
	osdStrDraw(0,2,"to do ISP !!", (1<<4)+0);
	HAL_Wait(500);
	i = 0;
	j = 0;
	//HAL_GetGPIOBit(SD_CDn, 0);
	while(i < 0x50){
		fwUpdateAdcGet(1, &value0);
		//printf("1 %x\n",value0);
		fwUpdateAdcGet(2, &value1);
		//printf("2 %x\n",value1);
		if(((0x70 < value0) && (value0 < 0x370)) || ((0x70 < value1) && (value1 < 0x370))) {
			i ++;
			continue;
		}
		i = 0;
		HAL_Wait(1);
		j ++;
		//printf("j %lx\n",j);
		if( j > 5000 ) {
			osdClear(0, 0);
			HAL_DispParaSet(HAL_DISP_PARA_IMG_LAYER_ENABLE, 1, 0, 0, 0, 0);
			HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
			HAL_DispParaSet(HAL_DISP_PARA_BG_COLOR, 0, 0, 0, 0, 0);  /* blue. */
			HAL_DispParaSet(HAL_DISP_PARA_GLOB_BLEND_FACTOR, 0 << 4, 8, 0, 0, 0);
			HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_OSD);
			return FALSE;
		}
		/*
		HAL_GetGPIOBit(SD_CDn, &value);
		if(value & 0x08) {
			osdClear(0, (1<<4)+8);
			osdStrDraw(OsdMaxX/2 - 2,OsdMaxY/2,"Off ..", (1<<4)+0);
			HAL_Wait(2000);
			HAL_SetGPIOBit(PWR_ON_EN,0);
			while(1);
		}
		*/
	}
	osdClear(0, (1<<4)+8);
	osdStrDraw(OsdMaxX/2 - 2,OsdMaxY/2,"ISP...", (3<<4)+0);
	addr = addr;
	return TRUE;
}
void
fwUpdateOkOsd(
	UINT32 addr
)
{
	UINT8 OsdMaxX, OsdMaxY;
	osdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
	osdStrDraw(OsdMaxX/2 - 2,OsdMaxY/2,"OK !!!", (1<<4)+0);
	addr = addr;
}
#endif

/**
 * @brief	sp1kFwUpdateAddrSet
 * @param	file path/name, buf addr (Word)
 * @retval
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug
*/
void
sp1kFwUpdateAddrSet(
	UINT32 addr
)
{
	G_ISP_Addr = addr;
}

/**
 * @brief	sp1kCardFirmwareUpdate
 * @param	file path/name, buf addr (Word)
 * @retval
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug
*/
extern xdata UINT8    G_Firmware_Update;
UINT8
sp1kCardFirmwareUpdate(
	UINT8 *name,
	UINT32 addr
)
{
	UINT8 sts = SUCCESS, status, fd;
	UINT32 size, temp;
	UINT32 startAddr = K_SDRAM_CommonEndAddr;
	UINT8 OsdMaxX, OsdMaxY;
	UINT8 buf[3];

	if( addr != 0 ){
		startAddr = addr;
	}

	// skip CDSP write to DRAM
	buf[0] = XBYTE[REG_Front_Finten_low];
	buf[1] = XBYTE[REG_Cdsp_Cdspinten_lo];
	buf[2] = XBYTE[0x2277];
	XBYTE[REG_Front_Finten_low] = 0x00;
	XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
	XBYTE[0x2277] = 0x07;

	ENTER_CRITICAL(status);
	printf("FW %s, %lu KB\n",name, ((K_SDRAM_TotalSize - startAddr) << 1) >> 10);
	if( FALSE == dramFileLoad(name, startAddr, &size)){
		dosRwErr(__FILE,(unsigned short)__LINE__);
		EXIT_CRITICAL(status);
		XBYTE[REG_Front_Finten_low] = buf[0];
		XBYTE[REG_Cdsp_Cdspinten_lo] = buf[1];
		XBYTE[0x2277] = buf[2];
		return FAIL;
	}
	if( size > ((K_SDRAM_TotalSize - startAddr) << 1)){
		EXIT_CRITICAL(status);
		dosRwErr(__FILE,__LINE__);
		XBYTE[REG_Front_Finten_low] = buf[0];
		XBYTE[REG_Cdsp_Cdspinten_lo] = buf[1];
		XBYTE[0x2277] = buf[2];
		return FAIL;
	}
	//printf("size=%lx\n",size);
	/* Read 1st 32 bytes for BRN format check */
	SDRAM_Dram2Sram( startAddr, G_ucStorData, 512 );
	//if ( memcmp(((UINT8 *)G_ucStorData), "SUNPLUS", 7) != 0 ) {//it's fw.bin
	if( G_ucStorData[0] == 'S' && G_ucStorData[1] == 'U' && \
		G_ucStorData[2] == 'N' && G_ucStorData[3] == 'P' && \
		G_ucStorData[4] == 'L' && G_ucStorData[5] == 'U' && \
		G_ucStorData[6] == 'S')//it's 1528.bin
	{
		fd = DOS_FileOpen("ISP.BIN", 'r');
		if(fd != 0) {
			temp = startAddr + ((size + 1) >> 1);
			HAL_GprmDramFill(temp, 1024, 0xFF);
			//This will be used inside isp.bin to set the start wroking address
			//Use these register to save the address
			XBYTE[0x258B] = (UINT8)temp;
			XBYTE[0x258C] = (UINT8)(temp >> 8);
			XBYTE[0x258D] = (UINT8)(temp >> 16);
			XBYTE[0x258A] = (UINT8)(temp >> 24);
			SDRAM_CardSetStartAddress(temp, FALSE);
			//Keep 1528.bin size
			M_DRAM_WriteWord((UINT8)size, (UINT8)(size >> 8));
			M_DRAM_WriteWord((UINT8)(size >> 16), (UINT8)(size >> 24));
			//Keep 1528.bin starting addr
			M_DRAM_WriteWord((UINT8)startAddr, (UINT8)(startAddr >> 8));
			M_DRAM_WriteWord((UINT8)(startAddr >> 16), (UINT8)(startAddr >> 24));
			//Where to put isp.bin, offset 1024 bytes to put the 1528.bin size, address and isp.bin size
			//This value will be used inside isp.bin
			startAddr += ((size + 1 + 1024) >> 1);
			DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
			DOS_FileRead(fd, startAddr << 1, size, 0);
			DOS_FileClose(fd);
			G_ISPDRAMAddr = startAddr;
			G_BulkSize = size;
			//Keep isp.bin size
			SDRAM_CardSetStartAddress(temp + 4, FALSE);
			M_DRAM_WriteWord((UINT8)G_BulkSize, (UINT8)(G_BulkSize >> 8));
			M_DRAM_WriteWord((UINT8)(G_BulkSize >> 16), (UINT8)(G_BulkSize >> 24));
			//if DRAMPARA.TXT exist, load it to Workin address + 256(WORD)
			fd = DOS_FileOpen("DRAMPARA.TXT", 'r');
			if(fd != 0) {
				DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
				//if DRAMPARA.TXT exist, load it to Workin address + 256(WORD)
				DOS_FileRead(fd, (temp + 256) << 1, size, 0);
			}
			else{
				//if no dram para file, load from nand/spi block 0, page 1
				flashLoadDramPara(temp + 256);
				size = 512;
			}
			DOS_FileClose(fd);
			//DRAM PARA size
			SDRAM_CardSetStartAddress(temp + 6, FALSE);
			M_DRAM_WriteWord((UINT8)size, (UINT8)(size >> 8));
			M_DRAM_WriteWord((UINT8)(size >> 16), (UINT8)(size >> 24));
			//LCD window
			osdLayoutAttrGet(&OsdMaxX, &OsdMaxY);
			M_DRAM_WriteWord((UINT8)OsdMaxX, (UINT8)(OsdMaxX >> 8));
			M_DRAM_WriteWord((UINT8)OsdMaxY, (UINT8)(OsdMaxY >> 8));
			//osd display buffer
			M_DRAM_WriteWord((UINT8)K_SDRAM_OsdDispBufAddr_A, (UINT8)(K_SDRAM_OsdDispBufAddr_A >> 8));
			M_DRAM_WriteWord((UINT8)(K_SDRAM_OsdDispBufAddr_A >> 16), (UINT8)(K_SDRAM_OsdDispBufAddr_A >> 24));
			//char/font buffer
			M_DRAM_WriteWord((UINT8)K_SDRAM_OsdFontBufAddr, (UINT8)(K_SDRAM_OsdFontBufAddr >> 8));
			M_DRAM_WriteWord((UINT8)(K_SDRAM_OsdFontBufAddr >> 16), (UINT8)(K_SDRAM_OsdFontBufAddr >> 24));
			bootCore();
			//printf("open err...\n");
		}
		DOS_FileClose(fd);
		startAddr = startAddr + (fwUpdateAddrGet() >> 1) + 1;
	}
	//else{//supposed to be fw.bin
	G_Firmware_Update = 0x5a;//to do update
	sp1kDiskRsvWrite(0x100, startAddr << 1);
	//}

	EXIT_CRITICAL(status);
	printf("OK..\n");
	return sts;
}
#endif
#if CARD_MOUNT_ISP
/**
 * @brief	sp1kCardFirmwareBoot
 * @param	file path/name, buf addr (Word)
 * @retval
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug
*/
UINT8
sp1kCardFirmwareBoot(
	UINT8 *name,
	UINT32 addr
)
{
	UINT8 sts = SUCCESS;
	UINT32 size;
	UINT32 startAddr = K_SDRAM_CommonEndAddr;
	UINT8 buf[3];

	if( addr != 0 ){
		startAddr = addr;
	}
	// skip CDSP write to DRAM
	buf[0] = XBYTE[REG_Front_Finten_low];
	buf[1] = XBYTE[REG_Cdsp_Cdspinten_lo];
	buf[2] = XBYTE[0x2277];
	XBYTE[REG_Front_Finten_low] = 0x00;
	XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
	XBYTE[0x2277] = 0x07;

	printf("Boot %s, %lu KB\n",name, ((K_SDRAM_TotalSize - startAddr) << 1) >> 10);
	if( FALSE == dramFileLoad(name, startAddr, &size)){
		dosRwErr(__FILE,(unsigned short)__LINE__);
		//EXIT_CRITICAL(sts);
		XBYTE[REG_Front_Finten_low] = buf[0];
		XBYTE[REG_Cdsp_Cdspinten_lo] = buf[1];
		XBYTE[0x2277] = buf[2];
		return FAIL;
	}
	if( size > ((K_SDRAM_TotalSize - startAddr) << 1)){
		//EXIT_CRITICAL(status);
		dosRwErr(__FILE,__LINE__);
		XBYTE[REG_Front_Finten_low] = buf[0];
		XBYTE[REG_Cdsp_Cdspinten_lo] = buf[1];
		XBYTE[0x2277] = buf[2];
		return FAIL;
	}
	#if 1
	/* Read 1st 32 bytes for BRN format check */
	SDRAM_Dram2Sram( startAddr, G_ucStorData, 512 );
	if( G_ucStorData[0] == 'S' && G_ucStorData[1] == 'U' && \
		G_ucStorData[2] == 'N' && G_ucStorData[3] == 'P' && \
		G_ucStorData[4] == 'L' && G_ucStorData[5] == 'U' && \
		G_ucStorData[6] == 'S')//it's 1528.bin
	{
		{
			G_ISPDRAMAddr = startAddr + (fwUpdateAddrGet() >> 1) + 1;
		}
		G_BulkSize = 0x80000;
		//dumpData(512, G_ISPDRAMAddr, 1);
	}
	else{//supposed to be fw.bin
		G_ISPDRAMAddr = startAddr;
		G_BulkSize = size;
	}
	#else
	G_ISPDRAMAddr = startAddr;
	G_BulkSize = size;
	#endif
	bootCore();

	//EXIT_CRITICAL(sts);
	return sts;
}
#endif

#if CARD_MOUNT_ISP
UINT8
fwUpdateDo(
	UINT32 addr
)
{
	UINT8 i, file;
	UINT8 sts = FAIL, fd;
	UINT32 size;
	UINT32 startAddr = K_SDRAM_CommonEndAddr;
	if( addr != 0 ){
		startAddr = addr;
	}
	fd = DOS_FileOpen("ISPKEY.TXT", 'r');
	if ( fd==0 ) {
		//dosRwErr(__FILE,(unsigned short)__LINE__);
		return FAIL;
	}
	HAL_GprmDramFill(startAddr, 512, 0x00);
	DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)&size);
	DOS_FileRead(fd, startAddr << 1, 512, 0);
	DOS_FileClose(fd);
	//printf("size=%ld\n",size);
	if( size != 343 ){
		dosRwErr(__FILE,(unsigned short)__LINE__);
		return FAIL;
	}
	SDRAM_Dram2Sram( startAddr, G_ucStorData, 512 );
	sts = FALSE;
	//dumpData(512, &G_ucStorData[0], 0);
	if( G_ucStorData[0] == 0x54 && G_ucStorData[0x11] == 0x4F && \
		G_ucStorData[0x22] == 0x69 && G_ucStorData[0x33] == 0x40 && \
		G_ucStorData[0x44] == 0x61 && G_ucStorData[0x55] == 0x67 && \
		G_ucStorData[0x66] == 0x74 && G_ucStorData[0x77] == 0x6E)
	{
		//printf("magic\n");
		i = 0;
		if(DOS_FileOpen("FW.BIN", 'r') != 0){
			file = 1;
			DOS_FileClose(fd);
		}
		else if(DOS_FileOpen("1528.BIN", 'r') != 0){
			file = 2;
			DOS_FileClose(fd);
		}
		else{
			file = 0;
			dosRwErr(__FILE,(unsigned short)__LINE__);
			return FAIL;
		}
		if(FALSE == fwUpdateConfirmOsd(startAddr)){
			dosRwErr(__FILE,(unsigned short)__LINE__);
			return FAIL;
		}
		fd = DOS_FileOpen("OXOXOXOX.XOX", 'r');
		if ( fd == 0 ) {
			DOS_FileDel("ISPKEY.TXT");
			osdStrDraw(0,0 + i,"Del ISPKey", (1<<4)+0);
			i++;
		}
		else{
			osdStrDraw(0,0 + i,"OXOXOXOX.XOX", (1<<4)+0);
			i++;
		}
		DOS_FileClose(fd);
		if( file == 2 ){
			osdStrDraw(0,0 + i,"1528.BIN", (1<<4)+0);
			i++;
			if(DOS_FileOpen("DRAMPARA.TXT", 'r') != 0){
				osdStrDraw(0,0 + i,"DRAMPARA.TXT", (1<<4)+0);
				i++;
				DOS_FileClose(fd);
			}
			sts = sp1kCardFirmwareUpdate("1528.BIN", startAddr);
		}
		else{
			osdStrDraw(0,0 + i,"FW.BIN", (1<<4)+0);
			i++;
			sts = sp1kCardFirmwareUpdate("FW.BIN", startAddr);
		}
		if( sts == SUCCESS ){
			fwUpdateOkOsd(startAddr);
			HAL_SetGPIOBit(PWR_ON_EN,0);
			HAL_Wait(3000);
		}
	}
	return sts;
}
#endif
#endif	//xian --

/**
 * @brief	DOS_BlockAlign_Get
 * @param	file number id
 * @retval	size of block align.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
//#if(RSV_OPTION == NAND_OPTION)
#if 0
extern UINT32 G_NANDF_BlockSize;
//#endif
UINT32 DOS_BlockAlign_Get( UINT8 fno )
{
	fno = fno;
	return 0; //disable aligment function

	UINT8 	DevId;
 	UINT32 blkSizeBySector;
 	UINT32 sec;
	xdata DISK *disk;
	DevId = dos.file[fno].DevId;
	if( DevId != DRIVE_NAND ){
		return 0;//design for Nand
	}
//#if(RSV_OPTION == NAND_OPTION)
 	blkSizeBySector = G_NANDF_BlockSize >> 9;
//#endif
	disk = (DISK *)&dos.disk[DevId];
	sec = disk->FreeCluster << disk->bitCluster2Sector;
#if DEBUG
	printf("%s %u sec=%lx,FreeCluster=%08lx bitCluster2Sector=%bx\n", __FILE__, (unsigned short)__LINE__,sec,disk->FreeCluster,disk->bitCluster2Sector);
#endif
	sec = sec % blkSizeBySector;
#if DEBUG
	printf("%s %u sec=%lx\n", __FILE__, (unsigned short)__LINE__,sec);
#endif
	if(sec){
		return (blkSizeBySector - sec);
	}
	else{
		return sec;
	}
}
#endif
#if 0
//#if(RSV_OPTION == NAND_OPTION)
UINT32 DOS_Do_Cluster2Block_Erase(UINT8 DevId, UINT32 cluster)
{
 	UINT32 sector, phyBlkAddr;
	UINT16 tmp,logBlk;
	UINT8 d1,d2;

	sector = FAT_Cluster2Sector(DevId, cluster);
	MTD_SelfShiftL8(sector);
	sector = sector << 1;
	SMC_ReadLpaToPpa(sector, &phyBlkAddr);
	if( phyBlkAddr == 0xffffffff ){
		return 0xffffffff;
	}
	SMC_RedunCard2Sram(phyBlkAddr);
	//dumpData(16, &G_ucStorData[K_Card_PageSize], 0);
	d1 = G_ucStorData[K_Card_PageSize + 6];
	d2 = G_ucStorData[K_Card_PageSize + 7];
	if(( d1 == 0xff ) && ( d2 == 0xff )){//already erased
		return 0xffffffff;
	}
	logBlk = SMC_CalcLba(d1, d2);
	//dumpData(128, G_SMC_EntryTable, 0);
	//dumpData(128, G_SMC_AllocatTable, 0);
#if 0
printf("%s %u d1=%bx,d2=%bx,logBlk=%x,blk=%lx\n", __FILE__, (unsigned short)__LINE__,d1,d2,logBlk, phyBlkAddr/G_NANDF_BlockSize);
#endif
	tmp = G_SMC_AllocatTable[logBlk];
	G_SMC_EntryTable[tmp >> 3] |= (0x01 << (tmp % 8));
	G_SMC_AllocatTable[logBlk] = 0xffff;
	SMC_EraseOneBlock(phyBlkAddr);
	//dumpData(128, G_SMC_EntryTable, 0);
	//dumpData(128, G_SMC_AllocatTable, 0);
	return phyBlkAddr;
}
//#endif

/**
 * @brief	DOS_BlockAlign_Get
 * @param	file number id
 * @retval	size of block align.
 * @see
 * @author	Eric Su
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
//#if(RSV_OPTION == NAND_OPTION)
UINT32 G_ClusterAlreadyErase = 0;
UINT8 DOS_FreeBlock_Erase( UINT8 reset, UINT32 *retBlkNum , UINT32 nrBlkFind)
{
	UINT32 t0, t1;
	UINT8 	DevId;
 	UINT32 blkSizeBySector, sec, phyAddr;
 	UINT32 cluster, next, secUnErase;//, maxClust;
	xdata DISK *disk;
	DevId = DOS_CurrDriverGet();
	if( DevId != DRIVE_NAND || (G_NANDF_BlockSize == 0)){
		//*startBlk = 0xffffffff;
		*retBlkNum = 0;//Not Nand is 0
		return TRUE;
	}
	sp1kHalReadGTimer(&t0);
	disk = (DISK *)&dos.disk[DevId];
	(*retBlkNum) = 0;
	secUnErase = 0;
 	blkSizeBySector = G_NANDF_BlockSize;
	MTD_SelfShiftR8(blkSizeBySector);
	blkSizeBySector = blkSizeBySector >> 1;
	nrBlkFind = nrBlkFind * blkSizeBySector;
	nrBlkFind = nrBlkFind >> disk->bitCluster2Sector;
	if( reset ){
		G_ClusterAlreadyErase = 0;
	}
	cluster = disk->FAT_FirstFreeCluster + G_ClusterAlreadyErase;
	if( disk->TotalCluster <= cluster ){
		G_ClusterAlreadyErase = 0;
		return TRUE;
	}
#if DEBUG
	printf("%s %u cluster=%lx, nrBlkFind=%lx, tolClust=%lx\n", __FILE__, (unsigned short)__LINE__,cluster, nrBlkFind, disk->TotalCluster);
#endif
	do{
		sec = cluster << disk->bitCluster2Sector;
		if( sec % blkSizeBySector ){
#if DEBUG
	printf("%s %u sec=%lx, %lx\n", __FILE__, (unsigned short)__LINE__,sec % blkSizeBySector, secUnErase);
#endif
			if( secUnErase != 0 ){
				secUnErase += disk->Cluster2Sector;
				G_ClusterAlreadyErase ++ ;
				if( secUnErase >= blkSizeBySector ){
					secUnErase = secUnErase % disk->Cluster2Sector;
					phyAddr = DOS_Do_Cluster2Block_Erase(DevId, cluster);
					if(phyAddr == 0xffffffff){
						continue;
					}
					(*retBlkNum) ++;
#if DEBUG
	printf("%s %u sector=%lx,cluster=%lx,blk=%lx\n", __FILE__, (unsigned short)__LINE__,(sector >> 9),cluster,phyBlkAddr/G_NANDF_BlockSize);
#endif
				}
			}
		}
		else{
			secUnErase += disk->Cluster2Sector;
			G_ClusterAlreadyErase ++ ;
			if( disk->Cluster2Sector == blkSizeBySector ){
				phyAddr = DOS_Do_Cluster2Block_Erase(DevId, cluster);
				if(phyAddr == 0xffffffff){
					continue;
				}
				(*retBlkNum) ++;
#if DEBUG
	printf("%s %u (*retBlkNum)=%lx\n", __FILE__, (unsigned short)__LINE__,(*retBlkNum));
#endif
			}
#if DEBUG
	printf("%s %u secUnErase=%lx,cluster=%lx,blk=%lx\n", __FILE__, (unsigned short)__LINE__,secUnErase,cluster,phyBlkAddr/G_NANDF_BlockSize);
#endif
		}
		if( disk->TotalCluster <= cluster ){
#if DEBUG
	printf("%s %u cluster=%lx,nrBlkFind=%lx\n", __FILE__, (unsigned short)__LINE__,cluster,nrBlkFind);
#endif
			break;
		}
		while(nrBlkFind){
#if DEBUG
	printf("%s %u cluster=%lx,next=%lx\n", __FILE__, (unsigned short)__LINE__,cluster,next);
#endif
			next = FAT_TableRead(DevId, cluster);
			if( next != 0 ){
#if DEBUG
	printf("%s %u cluster=%lx,next=%lx\n", __FILE__, (unsigned short)__LINE__,cluster,next);
#endif
				cluster ++;
				if( nrBlkFind == 0){
					goto DOS_FreeBlock_Erase_lp;
				}
				nrBlkFind --;
				secUnErase = 0;
				continue;
			}
			cluster ++;
			break;
		}
	}while( --nrBlkFind );
DOS_FreeBlock_Erase_lp:
	sp1kHalReadGTimer(&t1);
	//printf("t=%lx\n", t1-t0);
	return TRUE;
}
//#endif

//#if(RSV_OPTION == NAND_OPTION)
UINT32 G_SectorUnErase = 0;
//#endif
UINT8 DOS_Cluster2Block_Erase( UINT8 DevId, UINT32 cluster, UINT32 next )
{
 	UINT32 blkSizeBySector;
 	UINT32 sec,phyBlkAddr;
	xdata DISK *disk;
	devAttr_t* pDev;//add by phil for disk driver lib release

	/* add by phil for disk driver lib release s*/
	pDev = sp1kDevHdlGet();
	pDev += DEV_ID_DISK;
	if (pDev->devType != DEV_TYPE_DISK_NAND) {
		return TRUE;
	}
	/* add by phil for disk driver lib release e*/

	if( DevId != DRIVE_NAND || (G_NANDF_BlockSize == 0)){
		return TRUE;
	}
	if( next != cluster + 1) {
		G_SectorUnErase = 0;
		return TRUE;
	}
 	blkSizeBySector = G_NANDF_BlockSize;

	MTD_SelfShiftR8(blkSizeBySector);
	blkSizeBySector = blkSizeBySector >> 1;

	disk = (DISK *)&dos.disk[DevId];

	sec = cluster << disk->bitCluster2Sector;
#if DEBUG
	printf("%s %u sec=%lx,FreeCluster=%08lx bitCluster2Sector=%bx\n", __FILE__, (unsigned short)__LINE__,sec,disk->FreeCluster,disk->bitCluster2Sector);
#endif
#if DEBUG
	printf("%s %u sec=%lx\n", __FILE__, (unsigned short)__LINE__,sec % blkSizeBySector);
#endif
	if( sec % blkSizeBySector ){
		if( G_SectorUnErase == 0 ){
#if DEBUG
	printf("%s %u sec=%lx\n", __FILE__, (unsigned short)__LINE__,sec % blkSizeBySector);
#endif
			return TRUE;
		}
		G_SectorUnErase += disk->Cluster2Sector;
#if DEBUG
	printf("%s %u Pile=%lx\n", __FILE__, (unsigned short)__LINE__,G_SectorUnErase);
#endif
		if( G_SectorUnErase >= blkSizeBySector ){
			G_SectorUnErase = G_SectorUnErase % disk->Cluster2Sector;
			phyBlkAddr = DOS_Do_Cluster2Block_Erase(DevId, cluster);
#if DEBUG
	printf("%s %u sector=%lx,cluster=%lx,blk=%lx\n", __FILE__, (unsigned short)__LINE__,(sector >> 9),cluster,phyBlkAddr/G_NANDF_BlockSize);
#endif
		}
	}
	else{
		G_SectorUnErase = disk->Cluster2Sector;
		if( disk->Cluster2Sector == blkSizeBySector ){
			phyBlkAddr = DOS_Do_Cluster2Block_Erase(DevId, cluster);
#if DEBUG
	printf("%s %u sector=%lx,cluster=%lx,blk=%lx\n", __FILE__, (unsigned short)__LINE__,(sector >> 9),cluster,phyBlkAddr/G_NANDF_BlockSize);
#endif
		}
	}
	return TRUE;
}
#endif
UINT8 DOS_FileCurrSectorWrite(UINT8 fno, UINT32 srcAddr)
{
    UINT32 err = TRUE;
#if 0 //xian -- 20110809 temp for code size
	fno = fno;
	srcAddr = srcAddr;
#else
    xdata FILE *file;
    xdata DISK *disk;
    UINT8  DevId;
    UINT16 offset;
    UINT32 curr_pos;
    UINT32 sector;

    if (fno >= MAX_FILE) {
        return dosRwErr(__FILE,__LINE__);    
    }
    
    file   = (FILE *)&dos.file[fno];
    DevId  = file->DevId;
    disk   = (DISK *)&dos.disk[DevId];
    curr_pos = file->pos;

    if(curr_pos > file->size)
        return dosRwErr(__FILE,__LINE__);    
        
    offset  = (curr_pos % disk->Cluster2Byte) >> 9;
    sector = FAT_Cluster2Sector(file->DevId, file->CurrCluster) + offset;

    err = DOS_SectorWrite(DevId, sector, 1, srcAddr);
#endif
    return err;
}

