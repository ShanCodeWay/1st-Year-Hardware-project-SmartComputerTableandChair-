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
#define LCD_Dir  DDRC			/* Define LCD data port direction */
#define LCD_Port PORTC			/* Define LCD data port */
#define RS PC0				/* Define Register Select pin */
#define EN PC1 				/* Define Enable signal pin */
//include uart


#include "hx711/hx711.h"


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

	
LCD_Init();
LCD_Clear();
LCD_String_xy(0,0," Weight:");
	char printbuff[100];
DDRA=DDRA | (1<<0);
	//init hx711
	hx711_init(gain, scale, offset);
	
	
while(1){
		int32_t read = hx711_read();
		//ltoa(read, printbuff, 10);
		//get weight
		double weight = hx711_getweight();
		dtostrf(weight, 3, 3, printbuff);
		strcat(printbuff, " kg   ");
		LCD_String_xy(1,0,printbuff);

		
	
		
	_delay_ms(500);	
		if (weight>10)
		{
		PORTA=(1<<0);
		} 
		else
		{
			PORTA=(0<<0);
		}
		
		}
	
	
}

