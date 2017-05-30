#include <msp430.h> 
#include "serial.h"
#include "adc.h"

#include "i2c.h"
#include "xDS1307.h"
#include "xLCD.h"
#include "graphic.h"

//#ifndef TIMER0_A1_VECTOR
//#define TIMER0_A1_VECTOR TIMERA1_VECTOR
//#define TIMER0_A0_VECTOR TIMERA0_VECTOR
//#endif

unsigned char CommandCode;
unsigned char Command_Data[20];


void clock_init()
{
	if (CALBC1_16MHZ ==0xFF || CALDCO_16MHZ == 0xFF)
	{
		while(1); // If cal constants erased, trap CPU!!
	}
	BCSCTL1 = CALBC1_16MHZ; 	// Set range
	DCOCTL = CALDCO_16MHZ; 		// Set DCO step + modulation
	IFG1 &= ~OFIFG;				// disable oscillator-fault flag
	BCSCTL2 |= SELM_0 + DIVM_0; //select DCO/8 for MCLK
}

void io_init()
{
	// --led----
	P1DIR |= BIT4 + BIT3;
	P1OUT &=~ BIT4 + BIT3;

	//-- speaker---
	P2DIR |= BIT6;
	P2OUT &=~ BIT6;
	P2SEL &=~ BIT6;
	P2SEL2 &=~ BIT6;

	//-- sonar ---
	P2DIR |= BIT2;
	P2OUT &= ~BIT2;
	P2DIR &= ~BIT3;


	//--servo---
	P1DIR |= BIT5;
	P2DIR |= BIT0 + BIT1;
	P2SEL &=~ BIT0 + BIT1;
	P2SEL2 &=~ BIT0 + BIT1;

	P2OUT &= ~(BIT0 + BIT1);
	P1OUT &=~ BIT5;

	//--button---
	P2DIR &= ~BIT4;

}

void Led(unsigned char x,unsigned char y )            // x la led can chon: 1.led1, 2.led2;  y la bat tat led chon: 1.on, 2.off
{

	if ((x==1)&&(y==1)){P1OUT |= BIT3;}
	if ((x==1)&&(y==0)){P1OUT &= ~BIT3;}
	if ((x==2)&&(y==1)){P1OUT |= BIT4;}
	if ((x==2)&&(y==0)){P1OUT &= ~BIT4;}
}
void Speaker(unsigned char stt)
{
	if (stt==0) P2OUT &= ~BIT6;
	else P2OUT |= BIT6;
}

//========================================================
void Timer_A0_Init()
{
	TA0CCTL0 = CCIE;
	TA0CCTL1 = CCIE;
	TA0CCR0 = 800;
	TA0CCR1 = 64000;
	TA0CTL = TASSEL_2 + MC_2 +TAIE;

}

//=======================================================


//unsigned char a=0x56;
unsigned int count_10Hz=0;
unsigned char flag_10Hz=0;
unsigned char flag_SF05=0;

unsigned char flag_high=0;
unsigned char flag_sonar_count=0;

unsigned int distance_range=0;
unsigned int distance_value=0;

unsigned char flag_uart=0;

//=====servo=====
unsigned int count_servo=0;
unsigned char value_servo1=30;
unsigned char value_servo2=31;
unsigned char value_servo3=22;

//=====lcd========
unsigned char lcd_id=0;


#define flag_on 49
#define flag_off 48

struct
{
	unsigned char stop:1;
	unsigned char hot:1;
	unsigned char toxic:1;
	unsigned char lcd_change:1;
	unsigned char button:1;
	unsigned char alarm:1;

} event_flag;

#define max_range 600
#define dis_max 100
#define dis_min 50

unsigned int dis_filter_1=dis_max;
unsigned int dis_filter_2=dis_max;
unsigned int dis_filter_3=dis_max;
unsigned int dis_filter_4=dis_max;
unsigned int dis_filter_5=0;

unsigned int dis_average;

void check_distance(unsigned int dis)
{
	dis_filter_5 = dis_filter_4;
	dis_filter_4 = dis_filter_3;
	dis_filter_3 = dis_filter_2;
	dis_filter_2 = dis_filter_1;
	dis_filter_1=dis;
	dis_average=(dis_filter_1+dis_filter_2+dis_filter_3+dis_filter_4+dis_filter_5)/5;
//	Uart_int_display(dis_average);

	if(dis < dis_min)
	{
		if(event_flag.stop == 0)
		{
			event_flag.stop = 1;
			UartSendEvent(40,flag_on);
		}
	}
	else if(dis > dis_max)
	{
		if(event_flag.stop == 1)
		{
			event_flag.stop = 0;
			UartSendEvent(40,flag_off);
		}
	}

}

#define temp_max 755
#define temp_min 750
unsigned int temp_filter_1=temp_min;
unsigned int temp_filter_2=temp_min;
unsigned int temp_filter_3=temp_min;
unsigned int temp_filter_4=temp_min;
unsigned int temp_filter_5=temp_min;
unsigned int temp_filter_6=0;
unsigned int temp_average;


void check_temp(unsigned int temp)
{
	temp_filter_6 = temp_filter_5;
	temp_filter_5 = temp_filter_4;
	temp_filter_4 = temp_filter_3;
	temp_filter_3 = temp_filter_2;
	temp_filter_2 = temp_filter_1;
	temp_filter_1=temp;
	temp_average = (temp_filter_1 + temp_filter_2 +temp_filter_3 + temp_filter_4 + temp_filter_5 +temp_filter_6)/6;

//	Uart_int_display(temp_average );

	if(temp_average > temp_max)
	{
		if(event_flag.hot == 0)
		{
			event_flag.hot = 1;
			UartSendEvent(41,flag_on);
		}
	}
	else if(temp_average < temp_min)
	{
		if(event_flag.hot == 1)
		{
			event_flag.hot = 0;
			UartSendEvent(41,flag_off);
		}
	}
}


#define toxic_max 300
#define toxic_min 100
void check_gas(unsigned int toxic)
{
//	Uart_int_display(toxic );
	if(toxic > toxic_max)
	{
		if(event_flag.toxic == 0)
		{
			event_flag.toxic = 1;
			UartSendEvent(42,flag_on);
		}
	}
	else if (toxic < (toxic_min))
	{
		if(event_flag.toxic == 1)
		{
			event_flag.toxic = 0;
			UartSendEvent(42,flag_off);
		}
	}
}

unsigned char button_press_count=0;
void check_button ()
{
	if (P2IN & BIT4)
	{
		if (event_flag.button==0)
		{
			event_flag.button=1;
			UartSendEvent(43,flag_off);
		}
	}
	else
	{
		if(event_flag.button==1)
		{
			event_flag.button=0;
			UartSendEvent(43,flag_on);
		}

	}

}


unsigned char medic[2] = {4,20};
unsigned char alarm_1[2] = {22,5};

void check_time()
{
	unsigned char c_time[7];
	Time_read(c_time);
	if(Alarm_check(medic,c_time+1))
	{
		if (event_flag.alarm == 0)
		{
			event_flag.alarm=1;
			UartSendEvent(44,flag_on);
		}
	}
	else
	{
		if (event_flag.alarm == 1)
		{
			event_flag.alarm=0;
			UartSendEvent(44,flag_off);
		}
	}
}

void TrigSonar()
{
	P2OUT |=BIT2;
	__delay_cycles(240);
	P2OUT &= ~BIT2;
	flag_SF05 =1;
}


void main()
{
	WDTCTL = WDTPW + WDTHOLD; // watchdog timer setup
	clock_init();
	//
	//		Uart_Init();
	//		I2C_Init();
	//		OLED_Init();
	//
	//		P1DIR |=BIT3 +BIT4;
	//
	//
	//		Time_read(Current_time);
	//		LCD_task(1,0);
	////		LCD_Time_display(Current_time);
	//
	//		P1OUT ^=BIT4;
	//		while(1)
	//		{
	//			__delay_cycles(16000000);
	//			LCD_task(1,count_lcd);
	//			count_lcd++;
	//			if (count_lcd==4) count_lcd=0;
	//			Time_read(Current_time);
	//			P1OUT ^=BIT3;
	//		}


	io_init();
	Uart_Init();
	Timer_A0_Init();
	adc_init();



	P1OUT |= 0x18;
	P2OUT |= 0x40;
	_delay_cycles(16000000);
	P1OUT &=~ 0x18;
	P2OUT &=~ 0x40;
	UartStrPut("hello");

	I2C_Init();
	OLED_Init();
	LCD_task(robot_s);
	__bis_SR_register(GIE);       			// interrupts enabled

	while(1)
	{

		if (flag_uart==1)
		{
			flag_uart=0;
			switch (CommandCode)
			{
			case 33: //turn LED 1 - 0x21
			{
				if (Command_Data[0]=='0') Led(1,0);
				else Led(1,1);
				break;
			}
			case 34: //turn buzzer -0x22
			{
				if (Command_Data[0]=='0') Speaker(0);
				else Speaker(1);
				break;
			}
			case 35: //servo_1
			{
				set_int_value(Command_Data, &value_servo1);
				break;
			}
			case 36: // servo_2
			{
				set_int_value(Command_Data, &value_servo2);
				break;
			}
			case 37: //servo_3
			{
				set_int_value(Command_Data, &value_servo3);
				break;
			}
			case 50: //lcd
			{
				event_flag.lcd_change=1;
				set_int_value(Command_Data, &lcd_id);
				break;
			}

			default:
			{
				UartStrPut("???");
				break;
			}
			}
		}

		if(flag_10Hz ==1)
		{
			flag_10Hz=0;
			P1OUT ^= 0x10;
			TrigSonar();

			check_distance(distance_value);
			check_temp(temperature());
			check_gas(gas());
			check_button();
			check_time();
//			LCD_task(0);

		}

		if (event_flag.lcd_change==1)
		{
			event_flag.lcd_change=0;
			LCD_task(lcd_id);
		}

		//_delay_cycles(800000);
	}
}



#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

	CommandCode= UartCharGet();
	UartStrGet(Command_Data);
	flag_uart=1;
	//	UartStrPut(Command_Data);
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
	//	P1OUT ^= 0x08;
	TA0CCR0 += 800;

	if (flag_SF05 == 1)
	{
		if (P2IN & BIT3)
		{
			flag_sonar_count =1;
			flag_high =1;
		}
		else if (flag_high ==1)
		{
			flag_sonar_count =0;
			flag_high =0;
			flag_SF05 =0;
			if (distance_range < max_range) distance_value =distance_range;
			distance_range=0;
		}

		if (flag_sonar_count ==1) {distance_range++;}

	}

	count_servo++;
	if (count_servo == 400)
	{
		count_servo =0;
		P1OUT |= BIT5;
		P2OUT |= BIT0 + BIT1;
	}
	if (count_servo == value_servo1) {P2OUT &=~BIT1;}
	if (count_servo == value_servo2) {P2OUT &=~BIT0;}
	if (count_servo == value_servo3) {P1OUT &=~BIT5;}


}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1 (void)
{
	switch( TA0IV )
	{
	case  2:
	{
		TA0CCR1 += 64000;// Add Offset to CCR1
		count_10Hz++;
		if (count_10Hz==25)
		{
			count_10Hz=0;
			flag_10Hz=1;
		}
		//		P1OUT ^= 0x10;
		break;
	}
	case 10: {  break;}                 // Timer_A3 overflow
	}

}


