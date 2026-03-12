/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#include "general.h"
#include "app_main.h"
#include "os_msg.h"
#include "osd_api.h"
#include "app_osd_icon.h"
#include "app_menu_tab.h"
#include "app_menu_core.h"
#include "app_menu_api.h"
#include "App_ui_para.h"

/**************************************************************************
 *                      G L O B A L    C O N S T A N T                    *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32	menuIndex = 0;
static UINT8	layerIndex = 0;
static UINT8	menuCurrItem;// = 0;
static UINT8	layerScrollTab[4];
static UINT8	menuScrollMode = SCROLL_MODE_PAGE;
UINT8 specialMenuCurrItemIdx;//for ui tool
UINT8 specialMenuOrder[30]={0};//for ui tool
UINT8 specialMenuItemNum;//for ui tool
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT16	menuProcStrIndexGet (UINT8 layer, UINT8 item, UINT8 sub);

 UINT8 menuSpecialMoveUp(void)
 {
	if(specialMenuCurrItemIdx == 0)
	{
		specialMenuCurrItemIdx = specialMenuItemNum-1;
	}
	else
	{
		specialMenuCurrItemIdx--;
	}
	return SUCCESS;
 }
 UINT8 menuSpecialMoveDown(void)
 {
	if(specialMenuCurrItemIdx == specialMenuItemNum-1)
	{
		specialMenuCurrItemIdx = 0;
	}
	else
	{
		specialMenuCurrItemIdx++;
	}
	return SUCCESS;
 }
 /**************************************************************************
 *                                                                        *
 *  Function Name: menuProcMoveUp                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT8 menuProcMoveUp (void)
{
	UINT8 line, col;
	UINT8 nodeItem;
	UINT8 i;
	UINT8 item;

	menuLayerSizeGet(layerIndex, &line, &col);
	nodeItem = menuProcNodeItemGet(layerIndex, menuCurrItem);

	for (i=1; i<nodeItem; i++)
	{
		if (menuCurrItem < i)
			item = nodeItem + menuCurrItem - i;
		else
			item = menuCurrItem - i;

		if (menuProcIsActive(layerIndex, item))
		{
			if (menuCurrItem > item)
			{
				menuCurrItem = item;
				menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
				menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

				if (menuScrollMode == SCROLL_MODE_PAGE)
					layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
				else if (menuCurrItem < layerScrollTab[layerIndex])
				{
					layerScrollTab[layerIndex] = menuCurrItem;
				}
			}
			else
			{
				menuCurrItem = item;
				menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
				menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

				if (menuScrollMode == SCROLL_MODE_PAGE)
					layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
				else if ((menuCurrItem+1) > (layerScrollTab[layerIndex] + line))
				{
					layerScrollTab[layerIndex] = menuCurrItem+1-line;
				}
			}
			return SUCCESS;
		}
	}
	if (menuProcIsActive(layerIndex, menuCurrItem))
		return SUCCESS;

	return FAIL;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcMoveDown                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcMoveDown (void)
{
	UINT8 line, col;
	UINT8 nodeItem;
	UINT8 i;
	UINT8 item;

	menuLayerSizeGet(layerIndex, &line, &col);
	nodeItem = menuProcNodeItemGet(layerIndex, menuCurrItem);
	//printf("layerIndex:%bu, menuCurrItem:%bu, nodeItem:%bu\n", layerIndex, menuCurrItem, nodeItem);
	for (i=1; i<nodeItem; i++)
	{
		if ((menuCurrItem + i) >= nodeItem)
			item = menuCurrItem + i - nodeItem;
		else
			item = menuCurrItem + i;

		if (menuProcIsActive(layerIndex, item))
		{
			if (menuCurrItem < item)
			{
				menuCurrItem = item;
				menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
				menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

				if (menuScrollMode == SCROLL_MODE_PAGE)
					layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
				else if ((menuCurrItem+1) > (layerScrollTab[layerIndex] + line))
				{
					layerScrollTab[layerIndex] = menuCurrItem+1-line;
				}
			}
			else
			{
				menuCurrItem = item;
				menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
				menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

				if (menuScrollMode == SCROLL_MODE_PAGE)
					layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
				else if (menuCurrItem < layerScrollTab[layerIndex])
				{
					layerScrollTab[layerIndex] = menuCurrItem;
				}
			}
			//printf("menuIndex:%lx; layerIndex:%bu, menuCurrItem:%bu\n",menuIndex, layerIndex, menuCurrItem);
			return SUCCESS;
		}
	}
	if (menuProcIsActive(layerIndex, menuCurrItem))
		return SUCCESS;

	return FAIL;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcParent                                    *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcParent (void)
{
	layerScrollTab[layerIndex] = 0;
	menuIndex &= (~((UINT32)0xff << ((3-layerIndex)*8)));
	layerIndex--;
	menuCurrItem = ((menuIndex>>((3-layerIndex)*8))&0xff)-1;

	while (!menuProcIsActive(layerIndex, menuCurrItem))
	{
		if (menuProcMoveDown() == FAIL)
		{
			menuProcParent();
		}
	}

	return SUCCESS;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcChild                                     *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcChild (UINT8 sel)
{
	UINT8 line, col;

	if (!menuProcIsActive(layerIndex, menuCurrItem))
	{
		return FAIL;
	}

	menuCurrItem = sel;
	layerIndex++;
	menuIndex += (((UINT32)menuCurrItem+1) << ((3-layerIndex)*8));

	menuLayerSizeGet(layerIndex, &line, &col);

	layerScrollTab[layerIndex] = 0;
	if (menuScrollMode == SCROLL_MODE_PAGE)
		layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
	else if ((menuCurrItem+1) > (layerScrollTab[layerIndex] + line))
	{
		layerScrollTab[layerIndex] = menuCurrItem + 1 - line;
	}

	if (!menuProcIsActive(layerIndex, menuCurrItem))
	{
		if (menuProcMoveDown() == FAIL)
		{
			menuProcParent();
			return FAIL;
		}
	}
	return SUCCESS;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcCurrPosGet                                     *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 menuProcCurrPosGet (void)
{
	return menuIndex;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcCurrPosGet                                     *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 menuProcCurrRootGet (void)
{
	return (menuIndex & 0xff000000);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcCurrPosSet                                     *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcCurrPosSet (UINT32 pos)
{
	UINT8 i;
	UINT8 nodeItem;
	UINT8 line, col;

	for (i=0; i< 4; i++)
		layerScrollTab[i] = 0x00;

	layerIndex = 0;
	menuCurrItem = 0;
	menuIndex = pos;

	if ((pos & 0xff000000) == 0)
		return FAIL;

	for (i=0; i< 4; i++)
	{
		if (pos & 0xff000000)
		{
			layerIndex = i;
			menuCurrItem = ((pos >> 24) & 0x000000ff) - 1;
			nodeItem = menuProcNodeItemGet(layerIndex, menuCurrItem);

			if (nodeItem == 0)
				return FAIL;

			menuLayerSizeGet(layerIndex, &line, &col);

			if (menuScrollMode == SCROLL_MODE_PAGE)
				layerScrollTab[layerIndex] = (menuCurrItem/line)*line;
			else if (menuCurrItem >= line)
				layerScrollTab[i] = menuCurrItem - line + 1;

			pos <<= 8;
		}
		else
		{
			break;
		}
	}

	if (!menuProcIsActive(layerIndex, menuCurrItem))
	{
		if (menuProcMoveDown() == FAIL)
		{
			menuProcParent();
			return FAIL;
		}
	}

	return SUCCESS;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcCurrItemGet                                    *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcCurrItemGet (UINT8 layer)
{
	if (layer > 3)
		return 0;
	return (UINT8)(((menuIndex >> ((3-layer)*8)) & 0xff)-1);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcScrollGet                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcScrollGet (UINT8 layer)
{
	if (layer > 3)
		return 0;
	return layerScrollTab[layer];
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcLayerGet                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcLayerGet (void)
{
	return layerIndex;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcChildPtrGet                                    *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT16 menuProcStrIndexGet (UINT8 layer, UINT8 item, UINT8 sub)
{
	static UINT8 lastlayer = 0;
	static UINT8 lastitem = 0;
	static UINT8 lastsub = 0;
	static UINT16 strIndex = 0;
	static UINT32 lastindex = 0;

	UINT32 tmpIndex=0;
	UINT8 root;

	if (!menuIndex)
		return MENU_UNKNOWN;

	if ((layer != lastlayer) || (item != lastitem) || (sub != lastsub))
	{
		lastlayer = layer;
		lastitem = item;
		lastsub = sub;

		item++;
		sub++;
		switch (layer)
		{
			case 0:
				tmpIndex = (menuIndex & 0x00000000) | ((UINT32)item << 24) | ((UINT32)sub << 16);
				break;

			case 1:
				tmpIndex = (menuIndex & 0xff000000) | ((UINT32)item << 16) | ((UINT32)sub << 8);
				break;

			case 2:
				tmpIndex = (menuIndex & 0xffff0000) | ((UINT32)item << 8) | ((UINT32)sub << 0);
				break;

			case 3:
				tmpIndex = (menuIndex & 0xffffff00) | ((UINT32)item << 0);
				break;

			default:
				tmpIndex = 0;
				//printf("layer error\n");
				break;
		}

	   	root = (UINT8)(tmpIndex >> 24)-1;
		if ((tmpIndex & 0xff000000) != (lastindex & 0xff000000))
		{
			lastindex = tmpIndex;
			for (strIndex = CustomMenuRootStart[menuLanguageGet()][root]; strIndex < CustomMenuRootStart[menuLanguageGet()][root+1]; strIndex++)
			{
				if(tmpIndex == CustomMenuIndex[menuLanguageGet()][strIndex])
				{
					return strIndex;
				}
			}
		}
		else if (tmpIndex > lastindex)
		{
			lastindex = tmpIndex;
			for (strIndex = strIndex+1; strIndex < CustomMenuRootStart[menuLanguageGet()][root+1]; strIndex++)
			{
				if(tmpIndex == CustomMenuIndex[menuLanguageGet()][strIndex])
				{
					return strIndex;
				}
			}
		}
		else
		{
			lastindex = tmpIndex;
			for (strIndex = strIndex-1; strIndex >= CustomMenuRootStart[menuLanguageGet()][root]; strIndex--)
			{
				if(tmpIndex == CustomMenuIndex[menuLanguageGet()][strIndex])
				{
					return strIndex;
				}
				if (strIndex == 0)
				{
					break;
				}
			}
		}
		lastlayer = 0;
		lastitem = 0;
		lastsub = 0;
		strIndex = 0;
		lastindex = 0;
		return MENU_UNKNOWN;
	}
	else
		return strIndex;
}
#define OSD_CHAR_BYTE_IDX_GRP 0x08
/**************************************************************************
 *                                                                        *
 *  Function Name: strGet                                                 *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 strGet (UINT8 buf[], UINT8 bufSize, UINT16 index)
{
	UINT8 lgh=0;
	UINT8* pstr8;
	UINT8 preC,nextC=0;

	pstr8 = CustomStrPtr[menuLanguageGet()][index];

	while (*pstr8|| nextC)
	{
		if ((lgh+2) >= bufSize){
			break;
		}
		preC = *pstr8;
		*buf = *pstr8;
		buf++;
		pstr8++;
		lgh++;
		if(*pstr8==0 && preC<= OSD_CHAR_BYTE_IDX_GRP){
			pstr8++;
			nextC=*pstr8;
			pstr8--;
		}
	}
	*buf = '\0';
	buf++;
	*buf = '\0';

	return lgh;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcPtrGet                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8* menuProcPtrGet (UINT8 layer, UINT8 item)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff);
	if(strIndex == MENU_UNKNOWN)
		return NULL;
	return CustomMenuPtr[menuLanguageGet()][strIndex];
}

/**************************************************************************
 *                                                                        *
 *  Function Name: strGet                                                 *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuStrGet(UINT8 layer, UINT8 item,UINT8 buf[], UINT8 bufSize)
{
	UINT8 lgh=0;
	UINT8* pstr8 = menuProcPtrGet(layer,item);
	UINT8 preC,nextC=0;

	while (*pstr8 || nextC)
	{
		if ((lgh+2) >= bufSize){
			break;
		}
		preC = *pstr8;
		*buf = *pstr8;
		buf++;
		pstr8++;
		lgh++;
		if(*pstr8==0 && preC<= OSD_CHAR_BYTE_IDX_GRP){
			pstr8++;
			nextC=*pstr8;
			pstr8--;
		}
	}
	*buf = '\0';
	buf++;
	*buf = '\0';

	return lgh;

}


#if 0
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcPtrGetByPos                                    *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8* menuProcPtrGetByPos (UINT32 pos)
{
	UINT16  strIndex;
	UINT8 	root = (UINT8)(pos >> 24)-1;

	for (strIndex = CustomMenuRootStart[menuLanguageGet()][root]; strIndex < CustomMenuRootStart[menuLanguageGet()][root+1]; strIndex++)
	{
		if(pos == CustomMenuIndex[menuLanguageGet()][strIndex])
		{
			return CustomMenuPtr[menuLanguageGet()][strIndex];
		}
	}

	return NULL;
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcChildPtrGet                                    *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8* menuProcChildPtrGet (UINT8 layer, UINT8 item, UINT8 sel)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, sel);
	if(strIndex == MENU_UNKNOWN)
		return NULL;
	return CustomMenuPtr[menuLanguageGet()][strIndex];
}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcIconIDGet                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuChildStrGet(UINT8 layer, UINT8 item,UINT8 sel,UINT8 buf[], UINT8 bufSize)
{
	UINT8 lgh=0;
	UINT8* pstr8 = menuProcChildPtrGet(layer,item, sel);
	UINT8 preC,nextC=0;

	while (*pstr8 || nextC)
	{
		if ((lgh+2) >= bufSize){
			break;
		}
		preC = *pstr8;
		*buf = *pstr8;
		buf++;
		pstr8++;
		lgh++;
		if(*pstr8==0 && preC<= OSD_CHAR_BYTE_IDX_GRP){
			pstr8++;
			nextC=*pstr8;
			pstr8--;
		}
	}
	*buf = '\0';
	buf++;
	*buf = '\0';

	return lgh;

}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcIconIDGet                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT16 strWidthGet(UINT8 *str)
{
	UINT8 i = 0;
	UINT8 buf[64];
	UINT8 preC,nextC=0;

	while ( *str || nextC) {
		if (i +2 == 63)
			break;
		preC = *str;
		buf[i++] = *str++;
		if(*str==0 && preC<= OSD_CHAR_BYTE_IDX_GRP){
			str++;
			nextC=*str;
			str--;
		}
	}
	buf[i] = 0;
	return osdStrWidthGet(buf);
}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcIconIDGet                                      *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
#if 0
UINT8* menuProcIconPtrGet (UINT8 layer, UINT8 item)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff);
	if(strIndex == MENU_UNKNOWN)
		return NULL;
	return CustomIconPtr[menuLanguageGet()][strIndex];
}
#else
UINT8 menuProcIconIDGet (UINT8 layer, UINT8 item)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff);
	if(strIndex == MENU_UNKNOWN)
		return NULL;
	return CustomIconPtr[menuLanguageGet()][strIndex];
}
#endif

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcIconIDGetByPos                                 *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcIconIDGetByPos (UINT32 pos)
{
	UINT16  strIndex;
	UINT8 	root = (UINT8)(pos >> 24)-1;

	for (strIndex = CustomMenuRootStart[menuLanguageGet()][root]; strIndex < CustomMenuRootStart[menuLanguageGet()][root+1]; strIndex++)
	{
		if(pos == CustomMenuIndex[menuLanguageGet()][strIndex])
		{
			return CustomIconPtr[menuLanguageGet()][strIndex];
		}
	}

	return NULL;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcChildIconIDGet                                 *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcChildIconIDGet (UINT8 layer, UINT8 item, UINT8 sel)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, sel);
	if(strIndex == MENU_UNKNOWN)
		return ID_ICON_NONE;
	return CustomIconPtr[menuLanguageGet()][strIndex];
}
#if 0
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcItemLghGet                                *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcItemLghGet (UINT8 layer, UINT8 item)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff);
	if(strIndex == MENU_UNKNOWN)
		return 0;
	return CustomMenuItemLength[menuLanguageGet()][strIndex];
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcNodeItemGet                               *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcNodeItemGet (UINT8 layer, UINT8 item)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex == MENU_UNKNOWN)
		return 0;
	return CustomMenuNodeItem[menuLanguageGet()][strIndex];
}
#if 0
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcNodeLghGet                                *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcNodeLghGet (UINT8 layer, UINT8 item)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff);
	if(strIndex == MENU_UNKNOWN)
		return 0;
	return CustomMenuNodeLength[menuLanguageGet()][strIndex];
}
#endif
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcSubItemGet                                *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcSubItemGet (UINT8 layer, UINT8 item)
{
	UINT16  strIndex;
	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex == MENU_UNKNOWN)
		return 0;
	return CustomMenuSubItem[menuLanguageGet()][strIndex];
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcParaGet                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcParaGet(UINT8 layer, UINT8 item)
{
	UINT16 strIndex;
	UINT8 para;
	UINT8* pMPara = (UINT8*)appUiParaGet();

	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex != MENU_UNKNOWN)
	{
		para = CustomMenuParaTab[menuLanguageGet()][strIndex];
		if (para)
			return pMPara[para];
	}
	return 0;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcParaSet                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuProcParaSet(UINT8 layer, UINT8 item)
{
	UINT16 strIndex;
	UINT8 para;
	UINT8* pMPara = (UINT8*)appUiParaGet();

	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex != MENU_UNKNOWN)
	{
		para = CustomMenuParaTab[menuLanguageGet()][strIndex];
		if (para)
			pMPara[para] = item;
	}
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcIsActive                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcIsActive(UINT8 layer, UINT8 item)
{
	UINT16 strIndex;

	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex != MENU_UNKNOWN)
	{
		if (CustomMenuFunc[menuLanguageGet()][strIndex] != NULL)
		{
			return CustomMenuFunc[menuLanguageGet()][strIndex](item);
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcExecute                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcExecute(UINT8 layer, UINT8 item, UINT16 msg)
{
	UINT16 strIndex;

	strIndex = menuProcStrIndexGet(layer, item, 0xff);

	if(strIndex != MENU_UNKNOWN)
	{
		if (CustomMenuExe[menuLanguageGet()][strIndex] != NULL)
		{
			return CustomMenuExe[menuLanguageGet()][strIndex](layer, item, msg);
		}
		else
		{
			return MENU_CALL_DEFAULT;
		}
	}
	return MENU_CALL_DEFAULT;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuScrollModeSet                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuScrollModeSet(UINT8 mode)
{
	if(mode < 2)
	{
		menuScrollMode = mode;
		return TRUE;
	}
	return FALSE;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcDbgMsg                                     *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcDbgMsg (UINT8 mode)
{
	switch (mode) {
	case 0:
		break;
	default:
		#if 0
		printf("menuIndex = 0x%08lx\n",menuIndex);
		#endif
		#if 0
		printf("layerIndex = %bu\n",layerIndex);
		printf("menuCurrItem = %bu\n",menuCurrItem);
		printf("layerScrollTab 0,1 = %bu,%bu\n",layerScrollTab[0],layerScrollTab[1]);
		printf("layerScrollTab 2,3 = %bu,%bu\n\n",layerScrollTab[2],layerScrollTab[3]);
		#endif
		break;
	}
	return 0;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcTPSet                                          *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
 #if FUNC_HOST_TOUCH_PANEL
UINT8 menuProcTPSet (UINT8 tpLine, UINT8 isChild)
{
	UINT8 line, col;
	UINT8 nodeItem;
	UINT8 item;

	if (isChild) isChild = 1;

	menuLayerSizeGet(layerIndex + isChild, &line, &col);
	nodeItem = menuProcNodeItemGet(layerIndex + isChild, 0);

	item = layerScrollTab[layerIndex + isChild] + tpLine;
	//if ((tpLine < line) && (item < nodeItem))
	{
		if (menuProcIsActive(layerIndex + isChild, item))
		{
			if (isChild)
				layerIndex++;
			menuCurrItem = item;
			menuIndex &= ~((UINT32)0x000000ff << ((3-layerIndex)*8));
			menuIndex += ((UINT32)(menuCurrItem+1) << ((3-layerIndex)*8));

			return SUCCESS;
		}
	}

	return FAIL;
}
/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcPageUp                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcPageUp (void)
{
	UINT8 line, col;
	UINT8 item;
	UINT8 layer;

	layer = menuProcLayerGet();
	if(!layer)
	{
		layer = 1;
	}
	menuLayerSizeGet(layer, &line, &col);
	item = layerScrollTab[layer];
	if (item >= line)
	{
		layerScrollTab[layer] -= line;
		return SUCCESS;
	}

	return FAIL;
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcPageDown                                       *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT8 menuProcPageDown (void)
{
	UINT8 line, col;
	UINT8 nodeItem;
	UINT8 item;
	UINT8 layer;

	layer = menuProcLayerGet();

	if(!layer)
	{
		layer = 1;
	}

	menuLayerSizeGet(layer, &line, &col);
	nodeItem = menuProcNodeItemGet(layer, 0);
	item = layerScrollTab[layer] + line;
	if (item < nodeItem)
	{
		layerScrollTab[layer] += line;
		return SUCCESS;
	}

	return FAIL;
}

#endif

