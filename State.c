/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// State.c
#include <stdio.h>
#include <time.h>
#include <wiringPi.h>

#include "State.h"
#include "FSM.h"
#include "radio.h"
#include "frame.h"
#include "appdata.h"
#include "timerevent.h"
#include "lora.h"
#include "showtime.h"

//// remove comment when debug ////
#define Debug_State

typedef void (*FUNC_PTR)(void);
FUNC_PTR func_p; //(*fp)(void);

volatile byte rx_cnt;
extern lora_t lora;

static void StateSetup_timer(FSM *fsm);

static void StateIdle_entry(FSM *fsm);
static void StateIdle_timer(FSM *fsm);

static void StateTx_entry(FSM *fsm);
/*
static void StateRx1_entry(FSM *fsm);
static void StateRx1_exit(FSM *fsm);
static void StateRx1_timer(FSM *fsm);
*/
static void StateRx2_entry(FSM *fsm);
static void StateRx2_exit(FSM *fsm);
static void StateRx2_timer(FSM *fsm);

//static void StateRxPkt_entry(FSM *fsm);

// State Object
static const State state_Setup = { 0, 0, StateSetup_timer };
static const State state_Idle = { StateIdle_entry, 0, StateIdle_timer };
static const State state_Tx = { StateTx_entry, 0, 0 };
/*
static const State state_Rx1 = { StateRx1_entry, StateRx1_exit, StateRx1_timer };
*/
static const State state_Rx2 = { StateRx2_entry, StateRx2_exit, StateRx2_timer };

// Initialize
void State_init(FSM *fsm){
	fsm->current_state = &state_Setup;
    
    lora.tx_period = INTERVAL_KEEP_ALIVE;
    Init_Lora();
    rx_cnt = 0;
    // start timer
    //fp = FSM_timer;
    func_p = FSM_timer;
    set_Timer(1, 0, FSM_timer);    
}

// Setup
static void StateSetup_timer(FSM *fsm){
#ifdef Debug_State
	show_time("State: Setup -> timer");
#endif
    FSM_change_state(fsm, &state_Idle);
}

// Idle
static void StateIdle_entry(FSM *fsm){
#ifdef Debug_State
	show_time("State: Idle -> entry");
#endif
    rx_cnt = 0;
    func_p = FSM_timer;
    set_Timer(1, 0, FSM_timer);     
}
static void StateIdle_timer(FSM *fsm){
#ifdef Debug_State
	show_time("State: Idle -> timer");
#endif
    FSM_change_state(fsm, &state_Tx);   
}

// TX
static void StateTx_entry(FSM *fsm){
#ifdef Debug_State
    show_time("State: Tx -> entry");
#endif
    Tx_lora();
    if(lora.join_st == NOJOIN){
        // Join Failed
        FSM_change_state(fsm, &state_Idle);
        return;
    }
    while( !is_tx_Done() ){
        delay(10);
    }
    start_RX2();        // like Class C, LISTEN RX2 ONLY 
    FSM_change_state(fsm, &state_Rx2);
}

static void StateRx2_entry(FSM *fsm){
#ifdef Debug_State
    show_time("State: Rx2 -> entry");
#endif
    if(Rx_lora()){        
        FSM_change_state(fsm, &state_Idle);
        return;
    }
    func_p = FSM_timer;
    set_Timer(1, 0, FSM_timer);
    printf("\tListening ");
    fflush(stdout);
}

static void StateRx2_timer(FSM *fsm){
    printf(".");
    fflush(stdout);
    if(++rx_cnt >= lora.tx_period || Rx_lora()){
        FSM_change_state(fsm, &state_Idle);
        return;
    }else{
        set_Timer(1, 0, FSM_timer);
    }
}

static void StateRx2_exit(FSM* fsm){
    printf("\n");
}
/*
static void StateRx1_entry(FSM *fsm){
#ifdef Debug_State
    show_time("State: Rx1 -> entry");
#endif
    start_RX1();
    if(Rx_lora()){        
        FSM_change_state(fsm, &state_Idle);
        return;
    }
    func_p = FSM_timer;
    set_Timer(1, 0, FSM_timer);
    printf("\tListening ");
    fflush(stdout);
}

static void StateRx1_timer(FSM *fsm){
#ifdef Debug_State
    show_time("State: Rx1 -> timer");
#endif
    printf(".");
    fflush(stdout);
    if(++rx_cnt >= lora.tx_period || Rx_lora()){
        FSM_change_state(fsm, &state_Idle);
        return;
    }else{
        set_Timer(1, 0, FSM_timer);
    }
}

static void StateRx1_exit(FSM* fsm){
    printf("\n");
}
*/
// End of State.c

