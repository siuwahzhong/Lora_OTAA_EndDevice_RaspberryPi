/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// lora.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lora.h"
#include "enddevice.h"
#include "showtime.h"
#include "radio.h"
#include "appdata.h"

// lora connection parameters
lora_t lora;

void show_lora_auth(){
    u1_t i;
    printf("\tDEVEUI(lsb): ");
    for(i=0;i<8;i++){
        printf("%02X:",DEVEUI[i]);
    }
    printf("\n");
    printf("\tAPPEUI(lsb): ");
    for(i=0;i<8;i++){
        printf("%02X:",APPEUI[i]);
    }
    printf("\n");
    printf("\tAPPKEY(msb): ");
    for(i=0;i<16;i++){
        printf("%02X:",APPKEY[i]);
    }       
    printf("\n");        
}

void Init_Lora(){
    lora.FCntUp = 0;
    lora.FCntDown = 0;
    lora.NFCntDown = 0;
    lora.AFCntDown = 0;

    //Radio.carr_sns_time = CARRIER_SENSE_TIME;
}

void Update_txlora(packet_t* txpkt_p){
    byte tx_pld_len;               // payload length
    u1_t telegram[128] = {};

    lora.transmit = 1;
    switch(lora.join_st){
        case NOJOIN:
            lora.join_st = JOINING;
            lora.nb_try_join = 1;
            show_time("Join Requesting.");
            Build_JoinRequest(txpkt_p);        
            break;
        case JOINING:
            //printf("nb try join:%d\n", lora.nb_try_join);
            if(lora.nb_try_join++ == NB_TRY_JOIN_REQ_MAX ){
                // Fail join and Retry
                lora.tx_period = (u2_t)(lora.tx_period * 1.2);
                if(lora.tx_period >= TX_PERIOD_MAX){
                    show_time2("CAN NOT JOIN, GIVE UP.", lora.nb_try_join);
                    exit(1);
                }
                show_time2("Join Failed and Retry.",lora.tx_period);
                lora.join_st = NOJOIN;
                lora.transmit = 0;
            }else{
                // Tx Empty Frame to make timing RX1/2 (even w/o Beacon)
                Build_EmptyFrame(txpkt_p);
                printf("\tTx Empty Frame for RX1/2.\n");
            }
            break;
        case JOINED:
            show_time("Data sending.");
            tx_pld_len = set_app_data( telegram );
            Build_txFrame(txpkt_p, telegram, (int)tx_pld_len);
            break;   
    }
}

void Tx_lora(){
    packet_t txpkt;

    Update_txlora(&txpkt);
    if(lora.transmit == 1 ){   
        Try_txFrame(&txpkt);
    }
}

bool Rx_lora(){
    packet_t rxpkt;
    bool result;
    Rx_Packet( &rxpkt );
    if (rxpkt.bytes >1){
        result = Decode_rxFrame(rxpkt.buf, rxpkt.bytes);
        return(result);
    }else{
        return(0);
    }
}
// end of lora.c