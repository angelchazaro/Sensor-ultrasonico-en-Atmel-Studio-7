/*
 * Sensor ultrasonico verult.c
 *
 * Created: 28/05/2018 11:12:32 p. m.
 * Author : dx_ch
 */ 

/*
 * Sensor ultrasonico medida con LCD.c
 *
 * Created: 18/05/2018 12:23:38 a. m.
 * Author : dx_ch
 */ 
#define F_CPU 8000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"			//Rutinas del LCD para el LCD de 20x4.
#define trigger_on	PC0		//Macro para el pin 0 del puerto C.

int TimerSobreFlujo=0;

int main(void)
{
	char cadena[10];		//Variable tipo caracter con un arreglo de 10 posiciones.
	long contador;			//Variable contador para convertir el tiempo del Timer 1.
	double distancia;		//Mostrar dos decimales
	
	DDRC|= 0x01;			//Se habilita el puerto C con OR en el pin 0.
	DDRD=0xff;				//Se habilita el puerto D completo.
	DDRB=0x02;				//Se habilita el pin PB1 para el RS del LCD.
	Lcd4_Init();			//Inicia LCD.
	Lcd4_Clear();			//Se limpia LCD.
	
	TIMSK1= (1<<TOIE1);		//Máscara de interrupción del Timer 1 con un espacio recorrido a habilitar interrupción por sobreflujo
	TCCR1A=0;				//Control y registro A del Timer 1.
	sei();					//Se habilitan interrupciones.

    /* Replace with your application code */
    while (1)	//Lazo infinito.
    {
		PORTC|= (1<<trigger_on);		//Salida con OR a 1 espacio recorrido a la izquierda a la macro "trigger_on".
		_delay_us(10);					//Retardo de 10 microsegundos.
		PORTC&= (~(1<<trigger_on));		//Salida con AND negada a 1 espacio recorrido a la izquierda a la macro "trigger_on".
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
		
		dtostrf(distancia,2,2,cadena);			//Convierte números enteros a cadena de tipo char. 
		strcat(cadena, " cm   ");				//Concatenación de cadena.
		EscribeMEnsajeLCD4("Distancia = ", 1,0);
		EscribeMEnsajeLCD4(cadena,1,12);
		_delay_ms(200);
    }
}
ISR(TIMER1_OVF_vect)
{
	TimerSobreFlujo++;
}
