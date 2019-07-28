#include "strc51io.h"

void main(){
	const char* c = "chuj";
	int i = 1234;
	devopen(RS232_OUTPUT);
	printf("%s %d", &c, &i);
	devopen(LCD_OUTPUT);
	printf("%s %d", &c, &i);
	devopen(LED_OUTPUT);
	putchar(cyfra_3);
	putchar(cyfra_4);
	putchar(cyfra_5);
    for(;;){
		buzzer_mksound(10);
    }
}