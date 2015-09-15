#include <assert.h>
#include <lwip/tcp.h>
#include <lwip/tcpip.h>
#include <stdlib.h>
#include <string.h>
//缓冲区
char tcp_client_recvbuf[512];

//关闭与服务器的连接
void tcp_client_connection_close(struct tcp_pcb *tpcb)
{
	//移除回调
	tcp_abort(tpcb);//终止连接,删除pcb控制块
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);  
}

//lwIP tcp_recv()函数的回调函数
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err)
{ 
	//arg参数可以由tcp_arg函数来设置。

	int data_len = 0;
	struct pbuf *q;
	struct tcp_client_struct *es;
	err_t ret_err; 
	//LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(struct tcp_client_struct *)arg; 
	if(p==NULL)//如果从服务器接收到空的数据帧就关闭连接
	{ 
		ret_err=ERR_OK;
	}else if(err!= ERR_OK)//当接收到一个非空的数据帧,但是err!=ERR_OK
	{ 
		if(p)
			pbuf_free(p);//释放接收pbuf
		ret_err=err;
	}else{
		if(p!=NULL)//当处于连接状态并且接收到的数据不为空时
		{
			memset(tcp_client_recvbuf,0,512);  //数据接收缓冲区清零
			for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
			{
				//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
				//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
				if(q->len > (512-data_len)) 
					memcpy(tcp_client_recvbuf+data_len,q->payload,(512-data_len));//拷贝数据
				else 
					memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  

				if(data_len > 512) 
					break; //超出TCP客户端接收数组,跳出	
			}
			tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
			pbuf_free(p);  	//释放内存
			ret_err=ERR_OK;
			printf(tcp_client_recvbuf);//打印出接受的数据
		}
	}
	return ret_err;
}
//lwIP tcp_err函数的回调函数
void tcp_client_error(void *arg,err_t err)
{  
	//这里我们不做任何处理
	char buffer[48];
	sprintf(buffer,"tcp出错%d\n",err);
	printf(buffer);
} 
//此函数用来发送数据
#define SEND_LEN  455
char sendBuffer[SEND_LEN];
void tcp_client_senddata(struct tcp_pcb *tpcb)
{
	struct pbuf *ptr; 
	err_t wr_err=ERR_OK;
	int index = 0;
	int len = 0;
	int last ;
	int i ;;
	for(i=0;i<SEND_LEN;i++)
	{
		sendBuffer[i] = '0';
	}
	sendBuffer[SEND_LEN-1] = 'w';
	len = tcp_sndbuf(tpcb)<SEND_LEN?tcp_sndbuf(tpcb):SEND_LEN;
	while(index<SEND_LEN)
	{
		wr_err=tcp_write(tpcb,sendBuffer+index,len,TCP_WRITE_FLAG_COPY); //将要发送的数据加入到发送缓冲队列中
		tcp_output(tpcb);		//将发送缓冲队列中的数据立即发送出去
		//pbuf_free()
		index = index + len ;
		last = SEND_LEN-index;
		len = last<tcp_sndbuf(tpcb)?last:tcp_sndbuf(tpcb);
	} 	
	
}

//lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	//struct tcp_client_struct *es;
	//LWIP_UNUSED_ARG(len);
	//es=(struct tcp_client_struct*)arg;
	//if(es->p)tcp_client_senddata(tpcb,es);//发送数据
	tcp_client_connection_close(tpcb);//关闭连接
	return ERR_OK;
}

//lwIP TCP连接的回调函数
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{ 
	if(err==ERR_OK)   
	{
			//tcp_arg(tpcb,es);        			//Ê¹ÓÃes¸üÐÂtpcbµÄcallback_arg
		tcp_recv(tpcb,tcp_client_recv);  	//初始化LwIP的tcp_recv回调功能   
		tcp_err(tpcb,tcp_client_error); 	//初始化tcp_err()回调函数
		tcp_sent(tpcb,tcp_client_sent);		//这里是当数据被server接受到了，那么菜调用这个回调函数
		//tcp_poll(tpcb,tcp_client_poll,1); 	//初始化LwIP的tcp_poll回调功能
		tcp_client_senddata(tpcb);
	}else
	{
		tcp_client_connection_close(tpcb);//关闭连接
	}
	return err;
}


void tcp_raw_init()
{
	struct tcp_pcb * tcpPcb  ;
	struct ip_addr rm_ip_addr ;//远端的ip
	//具体过程
	tcpPcb = tcp_new();
	if(tcpPcb)
	{
		//设置远程服务器地址
		IP4_ADDR(&rm_ip_addr,192,168,1,100);
		tcp_connect(tcpPcb,&rm_ip_addr,1000,tcp_client_connected);
	}
}