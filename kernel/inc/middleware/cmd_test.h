/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#ifndef __CMD_TEST_H__
#define __CMD_TEST_H__


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
//#define NAND_CMD_OPTION      0
//#define SD_CMD_OPTION        0
#define SECTOR_CMD_OPTION    0

#define NAND_RW 		0
#define CMD_BTN_TEST 	0
#define BADPIXEL_TEST 	0
#define FILE_ATTR_TEST 	0
#define CDSP_IQ_TEST	0		
#define FRONT_TEST	 	0
#define AE_TEST	 		0
#define I2C_TEST		0
#define RAW_TEST		0
#define SIZE_TEST		0
#define LSC_CAL_TEST    0
#define FW_RED_SD_ON   	0 /*meili add for fw read from sd 20101201*/
#define SHUTTER_TEST    0
#define MD_TEST			0
#define DISP_TEST		0

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
void testFunc(UINT8* ptr);
void testVoidFunc(void);
void CardInitTest(void);
void TestVideo(UINT8 * ptr);
void TestPreview(void);
void testShutterSound(UINT8 * ptr);
void TestSnap(UINT8 * ptr);
void Testaudio(UINT8 * ptr);
void TestDisplay(UINT8* ptr);
void TestWendy(UINT8* ptr);
void TestExp(void);
void initIntr(void);
void testAEtable(void);
void DRAMReliabilityTest(void);
void TestPb(UINT8* ptr);
void TestUI(UINT8* ptr);
void TestCDSP(UINT8* ptr);
void TestUSB(UINT8* ptr);
void LoadBinFile(void);
void testFrontGain(UINT8* ptr);
void testFrontTime(UINT8* ptr);

#if (SIZE_TEST == 1)
void TestReportSize(void);
#endif

#if (RAW_TEST == 1)
void TestRAW(UINT8 *pt);
#endif

#if (AE_TEST == 1)
void TestAE(UINT8 *pt);
#endif

#if (I2C_TEST == 1)
void TestI2C(UINT8 *ptr);
#endif

#if CDSP_IQ_TEST
void testEdgeFilter(UINT8 *ptr);
void testCoringFilter(UINT8 *ptr);
void testSnapIQ(UINT8* ptr);
void testTD(UINT8* ptr);
#endif

#if LSC_CAL_TEST
void lscCal(void);	
#endif

#if SHUTTER_TEST
void shutterOnOff(UINT8* ptr);
void shutterCal(void);
void sutterAETest(void);
#endif

#if MD_TEST
void motionDetTest(UINT8* ptr);
#endif

#if FW_RED_SD_ON//meili add for fw read from sd 20101201
void FWreadfrmSD(void);
#endif
void osdApiTest(void);
void testAlc( UINT8* ptr );
void testRawAccess( UINT8* ptr );
void testNandAccess( UINT8* ptr );
void close3A(UINT8* ptr);
void testTSnap(UINT8* ptr);
UINT8 testTSnapRoutine(UINT16* ptr);
void testTPlayback(UINT8* ptr);
UINT8 testTPlaybackRoutine(UINT16* ptr);
void nandTest( UINT8* ptr );
void sectorTest( UINT8* ptr );
void nandTest( UINT8* ptr );
void testCradIsp( UINT8* ptr );
void testCradBoot( UINT8* ptr );
void testTSnapStop(UINT8* ptr);
void dosInfoCmd(UINT8* ptr);
void testPbEffect(UINT8* ptr);
void testVideoclip(UINT8* ptr);
void testNandWR(UINT8* ptr);
void DscLife_span(UINT8* ptr);
UINT8  testDscLife_span(void);
void testButton(UINT8* ptr);
void testBp(UINT8* ptr);
void testFileAttr(UINT8* ptr);
void cmdMsgPost(UINT8* ptr);
void dpsCmd(UINT8* ptr);
void testBitRate(UINT8* ptr);
void cmdDispTest(UINT8 *ptr);

#endif /*__CMD_TEST_H__*/

