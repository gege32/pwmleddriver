#define no_of_samples    1
#define F_CPU 8000000UL

#include <avr/io.h> //standard include for ATMega8#include <avr/interrupt.h>
#include "main.h"
#include "uart.h"
#include "lcd.h"
#include "math.h"
#include "rprintf.h"
#include <util/delay.h>
#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator#define tbi(x,y) x ^= _BV(y) //toggle bit - using bitwise XOR operator#define is_high(x,y) (x & _BV(y) == _BV(y)) //check if the y'th bit of register 'x' is high ... test if its AND with 1 is 1static void setup();
unsigned long filter_adc();
unsigned long adc_read();
void LCDWriteStringXY(int x, int y, const char *s);
void resetcounters();
void updateLCD();

int led1Counterup[] = {	0,	17,	34,	50,	50,	50,	50,	50,	50,	50 };
int led2Counterup[] = {	0,	0,	0,	0,	17,	34,	50,	50,	50,	50 };
int led3Counterup[] = {	0,	0,	0,	0,	0,	0,	0,	17,	34,	50 };
int led1Counterdn[] = {	50,	33,	16,	0,	0,	0,	0,	0,	0,	0};
int led2Counterdn[] = {	50,	50,	50,	50,	33,	16,	0,	0,	0,	0};
int led3Counterdn[] = {	50,	50,	50,	50,	50,	50,	50,	33,	16,	0};

int cyclecounter = 0;
int led1Count = 0;
int led3Count = 0;
int led2Count = 0;
int led3Up = 1;
int led1Up = 1;
int led2Up = 1;

int preset = 0;
int lastpreset = 0;
char *disp_preset;
char *disp_avg;
char *message;
unsigned long avg = 0;
unsigned long lastAvg = 0;

int butUpPressed = 0;
int butDnPressed = 0;

int pc_read = 0;
int presettomodify = -1;

//uptime
int seconds=0;
int minutes=0;
int hours=0;
char *disp_seconds;
char *disp_minutes;
char *disp_hours;

//program parameters
int maxpreset;
char disp_maxpreset;
char progcode;

//lastpresetchange
char *disp_lastchangesec;
char *disp_lastchangemin;
char *disp_lastchangehour;

int doLCD = 0;

int main() {
	maxpreset = 9;
	progcode="Z";
	disp_avg = malloc(6 * sizeof(char));
	disp_preset = malloc(2 * sizeof(char));
	disp_maxpreset = malloc(2 * sizeof(char));

	disp_lastchangesec = malloc(2 * sizeof(char));
	disp_lastchangemin = malloc(2 * sizeof(char));
	disp_lastchangehour = malloc(2 * sizeof(char));
	disp_lastchangesec = '0';
	disp_lastchangemin = '0';
	disp_lastchangehour = '0';

	disp_seconds = malloc(3 * sizeof(char));
	disp_minutes = malloc(3 * sizeof(char));
	disp_hours = malloc(3 * sizeof(char));

	message = malloc(6 * sizeof(char));
	avg = 0;
	lastAvg = 0;
	setup();

	do {
		//check buttons
		if ((PINC & (1 << PC0)) && butUpPressed == 0) {
			butUpPressed = 1;
			if (preset < maxpreset) {
				preset++;
				resetcounters();
				setLastPresetChange();

			}
		} else {
			butUpPressed = 0;
		}
		if ((PINC & (1 << PC1)) && butDnPressed == 0) {
			butDnPressed = 1;
			if (preset > 0) {
				preset--;
				resetcounters();
				setLastPresetChange();
			}
		} else {
			butDnPressed = 0;
		}

		if (avg != lastAvg) {
			rprintfChar('L');
			rprintfNum(10, 4, FALSE, '0', avg);
			lastAvg = avg;
		}

		if (preset != lastpreset) {
			rprintfChar('P');
			rprintfNum(10, 4, FALSE, '0', preset);
			lastpreset = preset;
		}

		//read preset from PC
		if (!uartReceiveBufferIsEmpty()) {
			pc_read = uartGetByte();
			if (pc_read != -1){
				if(pc_read >= 0 && pc_read < 10){
					preset = pc_read;
				}else if(pc_read == '#'){
					while(uartReceiveBufferIsEmpty());
					maxpreset = uartGetByte();
					preset = 0;
				}else if(pc_read == '$'){
					while(uartReceiveBufferIsEmpty());
					presettomodify = uartGetByte();
					while(uartReceiveBufferIsEmpty());
					led1Counterup[presettomodify] = uartGetByte();
					while(uartReceiveBufferIsEmpty());
					led2Counterup[presettomodify] = uartGetByte();
					while(uartReceiveBufferIsEmpty());
					led3Counterup[presettomodify] = uartGetByte();

					while(uartReceiveBufferIsEmpty());
					led1Counterdn[presettomodify] = uartGetByte();
					while(uartReceiveBufferIsEmpty());
					led2Counterdn[presettomodify] = uartGetByte();
					while(uartReceiveBufferIsEmpty());
					led3Counterdn[presettomodify] = uartGetByte();
				}else if(pc_read == '&'){
					while(uartReceiveBufferIsEmpty());
					presettomodify = uartGetByte();
				}
			}

		}

		doLCD++;
		if(doLCD == 25){
			updateLCD();
			doLCD = 0;
		}



		_delay_ms(20);
	} while (1);

	return 0;
}

static void setup() {

	DDRD = 0xff; //PORTD as OUTPUT
	PORTD = 0x00; //All pins of PORTD LOW

	DDRC &= ~(1 << PC0);
	DDRC &= ~(1 << PC1);

	lcd_init(LCD_DISP_ON);
	uartInit();
	rprintfInit(uartSendByte);

	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS0);
	// ADEN: Set to turn on ADC , by default it is turned off
	//ADPS2: ADPS2 and ADPS0 set to make division factor 32
	ADMUX = 0x05; // ADC input channel set to PC5

	//LED timer interrupts
	OCR2 = 32;
	TCCR2 |= (1 << WGM21); // Set to CTC Mode
	TIMSK |= (1 << OCIE2); //Set interrupt on compare match
	TCCR2 |= (1 << CS21); // set prescaler to 64 and starts PWM


	//LCD timer interrupt
	OCR1A = 7930;
	TCCR1B |= (1 << WGM12);
	// Mode 4, CTC on OCR1A
	TIMSK |= (1 << OCIE1A);
	//Set interrupt on compare match
	TCCR1B |= (1 << CS12) | (1 << CS10);
	// set prescaler to 1024 and start the timer


	sei();

	_delay_ms(10);

}

unsigned long filter_adc() {
	/*unsigned long adc_value = 0;
	unsigned long tmp_value = 0;
	unsigned long sample = 0;

	while (sample < no_of_samples) {
		adc_value = (adc_read());
		tmp_value = (tmp_value + ((unsigned long) (adc_value * adc_value)));
		sample++;
	}

	adc_value = (tmp_value / no_of_samples);
	tmp_value = (sqrt(adc_value));
	return tmp_value;*/
	return adc_read();

}

unsigned long adc_read() {
	unsigned long adc_value = 0;
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // wait for conversion to complete
	adc_value = ADCW; //Store ADC value
	return adc_value;
}

void LCDWriteStringXY(int x, int y, const char *s) {
	lcd_gotoxy(y - 1, x - 1);
	lcd_puts(s);
}

//led setup interrupt 250us
ISR (TIMER2_COMP_vect)  // timer2 interrupt
{
	TCNT0 += 6;
	//toggle led1
	if (led1Up) {
		if (led1Count != led1Counterup[preset]) {
			if (led1Counterdn[preset] != 0)
				cbi(PORTD, PD2);
			led1Count = 0;
			led1Up = 0;
		} else {
			led1Count++;
		}
	} else if (!led1Up) {
		if (led1Count == led1Counterdn[preset]) {
			if (led1Counterup[preset] != 0)
				sbi(PORTD, PD2);
			led1Count = 0;
			led1Up = 1;
		} else {
			led1Count++;
		}
	}

	//toggle led2
	if (led2Up) {
		if (led2Count == led2Counterup[preset]) {
			if (led2Counterdn[preset] != 0)
				cbi(PORTD, PD3);
			led2Count = 0;
			led2Up = 0;
		} else {
			led2Count++;
		}
	} else if (!led2Up) {
		if (led2Count == led2Counterdn[preset]) {
			if (led2Counterup[preset] != 0)
				sbi(PORTD, PD3);
			led2Count = 0;
			led2Up = 1;
		} else {
			led2Count++;
		}
	}

	//toggle led3
	if (led3Up) {
		if (led3Count == led3Counterup[preset]) {
			if (led3Counterdn[preset] != 0)
				cbi(PORTD, PD4);
			led3Count = 0;
			led3Up = 0;
		} else {
			led3Count++;
		}
	} else if (!led3Up) {
		if (led3Count == led3Counterdn[preset]) {
			if (led3Counterup[preset] != 0)
				sbi(PORTD, PD4);
			led3Count = 0;
			led3Up = 1;
		} else {
			led3Count++;
		}
	}
}

ISR (TIMER1_COMPA_vect) {

	seconds++;
	if(seconds == 60){
		minutes++;
		seconds=0;
	}
	if(minutes == 60){
		hours++;
		minutes=0;
	}

	avg = 0;
	avg = filter_adc();

}

void resetcounters(){
	led1Count = 0;
	led2Count = 0;
	led3Count = 0;
	led1Up = 0;
	led2Up = 0;
	led3Up = 0;
}

void setLastPresetChange(){
	snprintf(disp_lastchangesec, 2, "%d", seconds);
	snprintf(disp_lastchangemin, 2, "%d", minutes);
	snprintf(disp_lastchangehour, 2, "%d", hours);
}

void updateLCD(){
	snprintf(disp_avg, 4, "%lu", avg);

	snprintf(disp_seconds, 3, "%d", seconds);
	snprintf(disp_minutes, 3, "%d", minutes);
	snprintf(disp_hours, 3, "%d", hours);
	lcd_clrscr();
	LCDWriteStringXY(1, 1, disp_hours);
	LCDWriteStringXY(1, 3, ":");
	LCDWriteStringXY(1, 4, disp_minutes);
	LCDWriteStringXY(1, 6, ":");
	LCDWriteStringXY(1, 7, disp_seconds);

	snprintf(disp_preset, 2, "%d", preset);
	LCDWriteStringXY(1, 10, disp_preset);
	LCDWriteStringXY(1, 11, "/");
	snprintf(disp_maxpreset, 2, "%d", maxpreset);
	LCDWriteStringXY(1, 12, disp_maxpreset);

	LCDWriteStringXY(1, 14, progcode);

	LCDWriteStringXY(2, 6, disp_lastchangehour);
	LCDWriteStringXY(2, 8, ":");
	LCDWriteStringXY(2, 9, disp_lastchangemin);
	LCDWriteStringXY(2, 11, ":");
	LCDWriteStringXY(2, 12, disp_lastchangesec);

	LCDWriteStringXY(2, 1, disp_avg);
}
