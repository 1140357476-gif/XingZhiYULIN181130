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
 * @author    wangjg
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "awb_func.h" 
#include "awb_ver.h"
//#include "sp1k_fd_api.h"
//#include "sp1k_ae_api.h"

#include "hal_gprm.h"
#include "reg_def.h"

#include "initio.h"
#include "intr.h"

#include "cardlink.h"
#include "Sp1k_vfs_api.h"
#include "os_api.h"
#include "pv_task.h"
#include "cmd.h"
#include "dbg_wdt.h"
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
UINT8 awbDebugCnt=0;

#if AWB_TIME
awb_timer_t awbTimer;
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


#define awbTestSimulateEn 0
#if awbTestSimulateEn==2
void awbTestSimulate(UINT16 idx,UINT8 scene)
{
	UINT32 size;
	UINT8 preName[] = "SUNP";

	UINT8 name[13];// = "SUNP0000.MOI";
	int i;
	UINT32 addr;
	awb_filter_sum_t rst;	

	sprintf(name,"%s%04u.MOI",preName,idx);
	
	if(!dramFileLoad(name, AWB_ADDR_Samp, &size))return;
	printf("%s,size:%lu--%bx\n",name,size,scene);
	//awbSceneUpdate(&rst);
	//HAL_MoeRunQueue(pawbaddr->queue[awb.scene]);
	//awbFilterResult(&rst);	
	awbEnable(0);
	awbDebugCnt = 0xff;
	awb.sceneIdx = 0;
	awb.scene = scene;
	addr = pawbaddr->queue[awb.scene]+(5*sizeof(moe_queue_t)+sizeof(awb_moe_offset_t))/2;
	HAL_MoeRunQueue(addr,MOE_Trigger_Awb);
	//awbFilterResult(&rst);
	//awbSceneUpdate(&rst);
	//if(!dramFileLoad(name, AWB_ADDR_Wnd1, &size))return;
	//HAL_MoeRunQueue(pawbaddr->queue[awb.scene],MOE_Trigger_Awb);
	awbFilterResult(&rst);
	//printf("scene:%bx,(%bx,%bx,%bx)\n",awb.scene,awb.gid,awb.luma,awb.contrast);
	for(i=0;i<10;i++)
	{		
		awb.flowFlag = 0;
		//awbFilterResult(&rst);
		awbSpotsDecider(&rst);		
	}
	awbDebugCnt = 0;
	HAL_DramWriteBytesAddr(AWB_ADDR_Param, &awb, sizeof(awb), 0);
	//sprintf(name,"%s%04u.TMP",preName,idx);
	//printf("-%s\n",name);
	//dramFileSave(name, AWB_ADDR_Tmp0, 1024*16);
	
	sprintf(name,"%s%04u.JPG",preName,idx);
	printf("-%s\n",name);
	dramFileSave(name, AWB_ADDR_Queue, 1024ul*16);
}

#elif awbTestSimulateEn==1
void awbTestSimulate(UINT16 idx)
{
	UINT32 size;
	UINT8 preName[] = "SUNP";

	UINT8 name[13];// = "SUNP0000.MOI";
	int i;
	awb_filter_sum_t rst;
	

	sprintf(name,"%s%04u.MOI",preName,idx);
	if(!dramFileLoad(name, AWB_ADDR_Wnd1, &size))return;
	printf("%s,size:%lu\n",name,size);
	//awbSceneUpdate(&rst);
	//HAL_MoeRunQueue(pawbaddr->queue[awb.scene]);
	//awbFilterResult(&rst);	

	awb.sceneIdx = 0;
	awb.scene = 0;
	HAL_MoeRunQueue(pawbaddr->queue[awb.scene],MOE_Trigger_Awb);
	awbFilterResult(&rst);
	awbSceneUpdate(&rst);
	//if(!dramFileLoad(name, AWB_ADDR_Wnd1, &size))return;
	//HAL_MoeRunQueue(pawbaddr->queue[awb.scene],MOE_Trigger_Awb);
	
	//printf("scene:%bx,(%bx,%bx,%bx)\n",awb.scene,awb.gid,awb.luma,awb.contrast);
	for(i=0;i<5;i++)
	{		
		awb.flowFlag = 0;
		awbFilterResult(&rst);
		awbSpotsDecider(&rst);		
	}
	HAL_DramWriteBytesAddr(AWB_ADDR_Param, &awb, sizeof(awb), 0);
	sprintf(name,"%s%04u.TMP",preName,idx);
	printf("-%s\n",name);
	dramFileSave(name, AWB_ADDR_Tmp0, 1024*16);
	
	sprintf(name,"%s%04u.INF",preName,idx);
	printf("-%s\n",name);
	dramFileSave(name, AWB_ADDR_Queue, 1024*16);
}
#endif

#if AWB_CALIBRATION
#define AWB_CALIBRATION_Test 2
#endif
#if AWB_CALIBRATION_Test 
void awbTestCalibration(UINT8 calib)
{
	UINT16 cnt=0xffff;
	//dbgWdtSet(0);
	awbCalibStart();
	
	//create awb ctt or calib 3 Light
	if(calib ==0 || calib == 1){
		//please switch A light
		while(cnt--);
		awbCalibDo(0);
		//please switch CW light
		while(cnt--);
		awbCalibDo(1);
		//please switch D Light
		while(cnt--);
		awbCalibDo(2);
		//please switch standand Light
		while(cnt--);
		awbCalibDo(3);		

	}else{
		//please switch standand Light
		while(cnt--);
		awbCalibDo(3);
		
	}
	awbCalibCtt(calib);
}

#endif
/**
 * @fn        void awbTest(UINT8* pt)
 * @brief     awbTest
 * @param     [in] pt	:test string
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-6
 * @todo      N/A
 * @bug       N/A
*/
void 
awbTest(
	UINT8* pt
	)
{
#if 0
	pt=pt;
#elif 1
	UINT32 arg[8];
	cmdArgGet(&pt, arg, 8, 1);
	printf("%s\n",pt);
	if(arg[0]<=1){
		awbEnable(arg[0]);
	/*}else if(0==strncmp(pt,"dc",2)){
		sp1kAwbParamSet(AWB_PARA_SpotSwitchDelayCnt, arg[1]);//val=0~255,值越大表示awb 光源switch越慢，默认值val=2；
	}else if(0==strncmp(pt,"ct",2)){
		sp1kAwbParamSet(AWB_PARA_ColorTemperatureMin, arg[1]);//设置最高色温，默认值7500K
		sp1kAwbParamSet(AWB_PARA_ColorTemperatureMax, arg[2]);//设置最高色温，默认值7500K
		printf("%u,%u\n",awbctmin,awbctmax);
		sp1kAwbInit(1);
		sp1kAwbEnable();*/
	}else if(0==strncmp(pt,"time",2)){		
		awbFileInfo(0);
	}else if(0==strncmp(pt,"st",2)){	
		sp1kAwbParamSet(AWB_PARA_JumpStep,arg[1]);
#if awbTestSimulateEn
	}else if(0==strncmp(pt,"ti",2)){
		awbCttSet(0xff,"AWBCTT.BIN");
	}else if(0==strncmp(pt,"ts",2)){
		
		awbTestSimulate(arg[1],arg[2]);		
#endif
//#if AWB_CALIBRATION_Test
	}else if(0==strncmp(pt,"cal",3)){
		awbTestCalibration(arg[1]);
//#endif
	}else{
		printf("else0\n");
	}
#else
	UINT8 i;
	UINT32 val[10],c=0,n=0,j=-1;
	memset(val,0,20);
	for(i=0;i<20;i++)
	{
		UINT8 b=pt[i+1];
		UINT8 a=0;
		if(b==0){
			if(j+1!=i)
			{				
				val[n++]=c;
			}	
			break;
		}
		else if(b==' ')
		{
			if(j+1!=i)
			{				
				val[n++]=c;
				c=0;
			}			
			j=i;
			continue;
		}
		else if(b>='0' && b<='9')a=b-'0';
		else if(b>='a' && b<='f')a=b-'a'+10;
		else if(b>='A' && b<='F')a=b-'A'+10;
		else continue;	

		c=c*16+a;
	}

	printf("%s,(%lx,%lx,%lx,%lx,%lx,%lx)\n",pt,val[0],val[1],val[2],val[3],val[4],val[5]);
	if(*pt=='p'){
		awbDebugCnt=val[0];
	}else if(*pt=='1'){
		sp1kAwbEnable();
	}else if(*pt=='0'){
		sp1kAwbDisable();
	}else if(*pt=='i'){	
		sp1kAwbInit(val[0]);
	}else if(*pt=='e'){
		G_AEEN= val[0];
	}else if(*pt=='g'){
		printf("%x\n",awb.globeGain);
		if(val[0]!=0xffff)
			awb.globeGain = val[0];
#if AWB_TIME
	}else if(*pt=='t'){	
		awbTimerView();
#endif
#if 0
	}else if(*pt=='m'){
		dramFileSave("QUEUE",K_SDRAM_DPSStartAddr , 1024*16);
		dramFileSave("TMP",K_SDRAM_PvMoeTmpAddr , 1024*16);
		dramFileSave("WND",K_SDRAM_PvAwbWndAddr , 1024*4);
#endif
#if awbTestSimulateEn
	}else if(*pt=='s'){
		awbEnable(0);
		awbDebugCnt = 0xff;
		if(val[1]){awb.gid = 80;awb.luma = 120;}
		else{awb.gid = 10;awb.luma = 120;}

		if(val[0]){
			for(i = 1;i<=val[0];i++)
				awbTestSimulate(i);			
		}else{
			awbTestSimulate(val[2]);
		}
#endif
#if AWB_Debug_Cmd
	}else if(*pt=='e'){
		aeInit(val[0]);
		sp1kAeInit();
		aeModeEnableSet(1);
	}else if(*pt=='2'){
		dramFileSave("AWB",K_SDRAM_PvAwbWndAddr,1024*4);
	}else if(*pt=='w'){
		HAL_CdspAWBWndGainSet(val[0],val[1]);
	}else if(*pt=='m'){		
		sp1kAwbModeSet(val[0]);
	}else if(*pt=='r'){
		//sp1kAwbReset();	
	}else if(*pt=='c'){
		//sp1kAwbCalibrate(val[0]);
	}else if(*pt=='o'){
		UINT8 e0 = awb.obEn;
		//UINT8 e1 = sp1kAwbAutoObSet(val[0],val[1]);
		printf("ob:%bx,%bx\n",e0,e1);
	}else if(*pt=='l'){
		UINT8 fn[]="AWB.BIN";
		if(val[0]==0)
			;//sp1kAwbQueueLoad(0xff,fn);
		else if(val[0]==1)
			dramFileSave(fn,pawbaddr->buf,7ul*1024);
	}else if(*pt=='q'){
		UINT8 fn[5];
		sprintf(fn,"AWB%bu",(UINT8)val[0]);
		dramFileSave(fn,pawbaddr->queue[0],1024*6);
#endif
#if AWB_Debug_PrintInfo
	}else{
		/*printf("queue:%lu(%lu,%lu,%lu)(%lu,%lu,%lu)\n"
			,(UINT32)sizeof(awb_moe_queue_t)
			,(UINT32)sizeof(awb_moe_normal_t)
			,(UINT32)sizeof(awb_moe_outdoor_t)
			,(UINT32)sizeof(awb_moe_rectone_t)
			,(UINT32)sizeof(awb_moe_normal_t)/24
			,(UINT32)sizeof(awb_moe_outdoor_t)/24
			,(UINT32)sizeof(awb_moe_rectone_t)/24
			);*/
		printf("awb:(%lx,%lx,%lx),%lx,%lx,%lx,%lx,%lx\n",pawbaddr->buf,pawbaddr->moe,pawbaddr->wnd,
			pawbaddr->queue[0],pawbaddr->queue[1],pawbaddr->queue[2],pawbaddr->queue[3],pawbaddr->queue[4]);
		printf("pv0:(%lx,%lx,%lx)\n",K_SDRAM_DPSStartAddr,K_SDRAM_PvMoeTmpAddr,K_SDRAM_PvAwbWndAddr);
		printf("avi:(%lx,%lx,%lx)\n",K_SDRAM_DPSStartAddr,K_SDRAM_AviMoeTmpAddr,K_SDRAM_AviAwbWndAddr);
		//printf("ctt-:%lu\n",(UINT32)256-160-sizeof(awb_t));
		//printf("ob:(%bx,%bx,%bx,%bx)\n",awb.ob0[0],awb.ob0[1],awb.ob0[2],awb.ob0[3]);
		//printf("qSize:%lu,%lu,%lu,%lu,%lu\n",(UINT32)sizeof(awb_moe_queue_t),(UINT32)sizeof(awb_ctt_t),(UINT32)sizeof(awb_t),(UINT32)sizeof(awb_addr_t));
	#endif
	}
#endif	


#if 0
printf("+%s+\n",pt);
	if(*pt=='i'){
		sp1kAwbInit();
		sp1kVdInit();
	}else if(*pt=='0'){
		//G_AWBEN = 0;
		sp1kAwbDisable();
	}else if(*pt=='1'){		
		sp1kAwbEnable();			
	}else if(*pt=='w'){
		UINT32 addr = awbFileInfo(0);
		UINT32 pt[] = {1,2,3,4};
		UINT8 fn0[] = "D:\\RAW00000.DAT";
		printf("addr:%lx\n",addr);
		dramFileSave(fn0,addr, 1024*4);
	}else if(*pt=='t'){
		//awbCttGet(NULL,0xff,"AWBCTT.BIN");
	}else if(*pt=='f'){
		//awbMoeFlag=pt[1]-'0';
	}else if(*pt=='r'){
		//awbMoeRuns();
	}else if(*pt=='a'){
		UINT8 i= pt[1]-'0';
		i*=16;
		//sp1kAwbGainSet((UINT16)i,(UINT16)i);
	}else if(*pt=='g'){
		UINT8 i= pt[1]-'0';
		i*=16;
		HAL_CdspAWBWndGainSet(i,i);
	}else if(*pt=='s'){
		UINT8 i= pt[1]-'0';
		i*=16;
		HAL_CdspWBGainSet(i,0x40,i,0x40);
	}else if(*pt=='c'){
		UINT8 i= pt[1]-'0';
		UINT8 scene[] = {AWB_MODE_Auto,AWB_MODE_Sunny,AWB_MODE_Flash,AWB_MODE_Calibation};
		sp1kAwbModeSet(scene[i]);
	}else if(*pt=='m'){
		UINT8 i= (pt[1]-'0')*10+(pt[2]-'0');
		sp1kAwbModeSet(i);
	}else if(*pt=='p'){
		UINT8 i= (pt[1]-'0');
		//sp1kAwbProcSet(i);
	awbDebugCnt
	}else{
		previewTest(" ");
		cmdDispTest(" ");
		exposureTest("20");
		
		sp1kAwbInit();
		sp1kVdInit();
		sp1kAwbEnable();


	}
#endif
}
