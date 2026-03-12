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
 * @file      hal_jpeg.c
 * @brief     jpeg hal function
 * @author    Tq.zhang
 * @since     2010-04-22
 * @date      2010-04-22
*/

#include "general.h"
#include "hal.h"
#include "hal_dram.h"
#include "reg_def.h"
#include "front_logi.h"
#include "video_clip.h"
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
xdata UINT8 zigzagTable[] =
{
		0,   1,   8,  16,   9,   2,   3,  10,
		17,  24,  32,  25,  18,  11,   4,   5,

		12,  19,  26,  33,  40,  48,  41,  34,
		27,  20,  13,   6,   7,  14,  21,  28,

		35,  42,  49,  56,  57,  50,  43,  36,
		29,  22,  15,  23,  30,  37,  44,  51,

		58,  59,  52,  45,  38,  31,  39,  46,
		53,  60,  61,  54,  47,  55,  62,  63
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void HAL_JpegSetYuvAddr(UINT32 JpegYuvAddrWord);
void HAL_JpegSetSclFact(UINT32 hfac,UINT32 vfac,UINT32 hfacini,UINT32 vfacini);
void HAL_JpegSetDramHSize(UINT16 hsize);
void HAL_JpegSetROI(UINT16 xsize, UINT16 ysize, UINT16 xoff, UINT16 yoff);
void HAL_JpegSetVlcSize(UINT16 xsize, UINT16 ysize);
UINT8 HAL_JpegRoiSclSet(UINT16 hfactor, UINT16 vfactor,
							UINT16 SrcWidth, UINT16 SrcHeight, UINT16 RoiWidth, UINT16 RoiHeight,
							UINT16 RoiXOff, UINT16 RoiYOff, UINT8 distClampx,
							UINT8 distClampy, UINT16 DstWidth, UINT16 DstHeight, UINT8 option);


/**************************************************************************
 *                                                                        *
 *  Function Name: HAL_JpegQTableWrite                                     *
 *                                                                        *
 *  Purposes: Fill the Q table                                            *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *      QTableIndex - The quantization table index, 8'h05 is reserved for *
 *                    all 1's of quantized value. The Q-table will not be *
 *                    changed if index is set to 8'h00.                   *
 *                    0x01 : fill QTable (user defined Q-table) in        *
 *                           raster scan                                  *
 *                    0x02 : fill QTable (user defined Q-table) in        *
 *                           zigzag scan                                  *
 *      QTable      - if QTableIndex is 0xf0 or 0xf1, QTable is filled    *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error Code                                                   *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
/**
 * @fn        void HAL_JpegSetYuvAddr(UINT32 JpegYuvAddrWord)
 * @brief     set JPEG yuv address
 * @param     [in] JpegYuvAddrWord : encode, source yuv address; decode, destination yuv address
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegSetYuvAddr(
	UINT32 JpegYuvAddrWord
)
{
	XBYTE[REG_Dram_Stillsb_StAddr_B0]=*((UINT8*)&JpegYuvAddrWord + 3 );                //set source image starting address
	XBYTE[REG_Dram_Stillsb_StAddr_B1]=*((UINT8*)&JpegYuvAddrWord + 2 );
	XBYTE[REG_Dram_Stillsb_StAddr_B2]=*((UINT8*)&JpegYuvAddrWord + 1 );
	XBYTE[REG_Dram_Stillsb_StAddr_B3]=*((UINT8*)&JpegYuvAddrWord );
}

/**
 * @fn        void HAL_JpegSetSclFact(UINT32 hfac, UINT32 vfac, UINT32 hfacini, UINT32 vfacini)
 * @brief     set Jpeg Scale up factor
 * @param     [in] hfac : horizontal scale up factor
 * @param     [in] vfac : vertical scale up factor
 * @param     [in] hfacini : horizontal scale up initialize factor
 * @param     [in] vfacini : vertical scale up initialize factor
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegSetSclFact(
	UINT32 hfac,
	UINT32 vfac,
	UINT32 hfacini,
	UINT32 vfacini
)
{
	/* set hsf. */
	XBYTE[REG_Jpeg_Hsf_Low]= *((UINT8*)&hfac+3    );
	XBYTE[REG_Jpeg_Hsf_High]= *((UINT8*)&hfac+2    );
	/* set vsf. */
	XBYTE[REG_Jpeg_Vsf_Low]= *((UINT8*)&vfac+3    );
	XBYTE[REG_Jpeg_Vsf_High]= *((UINT8*)&vfac+2    );
	/* set hsfini. */
	XBYTE[REG_Jpeg_Hsfini_Low]= *((UINT8*)&hfacini+3 );
	XBYTE[REG_Jpeg_Hsfini_High]= *((UINT8*)&hfacini+2 );
	/* set vsfini. */
	XBYTE[REG_Jpeg_Vsfini_Low]= *((UINT8*)&vfacini+3 );
	XBYTE[REG_Jpeg_Vsfini_High]= *((UINT8*)&vfacini+2 );
}

/**
 * @fn        void HAL_JpegSetDramHSize(UINT16 hsize)
 * @brief     set Jpeg DRAM horizontal size
 * @param     [in] hsize : 8 allign
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegSetDramHSize(UINT16 hsize)
{
	/* set Stillsb_hsize. */
	XBYTE[REG_Dram_Stillsb_HSize_Low]=*((UINT8*)&hsize+1  );
	XBYTE[REG_Dram_Stillsb_HSize_High]=*((UINT8*)&hsize	 );
}

/**
 * @fn        void HAL_JpegSetSourceSize(UINT16 SrcHsize, UINT16 SrcVsize)
 * @brief     Set Jpeg Source YUV size before ROI
 * @param     [in] SrcHsize : width based on pixel, 8 allign
 * @param     [in] SrcVsize : height based on pixel, 2 allign
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegSetSourceSize(UINT16 SrcHsize, UINT16 SrcVsize)
{
	/* set imgwidth. */
	XBYTE[REG_Jpeg_ImgWidth_Low]=*((UINT8*)&SrcHsize+1  );
	XBYTE[REG_Jpeg_ImgWidth_High]=*((UINT8*)&SrcHsize	 );
	/* set imgheight. */
	XBYTE[REG_Jpeg_ImgHeight_Low]=*((UINT8*)&SrcVsize+1 );
	XBYTE[REG_Jpeg_ImgHeight_High]=*((UINT8*)&SrcVsize	 );
}

/**
 * @fn        void HAL_JpegSetROI(UINT16 roi_hsize, UINT16 roi_vsize, UINT16 xoff, UINT16 yoff)
 * @brief     set ROI size and offset
 * @param     [in] roi_hsize : 1~2^16-1
 * @param     [in] roi_vsize : 1~2^16-1
 * @param     [in] xoff : 1~2^16-1
 * @param     [in] yoff : 1~2^16-1
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegSetROI(UINT16 roi_hsize, UINT16 roi_vsize, UINT16 xoff, UINT16 yoff)
{
	/* jpg limit : Turbo enable and jpg scale disable, h offset need 16x */

	/* set srcwidth. */
	XBYTE[REG_Jpeg_Srcwidth_Low]= *((UINT8*)&roi_hsize+1  );
	XBYTE[REG_Jpeg_Srcwidth_High]= *((UINT8*)&roi_hsize    );
	/* set srcheight. */
	XBYTE[REG_Jpeg_Srcheight_Low]= *((UINT8*)&roi_vsize+1  );
	XBYTE[REG_Jpeg_Srcheight_High]= *((UINT8*)&roi_vsize    );
	/* set offsetx. */
	XBYTE[REG_Jpeg_Offsetx_Low]= *((UINT8*)&xoff+1     );
	XBYTE[REG_Jpeg_Offsetx_High]= *((UINT8*)&xoff       );
	/* set offsety. */
	XBYTE[REG_Jpeg_Offsety_Low]= *((UINT8*)&yoff+1     );
	XBYTE[REG_Jpeg_Offsety_High]= *((UINT8*)&yoff       );
}

/**
 * @fn        void HAL_JpegSetDestSize(UINT16 DstWidth, UINT16 DstHeight)
 * @brief     set Vlc width and height
 * @param     [in] DstWidth : width MCU allign
 * @param     [in] DstHeight : height MCU allign
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegSetVlcSize(UINT16 VlcWidth, UINT16 VlcHeight)
{
	/* set Dstwidth. */
	XBYTE[REG_Jpeg_DstWidth_Low]=*((UINT8*)&VlcWidth+1 );
	XBYTE[REG_Jpeg_DstWidth_High]=*((UINT8*)&VlcWidth	 );
	/* set Dstheight. */
	XBYTE[REG_Jpeg_DstHeight_Low]=*((UINT8*)&VlcHeight+1  );
	XBYTE[REG_Jpeg_DstHeight_High]=*((UINT8*)&VlcHeight	 );
}

/**
 * @fn        void HAL_JpegSetVlcOffset(UINT16 offset)
 * @brief     set VLC offset
 * @param     [in] offset : byte count
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegSetVlcOffset(UINT16 offset)
{
	/* set Vlc offset. */
	XBYTE[REG_Jpeg_VLCofset_Low]=*((UINT8*)&offset+1 );
	XBYTE[REG_Jpeg_VLCofset_High]=*((UINT8*)&offset	 );
}

/**
 * @fn        UINT8 HAL_JpegQTableWrite(UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt)
 * @brief     setting Q table to hardware
 * @param     [in] The quantization table index. If QTableIndex is zero, it will use *QTable to set Q table
 * @param     [in] yQTable pointer
 * @param     [in] uvQTable pointer
 * @param     [in] zigzagOpt, '0' normal order, '1' zigzag order
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_JpegQTableWrite(
	UINT8 QTableIndex,
	UINT8 *yQTable,
	UINT8 *uvQTable,
	UINT8 zigzagOpt
) USING_0
{
	UINT8 status = HALK_SUCCESS;
	UINT8 i;
	UINT8 j;

	//body
	XBYTE[REG_Jpeg_Noqtbl] = QTableIndex;
	XBYTE[REG_Jpeg_Sramtestsel] = 0x00;                        //select Q-table SRAM

	if( QTableIndex == 100 ) {
		XBYTE[REG_Jpeg_Qvalueone]=0x01;
	}
	else {
		XBYTE[REG_Jpeg_Qvalueone]=0x00;

		if( zigzagOpt == 0 ) {
			for( i = 0; i < 0x40; i++ ) {
				XBYTE[REG_Jpeg_Yqtable + i] = yQTable[i];
			}

			for( i = 0; i < 0x40; i++ ) {
				XBYTE[REG_Jpeg_Cqtable + i] = uvQTable[i];
			}
		}
		else {
			for( i = 0; i < 64; i++ ) {
				j = zigzagTable[ i ];
				XBYTE[ REG_Jpeg_Yqtable + j ] = yQTable[ i ];
			}

			for( i = 0; i < 64; i++ ) {
				j = zigzagTable[ i ];
				XBYTE[ REG_Jpeg_Cqtable + j ] = uvQTable[ i ];
			}
		}
	}

	XBYTE[REG_Jpeg_Sramtestsel]=0x0C;                                     //Disable SRAM Test mode

	return(status);        //return
}


/**************************************************************************
 *                                                                        *
 *  Function Name: HAL_JpegQTableRead                                      *
 *                                                                        *
 *  Purposes: Read the Q table                                            *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *      QTableIndex - The quantization table index, 8'h05 is reserved for *
 *                    all 1's of quantized value. The Q-table will not be *
 *                    changed if index is set to 8'h00.                   *
 *                    0x01 : fill QTable (user defined Q-table) in        *
 *                           raster scan                                  *
 *                    0x02 : fill QTable (user defined Q-table) in        *
 *                           zigzag scan                                  *
 *      QTable      - if QTableIndex is 0xf0 or 0xf1, QTable is filled    *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error Code                                                   *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
/**
 * @fn        UINT8 HAL_JpegQTableRead(UINT8 *QTableIndex, UINT8 *QTable, UINT8 zigzagOpt)
 * @brief     read hal module Q table
 * @param     [out] QTableIndex pointer
 * @param     [out] QTable pointer
 * @param     [in] zigzagOpt, '0' normal order, '1' zigzag order
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_JpegQTableRead(
	UINT8 *QTableIndex,
	UINT8 *QTable,
	UINT8 zigzagOpt
) USING_0
{
	UINT8 status = HALK_SUCCESS;
	data UINT8 i;
	data UINT8 j;

	//body
	*QTableIndex=XBYTE[REG_Jpeg_Noqtbl];
	XBYTE[REG_Jpeg_Sramtestsel] = 0x00;

	if( *QTableIndex != 100 ) {
		if( zigzagOpt == 0 ) {
			for( i = 0; i < 128; i++ ) {
				QTable[ i ] = XBYTE[ REG_Jpeg_Yqtable + i ];
			}
		}
		else {
			for( i = 0; i < 64; i++ ) {
				j = zigzagTable[ i ];
				QTable[ i ] = XBYTE[ REG_Jpeg_Yqtable + j ];
			}
			for( i = 64; i < 128; i++ ) {
				j = zigzagTable[ i - 64 ];
				QTable[ i ] = XBYTE[ REG_Jpeg_Cqtable + j ];
			}
		}
	}

	XBYTE[REG_Jpeg_Sramtestsel]=0x0C;                                     //Disable SRAM Test mode

	return(status);          //return
}

/**
 * @fn        UINT8 HAL_JpegMjpgTrig(UINT32 VLCAAddr, UINT32 VLCBAddr, UINT8 Option)
 * @brief     Video Mjpg trigger to start
 * @param     [in] VLCAAddr: The vlc A buffer word address
 * @param     [in] VLCBAddr: The vlc B buffer word address
 * @param     [in] Option: bit[1:0] 0 - YUV422
 *                       	1 - YUV420
 *                       	2 - Black/White
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_JpegMjpgTrig(
	UINT32 VLCAAddrWord,
	UINT32 VLCBAddrWord,
	UINT8 Option
) USING_0
{
	UINT8 status = HALK_SUCCESS;
	UINT8 imgtype;
	UINT8 rotate;
	// reset vlc index
	XBYTE[REG_Dram_VlcRegIdx] = 0x00;
	XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x10;
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x10;
	//body
	imgtype   = Option&0x03;
	rotate = (Option & 0x1C)>>2;

	HAL_JpegSetVlcOffset(0x0000);
	HAL_DramAVLCAddrSet(VLCAAddrWord);
	HAL_DramBVLCAddrSet(VLCBAddrWord);

	XBYTE[REG_Jpeg_PbreScale]=0x00; // [mantis 20298] reset decode rescale factor

	XBYTE[REG_Jpeg_Rotation_Type]= rotate;    // no rotate
	XBYTE[REG_Jpeg_YUV_Type]=imgtype;
	if (rotate){
		XBYTE[REG_Jpeg_Turboen] = 0; // turbo mode not support rotation
	}else{
		XBYTE[REG_Jpeg_Turboen] = 1;
	}
	XBYTE[REG_Dram_Priority0_lvl2_master] = 0x04; // for HD JPEG first
	XBYTE[REG_Dram_Priority1_lvl2_master] = 0x01; // for audio/*CPU*/ as second priority

	XBYTE[REG_Jpeg_Enthumb]=0x00; //disable the thumbnail(DCT DC value)
	XBYTE[REG_Jpeg_JFIF]=0x01;    // JFIF format

	XBYTE[REG_Jpeg_Intr_Event]=0xFF;    // clear sts

	HAL_DramVLCRdyClr(0);
	HAL_DramVLCRdyClr(1);

	//XBYTE[REG_Dram_LensGainUpdt]=0x0c;           //{3'b0, mpgsb_usb_en, mpgsb_mjpg_en, mpgsb_dispimg_en, 2'h0}
	XBYTE[REG_Dram_LensGainUpdt] |= 0x08; //enable MJPG only, modified by phil

	HAL_DramZfUpdTrig(0x00); //clrpvzfupdate

	XBYTE[REG_Dram_ZfUpdEn]=0x07;           //zfupden

	if( XBYTE[REG_Jpeg_PbreScale] != (UINT8)0x00 ) {
		printf( "ERROR: REG_Jpeg_PbreScale[%bx]\n", XBYTE[REG_Jpeg_PbreScale] );
	}
	XBYTE[0x2000]=0x03;           //cammode: videoclip
	XBYTE[REG_Dram_ClipIdle]=0x01;           //startclip/clipidle
	return HALK_SUCCESS;

}

/**
 * @fn        void HAL_JpegMjpgAbort(void)
 * @brief     Video Mjpg stop
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegMjpgAbort(
	void
)
{    //UINT32 time;
	// 20101004 ShengKai modified for UVC MJPEG YUY2 format swithcing issue
	UINT32 timeout=0xffffffff;
	if(XBYTE[REG_Dram_ImgType]==2)
			XBYTE[REG_Jpeg_Swcontrol]=0x04;//{sw_stopclip,sw_dsof,sw_csof}
	HAL_DramVLCRdyClr(0);
	HAL_DramVLCRdyClr(1);
	while((XBYTE[REG_Dram_ClipIdle]&0x1)==0x0){
		if((timeout--)<1) break;
	};//startclip/clipidle

	XBYTE[0x2000]=0x01;//cammode: preview

	//XBYTE[REG_Dram_LensGainUpdt]=0x04;//{3'b0, mpgsb_usb_en, mpgsb_mjpg_en, mpgsb_dispimg_en, 2'h0}
	XBYTE[REG_Dram_LensGainUpdt] &= ~0x08; //disable MJPG only, modified by phil

	XBYTE[REG_Jpeg_Intr_Enable]&=~0x02;
	XBYTE[REG_Dram_VlcIntEn]&=~0x03;//{6'b0, vlcbinten, vlcainten}

	//HAL_GlobalReadGTimer(&time);printf("time=%ld\n",time);
	// 20101004 ShengKai modified to reset control block after host stopping transmission
	HAL_DramVLCRdyClr(0);
	HAL_DramVLCRdyClr(1);
	XBYTE[REG_Dram_SetVlcRdy]=0x40;
	XBYTE[REG_Usb_VidAudBufRst]|=0x01;
	XBYTE[REG_Dram_SetVlcRdy]=0x00;

	/* set vlc buffer idx to A */
	XBYTE[REG_Dram_MpgSb_RegIdx_En] |= 0x10;
	XBYTE[REG_Dram_VlcRegIdx] = 0x00;
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &= ~0x10;
}

/**
 * @fn        	UINT8 HAL_JpegEncode(UINT32 SrcAddr, UINT32 VLCAddr, UINT32 DCTDCAddr, UINT8 Option, UINT8 wait)
 * @brief		Jpeg encode
 * @SrcAddr		The YUV source word address
 * @VLCAddr		The vlc word address.
 * @DCTDCAddr	The DCT word address.
 * @Option 		bit[1:0] 0 - YUV422
 *                       1 - YUV420
 *                       2 - Black/White
 *              bit[2]     - JPEG rotation function is turned on if set to 1
 *              bit[3]   0 - rotate clockwise
 *                       1 - rotate counterclockwise
 *              bit[5]     - JFIF compatible VLC data is generated if set to 1
 *              bit[6]     - DCT DC value is generated if set to 1
 *              bit[7]     - bypass mode
 *
 * @wait		The block or non-blocking option.
 * @retval		SP_OK If the function return successfully.
 * @since		2007-12-18
 *
 * Jpeg encode
*/
UINT8 HAL_JpegEncode(
	UINT32 SrcAddrWord,
	UINT32 VLCAddrWord,
	UINT32 DCTDCAddrWord,
	UINT8 Option,
	UINT8 wait
)
{
	UINT8 status = HALK_SUCCESS;
	UINT8 error;
	UINT8 imgtype, jpgrote, jfif, dctdcen, bypass;
	UINT8 compdone;
	UINT32 i;

	imgtype   = Option&0x03;
	jpgrote   = (Option>>2)&0x07;
	jfif      = (Option>>5)&0x01;
	dctdcen   = (Option>>6)&0x01;
	bypass    = (Option>>7)&0x01;

	XBYTE[REG_Jpeg_Sclbypss] = bypass;

	if( bypass == (UINT8) 1 ) {
		XBYTE[REG_Jpeg_Enthumb]=0;
	}
	else {
		XBYTE[REG_Jpeg_Enthumb]=dctdcen;                //turn on the thumbnail(DCT DC value)
	}

	error = 0;
	HAL_JpegSetVlcOffset(0x0000);
	HAL_JpegSetYuvAddr(SrcAddrWord);
	HAL_DramAVLCAddrSet(VLCAddrWord);

	if(dctdcen==1 || bypass == 1){                                         //set collect DCT DC value
		XBYTE[REG_Dram_TMBAddr_B0]=*((UINT8*)&DCTDCAddrWord + 3 );      //Thumbnail addr
		XBYTE[REG_Dram_TMBAddr_B1]=*((UINT8*)&DCTDCAddrWord + 2 );
		XBYTE[REG_Dram_TMBAddr_B2]=*((UINT8*)&DCTDCAddrWord + 1 );
		XBYTE[REG_Dram_TMBAddr_B3]=*((UINT8*)&DCTDCAddrWord );
	}

	XBYTE[REG_Jpeg_Rotation_Type]=jpgrote&0x07;                             //Rotate/Flip
	XBYTE[REG_Jpeg_YUV_Type]=imgtype&0xFF;                             //set img type

	if(jfif==1){
		XBYTE[REG_Jpeg_JFIF]=0x01;                         //set JFIF compatible format
	}
	else{
		XBYTE[REG_Jpeg_JFIF]=0x00;
	}

	if( imgtype == 0x01 ) {  // turn off turbo mode for 420 to workaround 0x284c does not change
		XBYTE[REG_Jpeg_Turboen] = 0;
	}

	XBYTE[REG_Jpeg_PbreScale]=0x00; // [mantis 20298] reset decode rescale factor

	if (bypass==0){
		XBYTE[REG_Jpeg_Intr_Event]=0x02; // clean interrupt sts
	}
	else {
	    XBYTE[REG_Jpeg_Intr_Event]=0x10; // clean interrupt sts
	}

	XBYTE[0x2000]=0x00;                                     //Set idle mode
	XBYTE[0x2000]=0x02;                                     //Set DSC mode

	if( XBYTE[REG_Jpeg_PbreScale] != (UINT8)0x00 ) {
		printf( "ERROR: 0x2825[%bx]\n", XBYTE[REG_Jpeg_PbreScale] );
	}

	if (bypass==0){
		XBYTE[REG_Jpeg_Swcontrol]=0x01;                                     //trigger compress action
	   }
	else {
		XBYTE[REG_Jpeg_Swcontrol]=0x08;                                     //trigger compress action
	}

	//    printf("\nHAL_Compress: start to compress....option=%bx\n", Option);

	if( wait == 1 ) {
		compdone=0x00;
		i=0;
		do{
			//printf("[%bx]", XBYTE[REG_Jpeg_Currenty]);

			if( bypass == (UINT8) 1 ) {
				compdone=XBYTE[REG_Jpeg_Intr_Event]&0x10;
			}
			else {
				compdone=XBYTE[REG_Jpeg_Intr_Event]&0x02;
			}

			i++;
		}while(compdone==0x00);

		XBYTE[REG_Jpeg_Intr_Event] = 0x17;

	if( XBYTE[REG_Jpeg_PbreScale] != (UINT8)0x00 ) {
		printf( "ERROR: 0x2825[%bx]\n", XBYTE[REG_Jpeg_PbreScale] );
	}

		//printf("HAL_Compress: compress done....\n");
	}

	if( XBYTE[REG_Jpeg_PbreScale] != (UINT8)0x00 ) {
		printf( "ERROR: 0x2825[%bx]\n", XBYTE[REG_Jpeg_PbreScale] );
	}

	return(status);
}

/**
 * @fn        		UINT8 HAL_JpegDecode(UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr, UINT8 PbRescalSize, UINT8 Option, UINT16 MCURstNo)
 * @brief			Jpeg decode
 * @SrcAddr			The source word address
 * @SrcHsize		The horizontal size of source word address
 * @SrcVsize		The vertical size of source word address
 * @DstAddr			The destination word address
 * @PbRescalSize	Jpeg decompress rescale factor
 * @Option 			bit[1:0] 0 - YUV422
 *                           1 - YUV420
 *                           2 - Black/White
 *                  bit[2]     - JFIF compatible VLC data
 *                  bit[3]   0 - not exit this function till decompress done
 *                           1 - hit and run
 *
 * @MCURstNo		MCU restart number
 * @retval			SP_OK If the function return successfully.
 * @since			2007-12-18
 *
 * Jpeg decode
*/
UINT8 HAL_JpegDecode(
	UINT32 SrcAddrWord,
	UINT16 SrcHsize,
	UINT16 SrcVsize,
	UINT32 DstAddrWord,
	UINT8 PbRescalSize,
	UINT8 Option,
	UINT16 MCURstNo
) USING_0
{
	UINT8 status = HALK_SUCCESS;
	UINT8 error, imgtype, jfif, hitrun, oddAddr;
	UINT8 decompdone;
	UINT16 DramHsize;
	//UINT8 t1, t2;

	//body
	imgtype   =  Option    &0x03;
	jfif      = (Option>>2)&0x01;
	hitrun    = (Option>>3)&0x01;
	oddAddr   = (Option>>4)&0x01;

	error =0;

	/*++++++TQ add for clear vlc buffer index to 0 +++*/
	// 20100630
	XBYTE[REG_Dram_MpgSb_RegIdx_En] |=0x10;
	XBYTE[REG_Dram_VlcRegIdx] =0x00;
	XBYTE[REG_Dram_MpgSb_RegIdx_En] &=0xef;
	/*------------------------------------------*/
	HAL_JpegSetVlcOffset((UINT16)oddAddr);// DRAM get data by word, so set 1 for odd byte
	HAL_DramAVLCAddrSet(SrcAddrWord);
	HAL_JpegSetYuvAddr(DstAddrWord);

	switch(PbRescalSize){
		case 0:       DramHsize =     SrcHsize;                   break;
		case 1:       DramHsize = ((((SrcHsize>>3)  )+7)>>3)<<3;  break;
		case 2:       DramHsize = ((((SrcHsize>>3)*2)+7)>>3)<<3;  break;
		case 3:       DramHsize = ((((SrcHsize>>3)*3)+7)>>3)<<3;  break;
		case 4:       DramHsize = ((((SrcHsize>>3)*4)+7)>>3)<<3;  break;
		case 5:       DramHsize = ((((SrcHsize>>3)*5)+7)>>3)<<3;  break;
		case 6:       DramHsize = ((((SrcHsize>>3)*6)+7)>>3)<<3;  break;
		case 7:       DramHsize = ((((SrcHsize>>3)*7)+7)>>3)<<3;  break;
		default:      DramHsize =     SrcHsize;
	}

	HAL_JpegSetDramHSize(DramHsize);

	XBYTE[REG_Jpeg_Rotation_Type]=0x00; //no rotation

	HAL_JpegSetVlcSize(SrcHsize, SrcVsize);

	XBYTE[REG_Jpeg_PbreScale]= PbRescalSize&0x07;

	XBYTE[REG_Jpeg_YUV_Type]=imgtype&0x03;                     //set YUV format

	if(jfif==1){
		XBYTE[REG_Jpeg_JFIF]=0x01;                 //set JFIF compatible format
	}else{
		XBYTE[REG_Jpeg_JFIF]=0x00;
	}

	if(jfif==1 && MCURstNo !=0){
		XBYTE[REG_Jpeg_Restartmcu_Low] = *((UINT8*)&MCURstNo + 1 );
		XBYTE[REG_Jpeg_Restartmcu_High] = *((UINT8*)&MCURstNo );
	}
	else{
		XBYTE[REG_Jpeg_Restartmcu_Low] = 0;
		XBYTE[REG_Jpeg_Restartmcu_High] = 0;
	}
#ifdef JPG_TIME_MEASURE_IN_GPIO
	/* enable GPIO */
	XBYTE[0x2038] = 0xFF;
	XBYTE[0x2030] = 0xff;
#endif

	XBYTE[REG_Jpeg_Enthumb]=0x00;                                     //Disable thumbnail
	XBYTE[0x2000]=0x00;                                     //set idle mode
	XBYTE[0x2000]=0x05;                                     //set cammode to playback
	XBYTE[REG_Jpeg_Intr_Event] = 0x04; /* clr eofsts */

	XBYTE[REG_Jpeg_Swcontrol]=0x02;                                     //trigger decompress action

	if(hitrun==0) {
		do {                                          //wait till decompress done
			//printf( "[%bx]", XBYTE[REG_Jpeg_Rec_Mcuy_Low] );
			decompdone=XBYTE[REG_Jpeg_Intr_Event]&0x04;
			//    printf("JFIFend=%bx INT=%bx\n", XBYTE[REG_Jpeg_JFIFend], XBYTE[REG_Jpeg_Intr_Event]);
		}while(decompdone==0x00);

			//XBYTE[REG_Jpeg_Intr_Event]=0xFF;
			XBYTE[REG_Jpeg_Intr_Event]=0x04;
			//    printf("HAL_DeCompress: decompress done....CLR INT=%bx\n", XBYTE[REG_Jpeg_Intr_Event]);
	}
	else {
	//    printf("HAL_DeCompress in background....\n");
	}

#ifdef JPG_TIME_MEASURE_IN_GPIO
	XBYTE[0x2030] = 0x00;
#endif
	if(XBYTE[REG_Jpeg_JFIFend] == 0x00){ //error happen
		status = HALK_ERROR_GENERAL;
	}
	//printf("JFIFend=%bx INT=%bx\n", XBYTE[REG_Jpeg_JFIFend], XBYTE[REG_Jpeg_Intr_Event]);

	return(status);
}

/**
 * @fn        		UINT8 HAL_JpgZFactorCropSet(UINT16 hfactor, UINT16 vfactor, UINT16 SrcWidth, UINT16 SrcHeight, UINT16 DstWidth, UINT16 DstHeight, UINT8 option)
 * @brief			Jpeg encode scaling set
 * @hfactor			The horizontal scaling factor
 * @vfactor			The vertical scaling factor
 * @SrcWidth		The source width size
 * @SrcHeight		The source height size
 * @DstWidth		The destination width size
 * @DstHeight		The destination height size
 * @option 			bit[0] 		- JPEG rotation function is turned on if set to 1
 *              	bit[1:2]  0	- rotate clockwise
 *                       	  1	- rotate counterclockwise
 *                  bit[3]      - mode for jpeg scaling
 *                  bit[4]  	- bypass option
 *                  bit[5]  	- dist enable option, '1' enable, '0' disable
 * @retval			SP_OK If the function return successfully.
 * @since			2007-12-18
 *
 * Jpeg scaling set
 */
UINT8
HAL_JpgZFactorCropSet(
	UINT16 hfactor,
	UINT16 vfactor,
	UINT16 SrcWidth,
	UINT16 SrcHeight,
	UINT8 distClampx,
	UINT8 distClampy,
	UINT16 DstWidth,
	UINT16 DstHeight,
	UINT8 option
)
{
	HAL_JpegRoiSclSet(hfactor, vfactor, SrcWidth, SrcHeight,
					SrcWidth, SrcHeight, 0, 0, distClampx, distClampy, DstWidth, DstHeight, option&0xBF);

	return HALK_SUCCESS;
}

/**
 * @fn        UINT8 HAL_JpegRoiSclSet(UINT16 hfactor, UINT16 vfactor, UINT16 SrcWidth, UINT16 SrcHeight, UINT16 RoiWidth, UINT16 RoiHeight, UINT16 RoiXOff, UINT16 RoiYOff, UINT8 distClampx, UINT8 distClampy, UINT16 DstWidth, UINT16 DstHeight, UINT8 option)
 * @brief     set size Roi or Zoom scale
 * @param     [in] hfactor : zoom horizontal factor
 * @param     [in] vfactor : zoom vertical factor
 * @param     [in] SrcWidth : Source widht
 * @param     [in] SrcHeight : source height
 * @param     [in] RoiWidth : crop widht
 * @param     [in] RoiHeight : crop height
 * @param     [in] RoiXOff : crop x offset
 * @param     [in] RoiYOff : crop y offset
 * @param     [in] distClampx : dist clamp x
 * @param     [in] distClampy : dist clamp y
 * @param     [in] DstWidth : destination x size
 * @param     [in] DstHeight : destination y size
 * @param     [in] option :
 *					[2:0]: rotation mode
 *					[3] : scale factor mode// always set 0, set 1 to pop a scale factor from FIFO
 *					[4] : bypass enable
 *					[5] : dist enable
 *					[6] : size ROI enable
 *					[7] : video zoom status
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_JpegRoiSclSet(
	UINT16 hfactor,
	UINT16 vfactor,
	UINT16 SrcWidth,
	UINT16 SrcHeight,
	UINT16 RoiWidth,
	UINT16 RoiHeight,
	UINT16 RoiXOff,
	UINT16 RoiYOff,
	UINT8 distClampx,
	UINT8 distClampy,
	UINT16 DstWidth,
	UINT16 DstHeight,
	UINT8 option
)
{
	UINT32 hsf, vsf;
	UINT32 hsfini, vsfini;
	UINT32 roi_hsize, roi_vsize;
	UINT16 thumbWSize;
	UINT32 qh, rh, qv, rv;
	UINT16 distSrcX,distSrcY;
	UINT16 fHSize,fVSize;
	UINT8 bypass, rot, mode,disten;
	UINT8 sizeRoiEn; // '0' scale factor ROI, '1' set x/y size, x/y offset ROI.
	UINT8 vdZoom;
	UINT16 hfacT=100,vfacT=100;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points

	rot    = (option   )&0x07;
	mode   = (option>>3)&0x01;
	bypass = (option>>4)&0x01;
	disten = (option>>5)&0x01;
	sizeRoiEn = (option>>6)&0x01; // when it is on, DIST should be close
	vdZoom = (option>>7)&0x01; //when record video, image size should be the same with MPEG buffer size

	if (disten){
		distSrcX = SrcWidth - (UINT16)((UINT32)distClampx*DstWidth/(UINT32)320);
		distSrcY = SrcHeight - (UINT16)((UINT32)distClampy*DstWidth/(UINT32)320);
		//printf("Jpeg x=%d, y=%d, clmx=%bd, clmy=%bd\n",distSrcX,distSrcY,distClampx,distClampy);
	}
	else{
		distSrcX = SrcWidth;
		distSrcY = SrcHeight;
	}

	/* set turboen. */
	if( rot == 0 && mode == 0x00 ) {
		XBYTE[REG_Jpeg_Turboen] = 1;
	}
	else {
		XBYTE[REG_Jpeg_Turboen] = 0;
	}

	if (hfactor==101){
		hfacT = 100;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	}else{
		hfacT = hfactor;
	}
	if (vfactor==101){
		vfacT = 100;//for zoom in 1.5~4 right/bottom white line. resulted in JPEG scale catch error points
	}else{
		vfacT = vfactor;
	}
	if (sizeRoiEn==0){
		qh = (UINT32)distSrcX * (UINT32)100 / (UINT32)hfacT;
		rh = (UINT32)distSrcX * (UINT32)100 % (UINT32)hfacT;
		roi_hsize = qh * (UINT32)65536 + rh * (UINT32)65536 / (UINT32)hfacT;

		qv = (UINT32)distSrcY * (UINT32)100 / (UINT32)vfacT;
		rv = (UINT32)distSrcY * (UINT32)100 % (UINT32)vfacT;
		roi_vsize = qv * (UINT32)65536 + rv * (UINT32)65536 / (UINT32)vfacT;
	}else{
		qh = (UINT32)RoiWidth;
		roi_hsize = qh * (UINT32)65536 ;

		qv = (UINT32)RoiHeight;
		roi_vsize = qv * (UINT32)65536 ;
	}

	*((UINT8*)&hsfini  ) = *((UINT8*)&distSrcX  );
	*((UINT8*)&hsfini+1) = *((UINT8*)&distSrcX+1);
	*((UINT8*)&hsfini+2) = 0;
	*((UINT8*)&hsfini+3) = 0;

	*((UINT8*)&vsfini  ) = *((UINT8*)&distSrcY  );
	*((UINT8*)&vsfini+1) = *((UINT8*)&distSrcY+1);
	*((UINT8*)&vsfini+2) = 0;
	*((UINT8*)&vsfini+3) = 0;

	if (sizeRoiEn==0){
		hsfini = (hsfini-roi_hsize)>>1;
		vsfini = (vsfini-roi_vsize)>>1;
	}else{
		hsfini = (UINT32)RoiXOff;
		vsfini = (UINT32)RoiYOff;
	}

	hsf = ((UINT32)roi_hsize/(UINT32)DstWidth);
	vsf = ((UINT32)roi_vsize/(UINT32)DstHeight);

#if 1
	roi_hsize = hsf*(UINT32)DstWidth +(hsfini&0xFFFF);
	roi_vsize = vsf*(UINT32)DstHeight+(vsfini&0xFFFF);

	if ((roi_hsize&0xFFFF)!=0)	roi_hsize = roi_hsize+(UINT32)0x10000;
	if ((roi_vsize&0xFFFF)!=0)	roi_vsize = roi_vsize+(UINT32)0x10000;
#endif

	/* jpeg engine limitation: h and v must both scale or both no scale. */
	if ( qh == DstWidth && qv != DstHeight ) {
		//hsf = 0xFFFFFFFFL;
		hsf = 0x00000000L;
	}
	else if ( qv == DstHeight && qh != DstWidth ) {
		//vsf = 0xFFFFFFFFL;
		vsf = 0x00000000L;
	}

	if (hsf<0x2000){		/* turbo scale doesn't work while zoom factor >=8x*/
		XBYTE[REG_Jpeg_Turboen] = 0;
	}

	if ( rot & 0x01 ) {  /* rotate. */
		HAL_JpegSetSclFact(vsf, hsf, vsfini, hsfini);
	}
	else {  /* no rotate. */
		HAL_JpegSetSclFact(hsf, vsf, hsfini, vsfini);
	}

	/* jpg limit : Turbo enable and jpg scale disable, h offset need 16x */
	//HAL_JpegSetROI(roi_hsize>>16, roi_hsize>>16, hsfini>>16, vsfini>>16);
	if (hfactor==101){
		HAL_JpegSetROI(roi_hsize>>16, roi_vsize>>16, (hsfini>>16)+2, (vsfini>>16)+2);
	}else{
		HAL_JpegSetROI(roi_hsize>>16, roi_vsize>>16, hsfini>>16, vsfini>>16);
	}
	if ( mode == 0x00 ) {
		/* set Stillsb_hsize. */
		HAL_JpegSetDramHSize(SrcWidth);

		if (vdZoom){
			HAL_Dram1stPathFrmBufSizeGet(&fHSize,&fVSize);
			//fHSize = (roi_hsize>>16) + (hsfini>>16);
			//fVSize = (roi_vsize>>16) + (vsfini>>16);
			HAL_JpegSetSourceSize(fHSize, fVSize);
		}else{
			HAL_JpegSetSourceSize(SrcWidth, SrcHeight);
		}

		if ( rot & 0x01 ) {  /* rotate. */
			HAL_JpegSetVlcSize(DstHeight, DstWidth);

			if( bypass == 1 ) {
				thumbWSize = DstHeight;
			}
			else {
				thumbWSize = DstHeight>>3;
			}
		}
		else {  /* no rotate. */
			HAL_JpegSetVlcSize(DstWidth, DstHeight);

		if( bypass == 1 ) {
			thumbWSize = DstWidth;
			}
			else {
				thumbWSize = DstWidth>>3;
			}
		}

		/* set TMBhsize. */
		thumbWSize = ( thumbWSize + 7 ) & ~7;
		XBYTE[REG_Dram_TMBHSize_Low]=*((UINT8*)&thumbWSize+1  );
		XBYTE[REG_Dram_TMBHSize_High]=*((UINT8*)&thumbWSize    );
	}

	if ( mode == 0x01 ) {
		XBYTE[REG_Jpeg_ZfPop] = 1; // PUSH
	}

	#if 0
		printf (" hsf     = %x  \n", *((UINT16*)&hsf+1));
		printf (" vsf     = %x  \n", *((UINT16*)&vsf+1));
		printf (" hsfini  = %x  \n", *((UINT16*)&hsfini+1));
		printf (" vsfini  = %x  \n", *((UINT16*)&vsfini+1));
		printf (" roi_h   = %x  \n", *((UINT16*)&roi_hsize));
		printf (" roi_v   = %x  \n", *((UINT16*)&roi_vsize));
		printf (" roi_x   = %x  \n", *((UINT16*)&hsfini));
		printf (" roi_y   = %x  \n", *((UINT16*)&vsfini));
		printf (" factor  = %d  \n", hfactor);
		//printf (" 0x2860 = %bx \n", XBYTE[REG_Jpeg_Srcwidth_Low]);
		//printf (" 0x2861 = %bx \n", XBYTE[REG_Jpeg_Srcwidth_High]);
    #endif

	return HALK_SUCCESS;
}


/**
 * @fn       		UINT8 HAL_JpgROISet(UINT8 option, UINT16 SrcHsize, UINT16 SrcVsize, UINT16 SrcOffsetX, UINT16 SrcOffsetY, UINT16 DetHsize, UINT16 DetVsize, UINT8 rot)
 * @brief			Jpeg encode ROI set
 * @option 			bit[4]  	- bypass option
 * @SrcHsize		The horizontal size of source
 * @SrcVsize		The vertical size of source
 * @SrcOffsetX		The X size of offset
 * @SrcOffsetY		The Y size of offset
 * @DetHsize		The destination horizontal size
 * @DetVsize		The destination vertical size
 * @rot				The rotation option
 * @retval			SP_OK If the function return successfully.
 * @since			2007-12-18
 *
 * Jpeg encode ROI set
 */
UINT8
HAL_JpgROISet(
    UINT8  option,
	UINT16 SrcHsize,
	UINT16 SrcVsize,
	UINT16 SrcOffsetX,
	UINT16 SrcOffsetY,
	UINT16 DetHsize,
	UINT16 DetVsize,
    UINT8 rot
) USING_0
{
	UINT32 SRCx64k;
	UINT16 HSF, VSF, HSFINI, VSFINI;
	UINT16 thumbWSize;
	UINT8 bypass;

	bypass    = (option>>4)&0x01;

	if( SrcHsize == DetHsize && SrcVsize == DetVsize && rot == 0 ) {
		XBYTE[REG_Jpeg_Turboen] = 1;
	}
	else {
		XBYTE[REG_Jpeg_Turboen] = 0;
	}

	//******* ROI **********
	HAL_JpegSetROI(SrcHsize, SrcVsize, SrcOffsetX, SrcOffsetY);

	//******* Scaling up setting *******
	if((rot&0x01)==0)
	{
		*((UINT8*)&SRCx64k  ) = *((UINT8*)&SrcHsize  );
		*((UINT8*)&SRCx64k+1) = *((UINT8*)&SrcHsize+1);
		*((UINT8*)&SRCx64k+2) = 0;
		*((UINT8*)&SRCx64k+3) = 0;

		HSF = (UINT16)(SRCx64k/DetHsize);
		HSFINI=(UINT16)((SRCx64k-((UINT32)HSF*(UINT32)DetHsize))>>1);

		*((UINT8*)&SRCx64k  ) = *((UINT8*)&SrcVsize  );
		*((UINT8*)&SRCx64k+1) = *((UINT8*)&SrcVsize+1);

		VSF = (UINT16)(SRCx64k/DetVsize);

		VSFINI=(UINT16)((SRCx64k-((UINT32)VSF*(UINT32)DetVsize))>>1);
	}
	else
	{
		*((UINT8*)&SRCx64k  ) = *((UINT8*)&SrcVsize  );
		*((UINT8*)&SRCx64k+1) = *((UINT8*)&SrcVsize+1);
		*((UINT8*)&SRCx64k+2) = 0;
		*((UINT8*)&SRCx64k+3) = 0;

		HSF = (UINT16)(SRCx64k/DetHsize);
		HSFINI=(UINT16)((SRCx64k-((UINT32)HSF*(UINT32)DetHsize))>>1);

		*((UINT8*)&SRCx64k  ) = *((UINT8*)&SrcHsize  );
		*((UINT8*)&SRCx64k+1) = *((UINT8*)&SrcHsize+1);

		VSF = (UINT16)(SRCx64k/DetVsize);

		VSFINI=(UINT16)((SRCx64k-((UINT32)VSF*(UINT32)DetVsize))>>1);
	}
	//**********************************
	HAL_JpegSetSclFact((UINT32)HSF<<16, (UINT32)VSF<<16, (UINT32)HSFINI<<16, (UINT32)VSFINI<<16);

	if ((option&0x02)!=0x02) {
		HAL_JpegSetDramHSize(SrcHsize);
		HAL_JpegSetSourceSize(SrcHsize, SrcVsize);
		HAL_JpegSetVlcSize(DetHsize, DetVsize);

		if( bypass == 1 ) {
			if((rot&0x01)==0) {
				thumbWSize = DetHsize;
			}
			else {
				thumbWSize = DetVsize;
			}
			thumbWSize = ( ( thumbWSize + 7 ) >> 3 ) << 3;
		}
		else {
			if((rot&0x01)==0) {
				thumbWSize = DetHsize>>3;
			}
			else {
				thumbWSize = DetVsize>>3;
			}
			thumbWSize = ( ( thumbWSize + 7 ) >> 3 ) << 3;
		}

		XBYTE[REG_Dram_TMBHSize_Low]=*((UINT8*)&thumbWSize+1   );   			//Thumbnail width = src width/8
		XBYTE[REG_Dram_TMBHSize_High]=*((UINT8*)&thumbWSize     );
	}

    return HALK_SUCCESS;
}

/**
 * @fn        void HAL_JpegVlcBoundSet(UINT16 enable, UINT32 size)
 * @brief     Jpeg encode vlc boundary set
 * @param     [in] enable
 * @param     [in] size: The boundary size in byte unit
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegVlcBoundSet(
	UINT8 enable,
	UINT8 intrEn,
	UINT32 size
)
{
	XBYTE[REG_Jpeg_VLCBounden] = enable;

	XBYTE[REG_Jpeg_Intr_Enable] = (0xf7 & XBYTE[REG_Jpeg_Intr_Enable]) | ((intrEn&0x01)<<3);

	XBYTE[REG_Jpeg_VLCBound_Byte0]=*((UINT8*)&size + 3 );
	XBYTE[REG_Jpeg_VLCBound_Byte1]=*((UINT8*)&size + 2 );
	XBYTE[REG_Jpeg_VLCBound_Byte2]=*((UINT8*)&size + 1 );
}

/**
 * @fn        UINT8 HAL_JpegReadEncodeSize(UINT32 *Compsize)
 * @brief     Get jpeg encode vlc size
 * @param     [in] Vlc size in byte unit
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8 HAL_JpegReadEncodeSize(UINT32 *Compsize) USING_0
{
	UINT8 status = HALK_SUCCESS;
	UINT32 tmp;
	UINT8 tmp0,tmp1,tmp2;

	// SPCA 1628 feature, first read REG_Jpeg_VLCSize_Byte0, then REG_Jpeg_VLCSize_Byte1 and REG_Jpeg_VLCSize_Byte2

	tmp0 = XBYTE[REG_Jpeg_VLCSize_Byte0];
	tmp0 = XBYTE[REG_Jpeg_VLCSize_Byte0];
	tmp1 = XBYTE[REG_Jpeg_VLCSize_Byte1];
	tmp2 = XBYTE[REG_Jpeg_VLCSize_Byte2];
	tmp = (UINT32)tmp0 + ((UINT32)tmp1<<8) + ((UINT32)tmp2<<16);
	//printf("vlc size========%ld=%lx\n",tmp,tmp);
	*Compsize = tmp;

	return(status);
}

/**
 * @fn        void HAL_JpegDecodeWait(void)
 * @brief     wait decode completed
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
void HAL_JpegDecodeWait(void)
{
	while(!(XBYTE[REG_Jpeg_Intr_Event]&0x04));

	XBYTE[REG_Jpeg_Intr_Event]=0xFF;
}

/**
 * @fn        void sp1kHalJpegDecode(UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr, UINT8 PbRescalSize, UINT8 Option, UINT16 MCURstNo)
 * @brief     hal jpeg decode
 * @param     [in] SrcAddr: source word address
 * @param     [in] SrcHsize: source width
 * @param     [in] SrcVsize: source height
 * @param     [in] DstAddr: destination word address
 * @param     [in] PbRescalSize: scale down ratio(1/8 ~ 8/8)
 * @param     [in] Option: [1:0]:'0' for yuv 422, '1' for yuv420, '2' for yuv400
 * @param     [in] MCURstNo: Restart MCU number
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
void sp1kHalJpegDecode(
	UINT32 SrcAddrWord,
	UINT16 SrcHsize,
	UINT16 SrcVsize,
	UINT32 DstAddrWord,
	UINT8 PbRescalSize,
	UINT8 Option,
	UINT16 MCURstNo
) USING_0
{
	HAL_JpegDecode( SrcAddrWord, SrcHsize, SrcVsize, DstAddrWord, PbRescalSize, Option, MCURstNo );
}

/**
 * @fn        void sp1kHalJpegDecodeWait(void)
 * @brief     sp1k API for wait decode done
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
void
sp1kHalJpegDecodeWait(
	void
)
{
	HAL_JpegDecodeWait();
}

//QTab is prepared for zigzagOpt
/**
 * @fn        void INTR_halJpegQTableWrite(UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt)
 * @brief     set hal module Q table in interrupt
 * @param     [in] QTableIndex
 * @param     [in] yQTable pointer
 * @param     [in] uvQTable pointer
 * @param     [in] zigzagOpt
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
#pragma OT(8,SPEED) // optimize with speed first
void INTR_halJpegQTableWrite(
	UINT8 QTableIndex,
	UINT8 *yQTable_0,
	UINT8 *uvQTable_0,
	UINT8 zigzagOpt
) USING_1
{
	data UINT8 i;
	data UINT8 j;
	data UINT8* yQTable;
	data UINT8* uvQTable;

	yQTable = yQTable_0;
	uvQTable = uvQTable_0;
	//body
	XBYTE[REG_Jpeg_Noqtbl] = QTableIndex;
	XBYTE[REG_Jpeg_Sramtestsel] = 0x00;                        //select Q-table SRAM

	if( QTableIndex == 100 ) {
		XBYTE[REG_Jpeg_Qvalueone]=0x01;
	}
	else {
		XBYTE[REG_Jpeg_Qvalueone]=0x00;

		if( zigzagOpt == 0 ) {
			for( i = 0; i < 0x40; i++ ) {
				XBYTE[REG_Jpeg_Yqtable + i] = yQTable[i];
				XBYTE[REG_Jpeg_Cqtable + i] = uvQTable[i];
			}
		}
		else {
			for( i = 0; i < 64; i++ ) {
				j = zigzagTable[ i ];
				XBYTE[ REG_Jpeg_Yqtable + j ] = yQTable[ i ];
				XBYTE[ REG_Jpeg_Cqtable + j ] = uvQTable[ i ];
			}
		}
	}

	XBYTE[REG_Jpeg_Sramtestsel]=0x0C;                                     //Disable SRAM Test mode
}

//xian ++ 20090915
/**
 * @fn        UINT8 HAL_JpgZFactorSet(UINT16 hfactor, UINT16 vfactor, UINT16 SrcWidth, UINT16 SrcHeight, UINT16 DstWidth, UINT16 DstHeight, UINT8 option)
 * @brief     hal jpeg zoom factor set
 * @param     [in] hfactor
 * @param     [in] vfactor
 * @param     [in] SrcWidth
 * @param     [in] SrcHeight
 * @param     [in] DstWidth
 * @param     [in] DstHeight
 * @param     [in] option
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_JpgZFactorSet(
	UINT16 hfactor,
	UINT16 vfactor,
	UINT16 SrcWidth,
	UINT16 SrcHeight,
	UINT16 DstWidth,
	UINT16 DstHeight,
	UINT8 option
)
{
	return HAL_JpgZFactorCropSet(hfactor, vfactor, SrcWidth, SrcHeight, 0,0,DstWidth, DstHeight, option);
}
//xian --

//==========================================//
//  initial JPG src/dst add and setting
//  will
//=========================================//
/**
 * @fn        UINT8 Hal_JpgInit(UINT32 jpgsrcaddr, UINT32 vlcaddr, UINT16 hsize, UINT16 vsize, UINT8 fastmode)
 * @brief     Hal_Jpginit
 * @param     [in] jpgsrcaddr
 * @param     [in] vlcaddr
 * @param     [in] fastmode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will
 * @since     2010-4-22
 * @todo      N/A
 * @bug       N/A
*/



UINT8
HAL_JpgInit(
	UINT32 srcAddr,
	UINT32 vlcAddr
	//UINT8  fastMode

)
{

	//UINT32 tmpvlcaddr;
	UINT32 tmpsrcaddr;
	//printf("Enter do jpg init !!\n");
	//tmpvlcaddr = vlcaddr+0x138;

    XBYTE[REG_Jpeg_Swreset]=0x01;    //JPG software reset
	XBYTE[REG_Jpeg_Swreset]=0x00;

	tmpsrcaddr = srcAddr;
	XBYTE[0x2000]=0x00;      // will: camera mode

    HAL_DramAVLCAddrSet(vlcAddr); /* vlc a addr */

	HAL_DramBVLCAddrSet(vlcAddr); /* vlc b addr */

	HAL_JpegSetYuvAddr(srcAddr);	/* YUV address */

	//XBYTE[REG_Jpeg_Turboen] = fastMode;                    	/* turbo mode*/

	XBYTE[0x2000] = 0x00;                        	/* compress */
	XBYTE[REG_Jpeg_YUV_Type] = 0x00;                        	/* YUV422 */

	XBYTE[REG_Jpeg_Enthumb] = 0x00;                        	/* disable thumbnail */
	XBYTE[REG_Jpeg_JFIF] = 0x01;                        	/* JFIF   */
	XBYTE[REG_Jpeg_Truncated] = 0x00;                        	/* bit0: 0/1 => wo/wi truncation */
	//XBYTE[REG_Jpeg_Qvalueone] = 0x01;                        	/* Q=1,Qscale disable */
	XBYTE[REG_Jpeg_PbreScale] = 0x00;                        	/* It must be set to zero*/

	XBYTE[REG_Jpeg_Intr_Enable] = 0;                           	/* interrupten*/
	XBYTE[REG_Jpeg_Intr_Event] = 0xff;                        	/* clr interrupt*/
	//printf("Exit do jpg init !!\n");

	HAL_JpegSetVlcOffset(0);

	return HALK_SUCCESS;
}

/**
 * @fn        void HAL_JpegDataStampSet(UINT32 addrWord, UINT16 DramHsize, UINT16 xoff, UINT16 yoff, UINT8 xsize, UINT16 ysize, UINT8 thrhold)
 * @brief     set basic data stamp before enable data stamp function
 * @param     [in] addrWord : data stamp resource file word address. should be 4 allign, cause Jpeg get 4 word data from DRAM at a time.
 * @param     [in] DramHsize : dram horizontal size for resource file
 * @param     [in] xoff : x offset in snap picture based on MCU
 * @param     [in] yoff : y offset in snap picture based on MCU
 * @param     [in] xsize : data stamp x size based on MCU
 * @param     [in] ysize : data stamp y size based on MCU
 * @param     [in] thrhold : data stamp paste thresh hold
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_JpegDataStampSet(
	UINT32 addrWord,
	UINT16 DramHsize,
	UINT16 xoff,
	UINT16 yoff,
	UINT8  xsize,
	UINT16 ysize,
	UINT8  thrhold
)
{

	// xoff, yoff, xsize and ysize both base on MCU(422:16*8; 420:16*16; 400:16*8)
	ysize &= 0x01FF;  // only [8:0] is valid
	XBYTE[REG_Dram_StampHSize_Low] = *((UINT8*)&DramHsize+1);
	XBYTE[REG_Dram_StampHSize_High] = *((UINT8*)&DramHsize+0);
	XBYTE[REG_Dram_StampStAddr_B0] = *((UINT8*)&addrWord+3); /* stamp addr */
	XBYTE[REG_Dram_StampStAddr_B1] = *((UINT8*)&addrWord+2);
	XBYTE[REG_Dram_StampStAddr_B2] = *((UINT8*)&addrWord+1);
	XBYTE[REG_Dram_StampStAddr_B3] = *((UINT8*)&addrWord+0);

	XBYTE[REG_Jpeg_Stamhoffset_Low] = *((UINT8*)&xoff+1); /* stamp h offset */
	XBYTE[REG_Jpeg_Stamhoffset_High] = *((UINT8*)&xoff+0);
	XBYTE[REG_Jpeg_Stamvoffset_Low] = *((UINT8*)&yoff+1); /* stamp v offset */
	XBYTE[REG_Jpeg_Stamvoffset_High] = *((UINT8*)&yoff+0);
	XBYTE[REG_Jpeg_Stmhsize] = xsize; /* stamp h size */

	XBYTE[REG_Jpeg_Stamvsize_Low] = *((UINT8*)&ysize+1); /* stamp v size */
	XBYTE[REG_Jpeg_Stamvsize_High] = *((UINT8*)&ysize+0);

	XBYTE[REG_Jpeg_Stampthr] = thrhold; /* Y thr */

}

/**
 * @fn        void HAL_JpegDataStampEnable(UINT8 en, UINT8 ResMode)
 * @brief     enable data stamp function and set normal/reverse mode
 * @param     [in] en : '1' enable, '0' disable
 * @param     [in] Reverse/normal Mode: '1' for reverse, '0' for normal
 *				NOTE: when Reverse mode, date stamp size should be equal to YUV size.
 *				should  be xoff>0 and xoff+xsize < total hsize, yoff>0 and y0ff+ysize<total vsize
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-4-27
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_JpegDataStampEnable(UINT8 en, UINT8 RevMode){
	XBYTE[REG_Jpeg_StampEn] = en | (RevMode<<1); // [0]: Data Stamp enable, high active. [1]: reverse mode, high active
}

/**
 * @fn        void HAL_JpegBypassScaleUp(UINT16 SrcYuvAddrWord, UINT16 SrcWidth, UINT16 SrcHeight, UINT16 RoiWidth, UINT16 RoiHeight, UINT16 hOffset, UINT16 vOffset, UINT16 DstYuvAddrWord, UINT16 DstWidth, UINT16 DstHeight, UINT8 option)
 * @brief     jpeg bypass scale up
 * @param     [in] SrcYuvAddrWord : source yuv address
 * @param     [in] SrcWidth : source widht (total width)
 * @param     [in] SrcHeight : source height (total height)
 * @param     [in] RoiWidth : ROI width (Crop width)
 * @param     [in] RoiHeight : ROI height (Crop height)
 * @param     [in] hOffset : x offset
 * @param     [in] vOffset : y offset
 * @param     [in] DstYuvAddrWord : destination yuv address
 * @param     [in] DstWidth : destination width
 * @param     [in] DstHeight : destination height
 * @param     [in] option :
 *                    [1:0] : '0' : YUV422, '1' : YUV420, '2' : YUV400=BW mode, '3' reserved
 *                    [4:2] : rotate, '0': Normal, '1' :90+vertical mirror, '2' : vertical mirror '3': 270
 *                                '4' : vertical mirror, '5' : 90, '6' : 180, '7' : 90+horizontal mirror
 *                    [5] : jfif enable
 *                    [6] : dct dc enable
 *                    [7] : bypass
 * @retval    NONE
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-5-20
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_JpegBypassScaleUp(
	UINT32 SrcYuvAddrWord,
	UINT16 SrcWidth,
	UINT16 SrcHeight,
	UINT16 RoiWidth,
	UINT16 RoiHeight,
	UINT16 hOffset,
	UINT16 vOffset,
	UINT32 DstYuvAddrWord,
	UINT16 DstWidth,
	UINT16 DstHeight,
	UINT8 option
)
{
	UINT8 option_t=0;

	/* option :
	*	[1:0] : '0' : YUV422, '1' : YUV420, '2' : YUV400=BW mode, '3' reserved
	*	[4:2] : rotate, '0': Normal, '1' :90+vertical mirror, '2' : vertical mirror '3': 270
	*				'4' : vertical mirror, '5' : 90, '6' : 180, '7' : 90+horizontal mirror
	*	[5] : jfif enable
	*	[6] : dct dc enable
	*	[7] : bypass
	*/
	//option_t &= 0xD7; // force mode=0, disten=0;
	option_t |= 0x50; // force bypass=1, sizeRoiEn=1;
	option_t |= (option>>2)&0x07;
	HAL_JpegRoiSclSet(100, 100, SrcWidth, SrcHeight,
					RoiWidth, RoiHeight, hOffset, vOffset, 0, 0,
					DstWidth, DstHeight, option_t);
	option_t = option&0x1F; //disable jfif and dct dc
	option_t |= 0x80; //enable bypass
	HAL_JpegEncode(SrcYuvAddrWord, DstYuvAddrWord, DstYuvAddrWord, option_t, 1);
}

/**
 * @fn        UINT8 HAL_JpgRoiEncSet(UINT16 SrcWidth, UINT16 SrcHeight, UINT16 RoiWidth, UINT16 RoiHeight, UINT16 RoiXOff, UINT16 RoiYOff, UINT16 DstWidth, UINT16 DstHeight, UINT8 option)
 * @brief     HAL_JpgRoiEncSet for set size ROI
 * @param     [in] SrcWidth : source width(MCU align)
 * @param     [in] SrcHeight : source height
 * @param     [in] RoiWidth : ROI width
 * @param     [in] RoiHeight : ROI height
 * @param     [in] RoiXOff : ROI x offset
 * @param     [in] RoiYOff : ROI y offset
 * @param     [in] DstWidth : destination widht
 * @param     [in] DstHeight : destination height
 * @param     [in] option: diable bit [5]
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Tq.Zhang
 * @since     2010-7-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_JpgRoiEncSet(
	UINT16 SrcWidth,
	UINT16 SrcHeight,
	UINT16 RoiWidth,
	UINT16 RoiHeight,
	UINT16 RoiXOff,
	UINT16 RoiYOff,
	UINT16 DstWidth,
	UINT16 DstHeight,
	UINT8 option
) USING_0
{
	// option & 0xDF to disable DIST
	HAL_JpegRoiSclSet(100, 100, SrcWidth, SrcHeight,
					RoiWidth, RoiHeight, RoiXOff, RoiYOff, 0, 0, DstWidth, DstHeight, option&0xDF);

	return HALK_SUCCESS;
}
