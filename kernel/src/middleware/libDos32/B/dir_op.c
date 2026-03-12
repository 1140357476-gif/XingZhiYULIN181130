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

#define	DEBUG	0
extern DOS dos;

//TEST OK!
UINT8 DOS_Path2Drv(UINT8 *path)
{
	UINT8 DevId;
	DevId = DOS_CurrDriverGet();
	if(path[1]==':') {
		if(path[0]=='C' || path[0]=='D' || path[0]=='E') {
			//DevId = path[0] - 'C';
			if( path[0]=='C' ) DevId = DRIVE_NAND;
			else if( path[0]=='D' ) DevId = DRIVE_SD;
			else if( path[0]=='E' ) DevId = DRIVE_DRAM;
#if (DOS_RSV_PART == 1)
            else if( path[0]=='F' ) DevId = DRIVE_NAND_RSV;
#endif
			//DOS_CurrDriverSet(DevId);
		}
	}
	return DevId;
}

//TEST OK!
void DOS_CurrDirSet(UINT8 DevId, UINT32 entry)
{
	if(entry < 2)
		dos.disk[DevId].CurrDirStartCluster = dos.disk[DevId].RootStartCluster;
	else
		dos.disk[DevId].CurrDirStartCluster = entry;
	//printf("set dir %lu(0x%lX)\n",	dos.disk[DevId].CurrDirStartCluster,dos.disk[DevId].CurrDirStartCluster);
}

//TEST OK!
UINT32 DOS_CurrDirGet(UINT8 DevId)
{
	return dos.disk[DevId].CurrDirStartCluster;
}




/**
 * @brief	search file from curr directory
 * @param	path, ASCIIZ str
 * @retval	1 is success, -1 is find EMPTY Entry. 0=NG
 * @see
 * @author	Ted Chiu
 * @since	08/04/20 13:30
 * @todo
 * @bug
*/
//test OK!
UINT8 DIR_Change(UINT8 *path)
{
	UINT8  i8, DevId, *pName;
	UINT8  ret;
	UINT32 cluster;
	xdata FILE *file;
	xdata x86_FDB *fdb;
	file = (FILE *)&dos.file[FILE_DIR_LOOKUP];
	fdb  = (x86_FDB *)&file->fdb;
	file->mode   = 'r';
	file->status = 0;

#if	DEBUG
printf("DIR_Change %u path=<%s>\n", (unsigned short)__LINE__, path);
#endif

	DOS_FilenameUpper(path);
#if	DEBUG
printf("DIR_Change %u path=<%s>\n", (unsigned short)__LINE__, path);
#endif
	if(path[1]==':') {
		if(path[0]=='C' || path[0]=='D' || path[0]=='E') {
			//DevId = path[0] - 'C';
			if( path[0]=='C' ) DevId = DRIVE_NAND;
			else if( path[0]=='D' ) DevId = DRIVE_SD;
			else if( path[0]=='E' ) DevId = DRIVE_DRAM;
#if (DOS_RSV_PART == 1)
			else if( path[0]=='F' ) DevId = DRIVE_NAND_RSV;
#endif           
			path += 2;
			DOS_CurrDriverSet(DevId);
		}
		else {
			return DOS_ERR_INVALID_PATH;
		}
	}
	else {
		DevId = DOS_CurrDriverGet();
	}
#if	DEBUG
printf("DevId %bu\n", DevId);
#endif
	file->DevId = DevId;
	pName = fdb->name;	//buf size, min = 32 bytes
	pName[11] = 0;
	i8 = 0;
	if(*path == '.') {	//cd "." or ".."
		pName[i8++] = '.';
		path++;
	}
	if(*path == '.') {	//cd ".."
		pName[i8++] = '.';
		path++;
	}
	while(1) {
		#if	DEBUG
		printf("path=%s\n", path);
		#endif
		if((*path == '\\') || (*path == NULL)) {
			if(i8==0) {
				if(*path == '\\')  DOS_CurrDirSet(DevId,0);	//set to ROOT
			}
			else {
				while(i8 < 11) pName[i8++] = ' ';
				i8 = 0;
				#if	DEBUG
				printf("pName=<%s> path=<%s>\n", pName, path);
				#endif
				ret = DIR_Lookup(FILE_DIR_LOOKUP);
				if(ret==FIND_FILE_FAIL) {	//not found, but the filename is ready for MD
					if(*path==NULL || *(path+1)==NULL)
						file->status = 1;	//last path of input str
					#if	DEBUG
					printf("lookup <%s>, NG!\n",pName);
					#endif
					return DOS_FILE_NOT_FOUND;
				}
				if(fdb->attr & ATTR_D) {
					#if	DEBUG
					printf("<DIR>");
					#endif
				}
				else {
					return DOS_ERR_INVALID_PATH;
				}
				WRITE8(cluster,0,fdb->starthi[1]);
				WRITE8(cluster,1,fdb->starthi[0]);
				WRITE8(cluster,2,fdb->start[1]);
				WRITE8(cluster,3,fdb->start[0]);
				DOS_CurrDirSet(DevId,cluster);
			}
			if(*path == NULL)
				break;
		}
		else {
			if(*path != '.') {
				pName[i8++] = *path;
			}
			else {
				while(i8 < 8) pName[i8++] = ' ';
			}
		}
		path++;
	}
	return DOS_SUCCESS;
}



/**
 * @brief	search file from curr directory
 * @param	path, ASCIIZ str
 * @retval	1 is success, -1 is find EMPTY Entry. 0=NG
 * @see
 * @author	Ted Chiu
 * @since	08/04/20 13:30
 * @todo
 * @bug
*/
//test OK!
UINT8 DIR_Create(UINT8 *path)
{
	UINT8  ret,DevId;
	UINT32 old_entry;
	xdata FILE *file;
	xdata x86_FDB *fdb;
	xdata DISK *disk;
	file  = (FILE *)&dos.file[0];
	fdb   = (x86_FDB *)&file->fdb;
	DevId = DOS_Path2Drv(path);
	disk  = (DISK *)&dos.disk[DevId];

	#if	DEBUG
	printf("DIR_Create %u %s\n", (unsigned short)__LINE__,path);
	#endif

	if(disk->FreeCluster < 2)	{
		return DOS_DISK_FULL;	//can't found any space for new entry
	}

	//pold = path;
	old_entry = DOS_CurrDirGet(DevId);
	ret = DIR_Change(path);	//make sure the path is not exist...
	if(ret==DOS_SUCCESS) {	//path already exist
		#if	DEBUG
		printf("path <%s> already exist...\n", path);
		#endif
	}
	else if(ret==DOS_FILE_NOT_FOUND && file->status==1) {
		file->mode = 'w';
		ret = DIR_Lookup(0);
		if(ret==FIND_FILE_EMPTY) {
			#if	DEBUG
			printf("create <%s> OK!\n", path);
			#endif
			ret = DOS_SUCCESS;
		}
		else {
			#if	DEBUG
			printf("%s %u DOS_UNKNOW_CASE\n", __FILE__, (unsigned short)__LINE__);
			#endif
			ret = DOS_UNKNOW_CASE;	//unknown case
		}
	}
	else {
		ret = DOS_ERR_INVALID_PATH;
	}
	DOS_CurrDirSet(DevId, old_entry);
	return ret;
}
#if 1
//#undef	DEBUG
//test OK!
UINT8 DIR_Remove(UINT8 *fullpath)
{
	UINT8 DevId;
	UINT8 ret, cnt;
	UINT8 *path, *name;
	UINT32 next, curr, curr_dir;
	xdata FILE *file;
	xdata x86_FDB *fdb;
	xdata FDB *RetFdb;
	xdata DISK *disk;

	DevId = DOS_Path2Drv(fullpath);
	file = (FILE *)&dos.file[FILE_DIR_LOOKUP];
	file->mode = 'r';
	file->DevId = DevId;
	fdb  = (x86_FDB *)&file->fdb;
	disk = (DISK *)&dos.disk[DevId];

	name = fdb->name;
	path = dos.share;
	DOS_FileNameParser(fullpath, path, name);

	curr_dir = DOS_CurrDirGet(DevId);
	ret = DIR_Change(fullpath);
	if(ret != DOS_SUCCESS) {
		DOS_CurrDirSet(DevId, curr_dir);
		return DOS_ERR_INVALID_PATH;
	}

	DIR_SearchReset();
	RetFdb = (FDB *)dos.share;
	cnt = 0;
	while(1) {
		ret = DIR_Search(DevId, "*", RetFdb, 0);
		if(ret==FIND_FILE_FAIL)	//can't find more file
			break;
		if(ret==FIND_FILE_OK) {
			#if	DEBUG
			UINT8 i8;
			printf("<");
			for(i8=0; i8<8; i8++)
				printf("%c", RetFdb->name[i8]);
			if(RetFdb->name[i8] != ' ');
				printf(".");
			for(; i8<11; i8++)
				printf("%c", RetFdb->name[i8]);
			printf(">\n");
			#endif
			cnt++;
			if(cnt > 2){	//find "." & ".." & more & more
				DOS_CurrDirSet(DevId, curr_dir);
				return DOS_DIR_NOT_EMPTY;
			}
		}
	}

	//cur dir is empty now
	fdb->name[0] = 0xE5;	//ERASE

	//free FAT train
	WRITE8(curr, 0, fdb->starthi[1]);
	WRITE8(curr, 1, fdb->starthi[0]);
	WRITE8(curr, 2, fdb->start[1]);
	WRITE8(curr, 3, fdb->start[0]);
	file->FirstCluster = curr;
	//file->size         = 0;
	while(1) {
		if((curr > disk->MaxClusterNum-1) || (curr < 2)){
			next =  EOF_FAT32;
		}
		else {
			next = FAT_TableRead(DevId, curr);
		}

		FAT_TableWrite(DevId, curr, 0);
		if(curr < disk->FAT_FirstFreeCluster){ //xian ++ 20101115
			disk->FAT_FirstFreeCluster = curr;
		}
		disk->FreeCluster++;
		#if	DEBUG
		printf("curr=%lu next=", curr);
		#endif
		if((next & disk->FAT_EOFValue) == disk->FAT_EOFValue) {
			#if	DEBUG
			printf("EOF");
			#endif
			break;
		}
		#if	DEBUG
		printf("%lu\n",next);
		#endif
		curr = next;
	}
	DIR_AddFDB(FILE_DIR_LOOKUP);
	FAT_TableWriteBack(DevId);
	DOS_CurrDirSet(DevId, curr_dir);
	return DOS_SUCCESS;
}

//test OK!
UINT8 DIR_SearchName(UINT8 *path)
{
	UINT8  DevId, *pName;
	UINT8  ret;
	UINT32 cluster;
	xdata FILE *file;
	xdata x86_FDB *fdb;
	file = (FILE *)&dos.file[FILE_DIR_LOOKUP];
	fdb  = (x86_FDB *)&file->fdb;
	file->mode   = 'r';
	file->status = 0;
	file->DevId = DOS_CurrDriverGet();
	pName = fdb->name;	//buf size, min = 32 bytes
	pName[11] = 0;
	#if	0
	printf("lookup <%s>, NG!\n",pName);
	#endif
	ret = DIR_Lookup(FILE_DIR_LOOKUP);
	if(ret==FIND_FILE_FAIL) {	//not found, but the filename is ready for MD
		if(*path==NULL || *(path+1)==NULL)
			file->status = 1;	//last path of input str
		#if	0
		printf("lookup <%s>, NG!\n",pName);
		#endif
		return DOS_FILE_NOT_FOUND;
	}
	if(fdb->attr & ATTR_D) {
		#if	0
		printf("<DIR>");
		#endif
	}
	else {
		return DOS_ERR_INVALID_PATH;
	}
	WRITE8(cluster,0,fdb->starthi[1]);
	WRITE8(cluster,1,fdb->starthi[0]);
	WRITE8(cluster,2,fdb->start[1]);
	WRITE8(cluster,3,fdb->start[0]);
	DOS_CurrDirSet(DevId,cluster);

	path = path;
	return DOS_SUCCESS;
}
#endif
