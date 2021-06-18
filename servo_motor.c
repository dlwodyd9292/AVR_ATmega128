#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

//16MHz / 8분주 = 2MHz 주기
//서보모터 제어 주파수인 50Hz(20ms)를 얻기 위해선 40000번의 카운터가 필요 
//20ms = 40000이므로 0도인 1ms(5%)는 2000, 90도인 1.5ms는 3000, 2ms(10%)는 4000으로 설정
//서보모터 마다 펄스폭이 일정하지 않으므로 해당 값을 기준으로 조율 필요 

#define ROTATION_DELAY 	1000	// 1초 대기
#define PULSE_MIN		1300	// 최소 펄스 길이 : 0도
#define PULSE_MID		3200    // 중간 펄스 길이 : 90도
#define PULSE_MAX		5000	// 최대 펄스 길이 : 180도 

void InitializeTimer1(void)
{
	// 모드 14, 고속 PWM 모드
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	
	// 비반전 모드
	// TOP : ICR1, 비교일치값 : OCR1A 레지스터
	TCCR1A |= (1 << COM1A1);
	
	TCCR1B |= (1 << CS11);		// 분주율 8, 2MHz
	
	ICR1 = 39999;				// 20ms 주기 / 16비트 타이머 모드 14에서는 ICR1 값이 TOP으로 설정되므로 
}

void init_pin_out()
{
	// 모터 제어 핀을 출력으로 설정
	DDRB |= (1 << PB5);	
}

void do_servo()
{
	OCR1A = PULSE_MIN;		// 0도
	_delay_ms(ROTATION_DELAY);
	
	OCR1A = PULSE_MID;		// 90도
	_delay_ms(ROTATION_DELAY);
	
	OCR1A = PULSE_MAX;		// 180도
	_delay_ms(ROTATION_DELAY);	
}

int main(void)
{	
	init_pin_out();
	InitializeTimer1();			// 1번 타이머/카운터 설정
	
	while(1)
	{
		do_servo();
	}

	return 0;
}
