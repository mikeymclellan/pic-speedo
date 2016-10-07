/* 
 * File:   system.h
 * Author: mikey
 *
 * 1000000 * (1/50kph) = 20,000
 *
 * 1/(speed /(_XTAL_FREQ/4)) = 50
 * Created on August 12, 2013, 8:31 PM
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#ifdef	__cplusplus
extern "C" {
#endif

#define	_XTAL_FREQ	              4000000   // Crystal Hz.
#define	INSTRUCTION_CYCLE_FREQ	  1000000   // Instruction cycle frequency is _XTAL_FREQ/4
#define TIMER_PRESCALE_VALUE      8

#define PI                        3.1416
#define WHEEL_DIAMETER            0.6269    // In metres
#define STEPS_PER_KPH             3.2       // The number of motor steps to cover for one Kph
#define ZERO_OFFSET_KPH           8         // The speedo dial doesn't start at 0Kph, it starts at 8Kph
#define SPEEDO_MOTOR_STEP_DELAY   6         // Delay between changing motor inputs in ms. 2 minumum seems to work.
#define ODOMETER_MOTOR_STEP_DELAY 10        // Delay between changing motor inputs in ms.
#define FILTER_FRACTION           0.4       // Low pass filter fraction when calculating the revolution_period

// IO config
#define SENSOR          RB6 // Putting the sensor on RB6 was a bad idea because it's shared with one of the ICSP pins, use RB5 instead
#define SENSOR_BITMASK  0b01000000
#define MOTOR_A         RB3
#define MOTOR_B         RB2
#define MOTOR_C         RB1
#define MOTOR_D         RB0
#define MOTOR2_A        RA0
#define MOTOR2_B        RA2
#define MOTOR2_C        RA3
#define MOTOR2_D        RA6
#define LED_RED         RA1
#define BUTTON          RB5
#define BUTTON_BITMASK  0b00100000

const unsigned char motor1_state_map[] = {
          0b00001000
        , 0b00000010
        , 0b00000100
        , 0b00000001};

// PORTA  6 3 2 0
// State  D C B A
// 0 AD   1 0 0 1
// 1 A    0 0 0 1
// 2 C    0 1 0 0
// 3 BC   0 1 1 0
// 4 B    0 0 1 0
// 5 D    1 0 0 0

const unsigned char motor2_state_map[] = {
          0b01000001
        , 0b00000001
        , 0b00001000
        , 0b00001100
        , 0b00000100
        , 0b01000000};

union timer
{
   unsigned int lt;
   char bt[2];
};

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_H */

