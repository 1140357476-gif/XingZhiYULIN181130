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
#include "vndreq.h"
#include "main.h"
#include "initio.h"
#include "ctlsetup.h"
//#include "cardusr.h"
#include "cardlink.h"
//#ifdef ICAT_OPTION
//#include "i_cat.h"
//#endif
#include "dpf_scan.h"
#include "hal_cdsp_iq.h" 
#include "dcf_api.h"

#include "sp1k_fd_api.h"
#include "class.h"
#include "reg_def.h"
#include "usb_uitool.h"
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
typedef struct FramePos_s
{
	UINT16 xpos;
	UINT16 ypos;
	UINT16 width;
	UINT16 heigth;
}FramePos_t;
xdata FramePos_t AFWINDOW[5];
xdata UINT32 afBuf[5]={0,0,0,0,0};


 
xdata 	UINT8 	G_Image_Type;
#ifdef DPF_SCANNER
xdata 	UINT8 	G_DPF_Enable = 0x01;
xdata 	UINT8 	G_DPF_View_Frame = 0;	
#else
xdata 	UINT8 	G_DPF_Enable = 0x00;	
#endif 
UINT8  G_PCCAME_FD_ON = 0x00;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//extern UINT8 fdKeyRtX;
//extern UINT8 fdKeyRtY;
//extern UINT8 fdKeyRtS;
//extern UINT8 fdRtSize[];
//extern UINT8 fdFaceNum;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//VNDREQ_NoDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void VNDREQ_NoDataPhase(void)
 * @brief     process standard request without data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void VNDREQ_NoDataPhase(void) USING_1
{
	if (G_pCtrlCommand->bRequest)
    	{
    	//printf("VNDREQ_NoDataPhase\n");
        //not register write
		/*if (((G_pCtrlCommand->bRequest)== UITOOL_REQUEST)&&((G_pCtrlCommand->wIndex>>8)==0x03)) //CX ++ for UI TOOL
		{
			//call function
			printf("download finish\n");
			uiToolProcessData((UINT8)G_pCtrlCommand->wIndex);
		}
	        else */if ((G_pCtrlCommand->bRequest & 0xf0) != 0x20) //Deferred Procedure Call in main lock loop
	        {
			#ifdef DPF_SCANNER
				if(G_pCtrlCommand->bRequest == 0x51) {      //Set Object Count
					if (G_pCtrlCommand->wIndex == 0) {
						//printf("here\n");
						snapMainVlcSize = 0;
			 			snapHdrSize = 0;
					}
					else if (G_pCtrlCommand->wIndex == 2) { // preview clear the vlc after get image
						if ( G_DPF_View_Frame == 0x01 ){
							XBYTE[REG_Dram_VlcRdy] = 0x01;
						}
						else if ( G_DPF_View_Frame == 0x02 ){
							XBYTE[REG_Dram_VlcRdy] = 0x02;
						} 
						return;
					}
				}
			#endif
	                //lock fields of request
	            G_bRequestLock =
	                    G_bRequestLog =
	                            G_pCtrlCommand->bRequest;
	            G_wValueLock = G_pCtrlCommand->wValue;
	            G_wIndexLock = G_pCtrlCommand->wIndex;
	            //patch4.5@yichang for USB VC begin
	            G_HAL_ModuleID = (UINT8)(G_wIndexLock>>8);
	            G_L1_ModuleID = G_HAL_ModuleID;
	            G_FuncID = (UINT8)(G_wIndexLock&0x00ff);
	            G_HAL_FuncID = G_FuncID;
                    //patch4.5@yichang for USB VC end
		}
		else {                                //No Deferred Procedure Call in main lock loop
	            //vendor command to stop stream
	            if (G_pCtrlCommand->bRequest == 0x2f)
					G_btStopStream = 1;
		}
    }
    else
    {
        //register write
        //single write
        XBYTE[G_pCtrlCommand->wIndex] = (UINT8) (G_pCtrlCommand->wValue & 0x00ff);
    }
}

//-----------------------------------------------------------------------------
//VNDREQ_PreInDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void VNDREQ_PreInDataPhase(void)
 * @brief     pre-process setting of following in data phase

 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void VNDREQ_PreInDataPhase(void) USING_1
{
    UINT32 i,j;
    UINT8 bRequest;
    UINT16 wIndex, wValue;
	//UINT32 addr,size;

    G_pucCtrlDataPointer = G_ucCtrlData;
    bRequest = G_pCtrlCommand->bRequest;
    wIndex = G_pCtrlCommand->wIndex;
    wValue = G_pCtrlCommand->wValue;
	//printf("vnd i %bx wIndex=%x %x\n",G_pCtrlCommand->bRequest,wIndex,wValue);
    switch (bRequest)
	{
	    case 0x00:      //register read
	        //check address of burst read
	        if (wIndex == 0x21ff)
				wIndex = 0x1200;

	        //fill data to buffer
	        for (i = 0; i < G_usCtrlDataLength; i++)
	        {
				G_pucCtrlDataPointer[i] =
				(UINT8) XBYTE[(UINT32) wIndex + i];
	        }
	        break;

	    case 0x20:      //get version
            //fill data to buffer
            G_pucCtrlDataPointer[0] = K_FIRMWARE_VERSION;
            G_pucCtrlDataPointer[1] = K_FIRMWARE_RELEASE;
            G_pucCtrlDataPointer[2] = K_FIRMWARE_SEQUENCE;
            G_pucCtrlDataPointer[3] = XBYTE[0x20ff];
            G_pucCtrlDataPointer[4] = K_FIRMWARE_CHIPNUMBER;
            break;

	    case 0x21:      //get state
            //fill data to buffer
            switch (wIndex)
			{
			    case 0x00:      //Get ucState
		            G_pucCtrlDataPointer[0] = G_ucState;
		            break;

			    case 0x01:      //Get ucState2
		            G_pucCtrlDataPointer[0] = G_ucState2;
		            break;

			    case 0x02:      //Get ucState3
		            G_pucCtrlDataPointer[0] = G_ucState3;
		            break;

			    case 0x03:      //Get FW Status
		            G_pucCtrlDataPointer[0] = G_bRequestLog;
		            G_pucCtrlDataPointer[1] = M_LoByteOfWord(wIndex);
		            G_pucCtrlDataPointer[2] = M_HiByteOfWord(wIndex);
		            G_pucCtrlDataPointer[3] = G_ucState;
		            G_pucCtrlDataPointer[4] = G_ucState2;
		            G_pucCtrlDataPointer[5] = G_ucState3;
		            G_pucCtrlDataPointer[6] = bRequest;
		            break;

			    default:
					break;
			}
		//	printf("r 0x21 %x %bx %bx\n",wIndex,G_pucCtrlDataPointer[0],G_ucState);
	            break;

	    case 0x22:      //get error code
            G_pucCtrlDataPointer[0] = M_LoByteOfDword(G_ulErrorCode);
            G_pucCtrlDataPointer[1] = M_MidLoByteOfDword(G_ulErrorCode);
            G_pucCtrlDataPointer[2] = M_MidHiByteOfDword(G_ulErrorCode);
            G_pucCtrlDataPointer[3] = M_HiByteOfDword(G_ulErrorCode);
            break;

	    case 0x23:      //get information
            INITIO_GetInformation(wIndex);          //cch
            break;

	    case 0x24:      //get color information
            // SENSOR_GetColorInformation(wIndex);
            break;

	    case 0x25:      //get camera size index
            INITIO_GetCameraSizeIndex(wIndex);      //cch
            break;

	    case 0x26:      //get quantization table index
            G_pucCtrlDataPointer[0] = G_Image_QTableIndex;  //cch@10/15
            break;

	    case 0x27:      //get image type
            G_pucCtrlDataPointer[0] = G_Image_Type;         //cch@10/26
            break;

	    case 0x28:      //get image type
			//richie@vc0422
			if (wIndex == K_CARD_EXIST_CHECK_INDEX)
			{
				G_pucCtrlDataPointer[0] = 0;
				#if (CFC_OPTION == 1)
					if (G_Card_Type == K_MEDIA_CFC)
						G_pucCtrlDataPointer[0] = 1;
				#endif
				#if (SDF_OPTION == 1)
					if (G_Card_Type == K_MEDIA_SDF)
						G_pucCtrlDataPointer[0] = 1;
				#endif
				#if (EMMC_OPTION == 1)
					if (G_Card_Type == K_MEDIA_EMMC)
						G_pucCtrlDataPointer[0] = 1;
				#endif
				#if (SMC_OPTION == 1)
					if (G_Card_Type == K_MEDIA_SMC)
						G_pucCtrlDataPointer[0] = 1;
				#endif
				//patch4.2@richie@ds0511 begin
				#if (DOSRAM_OPTION == 1)
					if (G_Card_Type == K_MEDIA_DOSRAM)
						G_pucCtrlDataPointer[0] = 1;
				#endif
				//patch4.2@richie@ds0511 end
			}
			else if (wIndex == K_NAND_FLASH_EXIST_CHECK_INDEX)
			{
				G_pucCtrlDataPointer[0] = 0;
			#if (NANDF_OPTION == 1)
				if (G_Card_Type == K_MEDIA_NANDF)
					G_pucCtrlDataPointer[0] = 1;
			#endif
			}
			break;

		#ifdef DPF_SCANNER
	    case 0x51:      //Get Object Count
		{
			UINT32 addr = snapJpegVlcAddr;
			UINT32 size = snapHdrSize;
			addr = addr - ( snapHdrSize >> 1 );
            switch (wIndex)
            {
				case 0xfffd:      //Get video previes size
					//printf("r 0x51 2701=%bx %ld %ld\n",XBYTE[REG_Dram_VlcRdy],snapMainVlcSize,snapHdrSize);
					if(( XBYTE[REG_Dram_VlcRdy] & 0x03 ) == 0){   //vlc a/b is null
						size = 0;
						addr = 0;
						G_DPF_View_Frame = 0;
						
					}
					else{
						if(( XBYTE[REG_Dram_VlcRdy] & 0x03 ) == 0x03 ){
							if( G_DPF_View_Frame == 0x02){
								WRITE8(addr, 3,XBYTE[REG_Dram_VlcAAddr_B0]);
								WRITE8(addr, 2,XBYTE[REG_Dram_VlcAAddr_B1]);
								WRITE8(addr, 1,XBYTE[REG_Dram_VlcAAddr_B2]);
								WRITE8(addr, 0,XBYTE[REG_Dram_VlcAAddr_B3]);
								WRITE8(size,3,XBYTE[REG_Dram_VlcASize_B0]);
								WRITE8(size,2,XBYTE[REG_Dram_VlcASize_B1]);
								WRITE8(size,1,XBYTE[REG_Dram_VlcASize_B2]);
								WRITE8(size,0,0);
								G_DPF_View_Frame = 0x01;
							}	
							else{
								WRITE8(addr, 3,XBYTE[REG_Dram_VlcBAddr_B0]);
								WRITE8(addr, 2,XBYTE[REG_Dram_VlcBAddr_B1]);
								WRITE8(addr, 1,XBYTE[REG_Dram_VlcBAddr_B2]);
								WRITE8(addr, 0,XBYTE[REG_Dram_VlcBAddr_B3]);
								WRITE8(size,3,XBYTE[REG_Dram_VlcBSize_B0]);
								WRITE8(size,2,XBYTE[REG_Dram_VlcBSize_B1]);
								WRITE8(size,1,XBYTE[REG_Dram_VlcBSize_B2]);
								WRITE8(size,0,0);
								G_DPF_View_Frame = 0x02;
							}
						}
						else if(( XBYTE[REG_Dram_VlcRdy] & 0x01 ) == 0x01 ){
							//A ready
							WRITE8(addr, 3,XBYTE[REG_Dram_VlcAAddr_B0]);
							WRITE8(addr, 2,XBYTE[REG_Dram_VlcAAddr_B1]);
							WRITE8(addr, 1,XBYTE[REG_Dram_VlcAAddr_B2]);
							WRITE8(addr, 0,XBYTE[REG_Dram_VlcAAddr_B3]);
							WRITE8(size,3,XBYTE[REG_Dram_VlcASize_B0]);
							WRITE8(size,2,XBYTE[REG_Dram_VlcASize_B1]);
							WRITE8(size,1,XBYTE[REG_Dram_VlcASize_B2]);
							WRITE8(size,0,0);
							G_DPF_View_Frame = 0x01;
						}
						else if(( XBYTE[REG_Dram_VlcRdy] & 0x02 ) == 0x02 ){
							//B ready
							WRITE8(addr, 3,XBYTE[REG_Dram_VlcBAddr_B0]);
							WRITE8(addr, 2,XBYTE[REG_Dram_VlcBAddr_B1]);
							WRITE8(addr, 1,XBYTE[REG_Dram_VlcBAddr_B2]);
							WRITE8(addr, 0,XBYTE[REG_Dram_VlcBAddr_B3]);
							WRITE8(size,3,XBYTE[REG_Dram_VlcBSize_B0]);
							WRITE8(size,2,XBYTE[REG_Dram_VlcBSize_B1]);
							WRITE8(size,1,XBYTE[REG_Dram_VlcBSize_B2]);
							WRITE8(size,0,0);
							G_DPF_View_Frame = 0x02;
						}	
					}
					G_pucCtrlDataPointer[0] = READ8(size, 3);
					G_pucCtrlDataPointer[1] = READ8(size, 2);
					G_pucCtrlDataPointer[2] = READ8(size, 1);
					G_pucCtrlDataPointer[3] = READ8(size, 0);
					G_pucCtrlDataPointer[4] = READ8(addr, 3);
					G_pucCtrlDataPointer[5] = READ8(addr, 2);
					G_pucCtrlDataPointer[6] = READ8(addr, 1);
					G_pucCtrlDataPointer[7] = READ8(addr, 0);
					//printf("r 2701=%bx %ld %ld\n",XBYTE[REG_Dram_VlcRdy],size,addr);
					break;
				case 0xfffe:      //Get capture addr,size
					//	printf("r 0x51 %ld %ld %ld\n",addr,snapMainVlcSize,snapHdrSize);
                    G_pucCtrlDataPointer[0] = READ8(size, 3);
                    G_pucCtrlDataPointer[1] = READ8(size, 2);
                    G_pucCtrlDataPointer[2] = READ8(size, 1);
                    G_pucCtrlDataPointer[3] = READ8(size, 0);
                    G_pucCtrlDataPointer[4] = READ8(snapMainVlcSize, 3);
                    G_pucCtrlDataPointer[5] = READ8(snapMainVlcSize, 2);
                    G_pucCtrlDataPointer[6] = READ8(snapMainVlcSize, 1);
                    G_pucCtrlDataPointer[7] = READ8(snapMainVlcSize, 0);
                    G_pucCtrlDataPointer[8] = READ8(addr, 3);
                    G_pucCtrlDataPointer[9] = READ8(addr, 2);
                    G_pucCtrlDataPointer[10] = READ8(addr, 1);
                    G_pucCtrlDataPointer[11] = READ8(addr, 0);
                    break;
				case 0xffff:      //Abort
					break;
				default:
					break;
			}
		}
			break;
		#endif
	    case 0x54:      //Get Object Count
			//richie@vc0422
			////printf("0x54 G_USR_FileCount=%d,G_usCtrlDataLength=%d\n",G_USR_FileCount,G_usCtrlDataLength);
			if (wValue == 0x00)
			{
				UINT16 fileCount = dcfFileCountGet();
			    G_pucCtrlDataPointer[0] = M_LoByteOfWord(fileCount);
			    G_pucCtrlDataPointer[1] = M_HiByteOfWord(fileCount);
			}
			break;

			//patch4.3@richie@md0603 begin
		#if (SINGLE_DIRECTORY == 0)
	    case 0x55:      //Get Dir
			if (wIndex == 0x00)
			{
				UINT16 totalDir = dcfTotalDirectoryGet();
			    G_pucCtrlDataPointer[0] = M_LoByteOfWord(totalDir);
			    G_pucCtrlDataPointer[1] = M_HiByteOfWord(totalDir);
			}
			else if (wIndex == 0x02)
			{
				UINT16 dir1Index = dcfDir1IndexGet();
			    //patch4.4@ada@DirectoryIndexOrder begin
			    //G_pucCtrlDataPointer[0] = M_LoByteOfWord(G_USR_Dir1CountIndex);
			    //G_pucCtrlDataPointer[1] = M_HiByteOfWord(G_USR_Dir1CountIndex);
			    G_pucCtrlDataPointer[0] = M_LoByteOfWord(dir1Index);
			    G_pucCtrlDataPointer[1] = M_HiByteOfWord(dir1Index);
			    //patch4.4@ada@DirectoryIndexOrder end
			}
			break;
		#endif
	//patch4.3@richie@md0603 end

		case 0x70:
			//printf("0x70 request\n");
			if (!(XBYTE[0x20b1] & 0x04)){/*NAND boot*/
				G_pucCtrlDataPointer[0] = 1;
			}
			else {
				G_pucCtrlDataPointer[0] = 2;
			}
			//if ( wIndex != 0x00 || wValue != 0){
			/* put to DRAM addr = 0x48000 * 2 byte */
			G_pucCtrlDataPointer[1] = (UINT8)(0x00);
			G_pucCtrlDataPointer[2] = (UINT8)(0x80);
			G_pucCtrlDataPointer[3] = (UINT8)(0x04);
			G_pucCtrlDataPointer[4] = (UINT8)(0);
			XBYTE[REG_Front_Finten_low] = 0x00;
			XBYTE[REG_Cdsp_Cdspinten_lo] = 0x00;
			XBYTE[0x2277] = 0x07;
			//}
			//printf("0x70 request %bx %bx %bx %bx\n",G_pucCtrlDataPointer[0],G_pucCtrlDataPointer[1],G_pucCtrlDataPointer[2],G_pucCtrlDataPointer[3]);
			break;

		//#ifdef ICAT_OPTION//xian ++
		#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
		
			case ICAT_REQUEST:
				//printf("i_cat preIn %bx %bx %bx %bx\n",G_pucCtrlDataPointer[0],G_pucCtrlDataPointer[1],G_pucCtrlDataPointer[2],G_pucCtrlDataPointer[3]);
				icatVndCmdStub_Intr( wIndex, wValue, G_usCtrlDataLength, G_pucCtrlDataPointer );
			break;
		#endif
		case 0x80:	
		   switch (wIndex)
			{
			case 0x02: //cx++ for  AF WINDOW
				for(i=0;i<5;i++)
					{
						 //printf("get af window\n");
						 //printf("AFWINDOW[%bu]=%d,%d,%d,%d\n",(UINT8)i,AFWINDOW[i].xpos,AFWINDOW[i].ypos,AFWINDOW[i].width,AFWINDOW[i].heigth);
						 j=8*i;
						G_pucCtrlDataPointer[j]    =M_LoByteOfWord(AFWINDOW[i].xpos);
						G_pucCtrlDataPointer[j+1]=M_HiByteOfWord(AFWINDOW[i].xpos);
						G_pucCtrlDataPointer[j+2]=M_LoByteOfWord(AFWINDOW[i].ypos);
						G_pucCtrlDataPointer[j+3]=M_HiByteOfWord(AFWINDOW[i].ypos);
						G_pucCtrlDataPointer[j+4]=M_LoByteOfWord(AFWINDOW[i].width);
						G_pucCtrlDataPointer[j+5]=M_HiByteOfWord(AFWINDOW[i].width);
						G_pucCtrlDataPointer[j+6]=M_LoByteOfWord(AFWINDOW[i].heigth);
						G_pucCtrlDataPointer[j+7]=M_HiByteOfWord(AFWINDOW[i].heigth);
					}
				break;
		
				case 0x03: //cx++ for  AF  value
				for(i=0;i<5;i++)
					{
					  //printf("get af value\n");
					  //printf("afBuf[%bu]=%lu\n",(UINT8)i,afBuf[i]);
						 j=4*i;
						G_pucCtrlDataPointer[j] 	= READ8(afBuf[i], 3);
						G_pucCtrlDataPointer[j+1] = READ8(afBuf[i], 2);
						G_pucCtrlDataPointer[j+2] = READ8(afBuf[i], 1);
						G_pucCtrlDataPointer[j+3] = READ8(afBuf[i], 0);
					}
				break;
			}
		break;
#if 1 //CX ++ for UI TOOL
		case UITOOL_REQUEST:
		{	
			UINT32 addr = 0;
			UINT32 size = 0;
			UINT8 cmd = READ8(wIndex,0);
			UINT8 id = (UINT8)wIndex;
			UINT8 ret = sp1kUIToolGet(id,&addr,&size);
			//if(!ret)break;
			//printf("%x(%bx,%bx)(%lx,%lx)\n",wIndex,cmd,id,addr,size);
			if(cmd==0x01) //download  ,need addr and size
			{ 
				//printf("upload:%lx,%lx,%x\n",addr,size,wIndex);
				G_pucCtrlDataPointer[0] = READ8(addr, 3);
				G_pucCtrlDataPointer[1] = READ8(addr, 2);
				G_pucCtrlDataPointer[2] = READ8(addr, 1);
				G_pucCtrlDataPointer[3] = READ8(addr, 0);
				//G_pucCtrlDataPointer[4] = READ8(size, 3);
				//G_pucCtrlDataPointer[5] = READ8(size, 2);
				//G_pucCtrlDataPointer[6] = READ8(size, 1);
				//G_pucCtrlDataPointer[7] = READ8(size, 0);
				
			}
			else if (cmd==0x02) // upload ,get addr
			{
			  	//printf("download:%lx,%lx,%x\n",addr,size,wIndex);
				G_pucCtrlDataPointer[0] = READ8(addr, 3);
				G_pucCtrlDataPointer[1] = READ8(addr, 2);
				G_pucCtrlDataPointer[2] = READ8(addr, 1);
				G_pucCtrlDataPointer[3] = READ8(addr, 0);	
				G_pucCtrlDataPointer[4] = READ8(size, 3);
				G_pucCtrlDataPointer[5] = READ8(size, 2);
				G_pucCtrlDataPointer[6] = READ8(size, 1);
				G_pucCtrlDataPointer[7] = READ8(size, 0);
			}	
			else// check data
			{
				G_pucCtrlDataPointer[0] = 1;
				G_pucCtrlDataPointer[1] = 2;
				G_pucCtrlDataPointer[2] = 3;
				G_pucCtrlDataPointer[3] = 4;	
			}
		}
		break;
#endif				
/*#if(DDX_on)		
		case 0x80:  //cx++ get FD information  
			//  printf("0x80 s=%bu,x=%bu,y=%bu\n",fdKeyRtS,fdKeyRtX,fdKeyRtY);
       		if((G_PCCAME_FD_ON==2 )||(G_PCCAME_FD_ON==1))
        	        {
                        G_pucCtrlDataPointer[0]=fdFaceNum;
                        for(i=0;i<80;i++)
                        {
                            G_pucCtrlDataPointer[i+1]=fdRtSize[i];
                        }
              	        //memcpy( G_pucCtrlDataPointer+1, fdRtSize,80);
				//    G_pucCtrlDataPointer[0] = fdKeyRtS;
				//    G_pucCtrlDataPointer[1] = fdKeyRtX;
				//   G_pucCtrlDataPointer[2] = fdKeyRtY;
				//  printf("0x80 x=%bu\n",G_pucCtrlDataPointer[1]);
		 	}else if((G_PCCAME_FD_ON==0)||(G_PCCAME_FD_ON==3))
                    {
                        G_pucCtrlDataPointer[0]=0x00;
                    }

            break;
 #endif
   */        
	    default:
			break;
	}
}

//-----------------------------------------------------------------------------
//VNDREQ_PreOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void VNDREQ_PreOutDataPhase(void)
 * @brief     pre-process setting of following out data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void VNDREQ_PreOutDataPhase(void) USING_1
{
	G_pucCtrlDataPointer = G_ucCtrlData;
}

//-----------------------------------------------------------------------------
//VNDREQ_PostInDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void VNDREQ_PostInDataPhase(void)
 * @brief     post-process data of complete in data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void VNDREQ_PostInDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//VNDREQ_PostOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn        void VNDREQ_PostOutDataPhase(void)
 * @brief     post-process data of complete out data phase
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void VNDREQ_PostOutDataPhase(void) USING_1
{
    UINT32 i;
    UINT16 wIndex;
    UINT8 temp;
    UINT8 ArgIndex;

	#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
		UINT16 wValue;
		UINT8 *pt;
	#endif

	//	#ifdef ICAT_OPTION//xian ++
	#if defined(ICAT_OPTION) || defined(AMCAP_OPTION)
    if (G_pCtrlCommand->bRequest == ICAT_REQUEST) //Deferred Procedure Call in main lock loop
	{
		wIndex = G_pCtrlCommand->wIndex;
		wValue = G_pCtrlCommand->wValue;
		//printf("%x , %x, %bx\n", wIndex, G_pCtrlCommand->wIndex, G_bRequestLock);
		if(icatIsDPCCmd( wIndex )){
			//lock fields of request
			G_bRequestLock =
			G_bRequestLog =
			G_pCtrlCommand->bRequest;
			G_wValueLock = G_pCtrlCommand->wValue;
			G_wIndexLock = G_pCtrlCommand->wIndex;
			G_wLengthLock = G_pCtrlCommand->wLength;
			//memcpy(G_ArgBuf, G_ucCtrlData, 64);
			pt = (UINT8*)G_ArgBuf;
			for(i = 0; i < 64; i++)
			*pt ++ = G_ucCtrlData[i];

			//			printf(" ... %x ,%x ,%x\n", G_bRequestLock, G_wIndexLock, G_wLengthLock);
			//			printf(" ... %lx, %lx, %lx, %lx\n", G_ArgBuf[0], G_ArgBuf[1], G_ArgBuf[2], G_ArgBuf[3]);
		}
		else {
			icatVndCmdStub_Intr( wIndex, wValue, G_usCtrlDataLength, G_pucCtrlDataPointer );
		}
		return; //xian ++ 20080717
	}
	//xian --
	#endif


	wIndex = G_pCtrlCommand->wIndex;

	//printf("vnd o %bx wIndex=%x %x\n",G_pCtrlCommand->bRequest,wIndex,G_pCtrlCommand->wValue);
    switch (G_pCtrlCommand->bRequest)
	{
	    case 0x00:      //register write
			// check address of burst write
			if (wIndex == 0x217c)
			    wIndex = 0x2117;

			//set data from buffer
			for (i = 0; i < G_usCtrlDataIndex; i++)
			    XBYTE[(UINT32) wIndex + i] = G_pucCtrlDataPointer[i];
			break;

	    case 0x21:      //set state
			//printf("w 0x21 %x %x\n",wIndex,G_pucCtrlDataPointer[0]);
			switch (wIndex)
			{
			    case 0x00:      //get ucState
		            G_ucState |= (G_pucCtrlDataPointer[0] & 0x0f);
		            break;

			    case 0x01:      //get ucState2
		            G_ucState2 = G_pucCtrlDataPointer[0];
		            break;

			    case 0x02:      //get ucState3
		            G_ucState3 = G_pucCtrlDataPointer[0];
		            break;

			    default:
					break;
			}
			break;


	    case 0x22:      //set error code
            G_ulErrorCode = (UINT32) G_pucCtrlDataPointer[0];
            G_ulErrorCode |= ((UINT32) G_pucCtrlDataPointer[1]) << 8;
            G_ulErrorCode |= ((UINT32) G_pucCtrlDataPointer[2]) << 16;
            G_ulErrorCode |= ((UINT32) G_pucCtrlDataPointer[3]) << 24;
            break;

	    case 0x24:      //set color information
           // SENSOR_SetColorInformation(wIndex);
            break;

	    case 0x25:      //set camera size index
            INITIO_SetCameraSizeIndex(wIndex); //cch
            break;

	    case 0x26:      //set quantization table index
            G_Image_QTableIndex = G_pucCtrlDataPointer[0];
            break;
		//patch5.0@richie@sdram mapping begin
		//patch4.5@richie@image type begin
	    case 0x27:      //Set Image Type
			G_Image_Type = G_pucCtrlDataPointer[0];
			temp = G_Image_Type & 0x07;

			if (temp == 0)		//RawData 10 bit
				XBYTE[REG_Dram_ImgType] = 0;
			else if (temp == 1)	//RawData 8 bit
				XBYTE[REG_Dram_ImgType] = 0;
			else if (temp == 2)	//YUV422 Non-compress
				XBYTE[REG_Dram_ImgType] = 0;
			else if (temp == 3)	//YUV422 compress
				XBYTE[REG_Dram_ImgType] = 2;
			else if (temp == 4)	//YUV420 Non-compress
				XBYTE[REG_Dram_ImgType] = 0;
			else if (temp == 5)	//YUV420 Non-compress-UV
				XBYTE[REG_Dram_ImgType] = 0;
			else if (temp == 6)	//YUV420 Compress
				XBYTE[REG_Dram_ImgType] = 2;
			else if (temp == 7)	//YUV420 Compress-UV
				XBYTE[REG_Dram_ImgType] = 0;

			break;

	    case 0x28:      //Enable/Disable DPF scanner solution.
			G_DPF_Enable = G_pucCtrlDataPointer[0];
			break;
		//patch4.5@richie@partial file upload begin
		case 0x60:
			//lock fields of request
			G_bRequestLock =
			    G_bRequestLog =
					G_pCtrlCommand->bRequest;
			G_wValueLock = G_pCtrlCommand->wValue;
			G_wIndexLock = G_pCtrlCommand->wIndex;

			//printf("Partial File\n");
			for (ArgIndex=0; ArgIndex<16; ArgIndex++) // Collect HAL Function parameter from USB buffer
			    G_ArgBuf[ArgIndex] = M_UCHARToULONG(G_ucCtrlData[ArgIndex*4+3],G_ucCtrlData[ArgIndex*4+2],G_ucCtrlData[ArgIndex*4+1],G_ucCtrlData[ArgIndex*4]);

			//2Note:If using "printf" in this function, there'll be a minor warnning in compiling!

			//printf("arg1 = %lx,arg2 = %lx,arg3 = %lx\n",G_ArgBuf[0],G_ArgBuf[1],G_ArgBuf[2]);
			break;
		//patch4.5@richie@partial file upload end
#if(DDX_on)
		case 0x79:   //for ae on/off for ddx tool
               switch (wIndex)
                {
                case 0x01:
                    G_ae_ON=G_pucCtrlDataPointer[0];                       
                  //  printf("0x79  %bu\n",G_pucCtrlDataPointer[0]);
                    break;
                }
			break;
/*
         case 0x80:  //cx++ for  FD  on /off  
               switch (wIndex)
                {
                case 0x01:
                    G_PCCAME_FD_ON=G_pucCtrlDataPointer[0];  
                    break;
                }
            break;*/
#endif
#if  0
            case 0x80:  //cx++ for  FD  on /off  
               switch (wIndex)
                {
                case 0x01:
                    G_PCCAME_FD_ON=G_pucCtrlDataPointer[0];  //fd on
                 //   printf("0x80 fd %bu\n",G_pucCtrlDataPointer[0]);
                     if(G_PCCAME_FD_ON)
                   {
                     //   sp1kFdReset();
                        sp1kFdEnable();//cx++ for  fd open
                      //  printf("@pc cam fd on\n");
                     }else
                        { sp1kFdDisable();}
                    break;
                
                }

            break;
            #endif

        #if 0    
	    case 0xC0:	/* Brightness */ 
			//printf("d=%bx\n",G_pucCtrlDataPointer[0]);
            //XBYTE[REG_Cdsp_Yb] = (SINT8)(G_pucCtrlDataPointer[0] - 128);
            HAL_CdspYBrightOffsetSet((SINT8)(G_pucCtrlDataPointer[0] - 128));
			//printf("b=%bx\n",(SINT8)(G_pucCtrlDataPointer[0] - 128));
            break;
	    case 0xC1:	/* Contrast */ 
			//printf("d=%bx\n",G_pucCtrlDataPointer[0]);
            //XBYTE[REG_Cdsp_Yc] = G_pucCtrlDataPointer[0]* 0x1F;
            HAL_CdspYContrastOffsetSet(G_pucCtrlDataPointer[0]* 0x1F);
            break;
	    case 0xC2:	/* Hue */ 
			//printf("d=%bx\n",G_pucCtrlDataPointer[0]);
            XBYTE[REG_Cdsp_huesin1data] = G_pucCtrlDataPointer[0];
            break;
	    case 0xC3:	/* Saturation */ 
			//printf("d=%bx\n",G_pucCtrlDataPointer[0]);
            XBYTE[REG_Cdsp_UVsat] = G_pucCtrlDataPointer[0]* 0x1F;
            break;
	    case 0xC4:	/* Sharpness */ 
			//printf("d=%bx\n",G_pucCtrlDataPointer[0]);
            XBYTE[REG_Cdsp_ygain1] = G_pucCtrlDataPointer[0];
            break;
            #endif
            
	    default:
	        G_bRequestLock =
				G_bRequestLog =
					G_pCtrlCommand->bRequest;
			//printf("cmd=%bx\n",G_bRequestLock);
	        G_wValueLock = G_pCtrlCommand->wValue;
	        G_wIndexLock = G_pCtrlCommand->wIndex;
	        G_HAL_ModuleID = (UINT8)(G_wIndexLock>>8);
	        G_L1_ModuleID = G_HAL_ModuleID;
	        G_FuncID = (UINT8)(G_wIndexLock&0x00ff);
	        G_HAL_FuncID = G_FuncID;
			//printf("G_L1_ModuleID = %bx G_FuncID = %bx\n",G_L1_ModuleID,G_FuncID);
	        for (ArgIndex=0; ArgIndex<16; ArgIndex++) // Collect HAL Function parameter from USB buffer
				G_ArgBuf[ArgIndex] = M_UCHARToULONG(G_ucCtrlData[ArgIndex*4+3],G_ucCtrlData[ArgIndex*4+2],G_ucCtrlData[ArgIndex*4+1],G_ucCtrlData[ArgIndex*4]);
			//printf("Arg = %lx %lx %lx %lx\n",G_ArgBuf[0],G_ArgBuf[1],G_ArgBuf[2],G_ArgBuf[3]);

	        break;
	}
}

