

void set_float_value (char *string,float *kp);
void set_int_value(unsigned char *string, unsigned char *kp);
void UartSendEvent(unsigned char a1,unsigned char a2);
/* ------------init uart module --------------------
* clock input to module: 16Mhz
* baudrate: 128000bps
* interrupt: enable or disable, you choise
*/
void Uart_Init();

/*------------ send a byte -----------------
*
*/
void UartCharPut(unsigned char data_put);

/*------------ get a byte -----------------
* wait untill uart module receive a byte
*/
unsigned char UartCharGet();

/*------------ send a string-----------------
* terminator characte: '\0' = 0x00h = 0b
*/
void UartStrPut(unsigned char *uart_str);

/*------------ get a string-----------------
* wait untill uart module get a string
* terminator characte: '\0' = 0x00h = 0b
*/
void UartStrGet(unsigned char *uart_str);




//===================================number display=========================================================

/*---------- hi?n th? s? th?p phân d?ng chu?i-------------------
 * s? có d?ng xxx.xx (5 ch? s? có d?u)
 * kí t? k?t thúc '\n'
 * khuyên dùng: hàm t?n r?t nhi?u th?i gian x? lý nên b?n ph?i
 * ch?c ch?n ràng k ?nh hu?ng d?n các hàm khác
 * gi?i h?n s? do bi?n k quy?t d?nh (2byte k d?u)
 * => max,min = +-655.36
 *-----------------------------------------------------------*/
void Uart_int_display(int num );

/*---------- hi?n th? s? nguyên d?ng chu?i-------------------
 * s? có d?ng xxxxx (5 ch? s? có d?u)
 * kí t? k?t thúc '\n'
 * chú ý: s? 16bit có d?u
 *-----------------------------------------------------------*/
void Uart_Float_Display(float num);

