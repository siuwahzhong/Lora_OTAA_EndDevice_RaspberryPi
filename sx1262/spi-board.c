/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include <wiringPiSPI.h>
#include "spi-board.h"

void SpiInit( Spi_t *obj, SpiId_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss )
{
    wiringPiSPISetup (0, 10000000 ) ;
}

uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
    const uint8_t outDataB = (outData & 0xff);
    uint8_t inDataB = 0x00;

    inDataB = wiringPiSPIDataRW(0,&outDataB,1);

    return inDataB;
}
