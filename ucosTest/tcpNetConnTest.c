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

void tcpNetThread(void *arg)
{
	
	ip_addr_t server_ipaddr;
	ip_addr_t localIp;
	struct netconn *tcp_clientConn ;
	u16_t localport ;
	err_t err ;
	struct netbuf *recvBuf ;//接收缓冲区
	struct pbuf *iter ;//遍历
	int dataLen = 0;
	//过程
	sendDataInit();
	IP4_ADDR(&server_ipaddr, 192,168,10,120);//设置服务器地址
	while(1)
	{
		tcp_clientConn = netconn_new(NETCONN_TCP);
		err = netconn_connect(tcp_clientConn,&server_ipaddr,8080);
		//连接未成功
		if(err != ERR_OK)
		{
			netconn_delete(tcp_clientConn);
			continue;
		}else{
			//查看服务器的ip和本地ip
			tcp_clientConn->recv_timeout = 10 ;//超时10毫秒
			netconn_getaddr(tcp_clientConn,&localIp,&localport,1);//获取本机ip
			//printf("连接上服务器%d.%d.%d.%d,本机端口号为:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			//
			while(1){
				//发送数据
				netconn_write(tcp_clientConn,sendData,512,NETCONN_COPY);

				//接收数据
				if(netconn_recv(tcp_clientConn,&recvBuf) == ERR_OK)
				{
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
						dataLen = 0;
						printf(recvBuffer);
						netbuf_delete(recvBuf);
					}
				}
			}
			

		}

	}
}

void tcpNetConnTest()
{
	sys_thread_new("udpThread", tcpNetThread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
