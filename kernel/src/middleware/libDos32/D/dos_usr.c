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

#include "general.h"
#include "asicreg.h"
#include "impt.h"
#include "cardui.h"
//#include "exptcode.h"

#include "dosusr.h"
#include "doslink.h"
#include "doserr.h"
#include "cardlink.h"

//patch4.2@richie@ds0509
#include "dramfunc.h"

//patch3.2@ada@0401 For DPOF
//#include "doscon.h"

//patch3.2@ada@0401 for Multi-Frame Play Back
#include "initio.h"
#include "dosvar.h"


#include "audio.h"

//version4.0@ada@0513 for Play Back
//#include "exifhdr.h"

//patch4.5@ada@Complete H/W write protect function begin
//#include "cardusr.h"
#include "uiflow.h"
#include "lcdtv.h"
#include "dos32.h"
#include "hal_dram.h"
#include "hal_gprm.h"
#include "vfs_api.h"
#include "fileindex.h"

#include "dbg_def.h"
#include "dbg_wdt.h"
#include "dbg_mem.h"

#include "disp_api.h"
#include "pb_api.h"
#include "video_slclip.h"

#include "app_msg.h"
#include "os_msg.h"

#include "sp1k_snap_api.h"
#include "timelapse.h"
#include "dbg_def.h"
#include "sd_bg.h"
//patch4.5@ada@Complete H/W write protect function end

#define __FILE	__FILE_ID_DOS_USR__

#define FOLDER_FILE_OFFSET 0x40 
xdata   UINT16  G_USR_MaxNormalDirCnt;
xdata   UINT16  G_USR_MaxTlsVidDirCnt;

xdata   UINT16    G_USR_Dir0Cluster;
xdata   UINT16    G_USR_Dir1Cluster;
xdata   UINT16    G_USR_Dir1Count;

//version4.0@ada@0513 for Play Back
xdata   UINT16  G_USR_SearchPreFileIndex;
xdata   UINT16  G_USR_SearchNowFileIndex;
xdata   UINT16  G_USR_SearchNextFileIndex;
xdata   UINT16  G_USR_FirstFileIndex;
xdata   UINT16  G_USR_LastFileIndex;


xdata   UINT16    G_USR_FileIndex;
xdata   UINT8    G_USR_FileType;
xdata   UINT16    G_USR_FileCount;
xdata   UINT16    G_USR_PlayIndex;
xdata   UINT32   G_USR_AllFolderFileTotal;   //all folder file count

xdata UINT8 dcfQuickInit;
//patch5.1@ada@Fix MP3 bug begin
extern code UINT8 G_USR_Song_UpperSIG[];

#if (SINGLE_DIRECTORY == 0)
//version4.0@ada@0513 for Multi-Directory

//patch4.4@ada@DirectoryIndexOrder begin
//xdata   UINT16  G_USR_Dir1CountIndex;
xdata   UINT16  G_USR_MaxDir1Count;
//xdata   UINT16  G_USR_MaxOurDir1CountIndex;
xdata   UINT16  G_USR_TotalDirectory;
xdata   UINT8   G_USR_OurDirectory;
xdata   UINT16  G_USR_MinDir1Count;
xdata   UINT16  G_USR_MaxOurDir1Count;
xdata   UINT16  G_USR_PrevDir1Count;
//patch4.4@ada@DirectoryIndexOrder end

//patch4.5@ada@Duplicated file index in same DCF directory begin
xdata   UINT16  G_USR_TempSearchNowFileIndex;
xdata   UINT16  G_USR_TempLastFileIndex;
xdata   UINT16  G_USR_PracticalMaxDirIndexCount;
//patch4.5@ada@Duplicated file index in same DCF directory end
#endif

xdata   UINT16  G_USR_TempSearchNowFileIndex;
xdata   UINT16  G_USR_TempLastFileIndex;

UINT32 xdata G_USR_NewFileSizeInFdb;
//New dos
extern DOS dos;

//xdata UINT32 G_DOS_FdbAddress;
//xdata UINT32 dosFdbCluster;

//xian ++ 20080806
#define MT_DEBUG    0

#if 0//def MULTI_FOLDER_AS_ONE
#define OUR_DIR_MARK 0x8000
xdata   UINT32  G_USR_MediaFileTotal;   //all folder
xdata   UINT32  G_USR_CurFolderFirstFileIdx;
#endif
//xian --
extern xdata	dateStc_t	G_DATEINFO;
xdata UINT8 G_DCF_FileType_Mask = 0;    //xian ++ 20080826, enable all by default
#define K_USR_JPG_TYPE_MASK     (1<<0)
#define K_USR_AVI_TYPE_MASK     (1<<1)
#define K_USR_WAV_TYPE_MASK     (1<<2)
#define K_USR_WGI_TYPE_MASK     (1<<3)
//UINT8 DOS_SetFdb(UINT8 *fdb, UINT32 fdbLogAddr);

UINT8 G_USR_bUsrAssignFolder = 0;
UINT16 G_USR_UsrAssignFolderCount = 0;

void dcfDir1NameGet(UINT8* name, UINT16 dirIndex,UINT8 opt);
UINT8 USR_FolderCategory(UINT8 *name,UINT8 opt);

void dcfInitDir1NameGet(UINT8* name)
{
	UINT8 i;
	for ( i = 0; i < 12; i++ )
	{
		name[i]  = G_USR_Dir1Name[i];
	}
}

//-----------------------------------------------------------------------------
//dcfBaseNameGet
//-----------------------------------------------------------------------------
void dcfBaseNameGet(UINT8 *name, UINT16 fileIndex) USING_0
{
        UINT8 i;
        UINT8 ch;

	for ( i = 0; i < 4; i++ )
        	name[i]  = G_USR_Company_UpperSIG[i];
        for (i = 7; i > 3; i--)
        {
                ch = (UINT8) (fileIndex % 10) + '0';
                fileIndex /= 10;
                name[i] = ch;
        }
        name[11] = '\0';
}

void dcfMediaNameGet(UINT8 *name,UINT16 fileIndex,UINT8 type)
{
	dcfBaseNameGet(name,fileIndex);
	switch( type ){
		case MEDIA_JPG:
			name[8]  = 'J';
        	name[9]  = 'P';
	        name[10] = 'G';
	        //name[11] = 0x20;
		break;
		case MEDIA_TLS:
			name[8] = G_CustomerAVIName[0];
	   		name[9] = G_CustomerAVIName[1];
			name[10] = G_CustomerAVIName[2];
			name[11] = G_CustomerAVIName[3];
		break;
		case MEDIA_AVI:
			name[8] = 'A';
	   		name[9] = 'V';
			name[10] = 'I';
		break;
		case MEDIA_WAV:
			name[8]  = 'W';
	        name[9]  = 'A';
	        name[10] = 'V';
	        //name[11] = 0x20;
		break;
	}
}
//-----------------------------------------------------------------------------
//dcfJpgNameGet
//-----------------------------------------------------------------------------
#if 0
void dcfJpgNameGet(UINT8* name, UINT16 fileIndex) USING_0
{
        dcfBaseNameGet(name,fileIndex);
        name[8]  = 'J';
        name[9]  = 'P';
        name[10] = 'G';
        name[11] = 0x20;
}
#endif

//#if ( AUDIO_OPTION == 1 )
//-----------------------------------------------------------------------------
//dcfWaveNameGet
//-----------------------------------------------------------------------------
/*void dcfWaveNameGet(UINT8* name, UINT16 fileIndex) USING_0
{
        dcfBaseNameGet(name,fileIndex);
        name[8]  = 'W';
        name[9]  = 'A';
        name[10] = 'V';
        name[11] = 0x20;
}*/
//#endif

#if ( K_File_HaveVideo == 1 )

void dcfAviNameSet(UINT8* name)
{
//	name = NULL;
	UINT8 i;
	for ( i = 0; i < 3; i++ ){
		G_CustomerAVIName[i] = name[i];
	}
}

//-----------------------------------------------------------------------------
//dcfAviNameGet
//-----------------------------------------------------------------------------
#if 0
void dcfAviNameGet(UINT8* name, UINT16 fileIndex) USING_0
{
        dcfBaseNameGet(name,fileIndex);
       	name[8] = G_CustomerAVIName[8];
	   	name[9] = G_CustomerAVIName[9];
		name[10] = G_CustomerAVIName[10];
		name[11] = G_CustomerAVIName[11];
}
#endif
#endif
//patch4.4@ada@MP3 begin
//patch4.4@ada@MP3 end

//-----------------------------------------------------------------------------
//dcfDir0NameGet
//-----------------------------------------------------------------------------
void dcfDir0NameGet(UINT8* name) USING_0
{
        UINT8 i;

	for ( i = 0; i < 12; i++ )
        	name[i]  = G_USR_Dir0Name[i];
}
#if 0  // NOT USE
//-----------------------------------------------------------------------------
//dcfDir1NameGet
//-----------------------------------------------------------------------------
void dcfDir1NameGet(UINT8* name, UINT16 dirIndex) USING_0
{
	UINT8 i;
	UINT8 ch;
	UINT16 rem = dirIndex;

	for ( i = 0; i < 12; i++ )
		name[i]  = G_USR_Dir1Name[i];

	for (i = 2; i != 0xff; i--)
	{
	    ch = (UINT8) (rem % 10) + '0';
	    rem /= 10;
	    name[i] = ch;
	}

	//version4.0@ada@0513 for DCF
	//G_USR_Dir1Count++;
}
#endif
//patch4.5@ada@Fix filtering DCF file name bug begin
//patch4.4@ada@DCF begin
//-----------------------------------------------------------------------------
//USR_CheckDcfFileName
//-----------------------------------------------------------------------------
UINT8 USR_CheckDcfFileName(UINT8 *fdb)
{
	UINT8 sts;
	UINT8 i;

     for (i = 0; i < 4; i++)
     {
          if ((('A' <= fdb[i]) && (fdb[i] <= 'Z')) || (fdb[i] == '_') || (('0' <= fdb[i]) && (fdb[i] <= '9')))
          {
               sts = TRUE;
          }
		else
		{
			sts = FALSE;
			return sts;
		}
     }

     for (i = 4; i < 8; i++)
     {
          if (('0' <= fdb[i]) && (fdb[i] <= '9'))
          {
               sts = TRUE;
          }
		else
		{
			sts = FALSE;
			return sts;
     	}
	}

	return sts;
}
//patch4.4@ada@DCF end
//patch4.5@ada@Fix filtering DCF file name bug end

//-----------------------------------------------------------------------------
//USR_CheckType
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For DCF
//patch3.2@ada@0401 for Multi-Frame Play Back
//version4.0@ada@0513 for DCF
UINT8 USR_CheckType(UINT8 *fdb)
{
	UINT8 sts = TRUE;
	UINT8 i;
	UINT8 fileType = 0;
	UINT16 index = 0;

	//patch4.3@ada@0618 for playback file index order
	UINT16 NowIndex,PreIndex,NextIndex,TempNowIndex;
	UINT8 tmp1,tmp2;
	
	//patch4.5@ada@Duplicated file index in same DCF directory begin
	UINT16 TempPreFileIndex;
	UINT16 TempNextFileIndex;
	//patch4.5@ada@Duplicated file index in same DCF directory end
	UINT8 voiceMemo = 0;

	//ada@0401 For DCF
	fileType = USR_FileExist(fdb);
	if ( fileType )
	{
		//patch4.4@ada@DCF begin
		if (USR_CheckDcfFileName(fdb) == FALSE)
		{
			return FALSE;
		}
		//patch4.4@ada@DCF end
		
		//patch4.5@ada@Duplicated file index in same DCF directory begin
		//G_USR_SearchPreFileIndex = G_USR_SearchNowFileIndex;
		//patch4.5@ada@Duplicated file index in same DCF directory end
		
		for (i = 4; i < 8; i++)
		{
			index = index * 10 + (fdb[i] - 0x30);
		}	
		//patch4.5.1@ada@Fix DCF file name filtering bug begin
		if (index == 0x0000)
		{
			return FALSE;
		}
		//patch4.5.1@ada@Fix DCF file name filtering bug end
		
		//patch4.5@ada@Duplicated file index in same DCF directory begin
		//Check if this file index is already doubled
		HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)index) - 1) * 10 + 2),K_SDRAM_Prefetch);
		HAL_DramWordDataRead(&tmp1,&tmp2);
		
		if ((tmp1 == 0xff) && (tmp2 == 0xff))
		{
			return FALSE;
		}

		G_USR_SearchPreFileIndex = G_USR_SearchNowFileIndex;
		G_USR_TempSearchNowFileIndex = G_USR_SearchNowFileIndex;
		G_USR_TempLastFileIndex = G_USR_LastFileIndex;

		G_USR_SearchNowFileIndex = index;
		G_USR_LastFileIndex = index;

		if (G_USR_FileCount == 0)
		{
			G_USR_FirstFileIndex = G_USR_SearchNowFileIndex;
		}
		//patch4.5@ada@Duplicated file index in same DCF directory begin
		else
		{
			HAL_DramWordDataRead(&tmp1,&tmp2);
			HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10),K_SDRAM_Prefetch);
			HAL_DramWordDataRead(&tmp1,&tmp2);

			//Check if the file with same index already exist(FDB address > 0)
			if ((tmp1 > 0) || (tmp2 > 0))
			{
				HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10+2),K_SDRAM_Prefetch);
				HAL_DramWordDataRead(&tmp1,&tmp2);
				//Modify for same name between .jpg and .wav@wei sun 080314 begin
				do {
					if(tmp2==K_DCF_JPG_TYPE && fileType == K_DCF_WAV_TYPE)
					{
						if(/*flag != TRUE*/1) {
							//set Memo flag
							return FALSE;
						}
					}
					else if(tmp2==K_DCF_WAV_TYPE && fileType == K_DCF_JPG_TYPE)
					{
						if(/*flag != TRUE*/1) {
                                            //modify fdb table
                                            voiceMemo = 1;
                                            HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)index) - 1) * 10+2),K_SDRAM_Prefetch);
                                            HAL_DramWordDataRead(&tmp1,&tmp2);
                                            HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)index) - 1) * 10+2),K_SDRAM_NoPrefetch);
                                            HAL_DramWordDataWrite(tmp1,K_DCF_JPG_TYPE);
                                            return FALSE;
						}
	
					}

					//Modify for same name between .jpg and .wav@080314 end
					HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10 + 8),K_SDRAM_Prefetch);
					HAL_DramWordDataRead(&tmp1,&tmp2);
					TempPreFileIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
					HAL_DramWordDataRead(&tmp1,&tmp2);
					TempNextFileIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);

					if (TempPreFileIndex > 0)
					{
						//Link to the next file index
						HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)TempPreFileIndex) - 1) * 10 + 9),K_SDRAM_NoPrefetch);
						HAL_DramWordDataWrite((UINT8)(TempNextFileIndex & 0x00ff),(UINT8)((TempNextFileIndex & 0xff00) >> 8));
					}

					if (TempNextFileIndex > 0)
					{
						//Link to the previous file index
				     	     	HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)TempNextFileIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
						HAL_DramWordDataWrite((UINT8)(TempPreFileIndex & 0x00ff),(UINT8)((TempPreFileIndex & 0xff00) >> 8));
					}

					if (G_USR_SearchNowFileIndex == G_USR_TempLastFileIndex)
					{
						G_USR_LastFileIndex = TempPreFileIndex;
					}
					else
					{
						G_USR_LastFileIndex = G_USR_TempLastFileIndex;
					}
	
					//Check if the first file index must be changed
					if (G_USR_SearchNowFileIndex == G_USR_FirstFileIndex)
					{
						//Search forward for G_USR_FirstFileIndex
						HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_FirstFileIndex) - 1) * 10 + 9),K_SDRAM_Prefetch);
						HAL_DramWordDataRead(&tmp1,&tmp2);
						
						G_USR_FirstFileIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
					}

			          	HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10 + 2),K_SDRAM_Prefetch);
					HAL_DramWordDataRead(&tmp1,&tmp2);

	
			          	HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10),K_SDRAM_NoPrefetch);
	                // @patch5.2.1 @mattwang@ fix about hanging when SUNP0001.JPG, SUNP0002.JPG, SUNP0001.AVI, SUNP0002.AVI coexist beg
					//for (i = 0; i < 12; i ++)
					for (i = 0; i < 10; i ++)
	                // @patch5.2.1 @mattwang@ fix about hanging when SUNP0001.JPG, SUNP0002.JPG, SUNP0001.AVI, SUNP0002.AVI coexist end
					{
						HAL_DramWordDataWrite(0x00,0x00);
					}
	
					//Make a marker to note that this file index is doubled
					HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10 + 2),K_SDRAM_NoPrefetch);
					HAL_DramWordDataWrite(0xff,0xff);
	
					//Avoid the new file index duplicates
					if (G_USR_FileIndex < G_USR_SearchNowFileIndex)
					{
						G_USR_FileIndex = G_USR_SearchNowFileIndex;
					}
	
					//Same index file follows
					if (G_USR_SearchNowFileIndex == G_USR_TempSearchNowFileIndex)
					{
						//Find backward
						HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_TempSearchNowFileIndex) - 1) * 10 + 8),K_SDRAM_Prefetch);
						HAL_DramWordDataRead(&tmp1,&tmp2);
						G_USR_SearchNowFileIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
					}
					else
					{
						G_USR_SearchNowFileIndex = G_USR_TempSearchNowFileIndex;
					}
					
					G_USR_FileCount--;
					
					if (G_USR_FileCount == 0)
					{
						G_USR_SearchNowFileIndex = 0;
					}
					else
					{
						G_USR_SearchNowFileIndex = G_USR_LastFileIndex;
					}
	
					return FALSE;
				}while(0);
			}
		}
		//patch4.5@ada@Duplicated file index in same DCF directory end

          // WWWW0410, Fix zero indexed file
          if ((G_USR_FileIndex < G_USR_SearchNowFileIndex) || (G_USR_SearchNowFileIndex == 0 && G_USR_FileIndex == 0))
          {
               G_USR_FileType = fileType;
               G_USR_FileIndex = G_USR_SearchNowFileIndex;
          }

          //ada@0401 for Multi-Frame Play Back
          if (fileType == K_DCF_JPG_TYPE)
          {

               //version4.0@ada@0513 for DPOF

               //patch4.4@ada@DPOF begin
               //HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 3),K_SDRAM_NoPrefetch);
               //HAL_DramWordDataWrite(0xff,0x00);
               //patch4.4@ada@DPOF end

               //For play back JPEG file speeding-up
               //version4.0@ada@0513 for DCF
               //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 3),K_SDRAM_NoPrefetch); //ada@0401

			//patch4.5@ada@Record DCF file name begin
               HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10),K_SDRAM_NoPrefetch);
               //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 8),K_SDRAM_NoPrefetch);
			//patch4.5@ada@Record DCF file name end

               //Record file FDB logical address
               HAL_DramWordDataWrite((UINT8)(G_DOS_FdbAddress & 0x000000ff),(UINT8)((G_DOS_FdbAddress & 0x0000ff00) >> 8));
               HAL_DramWordDataWrite((UINT8)((G_DOS_FdbAddress & 0x00ff0000) >> 16),(UINT8)((G_DOS_FdbAddress & 0xff000000) >> 24));

               //Record file attribute and file type
               HAL_DramWordDataWrite(fdb[11],K_DCF_JPG_TYPE);

               //Record file starting cluster
               HAL_DramWordDataWrite(fdb[26],fdb[27]);

               //Record file size in bytes
               HAL_DramWordDataWrite(fdb[28],fdb[29]);
               HAL_DramWordDataWrite(fdb[30],fdb[31]);

		//patch4.5@ada@Record DCF file name begin
		//Record DCF file name
		for (i = 0; i < 2; i++)
		{
			HAL_DramWordDataWrite(fdb[i * 2],fdb[i * 2 + 1]);
		}
			//patch4.5@ada@Record DCF file name end
		//phil add for voice memo s
		if (voiceMemo) {
			return FALSE;
		}
		//phil add for voice memo e
               //patch4.3@ada@0618 for playback file index order
               if (G_USR_FileCount)
               {
                    if (G_USR_SearchNowFileIndex > G_USR_SearchPreFileIndex)
                    {
                         //Record previous file index(No matter JPG or AVI or WAV)
                         HAL_DramWordDataWrite((UINT8)(G_USR_SearchPreFileIndex & 0x00ff),(UINT8)((G_USR_SearchPreFileIndex & 0xff00) >> 8));
                         HAL_DramWordDataWrite(0x00,0x00);

					//patch4.5@ada@Record DCF file name begin
                         HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchPreFileIndex) - 1) * 10 + 9),K_SDRAM_NoPrefetch);
                         //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)G_USR_SearchPreFileIndex) - 1) * 8 + 7),K_SDRAM_NoPrefetch);
					//patch4.5@ada@Record DCF file name end

                         HAL_DramWordDataWrite((UINT8)(G_USR_SearchNowFileIndex & 0x00ff),(UINT8)((G_USR_SearchNowFileIndex & 0xff00) >> 8));
                    }
                    else
                    {
                         NowIndex = G_USR_SearchNowFileIndex;
                         PreIndex = G_USR_SearchPreFileIndex;
                         //G_USR_SearchNowFileIndex = PreIndex;mark for decreasing monut time 
                         G_USR_LastFileIndex = PreIndex;

                         //patch4.4@ada@DPOF skip AVI & WAV begin

	   				//patch4.4.1@ada@Fix DPOF bug begin
                         //G_USR_LastJpgFileIndex = PreIndex;
		   			//patch4.4.1@ada@Fix DPOF bug end

                         //patch4.4@ada@DPOF skip AVI & WAV end

                         do
                         {
                              TempNowIndex = PreIndex;

						//patch4.5@ada@Record DCF file name begin
                              HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 10 + 8),K_SDRAM_Prefetch);
                              //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 8 + 6),K_SDRAM_Prefetch);
						//patch4.5@ada@Record DCF file name end

                              HAL_DramWordDataRead(&tmp1,&tmp2);
                              PreIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
                              HAL_DramWordDataRead(&tmp1,&tmp2);
                              NextIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
                         } while (NowIndex < PreIndex);

                         if (PreIndex > 0)
                         {
						//patch4.5@ada@Record DCF file name begin
                              HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 10 + 9),K_SDRAM_NoPrefetch);
                              //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 8 + 7),K_SDRAM_NoPrefetch);
						//patch4.5@ada@Record DCF file name end

                              HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));

						//patch4.5@ada@Record DCF file name begin
                              HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                              //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
						//patch4.5@ada@Record DCF file name end

                              HAL_DramWordDataWrite((UINT8)(PreIndex & 0x00ff),(UINT8)((PreIndex & 0xff00) >> 8));
                              HAL_DramWordDataWrite((UINT8)(TempNowIndex & 0x00ff),(UINT8)((TempNowIndex & 0xff00) >> 8));

						//patch4.5@ada@Record DCF file name begin
                              HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                              //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
						//patch4.5@ada@Record DCF file name end

                              HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));
                         }
                         else
                         {
						//patch4.5@ada@Record DCF file name begin
                              HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                              //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
						//patch4.5@ada@Record DCF file name end

                              HAL_DramWordDataWrite(0x00,0x00);
                              HAL_DramWordDataWrite((UINT8)(TempNowIndex & 0x00ff),(UINT8)((TempNowIndex & 0xff00) >> 8));

						//patch4.5@ada@Record DCF file name begin
                              HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                              //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
						//patch4.5@ada@Record DCF file name end

                              HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));

                              G_USR_FirstFileIndex = NowIndex;
                         }
                    }
               }
          }
          else
          {
               if (fileType == K_DCF_AVI_TYPE || fileType == K_DCF_WGI_TYPE )
               {
                    //For play back AVI file speeding-up
                    //version4.0@ada@0513 for DCF
                    //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 3),K_SDRAM_NoPrefetch); //ada@0401

  				//patch4.5@ada@Record DCF file name begin
                    HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10),K_SDRAM_NoPrefetch);
                    //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 8),K_SDRAM_NoPrefetch);
  				//patch4.5@ada@Record DCF file name end

                    //Record file FDB logical address
                    HAL_DramWordDataWrite((UINT8)(G_DOS_FdbAddress & 0x000000ff),(UINT8)((G_DOS_FdbAddress & 0x0000ff00) >> 8));
                    HAL_DramWordDataWrite((UINT8)((G_DOS_FdbAddress & 0x00ff0000) >> 16),(UINT8)((G_DOS_FdbAddress & 0xff000000) >> 24));
                    //Record file attribute and file type
                    HAL_DramWordDataWrite(fdb[11],fileType);
                    //Record file starting cluster
                    HAL_DramWordDataWrite(fdb[26],fdb[27]);
                    //Record file size in bytes
                    HAL_DramWordDataWrite(fdb[28],fdb[29]);
                    HAL_DramWordDataWrite(fdb[30],fdb[31]);

				//patch4.5@ada@Record DCF file name begin
				//Record DCF file name
				for (i = 0; i < 2; i++)
				{
               		HAL_DramWordDataWrite(fdb[i * 2],fdb[i * 2 + 1]);
				}
				//patch4.5@ada@Record DCF file name end

                    //Record previous file index(No matter JPG or AVI or WAV)

				//patch4.4.1@ada@Fix DCF bug begin
                    //HAL_DramWordDataWrite((UINT8)(G_USR_SearchPreFileIndex & 0x00ff),(UINT8)((G_USR_SearchPreFileIndex & 0xff00) >> 8));
                    //HAL_DramWordDataWrite(0x00,0x00);
				//patch4.4.1@ada@Fix DCF bug end

                    //patch4.3@ada@0618 for playback file index order
                    if (G_USR_FileCount)
                    {
                         if (G_USR_SearchNowFileIndex > G_USR_SearchPreFileIndex)
                         {
                              //Record previous file index(No matter JPG or AVI or WAV)
                              HAL_DramWordDataWrite((UINT8)(G_USR_SearchPreFileIndex & 0x00ff),(UINT8)((G_USR_SearchPreFileIndex & 0xff00) >> 8));
                              HAL_DramWordDataWrite(0x00,0x00);

		  				//patch4.5@ada@Record DCF file name begin
                              HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)G_USR_SearchPreFileIndex) - 1) * 10 + 9),K_SDRAM_NoPrefetch);
                              //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)G_USR_SearchPreFileIndex) - 1) * 8 + 7),K_SDRAM_NoPrefetch);
		  				//patch4.5@ada@Record DCF file name end

                              HAL_DramWordDataWrite((UINT8)(G_USR_SearchNowFileIndex & 0x00ff),(UINT8)((G_USR_SearchNowFileIndex & 0xff00) >> 8));
                         }
                         else
                         {
                              NowIndex = G_USR_SearchNowFileIndex;
                              PreIndex = G_USR_SearchPreFileIndex;
                              //G_USR_SearchNowFileIndex = PreIndex;mark for decreasing monut time
                              G_USR_LastFileIndex = PreIndex;

                              do
                              {
                                   TempNowIndex = PreIndex;

			  				//patch4.5@ada@Record DCF file name begin
                                   HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 10 + 8),K_SDRAM_Prefetch);
                                   //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 8 + 6),K_SDRAM_Prefetch);
			  				//patch4.5@ada@Record DCF file name end

                                   HAL_DramWordDataRead(&tmp1,&tmp2);
                                   PreIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
                                   HAL_DramWordDataRead(&tmp1,&tmp2);
                                   NextIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
                              } while (NowIndex < PreIndex);

                              if (PreIndex > 0)
                              {
			  				//patch4.5@ada@Record DCF file name begin
                                   HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 10 + 9),K_SDRAM_NoPrefetch);
                                   //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 8 + 7),K_SDRAM_NoPrefetch);
			  				//patch4.5@ada@Record DCF file name end

                                   HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));

			  				//patch4.5@ada@Record DCF file name begin
                                   HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                                   //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
			  				//patch4.5@ada@Record DCF file name end

                                   HAL_DramWordDataWrite((UINT8)(PreIndex & 0x00ff),(UINT8)((PreIndex & 0xff00) >> 8));
                                   HAL_DramWordDataWrite((UINT8)(TempNowIndex & 0x00ff),(UINT8)((TempNowIndex & 0xff00) >> 8));

			  				//patch4.5@ada@Record DCF file name begin
                                   HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                                   //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
			  				//patch4.5@ada@Record DCF file name end

                                   HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));
                              }
                              else
                              {
			  				//patch4.5@ada@Record DCF file name begin
                                   HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                                   //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
			  				//patch4.5@ada@Record DCF file name end

                                   HAL_DramWordDataWrite(0x00,0x00);
                                   HAL_DramWordDataWrite((UINT8)(TempNowIndex & 0x00ff),(UINT8)((TempNowIndex & 0xff00) >> 8));

			  				//patch4.5@ada@Record DCF file name begin
                                   HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                                   //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
			  				//patch4.5@ada@Record DCF file name end

                                   HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));

                                   G_USR_FirstFileIndex = NowIndex;
                              }
                         }
                    }
               }
               else
               {
                    if (fileType == K_DCF_WAV_TYPE)
                    {
                         //For play back AVI file speeding-up
                         //version4.0@ada@0513 for DCF
                         //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 3),K_SDRAM_NoPrefetch); //ada@0401

					//patch4.5@ada@Record DCF file name begin
                         HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10),K_SDRAM_NoPrefetch); //ada@0401
                         //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 8),K_SDRAM_NoPrefetch); //ada@0401
					//patch4.5@ada@Record DCF file name end

                         //Record file FDB logical address
                         HAL_DramWordDataWrite((UINT8)(G_DOS_FdbAddress & 0x000000ff),(UINT8)((G_DOS_FdbAddress & 0x0000ff00) >> 8));
                         HAL_DramWordDataWrite((UINT8)((G_DOS_FdbAddress & 0x00ff0000) >> 16),(UINT8)((G_DOS_FdbAddress & 0xff000000) >> 24));

                         //Record file attribute and file type
                         HAL_DramWordDataWrite(fdb[11],K_DCF_WAV_TYPE);

                         //Record file starting cluster
                         HAL_DramWordDataWrite(fdb[26],fdb[27]);

                         //Record file size in bytes
                         HAL_DramWordDataWrite(fdb[28],fdb[29]);
                         HAL_DramWordDataWrite(fdb[30],fdb[31]);

					//patch4.5@ada@Record DCF file name begin
					//Record DCF file name
					for (i = 0; i < 2; i++)
					{
	               		HAL_DramWordDataWrite(fdb[i * 2],fdb[i * 2 + 1]);
					}
					//patch4.5@ada@Record DCF file name end

				   	//patch4.4.1@ada@Fix DCF bug begin
                         //Record previous file index(No matter JPG or AVI or WAV)
                         //HAL_DramWordDataWrite((UINT8)(G_USR_SearchPreFileIndex & 0x00ff),(UINT8)((G_USR_SearchPreFileIndex & 0xff00) >> 8));
                         //HAL_DramWordDataWrite(0x00,0x00);
				   	//patch4.4.1@ada@Fix DCF bug end

                         //patch4.3@ada@0618 for playback file index order
                         if (G_USR_FileCount)
                         {
                              if (G_USR_SearchNowFileIndex > G_USR_SearchPreFileIndex)
                              {
                                   //Record previous file index(No matter JPG or AVI or WAV)
                                   HAL_DramWordDataWrite((UINT8)(G_USR_SearchPreFileIndex & 0x00ff),(UINT8)((G_USR_SearchPreFileIndex & 0xff00) >> 8));
                                   HAL_DramWordDataWrite(0x00,0x00);

							//patch4.5@ada@Record DCF file name begin
                                   HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)G_USR_SearchPreFileIndex) - 1) * 10 + 9),K_SDRAM_NoPrefetch);
                                   //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)G_USR_SearchPreFileIndex) - 1) * 8 + 7),K_SDRAM_NoPrefetch);
							//patch4.5@ada@Record DCF file name end

                                   HAL_DramWordDataWrite((UINT8)(G_USR_SearchNowFileIndex & 0x00ff),(UINT8)((G_USR_SearchNowFileIndex & 0xff00) >> 8));
                              }
                              else
                              {
                                   NowIndex = G_USR_SearchNowFileIndex;
                                   PreIndex = G_USR_SearchPreFileIndex;
                                   G_USR_SearchNowFileIndex = PreIndex;
                                   G_USR_LastFileIndex = PreIndex;

                                   do
                                   {
                                        TempNowIndex = PreIndex;

								//patch4.5@ada@Record DCF file name begin
                                        HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 10 + 8),K_SDRAM_Prefetch);
                                        //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 8 + 6),K_SDRAM_Prefetch);
								//patch4.5@ada@Record DCF file name end

                                        HAL_DramWordDataRead(&tmp1,&tmp2);
                                        PreIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
                                        HAL_DramWordDataRead(&tmp1,&tmp2);
                                        NextIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
                                   } while (NowIndex < PreIndex);

                                   if (PreIndex > 0)
                                   {
								//patch4.5@ada@Record DCF file name begin
                                        HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 10 + 9),K_SDRAM_NoPrefetch);
                                        //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)PreIndex) - 1) * 8 + 7),K_SDRAM_NoPrefetch);
								//patch4.5@ada@Record DCF file name end

                                        HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));

								//patch4.5@ada@Record DCF file name begin
                                        HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                                        //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
								//patch4.5@ada@Record DCF file name end

                                        HAL_DramWordDataWrite((UINT8)(PreIndex & 0x00ff),(UINT8)((PreIndex & 0xff00) >> 8));
                                        HAL_DramWordDataWrite((UINT8)(TempNowIndex & 0x00ff),(UINT8)((TempNowIndex & 0xff00) >> 8));

								//patch4.5@ada@Record DCF file name begin
                                        HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                                        //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
								//patch4.5@ada@Record DCF file name end

                                        HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));
                                   }
                                   else
                                   {
								//patch4.5@ada@Record DCF file name begin
                                        HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                                        //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)NowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
								//patch4.5@ada@Record DCF file name end

                                        HAL_DramWordDataWrite(0x00,0x00);
                                        HAL_DramWordDataWrite((UINT8)(TempNowIndex & 0x00ff),(UINT8)((TempNowIndex & 0xff00) >> 8));

								//patch4.5@ada@Record DCF file name begin
                                        HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 10 + 8),K_SDRAM_NoPrefetch);
                                        //HAL_DramStartAddrSet((K_SDRAM_WAV_FdbParameterBufAddr + (((UINT32)TempNowIndex) - 1) * 8 + 6),K_SDRAM_NoPrefetch);
								//patch4.5@ada@Record DCF file name end

                                        HAL_DramWordDataWrite((UINT8)(NowIndex & 0x00ff),(UINT8)((NowIndex & 0xff00) >> 8));

                                        G_USR_FirstFileIndex = NowIndex;
                                   }
                              }
                         }
                    }
               }
          }

	G_USR_FileCount++;
	return TRUE;	
     }
}

UINT8 USR_IsOurBaseName(UINT8* name) USING_0
{
        if (((name[0]  == G_USR_Company_UpperSIG[0]) || (name[0]  == G_USR_Company_LowerSIG[0])) &&
            ((name[1]  == G_USR_Company_UpperSIG[1]) || (name[1]  == G_USR_Company_LowerSIG[1])) &&
            ((name[2]  == G_USR_Company_UpperSIG[2]) || (name[2]  == G_USR_Company_LowerSIG[2])) &&
            ((name[3]  == G_USR_Company_UpperSIG[3]) || (name[3]  == G_USR_Company_LowerSIG[3])) )
                return TRUE;
        else
                return FALSE;
}

#if 0
UINT8 USR_IsJpgExt(UINT8* name) USING_0
{
        if (    ((name[8]  == 'J') || (name[8]  == 'j')) &&
                ((name[9]  == 'P') || (name[9]  == 'p')) &&
                ((name[10] == 'G') || (name[10] == 'g')) )
                return TRUE;
        else
                return FALSE;
}
#endif

UINT8 USR_IsSameStr(UINT8*str1, UINT8*str2, UINT16 len) USING_0
{
        UINT16    k;

        for ( k = 0; k < len; k++ )
        {
                if ( str1[k] != str2[k] )
                        return FALSE;
        }

        return TRUE;
}

//#if ( AUDIO_OPTION == 1 )
#if 0
UINT8 USR_IsWaveExt(UINT8* name) USING_0
{
        if (    ((name[8]  == 'W') || (name[8]  == 'w')) &&
                ((name[9]  == 'A') || (name[9]  == 'a')) &&
                ((name[10] == 'V') || (name[10] == 'v')) )
                return TRUE;
        else
                return FALSE;
}
#endif

//#endif

#if ( K_File_HaveVideo == 1 )

#if 0
UINT8 USR_IsAviExt(UINT8* name) USING_0
{
        if (    ((name[8]  == 'A') || (name[8]  == 'a')) &&
                ((name[9]  == 'V') || (name[9]  == 'v')) &&
                ((name[10] == 'I') || (name[10] == 'i')) )
                return TRUE;
        else
                return FALSE;
}
#endif

#if 0
UINT8 USR_IsWGIExt(UINT8* name) USING_0
{
        if (    ((name[8]  == 'W') || (name[8]  == 'w')) &&
                ((name[9]  == 'G') || (name[9]  == 'g')) &&
                ((name[10] == 'I') || (name[10] == 'i')) )
                return TRUE;
        else
                return FALSE;
}

#endif

#endif

UINT8* dcfStrToUpper(UINT8 *str)
{	
	UINT8 *p = str;
	
	while(*str != '\0'){
		*str = *str -32;
		str++;
	}
	return p;
}

UINT8 USR_IsOurFileExt(UINT8 *name,MediaType_e opt)
{
	UINT8 fileExt[3]={'\0','\0','\0'};
	
	switch( opt ){
		case MEDIA_AVI:
			fileExt[0] = 'a';
			fileExt[1] = 'v';
			fileExt[2] = 'i';
		break;
		case MEDIA_JPG:
			fileExt[0] = 'j';
			fileExt[1] = 'p';
			fileExt[2] = 'g';
		break;
		case MEDIA_WAV:
			fileExt[0] = 'w';
			fileExt[1] = 'a';
			fileExt[2] = 'v';
		break;
		case MEDIA_TLS:
			fileExt[0] = 'w';
			fileExt[1] = 'g';
			fileExt[2] = 'i';
		break;
	}

	if((!strncmp(name+8,fileExt,3)) || (!strncmp(name+8,dcfStrToUpper(fileExt),3))){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

//patch4.4@ada@MP3 begin
//patch4.4@ada@MP3 end

//-----------------------------------------------------------------------------
//USR_IsOurFile
//-----------------------------------------------------------------------------
UINT8 USR_IsOurFile(UINT8 *name) USING_0
{
        UINT8    match = FALSE;

        if ( USR_IsOurBaseName(name) )
        {
               match = USR_CheckExtType(name);
        }

        return match;
}

//-----------------------------------------------------------------------------
//USR_FileExist
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For DCF
UINT8 USR_FileExist(UINT8 *fdb)
{
     UINT8 match = FALSE;

     //version4.0@ada@0513 for DCF                           //Long File Name
     if ((fdb[0] != 0xe5) && (fdb[0] != 0x2e) && (fdb[11] != 0x0f))
     {
          match = USR_CheckExtType(fdb);
     }

     return match;
}
#if 0
UINT8 USR_SearchMediaFolder(void)
{
	UINT8 i, ch, DevId;
	UINT16 	rem;
	UINT8 dirName[12] = "999MEDIA";
	UINT32 sts;

	G_USR_MaxDir1Count = 999;
	//G_USR_Dir1Count = 0;
	sts = TRUE;
	while((sts != DOS_SUCCESS) && ( G_USR_MaxDir1Count > 100 )){
		for (i = 2; i != 0xff ; i--)
		{
			ch = (UINT8) (rem % 10) + '0';
			rem /= 10;
			dirName[i] = ch;
		}                
		dirName[8]  = '\0';
		sts = DIR_SearchName(dirName);
		if (sts == DOS_SUCCESS){
			//G_USR_TotalDirectory++;
			printf("Found DIR=%s\n",dirName);
			printf("G_USR_MaxDir1Count=%u\n",G_USR_MaxDir1Count);
			return DOS_SUCCESS;
		}
		else{
			//printf("DIR=%s\n",dirName);
			G_USR_MaxDir1Count --;
			//G_USR_Dir1Count ;
			rem = G_USR_Dir1Count;
		}
	}
	return sts;
}
#endif
UINT8 USR_CheckMediaFolder(UINT8 *name)
{
	UINT8 i, ch;
	UINT16 	rem;

	if(G_USR_MaxDir1Count >= 999){
		return FALSE;
	}
	
	G_USR_MaxDir1Count ++;
	rem = G_USR_MaxDir1Count;
	//printf("rem=%u\n",rem);
	for (i = 2; i != 0xff ; i--)
	{
		ch = (UINT8) (rem % 10) + '0';
		rem /= 10;
		//printf("ch=%bx\n",ch);
		name[i] = ch;
	}                
	name[8]  = '\0';
	if( DIR_Change("..") != DOS_SUCCESS ) return FALSE; 
	return TRUE;
}

UINT16 dcfGetCurrFolderMaxFileIndex(UINT8 DevId)
{
	UINT32 cluster;
	UINT8 SectorNum, SectorNum2;
	UINT32 SectorStart;
	x86_FDB	*pfdb;
	xdata DISK *disk;
	UINT8  find;
	UINT8 num1,num3;
	UINT8 string[5] = "0000";
	UINT32 dramAddr;

	disk      = (DISK *)&dos.disk[DevId];
	cluster   = dos.FDB_DataValid ? dos.FDB_CurrCluster : disk->CurrDirStartCluster;
	SectorNum = (cluster < 2) ? (disk->RootEntries>>4) : disk->Cluster2Sector;
	SectorNum2 = (SectorNum>>1);

	SectorStart = FAT_Cluster2Sector(DevId, cluster);
	find = FIND_FILE_OK;

	HAL_DmaSetSramAddr(0x0000);
	do
	{
		DOS_SectorRead(DevId, SectorStart, SectorNum, dos.TmpBufAddr);
		dramAddr = dos.TmpBufAddr;
		for(num3=0;num3 < SectorNum2;num3++)
		{
			HAL_DmaSetDramAddr(dramAddr);
			HAL_DmaDo(0,1,1024,0,0);
			pfdb=(x86_FDB*)G_ucStorData;

			for(num1=0;num1<32;num1++)//read two sector one time
			{
				if(pfdb->name[0] == 0)
				{
					find = FIND_FILE_FAIL;
					break;
				}
				else if(pfdb->name[0] != 0xe5)
				{
					if (!(strncmp(&pfdb->name[4], string, 4)&0x80)) 
					{
						memcpy(string, &pfdb->name[4], 4);
					}				
				}
				pfdb++;
			}
			dramAddr+= 512;//tow sector add
		}
	
		cluster = FAT_TableRead(DevId, cluster);//get next cluster
		if((cluster & disk->FAT_EOFValue) == disk->FAT_EOFValue)// FDB End of this Directory
		{
			find = FIND_FILE_FAIL;
			break;
		}
		dos.FDB_CurrCluster = cluster;
		SectorStart = FAT_Cluster2Sector(DevId, cluster);
	}while(find == FIND_FILE_OK);

	G_USR_FileIndex= strtoul(string, NULL, 10);

	if(G_USR_FileIndex > 9999)
	{
		G_USR_FileIndex = 0;
	}
	return G_USR_FileIndex;
}

//-----------------------------------------------------------------------------
//dcfFileInitialize
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 for Multi-Frame Play Back
//patch3.2@ada@0401 for DCF
UINT8 dcfFileInitialize(void)
{
	UINT8 sts = TRUE;
	UINT8 DevId, ret;
	UINT32 i;
	FDB fdb;
	x86_FDB	*pfdb;
	pfdb  = (x86_FDB *)&dos.fdb;
	USR_ResetDramFdbParameterArea();

	M_DOS_ClearError();

    //initialization
    G_USR_FileType = 0x00;      //0x00: NON_SPECIFIC
    G_USR_FileIndex = 0;
    G_USR_FileCount = 0;


    G_USR_SearchPreFileIndex = 0;
    G_USR_SearchNowFileIndex = 0;
    G_USR_SearchNextFileIndex = 0;

	G_USR_FirstFileIndex = 0;

	DevId = DOS_CurrDriverGet();
	DIR_SearchReset();
	i = 0;
	//printf("K_SDRAM_JPG_FdbParameterBufAddr=%lx\n",K_SDRAM_JPG_FdbParameterBufAddr);
	G_DOS_StartClusterNumber = 0;
	if(dcfQuickInitStsGet())
	{
		dcfGetCurrFolderMaxFileIndex(DevId);
	}
	else
	{
		while(1) 
		{

			ret = DIR_Search(DevId, "*", (FDB *)&fdb, FILE_INFO_GET_SIZE); //fdb.size valid

			if(ret==FIND_FILE_FAIL)	//can't find more file
				break;
			if(ret==FIND_FILE_OK) {
				if(fdb.start && USR_FileExist((UINT8 *)pfdb)){// xian fix #29328,#29334
					G_DOS_StartClusterNumber = fdb.FDB_Sector;
					G_DOS_FdbAddress = ((UINT32)fdb.FDB_Sector << 4) + (fdb.FDB_Idx);////mantis-26979 
					USR_CheckType((UINT8 *)pfdb);  //xian: alg. of inserting link table maybe slow
					//dcfFileDramFdbParameterWrite();
					//printf("%s FDB_Sector=%lx, G_DOS_FdbAddress=%lx attr=%bu\n",pfdb->name,fdb.FDB_Sector,G_DOS_FdbAddress,fdb.attr);
					//printf("%s\n",pfdb->name);
					//printf("1 G_USR_FileIndex=%u\n",G_USR_FileIndex);
					//printf("2 G_USR_FileCount=%u\n",G_USR_FileCount);
					//printf("G_USR_FileType=%x\n",G_USR_FileType);
					i ++;
				}
				else if(fdb.attr==ATTR_A &&(strncmp(fdb.name, "AAAAAAAAAAA", 11) == 0 ||
					strncmp(fdb.name, "BBBBBBBBBBB", 11) == 0 ||(fdb.size==0))){
					DOS_FileDel(fdb.name);  //fix #33721
				}              
				#if 0
				else{
					printf("%s\n",fdb.name);
				}
				#endif
			}
			dbgWdtFeed(10000);	//for #40931
		}

	}
	G_USR_FileIndex++;  //next file index
	printf("G_USR_FileIndex=%u, %lu\n",G_USR_FileIndex, i);
	return sts;
}

UINT8 USR_DcfFdbAddrSet(UINT8 fno)
{
	xdata FILE * file;
	file  = (FILE *)&dos.file[fno];
	//Add for Host quick DCF
	G_DOS_StartClusterNumber = (UINT16)file->FirstCluster;
	G_DOS_FdbAddress = (file->FDB_Sector << 4) + (file->FDB_Idx);//mantis-26979
	G_DOS_FileAttribute = file->fdb.attr; //fixed mantis-26368 0n 20080620
	//printf("G_DOS_FileAttribute=%bx,G_USR_FileIndex=%x,file->FDB_Idx=%bx\n",G_DOS_FileAttribute,G_USR_FileIndex,file->FDB_Idx);
	//printf("G_DOS_FdbAddress=%lx\n",G_DOS_FdbAddress);
	return TRUE;
}

//-----------------------------------------------------------------------------
//USR_MatchFdb
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 for DPOF
//version4.0@ada@0513 for DCF
UINT8 USR_MatchFdb(UINT8 matchCond, UINT8 *fdb, UINT32 refCon) USING_0
{
        UINT8 match = FALSE;

        //ada@0418
        //UINT8 i;
        UINT16 index = 0;

        switch ( matchCond )
        {
                case K_DCF_MatchIndexName:
       			// match sunpxxxx.avi, sunpxxxx.jpg
                	match = USR_MatchIndexName(fdb,refCon);

                	if (match)
                	{
                		// WWWW0410, Fix delete last bug
             	          G_USR_FileType = match = USR_CheckExtType(fdb);
                    }

                        break;

                case K_DCF_MatchOurFile:
                	// WWWW0410, Fix delete last bug
                        G_USR_FileType = match = USR_IsOurFile(fdb);
                        if (refCon)
                        {
                        	if ( match != refCon )
                        		match = FALSE;
                        }
                        break;

                //version4.0@ada@0513 for DCF
                case K_DCF_ALL_DCF_FILE:
 			// @patch4.5@wyeo@fix delete all bug, begin
				// empty fdb	deleted file	read only, hidden, system, label, directory
			if (	fdb[0] == 0 || fdb[0] == 0xe5 || ((fdb[11] & 0x1f)!=0x00) || \
				// long file name
				(fdb[11] == 0x0F && fdb[12] == 0x00))
			{
				match = FALSE;
			}
			else
			{
			G_USR_FileType = match = USR_CheckExtType(fdb);
			if (G_USR_FileType == 0)
				match = FALSE;
			else
				match = TRUE;

			//patch5.1@ada@Not delete the files with duplicated file index begin
			if (match == TRUE)
			{	
				#if 0
          			for (i = 4; i < 8; i++)
          			{
	               			index = index * 10 + (fdb[i] - 0x30);
          			}

				HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)index) - 1) * 10 + 2),K_SDRAM_Prefetch);
				HAL_DramWordDataRead(&tmp1,&tmp2);

				if ((tmp1 == 0xff) && (tmp2 == 0xff))
				{
					match = FALSE;
				}
				#endif
			}
			//patch5.1@ada@Not delete the files with duplicated file index end
			}
			break;
        }

        return match;
}

//-----------------------------------------------------------------------------
//USR_MatchIndexName
//-----------------------------------------------------------------------------
UINT8 USR_MatchIndexName(UINT8 *fdb, UINT16 fileIndex) USING_0
{
        UINT8 match = FALSE;
	UINT8 i;
	UINT8 ch;
	UINT8 indexname[4];

          //version4.0@ada@0513 for DCF
       	//if ( USR_IsOurBaseName(fdb) )
       	//{
		for (i = 4; i > 0; i--)
	        {
	                ch = (UINT8) (fileIndex % 10) + '0';
	                fileIndex /= 10;
	                indexname[i-1] = ch;
	        }
		if ( USR_IsSameStr( &fdb[4], &indexname[0], 4 ) )
			match = TRUE;
       	//}
       	return match;
}

//-----------------------------------------------------------------------------
//dcfFileDramFdbParameterRead
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 for Multi-Frame Play Back
UINT8 dcfFileDramFdbParameterRead(UINT16 fileIdx, FDBInfo_t* fileInfo) USING_0
{
     UINT8 tmp1,tmp2,tmp3,tmp4;
     UINT8 ret = SUCCESS;

     if (fileInfo == NULL) {
//printf("1\n");
	     return FAIL;
     }

     HAL_DramStartAddrSet(K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)fileIdx - 1)) * 10, K_SDRAM_Prefetch);

     //version4.0@ada@0513 for DCF
     //Read file FDB logical address
     HAL_DramWordDataRead(&tmp1,&tmp2);
     HAL_DramWordDataRead(&tmp3,&tmp4);

     WRITE8(G_DOS_FdbAddress, 3, tmp1);
     WRITE8(G_DOS_FdbAddress, 2, tmp2);
     WRITE8(G_DOS_FdbAddress, 1, tmp3);
     WRITE8(G_DOS_FdbAddress, 0, tmp4);
     fileInfo->FdbAddr = G_DOS_FdbAddress;
     //Read file attribute and file type
     HAL_DramWordDataRead(&tmp1,&tmp2);

     fileInfo->FileAttrib = G_DOS_FileAttribute = tmp1;
     fileInfo->FileType = tmp2;

     //Read file start cluster
     HAL_DramWordDataRead(&tmp1,&tmp2);
//printf("2 tmp1 %bx, tmp2 %bx\n",tmp1,tmp2);

     WRITE8(fileInfo->StartCluster, 1, tmp1);
     WRITE8(fileInfo->StartCluster, 0, tmp2);
//printf("FileInfo->StartCluster %x\n",FileInfo->StartCluster);
     /*if (fileInfo->StartCluster == 0) {
//printf("2\n");
           ret  = FAIL;
     }*/

     //Read file size in bytes
     HAL_DramWordDataRead(&tmp1,&tmp2);
     HAL_DramWordDataRead(&tmp3,&tmp4);

     WRITE8(G_DOS_FileSize, 3, tmp1);
     WRITE8(G_DOS_FileSize, 2, tmp2);
     WRITE8(G_DOS_FileSize, 1, tmp3);
     WRITE8(G_DOS_FileSize, 0, tmp4);
     fileInfo->FileSize = G_DOS_FileSize;

     HAL_DramWordDataRead(&tmp1,&tmp2);
     HAL_DramWordDataRead(&tmp3,&tmp4);

     fileInfo->FileName[0] = tmp1;
     fileInfo->FileName[1] = tmp2;
     fileInfo->FileName[2] = tmp3;
     fileInfo->FileName[3] = tmp4;
	 
	 if(tmp1 == 0){ //file wiht this index not existed
		ret = FAIL;
	 }

     //Read previous file index
     HAL_DramWordDataRead(&tmp1,&tmp2);

     WRITE8(G_USR_SearchPreFileIndex, 1, tmp1);
     WRITE8(G_USR_SearchPreFileIndex, 0, tmp2);
     fileInfo->PreIndex = G_USR_SearchPreFileIndex;

     //Read next file index
     HAL_DramWordDataRead(&tmp1,&tmp2);

     WRITE8(G_USR_SearchNextFileIndex, 1, tmp1);
     WRITE8(G_USR_SearchNextFileIndex, 0, tmp2);
     fileInfo->NextIndex = G_USR_SearchNextFileIndex;

     return ret;
}

//-----------------------------------------------------------------------------
//dcfFileDramFdbParameterWrite
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 for Multi-Frame Play Back
//version4.0@ada@0513 for DCF
void dcfFileDramFdbParameterWrite(UINT8 fileType,UINT16 fileIndex) USING_0
{
     UINT32 DramAddrOffset,DramAddr;

	//patch4.5@ada@Record DCF file name begin
	UINT8 i;
	UINT8 name[4];
	//patch4.5@ada@Record DCF file name end

     DramAddrOffset = sysMemAddrGet(MEM_IDX_FDB);
	

	//patch4.5@ada@Record DCF file name begin
     DramAddr = DramAddrOffset + ((UINT32)fileIndex - 1) * 10;
     //DramAddr = DramAddrOffset + ((UINT32)FileIndex - 1) * 8;
	//patch4.5@ada@Record DCF file name end

     HAL_DramStartAddrSet(DramAddr,K_SDRAM_NoPrefetch);
	//printf("write G_DOS_FdbAddress %lx\n",G_DOS_FdbAddress);

     //version4.0@ada@0513 for DCF
     //Record file FDB logical address
     HAL_DramWordDataWrite((UINT8)(G_DOS_FdbAddress & 0x000000ff),(UINT8)((G_DOS_FdbAddress & 0x0000ff00) >> 8));
     HAL_DramWordDataWrite((UINT8)((G_DOS_FdbAddress & 0x00ff0000) >> 16),(UINT8)((G_DOS_FdbAddress & 0xff000000) >> 24));

     //Record file attribute and file type
     HAL_DramWordDataWrite(G_DOS_FileAttribute,fileType);

     //Record file start cluster
     HAL_DramWordDataWrite((UINT8)(G_DOS_StartClusterNumber & 0x00ff),(UINT8)((G_DOS_StartClusterNumber & 0xff00) >> 8));

     //Record file size in bytes
     HAL_DramWordDataWrite((UINT8)(G_USR_NewFileSizeInFdb & 0x000000ff),(UINT8)((G_USR_NewFileSizeInFdb & 0x0000ff00) >> 8));
     HAL_DramWordDataWrite((UINT8)((G_USR_NewFileSizeInFdb & 0x00ff0000) >> 16),(UINT8)((G_USR_NewFileSizeInFdb & 0xff000000) >> 24));

	//patch4.5@ada@Record DCF file name begin
	//Write DCF file name
	for (i = 0; i < 4; i++)
	{
        	name[i]  = G_USR_Company_UpperSIG[i];
	}

	for (i = 0; i < 4; i += 2)
	{
     	HAL_DramWordDataWrite(name[i],name[i + 1]);
	}
	//patch4.5@ada@Record DCF file name end

     G_USR_SearchPreFileIndex = G_USR_LastFileIndex;

     //Record previous file index
     HAL_DramWordDataWrite((UINT8)(G_USR_SearchPreFileIndex & 0x00ff),(UINT8)((G_USR_SearchPreFileIndex & 0xff00) >> 8));

     if (G_USR_FileCount)
     {
          G_USR_SearchNowFileIndex = G_USR_FileIndex;
          //In previous file FDB parameter area,record the next file index(This file index)

		//patch4.5@ada@Record DCF file name begin
          DramAddr = DramAddrOffset + ((UINT32)G_USR_SearchPreFileIndex - 1) * 10 + 9;
          //DramAddr = DramAddrOffset + ((UINT32)G_USR_SearchPreFileIndex - 1) * 8 + 7;
		//patch4.5@ada@Record DCF file name end

          HAL_DramStartAddrSet(DramAddr,K_SDRAM_NoPrefetch);
          HAL_DramWordDataWrite((UINT8)(G_USR_SearchNowFileIndex & 0x00ff),(UINT8)((G_USR_SearchNowFileIndex & 0xff00) >> 8));
     }
}

//-----------------------------------------------------------------------------
//dcfFileDramFdbParameterClear
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 for Multi-Frame Play Back
//version4.0@ada@0513 for DCF
void dcfFileDramFdbParameterClear(UINT8 fileType,UINT16 fileIndex) USING_0
{
     UINT8 i;
     UINT32 DramAddrOffset,DramAddr;
     UINT8 tmp1,tmp2;
     UINT16 PreIndex,NextIndex;
	 fileType = fileType;
	 DramAddrOffset = K_SDRAM_JPG_FdbParameterBufAddr;
	 /*
     if (fileType == K_DCF_JPG_TYPE)
     {
          DramAddrOffset = K_SDRAM_JPG_FdbParameterBufAddr;
     }
     else if (fileType == K_DCF_AVI_TYPE)
     {
          DramAddrOffset = K_SDRAM_AVI_FdbParameterBufAddr;
     }
     else if (fileType == K_DCF_WAV_TYPE)
     {
          DramAddrOffset = K_SDRAM_WAV_FdbParameterBufAddr;
     }
	 else
  	 {
  	 	  DramAddrOffset = K_SDRAM_JPG_FdbParameterBufAddr;
	 }
	*/
	//patch4.5@ada@Record DCF file name begin
     DramAddr = DramAddrOffset + ((UINT32)fileIndex - 1) * 10;
     //DramAddr = DramAddrOffset + ((UINT32)FileIndex - 1) * 8;
	//patch4.5@ada@Record DCF file name end

	//patch4.5@ada@Record DCF file name begin
     HAL_DramStartAddrSet(DramAddr + 8,K_SDRAM_Prefetch);
     //HAL_DramStartAddrSet(DramAddr + 6,K_SDRAM_Prefetch);
	//patch4.5@ada@Record DCF file name begin

     //Read previous file index
     HAL_DramWordDataRead(&tmp1,&tmp2);
     PreIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);
     //Read next file index
     HAL_DramWordDataRead(&tmp1,&tmp2);
     NextIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);

     ////printf("Previous index of this deleting file is %d\n",PreIndex);
     ////printf("Next index of this deleting file is %d\n",NextIndex);

     if (PreIndex == 0)  //The first file in this directory
     {
          if (NextIndex > 0)
          {
			//patch4.5@ada@Record DCF file name begin
               HAL_DramStartAddrSet(DramAddrOffset + ((UINT32)fileIndex - 1) * 10,K_SDRAM_NoPrefetch);
               //HAL_DramStartAddrSet(DramAddrOffset + ((UINT32)FileIndex - 1) * 8,K_SDRAM_NoPrefetch);
			//patch4.5@ada@Record DCF file name end

               HAL_DramWordDataWrite(0x00,0x00);
               G_USR_FirstFileIndex = NextIndex;
              // G_ChangePage = 0;
               //printf("Window stay1\n");
          }
          else
          {
               //G_ChangePage = 1;
          }
     }
     else
     {
          if (NextIndex > 0)  //The file not the first or the last in this directory
          {
			//patch4.5@ada@Record DCF file name begin
               HAL_DramStartAddrSet(DramAddrOffset + ((UINT32)PreIndex - 1) * 10 + 9,K_SDRAM_NoPrefetch);
               //HAL_DramStartAddrSet(DramAddrOffset + ((UINT32)PreIndex - 1) * 8 + 7,K_SDRAM_NoPrefetch);
			//patch4.5@ada@Record DCF file name end

               HAL_DramWordDataWrite((UINT8)(NextIndex & 0x00ff),(UINT8)((NextIndex & 0xff00) >> 8));

			//patch4.5@ada@Record DCF file name begin
               HAL_DramStartAddrSet(DramAddrOffset + ((UINT32)NextIndex - 1) * 10 + 8,K_SDRAM_NoPrefetch);
               //HAL_DramStartAddrSet(DramAddrOffset + ((UINT32)NextIndex - 1) * 8 + 6,K_SDRAM_NoPrefetch);
			//patch4.5@ada@Record DCF file name end

               HAL_DramWordDataWrite((UINT8)(PreIndex & 0x00ff),(UINT8)((PreIndex & 0xff00) >> 8));
               // G_ChangePage = 0;
               //printf("Window stay2\n");
          }
          else //The last file in this directory
          {
			//patch4.5@ada@Record DCF file name begin
               HAL_DramStartAddrSet(DramAddrOffset + ((UINT32)PreIndex - 1) * 10 + 9,K_SDRAM_NoPrefetch);
               //HAL_DramStartAddrSet(DramAddrOffset + ((UINT32)PreIndex - 1) * 8 + 7,K_SDRAM_NoPrefetch);
			//patch4.5@ada@Record DCF file name end

               HAL_DramWordDataWrite(0x00,0x00);

               if (G_USR_FileCount > 0)
               {
                    G_USR_LastFileIndex = PreIndex;
               }
			   //patch4.5@ada@Fix file index bug begin
               //else
               //{
               //     G_USR_LastFileIndex = 0;
               //}
			   //patch4.5@ada@Fix file index bug end
          }
     }

     HAL_DramStartAddrSet(DramAddr,K_SDRAM_NoPrefetch);

	//patch4.5@ada@Record DCF file name begin
     for (i = 0; i < 10; i++)
     {
          HAL_DramWordDataWrite(0x00,0x00);
     }

     //for (i = 0; i < 8; i++)
     //{
     //     HAL_DramWordDataWrite(0x00,0x00);
     //}
	//patch4.5@ada@Record DCF file name end

/*add by phil@08.05.16 s*/	
	 G_USR_FileCount--;
 
	 if (G_USR_FileCount == 0) {
		 G_USR_FirstFileIndex = 0;
		 G_USR_LastFileIndex = 0;
	 }
/*add by phil@08.05.16 e*/  

}

//-----------------------------------------------------------------------------
//USR_ResetDramFdbParameterArea
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 for Multi-Frame Play Back
//version4.0@ada@0513 for DCF
//patch4.2@ada@0527 for DCF
void USR_ResetDramFdbParameterArea(void)
{
     HAL_GprmDramFill(K_SDRAM_JPG_FdbParameterBufAddr,(K_SDRAM_JPG_FdbParameterBufSize<<1),0x00);
//     HAL_GprmDramFill(K_SDRAM_JPG_FdbParameterBufAddr,(UINT16)(K_SDRAM_JPG_FdbParameterBufSize / 2),0x00);
//     HAL_GprmDramFill(K_SDRAM_JPG_FdbParameterBufAddr + (K_SDRAM_JPG_FdbParameterBufSize / 2),(UINT16)(K_SDRAM_JPG_FdbParameterBufSize / 2),0x00);
//     HAL_GprmDramFill(K_SDRAM_AVI_FdbParameterBufAddr,(UINT16)(K_SDRAM_AVI_FdbParameterBufSize / 2),0x00);
//     HAL_GprmDramFill(K_SDRAM_AVI_FdbParameterBufAddr + (K_SDRAM_AVI_FdbParameterBufSize / 2),(UINT16)(K_SDRAM_AVI_FdbParameterBufSize / 2),0x00);
//     HAL_GprmDramFill(K_SDRAM_WAV_FdbParameterBufAddr,(UINT16)(K_SDRAM_WAV_FdbParameterBufSize / 2),0x00);
//     HAL_GprmDramFill(K_SDRAM_WAV_FdbParameterBufAddr + (K_SDRAM_WAV_FdbParameterBufSize / 2),(UINT16)(K_SDRAM_WAV_FdbParameterBufSize / 2),0x00);

     //patch4.4@ada@MP3 end
}

//-----------------------------------------------------------------------------
//save fdb info to buffer
//-----------------------------------------------------------------------------
UINT8 dcfFileDramFdbParameterSave(UINT16 fileIdx, FDBInfo_t* fileInfo) USING_0
{
	UINT8 ret = SUCCESS;

	if (fileInfo == NULL) {
		return FAIL;
	}

	HAL_DramStartAddrSet(K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)fileIdx - 1)) * 10, K_SDRAM_NoPrefetch);

	//Write file FDB logical address
	G_DOS_FdbAddress = fileInfo->FdbAddr;
	HAL_DramWordDataWrite((UINT8)(G_DOS_FdbAddress & 0x000000ff),(UINT8)((G_DOS_FdbAddress & 0x0000ff00) >> 8));
	HAL_DramWordDataWrite((UINT8)((G_DOS_FdbAddress & 0x00ff0000) >> 16),(UINT8)((G_DOS_FdbAddress & 0xff000000) >> 24));

	//Write file attribute and file type
	G_DOS_FileAttribute = fileInfo->FileAttrib;
	HAL_DramWordDataWrite(G_DOS_FileAttribute,fileInfo->FileType);

	//Write file start cluster
	HAL_DramWordDataWrite((UINT8)(fileInfo->StartCluster & 0x00ff),(UINT8)((fileInfo->StartCluster & 0xff00) >> 8));

	//Write file size in bytes
	G_DOS_FileSize = fileInfo->FileSize;
	HAL_DramWordDataWrite((UINT8)(G_DOS_FileSize & 0x000000ff),(UINT8)((G_DOS_FileSize & 0x0000ff00) >> 8));
	HAL_DramWordDataWrite((UINT8)((G_DOS_FileSize & 0x00ff0000) >> 16),(UINT8)((G_DOS_FileSize & 0xff000000) >> 24));

	//Write filename
	HAL_DramWordDataWrite(fileInfo->FileName[0],fileInfo->FileName[1]);
	HAL_DramWordDataWrite(fileInfo->FileName[2],fileInfo->FileName[3]);

	//Write previous file index
	G_USR_SearchPreFileIndex = fileInfo->PreIndex;
	HAL_DramWordDataWrite((UINT8)(G_USR_SearchPreFileIndex & 0x00ff),(UINT8)((G_USR_SearchPreFileIndex & 0xff00) >> 8));

	//Write next file index
	G_USR_SearchNextFileIndex = fileInfo->NextIndex;
	HAL_DramWordDataWrite((UINT8)(G_USR_SearchNextFileIndex & 0x00ff),(UINT8)((G_USR_SearchNextFileIndex & 0xff00) >> 8));

	return ret;
}

//-----------------------------------------------------------------------------
//dcfFileReadOnlyAttrSet
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for Read Only
void dcfFileReadOnlyAttrSet(UINT16 FileIndex,UINT8 FileType,UINT8 ReadOnlyOn) USING_0
{
	UINT8 tmp1,tmp2;
	UINT32 DramAddrOffset,DramAddr;

	//xian ++ 20080826
	UINT8 memoName[12];
	FDBInfo_t fileInfo;
	UINT16 fileCnt;

	UINT32 addrTmp =K_SDRAM_ImagePlaybackGeneralBufAddr;// K_SDRAM_ImagePlaybackBufAddr;
	UINT32 addr;
	UINT8 attr;
	UINT8 FDB_indx,DevId;
	UINT32 pageLogAddr ;
  
	DramAddrOffset = K_SDRAM_JPG_FdbParameterBufAddr;
	if(dcfFileDramFdbParameterRead(FileIndex, &fileInfo) != SUCCESS)
		return;
	DramAddr = DramAddrOffset + ((UINT32)FileIndex - 1) * 10;

	//update fdb
	FDB_indx=(UINT8)(G_DOS_FdbAddress&0x0000000f);
	pageLogAddr = G_DOS_FdbAddress>>4;
	addr = addrTmp+ FDB_indx*16 + 5;
	DevId = DOS_CurrDriverGet();
	if(0 == DOS_SectorRead(DevId, pageLogAddr, 1, addrTmp))
		return ;

	HAL_DramStartAddrSet(addr,K_SDRAM_Prefetch);
	HAL_DramWordDataRead(&tmp1,&tmp2);
	attr= (tmp2 & 0xfe) | (ReadOnlyOn?1:0);
	HAL_DramStartAddrSet(addr,K_SDRAM_NoPrefetch);
	HAL_DramWordDataWrite(tmp1,attr);
	if(0 == DOS_SectorWrite(DevId, pageLogAddr, 1, addrTmp))
		return ;

	//update fdb buffer
	HAL_DramStartAddrSet(DramAddr + 2,K_SDRAM_Prefetch);
	HAL_DramWordDataRead(&tmp1,&tmp2);

	HAL_DramStartAddrSet(DramAddr + 2,K_SDRAM_NoPrefetch);
	HAL_DramWordDataWrite(attr,tmp2);
		 
	FileType = fileInfo.FileType;
	if (FileType == K_DCF_JPG_TYPE){
		memoName[8] = 'W';
		memoName[9] = 'A';
		memoName[10] = 'V';
		memoName[11] = '\0';

		fileCnt = FileIndex;
		for(tmp1=7; tmp1 >= 4; tmp1--){
			memoName[tmp1] = '0' + (UINT8)(fileCnt % 10);
			fileCnt /= 10;
		}        
		for(tmp1=0; tmp1 < 4; tmp1++)
			memoName[tmp1] = fileInfo.FileName[tmp1];

		if(ReadOnlyOn)
			dcfFileAttrSet(memoName, memoName, ATTR_R | ATTR_A, ATTR_MODIFY_ATTR);
		else
			dcfFileAttrSet(memoName, memoName, ATTR_A, ATTR_MODIFY_ATTR);
		
		//xian ++ 20121116, #45355 dcfFileAttrSet() modify fdb buffer, so save back 
		fileInfo.FileAttrib = (fileInfo.FileAttrib & 0xfe) | (ReadOnlyOn?1:0);
		dcfFileDramFdbParameterSave(FileIndex, &fileInfo);
	}     

}
#if 0
//-----------------------------------------------------------------------------
//USR_SetFdb
//-----------------------------------------------------------------------------
UINT8 DOS_SetFdb(UINT8 *fdb, UINT32 fdbLogAddr) USING_0
{
        UINT8 sts;
        UINT16 offset;
	UINT32 pageLogAddr;
        UINT16 k;

	M_DOS_ClearError();

        pageLogAddr = ( fdbLogAddr >> K_Card_PageSizeShift ) << K_Card_PageSizeShift;
	offset = fdbLogAddr - pageLogAddr;
	//printf("1 pageLogAddr[%lu]\n",pageLogAddr);
        if ( sts = DOS_ReadSector( K_DOS_DstSram, pageLogAddr, 0 ) )
        {
                for ( k = 0; k < 32; k++ )
                        G_ucDataBuffer[offset+k] = fdb[k];
                // patch 4.4-6@wyeo@improve non smc nand card sram to card performance
		//dumpData(32, &G_ucDataBuffer[offset], 0);
	        sts = DOS_WriteSector(K_DOS_SrcSram, pageLogAddr, 0);
        }

        return sts;
}
#endif
#if 0
//-----------------------------------------------------------------------------
//USR_GetFdb
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for Read-Only
UINT8 USR_GetFdb(UINT8 *fdb,UINT32 fdbLogAddr) USING_0
{
     UINT8 sts;
     UINT16 offset;
	UINT32 pageLogAddr;
     UINT16 k;

	M_DOS_ClearError();

       pageLogAddr = (fdbLogAddr >> K_Card_PageSizeShift) << K_Card_PageSizeShift;
	offset = fdbLogAddr - pageLogAddr;

	//printf("2 pageLogAddr[%lu]\n",pageLogAddr);
	if (sts = DOS_ReadSector(K_DOS_DstSram,pageLogAddr,0))
	{
	  for ( k = 0; k < 32; k++ )
	  {
	       fdb[k] = G_ucDataBuffer[offset + k];
	
	  }
	}
	//dumpData(32, fdb, 0);

     return sts;
}
#endif
//-----------------------------------------------------------------------------
//dcfRtcTimeUpdate
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For Date & Time Update

void dcfRtcTimeUpdate(void) USING_0
{
     HAL_GlobalReadRTC(&G_DATEINFO);
     G_DOS_CreateDate = ((UINT16)(G_DATEINFO.Year + 20) << 9) + ((UINT16)(G_DATEINFO.Month) << 5) + (UINT16)(G_DATEINFO.Day);
     //patch4.4.1@richie@dt
     //update for dos date time error
     G_DOS_CreateTime = ((UINT16)(G_DATEINFO.Hour) << 11) + ((UINT16)(G_DATEINFO.Minute) << 5) + ((UINT16)(G_DATEINFO.Second) >> 1);
}

//-----------------------------------------------------------------------------
//USR_CheckExtType
//-----------------------------------------------------------------------------
UINT8 USR_CheckExtType(UINT8 *name) USING_0
{
        UINT8 fileType = 0;

       	if ( !(G_DCF_FileType_Mask&K_USR_JPG_TYPE_MASK) && USR_IsOurFileExt(name,MEDIA_JPG) )
               fileType = K_DCF_JPG_TYPE;
        #if ( K_File_HaveVideo )
       	else if ( !(G_DCF_FileType_Mask&K_USR_AVI_TYPE_MASK) && USR_IsOurFileExt(name,MEDIA_AVI) )
               fileType = K_DCF_AVI_TYPE;
        #endif
//        #if ( AUDIO_OPTION == 1 )
       	else if ( !(G_DCF_FileType_Mask&K_USR_WAV_TYPE_MASK) && USR_IsOurFileExt(name,MEDIA_WAV) )
               fileType = K_DCF_WAV_TYPE;
//        #endif
		else if ( !(G_DCF_FileType_Mask& K_USR_WGI_TYPE_MASK) && USR_IsOurFileExt(name,MEDIA_TLS) )
               fileType = K_DCF_WGI_TYPE;
        //patch4.4@ada@MP3 begin
        //patch4.4@ada@MP3 end

 	// WWWW0410, Fix delete last bug
        //if ( fileType != 0 )
        //	G_USR_FileType = fileType;

        return fileType;
}

//xian ++ 20080716
//return *count = 100~999
UINT8 USR_GetMediaFolderCount(UINT8 *name, UINT16 *count)
{
    UINT8 i;
    UINT8 ret = FAIL;

    if(!name || !count)
        return ret;

    *count = 0;
    for (i = 0; i < 3; i++)
    {
        if('0' > name[i] || '9' < name[i])
            return ret;
        *count = *count * 10 + (UINT16)(name[i] - '0');
    }
    if(*count > 999 || *count < 100)
        return ret;

    for(; i < 8; i++){
        if('_' != name[i] && ('A' > name[i] || 'Z' < name[i])&&('0' > name[i] || '9' < name[i]))
            return ret;
    }
    ret = SUCCESS;
	return ret;    
}


UINT8 USR_FolderCategory(UINT8 *name,UINT8 opt)
{
	UINT8 ret = TRUE;
    UINT8 i;
	UINT8 dirName[12];
	
	switch(opt){
		case NORMAL_SNAPFOLDER:
			memcpy( dirName,G_USR_Dir1Name, 8);
		break;
		case TIMELAPSE_VIDEOFOLDER:
			memcpy( dirName,G_USR_TLSVIDDirName, 8);
		break;
	}
	
	//printf("dirName=%s\n",dirName);
	for(i = 3; ret && i < 8; i++){
        ret = (name[i] == dirName[i]);
		if(!ret){
			break;
		}
    }
    return ret;
}

//-----------------------------------------------------------------------------
//dcfDirInitialize
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For DCF
UINT8 dcfDirInitialize(void)
{
	//UINT32 t0,t1;
	//UINT8 dirName[9] = "100MEDIA";
	UINT8 dirName[12];
	UINT8 sts = TRUE;
	UINT8 DevId = DOS_CurrDriverGet();
	UINT16 cnt;
	UINT8 curDev; //cx++
	FDB fdb;  //cx++
//	FDBMediaFolderInfo_t info;
	UINT16 dirCnt;  //100~999

	//dirName[8]  = '\0';
	dcfInitDir1NameGet(dirName);
//	printf("dcfDirInitialize = %bu\n",DevId);
	G_USR_MaxDir1Count = 100;
	G_USR_Dir1Count = G_USR_MaxDir1Count;
	G_USR_AllFolderFileTotal = 0;
	sts = DIR_Change("DCIM"); 
	if( sts != DOS_SUCCESS ){
		sts = DIR_Create("DCIM");
		if( sts != DOS_SUCCESS ) return FALSE; 
		sts = DIR_Change("DCIM"); 
		if( sts != DOS_SUCCESS ) return FALSE; 
	}
	//G_USR_FileCount = 0;
	//USR_ResetDramFdbParameterArea();
    //goto 100MEDIA

	DIR_SearchReset(); //xian ++ 20100830 #40739    
	curDev = DOS_CurrDriverGet();      
 	//dbgTimerStop();
 	//__FUNC_TRACK__;
	while(1)
	{//search or create 100XXXXX
		sts = DIR_Search(curDev, "*", (FDB *)&fdb, 0);
		//printf("--dir:%s, %lu\n", fdb.name, fdb.start);
		if(sts == FIND_FILE_FAIL)
		{
			//__FUNC_TRACK__;
			// printf("create 100\n");
			sts = DIR_Create(dirName); 
			sts = DIR_Change(dirName);     
			if( sts != DOS_SUCCESS )
			{
				return FALSE;
			}
			dirCnt = 100; //xian ++41152
			sts = TRUE;  
			break;
		}        
		else if(fdb.attr & ATTR_D && fdb.name[0] != '.')
		{
			//printf("dir:%s, %lu\n", fdb.name, fdb.start);
			//__FUNC_TRACK__;
			if( USR_GetMediaFolderCount(fdb.name, &dirCnt) == SUCCESS)
			{ 
				if(dirCnt == 100)
				{ //xian ++ 41152
					DIR_Change(fdb.name); 
					break;
				}
			}                
		}
	}        

	if( dirCnt != 100 )
	{ //xian ++ 41152
    	sts = DIR_Create(dirName); 
    	sts = DIR_Change(dirName); 
		if( sts != DOS_SUCCESS ) 
		{
			return FALSE;
		}
		sts = TRUE;
	}
	G_USR_Dir1Count = 100; // should goto 100XXXXX, it is easy to find last unempty folder
	USR_UpdateMediaFolderInfo();

	dcfFileInitialize();
	G_USR_AllFolderFileTotal+=G_USR_FileCount;
	for(cnt=1;cnt<G_USR_TotalDirectory;cnt++)
	{
		dcfFolderNext(1);
		dcfFileInitialize();
		G_USR_AllFolderFileTotal+=G_USR_FileCount;        
	}
	return sts;
}

#if MT_DEBUG
static void showInfo(FDBMediaFolderInfo_t *info)
{
    printf("------------------------\n");
    printf("startCluster = %lu\n", info->startCluster);
    printf("ourDirMark = %x\n", info->ourDirMark);
    printf("name = %c%c%c%c%c%c%c%c\n", 
        info->name[0],info->name[1],info->name[2],info->name[3],
        info->name[4],info->name[5],info->name[6],info->name[7]
        );
    printf("prevCnt = %u\n", info->prevCnt);
    printf("nextCnt = %u\n", info->nextCnt);    
    printf("------------------------\n");
}
#endif

UINT8 USR_GetMediaFolderInfo(UINT16 dirCnt, FDBMediaFolderInfo_t *info)
{
    UINT8 ret = FAIL;
    UINT16 i;
    UINT32 addr;
    UINT8 lVal, hVal;

    if(!info || dirCnt < 100 || dirCnt > 999)
        return ret;
    
    addr = K_SDRAM_DcfDirBufAddr + ((UINT32)dirCnt - 100) * (sizeof(FDBMediaFolderInfo_t) >> 1);               
    HAL_DramStartAddrSet(addr,K_SDRAM_Prefetch);

    HAL_DramWordDataRead(&lVal, &hVal);
    info->startCluster = (UINT32)lVal + ((UINT32)hVal << 8);
    HAL_DramWordDataRead(&lVal, &hVal);
    info->startCluster = info->startCluster + ((UINT32)lVal << 16) + ((UINT32)hVal << 24);
        
    HAL_DramWordDataRead(&lVal, &hVal);
    info->ourDirMark = (UINT16)lVal + ((UINT16)hVal << 8);

    for(i = 0; i < 8; i+=2){
        HAL_DramWordDataRead(&lVal, &hVal);
        info->name[i] = lVal;
        info->name[i+1] = hVal;
    }

    HAL_DramWordDataRead(&lVal, &hVal);
    info->prevCnt= (UINT16)lVal + ((UINT16)hVal << 8);

    HAL_DramWordDataRead(&lVal, &hVal);
    info->nextCnt= (UINT16)lVal + ((UINT16)hVal << 8);

    return SUCCESS;
}

static UINT8 USR_SetMediaFolderInfo(UINT16 dirCnt, FDBMediaFolderInfo_t *info)
{
    UINT8 ret = FAIL;
    UINT16 i;
    UINT32 addr;
    UINT8 lVal, hVal;

    if(!info || dirCnt < 100 || dirCnt > 999)
        return ret;
    
    addr = K_SDRAM_DcfDirBufAddr + ((UINT32)dirCnt - 100) * (sizeof(FDBMediaFolderInfo_t) >> 1);               
    HAL_DramStartAddrSet(addr,K_SDRAM_NoPrefetch);

    lVal = (UINT8)info->startCluster;
    hVal = (UINT8)(info->startCluster >> 8);
    HAL_DramWordDataWrite(lVal, hVal);
    lVal = (UINT8)(info->startCluster >> 16);
    hVal = (UINT8)(info->startCluster >> 24);
    HAL_DramWordDataWrite(lVal, hVal);
        
    lVal = (UINT8)info->ourDirMark;
    hVal = (UINT8)(info->ourDirMark >> 8);
    HAL_DramWordDataWrite(lVal, hVal);

    for(i = 0; i < 8; i+=2){
        HAL_DramWordDataWrite(info->name[i], info->name[i+1]);
    }

    lVal = (UINT8)info->prevCnt;
    hVal = (UINT8)(info->prevCnt >> 8);
    HAL_DramWordDataWrite(lVal, hVal);

    lVal = (UINT8)info->nextCnt;
    hVal = (UINT8)(info->nextCnt >> 8);
    HAL_DramWordDataWrite(lVal, hVal);

    return SUCCESS;
}

void USR_GetNextMediaFolderCnt(UINT16 curDir, UINT16 *nextDir, UINT16 *prevDir)
{
    UINT8 lVal, hVal;
    UINT32 addr;

    if(!nextDir || !prevDir || curDir < 100 || curDir > 999)
        return ;

    addr = K_SDRAM_DcfDirBufAddr + ((UINT32)curDir - 100) * (sizeof(FDBMediaFolderInfo_t) >> 1) 
        + (sizeof(FDBMediaFolderInfo_t) >> 1) - 2;               

    HAL_DramStartAddrSet(addr,K_SDRAM_Prefetch);

    HAL_DramWordDataRead(&lVal, &hVal);
    *prevDir = (UINT16)lVal + ((UINT16)hVal << 8);
    HAL_DramWordDataRead(&lVal, &hVal);
    *nextDir = (UINT16)lVal + ((UINT16)hVal << 8);

    //printf("get : 0x%lx, %u, %u, %u\n",addr, curDir, *nextDir, *prevDir);
}

static void USR_SetNextMediaFolderCnt(UINT16 curDir, UINT16 nextDir, UINT16 prevDir)
{
    UINT8 lVal, hVal;
    UINT32 addr;

    if(curDir < 100 || curDir > 999)
        return ;


    addr = K_SDRAM_DcfDirBufAddr + ((UINT32)curDir - 100) * (sizeof(FDBMediaFolderInfo_t) >> 1) 
        + (sizeof(FDBMediaFolderInfo_t) >> 1) - 2;               

    //printf("set : 0x%lx, %u, %u, %u\n",addr, curDir, nextDir, prevDir);
    HAL_DramStartAddrSet(addr,K_SDRAM_NoPrefetch);

    lVal = (UINT8)prevDir;
    hVal = (UINT8)(prevDir >> 8);
    HAL_DramWordDataWrite(lVal, hVal);
    lVal = (UINT8)nextDir;
    hVal = (UINT8)(nextDir >> 8);
    HAL_DramWordDataWrite(lVal, hVal);
}

static UINT8 AddMediaFolderInfo(UINT16 dirCnt, FDBMediaFolderInfo_t *info)
{
    UINT8 ret = FAIL;
    UINT16 srhDir, nextDir, prevDir;
    UINT16 i = 0;

    if(!info || dirCnt < 100 || dirCnt > 999)
        return ret;

    srhDir = G_USR_MaxDir1Count;   //pointer to the tail
    if(srhDir < 100 ){//first info
        info->prevCnt = 0;
        info->nextCnt = 0;
    }
    else{
        while(i ++ < 900){
            USR_GetNextMediaFolderCnt(srhDir, &nextDir, &prevDir);
            if(dirCnt > srhDir){ //insert after tail
                USR_SetNextMediaFolderCnt(srhDir, dirCnt, prevDir);

                info->prevCnt = srhDir;
                info->nextCnt = nextDir;

                srhDir = nextDir;
                USR_GetNextMediaFolderCnt(srhDir, &nextDir, &prevDir);
                USR_SetNextMediaFolderCnt(srhDir, nextDir, dirCnt);                
                
                break;
            }else if(dirCnt < srhDir){
                if(prevDir){//not the header
                    srhDir = prevDir;
                }
                else{//insert as a header
                    USR_SetNextMediaFolderCnt(srhDir, nextDir, dirCnt);
    
                    info->nextCnt = srhDir;
                    info->prevCnt = prevDir;
                                       
                    break;
                }
            }
            else
                return ret;     //overlap
        }        
    }
    
    if(G_USR_MinDir1Count > dirCnt)
        G_USR_MinDir1Count = dirCnt;
    
    if(G_USR_MaxDir1Count < dirCnt)
        G_USR_MaxDir1Count = dirCnt;

#if MT_DEBUG
    //showInfo(info);
#endif
    USR_SetMediaFolderInfo(dirCnt, info);

    return SUCCESS;
}

#if 1//ndef MULTI_FOLDER_AS_ONE
UINT8 USR_UpdateMediaFolderInfo()
{
    UINT8 ret = FAIL;
    UINT8 sts;
    UINT8 curDev;
    UINT32 curDir;
    FDB fdb;
    UINT8 rootName[1+13]; // "\DCIM"
    FDBMediaFolderInfo_t info;
    UINT16 dirCnt;  //100~999
    UINT8 i;
        
    curDev = DOS_CurrDriverGet();
    curDir = DOS_CurrDirGet(curDev);

    //goto DCIM 
    rootName[0] = '\\';
    dcfDir0NameGet(rootName+1);    
    rootName[13] = '\0';    //fix file error when plug sd card
    if(DIR_Change(rootName) != DOS_SUCCESS){
        return ret;
    }
    HAL_GprmDramFill(K_SDRAM_DcfDirBufAddr,(K_SDRAM_DcfDirBufSize << 1),0x00);
 //   HAL_GprmDramFill(K_SDRAM_DcfDirBufAddr,(UINT16)(K_SDRAM_DcfDirBufSize / 2),0x00);
 //   HAL_GprmDramFill(K_SDRAM_DcfDirBufAddr + (K_SDRAM_DcfDirBufSize / 2),(UINT16)(K_SDRAM_DcfDirBufSize / 2),0x00);
    G_USR_MaxDir1Count = 99;
    G_USR_MinDir1Count = 1000;
    G_USR_TotalDirectory = 0;
	G_USR_MaxNormalDirCnt = 100;
	
    ret = SUCCESS;
	DIR_SearchReset(); //xian ++ 20100830 #40739
    while(1){
        sts = DIR_Search(curDev, "*", (FDB *)&fdb, 0);
        if(sts == FIND_FILE_FAIL){
            break;
        }        
        else if(fdb.attr & ATTR_D && fdb.name[0] != '.')
        {
            if( USR_GetMediaFolderCount(fdb.name, &dirCnt) == SUCCESS)
            {
                info.startCluster = fdb.start;

                for(i=0; i < 8; i ++){
                    info.name[i] = fdb.name[i];
                }
                    
                if(USR_FolderCategory(fdb.name,NORMAL_SNAPFOLDER)){
                    info.ourDirMark = 0xffff;
                    if(G_USR_MaxNormalDirCnt < dirCnt){
						G_USR_MaxNormalDirCnt = dirCnt;
                    }
                }
                else if(USR_FolderCategory(fdb.name,TIMELAPSE_VIDEOFOLDER)){
					info.ourDirMark = 0xfff0;
					if(G_USR_MaxTlsVidDirCnt < dirCnt){
						G_USR_MaxTlsVidDirCnt = dirCnt;
					}
		        }else{
		            info.ourDirMark = 0;
		        }
                                                
                if(AddMediaFolderInfo(dirCnt, &info) == SUCCESS){
                    G_USR_TotalDirectory ++;

                    if(curDir == fdb.start)
                        G_USR_Dir1Count = dirCnt;
                }                
            }
        }        
    }

	printf("total=%u, cur=%u\n", G_USR_TotalDirectory, G_USR_Dir1Count);    
    DOS_CurrDirSet(curDev, curDir);//go back org dir
    
    return ret;    
}

UINT8 dcfFolderNext(UINT8 forward)
{
    UINT8 ret = FAIL;
    UINT8 curDev;
    UINT16 curDir, nextDir, prevDir;
    FDBMediaFolderInfo_t info;
    
    if(G_USR_TotalDirectory > 1){        
        curDev = DOS_CurrDriverGet();
        curDir = G_USR_Dir1Count;
        USR_GetNextMediaFolderCnt(curDir, &nextDir, &prevDir);
//printf("change %u, %u, %u\n", curDir, nextDir, prevDir);
        if(nextDir == 0)//curDir is last
            nextDir = G_USR_MinDir1Count;

        if(prevDir == 0)//curDir is first
            prevDir = G_USR_MaxDir1Count;

        if(!forward)
            nextDir = prevDir;
        ret = USR_GetMediaFolderInfo(nextDir, &info);
//printf("next=%u, %lu\n", nextDir, info.startCluster);
        if(ret == SUCCESS){
            DOS_CurrDirSet(curDev, info.startCluster);
            G_USR_Dir1Count = nextDir; 
        }
    }   
    return ret;
}
#endif

#if MT_DEBUG
void testMediaCnt()
{
    UINT16 dirCnt = G_USR_MinDir1Count;
    UINT16 i;
    FDBMediaFolderInfo_t info;

    for(i = 0; i < G_USR_TotalDirectory; i ++){
        USR_GetMediaFolderInfo(dirCnt, &info);
        showInfo(&info);
        dirCnt = info.nextCnt;
    }

#if 0//def MULTI_FOLDER_AS_ONE
    //printf("dirTotal: %u, fileTotal: %lu, curDir: %u, firstFileIdx: %lu\n", 
        G_USR_TotalDirectory, G_USR_MediaFileTotal, G_USR_Dir1Count, G_USR_CurFolderFirstFileIdx);
#else
    //printf("dirTotal: %u, curDir: %u\n", 
        G_USR_TotalDirectory, G_USR_Dir1Count);
#endif
    //printf("G_USR_FileIndex=%u, G_USR_FileCount=%u\n", G_USR_FileIndex, G_USR_FileCount);
}
#endif

#if 0//def MULTI_FOLDER_AS_ONE
//-----------------------------------------------------------------------------
//USR_InitMediaFolderInfo
//goto the suitable folder
//-----------------------------------------------------------------------------
UINT8 USR_InitMediaFolderInfo()
{
    UINT8 ret = FAIL;
    UINT8 sts;
    UINT8 curDev;
    FDB fdb;
    UINT8 rootName[1+13]; // "\DCIM"
    FDBMediaFolderInfo_t info;
    UINT16 dirCnt, srhDir, nextDir, prevDir;  //100~999
    UINT8 i;
        
    curDev = DOS_CurrDriverGet();

    //goto DCIM 
    rootName[0] = '\\';
    dcfDir0NameGet(rootName+1);    
    rootName[13] = '\0';
    if(DIR_Change(rootName) != DOS_SUCCESS)
        return ret;

    HAL_GprmDramFill(K_SDRAM_DcfDirBufAddr,(UINT16)(K_SDRAM_DcfDirBufSize / 2),0x00);
    HAL_GprmDramFill(K_SDRAM_DcfDirBufAddr + (K_SDRAM_DcfDirBufSize / 2),(UINT16)(K_SDRAM_DcfDirBufSize / 2),0x00);
    G_USR_MaxDir1Count = 99;
    G_USR_MinDir1Count = 1000;
    G_USR_TotalDirectory = 0;
    G_USR_MediaFileTotal = 0;
    G_USR_Dir1Count = 0;
    G_USR_CurFolderFirstFileIdx = 0;
        
    ret = SUCCESS;
    while(1){
        sts = DIR_Search(curDev, "*", (FDB *)&fdb, 0);
        if(sts == FIND_FILE_FAIL){
            break;
        }        
        else if(fdb.attr & ATTR_D && fdb.name[0] != '.'){
            if( USR_GetMediaFolderCount(fdb.name, &dirCnt) == SUCCESS){
                info.startCluster = fdb.start;

                for(i=0; i < 8; i ++)
                    info.name[i] = fdb.name[i];
                    
                info.ourDirMark = 0;
                if(USR_IsOurMediaFolder(fdb.name))
                    info.ourDirMark |= (UINT16)OUR_DIR_MARK;

                                                
                if(AddMediaFolderInfo(dirCnt, &info) == SUCCESS){
                    G_USR_TotalDirectory ++;
                }                
            }
        }        
    }

    //get file total of each folder, and find current folder
    nextDir = G_USR_MinDir1Count;
    do{            
        srhDir = nextDir;
        USR_GetMediaFolderInfo(srhDir, &info);
        DOS_CurrDirSet(curDev, info.startCluster);
        
        dcfFileInitialize();
        info.ourDirMark = (info.ourDirMark & OUR_DIR_MARK) + G_USR_FileCount;
        USR_SetMediaFolderInfo(srhDir, &info);

        if(G_USR_FileCount){
            G_USR_CurFolderFirstFileIdx = G_USR_MediaFileTotal;
            G_USR_Dir1Count = srhDir;
        }
        G_USR_MediaFileTotal += (UINT32)G_USR_FileCount;
                        
        USR_GetNextMediaFolderCnt(srhDir, &nextDir, &prevDir);
    }while(nextDir != 0);
    if(G_USR_Dir1Count == 0){ //all folder are empty
        G_USR_Dir1Count = G_USR_MinDir1Count;
        USR_GetMediaFolderInfo(G_USR_Dir1Count, &info);
        DOS_CurrDirSet(curDev, info.startCluster);
        dcfFileInitialize();
    }
    else if(G_USR_Dir1Count != G_USR_MaxDir1Count){//last is empty
        USR_GetMediaFolderInfo(G_USR_Dir1Count, &info);
        DOS_CurrDirSet(curDev, info.startCluster);
        dcfFileInitialize();
    }

    return ret;    
}

//-----------------------------------------------------------------------------
//USR_NextFolder
//return SUCCESS : folder changed
//-----------------------------------------------------------------------------
UINT8 USR_NextFolder(UINT8 forward, UINT8 isSkipEmpty, UINT8 isRepeat)
{
    UINT8 ret = FAIL;
    UINT8 curDev;
    UINT16 curDir, nextDir, prevDir;
    FDBMediaFolderInfo_t info;
    UINT16 curFileTotal, nextFileTotal;
    UINT32 firstFileIndex;

    if(G_USR_TotalDirectory <= 1)
        return ret;
    
    curDev = DOS_CurrDriverGet();
    curDir = G_USR_Dir1Count;
    curFileTotal = G_USR_FileCount;
    firstFileIndex = G_USR_CurFolderFirstFileIdx;        
    do{
        USR_GetNextMediaFolderCnt(curDir, &nextDir, &prevDir);
#if MT_DEBUG
        printf("change %u, %u, %u\n", curDir, nextDir, prevDir);
#endif
        if(nextDir == 0){//curDir is last
            nextDir = G_USR_MinDir1Count;            
        }

        if(prevDir == 0){//curDir is first
            prevDir = G_USR_MaxDir1Count;
        }

        if(!forward)
            nextDir = prevDir;
        
        ret = USR_GetMediaFolderInfo(nextDir, &info); 
        if(ret != SUCCESS)
            break; //error
        
        nextFileTotal = info.ourDirMark & ~OUR_DIR_MARK;
        
        if(nextDir == G_USR_MinDir1Count)//first
            firstFileIndex = 0;
        else if(nextDir == G_USR_MaxDir1Count){//last
            firstFileIndex = G_USR_MediaFileTotal - (UINT32)nextFileTotal;
        }
        else{
            if(!forward){                    
                firstFileIndex -= (UINT32)nextFileTotal;
            }
            else{
                firstFileIndex += (UINT32)curFileTotal;
            }
        }

        if(nextFileTotal || !isSkipEmpty){//found suitable folder
            break;
        }
        
        curDir = nextDir; 
        curFileTotal = nextFileTotal;

        if(!isRepeat && (curDir == G_USR_MinDir1Count || curDir == G_USR_MaxDir1Count))
            break;
    }while(curDir != G_USR_Dir1Count);

    if(ret == SUCCESS){
        DOS_CurrDirSet(curDev, info.startCluster);
        G_USR_Dir1Count = nextDir;
        G_USR_CurFolderFirstFileIdx = firstFileIndex;
        G_USR_FileCount = nextFileTotal;
    }

    return ret;
}

//-----------------------------------------------------------------------------
//USR_UpdateNewFolderInfo
//update new folder info, at the upper dir of this folder 
//-----------------------------------------------------------------------------
UINT8 USR_UpdateNewFolderInfo(UINT8 *dirName)
{
    FDB fdb;
    UINT8 sts;
    UINT8 curDev;
    UINT16 dirCnt;
    FDBMediaFolderInfo_t info;
    UINT8 i;

    curDev = DOS_CurrDriverGet();

    sts = DIR_Search(curDev, dirName, (FDB *)&fdb, 0);
    if(sts == FIND_FILE_FAIL){
        sts = FAIL;
    }        
    else if(fdb.attr & ATTR_D && fdb.name[0] != '.'){
        if( USR_GetMediaFolderCount(fdb.name, &dirCnt) == SUCCESS){
            info.startCluster = fdb.start;

            for(i=0; i < 8; i ++)
                info.name[i] = fdb.name[i];
                            
            info.ourDirMark = 0; //save file total also
            if(USR_IsOurMediaFolder(fdb.name))
                info.ourDirMark |= (UINT16)OUR_DIR_MARK;
                                            
            if(AddMediaFolderInfo(dirCnt, &info) == SUCCESS){
                G_USR_TotalDirectory ++;
            }                
        }
    	sts = SUCCESS;
    }                
        
    return sts;
}

//-----------------------------------------------------------------------------
//USR_UpdateCurFolderInfo
//when current dir file cnt changed
//-----------------------------------------------------------------------------
UINT8 USR_UpdateCurFolderInfo(UINT8 mode )
{
    UINT8 curDev;
    UINT32 curDir;
    UINT16 srhDir, nextDir, prevDir;
    FDBMediaFolderInfo_t info;
    UINT16 fileTotal;
    UINT16 i = 0;
    UINT8 ret = FAIL;

#if MT_DEBUG
    printf("update mode=%bu\n", mode);
#endif

    curDev = DOS_CurrDriverGet();
    curDir = DOS_CurrDirGet(curDev);
    srhDir = G_USR_MinDir1Count;
    do{            
        USR_GetMediaFolderInfo(srhDir, &info);
        if(curDir == info.startCluster){//found
            fileTotal = info.ourDirMark & ~OUR_DIR_MARK; //org 
            if(mode == K_UPDATE_MODE_ALL){
                G_USR_MediaFileTotal = G_USR_MediaFileTotal - (UINT32)fileTotal;

                dcfFileInitialize();
                info.ourDirMark = (info.ourDirMark & OUR_DIR_MARK) + G_USR_FileCount;
                G_USR_MediaFileTotal += (UINT32)G_USR_FileCount;
            }
            else if(mode == K_UPDATE_MODE_ADD_ONE){
                G_USR_MediaFileTotal ++;
//                G_USR_FileCount ++;
                info.ourDirMark ++;
            }
            else if(mode == K_UPDATE_MODE_DEL_ONE){
                G_USR_MediaFileTotal --;
//                G_USR_FileCount --;
                info.ourDirMark --;
            }

            USR_SetMediaFolderInfo(srhDir, &info);
            ret = SUCCESS;
            break;            
        }
                  
        USR_GetNextMediaFolderCnt(srhDir, &nextDir, &prevDir);
        srhDir = nextDir;
    }while(srhDir != 0);

    return ret;
}

//-----------------------------------------------------------------------------
//dcfDirInitialize
//-----------------------------------------------------------------------------
static void USR_GetNextFolderName(UINT8 *name)
{
	UINT8 i, ch;
	UINT16 	rem;

	rem = G_USR_Dir1Count + 1;
	for (i = 2; i != 0xff ; i--)
	{
		ch = (UINT8) (rem % 10) + '0';
		rem /= 10;
		//printf("ch=%bx\n",ch);
		name[i] = ch;
	}                
	name[8]  = '\0';
}

//-----------------------------------------------------------------------------
//dcfDirInitialize
//-----------------------------------------------------------------------------
UINT8 dcfDirInitialize(void)
{
    UINT32 curDir;
	UINT8 dirName[9] = "100MEDIA";
	UINT8 sts = TRUE;
	UINT8 DevId = DOS_CurrDriverGet();

	dirName[8]  = '\0';
//	printf("dcfDirInitialize = %bu\n",DevId);

	sts = DIR_Change("DCIM"); 
	if( sts != DOS_SUCCESS ){
		sts = DIR_Create("DCIM");
		if( sts != DOS_SUCCESS ) return FALSE; 
		sts = DIR_Change("DCIM"); 
		if( sts != DOS_SUCCESS ) return FALSE; 
	}

	sts = DIR_Change(dirName); //100MEDIA
	if(sts != DOS_SUCCESS){
        //printf("create new dir : %s\n", dirName);
		sts = DIR_Create(dirName); 
        sts = DIR_Change(dirName); 
	}
	if(sts == DOS_SUCCESS ){
		sts = TRUE;
	}
	else{
		sts = FALSE;
	}
    
    USR_InitMediaFolderInfo();
    if(G_USR_FileIndex >= 10000){//create new folder
        curDir = DOS_CurrDirGet(DevId);
        USR_GetNextFolderName(dirName);
    	if( DIR_Change("..") != DOS_SUCCESS ) return FALSE; 

    	sts = DIR_Change(dirName); 
    	if(sts != DOS_SUCCESS){
    		sts = DIR_Create(dirName); 
            sts = USR_UpdateNewFolderInfo(dirName);
            if(sts == SUCCESS)
                sts = TRUE;
            else
                sts = FALSE;
    	}
        else{
            sts = TRUE;            
        }

        DOS_CurrDirSet(DevId, curDir);  //go back org folder
    }
    
	return sts;
}

//-----------------------------------------------------------------------------
//dcfDirInitialize
//return SUCCESS : folder changed
//-----------------------------------------------------------------------------
UINT8 USR_LastFolder(UINT8 isSkipEmpty)
{
    UINT8 ret = FAIL;
    UINT8 curDev;
    UINT16 curDir, nextDir, prevDir;
    FDBMediaFolderInfo_t info;
    UINT16 fileTotal;

    if(G_USR_TotalDirectory <= 1)
        return ret;
    
    curDev = DOS_CurrDriverGet();
    //curDir = G_USR_Dir1Count;
    //curFileTotal = G_USR_FileCount;
    //firstFileIndex = G_USR_CurFolderFirstFileIdx;
    //printf("G_USR_Dir1Count=%u\n", G_USR_Dir1Count);
    curDir = G_USR_MaxDir1Count;
    do{
        ret = USR_GetMediaFolderInfo(curDir, &info); 
        if(ret != SUCCESS)
            break; //error

        fileTotal = info.ourDirMark & ~OUR_DIR_MARK;
        if(fileTotal || !isSkipEmpty)
            break; //success        
        
        USR_GetNextMediaFolderCnt(curDir, &nextDir, &prevDir);
        
        if(nextDir == 0){//curDir is last
            nextDir = G_USR_MinDir1Count;            
        }

        if(prevDir == 0){//curDir is first
            prevDir = G_USR_MaxDir1Count;
        }

        curDir = prevDir;                      
    }while(curDir != G_USR_Dir1Count);
#if MT_DEBUG
    printf("set curDir=%u, ret=%bu\n", curDir, ret);
#endif
    if(curDir != G_USR_Dir1Count){//change folder
        DOS_CurrDirSet(curDev, info.startCluster);
        G_USR_Dir1Count = curDir;
        G_USR_CurFolderFirstFileIdx = G_USR_MediaFileTotal - fileTotal;
        dcfFileInitialize();
    }
    else
        ret = FAIL;

    return ret;
}

UINT32 USR_GetMediaFileTotal()
{
    return G_USR_MediaFileTotal;
}

UINT32 USR_GetCurFolderFirstFileIdx()
{
    return G_USR_CurFolderFirstFileIdx;
}

#endif //MULTI_FOLDER_AS_ONE

//xian --

//xain ++ 20080826
//-----------------------------------------------------------------------------
//dcfFileTypeEnable
//-----------------------------------------------------------------------------
void dcfFileTypeEnable(UINT8 type, UINT8 en)
{
    UINT8 mask;
    
    switch(type){
        case 0:
            mask = K_USR_JPG_TYPE_MASK | K_USR_AVI_TYPE_MASK | K_USR_WAV_TYPE_MASK | K_USR_WGI_TYPE_MASK;
            break;
            
        case K_DCF_JPG_TYPE:
            mask = K_USR_JPG_TYPE_MASK;
            break;                
        case K_DCF_AVI_TYPE:
            mask = K_USR_AVI_TYPE_MASK;
            break;
        case K_DCF_WAV_TYPE:
            mask = K_USR_WAV_TYPE_MASK;
            break;
		case K_DCF_WGI_TYPE:
			mask = K_USR_WGI_TYPE_MASK;
			break;
        default:
            return;            
    }
    if(en)
        G_DCF_FileType_Mask &= ~mask;
    else
        G_DCF_FileType_Mask |= mask;

    //printf("G_DCF_FileType_Mask=%bx\n", G_DCF_FileType_Mask);
}

//xian ++ 20081203
UINT8 dcfFileIndexUpdate(void)
{
   // UINT8 dirName[9] = "100MEDIA";
	UINT8 dirName[12];  //modified by rhy 20110504
    FDBMediaFolderInfo_t info;
	UINT16 dirCnt;
	
	dcfInitDir1NameGet(dirName);
	if(G_USR_bUsrAssignFolder){//xian ++ 20100223, dont change folder
		if(G_USR_FileIndex > 9999){
			G_USR_FileIndex = 1;
		}			
		return TRUE;
	}

	if(1 != sp1kSnapParamGet(SP1K_SNAP_TIMESLAPE_PAUSE) && sp1kSnapParamGet(SP1K_SNAP_TIMESLAPE_STS) ){
		dirCnt = G_USR_MaxTlsVidDirCnt;
	}else{
		dirCnt = G_USR_MaxNormalDirCnt;
	}
    //goto last folder
    if(G_USR_Dir1Count < dirCnt){
        do{
            dcfFolderNext(1);
        }while(G_USR_Dir1Count < G_USR_MaxDir1Count);
        dcfFileInitialize();
    }
    else if(G_USR_Dir1Count > dirCnt) { 
    	do{
    		dcfFolderNext(0);
    	}while(G_USR_Dir1Count > dirCnt);
		 dcfFileInitialize();
    }

    //xian ++ 20090116
    USR_GetMediaFolderInfo(G_USR_Dir1Count, &info);
    #if 0//def MULTI_FOLDER_AS_ONE
    info.ourDirMark &= (UINT16)OUR_DIR_MARK;
    #endif
    //xian --

    if(G_USR_FileIndex > 9999 || !info.ourDirMark){
		if(info.ourDirMark == 0xfff0){ //timelapse folder
			dcfDir1NameGet(dirName,0,TIMELAPSE_VIDEOFOLDER);
		}
		
        if( USR_CheckMediaFolder(dirName) != TRUE ){
			osMsgPost(SP1K_MSG_CARD_DCFFOLDER_FULL); //add by rhy20110921
			return FALSE;
        }
        //printf("create dirName=%s\n", dirName);
        if( DIR_Create(dirName) != DOS_SUCCESS ){
			return FALSE;
        }
        if( DIR_Change(dirName) != DOS_SUCCESS ) {
			return FALSE;
        }
#if 0//def MULTI_FOLDER_AS_ONE  //xian ++ 20080806
        USR_UpdateNewFolderInfo(dirName);
#else
        USR_UpdateMediaFolderInfo();    //xian ++ 20081203, #33748
#endif

        dcfFileInitialize();
    }
    //printf("fileIdx:%u\n", G_USR_FileIndex);
    return TRUE;
}
//xian --

//xian ++ 20090107
void dcfFileBaseNameUpdate(UINT8 *name)
{
    UINT8 i;

    for(i = 0; i < 4; i++){
        if(*name >= 'A' && *name <= 'Z'){
            G_USR_Company_UpperSIG[i] = *name;
            G_USR_Company_LowerSIG[i] = *name - 'A' + 'a';
        }
        else if(*name == '_'){
            G_USR_Company_UpperSIG[i] = *name;
            G_USR_Company_LowerSIG[i] = *name;
        }
            
        name ++;
    }
}
//xian --
#if 0
//xian ++ 20090112
//return handle
UINT32 USR_MakeDPSFileHandle(UINT16 fileOrd)
{
    UINT32 addr;
    UINT8 tmp1, tmp2;
    UINT16 fileCnt; //0001~9999 < 2^14
    UINT32 handle;
    UINT8 dev;
    FDBInfo_t fileInfo;
    FDBMediaFolderInfo_t dirInfo;

    USR_GetMediaFolderInfo(G_USR_Dir1Count, &dirInfo);

    if(!(dirInfo.ourDirMark&0x8000))  //OUR_DIR_MARK
        return 0; // dir not supported
    
	if(fileOrd > G_USR_FileCount) {
		return 0;
	}
	
	addr = K_SDRAM_FileOrderTableAddr + (fileOrd - 1);
	HAL_DramStartAddrSet(addr , K_SDRAM_Prefetch);
	HAL_DramWordDataRead(&tmp1, &tmp2);
	fileCnt = (UINT16)(((UINT16)tmp2) <<8) +( (UINT16)tmp1);

    dcfFileDramFdbParameterRead(fileCnt, &fileInfo);
    if(fileInfo.FileType != K_DCF_JPG_TYPE || !USR_IsOurBaseName(fileInfo.FileName))
        return 0; // file not supported

    handle = G_USR_Dir1Count;   //100~999 < 2^10
    handle <<= 16; // ;2^14 = 16384
    handle += (UINT32)fileCnt;

    dev = DOS_CurrDriverGet();
    if(dev > 0xf)
        return 0; //only support 0~15
    
    handle += ((UINT32)dev << 26);
        
    return handle;
}
//return TRUE/FALSE
UINT8 USR_ParseDPSFileHandle(UINT32 handle, UINT8* outPathFilename)
{
    UINT16 fileCnt;
    UINT16 dirCnt;
    UINT8 dev;
    UINT8 i;
    UINT8 *pt = outPathFilename;

    fileCnt = (UINT16)handle & 0x3fff;
    dirCnt = (UINT16)(handle >> 16) & 0x3ff;
    dev = (UINT8)(handle >> 26) & 0x0f;

    switch(dev){
        case DRIVE_NAND:
            *pt ++ = 'C';
            break;
        case DRIVE_SD:
            *pt ++ = 'D';
            break;
        case DRIVE_DRAM:
            *pt ++ = 'E';
            break;
#if (DOS_RSV_PART == 1)
        case DRIVE_NAND_RSV:
            *pt ++ = 'F';
            break;
#endif
        default:
            *pt = 0;
            return FALSE;                
    }
    *pt ++ = ':';
    *pt ++ = '\\';
            
	for ( i = 0; i < 4; i++ )
        *pt ++  = G_USR_Dir0Name[i];    //DCIM
    *pt ++ = '\\';

    pt += 7;
    for(i = 7; i > 2; i--)
        *pt -- = G_USR_Dir1Name[i];    //; 100MEDIA
    i = 3;
    while(i){
        *pt -- = '0' + dirCnt%10;
        dirCnt /= 10;
        i --;
    };           
    pt += 9;
    *pt ++ = '\\';
    
    dcfBaseNameGet(pt, fileCnt);    
    pt += 8;
    *pt ++ = '.';
    *pt ++ = 'J';
    *pt ++ = 'P';
    *pt ++ = 'G';
    *pt = 0;

    return TRUE;    
}
//xian --
#endif
void dcfNewFileDramFdbParameterUpdate(UINT8 fileType, UINT32 fileSize)
{
	G_USR_NewFileSizeInFdb = fileSize;

	dcfFileDramFdbParameterWrite(fileType, G_USR_FileIndex);
	G_USR_LastFileIndex = G_USR_FileIndex; //yichang@0510

	if (G_USR_FileCount == 0)
	{
		G_USR_FirstFileIndex = G_USR_FileIndex;
	}
    
	G_USR_AllFolderFileTotal++; //cx add for multi folder pb
	G_USR_FileIndex ++;
	G_USR_FileCount++;
	DOS_FileOrderWrite(G_USR_FileCount , G_USR_LastFileIndex);
}

UINT16 dcfFileIndexGet(void)
{
	return 	G_USR_FileIndex;
}

UINT16 dcfFileCountGet(void)
{
	return G_USR_FileCount;
}

UINT16 dcfTotalDirectoryGet(void)
{
	return G_USR_TotalDirectory;
}

UINT16 dcfDir1IndexGet(void)
{
	return G_USR_Dir1Count;
}

UINT32 dcfAllFolderTotalFileGet(void)
{
	return G_USR_AllFolderFileTotal;
}

//wangjg++ 20100930
void dcfProtectAllFile(UINT8 isReadOnly)
{
#if 1
	UINT8 sts;
	UINT16 TempDirIndex;
	UINT8 tmp1,tmp2;

	UINT32 addr = K_SDRAM_JPG_FdbParameterBufAddr;
	UINT32 i;
	UINT16 diridx=G_USR_Dir1Count;
	UINT16 fileCnt = G_USR_FileCount;//(G_USR_AllFolderFileTotal>9999) ? 9999:G_USR_AllFolderFileTotal;
	//UINT16 cnt=G_USR_Dir1Count;
	//UINT8 CurrDirName[9]; 
	UINT8 buf[sizeof(FDBMediaFolderInfo_t) + 1];
	FDBMediaFolderInfo_t *info = (FDBMediaFolderInfo_t *)buf;
	UINT8 *CurrDirName;
	//UINT32 t0,t1,t2;
	//x86_FDB xdata *pFdb;
	//UINT8 devId= DOS_CurrDriverGet();
	//UINT32 dirId = DOS_CurrDirGet(devId);

	isReadOnly = isReadOnly?0x01:0;

	//HAL_GlobalTimerRead10us(&t0);
	TempDirIndex = G_USR_MaxDir1Count;
	DIR_Change("\\DCIM"); //xian ++
	ENTER_CRITICAL(sts);
	//HAL_CpuRamMapSet(dos.TmpBufAddr, 512, (void xdata* xdata*)&pFdb);

	do{
		//G_USR_Dir1Count = TempDirIndex;	//xian -- #42012
		//sprintf(CurrDirName, "%d",TempDirIndex); 
		//strcat(CurrDirName, "MEDIA"); 
		USR_GetMediaFolderInfo(TempDirIndex, info); //xian ++, #40739 maybe not "MEDIA"
		CurrDirName = info->name;
		CurrDirName[8] = 0;
		DIR_Change(CurrDirName);  

		__FUNC_TRACK__;
		sts = vfsFileProtectAll(isReadOnly);
		__FUNC_TRACK__;

		DIR_Change("\\DCIM"); 
		HAL_DramStartAddrSet((K_SDRAM_DcfDirBufAddr + (((UINT32)TempDirIndex) - 100) * 9 + 7),K_SDRAM_Prefetch);
		HAL_DramWordDataRead(&tmp1,&tmp2);

		TempDirIndex = (((UINT16)tmp2) << 8) | ((UINT16)tmp1);
	}while(TempDirIndex != 0);
	//HAL_GlobalTimerRead10us(&t1);
	//DIR_Change("..");
	//__FUNC_TRACK__;
	//dcfDirInitialize();	
	USR_GetMediaFolderInfo(diridx, info); //xian ++, #40739 maybe not "MEDIA"
	CurrDirName = info->name;
	CurrDirName[8] = 0;
	DIR_Change(CurrDirName);  
	addr +=2;
	for(i = 1;i<=9999;i++)
	{			
		HAL_DramStartAddrSet(addr,1);
		HAL_DramWordDataRead(&tmp1,&tmp2);

		HAL_DramStartAddrSet(addr,0);		
		HAL_DramWordDataWrite((tmp1&0xfe)|isReadOnly,tmp2);
		addr += 10;// * 10ul;	
	}
	//DOS_CurrDirSet(devId, dirId);
	//HAL_GlobalTimerRead10us(&t2);
	EXIT_CRITICAL(sts);
	//printf("protect:%ld,%ld,%d\n",t1-t0,t2-t1,fileCnt);
#endif
}
void dcfDeleteAllFile(void)
{
	UINT8 sts;
	UINT16 TempDirIndex;
	UINT8 tmp1,tmp2;
	//UINT8 CurrDirName[9]; 
	UINT8 buf[sizeof(FDBMediaFolderInfo_t) + 1];
	FDBMediaFolderInfo_t *info = (FDBMediaFolderInfo_t *)buf;
	UINT8 CurrDirName[9];
	//UINT32 t0,t1,t2,t3;
	UINT8 i;
	//UINT8 name[8];
	UINT32 addr = K_SDRAM_TotalSize -512 - 320ul*240;
	UINT8 ret;
	//HAL_GlobalTimerRead10us(&t0);
	TempDirIndex = G_USR_MaxDir1Count;
	#if !SUPPORT_MCU_WTD
	ENTER_CRITICAL(ret);
	#endif
	DIR_Change("\\DCIM"); //xian ++
	//HAL_GprmDramFill(K_SDRAM_ImagePlaybackDisplayBufAddr,320ul*240*2,0);
	//HAL_GprmCopy(pbDispBuffGet(1),UINT16 srcHSize,UINT16 srcVSize,UINT16 srcHOft,UINT16 srcVOft,UINT32 dstAddrWord,UINT16 dstHSize,UINT16 dstVSize,UINT16 dstHOft,UINT16 dstVOft,UINT16 cpHSize,UINT16 cpVSize,UINT16 pstThd,UINT8
	gprmBlackFill(addr,320,240);
	dispShow(addr,320,240,100);
	dbgWdtSet(0);
	//HAL_GlobalTimerRead10us(&t2);
	do{
#if 0
		G_USR_Dir1Count = TempDirIndex;
		//sprintf(CurrDirName, "%d",TempDirIndex); 
		//strcat(CurrDirName, "MEDIA"); 
		USR_GetMediaFolderInfo(TempDirIndex, info); //xian ++, #40739 maybe not "MEDIA"
		CurrDirName = info->name;
		CurrDirName[8] = 0;
#endif
		HAL_DramStartAddrSet((K_SDRAM_DcfDirBufAddr + (((UINT32)TempDirIndex) - 100) * 9 + 3),K_SDRAM_Prefetch);

		for(i = 0; i < 8; i+=2){
			HAL_DramWordDataRead(&tmp1, &tmp2);
			CurrDirName[i] = tmp1;
			CurrDirName[i+1] = tmp2;
		}
		CurrDirName[8]=0;
		printf("%s\n",CurrDirName);
		HAL_DramWordDataRead(&tmp1, &tmp2);
		WRITE8(TempDirIndex, 1, tmp1);
		WRITE8(TempDirIndex, 0, tmp2);
		DIR_Change(CurrDirName);  

		//dbgWdtSet(0);		
		sts = vfsFileDeleteAll();//(K_DCF_ALL_DCF_FILE, K_DCF_OUR_TYPE);		
		if(sts != TRUE){ //error
			break;
		}
		//dbgWdtSet(1);
		DIR_Change("\\DCIM"); 
		DIR_Remove(CurrDirName); 

		//HAL_DramStartAddrSet((K_SDRAM_DcfDirBufAddr + (((UINT32)TempDirIndex) - 100) * 9 + 7),K_SDRAM_Prefetch);
		//HAL_DramWordDataRead(&tmp1,&tmp2);

		//TempDirIndex = (((UINT16)tmp2) << 8) | ((UINT16)tmp1);
	}while(TempDirIndex != 0);
	//HAL_GlobalTimerRead10us(&t3);
	dbgWdtSet(1);
	//HAL_GprmDramFill(pbDispBuffGet(1),960ul*720*2,0);
	gprmBlackFill(pbDispBuffGet(0),960,720);
	gprmBlackFill(pbDispBuffGet(1),960,720);
	//HAL_GprmCopy(pbDispBuffGet(1),UINT16 srcHSize,UINT16 srcVSize,UINT16 srcHOft,UINT16 srcVOft,UINT32 dstAddrWord,UINT16 dstHSize,UINT16 dstVSize,UINT16 dstHOft,UINT16 dstVOft,UINT16 cpHSize,UINT16 cpVSize,UINT16 pstThd,UINT8
	dispShow(pbDispBuffGet(1),960,720,100);
	DIR_Change("..");
	//__FUNC_TRACK__;
	//HAL_GlobalTimerRead10us(&t1);
	//printf("Del All:%ld,%ld\n",t1-t0,t3-t2);
	dcfDirInitialize();	
	#if !SUPPORT_MCU_WTD
	EXIT_CRITICAL(ret);
	#endif
}
//wangjg++
UINT8 dcfGetFileName(UINT8 *name,UINT16 index,UINT8 *pAttr)
{
	UINT8 *pt = name;
	UINT8 type,attr;
	UINT32 addr = K_SDRAM_JPG_FdbParameterBufAddr + (UINT32)10*(index-1);
	UINT8 extName[][4]={"XXX","JPG","AVI","WAV"};
	HAL_DramStartAddrSet( addr + 2,1);
	HAL_DramWordDataRead(&attr,&type);
	HAL_DramStartAddrSet( addr + 6,1);
	HAL_DramWordDataRead(pt++,pt++);
	HAL_DramWordDataRead(pt++,pt++);
	if(type>K_DCF_WAV_TYPE){type=0;}
	sprintf(pt,"%04u%s",index,extName[type]);
	pt[7]=0;
	/*
	pt+=4;
	
	switch(type)
	{
	case K_DCF_JPG_TYPE:
		*pt ++ = 'J';
		*pt ++ = 'P';
		*pt ++ = 'G';
		break;
	case K_DCF_AVI_TYPE:
		*pt ++ = 'A';
		*pt ++ = 'V';
		*pt ++ = 'I';
		break;
	case K_DCF_WAV_TYPE:
		*pt ++ = 'W';
		*pt ++ = 'A';
		*pt ++ = 'V';
		break;
	default:
		*pt ++ = 'X';
		*pt ++ = 'X';
		*pt ++ = 'X';
		*pt = 0;
		return FAIL;
		//break;
	} 		
    *pt = 0;	
    */
    //printf("%s\n",name);
	//printf("%s--%u,%bu,%bu\n",name,index,attr,type);
	if(pAttr){
		*pAttr=attr;
		}
	return type;
}
UINT8 dcfDeleteFile(UINT16 order,UINT8 isUpdateDCF,UINT8 filterType)
{
	//UINT16 idx = sp1kPbFileIndexByOrderGet(1);
	UINT8 name[12];
	UINT8 sts = SUCCESS;
	UINT16 index = DOS_FileIndexGet(order);
	UINT32 t0,t1;
	UINT8 type,attr;
	//UINT8 ret;
	HAL_GlobalReadGTimer(&t0);
	if(index==0){
		//printf("no file\n");
		return 2;
	}
	
	//get file name
	type = dcfGetFileName(name,index,&attr);
	//printf("type=%bu\n",type);
	//printf("dcfDel:%s\n",name);
	//#if VIDEO_SLCLIP_FILE_NOT_DELETE_READONLY
	if(attr&0x01){
		//HAL_GlobalReadGTimer(&t1);
		//printf("###dcfDel:%lu\n",t1-t0);
		//printf("readonly\n");
		return FAIL;
	}
	
	if(filterType && (type!=filterType)){	
		printf("type:%bx-%bx\n",type,filterType);
		return FAIL;		
	}
	/*#else
	if(filterType && (type!=filterType)){	
		printf("type:%bx-%bx\n",type,filterType);
		return SUCCESS;		
	}
	#endif
	*/
	//dcfAviNameGet(sldeletefinename, index);
	sts = DOS_FileDel(name);
	//vidClipCtrl.freeCluster = DOS_FreeDiskSpaceByCluster();
	if(isUpdateDCF && (sts==SUCCESS)){
		dcfFileDramFdbParameterClear(K_DCF_OUR_TYPE, index);
		DOS_FIleOrderRst(order);
		DOS_FileLastIndexClear();
	}		
	HAL_GlobalReadGTimer(&t1);
	printf("dcfDel:%s-%4u-%lu-%bu\n",name,index,t1-t0,sts);
	return sts?1:0;
}

void dcfDeleteFolder(UINT16 FolderIndex)
{
#if 1
	UINT8 buf[sizeof(FDBMediaFolderInfo_t) + 1];
	FDBMediaFolderInfo_t *info = (FDBMediaFolderInfo_t *)buf;
	UINT8 *CurrDirName;
     
	USR_GetMediaFolderInfo(FolderIndex, info); //xian ++, #40739 maybe not "MEDIA"
	CurrDirName = info->name;
	CurrDirName[8] = 0;

	DIR_Change("\\DCIM"); 
	DIR_Remove(CurrDirName); 
	DIR_Change("..");
	dcfDirInitialize();	
#endif
}

UINT8 dcfMediaFolderAssign(UINT8 en, UINT16 folderIdx)
{
#if 1
	FDBMediaFolderInfo_t info;
    UINT8 dirName[9];
	UINT16 dirCountSv;
	UINT8 sts = SUCCESS;

	G_USR_bUsrAssignFolder = en;
	if(en && folderIdx >= 100 && folderIdx <= 999 ){		
		G_USR_UsrAssignFolderCount = folderIdx;
		dirCountSv = G_USR_Dir1Count;
		
	    //goto assign folder
	    if(G_USR_Dir1Count != folderIdx){//not current dir
			USR_GetMediaFolderInfo(folderIdx, &info);
			if(info.name[0] == 0){ //folder not existed
				dcfDir1NameGet(dirName, folderIdx,NORMAL_SNAPFOLDER);
				sts = DIR_Change(".."); 
				sts = DIR_Create(dirName); 
				sts = DIR_Change(dirName);
				if(sts != DOS_SUCCESS){
					sts = FAIL;
					ASSERT(0, 1);
				}
				else{
					USR_UpdateMediaFolderInfo();
					dcfFileInitialize();
				}
			}
			else{
				do{
					dcfFolderNext(1);		
				}while(G_USR_Dir1Count != folderIdx && dirCountSv != G_USR_Dir1Count);
				if(G_USR_Dir1Count == folderIdx){//found
					dcfFileInitialize();
				}
				else{
					sts = FAIL;
					ASSERT(0, 1);
				}
			}
			
	    }
	}

	if(sts == FAIL){
		G_USR_bUsrAssignFolder = 0;
	}

	return sts;
#endif
}

/*
UINT8 dcfBaseNameSet(UINT8 * fileName)
{
	UINT8 i;
	for ( i = 0; i < 12; i++ ){
		G_USR_Company_UpperSIG[i] = fileName[i];
	}	
	//printf("fileName=%s\n",G_USR_Company_UpperSIG);  
	return SUCCESS;
}
*/

UINT8 dcfDir1NameSet(TLSMod_e opt,UINT8 *dirName)
{
	UINT8 i;
	switch(opt){
		case NORMAL_SNAPFOLDER:
			for ( i = 3; i < 8; i++ ){
				G_USR_Dir1Name[i] = dirName[i];
			}
		break;
		case TIMELAPSE_VIDEOFOLDER:
			for ( i = 3; i < 8; i++ ){
				G_USR_TLSVIDDirName[i] = dirName[i];
			}
		break;
	}
	
	//printf("dirName=%s\n",G_USR_Dir1Name);  
	return SUCCESS;
}

void dcfDir1NameGet(UINT8* name, UINT16 dirIndex,UINT8 opt) USING_0
{
	UINT8 i;
	UINT8 ch;
	UINT16 rem = dirIndex;
	xdata UINT8 *dirName = NULL;
	
	switch(opt){
		case NORMAL_SNAPFOLDER:
			dirName = G_USR_Dir1Name;
		break;
		case TIMELAPSE_VIDEOFOLDER:
			dirName = G_USR_TLSVIDDirName;
		break;
	}
	
	for ( i = 0; i < 12; i++ ){
		name[i]  = dirName[i];
	}
	for (i = 2; i != 0xff; i--){
	    ch = (UINT8) (rem % 10) + '0';
	    rem /= 10;
	    name[i] = ch;
	}
}

UINT8 dcfTLSAssignFolder()
{
    UINT8 dirName[12];
	UINT8 sts = SUCCESS;
	FDBMediaFolderInfo_t info;
	UINT16 folderIdx;

//	printf("n=%u,t=%u\n",G_USR_MaxNormalDirCnt,G_USR_MaxTlsVidDirCnt);
	if(G_USR_MaxTlsVidDirCnt == 0){
		folderIdx = G_USR_MaxNormalDirCnt + 1;
	}
	else{
		folderIdx = G_USR_MaxTlsVidDirCnt;
	}

//	printf("folder=%u\n",folderIdx);
	if(folderIdx >= 100 && folderIdx <= 999){
		USR_GetMediaFolderInfo(folderIdx, &info);
		dcfDir1NameGet(dirName, folderIdx,TIMELAPSE_VIDEOFOLDER);
		if(info.name[0] == 0){ //folder not existed
			sts = DIR_Change(".."); 
			sts = DIR_Create(dirName); 
			sts = DIR_Change(dirName);
			if(sts != DOS_SUCCESS){
				return FAIL;
			}	
		}
		else{
			sts = DIR_Change(".."); 
			sts = DIR_Change(dirName); 

			if(sts != DOS_SUCCESS){
				printf("Line=%d\n",__LINE__);
			 	return FAIL;
			}
			 
		}
		USR_UpdateMediaFolderInfo();
		dcfFileInitialize();
	}
	else{
		sts = FAIL;
	}
	
	return sts;
}

void dcfQuickInitSet(UINT8 enable)
{

	dcfQuickInit = enable?1:0;
}

UINT8 dcfQuickInitStsGet(void)
{

	return dcfQuickInit;

}

//xian ++, 20120105,
static void dcfFileDramFdbUpdate(	UINT8 fno)
{
	xdata FILE *file;
	xdata x86_FDB *fdb;//, tmpFdb;
	UINT8 fileType = 0;
	UINT8 i8;
	UINT16 index = 0;

	file = (FILE *)&dos.file[fno];
	fdb  = (x86_FDB *)&file->fdb;

	//add for change the file attribute and name 
	fileType = USR_FileExist((UINT8 *)fdb);
	
	G_DOS_FdbAddress = ((UINT32)file->FDB_Sector << 4) + ((UINT32)file->FDB_Idx);

	//printf("name:%s\n",fdb->name);
	for (i8 = 4; i8 < 8; i8++)
	{
		index = index * 10 + (fdb->name[i8] - 0x30);
	}
	//printf("idx:%u\n",index);
	//if( G_USR_SearchNowFileIndex == 0 )
	//{
		G_USR_SearchNowFileIndex = index;
	//}
    HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10),K_SDRAM_NoPrefetch);
	//printf("Vfs ADDR=%lx\n",(K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 10));
	//printf("now idx:%u\n",G_USR_SearchNowFileIndex);
    //HAL_DramStartAddrSet((K_SDRAM_AVI_FdbParameterBufAddr + (((UINT32)G_USR_SearchNowFileIndex) - 1) * 8),K_SDRAM_NoPrefetch);
	//patch4.5@ada@Record DCF file name end
	
    //Record file FDB logical address
	HAL_DramWordDataWrite((UINT8)(G_DOS_FdbAddress & 0x000000ff),(UINT8)((G_DOS_FdbAddress & 0x0000ff00) >> 8));
	HAL_DramWordDataWrite((UINT8)((G_DOS_FdbAddress & 0x00ff0000) >> 16),(UINT8)((G_DOS_FdbAddress & 0xff000000) >> 24));
    //Record file attribute and file type
   	HAL_DramWordDataWrite(fdb->attr,fileType);
	//printf("attr=%bx,fileType=%bx\n",fdb->attr,fileType);
    //Record file starting cluster
  	HAL_DramWordDataWrite(fdb->start[0],fdb->start[1]);
    //Record file size in bytes
 	HAL_DramWordDataWrite(fdb->size[0],fdb->size[1]);
	HAL_DramWordDataWrite(fdb->size[2],fdb->size[3]);

	//patch4.5@ada@Record DCF file name begin
	//Record DCF file name
	for (i8 = 0; i8 < 2; i8++)
	{
       	HAL_DramWordDataWrite(fdb->name[i8 * 2],fdb->name[i8 * 2 + 1]);
	}
}

/**
 * @fn        UINT8 dcfFileAttrSet(UINT8 const* oldFileName, UINT8 const* newFileName, UINT8 attr, UINT8 option)
 * @brief     Set the attribute of a file.
 * @param     [in] oldFileName	Specify the old filename.
 * @param     [in] newFileName	Specify the new filename if option is ATTR_MODIFY_NAME.
 * @param     [in] attr			Specify the new attribute value. It can be:\n
 *\n				ATTR_R :	read only
 *\n				ATTR_H : hidden
 *\n				ATTR_S : system		
 *\n				ATTR_L : label
 *\n				ATTR_D : dir
 *\n				ATTR_A : archieve
 *\n				ATTR_LFN : ATTR_R+ATTR_H+ATTR_S+ATTR_L
 * @param     [in] option		Operation option. It can be ATTR_MODIFY_NAME, ATTR_MODIFY_ATTR and ATTR_MODIFY_SIZE.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
*/
UINT8 dcfFileAttrSet(
	UINT8 const* oldFileName,
	UINT8 const* newFileName,
	UINT8 attr,
	UINT8 option
)
{
	UINT8 fHdl;
	UINT8 ret = SUCCESS;
//	xdata FILE *file;
//	xdata x86_FDB *fdb;
	bgRwAbort();
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
			ret |= vfsFileAttrSet(fHdl, ATTR_MODIFY_NAME, (void*)newFileName);
		}

		if(ret == SUCCESS){ //xian ++ 20120105
			dcfFileDramFdbUpdate(fHdl);
		}
	}

	/* modify file attrib */
	if (option & ATTR_MODIFY_ATTR) {
		ret |= vfsFileAttrSet(fHdl, ATTR_MODIFY_ATTR, (void*)&attr);

		if(ret == SUCCESS){ //xian ++ 20120105
			dcfFileDramFdbUpdate(fHdl);
		}
	}

	//xian --, 20120105
	//file = (FILE *)&dos.file[fHdl];
	//fdb  = (x86_FDB *)&file->fdb;

	DOS_FileClose(fHdl);
	 bgRwResume();
	return ret;
}

void dcfMmsFileNameGet(UINT8 *fileName,UINT16 fileIndx)
{
	UINT8 i;
	UINT8 ch;
	UINT32 tmp;

	tmp = G_USR_Dir1Count;
	for ( i = 2; i!=0xff; i-- ){
		ch = (UINT8) (tmp % 10) + '0';
		tmp /= 10;
		fileName[i]  = ch;
	}
	fileName[3] = '_';
	tmp = fileIndx;
	for (i = 7; i != 3; i--){
	    ch = (UINT8) (tmp % 10) + '0';
	    tmp /= 10;
	    fileName[i] = ch;
	}
	fileName[8] = '.';
	fileName[9] = 'J';
	fileName[10] = 'P';
	fileName[11] = 'G';
}

UINT8 dcfEntryMmsDir()
{
	UINT8 sts;
	UINT8 *dirName ="THUMBJPG";
	
	DIR_Change(".."); 
	sts = DIR_Change(dirName);
	if(sts != DOS_SUCCESS){
		sts = DIR_Create(dirName); 
		sts = DIR_Change(dirName);
	}
	
	return SUCCESS;
}

void dcfExitMmsDir()
{
	UINT8 dir1Name[12];
	UINT8 ret;
	
	DIR_Change("..");  
	dcfDir1NameGet(dir1Name, G_USR_Dir1Count,NORMAL_SNAPFOLDER);
	ret = DIR_Change(dir1Name);
	
	if(ret!= DOS_SUCCESS){
		printf("name=%s\n",dir1Name);
	}	
}
