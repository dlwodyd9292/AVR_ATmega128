/*
 * Temp_Light_Sensor.c
 *
 * Created: 2021-06-22 오전 9:27:56
 * Author : kccistc
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "clcd.h"
#include "i2c_master.h"
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

void ADC_init(unsigned char channel)
{
	ADMUX |= (1 << REFS0); 		// AVCC를 기준 전압으로 선택
	
	ADCSRA |= 0x07;			// 분주비 설정
	ADCSRA |= (1 << ADEN);		// ADC 활성화
	ADCSRA |= (1 << ADFR);		// 프리 러닝 모드

	ADMUX |= ((ADMUX & 0xE0) | channel);	// 채널 선택
	ADCSRA |= (1 << ADSC);		// 변환 시작
}

int read_ADC(void)
{
	while(!(ADCSRA & (1 << ADIF)));	// 변환 종료 대기
	
	return ADC;				// 10비트 값을 반환
}


int main(void)
{	
	UART0_init();
	i2c_lcd_init();
	ADC_init(0); // 온도 센서 
	ADC_init(5); // 조도 센서 
	
	unsigned char str_temp[16];
	unsigned char str_light[16];
	
	int read_temp, temperature, read_light;
	float input_voltage_temp;
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
    /* Replace with your application code */
    while (1) 
    {	
		read_temp = read_ADC();			// 온도센서 ADC 값 읽기
		read_light = read_ADC();		// 조도센서 
		// 0에서 1023 사이의 값을 0V에서 5V 사이 값으로 변환한다.
		input_voltage_temp = read_temp * 5.0 / 1023.0;
		
		// 10mV에 1℃이므로 100을 곱해서 현재 온도를 얻는다.
		temperature = (int)(input_voltage_temp * 100.0);
		
		printf("temp = %dºC\n light = %dlm\r\n", temperature, read_light);
		
		sprintf(str_temp, "temp = %04d C", temperature);
		sprintf(str_light, "light = %04d L", read_light);
		
		i2c_lcd_string(0, 1, str_temp);
		i2c_lcd_string(1, 1, str_light);
		_delay_ms(1000);			// 1초에 한 번 읽음
		
		
    }
}

