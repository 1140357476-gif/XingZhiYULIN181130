/**************************************************************************
 *         Copyright(c) 2011 by i-Catch Technology Co., Ltd.              *
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
 * @file      cmd_mem.c
 * @brief     dump / fill all system memory
 * @author    Phil Lin
 * @since     2011-02-21
 * @date      2011-02-21
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "cmd.h"
#include "reg_def.h"

#define WDT_EN 1

#if WDT_EN
#include "dbg_wdt.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MEM_DUMP_CNT_MIN     0x20
#define DUMP_CHARS_PER_LINE  16

#define MEM_DUMP_FMT         0
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum memDump_e {
	MEM_DUMP_TYPE_IDATA = 0x00,
	MEM_DUMP_TYPE_XDATA = 0x01,
	MEM_DUMP_TYPE_CODE  = 0x02,
	MEM_DUMP_TYPE_DRAM  = 0x03,
	
	MEM_DUMP_TYPE_TOT   = 0x04,
} memDump_t;

typedef struct cmdMemCtrl_s {
	UINT32 addr;
	UINT32 size;
	UINT8 type;
	UINT8 hdr;
	UINT16 dat;
} cmdMemCtrl_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        void cmdMemHelpShow(void)
 * @brief     cmdMemHelpShow
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-21
 * @todo      N/A
 * @bug       N/A
*/
static void cmdMemHelpShow(void)
{
	printf("\nfill [type] <addr_s> [+offset] <addr_e> <val>\n");
	printf(  "dump [type] [addr_s] [+offset] [addr_e]\n");
}

/**
 * @fn        void cmdMemPrint(UINT32 addrS, UINT32 sizeTot, UINT8 memType, UINT8 hdr)
 * @brief     cmdMemPrint
 * @param     [in] addrS
 * @param     [in] sizeTot
 * @param     [in] memType
 * @param     [in] hdr
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-21
 * @todo      N/A
 * @bug       N/A
*/
static void
cmdMemPrint(
	UINT32 addrS,
	UINT32 sizeTot,
	UINT8 memType,
	UINT8 hdr
)
{
	UINT32 i, k;
	UINT8 j;
	BIT bDatOut;
	UINT8 datBuff[DUMP_CHARS_PER_LINE];
	UINT8 dat;
	UINT32 addr;
	UINT32 addrE;
	UINT8 code memTypeMask[MEM_DUMP_TYPE_TOT] = {
		0x00, // *(volatile UINT8 idata*)
		0x01, // *(volatile UINT8 xdata*)
		0xff, // *(volatile UINT8 code*)
		0x00  // reserved for dram
	};

	if (memType == MEM_DUMP_TYPE_DRAM) {
		/* set read start address */
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(&addrS, 3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(&addrS, 2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(&addrS, 1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(&addrS, 0);
		XBYTE[REG_Dram_IndirectPrefetch] = 0x01;

		sizeTot <<= 1; // convert to byte
	} else {
		WRITE8(addrS, 1, memTypeMask[memType]);
		WRITE8(addrS, 0, 0);
	}

	/* address aligment */
	addr = addrS & ~0x0f;
	addrE = addrS + sizeTot + 0x0f & ~0x0f;

	bDatOut = 0;
	j = 0;
	for(i = addr, k = addr; i < addrE; i++) {
		/* this is the line start */
		if (!j) {             		
			printf("%c:%06lxh: ", hdr, memType == MEM_DUMP_TYPE_DRAM ? k : (UINT32)((UINT16)i));
		}

		/* check to start */
		if (!bDatOut && (memType == MEM_DUMP_TYPE_DRAM ? k : i) >= addrS) bDatOut = 1;                  
#if MEM_DUMP_FMT		
		if ( bDatOut && (memType == MEM_DUMP_TYPE_DRAM ? k : i) >= addrS + sizeTot) bDatOut = 0;
#endif		

		if (!bDatOut) {
			/* dummy data */
			datBuff[j] = ' ';
			putchar(' '); putchar(' '); putchar(' ');
		} else {
			/* data get */
			if (memType == MEM_DUMP_TYPE_DRAM) {
				if (!(j & 0x01)) {
					dat = XBYTE[REG_Dram_IndirectRData_Low]; //LSB
				} else {
					dat = XBYTE[REG_Dram_IndirectRData_High]; //MSB
				}
			} else {
				dat = *(volatile UINT8 *)i;
			}
			/* output data */
			printf("%02bX", dat);                     
			datBuff[j] = dat;

			/* the middle tag */
			if (j == 0x07) {
				putchar('-');
			} else {
				putchar(' ');
			}
		}

		/* increase word address */
		(j & 0x01) && k++;
		
 		/* this is the line end */
		if (++j & 0xf0) { //++j > 0x0f
			putchar('|');
			putchar(' ');
			do {
				dat = datBuff[16 - j];
				if (dat < 0x20 || (dat & 0x80)) { //dat < ' ' || dat > 128
					putchar('.');
				} else {
					putchar(dat);
				}
			} while (--j);
			putchar('\n');
		}
	}	
}

/**
 * @fn        void cmdMemWrite(UINT32 addr, UINT32 sizeTot, UINT8 memType, UINT16 dat)
 * @brief     cmdMemWrite
 * @param     [in] addr
 * @param     [in] sizeTot
 * @param     [in] memType
 * @param     [in] dat
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-21
 * @todo      N/A
 * @bug       N/A
*/
static void
cmdMemWrite(
	UINT32 addr,
	UINT32 sizeTot,
	UINT8 memType,
	UINT16 dat
)
{
	UINT8 *data ptr;
	UINT16 size16;
	UINT8 code memTypeMask[MEM_DUMP_TYPE_TOT] = {
		0x00, // *(volatile UINT8 idata*)
		0x01, // *(volatile UINT8 xdata*)
		0xff, // *(volatile UINT8 code*)
		0x00  // reserved for dram
	};

	if (memType == MEM_DUMP_TYPE_DRAM) {
		/* set write start address */
		XBYTE[REG_Dram_IndirectAddr_B0] = READ8(&addr, 3);
		XBYTE[REG_Dram_IndirectAddr_B1] = READ8(&addr, 2);
		XBYTE[REG_Dram_IndirectAddr_B2] = READ8(&addr, 1);
		XBYTE[REG_Dram_IndirectAddr_B3] = READ8(&addr, 0);

		/* data write */
		do {
			while(XBYTE[REG_Dram_IndirectPrefetch]);
			XBYTE[REG_Dram_IndirectRData_Low] = READ8(dat, 1);
			XBYTE[REG_Dram_IndirectRData_High] = READ8(dat, 0);
		} while(--sizeTot);
	} else {
		/* assigned the pointer to the type: idata* | xdata* | code* */
		WRITE8(ptr, 2, READ8(addr, 3));
		WRITE8(ptr, 1, READ8(addr, 2));
		WRITE8(ptr, 0, memTypeMask[memType]);

		/* cut the address to 16 bits */
		size16 = sizeTot;

		/* date write */
		do {
			*ptr++ = READ8(dat, 1);
		} while(--size16);
	}
}

/**
 * @fn        UINT8 cmdMemParaGet(UINT8 *param, cmdMemCtrl_t *pPara)
 * @brief     cmdMemParaGet
 * @param     [in] param
 * @param     [in] pPara
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-21
 * @todo      N/A
 * @bug       N/A
*/
static UINT8
cmdMemParaGet(
	UINT8 *param,
	cmdMemCtrl_t *pPara
)
{
	UINT8 memType = MEM_DUMP_TYPE_XDATA;
	UINT32 argv[8];
	UINT32 xdata* pargv;
	UINT8 xdata* ptr;
	UINT32 addrS, sizeTot;
	UINT8 code hdrInfo[MEM_DUMP_TYPE_TOT] = { 'I', 'X', 'C', 'D', };
	UINT8 i;
	UINT8 type;

	/* get the arguments */
	cmdArgGet(&param, argv, 5, 1);

	/* check the first parameter */
	if (argv[0] == '\0' && argv[1] == '\0') {
		return FAIL;
	}

	/* token the first parameter */
	type = *(UINT8 xdata*)argv[0];
	pargv = &argv[0];
	for (i = 0; i < MEM_DUMP_TYPE_TOT; i++) {
		/* check the memory type */
		if (type == hdrInfo[i] || (type - 0x20) == hdrInfo[i]) {
			memType = i;
			pargv++;
			break;
		}
	}

	/* token the second parameter */
	addrS = *pargv++;

	/* check the third parameter */
	ptr = (UINT8 xdata*)*pargv;
	if (*ptr == '+') {
		/* get the true size */
		sizeTot = strtoul(ptr+1, NULL, 0);
	} else {
		/* this is the end address */
		sizeTot = *pargv;
		if (sizeTot){
			if (sizeTot <= addrS) {
				return FAIL;
			} else {
				/* caculate the ture size */
				sizeTot = sizeTot - addrS;
			}
		} else {
			/* "size == 0" is OK */
		}
	}

	/* what?! */
	if (!addrS && !sizeTot) {
		return FAIL;
	}

	/* token the fourth parameter */
	pargv++;

	/* pop the parameter */
	pPara->addr = addrS;
	pPara->size = sizeTot;
	pPara->type = memType;
	pPara->hdr = hdrInfo[memType];
	pPara->dat = *pargv;
	
	return SUCCESS;
}

/**
 * @fn        void cmdMemDump(UINT8 *param)
 * @brief     cmdMemDump
 * @param     [in] param
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-21
 * @todo      N/A
 * @bug       N/A
*/
void
cmdMemDump(
	UINT8 *param
)
{
	UINT8 ret;
	cmdMemCtrl_t Para, xdata* pPara;

	pPara = &Para;
	/* parameter get*/
	ret = cmdMemParaGet(param, pPara);

#if WDT_EN
	/* disable the WDT timer */
	dbgWdtSet(0);
#endif
	
	if (ret == SUCCESS) {
		if (pPara->size < MEM_DUMP_CNT_MIN) {
			pPara->size = MEM_DUMP_CNT_MIN;
		}
		/* print out the memory */
		cmdMemPrint(pPara->addr, pPara->size, pPara->type, pPara->hdr);
	} else {
		cmdMemHelpShow();	
	}

#if WDT_EN
	/* enable the WDT timer */
	dbgWdtSet(1);
#endif
}

/**
 * @fn        void cmdMemFill(UINT8 *param)
 * @brief     cmdMemFill
 * @param     [in] param
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-2-21
 * @todo      N/A
 * @bug       N/A
*/
void
cmdMemFill(
	UINT8 *param
)
{
	UINT8 ret;
	cmdMemCtrl_t Para, xdata* pPara;

	pPara = &Para;
	/* parameter get*/
	ret = cmdMemParaGet(param, pPara);

#if WDT_EN
	/* disable the WDT timer */
	dbgWdtSet(0);
#endif
	
	if (ret == SUCCESS && pPara->size) {
		/* write into the memory */
		cmdMemWrite(pPara->addr, pPara->size, pPara->type, pPara->dat);
	} else {
		cmdMemHelpShow();	
	}

#if WDT_EN
	/* enable the WDT timer */
	dbgWdtSet(1);
#endif
}


/***************************************************************************/

/* The functions belowed were just for testing... */

#if 0 //function register method dump + old fill

static UINT16 xdata dumpAddrS;

#define DUMP_CNT_MIN 0x20

#define IDATA_MAX 0x100
#define XDATA_MAX 0x10000
#define CODE_MAX  0x8000
#define DRAM_MAX  0x10000000

typedef UINT8 (*pFuncMemR)(void);
typedef void (*pFuncAddrShow)(UINT32);


static UINT8 idataRead(void);
static void idataAddrShow(UINT32 addr);
static UINT8 xdataRead(void);
static void xdataAddrShow(UINT32 addr);
static UINT8 codeRead(void);
static void codeAddrShow(UINT32 addr);
static UINT8 dramRead(void);
static void dramAddrShow(UINT32 addr);


static pFuncMemR code funcMemRTbl[] = {
	idataRead,
	xdataRead,
	codeRead,
	dramRead,
};

static pFuncAddrShow code funcAddrShowTbl[] = {
	idataAddrShow,
	xdataAddrShow,
	codeAddrShow,
	dramAddrShow,
};


static void
dumpReadStart(
	UINT16 addr
)
{
	dumpAddrS = addr;
}

static UINT8
idataRead(
	void
)
{
	return *(volatile UINT8 idata *)dumpAddrS++;
}
static void
idataAddrShow(
	UINT32 addr
)
{
	printf("I:%06lxh: ", addr);
}

static UINT8
xdataRead(
	void
)
{
	return *(volatile UINT8 xdata *)dumpAddrS++;
}

static void
xdataAddrShow(
	UINT32 addr
)
{
	printf("X:%06lxh: ", addr);
}

static UINT8
codeRead(
	void
)
{
	return *(volatile UINT8 code *)dumpAddrS++;
}

static void
codeAddrShow(
	UINT32 addr
)
{
	printf("C:%06lxh: ", addr);
}

static UINT8
dramRead(
	void
)
{
	static UINT8 LSB, MSB;
	
	if (!(dumpAddrS++ & 0x01)) {
		while(XBYTE[REG_Dram_IndirectPrefetch]);
		LSB = XBYTE[REG_Dram_IndirectRData_Low];
		MSB = XBYTE[REG_Dram_IndirectRData_High];
		return LSB;
	} else {
		return MSB;
	}
}

static void
dramAddrShow(
	UINT32 addr
)
{
	printf("D:%06lxh: ", addr >> 1);
}

static void
dumpPrint(
	UINT32 addr,
	UINT32 size,
	pFuncAddrShow *pfs,
	pFuncMemR *pfr
)
{
	UINT32 addrS = addr & ~0x0f;
	UINT32 sizeTot = addr + size + 0x0f & ~0x0f;
	UINT32 i;
	UINT8 j;
	BIT bDatOut = 0;
	UINT8 dat[16] = {0};

	j = 0;
	for(i = addrS; i < sizeTot; i++) {
		if (!j) {             		/* line start */
			(*pfs)(i);
		}

		if (!bDatOut && i >= addr) bDatOut = 1;                  /* check to start */
		//if (bDatOut && i >= addr + size) bDatOut = 0;
		
		if (!bDatOut) {
			dat[j] = ' ';
			printf("   ");
		} else {
			dat[j] = (*pfr)();
			printf("%02bX", dat[j]);                     /* output data */
			
			if (j == 0x07) {
				putchar('-');
			} else {
				putchar(' ');
			}
		}
		
 		
		if (j++ >= 0x0f) {
			putchar('|');
			putchar(' ');
			do {
				UINT8 dt;
				if (dat[16-j] < ' ' || dat[16-j] > 128) {
					dt = '.';
				} else {
					dt = dat[16-j];
				}
				printf("%c", dt);
			} while (--j);
			putchar('\n');
		}
	}
}

static UINT8
dumpCalcParam(
	UINT32 datMax,
	UINT32 xdata* argv,
	UINT32 *pAddr,
	UINT32 *pSize
)
{
	UINT32 addrS, addrE, size;
	UINT8 xdata *pargv;

	addrS = argv[0];
	pargv = (UINT8 xdata*)argv[1];
	if (pargv[0] == '+') {
		size = strtoul(pargv+1, NULL, 0);
	} else {
		addrE = argv[1];
		if (!addrE) {
			size = 0;
		} else if (addrE >= addrS) {
			size = addrE - addrS;
		} else {
			return FAIL;
		}
	}
	
	if (size < DUMP_CNT_MIN) {
		size = DUMP_CNT_MIN;
	}
	
	if (addrS > datMax) {
		addrS = 0;
	}
	if (addrS + size >= datMax) {
		size = datMax - addrS;
	}

	if (pAddr) {
		*pAddr = addrS;
		printf("addrS: %lx\n", (UINT32)addrS);
	}
	if (pSize) {
		*pSize = size;
		printf("size: %lx\n", (UINT32)size);
	}

	return SUCCESS;
}

static void
dumpIdata(
	UINT32 addr,
	UINT32 size
)
{
	dumpReadStart(addr);
	dumpPrint(addr, size, idataAddrShow, idataRead);
}

static void
dumpXdata(
	UINT32 addr,
	UINT32 size
)
{
	dumpReadStart(addr);
	dumpPrint(addr, size, xdataAddrShow, xdataRead);
}
static void
dumpCode(
	UINT32 addr,
	UINT32 size
)
{
	dumpReadStart(addr);
	dumpPrint(addr, size, codeAddrShow, codeRead);
}
static void
dumpDram(
	UINT32 addr,
	UINT32 size
)
{
	XBYTE[REG_Dram_IndirectAddr_B0] = READ8(&addr, 3);
	XBYTE[REG_Dram_IndirectAddr_B1] = READ8(&addr, 2);
	XBYTE[REG_Dram_IndirectAddr_B2] = READ8(&addr, 1);
	XBYTE[REG_Dram_IndirectAddr_B3] = READ8(&addr, 0);
	XBYTE[REG_Dram_IndirectPrefetch] = 0x01;
	
	dumpPrint(addr, size, dramAddrShow, dramRead);
}

void
cmdMemDump(
	UINT8 *param
)
{
#define PARAM_MAX	8
    UINT32 arg[PARAM_MAX] = {0};
    UINT32 addr, size;
    UINT8 ret = FAIL;

    cmdArgGet(&param, arg, 8, 1);

#if WDT_EN    
	dbgWdtSet(0);
#endif /* WDT_EN */

	if (arg[0] != 0) {
	
		ret = SUCCESS;
		
		switch(*(UINT8 xdata*)arg[0]) {
			case 'i':
				ret = dumpCalcParam(IDATA_MAX, &arg[1], &addr, &size);
				if (ret == SUCCESS) {
					dumpIdata(addr, size);
				}
				break;
			case 'c':
				ret = dumpCalcParam(CODE_MAX, &arg[1], &addr, &size);
				if (ret == SUCCESS) {
					dumpCode(addr, size);
				}
				break;
			case 'd':
				ret = dumpCalcParam(DRAM_MAX, &arg[1], &addr, &size);
				if (ret == SUCCESS) {
					dumpDram(addr, size);
				}
				break;
			case 'x':
				ret = dumpCalcParam(XDATA_MAX, &arg[1], &addr, &size);
				if (ret == SUCCESS) {
					dumpXdata(addr, size);
				}
				break;
			default:
				ret = dumpCalcParam(XDATA_MAX, &arg[0], &addr, &size);
				if (ret == SUCCESS) {
					dumpXdata(addr, size);
				}
				break;
		}
	}
	
	if (ret != SUCCESS) {
		cmdMemHelpShow();
	}

#if WDT_EN	
	dbgWdtSet(1);
#endif /* WDT_EN */	
}

#define CMD_MEM_TYPE_IDATA	0
#define CMD_MEM_TYPE_XDATA	1
#define CMD_MEM_TYPE_CODE	2
#define CMD_MEM_TYPE_DRAM	3

/*
 * Basic memory fill routine. An enter repeats the fill.
 */
void
cmdMemFill(
	UINT8 *param
)
{
	UINT32 sAddr, eAddr, fillLen;
	UINT16 value;
	UINT8 type;

	cmdWhiteSpaceStrip(&param);

	if ( *param == '\0' ) {
		//dbgPrintf(CSTR_CMD_MEM_HELP);
        cmdMemHelpShow();
		return;
	}

	//check mem type

	do {
		if (param[0] == 'i') {
			type = CMD_MEM_TYPE_IDATA;
		} else if (param[0] == 'x') {
			type = CMD_MEM_TYPE_XDATA;
		} else if (param[0] == 'c') {
			type = CMD_MEM_TYPE_CODE;
		} else if (param[0] == 'd') {
			type = CMD_MEM_TYPE_DRAM;
		} else {

		//	default mem data is xdata

			type = CMD_MEM_TYPE_XDATA;
			break;
		}
		cmdTokenStrip(&param);
		cmdWhiteSpaceStrip(&param);
		if ( *param == '\0' ) {
			//dbgPrintf(CSTR_CMD_MEM_HELP);
			cmdMemHelpShow();
			return;
		}
	} while (0);

	sAddr = strtoul(param, 0, 0);
	cmdTokenStrip(&param);
	cmdWhiteSpaceStrip(&param);
	if ( *param == '\0' ) {
		//dbgPrintf(CSTR_CMD_MEM_HELP);
        cmdMemHelpShow();
		return;
	}

	/*
	 *  Check to see if the address has a plus value to indicate a length.
	 */
	if ( *param == '+' ) {
		/*
		 * Need to be one less when a length to get the correct end
		 * address as the equal test is for <= not less.
		 */
		eAddr = sAddr - 1;
	}
	else {
		eAddr = 0;
	}
	eAddr += strtoul(param, 0, 0);

	/*
	 * Check the source address is less than or equal to the end address.
	 */

	if ( sAddr > eAddr ) {
		//dbgPrintf(CSTR_CMD_MEM_HELP);
        cmdMemHelpShow();
		return;
	}

	fillLen = 1;
	fillLen += (eAddr - sAddr);   //calculate the file

	cmdTokenStrip(&param);
	cmdWhiteSpaceStrip(&param);
	if ( *param == '\0' ) {
		//dbgPrintf(CSTR_CMD_MEM_HELP);
        cmdMemHelpShow();
		return;
	}

	value = (UINT16)strtoul(param, 0, 0);

	if (type == CMD_MEM_TYPE_DRAM) {

	// configure dram-fill start address

		XBYTE[REG_Dram_IndirectAddr_B0] = *((UINT8*)(&sAddr) + 3);	//set SDRAM starting addr
		XBYTE[REG_Dram_IndirectAddr_B1] = *((UINT8*)(&sAddr) + 2);
		XBYTE[REG_Dram_IndirectAddr_B2] = *((UINT8*)(&sAddr) + 1);
		XBYTE[REG_Dram_IndirectAddr_B3] = *((UINT8*)(&sAddr));
	}

	do {
		switch (type) {
			case CMD_MEM_TYPE_IDATA:
				*((volatile UINT8 idata *)((UINT16)sAddr)) = *((UINT8*)(&value) + 1);//LSB
				break;
			case CMD_MEM_TYPE_XDATA:
				*((volatile UINT8 xdata *)((UINT16)sAddr)) = *((UINT8*)(&value) + 1);//LSB
				break;
			case CMD_MEM_TYPE_CODE:
				//*((volatile UINT8 code *)((UINT16)sAddr)) = *((UINT8*)(&value) + 1);//LSB
				break;
			case CMD_MEM_TYPE_DRAM:

			// Fill by word ONLY

				while (XBYTE[REG_Dram_IndirectPrefetch]);
				XBYTE[REG_Dram_IndirectRData_Low] = *((UINT8*)(&value) + 1);	//LSB
				XBYTE[REG_Dram_IndirectRData_High] = *((UINT8*)(&value));	//MSB
				break;
			default:
				break;
		}
		sAddr++;
	}while(--fillLen);
    printf("fill end:0x%lx\n", sAddr);
}

#endif
