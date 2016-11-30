// ---------- INCLUDES ----------
#include <xc.h>
#include <pic18f25k80.h>
#pragma config XINST = OFF

// ---------- DEFINES ----------
#define NUT_PUMP RB4      
#define TOP_LIMIT_SWITCH RC4              
#define BOT_LIMIT_SWITCH RC2             
#define MIXER_M RC3         
#define VALVE RC5         
#define LINE_PUMP RB5

// ---------- VARIABLES ----------
unsigned int frequency;
unsigned int current_time;
unsigned int period;
unsigned long int period_cache=0;
unsigned int last_time =0;
unsigned char freq_count=0;
unsigned char total_volume;
unsigned char current_volume;

unsigned char it_a_day;
unsigned char conc;
unsigned char nut_conc;
unsigned int null_periods;

unsigned int counters;
unsigned long secs;
unsigned char mixing_time;


// ---------- PROTOTYPES ----------
void adc_init(void);
void gpios_init(void);
void serial_init(void);
void timer_init(void);
void cc_init(void);
void watering(void);
void mixer_init(void);
void fill_tank(void);
void nut_injection(void);
void mixing(void);
void bt_signal_handler(void);
void reset(unsigned long variable);

// ---------- MAIN ----------
void main(){
    IPEN= 1;
    GIEH=1;
    adc_init();
    gpios_init();
    serial_init();
    timer_init();
    cc_init();
    while(1){
    bt_signal_handler();
    if(BOT_LIMIT_SWITCH == 1){
            mixer_init();
            fill_tank();
            nut_injection();
            mixing();
        }
    else if (BOT_LIMIT_SWITCH == 0)
            watering();
    }
}

// ---------- FUNCTIONS ----------
void adc_init(void){
    ADCON2 = 0b00001100;
}

void gpios_init(void){
    TRISC = 0b00010100;
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
  RC1IE = 1;
  RC1IP = 1;
}

void timer_init(void){
      T2CON = 0b00000110;
      T3CON = 0b00100001;
      TMR3GE = 1;
      TMR2IE = 1;
}

void cc_init(void){
    CCP2IE = 1;
    CCP2IP = 1;
    C2TSEL = 1;
    CCP2CON=0b00000101;
}

void interrupt mezcladora(void){
   if((CCP2IF==1)&&(CCP2IE==1)){
        CCP2IF=0;
        current_time = (unsigned int)CCPR2H*256+(unsigned int)CCPR2L;
        period_cache = current_time-last_time+period_cache;
        freq_count++;
            if(freq_count>4){
                period = period_cache/5;
                frequency = 500000/(unsigned long)period;
                reset(period_cache);
                reset(freq_count);
                total_volume = (frequency*60)/288;
                current_volume+ = total_volume;
            }
            last_time = current_time;
    }


    if ((TMR2IF == 1)&&(TMR2IE==1)){
        TMR2IF = 0;
        while(counters++ > 999){
            reset(counters);
            mixing_time++;
        }
    }
}

void bt_signal_handler(void){
    if(RC1IF == 1){
        switch (RCREG1){
            case 1:
                if (RC1IF == 1){
                nut_conc = (RCREG1*10)/98;}
                break;
        
            case 2:
                if (RC1IF == 1){
                null_periods=(86400-(180*RCREG1))/RCREG1;}
                reset(counters);
                reset(secs);
                break;
        }
        }
    }

void mixer_init(void){
    reset(counters);
    reset(secs);
    NUT_PUMP = 1;
    LINE_PUMP = 1;
    VALVE = 1;
    MIXER_M = 1;
}

void fill_tank(void)    {
    reset(VALVE);
    while(TOP_LIMIT_SWITCH == 1);
    reset(VALVE);
}

void nut_injection(void){
    reset(NUT_PUMP);
    reset(total_volume);
    reset(current_volume);
    while(current_volume != nut_conc);
    reset(total_volume);
    reset(current_volume);
    NUT_PUMP = 1;
}

void mixing(void){
    reset(mixing_time);
    reset(MIXER_M);
    while (mixing_time < 5);
    MIXER_M =! MIXER_M;
}

void watering(void){
    reset(secs);
    reset(LINE_PUMP);
    while((secs <= 2)&&(BOT_LIMIT_SWITCH==0));
    reset(secs);
    LINE_PUMP =! LINE_PUMP;
    while((secs < null_periods)&&(BOT_LIMIT_SWITCH==0))
    reset(secs);

}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              

void reset(unsigned long variable){
    variable = 0;
}
