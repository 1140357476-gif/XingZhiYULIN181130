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
#ifndef _sensor_cdsp_h_
#define _sensor_cdsp_h_

#if 0  // mattwang remove.

#if (SENSOR_TYPE == Sensor_PA6371_60FPS)

#define Sensor_Preview_HSize 320
#define Sensor_Preview_VSize 240
#define Sensor_Preview_HOffset 2
#define Sensor_Preview_VOffset 2

#define Sensor_Preview_HReshen 0
#define Sensor_Preview_Hfall 30
#define Sensor_Preview_Hrise 40
#define Sensor_Preview_VReshen 0
#define Sensor_Preview_Vfall 5
#define Sensor_Preview_Vrise 4

#define Prev_Hsync_Inverse 1
#define Prev_Vsync_Inverse 0

#define Sensor_Snap_HSize 320
#define Sensor_Snap_VSize 240
#define Sensor_Snap_HOffset 2
#define Sensor_Snap_VOffset 2

#define Sensor_Snap_HReshen 0
#define Sensor_Snap_Hfall 2
#define Sensor_Snap_Hrise 4
#define Sensor_Snap_VReshen 0
#define Sensor_Snap_Vfall 2
#define Sensor_Snap_Vrise 3

#define Snap_Hsync_Inverse 1
#define Snap_Vsync_Inverse 0

#define Sensor_Preview_LineTotal 460//786
#define Sensor_Preview_FrameTotal 248//508

#define Sensor_Snap_LineTotal 460
#define Sensor_Snap_FrameTotal 248

#define Sensor_Slave_Address    0x80
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x00
#define Sensor_Snap_BayerPattern       0x00

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (160)
#define LC_centy (120)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 1
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 1

#define LC_Preview_stepfator1 0
#define LC_Preview_stepfator2 7
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 0
#define LC_Snap_stepfator2 7
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif
#if (SENSOR_TYPE == Sensor_PA6371)

#define Sensor_Preview_HSize 640
#define Sensor_Preview_VSize 480
#define Sensor_Preview_HOffset 4
#define Sensor_Preview_VOffset 4

#define Sensor_Preview_HReshen 0
#define Sensor_Preview_Hfall 30
#define Sensor_Preview_Hrise 40
#define Sensor_Preview_VReshen 0
#define Sensor_Preview_Vfall 5
#define Sensor_Preview_Vrise 4

#define Prev_Hsync_Inverse 1
#define Prev_Vsync_Inverse 0

#define Sensor_Snap_HSize 640
#define Sensor_Snap_VSize 480
#define Sensor_Snap_HOffset 4
#define Sensor_Snap_VOffset 4

#define Sensor_Snap_HReshen 0
#define Sensor_Snap_Hfall 2
#define Sensor_Snap_Hrise 4
#define Sensor_Snap_VReshen 0
#define Sensor_Snap_Vfall 2
#define Sensor_Snap_Vrise 3

#define Snap_Hsync_Inverse 1
#define Snap_Vsync_Inverse 0

#define Sensor_Preview_LineTotal 786
#define Sensor_Preview_FrameTotal 508

#define Sensor_Snap_LineTotal 786
#define Sensor_Snap_FrameTotal 508

#define Sensor_Slave_Address    0x80
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x00
#define Sensor_Snap_BayerPattern       0x00

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (320)
#define LC_centy (240)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 1
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 1

#define LC_Preview_stepfator1 0
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 0
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#define Sensor_EV10_Exposure_Index 100


#endif

#if (SENSOR_TYPE == Sensor_SOI768)

#if 0
#define Sensor_Preview_HSize 640
#define Sensor_Preview_VSize 480
#define Sensor_Preview_HOffset 44
#define Sensor_Preview_VOffset 5

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall 30
#define Sensor_Preview_Hrise 38
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 2
#define Sensor_Preview_Vrise 3

#define Prev_Hsync_Inverse 1
#define Prev_Vsync_Inverse 0

#define Sensor_Snap_HSize 640
#define Sensor_Snap_VSize 480

#define Sensor_Snap_HOffset 44
#define Sensor_Snap_VOffset 5

#define Sensor_Snap_HReshen 1
#define Sensor_Snap_Hfall 30
#define Sensor_Snap_Hrise 38
#define Sensor_Snap_VReshen 1
#define Sensor_Snap_Vfall 2
#define Sensor_Snap_Vrise 3

#define Snap_Hsync_Inverse 1
#define Snap_Vsync_Inverse 0
#else
#define Sensor_Preview_HSize 640
#define Sensor_Preview_VSize 480

#define Sensor_Preview_HOffset 50
#define Sensor_Preview_VOffset 5

#define Sensor_Preview_HReshen 0
#define Sensor_Preview_Hfall 30
#define Sensor_Preview_Hrise 40
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 2
#define Sensor_Preview_Vrise 3

#define Prev_Hsync_Inverse 1
#define Prev_Vsync_Inverse 0

#define Sensor_Snap_HSize 640
#define Sensor_Snap_VSize 480

#define Sensor_Snap_HOffset 48
#define Sensor_Snap_VOffset 5

#define Sensor_Snap_HReshen 0
#define Sensor_Snap_Hfall 2
#define Sensor_Snap_Hrise 4
#define Sensor_Snap_VReshen 1
#define Sensor_Snap_Vfall 2
#define Sensor_Snap_Vrise 3

#define Snap_Hsync_Inverse 1
#define Snap_Vsync_Inverse 0

#endif
#define Sensor_Preview_LineTotal 784
#define Sensor_Preview_FrameTotal 510

#define Sensor_Snap_LineTotal 784
#define Sensor_Snap_FrameTotal 510

#define Sensor_Slave_Address    0x42
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x02
#define Sensor_Snap_BayerPattern       0x02

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (320)
#define LC_centy (240)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 1
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 1

#define LC_Preview_stepfator1 0
#define LC_Preview_stepfator2 7
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 0
#define LC_Snap_stepfator2 7
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7
#endif

#if (SENSOR_TYPE == Sensor_OV7710)
#define Sensor_Preview_HSize 640
#define Sensor_Preview_VSize 480
#define Sensor_Preview_HOffset 280//128
#define Sensor_Preview_VOffset 24

#define Sensor_Preview_HReshen 0
#define Sensor_Preview_Hfall 39
#define Sensor_Preview_Hrise 40
#define Sensor_Preview_VReshen 0
#define Sensor_Preview_Vfall 2
#define Sensor_Preview_Vrise 3
#define Prev_Hsync_Inverse 0
#define Prev_Vsync_Inverse 0


#define Sensor_Snap_HSize 640
#define Sensor_Snap_VSize 480
#define Sensor_Snap_HOffset 280//128
#define Sensor_Snap_VOffset 24

#define Sensor_Snap_HReshen 0
#define Sensor_Snap_Hfall 2
#define Sensor_Snap_Hrise 4
#define Sensor_Snap_VReshen 0
#define Sensor_Snap_Vfall 2
#define Sensor_Snap_Vrise 3
#define Snap_Hsync_Inverse 0
#define Snap_Vsync_Inverse 0

#define Sensor_Preview_LineTotal 780
#define Sensor_Preview_FrameTotal 525

#define Sensor_Snap_LineTotal 780
#define Sensor_Snap_FrameTotal 525
#define Sensor_Slave_Address    0x60
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x00
#define Sensor_Snap_BayerPattern       0x00

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (320)
#define LC_centy (240)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 1
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 1

#define LC_Preview_stepfator1 1
#define LC_Preview_stepfator2 4
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 1
#define LC_Snap_stepfator2 4
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif

#if (SENSOR_TYPE == Sensor_OV3630)
#define Sensor_Preview_HSize 1024
#define Sensor_Preview_VSize 768
#define Sensor_Preview_HOffset 136//124//112
#define Sensor_Preview_VOffset (6+1)

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall 39
#define Sensor_Preview_Hrise 40
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 2
#define Sensor_Preview_Vrise 3
#define Prev_Hsync_Inverse 0
#define Prev_Vsync_Inverse 1


#define Sensor_Snap_HSize 2048
#define Sensor_Snap_VSize 1536
#define Sensor_Snap_HOffset (270+10)
#define Sensor_Snap_VOffset (15)

#define Sensor_Snap_HReshen 1
#define Sensor_Snap_Hfall 2
#define Sensor_Snap_Hrise 4
#define Sensor_Snap_VReshen 1
#define Sensor_Snap_Vfall 2
#define Sensor_Snap_Vrise 3
#define Snap_Hsync_Inverse 0
#define Snap_Vsync_Inverse 1

#define Sensor_Preview_LineTotal 1152
#define Sensor_Preview_FrameTotal 790

#define Sensor_Snap_LineTotal 2320
#define Sensor_Snap_FrameTotal 1567
#define Sensor_Slave_Address    0x60
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x02
#define Sensor_Snap_BayerPattern       0x02

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (1024)
#define LC_centy (768)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 2
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 2

#define LC_Preview_stepfator1 2
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 2
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif
#if (SENSOR_TYPE == Sensor_GC1303)
#define Sensor_Preview_HSize 640
#define Sensor_Preview_VSize 480
#define Sensor_Preview_HOffset 15
#define Sensor_Preview_VOffset 21

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall 3
#define Sensor_Preview_Hrise 4
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 2
#define Sensor_Preview_Vrise 3
#define Prev_Hsync_Inverse 0
#define Prev_Vsync_Inverse 1


#define Sensor_Snap_HSize 1280
#define Sensor_Snap_VSize 1024
#define Sensor_Snap_HOffset 5
#define Sensor_Snap_VOffset 5

#define Sensor_Snap_HReshen 0
#define Sensor_Snap_Hfall 2
#define Sensor_Snap_Hrise 4
#define Sensor_Snap_VReshen 1
#define Sensor_Snap_Vfall 2
#define Sensor_Snap_Vrise 3
#define Snap_Hsync_Inverse 1
#define Snap_Vsync_Inverse 1

#define Sensor_Preview_LineTotal  1739//1081
#define Sensor_Preview_FrameTotal  608

#define Sensor_Snap_LineTotal 1500
#define Sensor_Snap_FrameTotal 1064
#define Sensor_Slave_Address    0x50
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x00
#define Sensor_Snap_BayerPattern       0x00

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (640)
#define LC_centy (512)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 2
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 2

#define LC_Preview_stepfator1 1
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 1
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif

#if (SENSOR_TYPE == Sensor_OV5620)
#define Sensor_Preview_HSize 1280
#define Sensor_Preview_VSize 480
#define Sensor_Preview_HOffset 273
#define Sensor_Preview_VOffset 16

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall 2
#define Sensor_Preview_Hrise 3
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 2
#define Sensor_Preview_Vrise 3


#define Sensor_Snap_HSize 2592
#define Sensor_Snap_VSize 1944

#define Sensor_Snap_HOffset 273
#define Sensor_Snap_VOffset 20

#define Sensor_Snap_HReshen 1
#define Sensor_Snap_Hfall 2
#define Sensor_Snap_Hrise 4
#define Sensor_Snap_VReshen 1
#define Sensor_Snap_Vfall 1
#define Sensor_Snap_Vrise 2


#define Sensor_Preview_LineTotal 1609
#define Sensor_Preview_FrameTotal 498

#define Sensor_Snap_LineTotal 3252
#define Sensor_Snap_FrameTotal 1987
#define Sensor_Slave_Address    0x60
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x0
#define Sensor_Snap_BayerPattern       0x0

#define Sensor_EV10_Exposure_Index 100


#define LC_centx (1296)
#define LC_centy (972)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 2
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 4

#define LC_Preview_stepfator1 2
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 2
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif

#if (SENSOR_TYPE == Sensor_OV5620_60FPS)
#define Sensor_Preview_HSize 320
#define Sensor_Preview_VSize 240
#define Sensor_Preview_HOffset 69
#define Sensor_Preview_VOffset 16

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall 52
#define Sensor_Preview_Hrise 54
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall (2+8)
#define Sensor_Preview_Vrise (3+8)


#define Sensor_Snap_HSize 2592
#define Sensor_Snap_VSize 1944

#define Sensor_Snap_HOffset 273
#define Sensor_Snap_VOffset 20

#define Sensor_Snap_HReshen 1
#define Sensor_Snap_Hfall 2
#define Sensor_Snap_Hrise 4
#define Sensor_Snap_VReshen 1
#define Sensor_Snap_Vfall 1
#define Sensor_Snap_Vrise 2


#define Sensor_Preview_LineTotal 390
#define Sensor_Preview_FrameTotal 257

#define Sensor_Snap_LineTotal 3252
#define Sensor_Snap_FrameTotal 1987
#define Sensor_Slave_Address    0x60
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x0
#define Sensor_Snap_BayerPattern       0x0

#define Sensor_EV10_Exposure_Index 100


#define LC_centx (1296)
#define LC_centy (972)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 8
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 8

#define LC_Preview_stepfator1 2
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 2
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif

#if (SENSOR_TYPE == Sensor_OV5610)
#define Sensor_Preview_HSize 1280
#define Sensor_Preview_VSize 480
#define Sensor_Preview_HOffset 304
#define Sensor_Preview_VOffset 4

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall 298
#define Sensor_Preview_Hrise 300
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 1
#define Sensor_Preview_Vrise 2


#define Sensor_Snap_HSize 2592
#define Sensor_Snap_VSize 1944

#define Sensor_Snap_HOffset 264
#define Sensor_Snap_VOffset 10


#define Sensor_Snap_HReshen 0
#define Sensor_Snap_Hfall 298
#define Sensor_Snap_Hrise 300
#define Sensor_Snap_VReshen 1
#define Sensor_Snap_Vfall 1
#define Sensor_Snap_Vrise 2

#define Sensor_Preview_LineTotal 1632
#define Sensor_Preview_FrameTotal 490
#define Sensor_Snap_LineTotal 3000
#define Sensor_Snap_FrameTotal 2000
#define Sensor_Slave_Address    0x60
#define Sensor_SSC_Freq         0x03


#define LC_centx (1296)
#define LC_centy (972)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 2
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 4

#define LC_Preview_stepfator1 2
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 2
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif

#if (SENSOR_TYPE == Sensor_GC0316)

#define Sensor_Preview_HSize 640
#define Sensor_Preview_VSize 480

#define Sensor_Preview_HOffset  140//192
#define Sensor_Preview_VOffset 2

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall 56
#define Sensor_Preview_Hrise 60
#define Sensor_Preview_VReshen 0
#define Sensor_Preview_Vfall 4
#define Sensor_Preview_Vrise 5

#define Prev_Hsync_Inverse 0
#define Prev_Vsync_Inverse 1

#define Sensor_Snap_HSize 640
#define Sensor_Snap_VSize 480

#define Sensor_Snap_HOffset 140
#define Sensor_Snap_VOffset 2

#define Sensor_Snap_HReshen 1
#define Sensor_Snap_Hfall 56
#define Sensor_Snap_Hrise 60
#define Sensor_Snap_VReshen 0
#define Sensor_Snap_Vfall 4
#define Sensor_Snap_Vrise 5

#define Snap_Hsync_Inverse 0
#define Snap_Vsync_Inverse 1

#define Sensor_Preview_LineTotal 789
#define Sensor_Preview_FrameTotal 504
#define Sensor_Snap_LineTotal 789
#define Sensor_Snap_FrameTotal 504

#define Sensor_Slave_Address    0x42
#define Sensor_SSC_Freq         0x03

#define Sensor_Preview_BayerPattern    0x00
#define Sensor_Snap_BayerPattern       0x00

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (320)
#define LC_centy (240)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 1
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 1

#define LC_Preview_stepfator1 0
#define LC_Preview_stepfator2 9
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 0
#define LC_Snap_stepfator2 9
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif

#if (SENSOR_TYPE == Sensor_MI5100)
#define Sensor_Preview_HSize 640
#define Sensor_Preview_VSize 480
#define Sensor_Preview_HOffset 6
#define Sensor_Preview_VOffset 4

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall  2
#define Sensor_Preview_Hrise 4
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 1
#define Sensor_Preview_Vrise 2
#define Prev_Hsync_Inverse 1
#define Prev_Vsync_Inverse 1

#define Sensor_Snap_HSize 2592
#define Sensor_Snap_VSize 1944

#define Sensor_Snap_HOffset 2
#define Sensor_Snap_VOffset 2


#define Sensor_Snap_HReshen 1
#define Sensor_Snap_Hfall 1
#define Sensor_Snap_Hrise 2
#define Sensor_Snap_VReshen 1
#define Sensor_Snap_Vfall 1
#define Sensor_Snap_Vrise 2
#define Snap_Hsync_Inverse 1
#define Snap_Vsync_Inverse 1


#define Sensor_Preview_LineTotal 1548
#define Sensor_Preview_FrameTotal 516
#define Sensor_Snap_LineTotal 3552
#define Sensor_Snap_FrameTotal 2000
#define Sensor_Slave_Address    0xBA
#define Sensor_SSC_Freq         0x04

#define Sensor_Preview_BayerPattern    0x03
#define Sensor_Snap_BayerPattern       0x03

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (1296)
#define LC_centy (972)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 4
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 4

#define LC_Preview_stepfator1 2
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 2
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 2
#endif

#if (SENSOR_TYPE == Sensor_MI1320)
#if 0
#define Sensor_Preview_HSize 640 /*650 - 10*/
#define Sensor_Preview_VSize 480
#define Sensor_Preview_HOffset 191/*15.8us/83.3ns = 190 +5*/
#define Sensor_Preview_VOffset 24/*30+19*/

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall  50
#define Sensor_Preview_Hrise 60
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 8
#define Sensor_Preview_Vrise 12

#define Prev_Hsync_Inverse 0
#define Prev_Vsync_Inverse 0

#define Sensor_Snap_HSize 1280
#define Sensor_Snap_VSize 1024
#define Sensor_Snap_HOffset 281/*293*/
#define Sensor_Snap_VOffset 4


#define Sensor_Snap_HReshen 1
#define Sensor_Snap_Hfall 120
#define Sensor_Snap_Hrise 140
#define Sensor_Snap_VReshen 0
#define Sensor_Snap_Vfall 1
#define Sensor_Snap_Vrise 2

#define Snap_Hsync_Inverse 0
#define Snap_Vsync_Inverse 0


#define Sensor_Preview_LineTotal 840 /*70us/83.3ns = 840*/
#define Sensor_Preview_FrameTotal 548  /*38.4ms/70us = 548*/
#define Sensor_Snap_LineTotal 1588/*132.29us/83.3ns = 1588*/
#define Sensor_Snap_FrameTotal 1028/*136ms/132.29us = 1028*/
#define Sensor_Slave_Address    0x90
#define Sensor_SSC_Freq         0x04

#define Sensor_Preview_BayerPattern    0x03
#define Sensor_Snap_BayerPattern       0x03

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (320)
#define LC_centy (240)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 4
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 4

#define LC_Preview_stepfator1 2
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 2
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#else

#define Sensor_Preview_HSize 640
#define Sensor_Preview_VSize 480/*480*/
#define Sensor_Preview_HOffset 194
#define Sensor_Preview_VOffset 24/*35*/

#define Sensor_Preview_HReshen 1
#define Sensor_Preview_Hfall  40/*40*/
#define Sensor_Preview_Hrise 50/*50*/
#define Sensor_Preview_VReshen 1
#define Sensor_Preview_Vfall 8
#define Sensor_Preview_Vrise 10

#define Prev_Hsync_Inverse 0
#define Prev_Vsync_Inverse 0

#define Sensor_Snap_HSize 1280
#define Sensor_Snap_VSize 1024
#define Sensor_Snap_HOffset 288/*288*/
#define Sensor_Snap_VOffset 8


#define Sensor_Snap_HReshen 1
#define Sensor_Snap_Hfall 80
#define Sensor_Snap_Hrise 90
#define Sensor_Snap_VReshen 0
#define Sensor_Snap_Vfall 1
#define Sensor_Snap_Vrise 2

#define Snap_Hsync_Inverse 0
#define Snap_Vsync_Inverse 0


#define Sensor_Preview_LineTotal 835 /*70us/83.3ns = 840*/
#define Sensor_Preview_FrameTotal 552  /*38.4ms/70us = 548*/
#define Sensor_Snap_LineTotal 1566/*132.29us/83.3ns = 1588*/
#define Sensor_Snap_FrameTotal 1048/*136ms/132.29us = 1028*/
#define Sensor_Slave_Address    0x90
#define Sensor_SSC_Freq         0x04

#define Sensor_Preview_BayerPattern    0x02
#define Sensor_Snap_BayerPattern       0x02

#define Sensor_EV10_Exposure_Index 100

#define LC_centx (320)
#define LC_centy (240)

#define LC_Preview_xmoffset 0
#define LC_Preview_Xminc 4
#define LC_Preview_ymoffset 0
#define LC_Preview_Yminc 4

#define LC_Preview_stepfator1 2
#define LC_Preview_stepfator2 8
#define LC_Preview_yuvlens 0
#define LC_Preview_chmode 7

#define LC_Snap_xmoffset 0
#define LC_Snap_Xminc 1
#define LC_Snap_ymoffset 0
#define LC_Snap_Yminc 1

#define LC_Snap_stepfator1 2
#define LC_Snap_stepfator2 8
#define LC_Snap_yuvlens 0
#define LC_Snap_chmode 7

#endif
#endif
#endif

#endif

