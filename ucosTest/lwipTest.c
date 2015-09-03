#include "LWIP/sockets.h"
#include "LWIP/api.h"
#include "LWIP/init.h"
#include "LWIP/tcpip.h"
#include "ping.h"
#include  "includes.h"

OS_TCB   pingTCB;
CPU_STK  pingTaskStartStk[APP_TASK_START_STK_SIZE];
//测试ping命令，这个线程是不能停止的，否则其他的线程都停止了
void pingTestTask(void *p_arg)
{
	OS_ERR  err ;
	ping_init();
	//线程是不能退出的，
	while(DEF_TRUE)
	{
		OSTimeDlyHMSM(0, 0, 1, 0,
			OS_OPT_TIME_DLY,
			&err);
	}
}


OS_TCB   clientTCB;
CPU_STK  clientTaskStartStk[APP_TASK_START_STK_SIZE];
//测试ping命令
void clientTestTask(void *p_arg)
{
	//连接java写的8899的服务器

}