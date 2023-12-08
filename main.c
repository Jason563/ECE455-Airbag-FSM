#include <stdio.h>
#include "uart.h"
#include "StarterCode.h"
#include "systick.h"
#include "ABS.h"

extern uint32_t msTicks;


int main() {
	
	struct FSM ABS_handler;
	uint32_t fail_clk = 0;
	uint32_t crash_clk = 0;
	uint32_t prior_time = msTicks;
	uint32_t break_on = 0;
	uint32_t break_off = 0;
	
	SystemInit();
	
	Initialize();
	
	ResetLED();	
	initialize_FSM(&ABS_handler);
	systick();

	while (1){
		
		printf("%d %d\n",fail_clk, msTicks );
		
		ADC_reading(&ABS_handler, acceleration_voltage(4000));

		
		if((ABS_handler.state != COLLISION)&&(ABS_handler.state != FAILURE)) { 
			
			ADC_reading(&ABS_handler, acceleration_voltage(ReadPotentiometer()));
			
			if ((break_engaged(ReadJoystick(), ReadPushbutton()) == 1)&&(break_on == 0)&&(msTicks - 40 > break_off)) {
				brake_log(BREAKING, msTicks);
				break_on = msTicks;
				break_off = 0;
			}
			if ((break_engaged(ReadJoystick(), ReadPushbutton()) != 1)&&(break_on != 0)&&(msTicks - 40 > break_on)) {
				if (ABS_handler.state != CONST_VELOCITY) {
					brake_log(break_engaged(ReadJoystick(), ReadPushbutton()), msTicks);
				}else {
					brake_log(DISENGAGED, msTicks);
				}
				
				break_on = 0;
				break_off = msTicks;
			}
			
			
			if ((acceleration_voltage(ReadPotentiometer()) > 2 )&&(crash_clk == 0)) {
				crash_clk = msTicks;
			} 
			
			if (acceleration_voltage(ReadPotentiometer()) < 2 )	{
				crash_clk = 0;
			}
			
			if ((msTicks - crash_clk >= 5)&&(crash_clk != 0)) {
				state_transition(&ABS_handler, FAST_ACCELERATION);
				crash();
			}
			
			if ((ABS_handler.state == NORMAL_ACCELERATION)&&(fail_clk == 0)) {
				fail_clk = msTicks;
			}
			
			if (ABS_handler.prior_state == NORMAL_ACCELERATION) {
				fail_clk = 0;
			}
			
			if ((msTicks - fail_clk >= 25000)&&(fail_clk != 0)) {
				state_transition(&ABS_handler, ACCELEROMETER_FAILURE);
				failure_led();
			}
		}
		
	}
	
}

// Print statements for testing 
		/*
		if (msTicks > prior_time + 999) {
			prior_time = msTicks;
			printf("Time is %d", prior_time);
			printf("      FailCLk is %d \n", fail_clk);
			printf("Acceleration is %0.2f \n", acceleration_voltage(ReadPotentiometer()));
			printf("current state is %d \n", ABS_handler.state);
		}
		*/
