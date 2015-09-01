#include "app_util.h"

//作用：把sockaddr_in转化为sockadd类型，
//副作用，会改变内存，
int transport_sockaddr_in_2_sockaddr(struct sockaddr_in *to,struct sockaddr ** pTo)
{
	char temp ;
	//检查内存是否正确
	if(to == NULL || pTo == NULL)
		return RESULT_FALSE;
#ifndef WIN32//仅仅可以在win32下可以使用
	return RESULT_FALSE;
#endif
	(*pTo) =  (struct sockaddr *)to;
	//进行ip地址转换
	
	temp = (*pTo)->sa_data[2];
	(*pTo)->sa_data[2] = (*pTo)->sa_data[5];
	(*pTo)->sa_data[5] = temp ;

	temp = (*pTo)->sa_data[3] ;
	(*pTo)->sa_data[3] = (*pTo)->sa_data[4];
	(*pTo)->sa_data[4] = temp;

	//端口转化
	temp = (*pTo)->sa_data[0];
	(*pTo)->sa_data[0] = (*pTo)->sa_data[1];
	(*pTo)->sa_data[1] = temp;
	return RESULT_TRUE;
}