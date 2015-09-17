#include "includes.h"
#include "LWIP/tcpip.h"
#include "LWIP/pbuf.h"
#include "LWIP/tcp.h"

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

///阻塞,这个函数是不会退出的
void  udpNetCommThread(void * arg)
{
	struct ip_addr remoteIp ;
	struct netconn *pUdpConn  ;
	struct netbuf * recvBuf ;
	struct netbuf * sendBuf ;
	struct pbuf *iter ;//遍历
	err_t err  = ERR_OK;
	int dataLen = 0;
	pUdpConn = netconn_new(NETCONN_UDP);
	pUdpConn->recv_timeout = 10;
	
	//设置远端ip
	IP4_ADDR(&remoteIp,192,168,1,100);
	// 设置发送数据
	sendDataInit();
	//连接远端主机
	err = netconn_connect(pUdpConn,&remoteIp,8080);
	if(err == ERR_OK)
	{
		printf("连接远端主机成功\n");
		while(1)
		{
			//发送数据
			sendBuf = netbuf_new();
			netbuf_alloc(sendBuf,512);
			memcpy(sendBuf->p->payload,sendData,512);
			err = netconn_send(pUdpConn,sendBuf);
			netbuf_delete(sendBuf);
			//接受数据
			netconn_recv(pUdpConn,&recvBuf);
			if(recvBuf)
			{
				for(iter = recvBuf->p;iter != NULL;iter = iter->next)
				{
					//不检查是否越界
					memcpy(recvBuffer+dataLen,iter->payload,iter->len);
					dataLen  += iter->len;
					if(dataLen>509)
						break;
				}
				printf(recvBuffer);
				netbuf_delete(recvBuf);
			}
		}
	}else{
		printf("连接远端主机失败\n");
	}
}


//这种接口必须得是多线程的，否则无法运行
void udpNetCommInit()
{
	sys_thread_new("udpThread", udpNetCommThread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
