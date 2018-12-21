/*
 * Sensor ultrasonico 8.c
 *
 * Created: 20/12/2018 08:41:40 p. m.
 * Author : dx_ch
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"			//Rutinas del LCD para el LCD de 20x4.

int TimerSobreFlujo=0;
char cadena[10];		//Variable tipo caracter con un arreglo de 10 posiciones.
long contador;			//Variable contador para convertir el tiempo del Timer 1.
double distancia;		//Mostrar dos decimales

int main(void)
{	
	DDRC|= 0x01;			//Se habilita el puerto C con OR en el pin 0.
	DDRD=0xff;				//Se habilita el puerto D completo.
	DDRB=0x20;
	TIMSK1= (1<<TOIE1);		//Máscara de interrupción del Timer 1 con un espacio recorrido a habilitar interrupción por sobreflujo
	TCCR1A=0;				//Control y registro A del Timer 1.
	
	Lcd4_Init();			//Inicia LCD.
	Lcd4_Clear();			//Se limpia LCD.
	
	EscribeMEnsajeLCD4("Distancia = ", 1,0);
	
	sei();					//Se habilitan interrupciones.

	/* Replace with your application code */
	while (1)	//Lazo infinito.
	{
		PORTC|= 0x01;	//Salida con OR a 1 espacio recorrido a la izquierda.
		_delay_us(10);	//Retardo de 10 microsegundos.
		PORTC&= ~0x01;	//Salida con AND negada a 1 espacio recorrido a la izquierda.
		_delay_us(2);
		
		TCNT1=0;						//Contador del Timer 1 a cero.
		TCCR1B=0x41;					//65 decimal
		TIFR1=1<<ICF1;					//Registro del Timer 1 para la interrupción de bandera		//ICP1=PB0 para el atmega328p;
		TIFR1=1<<TOV1;					//Registro del Timer 1 para la interrupción de bandera.
		
		while((TIFR1 & (1<<ICF1))==0);
		TCNT1=0;
		TCCR1B= 0x01;
		TIFR1=1<<ICF1;
		TIFR1=1<<TOV1;
		TimerSobreFlujo=0;
		
		while((TIFR1 & (1<<ICF1))==0);
		contador= ICR1+(65535*TimerSobreFlujo);	//Contador igual al registro de comparación de entrada más el valor completo de
		//16 bits del Timer 1 por la variable TimerSobreFlujo que va en incremento en la interrupción.
		distancia=(double)contador/932.94;		//Valores probados para la distancia total que da el sensor ultrasónico y luego dividido entre 2.		//846;//932.94;	//932.94;	//466.47;
		_delay_ms(200);
		EscribeNumEnteroLCD2(distancia,1,12);
		
		if (distancia<=27)
		{
			PORTB|=0x20;
		}
		else
		{
			PORTB&=~0x20;
		}
	}
}
ISR(TIMER1_OVF_vect)
{
	TimerSobreFlujo++;
}

