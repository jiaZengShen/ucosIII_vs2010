#include "LWIP/sockets.h"
#include "LWIP/api.h"
#include "LWIP/init.h"
#include "LWIP/tcpip.h"
#include "ping.h"
#include  "includes.h"

OS_TCB   pingTCB;
CPU_STK  pingTaskStartStk[APP_TASK_START_STK_SIZE];
//����ping�������߳��ǲ���ֹͣ�ģ������������̶߳�ֹͣ��
void pingTestTask(void *p_arg)
{
	OS_ERR  err ;
	ping_init();
	//�߳��ǲ����˳��ģ�
	while(DEF_TRUE)
	{
		OSTimeDlyHMSM(0, 0, 1, 0,
			OS_OPT_TIME_DLY,
			&err);
	}
}


OS_TCB   clientTCB;
CPU_STK  clientTaskStartStk[APP_TASK_START_STK_SIZE];
//����ping����
void clientTestTask(void *p_arg)
{
	//����javaд��8899�ķ�����

}