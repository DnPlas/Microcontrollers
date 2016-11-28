// ---------- INCLUDES ----------
#include <stdio.h>
#include <xc.h>
#include <pic18f25k80.h>
#pragma config XINST= OFF

// ---------- DEFINES ----------
#define RAIN_S_CH 2
#define HUMIDITY_S_CH 1
#define TEMP_S_CH 3
#define COUNT_A_DAY 43200000
#define WATER_PUMP RC3
#define E_VALVE_1 RC2
#define E_VALVE_2 RC4
#define W_MOTOR_I1 RC5
#define W_MOTOR_I2 RB5
#define TOP_LIMIT_SWITCH RB4
#define BOT_LIMIT_SWITCH RB3

// ---------- VARIABLES ----------
unsigned long counter = 15000; //COUNT_A_DAY;
unsigned char mode_select = 0;
unsigned char emergency_stop = 0;

// ---------- PROTOTYPES ----------
void adc_init(void);
void gpios_init(void);
void serial_init(void);
void timer_init(void);
void watering_routine(unsigned char valve_select);
void auto_routines(void);
void bt_signal_handler(void);
void motor_routine(unsigned char open, unsigned char close);
int rain_sensor_data(void);
int temp_sensor_data(void);
int adc_read(unsigned char channel);

// ---------- MAIN ---------- 
void main(){
    IPEN= 1;
    GIEH=1;
    adc_init();
    gpios_init();
    serial_init();
    timer_init();
    BOT_LIMIT_SWITCH = 1;
    while(1){
        if (emergency_stop == 0 && mode_select == 0){
            auto_routines();
            motor_routine(0,0);
            bt_signal_handler();
        }
        else if (emergency_stop == 0 && mode_select == 1){
                bt_signal_handler();   
            }
        else{
            asm ("nop");
            }
        }
    }

// ---------- FUNCTIONS ----------
void adc_init(void){
    ADCON2 = 0b00001100;
}

void gpios_init(void){
    TRISC = 0b11000000;
    TRISB = 0b00011000;
    PORTC = 255;
    PORTB = 255;
}

void serial_init(void){
  RC1IF = 0;
  TXSTA1 = 0b00100000;
  RCSTA1 = 0b10010000;
  BAUDCON1 = 0;
  SPBRG = 12;
}

void timer_init(void){
      T2CON = 0b00000110;
}

void watering_routine(unsigned char valve_select){
    switch (valve_select){
        case 1:
            E_VALVE_1 =! E_VALVE_1;
            WATER_PUMP =! WATER_PUMP;
            break;
        case 2:
            E_VALVE_2 =! E_VALVE_2;
            WATER_PUMP =! WATER_PUMP;
            break;
    }
}

void motor_routine(unsigned char open, unsigned char close){
    if (mode_select == 0){
        if ((rain_sensor_data() == 1) || (temp_sensor_data() < 150)){
            W_MOTOR_I1 = 1;
            W_MOTOR_I2 = 0;
            while(TOP_LIMIT_SWITCH =! 0);
        }
        else if(temp_sensor_data() > 200){
            W_MOTOR_I1 = 0;
            W_MOTOR_I2 = 1;
            while(BOT_LIMIT_SWITCH =! 0);
        }
        else{
            W_MOTOR_I1 = 0;
            W_MOTOR_I2 = 0;
    }
    }
    else if (mode_select == 1){
        if (open == 1){
            W_MOTOR_I1 = 0;
            W_MOTOR_I2 = 1;
            while(BOT_LIMIT_SWITCH =! 0);
        }
        else if (close == 1){
            W_MOTOR_I1 = 1;
            W_MOTOR_I2 = 0;
            while(TOP_LIMIT_SWITCH =! 0);
        }
        else{
            W_MOTOR_I1 = 0;
            W_MOTOR_I2 = 0;  
        }
    }

}

void bt_signal_handler(void){
    if(RC1IF==1){
        switch (RCREG1){
        case 1:
            TXREG1 = rain_sensor_data();
            break;
        case 2:
            TXREG1 = temp_sensor_data();
            break;
        case 3:
            mode_select = 1;
            gpios_init();
            break;
        case 4:
            motor_routine(1,0);
            break;
        case 5:
            motor_routine(0,1);
            break;
        case 6:
            watering_routine(1);
            break;
        case 7:
            watering_routine(2);
            break;
        case 8:
            TXREG1 = hum_sensor_data();
            break;
        case 9:
            mode_select = 0;
            break;
        case 10:
            gpios_init();
            emergency_stop = 1;
            break;
        case 11:
            gpios_init();
            emergency_stop = 0;
            break;
        }
   }   
}

void auto_routines(void){
    if (TMR2IF == 1){
        TMR2IF = 0;
        counter--;
        switch (counter){
            case 14999:
                watering_routine(1);
                break;
            case 12500:
                watering_routine(1);
                watering_routine(2);
                break;
            case 10000:
                watering_routine(2);
                break;
            case 0:
                counter = 15000; //COUNT_A_DAY;
//            case 42750000:
//                watering_routine(1);
//                break;
//            case 40950000:
//                watering_routine(1);
//                watering_routine(2);
//                break;
//            case 39150000:
//                watering_routine(2);
//                break;
//            case 0:
//                counter = COUNT_A_DAY;
        }
    }
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

int rain_sensor_data(void){
    if (adc_read(RAIN_S_CH)<= 120)
           return 1;
       else
           return 0;
}

int temp_sensor_data(void){
    return adc_read(TEMP_S_CH);
}

int hum_sensor_data(void){
    return adc_read(HUMIDITY_S_CH);
}