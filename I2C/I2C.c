#include "I2C_RTC.h"

void I2C_init(void)
{
	DDRD |= (1 << I2C_SCL);	// SCL 핀을 출력으로 설정
	DDRD |= (1 << I2C_SDA);	// SDA 핀을 출력으로 설정

	TWBR = 32;			// I2C 클록 주파수 설정 200KHz
					// 연결하는 I2C 장치의 스펙에 따라 가변해서 적용 
}

void I2C_start(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	//TWEN : I2C 활성화
	//TWSTA : I2C 통신 시작
	//TWINT : 플래그를 클리어
	
	while( !(TWCR & (1 << TWINT)) );	// 시작 완료 대기
}

void I2C_transmit(uint8_t data)
{
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	//TWEA : 1로 세팅되어 있다면 마스터나 슬레이브가 1바이트를 수신한 경우 펄스 생성

	while( !(TWCR & (1 << TWINT)) );	// 전송 완료 대기
}

uint8_t I2C_receive_ACK(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	
	while( !(TWCR & (1 << TWINT)) );	// 수신 완료 대기
	
	return TWDR; // 송수신 데이터 저장 레지스터 
}

uint8_t I2C_receive_NACK(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN);
	
	while( !(TWCR & (1 << TWINT)) );	// 수신 완료 대기
	
	return TWDR;
}

void I2C_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	//			TWSTO : 마스터모드에서 전송을 끝내기 위해 1로 세팅 
}
