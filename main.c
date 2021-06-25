#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

volatile int bitCount; //32bit(4byte)를 센다
volatile unsigned long receivedData;
int LED_count = 0;

char LED_right[4] = {0x08, 0x04, 0x02, 0x01};
	
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
	static unsigned long code_value[] = {	// 코드값 (그림 12 참조)
		0xFF6897,
		0xFF30CF,
		0xFF18E7,
		0xFF7A85,
		0xFF10EF,
	};
	
	if(repeat == 1) return;
		
		PORTA = 0x01; 
		
	if(receivedData == 0xFF22DD) // << 버튼 값이 눌리면
	{	
		PORTA = 0x00;
		
		for(int i = 0; i < 4; i++)
		{
			PORTA =  (1 << i); 
			_delay_ms(1000);
			printf("LED SHIFT <<\r\n");
		}
	}
	
	else if(receivedData == 0xFF02FD) // >> 버튼 값이 눌리면
	{	
		PORTA = 0x08;
		
		for(int i = 0; i < 4; i++)
		{
			PORTA = LED_right[i];
			_delay_ms(1000);
			printf("LED SHIFT >>\r\n");			
		}

	}
	
	PORTA = get_LED_pattern(LED_count);; // 위의 if/else if 문의 결과로 나온 LED_count 값을 get_LED_pattern 함수를 통해서 Port B의 출력 제어
	
	for(int i = 0; i <= 8; i++){
		if(code_value[i] == receivedData)
		{	// 버튼 확인
			uint8_t LED_value = 0;
			printf("LED %d ON\r\n", i);
			
			for(int j = 0; j < i; j++){		// LED 제어 데이터 생성
				LED_value = (LED_value << 1) + 1;
			}
			
			PORTA = LED_value;
			break;
		}
	}
}

ISR(INT1_vect)
{
	int time = TCNT2;		   // 인터럽트 발생 시간을 time 변수에 저장
	int overflow = TIFR & (1 << TOV2); // 오버플로우 발생했을 때 값을 overflow로 저장
	if(bitCount == 32)
	{
		if ((time > 201) && (time < 221) && (overflow == 0)) // 일반 데이터 리드 코드 13.5ms = 약 211 클록
		{
			receivedData = 0;
			bitCount = 0;
		}
		else if((time > 166) && (time < 186) && (overflow == 0)) // 반복 데이터 리드 코드 11.25ms = 약 176 클록
		{
			print_received_data(1);
		}
		else
		{
			bitCount = 32; // 잘못된 신호인 경우 재시작
		}
	}
	else				// 논리 0 : 약 18클록, 논리 1 : 약 35 클록
	{
		if((time > 40) || (overflow != 0)) // 신호 간격이 너무 긴 경우
		bitCount = 32; // 재시작
		else
		{
			if(time > 26) 		// 1을 수신 했을 떄((18+35) / 2)
			receivedData = (receivedData << 1) + 1;
			else
			receivedData = (receivedData << 1);
			
			if(bitCount == 31)	// 4byte 데이터 수신 완료
			print_received_data(0);
			
			bitCount++;
		}
	}
	
	TCNT2 = 0; // 0번 타이머/카운터 클리어
	
	TIFR = TIFR | (1 << TOV2); // ISR 륀이 호출되지 않으므로 자동으로 클리어 되지 않으므로 클리어 시켜주는 코드를 넣어줘야 한다.
}

int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	UART0_init();
	
	TCCR2 |= (1 << CS22) | (0 << CS21) | (1 << CS20);  // 분주비 1024
	
	EIMSK |= (1 << INT1);				  // INT0 인터럽트 활성화
	EICRA |= (1 << ISC11);				  // 하강 에지에서 인터럽트 발생
	sei();
	
	bitCount = 32;
	
	DDRA = 0x0F;
	
	PORTA = get_LED_pattern(LED_count);
	
	printf("** Initialization Completed... \r\n");
	
	while(1);
	return 0;
}
