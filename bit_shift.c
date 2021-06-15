/*
 * LED_BIT_SHIFT.c
 *
 * Created: 2021-06-14 오전 10:04:47
 * Author : kccistc
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

char circular_shift_left(char pattern)
{
	return ( (pattern << 1) | (pattern >> 7) );
}

char circular_shift_right(char pattern)
{
	return ( (pattern >> 1) | (pattern << 7) );
}

void circular_shift_left_right(void)
{	
	
}
int get_button_state_0x01(void)
{
	if( (PINF & 0x01) >> 0 == 1){
		_delay_ms(10);
		if( (PINF & 0x01) >> 0 == 1){
			return 1;
		}
	}
	
	return 0;
}

int get_button_state_0x02(void)
{
	if( (PINF & 0x02) >> 1 == 1){
		_delay_ms(10);
		if( (PINF & 0x02) >> 1 == 1){
			return 1;
		}
	}
	
	return 0;
}

int get_button_state_0x04(void)
{
	if( (PINF & 0x04) >> 2 == 1){
		_delay_ms(10);
		if( (PINF & 0x04) >> 2 == 1){
			return 1;
		}
	}
	
	return 0;
}


int main(void)
{	
		DDRB = 0xFF;				// PORT B 핀을 출력으로 설정
		DDRF &= ~0x07;			// PORT F의 PF0,1,2 핀을 입력으로 설정
		
		PORTF |= 0x01; // F0번 핀 풀업 저항 설정 
		
		char pattern = 0x01;			// 초기 출력값
		PORTB = pattern;
		
		// 이전 및 현재 버튼의 상태
		char state_previous_0x01 = 1, state_current_0x01;
		char state_previous_0x02 = 1, state_current_0x02;
		char state_previous_0x04 = 0, state_current_0x04;
		
		void button_0 ()
		{
			state_current_0x01 = get_button_state_0x01();	// F0 버튼 상태 읽기
			// 버튼이 눌러지지 않은 상태에서 눌러진 상태로 바뀌는 경우
			if(state_current_0x01 == 1 && state_previous_0x01 == 0){
				pattern = circular_shift_left(pattern);
				PORTB = pattern;
			}
			state_previous_0x01 = state_current_0x01;	// F0 버튼 상태 업데이트
		}

		void button_1 ()
		{
			state_current_0x02 = get_button_state_0x02();	// F1 버튼 상태 읽기
			// 버튼이 눌러지지 않은 상태에서 눌러진 상태로 바뀌는 경우
			if(state_current_0x02 == 1 && state_previous_0x02 == 0){
				pattern = circular_shift_right(pattern);
				PORTB = pattern;
			}
			state_previous_0x02 = state_current_0x02;	// F1 버튼 상태 업데이트
			
		}

		void button_2 ()
		{	
			unsigned int flag = 0; 
			
			state_current_0x04 = get_button_state_0x04();	// F2 버튼 상태 읽기
			// 버튼이 눌러지지 않은 상태에서 눌러진 상태로 바뀌는 경우
			if(state_current_0x04 == 0 && state_previous_0x04 == 1)
			{
					pattern = circular_shift_right(pattern);
					PORTB = pattern;
				
			}
			state_previous_0x04 = state_current_0x04;	// F2 버튼 상태 업데이트
		}
		
	while (1)
	{	
			button_0();
			
			button_1();
			
			button_2();					
	}

	return 0;
}
