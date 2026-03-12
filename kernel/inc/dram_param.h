/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __DRAM_PARAM_H__
#define __DRAM_PARAM_H__

#define ENABLE 1
#define DISABLE 0

enum {
	SDR_16M = 0,
	SDR_64M,
	SDR_128M,
	SDR_256M,
	SDR_512M
};

enum {
	SdckMode = 1,
	CasMode = 2,
	SDRAMOe = 4,
	DisRefresh = 8,
	Disnxtact = 16,
	Discmdf = 32,
	Sstle_pwd_reg = 64,
	Discasxfr = 128
};

enum {
	CHAL = 0,
	CL3
};

enum {
	BANK0 = 1,
	BANK1 = 2,
	BANK2 = 4,
	BANK3 = 8
};

enum {
	BANK_TYPE1  = (BANK0),
	BANK_TYPE2  = (BANK1),
	BANK_TYPE3  = (BANK0 | BANK1),
	BANK_TYPE4  = (BANK2),
	BANK_TYPE5  = (BANK0 | BANK2),
	BANK_TYPE6  = (BANK1 | BANK2),
	BANK_TYPE7  = (BANK0 | BANK1 | BANK2),
	BANK_TYPE8  = (BANK3),
	BANK_TYPE9  = (BANK0 | BANK3),
	BANK_TYPE10 = (BANK1 | BANK3),
	BANK_TYPE11 = (BANK0 | BANK1 | BANK3),
	BANK_TYPE12 = (BANK2 | BANK3),
	BANK_TYPE13 = (BANK0 | BANK2 | BANK3),
	BANK_TYPE14 = (BANK1 | BANK2 | BANK3),
	BANK_TYPE15 = (BANK0 | BANK1 | BANK2 | BANK3)
};

#define Twrp 0x05
#define Trfc 0x0B
#define Trcd 0x02
#define Trrd 0x01
#define Trp  0x03
#define Tmrd 0x03

#define SdckPhase 0
#define LatchPhase 0
#define r_w_lat 5
#define w_r_lat 0

#define SDRAM_BANK_TYPE     BANK_TYPE15

#define SDRAM_TYPE          SDR_256M
#define SDRAM_MODE          (CasMode | SDRAMOe)
#define SDRAM_RWCASCADEN    (ENABLE << 4)
#define SDRAM_REF           1
#define SDRAM_REFINTVAL_L   0xA1
#define SDRAM_REFINTVAL_H   0x02
#define SDRAM_CTRL_TIMING0  (Twrp | (Trfc << 4))
#define SDRAM_CTRL_TIMING1  (Trcd | (Trrd << 3))
#define SDRAM_CTRL_TIMING2  (Trp | (Tmrd << 3))
#define SDRAM_CASLATENCY    CL3
#define SDRAM_PHASE         (SdckPhase | (LatchPhase << 4))
#define SDRAM_RWLatency     (r_w_lat | (w_r_lat << 3))

#endif /*__DRAM_PARAM_H__*/
