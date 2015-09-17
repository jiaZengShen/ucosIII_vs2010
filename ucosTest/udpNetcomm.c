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

///����,��������ǲ����˳���
void  udpNetCommThread(void * arg)
{
	struct ip_addr remoteIp ;
	struct netconn *pUdpConn  ;
	struct netbuf * recvBuf ;
	struct netbuf * sendBuf ;
	struct pbuf *iter ;//����
	err_t err  = ERR_OK;
	int dataLen = 0;
	pUdpConn = netconn_new(NETCONN_UDP);
	pUdpConn->recv_timeout = 10;
	
	//����Զ��ip
	IP4_ADDR(&remoteIp,192,168,1,100);
	// ���÷�������
	sendDataInit();
	//����Զ������
	err = netconn_connect(pUdpConn,&remoteIp,8080);
	if(err == ERR_OK)
	{
		printf("����Զ�������ɹ�\n");
		while(1)
		{
			//��������
			sendBuf = netbuf_new();
			netbuf_alloc(sendBuf,512);
			memcpy(sendBuf->p->payload,sendData,512);
			err = netconn_send(pUdpConn,sendBuf);
			netbuf_delete(sendBuf);
			//��������
			netconn_recv(pUdpConn,&recvBuf);
			if(recvBuf)
			{
				for(iter = recvBuf->p;iter != NULL;iter = iter->next)
				{
					//������Ƿ�Խ��
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
		printf("����Զ������ʧ��\n");
	}
}


//���ֽӿڱ�����Ƕ��̵߳ģ������޷�����
void udpNetCommInit()
{
	sys_thread_new("udpThread", udpNetCommThread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
