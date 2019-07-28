#ifndef STRC51IO_H
#define STRC51IO_H

/*
 * Device definitions
 */
#define NO_INPUT_DEVICE 0 
#define RS232_INPUT 1
#define KB_INPUT 2
#define NO_OUTPUT_DEVICE 3
#define RS232_OUTPUT 4
#define LED_OUTPUT 5
#define LCD_OUTPUT 6

/*
 * LED max word size
 */
#define MAX_WORD_SIZE 4

/*
 * LED segments definition
 */
#define SEG_A 0x02
#define SEG_B 0x04
#define SEG_C 0x40
#define SEG_D 0x10
#define SEG_E 0x08
#define SEG_F 0x01
#define SEG_G 0x20
#define SEG_H 0x80

/*
 * LED clear
 */
#define cyfra_null 0

/*
 * Predefined numbers
 */
#define cyfra_0 SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F
#define cyfra_1 SEG_B | SEG_C
#define cyfra_2 SEG_A | SEG_B | SEG_D | SEG_E | SEG_G
#define cyfra_3 SEG_A | SEG_B | SEG_C | SEG_D | SEG_G
#define cyfra_4 SEG_B | SEG_C | SEG_F | SEG_G
#define cyfra_5 SEG_A | SEG_C | SEG_D | SEG_F | SEG_G
#define cyfra_6 SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define cyfra_7 SEG_A | SEG_B | SEG_C
#define cyfra_8 SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define cyfra_9 SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G
#define cyfra_A SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G
#define cyfra_B SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define cyfra_C SEG_A | SEG_D | SEG_E | SEG_F
#define cyfra_D SEG_B | SEG_C | SEG_D | SEG_E | SEG_G
#define cyfra_E SEG_A | SEG_D | SEG_E | SEG_F | SEG_G
#define cyfra_F SEG_A | SEG_E | SEG_F | SEG_G

/*
 * High level functions
 */
void printf(char* s, ...);
void scanf(char* s, ...);
void puts(const char* s);
void gets(char* s);
void putchar(char s);
char getchar();
void devopen(char dev);

/*
 * Primitives
 */
void buzzer_mksound(int frequency);
char rs232_getchar();
void rs232_putchar(char c);
void led_putchar(char c);
void led_putcharpos(char c, int p);
unsigned char kbhit();
unsigned char getkey();
void lcd_putchar(char c);
void create_custom_character(unsigned char *pattern, const char location);
void lcd_clear_screen();
void lcd_clear_chars();
void lcd_puts(const char* s);
void goto_xy(char x, char y);
void send_command_8bit_mode(unsigned char command);
void send_command_4bit_mode(unsigned char command);
void wait(int d);

#endif