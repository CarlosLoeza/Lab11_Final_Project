

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

unsigned long patterns[] = {0x02, '0x01', '0x04', '0x01', '0x02'};
int pattern_i = 0;
unsigned char pattern_count = 0;

unsigned char s_row = 0xFE;

enum Falling_States {shift};
int Falling_Object(int state) {
    
    // Local Variables
    unsigned char pattern = patterns[pattern_i];        // LED pattern - 0: LED off; 1: LED on
    static unsigned char row = 0xFE;    // Row(s) displaying pattern.
                        
    // 0: display pattern on row   $
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
            if (s_row != 0xEF){
                s_row = ~s_row;
                s_row = s_row << 1;
                s_row = ~s_row;

            } else {
                row = 0xFE;
                if(pattern_i == 5)
                    pattern_i =0;
                else
                    pattern_i++;
            }
            break;

        default:
            break;
    }
    PORTC = pattern;    // Pattern to display
    PORTD = s_row;                // Row(s) displaying pattern
    return state;

}




//unsigned char set_row;
enum Joystick_States {Joystick_Start, Joystick_Wait, Joystick_Right, Joystick_Left} Joystick_State;

void Joystick(){

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
            Joystick_State = Joystick_Wait;
            break;
    }

    // Action
    switch(Joystick_State){
        // Shift column to the right
        case Joystick_Right:
            if(set_pattern == 0x01 && button == 3)
            set_pattern = 0x80;
        else if(set_pattern != 0x01 && button == 3)
                set_pattern = (set_pattern >> 1);
            break;
        // Shift column to the left
        case Joystick_Left:
            if (set_pattern == 0x80 && button == 4)
            set_pattern = 0x01;
        else if(set_pattern != 0x80 && button == 4)
                set_pattern = (set_pattern << 1);
            break;

        default:
            break;
    }
}

void A2D_init() {
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}


int main(void) {
    /* Insert DDR and PORT initializations */
    
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    DDRA = 0x00; PORTA = 0x0F;

    //set_row = 0xFE;
    //set_pattern = 0x80;
    Joystick_State = Joystick_Start;


    //static unsigned short x;

    int state = 0;

    TimerSet(500);
    TimerOn();
    A2D_init();

    /* Insert your solution below */
    while (1) {
        x = ADC;
        state = Falling_Object(state);
        //Joystick();
    
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}

