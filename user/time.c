#include "time.h"
#include "esp82xxutil.h"
#include <osapi.h>


#define TM_SEC_IN_DAYNIGHT 86400
#define DAYS_IN_8_YEARS 2922
#define DAYS_BEFORE_2000 10957

int tm_days_in_year[2] = {
	365,
	366
};
int tm_days_in_month[2][12] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

time_t ICACHE_FLASH_ATTR mktime(struct tm * t) {
	int time = ((t->tm_hour * 60) + t->tm_min) * 60 + t->tm_sec;
	int year = t->tm_year - 100; // 1900 -> 2000
	if (year < 0) return 0;
	else if (year >= 100) return TM_MAX_DATE;

	int date = DAYS_BEFORE_2000; // 1970 .. 2000
	int i;
	for (i = 0; i + 7 < year; i += 8) {
		date += DAYS_IN_8_YEARS;
	}
	for (; i < year; i++) {
		date += tm_days_in_year[tm_is_leap(i)];
	}
	int yday = 0;
	char l = tm_is_leap(year);
	for (i = 0; i < t->tm_mon; i++) {
		yday += tm_days_in_month[l][i];
	}
	yday += t->tm_mday - 1;
	date += yday;
	//Extend struct tm
	t->tm_wday = (date + 4) % 7;
	t->tm_yday = yday;

	return (date * TM_SEC_IN_DAYNIGHT) + time;
}

static struct tm tm;

struct tm * ICACHE_FLASH_ATTR localtime(const time_t * t) {
	time_t t1 = *t;
	if (t1 > TM_MAX_DATE) {
		t1 = TM_MAX_DATE;
	}
	int date = (t1 / TM_SEC_IN_DAYNIGHT);
	int time = t1 % TM_SEC_IN_DAYNIGHT;
	int i;
	int year;
	date -= DAYS_BEFORE_2000;
	if (date < 0) date = 0;
	tm.tm_wday = (date + 6) % 7;
	for (i = 100; date >= DAYS_IN_8_YEARS; i += 8) {
		date -= DAYS_IN_8_YEARS;
	}
	for (; date >= tm_days_in_year[tm_is_leap(i)]; i ++ ) {
		date -= tm_days_in_year[tm_is_leap(i)];
	}
	year = i;
	tm.tm_yday = date;
	char l = tm_is_leap(year);
	tm.tm_year = year;
	for (i = 0; date >= tm_days_in_month[l][i]; i++) {
		date -= tm_days_in_month[l][i];
	}
	tm.tm_mon = i;
	tm.tm_mday = date + 1;
	tm.tm_sec = time % 60;
	time /= 60;
	tm.tm_min = time % 60;
	time /= 60;
	tm.tm_hour = time;
	tm.tm_isdst = -1;
 
	return &tm;
}

/*
static char reult[24];

char * ICACHE_FLASH_ATTR
sntp_asctime_r(struct tm *tim_p ,char *result)
{
  static const char day_name[7][4] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char mon_name[12][4] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  os_sprintf (result, "%s %s %02d %02d:%02d:%02d %02d\n",
	   day_name[tim_p->tm_wday],
	   mon_name[tim_p->tm_mon],
	   tim_p->tm_mday, tim_p->tm_hour, tim_p->tm_min,
	   tim_p->tm_sec, 1900 + tim_p->tm_year);
  return result;
}

char *ICACHE_FLASH_ATTR
sntp_asctime(struct tm *tim_p)
{

    return sntp_asctime_r (tim_p, reult);
}
*/