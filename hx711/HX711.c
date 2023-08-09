/*
hx711 lib 0x02

copyright (c) Davide Gironi, 2018

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include "hx711.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/atomic.h>


//actual gain
static uint8_t hx711_gain = 0;
//actual scale
static double hx711_scale = 0;
//actual offset
static int32_t hx711_offset = 0;

/**
 * read raw value
 */
int32_t hx711_read() {
	uint32_t count = 0;
	uint8_t i = 0;

	//wait for the chip to became ready
	while (HX711_DTPIN & (1<<HX711_DTPINNUM));

#if HX711_ATOMICMODEENABLED == 1
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
#endif
	//read data with a 24 shift
	for(i=0;i<24;i++) {
		HX711_SCKPORT |= (1<<HX711_SCKPINNUM);
		asm volatile("nop");
		count=count<<1;
		HX711_SCKPORT &= ~(1<<HX711_SCKPINNUM);
		asm volatile("nop");
		if(HX711_DTPIN & (1<<HX711_DTPINNUM))
			count++;
	}
	count ^= 0x800000;

	//set the channel and the gain
	for (i=0; i<hx711_gain; i++) {
		HX711_SCKPORT |= (1<<HX711_SCKPINNUM);
		asm volatile("nop");
		HX711_SCKPORT &= ~(1<<HX711_SCKPINNUM);
	}
#if HX711_ATOMICMODEENABLED == 1
	}
#endif

	return count;
}

/**
 * read raw value using average
 */
int32_t hx711_readaverage(uint8_t times) {
	int32_t sum = 0;
	uint8_t i = 0;
	for (i=0; i<times; i++) {
		sum += hx711_read();
	}
	return (int32_t)(sum/times);
}

/**
 * perform a read excluding tare
 */
double hx711_readwithtare() {
#if HX711_USEAVERAGEONREAD == 1
	return (double)hx711_readaverage(HX711_READTIMES)-(double)hx711_offset;
#else
	return (double)hx711_read()-(double)hx711_offset;
#endif
}

/**
 * get the weight
 */
double hx711_getweight() {
	return hx711_readwithtare()/hx711_scale;
}

/**
 * set the gain
 */
void hx711_setgain(uint16_t gain) {
	if (gain == HX711_GAINCHANNELA128)
		hx711_gain = 1;
	else if (gain == HX711_GAINCHANNELA64)
		hx711_gain = 3;
	else if (gain == HX711_GAINCHANNELB32)
		hx711_gain = 2;
	else
		hx711_gain = 1;

	HX711_SCKPORT &= ~(1<<HX711_SCKPINNUM);
	hx711_read();
}

/**
 * get the actual gain
 */
uint16_t hx711_getgain() {
	return hx711_gain;
}

/**
 * set the scale to use
 */
void hx711_setscale(double scale) {
	hx711_scale = scale;
}

/**
 * get the actual scale
 */
double hx711_getscale() {
	return hx711_scale;
}

/**
 * set the offset raw value
 */
void hx711_setoffset(int32_t offset) {
	hx711_offset = offset;
}

/**
 * get the actual offset
 */
int32_t hx711_getoffset() {
	return hx711_offset;
}

/**
 * set tare to zero
 */
void hx711_taretozero() {
#if HX711_USEAVERAGEONREAD == 1
	double sum = hx711_readaverage(HX711_READTIMES);
#else
	double sum = hx711_read();
#endif
	hx711_setoffset(sum);
}

/**
 * power down the chip
 */
void hx711_powerdown() {
	HX711_SCKPORT &= ~(1<<HX711_SCKPINNUM);
	HX711_SCKPORT |= (1<<HX711_SCKPINNUM);
}

/**
 * power up the chip
 */
void hx711_powerup() {
	HX711_SCKPORT &= ~(1<<HX711_SCKPINNUM);
}

/**
 * calibration step 1 of 2, set the offset for tare zero
 */
void hx711_calibrate1setoffset() {
	hx711_setoffset(hx711_readaverage(HX711_CALIBRATIONREADTIMES));
}

/**
 * calibration step 2 of 2, set the scale
 */
void hx711_calibrate2setscale(double weight) {
	hx711_setscale((hx711_readaverage(HX711_CALIBRATIONREADTIMES)-hx711_offset)/weight);
}

/**
 * initialize chip
 */
void hx711_init(uint8_t gain, double scale, int32_t offset) {
	//set sck as output
	HX711_SCKDDR |= (1<<HX711_SCKPINNUM);
	HX711_SCKPORT &= ~(1<<HX711_SCKPINNUM);
	//set dt as input
	HX711_DTDDR &=~ (1<<HX711_DTPINNUM);

	//set gain
	hx711_setgain(gain);
	//set scale
	hx711_setscale(scale);
	//set offset
	hx711_setoffset(offset);
}
