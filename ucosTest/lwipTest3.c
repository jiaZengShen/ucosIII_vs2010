/* C runtime includes */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <conio.h>

/* lwIP core includes */
#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"

#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include <includes.h>
/* lwIP netif includes */
#include "netif/etharp.h"

/* include the port-dependent configuration */
#include "lwipcfg_msvc.h"

#include "pcapif.h"


#if NO_SYS
/* ... then we need information about the timer intervals: */
#include "lwip/ip_frag.h"
#include "lwip/igmp.h"
#endif /* NO_SYS */

#if PPP_SUPPORT
/* PPP includes */
#include "../netif/ppp/ppp_impl.h"
#include "lwip/sio.h"
#include "netif/ppp_oe.h"
#endif /* PPP_SUPPORT */

/* include the port-dependent configuration */
#include "lwipcfg_msvc.h"
#include "echo.h"
/** Define this to 1 to enable a PCAP interface as default interface. */
#ifndef USE_PCAPIF
#define USE_PCAPIF 1
#endif

/** Define this to 1 or 2 to support 1 or 2 SLIP interfaces. */
#ifndef USE_SLIPIF
#define USE_SLIPIF 0
#endif

/** Use an ethernet adapter? Default to enabled if PCAPIF or PPPoE are used. */
#ifndef USE_ETHERNET
#define USE_ETHERNET  (USE_PCAPIF || PPPOE_SUPPORT)
#endif

/** Use an ethernet adapter for TCP/IP? By default only if PCAPIF is used. */
#ifndef USE_ETHERNET_TCPIP
#define USE_ETHERNET_TCPIP  (USE_PCAPIF)
#endif

#if USE_ETHERNET
#include "pcapif.h"
#endif /* USE_ETHERNET */
#if USE_SLIPIF
#include <netif/slipif.h>
#endif /* USE_SLIPIF */

#ifndef USE_DHCP
#define USE_DHCP    LWIP_DHCP
#endif
#ifndef USE_AUTOIP
#define USE_AUTOIP  LWIP_AUTOIP
#endif

/* globales variables for netifs */
#if USE_ETHERNET
/* THE ethernet interface */
struct netif netif;
#if LWIP_DHCP
/* dhcp struct for the ethernet netif */
struct dhcp netif_dhcp;
#endif /* LWIP_DHCP */
#if LWIP_AUTOIP
/* autoip struct for the ethernet netif */
struct autoip netif_autoip;
#endif /* LWIP_AUTOIP */
#endif /* USE_ETHERNET */
#if PPP_SUPPORT
/* THE PPP descriptor */
int ppp_desc = -1;
u8_t sio_idx = 0;
sio_fd_t ppp_sio;
#endif /* PPP_SUPPORT */
#if USE_SLIPIF
struct netif slipif1;
#if USE_SLIPIF > 1
struct netif slipif2;
#endif /* USE_SLIPIF > 1 */
#endif /* USE_SLIPIF */



void status_callback(struct netif *netif)
{
	if (netif_is_up(netif)) {
		printf("status_callback==UP, local interface IP is %s\n", ip_ntoa(&netif->ip_addr));
	} else {
		printf("status_callback==DOWN\n");
	}
}



void link_callback(struct netif *netif)
{
	if (netif_is_link_up(netif)) {
		printf("link_callback==UP\n");
	} else {
		printf("link_callback==DOWN\n");
	}
}



/* This function initializes all network interfaces */
static void	msvc_netif_init()
{
	ip_addr_t ipaddr, netmask, gw;

	ip_addr_set_zero(&gw);
	ip_addr_set_zero(&ipaddr);
	ip_addr_set_zero(&netmask);
	LWIP_PORT_INIT_GW(&gw);//设置网关
	LWIP_PORT_INIT_IPADDR(&ipaddr);//设置ip地址
	LWIP_PORT_INIT_NETMASK(&netmask);//设置掩码
	printf("Starting lwIP, local interface IP is %s\n", ip_ntoa(&ipaddr));
	printf("the 网关是%s\n",ip_ntoa(&gw) );
	netif_set_default(netif_add(&netif, &ipaddr, &netmask, &gw, NULL, pcapif_init, tcpip_input));
	netif_set_status_callback(&netif, status_callback);
	netif_set_link_callback(&netif, link_callback);
	netif_set_up(&netif);
}




void my_tcp_server_init(void);
void tcp_raw_init() ;
void  udpNetCommInit();
void tcpNetConnTest();
void tcpNetconnServerTestInit();
void socketSelectInit();
void socketClientInit();
#include "udpRawTest.h"
#include "socket_examples.h"
/* This function initializes applications */
static void	apps_init()
{
	//echo_init();
	//tcp
	//my_tcp_server_init();
	
	//udp raw接口测试
	//udpRawTestInit();	
	
	//tcp_raw_init();

	//udpNetCommInit();
	//tcpNetConnTest();
	//tcpNetconnServerTestInit();
	

	//socket_examples_init();
	socketSelectInit();
	//socketClientInit();
}




/* This function initializes this lwIP test. When NO_SYS=1, this is done in
 * the main_loop context (there is no other one), when NO_SYS=0, this is done
 * in the tcpip_thread context */
static void
myTcp_init(void * arg)
{ /* remove compiler warning */

  sys_sem_t *init_sem;
  LWIP_ASSERT("arg != NULL", arg != NULL);
  init_sem = (sys_sem_t*)arg;


  /* init network interfaces */
  msvc_netif_init();

  /* init apps */
  //apps_init();

  sys_sem_signal(init_sem);

}
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  OS_TCB   ATCB;
static  CPU_STK  ATaskStartStk[APP_TASK_START_STK_SIZE];

static  OS_TCB   BTCB;
static  CPU_STK  BTaskStartStk[APP_TASK_START_STK_SIZE];

#include "os_app_hooks.h"

OS_SEM *p_sem ;//指向信号量
OS_SEM g_sem ;//实际的信号量
void  MyTaskA (void *p_arg)
{
	OS_ERR  err;
	APP_TRACE_DBG(("A is Running...\n\r"));
	while (DEF_TRUE) {        
		//如果把延时去掉，那么就不会实现任务切换了。
		/* Task body, always written as an infinite loop.       */
		OSTimeDlyHMSM(0, 0, 1, 0,
			OS_OPT_TIME_DLY,
			&err);
		//使用信号量来决定，谁能输出
		OSSemPend(p_sem,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		if(err == OS_ERR_NONE)//没有出错
		{
			APP_TRACE_DBG(("A: %d\n\r", OSTimeGet(&err)));
		}
		OSSemPost(p_sem,OS_OPT_POST_ALL,&err);
	}
}

void MyTaskB(void *p_arg)
{
	OS_ERR  err;
	APP_TRACE_DBG(("B is Running...\n\r"));
	while (DEF_TRUE) {                                            /* Task body, always written as an infinite loop.       */
		OSTimeDlyHMSM(0, 0, 1, 0,
			OS_OPT_TIME_DLY,
			&err);
		//使用信号量来决定，谁能输出
		OSSemPend(p_sem,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		if(err == OS_ERR_NONE)//没有出错
		{
			APP_TRACE_DBG(("B: %d\n\r", OSTimeGet(&err)));
		}
		OSSemPost(p_sem,OS_OPT_POST_ALL,&err);
	}
}
static  void  AppTaskStart (void *p_arg)
{
    OS_ERR  err;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize uC/CPU services                           */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif
#if OS_CFG_APP_HOOKS_EN				//使用钩子函数
	App_OS_SetAllHooks();			
#endif
    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));
	OS_CRITICAL_ENTER();	//进入临界区
	p_sem = &g_sem;
	//初始化一些参数
	//tcpip_init(NULL,NULL);
	//创建信号量
	OSSemCreate(p_sem,
		"sem_name",
		1,
		&err);
	//创建任务
	//A
	OSTaskCreate((OS_TCB     *)&ATCB,                /* Create the start task                                */
		(CPU_CHAR   *)"A",
		(OS_TASK_PTR ) MyTaskA,
		(void       *) 0,
		(OS_PRIO     ) APP_TASK_START_PRIO+1,
		(CPU_STK    *)&ATaskStartStk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err);
	////B
	OSTaskCreate((OS_TCB     *)&BTCB,                /* Create the start task                                */
		(CPU_CHAR   *)"A",
		(OS_TASK_PTR ) MyTaskB,
		(void       *) 0,
		(OS_PRIO     ) APP_TASK_START_PRIO+2,//优先级必须相互区分
		(CPU_STK    *)&BTaskStartStk[0],
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY  ) 0u,
		(OS_TICK     ) 0u,
		(void       *) 0,
		(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		(OS_ERR     *)&err);
	////创建ping的test
	//OSTaskCreate((OS_TCB     *)&pingTCB,                /* Create the start task                                */
	//	(CPU_CHAR   *)"ping",
	//	(OS_TASK_PTR ) pingTestTask,
	//	(void       *) 0,
	//	(OS_PRIO     ) APP_TASK_START_PRIO+3,
	//	(CPU_STK    *)&pingTaskStartStk[0],
	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
	//	(OS_MSG_QTY  ) 0u,
	//	(OS_TICK     ) 0u,
	//	(void       *) 0,
	//	(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
	//	(OS_ERR     *)&err);
	//client测试程序
	//OSTaskCreate((OS_TCB     *)&clientTCB,                /* Create the start task                                */
	//	(CPU_CHAR   *)"client",
	//	(OS_TASK_PTR ) clientTestTask,
	//	(void       *) 0,
	//	(OS_PRIO     ) APP_TASK_START_PRIO+4,
	//	(CPU_STK    *)&clientTaskStartStk[0],
	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
	//	(CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
	//	(OS_MSG_QTY  ) 0u,
	//	(OS_TICK     ) 0u,
	//	(void       *) 0,
	//	(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
	//	(OS_ERR     *)&err);
	OS_CRITICAL_EXIT();	//退出临界区

	//OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身

	//lwip_init();
	
	//ping_init();
    while (DEF_ON) {                                            /* Task body, always written as an infinite loop.       */
		OSTimeDlyHMSM(0, 0, 1, 0,
			OS_OPT_TIME_DLY,
			&err);

        APP_TRACE_DBG(("Time: %d\n\r", OSTimeGet(&err)));
    }
}
void osInit()
{
	OS_ERR  err;
	OSInit(&err);                                               /* Init uC/OS-III.                                      */
	OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
	                (CPU_CHAR   *)"App Task Start",
	                (OS_TASK_PTR ) AppTaskStart,
	                (void       *) 0,
	                (OS_PRIO     ) APP_TASK_START_PRIO,
	                (CPU_STK    *)&AppTaskStartStk[0],
	                (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
	                (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
	                (OS_MSG_QTY  ) 0u,
	                (OS_TICK     ) 0u,
	                (void       *) 0,
	                (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
	                (OS_ERR     *)&err);
	apps_init();
}

/* This is somewhat different to other ports: we have a main loop here:
 * a dedicated task that waits for packets to arrive. This would normally be
 * done from interrupt context with embedded hardware, but we don't get an
 * interrupt in windows for that :-) */
void main_loop()
{

  err_t err;
  sys_sem_t init_sem;



  /* initialize lwIP stack, network interfaces and applications */

  err = sys_sem_new(&init_sem, 0);
  tcpip_init(myTcp_init, &init_sem);
  /* we have to wait for initialization to finish before
   * calling update_adapter()! */
  sys_sem_wait(&init_sem);
  sys_sem_free(&init_sem);


  /* MAIN LOOP for driver update (and timers if NO_SYS) */
  osInit();
  OSStart(&err); 
  while (!_kbhit()) {
    sys_msleep(50);
  }

  /* release the pcap library... */
  pcapif_shutdown(&netif);

}

void main(void)

{
	/* no stdio-buffering, please! */
	setvbuf(stdout, NULL,_IONBF, 0);
	main_loop();
	return ;
}