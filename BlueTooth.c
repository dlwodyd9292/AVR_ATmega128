#include <avr/io.h>
#include <stdio.h>
#include "UART0.h"
#include "UART1.h"

void BlueTooth_PC_communication()
{
	if((UCSR0A >> RXC0) == 1) // PC를 통해 MCU로 데이터가 들어오는게 완료 되면
	{
		uint8_t data = UART0_receive(); // PC에서 받은 데이터를 data 변수에 저장
		UART1_transmit(data); // data 변수에 저장된 값을 UART1을 이용해 블루투스 모듈로 전송
	}
	
	if((UCSR1A >> RXC1) == 1) // 블루투스 모듈을 통해 MCU로 데이터가 들어오는게 완료 되면
	{
		uint8_t data = UART1_receive(); // 블루투스 모듈에서 받은 데이터를 data 변수에 저장
		UART0_transmit(data); // data 변수에 저장된 값을 UART0을 이용해 PC로 전송
	}	
}

int main(void)
{
	UART0_init(); // MCU - PC
	UART1_init(); // MCU - 블루투스 모듈용 
	
	while (1)
	{
		BlueTooth_PC_communication();
	}
	
	return 0;
}
