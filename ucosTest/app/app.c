///*
//*********************************************************************************************************
//*                                            EXAMPLE CODE
//*
//*                          (c) Copyright 2009-2011; Micrium, Inc.; Weston, FL
//*
//*               All rights reserved.  Protected by international copyright laws.
//*
//*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
//*               your application products.  Example code may be used as is, in whole or in
//*               part, or may be used as a reference only.
//*
//*               Please help us continue to provide the Embedded community with the finest
//*               software available.  Your honesty is greatly appreciated.
//*
//*               You can contact us at www.micrium.com.
//*********************************************************************************************************
//*/
//
///*
//*********************************************************************************************************
//*
//*                                          APPLICATION CODE
//*
//*                                          Microsoft Windows
//*
//* Filename      : app.c
//* Version       : V1.00
//* Programmer(s) : JJL
//*                 FGK
//*********************************************************************************************************
//*/
//
///*
//*********************************************************************************************************
//*                                             INCLUDE FILES
//*********************************************************************************************************
//*/
//
//#include <includes.h>
//
//
///*
//*********************************************************************************************************
//*                                             LOCAL DEFINES
//*********************************************************************************************************
//*/
//
///*
//*********************************************************************************************************
//*                                            LOCAL VARIABLES
//*********************************************************************************************************
//*/
//static  OS_TCB   AppTaskStartTCB;
//static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
//
//static  OS_TCB   ATCB;
//static  CPU_STK  ATaskStartStk[APP_TASK_START_STK_SIZE];
//
//static  OS_TCB   BTCB;
//static  CPU_STK  BTaskStartStk[APP_TASK_START_STK_SIZE];
///*
//*********************************************************************************************************
//*                                         FUNCTION PROTOTYPES
//*********************************************************************************************************
//*/
//
//static  void  AppTaskStart(void  *p_arg);
//
//
///*
//*********************************************************************************************************
//*                                                main()
//*
//* Description : This is the standard entry point for C code.  It is assumed that your code will call
//*               main() once you have performed all necessary initialization.
//*
//* Argument(s) : none.
//*
//* Return(s)   : none.
//*********************************************************************************************************
//*/
//
//int  ___main (void)
//{
//    OS_ERR  err;
//
//
//    OSInit(&err);                                               /* Init uC/OS-III.                                      */
//
//    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
//                 (CPU_CHAR   *)"App Task Start",
//                 (OS_TASK_PTR ) AppTaskStart,
//                 (void       *) 0,
//                 (OS_PRIO     ) APP_TASK_START_PRIO,
//                 (CPU_STK    *)&AppTaskStartStk[0],
//                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
//                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
//                 (OS_MSG_QTY  ) 0u,
//                 (OS_TICK     ) 0u,
//                 (void       *) 0,
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//                 (OS_ERR     *)&err);
//
//    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
//	return 0 ;
//}
//
//
///*
//*********************************************************************************************************
//*                                          STARTUP TASK
//*
//* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
//*               initialize the ticker only once multitasking has started.
//*
//* Argument(s) : p_arg       is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
//*
//* Return(s)   : none.
//*
//* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
//*                   used.  The compiler should not generate any code for this statement.
//*********************************************************************************************************
//*/
//#include "os_app_hooks.h"
//
//OS_SEM *p_sem ;//指向信号量
//OS_SEM g_sem ;//实际的信号量
//void  MyTaskA (void *p_arg)
//{
//	OS_ERR  err;
//	APP_TRACE_DBG(("A is Running...\n\r"));
//	while (DEF_TRUE) {        
//		//如果把延时去掉，那么就不会实现任务切换了。
//		/* Task body, always written as an infinite loop.       */
//		OSTimeDlyHMSM(0, 0, 1, 0,
//			OS_OPT_TIME_DLY,
//			&err);
//		//使用信号量来决定，谁能输出
//		OSSemPend(p_sem,0,OS_OPT_PEND_BLOCKING,NULL,&err);
//		if(err == OS_ERR_NONE)//没有出错
//		{
//			APP_TRACE_DBG(("A: %d\n\r", OSTimeGet(&err)));
//		}
//		OSSemPost(p_sem,OS_OPT_POST_ALL,&err);
//	}
//}
//
//void MyTaskB(void *p_arg)
//{
//	OS_ERR  err;
//	APP_TRACE_DBG(("B is Running...\n\r"));
//	while (DEF_TRUE) {                                            /* Task body, always written as an infinite loop.       */
//		OSTimeDlyHMSM(0, 0, 1, 0,
//			OS_OPT_TIME_DLY,
//			&err);
//		//使用信号量来决定，谁能输出
//		OSSemPend(p_sem,0,OS_OPT_PEND_BLOCKING,NULL,&err);
//		if(err == OS_ERR_NONE)//没有出错
//		{
//			APP_TRACE_DBG(("B: %d\n\r", OSTimeGet(&err)));
//		}
//		OSSemPost(p_sem,OS_OPT_POST_ALL,&err);
//	}
//}
//
//
//#include "lwipTest.h"
//
//static  void  AppTaskStart (void *p_arg)
//{
//    OS_ERR  err;
//
//
//   (void)p_arg;
//
//    BSP_Init();                                                 /* Initialize BSP functions                             */
//    CPU_Init();                                                 /* Initialize uC/CPU services                           */
//
//#if OS_CFG_STAT_TASK_EN > 0u
//    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
//#endif
//#if OS_CFG_APP_HOOKS_EN				//使用钩子函数
//	App_OS_SetAllHooks();			
//#endif
//    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));
//	OS_CRITICAL_ENTER();	//进入临界区
//	p_sem = &g_sem;
//	//初始化一些参数
//	//tcpip_init(NULL,NULL);
//	//创建信号量
//	OSSemCreate(p_sem,
//		"sem_name",
//		1,
//		&err);
//	//创建任务
//	//A
//	//OSTaskCreate((OS_TCB     *)&ATCB,                /* Create the start task                                */
//	//	(CPU_CHAR   *)"A",
//	//	(OS_TASK_PTR ) MyTaskA,
//	//	(void       *) 0,
//	//	(OS_PRIO     ) APP_TASK_START_PRIO+1,
//	//	(CPU_STK    *)&ATaskStartStk[0],
//	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
//	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
//	//	(OS_MSG_QTY  ) 0u,
//	//	(OS_TICK     ) 0u,
//	//	(void       *) 0,
//	//	(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//	//	(OS_ERR     *)&err);
//	////B
//	//OSTaskCreate((OS_TCB     *)&BTCB,                /* Create the start task                                */
//	//	(CPU_CHAR   *)"A",
//	//	(OS_TASK_PTR ) MyTaskB,
//	//	(void       *) 0,
//	//	(OS_PRIO     ) APP_TASK_START_PRIO+2,//优先级必须相互区分
//	//	(CPU_STK    *)&BTaskStartStk[0],
//	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
//	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
//	//	(OS_MSG_QTY  ) 0u,
//	//	(OS_TICK     ) 0u,
//	//	(void       *) 0,
//	//	(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//	//	(OS_ERR     *)&err);
//	////创建ping的test
//	//OSTaskCreate((OS_TCB     *)&pingTCB,                /* Create the start task                                */
//	//	(CPU_CHAR   *)"ping",
//	//	(OS_TASK_PTR ) pingTestTask,
//	//	(void       *) 0,
//	//	(OS_PRIO     ) APP_TASK_START_PRIO+3,
//	//	(CPU_STK    *)&pingTaskStartStk[0],
//	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
//	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
//	//	(OS_MSG_QTY  ) 0u,
//	//	(OS_TICK     ) 0u,
//	//	(void       *) 0,
//	//	(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//	//	(OS_ERR     *)&err);
//	//client测试程序
//	OSTaskCreate((OS_TCB     *)&clientTCB,                /* Create the start task                                */
//		(CPU_CHAR   *)"client",
//		(OS_TASK_PTR ) clientTestTask,
//		(void       *) 0,
//		(OS_PRIO     ) APP_TASK_START_PRIO+4,
//		(CPU_STK    *)&clientTaskStartStk[0],
//		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
//		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
//		(OS_MSG_QTY  ) 0u,
//		(OS_TICK     ) 0u,
//		(void       *) 0,
//		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//		(OS_ERR     *)&err);
//	OS_CRITICAL_EXIT();	//退出临界区
//
//	//OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
//
//	//lwip_init();
//	
//	//ping_init();
//    while (DEF_ON) {                                            /* Task body, always written as an infinite loop.       */
//		OSTimeDlyHMSM(0, 0, 1, 0,
//			OS_OPT_TIME_DLY,
//			&err);
//
//        APP_TRACE_DBG(("Time: %d\n\r", OSTimeGet(&err)));
//    }
//}
//
//
//
