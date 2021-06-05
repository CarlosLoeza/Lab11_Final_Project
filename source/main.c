
/*      Author: lab
 *  Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or examp$
 *      code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programme$

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start counter from here to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0; // Current internal clock of 1ms ticks

void TimerOn(){
    // AVR timer/counter controller register TCCR1
    TCCR1B = 0x0B;      // bit3 = 0: CTC mode(clear timer on compare)
                        // bit2bit1bit0 = 011;
                        // 0000 1011 : 0x0B
                        // So 8 MHz clock or 8,000,000 / 64  = 125,000 ticks
                        // Thus TCNT1 register will count 125,000 ticks

    // AVR out compare register OCR1A
    OCR1A = 125;        // Timer interrupt will be generated when TCNT1==OCR1A
                        // We want a 1ms tick. .001s * 125,000 ticks = 125
                        // So when TCNT1 == 125 then that means 1ms has passed

    // AVR timer register interrupt mask register
    TIMSK1 = 0x02;      // bit1: OCIE1A -- enables compare match register

    // Initiliaze AVR counter
    TCNT1 = 0;

    _avr_timer_cntcurr = _avr_timer_M;

    // enable global interrupts
    SREG |= 0x80;       // 1000 0000 
}

void TimerOff(){
    TCCR1B = 0x00;
}

void TimerISR(){
    TimerFlag = 1;
}

// In our approach C program does not touch this ISR, only TimerISR()
ISR(TIMER1_COMPA_vect){
    // CPU automatically calls when TCNT1 == OCR1 (Every 1ms per TimerOn settin$
    _avr_timer_cntcurr--;       // count down to 0
    if (_avr_timer_cntcurr == 0){
        TimerISR();     // call the ISR that the user uses
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

// Set timer to tick every M ms.
void TimerSet(unsigned long M){
    _avr_timer_M = M;
    _avr_timer_cntcurr =  _avr_timer_M;
}



// one
enum One_States {One_Start, One_col1} One_State;
void One(){
    switch(One_State){
        case One_Start:
            One_State = One_col1;
            break;
        default:
            One_State = One_Start;
            break;
    }
    
    
    switch(One_State){
        case One_col1:
            PORTC = 0x08;
            PORTD = 0x00;
            break;
        default:
            One_State = One_Start;
            break;
    }
}



// two
enum Two_States {Two_Start, Two_col1, Two_col2, Two_col3} Two_State;
void Two(){
    switch(Two_State){
        case Two_Start:
            Two_State = Two_col1;
            break;
        case Two_col1:
            Two_State = Two_col2;
            break;
        case Two_col2:
            Two_State = Two_col3;
            break;
        case Two_col3:
            Two_State = Two_col1;
            break;
        default:
            Two_State = Two_Start;
            break;
    }
    
    
    switch(Two_State){
        case Two_col1:
            PORTC = 0x10;
            PORTD = 0x02;
            break;
        case Two_col2:
            PORTC = 0x08;
            PORTD = 0x0A;
            break;
        case Two_col3:
            PORTC = 0x04;
            PORTD = 0x08;
            break;
    }
}


            

// three
enum Three_States {Three_Start, Three_col1, Three_col2, Three_col3} Three_State;
void Three(){
    switch(Three_State){
        case Three_Start:
            Three_State = Three_col1;
            break;
        case Three_col1:
            Three_State = Three_col2;
            break;
        case Three_col2:
            Three_State = Three_col3;
            break;
        case Three_col3:
            Three_State = Three_col1;
            break;
        default:
            Three_State = Three_Start;
            break;
    }
    
    
    switch(Three_State){
        case Three_col1:
            PORTC = 0x10;
            PORTD = 0x0A;
            break;
        case Three_col2:
            PORTC = 0x08;
            PORTD = 0x0A;
            break;
        case Three_col3:
            PORTC = 0x04;
            PORTD = 0x00;
            break;
    }
}



// four
enum Four_States {Four_Start, Four_col1, Four_col2, Four_col3} Four_State;
void Four(){
    switch(Four_State){
        case Four_Start:
            Four_State = Four_col1;
            break;
        case Four_col1:
            Four_State = Four_col2;
            break;
        case Four_col2:
            Four_State = Four_col3;
            break;
        case Four_col3:
            Four_State = Four_col1;
            break;
        default:
            Four_State =Four_Start;
            break;
    }
    
    
    switch(Four_State){
        case Four_col1:
            PORTC = 0x10;
            PORTD = 0x18;
            break;
        case Four_col2:
            PORTC = 0x08;
            PORTD = 0x1B;
            break;
        case Four_col3:
            PORTC = 0x04;
            PORTD = 0x00;
            break;
    }
}


// five

enum Five_States {Five_Start, Five_col1, Five_col2, Five_col3} Five_State;
void Five(){
    switch(Five_State){
        case Five_Start:
            Five_State = Five_col1;
            break;
        case Five_col1:
            Five_State = Five_col2;
            break;
        case Five_col2:
            Five_State = Five_col3;
            break;
        case Five_col3:
            Five_State = Five_col1;
            break;
        default:
            Five_State = Five_Start;
            break;
    }
    
    
    switch(Five_State){
        case Five_col1:
            PORTC = 0x10;
            PORTD = 0x08;
            break;
        case Five_col2:
            PORTC = 0x04;
            PORTD = 0x02;
            break;
        case Five_col3:
            PORTC = 0x08;
            PORTD = 0x0A;
            break;
    }
}


// six

enum Six_States {Six_Start, Six_col1, Six_col2, Six_col3} Six_State;
void Six(){
    // transitions
    switch(Six_State){
        case Six_Start:
            Six_State = Six_col1;
            break;
        case Six_col1:
            Six_State = Six_col2;
            break;
        case Six_col2:
            Six_State = Six_col3;
            break;
        case Six_col3:
            Six_State = Six_col1;
            break;
        default:
            Six_State = Six_Start;
            break;
    }
    // actions
    switch(Six_State){
        case Six_col1:
            PORTC = 0x10;
            PORTD = 0x00;
            break;
        case Six_col2:
            PORTC = 0x08;
            PORTD = 0x0A;
            break;
        case Six_col3:
            PORTC = 0x04;
            PORTD = 0x02;
            break;
    }
}


// seven
enum Seven_States {Seven_Start, Seven_col1, Seven_col2} Seven_State;
void Seven(){
    switch(Seven_State){
        case Seven_Start:
            Seven_State = Seven_col1;
            break;
        case Seven_col1:
            Seven_State = Seven_col2;
            break;
        case Seven_col2:
            Seven_State = Seven_col1;
            break;
        default:
            Seven_State = Seven_Start;
            break;
    }
    
    
    switch(Seven_State){
        case Seven_col1:
            PORTC = 0x1C;
            PORTD = 0x1E;
            break;
        case Seven_col2:
            PORTC = 0x04;
            PORTD = 0x00;
            break;
    }
}



enum Hello_States{Hello_Start, Hello_1, Hello_2, Hello_3} Hello_State;
void Hello(){ 
    switch(Hello_State){
	case Hello_Start:
	    Hello_State = Hello_1;
	    break;
	case Hello_1:
	    Hello_State = Hello_2;
            break;
	case Hello_2:
	    Hello_State = Hello_3;
	    break;
	case Hello_3:
	    Hello_State = Hello_1;
	    break;
	default:
	    Hello_State = Hello_Start;
	    break;
    }

    switch(Hello_State){
	case Hello_1:
	    PORTC = 0x10;
	    PORTD = 0x18;
	    break;
	case Hello_2:
            PORTC = 0x08;
	    PORTD = 0x1B;
            break;
	case Hello_3:
	    PORTC = 0x04;
	    PORTD = 0x00;
	    break;
    }
}


int main(void) {
    /* Insert DDR and PORT initializations */
    
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    DDRA = 0x00; PORTA = 0x0F;
    int count = 6;

    TimerSet(1);
    TimerOn();
    /* Insert your solution below */
    while (1) {
	//btn = ~PINA & 0x0F;
        //Calculate();
	//state = Demo_Tick(state);
	
//	Hello();	

	switch(count){
	    case 1:
		One();
		break;
	    case 2:
		Two();
		break;
	    case 3:
		Three();
		break;
	    case 4:
		Four();
		break;
	   case 5:
		Five();
		break;
	   case 6:
		Six();
		break;
	   case 7:
		Seven();
		break;
/*
	   case 8:
		Eight();
		break;
	   case 9:
		Nine();
		break;
	   case 10:
		Ten();
		break;
*/
	   default:
		break;
	}
	
	while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}




