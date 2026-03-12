/**************************************************************************
 *                                                                        *
 *       Copyright (c) 2004-2006 by Sunplus Technology Co., Ltd.          *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan                                                      *
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 5553 $
$Date:: 2010-12-09 20:35:58#$
 */
 
#include "general.h"
#include "pwrlog.h"

#ifdef LOG2_USE_8BIT
static code UINT8 log2tabxN[]={ /*tab[i]=256*log2(1+i/64)*/
	0,6,11,17,22,28,33,38,
	44,49,54,59,63,68,73,78,
	82,87,92,96,100,105,109,113,
	118,122,126,130,134,138,142,146,
	150,154,157,161,165,169,172,176,
	179,183,186,190,193,197,200,203,
	207,210,213,216,220,223,226,229,
	232,235,238,241,244,247,250,253,
};
#endif /*LOG2_USE_8BIT*/

#ifdef LOG2_USE_10BIT
static code UINT16 log2tabxN[]={ /*tab[i]=1024*log2(1+i/256)*/
0,6,11,17,23,29,34,40,45,51,57,62,68,73,79,84,
90,95,100,106,111,116,122,127,132,138,143,148,153,159,164,169,
174,179,184,189,194,199,204,209,214,219,224,229,234,239,244,249,
254,259,264,268,273,278,283,288,292,297,302,306,311,316,320,325,
330,334,339,343,348,353,357,362,366,371,375,380,384,388,393,397,
402,406,411,415,419,424,428,432,436,441,445,449,454,458,462,466,
470,475,479,483,487,491,495,500,504,508,512,516,520,524,528,532,
536,540,544,548,552,556,560,564,568,572,576,580,584,587,591,595,
599,603,607,610,614,618,622,626,629,633,637,641,644,648,652,656,
659,663,667,670,674,678,681,685,689,692,696,699,703,707,710,714,
717,721,724,728,731,735,738,742,745,749,752,756,759,763,766,770,
773,776,780,783,787,790,793,797,800,803,807,810,813,817,820,823,
827,830,833,837,840,843,846,850,853,856,859,863,866,869,872,875,
879,882,885,888,891,894,898,901,904,907,910,913,916,919,922,926,
929,932,935,938,941,944,947,950,953,956,959,962,965,968,971,974,
977,980,983,986,989,992,995,998,1001,1004,1007,1010,1012,1015,1018,1021,
};
#endif /*LOG2_USE_10BIT*/

SINT32
log2xyND(
		SINT32 num,
		SINT32 denom,
		SINT32 *log2X,
		SINT32 *log2Y
		)
{
	unsigned i,f;
	UINT32 n,d,q,r,t,u;
	SINT32 sign;

	if (denom == 0){
		*log2X = LOG2_DOMAIN_ERR;
		*log2Y = LOG2_DOMAIN_ERR;
		return LOG2_DOMAIN_ERR;
	}
	if (denom < 0) { denom = -denom; num = -num; }
	if (num <= 0) {
		*log2X = LOG2_DOMAIN_ERR;
		*log2Y = LOG2_DOMAIN_ERR;
		return LOG2_DOMAIN_ERR;
	}
	if (num >= denom) {
		sign = 0;
		n = num;	//分子
		d = denom;	//分母
	} else {
		sign = -1L;
		n = denom;	//分子
		d = num;	//分母
	}

	q = 0;
	while (n >= (t = d + d)) {
		++q;
		d = t;
	}
	if ((r = n - d) == 0) {
		t = 0;
		d = 1;
		goto __out_run;
	}

	if (d>=(UINT32)LOG2_OVERFLOW/2/LOG2_ONE) {
		d = (d + LOG2_ONE) / 2 / LOG2_ONE;
		r = (r + LOG2_ONE) / 2 / LOG2_ONE;
	}

	r *= LOG2_ON_NTH_PARTITION;
	i = r / d;
	f = r % d;
	if (i == LOG2_ON_NTH_PARTITION) {
		t = 1;
		d = 1;
		goto __out_run;
	}
	t = log2tabxN[i];
	if (f == 0) {
		q *= LOG2_ONE;
		d = LOG2_ONE;
		goto __out_run;
	}
	if (i == LOG2_ON_NTH_PARTITION - 1) u = LOG2_ONE;
	else u = log2tabxN[i+1];
	t = t*(d-f) + u*f;
	if (d>=(UINT32)LOG2_OVERFLOW/LOG2_ONE/LOG2_RANGE_ULIMIT) {
		q *= d;
		t = (t + LOG2_ONE / 2) / LOG2_ONE;
	} else {
		q *= d * LOG2_ONE;
		d *= LOG2_ONE;
	}
__out_run:
	*log2X = ((t + q) ^ sign) - sign; /*t+q if n>=d; -t-q if n<d*/
	*log2Y = d;
	return 0;
}

