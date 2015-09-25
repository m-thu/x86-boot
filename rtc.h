#ifndef RTC_H
#define RTC_H

void init_rtc();
struct time *get_time();

struct time {
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char day_of_week;
	unsigned char month;
	unsigned char year;
	unsigned char century;
};

#endif
