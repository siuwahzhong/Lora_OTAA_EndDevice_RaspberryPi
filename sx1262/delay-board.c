/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include "delay-board.h"

void DelayMsMcu( uint32_t ms )
{
    delayMicroseconds(ms);
}
