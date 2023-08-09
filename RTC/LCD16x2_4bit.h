/*
 * LCD16x2_4bit.h
 *
 * http://www.electronicwings.com
 */ 


#ifndef LCD16X2_4BIT_H_
#define LCD16X2_4BIT_H_

#define  F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define LCD_DPRT PORTB
#define LCD_DDDR DDRB
#define LCD_RS 0
#define LCD_EN 1

void lcdcommand(unsigned char cmnd);
void lcddata(unsigned char data);
void lcdinit();
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_print(char *str);
void lcd_clear();

#endif /* LCD16X2_4BIT_H_ */