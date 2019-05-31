/*
 * HoverboardController.c
 *
 * Created: 5/31/2019 10:15:57 AM
 * Author : Lucas
 */ 
#include "def_principais.h"
#include <avr/io.h>
unsigned int NewFromSerial = 0;
unsigned char recived_str[3];
int velocity;

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
			
		}
		if (status == 'D')
		{
			recived_str[0]= 'D';
			recived_str[1]= USART_Recebe();
			recived_str[2]= USART_Recebe();
			USART_Transmite('D');
			recived = 1;
		}
	}
	NewFromSerial = 1;
	
}

int main(void)
{
	DDRB = 0xFF;
	DDRD = 0xFF;
    USART_Inic(MYUBRR);
    set_bit(UCSR0B, RXCIE0);
	TCCR0A = 0b01100011;
	TCCR0B = 0b00001011;
	OCR0A = 100;
	OCR0B = 50;
	sei();
    while (1) 
    {
		if (NewFromSerial == 1)
		{
			NewFromSerial = 0;
			velocity = (int)((recived_str[1]-'0')*10) + (int)(recived_str[2]-'0');
			if(recived_str[0] == 'L')
			{
				OCR0B = velocity;
			}
			else if (recived_str[0]= 'D')
			{
				OCR0B = 0;
			}
			
		}	
		_delay_ms(10);
    }
}

