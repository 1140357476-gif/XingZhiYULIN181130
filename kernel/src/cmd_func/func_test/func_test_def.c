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
 * @file      func_test_def
 * @brief     function test definition for internal use
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#define FUNC_TEST_INTERNAL
#include "func_test_def.h"
#include "cmd.h"
#include "cmd_test.h"

#include "sp1k_video_api.h"
#include "awb_api.h"
#include "sp1k_fd_api.h"
#include "cdsp_filter.h"
#include "usbopt.h"
#include "dps_api.h"
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

FUNC_TEST_DCL cmd_t code testCmdArray[] = {
	{"autotest", cmdAutoTest},

	{"brc", testBitRate},
	{"awb", awbTest},
	{"fd", fdTest},
	{"vid",videoTest},
	
#if DISP_TEST	
	{"disp", cmdDispTest},
#endif	
	
#if (SIZE_TEST == 1)
	{"rpsize",  TestReportSize},
#endif

#if (RAW_TEST == 1)
	{"raw", TestRAW},
#endif

#if (AE_TEST == 1)
	{"ae", TestAE},
#endif

#if FRONT_TEST
	{"gain", testFrontGain},
	{"time", testFrontTime},
#endif	

#if (I2C_TEST == 1)
	{"i2c", TestI2C},
#endif
	
#if CDSP_IQ_TEST 
	{"edge", testEdgeFilter},
	{"coring", testCoringFilter},	
	{"snapiq", testSnapIQ},
	{"pvtd",testTD},
#endif

#if LSC_CAL_TEST 
	{"lsc",lscCal},
#endif	

#if SHUTTER_TEST
	{"sh",shutterOnOff},
	{"shcal",shutterCal},
	{"shtest",sutterAETest},
#endif

#if MD_TEST
	{"md",motionDetTest},
#endif
	//{"fpga" , cmdFPGA},

#if USBSIDC_OPTION == 2
	{"dps", dpsCmd},
#endif

#if TAG_TODO	


#if LIFE_SPAN_TEST
	{"dsclife", DscLife_span},
#endif

#if 0
	{"wendy", TestWendy},
	{"test", testFunc},
	{"intr",initIntr},
	{"TestVoid", testVoidFunc},
	{"card", CardInitTest},
	{"disp", TestDisplay},
	{"v", TestVideo},
	{"a",Testaudio},
	{"pv", TestPreview},
	{"ss", testShutterSound},
	{"snap", TestSnap},
	{"setexp",TestExp},
	{"ui", TestUI},
	{"osd", TestUI},
	{"check", TestCDSP},
	{"load",LoadBinFile},
	{"usb", TestUSB},
	{"alc", testAlc},
	{"3a",close3A,},
	{"isp", testCradIsp},
	{"boot", testCradBoot},
#endif
#if 0 //xian mask
	{"pb", TestPb},
	{"dPB", testTPlayback},
	{"dSnap", testTSnap},
	{"s", testTSnapStop},
#endif	
//	{"cpy", dosCpyCmd},
//	{"raw", testRawAccess},
#if 0//(NAND_CMD_OPTION == 1)
	{"nand", nandTest},
#endif
#if 0//(SD_CMD_OPTION == 1)
	{"sd", sdTest},
#endif
#if (SECTOR_CMD_OPTION == 1)
	{"sector", sectorTest},
#endif
	{"effect", testPbEffect},

#if NAND_RW
	{"rw", testNandWR},
#endif

#if CMD_BTN_TEST
	{"adc", testButton},
#endif
#if BADPIXEL_TEST
	{"bp", testBp},
#endif
#if FILE_ATTR_TEST
	{"attr", testFileAttr},
#endif

#endif //TAG_TODO
	
	NULL,
};

