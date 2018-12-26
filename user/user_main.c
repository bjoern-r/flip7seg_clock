//Copyright 2015 <>< Charles Lohr, see LICENSE file.

#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "ets_sys.h"
#include "uart.h"
#include "osapi.h"
#include "espconn.h"
#include "esp82xxutil.h"
#include "commonservices.h"
#include "vars.h"
#include <mdns.h>
#include "flip7seg.h"
#include "sntp.h"
#include "time.h"
#include "clock.h"

#define procTaskPrio        0
#define procTaskQueueLen    1

static volatile os_timer_t some_timer;
static volatile os_timer_t seg7update_timer;
static struct espconn *pUdpServer;
usr_conf_t * UsrCfg = (usr_conf_t*)(SETTINGS.UserData);

//int ICACHE_FLASH_ATTR StartMDNS();

#ifndef WEB_PORT
	#define WEB_PORT 80
#endif
#ifndef COM_PORT
	#define COM_PORT 7777
#endif
#ifndef BACKEND_PORT
	#define BACKEND_PORT 7878
#endif
#ifndef PAGE_OFFSET
	#define PAGE_OFFSET 65536
#endif
#ifndef VERSSTR
	#define VERSSTR "xx"
#endif

void user_rf_pre_init(void) { /*nothing*/ }

//Tasks that happen all the time.

os_event_t    procTaskQueue[procTaskQueueLen];

static void ICACHE_FLASH_ATTR procTask(os_event_t *events)
{
	CSTick( 0 );
	system_os_post(procTaskPrio, 0, 0 );
}

//Timer event.
static void ICACHE_FLASH_ATTR myTimer(void *arg)
{
	CSTick( 1 ); // handle internal ticks 
}

static void ICACHE_FLASH_ATTR update7segTimer(void *arg)
{
	clock_tick();
}


//Called when new packet comes in.
static void ICACHE_FLASH_ATTR
udpserver_recv(void *arg, char *pusrdata, unsigned short len)
{
	struct espconn *pespconn = (struct espconn *)arg;

	uart0_sendStr("X");
}

void ICACHE_FLASH_ATTR charrx( uint8_t c )
{
	//Called from UART.
}

void user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	uart0_sendStr("\r\nesp8266 7flip Web-GUI\r\n" VERSSTR "\b");

//Uncomment this to force a system restore.
//	system_restore();

	CSSettingsLoad( 0 );
	CSPreInit();

	flip7seg_init();
	set7force(1, 0);
	set7force(2, 0);
	set7force(3, 0);
	set7force(4, 0);
	do_display_text("wifi",4,0);

    pUdpServer = (struct espconn *)os_zalloc(sizeof(struct espconn));
	ets_memset( pUdpServer, 0, sizeof( struct espconn ) );
	espconn_create( pUdpServer );
	pUdpServer->type = ESPCONN_UDP;
	pUdpServer->proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	pUdpServer->proto.udp->local_port = COM_PORT;
	espconn_regist_recvcb(pUdpServer, udpserver_recv);

	if( espconn_create( pUdpServer ) )
	{
		while(1) { uart0_sendStr( "\r\nFAULT\r\n" ); }
	}

	start_ntp_clock(100); // 100 means use default timezone from clock.h

	CSInit();

	SetServiceName( "esp7seg" );
	AddMDNSName(    "esp82xx" );
	//AddMDNSName(    "espcom" );
	AddMDNSService( "_http._tcp",    "An esp7seg Webserver", WEB_PORT );
	//AddMDNSService( "_espcom._udp",  "ESP82XX Comunication", COM_PORT );
	AddMDNSService( "_esp82xx._udp", "ESP82XX Backend",      BACKEND_PORT );

	//Add a process
	system_os_task(procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);

	//Timer example
	os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer, (os_timer_func_t *)myTimer, NULL);
	os_timer_arm(&some_timer, 100, 1);

	os_timer_disarm(&seg7update_timer);
	os_timer_setfn(&seg7update_timer, update7segTimer, NULL);
	os_timer_arm(&seg7update_timer, 2000, 1);

	printf( "Boot Ok.\n" );
	do_shift_latch(0b01010100);

	wifi_set_sleep_type(LIGHT_SLEEP_T);
	wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);

	system_os_post(procTaskPrio, 0, 0 );
}


//There is no code in this project that will cause reboots if interrupts are disabled.
void EnterCritical() { }

void ExitCritical() { }


