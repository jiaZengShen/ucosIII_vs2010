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
	struct netbuf *recvBuf ;//���ջ�����
	struct pbuf *iter ;//����
	int dataLen = 0;
	//����
	sendDataInit();
	IP4_ADDR(&server_ipaddr, 192,168,10,120);//���÷�������ַ
	while(1)
	{
		tcp_clientConn = netconn_new(NETCONN_TCP);
		err = netconn_connect(tcp_clientConn,&server_ipaddr,8080);
		//����δ�ɹ�
		if(err != ERR_OK)
		{
			netconn_delete(tcp_clientConn);
			continue;
		}else{
			//�鿴��������ip�ͱ���ip
			tcp_clientConn->recv_timeout = 10 ;//��ʱ10����
			netconn_getaddr(tcp_clientConn,&localIp,&localport,1);//��ȡ����ip
			//printf("�����Ϸ�����%d.%d.%d.%d,�����˿ں�Ϊ:%d\r\n",lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3],loca_port);
			//
			while(1){
				//��������
				netconn_write(tcp_clientConn,sendData,512,NETCONN_COPY);

				//��������
				if(netconn_recv(tcp_clientConn,&recvBuf) == ERR_OK)
				{
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
