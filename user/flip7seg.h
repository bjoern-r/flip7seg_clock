
#include <c_types.h>
#include <osapi.h>
#include <c_types.h>
#include <mem.h>
#include <ets_sys.h>
#include <gpio.h>
#include <os_type.h>
#include <user_interface.h>

#ifndef _FLIP7SEG_H_
#define _FLIP7SEG_H_


#define dataPinNo  5
#define clockPinNo 4
#define latchPinNo 2


#define DR _BV(0)
#define RA0 _BV(1)
#define RA1 _BV(2)
#define RA2 _BV(3)
#define DC _BV(4)
#define CA0 _BV(5)
#define CA1 _BV(6)
#define CA2 _BV(7)

#define SEG_A 0
#define SEG_B 2
#define SEG_C 4
#define SEG_D 6
#define SEG_E 5
#define SEG_F 3
#define SEG_G 1
  /*  Pattern:
0 R1 A   5AA
1 R2 G  6   4
2 R3 B  F   B
3 R4 F   7GG
4 R5 C  3   1
5 R6 E  E   C
6 R7 D   2DD
  -> 0b01234567;
  _BV( 76543210  
  */
#define SEGA _BV(2)
#define SEGB _BV(3)
#define SEGC _BV(6)
#define SEGD _BV(5)
#define SEGE _BV(4)
#define SEGF _BV(1)
#define SEGG _BV(0)

#define SAA (0)
#define SAB (0|RA1)
#define SAC (0|RA2)
#define SAD (0|RA2|RA1)
#define SAE (0|RA2|RA0)
#define SAF (0|RA1|RA0)
#define SAG (0|RA0)

#define LET1 (0)
#define LET2 (0|CA0)
#define LET3 (0|CA1)
#define LET4 (0|CA1|CA0)
#define LET5 (0|CA2)

void flip7seg_init();


void do_shift_latch(uint8_t data);
uint8_t to7Segment(char character);
uint8_t do_7seg_to_dot(uint8_t pattern, uint8_t cache, uint8_t letter);
void do_set_all(uint8_t onoff, uint8_t letter);

void do_number_to_7seg(const int number);

void do_display_text(char *str, uint8_t len, uint8_t offset);
void set7c(uint8_t letter, uint8_t pattern);

#endif //_FLIP7SEG_H_