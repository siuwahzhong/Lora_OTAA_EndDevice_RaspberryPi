/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// FSM.c
#include <stdio.h>

#include "FSM.h"
#include "State.h"
#include "showtime.h"
#include "timerevent.h"

FSM fsm;

void FSM_init(){
	State_init(&fsm);
}

void FSM_timer(){ 
    stop_Timer();
    if (fsm.current_state && fsm.current_state->event_timer) {
        //show_time("Event: timer");
        fsm.current_state->event_timer(&fsm);
    }
}

// State transition(Common)
static void change_state_common(FSM *self, const State **current, const State *new_state){
	if (*current && (*current)->exit) {
		(*current)->exit(self);
	}
	*current = new_state;
	if (new_state && new_state->entry) {
		new_state->entry(self);
	}
}

// State transition
void FSM_change_state(FSM *self, const State *new_state){
	change_state_common(self, &self->current_state, new_state);
}
// End of FSM.c