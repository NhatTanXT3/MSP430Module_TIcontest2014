

extern void OLED_WrCmd(unsigned char I2C_Command);
extern void OLED_Init(void);
extern void OLED_WrDat(unsigned char I2C_Data);
extern void OLED_Set_Pos(unsigned char x, unsigned char y);
extern void OLED_Fill(unsigned char bmp_dat);
extern void OLED_CLS(void);
extern void OLED_P6x8Str(unsigned char x, unsigned char y,unsigned char ch[]);
extern void OLED_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[]);
//extern void OLED_P16x16Ch(unsigned char x, unsigned char y,unsigned char N);
extern void Draw_BMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
extern void MSPDraw_BMP(int choice, unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);

extern void LCD_task(unsigned char Event_flag);


