/**************************************************************************************************
 Name        : INTERFACING_PROJECT.c
 Author      : Hassan Sabry Ahmed Morsy Shahin
 Facebook	 : Hassan Sabry
 link		 : https://www.facebook.com/hassan.sabry.14/
 whatsapp	 : 01111309309
 Description : Stop Watch Project

 **************************************************************************************************/


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char TICK=0;//global variable contain the ticks count of the timer
unsigned char i=0;//global variable Used for looping
unsigned char t[6];//global array used to desplay 6 7-segment numbers


ISR(TIMER1_COMPA_vect){

	TICK++; // increament the tick value

	if(TICK==1){

		t[0]++;//increment value of the first number

		if(t[0]>9){//check the first 7-segment overflow(second overflow)

			t[0]=0;
			t[1]++;//increment 2nd 7-segment for second

			if(t[1]>5){//check the 2nd 7-segment overflow(second overflow) to increment mins

				t[1]=0;
				t[2]++;//increment 1st 7-segment for mins

			}
		}


		if(t[2]>9){//check the third 7-segment overflow(1st mins overflow) to increment mins

			t[2]=0;
			t[3]++;//increment  7-segment for 2nd mins

			if(t[3]>5){//check mins overflow to increment hours

				t[3]=0;
				t[4]++;//increment  7-segment for 1st hour
			}
		}

		if(t[4]>9){//check fifth 7-segment overflow(hours)
			t[4]=0;
			t[5]++;
		}

		if(t[5]==1){//check overflow of the hours
			if(t[4]>2){
				for(i=0;i<6;i++){//reset all values after hours overflow
					t[i]=0; //desplay t[0]-->t[5]=0
				}
			}
		}
		TICK=0;//reset tick counts to increament it again
	}
}


ISR(INT0_vect){

	PORTC&=0xF0;//reset 7-segment values

	for(i=0;i<6;i++){//desplay t[0]-->t[5]=0
		t[i]=0;
	}
}
ISR(INT1_vect){

	TIMSK&=~(1<<OCIE1A);//disable timer1 compare interrupt

}
ISR(INT2_vect){

	TIMSK|=(1<<OCIE1A);//enable timer1 compare interrupt

}

void timer_1(void){

	TCCR1B=(1<<CS10)|(1<<CS12)|(1<<WGM12);
	/* TCCR1B TO CONTROL TIMER OF REGISTER B
	 * CS10/CS12 TO ENABL 1024 PRESCALER
	 * WGM12 TO USE TIMER FOR COMPARE
	 */

	TCNT1=0;//SET INTIAL VALUE =0

	OCR1A=1000;//SET COMPARE VALUE

	TIMSK|=(1<<OCIE1A);//ENABLE TIMER A COMPARE INTERRUPT MASK

}

/* Fcpu=1MHZ,N(PRESCALER)=1024==>
 * Ftime=Fcpu/n=1 khz
 * Ttimer=1/freq= 1 msec==>1000 SEC
 */


void int_0(void){

	DDRD  &= ~(1<<PD2);// Configure INT0/PD2 as input pin

	PORTD|=(1<<PD2);// Activate the internal pull up resistor at PD2

	MCUCR|=(1<<ISC01);//ENABLE FALLING EDGE

	GICR|=(1<<INT0);//Enable external interrupt pin INT0

}
void int_1(void){

	DDRD  &= ~(1<<PD3);// Configure INT1/PD3 as input pin

	MCUCR|=(1<<ISC10)|(1<<ISC11);//ENABLE RISING EDGE

	GICR|=(1<<INT1);//Enable external interrupt pin INT1

}
void int_2(void){

	DDRB  &= ~(1<<PB2);// Configure INT2/PB2 as input pin

	PORTB|=(1<<PB2);// Activate the internal pull up resistor at PB2

	MCUCSR&=~(1<<ISC2);//ENABLE FALLING EDGE

	GICR|=(1<<INT2);//Enable external interrupt pin INT2

}


int main(void){

	DDRA|=0x3F;// FIRST 5 PINS IN PORTA ARE OUTPUT

	DDRC|=0x0F;//FIRST 4 PINS IN PORTC ARE OUTPUT

	PORTC&=0xF0;//7-SEGMENT DESPLAY 0 AT FIRST

	SREG|=(1<<7);//ENABLE I-BIT

	timer_1();//call timer 1 function

	int_0();//call interrupt0 function

	int_1();//call interrupt1 function

	int_2();//call interrupt2 function

	while(1){

		for(i=0;i<6;i++){//LOOP TO DESPLAY 7-SEGMENT

			PORTA=(PORTA&0xC0)|(1<<i);//display 7-segment 0->5

			PORTC=(PORTC&0xF0)|(t[i]&0x0F);//display numbers on 7-segment

			_delay_ms(2);//to display all 7-segment at the same time

		}
	}
}
