#include "msp430g2553.h"
#include "adc.h"

void adc_init()
{
	ADC10CTL1 = INCH_10 + ADC10DIV_0 + SHS_0 ; // Temp Sensor ADC10CLK
	/*
	 *  INCH : channel select : 			tempSensor
	 *  ADC10DIV : clock divider:			/1
	 *  SHS: sample-and-hold source select:	ADC10SC
	 *
	 */
	ADC10CTL0 = SREF_1 + ADC10SHT_3;
	/*
	 * SREF: select reference:			VR+ = VREF+ and VR- = AVSS
	 * ADC10SHT: sample-and-hold time:	64 x ADC10CLKs
	 * REFON: 							reference generator: 1.5V
	 *
	 */
	_delay_cycles(80); // Wait for ADC Ref to settle
}

unsigned int temperature()
{
	ADC10CTL1 = INCH_10 + ADC10DIV_0 + SHS_0 ; // Temp Sensor ADC10CLK
	ADC10CTL0 |= REFON + ADC10ON;
	_delay_cycles(50); // Wait for ADC Ref to settle
	ADC10CTL0 |= ENC + ADC10SC; // Sampling & conversion start
	_delay_cycles(500);
	ADC10CTL0 &= ~ENC;
	ADC10CTL0 &= ~(REFON + ADC10ON);
	return ADC10MEM;
}
/*
 * main.c
 */

unsigned int gas()
{
	ADC10CTL1 = INCH_0 + ADC10DIV_0 + SHS_0 ; // Temp Sensor ADC10CLK
	ADC10CTL0 |= REFON + ADC10ON;
	_delay_cycles(50); // Wait for ADC Ref to settle
	ADC10CTL0 |= ENC + ADC10SC; // Sampling & conversion start
	_delay_cycles(500);
	ADC10CTL0 &= ~ENC;
	ADC10CTL0 &= ~(REFON + ADC10ON);
	return ADC10MEM;
}
