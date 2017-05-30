#define REG_SECOND 	0x00
#define REG_MINUTE 	0x01
#define REG_HOUR 	0x02
#define REG_DAY 	0x03
#define REG_DATE 	0x04
#define REG_MONTH	0x05
#define REG_YEAR 	0x06
#define REG_CONTROL 0x07
#define SLAVE_ADDRESS_RTC 0x68
void Time_set(void);
extern void Time_read(unsigned char* rtc);
extern void Time_display(unsigned char* current_time);
extern unsigned char Alarm_check(unsigned char* time, unsigned char* current_time);
extern void LCD_Time_display(unsigned char* current_time);
