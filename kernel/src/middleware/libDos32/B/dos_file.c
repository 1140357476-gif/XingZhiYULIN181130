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
#include "hal.h"

//extern UINT32 dos_var_start;
extern DOS dos;

void DOS_FilenameUpper(UINT8 *name)
{
	UINT8 i8;
	//printf("%s %u p=%s\n", __FILE__, (unsigned short)__LINE__,name);
	for(i8=0; i8<255; i8++) {
		if( name[i8] == '\0') {/* add this because we do not know the name[] size */
			break;
		}
		if((name[i8] >= 'a') && (name[i8] <= 'z'))
			name[i8] = name[i8] - 'a' + 'A';
	}
	return;
}

void DOS_FileNameParser(UINT8 *fullpath, UINT8 *path, UINT8 *name)
{
	UINT8 i, pos;

	i   = 0;
	pos = 0;
	while(fullpath[i]) {
		if(fullpath[i]=='\\' || fullpath[i]==':')
		{
			pos = i;
		}
		i++;
	}
	if(pos) {
		for(i=0; i<=pos; i++)
			path[i] = fullpath[i];
		pos++;
	}
	path[pos] = 0;
	//printf("%s %u p=%s\n", __FILE__, (unsigned short)__LINE__,path);

	i = 0;
	while(1) {	//translate to 8.3 format
		if(fullpath[pos]==0) {
			while(i < 11) {
				name[i++] = ' ';
			}
			break;
		}
		else if(fullpath[pos]=='.') {
			while(i < 8) {
				name[i++] = ' ';
			}
		}
		else {
			name[i++] = fullpath[pos];
		}
		pos++;
	}
	name[i] = 0;
	//printf(" path=<%s> name=<%s>\n", path, name);
}

#if 0//move to dos_debug.c
UINT8 DOS_FileInfoGet(UINT8 fno, UINT8 mode, UINT8 *buf)
{
	UINT8 i8, j8, ret;
	pDateStc_t time;
	xdata x86_FDB *fdb;
	xdata FILE *file;
	file  = (FILE *)&dos.file[fno];
	if(file->DevId==DRIVE_NONE)
		return DOS_ERR_INVALID_FILE_HANDLE;
	fdb = (x86_FDB *)&file->fdb;

	i8 = 0;
	j8 = 0;
	ret= DOS_SUCCESS;

	if(mode==FILE_NAME) {
		while(fdb->name[j8] != ' ') {
			buf[i8++] = fdb->name[j8++];
			if(j8==8)
				buf[i8++] = '.';
			if(j8==11) break;
		}
		buf[i8] = 0;
	}
	else if(mode==FILE_ATTR) {
		buf[i8] = fdb->attr;
	}
	else if(mode==FILE_SIZE) {
		while(i8 < 4) {
			buf[i8] = fdb->size[3-i8];
			i8++;
		}
	}
	else if(mode==FILE_START) {
		buf[i8++] = fdb->starthi[1];
		buf[i8++] = fdb->starthi[0];
		buf[i8++] = fdb->start[1];
		buf[i8++] = fdb->start[0];
	}
	else if(mode==FILE_TIME) {
		time = (pDateStc_t)buf;
		time->Year   = (fdb->date[1] >> 1) - (2000-1980);
		time->Month  =((fdb->date[1] & 0x01)<< 3) + (fdb->date[0] >> 5);
		time->Day    =  fdb->date[0] & 0x1F;
		time->Hour   =  fdb->time[1] >> 3;
		time->Minute = (fdb->time[0] >> 5) + ((fdb->time[1] & 0x07) << 3);
		time->Second = (fdb->time[0] & 0x1F) << 1;
	}
	else {
		ret = DOS_ERR_INVALID_OPTION;
	}
	return ret;
}
#endif

#if 0
UINT8 DOS_FileInfoSet(UINT8 fno, UINT8 mode, UINT8 *buf)
{
	UINT8  i8, j8, ret;
	//UINT16 i16;
	//pDateStc_t time;
	xdata x86_FDB *fdb;
	xdata FILE *file;
	file  = (FILE *)&dos.file[fno];
	if(file->DevId==DRIVE_NONE)
		return DOS_ERR_INVALID_FILE_HANDLE;
	fdb = (x86_FDB *)&file->fdb;

	i8 = 0;
	j8 = 0;
	ret= DOS_SUCCESS;

	if(mode==FILE_NAME) {
		while(j8 < 11) {
			if(buf[i8] == 0) {
				while(j8 < 11) {
					fdb->name[j8++] = ' ';
				}
				break;
			}
			else if(buf[i8] == '.')	{
				while(j8 < 8) {
					fdb->name[j8++] = ' ';
				}
			}
			else {
				fdb->name[j8++] = buf[i8];
			}
			i8++;
		}
	}
	else if(mode==FILE_ATTR) {
		fdb->attr = buf[i8];
	}
	else if(mode==FILE_SIZE) {
#if	0
		while(i8 < 4) {
			fdb->size[3-i8] = buf[i8];
			i8++;
		}
#endif
	}
	else if(mode==FILE_TIME) {
#if	0
		/* date & time */
		time = (pDateStc_t)buf;
		time->Year += (2000-1980);	//RTC -> M$ time << 26;	//bit[31:26]
		i16 = (time->Hour << 11) + (time->Minute << 5) + (time->Second >> 1);
		fdb->time[0]  = READ8(i16,1);
		fdb->time[1]  = READ8(i16,0);
		//fdb->ctime[0] = READ8(i16,1);
		//fdb->ctime[1] = READ8(i16,0);
		i16 = (time->Year << 9) + (time->Month << 5) + (time->Day);
		fdb->date[0]  = READ8(i16,1);
		fdb->date[1]  = READ8(i16,0);
		fdb->adate[0] = READ8(i16,1);
		fdb->adate[1] = READ8(i16,0);
		//fdb->cdate[0] = READ8(i16,1);
		//fdb->cdate[1] = READ8(i16,0);
#endif
	}
	else {
		ret = DOS_ERR_INVALID_OPTION;
	}
	file->status = 1;	//dirty
	return ret;
}
#endif

UINT8 DOS_FileCurrInfoGet(UINT8 fno, UINT8 mode, UINT8 *buf)
{
	xdata FILE *file;
	UINT8 ret= DOS_SUCCESS;

	if (fno >= MAX_FILE) {
		return -1;
	}

	file = (FILE *)&dos.file[fno];

	if(mode == 0){
		*(UINT32*)buf = file->CurrCluster;
	}
	else{
		ret = DOS_ERR_INVALID_OPTION;
	}

	return ret;
}