
#include <c_types.h>
#include <osapi.h>
#include <c_types.h>
#include <mem.h>
#include <ets_sys.h>
#include <gpio.h>
#include <os_type.h>
#include <user_interface.h>

#include "flip7seg.h"
#include "esp82xxutil.h"

uint8_t cache[5];
int f7_bitdelayH_us;
int f7_bitdelayL_us;
int f7_latchdelay_us;

void do_segment(uint8_t seg, uint8_t letter, uint8_t onoff);
void do_shift_latch(uint8_t data);

void ICACHE_FLASH_ATTR flip7seg_init()
{
    gpio_init();

    f7_bitdelayH_us = 0;
    f7_bitdelayL_us = 0;
    f7_latchdelay_us = 5*1000;

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO4_U);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);
    
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTDI_U);

    GPIO_OUTPUT_SET(GPIO_ID_PIN(latchPinNo), 1);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(dataPinNo), 1);
    GPIO_OUTPUT_SET(GPIO_ID_PIN(clockPinNo), 1);
    gpio_output_set(0, (1<<latchPinNo)|(1<<dataPinNo)|(1<<clockPinNo), (1<<latchPinNo)|(1<<dataPinNo)|(1<<clockPinNo), 0); //set low and enable

}

void ICACHE_FLASH_ATTR shiftOut(const uint8_t dataPin, const uint8_t clockPin, uint8_t data) {
    //gpio_output_set(0, (1 << pin), 0, 0); //set low
    int i;
    for (i=0; i<8; i++) {
        if ((1 & data)==0x01)
            gpio_output_set((1 << dataPin), 0, 0, 0);// set high
        else
            gpio_output_set(0, (1 << dataPin), 0, 0); //set low
        //toggle clock
        gpio_output_set((1 << clockPin), 0, 0, 0);// set high
        os_delay_us(f7_bitdelayH_us);
        gpio_output_set(0, (1 << clockPin), 0, 0); //set low
        os_delay_us(f7_bitdelayL_us);
        data >>= 1;
        // expression
    }
}

void ICACHE_FLASH_ATTR do_shift_latch(uint8_t data)
{
    gpio_output_set(0, (1 << latchPinNo), (1 << latchPinNo), 0); //set low
    shiftOut(dataPinNo, clockPinNo, data);
    gpio_output_set((1 << latchPinNo), 0, (1 << latchPinNo), 0);// set high
    //os_delay_us(50);
    os_delay_us(f7_latchdelay_us);
    gpio_output_set(0, (1 << latchPinNo), (1 << latchPinNo), 0); //set low
}

void ICACHE_FLASH_ATTR do_segment(uint8_t seg, uint8_t letter, uint8_t onoff)
{
    uint8_t dat = 0;
    // C2 C1 C0 DC R2 R1 R0 DR (lsb)
    if (onoff)
        dat |= DR;
    else
        dat |= DC;
    dat |= (letter & 0x07) << 5;
    dat |= (seg & 0x07) << 1;
    //do_shift_latch(dat);
    gpio_output_set(0, (1 << latchPinNo), 0, 0); //set low

    shiftOut(dataPinNo, clockPinNo, dat);
    gpio_output_set((1 << latchPinNo), 0, 0, 0);// set high

    //delay(5);
    os_delay_us(f7_latchdelay_us);
    gpio_output_set(0, (1 << latchPinNo), 0, 0); //set low
}

uint8_t ICACHE_FLASH_ATTR to7Segment(char character)
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
  3   1
  3   1
   222  0
   
  -> 0b01234567;
  */
    switch (character)
    {
    case 0:
        bits = 0b01111110; //0x7e
        break;
    case 1:
        bits = 0b01001000; //0x48
        break;
    case 2:
    case 'z':
        bits = 0b00111101; //0x3d
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

uint8_t ICACHE_FLASH_ATTR do_7seg_to_dot(uint8_t pattern, uint8_t cache, uint8_t letter)
{
    if (cache == pattern)
        return pattern;

    uint8_t changes = cache ^ pattern;

    if (changes | _BV(0))
        do_segment(SEG_G, letter, (pattern & _BV(0)));
    if (changes | _BV(1))
        do_segment(SEG_F, letter, (pattern & _BV(1)));
    if (changes | _BV(2))
        do_segment(SEG_A, letter, (pattern & _BV(2)));
    if (changes | _BV(3))
        do_segment(SEG_B, letter, (pattern & _BV(3)));
    if (changes | _BV(4))
        do_segment(SEG_E, letter, (pattern & _BV(4)));
    if (changes | _BV(5))
        do_segment(SEG_D, letter, (pattern & _BV(5)));
    if (changes | _BV(6))
        do_segment(SEG_C, letter, (pattern & _BV(6)));
    return pattern;
}

void ICACHE_FLASH_ATTR do_set_all(uint8_t onoff, uint8_t letter)
{
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

void ICACHE_FLASH_ATTR do_number_to_7seg(const int number)
{
    int tmp = number;

    cache[1] = do_7seg_to_dot(to7Segment(tmp % 10), cache[1], 1);
    int i;
    for (i = 2; i < 5; i++)
    {
        tmp = tmp / 10;
        if (tmp)
            cache[i] = do_7seg_to_dot(to7Segment(tmp % 10), cache[i], i);
        else
            cache[i] = do_7seg_to_dot(0, cache[i], i);
    }
}

void ICACHE_FLASH_ATTR do_display_text(char *str, uint8_t len, uint8_t offset)
{
    int i;
    for (i = 1; i <= 5; i++)
    {
        //cache[i] = do_7seg_to_dot((len > (offset + 4 - i)) ? to7Segment(str[offset + 4 - i]) : 0, cache[i], i);
        set7c(i, (len > (offset + 4 - i)) ? to7Segment(str[offset + 4 - i]) : 0);
    }
}

void ICACHE_FLASH_ATTR set7c(uint8_t letter, uint8_t pattern){
    cache[letter] = do_7seg_to_dot(pattern, cache[letter], letter);
}
void ICACHE_FLASH_ATTR set7force(uint8_t letter, uint8_t pattern){
    cache[letter] = do_7seg_to_dot(pattern, !pattern, letter);
}

//#define set7c(letter, pattern) cache[letter] = do_7seg_to_dot(pattern, cache[letter], letter);
//#define set7force(letter, pattern) cache[letter] = do_7seg_to_dot(pattern, !pattern, letter);