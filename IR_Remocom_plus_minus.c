#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

volatile int bitCount;
volatile unsigned long receivedData;
int LED_count = 4;

uint8_t get_LED_pattern(int count) // LED 패턴을 Port B에 입력하는 함수 
{
	uint8_t value = 0;
  
	for(int i = 0; i < count; i++)
  {
		value = (value << 1) + 1;
	}
	return value;
}

void print_received_data(int repeat)
{
	if(repeat == 1) return;
	
	if(receivedData == 0xFFA857) // + 버튼 값이 눌리면 
	{
		LED_count++;
		if(LED_count > 8) LED_count = 8;
	}
	
	else if(receivedData == 0xFFE01F) // - 버튼 값이 눌리면 
	{
		LED_count--;
		if(LED_count < 0) LED_count = 0;
	}
	
	PORTB = get_LED_pattern(LED_count);; //위의 if/else if 문의 결과로 나온 LED_count 값을 get_LED_pattern 함수를 통해서 Port B의 출력 제어  
}

ISR(INT0_vect)
{
	int time = TCNT0;
	int overflow = TIFR & (1 << TOV0);
	if(bitCount == 32)
	{
		if ((time > 201) && (time < 221) && (overflow == 0)) 
		{
			receivedData = 0;
			bitCount = 0;
		}
		else if((time > 166) && (time < 186) && (overflow == 0))
		{
			print_received_data(1);
		}
		else
		{
			bitCount = 32;
		}
	}
	else
	{
		if((time > 40) || (overflow != 0))
		bitCount = 32;
		else
		{
			if(time > 26)
			receivedData = (receivedData << 1) + 1;
			else
			receivedData = (receivedData << 1);
			if(bitCount == 31)
			print_received_data(0);
			bitCount++;
		}
	}
	
	TCNT0 = 0;
	TIFR = TIFR | (1 << TOV0);
}

int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	UART0_init();
	
	TCCR0 |= (1 << CS00) | (1 << CS01) | (1 << CS02);
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	
	bitCount = 32;
	
	DDRB = 0xFF;
	
	PORTB = get_LED_pattern(LED_count);
	
	printf("** Initialization Completed... \r\n");
	
	while(1);
	return 0;
}
