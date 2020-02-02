/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// State.h
#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include "FSM.h"

/* 状態クラス */
typedef struct State {
	void (*entry)(FSM *fsm);
	void (*exit)(FSM *fsm);
	void (*event_timer)(FSM *fsm);
//	void (*event_rxframe)(FSM *fsm);
} State;

void State_init(FSM *fsm);

#endif /* STATE_H_INCLUDED */