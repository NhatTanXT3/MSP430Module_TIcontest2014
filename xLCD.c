#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"
#include "xLCDfont.h"
#include "xDS1307.h"
#include "graphic.h"


#define SLAVE_ADDRESS_LCD 0x3c
#define SLAVE_ADDRESS_EEPROM 0x50
#define	Brightness	0xcf
#define X_WIDTH 	128
#define Y_WIDTH 	64

unsigned char Current_time[7];


void OLED_WrDat(unsigned char I2C_Data)
{
	I2C_Write_1byte(SLAVE_ADDRESS_LCD,0x40,I2C_Data);
}
void OLED_WrCmd(unsigned char I2C_Command)
{
	I2C_Write_1byte(SLAVE_ADDRESS_LCD,0x00,I2C_Command);            //Slave address,SA0=0
}

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);
	OLED_WrCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char bmp_dat)
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
			OLED_WrDat(bmp_dat);
	}
}

void OLED_CLS(void)
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
			OLED_WrDat(0);
	}
}

void OLED_P6x8Str(unsigned char x, unsigned char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
			OLED_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
}

void OLED_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++)
			OLED_WrDat(F8X16[c*16+i]);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
			OLED_WrDat(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}

void OLED_P16x16Ch(unsigned char x, unsigned char y,unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
}

void Draw_BMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0) y=y1/8;
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
		for(x=x0;x<x1;x++)
		{
			OLED_WrDat(BMP[j++]);
		}
	}
}

void OLED_Init(void)
{

	OLED_WrCmd(0xae);//--turn off oled panel
	OLED_WrCmd(0x00);//---set low column address
	OLED_WrCmd(0x10);//---set high column address
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);//--set contrast control register
	OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0Ã—Ã³Ã“Ì‰Â·Â´Ã–Ä‚ 0xa1Æ Æ°Â³Â£
	OLED_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0Ã‰Ã�Ã�Ã‚Â·Â´Ã–Ä‚ 0xc8Æ Æ°Â³Â£
	OLED_WrCmd(0xa6);//--set normal display
	OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f);//--1/64 duty
	OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);//-not offset
	OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);//--set pre-charge period
	OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);//--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb);//--set vcomh
	OLED_WrCmd(0x40);//Set VCOM Deselect Level
	OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02);//
	OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WrCmd(0x14);//--set(0x10) disable
	OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7)
	OLED_WrCmd(0xaf);//--turn on oled panel
	OLED_Fill(0x00);
	OLED_Set_Pos(0,0);
}


/* HÃ m váº½ LCD trÃªn MSP, táº­n dá»¥ng tá»‘i thiá»ƒu tÃ i nguyÃªn MSP, báº±ng cÃ¡ch má»—i láº§n Ä‘á»�c EEPROM vÃ  ghi LCD vá»›i chá»‰ 128Byte.
 * Biáº¿n trigger Ä‘á»ƒ xÃ¡c Ä‘á»‹nh xem cÃ³ báº­t LCD hay khÃ´ng
 * Trigger = 0 -> khÃ´ng báº­t, !=0 -> báº­t theo biáº¿n event
 * Biáº¿n event Ä‘á»ƒ lá»±a chá»�n xem hÃ¬nh nÃ o sáº½ Ä‘Æ°á»£c hiá»ƒn thá»‹
 * event = 0 ->alarm, 1 ->drug, 2 -> danger, 3 ->cháº¥m há»�i
 */

void LCD_task(unsigned char Event_flag)
{
	if (Event_flag >= clear_lcd_s)
		OLED_CLS();
	else
	{
		unsigned char BMP[128];
		//OLED_Init();
		I2C_Read_DualAddress(SLAVE_ADDRESS_EEPROM,Event_flag*1024+0,BMP, 128);
		Draw_BMP(0,0,128,1,BMP);
		I2C_Read_DualAddress(SLAVE_ADDRESS_EEPROM,Event_flag*1024+128,BMP, 128);
		Draw_BMP(0,1,128,2,BMP);
		I2C_Read_DualAddress(SLAVE_ADDRESS_EEPROM,Event_flag*1024+128*2,BMP, 128);
		Draw_BMP(0,2,128,3,BMP);
		I2C_Read_DualAddress(SLAVE_ADDRESS_EEPROM,Event_flag*1024+128*3,BMP, 128);
		Draw_BMP(0,3,128,4,BMP);
		I2C_Read_DualAddress(SLAVE_ADDRESS_EEPROM,Event_flag*1024+128*4,BMP, 128);
		Draw_BMP(0,4,128,5,BMP);
		I2C_Read_DualAddress(SLAVE_ADDRESS_EEPROM,Event_flag*1024+128*5,BMP, 128);
		Draw_BMP(0,5,128,6,BMP);
		I2C_Read_DualAddress(SLAVE_ADDRESS_EEPROM,Event_flag*1024+128*6,BMP, 128);
		Draw_BMP(0,6,128,7,BMP);
		I2C_Read_DualAddress(SLAVE_ADDRESS_EEPROM,Event_flag*1024+128*7,BMP, 128);
		Draw_BMP(0,7,128,8,BMP);
		//LCD_Time_display();

		if (Event_flag == clock_s) //alarm
		{
			Time_read(Current_time);
			LCD_Time_display(Current_time);
		}
	}
}

