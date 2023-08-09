/*
 * RTC.c
 *
 * Created: 5/21/2022 2:22:35 PM
 * Author : mahim
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "I2C_Master_H_file.h"
#include "LCD16x2_4Bit.h"

#define Device_Write_address	0xD0	/* Define RTC DS1307 slave write address */
#define Device_Read_address	0xD1	/* Make LSB bit high of slave address for read */
#define TimeFormat12		0x40	/* Define 12 hour format */
#define AMPM			0x20
#define LCD_Dir  DDRA			/* Define LCD data port direction */
#define LCD_Port PORTA			/* Define LCD data port */
#define RS PA0				/* Define Register Select pin */
#define EN PA1

int second,minute,hour,day,date,month,year;
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

bool IsItPM(char hour_)
{
	if(hour_ & (AMPM))
	return 1;
	else
	return 0;
}

void RTC_Read_Clock(char read_clock_address)
{
	I2C_Start(Device_Write_address);/* Start I2C communication with RTC */
	I2C_Write(read_clock_address);	/* Write address to read */
	I2C_Repeated_Start(Device_Read_address);/* Repeated start with device read address */

	second = I2C_Read_Ack();	/* Read second */
	minute = I2C_Read_Ack();	/* Read minute */
	hour = I2C_Read_Nack();		/* Read hour with Nack */
	I2C_Stop();			/* Stop i2C communication */
}

void RTC_Read_Calendar(char read_calendar_address)
{
	I2C_Start(Device_Write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);

	day = I2C_Read_Ack();		/* Read day */
	date = I2C_Read_Ack();		/* Read date */
	month = I2C_Read_Ack();		/* Read month */
	year = I2C_Read_Nack();		/* Read the year with Nack */
	I2C_Stop();			/* Stop i2C communication */
}

int main(void)
{
	char buffer[20];
	char* days[7]= {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	I2C_Init();			/* Initialize I2C */
	LCD_Init();			/* Initialize LCD16x2 */
DDRB|=(1<<3);
DDRA|=(1<<2);
DDRD|=(1<<0);
DDRB &= ~(1<<PB0);
	while(1)
	{
		RTC_Read_Clock(0);	/* Read clock with second add. i.e location is 0 */
		
			sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
			
			sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
			LCD_String_xy(0,0,buffer);
			char outputsec[50];
			char outputmin[50];
			char outputhour[50];

			snprintf(outputsec, 50, "%02x", second);
			snprintf(outputmin, 50, "%02x", minute);
			snprintf(outputhour, 50, "%02x", (hour & 0b00011111));
			int H=atoi(outputhour);
			int M=atoi(outputmin);
			int S=atoi(outputsec);
			if (M==30||M==01&& S>0&&S<5)
			{
				LCD_String_xy(9,0,"Alarm");
				PORTB |= (1<<PB3);
				if (PINB & (1<<PB0) >1)
				{
					LCD_Command(0xC0);
					LCD_Clear();
					LCD_String_xy(0,0,"Adjust");
					PORTD |= (1<<PD0);
				}else{
					
					PORTD |= (0<<PD0);
					
				}
			}
		
		
		
	}
}