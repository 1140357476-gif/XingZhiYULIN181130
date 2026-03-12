void videoAddBalanceDmyFrame(UINT32 MovOff, UINT32 size, UINT8 num)
{
	vidClipCtrl.VidChunkCnt += (UINT32)num;
	vidClipCtrl.totalChuck += (UINT32)num;

	while((num-- > 0) && (num<200)){
		videoRecWriteVidIndex2Dram(MovOff, size);//, vidClipCtrl.AVIIdxAddr);
		//vidClipCtrl.AVIIdxAddr += 8;
	}
}

void videoEnableBalanceFrame(UINT8 en)
{
	DropBalanceEnable = en;
}
//-----------------------------------------------------------------------------
//videoRecAddDummy
//-----------------------------------------------------------------------------
/**
 * @fn		  void videoRecAddDummy(UINT32 addr)
 * @brief	  add dummy
 * @param	  [in] addrWord : frag buffer address
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Tq.Zhang
 * @since	  2010-5-10
 * @todo	  N/A
 * @bug 	  N/A
*/
void videoRecAddDummy(UINT32 addrWord)
{
	UINT8 tmp0 = 0;

	HAL_DramStartAddrSet(addrWord, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite(0x30,0x30);
	HAL_DramWordDataWrite(0x64,0x63);
	HAL_DramWordDataWrite(0x00,0x00);
	HAL_DramWordDataWrite(0x00,0x00);
	vid_ByteAddr += 8;
	vid_TmpNum	+= 8;
	videoRecWriteVidIndex2Dram(vidClipCtrl.MoviOfset, tmp0);//, vidClipCtrl.AVIIdxAddr);
	//vidClipCtrl.AVIIdxAddr += 8;
	vidClipCtrl.MoviOfset = vidClipCtrl.MoviOfset + 8 + tmp0;
	vidClipCtrl.VidChunkCnt++;
	vidClipCtrl.totalChuck++;
}


UINT32 vidCustomUpdate2Vlc(UINT8 idx, UINT32 vlcSize)
{
	UINT32 srcAddr, dstAddr;
	UINT32 dataSize = 0;
	UINT8 LSB, MSB;
	UINT8 ret;
	
	if(g_vidCustomUpdateEntry){
		srcAddr = g_vidCustomUpdateEntry;
		HAL_DramStartAddrSet(srcAddr, K_SDRAM_Prefetch );
		HAL_DramWordDataRead(&LSB, &MSB);
		dataSize = (LSB + ((UINT16)MSB<<8) + 3) & ~3;

		//printf("xx:0x%lx, %lx, %lx, %lu\n", srcAddr, vlcSize, G_Video_Bound_Size, dataSize);
		if(vlcSize + dataSize < vidVlcBufSize){
			dstAddr = (vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_A + idx)<<1) + vlcSize ; //byte addr
			srcAddr <<= 1;
			ret = HAL_GprmDramDmaAdv(srcAddr, dstAddr, dataSize, 0);
			if(ret != SUCCESS){
				dataSize = 0;
			}
		}
	}
	
	g_vidCustomUpdateEntry = 0;
	return dataSize;
}
void vidCustomUpdate2Idx(UINT32 idxAddr)
{
	HAL_DramStartAddrSet(idxAddr + 2, K_SDRAM_NoPrefetch );
	HAL_DramWordDataWrite(0x00,0x00);
}
UINT8 videoCustomUpdate(UINT32 dramAddrW)
{
	g_vidCustomUpdateEntry = dramAddrW;
	return SUCCESS;
}