#include "stdarg.h"
#include "strc51io.h"
#include <8051.h>

__xdata __at 0x8000 unsigned char U15;
__xdata __at 0x8000 unsigned char U12;
__xdata __at 0xFFFF unsigned char U10;

/*
 * NULL macro definition
 */
#define NULL (void *)0

/*
 * Keyboard column keys
 */
#define key_0 0xe0
#define key_1 0xd0
#define key_2 0xb0
#define key_3 0x70

/*
 * LCD sygnals
 */

#define E 0x08
#define RS 0x04

/*
 * LCD line length
 */
#define MAX_LINE_LCD 16

 /*
  * Globals
  */
char r = 0;
char t = 0;

typedef unsigned char(*getchar_func)();
typedef void(*putchar_func)(char);
 
getchar_func getc = NULL;
putchar_func putc = NULL;

//LED
const char segment_address[4] = { 0xDF, 0xEF, 0x7F, 0xBF };
char word[4];
char ledpos = 0;

//keyboard
const unsigned char column_address[4] = { 0xfe, 0xfd, 0xfb, 0xf7 };
int key = -1;
int key_pressed = 0;

//LCD
char pos_x = 0;
char pos_y = 0;

//Micsellaneous

//number to string
char *convert(int num, char base) { 
    char *ptr; 
    char buffer[12];

	ptr = &buffer[11]; 
	*ptr = '\0'; 
	do { 
		*--ptr = (num % base) > 9 ? (num % base) + 'A' : (num % base) + '0'; 
		num /= base; 
	} while(num != 0);

	return(ptr); 
}

//decimal
int dtoi(char* s) {
    int i;
    int c, n;
 
    n = 0;
    for (i = 0; (c = s[i]) != '\0'; ++i) {
        n *= 10;
        if (c >= '0' && c <= '9')
            n += c - '0';
        else
            return n;
    }
    return n;
}

//octal
int otoi(char* s) {
    int i;
    int c, n;
 
    n = 0;
    for (i = 0; (c = s[i]) != '\0'; ++i) {
        n *= 8;
        if (c >= '0' && c <= '7')
            n += c - '0';
        else
            return n;
    }
    return n;
}

//hexadecimal
int htoi(char* s) {
    int i;
    int c, n;
    
    n = 0;
    for (i = 0; (c = s[i]) != '\0'; ++i) {
        n *= 16;
        if (c >= '0' && c <= '9')
            n += c - '0';
        else if (c >= 'a' && c <= 'f')
            n += 10 + (c - 'a');
        else if (c >= 'A' && c <= 'F')
            n += 10 + (c - 'A');
        else
            return n;
    }
    return n;
}

void wait(int n){
    unsigned int i;
    for(i = 0; i < n; ++i) { }
}

/*
 * Main functions implementation
 */
void putchar(char c) {
	if (putc == NULL) return;
	putc(c);
}

void puts(const char *string) {
	if (putc == NULL) return;
	if (putc == lcd_putchar) lcd_puts(string);
	while (*string != '\0') {
		putchar(*string++);
	}
}

void printf(char* format, ...) {
	long i;
	char *s;

	va_list arg;
	va_start(arg, format);

	if (putc == NULL) return;
	if (putc == led_putchar) return;

	for (; *format != '\0'; format++) {
		if (*format == '%') {
			++format;
			switch (*format) {
			case 'c':
				i = va_arg(arg, char);
				putchar(i);
				break;
			case 's':
				s = va_arg(arg, char*);
				putc == lcd_putchar ? lcd_puts(s) : puts(s);
				break;
			case 'd':
				i = va_arg(arg, int);
				putc == lcd_putchar ? lcd_puts(convert(i, 10)) : puts(convert(i, 10));
				break;
			case 'o':
				i = va_arg(arg, int);
				putc == lcd_putchar ? lcd_puts(convert(i, 8)) : puts(convert(i, 8));
				break;
			case 'x':
				i = va_arg(arg, int);
				putc == lcd_putchar ? lcd_puts(convert(i, 16)) : puts(convert(i, 16));
				break;
			case '%':
				putchar('%');
				break;
			}
		}
		else {
			putchar(*format);
		}
	}
	va_end(arg);
}

char getchar() {
	char c;
	if (getc == NULL) return -1;

	c = getc();
	return c;
}

void gets(char* s) {
	char c;
	if (getc == NULL) return;
	if (getc == getkey) return;
	while ((c = getchar()) != '\r') {
		*s++ = c;
	}
	*s = '\0';
	putchar('\n');
	putchar('\r');
}

void scans(char* s) {
	char c;
	if (getc == NULL) return;
	if (getc == getkey) return;
	while ((c = getchar()) != '\r' && c != ' ') {
		*s++ = c;
	}
	*s = '\0';
}

void scanf(char* format, ...) {
	int* i;
	char* s;
	char buf[6];

	va_list arg;
	va_start(arg, format);

	if (getc == NULL) return;
	if (getc == getkey) return;

	for (; *format != '\0'; format++) {
		if (*format == '%') {
			++format;
			switch (*format) {
			case 'c':
				s = va_arg(arg, char*);
				*s = getchar();
				break;
			case 's':
				s = va_arg(arg, char*);
				scans(s);
				break;
			case 'd':
				i = va_arg(arg, int*);
				scans(buf);
				*i = dtoi(buf);
				break;
			case 'o':
				i = va_arg(arg, int*);
				scans(buf);
				*i = otoi(buf);
				break;
			case 'x':
				i = va_arg(arg, int*);
				scans(buf);
				*i = htoi(buf);
				break;
			}
		}
		else {

		}
	}

	va_end(arg);
	putchar('\n');
	putchar('\r');
}

void devopen(char c) {
	switch (c)
	{
		case NO_INPUT_DEVICE:
			getc = NULL;
			break;
		case RS232_INPUT:
			SCON = 0x50;
			TH1 = TL1 = 253;
			TCON = 0x40;
			PCON = 0x80;
			IE = 0x88;
			TH0 = TL0 = 0;
			TMOD &= 0xFF;
			TMOD |= 0x21;
			getc = &rs232_getchar;
			break;
		case KB_INPUT:
			getc = &getkey;
			break;
		case NO_OUTPUT_DEVICE:
			putc = NULL;
			break;
		case RS232_OUTPUT:
			SCON = 0x50;
			TH1 = TL1 = 253;
			TCON = 0x40;
			PCON = 0x80;
			IE = 0x88;
			TH0 = TL0 = 0;
			TMOD &= 0xFF;
			TMOD |= 0x21;
			putc = &rs232_putchar;
			break;
		case LED_OUTPUT:
			SCON = 0x50;
			TH1 = TL1 = 253;
			TCON = 0x40;
			PCON = 0x80;
			IE = 0x88;
			TH0 = TL0 = 0;
			TMOD &= 0xFF;
			TMOD |= 0x21;
			putc = led_putchar;
			break;
		case LCD_OUTPUT:
			send_command_8bit_mode(0x30);
			wait(100);
			send_command_8bit_mode(0x30);
			wait(100);
			send_command_8bit_mode(0x30);
			wait(100);
			send_command_8bit_mode(0x20);
			wait(100);
			send_command_4bit_mode(0x2F);
			wait(100);
			send_command_4bit_mode(0x08);
			wait(100);
			send_command_4bit_mode(0x0C);
			wait(100);
			send_command_4bit_mode(0x06);
			wait(100);

			lcd_clear_chars();

			putc = &lcd_putchar;
			break;
		default:
			putc = &rs232_putchar;
			printf("No input device found");
			putc = NULL;
			break;
	}
}

//Interrupt
void func_isr(void) __interrupt 3 {
	U15 = column_address[t];

	if ((U12 & 0xf0) == key_0) {
		if (key_pressed == 0) {
			key = 6 + t;
			key_pressed = 1;
		}
	}
	else if ((U12 & 0xf0) == key_1) {
		if (key_pressed == 0) {
			key = 10 + t;
			key_pressed = 1;
		}
	}
	else if ((U12 & 0xf0) == key_2) {
		if (key_pressed == 0) {
			key = 14 + t;
			key_pressed = 1;
		}
	}
	else if ((U12 & 0xf0) == key_3) {
		if (key_pressed == 0) {
			key = 18 + t;
			key_pressed = 1;
		}
	}

	U15 = segment_address[t];
	U10 = cyfra_null;
	U10 = word[t];

	t++;
	t = t % 4;
}

//BUZZER
void buzzer_mksound(int frequency){
    if((r & 0x01) == 0) {              
        T1 = 0;
    } else { 
        T1=1; 
    }
    wait(frequency);
    ++r;
}

//RS232
char rs232_getchar() {
    char c;
    while(RI == 0);

    RI = 0;
    c = SBUF;

    return c;
}

void rs232_putchar(char c) {
    SBUF = c;
    while(TI==0);
    TI=0;
}

//LED
void led_putchar(char c) {
	ledpos = ledpos % 4;
	led_putcharpos(c, ledpos++);
}

void led_putcharpos(char c, int p) {
	word[p] = c;
}

//keyboard
unsigned char kbhit() {
	return key_pressed;
}

unsigned char getkey() {
	char k;

	while (!kbhit());
	k = key;

	key = -1;
	key_pressed = 0;

	return k;
}
//LCD
void create_custom_character(unsigned char *pattern, const char location) { 
    int i;
        
    send_command_4bit_mode(0x40+(location*8));
    wait(100);
    for (i=0; i<8; ++i){
            lcd_putchar(pattern[i]);
            wait(100);
    }
}

void lcd_putchar(char c){
    P1=(c & 0xf0) | RS;
    P1=((c & 0xf0) | E) | RS;
    P1=((c & 0xf0) & !E) | RS;
    P1=((c << 4) & 0xf0) | RS;
    P1=(((c << 4) & 0xf0) | E) | RS;
    P1=(((c << 4) & 0xf0) & !E) | RS;
    wait(100);
}

void lcd_clear_screen(){
    send_command_4bit_mode(0x01);
    wait(1000);
}

void lcd_clear_chars() {
	char clear[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	int i;

	for (i = 0; i < 8; ++i)
		create_custom_character(clear, i);
}

void goto_xy(char x, char y) {
	if (y == 0) {
		pos_x = x;
		pos_y = 0;
		send_command_4bit_mode(0x80 | x);
	} else {
		pos_x = x;
		pos_y = 1;
		send_command_4bit_mode(0xC0 | x);
	}
}

void lcd_puts(const char* t) {
	char i;

	if (pos_y == 0) {
		for (i = pos_x; i < MAX_LINE_LCD; ++i) {
			if (*t == 0) return;
			lcd_putchar(*t++);
		}
		goto_xy(0, 1);
		pos_x = 0;
	}

	for (i = pos_x; i < MAX_LINE_LCD; ++i) {
		if (*t == 0) return;
		lcd_putchar(*t++);
	}
}

void send_command_8bit_mode(unsigned char command) {
        P1=command;
        P1=command | E;
        P1=command & !E;
}

void send_command_4bit_mode(unsigned char command) {
        P1=command & 0xf0;
        P1=(command & 0xf0) | E;
        P1=(command & 0xf0) & !E;
        P1=(command << 4) & 0xf0;
        P1=((command << 4) & 0xf0) | E;
        P1=((command << 4) & 0xf0) & !E;
}
