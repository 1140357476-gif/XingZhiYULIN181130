#include "EP932api.h"
#include "EP932Controller.h"  // HDMI Transmiter
#include "gpio_api.h"
#include "hal_global.h"

EP932C_REGISTER_MAP EP932C_Registers;

static UINT8 code bDmy; /* add by Phil reserved for link file */

/*
============================================================================
	need use customer's function 
		
	1. EP932 reset - use customer's GPIO function 

		EP_EP932M_Reset()
		
		### EP932 must reset after video format timing change ###
			
	2. EP932 IIC/DDC - use customer's IIC function

		DDC_If.c  	=> 	DDC_Write(......) , DDC_Read(.......)
		EP932_If.c	=> 	IIC_Write(......) , IIC_Read(.......)

		### customer' IIC function must can check IIC error (no ack, write error, read error) ###
	
	3. Timer 

		need [10ms timer] or [thread] to run EP932Controller_Task(); and EP932Controller_Timer();


   ============================================================================
	initial EP932 

	1. initial EP932 variable and customer's GPIO + I2C (if need).

		EP_HDMI_Init();

	2. set video interface and timing, timing need modify to fit with customer's require

		EP_HDMI_Set_Video_Timing( 3 );   // need change parameter

		EP_HDMI_Set_Video_Output(Video_OUT_Auto);

	3. set audio interface

		EP_HDMI_SetAudFmt(AUD_I2S, AUD_SF_48000Hz); // need change Audio sample rate 

	4. need to run EP932Controller_Task and EP932Controller_Timer every 10ms

		while(1)
		{
			EP932Controller_Task();
			EP932Controller_Timer();
		}

============================================================================
*/


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void EP_EP932M_Reset(void)
{
	////////////////////////////////////////////////////////////////
	// need use customer's GPIO to reset EP932 after video timing change
	//
	// 1. GPIO set to low level
	// 2. delay 200ms
	// 3. GPIO set to high level
	//
	///////////////////////////////////////////////////////////////

	/* reset the HDMI chip */
	printf("hdmi rst\n");
	
#if 1//_HW_SOLUTION_==_HW_CUSTOM_MODEL_A_
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0xff);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0x00);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0xff);
	HAL_GlobalTimerWait(100);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (9 & 0x07), 0xff);
	HAL_GlobalTimerWait(5);
#else
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x01 << (11 & 0x07), 0xff);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (11 & 0x07), 0x00);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x01 << (11 & 0x07), 0xff);
	HAL_GlobalTimerWait(100);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x01 << (11 & 0x07), 0xff);
	HAL_GlobalTimerWait(5);	
#endif

}

void  EP_HDMI_SetAudFmt(HDMI_AudFmt_t  Audfmt, HDMI_AudFreq  Audfreq)
{
	if(Audfmt == AUD_I2S)
	{
		EP932C_Registers.Audio_Interface = 0x18;		// 2 channel IIS
		printf(("Audio interface is IIS - 2.0 CH, "));
	}
	else
	{
		EP932C_Registers.Audio_Interface = 0x10;		// SPDIF
		printf(("Audio interface is SPDIF, "));
	}

	if(Audfreq == AUD_Mute)
	{
		EP932C_Registers.System_Configuration = 0x22;	// Audio mute enable	
	}
	else
	{
		EP932C_Registers.System_Configuration = 0x20;	// Audio mute disable	
	}

	EP932C_Registers.Audio_Input_Format = Audfreq;	// set Audio frequency
	printf(("Audio sample rate = "));
	switch(Audfreq)
	{
		case AUD_Mute:
			printf(("Audio Mute\r\n"));
			break;

		case AUD_SF_32000Hz:
			printf(("32K Hz\r\n"));
			break;
			
		case AUD_SF_44100Hz:
			printf(("44.1K Hz\r\n"));
			break;
			
		case AUD_SF_48000Hz:
			printf(("48K Hz\r\n"));
			break;
			
		case AUD_SF_88200Hz:
			printf(("88.2K Hz\r\n"));
			break;
			
		case AUD_SF_96000Hz:
			printf(("96K Hz\r\n"));
			break;
			
		case AUD_SF_176400Hz:
			printf(("176.4K Hz\r\n"));
			break;
			
		case AUD_SF_192000Hz:
			printf(("192K Hz\r\n"));
			break;

		default:
//			DBG_printf(("Unknown %d \r\n",Audfreq));

		//   printf(" unknow %d \n",Audfreq);
			break;
			
	}
}



void  EP_HDMI_Set_Video_Timing(int Timing)
{
		printf(("\r\n\r\n"));
		printf(("##############################################\r\n"));
	
		// no skew, Dual edge - falling edge first, 12 bit, FMT12 = 0, 
		EP932C_Registers.Video_Interface[0] = 0x06;// /*| DSEL_Dual_Edge*/ | BSEL_24bit| EDGE_rising /*| FMT_12*/;
		DBG_printf(("Video_Interface_0 = 0x%02X \r\n",(int)EP932C_Registers.Video_Interface[0] ));
		
		// mode: DE + Hsync + Vsync , input: YUV422
		EP932C_Registers.Video_Interface[1] = 0x00; 	// DE,HS,VS, YUV422
		DBG_printf(("Video_Interface_1 = 0x%02X \r\n",(int)EP932C_Registers.Video_Interface[1] ));
	
		switch (Timing)
		{
			case 0:
				DBG_printf(("Video_Input_720P_60Hz\r\n"));
				EP932C_Registers.Video_Input_Format[0] = 4;
				break;
	
			case 1:
				DBG_printf(("Video_Input_720P_50Hz\r\n"));
				EP932C_Registers.Video_Input_Format[0] = 19;
				break;
				
		   	case 2: 
				DBG_printf(("Video_Input_1080I_60Hz\r\n"));
				EP932C_Registers.Video_Input_Format[0] = 5;
				break;
	
			case 3: 
				DBG_printf(("Video_Input_1080P_60Hz\r\n"));
				EP932C_Registers.Video_Input_Format[0] = 16;
				break;
		
			default:
				DBG_printf(("Video_Input_Unknown : %d\r\n",Timing));
				EP932C_Registers.Video_Input_Format[0] = 0x00;
				break;
		}
		
		/*
			 0:  No video input
			 1:  CEA-861D 640 x 480 		(60Hz or 59.94Hz)	Progressive 4:3
			 2:  CEA-861D 720 x 480 		(60Hz or 59.94Hz)	Progressive 4:3
			 3:  CEA-861D 720 x 480 		(60Hz or 59.94Hz)	Progressive 16:9
			 4:  CEA-861D 1280 x 720		(60Hz or 59.94Hz)	Progressive 16:9
			 5:  CEA-861D 1920 x 1080	(60Hz or 59.94Hz)	Interlaced 16:9
			 6:  CEA-861D 720(1440) x 480	(60Hz or 59.94Hz)	Interlaced 4:3
			 7:  CEA-861D 720(1440) x 480	(60Hz or 59.94Hz)	Interlaced 16:9
			 8:  CEA-861D 720(1440) x 240	(60Hz or 59.94Hz)	Progressive 4:3
			 9:  CEA-861D 720(1440) x 240	(60Hz or 59.94Hz)	Progressive 16:9
			10: CEA-861D 2880 x 480 		(60Hz or 59.94Hz)	Interlaced 4:3
			11: CEA-861D 2880 x 480 		(60Hz or 59.94Hz)	Interlaced 16:9
			12: CEA-861D 2880 x 240 		(60Hz or 59.94Hz)	Progressive 4:3
			13: CEA-861D 2880 x 240 		(60Hz or 59.94Hz)	Progressive 16:9
			14: CEA-861D 1440 x 480 		(60Hz or 59.94Hz)	Progressive 4:3
			15: CEA-861D 1440 x 480 		(60Hz or 59.94Hz)	Progressive 16:9
			16: CEA-861D 1920 x 1080		(60Hz or 59.94Hz)	Progressive 16:9
			17: CEA-861D 720 x 576		(50Hz)			Progressive 4:3
			18: CEA-861D 720 x 576		(50Hz)			Progressive 16:9
			19: CEA-861D 1280 x 720 		(50Hz)			Progressive 16:9 
			20: CEA-861D 1920 x 1080		(50Hz)			Interlaced 16:9
			21: CEA-861D 720(1440) x 576	(50Hz)			Interlaced 4:3
			22: CEA-861D 720(1440) x 576	(50Hz)			Interlaced 16:9
			23: CEA-861D 720(1440) x 288	(50Hz)			Progressive 4:3
			24: CEA-861D 720(1440) x 288	(50Hz)			Progressive 16:9
			25: CEA-861D 2880 x 576 		(50Hz)			Interlaced 4:3
			26: CEA-861D 2880 x 576 		(50Hz)			Interlaced 16:9
			27: CEA-861D 2880 x 288 		(50Hz)			Progressive 4:3
			28: CEA-861D 2880 x 288 		(50Hz)			Progressive 16:9
			29: CEA-861D 1440 x 576 		(50Hz)			Progressive 4:3
			30: CEA-861D 1440 x 576 		(50Hz)			Progressive 16:9
			31: CEA-861D 1920 x 1080		(50Hz)			Progressive 16:9
			32: CEA-861D 1920 x 1080		(24Hz)			Progressive 16:9
			33: CEA-861D 1920 x 1080		(25Hz)			Progressive 16:9
			34: CEA-861D 1920 x 1080		(30Hz)			Progressive 16:9
		*/
	
		// power on 
		EP932C_Registers.Power_Control = 0x00;
	
	//===================================================================
	
		DBG_printf(("##############################################\r\n"));

	
}

void EP_HDMI_Set_Video_Output(int HDMI_output)
{	
	switch(HDMI_output)	
	{		
		default:		
		case Video_OUT_Auto:
			EP932C_Registers.Video_Output_Format = 0x00;		// Auto select output			
			break;	
			
		case Video_OUT_YUV_444:			
			EP932C_Registers.Video_Output_Format = 0x01;		// YUV 444 output			
			break;		
			
		case Video_OUT_YUV_422:			
			EP932C_Registers.Video_Output_Format = 0x02;		// YUV 422 output			
			break;		
			
		case Video_OUT_RGB_444:			
			EP932C_Registers.Video_Output_Format = 0x03;		// RGB 444 output			
			break;	
			
	}
}

unsigned char EP_HDMI_Init(void)
{

	EP932Controller_Initial(&EP932C_Registers, NULL);

	return 0; //HDMI_SUCCESS;
}

void hdmi2_start (void)
{
return ;
}
