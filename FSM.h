
struct transition {
	int curr_state;
	int event;
	int next_state;
};

struct FSM {
	int start_state;
	int state;
	int prior_state;
	int prior_event;
	int num_of_transitions;
	struct transition *transitions;
	
};

int next_state(struct FSM *FSM, int curr_state, int event);
void state_transition (struct FSM *FSM, int event);
