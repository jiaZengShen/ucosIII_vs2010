#include "includes.h"
#include "includes.h"
#include "LWIP/udp.h"
#include <assert.h>
char udp_demo_recvbuf[512];	//UDP½ÓÊÕÊý¾Ý»º³åÇø 

const char *tcp_demo_sendbuf="客户端发送数据\r\n";;

struct udp_pcb *pUdpPcb   = NULL;

//发送数据
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_demo_sendbuf),PBUF_POOL); //ÉêÇëÄÚ´æ
	if(ptr)
	{
		pbuf_take(ptr,(char*)tcp_demo_sendbuf,strlen((char*)tcp_demo_sendbuf)); //
		udp_send(upcb,ptr);	//ud
		pbuf_free(ptr);//ÊÍ·ÅÄÚ´æ
	} 
} 
//接收函数的回调函数
char recvBuffer[512];
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	struct pbuf *iter;
	int dataLen = 0;
	if(p != NULL)
	{
		for(iter = p ;iter != NULL;iter = iter->next)
		{
			//不检查是否越界
			memcpy(recvBuffer+dataLen,iter->payload,iter->len);
			dataLen  += iter->len;
			if(dataLen>509)
				break;
		}
		recvBuffer[dataLen+1] = '\n';
		recvBuffer[dataLen+1] = '\0';
		printf(recvBuffer);
	}
}
 void udpRawTestInit()
{
	struct ip_addr rmt_addr ;//远端ip
	int port ;
	err_t err ;
	pUdpPcb = udp_new();

	if(pUdpPcb)
	{
		IP4_ADDR(&rmt_addr,192,168,1,100);
		err = udp_connect(pUdpPcb,&rmt_addr,1000);
		if(err == ERR_OK)
		{
			//发送
			udp_demo_senddata(pUdpPcb);
			//接收数据
			udp_recv(pUdpPcb,udp_demo_recv,NULL);//
		}
	}else{
		assert(0);
	}
}
