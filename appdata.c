/*******************************************************************************
 * Copyright (c) 2019-2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// appdata.c

#include <stdio.h>
#include <string.h>

#ifdef USE_GPS_MODULE
#include <gps.h>
#endif

#include "enddevice.h"
#include "cayenne_lpp.h"
#include "appdata.h"

void init_GPS(){
#ifdef USE_GPS_MODULE
     gps_init();
#endif
}

byte set_app_data( u1_t* telegram ){
    /* Set serial port,serial0,ttyAMA0 in serial.h in libgps correctly before make
     Be careful with Difference between RPI2 and RPI3 */
    //// GPS version ////
    loc_t gps_data;
#ifdef USE_GPS_MODULE    
    gps_location(&gps_data);
#else    
    // fake gps data for test
    gps_data.latitude = 35.2145;
    gps_data.longitude = 138.4350;
    gps_data.altitude = 3776.12;
#endif
    printf("\tGPS:Lat:%3.4lf,Lon:%3.4lf,Alt:%3.1lf\n", gps_data.latitude, gps_data.longitude,gps_data.altitude);
    
    //// Cayenne_lpp format ////
    cayenne_lpp_t lpp;
    u1_t channel=1;      // set application channel
    
    cayenne_lpp_reset( &lpp); 
    cayenne_lpp_add_gps( &lpp, channel, gps_data.latitude, gps_data.longitude, gps_data.altitude);

    if(lpp.cursor > APP_DATA_SIZE) lpp.cursor=APP_DATA_SIZE;
    memcpy(telegram, lpp.buffer, lpp.cursor);
    printf("\tApp Data:");       
    for( int i=0;i< lpp.cursor; i++){
        printf("%02x:",lpp.buffer[i]);
    }
    printf("\n");
    
    return( (byte) lpp.cursor);
}

// end of appdata.c