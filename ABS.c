#include <lpc17xx.h>
#include <stdio.h>
#include "FSM.h"
#include "StarterCode.h"

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

extern uint32_t msTicks;
uint32_t debounce;
int brakes[100];
int entry;
uint32_t brake_timing[100];

struct transition TRANSITIONS[] = {{CONST_VELOCITY, SLOW_ACCELERATION, NORMAL_ACCELERATION},{NORMAL_ACCELERATION, ZERO_ACCELERATION, CONST_VELOCITY},
																 {NORMAL_ACCELERATION, FAST_ACCELERATION, COLLISION}, {NORMAL_ACCELERATION, ACCELEROMETER_FAILURE, FAILURE}};

float acceleration_voltage(unsigned int poteniometer) {
	float voltage = poteniometer;
	if (poteniometer < 190) {
		return 0;
	} 
	voltage = voltage/1180;
	return voltage;
}

int break_engaged(unsigned int joystick, unsigned int ABS) {
	if ((ABS == 1)&&(joystick == 8)) {
		return 2;
	}
	if (joystick == 8) {
		return 1;
	}
	
	return 0;
}

void brake_log(int brake_event, uint32_t timing) {
	int j;
	brakes[entry] = brake_event;
	brake_timing[entry] = timing;
	entry++;
	
	if ((msTicks - brake_timing[0] >= 5000)&&(brake_timing[0] != 0)) {
		if ((brakes[0]==1)&&(msTicks - brake_timing[1] <= 5000)) {
			brake_timing[0] = msTicks - 4999;;
			
		} else if ((brakes[0]==1)&&(msTicks - brake_timing[1] > 5000)){
			for(j = entry; j > 0; j--) {
				brakes[entry - j] = brakes[entry - j + 2];
				brake_timing[entry - j] = brake_timing[entry - j + 2];
			}
		
			brakes[entry] = 0;
			brake_timing[entry] = 0;
			brakes[entry -1] = 0;
			brake_timing[entry -1] = 0;
			entry = entry - 2;
		} else if (brakes[0]!=1){
			for(j = entry; j > 0; j--) {
				brakes[entry - j] = brakes[entry - j + 1];
				brake_timing[entry - j] = brake_timing[entry - j + 1];
			}
			brakes[entry] = 0;
			brake_timing[entry] = 0;
			entry--;

		}
		
	}

}

void crash() {
	int j;
	int k;
	airbag();
		
	for(k = 0; k <= entry; k++) {
		if (((msTicks - brake_timing[0] >= 5000)||(msTicks - brake_timing[1] >= 5000))&&(brake_timing[0] != 0)) {
			if ((brakes[0]==1)&&(msTicks - brake_timing[1] >= 5000)){
				for(j = entry; j > 1; j--) {
					brakes[entry - j] = brakes[entry - j + 2];
					brake_timing[entry - j] = brake_timing[entry - j + 2];
				}
			
				brakes[entry] = 0;
				brake_timing[entry] = 0;
				brakes[entry -1] = 0;
				brake_timing[entry -1] = 0;
				entry = entry - 2;
			} else if (brakes[0]!=1){
				for(j = entry; j > 0; j--) {
					brakes[entry - j] = brakes[entry - j + 1];
					brake_timing[entry - j] = brake_timing[entry - j + 1];
				}
				brakes[entry] = 0;
				brake_timing[entry] = 0;
				entry--;

			}
			
		}
	}
	
	if ((brakes[0] == 0)&&(brake_timing[0] != 0)) {
		for(j = entry; j >= 0; j--) {
			brakes[j + 1] = brakes[j];
			brake_timing[j + 1] = brake_timing[j];
		}
		brakes[0] = 1;
		brake_timing[0] = msTicks - 4999;
		entry++;
	}
	
	//printf("0: engaged:%d %d milliseconds ago \n",brakes[0], msTicks - brake_timing[0]);
	for(j = entry; j > 0; j--) {
		if(msTicks - brake_timing[entry - j] >= 5000) {
			brake_timing[entry - j] = msTicks - 4999;
		}
		if ((j==entry)||(brakes[entry - j] != brakes[entry - j-1])) {
			printf("%d %d\n",brakes[entry-j], msTicks - brake_timing[entry - j]);
		}
	}
}



void initialize_FSM(struct FSM *FSM) {
	FSM->num_of_transitions = 4;
	FSM->prior_event = ZERO_ACCELERATION;
	FSM->prior_state = NORMAL_ACCELERATION;
	FSM->start_state = CONST_VELOCITY;
	FSM->state = CONST_VELOCITY;
	FSM->transitions = TRANSITIONS;
}

void ADC_reading(struct FSM *FSM, float acceleration) {
	if (acceleration > 3) {
		crash();
		state_transition(FSM, FAST_ACCELERATION);
	}
	
	if ((FSM->state == ZERO_ACCELERATION)&&(acceleration > 0)) {
		state_transition(FSM, SLOW_ACCELERATION);
	}
	
	if ((FSM->state == NORMAL_ACCELERATION)&&(acceleration == 0)) {
		state_transition(FSM, ZERO_ACCELERATION);
	}
	
}

//PRINT STATEMENTS
// 			printf("j: %d, array [%d], engaged:%d %d milliseconds ago brake_timing %d\n", j, entry - j, brakes[entry - j], msTicks - brake_timing[entry - j],brake_timing[entry - j]);
