#include <xc.h>
#include <pic18f25k80.h>
#pragma CONFIG XINST = OFF

void interrupt second(seconds){
    if ((TMR0IF == 1) && (TMR0IE == 1)){
        TMR0IF = 0;
        segundos--;
    }
}

void interrupt minute(minutes){
    if ((TMR0IF == 1) && (TMR0IE == 1)){
        TMR0IF = 0;
        second(minutes*60);
    }
}

void interrupt hour(hours){
    if ((TMR0IF == 1) && (TMR0IE == 1)){
        TMR0IF = 0;
        minute(hours*60);
    }
}

void main (void){
    TRISC = 0; // Sets PORTC as outputs
    T0CON = 0b11000101; // Timer0 control register
    TMR0L = 6; // Timer0 starts with a count of 6
    
    IPEN = 1; // Enable interruptions
    TMR0IE = 1; // Enable TMR0 overflow timer
    
    while(1){
    }    
}
