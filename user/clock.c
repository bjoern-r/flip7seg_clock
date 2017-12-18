
#include <stdint.h>
#include <sntp.h>
#include "esp82xxutil.h"
#include "time.h"
#include "flip7seg.h"
#include "clock.h"

char * ntp_servers[] = {
    "1.europe.pool.ntp.org",
    "2.europe.pool.ntp.org",
    "time-c.nist.gov"
};
#define NTP_OFFSET 1 // GMT

enum date_time_e datetime_display = SHOW_HH_MM;
struct clock_next_t{
	unsigned int wait_ticks;
	enum date_time_e next;
    unsigned int flags;
}d_next = {0, SHOW_HH_MM, 0};


void ICACHE_FLASH_ATTR start_ntp_clock()
{
    int i;
    uint32 tm = 0;
    int ntp_servers_len = sizeof(ntp_servers) / sizeof(ntp_servers[0]);
    sntp_stop();
    for (i = 0; i < 3; i++)
    {
        if (i >= ntp_servers_len)
            break;
        sntp_setservername(i, ntp_servers[i]);
        printf("sntp server: %s\n", ntp_servers[i]);
    }
    sntp_set_timezone(NTP_OFFSET);
    sntp_init();
}

int ICACHE_FLASH_ATTR clock_update_7seg(enum date_time_e show_what)
{
    time_t tim = (time_t)sntp_get_current_timestamp();
    struct tm *t;
    char buf[5];
    char *dates=0;
    if (tim > 0)
    {
        t = sntp_localtime(&tim);
        printf("localtime ok, show_what: %d\n",show_what);
        //printf("NTP Time: %02d:%02d:%02d %02d.%02d.%04d (%d)sec\r\n", t->tm_hour, t->tm_min, t->tm_sec, t->tm_mday, t->tm_mon , t->tm_year + 1900, tim);
        dates = sntp_asctime(t);
        //Sat Dec 16 15:14:37 2017 -> len=24
        //0   3
        if (!dates)
            return 0;
        printf("  %s",dates);
        switch (show_what){
            case SHOW_HH_MM:
                //do_number_to_7seg(t->tm_hour * 100 + t->tm_min);
                os_sprintf(buf, "%2d%02d", t->tm_hour, (t->tm_min));
                do_display_text(buf,4,0);
                break;
            case SHOW_MM_SS:
                os_sprintf(buf, "%02d%02d", t->tm_min, t->tm_sec);
                do_display_text(buf,4,0);
                //do_number_to_7seg(t->tm_min * 100 + t->tm_sec);
                break;
            case SHOW_YYYY:
                do_number_to_7seg(t->tm_year + 1900);
                break;
            case SHOW_DD_MM:
                do_number_to_7seg(t->tm_mday*100 + t->tm_mon+1);
                break;
            case SHOW_HH_M0:
                os_sprintf(buf, "%2d%1d_", t->tm_hour, (t->tm_min/10));
                do_display_text(buf,4,0);
                break;
            case SHOW_MONTH:
                do_display_text(dates,24,3);
                break;
            case SHOW_DAY_NAME:
                do_display_text(dates,24,0);
                break;
            default:
                printf("err: unknown what\n");
        }//switch
        return tim;
    }
    else
    {
        printf("time error\n");
        return 0;
    }
}

void ICACHE_FLASH_ATTR clock_force_next(enum date_time_e next, unsigned int flags){
    d_next.wait_ticks=0;
    d_next.next = next;
    if (flags==S_FLAG_IGNORE)
        return;
    d_next.flags = flags;
}

int ICACHE_FLASH_ATTR clock_select_next(struct clock_next_t* dat){
    printf("IN sel_next: %d flags 0x%X\n", dat->next, dat->flags);
    switch (dat->next){
        case SHOW_DAY_NAME:
            dat->next=SHOW_DD_MM;
            dat->wait_ticks=1;
            break;
        case SHOW_DD_MM:
            dat->next=SHOW_YYYY;
            dat->wait_ticks=1;
            break;
        case SHOW_YYYY:
            dat->next=SHOW_HH_MM;
            dat->wait_ticks=1;
            break;
        case SHOW_HH_MM:
        case SHOW_HH_M0:{
            time_t tim = (time_t)sntp_get_current_timestamp();
            if (tim && !(dat->flags & S_FLAG_FORCE_MIN)){
                struct tm *t = sntp_localtime(&tim);
                if (((t->tm_hour > 7) && (t->tm_hour < 22)) )
                    dat->next=SHOW_HH_MM;
                else
                    dat->next=SHOW_HH_M0;
            }
            break;
        }
        case SHOW_MONTH:
            dat->next=SHOW_YYYY;
            dat->wait_ticks=4;
            break;
            break;
        case SHOW_MM_SS:
        case SHOW_NOP:
            break;
        default:
            //dat->next=SHOW_HH_MM;
        break;
    }
    return 1;
}

int ICACHE_FLASH_ATTR clock_tick(){
    if (d_next.wait_ticks > 0){
        d_next.wait_ticks--;
    }else{
        if (d_next.next != SHOW_NOP)
            datetime_display = d_next.next;
        clock_select_next(&d_next);
    }
    clock_update_7seg(datetime_display);
}