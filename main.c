/* 
 * Car Speedo Driver - Mikey McLellan 2014
 *
 * This is for reading a hall sensor to pick up axle revolutions and drive two stepper motors in an Datsun speedo
 * housing; one motor for the dial and one for the odometer. 
 *
 * The hall sensor is a Honeywell 1GT101DC and the two stepper motors are driven by two LD239 drivers.
 *
 * 245x45r16 tires are 0.6269m in diameter
 *
 * revolution_period          = (1000000 * (1/(kph/3.6)) * 3.14 * diameter)/TIMER_PRESCALE_VALUE
 * revolution_period @ 14Kph  = 63,272 (Max value for unsigned int is 65,536)
 * revolution_period @ 50Kph  = 17,716
 * revolution_period @ 100Kph = 8,858
 * revolution_period @ 200Kph = 4,429
 *
 * target_step_number         = ((INSTRUCTION_CYCLE_FREQ/revolution_period)*WHEEL_DIAMETER*PI*3.6/TIMER_PRESCALE_VALUE) * STEPS_PER_KPH
 * target_step_number @ 50Kph = ((1000000/17716) * 0.6269 * 3.14 * 3.6 / 8) * 3.2
 * target_step_number @ 50Kph = 160
 */
#include <htc.h>
#include <stdio.h>
#include <stdlib.h>
#include "system.h"

__CONFIG(CP_OFF & WDTE_OFF & BOREN_OFF & MCLRE_OFF & PWRTE_ON & FOSC_INTOSCIO & LVP_OFF);

/*
 * Local function declarations
 */
void interrupt isr(void);

/*
 * Globals
 */
// The number of instruction cycles for one revolution of the axle divided by 8 (8 = timer1 prescaler)
unsigned int revolution_period       = 0;
// The number of sensor triggers since the last full revolution
unsigned char trigger_count          = 0;
// The number of sensor triggers since the last time we moved the odomenter motor one revolution
unsigned char odometer_trigger_count = 0;

/*
 * 
 */
int main()
{
    unsigned int speedo_step_number = 300;
    unsigned int target_step_number;

    PORTA = 0;
    PORTB = 0;

    TRISA = BUTTON_BITMASK; // The button is an input
    TRISB = SENSOR_BITMASK; // The hall sensor is an input

    T1CON = 0b00110101;     // Enable timer1 1:8 prescaler
    
    PIE1   = 0b00000001;    // Enable timer1 interrupt
    INTCON = 0b10001000;    // Enable portb interrupts

    LED_RED = 1;

    // Reset the needle
    while (speedo_step_number) {
        PORTA = motor2_state_map[speedo_step_number--%6];
        __delay_ms(SPEEDO_MOTOR_STEP_DELAY);
    }
    
    while (1) {
        target_step_number = 0;
        
        if (revolution_period) {
            
            target_step_number = ((INSTRUCTION_CYCLE_FREQ/revolution_period)*WHEEL_DIAMETER*PI*3.6/TIMER_PRESCALE_VALUE) * STEPS_PER_KPH;
            target_step_number = max(target_step_number, ZERO_OFFSET_KPH*STEPS_PER_KPH) - ZERO_OFFSET_KPH*STEPS_PER_KPH;
        }

        if (speedo_step_number < target_step_number) {
                speedo_step_number++;
        }
        else if (speedo_step_number > target_step_number) {
            speedo_step_number--;
        }

        PORTA = motor2_state_map[speedo_step_number%6];

        __delay_ms(SPEEDO_MOTOR_STEP_DELAY);

        if (odometer_trigger_count > 14) {
            odometer_trigger_count = 0;
            
            PORTB = 0b00001000;
            __delay_ms(ODOMETER_MOTOR_STEP_DELAY);
            PORTB = 0b00000010;
            __delay_ms(ODOMETER_MOTOR_STEP_DELAY);
            PORTB = 0b00000100;
            __delay_ms(ODOMETER_MOTOR_STEP_DELAY);
            PORTB = 0b00000001;
            __delay_ms(ODOMETER_MOTOR_STEP_DELAY);
            PORTB = 0;
        }
    }

    return (EXIT_SUCCESS);
}

void interrupt isr(void)
{
    if (RBIF) {
        
        if (PORTB & SENSOR_BITMASK) {

            odometer_trigger_count++;
            
            if (++trigger_count >= 3) {

                // There's been 3 triggers (one revolution), read the timer
                union timer timer;

                timer.bt[0] = TMR1L;    // Read lower byte
                timer.bt[1] = TMR1H;    // Read upper byte

                TMR1L = 0;
                TMR1H = 0;

                // Anything less than 3500 (~250kph) will just be noise
                if (timer.lt > 3500) {
                    // Single pole low pass filter
                    revolution_period = revolution_period + (FILTER_FRACTION * ((signed short long)timer.lt - revolution_period));
                }
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

        // Timer1 overflowed, reset and start timing again
        trigger_count = 0;
        revolution_period = 0;

        TMR1L = 0;
        TMR1H = 0;

        TMR1IF = 0; // Clear interrupt
    }
}
