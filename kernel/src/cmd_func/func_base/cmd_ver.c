/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		cmd_ver.c
 * @brief		display lib version
 * @author	Phil Lin
 * @since		2008-07-25
 * @date		2008-11-06
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "main.h"
#include "cmd.h"
#include "solution.h"
#include "knl_func_opt.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/


/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
extern UINT8 *libAeVerGet(void);
extern UINT8 *libAudVerGet(void);
extern UINT8 *libAwbVerGet(void);
extern UINT8 *libBpVerGet(void);
extern UINT8 *libCalVerGet(void);
extern UINT8 *libCdspVerGet(void);
extern UINT8 *libCdspFilterVerGet(void);
extern UINT8 *libCmdVerGet(void);
extern UINT8 *libComVerGet(void);
extern UINT8 *libDsVerGet(void);
extern UINT8 *libDispVerGet(void);
extern UINT8 *libDistVerGet(void);
extern UINT8 *libDosAVerGet(void);
extern UINT8 *libDosBVerGet(void);
extern UINT8 *libDosCVerGet(void);
extern UINT8 *libDosDVerGet(void);
//extern UINT8 *libDpofVerGet(void);
extern UINT8 *libExifVerGet(void);
extern UINT8 *libFdVerGet(void);
extern UINT8 *libFrontVerGet(void);
extern UINT8 *libSysGlobalVerGet(void);
extern UINT8 *libGpioVerGet(void);
extern UINT8 *libGprmVerGet(void);
#ifdef ICAT_OPTION
	extern UINT8 *libIcatVerGet(void);
#endif
extern UINT8 *libIntrVerGet(void);
extern UINT8 *libOsVerGet(void);
extern UINT8 *libOsdVerGet(void);
extern UINT8 *libPbVerGet(void);
extern UINT8 *libPvVerGet(void);
extern UINT8 *libSnapVerGet(void);
//extern UINT8 *libSpeGet(void);
extern UINT8 *libDiskVerGet(void);
//extern UINT8 *libStorageNorVerGet(void);
extern UINT8 *libStorageSdVerGet(void);
extern UINT8 *libStorageComVerGet(void);
extern UINT8 *libSysVerGet(void);
extern UINT8 *libUsbAVerGet(void);
extern UINT8 *libUsbBVerGet(void);
extern UINT8 *libUsbCVerGet(void);
extern UINT8 *libUsbDVerGet(void);
extern UINT8 *libUsbGlobalVerGet(void);
extern UINT8 *libUtilVerGet(void);
extern UINT8 *libVideoClipVerGet(void);
extern UINT8 *libVideoPbVerGet(void);
extern UINT8 *libVideoCapVerGet(void);
extern UINT8 *libZoomVerGet(void);
extern UINT8 *libDebugVerGet(void);
extern UINT8 *libFuncVerGet(void);
extern UINT8 *lib3aApiVerGet(void);
extern UINT8 *libApiAudVerGet(void);
extern UINT8 *libApiDsVerGet(void);
extern UINT8 *libApiDispVerGet(void);
extern UINT8 *libApiGpioVerGet(void);
extern UINT8 *libApiRtcVerGet(void);
extern UINT8 *libHalAudVerGet(void);
extern UINT8 *libHalCardVerGet(void);
extern UINT8 *libHalCdspVerGet(void);
extern UINT8 *libHalDispVerGet(void);
//extern UINT8 *libHalDistVerGet(void);
extern UINT8 *libHalDramVerGet(void);
extern UINT8 *libHalFdVerGet(void);
extern UINT8 *libHalFlashVerGet(void);
extern UINT8 *libHalFrontVerGet(void);
extern UINT8 *libHalGlobalVerGet(void);
extern UINT8 *libHalGpioVerGet(void);
extern UINT8 *libHalGprmVerGet(void);
extern UINT8 *libHalJpegVerGet(void);
extern UINT8 *libHalMoeVerGet(void);
extern UINT8 *libHalPbVerGet(void);
extern UINT8 *libHalUsbVerGet(void);
extern UINT8 *libLscCalVerGet(void);
extern UINT8 *libMDVerGet(void);
#if FUNC_KNL_MSCAL
extern UINT8 *libMsCalVerGet(void);
#endif

//-----------------------------------------------------------------------------
//cmdVerManager
//-----------------------------------------------------------------------------
/**
 * @brief	Display F/W & lib version
 * @param	NULL
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since	2008-07-25
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
void
cmdVerManager(
	void
)
{
	const UINT8 verStrLen = 4; //define the length of lib version
	const UINT8 prfFmt[] = "%s";//reduce const string
	UINT8 i, j;
	/* for printf aligment */
	UINT8 strLenMax = 0;
	UINT8 strAlign;
	/* length of ver string is 4+1 */
	struct {
		UINT8 libName[16];
		UINT8* (code* pf)();
		UINT8 libVer[4+1];
	} libHdl[] = {
		{"ae", libAeVerGet, {0}},
		{"aud", libAudVerGet, {0}},
		{"awb", libAwbVerGet, {0}},
		#if FUNC_KNL_BP
		//{"bp", libBpVerGet, {0}},
		#endif
		{"cal", libCalVerGet, {0}},
		{"cdsp", libCdspVerGet, {0}},
		{"cdspfilter", libCdspFilterVerGet, {0}},
		{"cmd", libCmdVerGet, {0}},
		{"com", libComVerGet, {0}},
		{"ds", libDsVerGet, {0}},
		{"disp", libDispVerGet, {0}},
		{"dist", libDistVerGet, {0}},
		{"dosA",libDosAVerGet, {0}},
		{"dosB",libDosBVerGet, {0}},
		{"dosC",libDosCVerGet, {0}},
		{"dosD",libDosDVerGet, {0}},
		//{"dpof", libDpofVerGet, {0}},
		{"exif", libExifVerGet, {0}},
		{"fd", libFdVerGet, {0}},
		{"front", libFrontVerGet, {0}},
		{"global", libSysGlobalVerGet, {0}},
		{"gpio", libGpioVerGet, {0}},
		{"gprm", libGprmVerGet, {0}},
		#ifdef ICAT_OPTION
			{"icat", libIcatVerGet, {0}},
		#endif
		{"intr", libIntrVerGet, {0}},
		{"os", libOsVerGet, {0}},
		{"osd", libOsdVerGet, {0}},
		{"pb", libPbVerGet, {0}},
		{"pv", libPvVerGet, {0}},
		{"snap", libSnapVerGet, {0}},
		//{"spe", libSpeGet, {0}},
		{"disk", libDiskVerGet, {0}},
		//{"nor", libStorageNorVerGet, {0}},
		{"sd", libStorageSdVerGet, {0}},
		{"diskCom", libStorageComVerGet, {0}},
		{"sys", libSysVerGet, {0}},
		{"usbA", libUsbAVerGet, {0}},
		{"usbB", libUsbBVerGet, {0}},
		{"usbC", libUsbCVerGet, {0}},
		{"usbD", libUsbDVerGet, {0}},
		{"usbG", libUsbGlobalVerGet, {0}},
		{"util", libUtilVerGet, {0}},
		{"vidRec", libVideoClipVerGet, {0}},
		{"videPb", libVideoPbVerGet, {0}},
		{"vidCap", libVideoCapVerGet, {0}},		
		{"zoom", libZoomVerGet, {0}},
		{"debug", libDebugVerGet, {0}},
//		{"func", libFuncVerGet, {0}},
		//{"api3A", lib3aApiVerGet, {0}},
		{"apiAud", libApiAudVerGet, {0}},
		{"apiDs", libApiDsVerGet, {0}},
		{"apiDisp", libApiDispVerGet, {0}},
		{"apiGpio", libApiGpioVerGet, {0}},
		{"apiRtc", libApiRtcVerGet, {0}},
		{"hAud", libHalAudVerGet, {0}},
		{"hCard", libHalCardVerGet, {0}},
		{"hCdsp", libHalCdspVerGet, {0}},
		{"hDisp", libHalDispVerGet, {0}},
		//{"hDist", libHalDistVerGet, {0}},
		{"hDram", libHalDramVerGet, {0}},
		{"hFd", libHalFdVerGet, {0}},
		{"hFlash", libHalFlashVerGet, {0}},
		{"hFront", libHalFrontVerGet, {0}},
		{"hGlobal", libHalGlobalVerGet, {0}},
		{"hGpio", libHalGpioVerGet, {0}},
		{"hGprm", libHalGprmVerGet, {0}},
		{"hJpeg", libHalJpegVerGet, {0}},
		{"hMoe", libHalMoeVerGet, {0}},
		{"hPb", libHalPbVerGet, {0}},
		{"hUsb", libHalUsbVerGet, {0}},

		#if FUNC_KNL_LSCCAL
		{"lsccal", libLscCalVerGet, {0}},
		#endif
		
		#if FUNC_KNL_MD
		{"md",libMDVerGet, {0}},
		#endif
		#if FUNC_KNL_MSCAL
		{"msCal",libMsCalVerGet, {0}},
		#endif
		{{0}, NULL, {0}}, // keep this item at the last
	};

	for (i = 0; libHdl[i].pf != NULL; i++) {
		strLenMax = strlen(libHdl[i].libName) > strLenMax ? strlen(libHdl[i].libName) : strLenMax;
		/* copy to local buffer, fixed the string length*/
		memcpy(&libHdl[i].libVer[0], (*libHdl[i].pf)(), verStrLen);
	}
	//printf("strLenMax %bu\n", strLenMax);
	if (strLenMax > sizeof(libHdl[0].libName)) {
		//printf("The length of lib version is too long!\n");
		return;
	}

	/* Show F/W version firstly */
	printf("\nCurrent F/W version: %s.%lu\n", sp1kFWVerGet(),sp1kSvnVerGet());

	/*Show lib version*/

	for (j = 0; j < i; j++) {
		if (j & 0x01) {
			putchar('\t');
			putchar('\t');
		} else {
			putchar('\n');
		}
		strAlign = strLenMax;
		strAlign -= printf(prfFmt, libHdl[j].libName);

		/* fill space for printf alignment*/
		while(strAlign--) {
			putchar(' ');
		}
		printf(": %s", libHdl[j].libVer);
	}

	/*Finished*/
	putchar('\n');
}

