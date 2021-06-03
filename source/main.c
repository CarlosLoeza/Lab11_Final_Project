/*    Author: lab
 *  Partner(s) Name:
 *    Lab Section:
 *    Assignment: Lab #  Exercise #
 *    Exercise Description: [optional - include for your own benefit]
 *
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
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

unsigned char set_row;
unsigned char set_pattern;
unsigned char button;
unsigned char btn;
unsigned short x;

int i = 0;
unsigned char falling_row;
unsigned char falling_pattern;

enum Falling_States {shift};
int Falling_Object(int state) {
    
    unsigned temp_falling_row = falling_row;
    unsigned char temp_falling_pattern = falling_pattern; // joystick pattern


    // Local Variables
    static unsigned long patterns[] = {0x02, 0x80, 0x01};        // LED pattern - 0: LED off; 1: LED on

    // Transitions
    switch (state) {
        case shift:
            break;
        default:
            state = shift;
            break;
    }
    // Actions
    switch (state) {
        case shift:
        // if row != last row, go to next row
            if (temp_falling_row != 0xEF){
                temp_falling_row = ~temp_falling_row;
                temp_falling_row = temp_falling_row << 1;
                temp_falling_row = ~temp_falling_row;
            } else {
                temp_falling_row = 0xFE;
                if(i<3){ i++; } else {i = 0;}
            }
            break;

        default:
            break;
    }
    
    falling_pattern = patterns[i];
    falling_row = temp_falling_row;
    return state;

}




//unsigned char set_row;
enum Joystick_States {Joystick_Start, Joystick_Wait, Joystick_Right, Joystick_Left} Joystick_State;

unsigned joystick_row;
unsigned char joystick_pattern; // joystick pattern

void Joystick(){

    unsigned temp_joystick_row = joystick_row;
    unsigned temp_joystick_pattern = joystick_pattern;
    //x = ADC;
    // Transition
    switch(Joystick_State){
        case Joystick_Start:
            Joystick_State = Joystick_Wait;
            button = 0;
            break;
        case Joystick_Wait:
            if(x > 512 && x < 600){
                button = 0;
                Joystick_State = Joystick_Wait;
            }
 
        else if(x > 600){
                Joystick_State = Joystick_Right;
                button = 3;
            } else if(x < 512){
                Joystick_State = Joystick_Left;
                button = 4;
            }
            break;

    // if left or right
        case Joystick_Right:
        Joystick_State = Joystick_Wait;
        break;
        case Joystick_Left:
            Joystick_State = Joystick_Wait;
            break;

        default:
            Joystick_State = Joystick_Start;
            break;
    }

    // Action
    switch(Joystick_State){
        // Shift column to the right
        case Joystick_Right:
            if(temp_joystick_pattern == 0x07 && button == 3)
                temp_joystick_pattern = 0x07;
            else if(temp_joystick_pattern != 0x07 && button == 3)
                temp_joystick_pattern = (temp_joystick_pattern >> 1);
            break;
        // Shift column to the left
        case Joystick_Left:
            if (temp_joystick_pattern == 0xE0 && button == 4)
                temp_joystick_pattern = 0xE0;
            else if(joystick_pattern != 0xE0 && button == 4)
                temp_joystick_pattern = (temp_joystick_pattern << 1);
            break;

        default:
            break;
    }
    joystick_row = temp_joystick_row;
    joystick_pattern = temp_joystick_pattern;
}

enum LED_States{LED_Start, LED_Falling_Object, LED_Joystick} LED_State;
void LED_Display(){
    
    switch(LED_State){
        case LED_Start:
            LED_State = LED_Falling_Object;
            break;
        case LED_Falling_Object:
            LED_State = LED_Joystick;
            break;
        case LED_Joystick:
            LED_State = LED_Falling_Object;
            break;
        default:
            LED_State = LED_Start;
            break;
        }

        switch(LED_State){
            case LED_Falling_Object:
                PORTD = falling_row;
		PORTC = falling_pattern;
                break;
            case LED_Joystick:
//                PORTD = joystick_pattern;
//		PORTC = joystick_row;
                break;
        }
}
    
    


void A2D_init() {
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

             
int main(void) {
    DDRA = 0x00; PORTA = 0x0F;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    unsigned long timer = 10;
    unsigned long Falling_Object_timer = 1500;
    unsigned char falling_row = 0xFE;    // Row(s) displaying pattern.
    
    Joystick_State = Joystick_Start;
    joystick_row = 0xEF;
    joystick_pattern = 0xE0;
    unsigned long Joystick_timer = 400;
    

    LED_State = LED_Start;
    unsigned long LED_timer = 10;
    //static unsigned short x;

    int state = 0;

    TimerSet(timer);
    TimerOn();
    A2D_init();

    /* Insert your solution below */
    while (1) {
        
        x = ADC;
        if(Falling_Object_timer <= 1500){
            state = Falling_Object(state);
            Falling_Object_timer = 0;
        }
        if(Joystick_timer <= 400){
            Joystick();
            Joystick_timer = 0;
        }
        if(LED_timer <= 100){
            LED_Display();
	    LED_timer = 0;
	}
        while(!TimerFlag);
        TimerFlag = 0;
        Falling_Object_timer += timer;
        Joystick_timer += timer;
	LED_timer +=timer;
    }
    return 1;
    
}

