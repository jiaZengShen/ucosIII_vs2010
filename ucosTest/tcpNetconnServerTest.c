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
	conn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
	netconn_bind(conn,IP_ADDR_ANY,8080);  //绑定端口 8号端口
	netconn_listen(conn);  		//进入监听模式
	while(1)
	{
		//accept
		err = netconn_accept(conn,&newconn);
		if(err == ERR_OK)
		{
			//发送数据
			err = netconn_write(newconn,sendData,512,NETCONN_COPY);

			//断开
			netconn_close(newconn);
			netconn_delete(newconn);
			printf("断开\n");
		}else{
			continue;
		}
		

	}
}


void tcpNetconnServerTestInit()
{
	sys_thread_new("udpThread", tcpNetconnServerThread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}