/*******************************************************************************
 * enddevice.c
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "enddevice.h"
#include "FSM.h"
#include "radio.h"
#include "showtime.h"
#include "lora.h"
#include "appdata.h"

int main(){
    puts("===== LoRa OTAA End Device for RPI/Dragino GPS HAT =====");
    // setup wiringPI, pins and SPI
    if( Init_radio() ){    
        show_time("SX1276 Detected. Starting.");
        printf("\tSend packet every %d seconds.\n", INTERVAL_KEEP_ALIVE);
        show_lora_auth();
        show_lora_radio();
#ifdef USE_GPS_MODULE
        puts("\tGPS Module enabled.");
#else
        puts("\tGPS Module Disabled, Send Dummy data");
#endif
        init_GPS();
        FSM_init();     // init state machine
        while(1){
            pause();
        }
    }else{
        puts("SX1276 NOT Detected. Stop.");
        exit(1);         
    }
    return(0);
}
