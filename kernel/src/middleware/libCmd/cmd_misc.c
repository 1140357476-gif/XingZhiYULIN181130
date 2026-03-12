#define  CMD_MISC
#include "general.h"
#include "cmd.h"
#include "hal.h"
#include "hal_dram.h"
#include "dbg_def.h"

#define isDigital(a)   ((a) >= '0' && (a) <= '9')

#if 0
#define cmdMiscPrintf printf
#else
#define cmdMiscPrintf /DBG_SLASH
#endif

void
cmdWhiteSpaceStrip(
	UINT8 **pptr
)
{
	UINT8 *ptr;

	ptr = *pptr;
	while ( *ptr != '\0' ) {
		if ((*ptr != ' ')  &&
		    (*ptr != '\t') &&
		    (*ptr != '\n') &&
		    (*ptr != '\r')) {
			break;
		}
		else {
			ptr++;
		}
	}
	*pptr = ptr;
}

void
cmdTokenStrip(
	UINT8 **pptr
)
{
	UINT8 *ptr;

	ptr = *pptr;
	while ( *ptr != '\0' ) {
		if ((*ptr == ' ')  ||
		    (*ptr == '\t') ||
		    (*ptr == '\n') ||
		    (*ptr == '\r')) {
			break;
		}
		else {
			ptr++;
		}
	}
	*pptr = ptr;
}

/**
 * @brief	parse the arguments in cmd line buffer.
 * @param	pptr = [in] the pointer address which point to cmd line buffer.
 * @param	pArg = [out] buffer for arguments.
 * @param	cnt = [in] the count of the argument for parsing.
 * @param	bakPtr = [in] option to backup the pointer which point to cmd line buffer.
 * @retval	NONE.
 * @see
 * @author	Phil Lin
 * @since	2010-01-12
 * @todo	N/A
 * @bug		N/A
*/
void cmdArgGet(UINT8** pptr, UINT32 pArg[], UINT8 cnt, UINT8 bakPtr)
{
    UINT8* ptrBak, *ptrTmp0, *ptrTmp1;
    UINT8 i;
    UINT16 tokLen;

    ptrBak = *pptr;
    if (cnt > 16) cnt = 16;
    
    for (i = 0; i < cnt; i++) {
    	if (**pptr == '\0') {
			pArg[i] = 0;
			continue;
    	}
    	if (isDigital(**pptr)) {
			pArg[i] = strtoul(*pptr, &ptrTmp0, NULL);
        }

        /* get the lenth of current word */
        ptrTmp1 = *pptr;
        cmdTokenStrip(pptr);
        tokLen = *pptr - ptrTmp0;
        *pptr = ptrTmp1;
        
        /* check if the argument is a string or integer */
        if (!tokLen) {
			cmdMiscPrintf("arg[%bx].i: %lx\n", i, (UINT32)pArg[i]);
        } else {
            pArg[i] = (UINT32)*pptr;
            cmdMiscPrintf("arg[%bx].s: %s\n", i, *pptr);
        }

        cmdTokenStrip(pptr);
        cmdWhiteSpaceStrip(pptr);
    }

    if (bakPtr) *pptr = ptrBak;
}

/**
 * @brief	compare two block of memory in dram.
 * @param	addrWord0 = [in] the linear word address of memory A.
 * @param	addrWord1 = [in] the linear word address of memory B.
 * @param	cmpLenthWord = [in] the word size to compare
 * @param	stopCnt = [in] stop compare when error words is over stopCnt
 * @retval	0: SUCCESS
 *          1: FAIL
 * @see
 * @author	Phil Lin
 * @since	2010-01-12
 * @todo	N/A
 * @bug		N/A
*/
UINT8 cmdMemCmp(UINT32 addrWord0, UINT32 addrWord1, UINT32 cmpLenthWord, UINT32 stopCnt)
{

#if TAG_TODO
#define swap(a, b)  {UINT32 tmp; tmp = (a); (a) = (b); (b) = tmp;}
#define BUF_LENTH  0x400

    UINT8  buf0[BUF_LENTH];
    UINT8  buf1[BUF_LENTH];
    UINT32 i32, j32, readLen;
    UINT8 LSB, MSB;
    UINT8* pbuf;
    UINT8 ret = SUCCESS;

    cmdMiscPrintf("cmp 0x%lx VS 0x%lx, lenth:0x%lx\n", addrWord0, addrWord1, cmpLenthWord);
    j32 = 0;
    while(1) {
	dbgWdtFeed(100000);
        memset(buf0, 0, BUF_LENTH);
        memset(buf1, 0, BUF_LENTH);
        
        readLen = cmpLenthWord;
        
        HAL_DramStartAddrSet(addrWord0 + (j32 * (UINT32)BUF_LENTH >> 1), 1);
        i32 = 0;
        pbuf = &buf0[0];
        while(1) {
            HAL_DramWordDataRead(&LSB, &MSB);
            *pbuf++ = LSB;
            *pbuf++ = MSB;
            i32++;
            readLen--;
            
            if (i32 >= (BUF_LENTH >> 1)) break;
            if (readLen == 0) break;
        }
        swap(readLen, cmpLenthWord);
        
        HAL_DramStartAddrSet(addrWord1 + (j32 * (UINT32)BUF_LENTH >> 1), 1);
        i32 = 0;
        pbuf = &buf1[0];
        while(1) {
            HAL_DramWordDataRead(&LSB, &MSB);
            *pbuf++ = LSB;
            *pbuf++ = MSB;
            i32++;
            readLen--;
            
            if (i32 >= (BUF_LENTH >> 1)) break;
            if (readLen == 0) break;
        }

        for (i32 = 0; i32 < BUF_LENTH; i32++) {
            if (buf0[i32] != buf1[i32]) {
                cmdMiscPrintf("cmp error! addr0: 0x%lx, addr1: 0x%lx >> ", \
                    addrWord0 + ((i32 + j32 * (UINT32)BUF_LENTH) >> 1), \
                    addrWord1 + ((i32 + j32 * (UINT32)BUF_LENTH) >> 1));
                if (i32 & 0x01) {
                    cmdMiscPrintf("MSB "); 
                } else {
                    cmdMiscPrintf("LSB ");
                }
                cmdMiscPrintf("data0:0x%02bx, data1:0x%02bx\n", buf0[i32], buf1[i32]);
                ret = FAIL;
                
                stopCnt --;
                if(!stopCnt){
                    cmdMiscPrintf("error and stop cmp !\n");
                    cmpLenthWord = 0;
                    break;
                }
            }
        }        

        if (cmpLenthWord == 0) break;
        j32++;
    }

    if (ret == SUCCESS) cmdMiscPrintf("cmp success!\n");
#else
	UINT8 ret = SUCCESS;

	addrWord0 = addrWord0;
	addrWord1 = addrWord1;
	cmpLenthWord = cmpLenthWord;
	stopCnt = stopCnt;	
#endif    
    return ret;
}

/**
 * @fn cmdArgValueGet
 * @brief	Get UINT32 args from command string
 * @param[in] 	cmdStr	Specify the command string:
 *\n          [out]	arg		arg buf for saving values
 *\n          [in]		argNum	max arg num
 * @retval arg num in fact
 */
UINT8 cmdArgValueGet(
    UINT8 *cmdStr,
    UINT32 *arg,
    UINT8 argNum
)
{
    UINT8 i;

    if(cmdStr == 0 || *cmdStr == 0){
        argNum = 0;
		return 0;
    }
    
    cmdMiscPrintf("arg[%bu]: ", argNum);
    for (i = 0; i < argNum; i++) {
        if(*cmdStr == 0)
            break;
        
        arg[i] = strtoul(cmdStr, NULL, NULL);
        cmdMiscPrintf("%lx, ", (UINT32)arg[i]);

        cmdTokenStrip(&cmdStr);
        cmdWhiteSpaceStrip(&cmdStr);
    }
    cmdMiscPrintf("\n");

    return i;
}
/**
 * @fn cmdRst
 * @brief	Get UINT32 args from command string
 * @param[in] 	cmdStr	Specify the command string:
 *\n          [out]	arg		arg buf for saving values
 *\n          [in]		argNum	max arg num
 * @retval arg num in fact
 */

void cmdRst(void)
{
    /* software reset */
    ((void (code*)(void))0)();
}
