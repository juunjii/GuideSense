/*
 * File:   motor.h
 * Author: chehj
 *
 * Created on December 1, 2024, 8:53 PM
 */

#ifndef MOTOR_H
#define MOTOR_H


#include <avr/io.h> // For PORTA register definitions

// Motor Pins
#define LEFT_MOTOR PIN4_bm
#define MIDDLE_MOTOR PIN5_bm
#define RIGHT_MOTOR PIN6_bm


// External Variables
extern volatile uint8_t pulseCounter;
extern volatile int  secondCounter;
extern volatile uint8_t statesActive;



// Function Prototypes
/**
 * @brief Initializes motor pins as output.
 */
void initMotors(void);

/**
 * @brief Clears all motor control signals.
 */
void clearMotors(void);

/**
 * @brief Generates a pulse sequence for the left motor.
 */
void pulseLeft(void);

/**
 * @brief Generates a pulse sequence for the middle motor.
 */
void pulseMiddle(void);

/**
 * @brief Generates a pulse sequence for the right motor.
 */
void pulseRight(void);

/**
 * @brief Generates a pulse sequence for both left and right motors (closer).
 */
void pulseCloser(void);

/**
 * @brief Generates a pulse sequence for the middle motor and alternating left and right motors (further).
 */
void pulseFurther(void);

#endif /* MOTOR_H */
