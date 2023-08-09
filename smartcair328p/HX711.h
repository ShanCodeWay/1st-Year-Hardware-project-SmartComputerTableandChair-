

#include <avr/io.h>


#ifndef HX711_H_
#define HX711_H_


//set ports and pins
#define HX711_DTPORT PORTA
#define HX711_DTDDR DDRA
#define HX711_DTPIN PINA
#define HX711_DTPINNUM PA3
#define HX711_SCKPORT PORTA
#define HX711_SCKDDR DDRA
#define HX711_SCKPINNUM PA4

//defines gain
#define HX711_GAINCHANNELA128 1
#define HX711_GAINCHANNELA64 3
#define HX711_GAINCHANNELB32 2
#define HX711_GAINDEFAULT HX711_GAINCHANNELA128

//defines scale
#define HX711_SCALEDEFAULT 10000

//defines offset
#define HX711_OFFSETDEFAULT 8000000

//set how many time to read
#define HX711_READTIMES 10

//set if use average for read
#define HX711_USEAVERAGEONREAD 0

//calibration average times read
#define HX711_CALIBRATIONREADTIMES 5

//enable the atomic mode on shift in
#define HX711_ATOMICMODEENABLED 1

//functions
extern int32_t hx711_read();
extern int32_t hx711_readaverage(uint8_t times);
extern double hx711_readwithtare();
extern double hx711_getweight();
extern void hx711_setgain(uint16_t gain);
extern uint16_t hx711_getgain();
extern void hx711_setscale(double scale);
extern double hx711_getscale();
extern void hx711_setoffset(int32_t offset);
extern int32_t hx711_getoffset();
extern void hx711_taretozero();
extern void hx711_powerdown();
extern void hx711_powerup();
extern void hx711_calibrate1setoffset();
extern void hx711_calibrate2setscale(double weight);
extern void hx711_init(uint8_t gain, double scale, int32_t offset);

#endif
