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
#ifndef __RSC_ID_H__
#define __RSC_ID_H__


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define SP1K_RSC_ID_LSC      0x07
#define SP1K_RSC_ID_GAMMA    0x08
#define SP1K_RSC_ID_CDSPPAR  0x09
#define SP1K_RSC_ID_WDR      0x3b
#define SP1K_RSC_ID_PVPAR    0x3c
#define SP1K_RSC_ID_STILLPAR 0x3e
#define SP1K_RSC_ID_MILSCCAL 0x42

/* Still CDSP Edge Enhan. Strong{s,m,w}, Middle{s,m,w}, Weak{s,m,w} */
#define SP1K_RSC_ID_STILLPAR_MIDDLE 0xba
#define SP1K_RSC_ID_STILLPAR_WEAK 0xbb
#define SP1K_RSC_ID_STILLPAR_STRONG 0xbc

#define SP1K_RSC_ID_STILL_960P_ZOOM_FACTOR    0xe3
#define SP1K_RSC_ID_STILL_960P_ZOOM_REG       0xe4
#define SP1K_RSC_ID_STILL_960P_ZOOM_REG_VALUE 0xe5

#define SP1K_RSC_ID_VIDEO_720P_ZOOM_FACTOR    0xbd
#define SP1K_RSC_ID_VIDEO_720P_ZOOM_REG       0xbe
#define SP1K_RSC_ID_VIDEO_720P_ZOOM_REG_VALUE 0xbf

/* LCD */
#define SP1K_RSC_ID_STILL_VGA_43              SP1K_RSC_ID_VIDEO_VGA
#define SP1K_RSC_ID_STILL_VGA_169             0xfc
#define SP1K_RSC_ID_VIDEO_HD_88               0xe6
#define SP1K_RSC_ID_VIDEO_HD_68               0xe7
#define SP1K_RSC_ID_VIDEO_D1                  0xe8
#define SP1K_RSC_ID_VIDEO_VGA                 0xe9
#define SP1K_RSC_ID_VIDEO_QVGA                0xea
#define SP1K_RSC_ID_VIDEO_FHD                 0xeb
#define SP1K_RSC_ID_VIDEO_VGA_HIGH            0xec

/* NTSC */
#define SP1K_RSC_ID_STILL_VGA_43_NTSC              SP1K_RSC_ID_VIDEO_VGA_NTSC
#define SP1K_RSC_ID_STILL_VGA_169_NTSC             0xfd
#define SP1K_RSC_ID_VIDEO_HD_88_NTSC               0xed
#define SP1K_RSC_ID_VIDEO_HD_68_NTSC               0xee
#define SP1K_RSC_ID_VIDEO_D1_NTSC                  0xef
#define SP1K_RSC_ID_VIDEO_VGA_NTSC                 0xf0
#define SP1K_RSC_ID_VIDEO_QVGA_NTSC                0xf1
#define SP1K_RSC_ID_VIDEO_FHD_NTSC                 0xf2
#define SP1K_RSC_ID_VIDEO_VGA_HIGH_NTSC            0xf3

/* PAL */
#define SP1K_RSC_ID_STILL_VGA_43_PAL              SP1K_RSC_ID_VIDEO_VGA_PAL
#define SP1K_RSC_ID_STILL_VGA_169_PAL             0xfe
#define SP1K_RSC_ID_VIDEO_HD_88_PAL               0xf4
#define SP1K_RSC_ID_VIDEO_HD_68_PAL               0xf5
#define SP1K_RSC_ID_VIDEO_D1_PAL                  0xf6
#define SP1K_RSC_ID_VIDEO_VGA_PAL                 0xf7
#define SP1K_RSC_ID_VIDEO_QVGA_PAL                0xf8
#define SP1K_RSC_ID_VIDEO_FHD_PAL                 0xf9
#define SP1K_RSC_ID_VIDEO_VGA_HIGH_PAL            0xfa

/* PCCAM */
#define SP1K_RSC_ID_PCCAM_HD_88               SP1K_RSC_ID_VIDEO_HD_88
#define SP1K_RSC_ID_PCCAM_HD_68               SP1K_RSC_ID_VIDEO_HD_68
#define SP1K_RSC_ID_PCCAM_D1                  SP1K_RSC_ID_VIDEO_D1
#define SP1K_RSC_ID_PCCAM_VGA                 SP1K_RSC_ID_VIDEO_VGA
#define SP1K_RSC_ID_PCCAM_QVGA                SP1K_RSC_ID_VIDEO_QVGA
#define SP1K_RSC_ID_PCCAM_QQVGA               0xff//0xfb
#define SP1K_RSC_ID_PCCAM_FHD                 SP1K_RSC_ID_VIDEO_FHD
#define SP1K_RSC_ID_PCCAM_VGA_HIGH            SP1K_RSC_ID_VIDEO_VGA_HIGH





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
#endif 

