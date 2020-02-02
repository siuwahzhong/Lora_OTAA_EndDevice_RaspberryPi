/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// lora.h

#ifndef LORA_H_INCLUDED
#define LORA_H_INCLUDED

#include "enddevice.h"
#include "frame.h"

enum activate_t { NOJOIN=0, JOINING, JOINED };

typedef struct {
    enum activate_t join_st;
    byte nb_try_join;
    u2_t tx_period;
    bool transmit;
    u2_t FCntUp;
    u4_t FCntDown;
    u4_t NFCntDown; 
    u4_t AFCntDown;
    u1_t NwkID;
    u4_t NwkAddr;
    u1_t NwkSKey[16];
    u1_t AppSKey[16];
    u1_t AppEUI[8];     // confiure
    u1_t DevEUI[8];     //
    u1_t AppKey[16];    //
    u2_t DevNonce;
    u4_t DevAddr;
}lora_t;

void show_lora_auth();
void Init_Lora(); 
void Update_txlora(packet_t*);
void Tx_lora();
bool Rx_lora();

#endif /* LORA_H_INCLUDED */