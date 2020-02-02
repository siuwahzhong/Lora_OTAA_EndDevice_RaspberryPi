/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// timerevent.h

#ifndef TIMEREVENT_H_INCLUDED
#define TIMEREVENT_H_INCLUDED

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "enddevice.h"

typedef void (*FUNC_PTR)(void);

void set_Timer(int, int, FUNC_PTR );
void stop_Timer();

#endif
// end of timerevent.h