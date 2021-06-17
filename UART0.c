/*
 * UART0.c
 *
 * Created: 2021-06-17 오전 11:31:37
 *  Author: kccistc
 */ 
#include <avr/io.h>

void UART0_init(void)
{
	UBRR0H = 0x00;			// 9600 보율로 설정
	UBRR0L = 207;
	
	UCSR0A |= _BV(U2X0);			// 2배속 모드
	// 비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드
	UCSR0C |= 0x06;
	
	UCSR0B |= _BV(RXEN0);		// 송수신 가능
	UCSR0B |= _BV(TXEN0);
}

void UART0_transmit(char data)
{
	while( !(UCSR0A & (1 << UDRE0)) );	// 송신 가능 대기
	// UDRE1 비트를 체크해서 1일 경우 루프를 빠져 나온다 (UDRE 버퍼가 비어 데이터를 받을 준비가 되어 있을 때 1)
	// UDRE1 비트를 체크해서 1이 아닐 경우 루프에 남는다 (UDRE 버퍼가 비어 데이터를 받을 준비가 되어 있지 않을 때 1이 아님)
	
	UDR0 = data; // UDRE 버퍼가 비어서 데이터를 받을 준비가 되어 있을때 루프를 빠져나와 데이터 전송
}

unsigned char UART0_receive(void)
{
	while( !(UCSR0A & (1<<RXC0)) );	// 데이터 수신 대기
	// RXC1 비트를 체크해서 1일 경우 루프를 빠져나온다 (수신 버퍼 UDRE에 읽지 않은 문자가 있을 때 1)
	// RXC1 비트를 체크해서 1이 아닐 경우(0일 경우) 루프에 남는다 (수신 버퍼 UDRE가 비어 있을 때 0)
 
	return UDR0; // 수신 버퍼 UDRE에 읽지 않는 문자가 있을 때 루프를 빠져 나와서 데이터 수신 
}

void UART0_print_string(char *str)	// 문자열 송신
{
	for(int i = 0; str[i]; i++)			// ‘\0’ 문자를 만날 때까지 반복
	UART0_transmit(str[i]);			// 바이트 단위 출력
}

void UART0_print_1_byte_number(uint8_t n)
{
	char numString[4] = "0";
	int i, index = 0;
	
	if(n > 0){					// 문자열 변환
		for(i = 0; n != 0 ; i++)
		{
			numString[i] = n % 10 + '0';
			n = n / 10;
		}
		numString[i] = '\0';
		index = i - 1;
	}
	
	for(i = index; i >= 0; i--)		// 변환된 문자열을 역순으로 출력
	UART0_transmit(numString[i]);
}
