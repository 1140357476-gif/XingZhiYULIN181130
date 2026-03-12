/*************************************************************************
 *
 *	Copyright (c) 2007 Sunplus Technology Co., Ltd.
 *
 * Module Name:
 *
 *		pb_libver.c
 *
 * Abstract:
 *
 *		Module related to playback lib version
 *
 * Environment:
 *
 *		Keil C51 Compiler
 *
 * Revision History:
 *
 *		07/24/2007		Phil	created
 *
 * Change Log:
 *	v01, 20111024 wangjg
 *		a. initialize version
 *  	v02, 20111031 renhy
 *		a. fixed pb crop bug
 *	v03, 20111122 xu.gao
 *		a. fixed pb memo bug
 *	v04, 20111202 delei.gu
 *		a. modify dispaly of direction-arrow zoom in
 *	v05,20111207 hy.ren
 *		a.modify pb display buffer size accord different display device.
 *	v06,20111207 hy.ren
 *      	a.add time info in pbFileInfoGet function
 *	v07,20111209 kai.wang
 *      	a.modify multiplay osd show on TV
 *	v08,20111209 kai.wang
 *      	a.modify multiplay osd show on TV -2
 *	v09,20120105 xu.gao
 *		a.fix 16:9 playback problem
 *	v10,20120222 dongdong.wang
 *		a.add distortion
 *	v11,20120224 dongdong.wang
 *		a.add  16:9 solution for distortion
 *	v12,20120228 dongdong.wang
 *		a.add decrease time of distortion
 *	v13,20120302 dongdong.wang
 *		a.modify for distortion hung up
 *	v14,20120919 xianxin
 *		a.correct 16:9 decode size
 *	v15,20120921 xianxin
 *		a.for distortion
 *	v16,20120924 xianxin
 *		a.fix calculational error
 *	v17,20121012 xianxin
 *		a.modify roi for distortion to avoid boundary aligment
 *           b.decode top left for distorion
 *	v18,20121116 xianxin
 *		a.fix disp rate for real 3:4 JPG
 *		b.rewrite pbImgDeCmpRot() for fillblack
 *		c.use main img if no thumbnail
 *	v19,20121128 xianxin
 *		a.fix 46334
 **************************************************************************/	

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static UINT8 code verStr[] = "$pb19";

/**************************************************************************
 *
 *  Function Name: pbLibVerGet
 *
 *  Purposes:
 *
 *  Descriptions:
 *
 *  Arguments:
 *
 *  Returns:
 *
 *  See also:
 *
 **************************************************************************/
UINT8 *
libPbVerGet(
	void
)
{
	return (verStr + 3);
}

