#include <stdint.h>
#include <stdbool.h>
#include "15348.h"
#include "timer.h"
#include "serial.h"

//#define PB0 (*((volatile uint32_t *)0x40005004))
/*#define PB1 (*((volatile uint32_t *)0x40005008))
#define PB2 (*((volatile uint32_t *)0x40005010))
#define PB3 (*((volatile uint32_t *)0x40005020))*/
//#define PD0 (*((volatile uint32_t *)0x40007004))
/*#define PD1 (*((volatile uint32_t *)0x40007008))
#define PD2 (*((volatile uint32_t *)0x40007010))
#define PD3 (*((volatile uint32_t *)0x40007020))*/
#define PF0 (*((volatile uint32_t *)0x40025004))
/*#define PF1 (*((volatile uint32_t *)0x40025008))
#define PF2 (*((volatile uint32_t *)0x40025010))
#define PF4 (*((volatile uint32_t *)0x40025040))*/
#define PE0 (*((volatile uint32_t *)0x40024004))
#define PE1 (*((volatile uint32_t *)0x40024008))
#define PE2 (*((volatile uint32_t *)0x40024004)) //10
#define PE3 (*((volatile uint32_t *)0x40024008)) //20
#define PE4 (*((volatile uint32_t *)0x40024040)) //40
#define PE5 (*((volatile uint32_t *)0x40024080)) //40
	   

void PWM_Init(uint16_t period, uint16_t high){
	SYSCTL_RCGCTIMER_R |= 0x01; //active timer0
	SYSCTL_RCGCGPIO_R |= 0x02; // activate PORTB
	GPIO_PORTB_AFSEL_R = 0x40; // PB6
	GPIO_PORTB_DEN_R = 0x40;
	GPIO_PORTB_PCTL_R = 0xF7FFFFFF; //Timer0A
	TIMER0_CFG_R = 0x04; // 16 bit timer
	TIMER0_TAMR_R = 0x0000000A; // PWM and periodic mode
	TIMER0_TAILR_R = period - 1; // This is where the timer starts
	TIMER0_TAMATCHR_R = period-high-1; // point to make output low
}

/*void timer_handler()
{
				NVIC_ST_CURRENT_R = 0;
				time += 900719925;
				if ((time >> 32) >= 1)
				{
					time = time & 0xffffffff;
					writeToSR((unsigned int)(0xfffe + second_on));
					second_on = !second_on;
					seconds += 1;
					if (seconds == 60)
					{
						seconds = 0;
						minutes += 1;
						if (minutes == 60)
						{
							minutes = 0;
							hours = (hours)%12 + 1;
						}
					}
				}
}*/

void PLLInit()
{
    SYSCTL_RCC2_R |= 0x80000000;
    SYSCTL_RCC2_R |= 0x00000800;
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000007C0) + 0x00000540;
    SYSCTL_RCC2_R &= ~0x00000070;
    SYSCTL_RCC2_R &= ~0x00002000;
    SYSCTL_RCC2_R |= 0x40000000;
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + (4 << 22);
    while ((SYSCTL_RIS_R &0x00000040)==0){};
    SYSCTL_RCC2_R &= ~0x00000800;
}


/*
uint32_t	time_to_cycles(uint32_t time)
{
	//time in ms
	return time * 80000;
}

*/
/*
 * main.c
 */
int main(void) 
{
  PLLInit();
	SystickInit();
	SetupSerial();
    /** your code here **/
	/** port initializations **/
		SYSCTL_RCGCGPIO_R |= 0x3A;
		
		// unlock GPIOCR register
		GPIO_PORTE_LOCK_R = 0x4c4f434b;
		//GPIO_PORTB_LOCK_R = 0x4c4f434b;
		GPIO_PORTD_LOCK_R = 0x4c4f434b;
		
		// enable bits in commit register
		GPIO_PORTE_CR_R = 0xFF;
		//GPIO_PORTB_CR_R = 0xFF;	
		GPIO_PORTD_CR_R = 0xFF;	
	
		// disable analog
		GPIO_PORTE_AMSEL_R = 0x00;
		//GPIO_PORTB_AMSEL_R = 0x00;
		GPIO_PORTD_AMSEL_R = 0x00;	
		
		// disable alternate functionality
		GPIO_PORTE_AFSEL_R = 0x00;
		//GPIO_PORTB_AFSEL_R = 0x00;
		GPIO_PORTD_AFSEL_R = 0x00;	
		
		GPIO_PORTE_PCTL_R = 0x00000000;
		//GPIO_PORTB_PCTL_R = 0x00000000;
		GPIO_PORTD_PCTL_R = 0x00000000;	
		
		// enable pull up registers
		GPIO_PORTE_PUR_R = 0x00; // enables for F0 and F4
		//GPIO_PORTB_PDR_R = 0x00;
		
		// enable digital IO
		GPIO_PORTE_DEN_R = 0xFF;
		//GPIO_PORTB_DEN_R = 0xFF;
		GPIO_PORTD_DEN_R = 0xFF;
		
		
		GPIO_PORTE_DIR_R = 0x3f;
		//GPIO_PORTB_DIR_R = 0x0F;
		GPIO_PORTD_DIR_R = 0x0F;
		
		//ADC0_InitSWTriggerSeq3_Ch9();
		
		// main loop
		//writeToSR((unsigned int)0x0000);
		
		PWM_Init(4000, 3800);
		unsigned int template = 0xfafe;
		int i = 0;
		int direction = 1;
		bool soft_break = 0;
		bool hard_break = 0;
		//NVIC_ST_RELOAD_R = 0xffffff;
		while(1)
		{
			PE0 = 1;
			PE1 = 1;
			PE2 = 1;
			PE3 = 1;
			PE4 = 1; //Motors enabled
			PE5 = 1;
			/*if (direction > 0)
			{
				PE0 = 1;
				PE1 = 0;
				PE2 = 1;
				PE3 = 0;
			}
			else if (direction < 0)
			{
				PE0 = 0;
				PE1 = 1;
				PE2 = 0;
				PE3 = 1;				
			}
			else if (soft_break)
			{
				PE0 = 1;
				PE1 = 1;
				PE2 = 1;
				PE3 = 1;					
			}
			else
			{
				PE0 = 0;
				PE1 = 0;
				PE2 = 0;
				PE3 = 0;				
			}*/
		}
}
