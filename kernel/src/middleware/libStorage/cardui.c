#include "general.h"
#include "cardlink.h"

// WWW2 start
code	UINT8	C_USR_Company_UpperSIG[] =
{
	'S', 'U', 'N', 'P'
};

code	UINT8	C_USR_Company_LowerSIG[] =
{
	's', 'u', 'n', 'p'
};

code	UINT8	C_USR_Dir0Name[] = 
{
	'D', 'C', 'I', 'M', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x10
};

code	UINT8	C_USR_Dir1Name[] = 
{
	'1', '0', '0', 'M', 'E', 'D', 'I', 'A', ' ', ' ', ' ', 0x10
};

xdata	UINT8	G_USR_TLSVIDDirName[] = 
{
	'1', '0', '0', 'T', 'L', 'V', 'I', 'D', ' ', ' ', ' ', 0x10
};

xdata	UINT8	G_CustomerAVIName[] = 
{
	'A', 'V', 'I', 0x10
};

xdata	UINT8	G_USR_Company_UpperSIG[] =
{
	'S', 'U', 'N', 'P'
};

xdata	UINT8	G_USR_Company_LowerSIG[] =
{
	's', 'u', 'n', 'p'
};

xdata	UINT8	G_USR_Dir0Name[] = 
{
	'D', 'C', 'I', 'M', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x10
};

xdata	UINT8	G_USR_Dir1Name[] = 
{
	'1', '0', '0', 'M', 'E', 'D', 'I', 'A', ' ', ' ', ' ', 0x10
};
// WWW2 end


#if ( K_MEDIA_UI_OPTION == 1 )

UINT32 File_SendMessage( UINT16 message, UINT32 param ) USING_0
{
	UINT32	result = 0;
	
	switch ( message )
	{
		case K_File_UI_Start_DscFile:
			break;
		case K_File_UI_Start_DscImage:
			if (UI_CheckIfUseFlash())
			{
				DoFlash();
				
                                //enable VD interrupt ????
			}
			else
			{
				AE_AWB_C_Snap();

                                //snap ????
			}
			break;
		case K_File_UI_End_DscImage:
			break;
		case K_File_UI_Start_VideoImage:
			if (UI_CheckIfUseFlash())
				DoFlash();		
			break;
		case K_File_UI_End_VideoImage:
			if (UI_CheckIfUseFlash())
			{
                                //enable VD interrupt ????
			}
			break;
		case K_File_UI_End_DscFile:
			Y_IncreaseOne();
			break;
		case K_File_UI_Err_DscFile:
			Y_IncreaseNone();
			break;
		case K_File_UI_Start_AviFile:
			break;
		case K_File_UI_End_AviFile:
			Y_IncreaseOne();
			break;
		case K_File_UI_Err_AviFile:
			Y_IncreaseNone();
			break;
		case K_File_UI_Periodical:
			Y_UI_StopKeyRead(0);
			break;
		case K_File_UI_VideoStrm1Frame:
			Y_UI_StopKeyRead(1);
			break;
		case K_File_UI_Error:
			break;
	
	} // end switch
	
	return result;
}

#else

UINT32 File_SendMessage( UINT16 message, UINT32 param ) USING_0
{
        UINT32   ulTemp = param;
	UINT32	result = 0;
	
	switch ( message )
	{
		case K_File_UI_Start_DscFile:
			break;
		case K_File_UI_Start_DscImage:
			break;
		case K_File_UI_End_DscImage:
			break;
		case K_File_UI_Start_VideoImage:	// 500 use
			break;
		case K_File_UI_End_VideoImage:		// 500 use
			break;
		case K_File_UI_End_DscFile:
			break;
		case K_File_UI_Err_DscFile:
			break;
		case K_File_UI_Start_AviFile:
			break;
		case K_File_UI_End_AviFile:
			break;
		case K_File_UI_Err_AviFile:
			break;
		case K_File_UI_Periodical:
			break;
		case K_File_UI_VideoStrm1Frame:
			break;
		case K_File_UI_Error:
			break;
		case K_File_UI_Start_WaveFile:		// WAV file, @102501
			break;
		case K_File_UI_End_WaveFile:		// WAV file, @102501
			break;
		case K_File_UI_Err_WaveFile:		// WAV file, @102501
			break;
	
	} // end switch
	
	return result;
}

#endif


