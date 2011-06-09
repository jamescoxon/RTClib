// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#ifndef __RTC_DS3234_H__
#define __RTC_DS3234_H__

#include <RTClib.h>

// RTC based on the DS3234 chip connected via SPI and the SPI library
class RTC_DS3234
{
public:
    RTC_DS3234(int _cs_pin): cs_pin(_cs_pin) {}
    uint8_t begin(uint8_t al1, uint8_t al2);
    void adjust(const DateTime& dt);
    uint8_t isrunning(void);
    DateTime now();
	DateTime get_alarm(int alarm);
	void set_alarm(int alarm, const DateTime& dt, char flags);
	void reset_alarm();

protected:
    void cs(int _value)
    {
        digitalWrite(cs_pin,_value);
    }

private:
    int cs_pin;
};

#endif // __RTC_DS3234_H__

// vim:ci:sw=4 sts=4 ft=cpp
