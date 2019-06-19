/*
 * HoverboardController.c
 *
 * Created: 5/31/2019 10:15:57 AM
 * Author : Lucas
 */ 
#include "def_principais.h"
#include <avr/io.h>
unsigned char showStr[4];
int x;
volatile int V1;
int MeasuredCurrent; 
int MeasuredVoltage;
unsigned int NewFromSerial = 0;
unsigned char recived_str[3];
int velocity;
int CurrentVelocity = 0;
int Amode;
int teste;
int CurLimit;
int ShowTime;
#define midPoint 512

uint16_t ReadADC(uint8_t ch)
{
	ADMUX = (ADMUX & 0xF8)|ch;
	ADCSRA |= (1<<ADSC);
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
			/*USART_Transmite('L');*/
			recived = 1;
			NewFromSerial = 1;	
		}
		if (status == 'D')
		{
			recived_str[0]= 'D';
			recived_str[1]= USART_Recebe();
			recived_str[2]= USART_Recebe();
			/*USART_Transmite('D');*/
			recived = 1;
			NewFromSerial = 1;
		}
		if (status == 'K')
		{
			recived_str[0]= 'K';
			recived_str[1]= USART_Recebe();
			recived_str[2]= USART_Recebe();
			CurLimit = (int)((recived_str[1]-'0')*10) + (int)(recived_str[2]-'0');
			CurLimit = CurLimit * 10;
			CurLimit = (int) CurLimit + midPoint;
			ident_num(CurLimit, showStr);
// 			USART_Transmite('K');
// 			USART_Transmite(':');
// 			USART_Transmite(showStr[3]);
// 			USART_Transmite(showStr[2]);
// 			USART_Transmite(showStr[1]);
// 			USART_Transmite(showStr[0]);
// 			USART_Transmite('\n');
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
		for (x=0;x<10;x++)
		{
			V1 = (int) ReadADC(4) + V1;
			_delay_us(10);
		}
		
		MeasuredCurrent = (int) V1/10;
		MeasuredVoltage = (int) ReadADC(0) * 10;
		if ((MeasuredCurrent>=(CurLimit))&& (Amode!=0))
		{
			teste = 0;
			Amode = 0;
			USART_Transmite('E');
			USART_Transmite('\n');
			teste = 100;
		}
		if ((MeasuredCurrent<=(midPoint-10))&&(MeasuredVoltage>=5940)&&(Amode!=0))
		{
			teste = 0;
			Amode = 0;
			USART_Transmite('O');
			USART_Transmite('\n');
			teste = 100;
		}
		_delay_ms(1);
		ShowTime++;
		if (ShowTime>=250)
		{
			ShowTime = 0;
			USART_Transmite('\n');
			ident_num(MeasuredCurrent, showStr);
			USART_Transmite(showStr[3]);
			USART_Transmite(showStr[2]);
			USART_Transmite(showStr[1]);
			USART_Transmite(showStr[0]);
			USART_Transmite('\n');
			ident_num(MeasuredVoltage, showStr);
			USART_Transmite(showStr[3]);
			USART_Transmite(showStr[2]);
			USART_Transmite(showStr[1]);
			USART_Transmite(showStr[0]);
			USART_Transmite('\n');
		}
    }
}

