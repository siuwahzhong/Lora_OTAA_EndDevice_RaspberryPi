/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "board.h"

void BoardInitMcu( void )
{
    
}

void BoardInitPeriph( void )
{
}

void BoardLowPowerHandler( void )
{
    // __wfi();
}

uint8_t BoardGetBatteryLevel( void )
{
    return 0;
}

uint32_t BoardGetRandomSeed( void )
{
    uint8_t id[8];

    BoardGetUniqueId(id);

    return (id[3] << 24) | (id[2] << 16) | (id[1] << 1) | id[0];
}

void BoardGetUniqueId( uint8_t *id )
{
    char buff[2048];
    char uuid[17];
    uint64_t uuid_int;
    const char *uuid_file = "/proc/cpuinfo";
    const char *serial_locate= "Serial";
    int fd = 0;
    int ret = 0;
    char *ptr = NULL;
    fd = open(uuid_file,O_RDONLY);
    if(-1==fd){
            fprintf(stderr,"open cpuinfor file fail");
            return fd;
    }

    ret = read(fd,buff,sizeof(buff));
    if(-1 == ret){
            fprintf(stderr,"read cpuinfo file fail");
            return ret;
    }
    //fprintf(stdout,buff);
    ptr = strstr(buff,serial_locate);
    if(NULL != ptr){
    //      printf("serial offset: %ld\n",ptr - buff);
    }
    ptr = ptr+10;
    strncpy(uuid,ptr,16);
    
    //printf("%s\n",uuid);
    //printf("%lld\n",strtoll(uuid,NULL,16));

    uuid_int = strtoll(uuid,NULL,16);
    memcpy(id, (unsigned char *)&uuid_int, 8);

    close(fd);
}

void BoardCriticalSectionBegin( uint32_t *mask )
{
    // *mask = save_and_disable_interrupts();
}

void BoardCriticalSectionEnd( uint32_t *mask )
{
    // restore_interrupts(*mask);
}

void BoardResetMcu( void )
{
}
