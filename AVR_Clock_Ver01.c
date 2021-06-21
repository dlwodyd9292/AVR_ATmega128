/*
 * Atmega128a_Clock.c
 *
 * Created: 2021-06-21 오후 2:10:28
 * Author : kccistc
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "clcd.h"
#include "i2c_master.h"

unsigned int count = 0, sec = 0, min = 0, hour = 0;
unsigned int ampm = 1;


ISR(TIMER0_COMP_vect)//오버플로우 인터럽트의 경우 : TIMER0_OVF_vect
{
TCNT0 = 0; //오버플로우 인터럽트의 경우 TCNT0 = 6;

count++; // 1ms 당 1회씩 증가
if (count > 1000) //1000ms = 1초 증가 
{
	sec++; count = 0;
	if(sec >= 60)
	{
		min++; sec = 0;
		if (min >= 60)
		{
			hour++; min =0;
			if(hour > 12)
			{
				hour = 0;
				if(ampm == 0) ampm = 1;
				else if(ampm == 1) ampm = 0;
				count =0;
			}
		}
	}
}

}

void timer_init()
{
	// 타이머 인터럽트 레지스터
   /*
    TIMSK |= (1 << TOIE0); // R/W 선택 TIMER 0 사용
    TCCR0 |= (1 << CS02); // 분주비 64
    TCNT0 = 6; // 6에서 시작 255가되어 256이 되면 OVF가 되어 인터럽트 구문을 실행한다.
    */
   
	TIMSK |= (1 << OCIE0); // R/W 선택 TIMER 0 사용
	TCCR0 |= (1 << CS02); // 분주비 64
	OCR0 = 250;
   
    /*
    1/16000000 = 0.0000000625
    분주비가 64
    0.0000000625 *64 = 0.000004 // TCNT0가 1올라가는 속도.
    ISR이 발생하는 시간 = 0.000004
    TCNT 250 회로 OVF 발생시 걸리는 시간 0.001
    500번이 OVF 인터럽트가 발생하면 1초가 된다.
    */
	
	sei();
}

int main(void)
{
	char str[16];
	
	i2c_lcd_init();
	timer_init(); 
	
	i2c_lcd_string(0, 2, "LEE JAE YONG");
		
    while (1) 
    {
		sprintf(str," %2d :%2d :%2d %2s", hour, min, sec, (ampm==0)?"AM":"PM");
		i2c_lcd_string(1, 0, str);
    }
}

