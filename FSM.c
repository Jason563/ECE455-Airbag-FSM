#include <lpc17xx.h>
#include <stdio.h>
#include "FSM.h"

int next_state(struct FSM *FSM, int curr_state, int event) {
	
	int i;
	
	for (i = 0; i < FSM->num_of_transitions; i++) {
		if((FSM->transitions[i].curr_state == curr_state) && (FSM->transitions[i].event == event)) {
			return FSM->transitions[i].next_state;
		}
  }
	return curr_state;
}

void state_transition (struct FSM *FSM, int event) {
	int prior_state = FSM->state;
	int next;
	next = next_state(FSM, prior_state, event);
	
	if( next != prior_state) {
		FSM->state = next;
		FSM->prior_state = prior_state;
		FSM->prior_event = event;
	}
}

	//printf("1 Curr_state %d,    Input State %d,     Event Needed %d,      Input Event %d \n", FSM->transitions[0].curr_state, curr_state, FSM->transitions[0].event, event);
	//printf("2 Curr_state %d,    Input State %d,     Event Needed %d,      Input Event %d \n", FSM->transitions[1].curr_state, curr_state, FSM->transitions[1].event, event);
