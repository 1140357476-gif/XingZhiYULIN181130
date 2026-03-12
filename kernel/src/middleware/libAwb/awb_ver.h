#ifndef _AAA_AWB_VER_H__
#define _AAA_AWB_VER_H__

#define awbVersion 12062501

/* v10030401 wangjg++
 * creat new awb lib
 * 
 *
 *
 * v10030402 wangjg++
 * 1、添加函数 : void awbFrameProc(void)，
 *        主要针对采样点不足造成稳定性差的问题，采用多帧采样点以前decide的方式。
 * 2、修改Outdoor判断条件；
 * 3、修改outdoor曲线；
 * 4、修改手动白平衡：cloudy
 *
 * v10030901 wangjg++
 * 多帧decide增加权重；
 *
 * v10031001 wangjg++
 * outdoor spot[1] wp;
 * awbFlog0;
 *
 * v10031101
 * v10031102
 * v10031103
 *	1.白点数量的上限wp_thr
 *	2.修改rgb_thrs
 *	3.
 * v10031104
 *    1.call awbCalConvergeGain() in awbNoEnoughPoint()
 *    2.sram
 * v10031104
 *   1. spot 1 wp in outdoor
 *   2. 
 * v10091501
 *   1. remove spot queue moe_queue_t dis_rgain_c0
 *
 * v10091502
 *   1. modefy spot mode.
 *
 * v10091601
 *   1. awb Run mode.
 *   2. enable awb run 1pf
 * v10092001
 *   1. lack white point use spot fix gain
 * v10102201
 *   1. correct determinant falsely A light to sward(outdoor).
 * v10102202
 *   1. correct Auto OB err for A light.
 * v10111514
 *   室外大阴影不稳定(加强outdoor 状态惰性判断
 * v10111522
 *   1.加强scene 惰性
 *   2.增加办公室环境判断矩形
 * v10111609
 *   1.增加室外sward、sky判断条件
 * v10111720
 * v10111810
 * v10111815
 *   1.室内增加标准光源的权重
 * v10112419
 * v10113011 
 * v10120310  
 * v10120314
 * v11102001
 *  1.decrease awb darkluma checkpoint
 * v12062501
 *  1.add awb moe queue CodeTbl, to speed up power on flow;
 */
#endif
