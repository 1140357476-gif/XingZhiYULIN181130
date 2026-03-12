/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 /**
 * @file		exif_fun.c
 * @brief		exif standard 2.2
 * @author		hy.ren
 * @since		2008-03-14
 * @date		2008-10-31
 */
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/

#include "exif_fun.h"
#include "general.h"
short exif_get_sshort (const unsigned char *buf, ExifByteOrder order)
{
	if (!buf) return 0;
        switch (order) {
        case EXIF_BYTE_ORDER_MOTOROLA:
                return ((buf[0] << 8) | buf[1]);
        case EXIF_BYTE_ORDER_INTEL:
                return ((buf[1] << 8) | buf[0]);
        }

	/* Won't be reached */
	return (0);
}

unsigned short exif_get_short (const unsigned char *buf, ExifByteOrder order)
{
	return (exif_get_sshort (buf, order) & 0xffff);
}

unsigned long exif_get_slong (const unsigned char *b, ExifByteOrder order)
{
	if (!b) return 0;
        switch (order) {
        case EXIF_BYTE_ORDER_MOTOROLA:
                return ((b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]);
        case EXIF_BYTE_ORDER_INTEL:
                return ((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0]);
        }

	/* Won't be reached */
	return (0);
}

long exif_get_long (const unsigned char *buf, ExifByteOrder order)
{
        return (exif_get_slong (buf, order) & 0xffffffff);
}
 
unsigned short exif_swap_ushort (unsigned short x)
{
    unsigned short y = 0;  
    unsigned short z = 0;  
  
    z = x & 0x00FF;  
    y = z << 8;  
    z = x & 0xFF00;  
    y += (z >> 8);  
    return y;  
	//return (x<<8) | (x>>8);
}

unsigned long exif_swap_ulong (unsigned long x)
{	
#if 0
	unsigned long y = 0;  
    unsigned long z = 0;  
  
    z = x & 0x000000FF;  
    y = z << 24;  
    z = x & 0x0000FF00;  
    y += (z << 8);  
    z = x & 0x00FF0000;  
    y += (z >> 8);  
    z = x & 0xFF000000;  
    y += (z >> 24);  
    return y;  
#endif
	return ( ( x & 0xff000000 ) >> 24 ) | ( ( x & 0x00ff0000 ) >> 8 ) | ( ( x & 0x0000ff00 ) << 8 ) | ( ( x & 0x000000ff ) << 24 );
}

UINT16 swapWord(UINT16 x)
{	
	return (x<<8) | (x>>8);
}

UINT16 exif_shortValue_get(UINT16 x, ExifByteOrder byteorder)
{
	if (!x) return 0;
	if(EXIF_BYTE_ORDER_INTEL == byteorder)
		x=((x << 8) | (x >> 8 ));
	return x;
}

UINT32 exif_longValue_get(UINT32 x, ExifByteOrder byteorder)
{
	if (!x) return 0;
	if(EXIF_BYTE_ORDER_INTEL == byteorder)
		x=((x << 24) | ((x & 0x0000ff00 ) << 8) | (( x & 0x00ff0000 ) >> 8) | (x >> 24 ));
	return x;
}

