/***********************************************************************
 * 
 * Parking assistant with 2 HC SR04 sensors for distance measurement.
 *
 * ATmega328P , 16 MHz, AVR 8-bit 
 *
 * Author: Marek Sicha
 * 2020
 **********************************************************************/



/* Definitions ----------------------------------------------*/
#define F_CPU 16000000			// CPU frequency 16MHz
#define P_Echo_Left PC5			// AVR pin where Echo of left sensor is connected
#define P_Trigger_Left PC4		// AVR pin where TRigger of left sensor is connected
#define P_Echo_Right PC3		// AVR pin where Echo of right sensor is connected
#define P_Trigger_Right PC2		// AVR pin where TRigger of right sensor is connected
#define Audio PC0			// AVR pin where Audio output is connected

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "lcd.h"            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for conversion function
#include "uart.h"           // Peter Fleury's UART library
#include "gpio.h"	    // GPIO library for AVR-GCC
#include <util/delay.h>	    // Functions for busy-wait delay loops

/* Variables ---------------------------------------------------------*/

volatile uint16_t num_of_overflows_Left =0;
volatile uint16_t distance_Left = 0 ;
volatile uint8_t Trigger_Left = 0;
volatile uint8_t first_High_Left = 0;

volatile uint16_t num_of_overflows_Right =0;
volatile uint16_t distance_Right = 0 ;
volatile uint8_t Trigger_Right = 0;
volatile uint8_t first_High_Right = 0;

/* Functions ---------------------------------------------------------*/
/**
* Main function where the program execution begins.
*
*/

int main(void)
{
	
    GPIO_config_output(&DDRC,P_Trigger_Left);		// set PC4 as output
    GPIO_write_low(&PORTC,P_Trigger_Left);		// set PC4 low
    GPIO_config_input_nopull(&DDRC,P_Echo_Left);	// set PC5 as input
	
    GPIO_config_output(&DDRC,P_Trigger_Right);		// set PC2 as output
    GPIO_write_low(&PORTC,P_Trigger_Right);		// set PC2 low
    GPIO_config_input_nopull(&DDRC,P_Echo_Right);	// set PC3 as input
	
    GPIO_config_output(&DDRC,Audio);			// set PC0 as output
    GPIO_write_low(&PORTC,Audio);			// set PC0 low
	
    // Configuration all LEDs as output
    GPIO_config_output(&DDRB,PB2);
    GPIO_config_output(&DDRB,PB3);
    GPIO_config_output(&DDRB,PB4);
    GPIO_config_output(&DDRB,PB5);
    GPIO_config_output(&DDRB,PB6);

    GPIO_config_output(&DDRD,PD0);
    GPIO_config_output(&DDRD,PD1);
    GPIO_config_output(&DDRD,PD2);
    GPIO_config_output(&DDRD,PD3);
    GPIO_config_output(&DDRC,PC6);
	
    // Configure 8-bit Timer/Counter2
    // Enable interrupt and set the overflow prescaler to 16 us
    TIM2_overflow_16us();
    TIM2_overflow_interrupt_enable();
	
    // Configure 16-bit Timer/Counter1
    // Enable interrupt and set the overflow prescaler to 262 ms
    TIM1_overflow_262ms();
    TIM1_overflow_interrupt_enable();

    // Configure 8-bit Timer/Counter0
    // Enable interrupt and set the overflow prescaler to 16 ms
    TIM0_overflow_16ms();
    TIM0_overflow_interrupt_enable();
	
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);
    lcd_gotoxy(0,0);
    lcd_puts("L Distance in cm:");
    lcd_gotoxy(0,1);
vlcd_puts("R Distance in cm:");
	
    // Initialize UART to asynchronous, 8N1, 9600
    uart_init(UART_BAUD_SELECT(9600,F_CPU));
	
    // Enables interrupts by setting the global interrupt mask
    sei();
	
    // Infinite loop
    while (1)
	{	
		if (Trigger_Left == 0) // Send start 10 us pulse to left sensor
		{
			GPIO_write_low(&PORTC,P_Trigger_Left);
			_delay_us(2);
			GPIO_write_high(&PORTC,P_Trigger_Left);
			_delay_us(10);
			GPIO_write_low(&PORTC,P_Trigger_Left);
			first_High_Left =0;
			Trigger_Left =1;
		}
		
		else if ((GPIO_read(&PINC,P_Echo_Left)==1) & (first_High_Left ==0)) // Reset counter after Echo sends pulse
		{
			num_of_overflows_Left =0;
			first_High_Left =1;
		}
		
		else if ((GPIO_read(&PINC,P_Echo_Left)==0) & (first_High_Left ==1)) // Counter distance after Echo receives pulse
		{
			distance_Left =num_of_overflows_Left* 0.2671;
			Trigger_Left =0;
		}

		if (Trigger_Right == 0)  // Send start 10 us pulse to right sensor
		{
			GPIO_write_low(&PORTC,P_Trigger_Right);
			_delay_us(2);
			GPIO_write_high(&PORTC,P_Trigger_Right);
			_delay_us(10);
			GPIO_write_low(&PORTC,P_Trigger_Right);
			first_High_Right =0;
			Trigger_Right =1;
		}
		
		else if ((GPIO_read(&PINC,P_Echo_Right)==1) & (first_High_Right ==0)) // Reset counter after Echo sends pulse
		{
			num_of_overflows_Right =0;
			first_High_Right =1;
		}
		
		else if ((GPIO_read(&PINC,P_Echo_Right)==0) & (first_High_Right ==1)) // Counter distance after Echo receives pulse 
		{
			distance_Right =num_of_overflows_Right* 0.2671;
			Trigger_Right =0;
		}
	}
	
	// Will never reach this
	return 0;
}


/* Interrupt service routines ----------------------------------------*/

/**
* ISR starts when Timer/Counter2 overflows. Counter is used for
* measuring time => one overflow is 16 us
* audio output frequency setting
*/

ISR(TIMER2_OVF_vect)
{
	static uint8_t distance =0;
	static uint16_t freq_counter =0;
	freq_counter ++;
	
	num_of_overflows_Left ++;
	num_of_overflows_Right ++;
	
	// Frequency setting for the shortest distance only
	if (distance_Left > distance_Right)
	{
		distance = distance_Right;
	}
	else
	{
		distance = distance_Left;
	}
	
	// Frequency setting depending on distance
	if (distance  <= 100 && distance > 50)
	{
		if (freq_counter == 188)
		{
			freq_counter =0;
			GPIO_write_high(&PORTC,Audio);
		}
	}
	else if (distance <= 50 && distance > 10 )
	{
		if (freq_counter == 38)
		{
			GPIO_write_high(&PORTC,Audio);
			freq_counter =0;
		}
	}
	else if (distance <= 10 &&distance > 2)
	{
		if (freq_counter == 8)
		{
			GPIO_write_high(&PORTC,Audio);
			freq_counter =0;
		}
	}
	else if (distance <= 2)
	{
		if (freq_counter == 5)
		{
			GPIO_write_high(&PORTC,Audio);
			freq_counter =0;
		}
	}		
}

/**
* ISR starts when Timer/Counter1 overflows.
* Send distance from left sensor to uart
* Display distance value on display
* Controlling the LED bar
*/

ISR(TIMER1_OVF_vect)
{
	char char_distance_Left[3];
	static uint8_t old_distance_Left = 0;
	static uint8_t counter1 =0; 
	counter1 ++;
	if ((distance_Left != old_distance_Left) & (counter1 ==1))
	{
		counter1 =0;
		if (distance_Left >= 400){distance_Left =400;}
		if (distance_Left <= 2){distance_Left =2;}
			
		itoa(distance_Left,char_distance_Left,10);
		
		// Display distance on LCD
		lcd_gotoxy(17,0);
		lcd_puts("    ");
		lcd_gotoxy(17,0);
		lcd_puts(char_distance_Left);
		
		// Send distance to uart
		uart_puts(" Left distance in cm: ");
		uart_puts(char_distance_Left);
		uart_puts("\n\r");
		old_distance_Left = distance_Left;
		
		// Setting the LED bar depending on the distance
		if (distance_Left >= 400)
		{
			PORTB &= ~((1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5)|(1<<PB6));
		}
		else if (distance_Left >= 300)
		{
			PORTB &= ~((1<<PB3)|(1<<PB4)|(1<<PB5)|(1<<PB6)); PORTB |=(1<<PB2);
		}
		else if (distance_Left >= 200)
		{
			PORTB &= ~((1<<PB4)|(1<<PB5)|(1<<PB6)); PORTB |=((1<<PB2)|(1<<PB3));	
		}
		else if (distance_Left >= 100)
		{
			PORTB &= ~((1<<PB5)|(1<<PB6)); PORTB |=((1<<PB2)|(1<<PB3)|(1<<PB4));
		}
		else if (distance_Left >= 50)
		{
			PORTB &= ~(1<<PB6); PORTB |=((1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5));
		}
		else if (distance_Left >= 2)
		{
			PORTB |=((1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5)|(1<<PB6));
		}
	}
	
	// Prevents freezing at distance < 10 cm
	else if ((distance_Left == old_distance_Left )&(counter1 ==1))
	{
		Trigger_Left =0;
		distance_Left =2;
		counter1 =0;
	}

}

/**
* ISR starts when Timer/Counter0 overflows.
* send distance from right sensor to uart
* Display distance value on display
* Controlling the LED bar
*/
ISR(TIMER0_OVF_vect)
{
	char char_distance_Right[3];
	static uint8_t old_distance_Right = 1;
	static uint8_t counter0 = 0;
	counter0 ++;
	if ((counter0 ==16) &(old_distance_Right != distance_Right))
	{
		counter0 =0;
		if (distance_Right >= 400){distance_Right =400;}
		if (distance_Right <= 2){distance_Right =2;}
			
		itoa(distance_Right,char_distance_Right,10);
		
		// Display distance on LCD
		lcd_gotoxy(17,1);
		lcd_puts("    ");
		lcd_gotoxy(17,1);
		lcd_puts(char_distance_Right);
		
		// Send distance to uart
		uart_puts("Right distance in cm: ");
		uart_puts(char_distance_Right);
		uart_puts("\n\r");
		old_distance_Right = distance_Right;
		
		// Setting the LED bar depending on the distance
		if (distance_Right >= 400)
		{
			PORTD &= ~((1<<PD0)|(1<<PD1)|(1<<PD2)|(1<<PD3)); PORTC&= ~(1<<PC6);
		}
		else if (distance_Right >= 300)
		{
			PORTD &= ~((1<<PD1)|(1<<PD2)|(1<<PD3)); PORTC&= ~(1<<PC6); PORTD |= (1<<PD0);
		}
		else if (distance_Right >= 200)
		{
			PORTD &= ~((1<<PD2)|(1<<PD3)); PORTC&= ~(1<<PC6); PORTD |= ((1<<PD0)|(1<<PD1));
		}
		else if (distance_Right >= 100)
		{
			PORTD &= ~(1<<PD3); PORTC&= ~(1<<PC6); PORTD |= ((1<<PD0)|(1<<PD1)|(1<<PD2));
		}
		else if (distance_Right >= 50)
		{
			PORTC&= ~(1<<PC6); PORTD |= ((1<<PD0)|(1<<PD1)|(1<<PD2)|(1<<PD3));
		}
		else if (distance_Right >= 2)
		{
			PORTC|= (1<<PC6); PORTD |= ((1<<PD0)|(1<<PD1)|(1<<PD2)|(1<<PD3));
		}			
		
	}
	
	// Prevents freezing at distance < 10 cm
	else if ((counter0 == 14) &(old_distance_Right == distance_Right))
	{	
		Trigger_Right =0;
		distance_Right =2;
		counter0 =0;
	}
}
