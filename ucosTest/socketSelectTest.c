#include "LWIP/api.h"
#include "LWIP/sockets.h"
#include <string.h>
//typedef int SOCKET ;
//typedef struct sockaddr_in SOCKADDR_IN;
#define SOCKET_ERROR -1  //socket出现错误
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


void socketSelectThread(void *arg)
{

	int   ListenSocket;					// 监听套接字
	int   AcceptSocket;					// 与客户端进行通信的套接字
	struct sockaddr_in    InternetAddr;			// 服务器的地址


	struct fd_set   WriteSet;							// 获取可写性的套接字集合
	struct fd_set   ReadSet;							// 获取可读性的套接字集合
	
	int noneBlock = 1 ;
	int recvLen = 0;
	int len = 0;
	int  bSocketInit = 0;//socket是否初始化过
	int err ;
	int blockTime = 1000;//1秒的阻塞
	struct timeval waiteTime ;
	waiteTime.tv_sec =0;
	waiteTime.tv_usec = 1;
	
	lwip_socket_init();

	ListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	InternetAddr.sin_family = AF_INET;   
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);   
	InternetAddr.sin_port = htons(8080); 
	//绑定
	if(bind(ListenSocket, (struct sockaddr*)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)   
	{   
		printf("bind()   failed   with   error\n");   
		return ;   
	} 
	//监听
	listen(ListenSocket,1);
	//设置费阻塞
	
	//lwip_setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO, &blockTime, sizeof(int));
	//ioctlsocket(ListenSocket,FIONBIO,&noneBlock);
	
	while(1)
	{
		//清空
		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);

		//指定listensocket可以读
		FD_SET(ListenSocket,&ReadSet);

		if(bSocketInit)
		{
			FD_SET(AcceptSocket,&ReadSet);//注意，就只能连接一个，因为这样代码简单
			FD_SET(AcceptSocket,&WriteSet);
		}

		//循环调用select函数
		/*
		select函数的第一个参数必须大于socket的个数，但是也不能太大，太大了也出错。
		*/
		select(3,&ReadSet,&WriteSet,NULL,&waiteTime);//不检测错误信息，不阻塞

		//判断有没有新的socket连接
		if(FD_ISSET(ListenSocket,&ReadSet))
		{
			AcceptSocket = accept(ListenSocket,NULL,NULL);
			//设置新来的socket为非阻塞
			//ioctlsocket(AcceptSocket,FIONBIO,&noneBlock);
			//lwip_setsockopt(AcceptSocket, SOL_SOCKET, SO_RCVTIMEO, &blockTime, sizeof(int));
			bSocketInit = 1;//就能连接一个
		}
		//判断新的socket
		if(bSocketInit)
		{
			if(FD_ISSET(AcceptSocket,&ReadSet)){
				//接受数据
				len = recv(AcceptSocket,recvBuffer,512,0);
				if(len>0){
					recvLen = len;
				}else{
					//客户端关闭了
					bSocketInit = 0;
					closesocket(AcceptSocket);
					len = 0;
					recvLen = 0;
				}
			}
			if(FD_ISSET(AcceptSocket,&WriteSet))
			{
				//
				if(recvLen>0)
				{
					if( send(AcceptSocket,recvBuffer,recvLen,0)
						!= SOCKET_ERROR)
					{
						recvLen = 0;
						printf(recvBuffer);
					}
				}
			}
		}
	}
}

void socketSelectInit()
{
	sys_thread_new("udpThread", socketSelectThread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}