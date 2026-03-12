/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      util_fs.c
 * @brief     Brief
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "dos32.h"

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
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
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
UINT8 dramFileSave(UINT8 *name,UINT32 startAddrWord,UINT32 size)
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
	startAddrWord = startAddrWord << 1;
	//printf("fn:%s\n", name);
	fd = DOS_FileOpen(name, 'w');
	if(fd == 0) {
		//printf("open err...\n");
		DOS_FileClose(fd);
		return FALSE;
	}
//XBYTE[0x2038] |= 0x01;
//dbgTimerStart();
//XBYTE[0x2030] |= 0x01;
	cnt = DOS_FileWrite(fd, startAddrWord, size);
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
UINT8 dramFileLoad(UINT8 *name,UINT32 startAddrWord,UINT32 *size)
{
	UINT32 cnt = 0;

	UINT8 fd;
	//BIT es0;
	//UINT32 t0,t1;

	//HAL_GlobalReadGTimer(&t0);
	startAddrWord = startAddrWord << 1;

	fd = DOS_FileOpen(name, 'r');
	//HAL_GlobalReadGTimer(&t1);
//printf("t1=%lu\n",t1-t0);
	if (fd != 0)
	{
		//HAL_GlobalReadGTimer(&t0);
		DOS_FileInfoGet(fd, FILE_SIZE, (UINT8 *)size);
//XBYTE[0x2038] |= 0x01;
//dbgTimerStart();
		cnt = DOS_FileRead(fd, startAddrWord, *size, 0);
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

