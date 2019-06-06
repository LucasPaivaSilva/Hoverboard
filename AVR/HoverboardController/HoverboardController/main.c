/*
 * HoverboardController.c
 *
 * Created: 5/31/2019 10:15:57 AM
 * Author : Lucas
 */ 
#include "def_principais.h"
#include <avr/io.h>
unsigned char showStr[4];
float CurrentInput = 0;
int x;
int V1;
int V2;
float V3;
unsigned int NewFromSerial = 0;
unsigned char recived_str[3];
int velocity;
int CurrentVelocity = 0;
int Amode;
int teste;
int CurLimit;
uint16_t ReadADC(uint8_t ch)
{
	// select the corresponding channel 0~7
	// ANDing with ?7? will always keep the value
	// of ?ch? between 0 and 7
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch; // clears the bottom 3 bits before ORing
	
	// start single convertion
	// write ?1? to ADSC
	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ?0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}

ISR(USART_RX_vect)
{
	char status;
	int recived = 0;
	while(recived == 0)
	{
		status = USART_Recebe();
		if (status == 'L')
		{
			recived_str[0]= 'L';
			recived_str[1]= USART_Recebe();
			recived_str[2]= USART_Recebe();
			USART_Transmite('L');
			recived = 1;
			NewFromSerial = 1;	
		}
		if (status == 'D')
		{
			recived_str[0]= 'D';
			recived_str[1]= USART_Recebe();
			recived_str[2]= USART_Recebe();
			USART_Transmite('D');
			recived = 1;
			NewFromSerial = 1;
		}
		if (status == 'K')
		{
			recived_str[0]= 'K';
			recived_str[1]= USART_Recebe();
			recived_str[2]= USART_Recebe();
			CurLimit = (int)((recived_str[1]-'0')*10) + (int)(recived_str[2]-'0');
			USART_Transmite('K');
			recived = 1;
		}
	}
	
}

ISR(TIMER0_OVF_vect)   
{
	teste = teste + 1;
	if (teste>=100)
	{
		teste = 0;
		if (NewFromSerial == 1)
		{
			NewFromSerial = 0;
			velocity = (int)((recived_str[1]-'0')*10) + (int)(recived_str[2]-'0');
			if(recived_str[0] == 'L')
			{
				Amode = 1;// Mode = 1, acelera
			}
			else if (recived_str[0] == 'D')
			{
				Amode = 0;// Mode = 0, desliga
				velocity = 0;
			}
			
		}
		if (Amode == 3)
		{
			velocity = 0;
			CurrentVelocity = 0;
			Amode = 2;
		}
		if (Amode == 1)
		{
			if (CurrentVelocity>velocity)
			{
				CurrentVelocity = CurrentVelocity - 1;
			}
			if (CurrentVelocity<velocity)
			{
				CurrentVelocity = CurrentVelocity + 1;
			}
			if (CurrentVelocity == velocity)
			{
				Amode = 2;
			}
		}
		if (Amode == 0)
		{
			if (CurrentVelocity>=4)
			{
				CurrentVelocity = CurrentVelocity - 4;
			}
			if (CurrentVelocity == 0)
			{
				Amode = 2;
			}
		}
		OCR0B = CurrentVelocity;
	}
	
}

int main(void)
{
	DDRB = 0xFF;
	DDRD = 0xFF;
    USART_Inic(MYUBRR);
    set_bit(UCSR0B, RXCIE0);
	TCCR0B = (1<<CS02) | (1<<CS01);/*TC0 com prescaler de 256, a 16 MHz gera uma interrupção a cada ~4ms*/ 
	TIMSK0 = 1<<TOIE0;
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	TCCR0A = 0b01100011;
	TCCR0B = 0b00001011;
	OCR0A = 100;
	OCR0B = 0;
	sei();
    while (1) 
    {
		V1 = 0;
		V2 = 0;
		CurrentInput = 0;
// 		for (x=0;x<=10;x++)
// 		{
// 			V1 = ReadADC(0) + V1;
// 			V2 = ReadADC(1) + V2;
// 			_delay_us(10);
// 		}
		V1 = ReadADC(4) - 512;
		if (V1>=CurLimit)
		{
			teste = 100;
			Amode = 3;
			USART_Transmite('E');
		}
		_delay_ms(10);
    }
}

