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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "hal_dram.h"

#include "doslink.h"
#include "dosvar.h"
#include "dos32.h"
#include "dosusr.h"
#include "dbg_file_list.h"
#include "dbg_wdt.h"
#include "dbg_mem.h"
#include "dbg_assert.h"
#include "initio.h"
#include "reg_def.h"
#include "sd_bg.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_VFS_API__
#define	DEBUG	0
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern UINT8 xdata G_ucStorData[]; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern DOS dos;
extern UINT32 dos_var_start;

 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
//wangjg++ 20100930
UINT8 vfsFileProtectAll(UINT8 isReadOnly)
{
#if 1
	UINT8 DevId, ret;

	FDB fdb;
	UINT8 wflag = 0;
	UINT32 addr = 0;	
	x86_FDB xdata* pFdb = (x86_FDB xdata*)G_ucStorData;//[512];
	x86_FDB xdata* pt;
	UINT32 addrTmp = K_SDRAM_ImagePlaybackGeneralBufAddr;//K_SDRAM_ImagePlaybackBufAddr;//K_SDRAM_DPSStartAddr + (10*512ul);//8;
	DevId = DOS_CurrDriverGet();

	DIR_SearchReset();
	//DOS_FileDelReset();
	
	while(1) 
	{
		dbgWdtFeed(10000);
		ret = DIR_Search(DevId, "*", (FDB *)&fdb, FILE_INFO_GET_SIZE);
		if(ret==FIND_FILE_FAIL)	break;
		if(ret!=FIND_FILE_OK)	continue;
		//printf("name=%s\n",fdb.name);
		ret = USR_CheckExtType(fdb.name);//USR_MatchFdb(matchCond, (UINT8 *)&(fdb.name), refCon);
		//printf("name=%s,%bx\n",fdb.name,ret);
		if (ret) 
		{			
			//UINT32 fdbLogAddr = ((UINT32)fdb.FDB_Sector << 9) + (fdb.FDB_Idx << 5);
			//UINT32 pageLogAddr = ( fdbLogAddr >> K_Card_PageSizeShift ) << K_Card_PageSizeShift;
			UINT32 pageLogAddr = fdb.FDB_Sector;
			//UINT16 offset =((UINT16)fdb.FDB_Idx)<<5;// fdbLogAddr - pageLogAddr;
			pt = pFdb+fdb.FDB_Idx;
			if(addr != pageLogAddr)
			{
				if(wflag)
				{
					//HAL_DramDmaWrite(pFdb, addrTmp, 512, 1);
					HAL_DmaSetSramAddr(0x0000);
					HAL_DmaSetDramAddr(addrTmp);
					HAL_DmaDo(1, 0, 512, 1, 0);
					if(0 == DOS_SectorWrite(DevId, addr, 1, addrTmp))
						return 0;
					//ret = DOS_WriteSector(K_DOS_SrcSram, addr, 0);
					
					wflag = 0;
				}
				addr = pageLogAddr;
				if(0 == DOS_SectorRead(DevId, addr, 1, addrTmp))
					return 0;
				HAL_DmaSetSramAddr(0x0000);
				HAL_DmaSetDramAddr(addrTmp);
				HAL_DmaDo(0, 1, 512, 1, 0);
				//ret = DOS_ReadSector( K_DOS_DstSram, pageLogAddr, 0 ) ;
				
				//HAL_DramDmaRead(pFdb, dos.TmpBufAddr, 512, 1);

			}
			//if ( ret)
			//{
				//UINT16 offset = fdbLogAddr - pageLogAddr;
				//pFdb = G_ucDataBuffer+offset;
				//ret = pFdb[11] & 0x01;
				//if(isReadOnly!=ret)
				//{
					pt->attr = (pt->attr & 0xfe) | isReadOnly;				
					wflag = 1;
				//}
			//}
		}		
	}
	if(wflag)
	{
		//HAL_DramDmaWrite(pFdb, addrTmp, 512, 1);
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaSetDramAddr(addrTmp);
		HAL_DmaDo(1, 0, 512, 1, 0);
		if(0 == DOS_SectorWrite(DevId, addr, 1, addrTmp))
			return 0 ;
			//ret = DOS_WriteSector(K_DOS_SrcSram, addr, 0);
		
	}
	#endif
	return TRUE;
}
/**
 * @fn        UINT8 vfsFileDelete(UINT8 *name)
 * @brief     Delete a file
 * @param     [in] name		The name of the file to be deleted.
 * @retval    return = TRUE / FALSE
 * @see       NULL
*/
UINT8 vfsFileDelete(UINT8 *name)
{
return !DOS_FileDel(name);
#if 0
#if 1
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
#endif
#endif
}

/**
 * @brief	delete all file
 * @param	name, start sectors (1 sector = 512 Bytes)
 * @retval	0 is success, others is FAIL.
 * @see
 * @author	wangjg
 * @since	08/04/16 17:36
 * @todo
 * @bug		device name...
*/
UINT8 vfsFileDeleteAll()
{
	UINT8 devId = DOS_CurrDriverGet();
	DISK xdata*disk	  = (DISK xdata*)&dos.disk[devId];
	UINT32 cluster	 = disk->CurrDirStartCluster;
	UINT32 cluster2Sector = disk->Cluster2Sector;
	UINT32 cluster2Byte = disk->Cluster2Byte;
	UINT32 cluster2Fdb = disk->Cluster2Fdb;
	UINT32 clusterNum = disk->TotalCluster;
	UINT32 sectorNum = (clusterNum+127)/128;
	UINT32 fatStartSector = disk->FAT_StartSector;
	UINT32 clusterNumMax = disk->MaxClusterNum;
	UINT32 eof = disk->FAT_EOFValue;
	UINT32 firstFreeCluster = disk->FAT_FirstFreeCluster;
	UINT8 fatType = disk->FsType;
	UINT8 bitCluster2Sector = disk->bitCluster2Sector;
	UINT8 mskCluster2Sector = (1ul<<bitCluster2Sector)-1;

	UINT16 bufSize = 1024*4;
	UINT8 xdata* buf=G_FPGAMemPool;
	
	UINT32 fdbSector;
	UINT32 dosTmpBuf = dos.TmpBufAddr;

	//delete all
	UINT32 fatAddr0=K_SDRAM_ImagePlaybackDisplayBufAddr; //user 64k K_SDRAM_ImagePlaybackBufAddr;//
	UINT32 fatAddr1=K_SDRAM_JPG_FdbParameterBufAddr;// K_SDRAM_ImagePlaybackDisplayBufAddr+SP1K_PB_DISP_BUF_SIZE_A;  //max 1M buffer
	UINT32 fatSize0= K_SDRAM_TotalSize - fatAddr0 -512 - 320ul*240;
	UINT32 fatSize1= K_SDRAM_JPG_FdbParameterBufSize;
	
	UINT8 fatTypeFactor = (fatType==32)?2:1;
	UINT32 fatBankSizeSector0 = fatSize0/256;
	UINT32 fatBankSizeSector1 = fatSize1/256;

	UINT32 fatBankClusterNum0 = fatSize0/fatTypeFactor;
	UINT32 fatBankClusterNum1 = fatSize1/fatTypeFactor;
	UINT32 bank = fatStartSector+fatBankSizeSector0;
	//UINT32 fatAddr = fatAddr0;
	//UINT32 curr,next;
	//UINT32 sector,secidx,addr;
	//UINT8 tmp0,tmp1,tmp2,tmp3;
	
	UINT32 fdbCluster[40];
	UINT8 cnt = 40 ;
	UINT8 idx;
	UINT32 cc = (cluster2Byte+bufSize-1)/bufSize;
	UINT32 ii; 
	UINT8 cc1 = bufSize/32;
	UINT8 ii1;
	UINT8 sts=0;
    UINT8 tmp; //cx++ for FAT12
	//UINT16 dbgCnt = 0;
	printf("DelAll:%lx\n",fatSize0);
	//load dir fdb cluster
	//printf("fatType:%bx,%bx,%bx,%bx\n",fatType,fatTypeFactor,bitCluster2Sector,mskCluster2Sector);
	//if(fatType!=32 && fatType!=16)return FALSE;  //cx mask
	//printf("cluster:%lx,%lx,%lx\n",cluster2Sector,cluster2Fdb,cluster2Byte);
	//printf("cluster:%lx\n",cluster);
	fdbCluster[0] = cluster;
	for(idx=1;idx<cnt;idx++)
	{		
		cluster = FAT_TableRead(devId, cluster);
		if((cluster > clusterNumMax-1) || (cluster < 2) || (cluster == eof))
			break;
		fdbCluster[idx] = cluster;
		//printf("cluster:%lx\n",cluster);
		
	}
	cnt = idx;
	//printf("loop:%bx,%lx,%bx\n",cnt,cc,cc1,(disk->Cluster2Byte+1023)/1024);
	//load fat table to bank0/bank1
	//printf("addr(%lx,%lx)size(%lx,%lx)\n",fatAddr0,fatAddr1,fatSize0,fatSize1);
	//printf("%lx,%lx,%lx,%lx\n",clusterNum,bank, fatNum0,fatNum1);
	//printf("ssect:%lx\n",fatStartSector);
	if(clusterNum > fatBankClusterNum0)
	{
		DOS_SectorRead(devId, fatStartSector, fatBankSizeSector0, fatAddr0); //read total fat to dram 
		DOS_SectorRead(devId, bank, fatBankSizeSector1, fatAddr1); 
		__PRINTF_POS();
	}
	else
	{
		DOS_SectorRead(devId, fatStartSector, sectorNum, fatAddr0); //read total fat to dram 
		bank =0;
	}
	//dumpData(512, fatAddr0, 1);
	//HAL_DmaSetSramAddr(0x0000);
	for(idx=0;idx<cnt;idx++)// load dir fdb cluster to dram
	{
		if(sts)break;
		cluster = fdbCluster[idx];		
		fdbSector = FAT_Cluster2Sector(devId, cluster);
		printf("r0:%bx,%lx\n",idx,cluster);
		if( 0 == DOS_SectorRead(devId, fdbSector, cluster2Sector, dosTmpBuf)) {
			__PRINTF_POS();
			//return dosRwErr(__FILE,(unsigned short)__LINE__);
		}
		//findAddr= dosTmpBuf;	//start from name[0]
		
		for(ii=0;ii<cc;ii++)//load dir fdb 1024 byte to sram
		{
			UINT32 findAddr = dosTmpBuf + ii*bufSize/2;
			//HAL_DramDmaRead(buf,findAddr,1024,0);
			if(sts)break;
			//HAL_DmaSetDramAddr(findAddr);
			//HAL_DmaDo(0,1,1024,1, 0);
			HAL_DramDmaRead(buf, findAddr,bufSize, 1);
			printf("r1:%lx:%lx\n",ii,findAddr);
			if(ii==cc-1 && (cluster2Byte&(bufSize-1))!=0)cc1=(cluster2Byte&(bufSize-1))/32;
			for(ii1 = 0; ii1<cc1; ii1++)//load fdb
			{
				x86_FDB xdata* pFdb = ((x86_FDB xdata*)buf)+ii1;
				UINT8 val= ((UINT8 xdata*)pFdb)[0]; 			
				UINT32 curr = 0;

				UINT32 sector;
				UINT8 secidx;
				UINT32 addr;
				UINT8 tmp0,tmp1,tmp2,tmp3;
				///////
				//UINT8 names[12];
				//memcpy(names,pFdb->name,11);names[11]=0;
				//printf("%s,%bx,%bx\n",names,ii1,pFdb->attr);
				//printf("%s,%04x,%02bx\n",names,(UINT16)pFdb,val);
				//printf("%s,pt(%x,%x)addr(%lx)%bx\n",names,(UINT16)pFdb,(UINT16)buf+1024,findAddr,val);
				///////
				if(sts)break;
				if(val == 0){sts = 1;break;}//end search
				if((val == 0xE5)//erased
					||(pFdb->attr&0x01)//readonly
					||(!USR_CheckExtType(pFdb->name))//is not ourfile
					)continue;
									
				//////////////
				//delete all
				pFdb->name[0]= 0xE5;	//erase mark
				if(fatType==32){
					WRITE8(curr,0,pFdb->starthi[1]);
					WRITE8(curr,1,pFdb->starthi[0]);
				}
				if(fatType==12)
				{
					WRITE8(curr,2,pFdb->start[1]&0x0f);
					WRITE8(curr,3,pFdb->start[0]);
				}
				else
				{
					WRITE8(curr,2,pFdb->start[1]);
					WRITE8(curr,3,pFdb->start[0]);
				}

				//printf("del:%lx\n",curr);
				while(curr != eof && curr < clusterNumMax)//remove cluster list
				{			
					if(curr<2){ 
						printf("curr:%lx\n",curr);
						__PRINTF_POS();
						break;
						//dosRwErr(__FILE,(unsigned short)__LINE__);
					}
					//sector = curr>>bitCluster2Sector;//curr fat pos
					sector = ((curr<<fatTypeFactor)>>9)+fatStartSector;
					//disk->DataStartSector + ((Cluster-2) << disk->bitCluster2Sector);
					secidx = curr &mskCluster2Sector;
				
					//printf("%lx\n",curr);
					if( curr < firstFreeCluster ){
						firstFreeCluster = curr;//mantis-26586 20080620
					}

					if(sector < fatBankSizeSector0+fatStartSector){ //inside of  bank0
					    if(fatType==12)  //fatAddr0 is larger than fat12 fat buf
						{ 
							tmp = curr % 4;
							addr = fatAddr0 + ((curr >> 2) * 3) + (tmp >> 1);
						}
						else
						{
							addr = fatAddr0 + curr*fatTypeFactor;
						}
					}else if(sector>=bank&&sector<(bank+fatBankSizeSector1)){//inside of bank1
						addr = fatAddr1+secidx*fatTypeFactor+(sector-bank)*256; 							
					}else if(bank){//switch bank						
						if(0 == DOS_SectorWrite(devId, bank, fatBankSizeSector1, fatAddr1)){ 
							//dosRwErr(__FILE,(unsigned short)__LINE__);	
							__PRINTF_POS();
							break;
						}
						if(0 == DOS_SectorRead(devId, sector, fatBankSizeSector1, fatAddr1)){
							//dosRwErr(__FILE,(unsigned short)__LINE__);		
							__PRINTF_POS();
							break;
						}
						//printf("*");
						//dbgCnt++:
						bank=sector;
						addr = fatAddr1+secidx*fatTypeFactor;
					}else{
						//dosRwErr(__FILE,(unsigned short)__LINE__);
						__PRINTF_POS();
						break;
					}
					//HAL_DramStartAddrSet(addr, 1);//read next cluster
					//HAL_DramWordDataRead(&tmp0,&tmp1);
					//HAL_DramWordDataRead(&tmp2,&tmp3);
					XBYTE[REG_Dram_IndirectAddr_B0] = READ8(&addr, 3);
					XBYTE[REG_Dram_IndirectAddr_B1] = READ8(&addr, 2);
					XBYTE[REG_Dram_IndirectAddr_B2] = READ8(&addr, 1);
					XBYTE[REG_Dram_IndirectAddr_B3] = READ8(&addr, 0);
					XBYTE[REG_Dram_IndirectPrefetch]=0x01;
					while(XBYTE[REG_Dram_IndirectPrefetch]);
					tmp0 = XBYTE[REG_Dram_IndirectRData_Low];
					tmp1 = XBYTE[REG_Dram_IndirectRData_High];
					if((fatType==32)||(fatType==12)){
					while(XBYTE[REG_Dram_IndirectPrefetch]);
					tmp2 = XBYTE[REG_Dram_IndirectRData_Low];
					tmp3 = XBYTE[REG_Dram_IndirectRData_High];
					}
                    
					////////////get next cluster//////////////////////////
					if(fatType==12){
						if( tmp == 0 ){
							//        0  1  2  3
							//ret32 = FF XF XX XX
							WRITE8(curr, 3, tmp0);
							WRITE8(curr, 2, tmp1 & 0x0F);
						}
						else if( tmp == 1 ){
							//        0  1  2  3
							//ret32 = XX FX FF XX
							WRITE8(curr, 2, tmp2);
							WRITE8(curr, 3, tmp1);
							curr = curr >> 4;
						}
						else if( tmp == 2 ){
							//        0  1  2  3
							//ret32 = XX FF XF XX
							WRITE8(curr, 3, tmp1);
							WRITE8(curr, 2, tmp2 & 0x0F);
						}
						else if( tmp == 3 ){
							//        0  1  2  3
							//ret32 = XX XX FX FF
							WRITE8(curr, 2, tmp3);
							WRITE8(curr, 3, tmp2);
							curr = curr >> 4;
						}
						curr = curr & 0x0fff;
						}
					else{  //FAT16&FAT32
						WRITE8(curr, 3, tmp0);
						WRITE8(curr, 2, tmp1);
						if(fatType==32){
							WRITE8(curr, 1, tmp2);
							WRITE8(curr, 0, tmp3);
						}
					}       
					////////////////next cluster end

					////////////////clean curr cluster 0
					XBYTE[REG_Dram_IndirectAddr_B0] = READ8(&addr, 3);
					XBYTE[REG_Dram_IndirectAddr_B1] = READ8(&addr, 2);
					XBYTE[REG_Dram_IndirectAddr_B2] = READ8(&addr, 1);
					XBYTE[REG_Dram_IndirectAddr_B3] = READ8(&addr, 0);
					XBYTE[REG_Dram_IndirectPrefetch]=0x00;
					while(XBYTE[REG_Dram_IndirectPrefetch]);
					if(fatType==12){
						if( tmp == 0 ){
							//       0  1  2  3
							//val = FF XF XX XX
							tmp0=0x00;
							tmp1&=0xF0;
						}
						else if( tmp == 1 ){
							//       0  1  2  3
							//val = XX FX FF XX
							tmp1&=0x0F;
							tmp2=0x00;
						}
						else if( tmp == 2 ){
							//       0  1  2  3
							//val = XX FF XF XX
							tmp1=0x0;
							tmp2&=0xF0;
						}
						else if( tmp == 3 ){
							//       0  1  2  3
							//val = XX XX FX FF
							tmp2&=0x0F;
							tmp3=0x00;
						}
						//write 2 or 4 bytes to DRAM
						XBYTE[REG_Dram_IndirectRData_Low] =tmp0;// READ8(val32,0);
						XBYTE[REG_Dram_IndirectRData_High] =tmp1; //READ8(val32,1);
						while(XBYTE[REG_Dram_IndirectPrefetch]);	   
						XBYTE[REG_Dram_IndirectRData_Low] =tmp2; //READ8(val32,2);
						XBYTE[REG_Dram_IndirectRData_High] =tmp3;// READ8(val32,3);
					}
					else {  //FAT16&FAT32
						XBYTE[REG_Dram_IndirectRData_Low]=0;
						XBYTE[REG_Dram_IndirectRData_High]=0;
						if(fatType==32){
							while(XBYTE[REG_Dram_IndirectPrefetch]);
							XBYTE[REG_Dram_IndirectRData_Low]=0;
							XBYTE[REG_Dram_IndirectRData_High]=0;
						}
					}

					//HAL_DramStartAddrSet(addr, 0);//mask curr deleted
					//HAL_DramWordDataWrite(0x00, 0x00);
					//HAL_DramWordDataWrite(0x00, 0x00);
					disk->FreeCluster++;
					}			
				}		
			HAL_DramDmaWrite(buf, findAddr,bufSize, 1);
			//HAL_DmaDo(1,0,1024,1, 0);
			//HAL_DramDmaWrite(buf,findAddr,1024,0);			
			//printf("w1\n");
		}

		if( 0 == DOS_SectorWrite(devId, fdbSector, cluster2Sector, dosTmpBuf)) {
			__PRINTF_POS();
			//return dosRwErr(__FILE,(unsigned short)__LINE__);		
		}
		//printf("w0\n");
	}
	disk->FAT_FirstFreeCluster = firstFreeCluster;
	if(clusterNum > fatBankClusterNum0)
	{	 
		//printf("fatW:%bx,%lx,%lx,%lx\n",devId, fatStartSector, fatSectorNum0, fatAddr0);
		//printf("fatW:%bx,%lx,%lx,%lx\n",devId, bank, fatSectorNum1, fatAddr1);
	  
		if( 0 == DOS_SectorWrite(devId, fatStartSector, fatBankSizeSector0, fatAddr0)) {
			__PRINTF_POS();
			//return dosRwErr(__FILE,(unsigned short)__LINE__);
		}
		if( 0 == DOS_SectorWrite(devId, bank, fatBankSizeSector1, fatAddr1)) {
			__PRINTF_POS();
			//return dosRwErr(__FILE,(unsigned short)__LINE__);
		}
	}
	else
	{
		//printf("fatW:%bx,%lx,%lx,%lx\n",devId, fatStartSector, sectorNum, fatAddr0);
		if( 0 == DOS_SectorWrite(devId, fatStartSector, sectorNum, fatAddr0)) {
			__PRINTF_POS();
			//return dosRwErr(__FILE,(unsigned short)__LINE__);
		}
	}
	//dumpData(512, fatAddr0, 1);
	//printf("%x,%lx\n",disk->FAT_Bank2Sector,disk->FAT_Bank2Cluster);
	if(0 == DOS_SectorRead(devId, disk->FAT_StartSector + (disk->FAT_CurrBank<< disk->FAT_bitBank2Sector), disk->FAT_Bank2Sector, disk->FAT_BufAddr)){ //read total fat to dram
		return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	//dumpData(512, disk->FAT_BufAddr, 1);
	//printf("bank switch:%d\n",dbgCnt);
	return TRUE;
}

#if 0
UINT8 vfsFileDeleteAll()
{
	UINT8 DevId = DOS_CurrDriverGet();
	xdata DISK *disk      = (DISK *)&dos.disk[DevId];
	UINT32 cluster   = disk->CurrDirStartCluster;
	UINT32 SectorNum = disk->Cluster2Sector;
	UINT32 cluster0 = 0;
	UINT8 fatFlag = 0;
	UINT8 xdata* buf=G_ucStorData;
	x86_FDB xdata* pFdb = (x86_FDB xdata*)(buf+1024);//[512];
	
	UINT32 SectorStart;
	UINT32 FindEnd = dos.TmpBufAddr+disk->Cluster2Byte/2;
	UINT32 FindAddr=FindEnd+1;// FindNum;

	UINT8 fdbIdx = 0;
	UINT8 fdbFlag0 = 0;
	UINT8 fdbFlag1 = 0;
	UINT8 val;

	UINT8 names[12];
	//UINT8 endFlag;

	//delete all
	UINT32 fatStart = disk->FAT_StartSector;
	UINT32 fatAddr0=K_SDRAM_ImagePlaybackBufAddr; //user 64k
    UINT32 fatAddr1=K_SDRAM_JPG_FdbParameterBufAddr;// K_SDRAM_ImagePlaybackDisplayBufAddr+SP1K_PB_DISP_BUF_SIZE_A;  //max 1M buffer
	UINT32 fatSize0= K_SDRAM_TotalSize - fatAddr0;
	UINT32 fatSize1= K_SDRAM_JPG_FdbParameterBufSize;
	UINT8 fatType = 32;
	UINT32 fatSectorNum0 = fatSize0/256;
	UINT32 fatSectorNum1 = fatSize1/256;

	UINT32 fatNum0 = fatSize0/2;
	UINT32 fatNum1 = fatSize1/2;
	UINT32 bank = disk->FAT_StartSector+fatSectorNum0;
	UINT32 fatAddr = fatAddr0;
	UINT32 curr,next;
	UINT32 sector,secidx,addr;
	UINT8 tmp0,tmp1,tmp2,tmp3;

	UINT32 clusterNum = disk->TotalCluster;
	UINT32 sectorNum = (clusterNum+127)/128;
	UINT32 fatStartSector = disk->FAT_StartSector;

	UINT32 dosTmpBuf = dos.TmpBufAddr;
	UINT32 fdbCluster[20];
	UINT32 cnt = 20 , idx = 0;
	fdbCluster[0] = cluster;

	printf("cluster:%lx,%lx,%lx\n",disk->Cluster2Sector,disk->Cluster2Fdb,disk->Cluster2Byte);
	printf("cluster:%lx\n",cluster);
	for(val=1;val<cnt;val++)
	{		
		fdbCluster[val] = cluster = FAT_TableRead(DevId, cluster);
		printf("cluster:%lx\n",cluster);
		if(cluster == disk->FAT_EOFValue)break;
	}
	cluster = fdbCluster[0];
	printf("addr(%lx,%lx)size(%lx,%lx)\n",fatAddr0,fatAddr1,fatSize0,fatSize1);
	printf("%lx,%lx,%lx,%lx\n",clusterNum,bank, fatNum0,fatNum1);
	printf("ssect:%lx\n",fatStartSector);
    if(clusterNum > fatNum0)
    {
        DOS_SectorRead(DevId, fatStartSector, fatNum0, fatAddr0); //read total fat to dram 
        DOS_SectorRead(DevId, bank, fatNum1, fatAddr1); 
    }
    else
    {
        DOS_SectorRead(DevId, fatStartSector, sectorNum, fatAddr0); //read total fat to dram 
		bank =0;
    }
	dumpData(512, fatAddr0, 1);
	do
	{
		dbgWdtFeed(20000);
		if(FindAddr>FindEnd)//switch cluster
		{//read fdb table of dir
			if(fdbFlag0)//write cluster to storage
			{
				if( 0 == DOS_SectorWrite(DevId, SectorStart, SectorNum, dosTmpBuf)) 
					return dosRwErr(__FILE,(unsigned short)__LINE__);
				fdbFlag0 = 0;
				
				//cluster = FAT_TableRead(DevId, cluster);
				printf("w0\n");
				//break;
			}
			if(idx>=cnt)return dosRwErr(__FILE,(unsigned short)__LINE__);
			cluster = fdbCluster[idx++];
			if((cluster > disk->MaxClusterNum-1) || (cluster < 2) || cluster == disk->FAT_EOFValue){
				break;
			}
			else {	
				printf("r0\n");
				SectorStart = FAT_Cluster2Sector(DevId, cluster);
				if( 0 == DOS_SectorRead(DevId, SectorStart, SectorNum, dosTmpBuf)) 
					return dosRwErr(__FILE,(unsigned short)__LINE__);
			}			
			
			//FindNum = SectorNum << 4;	//16 FDB per sector
			FindAddr= dosTmpBuf;	//start from name[0]
		}
		//HAL_DramReadBytesAddr(FindAddr,pt,UINT32_cnt,UINT8_fmt)
		
		if((UINT16)pFdb>=(UINT16)buf+1024)
		{
			if(fdbFlag1)
			{
				HAL_DramDmaWrite(buf,FindAddr,1024,1);
				fdbFlag1 = 0;
				fdbFlag0 = 1;				
				printf("w1\n");
			}
			
			HAL_DramDmaRead(buf,FindAddr,1024,1);
			FindAddr+=512;
			pFdb = (x86_FDB xdata*)buf;
			printf("r1\n");
		}		
		val = ((UINT8 xdata*)pFdb)[0];
		memcpy(names,pFdb->name,11);names[11]=0;
		printf("%s,pt(%x,%x)addr(%lx,%lx)%bx\n",names,(UINT16)pFdb,(UINT16)buf+1024,FindAddr,FindEnd,val);
		
		
		if(val == 0)break;//end search
		if((val != 0xE5)//erased
			&&(!(pFdb->attr&0x01))
			//&&(!(pFdb->attr & 0x1f))//read only/hidden/system file/volume/dir/////archieve
			&&(USR_CheckExtType(pFdb->name))
			)
		{
			
			//////////////
			//delete all
	        pFdb->name[0]= 0xE5;	//erase mark
	        WRITE8(curr,0,pFdb->starthi[1]);
			WRITE8(curr,1,pFdb->starthi[0]);
			WRITE8(curr,2,pFdb->start[1]);
			WRITE8(curr,3,pFdb->start[0]);
			fdbFlag1 = 1;
			printf("del:%lx\n",curr);
			while(curr != disk->FAT_EOFValue)
			{
				//printf("%lx\n",curr);
				if( curr < disk->FAT_FirstFreeCluster ){
		            disk->FAT_FirstFreeCluster = curr;//mantis-26586 20080620
		        }			
	            if((curr > disk->MaxClusterNum-1) || (curr < 2)){
	                next =  disk->FAT_EOFValue;
	                break;
	            }
				sector = curr>>7;
	            secidx = curr &0x8f;
	            if(sector < fatSectorNum0){  
	                addr = fatAddr0 + curr*2;
	            }else if(sector<bank||sector>=(bank+fatSectorNum1)){
	                DOS_SectorWrite(DevId, bank, fatSectorNum1, fatAddr1); 
	                DOS_SectorRead(DevId, sector, fatSectorNum1, fatAddr1);
	                bank=sector;
					addr = fatAddr1+secidx*2;
	            }else{
	                addr = fatAddr1+secidx*2+(sector-bank)*256;
	            }
	            HAL_DramStartAddrSet(addr, 1);
	            HAL_DramWordDataRead(&tmp0,&tmp1);
	            HAL_DramWordDataRead(&tmp2,&tmp3);

	            WRITE8(curr, 3, tmp0);
	            WRITE8(curr, 2, tmp1);
	            WRITE8(curr, 1, tmp2);
	            WRITE8(curr, 0, tmp3);

				HAL_DramStartAddrSet(addr, 0);
	            HAL_DramWordDataWrite(0x00, 0x00);
	            HAL_DramWordDataWrite(0x00, 0x00);
				disk->FreeCluster++;
				//curr = next;
			}      

		/////////////
		}
		pFdb++;	
		printf("%lx,%lx\n",cluster,disk->FAT_EOFValue);
	}while(cluster != disk->FAT_EOFValue);
	if(fdbFlag1)
	{
		HAL_DramDmaWrite(buf,FindAddr,1024,1);
		fdbFlag0 = 1;
	}		
	if(fdbFlag0)//write cluster to storage
	{
		if( 0 == DOS_SectorWrite(DevId, SectorStart, SectorNum, dosTmpBuf))
			return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	if(clusterNum > fatNum0)
    {    
    	printf("fatW:%bx,%lx,%lx,%lx\n",DevId, fatStartSector, fatSectorNum0, fatAddr0);
      	printf("fatW:%bx,%lx,%lx,%lx\n",DevId, bank, fatSectorNum1, fatAddr1);
      
        if( 0 == DOS_SectorWrite(DevId, fatStartSector, fatSectorNum0, fatAddr0)) 
			return dosRwErr(__FILE,(unsigned short)__LINE__);
        if( 0 == DOS_SectorWrite(DevId, bank, fatSectorNum1, fatAddr1)) 
			return dosRwErr(__FILE,(unsigned short)__LINE__);
    }
    else
    {
    	printf("fatW:%bx,%lx,%lx,%lx\n",DevId, fatStartSector, sectorNum, fatAddr0);
        if( 0 == DOS_SectorWrite(DevId, fatStartSector, sectorNum, fatAddr0)) 
			return dosRwErr(__FILE,(unsigned short)__LINE__);
    }
	dumpData(512, fatAddr0, 1);
	printf("%x,%lx\n",disk->FAT_Bank2Sector,disk->FAT_Bank2Cluster);
	DOS_SectorRead(DevId, disk->FAT_StartSector + (disk->FAT_CurrBank<< disk->FAT_bitBank2Sector), disk->FAT_Bank2Sector, disk->FAT_BufAddr); //read total fat to dram
	dumpData(512, disk->FAT_BufAddr, 1);
	return TRUE;
}
#endif

#if 0
UINT8 vfsFileDeleteAll(void)//(UINT8 matchCond, UINT32 refCon)
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
		dbgWdtFeed(10000);
		ret = DIR_Search(DevId, "*", (FDB *)&fdb, FILE_INFO_GET_SIZE);
      //  printf("search ret=%bu\n",ret);
		if(ret==FIND_FILE_FAIL)	//can't find more file
    {   // printf("search fail\n");  
			break;
          }
		if(ret==FIND_FILE_OK) {
			//printf("name=%s\n",fdb.name);
          //  printf("ret=%bu\n",ret);
			//cx modify mantis #0041266
			ret = USR_CheckExtType(fdb.name);//USR_MatchFdb(matchCond, (UINT8 *)&(fdb.name), refCon);
			//printf("ret=%bu\n",ret);
			if (ret && !(fdb.attr & 0x03)){//Write Protect
				if( fdb.size < 0x100000 ){ //if file size < 1MB, use DOS_FileDelX flow
					DOS_FileDelX(&fdb, 0);
    					continue;
				}
				DOS_FileDelStop();
				ret = DOS_FileDel(fdb.name);
				DOS_FileDelReset();
			}
			
			//else{
			//	printf("not del %s\n",fdb);
			//}
			
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
#endif
/**
 * @fn        UINT16 vfsFileOpen(UINT8 *name, UINT8 rwMode, UINT16 numCluster)
 * @brief     Open a file.
 * @param     [in] name			The name of the file to read.
 * @param     [in] rwMode 		File access mode, it can be K_DOS_NewFile, K_DOS_ReadFile and K_DOS_NewFile_LinearFAT.
 * @param     [in] numCluster 	Not used.
 * @retval    File handle. The value is bigger than 1 if open file success.
 * @see       NULL
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

/**
 * @fn        UINT8 vfsFileRead(UINT16 fHandle, UINT8 dstPath, UINT32 readSize, UINT32 ramAddr, UINT32 fpos)
 * @brief     Read block of data from a file.
 * @param     [in] fHandle	The handle of the file to read.
 * @param     [in] dstPath	Not used.
 * @param     [in] readSize	Read size in bytes.
 * @param     [in] ramAddr	Dram address in word units.
 * @param     [in] fpos		Start offset to read.
 * @retval    return = TRUE / FALSE
 * @see       NULL
*/
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
/**
 * @fn        UINT8 vfsFileWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage)
 * @brief     Write block of data to a file.
 * @param     [in] fHandle			The handle of the file to write.
 * @param     [in] srcPath			Not used.
 * @param     [in] writeSize			Size in bytes.
 * @param     [in] ramAddr			Dram address in word units.
 * @param     [in] findHeaderPage	Not used.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
*/
UINT8 vfsFileWrite(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage)
{
	return File_LWrite( fHandle,  srcPath,  writeSize,  ramAddr,  findHeaderPage);
}
UINT8 File_LWriteSD(UINT16 fHandle, UINT8 srcPath, UINT32 writeSize, UINT32 ramAddr, UINT8 findHeaderPage)
{
	return File_LWrite( fHandle,  srcPath,  writeSize,  ramAddr,  findHeaderPage);
}
/**
 * @fn        UINT8 vfsFileClose(UINT16 fHandle, UINT8* name, UINT8 rwMode, UINT32 fileSize)
 * @brief     Close a file
 * @param     [in] fHandle	The handle of the file to be closed.
 * @param     [in] name		Not used
 * @param     [in] rwMode	Not used
 * @param     [in] fileSize	Not used
 * @retval    return = TRUE
 * @see       NULL
*/
UINT8 vfsFileClose(UINT16 fHandle, UINT8* name, UINT8 rwMode, UINT32 fileSize)
{
	DOS_FileClose(fHandle);
	return TRUE;
	name = name;
	fileSize = fileSize;
	rwMode = rwMode;
}

/**
 * @brief		vfsFileAttrSet
 * @param	fno, option, pData
 * @retval	SUCCESS / FAIL
 * @see
 * @author	Phil.lin
 * @since		08/08/21
 * @todo
 * @bug
*/
UINT8 vfsFileAttrSet(
	UINT8 fno,
	UINT8 option,
	void* pData
)
{
//	UINT8 ret;
	UINT8 DevId;
	//UINT8 *path, *name;
	//UINT32 max, curr, next;
	xdata FILE *file;
	xdata x86_FDB *fdb;//, tmpFdb;
	//xdata DISK *disk;
	UINT8 i8;
	UINT32 i32;
	UINT8 xdata* ptr;
	bgRwAbort();
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

	#if 0 //xian --, 20120105, move to dcfFileAttrSet(), and replace with dcfFileDramFdbUpdate() to write fdb buffer directly 
	if((option & ATTR_MODIFY_NAME) == 0){//xian ++ 20110301, G_USR_FileCount error, so should init dcf file info by caller
		G_DOS_FdbAddress = ((UINT32)file->FDB_Sector << 4) + ((UINT32)file->FDB_Idx);////mantis-26979
		//printf("file->FDB_Sector: %lx\n", file->FDB_Sector);
		//printf("file->FDB_Idx %bx \n", file->FDB_Idx);
		//printf("G_DOS_FdbAddress: %lx\n", G_DOS_FdbAddress);
		ret = USR_CheckType((UINT8 *)fdb);
	}
	#endif
	 bgRwResume();
	return SUCCESS;
}

#if 0  //move to dcf lib
/**
 * @fn        UINT8 vfsFileAttrSet(UINT8 const* oldFileName, UINT8 const* newFileName, UINT8 attr, UINT8 option)
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

UINT8 vfsFileJoint(UINT16 head, UINT16 tail)
{	
	return DOS_FileJoint((UINT8)head, (UINT8)tail); //DOS_SUCCESS
}

UINT8 vfsFileSeek(UINT16 fno, UINT32 offset, UINT8 mode)
{
	return DOS_FileSeek((UINT8)fno, offset, mode);
}

UINT8 vfsDirRemove(UINT8 *pathName)
{
	return DIR_Remove(pathName);
}

UINT32 vfsFileSizeGet(UINT16 fno)
{
	return G_DOS_FileSize;
	fno = fno;
}

UINT32 vfsClusterSizeGet(void)
{
	return G_DOS_ClusterSize;
}

UINT8 vfsFileMove(UINT8 *srcName, UINT8 *dstName)
{
	UINT8 fno_s, fno_d;
	UINT8 devId;
	xdata FILE *file_s, *file_d;
	xdata x86_FDB *fdb;
	UINT8 i8;
	UINT8 *pt1, *pt2;
	UINT32 i32;
	
	devId = DOS_Path2Drv(srcName);
	if(devId != DOS_Path2Drv(dstName)){
		return FAIL;
	}

	fno_s = DOS_FileOpen(srcName, 'r');
	if(!fno_s){
		return FAIL;
	}
	fno_d = DOS_FileOpen(dstName, 'w');
	if(!fno_d){
		DOS_FileClose(fno_s);
		return FAIL;
	}

	file_s = (FILE *)&dos.file[fno_s];
	file_d = (FILE *)&dos.file[fno_d];

	file_d->FirstCluster = file_s->FirstCluster;
	file_d->pos = file_s->pos;
	file_d->size = file_s->size;

	pt1 = (UINT8*)&file_s->fdb;
	pt2 = (UINT8*)&file_d->fdb;
	for(i8 = 11; i8 < 32; i8 ++){ //copy fdb except name
		pt2[i8] = pt1[i8];
		//printf("%bx ", pt2[i8]);
	}
	//printf("\n");

	//DIR_AddFDB(fno_d);
	if( 0 == DOS_SectorRead(devId, file_d->FDB_Sector, 1, dos_var_start)){
		return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	i32 = ((UINT32)file_d->FDB_Idx << 4) + dos_var_start;
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(i32,3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(i32,2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(i32,1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(i32,0);
	for(i8=0; i8<32/2; i8++) {
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		XBYTE[REG_Dram_IndirectRData_Low] = *pt2++;
		XBYTE[REG_Dram_IndirectRData_High] =	*pt2++;
	}
	if( 0 == DOS_SectorWrite(devId, file_d->FDB_Sector, 1, dos_var_start)) {
		return dosRwErr(__FILE,(unsigned short)__LINE__);
	}
	
	FAT_TableWriteBack(devId);
	file_d->status = 0;
	file_d->DevId = -1; //free this file handle

	//erase the source file
	fdb  = (x86_FDB *)&file_s->fdb;
	fdb->name[0] = 0xE5;
	DIR_AddFDB(fno_s);
	file_s->DevId = -1; //free this file handle

	return SUCCESS;
}

