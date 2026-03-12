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
 * @author    XianXin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "sd_bg.h"
#include "sd_def.h"
#include "reg_def.h"
#include "hal.h"
#include "hal_dram.h"
#include "hal_sd.h"
#include "intr.h"
#include "Dos32.h"
#include "GPIO.h"
#include "dbg_wdt.h"
#include "sd_bg_def.h"
#define __FILE __FILE_ID_SD_BG__
#if SD_BG_EN
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
#define BgNodeCount  100
BgNode_t bgNodes[BgNodeCount];
UINT8 bgNode,bgNodeLast;//,bgStatus=0;
//UINT8 bgStatus,bgAbort;
UINT8 bgAbort = 0;
//UINT8 bgEnable=0;
UINT8 bgStatus=0;// 0 stop, 1 run, 2 abort
UINT32 bgMaxSec = 1024;//1024*32;//
//UINT32 bgTrigSize = 0;
UINT32 bgSaveSize;// _at_ 0x3700;
UINT32 bgLastSize;//=0;
BgFileObj_t bgFileFat;
UINT8 bgIntrReg;
UINT32 bgIntrTime;
//UINT32 bgRunTime0,bgRunTime1;
//UINT8 bgTestMode = 0;
//UINT8 bgTestTimeEn = 0;
//UINT32 bgTestSize = 0,bgTestTime=0;
//UINT8 bgTest000 = 0;
//UINT32 bgTestSave = 0;//150ul*1024;
//UINT8 bgTestTrigCnt = 0,bgTestAddCnt=0;
//UINT32 bgTestCnt0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               D E B U G               *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void bgPrintfNodes(BgFileObj_t *bgfile){
	if(bgfile==NULL)bgfile = &bgFileFat;
	//printf("(%lu,%lu)(%u,%u)\n",bgfile->listCnt,bgfile->listMax,(UINT16)bgfile->listNode,(UINT16)bgfile->list);
	//printf("%lu(%u,%lu,%lu)%bu\n",bgfile->listNodeIdx,bgfile->fsizeErr,bgfile->fsize,bgfile->fsizeMax,bgfile->mode);
	//printf("(%lu,%lu)\n",bgfile->clusterFirst,bgfile->clusterLast);
	printf("(%lx,%lx)(%lx,%lx)\n",bgfile->clusterFirst,bgfile->clusterLast,bgfile->fsize,bgfile->fsizeMax);	
}
UINT8 bgSetSpace(UINT8 xdata* buf,UINT32 bufLen,UINT32 fileSize)//kb
{
	DISK xdata* disk = dos.disk+dos.CurrDrv;
	UINT32 count;
	UINT32 freeCnt = disk->FreeCluster;
	UINT32 t0,t1;

	bgTimerGet(&t0);
	//memset(&bgFileFat,0,sizeof(bgFileFat));
	if(fileSize>=0x400000){fileSize=0x3fffff;}
	count = (fileSize>>(disk->bitCluster2Byte-10))+1;//(fileSize+disk->Cluster2Byte-1)>>disk->bitCluster2Byte;
	bgPrintfImport("bgSetSpace:%lx,%lx,%lx,%lx\n",freeCnt,count,fileSize,disk->FAT_FirstFreeCluster);	
	if(count>freeCnt){count = freeCnt;}
	bgFileFat.list = (BgClusters_t xdata*)buf;
	bgFileFat.listMax = bufLen/8;
	bgCreateList(&bgFileFat,count);
	bgTimerGet(&t1);	
	bgPrintfTimer(" setSpace:%lu\n",t1-t0);	
	t1=t0;//clean warning
	return bgFileFat.fsizeMax?1:0;
}
UINT32 bgGetFree(BgFileObj_t *bgfile)
{	
	if(bgfile==NULL)bgfile=&bgFileFat;
	if(!bgfile->fsizeMax)return (UINT32)-1;
	return bgfile->fsizeMax-bgfile->fsize;
}
void bgInit()
{
	bgPrintfImport("bgInit\n");

	HAL_FlashMode(G_Card_Type, K_MEDIA_NANDF, 1);
	HAL_SDPhaseChange(2);//HAL_SD_PHASE_HI_WRITE);
	memset(bgNodes,0,sizeof(bgNodes));
	memset(&bgFileFat,0,sizeof(bgFileFat));
	bgNode=bgNodeLast =0;

	//bgEnable = 1;
	bgStatus=0;
	bgAbort=0;
	bgSaveSize=0;
	bgLastSize = 0;
	//bgTrigSize = 0;

	XBYTE[REG_Fm_intEn] &=~4;
	bgIntrReg = XBYTE[REG_Fm_intEn] ;

	bgIntrTime = 0;
	//bgRunTime0 = bgRunTime1 = 0;
}
void bgStop()
{	
	DISK xdata* disk = dos.disk+dos.CurrDrv;
	bgPrintfImport("bgStop\n");
	bgWait(100000,__FILE,__LINE__);
	HAL_DmaReset();
	bgStatus = 0;
	//printf("fsize:%lx/%lx-%lx\n",bgFileFat.fsize,bgFileFat.fsizeMax,disk->FreeCluster);
	if(bgFileFat.fsizeMax){	bgModifyDisk(&bgFileFat);}
	XBYTE[REG_Fm_intEn] &=~4;
	bgPrintfImport("bgStop++%lu\n",bgIntrTime);
}
void bgRwResume()
{
	if(bgStatus!=2){return;	}
	bgPrintfImport("bgResume\n");
	bgRwTrig();
}
void bgRwAbort()
{	
	if(bgStatus!=1){return;}
	bgPrintfImport("bgAbort\n");
	bgAbort = 1;	
	bgWait(100000,__FILE,__LINE__);
	bgAbort = 0;	
	bgStatus = 2;	
}
#pragma disable //disable interrupt
void bgRwTrig()
{
	UINT8 ret;
//	UINT32 tt0,tt1;
	BgNode_t xdata* bn;
	//intrPutChar('T');
	if(sD_CdGet()){return ;}
#if BG_INTR_TIMR_ENABLE
	WRITE8(tt0, 0, XBYTE[0x20C4]);
	WRITE8(tt0, 1, XBYTE[0x20C3]);
	WRITE8(tt0, 2, XBYTE[0x20C2]);
	WRITE8(tt0, 3, XBYTE[0x20C1]);
#endif
NEXT:
	bn = bgNodeGet();	
	if((bn->status&1) )//&& XBYTE[REG_Fm_dramfmread_dmasel] & 0x10 && XBYTE[REG_Dma_DmaCmp] )
	{
		UINT32 addr = bn->addr;
		UINT32 sector = bn->sector;
		UINT32 size = bn->size;
		UINT32 max = bgMaxSec<<9;//G_SD_MultiSecNum<<9;
		if(bgAbort){
			bn->status |= 4;			
			ret = HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);
			if(ret==0){sdWaitDat0High();}//INTR_putchar('S');}	
			if(sdModePostIdle){HAL_SDTxDummy();}
			XBYTE[REG_Fm_intEn] &= ~4;
			bgStatus = 0;
			bgIntrReg = XBYTE[REG_Fm_intEn] ;

			if(bn->size==0){
				bn->status = 0;
				bgNodeNext();	
			}
			
			return;
		}
		if(bn->status&4){	
			if(sdByteMode){sector<<=9;}
			XBYTE[REG_Fm_SD_diphsel_48m_inclkphsel] = G_SD_WritePhase;	
			while(HAL_SDTxCommand(25, sector, RSP_TYPE_R1, RspBuf)){
				//INTR_putchar('X');
				ret = HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);
				if(ret==0){sdWaitDat0High();}//INTR_putchar('S');}	
				if(sdModePostIdle){HAL_SDTxDummy();}
				if(sD_CdGet())return;
			}
			bn->status&=~4;
			bgputcharIntr('A');
			//XBYTE[REG_Fm_evt] |= 4;	
			XBYTE[REG_Fm_intEn] |= 4;	//SD controller interrupt enable
		}
		if(bn->size==0){
			ret = HAL_SDTxCommand(12, 0, RSP_TYPE_R1, RspBuf);
			if(ret==0){sdWaitDat0High();}		
			if(sdModePostIdle){HAL_SDTxDummy();}
			bn->status = 0;
			bgNodeNext();	
			bgputcharIntr('C');
			goto NEXT;
		}
		
		if(size>max){
			size = max;
			bn->addr+=size;
			bn->sector+=bgMaxSec;
			bn->size-=size;
		}else{
			bn->size = 0;
		}
		
		INTR_HAL_DmaSetDramAddr(addr>>1);
		XBYTE[REG_Fm_evt] =0xff;
		ret = INTR_HAL_DmaDo(DMA_PATH_DRAM,DMA_PATH_FLASH,size,1,0);
		//intrPutUint8(bgNode);
		//bgTrigSize+=1024;
		if(bgSaveSize>=bgLastSize){
			bgSaveSize -= bgLastSize;
		}else {
			intrPutChar('E');
			//bgDbgPrintf(1);
			//while(1);
		}
		bgLastSize=size;
		bgStatus=1;
		bgputcharIntr('Y');
        }
	else{		
		if(bgSaveSize>=bgLastSize){
			bgSaveSize -= bgLastSize;
		}
		else {
			intrPutChar('E');
			//bgDbgPrintf(2);
			//while(1);
		}
		bgLastSize=0;
		XBYTE[REG_Fm_intEn] &= ~4;	
		bgputcharIntr('X');
		bgStatus=0;
	}
	bgIntrReg = XBYTE[REG_Fm_intEn] ;
	//intrPutChar('t');
	//intrPutChar('\n');
#if BG_INTR_TIMR_ENABLE
	WRITE8(tt1, 0, XBYTE[0x20C4]);
	WRITE8(tt1, 1, XBYTE[0x20C3]);
	WRITE8(tt1, 2, XBYTE[0x20C2]);
	WRITE8(tt1, 3, XBYTE[0x20C1]);
	if(tt1>tt0){
		tt1-=tt0;
		if(bgIntrTime<tt1)
			bgIntrTime=tt1;
	}
#endif
}

UINT32 bgFileOpen(BgFileObj_t *bgfile,UINT32 size,UINT8* fname)//kb
{
	UINT8 dev = dos.CurrDrv;//DOS_CurrDriverGet();//file->DevId;
	DISK xdata* disk = dos.disk+dev;
	UINT32 count;
	UINT32 freeCnt = disk->FreeCluster+bgFileFat.fsizeMax;

	UINT32 t0,t1;

	bgTimerGet(&t0);
	if(size>=0x400000){size=0x3fffff;}
	count = (size>>(disk->bitCluster2Byte-10))+1;//(size+disk->Cluster2Byte-1)>>disk->bitCluster2Byte;
	bgPrintfImport("bgOpen:%lx,%lx,%lx\n",freeCnt,count,size);	
	if(count>freeCnt){count = freeCnt;}
	if(fname){memcpy(bgfile->fname,fname,13);}
	//if(!bgfile->list){bgfile->mode = 0;}
	bgfile->fsize = 0;
	bgfile->fsizeMax = count;
	
	if(bgfile->mode == 0){//readFat + writeFat	
		DOS_FileDel(fname);
		bgfile->fno = DOS_FileOpen( fname, 'w');
	}else if(bgfile->mode ==1){//readList + writeList
		bgCreateList(bgfile,count);
		bgfile->fno = 0;
	}else if(bgfile->mode ==2){//readList + WriteFat
		bgCreateList(bgfile,count);
		DOS_FileDel(fname);
		bgfile->fno = DOS_FileOpen( fname, 'w');
	}else if(bgfile->mode ==3){//readSpace + WriteFat
		DOS_FileDel(fname);
		bgfile->fno = DOS_FileOpen( fname, 'w');
	}else if(bgfile->mode ==4){//readSpace + WriteList
		//bgfile->fno = DOS_FileOpen( fname, 'w');
		bgfile->listNode = bgfile->list;
	}		
	
	bgTimerGet(&t1);	
	bgPrintfTimer(" bgOpen:0x%lx,%lu\n",size,t1-t0);		
	return count<<disk->bitCluster2Byte;
	t1=t0;//clean warning
}
#if 0
UINT8 bgFileClose(BgFileObj_t *bgfile,UINT8* fname,UINT8 isClose)
{
	UINT32 t0,t1;
	bgTimerGet(&t0);
	if(fname){	memcpy(bgfile->fname,fname,13);	}	
	
	bgWait(100000,__FILE,__LINE__);
	if(bgfile->mode==0){
		if(isClose){DOS_FileClose(bgfile->fno);}	
	}else if(bgfile->mode==1){
		bgCreateFat(bgfile);
		bgCreateFile(bgfile,isClose);
		bgModifyDisk(bgfile);
	}else if(bgfile->mode==2){
		if(isClose){DOS_FileClose(bgfile->fno);}
	}else if(bgfile->mode==3){
		if(isClose){DOS_FileClose(bgfile->fno);}
	}else if(bgfile->mode==4){
		bgCreateFat(bgfile);
		bgCreateFile(bgfile,isClose);
	}
	
	bgTimerGet(&t1);

	bgPrintfImport("bgClose:%s ",bgfile->fname);
	bgPrintfTimer(" time:%lu\n",t1-t0);	 	
	return bgfile->fno;
	t1=t0;//clean warning
}
#endif
UINT8 bgFileJoint(BgFileObj_t *bgfile,UINT8* fname,UINT8 isClose)
{	
	//UINT32 t0,t1,t2,t3,t4,t5,t6;
	BgFileObj_t xdata* file = bgfile;
	//HAL_GlobalReadGTimer(&t0);
	//UINT8 fno = bgfile->fno;
	if(fname){memcpy(bgfile->fname,fname,13);}
	bgWait(100000,__FILE,__LINE__);
	while(1){
		//HAL_GlobalReadGTimer(&t1);
		bgPrintfNodes(file);		
		switch(file->mode){
		case 0:
			//if(isClose){DOS_FileClose(bgfile->fno);}	
			break;
		case 1:
			bgCreateFat(file);
			//bgCreateFile(bgfile,isClose);
			bgModifyDisk(file);
			break;
		case 2:
			//if(isClose){DOS_FileClose(bgfile->fno);}
			break;
		case 3:
			//if(isClose){DOS_FileClose(bgfile->fno);}
			break;
		case 4:
			bgCreateFat(file);
			//bgCreateFile(bgfile,isClose);
			break;
		default:
			break;
		}
		//HAL_GlobalReadGTimer(&t2);
		if(file->nextObj){//joint
		
			printf("c:%lx->%lx\n",file->clusterLast, file->nextObj->clusterFirst);
			FAT_TableWrite(dos.CurrDrv, file->clusterLast, file->nextObj->clusterFirst);			
			file = file->nextObj;
			bgfile->fsize+=file->fsize;
			bgfile->fsizeErr = file->fsizeErr;
			bgfile->clusterLast = file->clusterLast;
			if(file->fno){
				if(!bgfile->fno){
					bgfile->fno = file->fno;		
					//dos.file[fno].FirstCluster = bgfile->clusterFirst;
					memcpy(dos.file[bgfile->fno].fdb.name ,bgfile->fname,11);
				}else{
					dos.file[file->fno].fdb.name[0] = 0xE5;
					DIR_AddFDB(file->fno);
				}
			}
		}else{
			break;
		}
		//HAL_GlobalReadGTimer(&t3);printf("+%lu,%lu\n",t2-t1,t3-t2);
		
	}
	FAT_TableWriteBack(dos.CurrDrv);
	//bgfile->fno = fno;
	//HAL_GlobalReadGTimer(&t5);
	bgCreateFile(bgfile,isClose);
	//HAL_GlobalReadGTimer(&t6);
	//printf("%lu+%lu=%lu\n",t5-t0,t6-t5,t6-t0);
	return 1;
}
UINT32 bgFileWrite(BgFileObj_t *bgfile,UINT32 addrbyte,UINT32 size, UINT32 minFreeList)
{
	UINT32 sector ;
	BgNode_t xdata* bn=bgNodeLastGet();
	UINT8 dev = dos.CurrDrv;//DOS_CurrDriverGet();//file->DevId;
	DISK xdata* disk = dos.disk+dev;
	UINT32 cnt= (size+disk->Cluster2Byte-1)>>disk->bitCluster2Byte;
	UINT32 cnt0 = cnt;
	UINT32 bnCnt;
	UINT32 timeout;
	//extern UINT8 vidTestCnt;
	//putchar('W');	
	if(sD_CdGet()){return 0;}
	if(!cnt){
		__PRINTF_POS();
		return 0;
	}else if(bgfile->listCnt+minFreeList>=bgfile->listMax){
		__PRINTF_POS();
		return 0;		
	}else if(bgFileFat.fsizeMax && (bgFileFat.fsize+ cnt>bgFileFat.fsizeMax) ){
		__PRINTF_POS();
		return 0;
	}else if(bgfile->fsize + cnt<=bgfile->fsizeMax){
	}else {
		__PRINTF_POS();
		return 0;
		//cnt= bgfile->fsizeMax - bgfile->fsize;
		//size = cnt<<disk->bitCluster2Byte;
	}
	bgfile->fsize+=cnt;
	bgfile->fsizeErr = (cnt<<disk->bitCluster2Byte)-size;

	while(cnt && bgfile->listCnt+minFreeList<bgfile->listMax)
	{			
		bn=bgNodeLastGet();
		timeout = 0x10000;
		while(bgStatus && (bn->status&1) && --timeout && (bgNodeLast == bgNode));//{if(!(timeout&0xf))putchar('^');}
		if(!timeout){
			//putchar('!');		
			__PRINTF_POS();
			bgRwTrig();
			timeout = 0x1000;
			while(bgStatus && (bn->status&1) && --timeout && (bgNodeLast == bgNode));//{if(!(timeout&0xf))putchar('o');}
			if(!timeout){
				__PRINTF_POS();
			}
		}
		
		if		(bgfile->mode==0)	{
			bnCnt = FAT_FreeClusterGet(dev, cnt, &sector);
			bgFillFat(bgfile->fno, sector, bnCnt);
		}else if	(bgfile->mode==1)	{
			bnCnt = bgFreeClusterGet(bgfile, cnt, &sector);
		}else if	(bgfile->mode==2)	{
			bnCnt = bgFreeClusterGet(bgfile, cnt, &sector);
			bgFillFat(bgfile->fno,sector,bnCnt);
		}else if	(bgfile->mode==3)	{
			bnCnt = bgFreeClusterGet(&bgFileFat, cnt, &sector);
			bgFillFat(bgfile->fno,sector,bnCnt);
			bgFileFat.fsize+=bnCnt;
			
		}else if	(bgfile->mode==4)	{
			bnCnt = bgFreeClusterGet(&bgFileFat, cnt, &sector);
			bgfile->listNode->cnt=bnCnt;
			bgfile->listNode->start=sector;
			bgfile->listNode++;
			bgfile->listCnt++;
			bgFileFat.fsize+=bnCnt;

		}
		if(!bnCnt){__PRINTF_POS();break;}	
		if(sector<2){__PRINTF_POS();break;}	
		bgfile->clusterLast = sector+bnCnt-1;
		if(!bgfile->clusterFirst){	
			bgfile->clusterFirst = sector;
				//=(bgfile->fno)?
				//	(dos.file[bgfile->fno].FirstCluster):
				//	(bgfile->list->start);	
			printf("-1st:%lx(%bx,%lx)%lx\n",bgfile->clusterFirst ,bgfile->fno,dos.file[bgfile->fno].FirstCluster,bgfile->list->start);
		}
		sector = FAT_Cluster2Sector(dev, sector);
		
		//printf("%lu,%lu,%lu\n",c,cc,idx);
		cnt-=bnCnt;
		
		bn->addr = addrbyte;
		bn->sector = sector;
		bn->size = bnCnt<<disk->bitCluster2Byte;
		bn->status = 0xff;
		addrbyte+=bn->size;
		bgNodeAdd();

	}
	//cnt<<=disk->bitCluster2Byte;
	//if(size>cnt){size-=cnt;}
	//else {size = 0;}	
	size = (cnt0-cnt)<<disk->bitCluster2Byte;

	BG_ENTER_CRITICAL();
	bgSaveSize += size;
	BG_EXIT_CRITICAL();
	
	//if(!vidTestCnt)	{bgDbgPrintf(size);}
	if(!bgStatus)		{bgRwTrig();}
	
	return size;
}

UINT8 bgWait(UINT32 timeout,UINT32 file,UINT32 line)
{
	dbgWdtFeed(-1);
	while(bgStatus && --timeout && !sD_CdGet()); //timeout=100, t=~2270us=2~3ms
	dbgWdtFeed(-1);
	if(!timeout){__PRINTF(file,line);return FALSE;}
	else 	{return TRUE;}
}
/*UINT32 bgLastClusterGet(BgFileObj_t *bgfile)
{
	BgClusters_t xdata* node = bgfile->listNode;
	UINT32 last = 0;//bgfile->listNode->start+bgfile->listNodeIdx;
	UINT32 cnt = bgfile->listNodeIdx;
	if(bgfile->fsize>=bgfile->fsizeMax){last=0;}
	else if(!cnt){node--;last = node->start+node->cnt-1;}
	else {last = node->start + cnt-1;}
	return last;
}*/
void bgCreateList(BgFileObj_t *bgfile,UINT32 count)
{
	UINT32 cnt=0;
	UINT32 freeCnt,cluster;
	UINT32 useCnt=0;
	UINT8 dev = dos.CurrDrv;
	DISK xdata* disk = dos.disk+dev;
	BgClusters_t xdata* list = bgfile->list;
	BgClusters_t xdata* node = list;
	memset(bgfile->list,0,bgfile->listMax*8);
	//node--;
	node->start=0;
	node->cnt=0;
	while(count)
	{	
		freeCnt = FAT_FreeClusterGet(dev, count, &cluster);// only return continious free block
		//printf("%lu,%lu\n",cluster,freeCnt);	
		if( freeCnt == 0 ){	__PRINTF_POS();	break;}		
		if( cluster<2|| cluster>=disk->MaxClusterNum){	
			__PRINTF_POS();break;
		}		
		if( count >= freeCnt ){count-=freeCnt;}
		else{freeCnt = count; count=0;__PRINTF_POS();}
		
		if(node->start+node->cnt == cluster){
			node->cnt += freeCnt;
		}else{
			if(cnt+1>=bgfile->listMax){__PRINTF_POS();break;}
			if(cnt++){node++;}			
			
			node->start = cluster;
			node->cnt = freeCnt;			
		}
		disk->FAT_FirstFreeCluster=cluster+freeCnt;
		useCnt+=freeCnt;	
		
	}
	bgfile->fsizeMax = useCnt;
	bgfile->listCnt = cnt;
	bgfile->listNodeIdx = 0;
	bgfile->listNode = list;	
	bgfile->fsize = 0;
	//printf("fsize:%lx/%lx-%lx\n",bgfile->fsize,bgfile->fsizeMax,disk->FreeCluster);
}
void bgCreateFat(BgFileObj_t *bgfile)
{
	UINT8 dev = dos.CurrDrv;
	DISK xdata* disk = dos.disk+dev;
	BgClusters_t xdata* list = bgfile->list;
	BgClusters_t xdata* node = list;
	BgClusters_t xdata* nodeLast = bgfile->listNode;
	UINT32 cnt,i,next,c=0,last=0,curr=0;
	for(node = list;node<=nodeLast;node++)
	{
		if(node == nodeLast){
			cnt = bgfile->listNodeIdx;
			if(bgfile->fsize>=bgfile->fsizeMax){last=0;}
			else if(cnt==node->cnt){last = node[1].start;}
			else {last = node->start + cnt;}
		}else {
			cnt = node->cnt;
		}
		next = node->start;
		for(i=0;i<cnt;i++)
		{
			//printf("%lu-%lu\n",c,next);
			if(curr){
				FAT_TableWrite(dev, curr, next);
			}	
			curr=next++;
			c++;
		}	
	}
	FAT_TableWrite(dev, curr, disk->FAT_EOFValue);
	//bgfile->cluster = curr;
	//printf("curr:%lx,%lx\n",curr,bgfile->clusterLast);
	//bgPrintfImport("(%lu,%lu,%lu,%x)",c,bgfile->fsize,bgfile->fsizeMax,bgfile->fsizeErr);
	if(c!=bgfile->fsize){__PRINTF(__FILE, (unsigned short)__LINE__);bgfile->fsize=c;}
	//if(last && disk->FAT_FirstFreeCluster>last){disk->FAT_FirstFreeCluster = last;	}
	//disk->FreeCluster+=bgfile->fsizeMax-bgfile->fsize;//dosfile->size>>disk->bitCluster2Byte;
}
void bgCreateFile(BgFileObj_t *bgfile,UINT8 isClose)
{
	FILE xdata*file;
	BgClusters_t xdata* list = bgfile->list;
	DISK xdata* disk = dos.disk+dos.CurrDrv;
	BgClusters_t xdata* last = bgfile->listNode;
	if(bgfile->fno==0){
		DOS_FileDel(bgfile->fname);
		bgfile->fno = DOS_FileOpen(bgfile->fname, 'w');
		if(bgfile->fno==0){__PRINTF_POS();return;}
	}
	file = dos.file + bgfile->fno;
	file->size=(bgfile->fsize<<disk->bitCluster2Byte)-bgfile->fsizeErr;
	file->FirstCluster= bgfile->clusterFirst;//list->start;
	file->CurrCluster= bgfile->clusterLast;//bgLastClusterGet(bgfile);//bgfile->listNode->start+bgfile->listNodeIdx;//cluster->cluster+bgfile->idx;//bgfile->cluster;//
	file->LastCluster = file->CurrCluster;
	file->pos=file->size;
	file->CurrSector=-1;
	//printf("f:%lx,%lx\n",file->FirstCluster,file->LastCluster);
	/*printf("file%bu:%lx-%lx\n",fno,file->size,c);*/
	//if(isClose)DOS_FileClose(bgfile->fno);
	if(isClose){
		DIR_AddFDB(bgfile->fno);		
		//FAT_TableWriteBack(DevId);
		USR_DcfFdbAddrSet(bgfile->fno);
		file->status = 0;		
		file->DevId = -1;
	}
}

void bgModifyDisk(BgFileObj_t *bgfile)
{
	DISK xdata* disk = dos.disk+dos.CurrDrv;
	BgClusters_t xdata* node = bgfile->listNode;
	UINT32 last = bgfile->clusterLast;//bgLastClusterGet(bgfile);//0;//bgfile->listNode->start+bgfile->listNodeIdx;
	//UINT32 cnt = bgfile->listNodeIdx;
	//printf("free:%lu(%lx,%lx)%lu/%lu\n",disk->FreeCluster,disk->FAT_FirstFreeCluster,last,bgfile->fsize,bgfile->fsizeMax);
	//if(bgfile->fsize>=bgfile->fsizeMax){last=0;}
	//else if(cnt==node->cnt){last = node[1].start;}
	//else {last = node->start + cnt;}
	//printf("clust(%lx,%lx)\n",disk->FAT_FirstFreeCluster,disk->FreeCluster);
	if(last && disk->FAT_FirstFreeCluster>last){disk->FAT_FirstFreeCluster = last+1;	}
	disk->FreeCluster+=bgfile->fsizeMax-bgfile->fsize;//dosfile->size>>disk->bitCluster2Byte;
	//printf("free:%lx(%lx,%lx)%lx/%lx\n",disk->FreeCluster,disk->FAT_FirstFreeCluster,last,bgfile->fsize,bgfile->fsizeMax);

}
void bgFillFat(UINT8 fno,UINT32 next,UINT32 cnt)
{
	FILE xdata*file = dos.file+fno;
	UINT8  dev = file->DevId;
	DISK xdata*disk = dos.disk+dev;
	UINT32 freeCnt = cnt;//FAT_FreeClusterGet(dev, cnt, next);
	UINT32 cluster = file->CurrCluster;
	//cnt = freeCnt;
	if( cluster==0){
		file->FirstCluster= next;	
		printf("+1st:%lx\n",next);
	}else{
		FAT_TableWrite(dev, cluster, next);		
	}
	cluster = next;
	if(!freeCnt){
		__PRINTF_POS();
		return;
	}
	while(--freeCnt){FAT_TableWrite(dev, cluster++, cluster);}		
	FAT_TableWrite(dev, cluster, disk->FAT_EOFValue);	
	file->CurrCluster = cluster;	
	file->LastCluster = cluster;	
	file->size+=cnt<<disk->bitCluster2Byte;
	file->pos = file->size;
	//return cnt;
}
UINT32 bgFreeClusterGet(BgFileObj_t *bgfile,UINT32 cnt,UINT32* next)
{
	BgClusters_t xdata*  node = bgfile->listNode;
	UINT32 idx = bgfile->listNodeIdx;
	UINT32 freeCnt = node->cnt - idx;
	if(node>=bgfile->list+bgfile->listCnt){__PRINTF_POS();return 0;}
	if(bgfile->fsize+cnt>bgfile->fsizeMax){__PRINTF_POS();return 0;}
	*next = node->start+idx;
	if(*next<2 ){
		__PRINTF_POS();
		//bgPrintfNodes(bgfile);
		//bgPrintfNodes(&bgFileFat);
		return 0;
	}
	if(freeCnt<=cnt){
		cnt = freeCnt;
		node++;
		idx = 0;
	}else{
		idx+=cnt;		
	}
	bgfile->listNodeIdx = idx;
	bgfile->listNode = node;
	if(!bgfile->clusterFirst){
		bgfile->clusterFirst = *next;
	}
	if(cnt){
		bgfile->clusterLast = *next+cnt-1;
	}
	return cnt;//node->start+idx;
}

UINT32 bgSaveSizeGet()
{
	UINT32 size;
	BG_ENTER_CRITICAL();
	size = bgSaveSize;
	BG_EXIT_CRITICAL();
	return size;
}
/*#pragma disable 
void bgDbgPrintf(UINT32 val)
{
	printf("(%bu,%bu)(%bu,%bu)(%lu,%lu)%lu-%lu=",bgStatus,bgAbort,bgNodeLast ,bgNode, bgSaveSize,bgLastSize,bgTrigSize,val);			
}*/
#if 0

UINT32 bgFileWrite(BgFileObj_t *bgfile,UINT32 addrbyte,UINT32 size)
{
	UINT32 sector ;
	BgNode_t xdata* bn=bgNodeLastGet();
	BgClusters_t xdata*  node = bgfile->listNode;
	//FILE xdata*file = dos.file + bgfile->fno;
	UINT8 dev = DOS_CurrDriverGet();//file->DevId;
	DISK xdata* disk = (DISK *)&dos.disk[dev];
	UINT32 cnt= (size+disk->Cluster2Byte-1)>>disk->bitCluster2Byte;
	UINT32 idx = bgfile->listNodeIdx;
	UINT32 timeout;
	/*printf("Add(%lx,%lx)%lu(%lx,%lu,%lu)(%bu,%bu)\n"
		,addrbyte,size
		,bgfile->cnt
		,cluster->cluster,cluster->cnt,bgfile->idx
		,bgNode,bgNodeLast
		);*/
	//putchar('W');	
	if(bgfile->fsize + cnt<=bgfile->fsizeMax){
		//bgfile->fsize+=cnt;
		//bgfile->fsizeErr = (cnt<<disk->bitCluster2Byte)-size;
	}else if(bgfile->fsize>=bgfile->fsizeMax){
		__PRINTF_POS();
		return 0;
	}else {
		//printf("\n\nbgFileFull:%lu+%lu>%lu\n\n",bgfile->fsize,cnt,bgfile->fsizeMax);
		__PRINTF_POS();
		//return 0;
		cnt= bgfile->fsizeMax - bgfile->fsize;
		size = cnt<<disk->bitCluster2Byte;
	}
	bgfile->fsize+=cnt;
	bgfile->fsizeErr = (cnt<<disk->bitCluster2Byte)-size;
	if(!bgfile->list){
		return bgFnoWrite(bgfile->fno, addrbyte, size);
	}
	while(cnt)
	{	
		UINT32 freeCnt = node->cnt - idx;
		UINT32 bnCnt;// = (freeCnt<cnt)?(cnt-freeCnt):cnt;
		bn=bgNodeLastGet();
		timeout = 0x10000;
		while(bgStatus && (bn->status&1) && timeout-- && (bgNodeLast == bgNode));//{if(!(timeout&0xf))putchar('^');}
		if(!timeout){
			putchar('!');		
			__PRINTF_POS();
			bgRwTrig();
			timeout = 0x1000;
			while(bgStatus && (bn->status&1) && timeout-- && (bgNodeLast == bgNode));//{if(!(timeout&0xf))putchar('o');}
			if(!timeout){
				__PRINTF_POS();
			}
		}
		
		sector = FAT_Cluster2Sector(dev, node->start+idx);
		//printf("%lu,%lu,%lu\n",c,cc,idx);
		if(freeCnt<cnt){
			bnCnt = freeCnt;//cnt-freeCnt;
			cnt-=bnCnt;
			node++;
			idx = 0;
		}else{
			bnCnt = cnt;
			cnt = 0;	
			idx+=bnCnt;
		}
		
		bn->addr = addrbyte;
		bn->sector = sector;
		bn->size = bnCnt<<disk->bitCluster2Byte;
		bn->status = 0xff;
		addrbyte+=bn->size;
		bgNodeAdd();
		
		if(node>=bgfile->list+bgfile->listCnt){break;}
	}
	cnt<<=disk->bitCluster2Byte;
	if(size>cnt){size-=cnt;}
	else {size = 0;}
	bgfile->listNodeIdx = idx;
	bgfile->listNode = node;
	bgSaveSize+=size;
	
	//if(bgRunTime0 && (bgRunTime1>bgRunTime0)&&(bgRunTime1-bgRunTime0>100000)){
		//bgStatus = 0;
	//	printf("%lu\n",bgRunTime1-bgRunTime0);
	//	__PRINTF_POS();
	//}
	if(!bgStatus){
		bgRwTrig();
	}
	return size;
}


UINT32 bgFnoWrite(UINT8 fno, UINT32 addrByte, UINT32 size)
{	
	FILE xdata*file = (FILE *)&dos.file[fno];
	UINT8  DevId = file->DevId;
	DISK xdata*disk = (DISK *)&dos.disk[DevId];
	UINT32 addr = addrByte;
	UINT32 next;//, size;
	//UINT32 sector;
	UINT32 FreeCluster;

	UINT32 cluster = file->CurrCluster;
	UINT32 cnt = (size+disk->Cluster2Byte-1)>>disk->bitCluster2Byte;
	BgNode_t xdata* bn;

	sdputchar('M');

	while(cnt)
	{
		bn = bgNodeLastGet();		
		if(bn->status&1){
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
			break;
		}
			
		FreeCluster = FAT_FreeClusterGet(DevId, cnt, &next);// only return continious free block
		if( FreeCluster == 0 ){
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
			break;
		}		
		
		if( cnt >= FreeCluster ){cnt-=FreeCluster;}
		else{
			FreeCluster = cnt; cnt=0;
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
		}	

		//add node
		bn->addr = addr;
		bn->sector = FAT_Cluster2Sector(DevId, next);
		bn->size = FreeCluster<<disk->bitCluster2Byte;
		bgNodeAdd();

		if( cluster==0){
			file->FirstCluster= next;			
		}else{
			FAT_TableWrite(DevId, cluster, next);
			//FreeCluster--;
		}		
		while(--FreeCluster){cluster = next++;FAT_TableWrite(DevId, cluster, next);}
		cluster = next;		
		FAT_TableWrite(DevId, cluster, disk->FAT_EOFValue);	
		bn->status = 0xff;
		addr+=bn->size>>1;
		//printf("(%bu,%bu)\n",bgNode,bgNodeLast);
	}
	
	//size-=cnt<< disk->bitCluster2Byte;
	cnt<<=disk->bitCluster2Byte;
	if(cnt && size>cnt){size-=cnt;}
	else {size = 0;}
	if(size){
		file->size+=size;
		file->CurrCluster=cluster;
		file->LastCluster = cluster;
		file->pos+=size;
	}
	bgSaveSize+=size;

	if(!bgStatus ){
		bgRwTrig();
	}

	sdputchar('m');
	return size;
}


#endif

#if 0
UINT32 bgFatAddr0,bgFatAddr1,bgFatAddr,bgFatSize;
UINT32 bgFatCluster0,bgFatCluster1,bgFatCluster,bgFatClusterCnt;
UINT8 bgFatBank;
UINT32 bgFileClose(BgFileObj_t *bgfile)
{
}
UINT32 bgFileWrite(UINT8 fno,UINT32 addrbyte,UINT32 size)
{
	xdata FILE *file = (FILE *)&dos.file[fno];
	UINT8  DevId = file->DevId;
	xdata DISK *disk = (DISK *)&dos.disk[DevId];
	UINT32 addr = addrbyte;
	UINT32 next;//, size;
	//UINT32 sector;
	UINT32 FreeCluster;

	UINT32 cluster = file->CurrCluster;
	UINT32 count = cnt >> disk->bitCluster2Byte;	
	BgNode_t xdata* bn;

	sdputchar('M');

	while(count)
	{	
		FreeCluster = bgFreeClusterGet(&next, count);//FAT_FreeClusterGet(DevId, count, &next);// only return continious free block
		if( FreeCluster == 0 ){
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
			break;
		}		
		
		if( count >= FreeCluster ){count-=FreeCluster;}
		else{
			FreeCluster = count; count=0;
			__PRINTF_POS();
		}			
		
		ret = bgSectorNodeAdd(addr, next, FreeCluster<<disk->bitCluster2Byte, 0);
		
		if( cluster==0){
			file->FirstCluster= cluster;			
		}
		cluster = next;			
		addr+=bn->size>>1;
		//printf("(%bu,%bu)\n",bgNode,bgNodeLast);
	}
	
	size-=count<< disk->bitCluster2Byte;

	if(size){
		file->size+=size;
		file->CurrCluster= cluster;
		file->LastCluster = cluster;
		file->pos+=size;
	}
	if(!bgStatus ){
		bgRwTrig();
	}
	sdputchar('m');

	return size;
}
UINT8 bgSectorNodeAdd(UINT32 addrByte, UINT32 sector, UINT32 size,UINT8 flag)
{
	BgNode_t xdata* bn = bgNodeLastGet();		
	if(bn->status&1){
		printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
		break;
	}
	//add node
	bn->addr = addrByte;
	bn->sector = sector;//FAT_Cluster2Sector(DevId, sector);
	bn->size = size;//<<disk->bitCluster2Byte;
	bgNodeAdd();
	bn->status = flag;
	bgSaveSize+=size;
}
void bgFatTableSwitch()
{
	UINT32 addr;
	if(bgFatAddr==bgFatAddr0){
		if(bgFatBank==0){ return;}
		bgFatBank = 0;
		addr = bgFatAddr0;
	}else{
		if(bgFatBank!=0){bgFatBank = 0; return;}
		addr = bgFatAddr1;
	}
	
	bgSectorNodeAdd(addr,bgFatCluster,bgFatSize,0);
	bgSectorNodeAdd(addr,bgFatCluster,bgFatSize,1);
	bgFatCluster++;
}
UINT32 bgFreeClusterGet(UINT32* cluster, UINT32 cnt)
{
	UINT32 next;
	UINT32 freeCluster;
	bgFatTableSwitch();
	*cluster = bgFatTableWrite(*cluster,next,freeCluster);
	return 0;
}
UINT32 bgFatTableWrite(UINT32 cluster,UINT32 next,UINT32 count)
{
	UINT32 addr;
	if(cluster>=bgFatCluster+bgFatClusterCnt){
		bgFatTableSwitch();
	}
	addr  = bgFatAddr + cluster-bgFatCluster;
	//UINT8 fsType;
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
	while(XBYTE[REG_Dram_IndirectPrefetch]);
	//write 2 or 4 bytes to DRAM
	HAL_DramWrite32(next);	
	if(count){
		//addr = 
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(addr,3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(addr,2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(addr,1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(addr,0);
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		//write 2 or 4 bytes to DRAM
		while(--count){
			HAL_DramWrite32(++next);			
		}
	}

	return next;
}
#endif
#if 0
UINT32 bgFnoWrite(UINT8 fno, UINT32 addrByte, UINT32 cnt)
{	
	xdata FILE *file = (FILE *)&dos.file[fno];
	UINT8  DevId = file->DevId;
	xdata DISK *disk = (DISK *)&dos.disk[DevId];
	UINT32 addr = addrByte;
	UINT32 next;//, size;
	//UINT32 sector;
	UINT32 FreeCluster;

	UINT32 cluster = file->CurrCluster;
	UINT32 count = cnt >> disk->bitCluster2Byte;	
	BgNode_t xdata* bn;

	sdputchar('M');

	while(count)
	{
		bn = bgNodeLastGet();		
		if(bn->status&1){
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
			break;
		}
			
		FreeCluster = FAT_FreeClusterGet(DevId, count, &next);// only return continious free block
		if( FreeCluster == 0 ){
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
			break;
		}		
		
		if( count >= FreeCluster ){count-=FreeCluster;}
		else{
			FreeCluster = count; count=0;
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
		}	

		//add node
		bn->addr = addr;
		bn->sector = FAT_Cluster2Sector(DevId, next);
		bn->size = FreeCluster<<disk->bitCluster2Byte;
		bgNodeAdd();

		if( cluster==0){
			file->FirstCluster= next;			
		}else{
			FAT_TableWrite(DevId, cluster, next);
			//FreeCluster--;
		}		
		while(--FreeCluster){cluster = next++;FAT_TableWrite(DevId, cluster, next);}
		cluster = next;		
		FAT_TableWrite(DevId, cluster, disk->FAT_EOFValue);	
		bn->status = 0xff;
		addr+=bn->size>>1;
		//printf("(%bu,%bu)\n",bgNode,bgNodeLast);
	}
	
	cnt-=count<< disk->bitCluster2Byte;
//EXIT_CRITICAL(sts);
	if(cnt){
		file->size+=cnt;
		file->CurrCluster=cluster;
		file->LastCluster = cluster;
		file->pos+=cnt;
	}
	//if(bgTest000)printf("++(%lx,%lx)(%bu,%bu)%bu\n",bgSaveSize,cnt,bgNode,bgNodeLast, bgStatus);
	bgSaveSize+=cnt;
	#if SdBgDebugIO
	XBYTE[0x2d83] |= 0x02; 
	XBYTE[0x2d83+4] &= ~0x02; 
	XBYTE[0x2d83+8] |= 0x02; 
	#endif

	if(!bgStatus ){
		bgRwTrig();
	}

	sdputchar('m');
	return cnt;
}
#endif
#if 0//BgNoUsed
void bgAddNode0(UINT32 addrbyte, UINT32 sector, UINT32 size)
{
	BgNode_t xdata* bn=bgNodeLastGet();
	bn->addr = addrbyte;
	bn->sector = sector;
	bn->size = size;
	bgNodeAdd();
	bn->status = 0xff;
	bgSaveSize+=size;
	if(!bgStatus ){
		bgRwTrig();
	}
}
UINT32 bgClusterAlign(UINT8 fno, UINT32 size,UINT32* cluster)
{
	UINT32 size0=DOS_ClusterSizeGet();
	UINT8 i,cnt=0;
	BgNode_t xdata* bn=bgNodes;
	UINT8 dev = DOS_CurrDriverGet();
	UINT32 cluster0;
	bgInit();
	bgStatus=1;
	size = bgFnoWrite(fno,  0, size);
	
	for(i=0;i<bgNodeLast;i++){
		cluster0 = FAT_Sector2Cluster(dev, bn->sector);
		printf("<%lu,%lu\n",bn->sector,cluster0);
		while(bn->size){
			*cluster++=cluster0++;
			bn->size-=size0;
			
			cnt++;
			printf("%bu-%lu(%lu,%lu)\n",cnt-1,*(cluster-1),bn->size,size0);
		}
		bn++;
	}
	return cnt;
}
UINT32 bgClusterAlign0(UINT8 fno, UINT32 size,BgClusters_t* cluster)
{
	//UINT32 size0=DOS_ClusterSizeGet();
	UINT8 i;
	UINT32 cnt=0;
	BgNode_t xdata* bn=bgNodes;
	UINT8 dev = DOS_CurrDriverGet();
	DISK xdata* disk = (DISK *)&dos.disk[dev];
	UINT32 cluster0;
	UINT32 t0,t1;
	HAL_GlobalReadGTimer(&t0);
	bgInit();
	bgStatus=1;
	size = bgFnoWrite(fno,  0, size);
	
	for(i=0;i<bgNodeLast;i++){
		cluster0 = FAT_Sector2Cluster(dev, bn->sector);		
		cluster->cluster = cluster0;
		cluster->cnt = (bn->size)>>(disk->bitCluster2Byte);
		printf("%bu(%lx,%u)%lx,%bu\n",i,cluster->cluster,cluster->cnt,bn->size,disk->bitCluster2Byte );
		cnt+=cluster->cnt;
		cluster++;
		bn++;
		
	}
	HAL_GlobalReadGTimer(&t1);
	printf("(%lu,%lu)-%lu\n",size,cnt,t1-t0);
	return cnt;
}
void bgClusterAddNode(UINT32 addrbyte,UINT32 size,BgFileObj_t *clusters)
{
	UINT32 sector ;//= clusters->clusters->cluster
	BgNode_t xdata* bn=bgNodeLastGet();
	BgClusters_t xdata*  cluster = clusters->clusters;
	UINT8 dev = DOS_CurrDriverGet();
	sector = FAT_Cluster2Sector( dev, cluster->cluster);
	bn->addr = addrbyte;
	bn->sector = sector;
	bn->size = size;
	bgNodeAdd();
	bn->status = 0xff;
	bgSaveSize+=size;
	if(!bgStatus ){
		bgRwTrig();
	}
}

void bgClusterDelete(BgFileObj_t *clusters)
{
	UINT32 cnt = clusters->cnt;
	BgClusters_t* cluster = clusters->clusters;
	FILE xdata*file = dos.file + clusters->fno;
	UINT8 dev = file->DevId;
	DISK xdata*disk = (DISK *)&dos.disk[dev];
	UINT32 clusterSize=disk->Cluster2Byte;
	UINT32 cluster0 = cluster->cluster;
	file->size-=cnt*clusterSize;	
	file->CurrCluster=cluster->cluster;
	file->LastCluster = file->CurrCluster;
	file->pos=file->size;
	disk->FreeCluster+=cnt;//vidIdxClusterCnt-vidIdxClusterIdx;
	FAT_TableWrite(dev, cluster0, 0);
	while(cnt--)
	{	
		cluster0 = cluster->cluster;
		printf("%lu(%lx,%u)\n",cnt,cluster->cluster,cluster->cnt);
		FAT_TableWrite(dev, cluster0, 0);
		if(disk->FAT_FirstFreeCluster>cluster0){
			disk->FAT_FirstFreeCluster=cluster0;
		}
		if(--(cluster->cnt)<1){cluster++;}
		else{cluster->cluster++;}			
		
	}
}
/*UINT32 bgSectorAlign(UINT8 fno, UINT32 size,sector_align_t* pair)
{
	UINT32 size0;
	UINT8 i;
	BgNode_t xdata* bn=bgNodes;
	bgInit();
	size0 = bgFnoWrite(fno,  0, size);
	for(i=0;i<bgNodeLast;i++){
		pair->sector = bn->sector;
		pair->size = bn->size;
		pair++;
		bn++;
	}
	return i;
}*/
/*void bgWriteSectors(UINT32 addrbyte,UINT32 size,sector_align_t *vidIdxSector)
{
	UINT8 dev = DOS_CurrDriverGet();
	while(size){
		DOS_SectorWrite(dev, vidIdxSector->sector, size>>9, addr>>1);
	}
} */
UINT8 sdWaitDat0High0(void)
{
	UINT32 timeout_count = 0x400000L;
	XBYTE[REG_Fm_Tx_Rx_trigger] = 1<<6;
	do
	{
		if (timeout_count >	0)
		{
			timeout_count--;
		}
		else
		{
			printf(">0<\n");
			return FALSE;
		}
		if(sD_CdGet()){
			printf(">1<\n");
			return FALSE;
		}
		if(XBYTE[REG_Fm_SD_Status] & 0x20) 
		{
			UINT8 i=20;
			while(--i){
				if(!(XBYTE[REG_Fm_SD_Status] & 0x20))
				{
					break;
				}
			}
			if(i==0 && (XBYTE[REG_Fm_SD_Status] & 0x20))
			{
			printf(">2<\n");
				return TRUE;
			}
		}
	} while(1);//((XBYTE[REG_Fm_SD_Status] & 0x20) == 0);	
	return TRUE;
}
#endif


#if 0
void bgTest0()
{
	UINT32 t0,t1,t2,t3,t4,t5;
	UINT8 fno = DOS_FileOpen("BG.DAT",'w');
	HAL_GlobalTimerRead10us(&t0);
	bgFnoWrite(fno, 0, 1024ul*32*20*2);
	HAL_GlobalTimerRead10us(&t1);
	printf("000\n");	
	bgFnoWrite(fno, 0, 1024ul*32*20*10);
	HAL_GlobalTimerRead10us(&t2);
	printf("011\n");	
	while(bgStatus)
	{
	//putchar('^');
		/*if(XBYTE[REG_Fm_evt] & (1<<2)) {
			XBYTE[REG_Fm_evt] |= (1<<2);
		
			bgRwTrig();
		}*/
	}
	HAL_GlobalTimerRead10us(&t3);
	printf("010\n");	
	bgFnoWrite(fno, 0, 1024ul*32*20*2);
	HAL_GlobalTimerRead10us(&t4);

	while(bgStatus);
	HAL_GlobalTimerRead10us(&t5);
	printf("111\n");
	printf("1280k:%lu\n6400k:%lu\n6400k+1280k:%lu,%lu\n640k:%lu,%lu\n",
		t1-t0,t2-t1,
		t2-t0,t3-t2,
		t4-t3,t5-t4);
	
	DOS_FileClose(fno);
}
void bgTest1()
{
	UINT32 t0,t1,t2,t3,t4,t5;
	UINT8 fn0 = DOS_FileOpen("BG0.DAT",'w');
	UINT8 fn1 = DOS_FileOpen("BG1.DAT",'w');
	UINT16 i,j;
	UINT32 cnt = 1024ul*32*5;
	HAL_GlobalTimerRead10us(&t0);
	for(i=0;i<26;i++)
	{
		j=1000;while(j--);
		bgFnoWrite(fn0, 0,  cnt);
	}
	while(bgStatus);
	HAL_GlobalTimerRead10us(&t1);
	printf("160k*20:%lu\n6400k:%lu\n6400k+1280k:%lu,%lu\n640k:%lu,%lu\n",
		t1-t0,t2-t1,
		t2-t0,t3-t2,
		t4-t3,t5-t4);
	DOS_FileClose(fn0);
	DOS_FileClose(fn1);
}

void bgTest(UINT32* arg)
{

	if(arg[0]){bgMaxSec = arg[0];}
	if(arg[1]){bgTestSave = arg[1]*1024;}
	bgTestMode= arg[2];
	bgTest000 ^= 1;
	{
		UINT8 sts;
		UINT32 t0;
		sts = XBYTE[REG_Fm_intEn];
		XBYTE[REG_Fm_intEn] &= ~4;
		HAL_GlobalReadGTimer(&t0);
		if(bgTestTimeEn){
			bgTestTimeEn = 0;
			printf("(%lu-%lu=%lu)-%lu-%lu\n",t0,bgTestTime,t0-bgTestTime
				,bgTestSize
				,bgTestSize/(t0-bgTestTime));
		}else{
			bgTestTimeEn = 1;
			bgTestSize = 0;	
			bgTestTime = t0;
		}
		XBYTE[REG_Fm_intEn] &= sts;
	}
	/*{
			UINT8 cc = 10;
		XBYTE[0x20de]=0;
		XBYTE[0x20e1]=0x03;
		XBYTE[0x23e0]=0x10;
		while(cc--){printf("%bx\n",XBYTE[0x20fb]);}
	}*/
	bgRwTrig();
	//dbgWdtFeed(1000ul*60*2);	
	//sdInitVideo();
	//if(arg[1]==0){bgTest0();}
	//else if(arg[1]==1){bgTest1();}
	printf("(%bu,%bu-%bu,%bu)\n",bgStatus,bgAbort,bgNode,bgNodeLast);
	printf("%lu\n",bgSaveSize);
	
}
#elif 0
void bgTest(UINT32* arg)
{
	UINT8 fno;// = DOS_FileOpen("AAAA0000.TMP", 'w');
	UINT32 size = 0xffff0000;
	BgFileObj_t bgfile;
	UINT32 size0 = 0;
	UINT32 cnt = 4ul*128;
	UINT32 t0,t1,t2,t3;
	//dbgWdtFeed(60000);
	//dbgWdtSet(0);
	HAL_GlobalReadGTimer(&t0);
	DOS_FileDel("AAAA0000.TMP");
	fno = DOS_FileOpen("AAAA0000.TMP", 'w');
	bgfile.cnt = 1024;
	bgfile.fno = fno;
	bgfile.clusters = 0xC000;
	bgInit();
	size0 = bgFileOpen(&bgfile, size);
	HAL_GlobalReadGTimer(&t1);
	//dbgWdtFeed(60000);
	dbgWdtSet(0);
	while(cnt--){
		bgFileWrite(&bgfile, 0, 1024ul*32);
		bgFileWrite(&bgfile, 0, 1024ul*32);
		bgFileWrite(&bgfile, 0, 1024ul*64);
		bgFileWrite(&bgfile, 0, 1024ul*128);
	}
	HAL_GlobalReadGTimer(&t2);
	bgFileClose(&bgfile);
	HAL_GlobalReadGTimer(&t3);
	DOS_FileClose(fno);
	printf("%lu,%lu,%lu\n",t1-t0,t2-t1,t3-t2);
}
#elif 0
void bgTest(UINT32* arg)
{
	extern UINT8 vidBgEn;
	vidBgEn = !vidBgEn;
#if 0	
	UINT32 size = 0x10000000;
	BgFileObj_t bgfile0,bgfile1;
	UINT32 size0 = 0,size1 = 0;
	UINT32 i,cnt = 32;
	UINT32 t0,t1,t2,t3,t4,t5,t6;
	UINT8 fno;
	UINT32 addr = 0x2;
	UINT32 len = 512ul*1024;
	if(arg[0])cnt=arg[0];
	//dbgWdtFeed(60000);
	//dbgWdtSet(0);
	HAL_GlobalReadGTimer(&t0);
#if 1	
	bgfile0.listCnt = 1024;
	//bgfile.fno = fno;
	bgfile0.list = 0xC000;
	bgfile1.listCnt = 1024;
	bgfile1.list = 0xD000;
	bgInit();
	size0 = bgFileOpen(&bgfile0, size,"AAAA0000.TMP");
	HAL_GlobalReadGTimer(&t1);
	size1 = bgFileOpen(&bgfile1, size,"BBBB0000.TMP");
	HAL_GlobalReadGTimer(&t2);
	//dbgWdtFeed(60000);
	dbgWdtSet(0);
	//while(cnt--){
	for(i=0;i<cnt;i++){
		//bgFileWrite(&bgfile0, 0, 1024ul*32);
		//bgFileWrite(&bgfile1, 0, 1024ul*32);
		bgFileWrite(&bgfile0, addr, len);
		bgFileWrite(&bgfile1, addr, len);
	}
	HAL_GlobalReadGTimer(&t3);
	bgFileClose(&bgfile0,NULL,1);

	HAL_GlobalReadGTimer(&t4);
	bgFileClose(&bgfile1,NULL,1);
	HAL_GlobalReadGTimer(&t5);

	bgFileOpen(&bgfile0,size,"CCCC0000.TMP");
	for(i=0;i<cnt;i++){
		bgFileWrite(&bgfile0, addr, len);
	}
	bgFileClose(&bgfile0,NULL,1);
#endif
	//dbgWdtSet(1);
	DOS_FileDel("ABCD0000.TMP");
	printf("=1=\n");
	fno = DOS_FileOpen("ABCD0000.TMP", 'w');
	//while(cnt--){
	printf("=2=%bu\n",fno);
	for(i=0;i<cnt;i++){
		printf("%lu\n",i);
		DOS_FileWrite(fno, addr, len);
	}
	DOS_FileClose(fno);
	HAL_GlobalReadGTimer(&t6);
	printf("%lu,%lu,%lu,%lu,%lu,%lu\n",t1-t0,t2-t1,t3-t2,t4-t3,t5-t4,t6-t5);

#endif	
}
#elif 0
void bgTestCreateFile(UINT8 fname[],UINT32 fsize)
{
	UINT8 dev = DOS_CurrDriverGet();//file->DevId;
	DISK xdata* disk = (DISK *)&dos.disk[dev];
	UINT32 curr=0,next;
	UINT32 count = fsize>>disk->bitCluster2Byte;
	UINT8 fno;
	FILE xdata*dosfile;
	UINT32 culster0;
	UINT32 c=1;
	while(count)
	{	
		UINT32 clusterFreeCnt = FAT_FreeClusterGet(dev, count, &next);// only return continious free block
		if( clusterFreeCnt == 0 ){
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
			break;
		}		
		
		if( count >= clusterFreeCnt ){count-=clusterFreeCnt; }
		else{
			clusterFreeCnt = count; count=0;
			printf("%s %u\n", __FILE__, (unsigned short)__LINE__);
		}	
		if(curr ==0) 	{curr = next++;clusterFreeCnt--;culster0 = curr;}
		while(--clusterFreeCnt){FAT_TableWrite(dev, curr, next);curr = next++;c++;}		
	}

	FAT_TableWrite(dev, curr, disk->FAT_EOFValue);	
	DOS_FileDel(fname);
	fno = DOS_FileOpen(fname, 'w');
	dosfile = dos.file + fno;
	dosfile->size=c<<disk->bitCluster2Byte;
	dosfile->FirstCluster= culster0;
	dosfile->CurrCluster= curr;//cluster->cluster+bgfile->idx;
	dosfile->LastCluster = dosfile->CurrCluster;
	dosfile->pos=dosfile->size;

	DOS_FileClose(fno);
}
void bgTest(UINT32* arg)
{
	UINT32 size =  1ul*1024*1024;//kb
	UINT8 fname[13];
	UINT16 i=500;
	UINT32 count;
	
	dbgWdtSet(0);

	while(--i){

		sprintf(fname,"SUNP%04uJPG",i);
		//printf("%04u\n",i);
		bgTestCreateFile(fname,size);
	}
	dbgWdtSet(1);
}
#else
/*void bgTest(UINT32* arg){
	arg = arg;
	printf("%bu(%bu,%bu)%lu\n",bgStatus,bgNode,bgNodeLast,bgSaveSize);
}*/
#endif
#endif
