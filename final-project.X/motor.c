/*
 * File:   motor.c
 * Author: chehj
 *
 * Created on December 1, 2024, 8:53 PM
 */

#include "motor.h"

// Define motor pulse states for readability and state management
#define PULSE_LEFT    0x01
#define PULSE_MIDDLE  0x02
#define PULSE_RIGHT   0x04
#define PULSE_CLOSER  0x08
#define PULSE_FURTHER 0x10
#define PULSE_ARRIVED 0x20
#define PULSE_DEST_FARTHER 0x40
#define PULSE_DEST_CLOSER 0x80

/**
 * @brief Initialize motor control pins as outputs.
 * Sets the appropriate pins for left, middle, and right motors as outputs
 * to enable control.
 */
void initMotors() {
    PORTA.DIR |= LEFT_MOTOR;   // Set the left motor pin as output
    PORTA.DIR |= MIDDLE_MOTOR; // Set the middle motor pin as output
    PORTA.DIR |= RIGHT_MOTOR;  // Set the right motor pin as output
}

/**
 * @brief Clear motor states.
 * Turns off all motors by resetting their output pins.
 */
void clearMotors() {
    PORTA.OUT &= ~LEFT_MOTOR;   // Turn off the left motor
    PORTA.OUT &= ~MIDDLE_MOTOR; // Turn off the middle motor
    PORTA.OUT &= ~RIGHT_MOTOR;  // Turn off the right motor
}

/**
 * @brief Pulse the left motor with specific timing logic.
 * Alternates between activating the left and middle motors based on the 
 * value of `secondCounter` for 3 pulses.
 */
void pulseLeft() {
    if (pulseCounter < 3) {
        if (secondCounter == 1) {
            PORTA.OUT &= ~LEFT_MOTOR;
            PORTA.OUT |= MIDDLE_MOTOR;  // Activate middle motor
            PORTA.OUT &= ~RIGHT_MOTOR;
        } else if (secondCounter == 2) {
            PORTA.OUT |= LEFT_MOTOR;   // Activate left motor
            PORTA.OUT &= ~MIDDLE_MOTOR;
            PORTA.OUT &= ~RIGHT_MOTOR;
        }
    } else {
        pulseCounter = 0;             // Reset pulse counter
        statesActive &= ~PULSE_LEFT;  // Clear the pulse left state
        clearMotors();
    }
}

/**
 * @brief Pulse the middle motor with specific timing logic.
 * Alternates the middle motor activity, turning it on or off for 3 pulses.
 */
void pulseMiddle() {
    if (pulseCounter < 3) {
        if (secondCounter == 1) {
            PORTA.OUT &= ~LEFT_MOTOR;
            PORTA.OUT |= MIDDLE_MOTOR; // Activate middle motor
            PORTA.OUT &= ~RIGHT_MOTOR;
        } else if (secondCounter == 2) {
            PORTA.OUT &= ~LEFT_MOTOR;
            PORTA.OUT &= ~MIDDLE_MOTOR; // Deactivate middle motor
            PORTA.OUT &= ~RIGHT_MOTOR;
        }
    } else {
        pulseCounter = 0;              // Reset pulse counter
        statesActive &= ~PULSE_LEFT;   // Clear the pulse left state
        clearMotors();
    }
}

/**
 * @brief Pulse the right motor with specific timing logic.
 * Alternates between activating the right and middle motors for 3 pulses.
 */
void pulseRight() {
    if (pulseCounter < 3) {
        if (secondCounter == 1) {
            PORTA.OUT &= ~LEFT_MOTOR;
            PORTA.OUT |= MIDDLE_MOTOR; // Activate middle motor
            PORTA.OUT &= ~RIGHT_MOTOR;
        } else if (secondCounter == 2) {
            PORTA.OUT &= ~LEFT_MOTOR;
            PORTA.OUT &= ~MIDDLE_MOTOR;
            PORTA.OUT |= RIGHT_MOTOR; // Activate right motor
        }
    } else {
        pulseCounter = 0;              // Reset pulse counter
        statesActive &= ~PULSE_LEFT;   // Clear the pulse left state
        clearMotors();
    }
}

/**
 * @brief Pulse motors in a "closer" pattern.
 * Alternates between activating the left/right motors together and the middle motor alone.
 */
void pulseCloser() {
    if (pulseCounter < 3) {
        PORTA.OUT |= LEFT_MOTOR;      // Activate left motor
        PORTA.OUT &= ~MIDDLE_MOTOR;   // Deactivate middle motor
        PORTA.OUT |= RIGHT_MOTOR;     // Activate right motor
    } else if (pulseCounter == 3) {
        PORTA.OUT &= ~LEFT_MOTOR;     // Deactivate left motor
        PORTA.OUT |= MIDDLE_MOTOR;    // Activate middle motor
        PORTA.OUT &= ~RIGHT_MOTOR;    // Deactivate right motor
    } else {
        pulseCounter = 0;             // Reset pulse counter
        statesActive &= ~PULSE_LEFT;  // Clear the pulse left state
        clearMotors();
    }
}

/**
 * @brief Pulse motors in a "further" pattern.
 * Alternates between activating the middle motor and the left/right motors together.
 */
void pulseFurther() {
    if (pulseCounter < 3) {
        PORTA.OUT &= ~LEFT_MOTOR;    // Deactivate left motor
        PORTA.OUT |= MIDDLE_MOTOR;   // Activate middle motor
        PORTA.OUT &= ~RIGHT_MOTOR;   // Deactivate right motor
    } else if (pulseCounter == 3) {
        PORTA.OUT |= LEFT_MOTOR;     // Activate left motor
        PORTA.OUT &= ~MIDDLE_MOTOR;  // Deactivate middle motor
        PORTA.OUT |= RIGHT_MOTOR;    // Activate right motor
    } else {
        pulseCounter = 0;            // Reset pulse counter
        statesActive &= ~PULSE_LEFT; // Clear the pulse left state
        clearMotors();
    }
}


void pulseArrived(){
    if ((pulseCounter < 4) & (secondCounter == 1)){
        PORTA.OUT |= LEFT_MOTOR;
        PORTA.OUT |= MIDDLE_MOTOR;
        PORTA.OUT |= RIGHT_MOTOR;

    } else if ((pulseCounter < 4) & (secondCounter == 2)){
        PORTA.OUT &= ~LEFT_MOTOR;
        PORTA.OUT &= ~MIDDLE_MOTOR;
        PORTA.OUT &= ~RIGHT_MOTOR; 
    }else {
        pulseCounter = 0;
        statesActive &= ~PULSE_ARRIVED;
        clearMotors();
    }
}
