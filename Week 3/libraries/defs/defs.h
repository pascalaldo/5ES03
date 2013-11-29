#ifndef DEFS_H
#define DEFS_H

#define BOT_SERIAL 1
#define NO_DEBUG

/* 
 * 
 */
#include <math.h> //include math functions

#define	ID_ADC_SOUND     0
#define ID_ADC_LINE      2
#define ID_SPEED_R       5
#define ID_SPEED_L       6
#define ID_DIRECTION_R   7
#define ID_DIRECTION_L   8
#define	ID_LEFTRIGHT	12
#define	ID_FRONTREAR	13
#define	ID_ADC_DISTANCE  3

// Define some IDs
#define ID_ADC_LINE       2
// Same ID?
#define ID_ENCODER_R      2
#define ID_ENCODER_L      4
#define ID_SPEED_R        5
#define ID_SPEED_L        6

#define ID_DIRECTION_R    7
#define ID_DIRECTION_L    8
#define ID_LEFTRIGHT     12
#define ID_FRONTREAR     13

#define ID_LED_BLUE       9
// Try not to use the red LED, since it might interfere 
// with the line detection
#define ID_LED_RED       10
#define ID_LED_GREEN     11

#define DELAY_VAL	  0
#define DELAY_READ_LINE   1

// Motor corrections
#ifdef BOT_SERIAL
#define MOTOR_CORRECTION_L 255
#define MOTOR_CORRECTION_R 251
#else
#define MOTOR_CORRECTION_L 250
#define MOTOR_CORRECTION_R 255
#endif

// Should be made dependent on the bot nr.
#define CORRECTION_LEFT   1.0f
#define CORRECTION_RIGHT  1.0f

// Threshold of what is considered line (white)
#define LINE_THRESHOLD  300
// How severe the reaction to seeing the edge of the line is
#define LINE_FACTOR       2.0f
// How long the bot should 'recover' from being off track. This is a delay that
// makes sure the bot does not to quickly assume it is back on track.
#define RECOVERY_STEPS   50

#define MOTOR_SPEED     255.0f
// Speed fraction to turn around with
#define TURN_SPEED        1.0f

// Distance at which keepdistance should trigger.
#ifdef BOT_SERIAL
#define DIST_THRESHOLD  950
#define DIST_THRESHOLD_CRITICAL  200
#else
#define DIST_THRESHOLD   50
#define DIST_THRESHOLD_CRITICAL  10
#endif

// Clap thresholds
#ifdef BOT_SERIAL
#define CLAP_UPPER      400
#define CLAP_LOWER      100
#else
#define CLAP_UPPER      400
#define CLAP_LOWER      150
#endif

#define MOTOR_CORRECTION_DELAY 150

#endif
