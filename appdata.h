// appdata.h

#ifndef APPDATA_H_INCLUDED
#define APPDATA_H_INCLUDED

#include <stdio.h>

#include "enddevice.h"

// caneyee_lpp GPS data size
#define APP_DATA_SIZE 11

#ifndef USE_GPS_MODULE
typedef struct{
    double latitude;
    double longitude;
    double altitude;
} loc_t;
#endif

// function prototype
void init_GPS();
byte set_app_data( u1_t* );

#endif /* APPDATA_H_INCLUDED */