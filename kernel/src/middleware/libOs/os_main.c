/*

Copyright (c) 2007 Sunplus Technology Co., Ltd.

Module Name:

		os_main.c

Abstract:

		Module related to OS

Environment:

		Keil C51 Compiler

Revision History:

		07/24/2007		Phil	created

*/

//=============================================================================
//Header file
//=============================================================================
#include "general.h"
#define SYS_OS_INTERNAL // for libOs internal use
#include "func_os_def.h"
#include "os_api.h"
#include "os_main.h"
#include "func_ptr.h"
//=============================================================================
//Symbol
//=============================================================================

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (code* pfp)(void);

//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------
#define OS_TASK_WAIT_EVER		0
#define OS_CHECK_TO_RUN(osTaskId)		{\
											if ((osTaskId)) {\
												(*pTcb->osTask)();\
												if (pTcb->osTaskID) {\
													osTaskId = 0;\
													pTcb->osTaskDly = pTcb->osTaskPrio;\
												}\
											}\
											pTcb = pTcb->osTcbNext;\
											if (pTcb == pFree){\
												break;\
											}\
										}
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------
static UINT8 xdata OSMapTbl[9] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00};
static OS_TCB xdata osTaskBuf[OS_TASK_MAX];
static OS_TCB xdata* xdata pFreeTaskList;
static OS_TCB xdata* xdata pTaskList;
static UINT8 bdata osTaskState;
sbit OS_TASK_0 = osTaskState^0;
sbit OS_TASK_1 = osTaskState^1;
sbit OS_TASK_2 = osTaskState^2;
sbit OS_TASK_3 = osTaskState^3;
sbit OS_TASK_4 = osTaskState^4;
sbit OS_TASK_5 = osTaskState^5;
sbit OS_TASK_6 = osTaskState^6;
sbit OS_TASK_7 = osTaskState^7;

extern SYS_OS_DCL pFuncOS code funcOsArray[];

//SysMode_t osSysMode = OS_SYS_MODE_DEFAULT;
osSysMode_t osSysMode = OS_SYS_MODE_STILL_PV;

// add by phil for correcting the call tree for function pointer E
//=============================================================================
//Program
//=============================================================================
//-----------------------------------------------------------------------------
//osInit
//-----------------------------------------------------------------------------
#if (OS_TASK_MAX)
void osInit(void ) USING_0
{
	UINT8 i;

	pFreeTaskList = osTaskBuf;
	pTaskList = (OS_TCB*)0;
	for (i = 0; i < OS_TASK_MAX; i++) {
		osTaskBuf[i].osTask = (pFunc)0;
		osTaskBuf[i].osTaskID = i;
		osTaskBuf[i].osTaskDly = osTaskBuf[i].osTaskPrio = OS_TASK_WAIT_EVER;
		osTaskBuf[i].osTaskBank = 0;
		osTaskBuf[i].osTcbNext = &osTaskBuf[i + 1];
	}
	osTaskBuf[OS_TASK_MAX - 1].osTcbNext = (OS_TCB*)0;

	/*The first task must be always in ready*/
	osTaskState = 0x01;
}
//-----------------------------------------------------------------------------
//osTaskCreate
//-----------------------------------------------------------------------------

UINT8 osTaskCreate(pFunc TaskFunc, UINT8 TaskPrio) USING_0
{
	UINT8 status;
	OS_TCB* pTcb;
/*
	if (TaskBank > (SP1K_BANK_NUM_MAX - 1)) {
		//Task bank must be 0~14!
		return;
	}
*/
	ENTER_CRITICAL( status );
	pTcb = pFreeTaskList;
	if (pFreeTaskList) {
		pFreeTaskList = pFreeTaskList->osTcbNext;
	}
	if (pTcb == (OS_TCB*)0) {
		//ran out of OS task control block!
		EXIT_CRITICAL( status );
		return -1;
	}
	pTcb->osTask = TaskFunc;
	pTcb->osTaskPrio = TaskPrio;
	pTcb->osTaskDly = TaskPrio;
//	pTcb->osTaskBank = TaskBank;

	pTaskList = pTcb;
	EXIT_CRITICAL( status );

	//printf("ID %bu\n", pTaskList->osTaskID);
	return pTaskList->osTaskID;
}

//-----------------------------------------------------------------------------
//osTimeTick
//
//Executed in timer interrupt routine
//-----------------------------------------------------------------------------
void osTimeTick(void) USING_2
{
	OS_TCB* pTcb;
	OS_TCB* pFree;

	pTcb = osTaskBuf;
	pFree = pFreeTaskList;

	do {
		if (pTcb->osTaskID) {
			if (--pTcb->osTaskDly == 0) {
#ifndef TS
				osTaskState |= OSMapTbl[pTcb->osTaskID];
#else
				osTaskState |= (1 << pTcb->osTaskID);
#endif
				//printf("State is %bu\n", osTaskState);
			}
		}
		pTcb = pTcb->osTcbNext;
	} while (pTcb != pFree);

}

//-----------------------------------------------------------------------------
//osSched
//
//Execute tasks which are already in ready in task list
//-----------------------------------------------------------------------------
void osSched(void) USING_0
{
	static OS_TCB* pTcb;
	static OS_TCB* pFree;
	pFuncOS* pf;

	pTcb = osTaskBuf;
	pFree = pFreeTaskList;

//dbgTimerStart();
//printf("State is %bu\n", osTaskState);
	do {
		OS_CHECK_TO_RUN(OS_TASK_0);
		OS_CHECK_TO_RUN(OS_TASK_1);
		OS_CHECK_TO_RUN(OS_TASK_2);
		OS_CHECK_TO_RUN(OS_TASK_3);
		OS_CHECK_TO_RUN(OS_TASK_4);
		OS_CHECK_TO_RUN(OS_TASK_5);
		OS_CHECK_TO_RUN(OS_TASK_6);
		OS_CHECK_TO_RUN(OS_TASK_7);
		pf = funcOsArray; // add by phil for correcting the call tree for function pointer
	} while(0);

//dbgTimerStop();
	//printf("osSched finished \n");
}

//-----------------------------------------------------------------------------
//osTaskIdle
//
//
//-----------------------------------------------------------------------------
UINT8 osTaskIdle(void) USING_0
{
	//printf("osTaskIdle\n");
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//osActiveFuncSet
//
//
//-----------------------------------------------------------------------------

void osActiveFuncSet(pFunc Func) USING_0
{
	UINT8 status;
/*
	if (FuncBank > SP1K_BANK_NUM_MAX) {
		return;
	}
*/
	ENTER_CRITICAL( status );
	osTaskBuf[0].osTask = Func;
//	osTaskBuf[0].osTaskBank = FuncBank;
	EXIT_CRITICAL( status );
}
//-----------------------------------------------------------------------------
//osTaskSuspend
//
//
//-----------------------------------------------------------------------------

void osTaskSuspend(UINT8 TaskID) USING_0
{
	UINT8 status;

	if (TaskID > OS_TASK_MAX) {
		return;
	}
	ENTER_CRITICAL( status );
	osTaskBuf[TaskID].osTaskDly = OS_TASK_WAIT_EVER;
	osTaskState &= ~OSMapTbl[TaskID];
	EXIT_CRITICAL( status );
	//printf("osTaskState is %bu\n", osTaskState );
}
//-----------------------------------------------------------------------------
//osTaskResume
//
//
//-----------------------------------------------------------------------------

void osTaskResume(UINT8 TaskID) USING_0
{
	UINT8 status;

	if (TaskID > OS_TASK_MAX) {
		return;
	}
	ENTER_CRITICAL( status );
	osTaskBuf[TaskID].osTaskDly = osTaskBuf[TaskID].osTaskPrio;
	osTaskState |= OSMapTbl[TaskID];
	EXIT_CRITICAL( status );
}

//-----------------------------------------------------------------------------
//osAppModeSet
//-----------------------------------------------------------------------------
void osSysModeSet(osSysMode_t mode) USING_0
{
	if(mode < OS_SYS_MODE_MAX)
		osSysMode = mode;
}

osSysMode_t osSysModeGet(void) USING_0
{
	return osSysMode;
}

#endif /*OS_TASK_MAX*/

