/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include <wiringPi.h>
#include "gpio-board.h"

void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value )
{
    obj->pin = pin;

    if( pin == NC )
    {
        return;
    }
    
    if (mode == PIN_INPUT)
    {
        pinMode (pin, INPUT);
    }
    else if (mode == PIN_OUTPUT)
    {
        pinMode(pin, OUTPUT);
    }

    if (config == PIN_PUSH_PULL)
    {
        if (type == PIN_NO_PULL)
        {
            pullUpDnControl (pin,PUD_OFF);
        }
        else if (type == PIN_PULL_UP)
        {
            pullUpDnControl (pin,PUD_UP);
        }
        else if (type == PIN_PULL_DOWN)
        {
            pullUpDnControl(pin,PUD_DOWN);
        }
    }

    if( mode == PIN_OUTPUT )
    {
        GpioMcuWrite( obj, value );
    }
}

void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
    digitalWrite (obj->pin, value);
}

uint32_t GpioMcuRead( Gpio_t *obj )
{
    return digitalRead (obj->pin);
}

void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    wiringPiISR(obj->pin,INT_EDGE_RISING,irqHandler);
}

void GpioMcuRemoveInterrupt( Gpio_t *obj )
{
    //wiringPiISR(obj->pin,INT_EDGE_SETUP,irqHandler);
}

void GpioMcuSetContext( Gpio_t *obj, void* context )
{

}

void GpioMcuToggle( Gpio_t *obj )
{

}