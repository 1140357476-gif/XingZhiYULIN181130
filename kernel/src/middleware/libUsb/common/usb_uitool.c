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
#include "usb_uitool.h" 
#include "initio.h"
#include "hal_dram.h"
#include "sp1k_rsvblk_api.h"
#include "utility.h"
#include "os_msg.h"

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
typedef struct
{
	UINT32 addr,size;		
	UINT16 resid;
	UINT16 msg;
	UINT8 id;//0 is not invalid
}uiTool_t;
#define UITOOL_ID_COUNT 15
uiTool_t uiTool[UITOOL_ID_COUNT]; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

//Fill data to uiTool dataTable
UINT8
sp1kUIToolSet(
	UINT8 order,
	UINT8 id,
	UINT32 addr,
	UINT32 size,
	UINT16 resid,
	UINT16 msg
)
{
	uiTool_t* uit= uiTool+order;
	if(order==0xff){
		memset(uiTool,0,sizeof(uiTool));
		return FALSE;
	}
	if(order>=UITOOL_ID_COUNT){
		return FALSE;
	}
	
	uit->addr = addr;
	uit->size = size;	
	uit->resid= resid;	
	uit->id = id;	
	uit->msg = msg;
	return TRUE;
}
//get data from uiTool dataTable
UINT8
sp1kUIToolGet(
	UINT8 id,
	UINT32 xdata* addr,
	UINT32 xdata* size
)
{
	uiTool_t* uit = uiTool;
	UINT8 i;
	//if(id>=UITOOL_ID_COUNT)return FALSE;
	//printf("0x%bx\n",id);
	for(i=0;i<UITOOL_ID_COUNT;i++,uit++){
		if(uit->id == id){break;}
	}
	if(i>=UITOOL_ID_COUNT)return FALSE;
	if(!uit->id)return FALSE;
	*addr = uit->addr>>1;
	*size = uit->size;
	return TRUE;
}
//prepare data to usb,include assign addr and read resource file to dram
void uiToolPrepareData()
{
	UINT8 i;
	UINT16 ResId;
	UINT32 addr = (K_SDRAM_CommonEndAddr<<1);//+128*1024;
	for(i=0;i<UITOOL_ID_COUNT;i++)
	{
		uiTool_t* uit = uiTool+i;
		if(!uit->id)break;
		//id = uit->id;
		//if(i!=uit->id)break;
		if(uit->resid!=0xffff){//upload udf.bin replace adf.bin
			/*if(uit->resid == 0)
			{
				ResId=0x801;
			}
			else
			{
				ResId=uit->resid;
			}*/
			ResId=uit->resid;
		
			if(!uit->size){
				sp1kNandRsvSizeGet(ResId,NULL,&uit->size);
			}
			//if((!uit->addr) ||(((UINT8)uit->addr) &0x01)){
			//	uit->addr = addr;
			//	addr+=(uit->size+1)&~1;
			//}			
			sp1kNandRsvRead((UINT32)ResId, addr);
			
			//printf("+\n");
		}
		else{// if(((UINT8)uit->addr) &0x01){
			HAL_GprmDramDmaAdv(uit->addr, addr, uit->size, 1);
			//uit->addr = addr;
			//addr+=(uit->size+1)&~0x01;
		}	
		uit->addr = addr;
		addr+=((uit->size+1)&~0x01)+1024;
		//{
			//UINT8 name[12];
			//sprintf(name, "%BX_%X.BIN",uit->id,uit->resid);
			//dramFileSave(name, uit->addr>>1 , uit->size);
			//printf("%bx(%lx,%lx)%x,%s\n",uit->id,uit->addr,uit->size,uit->resid,name);
			uiToolPrintf("%bx(%lx,%lx)%x\n",uit->id,uit->addr,uit->size,uit->resid);
		//}
		
	}
}
//process data from usb,include send msg and save data to resource file 
UINT8 uiToolProcessData(UINT8 id)
{
	uiTool_t* uit = uiTool;
	UINT8 i;
	//if(id>=UITOOL_ID_COUNT)return FALSE;
	uiToolPrintf("0x%bx",id);
	for(i=0;i<UITOOL_ID_COUNT;i++,uit++){
		if(uit->id == id){
			break;
		}
	}
	uiToolPrintf("-%x(%lx,%lx)%x\n",uit->resid,uit->addr,uit->size,uit->msg);
	if(i>=UITOOL_ID_COUNT)return FALSE;
	//if(id!=uiTool[id].id)return FALSE;
	if(uit->msg)osMsgPost(uit->msg);
	if(uit->resid!=0xffff)sp1kDiskRsvWrite(uit->resid,uit->addr);
	
	/*if(uit->resid == 0)	{
		osMsgPost(SP1K_MSG_DO_RESET);
	}*/
	
	
	return TRUE;
}
