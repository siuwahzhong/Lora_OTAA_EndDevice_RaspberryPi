/*******************************************************************************
 * Copyright (c) 2019-2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// showtime.c

#include <stdio.h>
#include <time.h>
#include <stdint.h>

// display time and event
void show_tm(void){
    struct timespec tvToday; // for msec
    struct tm *ptm; // for date and time

    clock_gettime(CLOCK_REALTIME_COARSE, &tvToday);
    ptm = localtime(&tvToday.tv_sec);
    // time
    printf("%02d:%02d:%02d.",ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    // msec
    printf("%03d ", (uint16_t)(tvToday.tv_nsec / 1000000));
}
void show_time(const char *mess){
    show_tm();
    printf(": %s\n", mess);
    fflush(stdout);
}
void show_time2(const char *mess, int val){
    show_tm();
    printf(": %s %d\n", mess, val);
    fflush(stdout);
}
void show_timeh(const char *mess, int hex_val){
    show_tm();
    printf(": %s %x\n", mess, hex_val);
    fflush(stdout);
}
// end of showtime.c