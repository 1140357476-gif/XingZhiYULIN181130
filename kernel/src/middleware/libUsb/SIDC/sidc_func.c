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
 #include "sidc_func.h"
 #include "dbg_mem.h"
 #include "reg_usb.h"
 #if USBSIDC_OPTION
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
UINT32 usbIntrDma(UINT32 addr, UINT32 sizeByte, UINT8 isWait,UINT8 isToUsb)
{	
	UINT8 bSram = DMA_PATH_SRAM; //indicate if the pSrc is in SRAM
	UINT8 memType = 2;//TYPE_DMEM; // DMEM / IMEM
	UINT16 sramAddr = (UINT16)addr;
	UINT32 dramAddr = addr;		
	UINT8 rst;
	UINT32 bkpktcnt; 
	UINT32 timeOut=100000;
	
	//UINT32 maxDmaSize = 2*1024ul*1024ul;	
	if (addr>0x10000){
		dramAddr = addr;
		bSram = DMA_PATH_DRAM;
	} else if (sramAddr >= 0x4000) {// 0x4000 <= pSrc <= 0xFFFF
		UINT32 shadowAddr = K_SDRAM_ShadowAddr;
		dramAddr = shadowAddr + (sramAddr - 0x4000)/2;
		bSram = DMA_PATH_DRAM;
	} else if (sramAddr >= 0x3800) {// 0x3800 <= pSrc <= 0x3BFF
		sramAddr &= 0x3FF; // sramAddr -= 0x3800
		memType = 0;//TYPE_SWAP_MEM;
	} else if (sramAddr >= 0x3000) {// 0x3000 <= pSrc <= 0x37FF
		sramAddr &= 0x7FF; // sramAddr -= 0x3000
		memType = 1;//TYPE_IMEM;
	} //else //0x0000 <= pSrc <= 0x1FFF
	
	INTR_HAL_DmaSetSramAddr(sramAddr);
	INTR_HAL_DmaSetDramAddr(dramAddr);
	if(isToUsb){
		UINT16 bulkSize = G_bUSBSpeed?512:64;
        XBYTE[REG_Usb_BufClr]=0x08; 
        XBYTE[REG_Usb_AckCntRst]|=0x08; 
        XBYTE[REG_Usb_AckCntRst]&=~0x08; 
        XBYTE[REG_Usb_BulkAutoMode] |=0x08; 
        //sidcPrintfBulk("dma in size:%lx\n",sizeByte); 

		bkpktcnt = (sizeByte+bulkSize-1)/bulkSize;

		//sidcPrintfBulk("dma in size:%lx,%x,%lx,%bx\n",sizeByte,bulkSize,bkpktcnt,G_bUSBSpeed); 
		
		//bkpktcnt &=0x00FFFFFF;
        XBYTE[REG_Usb_DMAPktCntBkIn0] = READ8(bkpktcnt,3); 
        XBYTE[REG_Usb_DMAPktCntBkIn1] = READ8(bkpktcnt,2); 
        XBYTE[REG_Usb_DMAPktCntBkIn2] = READ8(bkpktcnt,1);

        rst = INTR_HAL_DmaDo(bSram,DMA_PATH_USB, sizeByte, !isWait, memType); 
        while(!XBYTE[REG_Usb_BulkInAckCnt_match] && --timeOut); 
		
		if(sizeByte%bulkSize==0)
		{
			//sidcPrintfBulk("empty bulk!%bx\n",XBYTE[REG_Usb_TransactionEn]);
			//HAL_DmaDo(bSram,DMA_PATH_USB, 0, 1, memType);
			XBYTE[REG_Usb_BufClr]|=0x08;
			XBYTE[REG_Usb_TransactionEn]|=0x08;
		}
			
		XBYTE[REG_Usb_EpAckIntSts]|=0x08;
	}else{
		XBYTE[REG_Usb_AckCntRst]|=0x10;
		XBYTE[REG_Usb_AckCntRst]&=~0x10;
		XBYTE[REG_Usb_BulkAutoMode] |=0x10;
		if(!sizeByte){
			WRITE8(sizeByte, 3, XBYTE[REG_Usb_BulkOutBufSize0]);
			WRITE8(sizeByte, 2, XBYTE[REG_Usb_BulkOutBufSize1]);			
		}
		//sidcPrintfBulk("dma out size:%lx\n",sizeByte);
		if(!sizeByte){
			sidcPrintfErr("nosize\n");
			return 0;
		}
		
		rst = INTR_HAL_DmaDo(DMA_PATH_USB, bSram,sizeByte, !isWait, memType);		
		//XBYTE[REG_Usb_BufClr]|=0x10;
		XBYTE[REG_Usb_EpAckIntSts]|=0x10;
	}
	//sidcPrintfBulk("dmaIntr:(%lx,%lx,%x)%lu-%bx-%bx\n",addr,dramAddr,sramAddr,sizeByte,isToUsb,rst);
	#if 0
	if(addr<0x10000 && rst==SUCCESS)
	{
		UINT16 i;
		UINT8 xdata* pSrc = (UINT8 xdata*)addr; 
		for(i=0;i<sizeByte;i++)
			sidcPrintf("%bx ",pSrc[i]);
		sidcPrintf("\n");
	}
	#endif	
	return (rst==SUCCESS)?sizeByte:0;
}
/**
 * @fn        UINT16 sidcIntrDataOut(void)
 * @brief     sidcIntrDataOut
 * @param     NONE
 * @retval    return size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sidcIntrDataOut()
{
	sidc_container_t* pData = sidcData->hData.container;//sidcData->outContainer;	
	UINT32 size;// = sidcIntrBulkOut(((UINT16)pData),0);
	UINT32 len;// = M_ByteSwapOfDword(pData->ulContainerLength);
	
	XBYTE[REG_Usb_EpAckIntEn]&=~0x10;
	size = sidcIntrBulkOut(((UINT16)pData),0);
	len = M_ByteSwapOfDword(pData->ulContainerLength);
	//sidcPrintfFunc("sidcIntrDataOut-C:%x,T:%x,L:%lx--%lx\n",pData->usCode,pData->usContainerType,len,size);
	
	if(size==0){
		sidcIntrResponse(K_PIMARES_GeneralError,0,0);
		sidcPrintfErr("size==0\n");
		return 0;
	}
	if(size<len)
	{
		size+= sidcIntrBulkOut((UINT16)pData+size,len-size);			
	}
	if(size<len)
	{
		sidcIntrResponse(K_PIMARES_GeneralError,0,0);
		return 0;
	}	
	XBYTE[REG_Usb_BufClr]|=0x10;
	XBYTE[REG_Usb_EpAckIntSts]|=0x10;
	XBYTE[REG_Usb_EpAckIntEn]|=0x10;
	
	return len;
}
/**
 * @fn        void sidcIntrDataIn(UINT32 size)
 * @brief     sidcIntrDataIn
 * @param     [in] size
 * @retval    size
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sidcIntrDataIn(UINT32 size)
{
	sidc_container_t* pContainer=(sidc_container_t*)G_ucStorData;
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 len = size + 12;
	pContainer->ulContainerLength = M_ByteSwapOfDword(len);
	pContainer->usContainerType = K_PIMADATA_ContainerType_DataBlock;
	pContainer->usCode = pCommand->usCode;
	pContainer->ulTransactionId = pCommand->ulTransactionId;
	//sidcPrintfBulk("IntrDataIn-%lx,%lu\n",pContainer,size);
	return sidcIntrBulkIn((UINT16)pContainer, len);
	//G_ucBulkPhase = K_InDataPhase;
}
/**
 * @fn        void sidcIntrResponse(UINT8 rst, UINT8 cnt, UINT32* ps)
 * @brief     sidcIntrResponse
 * @param     [in] rst
 * @param     [in] cnt
 * @param     [in] ps
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sidcIntrResponse(UINT16 rst, UINT8 cnt, UINT32* ps)
{
	UINT8 i;
	UINT8 buf[36];
	sidc_container_t* pResponse = (sidc_container_t*) buf;	
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 len = 12+cnt*4;

	pResponse->ulContainerLength = M_ByteSwapOfDword(len);
	pResponse->usContainerType = K_PIMADATA_ContainerType_ResponseBlock;
	pResponse->usCode = rst;
	pResponse->ulTransactionId = pCommand->ulTransactionId;

	if(ps){
	       for (i = 0; i < cnt; i++){
			pResponse->ulParameter[i] = ps[i];
		}
	}
	//sidcPrintfBulk("IntrResponse-%lx,%lu\n",(UINT32)buf,len);
	return sidcIntrBulkIn((UINT16)pResponse,len)>0;
}
UINT32 usbDma(UINT32 addr, UINT32 sizeByte, UINT8 isWait,UINT8 isToUsb)
{	
	UINT8 bSram = DMA_PATH_SRAM; //indicate if the pSrc is in SRAM
	UINT8 memType = 2;//TYPE_DMEM; // DMEM / IMEM
	UINT16 sramAddr = (UINT16)addr;
	UINT32 dramAddr = addr;		
	UINT8 rst;
	UINT32 bkpktcnt; 
	UINT32 timeOut=100000;
	//UINT32 size;
	//UINT32 maxDmaSize = 2*1024ul*1024ul;	
	dbgWdtFeed(-1);	
	if (addr>0x10000){
		dramAddr = addr;
		bSram = DMA_PATH_DRAM;
	} else if (sramAddr >= 0x4000) {// 0x4000 <= pSrc <= 0xFFFF
		UINT32 shadowAddr = K_SDRAM_ShadowAddr;
		dramAddr = K_SDRAM_ShadowAddr + (sramAddr - 0x4000)/2;
		bSram = DMA_PATH_DRAM;
	} else if (sramAddr >= 0x3800) {// 0x3800 <= pSrc <= 0x3BFF
		sramAddr &= 0x3FF; // sramAddr -= 0x3800
		memType = 0;//TYPE_SWAP_MEM;
	} else if (sramAddr >= 0x3000) {// 0x3000 <= pSrc <= 0x37FF
		sramAddr &= 0x7FF; // sramAddr -= 0x3000
		memType = 1;//TYPE_IMEM;
	} //else //0x0000 <= pSrc <= 0x1FFF
	
	HAL_DmaSetSramAddr(sramAddr);
	HAL_DmaSetDramAddr(dramAddr);
	if(isToUsb){ 
		UINT16 bulkSize = G_bUSBSpeed?512:64;
		UINT32 maxSize = 1024ul*1024*2;
		UINT32 addr = dramAddr;
		UINT32 size = sizeByte;
        /*XBYTE[REG_Usb_BufClr]=0x08; 
        XBYTE[REG_Usb_AckCntRst]|=0x08; 
        XBYTE[REG_Usb_AckCntRst]&=~0x08; */
        XBYTE[REG_Usb_BulkAutoMode] |=0x08; 
        sidcPrintfBulk("dma In(%lu,%u)\n",sizeByte,bulkSize); 

		/*bkpktcnt = (sizeByte+bulkSize-1)/bulkSize;

		sidcPrintfBulk("dma in size:%lx,%x,%lx,%bx\n",sizeByte,bulkSize,bkpktcnt,G_bUSBSpeed); 
		
		//bkpktcnt &=0x00FFFFFF;
        XBYTE[REG_Usb_DMAPktCntBkIn0] = READ8(bkpktcnt,3); 
        XBYTE[REG_Usb_DMAPktCntBkIn1] = READ8(bkpktcnt,2); 
        XBYTE[REG_Usb_DMAPktCntBkIn2] = READ8(bkpktcnt,1);
		*/
		while(size>=maxSize){
			dbgWdtFeed(-1);	
			bkpktcnt = maxSize/bulkSize;
			sidcPrintfBulk("dmaIn\n");
			XBYTE[REG_Usb_BufClr]=0x08; 
	        XBYTE[REG_Usb_AckCntRst]|=0x08; 
	        XBYTE[REG_Usb_AckCntRst]&=~0x08;
			XBYTE[REG_Usb_DMAPktCntBkIn0] = READ8(bkpktcnt,3); 
			XBYTE[REG_Usb_DMAPktCntBkIn1] = READ8(bkpktcnt,2); 
			XBYTE[REG_Usb_DMAPktCntBkIn2] = READ8(bkpktcnt,1);
        	rst = HAL_DmaDo(bSram,DMA_PATH_USB, maxSize, !isWait, memType);
        	while(!XBYTE[REG_Usb_BulkInAckCnt_match] && --timeOut); 
        	size -= maxSize;
        	addr += maxSize/2;
        	HAL_DmaSetDramAddr(addr);
        }
        dbgWdtFeed(-1);	     
		if(size){
			bkpktcnt = (size+bulkSize-1)/bulkSize;
			sidcPrintfBulk("dmaIn(%lu,%u)\n",size,bkpktcnt); 
			XBYTE[REG_Usb_BufClr]=0x08; 
	        XBYTE[REG_Usb_AckCntRst]|=0x08; 
	        XBYTE[REG_Usb_AckCntRst]&=~0x08;
		    XBYTE[REG_Usb_DMAPktCntBkIn0] = READ8(bkpktcnt,3); 
		    XBYTE[REG_Usb_DMAPktCntBkIn1] = READ8(bkpktcnt,2); 
		    XBYTE[REG_Usb_DMAPktCntBkIn2] = READ8(bkpktcnt,1);
		    rst = HAL_DmaDo(bSram,DMA_PATH_USB, size, !isWait, memType);
        	while(!XBYTE[REG_Usb_BulkInAckCnt_match] && --timeOut);
		}else{
			sidcPrintfBulk("dmaIn(0)\n");
			XBYTE[REG_Usb_BufClr]|=0x08;
			XBYTE[REG_Usb_TransactionEn]|=0x08;
		}
			
		XBYTE[REG_Usb_EpAckIntSts]|=0x08;
	}else{
		XBYTE[REG_Usb_AckCntRst]|=0x10;
		XBYTE[REG_Usb_AckCntRst]&=~0x10;
		XBYTE[REG_Usb_BulkAutoMode] |=0x10;
		if(!sizeByte){
			WRITE8(sizeByte, 3, XBYTE[REG_Usb_BulkOutBufSize0]);
			WRITE8(sizeByte, 2, XBYTE[REG_Usb_BulkOutBufSize1]);			
		}
		sidcPrintfBulk("dma out size:%lx\n",sizeByte);
		if(!sizeByte){
			sidcPrintfErr("Dma Size=0\n");
			return 0;
		}
		
		rst = HAL_DmaDo(DMA_PATH_USB, bSram,sizeByte, !isWait, memType);
		//XBYTE[REG_Usb_BufClr]|=0x10;
		XBYTE[REG_Usb_EpAckIntSts]|=0x10;
	}
	//printf("dmaIntr:(%lx,%lx,%x)%lu-%bx-%bx\n",addr,dramAddr,sramAddr,sizeByte,isToUsb,rst);
	#if 0
	if(addr<0x10000 && rst==SUCCESS)
	{
		UINT16 i;
		UINT8 xdata* pSrc = (UINT8 xdata*)addr; 
		for(i=0;i<sizeByte;i++)
			sidcPrintf("%bx ",pSrc[i]);
		sidcPrintf("\n");
	}
	#endif
	
	return (rst==SUCCESS)?sizeByte:0;
}
/**
 * @fn        void sidcIntrDataIn(UINT32 size)
 * @brief     sidcIntrDataIn
 * @param     [in] size
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT32 sidcContainerBuild(UINT8 xdata* pt,UINT32 size)
{
	sidc_container_t* pContainer=(sidc_container_t*)pt;
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 len = size + 12;
	pContainer->ulContainerLength = M_ByteSwapOfDword(len);
	pContainer->usContainerType = K_PIMADATA_ContainerType_DataBlock;
	pContainer->usCode = pCommand->usCode;
	pContainer->ulTransactionId = pCommand->ulTransactionId;
	return len;
}
UINT32 sidcDataIn(UINT32 addr, UINT32 size)
{
	//UINT8 xdata *pt = (ptContainer)?ptContainer:G_ucStorData;
	UINT8 xdata*pt = G_ucStorData;
	UINT8 isDram = addr>K_SDRAM_ShadowAddr;
	
	if(!isDram)pt=(UINT8 xdata*)addr;
	size = sidcContainerBuild(pt,size);
	sidcPrintfBulk("dataIn-%lx,%lu\n",addr,size);
	if(isDram){
		//sidcBulkIn(ptContainer, 12);
		HAL_DramWriteBytesAddr(addr, pt, 12, 0);
	}
	
	return sidcBulkIn(addr, size);
}
/**
 * @fn        void sidcResponse(UINT8 rst, UINT8 cnt, UINT32* ps)
 * @brief     sidcResponse
 * @param     [in] rst
 * @param     [in] cnt
 * @param     [in] ps
 * @retval    NONE
 * @author    wangjg
 * @since     2010-5-12
*/
UINT8 sidcResponse(UINT16 rst, UINT8 cnt, UINT32* ps)
{
	UINT8 i;
	UINT8 buf[36];
	sidc_container_t* pResponse = (sidc_container_t*) buf;	
	sidc_container_t* pCommand = (sidc_container_t*) G_ucStorCommand;
	UINT32 len = 12+cnt*4;

	pResponse->ulContainerLength = M_ByteSwapOfDword(len);
	pResponse->usContainerType = K_PIMADATA_ContainerType_ResponseBlock;
	pResponse->usCode = rst;
	pResponse->ulTransactionId = pCommand->ulTransactionId;

	if(ps){
	       for (i = 0; i < cnt; i++){
			pResponse->ulParameter[i] = ps[i];
		}
	}
	sidcPrintfBulk("Response-%lx,%lu\n",(UINT32)buf,len);
	return sidcBulkIn((UINT16)pResponse,len)>0;
}
/**
 * @fn        UINT32 sidcIntrRes(UINT8 *pt, UINT8 *pt,UINT8idx)
 * @brief     sidcIntrRes
 * @param     [out] pt
 * @param     [in] idx
 * @retval    return = size
 * @author    wangjg
 * @since     2010-6-1
*/
UINT32 sidcIntrRes(UINT8 *pt,UINT8 idx)
{
	UINT8 offset,size;
	idx<<=1;
	offset = sidcData->devData[8+idx];
	size = sidcData->devData[9+idx];
	if(offset && size ){
		//sidcIntrDramReadBytes(pt,addr+offset/2,size);
		memcpy(pt,sidcData->devData+offset,size);
	}	
	return size;
}
#endif
