/*
�����ࣺ
����jeanWin,
*/
#include "lwip/sockets.h"
#define RESULT_TRUE  1
#define RESULT_FALSE 0  //����
//���ã���sockaddr_inת��Ϊsockadd���ͣ�
//�����ã���ı��ڴ棬
int transport_sockaddr_in_2_sockaddr(struct sockaddr_in *to,struct sockaddr ** pTo);
