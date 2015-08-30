/*
工具类：
作者jeanWin,
*/
#include "lwip/sockets.h"
#define RESULT_TRUE  1
#define RESULT_FALSE 0  //出错
//作用：把sockaddr_in转化为sockadd类型，
//副作用，会改变内存，
int transport_sockaddr_in_2_sockaddr(struct sockaddr_in *to,struct sockaddr ** pTo);
