#include "app_util.h"

//���ã���sockaddr_inת��Ϊsockadd���ͣ�
//�����ã���ı��ڴ棬
int transport_sockaddr_in_2_sockaddr(struct sockaddr_in *to,struct sockaddr ** pTo)
{
	char temp ;
	//����ڴ��Ƿ���ȷ
	if(to == NULL || pTo == NULL)
		return RESULT_FALSE;
#ifndef WIN32//����������win32�¿���ʹ��
	return RESULT_FALSE;
#endif
	(*pTo) =  (struct sockaddr *)to;
	//����ip��ַת��
	
	temp = (*pTo)->sa_data[2];
	(*pTo)->sa_data[2] = (*pTo)->sa_data[5];
	(*pTo)->sa_data[5] = temp ;

	temp = (*pTo)->sa_data[3] ;
	(*pTo)->sa_data[3] = (*pTo)->sa_data[4];
	(*pTo)->sa_data[4] = temp;

	//�˿�ת��
	temp = (*pTo)->sa_data[0];
	(*pTo)->sa_data[0] = (*pTo)->sa_data[1];
	(*pTo)->sa_data[1] = temp;
	return RESULT_TRUE;
}