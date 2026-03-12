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
#ifndef _DOS_H_
#define _DOS_H_

#include "general.h"
#include "hal.h"
#include "dos_dev_support.h"
#include "hal_global.h"

#define	SEARCH_8N_16N_PLUS_1	1
#define	FAT_DIRECT_BUF	0

#define	FAT_COPIES	2
#define MAX_DEV		4		/* Maximum devices supported */
#define MAX_FILE    8	//0 was reserved for DIR, 1~7 for file op
#define MAX_FAT_BUF    16384L	//use for FAT table direct mapping, could be modified

#define	FILE_DIR_LOOKUP	0	//for dir lookup
#define	FILE_FAST_OPEN	1
#define	FILE_NORMAL		2

#define DOS_RSV_PART    1

enum {
	DOS_SUCCESS=0,
	DOS_ERR_NONE=0,
	DOS_ERR_INVALID_DRV_ID=-100,
	DOS_ERR_INVALID_FAT_BUFFER,
	DOS_ERR_INVALID_FDB_BUFFER,
	DOS_ERR_INVALID_MBS,
	DOS_ERR_INVALID_PBS,
	DOS_ERR_INVALID_SECTOR_SIZE,
	DOS_ERR_INVALID_CLUSTER_SIZE,
	DOS_ERR_INVALID_CLUSTER_NUM,
	DOS_ERR_INVALID_FAT_COPIES_NUM,
	DOS_ERR_INVALID_ROOT_NUM,
	DOS_ERR_INVALID_FAT16,
	DOS_ERR_INVALID_FAT32,
	DOS_ERR_INVALID_FAT,	//If(CountofClusters < 4085)
							//	Volume is FAT12
							//else if(CountofClusters < 65525)
							//	Volume is FAT16
							//else
							//	Volume is FAT32
	DOS_ERR_INVALID_FILE_HANDLE,
	DOS_ERR_INVALID_OPTION,
	DOS_ERR_INVALID_PATH,
	DOS_ERR_INVALID_NAME,
	DOS_FILE_SIZE_MISMATCH,
	DOS_FILE_NOT_FOUND,
	DOS_DIR_NOT_EMPTY,
	DOS_DISK_FULL,
	DOS_UNKNOW_CASE=-2,
	DOS_FAIL=-1,
};

enum {
	FIND_FILE_OK=1,
	FIND_FILE_EMPTY=-1,
	FIND_FILE_FAIL=0
};

enum {	//SEEK, max file size is 4G-1 bytes
	SEEK_SET=0,		//seek from start
	SEEK_CUR=1,		//seek from curr pos,
					//	+SEEK_CUR is means FORWARD,
					//	-SEEK_CUR is means BACKWARD
	SEEK_END=2,		//seek from end of file,
					//	+SEEK_END is means FORWARD,
					//	-SEEK_END is means BACKWARD
};


enum {
	FILE_NAME,
	FILE_ATTR,
	FILE_START,
	FILE_TIME,
	FILE_SIZE,
};

enum {
	DRIVE_SD,
	DRIVE_NAND,
	DRIVE_DRAM,
	DRIVE_NAND_RSV,
	DRIVE_NONE=-1
};

enum {
	FILE_INFO_GET_SIZE=1,
	FILE_INFO_GET_TIME=2,
};

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	VAR_BUF_SIZE	(32*1024L/2)	//32KB*4 for DRAM mapping to SRAM
#define	FAT_BUF_SIZE_SD		(64*1024L/2)	//must be 2^n
#define	FAT_BUF_SIZE_NAND	(64*1024L/2)//(32*1024L/2)	//must be 2^n
#define	FAT_BUF_SIZE_DRAM	(16*1024L/2)//Dram no need FAT buffer
#define	TMP_BUF_SIZE		(64*1024L)
#define	FILE_SECTOR_RW_SIZE	(512L*MAX_FILE/2)	//64KB * 2 for dir/fwrite/fread
#define	DOS_MIN_REQ		FAT_BUF_SIZE_SD + FAT_BUF_SIZE_NAND + FAT_BUF_SIZE_DRAM +\
						(TMP_BUF_SIZE/2) + FILE_SECTOR_RW_SIZE +\
						VAR_BUF_SIZE
#define DEV_STR_LEN  3		/* The length of mounting pointlike "C:\" */
#define	DRV_NAME_LEN 7

#define ATTR_MODIFY_NAME (1 << 0)
#define ATTR_MODIFY_ATTR (1 << 1)
#define ATTR_MODIFY_SIZE (1 << 2)

#define	PATT_SEARCH_FAT16_FREE			(0x01)
#define	PATT_SEARCH_FAT16_FREE_TOTAL	(0x08+PATT_SEARCH_FAT16_FREE)
#define	PATT_SEARCH_FAT32_FREE			(0x03)
#define	PATT_SEARCH_FAT32_FREE_TOTAL	(0x08+PATT_SEARCH_FAT32_FREE)
#define	PATT_SEARCH_FDB					(0x0F)

#define EOF_FAT12           0xFFF8 //0x0FFF//To compatible 533 DOS
#define EOF_FAT16           0xFFF8 //0xFFFF//To compatible 533 DOS
#define EOF_FAT32           0x0FFFFFF8L //0x0FFFFFFFL//To compatible 533 DOS

#define	ATTR_R	0x01	//read only
#define	ATTR_H	0x02	//hidden
#define	ATTR_S	0x04	//system
#define	ATTR_L	0x08	//label
#define	ATTR_D	0x10	//dir
#define	ATTR_A	0x20	//archive
#define	ATTR_LFN	(ATTR_R+ATTR_H+ATTR_S+ATTR_L)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/*
 *  Data types of super block, node, dir, and file objects.
 */

/* partition table */
typedef struct  {
	UINT8  BootId;
	UINT8  StartHead;
	UINT8  StartSector;
	UINT8  StartCylinder;
	UINT8  SysId;
	UINT8  EndHead;
	UINT8  EndSector;
	UINT8  EndCylinder;
	UINT8  StartLogSector[4];
	UINT8  PartitionSize[4];
} x86_PartitionTable;


typedef struct {
	UINT8 BootCmd[0x1BE];
	x86_PartitionTable partition1;
	x86_PartitionTable partition2;
	x86_PartitionTable partition3;
	x86_PartitionTable partition4;
	UINT8 Signature[2];
} x86_MBS;

/* boot table */
typedef struct  {
	UINT8  diskNum;          /* 0x24: disk number                         */
	UINT8  reserved1;        /* 0x25:                                     */
	UINT8  extCode;          /* 0x26: always equal to 0x29                */
	UINT8  volId[4];         /* 0x27: volume ID, not aligned on word      */
	UINT8  volLabel[11];     /* 0x2b: volume label                        */
	UINT8  fsName[8];        /* 0x36: ASCII string FAT12/16, with space   */
	UINT8  reserved2[448-420];   /* 0x3e: Bootstrap program, ...              */
} x86_Fat16Only;

typedef struct  {
	UINT8 fat32Length[4];   /* 0x24: sectors/FAT                         */
	UINT8 flags[2];         /* 0x28: bit8: fat mirror, low4: active fat  */
	UINT8 version[2];     	/* 0x2a: major, minor filesystem version     */
	UINT8 rootClust[4];     /* 0x2c: first cluster in root directory     */
	UINT8 infoSect[2];      /* 0x30: filesystem info sector              */
	UINT8 backupBoot[2];    /* 0x32: backup boot sector                  */
	UINT8 reserved1[12];    /* 0x34: Unused                              */
	UINT8 diskNum;          /* 0x40: disk number                         */
	UINT8 reserved2;        /* 0x41:                                     */
	UINT8 extCode;          /* 0x42: always equal to 0x29                */
	UINT8 volId[4];         /* 0x43: volume ID, not aligned on word      */
	UINT8 volLabel[11];     /* 0x47: volume label                        */
	UINT8 fsName[8];        /* 0x52: ASCII string FAT32 with space       */
} x86_Fat32Only;


typedef struct  {
	UINT8  ignored[3];       /* 0x00: Boot strap short or near jump       */
	UINT8  sysId[8];         /* 0x03: Name: for partition manager volumes */
	UINT8  sectSize[2];      /* 0x0b: bytes per logical sector            */
	UINT8  sectPerClust;     /* 0x0d: sectors/cluster                     */
	UINT8  reserved[2];      /* 0x0e: reserved sectors                    */
	UINT8  fats;             /* 0x10: number of FATs                      */
	UINT8  dirEntries[2];    /* 0x11: root directory entries              */
	UINT8  sects[2];         /* 0x13: number of sectors                   */
	UINT8  media;            /* 0x15: media code (unused)                 */
	UINT8  fatLen[2];        /* 0x16: sectors/FAT                         */
	UINT8  sectsPerTrack[2]; /* 0x18: sectors per track                   */
	UINT8  heads[2];         /* 0x1a: number of heads                     */
	UINT8  hidden[4];        /* 0x1c: hidden sectors                      */
	UINT8  totalSects[4];    /* 0x20: number of sectors (if sectors == 0) */
	union {
		x86_Fat16Only fat16only;
		x86_Fat32Only fat32only;
	} u;
	UINT8  reserved3[420];   /* 0x5a: Bootstrap program, ...              */
	UINT8  Signature[2];     /* signature must reside here(for alignment) */
} x86_PBS;


typedef struct  {	//0xC000, must be EVEN number
	UINT8  FsType;		//type of file system, 32=FAT32, 16=FAT16, 0=invalid */
	UINT8  FsShift;
	UINT8  Name[11];
	UINT32 PhySectors;	//given by caller before mount
	UINT32 LogSectors;	//get from partition table
	UINT32 HiddenSectors;		//
	UINT32 Cluster2Byte;		//cluster size in bytes
	UINT8  bitCluster2Byte;
	UINT8  Cluster2Sector;
	UINT8  bitCluster2Sector;
	UINT8  rsv_236;
	UINT16 Cluster2Fdb;			//Num of FDB per cluster
	UINT16 RootEntries;
	UINT8  FAT_WriteOnly1Copy;	//UINT8  rsv_185[1];
	UINT32 FDB_CurrCluster;		//cluster number of FDB read lookup buffer
	UINT32 CurrDirStartCluster;
	UINT32 RootStartCluster;	//start cluster of root dir
	UINT32 RootStartSector;		//start sector of root dir
	UINT32 DataStartSector;		//first data sector
	UINT32 FAT_StartSector;		//start sector of FAT
	UINT32 FAT_Sectors;			//sectors per FAT
	UINT32 FAT_BufAddr;
	UINT32 FAT_BufSize;
	UINT16 FAT_TotalBank;		//Total banks of one FAT table
	UINT16 FAT_CurrBank;     	//Current bank of FAT table
	UINT32 FAT_Bank2Cluster;	//record of clusters per FAT table bank
	UINT16 FAT_Bank2Sector;		//sizeof(FAT_table) / bank (UINT:sector)
	UINT8  FAT_bitBank2Cluster;	//2^n
	UINT8  FAT_bitBank2Sector;
	UINT32 FAT_BankStartCluster;//first cluster num of curr bank
	UINT32 FAT_FirstFreeCluster;
	UINT16 FAT_LocalDirtyStart;	//FAT dirty sector
	UINT16 FAT_LocalDirtyLen;	//Length of dirty FAT
	UINT8  FAT_ContFreeCluster;
	UINT8  FAT_CopyNum;
	UINT32 TotalCluster;		//0xC070, number of clusters
	UINT32 MaxClusterNum;		//0xC074
	UINT32 FreeCluster;			//0xC078, Num of clusters available
	UINT32 FAT_EOFValue;
	UINT32 FAT_ChainIndex;
	UINT8  FAT_ChainIndexFno;
	UINT8  tmp[128-119];
} DISK;




typedef struct  {	//32 bytes
	UINT8 name[11];	/* name and extension               */
	UINT8 attr;		/* attribute bits                   */
	UINT8 lcase;	/* Reserved for use by Windows NT.
						   Set value to 0 when a file is created
						   and never modify or look at it after that. */
	UINT8 ctimeMs;		/* Creation time, milliseconds      */
	UINT8 ctime[2];		/* Creation time                    */
	UINT8 cdate[2];		/* Creation date                    */
	UINT8 adate[2];		/* Last access date                 */
	UINT8 starthi[2];	/* High 16 bits of cluster in FAT32 */
	UINT8 time[2];
	UINT8 date[2];
	UINT8 start[2];		/* first cluster     */
	UINT8 size[4];		/* file size (in bytes)             */
} x86_FDB;

typedef struct  {	//32 bytes
	UINT8  DevId;
	UINT8  name[11];	//name
	UINT32 FDB_Sector;
	UINT8  FDB_Idx;
	UINT8  attr;		//attribute
  	UINT8  Year;	//+20 is RTC time of 1528
  	UINT8  Month;
  	UINT8  Day;
  	UINT8  Hour;
  	UINT8  Minute;
  	UINT8  Second;
	UINT32 size;		//file size (in bytes)
	UINT32 start;		//start cluster
} FDB;

typedef struct  {
	x86_FDB    fdb;	//fopen()前清為0,若找到FDB(檔名相符),記錄當時的FDB
					//if fdb.name[0]=0xE5 || 0x00,表示舊檔不存在
	UINT8  DevId;	//DRIVE_NONE for invalid file handle
	UINT8  mode;	//"r"=READ, "w"=R/W, "a"=APPEND,
	UINT8  status;	//for dir, 1=data valid, for file,1=valid if read, dirty if write
	UINT8  FDB_Idx;	//*32=addr offset
	UINT32 FDB_Sector;	//logical sector addr for current FDB
						//fopen()前清為0,若找到FDB(空的,或檔名相符),記錄當時的sector
	UINT32 FirstCluster;	/* logical first cluster          */
	UINT32 CurrCluster;		/* next for dir read, curr for file read/write */
	UINT32 CurrSector;		//target sector for data flush
	UINT32 LastCluster;		//last cluster number of file
	UINT32 TargetCluster;	//patch for avi idx table quick search
	UINT32 pos;		//curr r/w pos
	UINT32 size;	//curr file size
	UINT32 buf;
//	UINT8  tmp[80-76];
} FILE;

#define	FAST_DEL_TMP	0x400
typedef struct  {
	UINT8 share[0x1000];	//0x000~0x1FF for MBS, path_buf / name_buf, DIR_AddFDB
							//0x200~0x3FF for PBS
							//0x400~0x5FF for FAT (format), DOS_FileDelX
							//
	DISK disk[MAX_DEV];
	FILE file[MAX_FILE];
	dateStc_t date;	//6 bytes
	UINT8   CurrDrv;
	UINT8   FDB_DataValid;	//for search
	x86_FDB fdb;
	UINT32  TmpBufAddr;	//64KB, for lookup & search
	UINT32  FDB_CurrCluster;
	UINT16  FDB_CurrIdx;
	#if ( FAT_DIRECT_BUF == 1 )
	UINT32   FAT_DirectBufAddr;
	UINT8   FAT_BufIdx;
	UINT8   FAT_BufBank;
	UINT8   fatBuf[MAX_FAT_BUF];	//use for FAT table direct mapping, could be modified
	#endif
} DOS;


typedef struct {
	UINT32 TotalSectors;
	UINT8  nrHead;
	UINT8  sectPerTrack;
} chsTbl_t;

typedef struct  {
	UINT32 TotalSectors;
	UINT8  sectPerClust;
	UINT8  boundaryUnit;
} boundaryUnit_t;

UINT8 DOS_Initialize(void);
//UINT8 DOS_LoadMBS(UINT32* pbsAddr);
//UINT8 DOS_LoadPBS(UINT32 pbsAddr);

UINT8 DOS_DevPreCheck(UINT8 DevId);
UINT8 DOS_SectorRead(UINT8 DevId, UINT32 Start, UINT32 Num, UINT32 TarAddr);
UINT8 DOS_SectorWrite(UINT8 DevId, UINT32 Start, UINT32 Num, UINT32 SrcAddr);

UINT32 DOS_BankNumGet(UINT32 DevId);
UINT8  DOS_Mount(UINT8 DevId, UINT32 TotalSectors);
UINT8  DOS_UnMount(UINT8 DevId);
UINT8  DOS_CurrDriverGet(void);
void   DOS_CurrDriverSet(UINT8 DevId);
void   DOS_CurrDriverSet_Intr(UINT8 DevId);
void   DOS_FilenameUpper(UINT8 *name);
UINT32 DOS_FileCurrClusterGet(UINT8 fno);
UINT8 DOS_FileTagClusterSet(UINT8 fno, UINT32 cluster);

UINT32 FAT_TableRead(UINT8 DevId, UINT32 cluster);
void   FAT_TableWrite(UINT8 DevId, UINT32 curr, UINT32 next);
UINT32  FAT_FreeClusterGet(UINT8 DevId, UINT32 want, UINT32 *next);
void   FAT_TableWriteBack(UINT8 devId);
UINT32 FAT_Cluster2Sector(UINT8 DevId, UINT32 Cluster);
UINT32 FAT_Sector2Cluster(UINT8 DevId, UINT32 Sector);

UINT8  DOS_Path2Drv(UINT8 *path);
UINT8  DIR_Change(UINT8 *path);
UINT8  DIR_Create(UINT8 *path);
UINT8  DIR_Remove(UINT8 *fullpath);
UINT8  DIR_Lookup(UINT8 fno);
UINT8  DIR_AddFDB(UINT8 fno);
void   DIR_SearchReset(void);
UINT8  DIR_Search(UINT8 DevId, UINT8 *TarName, FDB *RetFdb, UINT8 option);
UINT8  DIR_SearchName(UINT8 *TarName);
UINT8  DOS_FileInfoGet(UINT8 fno, UINT8 mode, UINT8 *buf);
void   DOS_FileNameParser(UINT8 *fullpath, UINT8 *path, UINT8 *name);
UINT8  DOS_Format(UINT8 DevId, UINT8 *name, UINT8 opt, UINT8 erase);
void DOS_FormatErrProc(UINT8 DevId);
UINT8 DOS_LableStrSet(const UINT8* pLabel);
UINT8  DOS_BuffSet(UINT32 start, UINT32 size);
UINT8 DOS_DosRamCfg(UINT8 enable);
UINT8 DOS_DosRamStsGet(void);

void   DOS_InfoShow(UINT8 DevId);

UINT8 DOS_FileOpen(UINT8 *fname, UINT8 arg);
UINT8 DOS_FileOpenX(FDB *InputFdb);
void  DOS_FileClose(UINT8 fno);
UINT8 DOS_FileSeek(UINT8 fno, UINT32 offset, UINT8 mode);
UINT8 DOS_FileJoint(UINT8 head, UINT8 tail);
UINT8 DOS_FileDel(UINT8 *fname);
void  DOS_FileDelReset(void);
void  DOS_FileDelX(FDB *InputFdb, UINT8 wrt);	//for READ only
void  DOS_FileDelStop(void);

UINT32 DOS_FileRead(UINT8 fno, UINT32 buf, UINT32 cnt, UINT32 pos);
UINT32 DOS_FileWrite(UINT8 fno, UINT32 buf, UINT32 cnt);
UINT32 DOS_GetDiskSpace(void);
UINT32 DOS_TotalSize(UINT8 DevId);

void cmdMemDump(UINT8 *param);
UINT32 DOS_FreeDiskSpaceByCluster(void);
UINT32 DOS_ClusterSizeGet(void);
UINT8 USR_DcfFdbAddrSet(UINT8 fno);
UINT8 dosRwErr(UINT16 fileId, UINT16 line);
UINT8 USR_CheckMediaFolder(UINT8 *name);
UINT32 DOS_CurrDirGet(UINT8 DevId);
void DOS_CurrDirSet(UINT8 DevId, UINT32 entry);
void FAT12_PattSearch(UINT8 mode, UINT32 addr, UINT32 length, UINT32 patt,
					   UINT32 *first, UINT32 *total, UINT8 *hit);

UINT32 DOS_BlockAlign_Get( UINT8 fno );
void DOS_FileCreateTimeByRtc( UINT8 withRtc );
UINT8 DOS_FileCreateTimeSet( dateStc_t *pdate );
UINT8 DOS_FileCreateTimeGet( dateStc_t *pdate );
void FAT_CopyTable(UINT8 DevId);
void DOS_CacheFlush(void);
//UINT8 vfsFileAttrSet(UINT8 const* oldFileName, UINT8 const* newFileName, UINT8 attr, UINT8 option);
void FAT_BankSwitch(UINT8 DevId, UINT32 bank, UINT32 buf);
//UINT8 sp1kCardFirmwareUpdate(UINT8 *name, UINT32 addr);
//UINT8 sp1kCardFirmwareBoot(UINT8 *name, UINT32 addr);
//void sp1kFwUpdateAddrSet(UINT32 addr);
void DOS_FileReadMakeFatEnable(UINT8 enable, UINT8 fd);

UINT8 DOS_RsvPartSet(UINT32 size);
UINT8 DOS_RsvPartSet_Intr(UINT32 size);
UINT8 DOS_RsvPartStsGet(void);
UINT32 DOS_RsvPartSizeGet(void);
void DOS_RsvPartActSet(UINT8 en);
void DOS_RsvPartActSet_Intr(UINT8 en);
UINT8 DOS_RsvPartActGet(void);
void DOS_PartActOffsetSet(UINT32 offset);
UINT32 DOS_PartActOffsetGet(void);
void DOS_RsvPartWPSet(UINT8 wp);
UINT8 DOS_RsvPartWPGet(void);

//xian ++
UINT8 DOS_ReadSector(UINT8 dstPath, UINT32 logAddr, UINT32 ramAddr);
UINT8 DOS_ReadMoreSector(UINT8 dstPath, UINT32	logAddr, UINT32 ramAddr,UINT32 size);
UINT8 DOS_WriteSector(UINT8 srcPath, UINT32 logAddr, UINT32 ramAddr);
UINT8 DOS_WriteMoreSector(UINT8 srcPath, UINT32 logAddr, UINT32	ramAddr, UINT32 size);
//xian --

UINT8 DOS_FileCurrSectorWrite(UINT8 fno, UINT32 srcAddr);
UINT8 fastFileSeekEnd( UINT8 fno, UINT32 lastCluster);
/**************************************************************************
 *                       E X T E R N    D A T A    T Y P E S               *
 **************************************************************************/


#endif /* _DOS_H_ */


