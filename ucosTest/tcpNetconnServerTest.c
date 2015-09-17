#include "lwip/opt.h"

#include "LWIP/ip_addr.h"
#include "lwip/api.h"
#include "includes.h"
//
static char sendData[512];
static char recvBuffer[512];
static void sendDataInit()
{
	int i=0;
	char c = '0';
	for(i=0;i<512;i++)
	{
		if(c>'z')
			c='0';
		sendData[i] = c;
		c++;
	}
	return ;
}
void tcpNetconnServerThread(void *arg)
{
	unsigned int  data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	char remot_addr[4];
	struct netconn *conn, *newconn;
	static ip_addr_t ipaddr;
	static u16_t 			port;

	LWIP_UNUSED_ARG(arg);
	sendDataInit();
	conn = netconn_new(NETCONN_TCP);  //����һ��TCP����
	netconn_bind(conn,IP_ADDR_ANY,8080);  //�󶨶˿� 8�Ŷ˿�
	netconn_listen(conn);  		//�������ģʽ
	while(1)
	{
		//accept
		err = netconn_accept(conn,&newconn);
		if(err == ERR_OK)
		{
			//��������
			err = netconn_write(newconn,sendData,512,NETCONN_COPY);

			//�Ͽ�
			netconn_close(newconn);
			netconn_delete(newconn);
			printf("�Ͽ�\n");
		}else{
			continue;
		}
		

	}
}


void tcpNetconnServerTestInit()
{
	sys_thread_new("udpThread", tcpNetconnServerThread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}