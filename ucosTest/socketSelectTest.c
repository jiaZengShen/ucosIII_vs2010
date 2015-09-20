#include "LWIP/api.h"
#include "LWIP/sockets.h"
#include <string.h>
//typedef int SOCKET ;
//typedef struct sockaddr_in SOCKADDR_IN;
#define SOCKET_ERROR -1  //socket���ִ���
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

	int   ListenSocket;					// �����׽���
	int   AcceptSocket;					// ��ͻ��˽���ͨ�ŵ��׽���
	struct sockaddr_in    InternetAddr;			// �������ĵ�ַ


	struct fd_set   WriteSet;							// ��ȡ��д�Ե��׽��ּ���
	struct fd_set   ReadSet;							// ��ȡ�ɶ��Ե��׽��ּ���
	
	int noneBlock = 1 ;
	int recvLen = 0;
	int len = 0;
	int  bSocketInit = 0;//socket�Ƿ��ʼ����
	int err ;
	int blockTime = 1000;//1�������
	struct timeval waiteTime ;
	waiteTime.tv_sec =0;
	waiteTime.tv_usec = 1;
	
	lwip_socket_init();

	ListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	InternetAddr.sin_family = AF_INET;   
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);   
	InternetAddr.sin_port = htons(8080); 
	//��
	if(bind(ListenSocket, (struct sockaddr*)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)   
	{   
		printf("bind()   failed   with   error\n");   
		return ;   
	} 
	//����
	listen(ListenSocket,1);
	//���÷�����
	
	//lwip_setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO, &blockTime, sizeof(int));
	//ioctlsocket(ListenSocket,FIONBIO,&noneBlock);
	
	while(1)
	{
		//���
		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);

		//ָ��listensocket���Զ�
		FD_SET(ListenSocket,&ReadSet);

		if(bSocketInit)
		{
			FD_SET(AcceptSocket,&ReadSet);//ע�⣬��ֻ������һ������Ϊ���������
			FD_SET(AcceptSocket,&WriteSet);
		}

		//ѭ������select����
		/*
		select�����ĵ�һ�������������socket�ĸ���������Ҳ����̫��̫����Ҳ����
		*/
		select(3,&ReadSet,&WriteSet,NULL,&waiteTime);//����������Ϣ��������

		//�ж���û���µ�socket����
		if(FD_ISSET(ListenSocket,&ReadSet))
		{
			AcceptSocket = accept(ListenSocket,NULL,NULL);
			//����������socketΪ������
			//ioctlsocket(AcceptSocket,FIONBIO,&noneBlock);
			//lwip_setsockopt(AcceptSocket, SOL_SOCKET, SO_RCVTIMEO, &blockTime, sizeof(int));
			bSocketInit = 1;//��������һ��
		}
		//�ж��µ�socket
		if(bSocketInit)
		{
			if(FD_ISSET(AcceptSocket,&ReadSet)){
				//��������
				len = recv(AcceptSocket,recvBuffer,512,0);
				if(len>0){
					recvLen = len;
				}else{
					//�ͻ��˹ر���
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