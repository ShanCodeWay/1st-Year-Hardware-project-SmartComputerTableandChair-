#ifndef RTC_h
#define RTC_h

#define DS1307_I2C_ADDRESS 0x68

#include <inttypes.h>

struct RTCTime
{
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year;
  uint8_t dayOfWeek;
};

class RTCClass
{
  public:
    RTCTime time();
    
    void set(RTCTime tm);
    void set(uint8_t year, uint8_t month, uint8_t day, uint8_t dayOfWeek, uint8_t hour, uint8_t minute, uint8_t second);
};


extern RTCClass RTC;

#endif
