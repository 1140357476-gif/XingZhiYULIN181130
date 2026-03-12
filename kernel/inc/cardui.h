/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef _cardui_H_
#define _cardui_H_

//------------------------------------------------------------------------------
//Function prototype
//------------------------------------------------------------------------------
UINT32 File_SendMessage( UINT16 message, UINT32 param );
UINT32 File_DoPeriodMsg( UINT32 param );

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
// 0 to 2000 reserved by Sunplus

#define K_File_UI_Start_DscFile		1 	// Start capturing still image file
#define K_File_UI_Start_DscImage	2 	// Start capturing still image
#define K_File_UI_End_DscImage		3 	// End capturing still image
						// param is image size
#define K_File_UI_Start_VideoImage	4	// Start capturing still image by video mode
						// this message use in SPCA 500 only for capture
						// still image with subsample, 500 require using
						// video mode
#define K_File_UI_End_VideoImage	5	// End capturing still image by video mode
						// param is image size
#define K_File_UI_End_DscFile		6	// End capturing still image file
						// param is G_USR_FileCount
#define K_File_UI_Err_DscFile		7	// Error capturing still image file. If param is
						// non zero, definition is same to 
						// K_File_UI_Error.
#define K_File_UI_Start_AviFile		8	// Start recodring avi image file
#define K_File_UI_End_AviFile		9	// End recodring avi image file
						// param is G_USR_FileCount
#define K_File_UI_Err_AviFile		10	// Error capturing avi image file. If param is
						// non zero, definition is same to 
						// K_File_UI_Error.
#define K_File_UI_Periodical		11	// Card module is doing lengthy operation
						// There are many different kind of lengthy ope-
						// ration such as formatting, deleting all, etc.
						// These operations are passed in param to indi-
						// cate what kind of lengthy operation is under
						// going.
#define K_File_UI_VideoStrm1Frame	12	// During recording avi, if one video frame is
						// produced, this message will be sent. The pa-
						// ram is the video frame count G_File_AviImage
						// -Count.
#define K_File_UI_Error			13	// If any error happen, this message will be 
						// sent. If param is zero, the error cause is 
						// unknown. If param is non zero, please find
						// header file for error code meanning.
#define K_File_UI_Start_WaveFile	14	// Start recording audio wave file
#define K_File_UI_End_WaveFile		15	// End recording audio wave file
						// param is G_USR_FileCount.
#define K_File_UI_Err_WaveFile		16	// Error recording audio wave file. If param is
						// non zero, definition is same to 
						// K_File_UI_Error.

#define K_File_UI_Start_DscCapture	17	// Start capture dsc image. At this point, came-
						// ra must be fixed until K_File_UI_End_DscCapt-
						// ure message is received.
#define K_File_UI_End_DscCapture	18	// End capture dsc image. If this message rece-
						// ived, user is free to move camera. You may
						// beep a sound to inform user.
#define K_File_UI_Start_DscCompress	19	// Start compressing dsc image. While the asic
						// is compressing, camera is allowed to move.
#define K_File_UI_End_DscCompress	20	// End compressing dsc image.

#define K_File_UI_Start_Save_File	50	// Start saving data to card.
#define K_File_UI_End_Save_File		51	// End saving data to file. If param is non
						// zero, definition is same to K_File_UI_Error.
#define K_File_UI_Start_Delete1File	52	// Start deleting 1 file.
#define K_File_UI_End_Delete1File	53	// End deleting 1 file. Param is G_USR_FileCount.
#define K_File_UI_Err_Delete1File	54	// Error deleting 1 file. If param is non zero,
						// definition is same to K_File_UI_Error.
#define K_File_UI_Start_DeleteAll	55	// Start deleting all file
#define K_File_UI_End_DeleteAll		56	// End deleting all file. Param is G_USR_File-
						// Count.
#define K_File_UI_Err_DeleteAll		57	// Error deleting all file. If param is non zero
						// , definition is same to K_File_UI_Error.
#define K_File_UI_Start_Initialize	58	// Start initialize card, dos, etc.
#define K_File_UI_End_Initialize	59	// End initialization, If param is non zero,
						// definition is same to K_File_UI_Error.
#define K_File_UI_Start_FormatCard	60	// Start formatting card.
#define K_File_UI_End_FormatCard	61	// End formatting card. If param is non zero,
						// definition is same to K_File_UI_Error.

#define K_File_UI_Start_PlayDscFile	21 	// Start playing still image file
						// param is file index
#define K_File_UI_End_PlayDscFile	22	// End playing still image file
#define K_File_UI_Start_PlayAviFile	23	// Start play avi image file
#define K_File_UI_End_PlayAviFile	24	// End play avi image file
#define K_File_UI_PlayVideoStrm1Frame	25	// During playing avi, if one video frame is
						// produced, this message will be sent. The pa-
						// ram is the video frame count G_File_AviImage
#define K_File_UI_Start_PlayWaveFile	26	// Start playing audio wave file
#define K_File_UI_End_PlayWaveFile	27	// End playing audio wave file

// 0 to 2000 reserved by Sunplus
#define K_Act_Go_SaveFile		1	// saving file action is running
#define K_Act_Go_Delete1File		2	// delete one file action is running
#define K_Act_Go_DeletAll		3	// delete all file action is running
#define K_Act_Go_Initialize		4	// initializing card action is running
#define K_Act_Go_FormatCard		5	// formatting card action is running
#define K_Act_Go_RecordAvi		6	// recording avi action is running
#define K_Act_Go_RecordAviAud		7	// recording avi with audio action is running
#define K_Act_Go_RecordAudio		8	// recording audio action is running

#define K_Act_Go_PlayAvi		9	// recording avi action is runninn
#define K_Act_Go_PlayAviAud		10	// playing avi with audio action is running
#define K_Act_Go_PlayAudio		11	// playing audio action is running

#define K_Act_Go_BuildLinearFAT		12	// when record and playing avi, you specify
						// build LinearFAT, this message will be sent

//------------------------------------------------------------------------------
//External variable
//------------------------------------------------------------------------------
// WWWW0327 start
extern	code	UINT8	C_USR_Company_UpperSIG[];
extern	code	UINT8	C_USR_Company_LowerSIG[];
extern	code	UINT8	C_USR_Dir0Name[];
extern	code	UINT8	C_USR_Dir1Name[];

extern	xdata	UINT8	G_USR_Company_UpperSIG[];
extern	xdata	UINT8	G_USR_Company_LowerSIG[];
extern	xdata	UINT8	G_USR_Dir0Name[];
extern	xdata	UINT8	G_USR_Dir1Name[];
extern  xdata   UINT8   G_USR_TLSVIDDirName[];
extern  xdata	UINT8	G_CustomerAVIName[];
// WWWW0327 end

#endif /*__cardui_H__*/