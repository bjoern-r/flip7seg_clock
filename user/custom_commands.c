//Copyright 2015 <>< Charles Lohr, see LICENSE file.

#include <commonservices.h>
#include "esp82xxutil.h"
#include "flip7seg.h"
#include "time.h"
#include <sntp.h>
#include "clock.h"

extern enum date_time_e datetime_display;


int ICACHE_FLASH_ATTR CustomCommand(char *buffer, int retsize, char *pusrdata, unsigned short len)
{
	char *buffend = buffer;
	int i;

	switch (pusrdata[1])
	{
	// Custom command test
	case 'C':
	case 'c':
		buffend += ets_sprintf(buffend, "CC");
		printf("CC");
		return buffend - buffer;
		break;
	// Echo to UART
	case 'E':
	case 'e':
		if (retsize <= len)
			return -1;
		ets_memcpy(buffend, &(pusrdata[2]), len - 2);
		buffend[len - 2] = '\0';
		printf("%s\n", buffend);
		return len - 2;
		break;
	// Custom command clear all segments
	case 'T':
	case 't':
		buffend += ets_sprintf(buffend, "cleared");
		for (i = 1; i < 5; i++)
		{
			set7force(i, 0);
		}
		return buffend - buffer;
		break;
	case 'n':
		sntp_stop();
		break;
	// start ntp
	case 'N':
		start_ntp_clock();
		buffend += ets_sprintf(buffend, "startNTP()\n");
		return buffend - buffer;
		break;
	//update time
	case 'u':
		clock_update_7seg(datetime_display);
		return buffend - buffer;
		break;
	//display
	case 'D':
	case 'd':
		if (len==3){
			switch (pusrdata[2]){
				case 'y':
					clock_force_next(SHOW_YYYY, S_FLAG_IGNORE);
					buffend += ets_sprintf(buffend, "Year\n");
					break;
				case 'd':
					clock_force_next(SHOW_DAY_NAME, S_FLAG_STR_MON_DAY );
					buffend += ets_sprintf(buffend, "DD-MM name\n");
					break;
				case 'M':
					clock_force_next(SHOW_MONTH, S_FLAG_IGNORE);
					buffend += ets_sprintf(buffend, "Month\n");
					break;
				case 'm':
					clock_force_next(SHOW_HH_MM, S_FLAG_FORCE_MIN);
					buffend += ets_sprintf(buffend, "HH-MM force\n");
					break;
				case 's':
					clock_force_next(SHOW_MM_SS, S_FLAG_IGNORE);
					buffend += ets_sprintf(buffend, "MM-SS\n");
					break;
				case 'n':
					clock_force_next(SHOW_HH_M0, S_FLAG_FORCE_MIN);
					buffend += ets_sprintf(buffend, "HH-M_\n");
					break;
				case 't':
				default:
					clock_force_next(SHOW_HH_MM, 0);
					buffend += ets_sprintf(buffend, "HH-MM\n");
					break;
			}
			
		}
		buffend += ets_sprintf(buffend, "y d m s n t*\n");
		return buffend - buffer;
		break;
	case 'X':
	case 'x':
		buffend += ets_sprintf(buffend, "flip segment G %s\n", pusrdata[1] == 'X' ? "on" : "off");
		//do_shift_latch(0b00000000);
		do_segment(SEG_G, 2, pusrdata[1] == 'X' ? 1 : 0);
		return buffend - buffer;
		break;

	// set flip7seg
	case 'f':
	case 'F':
		if (len == 6)
		{
			int letter = 5;
			set7c(--letter, to7Segment(pusrdata[2 + (4 - letter)]));
			set7c(--letter, to7Segment(pusrdata[2 + (4 - letter)]));
			set7c(--letter, to7Segment(pusrdata[2 + (4 - letter)]));
			set7c(--letter, to7Segment(pusrdata[2 + (4 - letter)]));
		}
		else if (len == 3)
		{
			set7c(2, to7Segment(pusrdata[2]));
		}
		else
		{
			buffend += ets_sprintf(buffend, "need 4 or 1 args");
			return buffend - buffer;
		}
		break;
	} //switch
	return -1;
}
