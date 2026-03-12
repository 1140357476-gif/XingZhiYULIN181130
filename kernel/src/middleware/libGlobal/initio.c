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
#define __FILE   __FILE_ID_INITIO__ 

#include "general.h"
#include "initio.h"

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

//-----------------------------------------------------------------------------
//INITIO_SoftwareReset
//-----------------------------------------------------------------------------
void INITIO_SoftwareReset(void) USING_0
/*++

Routine Description:

        reset system by code

Arguments:

        none

Return Value:

        none

--*/
{
}

#if TAG_TODO
//-----------------------------------------------------------------------------
//INITIO_SetCameraSize
//-----------------------------------------------------------------------------
void INITIO_SetCameraSize(void) USING_0    //cch
/*++

Routine Description:

        set sensor width ,height ,Hsubsample ,Vsubsample

Arguments:

        cameraMode : 2 :dsc , 3:vido clip mode : 4: Pc camera

Return Value:

        none

--*/
{
        UINT16	k;
		UINT8*	ProductDescriptor;
		UINT8*	PcCameraSizeMapping;


		ProductDescriptor = ProductDescriptor1;
		PcCameraSizeMapping = PcCameraSizeMapping1;


        k = 4 + ( ProductDescriptor[3 + 8 * 2 + G_ucPccameraIndex] - 1 ) * 4;

		G_usPostJpegWidth = ((UINT16)PcCameraSizeMapping[k] << 8) | ((UINT16)PcCameraSizeMapping[k + 1]);
		G_usPostJpegHeight = ((UINT16)PcCameraSizeMapping[k + 2] << 8) | ((UINT16)PcCameraSizeMapping[k + 3]);

		printf("w,h=%d,%d\n",G_usPostJpegWidth,G_usPostJpegHeight);

}

//-----------------------------------------------------------------------------
//INITIO_GetCameraSizeIndex      //cch
//-----------------------------------------------------------------------------
void INITIO_GetCameraSizeIndex(UINT16 index) USING_1
/*++

Routine Description:


Arguments:

        none

Return Value:

        none

--*/
{
    switch (index)
	{
	    case 0x0001:      //Get Preview Size Index
            G_pucCtrlDataPointer[0] = G_ucDscIndex;
            break;

	    case 0x0002:      //Get DSC Size Index
            G_pucCtrlDataPointer[0] = G_ucDscIndex;
            break;

	    case 0x0003:      //Get Video Clip Size Index
            G_pucCtrlDataPointer[0] = G_ucClipIndex;
            break;

	    case 0x0004:      //Get PC camera Index
            G_pucCtrlDataPointer[0] = G_ucPccameraIndex;
            break;

	    case 0x0005:      //Get Playback Index
            G_pucCtrlDataPointer[0] = G_ucDscIndex;
            break;

	    case 0x0011:      //Get Preview Size Index Max
            G_pucCtrlDataPointer[0] = G_ucDscIndexMax;
            break;

	    case 0x0012:      //Get DSC Size Index Max
            G_pucCtrlDataPointer[0] = G_ucDscIndexMax;
            break;

	    case 0x0013:      //Get Video Clip Size Index Max
            G_pucCtrlDataPointer[0] = G_ucClipIndexMax;
            break;

	    case 0x0014:      //Get PC camera Index Max
            G_pucCtrlDataPointer[0] = G_ucPccameraIndexMax;
            break;

	    case 0x0015:      //Get Playback Index
            G_pucCtrlDataPointer[0] = G_ucDscIndexMax;
            break;

	    default:
            break;
	}
}

//-----------------------------------------------------------------------------
//INITIO_SetCameraSizeIndex     //cch
//-----------------------------------------------------------------------------
void INITIO_SetCameraSizeIndex(UINT16 index) USING_1
/*++

Routine Description:


Arguments:

        none

Return Value:

        none

--*/
{
        switch (index)
        {
                case 0x0001:      //Set Preview Size Index
                        G_ucDscIndex = G_pucCtrlDataPointer[0];
                        break;

                case 0x0002:      //Set DSC Size Index
                        G_ucDscIndex = G_pucCtrlDataPointer[0];
                        break;

                case 0x0003:      //Set Video Clip Size Index
                        G_ucClipIndex = G_pucCtrlDataPointer[0];
                        break;

                case 0x0004:      //Set PC camera Index
                        G_ucPccameraIndex = G_pucCtrlDataPointer[0];
                        break;

                case 0x0005:      //St Playback Index
                        G_ucDscIndex = G_pucCtrlDataPointer[0];
                        break;

                case 0x0011:      //St Preview Size Index Max
                        G_ucDscIndexMax = G_pucCtrlDataPointer[0];
                        break;

                case 0x0012:      //Set DSC Size Index Max
                        G_ucDscIndexMax = G_pucCtrlDataPointer[0];
                        break;

                case 0x0013:      //Set Video Clip Size Index Max
                        G_ucClipIndexMax = G_pucCtrlDataPointer[0];
                        break;

                case 0x0014:      //Set PC camera Index Max
                        G_ucPccameraIndexMax = G_pucCtrlDataPointer[0];
                        break;

                case 0x0015:      //Set Playback Index Max
                        G_ucDscIndexMax = G_pucCtrlDataPointer[0];
                        break;

                default:
                        break;
        }
}

//-----------------------------------------------------------------------------
//INITIO_GetInformation
//-----------------------------------------------------------------------------
void INITIO_GetInformation(UINT16 infoType) USING_1
/*++

Routine Description:

        get system information

Arguments:

        none

Return Value:

        none

--*/
{
    UINT16 i = infoType;
	//version4.0 richie@ps0503
	UINT8*	ProductDescriptor;
	UINT8*	PcCameraSizeMapping;
	UINT8   PcCameraSizeMappingSize;

		ProductDescriptor = ProductDescriptor1;
		PcCameraSizeMapping = PcCameraSizeMapping1;
		PcCameraSizeMappingSize = K_PcCameraSizeMapping1;

        switch (infoType)
        {
                case 0x0000:      //Get ProductDescriptor information
                        for (i = 0; i < K_ProductDescriptorSize; i++)
                                G_pucCtrlDataPointer[i] = (UINT8) ProductDescriptor[i];
                        break;

                case 0x0001:      //Get SensorSizeInformation
			//version4.0 richie@ps0503
                        for (i = 0; i < PcCameraSizeMappingSize; i++)
                                G_pucCtrlDataPointer[i] = PcCameraSizeMapping[i];
                        break;
#if 0
                case 0x0002:      //Get DscSizeMapping Information
                        for (i = 2; i < K_DscSizeMapping; i++)
                                G_pucCtrlDataPointer[i] = DscSizeMapping[i];
                        break;

                case 0x0064:      //Get Storage information
                        i = DOS_GetClusterSize();
                        i /=  512;
                        G_pucCtrlDataPointer[0] = i;
                        G_pucCtrlDataPointer[1] = i >> 8;
                        for (i = 2; i < K_CtrlDataSize; i++)
                                G_pucCtrlDataPointer[i] = i;
                        //Card_GetInformation(G_pucCtrlDataPointer);   // @WYEO, 1008
                        break;

		//version4.0 richie@0514
                case 0x0065:      //Get Storage information   compiler error
                        G_pucCtrlDataPointer[48] = 0;         //cch@02/26
                        Card_GetInformation(&(G_pucCtrlDataPointer[0])); // @HISWYEO
                        break;
#endif
                default:
                        break;
        }
}
#endif //TAG_TODO


