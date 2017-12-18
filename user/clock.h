#ifndef _CLOCK_H
#define _CLOCK_H

enum date_time_e{
    SHOW_HH_MM,
    SHOW_MM_SS,
    SHOW_YYYY,
    SHOW_DD_MM,
    SHOW_HH_M0,
    SHOW_DAY_NAME,
    SHOW_MONTH,
    SHOW_NONE,
    SHOW_NOP,
};

enum next_flags_e {
    S_FLAG_IGNORE = _BV(0),
    S_FLAG_FORCE_MIN = _BV(1),
    S_FLAG_STR_MON_DAY = _BV(2),
};

void start_ntp_clock();
int clock_update_7seg(enum date_time_e show_what);
int clock_tick();
void clock_force_next(enum date_time_e next, unsigned int flags);

#endif