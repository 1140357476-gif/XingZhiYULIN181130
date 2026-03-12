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
 * @file      snap_qtable.c
 * @brief     The q-table flow
 * @author    Wen-Guo
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_jpeg.h"
#include "app_main.h"
#include "snap_qtable.h"
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_SNAP_QTABLE__
	
#define DCTSIZE2 64
	
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if 0 /* Q table in standard */
		code UINT32 std_luminance_quant_tbl[DCTSIZE2] = {
			16,  11,  10,  16,	24,  40,  51,  61,
			12,  12,  14,  19,	26,  58,  60,  55,
			14,  13,  16,  24,	40,  57,  69,  56,
			14,  17,  22,  29,	51,  87,  80,  62,
			18,  22,  37,  56,	68, 109, 103,  77,
			24,  35,  55,  64,	81, 104, 113,  92,
			49,  64,  78,  87, 103, 121, 120, 101,
			72,  92,  95,  98, 112, 100, 103,  99
		};
		code UINT32 std_chrominance_quant_tbl[DCTSIZE2] = {
			17,  18,  24,  47,	99,  99,  99,  99,
			18,  21,  26,  66,	99,  99,  99,  99,
			24,  26,  56,  99,	99,  99,  99,  99,
			47,  66,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99
		};
#else /* Q table in zig-zag order */
		code UINT8 std_luminance_quant_tbl[DCTSIZE2] = {
			16,  11,  12,  14,	12,  10,  16,  14,
			13,  14,  18,  17,	16,  19,  24,  40,
			26,  24,  22,  22,	24,  49,  35,  37,
			29,  40,  58,  51,	61,  60,  57,  51,
			56,  55,  64,  72,	92,  78,  64,  68,
			87,  69,  55,  56,	80, 109,  81,  87,
			95,  98, 103, 104, 103,  62,  77, 113,
		   121, 112, 100, 120,	92, 101, 103,  99
		};
		
		code UINT8 std_chrominance_quant_tbl[DCTSIZE2] = {
			17,  18,  18,  24,	21,  24,  47,  26,
			26,  47,  99,  66,	56,  66,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99,
			99,  99,  99,  99,	99,  99,  99,  99
		};
#endif

xdata UINT8 snapMainQIdx;

/**************************************************************************
 *               S T A T I C   L O C A L   F U N C T I O N                *
 **************************************************************************/
static SINT32 jpegQualityScaling( SINT32 quality );
static void jpegYQTableGen( SINT32 quality, UINT8 *qtable );
static void jpegUVQTableGen( SINT32 quality, UINT8 *qtable );
static UINT8 jpegQTableWrite( UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt );
static UINT8 jpegQTableRead( UINT8 *QTableIndex, UINT8 *QTable, UINT8 zigzagOpt );
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**
 * @fn        UINT8 jpegQTableSet(UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt)
 * @brief     set Q table to back-end
 * @param     [in] QTableIndex : 0/100 for Q1, all q value is '1'. 
 *							1~99, firmware will generate q table ,and set it to back-end. Index is bigger and the picture quality is better
 * @param     [in] yQTable : luminance Q table pointer. when QTableIndex=0/100, the input Q table is set to back-end directly 
 *						or the Q table is generated by firmware.
 * @param     [in] uvQTable : color U&V Q table. similar to Y Q table
 * @param     [in] zigzagOpt : '0' for raser scan order, '1' for zigzag scan order.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
jpegQTableSet(
	UINT8 QTableIndex,
	UINT8 *yQTable,
	UINT8 *uvQTable,
	UINT8 zigzagOpt
) USING_0
{
	UINT8 qtable[ 128 ];

	if( QTableIndex == 0 || QTableIndex == 100 ) { // no qtable needed for Q1
		jpegQTableWrite( QTableIndex, yQTable, uvQTable, zigzagOpt );
	}
	else {
		// generation
		jpegYQTableGen( (SINT32)QTableIndex, qtable );
		jpegUVQTableGen( (SINT32)QTableIndex, qtable + 64 );

		jpegQTableWrite( QTableIndex, qtable, qtable + 64, zigzagOpt );
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 jpegQTableGet(UINT8 *qTableIndex, UINT8 *qTable, UINT8 zigzagOpt)
 * @brief     get q table information from back-end
 * @param     [in] qTableIndex : read q table index from back-end
 * @param     [in] qTable : Y/ U&V q table pointer. get q table from back-end. But when qTableIndex=100, it is generated by FW
 * @param     [in] zigzagOpt : '0' for raser scan order, '1' for zigzag scan order.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
jpegQTableGet(
	UINT8 *qTableIndex,
	UINT8 *qTable,
	UINT8 zigzagOpt
) USING_0
{
	jpegQTableRead( qTableIndex, qTable, zigzagOpt );

	if( *qTableIndex == 100 ) {
		// generation
		jpegYQTableGen( (SINT32)100, qTable );
		jpegUVQTableGen( (SINT32)100, qTable + 64 );
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 jpegQTableGen(UINT8 QTableIndex, UINT8 *QTable, UINT8 opt)
 * @brief     generate Q table
 * @param     [in] QTableIndex : input Q index
 * @param     [in] QTable : Q table pointer
 * @param     [in] opt : '0', generate Y, U&V q table(total 128 bytes)
 *					'1',  generate Y q table(total 64 bytes)
 *					'2' generate U/V q table(total 64 bytes)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
jpegQTableGen(
	UINT8 QTableIndex,
	UINT8 *QTable,
	UINT8 opt
) USING_0
{
	if( opt == 0 ) {
		jpegYQTableGen( (SINT32)QTableIndex, QTable );
		jpegUVQTableGen( (SINT32)QTableIndex, QTable + 64 );
	}
	else if( opt == 1 ) {
		jpegYQTableGen( (SINT32)QTableIndex, QTable );
	}
	else if( opt == 2 ) {
		jpegUVQTableGen( (SINT32)QTableIndex, QTable );
	}

	return SUCCESS;
}


/**
 * @fn        void jpegMainImgQIdxGet(UINT8 *idx)
 * @brief     jpegMainImgQIdxGet
 * @param     [in] idx
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void 
jpegMainImgQIdxGet(
	UINT8 *idx
)
{
	*idx = snapMainQIdx; 
}


/**
 * @fn        SINT32 jpegQualityScaling(SINT32 quality)
 * @brief     input Q table index, and return q table scaling factor
 * @param     [in] quality : 1~100 map to return value 5000~0. so quality is bigger, the picture quality is better.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
static SINT32
jpegQualityScaling(
	SINT32 quality_t
)
/* Convert a user-specified quality rating to a percentage scaling factor
 * for an underlying quantization table, using our recommended scaling curve.
 * The input 'quality' factor should be 0 (terrible) to 100 (very good).
 */
{
	data SINT32 quality;
	quality = quality_t;
	/* Safety limit on quality factor.  Convert 0 to 1 to avoid zero divide. */
	if (quality <= 0) quality = 1;
	if (quality > 100) quality = 100;

	/* The basic table is used as-is (scaling 100) for a quality of 50.
	 * Qualities 50..100 are converted to scaling percentage 200 - 2*Q;
	 * note that at Q=100 the scaling is 0, which will cause jpeg_add_quant_table
	 * to make all the table entries 1 (hence, minimum quantization loss).
	 * Qualities 1..50 are converted to scaling percentage 5000/Q.
	 */
	if (quality < 50)
		quality = 5000 / quality;
	else
		quality = 200 - (quality<<1);

	return quality;
}


/**
 * @fn        void jpegYQTableGen(SINT32 quality, UINT8 *qtable)
 * @brief     generate Y q table
 * @param     [in] quality : input Q table index
 * @param     [out] qtable : output q table pointer
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void
jpegYQTableGen(
	SINT32 quality,
	UINT8 *qtable
)
{
	/* Define a quantization table equal to the basic_table times
	 * a scale factor (given as a percentage).
	 * If force_baseline is TRUE, the computed quantization table entries
	 * are limited to 1..255 for JPEG baseline compatibility.
	 */
	#if 0
	const UINT32 *basic_table;
	#else
	const UINT8 *basic_table;
	#endif

	data UINT8 i;
	data UINT16 scale_factor;
	data SINT32 temp;

	scale_factor = (UINT16)jpegQualityScaling(quality);
	basic_table = std_luminance_quant_tbl;

	for (i = 0; i < DCTSIZE2; i++) {
		temp = ((SINT32) basic_table[i] * scale_factor + 50L) / 100L;
		/* limit the values to the valid range */
		if (temp <= 0L)    temp = 1L;
		//if (temp > 32767L) temp = 32767L; /* max quantizer needed for 12 bits */
		if (temp > 255L)   temp = 255L;		/* limit to baseline range if requested */
		qtable[i] = (UINT8)temp;
	}
}

/**
 * @fn        void jpegUVQTableGen(SINT32 quality, UINT8 *qtable)
 * @brief     generate U/V q table
 * @param     [in] quality : input Q table index
 * @param     [out] qtable : output q table pointer
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
void
jpegUVQTableGen(
	SINT32 quality,
	UINT8 *qtable
)
{
	/* Define a quantization table equal to the basic_table times
	 * a scale factor (given as a percentage).
	 * If force_baseline is TRUE, the computed quantization table entries
	 * are limited to 1..255 for JPEG baseline compatibility.
	 */
	#if 0
	const UINT32 *basic_table;
	#else
	const UINT8 *basic_table;
	#endif

	data UINT8 i;
	data UINT16 scale_factor;
	data SINT32 temp;

	scale_factor = (UINT16)jpegQualityScaling(quality);
	basic_table = std_chrominance_quant_tbl;

	for (i = 0; i < DCTSIZE2; i++) {
		temp = ((SINT32) basic_table[i] * scale_factor + 50L) / 100L;
		/* limit the values to the valid range */
		if (temp <= 0L)    temp = 1L;
		//if (temp > 32767L) temp = 32767L; /* max quantizer needed for 12 bits */
		if (temp > 255L)   temp = 255L;		/* limit to baseline range if requested */
		qtable[i] = (UINT8)temp;
	}
}

/**
 * @fn        UINT8 jpegQTableWrite(UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt)
 * @brief     set input Q table into back-end
 * @param     [in] QTableIndex : input Q table index
 * @param     [in] yQTable : Y q table pointer
 * @param     [in] uvQTable : U/V q table pointer
 * @param     [in] zigzagOpt : '0' for raser scan order, '1' for zigzag scan order.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
static UINT8 
jpegQTableWrite(
	UINT8 QTableIndex,
	UINT8 *yQTable,
	UINT8 *uvQTable,
	UINT8 zigzagOpt
) USING_0
{

	HAL_JpegQTableWrite( QTableIndex, yQTable, uvQTable, zigzagOpt );

	return SUCCESS;
}

/**
 * @fn        UINT8 jpegQTableRead(UINT8 *QTableIndex, UINT8 *QTable, UINT8 zigzagOpt)
 * @brief     get Q table from back-end
 * @param     [in] QTableIndex : output Q table index
 * @param     [in] QTable : output Y and UV table pointer
 * @param     [in] zigzagOpt : '0' for raser scan order, '1' for zigzag scan order.
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
static UINT8 
jpegQTableRead(
	UINT8 *QTableIndex,
	UINT8 *QTable,
	UINT8 zigzagOpt
)
{

	HAL_JpegQTableRead( QTableIndex, QTable, zigzagOpt );

	return SUCCESS;
}



/**
 * @fn        UINT8 sp1kJpegQTableSet(UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt)
 * @brief     It set the Q-table which jpeg uses for encode and decode. It can set with Q index or customized table
 * @param     [in] QTableIndex : q table index
 * @param     [in] yQTable : input Y q table
 * @param     [in] uvQTable : input U/V q table
 * @param     [in] zigzagOpt : '0' for raser scan order, '1' for zigzag scan order.
 * @retval    return = SUCCESS / FAIL
 * @see       jpegQTableSet
 * @author    Will Chiu
 * @since     2010-5-13
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
sp1kJpegQTableSet(
	UINT8 QTableIndex,
	UINT8 *yQTable,
	UINT8 *uvQTable,
	UINT8 zigzagOpt
) USING_0
{
	jpegQTableSet( QTableIndex, yQTable, uvQTable, zigzagOpt );

	snapMainQIdx = QTableIndex;

	return SUCCESS;
}

