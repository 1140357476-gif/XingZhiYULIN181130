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
 * @file      gui_evt.c
 * @brief     GUI related function
 * @author    Phil Lin
 * @since     2011-09-02
 * @date      2011-09-02
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "gui_evt.h"
#include "gui_api.h"
#include "gui_dbg.h"
#include "gui_tp.h"
#include "osd_api.h" 
#define SYS_GUI_INTERNAL // for libGui internal use
#include "func_gui_def.h"
#include "os_msg.h"
#include "dbg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define GUI_EVT_DBG_EN   0

#if GUI_EVT_DBG_EN
#define guiEvtPrintf printf
#else
#define guiEvtPrintf ;/DBG_SLASH
#endif
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define ABS(x,y)  ((x)>(y)) ? ((x)-(y)) : ((y)-(x))
#define MIN(x,y)  ((x)<(y)) ? (x) : (y)
#define MAX(x,y)  ((x)>(y)) ? (x) : (y)
#define guiSlidePixPdSet(a, b)	do{guiPixPd.x = (a); guiPixPd.y = (b);}while(0)
#define guiSlidePdPosSet(a, b)	do{guiSlidePdPos.x = (a); guiSlidePdPos.y = (b);}while(0)
#define guiSlidePdPosClr(a, b)	do{guiSlidePdPos.x = (a); guiSlidePdPos.y = (b);}while(0)
#define guiSlideNewPosClr(a, b)	do{guiNewPos.x = (a); guiNewPos.y = (b);}while(0)
#define guiSlidePixPuSet(a, b)	do{guiPixPu.x = (a); guiPixPu.y = (b);}while(0)
#define guiSlidePixMvXGet() (ABS(guiPixPd.x, guiPixPu.x))
#define guiSlidePixMvYGet() (ABS(guiPixPd.y, guiPixPu.y))

//#define guiSlidePixRst()	do{guiSlidePixPdSet(-1, -1); guiSlidePixPuSet(-1, -1);}while(0)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern SYS_GUI_DCL pFuncGui code funcGuiCbArray[];
extern UINT8 xdata guiDbgEn; 
UINT8 xdata guiPenDnSts;
UINT8 xdata guiPenDnFirst=0;
static UINT8 guiFirstPenDn=0;
static guiObj_t *pGuiObjAct;
static guiObj_t *pGuiObjPenDn;
static guiPix_t guiPixPd, guiPixPu;
static guiPix_t guiLength,guiNewPos,guiOldPos,guiSlidePdPos;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 guiObjEvtHdl(SINT16 x, SINT16 y)
 * @brief     guiObjEvtHdl
 * @param     [in] x
 * @param     [in] y
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjEvtHdl(
	SINT16 x,
	SINT16 y
)
{
	guiObj_t *pObj;
	UINT16 msg = SP1K_MSG_NULL;
	pFuncGui *ptr = funcGuiCbArray; /* call tree correction */
	guiObj_t *pAct = pGuiObjAct;

#if 0	
	if(guiDbgEn) {
		if (x != -1 && y != -1) {
			osdIndexSave();
			osdPenCfg(0x40 | 0x3c, 2);
			osdRectDraw(x>2?x-2:x, y>2?y-2:y, 4, 4);
			osdIndexRestore();
		}
	}
#endif

	//guiEvtPrintf("X:%i, Y:%i\n", x, y);

	if (x != -1 && y != -1) 
	{
		if (!guiPenDnSts)
		 {
			guiPenDnSts = 1;
			guiPenDnFirst=1;
			//printf("D:%i,%i\n", x, y);
			guiSlidePixPdSet(x, y);
			guiObjLocate(x, y, &pGuiObjPenDn);
		}
	}

	if (guiObjLocate(x, y, &pObj) == SUCCESS) 
	{
		/* pen down (idle->active) */
		if (!pAct) 
		{

			/* record the active object */
			pAct = pObj;
			//pGuiObjPenDn = pAct; /* save the pen down object */
			//intf("DN:(%d,%d,)=>(%d,%d),Sts=%bu\n",pGuiObjPenDn->x0,pGuiObjPenDn->y0,pGuiObjPenDn->x1,pGuiObjPenDn->y1,guiPenDnSts);
			if (guiPenDnFirst) 
			{
				guiEvtPrintf("pd->");
				//guiPenDnSts = 1;
 				
				/* pen down event */
				if (pAct->pObjExt && pAct->pObjExt->pPenDn) 
				{
					pAct->pObjExt->pPenDn(pAct);
				} 
				else 
				{
					guiEvtPrintf("N");
				}
				guiPenDnFirst=0;
				tpScanProcHdl(TP_PROC_PD);
			} 
			else
			{
				/* pen move the active */
				guiEvtPrintf("G2A->");
			}
			
			/* focus get */
			guiEvtPrintf("act\n");
			if (pAct->pObjExt && pAct->pObjExt->pFocusGet) 
			{
				if (!pAct->attr.focus)
				{	
					pAct->pObjExt->pFocusGet(pAct);
				} 
				else
				{
					guiEvtPrintf("FAG\n");//Focus already get
				}
			}
			else 
			{
				if(guiDbgEn) 
				{
					guiObjDraw(pAct, 0xc0 | 0x10);
				}
				guiEvtPrintf("N");
			}
			pAct->attr.focus = 1;
		}
		else 
		{
			if (pAct == pObj) 
			{
				static UINT8 countNum;
				guiEvtPrintf("r\n");
				if(countNum==25){
					if(pAct->pObjExt && pAct->pObjExt->msg)
					{
						osMsgPost(SP1K_MSG_GUI_PEN_REPEATE);
					}
					countNum=0;
				}
				countNum++;
				/* pen repeate (active->active) */
				/* start the background repeate parsing in time0 ISR */
			}
			else
			{
				guiEvtPrintf("A2A->");		
				/* pen move to another object */
				/* foces lost event handle */
				guiEvtPrintf("lost\n");				
				if (pAct->pObjExt && pAct->pObjExt->pFocusLost)
				{
					if (pAct->attr.focus) 
					{
						pAct->pObjExt->pFocusLost(pAct);
					} 
					else 
					{
						guiEvtPrintf("FNF\n");//focus not found
					}
					tpScanProcHdl(TP_PROC_PD);
				}
				else 
				{
					if(guiDbgEn) 
					{
						guiObjDraw(pAct, 0x40 | 0x3c);
					}
					guiEvtPrintf("N");
				}
				pAct->attr.focus = 0;

				guiEvtPrintf("act\n");
				/* get new obj handle */
				pAct = pObj;
				
				//printf("NEW:(%d,%d,)=>(%d,%d)\n",pAct->x0,pAct->y0,pAct->x1,pAct->y1);
				if (pAct->pObjExt && pAct->pObjExt->pFocusGet) 
				{
					if (!pAct->attr.focus) 
					{	
						pAct->pObjExt->pFocusGet(pAct);
					}
					else
					{
						guiEvtPrintf("FAG\n");//Focus already get
					}
				} 
				else 
				{
					if(guiDbgEn) 
					{
						guiObjDraw(pAct, 0xc0 | 0x10);
					}
					guiEvtPrintf("N");
				}
				pAct->attr.focus = 1;
			}
		}
		
	}
	else
	{
		//printf(" @#$ %bu\n",tpScanProcGet());


		/* pen up (Active->idle) */
		if (pAct) 
		{
			guiEvtPrintf("lst\n");
			/* foces lost event handle */
			if (pAct->pObjExt && pAct->pObjExt->pFocusLost)
			{
				if (pAct->attr.focus)
				{
					pAct->pObjExt->pFocusLost(pAct);
				} 
				else
				{
					guiEvtPrintf("FNF\n");//focus not found
				}
			} 
			else 
			{
				if(guiDbgEn)
				{
					guiObjDraw(pAct, 0x40 | 0x3c);
				}
				guiEvtPrintf("N");
			}
			pAct->attr.focus = 0;
			if (tpScanProcGet() & TP_PROC_PU) 
			{
				/* pen up event handle */
				if (guiPenDnSts) 
				{
					guiPenDnSts = 0;

					guiEvtPrintf("pu->");
					if (guiObjOpModeGet(GUI_OP_MODE_PENUP) == MODE_PENUP_ALWAYS \
						|| (pGuiObjPenDn == pAct)) 
					{
						if (pAct->pObjExt && pAct->pObjExt->msg)
						{
							msg = pAct->pObjExt->msg;//avoid pObj->pPenUp modifying msg
						}
						
						if (pAct->pPenUp)
						{
							pAct->pPenUp(pAct); 	
						}
						else
						{
							guiEvtPrintf("N");
						}
						
						if (msg != SP1K_MSG_NULL) 
						{
							//printf("msg=0x%x\n",msg);
							osMsgPost(msg);
						}
					}
				pAct = NULL;
					pGuiObjPenDn = NULL;
					tpScanProcHdl(TP_PROC_PU);
				}		
			} 

		
		}
		else
		{
			/* idle to idle */		
			//guiPenDnSts = 0;
			pGuiObjPenDn= NULL;
			pAct = NULL;

			
		}
	}

	pGuiObjAct = pAct;

	if (x != -1 && y != -1)
	{
		guiSlidePixPuSet(x, y);
		guiObjLocate(x, y, &pGuiObjAct);
	}
	else
	{
		tpScanProcHdl(TP_PROC_PD|TP_PROC_PU);
		guiSlidePdPosClr(-1,-1);
		guiSlideNewPosClr(-1,-1);
		guiPenDnSts = 0;
	}

	return SUCCESS;
}


void guiObjFisrtPdStsClr(void)
{
	guiFirstPenDn=0;
}
/**
 * @fn        void guiObjActHdlGet(guiObj_t **pObj)
 * @brief     guiObjActHdlGet
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiObjSlideHdl(
		SINT16 x,
		SINT16 y
)
{

	if (x != -1 && y != -1) 
	{

		if(!guiFirstPenDn){
			guiFirstPenDn=1;
			guiSlidePdPosSet(x, y);
			//printf("D(%d,%d)->\n",x,y);
			guiOldPos.x = guiNewPos.x = x;
			guiOldPos.y = guiNewPos.y = y;
		}
		else
		{
			if ((ABS(x ,  guiNewPos.x)) >= 1 || (ABS(y, guiNewPos.y))>= 1) 
			{
				guiNewPos.x = x;
				guiNewPos.y = y;
				//printf("Nx=%d,y=%d\n",x,y);
			}

			if ((ABS(guiOldPos.x ,	guiNewPos.x)) >= guiObjOpParaGet(GUI_OP_PARA_SLIDE_THD_X) || \
				(ABS(guiOldPos.y, guiNewPos.y))>=guiObjOpParaGet(GUI_OP_PARA_SLIDE_THD_Y)
			) 
			{
				osMsgPost(SP1K_MSG_GUI_PEN_SLIDE);
				//printf("N(%d,%d)\n",x,y);
				guiLength.x = guiNewPos.x - guiOldPos.x;
				guiLength.y = guiNewPos.y - guiOldPos.y;
				guiOldPos.x = guiNewPos.x = x;
				guiOldPos.y = guiNewPos.y = y;
			}
		
		}
	}
	return SUCCESS;
}
/**
 * @fn        void guiObjActHdlGet(guiObj_t **pObj)
 * @brief     guiObjActHdlGet
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
void
guiObjActHdlGet(
	guiObj_t **pObj
)
{
	*pObj = pGuiObjAct;
}

/**
 * @fn        UINT8 guiSlidePixGet(guiPix_t *pixS, guiPix_t *pixE)
 * @brief     guiSlidePixGet
 * @param     [in] pixS
 * @param     [in] pixE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-11
 * @todo      N/A
 * @bug       N/A
*/
UINT8
guiSlidePixGet(
	guiPix_t *pixS,
	guiPix_t *pixE
)	
{
	if (pixS) {
		pixS->x = guiPixPd.x;
		pixS->y = guiPixPd.y;
	}
	if (pixE) {
		pixE->x = guiPixPu.x;
		pixE->y = guiPixPu.y;
	}
	return SUCCESS;
}

UINT8 guiCurrenPosGet(guiPix_t *pixS,guiPix_t *pixE)
{
	if (pixS) 
	{
		pixS->x = guiSlidePdPos.x;
		pixS->y = guiSlidePdPos.y;
	}
	
	if (pixE) 
	{
		pixE->x = guiNewPos.x;
		pixE->y = guiNewPos.y;
	}
	return SUCCESS;
}


UINT8 guiMoveLengthGet(guiPix_t *pos)
{
	if(pos)
	{
		pos->x = guiLength.x;
		pos->y = guiLength.y;
	}
	return SUCCESS;
	
}
