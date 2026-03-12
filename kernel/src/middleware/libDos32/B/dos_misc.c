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
#include "initio.h"
#include "cardopt.h"

//UINT32 dos_var_start = 0;
//UINT32 dos_file_buf[4];
//UINT8 xdata G_DosRamEn;
static UINT8 xdata sDosRsvPartEn;
static UINT32 xdata sDosRsvPartSector;
static UINT32 xdata sDosPartActOffset;
static UINT8 xdata sDosRsvPartAct;
static UINT8 xdata sDosRsvPartWP;

//extern DOS dos;

#if 0// move to dos_support
/*
	start, word addr
	size, UNIT=word
*/
UINT8 DOS_BuffSet(UINT32 start, UINT32 size)
{
	//UINT16 idx;
	UINT8 i8;

	size = size;

#if 0

#else
	if( dos_var_start != 0 ){//Only initial Once ! 
		return TRUE;
	}

	dos_var_start = K_SDRAM_ShadowAddr;
#endif

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
	for(i8=0; i8<MAX_FILE; i8++) { // One file reserve 512 bytes, now is 8 file
		dos.file[i8].DevId = -1;
		dos.file[i8].buf = dos.TmpBufAddr + TMP_BUF_SIZE/2 + i8*512L/2;
	}
	#if 0
	//printf("start = %lx, size=%lu,%luKB, DOS %lu,%luKB...\n",start, size, size >> 10, DOS_MIN_REQ,(UINT32)DOS_MIN_REQ >> 10);
	printf("dosbuf=%lx,size= %lx,size=%lx,DOS=%lx\n", dos_var_start, (UINT32)sizeof(dos), size >> 10, (UINT32)DOS_MIN_REQ);
	printf("sd = %lx nand= %lx ram= %lx,tmp=%lx\n", dos.disk[DRIVE_SD].FAT_BufAddr, dos.disk[DRIVE_NAND], dos.disk[DRIVE_DRAM].FAT_BufAddr, dos.TmpBufAddr);
	printf("1 sd = %lx nand= %lx,tmp=%lx\n", (dos.disk[DRIVE_NAND].FAT_BufAddr - dos.disk[DRIVE_SD].FAT_BufAddr) * 2, \
		(dos.TmpBufAddr - dos.disk[DRIVE_NAND].FAT_BufAddr) * 2, (dos.file[0].buf - dos.TmpBufAddr) * 2);
	printf("2 sd = %lx nand= %lx,tmp=%lx\n", dos.disk[DRIVE_SD].FAT_BufSize, dos.disk[DRIVE_NAND].FAT_BufSize, TMP_BUF_SIZE);
	for(i8=0; i8<3; i8++)
		printf(" fat[%bu]=%08lX\n", i8, dos.disk[i8].FAT_BufAddr);
	for(i8=0; i8<MAX_FILE; i8++)
		printf("file[%bu]=%08lX\n", i8, dos.file[i8].buf);
	#endif
	return 0;
}

UINT8 DOS_DosRamCfg(UINT8 enable)
{
	if (enable) {
		G_DosRamEn = 1;
	} else {
		G_DosRamEn = 0;
	}

	return 0;
}

UINT8 DOS_DosRamStsGet(void)
{
	if (G_DosRamEn) {
		return TRUE;
	} else {
		return FALSE;
	}
}
#endif

#if (DOS_RSV_PART == 1)

//-----------------------------------------------------------------------------
//DOS_RsvPartSet
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 DOS_RsvPartSet(UINT32 size)
 * @brief     Set the size for nand reserved partition
 * @param     [in] size	unit of BYTE.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
*/
UINT8 DOS_RsvPartSet(UINT32 size) 
{	
    if (size) {
        sDosRsvPartEn = 1;
        sDosRsvPartSector = size >> 9;
    } else {
        sDosRsvPartEn = 0;
        sDosRsvPartSector = 0;
    }     
    
    return SUCCESS;
}

//-----------------------------------------------------------------------------
//DOS_RsvPartStsGet
//-----------------------------------------------------------------------------
/**
 * @fn        UINT8 DOS_RsvPartStsGet(void)
 * @brief     Show whether the option for reserved partition function is enabled
 * @param     NONE
 * @retval    return		1: the reserved partition function is ENABLED\n
 *\n					0: the reserved partition function is DISABLED
 * @see       NULL
*/
UINT8 DOS_RsvPartStsGet(void)
{
    return sDosRsvPartEn;
}
//-----------------------------------------------------------------------------
//DOS_RsvPartSizeGet
//-----------------------------------------------------------------------------
/**
 * @fn        UINT32 DOS_RsvPartSizeGet(void)
 * @brief     Get the size of reserved partition
 * @param     NONE
 * @retval    return		the size of reserved partition (Unit of sectors).
 * @see       NULL
*/
UINT32 DOS_RsvPartSizeGet(void)
{
    return sDosRsvPartSector;
}
//-----------------------------------------------------------------------------
//DOS_PartActOffsetSet
//-----------------------------------------------------------------------------
/**
 * @brief	Set the offset of active partition
 * @param	size: unit of Sectors
 * @retval	SUCCESS
 * @see
 * @author	Phil Lin
 * @since   2009-11-11
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
**/

void DOS_PartActOffsetSet(UINT32 offset)
{
    sDosPartActOffset = offset;
}

//-----------------------------------------------------------------------------
//DOS_PartActOffsetGet
//-----------------------------------------------------------------------------
/**
 * @brief	Get the offset of active partition
 * @param	NULL
 * @retval	the offset of reserved partition (unit of Sectors)
 * @see
 * @author	Phil Lin
 * @since   2009-11-11
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
**/

UINT32 DOS_PartActOffsetGet(void)
{
    return sDosPartActOffset;
}

/*
en: 1: point to the reserved nand partition
    0: point to the common nand partition
*/
//-----------------------------------------------------------------------------
//DOS_RsvPartActSet
//-----------------------------------------------------------------------------
/**
 * @fn        DOS DOS_RsvPartActSet(UINT8 en)
 * @brief     Active the reserved partition
 * @param     [in] en	1: Actived the reserved partition, thus, the all the DOS operation will be base on reserved partition\n
 *\n					0: Inactived the reserved partition
 * @retval    NONE
 * @see       NULL
*/
void DOS_RsvPartActSet(UINT8 en)
{
    if (en) {
        sDosRsvPartAct = 1;
    } else {
        sDosRsvPartAct = 0;
    }
}
//-----------------------------------------------------------------------------
//DOS_RsvPartActGet
//-----------------------------------------------------------------------------
/**
 * @brief	Show whether the Reserved partition is actived
 * @param	NULL
 * @retval	1: the Reserved partition is actived
            0: the Reserved partition is inactived
 * @see
 * @author	Phil Lin
 * @since   2009-11-11
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
**/
UINT8 DOS_RsvPartActGet(void)
{
    return sDosRsvPartAct;
}
//-----------------------------------------------------------------------------
//DOS_RsvPartWPSet
//-----------------------------------------------------------------------------
/**
 * @fn        void DOS_RsvPartWPSet(UINT8 wp)
 * @brief     Set the write protect attribute for reserved partition
 * @param     [in] wp	1: write protect is ENABLED for reserved partition\n
 *\n					0: write protect is DISABLED for reserved partition
 * @retval    NONE
 * @see       NULL
*/
void DOS_RsvPartWPSet(UINT8 wp)
{
    if (wp) {
        sDosRsvPartWP = 1;
    } else {
        sDosRsvPartWP = 0;
    }
}

//-----------------------------------------------------------------------------
//DOS_RsvPartWPGet
//-----------------------------------------------------------------------------
/**
 * @brief	Get the write protect attribute for reserved partition
 * @param	NULL
 * @retval	1: write protect is ENABLED for reserved partition
            0: write protect is DISABLED for reserved partition
 * @see
 * @author	Phil Lin
 * @since   2009-11-11
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
**/
UINT8 DOS_RsvPartWPGet(void)
{
    return sDosRsvPartWP;
}

void DOS_RsvPartActSet_Intr(UINT8 en)
{
    if (en) {
        sDosRsvPartAct = 1;
    } else {
        sDosRsvPartAct = 0;
    }
}

#endif

