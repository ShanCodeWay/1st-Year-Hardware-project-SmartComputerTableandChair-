/*
 * ULTRASONIC.c
 *
 * Created: 29/05/2022 11:30:23 AM
 * Author : 205116E Wijebahu
 */ 
#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif


#include <avr/io.h>             // This header file includes the appropriate Input/output definitions for the device
#include <util/delay.h>         // to use delay function we need to include this library
#include <stdlib.h>             // we'll be using itoa() function to convert integer to character array that resides in this library

#define US_PORT PORTC           // we have connected the Ultrasonic sensor on port C. to use the ultrasonic we need two pins of the ultrasonic to be connected on port C
#define	US_PIN	PINC            // we need to initialize the pin resistor when we want to take input.
#define US_DDR 	DDRC            // we need data-direction-resistor (DDR) to set the direction of data flow. input or output. 

#define US_TRIG_POS	PC0         // the trigger pin of ultrasonic sound sensor is connected to port C pin 0
#define US_ECHO_POS	PC1         // the echo pin of the ultrasonic sound sensor is connected to port C pin 1


#define US_ERROR		-1      // we're defining two more variables two know if the ultrasonic sensor is working or not
#define	US_NO_OBSTACLE	-2

int distance, previous_distance;


void HCSR04Init();
void HCSR04Trigger();


void HCSR04Init()
{
	
	// we're setting the trigger pin as output as it will generate ultrasonic sound wave
	US_DDR|=(1<<US_TRIG_POS);
}

void HCSR04Trigger()
{   // this function will generate ultrasonic sound wave for 15 microseconds
	//Send a 10uS pulse on trigger line
	
	US_PORT|=(1<<US_TRIG_POS);	//high
	
	_delay_us(15);				//wait 15uS
	
	US_PORT&=~(1<<US_TRIG_POS);	//low
}

uint16_t GetPulseWidth()
{
	// this function will be used to measure the pulse duration. When the ultra sound echo back after hitting an object
	// the microcontroller will read the pulse using the echo pin of the ultrasonic sensor connected to it.
	
	uint32_t i,result;

	// Section - 1: the following lines of code before the section - 2 is checking if the ultrasonic is working or not
	// it check the echo pin for a certain amount of time. If there is no signal it means the sensor is not working or not connect properly
	for(i=0;i<600000;i++)
	{
		if(!(US_PIN & (1<<US_ECHO_POS)))
		continue;	//Line is still low, so wait
		else
		break;		//High edge detected, so break.
	}

	if(i==600000)
	return US_ERROR;	//Indicates time out
	
	//High Edge Found
	
	// Section -2 : This section is all about preparing the timer for counting the time of the pulse. Timers in microcontrllers is used for timimg operation
	//Setup Timer1
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);	// This line sets the resolution of the timer. Maximum of how much value it should count.
	TCNT1=0x00;			// This line start the counter to start counting time

	// Section -3 : This section checks weather the there is any object or not
	for(i=0;i<600000;i++)                // the 600000 value is used randomly to denote a very small amount of time, almost 40 miliseconds
	{
		if(US_PIN & (1<<US_ECHO_POS))
		{
			if(TCNT1 > 60000) break; else continue;   // if the TCNT1 value gets higher than 60000 it means there is not object in the range of the sensor
		}
		else
		break;
	}

	if(i==600000)
	return US_NO_OBSTACLE;	//Indicates time out

	//Falling edge found

	result=TCNT1;          // microcontroller stores the the value of the counted pulse time in the TCNT1 register. So, we're returning this value to the
	// main function for utilizing it later

	//Stop Timer
	TCCR1B=0x00;

	if(result > 60000)
	return US_NO_OBSTACLE;	//No obstacle
	else
	return (result>>1);
}




int main()
{
	DDRB= 0xFF; // direction of port B as output
	DDRA=0xff;
	

	while(1) {
		
		uint16_t r;
		
		_delay_ms(100);	


		
		//Set io port direction of sensor
		HCSR04Init();


		while(1)
		{
			
			//Send a trigger pulse
			HCSR04Trigger();               // calling the ultrasonic sound wave generator function

			//Measure the width of pulse
			r=GetPulseWidth();             // getting the duration of the ultrasound took to echo back after hitting the object

			//Handle Errors
			if(r==US_ERROR)                // if microcontroller doesn't get any pulse then it will set the US_ERROR variable to -1
			
		;
			else 
			{
				
				distance=(r*0.034/2.0);	// This will give the distance in centimeters
				
			   
				
				if (distance != previous_distance)    // the LCD screen only need to be cleared if the distance is changed otherwise it is not required
				{
					
					if (distance>70 && distance<100){
						PORTB = 0x0A; // motor rotation in clockwise direction

						
						
					}
					
					
					else if (distance<70 && distance>15)
					{	PORTB = 0x05; //motor rotation in anticlockwise direction

						
					}
					else
					{		PORTB =  0x00; // motor stopped
						
					}
				}
				
				
				
				
				
				previous_distance = distance;
				_delay_ms(30);
				
				
				 
			}
		}
			
	}
}