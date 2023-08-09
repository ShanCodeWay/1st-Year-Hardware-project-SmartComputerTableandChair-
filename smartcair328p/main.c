/*
 * HX711222.c
 *
 * Created: 5/19/2022 9:09:53 PM
 * Author : mahim
 */ 

//hx711 lib example

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define LCD_Dir  DDRB			/* Define LCD data port direction */
#define LCD_Port PORTB			/* Define LCD data port */
#define RS PB0				/* Define Register Select pin */
#define EN PB1 				/* Define Enable signal pin */
#define Device_Write_address	0xD0				/* Define RTC DS1307 slave address for write operation */
#define Device_Read_address		0xD1				/* Make LSB bit high of slave address for read operation */
#define TimeFormat12			0x40				/* Define 12 hour format */
#define AMPM					0x20
#include "hx711.h"
char time[40];
//defines running modes
#define HX711_MODERUNNING 1
#define HX711_MODECALIBRATION1OF2 2
#define HX711_MODECALIBRATION2OF2 3
#define HX711_MODECURRENT 1


int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}

void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);

	// ADC Enable and prescaler of 128
	
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

// read adc value
uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch;

	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);

	// wait for conversion to complete
	// ADSC becomes '0' again
	
	while(ADCSRA & (1<<ADSC));

	return (ADC);
}






void LCD_Command( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0); /* sending upper nibble */
	LCD_Port &= ~ (1<<RS);		/* RS=0, command reg. */
	LCD_Port |= (1<<EN);		/* Enable pulse */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}


void LCD_Char( unsigned char data )
{
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0); /* sending upper nibble */
	LCD_Port |= (1<<RS);		/* RS=1, data reg. */
	LCD_Port|= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (data << 4); /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Dir = 0xFF;			/* Make LCD port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	
	LCD_Command(0x02);		/* send for 4 bit initialization of LCD  */
	LCD_Command(0x28);              /* 2 line, 5*7 matrix in 4-bit mode */
	LCD_Command(0x0c);              /* Display on cursor off*/
	LCD_Command(0x06);              /* Increment cursor (shift cursor to right)*/
	LCD_Command(0x01);              /* Clear display screen*/
	_delay_ms(2);
}


void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)	/* Send string to LCD with xy position */
{
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
	LCD_String(str);		/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* Clear display */
	_delay_ms(2);
	LCD_Command (0x80);		/* Cursor at home position */
}

//2 step calibration procedure
//set the mode to calibration step 1
//read the calibration offset leaving the load cell with no weight
//set the offset to value read
//put a know weight on the load cell and set calibrationweight value
//run the calibration stes 2 of 2
//read the calibration scale
//set the scale to value read

//set the gain
int8_t gain = HX711_GAINCHANNELA128;

#if HX711_MODECURRENT == HX711_MODERUNNING
//set the offset
int32_t offset = 8389246;
//set the scale
double scale = 15797.8;
#elif HX711_MODECURRENT == HX711_MODECALIBRATION1OF2
//set the offset
int32_t offset = HX711_OFFSETDEFAULT;
//set the scale
double scale = HX711_SCALEDEFAULT;
#elif HX711_MODECURRENT == HX711_MODECALIBRATION2OF2
//set the offset
int32_t offset = 8389246;
//set the scale
double scale = HX711_SCALEDEFAULT;
//set the calibration weight
double calibrationweight = 0.082;
#endif


int main(void) {
	//init uart
	DDRD=DDRD | (1<<PD2);
	DDRD=DDRD | (1<<PD5);
	DDRD=DDRD | (1<<PD7);
	DDRC=DDRC | (1<<PC5);
	DDRC=DDRC | (1<<PC6);
	DDRD=DDRD & ~(1<<PD3);
	DDRD=DDRD & ~(1<<PD4);
	DDRD=DDRD & ~(1<<PD0);
	DDRD=DDRD & ~(1<<PD1);
	DDRC=DDRC & ~(1<<PC1);
	DDRC=DDRC & ~(1<<PC0);
	char buffer[10];
	LCD_Init();

uint16_t adc_result0,ldr;
int temp,ldr1;
int far;

char string[10];
long count;


// initialize adc and lcd

DDRA = DDRA | (1<<2);
PORTD = (1<<6);		/* Turn on Pull-up */



sei();			/* Enable global interrupt */
TIMSK = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
TCCR1A = 0;		/* Set all bit to zero Normal operation */
adc_init();
char printbuff[100];
//init hx711
hx711_init(gain, scale, offset);
while(1){
		LCD_Clear();
		LCD_String_xy(0,0,"Do You Want to");
		LCD_String_xy(1,0,"  Start ");
		double weight = hx711_getweight();
		int s=PINC & (1<<PC1);
		itoa(s,buffer,10);
		LCD_String_xy(1,12,buffer);
		_delay_ms(500);
while(s>0 && weight>30){
LCD_Clear();
LCD_String_xy(0,0,"Wei:");
LCD_String_xy(1,0,"Tem: ");
adc_result0 = adc_read(0);      // read adc value at PA0
temp=adc_result0/2.01;   // finding the temperature
itoa(temp,buffer,10);
strcat(buffer, "C");	
LCD_String_xy(1,4,buffer);
//_delay_ms(100);
int b=PIND & (1<<PD0);
itoa(b,buffer,10);
LCD_String_xy(0,14,buffer);
int c=PIND & (1<<PD1);
itoa(c,buffer,10);
LCD_String_xy(0,15,buffer);










		//get weight
		double weight = hx711_getweight();
		dtostrf(weight, 3, 2, string);
		strcat(string, " kg");	
		LCD_String_xy(0,4,string);
		
int s=PINC & (1<<PC1);
int chair=PINC & (1<<PC0);
//itoa(s,buffer,10);
//LCD_String_xy(1,12,buffer);
		

		//_delay_ms(500);
		if(temp>30 && weight>30)
		{
			
			PORTD=(1<<PIND2);
			
			
		}else{
			PORTD=(0<<PIND2);
		}
		
		
		while((!(PIND & (1<<PD3))) && weight>30)
		{
			PORTD = (1<<PD5);
		}
		
		
		while((!(PIND & (1<<PD4))) && weight>30){
			PORTD = (1<<PD7);
		}
		
		if(weight>30){
			
			PORTA=(1<<PINA2);
		}else{
			
			PORTA=(0<<PINA2);
		}
		
		
		if(b<1 && weight>30)
		{
			PORTC = (1<<PC5);
			_delay_ms(1000);
			
		}else{
			PORTC = (0<<PC5);
		}
		
		
		if(c<1&&weight>30){
			PORTC = (1<<PC6);
			_delay_ms(1000);
		}else{
		PORTC = (0<<PC6);
	}
	
	while (chair>0)
	{
	PORTD = (1<<PD5);
	_delay_ms(2000);	
	}
		
		
		
		_delay_ms(500);
		
		
	}
	
	
	while(weight<30){
		_delay_ms(30000);
		LCD_Clear();
		LCD_String_xy(0,0,"Machine OFF");
	}
	}
		}
	

