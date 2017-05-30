#include <msp430g2553.h>
#include <stdint.h>
#include <stdbool.h>
#include "xDS1307.h"
#include "serial.h"
#include "i2c.h"
#include "xLCD.h"
/*
 * HÃ m set thá»�i gian
 * NOTE: Chá»‰ dÃ¹ng khi thá»�i gian Ä‘Ã£ bá»‹ set sai
 * thay Ä‘á»•i cÃ¡c giÃ¡ trá»‹ thanh ghi tÆ°Æ¡ng á»©ng Ä‘á»ƒ cÃ³ giÃ¡ trá»‹ thá»�i gian mong muá»‘n
 */
void Time_set(void)
{
	unsigned char Time_data[8]={0x00,0x05,0x22,0x01,0x07,0x09,0x14,0x03};
	I2C_Write(SLAVE_ADDRESS_RTC,REG_SECOND,Time_data,8 );

}
/*
 * Ä�á»�c thá»�i gian
 * Thá»�i gian Ä‘Æ°á»£c Ä‘á»�c vÃ o máº£ng rtc, theo thá»© tá»± chÃº thÃ­ch á»Ÿ dÆ°á»›i
 */
void Time_read(unsigned char* rtc)
{
	i2c_min_speed();
	I2C_Read(SLAVE_ADDRESS_RTC, REG_SECOND, rtc, 7);
	i2c_max_speed();
	*rtc = ((rtc[0]&0x70)>>4)*10+(rtc[0]&0x0f); // GiÃ¢y
	*(rtc+1) = ((rtc[1]&0x70)>>4)*10+(rtc[1]&0x0f); // PhÃºt
	*(rtc+2) = ((rtc[2]&0x30)>>4)*10+(rtc[2]&0x0f); // Giá»�
	*(rtc+3) = ( rtc[3]&0x07); 					  // Thá»©
	*(rtc+4) = ((rtc[4]&0x30)>>4)*10+(rtc[4]&0x0f); // NgÃ y
	*(rtc+5) = ((rtc[5]&0x10)>>4)*10+(rtc[5]&0x0f); // ThÃ¡ng
	*(rtc+6) = ((rtc[6]&0xf0)>>4)*10+(rtc[6]&0x0f); // NÄƒm
}
/*
 * Hiá»ƒn thá»‹ giá»� trÃªn Terminal
 */
void Time_display(unsigned char* current_time)
{
	//SysCtlDelay(SysCtlClockGet()/3);
	UartStrPut("Thu ");
	Uart_int_display(*(current_time+3)); // thá»©
	UartCharPut(',');
	Uart_int_display(*(current_time+4)); // ngÃ y
	UartCharPut('/');
	Uart_int_display(*(current_time+5)); // thÃ¡ng
	UartCharPut('/');
	Uart_int_display(*(current_time+6)); // nÄƒm
	UartCharPut(' ');
	Uart_int_display(*(current_time+2)); // giá»�
	UartCharPut(':');
	Uart_int_display(*(current_time+1)); // phÃºt
	UartCharPut(':');
	Uart_int_display(*current_time); // giÃ¢y
	UartCharPut('.');
	UartStrPut("            ");
}
/*
 * Check thá»�i gian
 * giÃ¡ trá»‹ tráº£ vá»� báº±ng 1 náº¿u thá»�i gian Ä‘Ãºng
 * tráº£ vá»� 0 náº¿u thá»�i gian sai
 */
unsigned char Alarm_check(unsigned char* time, unsigned char* current_time)
{
	if((*time==*(current_time+1)) & (*(time+1)== *current_time)) return 1;
	else return 0;
}
/*
 * Hiá»ƒn thá»‹ thá»�i gian lÃªn LCD
 */
void LCD_Time_display(unsigned char* current_time)
{
	unsigned char Day[12];
	unsigned char Time[9];
	unsigned char temp;

	temp = (*(current_time+2) - *(current_time+2) % 10)/10; // Há»‡ sá»‘ cao cá»§a giá»�
	Time[0]=temp+48;

	temp = *(current_time+2) % 10; // Há»‡ sá»‘ tháº¥p cá»§a giá»�
	Time[1]=temp+48;

	Time[2]=':';

	temp = (*(current_time+1) - *(current_time+1) % 10)/10; // Há»‡ sá»‘ cao cá»§a phÃºt
	Time[3]=temp+48;

	temp = *(current_time+1) % 10; // Há»‡ sá»‘ tháº¥p cá»§a  phÃºt
	Time[4]=temp+48;

	Time[5]=':';

	temp = (*current_time - *current_time % 10)/10; // Há»‡ sá»‘ cao cá»§a giÃ¢y
	Time[6]=temp+48;

	temp = *current_time % 10; // Há»‡ sá»‘ tháº¥p cá»§a giÃ¢y
	Time[7]=temp+48;
	Time[8]='\0';

	switch (*(current_time+3)) // Chá»‰nh giÃ¡ trá»‹ hiá»ƒn thá»‹ cá»§a thá»© trong tuáº§n
	{
	case 1: {Day[0]='S'; Day[1]='u';Day[2]='n';break;};
	case 2: {Day[0]='M'; Day[1]='o';Day[2]='n';break;};
	case 3: {Day[0]='T'; Day[1]='u';Day[2]='e';break;};
	case 4: {Day[0]='W'; Day[1]='e';Day[2]='d';break;};
	case 5: {Day[0]='T'; Day[1]='h';Day[2]='u';break;};
	case 6: {Day[0]='F'; Day[1]='r';Day[2]='i';break;};
	case 7: {Day[0]='S'; Day[1]='a';Day[2]='t';break;};
	}
	Day[3]=',';
	Day[4]=' ';
	switch (*(current_time+5))// Chá»‰nh giÃ¡ trá»‹ hiá»ƒn thá»‹ cá»§a thÃ¡ng trong tuáº§n
	{
	case 1: {Day[5]='J'; Day[6]='a';Day[7]='n';break;};
	case 2: {Day[5]='F'; Day[6]='e';Day[7]='b';break;};
	case 3: {Day[5]='M'; Day[6]='a';Day[7]='r';break;};
	case 4: {Day[5]='A'; Day[6]='p';Day[7]='r';break;};
	case 5: {Day[5]='M'; Day[6]='a';Day[7]='y';break;};
	case 6: {Day[5]='J'; Day[6]='u';Day[7]='n';break;};
	case 7: {Day[5]='J'; Day[6]='u';Day[7]='l';break;};
	case 8: {Day[5]='A'; Day[6]='u';Day[7]='g';break;};
	case 9: {Day[5]='S'; Day[6]='e';Day[7]='p';break;};
	case 10: {Day[5]='O'; Day[6]='c';Day[7]='t';break;};
	case 11: {Day[5]='N'; Day[6]='o';Day[7]='v';break;};
	case 12: {Day[5]='D'; Day[6]='e';Day[7]='c';break;};
	}
	Day[8]=' ';
	temp = (*(current_time+4) - *(current_time+4) % 10)/10; // Há»‡ sá»‘ cao cá»§a ngÃ y
	Day[9]=temp+48;

	temp = *(current_time+4) % 10; // Há»‡ sá»‘ tháº¥p cá»§a  ngÃ y
	Day[10]=temp+48;

	Day[11]='\0';

	// Viáº¿t giÃ¡ trá»‹ chuá»—i thá»�i gian lÃªn mÃ n hÃ¬nh.
	OLED_P8x16Str(0,3,Time);
	OLED_P6x8Str(0,6,Day);

}



