void UART1_transmit(char data)
{
	while( !(UCSR1A & (1 << UDRE1)) );	// 송신 가능 대기
	// UDRE1 비트를 체크해서 1일 경우 루프를 빠져 나온다 (UDRE 버퍼가 비어 데이터를 받을 준비가 되어 있을 때 1)
	// UDRE1 비트를 체크해서 1이 아닐 경우 루프에 남는다 (UDRE 버퍼가 비어 데이터를 받을 준비가 되어 있지 않을 때 1이 아님)
	
	UDR1 = data; // UDRE 버퍼가 비어서 데이터를 받을 준비가 되어 있을때 루프를 빠져나와 데이터 전송
}

unsigned char UART1_receive(void)
{
	while( !(UCSR1A & (1<<RXC1)) );	// 데이터 수신 대기
	// RXC1 비트를 체크해서 1일 경우 루프를 빠져나온다 (수신 버퍼 UDRE에 읽지 않은 문자가 있을 때 1)
	// RXC1 비트를 체크해서 1이 아닐 경우(0일 경우) 루프에 남는다 (수신 버퍼 UDRE가 비어 있을 때 0)
 
	return UDR1; // 수신 버퍼 UDRE에 읽지 않는 문자가 있을 때 루프를 빠져 나와서 데이터 수신 
}
