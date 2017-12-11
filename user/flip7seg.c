
#include <c_types.h>
#include <osapi.h>
#include <c_types.h>
#include <mem.h>
#include <ets_sys.h>
#include <gpio.h>
#include <os_type.h>
#include <user_interface.h>

#include "flip7seg.h"

static uint8_t cache[5];

void flip7seg_init()
{
    pinMode(13, OUTPUT); //LED
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    digitalWrite(latchPin, LOW);

    do_shift_latch(0b00000000);
}

uint8_t to7Segment(char character)
{
    uint8_t bits;
    if (character >= 'A' && character <= 'Z')
    {
        character -= 'A' - 'a';
    }
    if (character >= '0' && character <= '9')
    {
        character -= '0';
    }
    /*
  Pattern:
   555
  6   4
  6   4
   777
  3   5
  3   1
   222  0
   
  -> 0b01234567;
  */
    switch (character)
    {
    case 0:
        bits = 0b01111110;
        break;
    case 1:
        bits = 0b01001000;
        break;
    case 2:
    case 'z':
        bits = 0b00111101;
        break;
    case 3:
        bits = 0b01101101;
        break;
    case 4:
        bits = 0b01001011;
        break;
    case 5:
    case 's':
        bits = 0b01100111;
        break;
    case 6:
        bits = 0b01110111;
        break;
    case 7:
        bits = 0b01001100;
        break;
    case 8:
        bits = 0b01111111;
        break;
    case 9:
        bits = 0b01101111;
        break;
    case 'a':
        bits = 0b01011111;
        break;
    case 'b':
        bits = 0b01110011;
        break;
    case 'c':
        bits = 0b00110001;
        break;
    case 'd':
        bits = 0b01111001;
        break;
    case 'e':
        bits = 0b00110111;
        break;
    case 'f':
        bits = 0b00010111;
        break;
    case 'g':
        bits = 0b01110110;
        break;
    case 'h':
        bits = 0b01010011;
        break;
    case 'i':
        bits = 0b00010010;
        break;
    case 'j':
        bits = 0b01111100;
        break;
    case 'k':
        bits = 0b01010111;
        break;
    case 'l':
        bits = 0b00110010;
        break;
    case 'm':
        bits = 0b01011110;
        break;
    case 'n':
        bits = 0b01010001;
        break;
    case 'o':
        bits = 0b01110001;
        break;
    case 'p':
        bits = 0b00011111;
        break;
    case 'q':
        bits = 0b01001111;
        break;
    case 'r':
        bits = 0b00010001;
        break;
    case 't':
        bits = 0b00110011;
        break;
    case 'u':
    case 'v':
        bits = 0b01110000;
        break;
    case 'w':
        bits = 0b01111010;
        break;
    case 'x':
        bits = 0b01011011;
        break;
    case 'y':
        bits = 0b01001011;
        break;
    case ' ':
        bits = 0;
        break;
    case '.':
        bits = 0b10000000;
        break;
    case '-':
        bits = 0b00000001;
        break;
    case '_':
        bits = 0b00100000;
        break;
    case '=':
        bits = 0b00100001;
        break;
    case '/':
        bits = 0b00011001;
        break;
    case '\\':
        bits = 0b01000011;
        break;
    case '(':
    case '[':
        bits = 0b00110110;
        break;
    case ')':
    case ']':
        bits = 0b01101100;
        break;
    case '"':
        bits = 0b00001010;
        break;
    case '\'':
        bits = 0b00000010;
        break;
    case '´':
        bits = 0b00000110;
        break;
    case '`':
        bits = 0b00001100;
        break;
    case '!':
        bits = 0b10001000;
        break;
    case '^':
        bits = 0b00001110;
        break;
    case '~':
        bits = 0b00001111;
        break;
    case ',':
        bits = 0b01000000;
        break;
    case '@':
        bits = 0b01111101;
        break;
    case '?':
        bits = 0b10001100;
        break;
    default:
        bits = 0b00100101;
        break;
    }
    return bits;
}

uint8_t do_7seg_to_dot(uint8_t pattern, uint8_t cache, uint8_t letter)
{
    if (cache == pattern)
        return pattern;

    uint8_t changes = cache ^ pattern;

    if (changes | _BV(0))
        do_segment2(SEG_G, letter, (pattern & _BV(0)));
    if (changes | _BV(1))
        do_segment2(SEG_F, letter, (pattern & _BV(1)));
    if (changes | _BV(2))
        do_segment2(SEG_A, letter, (pattern & _BV(2)));
    if (changes | _BV(3))
        do_segment2(SEG_B, letter, (pattern & _BV(3)));
    if (changes | _BV(4))
        do_segment2(SEG_E, letter, (pattern & _BV(4)));
    if (changes | _BV(5))
        do_segment2(SEG_D, letter, (pattern & _BV(5)));
    if (changes | _BV(6))
        do_segment2(SEG_C, letter, (pattern & _BV(6)));
    return pattern;
}

void do_set_all(uint8_t onoff, uint8_t letter)
{
    //for(uint8_t seg=0; seg<7; seg++){
    //  do_segment(seg, letter, onoff);
    //}
    uint8_t dat = letter;
    if (onoff)
        dat |= DR;
    else
        dat |= DC;
    do_shift_latch(dat | SAA);
    do_shift_latch(dat | SAB);
    do_shift_latch(dat | SAC);
    do_shift_latch(dat | SAD);
    do_shift_latch(dat | SAE);
    do_shift_latch(dat | SAF);
    do_shift_latch(dat | SAG);
}

void do_number_to_7seg(const int number)
{
    int tmp = number;

    cache[1] = do_7seg_to_dot(to7Segment(tmp % 10), cache[1], 1);
    for (int i = 2; i < 5; i++)
    {
        tmp = tmp / 10;
        if (tmp)
            cache[i] = do_7seg_to_dot(to7Segment(tmp % 10), cache[i], i);
        else
            cache[i] = do_7seg_to_dot(0, cache[i], i);
    }
}

void do_display_text(char *str, uint8_t len, uint8_t offset)
{
    for (int i = 1; i <= 5; i++)
    {
        cache[i] = do_7seg_to_dot((len > (offset + 4 - i)) ? to7Segment(str[offset + 4 - i]) : 0, cache[i], i);
    }
}

#define set7c(letter, pattern) cache[letter] = do_7seg_to_dot(pattern, cache[letter], letter);
#define set7force(letter, pattern) cache[letter] = do_7seg_to_dot(pattern, !pattern, letter);