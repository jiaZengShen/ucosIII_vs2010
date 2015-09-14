#include "includes.h"
#include "LWIP/tcpip.h"
#include "LWIP/pbuf.h"
#include "LWIP/tcp.h"
static err_t my_recv(void *arg ,struct tcp_pcb*pcb,struct pbuf*p,err_t err)
{
	LWIP_UNUSED_ARG(arg);

	if (err == ERR_OK && p != NULL) {
		tcp_recved(pcb, p->tot_len);
		printf("%s",p->payload);
		pbuf_free(p);
	} else {
		pbuf_free(p);
	}

	if (err == ERR_OK && p == NULL) {
		tcp_arg(pcb, NULL);
		tcp_sent(pcb, NULL);
		tcp_recv(pcb, NULL);
		tcp_close(pcb);
	}

	return ERR_OK;
}


static err_t my_accept(void *arg ,struct tcp_pcb *pcb,err_t err)
{
	LWIP_UNUSED_ARG(arg);//È¥µôwarning
	LWIP_UNUSED_ARG(err);

	tcp_arg(pcb,NULL);
	tcp_sent(pcb,NULL);
	tcp_recv(pcb,my_recv);

	return ERR_OK;

}




void my_tcp_server_init(void)
{
	struct tcp_pcb *pcb ;
	pcb = tcp_new();
	tcp_bind(pcb,IP_ADDR_ANY,2000);
	pcb = tcp_listen(pcb);
	tcp_accept(pcb,my_accept);
}