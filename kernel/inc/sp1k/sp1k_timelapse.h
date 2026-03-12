#ifndef __SP1K__TIMELAPSE__H__
#define __SP1K_TIMELAPSE__H__

#include "timelapse.h"

#ifdef SP1K_DOXY_GEN

/**
* @fn void sp1kTimelapse_ParamSet()
* @brief   set timelapse parameter 
* @param[in] paraId
* @param[in] value
* @retval  none
*/			                	
void sp1kTimelapse_ParamSet( UINT8 paraId, UINT16 value );
/**
* @fn UINT16 sp1kTimelapse_ParamGet();
* @brief   get timelapse parameter 
* @param[in] paraId	 
* @retval param value	
*/
UINT16 sp1kTimelapse_ParamGet( UINT8 paraId );
/**
* @fn UINT8 timelapse_Entry(UINT32 dramAddrW, UINT16 width, UINT16 height, TimeLapse_Mode_t mode);
* @brief   timelapse entry
* @param[in] dramAddrW
* @param[in] width
* @param[in] height
* @param[in] mode 
* @retval SUCCESS/FAIL	
*/
UINT8 sp1kTimeLapse_Entry(UINT32 dramAddrW, UINT16 width, UINT16 height,UINT32 vlcSize);
/**
* @fn UINT8 timelapse_AddData(UINT32 startBufAddrW, UINT32 dataOffsetW, UINT32 dataLength, TimeLapse_Mode_t mode);
* @brief   stop timelapse  
* @param[in] dramAddrW
* @param[in] mode 
* @retval SUCCESS/FAIL	
*/
UINT8 sp1kTimeLpase_AppendFrame(UINT32 startBufAddrW, UINT32 dataLength );

/**
* @fn UINT8 sp1kTimelapse_Stop(UINT32 dramAddrW, TimeLapse_Mode_t mode);
* @brief  write data to video  
* @param[in] startBufAddrW
* @param[in] dataOffsetW 
* @param[in] dataLength
* @param[in] mode 
* @retval SUCCESS/FAIL	
*/
void sp1kTimelapse_AddFrameHdr(UINT32 addr,UINT16 width,UINT16 height,UINT8 qIdx);

#else

#define sp1kTimelapse_ParamSet          timelapse_ParamSet
#define sp1kTimelapse_ParamGet			timelapse_ParamGet
#define sp1kTimeLapse_Entry				timelapse_Entry
#define sp1kTimeLpase_AppendFrame		timelapse_AppendFrame
#define sp1kTimelapse_AddFrameHdr		timelapse_AddFrameHdr

#endif


#endif
