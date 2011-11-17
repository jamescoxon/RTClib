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
    uint8_t begin();
    void adjust(const DateTime& dt);
    uint8_t isrunning(void);
    DateTime now();
	DateTime get_alarm(int alarm);
	void set_alarm(int alarm, const DateTime& dt, char flags);
	void reset_alarm();
	uint8_t get_control();
	uint8_t get_status();
	void setup(uint8_t al1, uint8_t al2);

protected:
    void cs(int _value);

private:
    int cs_pin;
};

#endif // __RTC_DS3234_H__

// vim:ai:cin:sw=4 sts=4 ft=cpp
