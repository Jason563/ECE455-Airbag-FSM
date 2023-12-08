#include "FSM.h"
#include <lpc17xx.h>

#define PINSEL_EINT0    20
#define PINSEL_EINT1    22

#define LED1            0   
#define LED2            1    

#define SBIT_EINT0      0
#define SBIT_EINT1      1  

#define SBIT_EXTMODE0   0
#define SBIT_EXTMODE1   1

#define SBIT_EXTPOLAR0  0
#define SBIT_EXTPOLAR1  1

#define ZERO_ACCELERATION 		0
#define SLOW_ACCELERATION			1
#define FAST_ACCELERATION			2
#define ACCELEROMETER_FAILURE 3

#define CONST_VELOCITY				0
#define NORMAL_ACCELERATION		1	
#define COLLISION							2 
#define FAILURE								3

#define BREAKING 							1
#define DISENGAGED						0


float acceleration_voltage(unsigned int poteniometer);
int break_engaged(unsigned int joystick, unsigned int ABS);
    
void initialize_FSM(struct FSM *FSM);
void ADC_reading(struct FSM *FSM, float acceleration);
void crash();
void brake_log(int brake_event, uint32_t timing);