#include "LWIP/api.h"
#include "LWIP/sockets.h"
#include <string.h>
#include <assert.h>
static void   socketClientTestThread(void *arg)
{
	int s;
	int ret;
	int err;
	int opt;
	struct sockaddr_in addr;
	size_t len;
	char rxbuf[1024];
	fd_set readset;
	fd_set errset;
	fd_set writeset ;
	struct timeval tv;

	LWIP_UNUSED_ARG(arg);
	//初始化buffer
	memset(rxbuf,0,1024);
	/* set up address to connect to */
	memset(&addr, 0, sizeof(addr));
	addr.sin_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = PP_HTONS(8080);
	addr.sin_addr.s_addr = inet_addr("192.168.1.100");;//连接的目标地址

	/* first try blocking: */

	/* create the socket */
	s = lwip_socket(AF_INET, SOCK_STREAM, 0);
	LWIP_ASSERT("s >= 0", s >= 0);

	/* connect */
	ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
	/* should succeed */
	LWIP_ASSERT("ret == 0", ret == 0);

	/* set recv timeout (100 ms)，设置为非阻塞 */
	opt = 1000;
	ret = lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &opt, sizeof(int));
	LWIP_ASSERT("ret == 0", ret == 0);

	/* write the start of a GET request */
#define SNDSTR1 "由客户端发出第一个数据"
	len = strlen(SNDSTR1);
	ret = lwip_write(s, SNDSTR1, len);
	LWIP_ASSERT("ret == len", ret == (int)len);

	//从服务器读取数据，我的服务器是总是向外发送数据的
	ret = lwip_read(s, rxbuf, 1);
	printf(rxbuf);//把数据显示

	/* write the rest of a GET request */
#define SNDSTR2 "客户端发出的第二个数据"
	len = strlen(SNDSTR2);
	ret = lwip_write(s, SNDSTR2, len);
	LWIP_ASSERT("ret == len", ret == (int)len);

	/* wait a while: should be enough for the server to send a response */
	sys_msleep(1000);

	

	/* now select should directly return because the socket is readable */
	FD_ZERO(&readset);
	FD_ZERO(&errset);
	FD_ZERO(&writeset);
	FD_SET(s, &readset);
	FD_SET(s,&writeset);
	FD_SET(s, &errset);
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	ret = lwip_select(s + 1, &readset, &writeset, &errset, &tv);
	//LWIP_ASSERT("ret == 1", ret == 1);
	LWIP_ASSERT("!FD_ISSET(s, &errset)", !FD_ISSET(s, &errset));
	LWIP_ASSERT("FD_ISSET(s, &readset)", FD_ISSET(s, &readset));
	ret = FD_ISSET(s,&readset);
	ret = FD_ISSET(s,&writeset);
	/* should not time out but receive a response */
	ret = lwip_read(s, rxbuf, 1024);
	if(ret>0)
	{
		printf(rxbuf);//
	}else{
		assert(0);//我的服务器必须有数据
	}
	/* might receive a second packet for HTTP/1.1 servers */
	//if (ret > 0) {
	//	/* should return 0: closed */
	//	ret = lwip_read(s, rxbuf, 1024);
	//	//LWIP_ASSERT("ret == 0", ret == 0);
	//}

	/* close */
	ret = lwip_close(s);
	LWIP_ASSERT("ret == 0", ret == 0);

	printf("sockex_testrecv finished successfully\n");
}

void socketClientInit()
{
	sys_thread_new("udpThread", socketClientTestThread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}