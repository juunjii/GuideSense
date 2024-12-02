/*
 * File:   motor.c
 * Author: chehj
 *
 * Created on December 1, 2024, 8:53 PM
 */


#include "motor.h"

#define PULSE_LEFT 0x01
#define PULSE_MIDDLE 0x02
#define PULSE_RIGHT 0x04
#define PULSE_CLOSER 0x08
#define PULSE_FURTHER 0x10


void initMotors(){
    PORTA.DIR |= LEFT_MOTOR; //left
    PORTA.DIR |= MIDDLE_MOTOR; //middle
    PORTA.DIR |= RIGHT_MOTOR; //right
}

void clearMotors(){
    PORTA.OUT &= ~LEFT_MOTOR;
    PORTA.OUT &= ~MIDDLE_MOTOR;
    PORTA.OUT &= ~RIGHT_MOTOR;
}

void pulseLeft(){
    if (pulseCounter < 3){
            if (secondCounter == 1){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT |= MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
            } else if (secondCounter == 2){
                PORTA.OUT |= LEFT_MOTOR;
                PORTA.OUT &= ~MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
            }
//            pulseCounter++;
        } else {
            pulseCounter = 0;
            statesActive &= ~PULSE_LEFT;
            clearMotors();
        }
}

void pulseMiddle(){
    if (pulseCounter < 3){
            if (secondCounter == 1){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT |= MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
            } else if (secondCounter == 2){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT &= ~MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
//                pulseCounter++;
            }
            
        } else {
            pulseCounter = 0;
            statesActive &= ~PULSE_LEFT;
            clearMotors();
        }
}

void pulseRight(){
    if (pulseCounter < 3){
            if (secondCounter == 1){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT |= MIDDLE_MOTOR;
                PORTA.OUT &= ~RIGHT_MOTOR;
            } else if (secondCounter == 2){
                PORTA.OUT &= ~LEFT_MOTOR;
                PORTA.OUT &= ~MIDDLE_MOTOR;
                PORTA.OUT |= RIGHT_MOTOR;
//                pulseCounter++;
            }
            
        } else {
            pulseCounter = 0;
            statesActive &= ~PULSE_LEFT;
            clearMotors();
        }
}

void pulseCloser(){
    if (pulseCounter < 3){
        PORTA.OUT |= LEFT_MOTOR;
        PORTA.OUT &= ~MIDDLE_MOTOR;
        PORTA.OUT |= RIGHT_MOTOR;

    } else if (pulseCounter == 3){
        PORTA.OUT &= ~LEFT_MOTOR;
        PORTA.OUT |= MIDDLE_MOTOR;
        PORTA.OUT &= ~RIGHT_MOTOR;
            
    }else {
        pulseCounter = 0;
            statesActive &= ~PULSE_LEFT;
            clearMotors();
        }
}

void pulseFurther(){
    if (pulseCounter < 3){
        PORTA.OUT &= ~LEFT_MOTOR;
        PORTA.OUT |= MIDDLE_MOTOR;
        PORTA.OUT &= ~RIGHT_MOTOR;

    } else if (pulseCounter == 3){
        PORTA.OUT |= LEFT_MOTOR;
        PORTA.OUT &= ~MIDDLE_MOTOR;
        PORTA.OUT |= RIGHT_MOTOR; 
    }else {
        pulseCounter = 0;
        statesActive &= ~PULSE_LEFT;
        clearMotors();
    }
}

