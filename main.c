#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

volatile int bitCount; //32bit(4byte)�� ����
volatile unsigned long receivedData;
int LED_count = 0;

char LED_right[4] = {0x08, 0x04, 0x02, 0x01};
	
uint8_t get_LED_pattern(int count) // LED ������ Port B�� �Է��ϴ� �Լ�
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
	static unsigned long code_value[] = {	// �ڵ尪 (�׸� 12 ����)
		0xFF6897,
		0xFF30CF,
		0xFF18E7,
		0xFF7A85,
		0xFF10EF,
	};
	
	if(repeat == 1) return;
		
		PORTA = 0x01; 
		
	if(receivedData == 0xFF22DD) // << ��ư ���� ������
	{	
		PORTA = 0x00;
		
		for(int i = 0; i < 4; i++)
		{
			PORTA =  (1 << i); 
			_delay_ms(1000);
			printf("LED SHIFT <<\r\n");
		}
	}
	
	else if(receivedData == 0xFF02FD) // >> ��ư ���� ������
	{	
		PORTA = 0x08;
		
		for(int i = 0; i < 4; i++)
		{
			PORTA = LED_right[i];
			_delay_ms(1000);
			printf("LED SHIFT >>\r\n");			
		}

	}
	
	PORTA = get_LED_pattern(LED_count);; // ���� if/else if ���� ����� ���� LED_count ���� get_LED_pattern �Լ��� ���ؼ� Port B�� ��� ����
	
	for(int i = 0; i <= 8; i++){
		if(code_value[i] == receivedData)
		{	// ��ư Ȯ��
			uint8_t LED_value = 0;
			printf("LED %d ON\r\n", i);
			
			for(int j = 0; j < i; j++){		// LED ���� ������ ����
				LED_value = (LED_value << 1) + 1;
			}
			
			PORTA = LED_value;
			break;
		}
	}
}

ISR(INT1_vect)
{
	int time = TCNT2;		   // ���ͷ�Ʈ �߻� �ð��� time ������ ����
	int overflow = TIFR & (1 << TOV2); // �����÷ο� �߻����� �� ���� overflow�� ����
	if(bitCount == 32)
	{
		if ((time > 201) && (time < 221) && (overflow == 0)) // �Ϲ� ������ ���� �ڵ� 13.5ms = �� 211 Ŭ��
		{
			receivedData = 0;
			bitCount = 0;
		}
		else if((time > 166) && (time < 186) && (overflow == 0)) // �ݺ� ������ ���� �ڵ� 11.25ms = �� 176 Ŭ��
		{
			print_received_data(1);
		}
		else
		{
			bitCount = 32; // �߸��� ��ȣ�� ��� �����
		}
	}
	else				// �� 0 : �� 18Ŭ��, �� 1 : �� 35 Ŭ��
	{
		if((time > 40) || (overflow != 0)) // ��ȣ ������ �ʹ� �� ���
		bitCount = 32; // �����
		else
		{
			if(time > 26) 		// 1�� ���� ���� ��((18+35) / 2)
			receivedData = (receivedData << 1) + 1;
			else
			receivedData = (receivedData << 1);
			
			if(bitCount == 31)	// 4byte ������ ���� �Ϸ�
			print_received_data(0);
			
			bitCount++;
		}
	}
	
	TCNT2 = 0; // 0�� Ÿ�̸�/ī���� Ŭ����
	
	TIFR = TIFR | (1 << TOV2); // ISR ���� ȣ����� �����Ƿ� �ڵ����� Ŭ���� ���� �����Ƿ� Ŭ���� �����ִ� �ڵ带 �־���� �Ѵ�.
}

int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	UART0_init();
	
	TCCR2 |= (1 << CS22) | (0 << CS21) | (1 << CS20);  // ���ֺ� 1024
	
	EIMSK |= (1 << INT1);				  // INT0 ���ͷ�Ʈ Ȱ��ȭ
	EICRA |= (1 << ISC11);				  // �ϰ� �������� ���ͷ�Ʈ �߻�
	sei();
	
	bitCount = 32;
	
	DDRA = 0x0F;
	
	PORTA = get_LED_pattern(LED_count);
	
	printf("** Initialization Completed... \r\n");
	
	while(1);
	return 0;
}
