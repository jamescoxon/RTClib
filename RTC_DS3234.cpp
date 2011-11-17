// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!
// Additions by James Coxon jacoxon@googlemail.com and Phil Heron phil@sanslogic.co.uk
// Added code to initiate, set, get and reset the alarms on the DS3234

#include <avr/pgmspace.h>
#include <WProgram.h>
#include <SPI.h>
#include "RTClib.h"
#include "RTC_DS3234.h"

////////////////////////////////////////////////////////////////////////////////
// RTC_DS3234 implementation

// Registers we use
const int CONTROL_R = 0x0e;
const int CONTROL_W = 0x8e;
const int CONTROL_STATUS_R = 0x0f;
const int CONTROL_STATUS_W = 0x8f;
const int SECONDS_R = 0x00;
const int SECONDS_W = 0x80;

const int ALARM1_R = 0x07;
const int ALARM1_W = 0x87;
const int ALARM2_R = 0x0B;
const int ALARM2_W = 0x8B;

// Bits we use
const int EOSC = 7;
const int OSF = 7;

uint8_t RTC_DS3234::begin()
{
    pinMode(cs_pin,OUTPUT);
    cs(HIGH);
    SPI.setBitOrder(MSBFIRST);

    //Ugh!  In order to get this to interop with other SPI devices,
    //This has to be done in cs()
    SPI.setDataMode(SPI_MODE1);
    delay(1);

    //Clear oscilator stop flag, 32kHz pin
    cs(LOW);
    SPI.transfer(CONTROL_STATUS_W);
    SPI.transfer(0x0);
    cs(HIGH);
    delay(1);

    return 1;
}

void RTC_DS3234::setup(uint8_t al1, uint8_t al2)
{
	//Enable oscillator, disable square wave, enable alarms
    cs(LOW);
    SPI.transfer(CONTROL_W);
	if(al1 == 1 && al2 ==1)
	{ SPI.transfer(0x1F); }
	else if(al1 == 1 && al2 ==0)
	{ SPI.transfer(0x1D); }
	else if(al1 == 0 && al2 ==1)
	{ SPI.transfer(0x1E); }
	else
	{ SPI.transfer(0x1D); }
	cs(HIGH);
}

void RTC_DS3234::cs(int _value)
{
    SPI.setDataMode(SPI_MODE1);
    digitalWrite(cs_pin,_value);
}

uint8_t RTC_DS3234::isrunning(void)
{
    cs(LOW);
    SPI.transfer(CONTROL_R);
    uint8_t ss = SPI.transfer(-1);
    cs(HIGH);
    return !(ss & _BV(OSF));
}

void RTC_DS3234::adjust(const DateTime& dt)
{
    cs(LOW);
    SPI.transfer(SECONDS_W);
    SPI.transfer(bin2bcd(dt.second()));
    SPI.transfer(bin2bcd(dt.minute()));
    SPI.transfer(bin2bcd(dt.hour()));
    SPI.transfer(bin2bcd(dt.dayOfWeek()));
    SPI.transfer(bin2bcd(dt.day()));
    SPI.transfer(bin2bcd(dt.month()));
    SPI.transfer(bin2bcd(dt.year() - 2000));
    cs(HIGH);

}

uint8_t RTC_DS3234::get_control()
{
	uint8_t data;
	cs(LOW);
	SPI.transfer(CONTROL_R);
	data = SPI.transfer(-1);
	cs(HIGH);
	return data;
}

uint8_t RTC_DS3234::get_status()
{
	uint8_t data;
	cs(LOW);
	SPI.transfer(CONTROL_STATUS_R);
	data = SPI.transfer(-1);
	cs(HIGH);
	return data;
}

DateTime RTC_DS3234::get_alarm(int alarm)
{
	cs(LOW);
	SPI.transfer((alarm == 1 ? ALARM1_R : ALARM2_R));
	uint8_t ss = bcd2bin(SPI.transfer(-1) & 0x7F);
	uint8_t mm = bcd2bin(SPI.transfer(-1) & 0x7F);
	uint8_t hh = bcd2bin(SPI.transfer(-1) & 0x7F);
	uint8_t d = bcd2bin(SPI.transfer(-1) & 0x7F);
	cs(HIGH);
	
	return DateTime (0, 0, d, hh, mm, ss);
}

void RTC_DS3234::set_alarm(int alarm, const DateTime& dt, char flags)
{
    cs(LOW);
    SPI.transfer((alarm == 1 ? ALARM1_W : ALARM2_W));
    SPI.transfer(((flags & 1) << 7) | bin2bcd(dt.second())); flags >>= 1;
    SPI.transfer(((flags & 1) << 7) | bin2bcd(dt.minute())); flags >>= 1;
    SPI.transfer(((flags & 1) << 7) | bin2bcd(dt.hour()));   flags >>= 1;
    SPI.transfer(((flags & 1) << 7) | bin2bcd(dt.day()));    flags >>= 1;
    cs(HIGH);
}

void RTC_DS3234::reset_alarm()
{
	//Clear oscilator stop flag, 32kHz pin
    cs(LOW);
    SPI.transfer(CONTROL_STATUS_W);
    SPI.transfer(0x0);
    cs(HIGH);	
}

DateTime RTC_DS3234::now()
{
    cs(LOW);
    SPI.transfer(SECONDS_R);
    uint8_t ss = bcd2bin(SPI.transfer(-1) & 0x7F);
    uint8_t mm = bcd2bin(SPI.transfer(-1));
    uint8_t hh = bcd2bin(SPI.transfer(-1));
    SPI.transfer(-1);
    uint8_t d = bcd2bin(SPI.transfer(-1));
    uint8_t m = bcd2bin(SPI.transfer(-1));
    uint16_t y = bcd2bin(SPI.transfer(-1)) + 2000;
    cs(HIGH);

    return DateTime (y, m, d, hh, mm, ss);
}

// vim:ai:cin:sw=4 sts=4 ft=cpp
