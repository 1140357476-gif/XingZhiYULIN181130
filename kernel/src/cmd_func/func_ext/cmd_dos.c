/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
 
/**
 * @file		cmd_dos.c
 * @brief		
 * @author		Phil Lin
 * @since		2009-11-25
 * @date		2009-11-25
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "dos32.h"
#include "hal.h"
#include "cmd.h"
#include "cardopt.h"
//#include "cardfunc.h"
#include "Dbg_wdt.h"
#include "doslink.h"
#include "dosusr.h"
#include "sp1k_hal_api.h"
#include "cmd_ext.h"
#include "sp1k_disk_api.h"
#include "Sp1k_util_api.h"
#include "Sp1k_dcf_api.h"
#include "Sp1k_vfs_api.h"

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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdata UINT8 G_ucStorageType;
extern xdata UINT8 G_Card_Type;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------

#if (DOS_CMD_OPTION == 1)
extern DOS dos;
void dosDirCmd(UINT8* ptr) USING_0
{
	//UINT8 status;
#if 1//def DOS32_SUPPORT
	FDB RetFdb;
	UINT8 ret, num=0, i8;
	UINT8 disk;
	
	ptr = ptr;
	disk = DOS_CurrDriverGet();
	if( disk >= MAX_DEV ){
		printf("No drive\n");
		return;
	}
	if (disk == DRIVE_SD) {
		disk = 'D';
	} else if (disk == DRIVE_NAND) {
		disk = 'C';
	} else {
		disk += 'C'; 
	}

	printf("\n Directory of %c:\\\n\n", disk);
	
	//ENTER_CRITICAL( status );
	DIR_SearchReset();// reset will search from 0
	while(1) {
		// search 1 file a times from previous index
		// if search JPG file, ret = DIR_Search(0, "????????JPG", (FDB *)&RetFdb, 3);
		// if ret = FAIL means search no file
		ret = DIR_Search(DOS_CurrDriverGet(), "*", (FDB *)&RetFdb, 3);
		if(ret==FIND_FILE_FAIL)
			break;
		for(i8=0; i8<8; i8++)
			printf("%c", RetFdb.name[i8]);
		if(RetFdb.name[i8] != ' ');
			printf(".");
		for(; i8<11; i8++)
			printf("%c", RetFdb.name[i8]);
		if(RetFdb.attr & ATTR_D)
			printf(" <DIR>   ");
		else if(RetFdb.attr & ATTR_L)
			printf(" <VOL>   ");
		else
			printf(" %8lu",RetFdb.size);
		#if 0
		printf(" %02bu/%02bu/%02bu %02bu:%02bu:%02bu ",\
						RetFdb.Year,RetFdb.Month,RetFdb.Day,\
						RetFdb.Hour,RetFdb.Minute,RetFdb.Second);
		#endif
		printf(" %-8lu\n",RetFdb.start);
		dbgWdtFeed(10000);
	}
	//EXIT_CRITICAL( status );
#else
	ptr = ptr;
	//ENTER_CRITICAL( status );
	//printf("dir\n");
	G_File_Display = 1;
	dcfDirInitialize();
	//dcfFileInitialize();
	printf("\nDisk Size : %ld Bytes/ %ld KB\n",USR_DiskFreeSizeGet(), USR_DiskFreeSizeGet()/1024);
	G_File_Display = 0;
	//EXIT_CRITICAL( status );
#endif
}
/*
void dosCdCmd(UINT8* ptr) USING_0
{
	//UINT32 v1,v2;
	//printf("%s\n",ptr);
	G_File_Display = 1;
	if (strcmp(ptr, "C:") == 0) {
		//printf("C:..\n");
	}
	else if (strcmp(ptr, "D:") == 0) {
		//printf("D:..\n");
	}
	G_File_Display = 0;
}
void dosMkCmd(UINT8* ptr) USING_0
{
	//UINT32 v1,v2;
	ptr = ptr;
	//printf("%s\n",ptr);
	G_File_Display = 1;
	DOS_MakeDir(ptr, 1);
	G_File_Display = 0;
}
*/

void dosInfoCmd(UINT8* ptr) USING_0
{
#if 1//def DOS32_SUPPORT
	if (strcmp(ptr, "C") == 0) {
		DOS_InfoShow(DRIVE_NAND);
	}
	if (strcmp(ptr, "D") == 0) {
		DOS_InfoShow(DRIVE_SD);
	}
	if (strcmp(ptr, "E") == 0) {
		DOS_InfoShow(DRIVE_DRAM);
	}
#if (DOS_RSV_PART == 1)
	if (strcmp(ptr, "F") == 0) {
		DOS_InfoShow(DRIVE_NAND_RSV);
	}    
#endif    
#else
	ptr=ptr;
	printf("G_Card_TotalSizeMB     = %d MB\n",G_Card_TotalSizeMB);
	printf("G_DOS_SystemId         = 0x%bx ",G_DOS_SystemId);
	if( G_DOS_SystemId == 0x01 ){
		printf("(FAT12)\n");
	}
	else if( G_DOS_SystemId == 0x06 ){
		printf("(FAT16)\n");
	}
	else if( G_DOS_SystemId == 0x0B ){
		printf("(FAT32)\n");
	}
	else{
		printf("(???)\n");
	}
	printf("G_DOS_FileSystemType   = 0x%x\n",G_DOS_FileSystemType);
	printf("G_DOS_SectorSize       = 0x%x\n",G_DOS_SectorSize);
	printf("G_DOS_ClusterSize      = 0x%lx\n",G_DOS_ClusterSize);
	printf("G_DOS_SectorPerFat     = 0x%x\n",G_DOS_SectorPerFat);
	printf("G_DOS_TotalRootDir     = 0x%x\n",G_DOS_TotalRootDir);
	printf("G_DOS_TotalFreeCluster = 0x%x\n",G_DOS_TotalFreeCluster);
	printf("G_Card_BytePerSector   = 0x%x\n",G_Card_BytePerSector);
	printf("G_Card_SectorPerCard   = 0x%x\n",G_Card_SectorPerCard);
#endif
}
void dosFmtCmd(UINT8* ptr) USING_0
{
	//UINT32 v1,v2;
	UINT32 sts = FALSE;
	//UINT8 status;
	//ENTER_CRITICAL( status );
	UINT8 devID = DOS_CurrDriverGet();
    
	if (strcmp(ptr, "C") == 0) {
		G_Card_Type = K_MEDIA_NANDF;
		G_ucStorageType = G_Card_Type;
		sts = TRUE;
#if (DOS_RSV_PART == 1)
        DOS_RsvPartActSet(0);
#endif
	}
	if (strcmp(ptr, "D") == 0) {
		G_Card_Type = K_MEDIA_SDF;
		G_ucStorageType = G_Card_Type;
		sts = TRUE;
	}
	if (strcmp(ptr, "E") == 0) {
		G_Card_Type = K_MEDIA_DOSRAM;
		G_ucStorageType = G_Card_Type;
		sts = TRUE;
	}
#if (DOS_RSV_PART == 1)
	if (strcmp(ptr, "F") == 0) {
		G_Card_Type = K_MEDIA_NANDF;
		G_ucStorageType = G_Card_Type;
		sts = TRUE;
        //DOS_LableStrSet("DISK ROM");
        if (DOS_RsvPartSizeGet()) {
            DOS_RsvPartActSet(1);
        } else {
            return;
        }
	}    
#endif
	if( sts == TRUE ){
		//printf("%s,%d,G_Card_Type=%bx\n",__FILE__,__LINE__, G_Card_Type);
		sts = diskInitialize(1);
		//printf("%s,%d,G_DOS_Initial_Status=%bx\n",__FILE__,(UINT16)__LINE__, G_DOS_Initial_Status);
		//printf("Card init fail!\n");
		diskFormat(0x41);
		//printf("%s,%d,sts=%lx\n",__FILE__,__LINE__, sts);
#if (DOS_RSV_PART == 1)
        if (devID == DRIVE_NAND_RSV) {
            DOS_RsvPartActSet(1);
        }
#endif        
		sts = diskInitialize(1);
	}
	//EXIT_CRITICAL( status );
}
void dosNameParsing(UINT32 nameLen, UINT8 *pstr, UINT8 *name) USING_0
{
	UINT32 j;
	UINT32 i,mk;
	i = nameLen - 1;
	j = 10;
	mk = 0;
	while(1){
		if( pstr[i] != '.' && i > 0){
			name[j] = pstr[i];
			if( j == 0 ){
				break;
			}
			j--;
		}
		if( i == 0  && mk == 0 ){
			name[j] = pstr[i];
			if( j == 0 ){
				break;
			}
			j--;
			mk = 1;
		}
		if( mk ){
			name[j]	= ' ';
			if( j == 0 ){
				break;
			}
			j--;
		}
		else{
			i--;
		}
	}
	name[11] = '\0';
}
void dosCdCmd(UINT8* ptr) USING_0
{
#if 1//def DOS32_SUPPORT
	UINT8 name[32];
	UINT8 *pstr;
	UINT32 nameLen;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	else {
		//printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	nameLen = (UINT8 *)ptr - pstr;
	memcpy(name, pstr, nameLen);
	name[nameLen] = '\0';
	cmdWhiteSpaceStrip(&ptr);
	if( 0 != DIR_Change(name) ){
		//printf("CD %s fail\n",name );
	}
#else
	*ptr = *ptr;
#endif
}

void dosWriteCmd(UINT8* ptr) USING_0
{
	UINT32 v1,v2, t0, t1;
	UINT8 name[14];
	UINT8 *pstr;
	UINT32 nameLen;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	else {
		//printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	nameLen = (UINT8 *)ptr - pstr;
	dosNameParsing(nameLen,pstr,name);
	cmdWhiteSpaceStrip(&ptr);
	v1 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	cmdWhiteSpaceStrip(&ptr);
	v2 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	sp1kHalReadGTimer(&t0);
	if(dramFileSave(name, v1, v2)){
		sp1kHalReadGTimer(&t1);
		printf("file:%s,addr=0x%lx,size=%lx,time=%ld\n",name,v1,v2, t1- t0);
	}
}

void dosReadCmd(UINT8* ptr) USING_0
{
	UINT32 v1,v2, t0, t1;
	UINT8 name[14];
	UINT8 *pstr;
	UINT32 nameLen;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	else {
		//printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	nameLen = (UINT8 *)ptr - pstr;
	dosNameParsing(nameLen,pstr,name);
	cmdWhiteSpaceStrip(&ptr);
	v1 = strtoul(ptr, 0, 0);
	cmdTokenStrip(&ptr);
	sp1kHalReadGTimer(&t0);
	if(dramFileLoad(name, v1, &v2)){
		sp1kHalReadGTimer(&t1);
		printf("file:%s,addr=0x%lx,size=%lx,time=%ld\n",name,v1,v2, t1- t0);
	}
}

void dosDelCmd(UINT8* ptr) USING_0
{
	UINT32 sts;
	UINT8 name[12];
	UINT8 *pstr;
	UINT32 nameLen;
	UINT8 deleteAll;
	if ( ptr[0] != '\0' ) {
		cmdWhiteSpaceStrip(&ptr);
		pstr = ptr;
	}
	else {
		//printf( "Invalid input\n" );
	}
	cmdTokenStrip(&ptr);
	nameLen = (UINT8 *)ptr - pstr;
	if( nameLen > 12 ){
		//printf("name fail\n");
		return;
	}
	deleteAll = 0;
	if( pstr[0] == '*' && pstr[1] == '.' && pstr[2] == '*'){
		deleteAll = 1;
	}
	else{
		dosNameParsing(nameLen,pstr,name);
	}
	cmdTokenStrip(&ptr);

	if( deleteAll ){
		sts = vfsFileDeleteAll();//(K_DCF_ALL_DCF_FILE, K_DCF_OUR_TYPE);
		sts = diskInitialize(1);
	}
	else{
	     	sts = vfsFileDelete(name);
		dcfFileInitialize();
		if( sts )
			printf("file:%s,is deleted\n",name);
	}
}

#endif
