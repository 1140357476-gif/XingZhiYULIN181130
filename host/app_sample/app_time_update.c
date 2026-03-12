#include "general.h"
#include "cardlink.h"
#include "Dos32.h"
#include "initio.h"

#include "Dbg_def.h"

#include "sp1k_hal_api.h"
#include "sp1k_util_api.h"


#define tagbuffsize	16
#define Cardtxt_SRAMStartaddr	0x3800//0x1c00
#define Cardtxt_SRAMbuffsize		64
#define Filename	"TIMESYNC.TXT"//"CONFIG.TXT"


UINT8 txtbuff[Cardtxt_SRAMbuffsize];

void
strWhiteSpaceStrip(
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


/**
 * @brief		TAG.TXT file detect
 * @param	NULL
 * @retval	0:fail;	1:sucess
 * @see
 * @author	wang kai
 * @since		2009-05-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 CardTxt_Detect ( void ) 
{
	UINT8  DevId;
	UINT8 Flag1;
	UINT8 sts= FALSE;
	
	DevId = DOS_CurrDriverGet();
	if ( DevId != 0 ) {
		printf("NO SD!\n");
		return FALSE;
	}
	
	//DIR_Change("D:\\");
		
	Flag1 = DOS_FileOpen("TIMESYNCTXT", 'r');
	printf("hd:%bx\n",Flag1);
	if (Flag1) {
		sts = TRUE;
	}else {
		sts = FALSE;
	}
	DOS_FileClose(Flag1);

	return sts;
}

UINT8 xdata tagbuff[tagbuffsize];
/**
 * @brief		detect the tag content and be stored in tagbuff
 * @param	dramaddr:   the first address after tag in dram
 * @retval	0: fail;	1:sucess
 * @see
 * @author	wang kai
 * @since		2009-05-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 CardTxt_Tagdetect ( UINT32 *dramaddr ) 
{
	UINT32 size;
	UINT32 startAddr = K_SDRAM_KeySoundAddr;//K_SDRAM_TAGParseAddr;
	UINT32 tempAddr;
	UINT8 X0,Y0,index;
	UINT8 j, i = 0;
	UINT8 t = 0;
	UINT8 temp = 0;
	UINT8 Flag1;	

	sp1kHalDRAMSetStartAddr(startAddr, 0);	
	for ( index = 0 ; index < ((Cardtxt_SRAMbuffsize+tagbuffsize)>>1) ;index++) 	
		
		sp1kHalDRAMWriteWord('\0','\0');
			
	Flag1 = sp1kDramFileLoad(Filename, startAddr, &size);
	
	if ( Flag1 == 0 ) {
		//printf("read file fail!flag1=%bx\n",Flag1);
		return FALSE;
	}
	
	do {		
		//printf("startaddr=%lx\n",startAddr);
		sp1kHalDRAMSetStartAddr(startAddr++, 1);
		sp1kHalDRAMReadWord(&X0, &Y0);
			//i++;
		if ( X0 == '[' ) {
			tempAddr=startAddr;
			for( j = 0; j <= tagbuffsize; j ++ ) {
				
				tagbuff[j] = X0;
				if( tagbuff[j] == ']' ) {
					temp = Y0;
					break;
				}
				tagbuff[++j] = Y0;
				if( tagbuff[j] == ']' )
					break;
			//	printf("%bx,tagbuff=%bx\n",j,tagbuff[j-1]);
				sp1kHalDRAMReadWord(&X0, &Y0);	
				i++;
				}
			//break;
			printf("j=%bx",j);
			}
		else if( Y0 == '[' ) {
			tempAddr = startAddr;
			for( j = 0; j <= tagbuffsize; j ++ ) {
				
				tagbuff[j] = Y0;
				if(tagbuff[j] == ']' )
					break;
				
			//	printf("%bx,tagbuff=%bx\n",j,tagbuff[j-1]);
				sp1kHalDRAMReadWord(&X0, &Y0);i++;	
				tagbuff[++j] = X0;
				if(tagbuff[j] == ']') {
					temp = Y0;
					break;
					}
				}
			//break;
			//printf("j=%bx\n",j);
			}		
		//printf("x=%bx,y=%bx\n",X0,Y0);
		} while ( tagbuff[j] != ']' && (X0 !='\0'||Y0 != '\0'));
	//printf("i=%bx,tempaddr=%lx,startaddr=%lx\n",i,tempAddr,startAddr);
	tempAddr += i;
	*dramaddr = tempAddr;
	//printf("tempaddr=%lx\n",tempAddr);
	CPrintf(tagbuff);
	return TRUE;
}


/**
 * @brief		get the tagbuff address
 * @param	NONE
 * @retval	the buffer address
 * @see
 * @author	wang kai
 * @since		2009-05-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 * CardTxt_Tagget( void )
{
	UINT8  * addr; 
	
	addr = tagbuff;
	return  (addr);
}


/**
 * @brief		DMA the str content after tag from dram to sram
 * @param	none
 * @retval	the first address of str in sram
 * @see
 * @author	wang kai
 * @since		2009-05-20
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/		
UINT8 * CardTxt_DMA2SRAM ( void )
{
	UINT8 ptr,*straddr;
	UINT8 i;
	UINT32 tagAddr = 0;
	
	ptr = CardTxt_Tagdetect(&tagAddr);
	//printf("tagaddr=%lx\n",tagAddr);
	if ( ptr == FALSE )
		return NULL;
	
	//printf("tag detect ok!\n");
	HAL_DmaSetSramAddr(Cardtxt_SRAMStartaddr);
	HAL_DmaSetDramAddr(tagAddr);
	HAL_DmaDo(0,1,Cardtxt_SRAMbuffsize,0,0);
		
	straddr = &XBYTE[Cardtxt_SRAMStartaddr];
	strWhiteSpaceStrip(&straddr);
	for (i = 0 ;i < Cardtxt_SRAMbuffsize && (*straddr) != '\0';i++ ) {
		
		txtbuff[i] = *(straddr++);
		putchar(txtbuff[i]);
		
		}
	//printf("\nxbyte addr =%lx,%bx\n",straddr,strlen(txtbuff));
	CPrintf(txtbuff);
	
	DOS_FileDel(Filename);
	return txtbuff;	
}


void appCardTxt_DATEParse ( void )
{	
	UINT8 * p,i;
	UINT8 temp[2];
	UINT8 temp0;
	UINT8 year,month,day,hour,minute,second;
	dateStc_t rtcDate = {0};
	
	p = CardTxt_DMA2SRAM();//&XBYTE[Cardtxt_SRAMStartaddr];
	
	if ( strncmp(CardTxt_Tagget(),"[Date]",6) == 0 ) {

		strWhiteSpaceStrip(&p);
		
		for ( i= 0; i < 3; i ++ ) {
			p+=2;
			temp[0] = *p;
			p++;
			temp[1] =*p;
			temp0 = (temp[0]-0x30)*10 + (temp[1]-0x30);
			if ( i == 0 ) {
				year = temp0;
				continue;
				}
			else if ( i == 1 ) {
				month = temp0;
				continue;
				}
			else if ( i == 2 ) {
				day = temp0;
				continue;
				}
		}
		p++;
		strWhiteSpaceStrip(&p);
		for( i = 0; i < 3; i ++) {
			temp[0] = *p;
			p++;
			temp[1] =*p;
			temp0 = (temp[0]-0x30)*10 + (temp[1]-0x30);
			if( i == 0 ) {
				hour = temp0;
				p+=2;
				continue;
				}
			else if( i == 1 ) {
				minute = temp0;
				p+=2;
				continue;
				}
			else if( i == 2 ) {
				second = temp0;
				continue;
				}
		}
		printf ("%bx/%bx/%bx/%bx/%bx/%bx\n",year,month,day,hour,minute,second);
		
		rtcDate.Year = year;
		rtcDate.Month = month;
		rtcDate.Day = day;
		rtcDate.Hour = hour;
		rtcDate.Minute = minute;
		rtcDate.Second = second;
		HAL_GlobalWriteRTC(&rtcDate);
		
		}
}

void appTimeSync(void)
{
	UINT8 sts,en;
	
	ENTER_CRITICAL(en);
	
	sts = CardTxt_Detect (); 
	if(sts!= TRUE) {
		printf("No text file!\n");
	}
	else {	
		appCardTxt_DATEParse();		
	}	
	//DIR_Change("D:\\DCIM\\");
	//DIR_Change("100MEDIA");
	
	EXIT_CRITICAL(en);
}	
