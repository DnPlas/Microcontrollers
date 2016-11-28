// ---------- INCLUDES ----------
#include <stdio.h>
#include <xc.h>
#include <pic18f25k80.h>
#pragma config XINST= OFF

// ---------- DEFINES ----------
#define RAIN_SENSOR_CH 2

// ---------- PROTOTYPES ----------
void adc_init(void);
void gpios_init(void);
void serial_init(void);
void rain_notification(void);
int adc_read(unsigned char channel);

// ---------- MAIN ---------- 
void main (){
    adc_init();
    gpios_init();
    serial_init();
    while(1){
    }
}

// ---------- FUNCTIONS ----------
void adc_init(void){
    ADCON2 = 0b00001100;
}

void gpios_init(void){
    //TRISC = 0;
    TRISC = 0b11100000;
}

void serial_init(void){
  RC1IE = 1;
  RC1IF = 0;
  TXSTA1 = 0b00100000;
  RCSTA1 = 0b10010000;
  BAUDCON1 = 0;
  SPBRG = 12;
}

   
int adc_read(unsigned char channel){
    if (channel > 13) return 0;
    ADCON0 = 0x00;
    ADCON0 = (channel<<2);
    ADON = 1;
    GODONE = 1;
    while (GODONE);
    ADON = 0;
    return ADRESH;
}

void rain_notification(void){
   unsigned char rain_sensor_val;
   rain_sensor_val  = adc_read(RAIN_SENSOR_CH);
   if((RC1IF==1) && (RC1IE == 1)){
       if (RCREG1 = 1)
       {
           if (rain_sensor_val <= 120)
               TXREG1 = 1;
           else
               TXREG1 = 0;
       }
   }
}
