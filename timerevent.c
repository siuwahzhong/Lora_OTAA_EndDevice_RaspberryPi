/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// timerevent.c
// gcc -lrt

#include "timerevent.h"

static timer_t timer_id;

void set_Timer(int tms, int tmn, FUNC_PTR handler){
    //timer_t timer_id;
    struct sigaction sigact;
    struct itimerspec itval;

    // シグナルハンドラを設定する
    sigact.sa_handler = (void(*)())handler;

    // シグナルハンドラの動作を変更する為のフラグ(特に指定しない)
    sigact.sa_flags = 0;

    // シグナル集合を空にする
    // (シグナルハンドラを設定する対象のシグナルをクリアする)
    sigemptyset( &sigact.sa_mask );

    // SIGALRMに対してシグナルハンドラを設定
    sigaction( SIGALRM, &sigact, NULL );

    // タイマーを作成
    //timer_create( CLOCK_MONOTONIC, NULL, &timer_id );
    timer_create( CLOCK_MONOTONIC, NULL, &timer_id );

    // タイマーが満了するまでの時間
    itval.it_value.tv_sec = tms;
    itval.it_value.tv_nsec = tmn;

    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = 0;

    // タイマーを設定
    timer_settime( timer_id, 0, &itval, NULL );
}

void stop_Timer(){
    // タイマーを削除
    timer_delete( timer_id );
}
// end of timerevent.c