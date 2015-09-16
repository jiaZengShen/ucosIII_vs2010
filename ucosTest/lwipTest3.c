/* C runtime includes */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <conio.h>

/* lwIP core includes */
#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"

#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"

/* lwIP netif includes */
#include "netif/etharp.h"

/* include the port-dependent configuration */
#include "lwipcfg_msvc.h"

#include "pcapif.h"


#if NO_SYS
/* ... then we need information about the timer intervals: */
#include "lwip/ip_frag.h"
#include "lwip/igmp.h"
#endif /* NO_SYS */

#if PPP_SUPPORT
/* PPP includes */
#include "../netif/ppp/ppp_impl.h"
#include "lwip/sio.h"
#include "netif/ppp_oe.h"
#endif /* PPP_SUPPORT */

/* include the port-dependent configuration */
#include "lwipcfg_msvc.h"
#include "echo.h"
/** Define this to 1 to enable a PCAP interface as default interface. */
#ifndef USE_PCAPIF
#define USE_PCAPIF 1
#endif

/** Define this to 1 or 2 to support 1 or 2 SLIP interfaces. */
#ifndef USE_SLIPIF
#define USE_SLIPIF 0
#endif

/** Use an ethernet adapter? Default to enabled if PCAPIF or PPPoE are used. */
#ifndef USE_ETHERNET
#define USE_ETHERNET  (USE_PCAPIF || PPPOE_SUPPORT)
#endif

/** Use an ethernet adapter for TCP/IP? By default only if PCAPIF is used. */
#ifndef USE_ETHERNET_TCPIP
#define USE_ETHERNET_TCPIP  (USE_PCAPIF)
#endif

#if USE_ETHERNET
#include "pcapif.h"
#endif /* USE_ETHERNET */
#if USE_SLIPIF
#include <netif/slipif.h>
#endif /* USE_SLIPIF */

#ifndef USE_DHCP
#define USE_DHCP    LWIP_DHCP
#endif
#ifndef USE_AUTOIP
#define USE_AUTOIP  LWIP_AUTOIP
#endif

/* globales variables for netifs */
#if USE_ETHERNET
/* THE ethernet interface */
struct netif netif;
#if LWIP_DHCP
/* dhcp struct for the ethernet netif */
struct dhcp netif_dhcp;
#endif /* LWIP_DHCP */
#if LWIP_AUTOIP
/* autoip struct for the ethernet netif */
struct autoip netif_autoip;
#endif /* LWIP_AUTOIP */
#endif /* USE_ETHERNET */
#if PPP_SUPPORT
/* THE PPP descriptor */
int ppp_desc = -1;
u8_t sio_idx = 0;
sio_fd_t ppp_sio;
#endif /* PPP_SUPPORT */
#if USE_SLIPIF
struct netif slipif1;
#if USE_SLIPIF > 1
struct netif slipif2;
#endif /* USE_SLIPIF > 1 */
#endif /* USE_SLIPIF */



void status_callback(struct netif *netif)
{
	if (netif_is_up(netif)) {
		printf("status_callback==UP, local interface IP is %s\n", ip_ntoa(&netif->ip_addr));
	} else {
		printf("status_callback==DOWN\n");
	}
}



void link_callback(struct netif *netif)
{
	if (netif_is_link_up(netif)) {
		printf("link_callback==UP\n");
	} else {
		printf("link_callback==DOWN\n");
	}
}



/* This function initializes all network interfaces */
static void	msvc_netif_init()
{
	ip_addr_t ipaddr, netmask, gw;



	


	ip_addr_set_zero(&gw);
	ip_addr_set_zero(&ipaddr);
	ip_addr_set_zero(&netmask);


	LWIP_PORT_INIT_GW(&gw);//设置网关
	LWIP_PORT_INIT_IPADDR(&ipaddr);//设置ip地址
	LWIP_PORT_INIT_NETMASK(&netmask);//设置掩码
	printf("Starting lwIP, local interface IP is %s\n", ip_ntoa(&ipaddr));
	printf("the 网关是%s\n",ip_ntoa(&gw) );



	netif_set_default(netif_add(&netif, &ipaddr, &netmask, &gw, NULL, pcapif_init, tcpip_input));


	netif_set_status_callback(&netif, status_callback);


	netif_set_link_callback(&netif, link_callback);
	netif_set_up(&netif);
}




void my_tcp_server_init(void);
void tcp_raw_init() ;
void  udpNetCommInit();
#include "udpRawTest.h"
/* This function initializes applications */
static void	apps_init()
{
	//echo_init();
	//tcp
	//my_tcp_server_init();
	
	//udp raw接口测试
	//udpRawTestInit();	
	
	//tcp_raw_init();

	udpNetCommInit();
}




/* This function initializes this lwIP test. When NO_SYS=1, this is done in
 * the main_loop context (there is no other one), when NO_SYS=0, this is done
 * in the tcpip_thread context */
static void
test_init(void * arg)
{ /* remove compiler warning */

  sys_sem_t *init_sem;
  LWIP_ASSERT("arg != NULL", arg != NULL);
  init_sem = (sys_sem_t*)arg;


  /* init network interfaces */
  msvc_netif_init();

  /* init apps */
  apps_init();

  sys_sem_signal(init_sem);

}


/* This is somewhat different to other ports: we have a main loop here:
 * a dedicated task that waits for packets to arrive. This would normally be
 * done from interrupt context with embedded hardware, but we don't get an
 * interrupt in windows for that :-) */
void main_loop()
{

  err_t err;
  sys_sem_t init_sem;



  /* initialize lwIP stack, network interfaces and applications */

  err = sys_sem_new(&init_sem, 0);
  tcpip_init(test_init, &init_sem);
  /* we have to wait for initialization to finish before
   * calling update_adapter()! */
  sys_sem_wait(&init_sem);
  sys_sem_free(&init_sem);


  /* MAIN LOOP for driver update (and timers if NO_SYS) */
  while (!_kbhit()) {




    sys_msleep(50);



  }



  /* release the pcap library... */
  pcapif_shutdown(&netif);

}

void main(void)

{

	/* no stdio-buffering, please! */
	setvbuf(stdout, NULL,_IONBF, 0);

	main_loop();

	return ;
}