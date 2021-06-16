/*
 * 7_segment.c
 *
 * Created: 2021-06-16 오후 12:55:18
 * Author : kccistc
 * 버튼을 누르면 멈추고 버튼을 누르면 카운트가 시작되는 시계 프로그램 
 */ 

#define F_CPU 16000000L // 16MHZ 클럭 설정
#include <avr/io.h>		// io 헤더파일
#include <avr/interrupt.h> // 인터럽트 헤더 파일
#include <util/delay.h> // 딜레이 함수 헤더 파일
#define SEGMENT_DELAY 5 // LED 점등 대기 시간
#define MILLIS_PER_OVERFLOW 16 // ?? 
#define MICROS_PER_OVERFLOW 384 // ??
uint8_t numbers[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67}; //7세그먼트 숫자
	
volatile uint32_t millis0 = 0; // 밀리스초 저장을 위한 변수
volatile uint16_t micros = 0; // 마이크로초 저장을 위한 변수 

ISR(TIMER0_OVF_vect) //타이머 0번 인터럽트 서비스 루틴
{
	micros += MICROS_PER_OVERFLOW; // 인터럽트가 발생하면 MICROS_PER_OVERFLOW 값을 micros 변수에 저장 
	millis0 += MILLIS_PER_OVERFLOW; // 인터럽트가 발생하면 MICROS_PER_OVERFLOW 값을 millis0 변수에 저장 

	millis0 += (micros / 1000); // 밀리스는 1000분의 1 마이크로초이므로 해당 연산을 통해 밀리스 계산
	micros %= 1000; //나머지 연산을 통해 마이크로초 계산 
}
void display_digit(int position, int number) //숫자를 7세그먼트에 표시하는 함수
{				//출력할 자리(1~4), 출력할 숫자(0~9)
	PORTC |= 0x0F; // 포트C의 0,1,2번 핀을 출력으로 설정
	PORTC &= ~(1 << (position - 1)); // position 값에 -1 한만큼 1을 반전하여 and 연산하여 0으로 설정, 즉 포지션 - 1만큼 입력으로 설정 

	PORTD = numbers[number]; // number에 해당하는 배열의 세그먼트를 표시 
}
unsigned long millis() //밀리스 함수 : 프로그램 시작 후 경과 시간을 알려주는 함수  
{
	unsigned long m; 
	uint8_t oldSREG = SREG; // 상태 레지스터 값 저장

	cli(); // millis0가 값을 읽는 동안 해당 변수 값이 변하지 않도록 인터럽트 비활성화

	m = millis0;

	SREG = oldSREG; // 이전상태 레지스터 복원

	return m; //프로그램 시작 후 경과 시간
}
int main(void)
{
	DDRD = 0xFF; //DDRD 핀을 출력으로 설정
	DDRC = 0x0F; //DDRC 0,1,2번 핀을 출력으로 설정

	DDRB = 0x00; //B포트를 입력으로 설정 

	TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00); //타이머 0 분주비를 1024로 설정
	TIMSK |= (1 << TOIE0); // 타이머 0에 오버플로 인터럽트 설정
	sei(); // 전역 인터럽트 설정 
	
	int minutes = 0, seconds = 0; // 분, 초를 저장하는 함수
	unsigned long time_previous, time_current; // 이전 및 현재 시간을 밀리스 함수로 읽어서 값을 저장한 변수
	uint8_t start = 0; //버튼 눌림을 확인하는 플래그 변수 

	time_previous = millis(); //밀리스 함수를 이용해 이전 시간 값을 저장 

	while(1)
	{
		uint8_t button_state = PINB & 0x01; //button_state 값에 1이 틀어오는지 확인하는 변수

		if(button_state) //button_state값에 1이 들어온다면, 즉 버튼이 눌렸다면
		{
			start = (start + 1) % 2; //start 값을 1 더한 후 2로 나머지 연산하여 그 값을 start에 저장 
		}

		time_current = millis(); //밀리스 함수를 이용해 현재 시간 값을 저장 

		if(start == 0) //버튼이 눌리지 않았다면
		{
			time_previous = time_current; // 현재시간을 이전 시간으로 설정
		}

		if(time_current - time_previous >= 1000) // 현재시간과 이전 시간의 차가 1000ms보다 크다면
		{
			time_previous = time_current; //현재시간을 이전 시간으로 설정
			seconds++; // 초 증가 
			if(seconds >= 60) // 초가 60이 되면
			{
				minutes = (minutes + 1) % 60; // 분 증가
				seconds %= 60; //초 나머지를 구한다 
			}
		}
		display_digit(1, minutes / 10); // 십의 단위 분
		_delay_ms(SEGMENT_DELAY);
		display_digit(2, minutes % 10); // 일의 단위 분
		_delay_ms(SEGMENT_DELAY);
		display_digit(3, seconds / 10); // 십의 단위 초
		_delay_ms(SEGMENT_DELAY);
		display_digit(4, seconds % 10); // 일의 단위 초 
		_delay_ms(SEGMENT_DELAY);
	}

	return 0;
}
