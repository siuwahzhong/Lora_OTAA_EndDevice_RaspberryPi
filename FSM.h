/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// FSM.h
#ifndef FSM_H_INCLUDED
#define FSM_H_INCLUDED

struct State;

typedef struct FSM {
	const struct State *current_state;    /* Current State */
} FSM;

void FSM_init();
void FSM_timer();

/* State transition */
void FSM_change_state(FSM *self, const struct State *new_state);

#endif /* FSM_H_INCLUDED */