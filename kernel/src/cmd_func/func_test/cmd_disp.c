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
 * @file      cmd_disp.c
 * @brief     display control command line
 * @author    Phil Lin
 * @since     2011-04-12
 * @date      2011-04-12
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "reg_def.h" 
#include "cmd.h" 
#include "hal_disp.h" 
#include "cmd_test.h" 

#if DISP_TEST
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

static void cmdDispRptSize(void)
{
  UINT32 pvX0, pvY0, pvX1, pvY1;
  UINT32 pvH0, pvV0, pvH1, pvV1;
  UINT32 srcW, srcH, dispW, dispH;

  WRITE8(pvX0, 3, XBYTE[REG_Disp_PvX0L]);
  WRITE8(pvX0, 2, XBYTE[REG_Disp_PvX0H]);
  WRITE8(pvX0, 1, 0);
  WRITE8(pvX0, 0, 0);
  WRITE8(pvX1, 3, XBYTE[REG_Disp_PvX1L]);
  WRITE8(pvX1, 2, XBYTE[REG_Disp_PvX1H]);
  WRITE8(pvX1, 1, 0);
  WRITE8(pvX1, 0, 0);
  WRITE8(pvY0, 3, XBYTE[REG_Disp_PvY0L]);
  WRITE8(pvY0, 2, XBYTE[REG_Disp_PvY0H]);
  WRITE8(pvY0, 1, 0);
  WRITE8(pvY0, 0, 0);
  WRITE8(pvY1, 3, XBYTE[REG_Disp_PvY1L]);
  WRITE8(pvY1, 2, XBYTE[REG_Disp_PvY1H]);
  WRITE8(pvY1, 1, 0);
  WRITE8(pvY1, 0, 0);
  srcW = pvX1 - pvX0 + 1;
  srcH = pvY1 - pvY0 + 1;

  WRITE8(pvH0, 3, XBYTE[REG_Disp_PvH0L]);
  WRITE8(pvH0, 2, XBYTE[REG_Disp_PvH0H]);
  WRITE8(pvH0, 1, 0);
  WRITE8(pvH0, 0, 0);
  WRITE8(pvH1, 3, XBYTE[REG_Disp_PvH1L]);
  WRITE8(pvH1, 2, XBYTE[REG_Disp_PvH1H]);
  WRITE8(pvH1, 1, 0);
  WRITE8(pvH1, 0, 0);
  WRITE8(pvV0, 3, XBYTE[REG_Disp_PvV0L]);
  WRITE8(pvV0, 2, XBYTE[REG_Disp_PvV0H]);
  WRITE8(pvV0, 1, 0);
  WRITE8(pvV0, 0, 0);
  WRITE8(pvV1, 3, XBYTE[REG_Disp_PvV1L]);
  WRITE8(pvV1, 2, XBYTE[REG_Disp_PvV1H]);
  WRITE8(pvV1, 1, 0);
  WRITE8(pvV1, 0, 0);
  dispW = pvH1 - pvH0 + 1;
  dispH = pvV1 - pvV0 + 1;
  
  printf("\n");
  printf("pvx0 = %ld, pvy0 = %ld, pvx1 = %ld, pvy1 = %ld\n",pvX0,pvY0,pvX1,pvY1);
  printf("pvh0 = %ld, pvv0 = %ld, pvh1 = %ld, pvv1 = %ld\n",pvH0,pvV0,pvH1,pvV1);
  printf("IMG SRC Hsize = %ld, Vsize = %ld\n",srcW,srcH);
  printf("IMG DST Hsize = %ld, Vsize = %ld\n",dispW,dispH);

  WRITE8(pvX0, 3, XBYTE[REG_Disp_OsdX0L]);
  WRITE8(pvX0, 2, XBYTE[REG_Disp_OsdX0H]);
  WRITE8(pvX0, 1, 0);
  WRITE8(pvX0, 0, 0);
  WRITE8(pvX1, 3, XBYTE[REG_Disp_OsdX1L]);
  WRITE8(pvX1, 2, XBYTE[REG_Disp_OsdX1H]);
  WRITE8(pvX1, 1, 0);
  WRITE8(pvX1, 0, 0);
  WRITE8(pvY0, 3, XBYTE[REG_Disp_OsdY0L]);
  WRITE8(pvY0, 2, XBYTE[REG_Disp_OsdY0H]);
  WRITE8(pvY0, 1, 0);
  WRITE8(pvY0, 0, 0);
  WRITE8(pvY1, 3, XBYTE[REG_Disp_OsdY1L]);
  WRITE8(pvY1, 2, XBYTE[REG_Disp_OsdY1H]);
  WRITE8(pvY1, 1, 0);
  WRITE8(pvY1, 0, 0);
  srcW = pvX1 - pvX0 + 1;
  srcH = pvY1 - pvY0 + 1;

  WRITE8(pvH0, 3, XBYTE[REG_Disp_OsdH0L]);
  WRITE8(pvH0, 2, XBYTE[REG_Disp_OsdH0H]);
  WRITE8(pvH0, 1, 0);
  WRITE8(pvH0, 0, 0);
  WRITE8(pvH1, 3, XBYTE[REG_Disp_OsdH1L]);
  WRITE8(pvH1, 2, XBYTE[REG_Disp_OsdH1H]);
  WRITE8(pvH1, 1, 0);
  WRITE8(pvH1, 0, 0);
  WRITE8(pvV0, 3, XBYTE[REG_Disp_OsdV0L]);
  WRITE8(pvV0, 2, XBYTE[REG_Disp_OsdV0H]);
  WRITE8(pvV0, 1, 0);
  WRITE8(pvV0, 0, 0);
  WRITE8(pvV1, 3, XBYTE[REG_Disp_OsdV1L]);
  WRITE8(pvV1, 2, XBYTE[REG_Disp_OsdV1H]);
  WRITE8(pvV1, 1, 0);
  WRITE8(pvV1, 0, 0);
  dispW = pvH1 - pvH0 + 1;
  dispH = pvV1 - pvV0 + 1;
  
  printf("\n");
  printf("osdx0 = %ld, osdy0 = %ld, osdx1 = %ld, osdy1 = %ld\n",pvX0,pvY0,pvX1,pvY1);
  printf("osdh0 = %ld, osdv0 = %ld, osdh1 = %ld, osdv1 = %ld\n",pvH0,pvV0,pvH1,pvV1);
  printf("OSD SRC Hsize = %ld, Vsize = %ld\n",srcW,srcH);
  printf("OSD DST Hsize = %ld, Vsize = %ld\n",dispW,dispH);

}

static void cmdDispImgUpd(void)
{
  printf("imgupd\n");
  XBYTE[REG_Disp_UpdPvOsd] = 0x01;
}
static void cmdDispOsdUpd(void)
{
  printf("osdupd\n");
  XBYTE[REG_Disp_UpdPvOsd] = 0x02;
}

static void cmdDispImgEnSet(UINT8 en)
{
	printf("img");
	
	if (en) {
		printf("on\n");
		XBYTE[REG_Disp_DispEn_Flip] |= 0x02;
	} else {
		printf("off\n");
		XBYTE[REG_Disp_DispEn_Flip] &= ~0x02;
	}
	
	XBYTE[REG_Disp_UpdPvOsd] = 0x01;
}

static void cmdDispOsdEnSet(UINT8 en)
{
	printf("osd");
	
	if (en) {
		printf("on\n");
		XBYTE[REG_Disp_DispEn_Flip] |= 0x04;
	} else {
		printf("off\n");
		XBYTE[REG_Disp_DispEn_Flip] &= ~0x04;
	}
	
	XBYTE[REG_Disp_UpdPvOsd] = 0x02;
}

static void cmdDispImgXy(UINT16 x0,UINT16 x1,UINT16 y0,UINT16 y1)
{
	XBYTE[REG_Disp_PvX0L] = READ8(x0, 1);
	XBYTE[REG_Disp_PvX0H] = READ8(x0, 0);
	XBYTE[REG_Disp_PvX1L] = READ8(x1, 1);
	XBYTE[REG_Disp_PvX1H] = READ8(x1, 0);
	XBYTE[REG_Disp_PvY0L] = READ8(y0, 1);
	XBYTE[REG_Disp_PvY0H] = READ8(y0, 0);
	XBYTE[REG_Disp_PvY1L] = READ8(y1, 1);
	XBYTE[REG_Disp_PvY1H] = READ8(y1, 0);
}

static void cmdDispImgHv(UINT16 h0,UINT16 h1,UINT16 v0,UINT16 v1)
{
	XBYTE[REG_Disp_PvH0L] = READ8(h0, 1);
	XBYTE[REG_Disp_PvH0H] = READ8(h0, 0);
	XBYTE[REG_Disp_PvH1L] = READ8(h1, 1);
	XBYTE[REG_Disp_PvH1H] = READ8(h1, 0);
	XBYTE[REG_Disp_PvV0L] = READ8(v0, 1);
	XBYTE[REG_Disp_PvV0H] = READ8(v0, 0);
	XBYTE[REG_Disp_PvV1L] = READ8(v1, 1);
	XBYTE[REG_Disp_PvV1H] = READ8(v1, 0);
	
	HAL_DispPnlInitHvSet(h0, h1, v0, v1);
}

static void cmdDispOsdXy(UINT16 x0,UINT16 x1,UINT16 y0,UINT16 y1)
{
	XBYTE[REG_Disp_OsdX0L] = READ8(x0, 1);
	XBYTE[REG_Disp_OsdX0H] = READ8(x0, 0);
	XBYTE[REG_Disp_OsdX1L] = READ8(x1, 1);
	XBYTE[REG_Disp_OsdX1H] = READ8(x1, 0);
	XBYTE[REG_Disp_OsdY0L] = READ8(y0, 1);
	XBYTE[REG_Disp_OsdY0H] = READ8(y0, 0);
	XBYTE[REG_Disp_OsdY1L] = READ8(y1, 1);
	XBYTE[REG_Disp_OsdY1H] = READ8(y1, 0);
}

static void cmdDispOsdHv(UINT16 h0,UINT16 h1,UINT16 v0,UINT16 v1)
{
	XBYTE[REG_Disp_OsdH0L] = READ8(h0, 1);
	XBYTE[REG_Disp_OsdH0H] = READ8(h0, 0);
	XBYTE[REG_Disp_OsdH1L] = READ8(h1, 1);
	XBYTE[REG_Disp_OsdH1H] = READ8(h1, 0);
	XBYTE[REG_Disp_OsdV0L] = READ8(v0, 1);
	XBYTE[REG_Disp_OsdV0H] = READ8(v0, 0);
	XBYTE[REG_Disp_OsdV1L] = READ8(v1, 1);
	XBYTE[REG_Disp_OsdV1H] = READ8(v1, 0);
}


static void cmdDispPnlSet(UINT16 vline,UINT16 hpixel,UINT8 vsyncos,UINT8 hsyncos,UINT8 vsyncw,UINT8 hsyncw,
                   UINT16 pnlh0,UINT16 pnlh1,UINT16 pnlv0,UINT16 pnlv1)
{
	XBYTE[REG_Disp_VlineL]  = READ8(vline, 1);
	XBYTE[REG_Disp_VlineH]  = READ8(vline, 0);
	XBYTE[REG_Disp_HpixelL] = READ8(hpixel, 1);
	XBYTE[REG_Disp_HpixelH] = READ8(hpixel, 0);
	
	XBYTE[REG_Disp_Vsyncos] = vsyncos;
	XBYTE[REG_Disp_Hsyncos] = hsyncos;
	XBYTE[REG_Disp_Vsyncw]  = vsyncw;
	XBYTE[REG_Disp_Hsyncw]  = hsyncw;
	
	XBYTE[REG_Disp_PnlH0L]  = READ8(pnlh0, 1);
	XBYTE[REG_Disp_PnlH0H]  = READ8(pnlh0, 0);
	XBYTE[REG_Disp_PnlH1L]  = READ8(pnlh1, 1);
	XBYTE[REG_Disp_PnlH1H]  = READ8(pnlh1, 0);
	XBYTE[REG_Disp_PnlV0L]  = READ8(pnlv0, 1);
	XBYTE[REG_Disp_PnlV0H]  = READ8(pnlv0, 0);
	XBYTE[REG_Disp_PnlV1L]  = READ8(pnlv1, 1);
	XBYTE[REG_Disp_PnlV1H]  = READ8(pnlv1, 0);
}

void cmdDispTest(UINT8 *ptr)
{
    UINT32 arg[8] = {0};

    cmdArgGet(&ptr, arg, 8, 1);
	
    if (strncmp((UINT8*)arg[0], "rptsize", 6) == 0) {
        cmdDispRptSize();
    } else if (strncmp((UINT8*)arg[0], "imgxy", 5) == 0) {
        cmdDispImgXy(arg[1], arg[2], arg[3], arg[4]);
    } else if (strncmp((UINT8*)arg[0], "imghv", 5) == 0) {
        cmdDispImgHv(arg[1], arg[2], arg[3], arg[4]);
    } else if (strncmp((UINT8*)arg[0], "osdxy", 5) == 0) {
        cmdDispOsdXy(arg[1], arg[2], arg[3], arg[4]);
    } else if (strncmp((UINT8*)arg[0], "osdhv", 5) == 0) {
        cmdDispOsdHv(arg[1], arg[2], arg[3], arg[4]);
    } else if (strncmp((UINT8*)arg[0], "imgupd", 6) == 0) {
        cmdDispImgUpd();
    } else if (strncmp((UINT8*)arg[0], "osdupd", 6) == 0) {
        cmdDispOsdUpd();
    } else if (strncmp((UINT8*)arg[0], "imgon", 6) == 0) {
        cmdDispImgEnSet(1);
    } else if (strncmp((UINT8*)arg[0], "imgoff", 6) == 0) {
        cmdDispImgEnSet(0);
    } else if (strncmp((UINT8*)arg[0], "osdon", 6) == 0) {
        cmdDispOsdEnSet(1);
    } else if (strncmp((UINT8*)arg[0], "osdoff", 6) == 0) {
        cmdDispOsdEnSet(0);
	} else {
#if 0
		/* cmd line help information */
		printf("Usage: disp [control]\n");
		printf("control parameter:\n");
		printf("rptsize\n");
		printf("imgxy\n");
		printf("imghv\n");
		printf("osdxy\n");
		printf("osdhv\n");
		printf("imgupd\n");
		printf("osdupd\n");
		printf("imgon\n");
		printf("imgoff\n");
		printf("osdon\n");
		printf("osdoff\n");
#endif
	}
}
#else
static UINT8 code cmdDispRsv;
#endif /* DISP_TEST */
