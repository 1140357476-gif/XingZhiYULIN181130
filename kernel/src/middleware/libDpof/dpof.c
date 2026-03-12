/*++

Copyright (c) 2001 Sunplus Technology Co., Ltd.

Module Name:

        dpof.c

Abstract:

        Module related to chip initialization

Environment:

        Keil C51 Compiler

Revision History:

        04/01/2002      Leeada    created

--*/

//=============================================================================
//Header file
//=============================================================================
//patch3.2@ada@0401 For DPOF
#include "general.h"

#if (APP_DPOF_INIT)	/*Add by Phil@07.10.02*/
#include "dpof.h"
#include "dpofcon.h"
#include "doslink.h"
//#include "dosusr.h"
#include "doserr.h"
#include "initio.h"
#include "main.h"
#include "sdramhw.h"
//#include "cardfunc.h"
//#include "cardcon.h"
#include "cardlink.h"
//#include "exifhdr.h"
#include "cardui.h"
#include "dcf_api.h"

//=============================================================================
//Symbol
//=============================================================================

//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
     
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
UINT16 G_DPOF_Dir0Cluster;
UINT32 PreviousDpofFileDramAddr;
UINT32 PreviousDpofFileByteCount;
UINT32 NewDpofFileDramAddr;
UINT32 NewDpofFileByteCount;
UINT16 NewDpofJobSectionPRTPID;
UINT16 TotalSunplusDpofFileNumber;
UINT8 Dpof_File_Modify;
UINT8 Need_Build_DpofFloder;
UINT8 Other_DpofJobSection_Exist;

#if (APP_DPOF_INIT)	/*Add by Phil@07.10.02*/
//patch4.5@ada@Fix DPOF error begin
//UINT8 DpofDir1Name[8];
UINT8 DpofDir1Name[9];
//patch4.5@ada@Fix DPOF error end
#endif /*APP_DPOF_INIT*/
extern xdata	dateStc_t	G_DATEINFO;
//UINT8 DpofFileName[4];

//=============================================================================
//Program
//=============================================================================

//-----------------------------------------------------------------------------
//DPOF_SetDpofDir0Name
//-----------------------------------------------------------------------------
void DPOF_SetDpofDir0Name(UINT8* name) USING_0
{
     UINT8 i;

	for (i = 0; i < 12; i++)
     {          
    	     name[i]  = DPOF_Dir0Name[i];
     }
}

//-----------------------------------------------------------------------------
//DPOF_SetDpofFileName
//-----------------------------------------------------------------------------
void DPOF_SetDpofFileName(UINT8* name) USING_0
{
     UINT8 i;

     for (i = 0; i < 12; i++)
     {
	    name[i] = DPOF_FileName[i];
     }
}

//-----------------------------------------------------------------------------
//DPOF_ClearDramDpofArea
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for DPOF
void DPOF_ClearDramDpofArea(void) USING_0
{
     HAL_GprmDramFill(K_SDRAM_DPOF1_BufAddr,(UINT16)(K_SDRAM_DPOF1_BufSize / 2),0x00);
     HAL_GprmDramFill(K_SDRAM_DPOF1_BufAddr + (K_SDRAM_DPOF1_BufSize / 2),(UINT16)(K_SDRAM_DPOF1_BufSize / 2),0x00);
     //HAL_GprmDramFill(K_SDRAM_DPOF2_BufAddr,(UINT16)K_SDRAM_DPOF2_BufSize,0x00);
     HAL_GprmDramFill(K_SDRAM_DPOF3_BufAddr,(UINT16)(K_SDRAM_DPOF3_BufSize / 2),0x00);
     HAL_GprmDramFill(K_SDRAM_DPOF3_BufAddr + (K_SDRAM_DPOF3_BufSize / 2),(UINT16)(K_SDRAM_DPOF3_BufSize / 2),0x00);
     HAL_GprmDramFill(K_SDRAM_DPOF4_BufAddr,(UINT16)(K_SDRAM_DPOF4_BufSize / 2),0x00);
     HAL_GprmDramFill(K_SDRAM_DPOF4_BufAddr + (K_SDRAM_DPOF4_BufSize / 2),(UINT16)(K_SDRAM_DPOF4_BufSize / 2),0x00);
}

//-----------------------------------------------------------------------------
//DPOF_GetDpofDir1Name
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for DPOF
void DPOF_GetDpofDir1Name(void) USING_0
{
     UINT8 tmp1;

     //version4.0@ada@0513 for Multi-Directory
     #if (SINGLE_DIRECTORY == 0)
     UINT8 i,tmp2;
     #endif

     #if (SINGLE_DIRECTORY)
     tmp1 = (UINT8)(G_USR_Dir1Count / 100);
     DpofDir1Name[0] = tmp1 + '0';
     DpofDir1Name[1] = (UINT8)((G_USR_Dir1Count % 100) / 10) + '0';
     DpofDir1Name[2] = (UINT8)(G_USR_Dir1Count % 10) + '0';

     //patch4.4@ada@DPOF begin
     DpofDir1Name[3] = C_USR_Dir0Name[0];
     DpofDir1Name[4] = C_USR_Dir0Name[1];
     DpofDir1Name[5] = C_USR_Dir0Name[2];
     DpofDir1Name[6] = C_USR_Dir0Name[3];
     DpofDir1Name[7] = C_USR_Dir0Name[4];

     /*
     DpofDir1Name[3] = 'M';
     DpofDir1Name[4] = 'E';
     DpofDir1Name[5] = 'D';
     DpofDir1Name[6] = 'I';
     DpofDir1Name[7] = 'A';
     */
     //patch4.4@ada@DPOF end
     
     #else
     //patch4.4@ada@DirectoryIndexOrder begin
     //HAL_DramStartAddrSet((K_SDRAM_DcfDirBufAddr + ((UINT32)G_USR_Dir1CountIndex - 1) * 7 + 3),K_SDRAM_Prefetch);               
     HAL_DramStartAddrSet((K_SDRAM_DcfDirBufAddr + ((UINT32)G_USR_Dir1Count - 100) * 8 + 2),K_SDRAM_Prefetch);               
     //patch4.4@ada@DirectoryIndexOrder end

     for (i = 0; i < 8; i += 2)
     {
          HAL_DramWordDataRead(&tmp1,&tmp2);     
	     DpofDir1Name[i] = tmp1;
	     DpofDir1Name[i + 1] = tmp2;
          //printf("DpofDir1Name[%bd] = %c\n",i,DpofDir1Name[i]);
          //printf("DpofDir1Name[%bd] = %c\n",i + 1,DpofDir1Name[i + 1]);
     }
     #endif
}

//-----------------------------------------------------------------------------
//DPOF_DirInitialize
//-----------------------------------------------------------------------------
UINT8 DPOF_DirInitialize(void) USING_0
{
     UINT8 sts = TRUE;
     UINT8 name[12];
     UINT16 startCluster;

     //printf("DPOF initialize\n");

     NewDpofJobSectionPRTPID = 1;
     TotalSunplusDpofFileNumber = 0;
     Dpof_File_Modify = 0;
     PreviousDpofFileByteCount = 0;
     PreviousDpofFileDramAddr = 0;
     NewDpofFileByteCount = (UINT32)K_DPOF_HDR_SIZE;
     NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
     Need_Build_DpofFloder = 0;
     Other_DpofJobSection_Exist = 0;

     DPOF_SetDpofDir0Name(name);

     DPOF_GetDpofDir1Name();

     DPOF_CheckJpgFileExist();

     if ((startCluster = DOS_RootFindName(name)) > 0)
     {
          //printf("MISC is found\n");
          G_DPOF_Dir0Cluster = startCluster;

          DOS_SetCurrDir(startCluster);

          DPOF_FileInitialize();

          //Back to .\DCIM\******** folder
          DOS_SetCurrDir(G_USR_Dir1Cluster);
     }
     else
     {
          Need_Build_DpofFloder = 1;
     }

     return sts;
}

//patch4.4@ada@DPOF begin
//-----------------------------------------------------------------------------
//DPOF_FileInitialize
//-----------------------------------------------------------------------------
void DPOF_CheckJpgFileExist(void) USING_0
{
     //patch4.4@ada@DPOF begin
     UINT8 tmp1,tmp2;
     UINT16 NowFileIndex;
     //patch4.4@ada@DPOF end

     DPOF_ClearDramDpofArea();

     NowFileIndex = G_USR_FirstFileIndex;

     //printf("G_USR_FirstFileIndex = %d\n",G_USR_FirstFileIndex);
     //printf("G_USR_FileCount = %d\n",G_USR_FileCount);

     if (dcfFileCountGet() > 0)
     {
          do
          {  
			//patch4.5@ada@Record DCF file name begin
               HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)NowFileIndex) - 1) * 10 + 2),K_SDRAM_Prefetch);         
               //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)NowFileIndex) - 1) * 8 + 2),K_SDRAM_Prefetch);         
			//patch4.5@ada@Record DCF file name end

               HAL_DramWordDataRead(&tmp1,&tmp2);           
               //printf("tmp1 = %bd, tmp2 = %bd\n",tmp1,tmp2);

               //check file type
               if (tmp2 == K_DCF_JPG_TYPE)
               {
                    //printf("File %d exist\n",NowFileIndex);
                    HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + (((UINT32)NowFileIndex) - 1) * 3),K_SDRAM_NoPrefetch);         
                    HAL_DramWordDataWrite(0xff,0x00);
               }

			//patch4.5@ada@Record DCF file name begin
               HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)NowFileIndex) - 1) * 10 + 9),K_SDRAM_Prefetch);         
               //HAL_DramStartAddrSet((K_SDRAM_JPG_FdbParameterBufAddr + (((UINT32)NowFileIndex) - 1) * 8 + 7),K_SDRAM_Prefetch);         
			//patch4.5@ada@Record DCF file name end

               HAL_DramWordDataRead(&tmp1,&tmp2);              
               NowFileIndex = (UINT16)tmp1 | (((UINT16)tmp2) << 8);

          } while (NowFileIndex > 0);
     }
}
//patch4.4@ada@DPOF end

//-----------------------------------------------------------------------------
//DPOF_FileInitialize
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for DCF
UINT8 DPOF_FileInitialize(void) USING_0    
{
     UINT8 sts = TRUE;
     UINT16 fHandle;

	M_DOS_ClearError();

     //If AUTPRINT.MRK file exists,then temprarily put it to
     //DRAM address K_SDRAM_DPOF1_BufAddr(K_SDRAM_DPOF1_BufSize words range)
     //If AUTPRINT.MRK file does not exist,then create one.
     //and temperarily read to DRAM address K_SDRAM_DPOF1_BufAddr

     if (DPOF_OpenDpofFile(DPOF_FileName,&fHandle,0) == TRUE)
     {
          //printf("Open the DPOF file\n");
          DPOF_RecordDpofJobSection();
     }

	 //patch5.1@ada@Fix the zero file size problem begin
	 else
	 {
	 	  sts = FALSE;
		  return sts;
	 }
	 //patch5.1@ada@Fix the zero file size problem end

     if (fHandle)
     {
          vfsFileClose(fHandle, DPOF_FileName, K_DOS_ReadFile, 0);
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_OpenDpofFile
//-----------------------------------------------------------------------------
UINT8 DPOF_OpenDpofFile(UINT8 *name,UINT16 *fHandle,UINT16 startCluster) USING_0
{
     UINT8 sts = FALSE;
	 UINT32 filesize;

     *fHandle = vfsFileOpen(name,K_DOS_ReadFile,startCluster);
	 filesize = vfsFileSizeGet(*fHandle);

     //Only 256k bytes DRAM area is reserved for loading AUTPRINT.MRK

	//patch5.1@ada@Fix the zero file size problem begin
     if ((*fHandle) && (filesize <= 0x00040000) && (filesize > 0))
     //if ((*fHandle) & (vfsFileSizeGet() <= 0x00040000))
	//patch5.1@ada@Fix the zero file size problem end
     {
          sts = vfsFileRead(*fHandle,K_DOS_DstDram,filesize,K_SDRAM_DPOF1_BufAddr,0);   
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileHeaderSection
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For Date & Time Update
void DPOF_WriteDpofFileHeaderSection(void) USING_0 
{
     UINT8 i;
     UINT8 tmp1,tmp2;

     //Load Sunplus default components of DPOF file header section,including GEN_REV,GEN_CRT,GEN_DTM
     for (i = 0; i < K_DPOF_HDR_SIZE; i++)
     {
          G_ucStorData[i] = DPOF_HeaderSectionContent[i];
     }

     //ada@0401 For Date & Time Update
     dcfRtcTimeUpdate();

     HAL_GlobalRTCTransferText(&G_DATEINFO,&G_ucStorData[K_DPOF_HDR_GEN_DTM_START_INDEX]);

     HAL_DramStartAddrSet(K_SDRAM_DPOF4_BufAddr,K_SDRAM_NoPrefetch);
     
     for (i = 0; i < (K_DPOF_HDR_SIZE >> 1); i++)
     {
          HAL_DramWordDataWrite(G_ucStorData[i * 2],G_ucStorData[i * 2 + 1]);
     }

     if (K_DPOF_HDR_SIZE & 0x0001)
     {
          HAL_DramStartAddrSet((K_SDRAM_DPOF4_BufAddr + (K_DPOF_HDR_SIZE >> 1)),K_SDRAM_Prefetch);

          HAL_DramWordDataRead(&tmp1,&tmp2);

          HAL_DramStartAddrSet((K_SDRAM_DPOF4_BufAddr + (K_DPOF_HDR_SIZE >> 1)),K_SDRAM_NoPrefetch);

          HAL_DramWordDataWrite(G_ucStorData[K_DPOF_HDR_SIZE - 1],tmp2);
     }
}

//-----------------------------------------------------------------------------
//DPOF_RecordDpofJobSection
//-----------------------------------------------------------------------------
UINT8 DPOF_RecordDpofJobSection(void) USING_0
{
     UINT32 NewDramAddr;
     UINT32 ByteCount;
     UINT8 sts = TRUE;
     UINT32 JobSection_StartDramAddr = 0;
     UINT32 JobSection_StartByteCount = 0;
     UINT32 JobSection_EndDramAddr = 0;
     UINT32 JobSection_EndByteCount = 0;
     UINT32 JobSection_Length = 0;
     UINT32 JobSection_TmpDramAddr = 0;
     UINT32 JobSection_TmpByteCount = 0;

     UINT16 PrtPid;
     UINT8 PrtTyp;
     UINT16 PrtQty;
     UINT8 ImgFmt;
     UINT16 ImgSrcIdx;
     
     UINT32 TmpByteCount;
     UINT32 DpofTmpDramAddr;
     UINT8 tmp1,tmp2,tmp3,tmp4;

     NewDramAddr = K_SDRAM_DPOF1_BufAddr;
     ByteCount = 0;

     while ((ByteCount < (vfsFileSizeGet(0) - 2)) && (sts == TRUE))
     {           
          if (DPOF_FindDpofJobSection(&NewDramAddr,&ByteCount,&JobSection_StartDramAddr,&JobSection_StartByteCount))
          {
               JobSection_TmpDramAddr = NewDramAddr;  //Backup
               JobSection_TmpByteCount = ByteCount;

               PrtPid = 0;
               PrtTyp = 0;
               PrtQty = 0;
               ImgFmt = 0;
               ImgSrcIdx = 0;

               //This job section is created by this camera
               if (DPOF_FindDpofIMGSRC(&NewDramAddr,&ByteCount,&ImgSrcIdx))  
               {
                    //printf("TotalSunplusDpofFileNumber + 1\n");
                    TotalSunplusDpofFileNumber += 1;
                    NewDramAddr = JobSection_TmpDramAddr;   //Restore
                    ByteCount = JobSection_TmpByteCount;
               
                    if (DPOF_FindDpofPRTPID(&NewDramAddr,&ByteCount,&PrtPid))
                    {
                         if (DPOF_FindDpofPRTTYP(&NewDramAddr,&ByteCount,&PrtTyp))
                         {
                              if (DPOF_FindDpofPRTQTY(&NewDramAddr,&ByteCount,&PrtQty))
                              {
                                   if (DPOF_FindDpofIMGFMT(&NewDramAddr,&ByteCount,&ImgFmt))
                                   {
										//patch4.5@ada@Not necessary call begin
                                        //DPOF_FindDpofIMGSRC(&NewDramAddr,&ByteCount,&ImgSrcIdx);
										//patch4.5@ada@Not necessary call end

                                        if (DPOF_CheckDpofJobFileExist(ImgSrcIdx))     
                                        {
                                             DPOF_RecordDpofJobParameter(ImgSrcIdx,/*PrtPid,*/PrtTyp,PrtQty,ImgFmt);
                                        }
                                        //version4.0@ada@0513 for DPOF
                                        else
                                        {
                                             Dpof_File_Modify = 1;
                                        }
                                   }
                                   else
                                   {
                                        sts = FALSE;
                                   }
                              }
                              else
                              {
                                   sts = FALSE;
                              }
                         }
                         else
                         {
                              sts = FALSE;
                         }
                    }
                    else
                    {
                         sts = FALSE;
                    }
               }
               else //This job section is created by other camera
               {
                    NewDramAddr = JobSection_TmpDramAddr;   //Restore
                    ByteCount = JobSection_TmpByteCount;

                    DPOF_ModifyDpofPRTPID(NewDramAddr,ByteCount);

                    //Copy area between two JOB heads
                    if (DPOF_FindDpofJobSection(&NewDramAddr,&ByteCount,&JobSection_EndDramAddr,&JobSection_EndByteCount))
                    {
                         JobSection_Length = (JobSection_EndByteCount - JobSection_StartByteCount);

                         DPOF_CopyJobSection(JobSection_StartDramAddr,JobSection_Length,(JobSection_StartByteCount - 1));

                         NewDramAddr = JobSection_TmpDramAddr; //Restore 
                         ByteCount = JobSection_TmpByteCount;
                    }
                    else //Copy area from this JOB head to the end of the file
                    {
                         JobSection_Length = ((vfsFileSizeGet(0) + 1) - JobSection_StartByteCount);
                         
                         DPOF_CopyJobSection(JobSection_StartDramAddr,JobSection_Length,(JobSection_StartByteCount - 1));                         

                         TmpByteCount = (NewDpofFileByteCount - 2);
                         DpofTmpDramAddr = K_SDRAM_DPOF4_BufAddr + (TmpByteCount >> 1);

                         if (TmpByteCount & 0x00000001)
                         {         
                              HAL_DramStartAddrSet(DpofTmpDramAddr,K_SDRAM_Prefetch);
                              HAL_DramWordDataRead(&tmp1,&tmp2);
                              HAL_DramWordDataRead(&tmp3,&tmp4);

                              if ((tmp2 != 0x0d) && (tmp3 != 0x0a))   //File ended with no 0x0d & 0x0a
                              {          
                                   NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);

                                   HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_Prefetch);
                                   HAL_DramWordDataRead(&tmp1,&tmp2);

                                   HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                                   HAL_DramWordDataWrite(tmp1,0x0d);
                                   HAL_DramWordDataWrite(0x0a,0x00);

                                   NewDpofFileByteCount += 2;
                                   NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
                              }
                         }
                         else
                         {
                              HAL_DramStartAddrSet(DpofTmpDramAddr,K_SDRAM_Prefetch);
                              HAL_DramWordDataRead(&tmp1,&tmp2);

                              if ((tmp1 != 0x0d) && (tmp2 != 0x0a))
                              {    
                                   NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);

                                   HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                                   HAL_DramWordDataWrite(0x0d,0x0a);

                                   NewDpofFileByteCount += 2;
                                   NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
                              }
                         }

                         break;
                    }
               }
          }
     }

     if (Other_DpofJobSection_Exist)
     {
          PreviousDpofFileByteCount = NewDpofFileByteCount;
          PreviousDpofFileDramAddr = NewDpofFileDramAddr;
     }

     return sts;                    
}

//-----------------------------------------------------------------------------
//DPOF_WriteSunplusImageField
//-----------------------------------------------------------------------------
void DPOF_WriteSunplusImageField(void) USING_0
{
     UINT8 PrtTyp;
     UINT16 PrtQty;
     UINT8 ImgFmt;
     UINT16 ImgSrcIdx;
     UINT16 i;
     UINT8 tmp1,tmp2,tmp3,tmp4;
     UINT32 TmpByteCount;
     UINT32 DpofTmpDramAddr;

     TmpByteCount = NewDpofFileByteCount;
     DpofTmpDramAddr = NewDpofFileDramAddr;

     TmpByteCount -= 4;
     DpofTmpDramAddr = K_SDRAM_DPOF4_BufAddr + (TmpByteCount >> 1);

     if (TmpByteCount & 0x00000001)
     {         
          HAL_DramStartAddrSet(DpofTmpDramAddr,K_SDRAM_Prefetch);
          HAL_DramWordDataRead(&tmp1,&tmp2);
          HAL_DramWordDataRead(&tmp3,&tmp4);

          if ((tmp2 == 0x0d) && (tmp3 == 0x0a))
          {
               NewDpofFileByteCount -= 2;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);

               HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_Prefetch);
               HAL_DramWordDataRead(&tmp1,&tmp2);

               HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
               HAL_DramWordDataWrite(tmp1,0x00);
               HAL_DramWordDataWrite(0x00,0x00);
          }
     }
     else
     {
          HAL_DramStartAddrSet(DpofTmpDramAddr,K_SDRAM_Prefetch);
          HAL_DramWordDataRead(&tmp1,&tmp2);

          if ((tmp1 == 0x0d) && (tmp2 == 0x0a))
          {
               NewDpofFileByteCount -= 2;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
     
               HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
               HAL_DramWordDataWrite(0x00,0x00);
          }
     }

     i = TotalSunplusDpofFileNumber;

     //version4.0@ada@0513 for DPOF

	 //patch4.5@ada@Fix DPOF bug after delete all begin
     for (ImgSrcIdx = 1; ((ImgSrcIdx < 10000) && (TotalSunplusDpofFileNumber > 0)); ImgSrcIdx++)
     //for (ImgSrcIdx = 1; ImgSrcIdx < 10000; ImgSrcIdx++)
	 //patch4.5@ada@Fix DPOF bug after delete all end
     {
          if (DPOF_CheckDpofJobFieldExist(ImgSrcIdx) == TRUE) 
          {
               DPOF_ReadDpofJobParameter(ImgSrcIdx,/*&PrtPid,*/&PrtTyp,&PrtQty,&ImgFmt);

               DPOF_BuildOneDpofFileJobSection(ImgSrcIdx,PrtTyp,PrtQty,ImgFmt);
         
               i --;

               if (i == 0)
               {
                    break;
               }
          }
     }
}

//-----------------------------------------------------------------------------
//DPOF_BuildDpofFile
//-----------------------------------------------------------------------------
//patch3.2@ada@0401 For Date & Time Update
UINT8 DPOF_BuildDpofFile(void) USING_0
{
     UINT8 sts = TRUE;
     UINT8 name[12];               
     UINT16 startCluster;

     //printf("Dpof_File_Modify = %bx\n",Dpof_File_Modify);
     //printf("TotalSunplusDpofFileNumber = %x\n",TotalSunplusDpofFileNumber);
     //printf("Other_DpofJobSection_Exist = %bx\n",Other_DpofJobSection_Exist);

     if ((Dpof_File_Modify == 1) && ((TotalSunplusDpofFileNumber > 0) || (Other_DpofJobSection_Exist == 1))) //ada@0325
     {     
          //Make MISC folder if never exists
          if (Need_Build_DpofFloder)
          {
               DPOF_SetDpofDir0Name(name);

               //ada@0401 For Date & Time Update
               dcfRtcTimeUpdate();

               if ((startCluster = DOS_RootMakeDir(name, 1)) == 0)
               {
                    DOS_SetCurrDir(G_USR_Dir1Cluster);
             	     return FALSE;            
               }           

               Need_Build_DpofFloder = 0;
               G_DPOF_Dir0Cluster = startCluster;
          }

          if (Other_DpofJobSection_Exist)
          {
               NewDpofFileByteCount = PreviousDpofFileByteCount;
               NewDpofFileDramAddr = PreviousDpofFileDramAddr;
          }
          else
          {
               NewDpofFileByteCount = (UINT32)K_DPOF_HDR_SIZE;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
          }

          DPOF_WriteDpofFileHeaderSection();

          DPOF_WriteSunplusImageField();

          DPOF_SetDpofFileName(name);

          DOS_SetCurrDir(G_DPOF_Dir0Cluster);                                
          
          DPOF_DeleteFile(name);

          //printf("Make DPOF file\n");
	  //eric for shrink code
	dramFileSave(name,UINT32 K_SDRAM_DPOF4_BufAddr,NewDpofFileByteCount)

          DOS_SetCurrDir(G_USR_Dir1Cluster);
     }
     else
     {
          if ((TotalSunplusDpofFileNumber == 0) && (Other_DpofJobSection_Exist == 0) && (Dpof_File_Modify == 1))
          {
               // patch 5.2.1_32@mattwang@fix searching nonexisting 'MISC' dir beg
               if( !Need_Build_DpofFloder )  // 20040503 mattwang add to fix searching nonexisting 'MISC' dir
               {
                   DPOF_SetDpofFileName(name);

                   DOS_SetCurrDir(G_DPOF_Dir0Cluster);                                

                   //printf("No JOB section exist,so delete DPOF file\n");

                   DPOF_DeleteFile(name);

                   DOS_SetCurrDir(G_USR_Dir1Cluster);
               }
               // patch 5.2.1_32@mattwang@fix searching nonexisting 'MISC' dir end
          }
     }

     Dpof_File_Modify = 0;

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_DeleteFile
//-----------------------------------------------------------------------------
UINT8 DPOF_DeleteFile(UINT8 *name) USING_0
{
	UINT8 sts;

	G_Card_UI_Action = 0;
	sts = vfsFileDelete(name);

	G_Card_UI_Action = 0;

	return sts;
}

//-----------------------------------------------------------------------------
//DPOF_CheckDpofJobFieldExist
//-----------------------------------------------------------------------------
UINT8 DPOF_CheckDpofJobFieldExist(UINT16 ImgSrcIdx) USING_0
{
     UINT8 sts = FALSE;
     UINT8 tmp1,tmp2;

     HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + (((UINT32)ImgSrcIdx) - 1) * 3),K_SDRAM_Prefetch);

     HAL_DramWordDataRead(&tmp1,&tmp2);     

     if ((tmp1 == 0xff) && (tmp2 == 0xff))
     { 
          sts = TRUE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_CheckDpofJobFileExist
//-----------------------------------------------------------------------------
UINT8 DPOF_CheckDpofJobFileExist(UINT16 ImgSrcIdx) USING_0
{
     UINT8 sts = FALSE;
     UINT8 tmp1,tmp2;

     HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + ((UINT32)ImgSrcIdx - 1) * 3),K_SDRAM_Prefetch);

     HAL_DramWordDataRead(&tmp1,&tmp2);     

     if (tmp1 == 0xff)
     { 
          sts = TRUE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_RecordDpofJobParameter
//-----------------------------------------------------------------------------
void DPOF_RecordDpofJobParameter(UINT16 ImgSrcIdx,/*UINT16 PrtPid,*/UINT8 PrtTyp,UINT16 PrtQty,UINT8 ImgFmt) USING_0
{     
     //printf("Record file index %d\n",ImgSrcIdx);
     HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + ((UINT32)ImgSrcIdx - 1) * 3),K_SDRAM_NoPrefetch);

     HAL_DramWordDataWrite(0xff,0xff);     

     HAL_DramWordDataWrite(PrtTyp,ImgFmt);     

     HAL_DramWordDataWrite((UINT8)(PrtQty & 0x00ff),(UINT8)((PrtQty & 0xff00) >> 8));
}

//-----------------------------------------------------------------------------
//DPOF_EraseDpofJobParameter
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for DPOF
void DPOF_EraseDpofJobParameter(UINT16 ImgSrcIdx,UINT8 File_Still_Exist) USING_0
{
     UINT8 tmp1,tmp2;

     HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + ((UINT32)ImgSrcIdx - 1) * 3 + 1),K_SDRAM_Prefetch);
     HAL_DramWordDataRead(&tmp1,&tmp2);     

     HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + ((UINT32)ImgSrcIdx - 1) * 3),K_SDRAM_NoPrefetch);

     if (File_Still_Exist)
     {
          HAL_DramWordDataWrite(0xff,0x00);     
          HAL_DramWordDataWrite(0x00,tmp2);     
     }
     else
     {
          HAL_DramWordDataWrite(0x00,0x00);     
          HAL_DramWordDataWrite(0x00,0x00);     
     }     

     HAL_DramWordDataWrite(0x00,0x00);
}

//-----------------------------------------------------------------------------
//DPOF_ReadDpofJobParameter
//-----------------------------------------------------------------------------
UINT8 DPOF_ReadDpofJobParameter(UINT16 ImgSrcIdx,/*UINT16 *PrtPid,*/UINT8 *PrtTyp,UINT16 *PrtQty,UINT8 *ImgFmt) USING_0
{
     UINT8 sts = FALSE;
     UINT8 tmp1,tmp2;

     HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + ((UINT32)ImgSrcIdx - 1) * 3),K_SDRAM_Prefetch);

     HAL_DramWordDataRead(&tmp1,&tmp2);     

     if ((tmp1 == 0xff) && (tmp2 == 0xff))
     {
          //version4.0@ada@0513 for DPOF
          HAL_DramWordDataRead(&tmp1,&tmp2);     
          *PrtTyp = tmp1;
          *ImgFmt = tmp2;
          HAL_DramWordDataRead(&tmp1,&tmp2);  
          *PrtQty = ((UINT16)tmp1) | (((UINT16)tmp2) << 8);
          sts = TRUE;
     }
     else
     {
          *PrtTyp = 0;
          *ImgFmt = 0;
          *PrtQty = 0;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_SelectSunplusImage
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for DPOF
UINT8 DPOF_SelectSunplusImage(UINT16 ImgSrcIdx,/*UINT16 PrtPid,*/UINT8 PrtTyp,UINT16 PrtQty/*,UINT8 ImgFmt*/) USING_0
{
     UINT8 sts = FALSE;

     UINT8 tmp1,tmp2,tmp3;
     UINT8 ImgFmt;

     //patch4.4@ada@DPOF begin
     UINT16 Quantity;
     UINT8 tmp4,tmp5;
     //patch4.4@ada@DPOF end
     
     HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + (((UINT32)ImgSrcIdx) - 1) * 3),K_SDRAM_Prefetch);
     HAL_DramWordDataRead(&tmp1,&tmp2);   
     HAL_DramWordDataRead(&tmp3,&ImgFmt);   

     //patch4.4@ada@DPOF begin
     HAL_DramWordDataRead(&tmp4,&tmp5);  
     Quantity = ((UINT16)tmp4) | (((UINT16)tmp5) << 8);
     //patch4.4@ada@DPOF begin

     if ((TotalSunplusDpofFileNumber < 999) && (DPOF_CheckDpofJobFieldExist(ImgSrcIdx) == FALSE) && (tmp1 == 0xff) && (tmp2 == 0x00))
     {
          DPOF_RecordDpofJobParameter(ImgSrcIdx,/*PrtPid,*/PrtTyp,PrtQty,ImgFmt);

          Dpof_File_Modify = 1;

          //patch4.4@ada@DPOF begin
          if (Quantity == 0)
          {
               //printf("Select file %d\n",ImgSrcIdx);
               TotalSunplusDpofFileNumber++;
          }
          //patch4.4@ada@DPOF end

          sts = TRUE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_DeselectSunplusImage
//-----------------------------------------------------------------------------
UINT8 DPOF_DeselectSunplusImage(UINT16 ImgSrcIdx,UINT8 File_Still_Exist) USING_0
{
     UINT8 sts = FALSE;

     if ((TotalSunplusDpofFileNumber > 0) && (DPOF_CheckDpofJobFieldExist(ImgSrcIdx) == TRUE))
     {
          DPOF_EraseDpofJobParameter(ImgSrcIdx,File_Still_Exist);

          TotalSunplusDpofFileNumber--;
          Dpof_File_Modify = 1;         
          sts = TRUE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_ModifySunplusImagePRTQTY
//-----------------------------------------------------------------------------
UINT8 DPOF_ModifySunplusImagePRTQTY(UINT16 ImgSrcIdx,UINT16 PrtQty) USING_0
{
     UINT8 sts = FALSE;

     if ((TotalSunplusDpofFileNumber > 0) && (DPOF_CheckDpofJobFieldExist(ImgSrcIdx) == TRUE))
     {
          HAL_DramStartAddrSet((K_SDRAM_DPOF3_BufAddr + ((UINT32)ImgSrcIdx - 1) * 3 + 2),K_SDRAM_NoPrefetch);  

          HAL_DramWordDataWrite((UINT8)(PrtQty & 0x00ff),(UINT8)((PrtQty & 0xff00) >> 8));
          Dpof_File_Modify = 1;
          //printf("Modify PrtQty success\n");
          sts = TRUE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_CopyJobSection
//-----------------------------------------------------------------------------
void DPOF_CopyJobSection(UINT32 JobSection_StartDramAddr,UINT32 JobSection_Length,UINT32 ByteCount) USING_0
{
     UINT8 tmp1,tmp2,tmp3,tmp4;
     UINT32 k = 0;

     Other_DpofJobSection_Exist = 1;

     if (NewDpofFileByteCount & 0x00000001)
     {
          if (ByteCount & 0x00000001)
          {        
               //Copy the first byte tmp2
               HAL_DramStartAddrSet(JobSection_StartDramAddr,K_SDRAM_Prefetch);
               HAL_DramWordDataRead(&tmp1,&tmp2);

               HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_Prefetch);
               HAL_DramWordDataRead(&tmp3,&tmp4);

               HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
               HAL_DramWordDataWrite(tmp3,tmp2);

               //Next source address is wordboundary
               JobSection_StartDramAddr += 1; 
               JobSection_Length -= 1;

               //Next destination address is also wordboundary
               NewDpofFileByteCount += 1;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);

               //Copy the rest from wordboundary start-address area
               HAL_DoDRAMDMA((JobSection_StartDramAddr * 2),(NewDpofFileDramAddr * 2),(((JobSection_Length + 1) >> 1) << 1));
               
               NewDpofFileByteCount += JobSection_Length;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
          }
          else
          {
               if (JobSection_Length & 0x00000001)
               {
                    while ((JobSection_Length - 1) > 0)
                    {
                         //Copy the first byte tmp2
                         HAL_DramStartAddrSet(JobSection_StartDramAddr,K_SDRAM_Prefetch);
                         HAL_DramWordDataRead(&tmp1,&tmp2);
          
                         HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_Prefetch);
                         HAL_DramWordDataRead(&tmp3,&tmp4);
          
                         HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                         HAL_DramWordDataWrite(tmp3,tmp1);
                         HAL_DramWordDataWrite(tmp2,0x00);

                         //Next source address is not wordboundary
                         JobSection_StartDramAddr += 1; 
                         JobSection_Length -= 2;

                         //Next destination address is not wordboundary
                         NewDpofFileByteCount += 2;
                         NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
                    }

                    //Copy the last byte tmp1
                    HAL_DramStartAddrSet(JobSection_StartDramAddr,K_SDRAM_Prefetch);
                    HAL_DramWordDataRead(&tmp1,&tmp2);
          
                    HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_Prefetch);
                    HAL_DramWordDataRead(&tmp3,&tmp4);
         
                    HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                    HAL_DramWordDataWrite(tmp3,tmp1);

                    //Next destination address is wordboundary
                    NewDpofFileByteCount += 1;               
                    NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
               }
               else
               {
                    while (JobSection_Length > 0)
                    {
                         //Copy the first byte tmp2
                         HAL_DramStartAddrSet(JobSection_StartDramAddr,K_SDRAM_Prefetch);
                         HAL_DramWordDataRead(&tmp1,&tmp2);
          
                         HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_Prefetch);
                         HAL_DramWordDataRead(&tmp3,&tmp4);
          
                         HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                         HAL_DramWordDataWrite(tmp3,tmp1);
                         HAL_DramWordDataWrite(tmp2,0x00);

                         //Next source address is wordboundary
                         JobSection_StartDramAddr += 1; 
                         JobSection_Length -= 2;

                         //Next destination address is not wordboundary
                         NewDpofFileByteCount += 2;
                         NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
                    }
               }
          }
     }
     else
     {
          if (ByteCount & 0x00000001)
          {
               if (JobSection_Length & 0x00000001)
               {
                    while ((JobSection_Length - 1) > 0)
                    {
                         HAL_DramStartAddrSet(JobSection_StartDramAddr,K_SDRAM_Prefetch);
                         HAL_DramWordDataRead(&tmp1,&tmp2);
                         HAL_DramWordDataRead(&tmp3,&tmp4);         
          
                         HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                         HAL_DramWordDataWrite(tmp2,tmp3);

                         //Next source address is not wordboundary
                         JobSection_StartDramAddr += 1; 
                         JobSection_Length -= 2;

                         //Next destination address iswordboundary
                         NewDpofFileByteCount += 2;
                         NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
                    }

                    //Copy the last byte tmp1
                    HAL_DramStartAddrSet(JobSection_StartDramAddr,K_SDRAM_Prefetch);
                    HAL_DramWordDataRead(&tmp1,&tmp2);         
         
                    HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                    HAL_DramWordDataWrite(tmp2,0x00);

                    //Next destination address is not wordboundary
                    NewDpofFileByteCount += 1;               
                    NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
               }
               else
               {
                    while (JobSection_Length > 0)
                    {
                         //Copy the first byte tmp2
                         HAL_DramStartAddrSet(JobSection_StartDramAddr,K_SDRAM_Prefetch);
                         HAL_DramWordDataRead(&tmp1,&tmp2);
                         HAL_DramWordDataRead(&tmp3,&tmp4);
                    
                         HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                         HAL_DramWordDataWrite(tmp2,tmp3);

                         //Next source address is not wordboundary
                         JobSection_StartDramAddr += 1; 
                         JobSection_Length -= 2;

                         //Next destination address is wordboundary
                         NewDpofFileByteCount += 2;
                         NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
                    }
               }
          }
          else //SrcAddr & DestAddr are both wordboundary
          {
               HAL_DoDRAMDMA((JobSection_StartDramAddr * 2),(NewDpofFileDramAddr * 2),(((JobSection_Length + 1) >> 1) << 1));
               
               NewDpofFileByteCount += JobSection_Length;               
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
          }
     }
}

//-----------------------------------------------------------------------------
//DPOF_FindDpofJobSection
//-----------------------------------------------------------------------------
UINT8 DPOF_FindDpofJobSection(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT32 *JobSection_StartDramAddr,UINT32 *JobSection_StartByteCount) USING_0
{
     UINT16 i,j,k;
     BIT find_job = 0;
     UINT8 sts = TRUE;

     if ((*ByteCount < vfsFileSizeGet(0)) && (!find_job))
     {                    
          if (*ByteCount & 0x00000001)
          {
               k = 1;
          }
          else
          {
               k = 0;
          }

          SDRAM_Dram2Sram((*NewDramAddr),G_ucStorData,K_DPOF_SEARCH_SIZE);

          for (i = k; i < K_DPOF_SEARCH_SIZE; i++)
          {
               *ByteCount += 1;
               //Compare characters [JOB]
               // patch 5.2.1_30@mattwang@fix DPOF about deleting image and rebuilding DPOF error beg
               //if ((G_ucStorData[i] == DPOF_JobSectionTitle[0]) && (G_ucStorData[i + 1] == DPOF_JobSectionTitle[1]) && (G_ucStorData[i + 2] == DPOF_JobSectionTitle[2]) && (G_ucStorData[i + 3] == DPOF_JobSectionTitle[3]) && (G_ucStorData[i + 4] == DPOF_JobSectionTitle[4]))  // 20040316 mattwang mark
               if ((*ByteCount < vfsFileSizeGet(0)) && (G_ucStorData[i] == DPOF_JobSectionTitle[0]) && (G_ucStorData[i + 1] == DPOF_JobSectionTitle[1]) && (G_ucStorData[i + 2] == DPOF_JobSectionTitle[2]) && (G_ucStorData[i + 3] == DPOF_JobSectionTitle[3]) && (G_ucStorData[i + 4] == DPOF_JobSectionTitle[4]))  // 20040316 mattwang fix
               // patch 5.2.1_30@mattwang@fix DPOF about deleting image and rebuilding DPOF error end
               {
                    *JobSection_StartByteCount = *ByteCount;
                    *JobSection_StartDramAddr = K_SDRAM_DPOF1_BufAddr + ((*JobSection_StartByteCount - 1) >> 1);
                    *ByteCount += 4;
                    find_job = 1;

                    for (j = (i + 5); j < K_DPOF_SEARCH_SIZE; j++)
                    {                         
                         *ByteCount += 1;
                         //Compare line-feed and return
                         if ((G_ucStorData[j] == 0x0d) && (G_ucStorData[j + 1] == 0x0a))
                         {
                              *ByteCount += 1;
                              *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                              break;
                         }
                    }

                    break;
               }          
          }
     }

     if (!find_job)
     {
          sts = FALSE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_FindDpofIMGSRC
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for DPOF
UINT8 DPOF_FindDpofIMGSRC(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT16 *ImgSrcIdx) USING_0
{
     UINT16 i,k,l;
     UINT8 sts = FALSE;
     UINT8 tmp;
     UINT8 char1,char2,char3;

     if (*ByteCount < vfsFileSizeGet(0))
     {
          //Find the directory index
          tmp = (UINT8)(G_USR_Dir1Count / 100);
          char1 = tmp + '0';
          char2 = (UINT8)(G_USR_Dir1Count - (((UINT16)tmp) * 100)) / 10 + '0';
          char3 = (UINT8)(G_USR_Dir1Count % 10) + '0';

          if (*ByteCount & 0x00000001)
          {
               l = 1;
          }
          else
          {
               l = 0;
          }

          SDRAM_Dram2Sram(*NewDramAddr,G_ucStorData,K_DPOF_SEARCH_SIZE);

          for (i = l; i < K_DPOF_SEARCH_SIZE; i++)
          {
               *ByteCount += 1;
               //Compare with "IMG SRC"
               if ((G_ucStorData[i] == DPOF_ImgSrcStdSunpXXXXjpg[1]) && (G_ucStorData[i + 1] == DPOF_ImgSrcStdSunpXXXXjpg[2]) &&
                    (G_ucStorData[i + 2] == DPOF_ImgSrcStdSunpXXXXjpg[3]) && (G_ucStorData[i + 3] == DPOF_ImgSrcStdSunpXXXXjpg[4]) && 
                    (G_ucStorData[i + 4] == DPOF_ImgSrcStdSunpXXXXjpg[5]) && (G_ucStorData[i + 5] == DPOF_ImgSrcStdSunpXXXXjpg[6]) &&
                    (G_ucStorData[i + 6] == DPOF_ImgSrcStdSunpXXXXjpg[7]))
               {
                    //Compare "DCIM/xxxxxxxx"
                    //version4.0@ada@0513 for DPOF
                    if ((G_ucStorData[i + 14] == G_USR_Dir0Name[0]) && (G_ucStorData[i + 15] == G_USR_Dir0Name[1]) && (G_ucStorData[i + 16] == G_USR_Dir0Name[2]) && (G_ucStorData[i + 17] == G_USR_Dir0Name[3]) && (G_ucStorData[i + 18] == 0x2f) &&
                         //(G_ucStorData[i + 19] == G_USR_Dir1Name[0]) && (G_ucStorData[i + 20] == G_USR_Dir1Name[1]) && (G_ucStorData[i + 21] == G_USR_Dir1Name[2]) && (G_ucStorData[i + 22] == G_USR_Dir1Name[3]) &&
                         //(G_ucStorData[i + 19] == char1) && (G_ucStorData[i + 20] == char2) && (G_ucStorData[i + 21] == char3) && (G_ucStorData[i + 22] == G_USR_Dir1Name[3]) &&
                         //(G_ucStorData[i + 23] == G_USR_Dir1Name[4]) && (G_ucStorData[i + 24] == G_USR_Dir1Name[5]) && (G_ucStorData[i + 25] == G_USR_Dir1Name[6]) && (G_ucStorData[i + 26] == G_USR_Dir1Name[7]))
                         (G_ucStorData[i + 19] == DpofDir1Name[0]) && (G_ucStorData[i + 20] == DpofDir1Name[1]) && (G_ucStorData[i + 21] == DpofDir1Name[2]) && (G_ucStorData[i + 22] == DpofDir1Name[3]) &&
                         (G_ucStorData[i + 23] == DpofDir1Name[4]) && (G_ucStorData[i + 24] == DpofDir1Name[5]) && (G_ucStorData[i + 25] == DpofDir1Name[6]) && (G_ucStorData[i + 26] == DpofDir1Name[7]))
                    {                                   
                         *ImgSrcIdx = ((((UINT16)G_ucStorData[i + 32] - '0') * 1000) + (((UINT16)G_ucStorData[i + 33] - '0') * 100) + (((UINT16)G_ucStorData[i + 34] - '0') * 10) + (UINT16)G_ucStorData[i + 35] -'0');
                         *ByteCount += 35;   
                         sts = TRUE;

                         for (k = (i + 42); k < K_DPOF_SEARCH_SIZE; k++)
                         {                         
                              *ByteCount += 1;
                              //Compare line-feed and return
                              if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                              {
                                   *ByteCount += 7;
                                   *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                   break;
                              }
                         }
                    }

                    break;
               }
          }
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_FindDpofPRTPID
//-----------------------------------------------------------------------------
UINT8 DPOF_FindDpofPRTPID(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT16 *PrtPid) USING_0
{
     UINT16 i,j,k,l;
     UINT8 sts = TRUE;

     if ((*ByteCount < vfsFileSizeGet(0)) && (*PrtPid == 0))
     {
          if (*ByteCount & 0x00000001)
          {
               l = 1;
          }
          else
          {
               l = 0;
          }

          SDRAM_Dram2Sram(*NewDramAddr,G_ucStorData,K_DPOF_SEARCH_SIZE);

          for (i = l; i < K_DPOF_SEARCH_SIZE; i++)
          {
               *ByteCount += 1;

               if ((G_ucStorData[i] == 'P') && (G_ucStorData[i + 1] == 'R') && (G_ucStorData[i + 2] == 'T') && (G_ucStorData[i + 3] == ' ') && 
                    (G_ucStorData[i + 4] == 'P') && (G_ucStorData[i + 5] == 'I') && (G_ucStorData[i + 6] == 'D'))
               {
                    *ByteCount += 6;

                    for (j = (i + 7); j < K_DPOF_SEARCH_SIZE; j++)                        
                    {
                         *ByteCount += 1;
                              
                         if ((G_ucStorData[j] >= '0') && (G_ucStorData[j] <= '9'))
                         {                                   
                              *PrtPid = (((UINT16)(G_ucStorData[j] - '0') * 100) + ((UINT16)(G_ucStorData[j + 1] - '0') * 10) + ((UINT16)G_ucStorData[j + 2] - '0'));
                              *ByteCount += 2;

                              for (k = (j + 3); k < K_DPOF_SEARCH_SIZE; k++)
                              {                         
                                   *ByteCount += 1;
                                   //Compare line-feed and return
                                   if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                   {
                                        *ByteCount += 1;
                                        *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                        break;
                                   }
                              }

                              break;
                         }                              
                    }
               }

               break;             
          }
     }

     if (*PrtPid == 0)
     {
          sts = FALSE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_ModifyDpofPRTPID
//-----------------------------------------------------------------------------
UINT8 DPOF_ModifyDpofPRTPID(UINT32 NewDramAddr,UINT32 ByteCount) USING_0
{
     UINT16 i,j,l;
     UINT8 sts = TRUE;
     BIT PrtPid_Found = 0;

     if ((ByteCount < vfsFileSizeGet(0)) && (PrtPid_Found == 0))
     {
          if (ByteCount & 0x00000001)
          {
               l = 1;
          }
          else
          {
               l = 0;
          }

          SDRAM_Dram2Sram(NewDramAddr,G_ucStorData,K_DPOF_SEARCH_SIZE);

          for (i = l; i < K_DPOF_SEARCH_SIZE; i++)
          {
               ByteCount += 1;

               if ((G_ucStorData[i] == 'P') && (G_ucStorData[i + 1] == 'R') && (G_ucStorData[i + 2] == 'T') && (G_ucStorData[i + 3] == ' ') && 
                    (G_ucStorData[i + 4] == 'P') && (G_ucStorData[i + 5] == 'I') && (G_ucStorData[i + 6] == 'D'))
               {
                    ByteCount += 6;

                    for (j = (i + 7); j < K_DPOF_SEARCH_SIZE; j++)                        
                    {
                         ByteCount += 1;
                              
                         if ((G_ucStorData[j] >= '0') && (G_ucStorData[j] <= '9'))
                         {                   
                              PrtPid_Found = 1;                
                              G_ucStorData[j] = (UINT8)(NewDpofJobSectionPRTPID / 100);
                              G_ucStorData[j + 1] = (UINT8)((NewDpofJobSectionPRTPID - (((UINT16)G_ucStorData[j]) * 100)) / 10) + '0';
                              G_ucStorData[j] += '0';
                              G_ucStorData[j + 2] = (UINT8)(NewDpofJobSectionPRTPID % 10) + '0';

                              SDRAM_Sram2Dram(G_ucStorData,NewDramAddr,K_DPOF_SEARCH_SIZE);

                              NewDpofJobSectionPRTPID ++;

                              break;
                         }                              
                    }
               }

               break;             
          }
     }

     if (PrtPid_Found == 0)
     {
          sts = FALSE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_FindDpofPRTTYP
//-----------------------------------------------------------------------------
UINT8 DPOF_FindDpofPRTTYP(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT8 *PrtTyp) USING_0
{
     UINT16 i,j,k,l;
     UINT8 sts = TRUE;

     if ((*ByteCount < vfsFileSizeGet(0)) && (*PrtTyp == 0))
     {
          if (*ByteCount & 0x00000001)
          {
               l = 1;
          }
          else
          {
               l = 0;
          }

          SDRAM_Dram2Sram(*NewDramAddr,G_ucStorData,K_DPOF_SEARCH_SIZE);

          for (i = l; i < K_DPOF_SEARCH_SIZE; i++)
          {
               *ByteCount += 1;

               if ((G_ucStorData[i] == 'P') && (G_ucStorData[i + 1] == 'R') && (G_ucStorData[i + 2] == 'T') && (G_ucStorData[i + 3] == ' ') && 
                    (G_ucStorData[i + 4] == 'T') && (G_ucStorData[i + 5] == 'Y') && (G_ucStorData[i + 6] == 'P'))
               {
                    *ByteCount += 6;

                    for (j = (i + 7); j < K_DPOF_SEARCH_SIZE; j++)                        
                    {
                         *ByteCount += 1;
                       
                         if ((G_ucStorData[j] == 'S') && (G_ucStorData[j + 1] == 'T') && (G_ucStorData[j + 2] == 'D'))
                         {                                   
                              *PrtTyp = K_DPOF_STD;
                              //printf("Print type = %bx\n",K_DPOF_STD);
                              *ByteCount += 2;

                              for (k = (j + 3); k < K_DPOF_SEARCH_SIZE; k++)
                              {                         
                                   *ByteCount += 1;
                                   //Compare line-feed and return
                                   if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                   {
                                        *ByteCount += 1;
                                        *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                        break;
                                   }
                              }

                              break;
                         }
                         else
                         {
                              if ((G_ucStorData[j] == 'I') && (G_ucStorData[j + 1] == 'D') && (G_ucStorData[j + 2] == 'X'))
                              {                                   
                                   *PrtTyp = K_DPOF_IDX;    
                                   //printf("Print type = %bx\n",K_DPOF_IDX);
                                   *ByteCount += 2;

                                   for (k = (j + 3); k < K_DPOF_SEARCH_SIZE; k++)
                                   {                         
                                        *ByteCount += 1;
                                        //Compare line-feed and return
                                        if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                        {
                                             *ByteCount += 1;
                                             *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                             break;
                                        }
                                   }
     
                                   break;
                              }                                              
                         }
                    }

                    break;
               }
          }
     }

     if (*PrtTyp == 0)
     {
          sts = FALSE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_FindDpofPRTQTY
//-----------------------------------------------------------------------------
UINT8 DPOF_FindDpofPRTQTY(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT16 *PrtQty) USING_0
{
     UINT32 tmp_NewDramAddr;
     UINT32 tmp_ByteCount;
     UINT8 search_count = 4;  //4 * 512 = 2K Byte range
     UINT16 i,j,k,l;
     UINT8 sts = TRUE;
     
     tmp_NewDramAddr = *NewDramAddr;
     tmp_ByteCount = *ByteCount;

     if ((*ByteCount < vfsFileSizeGet(0)) && (*PrtQty == 0) && (search_count > 0))
     {
          if (*ByteCount & 0x00000001)
          {
               l = 1;
          }
          else
          {
               l = 0;
          }

          SDRAM_Dram2Sram(*NewDramAddr,G_ucStorData,K_DPOF_SEARCH_SIZE);

          for (i = l; i < K_DPOF_SEARCH_SIZE; i++)
          {
               *ByteCount += 1;

               if ((G_ucStorData[i] == 'P') && (G_ucStorData[i + 1] == 'R') && (G_ucStorData[i + 2] == 'T') && (G_ucStorData[i + 3] == ' ') && 
                    (G_ucStorData[i + 4] == 'Q') && (G_ucStorData[i + 5] == 'T') && (G_ucStorData[i + 6] == 'Y'))
               {
                    *ByteCount += 6;

                    for (j = (i + 7); j < K_DPOF_SEARCH_SIZE; j++)                        
                    {
                         *ByteCount += 1;
                              
                         if ((G_ucStorData[j] >= '0') && (G_ucStorData[j] <= '9'))
                         {                                   
                              *PrtQty = (((UINT16)(G_ucStorData[j] - '0') * 100) + ((UINT16)(G_ucStorData[j + 1] - '0') * 10) + (UINT16)(G_ucStorData[j + 2] - '0'));
                              *ByteCount += 2;

                              for (k = (j + 3); k < K_DPOF_SEARCH_SIZE; k++)
                              {                         
                                   *ByteCount += 1;
                                   //Compare line-feed and return
                                   if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                   {
                                        *ByteCount += 1;
                                        *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                        break;
                                   }
                              }

                              break;
                         }
                    }

                    break;
               }
               else
               {
                    if ((G_ucStorData[i] == 'I') && (G_ucStorData[i + 1] == 'M') && (G_ucStorData[i + 2] == 'G'))  //Find other parameter
                    {
                         *NewDramAddr = tmp_NewDramAddr;
                         *ByteCount = tmp_ByteCount;
                         *PrtQty = 1;
                         break;
                    }
               }
          }

          search_count -= 1;
     }

     if (*PrtQty == 0)
     {
          sts = FALSE;
     }

     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_FindDpofIMGFMT
//-----------------------------------------------------------------------------
UINT8 DPOF_FindDpofIMGFMT(UINT32 *NewDramAddr,UINT32 *ByteCount,UINT8 *ImgFmt) USING_0
{
     UINT16 i,j,k,l;
     UINT8 sts = TRUE;

     if ((*ByteCount < vfsFileSizeGet(0)) && (*ImgFmt == 0))
     {
          if (*ByteCount & 0x00000001)
          {
               l = 1;
          }
          else
          {
               l = 0;
          }

          SDRAM_Dram2Sram(*NewDramAddr,G_ucStorData,K_DPOF_SEARCH_SIZE);

          for (i = l; i < K_DPOF_SEARCH_SIZE; i++)
          {
               *ByteCount += 1;

               if ((G_ucStorData[i] == 'I') && (G_ucStorData[i + 1] == 'M') && (G_ucStorData[i + 2] == 'G') && (G_ucStorData[i + 3] == ' ') && 
                    (G_ucStorData[i + 4] == 'F') && (G_ucStorData[i + 5] == 'M') && (G_ucStorData[i + 6] == 'T'))
               {
                    *ByteCount += 6;
     
                    for (j = (i + 7); j < K_DPOF_SEARCH_SIZE; j++)                        
                    {
                         *ByteCount += 1;
                                  
                         if ((G_ucStorData[j] == 'C') && (G_ucStorData[j + 1] == 'I') && (G_ucStorData[j + 2] == 'F') &&
                              (G_ucStorData[j + 3] == 'F') && (G_ucStorData[j + 4] == '1'))
                         {                                   
                              *ImgFmt = K_DPOF_CIFF1_IMAGE;
                              *ByteCount += 3;

                              for (k = (j + 4); k < K_DPOF_SEARCH_SIZE; k++)
                              {                         
                                   *ByteCount += 1;
                                   //Compare line-feed and return
                                   if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                   {
                                        *ByteCount += 1;
                                        *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                        break;
                                   }
                              }
     
                              break;
                         }
                         else
                         {                         
                              if ((G_ucStorData[j] == 'E') && (G_ucStorData[j + 1] == 'X') && (G_ucStorData[j + 2] == 'I') &&
                                   (G_ucStorData[j + 3] == 'F') && (G_ucStorData[j + 4] == '1'))
                              {                                   
                                   *ByteCount += 4;

                                   for (k = (j + 5); k < K_DPOF_SEARCH_SIZE; k++)
                                   {                         
                                        *ByteCount += 1;
                                        //Compare EXIF1 -J or -T type
                                        if (G_ucStorData[k] == 'J')
                                        {
                                             *ImgFmt = K_DPOF_EXIF1_J_IMAGE;
                                             break;
                                        }
                                        else
                                        {
                                             if (G_ucStorData[k] == 'T')
                                             {
                                                  *ImgFmt = K_DPOF_EXIF1_T_IMAGE;
                                                  break;
                                             }
                                        }
                                   }
          
                                   for (; k < K_DPOF_SEARCH_SIZE; k++)
                                   {                         
                                        *ByteCount += 1;
                                        //Compare line-feed and return
                                        if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                        {
                                             *ByteCount += 1;
                                             *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                             break;
                                        }
                                   }
     
                                   break;
                              }
                              else
                              {
                                   if ((G_ucStorData[j] == 'E') && (G_ucStorData[j + 1] == 'X') && (G_ucStorData[j + 2] == 'I') &&
                                        (G_ucStorData[j + 3] == 'F') && (G_ucStorData[j + 4] == '2'))
                                   {                                   
                                        *ByteCount += 4;
          
                                        for (k = (j + 5); k < K_DPOF_SEARCH_SIZE; k++)
                                        {                         
                                             *ByteCount += 1;
                                             //Compare EXIF2 -J or -T type
                                             if (G_ucStorData[k] == 'J')
                                             {
                                                  *ImgFmt = K_DPOF_EXIF2_J_IMAGE;
                                                  break;
                                             }
                                             else
                                             {
                                                  if (G_ucStorData[k] == 'T')
                                                  {
                                                       *ImgFmt = K_DPOF_EXIF2_T_IMAGE;
                                                       break;
                                                  }
                                             }
                                        }
               
                                        for (; k < K_DPOF_SEARCH_SIZE; k++)
                                        {                         
                                             *ByteCount += 1;
                                             //Compare line-feed and return
                                             if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                             {
                                                  *ByteCount += 1;
                                                  *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                                  break;
                                             }
                                        }
     
                                        break;
                                   }
                                   else
                                   {          
                                        if ((G_ucStorData[j] == 'J') && (G_ucStorData[j + 1] == 'F') && (G_ucStorData[j + 2] == 'I') &&
                                             (G_ucStorData[j + 3] == 'F'))
                                        {                                   
                                             *ImgFmt = K_DPOF_JFIF_IMAGE;
                                             *ByteCount += 3;

                                             for (k = (j + 4); k < K_DPOF_SEARCH_SIZE; k++)
                                             {                         
                                                  *ByteCount += 1;
                                                  //Compare line-feed and return
                                                  if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                                  {
                                                       *ByteCount += 1;
                                                       *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                                       break;
                                                  }
                                             }
                    
                                             break;
                                        }
                                        else
                                        {
                                             if ((G_ucStorData[j] == 'F') && (G_ucStorData[j + 1] == 'P') && (G_ucStorData[j + 2] == 'X') &&
                                                  (G_ucStorData[j + 3] == '1'))
                                             {                                   
                                                  *ImgFmt = K_DPOF_FPX1_IMAGE;                    
                                                  *ByteCount += 3;

                                                  for (k = (i + 4); k < K_DPOF_SEARCH_SIZE; k++)
                                                  {                         
                                                       *ByteCount += 1;
                                                       //Compare line-feed and return
                                                       if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                                       {
                                                            *ByteCount += 1;
                                                            *NewDramAddr = K_SDRAM_DPOF1_BufAddr + (*ByteCount >> 1);
                                                            break;
                                                       }
                                                  }
                         
                                                  break;
                                             }
                                             else
                                             {
                                                  if ((G_ucStorData[j] == 'U') && (G_ucStorData[j + 1] == 'N') && (G_ucStorData[j + 2] == 'D') &&
                                                       (G_ucStorData[j + 3] == 'E') && (G_ucStorData[j + 4] == 'F'))
                                                  {                                   
                                                       *ImgFmt = K_DPOF_UNDEF_IMAGE;                         
                                                       *ByteCount += 4;
                         
                                                       for (k = (j + 5); k < K_DPOF_SEARCH_SIZE; k++)
                                                       {                         
                                                            *ByteCount += 1;
                                                            //Compare line-feed and return
                                                            if ((G_ucStorData[k] == 0x0d) && (G_ucStorData[k + 1] == 0x0a))
                                                            {
                                                                 *ByteCount += 1;
                                                                 *NewDramAddr = K_SDRAM_DPOF4_BufAddr + (*ByteCount >> 1);
                                                                 break;
                                                            }
                                                       }
                         
                                                       break;
                                                  }
                                             }
                                        }
                                   }
                              }
                         }
                    }

                    break;
               }
          }
     }

     if (*ImgFmt == 0)
     {
          sts = FALSE;
     }
          
     return sts;
}

//-----------------------------------------------------------------------------
//DPOF_BuildOneDpofFileJobSection
//-----------------------------------------------------------------------------
void DPOF_BuildOneDpofFileJobSection(UINT16 ImgSrcIdx,UINT8 PrtTyp,UINT16 PrtQty,UINT8 ImgFmt) USING_0
{
     UINT8 tmp1,tmp2;

     NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);         

     if (NewDpofFileByteCount & 0x00000001)
     {
          HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_Prefetch);
          HAL_DramWordDataRead(&tmp1,&tmp2);

          HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
          HAL_DramWordDataWrite(tmp1,0x0d);
          HAL_DramWordDataWrite(0x0a,0x00);
     }
     else
     {
          HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
          HAL_DramWordDataWrite(0x0d,0x0a);
     }

     NewDpofFileByteCount += 2;
     NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);         

     DPOF_WriteDpofFileJobSectionTitle();

     DPOF_WriteDpofFileParameter_PRTPID();

     DPOF_WriteDpofFileParameter_PRTTYP(PrtTyp);

     DPOF_WriteDpofFileParameter_PRTQTY(PrtQty);

     DPOF_WriteDpofFileParameter_IMGFMT(ImgFmt);

     DPOF_WriteDpofFileParameter_IMGSRC(PrtTyp,ImgSrcIdx);
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileJobParameter
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileJobParameter(UINT8 *ParameterTable,UINT16 TableLength) USING_0
{
     UINT16 i;
     UINT8 tmp1,tmp2;

     if (NewDpofFileByteCount & 0x00000001)
     {
          HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_Prefetch);

          HAL_DramWordDataRead(&tmp1,&tmp2);

          HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);
                   
          HAL_DramWordDataWrite(tmp1,ParameterTable[0]);

          for (i = 0; i < ((TableLength - 1) >> 1); i++)
          {
               HAL_DramWordDataWrite(ParameterTable[i * 2 + 1],ParameterTable[(i + 1) * 2]);
          }

          if (TableLength & 0x0001)
          {
               NewDpofFileByteCount += TableLength;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
          }
          else
          {
               HAL_DramWordDataWrite(ParameterTable[TableLength - 1],0x00);

               NewDpofFileByteCount += TableLength;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
          }                         
     }
     else //Destination address is wordboundary
     {
          HAL_DramStartAddrSet(NewDpofFileDramAddr,K_SDRAM_NoPrefetch);

          for (i = 0; i < (TableLength >> 1); i++)
          {
               HAL_DramWordDataWrite(ParameterTable[i * 2],ParameterTable[i * 2 + 1]);
          }

          if (TableLength & 0x0001)
          {
               HAL_DramWordDataWrite(ParameterTable[TableLength - 1],0x00);

               NewDpofFileByteCount += TableLength;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
          }                         
          else
          {
               NewDpofFileByteCount += TableLength;
               NewDpofFileDramAddr = K_SDRAM_DPOF4_BufAddr + (NewDpofFileByteCount >> 1);
          }
     }
}

//-----------------------------------------------------------------------------
//DPOF_WritekDpofFileJobSectionTitle
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileJobSectionTitle(void) USING_0
{
     UINT16 i;

     for (i = 0; i < K_DPOF_JOBSECTIONTITLE_SIZE; i++)
     {
          G_ucStorData[i] = DPOF_JobSectionTitle[i];
     }

     DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_JOBSECTIONTITLE_SIZE);
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileParameter_PRTPID
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileParameter_PRTPID(void) USING_0     
{
     UINT16 i;
     UINT8 tmp;

     for (i = 0; i < K_DPOF_PRTPID_SIZE; i++)
     {
          G_ucStorData[i] = DPOF_PrtPid[i];
     }

     //Modify the PID
     tmp = (UINT8)(NewDpofJobSectionPRTPID / 100);
     G_ucStorData[K_DPOF_PRTPID_SIZE - 5] = tmp + '0';
     G_ucStorData[K_DPOF_PRTPID_SIZE - 4] = (UINT8)((NewDpofJobSectionPRTPID - tmp * 100) / 10) + '0';
     G_ucStorData[K_DPOF_PRTPID_SIZE - 3] = (UINT8)(NewDpofJobSectionPRTPID % 10) + '0';
     
     DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_PRTPID_SIZE);

     NewDpofJobSectionPRTPID++;
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileParameter_PRTTYP
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileParameter_PRTTYP(UINT8 PrintType) USING_0     
{
     UINT16 i;

     if (PrintType == K_DPOF_STD)
     {
          for (i = 0; i < K_DPOF_PRTTYPSTD_SIZE; i++)
          {
               G_ucStorData[i] = DPOF_PrtTypStd[i];
          }

          DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_PRTTYPSTD_SIZE);
     }
     else //PrintType = K_DPOF_IDX
     {
          for (i = 0; i < K_DPOF_PRTTYPIDX_SIZE; i++)
          {
               G_ucStorData[i] = DPOF_PrtTypIdx[i];
          }

          DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_PRTTYPIDX_SIZE);
     }     
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileParameter_PRTQTY
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileParameter_PRTQTY(UINT16 PrintQuantity) USING_0     
{
     UINT16 tmp;
     UINT16 i;

     for (i = 0; i < K_DPOF_PRTQTY_SIZE; i++)
     {
          G_ucStorData[i] = DPOF_PrtQty[i];
     }

     //Modify the QTY
     tmp = (PrintQuantity / 100);
     G_ucStorData[K_DPOF_PRTQTY_SIZE - 5] = (UINT8)tmp + '0';
     G_ucStorData[K_DPOF_PRTQTY_SIZE - 4] = (UINT8)((PrintQuantity - tmp * 100) / 10) + '0';
     G_ucStorData[K_DPOF_PRTQTY_SIZE - 3] = (UINT8)(PrintQuantity % 10) + '0';
     
     DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_PRTQTY_SIZE);
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileParameter_IMGFMT
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileParameter_IMGFMT(UINT8 ImageFormat) USING_0     
{
     UINT16 i;

     if (ImageFormat == K_JFIF_IMAGE_FORMAT)
     {
          for (i = 0; i < K_DPOF_IMGFMTJFIF_SIZE; i++)
          {
               G_ucStorData[i] = DPOF_ImgFmtJfif[i];
          }

          DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_IMGFMTJFIF_SIZE);
     }
     else //ImageFormat == K_EXIF_IMAGE_FORMAT 
     {
          for (i = 0; i < K_DPOF_IMGFMTEXIF2J_SIZE; i++)
          {
               G_ucStorData[i] = DPOF_ImgFmtExif2j[i];
          }

          DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_IMGFMTEXIF2J_SIZE);
     }     
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileParameter_IMGSRC
//-----------------------------------------------------------------------------
//version4.0@ada@0513 for DPOF
void DPOF_WriteDpofFileParameter_IMGSRC(UINT8 PrintType,UINT16 FileIndex) USING_0     
{
     UINT16 i;

	//patch4.5@ada@Record DCF file name begin	
     //UINT8 tmp;
	UINT8 name[8];
	UINT8 ch;

     HAL_DramStartAddrSet(K_SDRAM_JPG_FdbParameterBufAddr + ((UINT32)FileIndex - 1) * 10 + 6,K_SDRAM_Prefetch);
	
	for (i = 0; i < 4; i += 2)
	{
		HAL_DramWordDataRead(&name[i],&name[i + 1]);
	}

     for (i = 7; i > 3; i--)
     {
          ch = (UINT8)(FileIndex % 10) + '0';
          FileIndex /= 10;
          name[i] = ch;
     }
	//patch4.5@ada@Record DCF file name end

     if (PrintType == K_DPOF_STD)
     {
          for (i = 0; i < K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE; i++)
          {
               G_ucStorData[i] = DPOF_ImgSrcStdSunpXXXXjpg[i];
          }

          //Write the directory name
          for (i = 0; i < 8; i++)
          {
               G_ucStorData[20 + i] = DpofDir1Name[i];
          }

		//patch4.5@ada@Record DCF file name begin
          for (i = 0; i < 4; i++)
          {
               G_ucStorData[29 + i] = name[i];
          }

          G_ucStorData[K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE - 12] = name[4];
          G_ucStorData[K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE - 11] = name[5];
          G_ucStorData[K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE - 10] = name[6];
          G_ucStorData[K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE - 9] = name[7];
			
          //for (i = 0; i < 4; i++)
          //{
          //     G_ucStorData[29 + i] = G_USR_Company_UpperSIG[i];
          //}

          //Modify the PID
          //tmp = (FileIndex / 1000);
          //G_ucStorData[K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE - 12] = (UINT8)tmp + '0';
          //FileIndex -= (tmp * 1000);
          //tmp = (FileIndex / 100);
          //G_ucStorData[K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE - 11] = (UINT8)tmp + '0';
          //G_ucStorData[K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE - 10] = (UINT8)((FileIndex - tmp * 100) / 10) + '0';
          //G_ucStorData[K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE - 9] = (UINT8)(FileIndex % 10) + '0';

		//patch4.5@ada@Record DCF file name end

          DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_IMGSRCSTDSUNPXXXXJPG_SIZE);
     }
     else
     {
          for (i = 0; i < K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE; i++)
          {
               G_ucStorData[i] = DPOF_ImgSrcIdxSunpXXXXjpg[i];
          }

          //Write the directory name
          for (i = 0; i < 8; i++)
          {
               G_ucStorData[19 + i] = DpofDir1Name[i];
          }

		//patch4.5@ada@Record DCF file name begin
          for (i = 0; i < 4; i++)
          {
               G_ucStorData[28 + i] = name[i];
          }

          //Modify the PID
          G_ucStorData[K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE - 11] = name[4];
          G_ucStorData[K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE - 10] = name[5];
          G_ucStorData[K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE - 9] = name[6];
          G_ucStorData[K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE - 8] = name[7];

          //for (i = 0; i < 4; i++)
          //{
          //     G_ucStorData[28 + i] = G_USR_Company_UpperSIG[i];
          //}

          //Modify the PID
          //tmp = (FileIndex / 1000);
          //G_ucStorData[K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE - 11] = (UINT8)tmp + '0';
          //FileIndex -= (tmp * 1000);
          //tmp = (FileIndex / 100);
          //G_ucStorData[K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE - 10] = (UINT8)tmp + '0';
          //G_ucStorData[K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE - 9] = (UINT8)((FileIndex - tmp * 100) / 10) + '0';
          //G_ucStorData[K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE - 8] = (UINT8)(FileIndex % 10) + '0';

		//patch4.5@ada@Record DCF file name end

          DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_IMGSRCIDXSUNPXXXXJPG_SIZE);
     }
}
/*
//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileParameter_CFGDSC
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileParameter_CFGDSC(UINT8 PrintType,UINT8 *StringArray,UINT16 StringLength,UINT8 AttributeType,UINT32 *DramAddr,UINT8 *NonWordBoundry) USING_0     
{
     UINT16 i;

     if (PrintType == K_DPOF_STD)
     {
          for (i = 0; i < K_DPOF_CFGDSCSTD_SIZE; i++)
          {
               G_ucStorData[i] = DPOF_CfgDscStd[i];
          }

          for (i = K_DPOF_CFGDSCSTDSTRING_START_INDEX; i < (K_DPOF_CFGDSCSTDSTRING_START_INDEX + StringLength); i++)
          {
               G_ucStorData[i] = *(StringArray + i)
          }

          G_ucStorData[K_DPOF_CFGDSCSTD_START_INDEX + StringLength] = '"';

          if (StringLength < 12)
          {
               for (i = (K_DPOF_CFGDSCSTD_START_INDEX + StringLength + 1); i < K_DPOF_CFGDSCSTDSTRING_END_INDEX; i++)
               {
                    G_ucStorData[i] = ' ',
               }
          }

          if (AttributeType == K_DTM)
          {
               G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX] = 'D';
               G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX + 1] = 'T';
               G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX + 2] = 'M';
          }
          else
          {
               if (AttributeType == K_FID)
               {
                    G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX] = 'F';
                    G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX + 1] = 'I';
                    G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX + 2] = 'D';
               }
               else //(AttributeType == K_CMT)
               {
                    G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX] = 'C';
                    G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX + 1] = 'M';
                    G_ucStorData[K_DPOF_CFGDSCSTDATTRIBUTE_START_INDEX + 2] = 'T';
               }
          }
    
          DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_PRTTYPSTD_SIZE,DramAddr,NonWordBoundry);
     }
     else //PrintType = K_DPOF_IDX
     {
          for (i = 0; i < K_DPOF_CFGDSCIDX_SIZE; i++)
          {
               G_ucStorData[i] = DPOF_CfgDscIdx[i];
          }

          for (i = K_DPOF_CFGDSCIDX_START_INDEX; i < (K_DPOF_CFGDSCIDX_START_INDEX + StringLength); i++)
          {
               G_ucStorData[i] = *(StringArray + i)
          }

          G_ucStorData[K_DPOF_CFGDSCIDX_START_INDEX + StringLength] = '"';
          G_ucStorData[K_DPOF_CFGDSCIDX_START_INDEX + StringLength + 1] = 0x0d;
          G_ucStorData[K_DPOF_CFGDSCIDX_START_INDEX + StringLength + 2] = 0x0a;

          DPOF_WriteDpofFileJobParameter(&G_ucStorData,(K_DPOF_CFGDSCIDX_START_INDEX + StringLength + 3),DramAddr,NonWordBoundry);
     }     
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileParameter_CFGTRM
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileParameter_CFGTRM(UINT16 x,UINT16 y,UINT16 w,UINT16 H,UINT32 *DramAddr,UINT8 *NonWordBoundry) USING_0     
{

     
}

//-----------------------------------------------------------------------------
//DPOF_WriteDpofFileParameter_CFGROT
//-----------------------------------------------------------------------------
void DPOF_WriteDpofFileParameter_CFGROT(UINT16 RotateDegree,UINT32 *DramAddr,UINT8 *NonWordBoundry) USING_0     
{
     UINT16 i;

     for (i = 0; i < K_DPOF_CFGROT_SIZE; i++)
     {
          G_ucStorData[i] = DPOF_CfgRot[i];
     }

     if (0 < RotateDegree < 360)
     {
          //Modify the ROT
          tmp = (UINT8)(RotateDegree / 100);
          G_ucStorData[K_DPOF_CFGROT_SIZE - 5] = tmp + '0';
          G_ucStorData[K_DPOF_CFGROT_SIZE - 4] = (UINT8)((RotateDegree - tmp * 100) / 10) + '0';
          G_ucStorData[K_DPOF_CFGROT_SIZE - 3] = (UINT8)(RotateDegree % 10) + '0';
     }
     
     DPOF_WriteDpofFileJobParameter(&G_ucStorData,K_DPOF_CFGROT_SIZE,DramAddr,NonWordBoundry);
}


//-----------------------------------------------------------------------------
//DPOF_CheckDpofFileHeaderSection
//-----------------------------------------------------------------------------
UINT8 DPOF_CheckDpofFileHeaderSection(void) USING_0
{
     UINT16 i;
     UINT8 sts = TRUE;

     SDRAM_Dram2Sram(K_SDRAM_DPOF1_BufAddr,K_DPOF_SEARCH_SIZE);

     for (i = K_DPOF_HDR_GEN_CRT_START_INDEX; i < (K_DPOF_HDR_GEN_CRT_START_INDEX + K_DPOF_HDR_GEN_CRT_SIZE); i++)
     {
          if (G_ucStorData[i] != DPOF_HeaderSectionContent[i])
          {
               //Sunplus camera is not the last one to modify this file
               sts = FALSE;               
               break;
          }                        
     }

     return sts;
}
*/     

#endif
