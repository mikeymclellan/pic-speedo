/*
 * 
 */
#include <htc.h>
#include <stdio.h>
#include <stdlib.h>
#include "system.h"

__CONFIG(CP_OFF & WDTE_OFF & BOREN_OFF & MCLRE_OFF & PWRTE_ON & FOSC_INTOSCIO & LVP_OFF);

#define SPEEDO_MULTIPLIER 5.4
#define SPEEDO_MOTOR_STEP_DELAY 4 // 2 minumum seems to work
#define ODOMETER_MOTOR_STEP_DELAY 10
#define FILTER_FRACTION 0.7

/*
 * Local function declarations
 */
void delay_ms(unsigned int ui_value);
void interrupt isr(void);
void set_step(unsigned int speed);

/*
 * Globals
 */
unsigned int speedo_step_number;
unsigned int revolution_period;
unsigned char trigger_count;
unsigned char odometer_trigger_count;

/*
 * 
 */
int main()
{    
    PORTA = 0;
    PORTB = 0;

    TRISA = 0b00010000;
    TRISB = 0b01000000;

    T1CON = 0b00110101; // Enable timer1 1:8 prescaler
    
    PIE1   = 0b00000001; // Enable timer1 interrupt
    INTCON = 0b10001000; // Enable portb interrupts    

    LED_RED = 1;

    // reset speedo
    speedo_step_number = 300;
    set_step(0);
    speedo_step_number = 0;

    trigger_count = 0;
    revolution_period = 0;
    
    while (1) {
        
        if (revolution_period) {
            // 522000
            set_step((unsigned int)(530000/(revolution_period/SPEEDO_MULTIPLIER)));
        } else {
            set_step(0);
        }

        if (odometer_trigger_count > 14) {
            PORTB = 0b00001000;
            delay_ms(ODOMETER_MOTOR_STEP_DELAY);
            PORTB = 0b00000010;
            delay_ms(ODOMETER_MOTOR_STEP_DELAY);
            PORTB = 0b00000100;
            delay_ms(ODOMETER_MOTOR_STEP_DELAY);
            PORTB = 0b00000001;
            delay_ms(ODOMETER_MOTOR_STEP_DELAY);
            PORTB = 0;

            odometer_trigger_count = 0;
        }
    }

    return (EXIT_SUCCESS);
}

void set_step(unsigned int step)
{
    unsigned int target_step_number = max(step, 12) - 12;

    while (speedo_step_number != target_step_number) {

        if (speedo_step_number < target_step_number) {
            speedo_step_number++;
        } else {
            speedo_step_number--;
        }
        
        PORTA = motor2_state_map[speedo_step_number%6];

        delay_ms(SPEEDO_MOTOR_STEP_DELAY);
    }
}

void interrupt isr(void)
{
    if (RBIF) {
        unsigned char b = PORTB;
        
        if (b & SENSOR_BITMASK) {

            odometer_trigger_count++;
            
            if (++trigger_count >= 4) {

                // There's been 4 triggers (one revolution), read the timer
                union timer timer;

                timer.bt[0] = TMR1L;    // Read Lower byte
                timer.bt[1] = TMR1H;    // Read upper byte

                TMR1L = 0;
                TMR1H = 0;

                // Single pole low pass filter
                revolution_period = revolution_period + (FILTER_FRACTION * ((signed short long)timer.lt - revolution_period));

                trigger_count = 0;
            }

            if (LED_RED) {
                LED_RED = 0;
            } else {
                LED_RED = 1;
            }
        }
        
        RBIF = 0; // Clear interrupt
    }

    if (TMR1IF == 1) {

        // Timer1 has overflowed, reset and start timing again.
        trigger_count = 0;
        revolution_period = 0;

        TMR1L = 0;
        TMR1H = 0;

        TMR1IF = 0; // Clear interrupt
    }
}

/*
 * Delay in milliseconds.
 *
 */
void delay_ms(unsigned int ui_value)
{
    while (ui_value-- > 0) {
        __delay_ms(1);
    }
}
