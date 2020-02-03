/*******************************************************************************
 * Copyright (c) 2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// enddevice.h

#ifndef ENDDEVICE_H_INCLUDED
#define ENDDEVICE_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

/* OTAA parameters, Set your value */  
/* lsb, make using wlan mac address of raspberry pi inserted ff ff    */
static const uint8_t DEVEUI[8] = { 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00 };
/* lsb, TTN = .. .. ..0x7E, 0xD5, 0xB3, 0x70    */
static const uint8_t APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x7E, 0xD5, 0xB3, 0x70 };
/* msb, got from TTN console        */
static const uint8_t APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* Configure Frequency according to your region and Gateway channel */ 
/* EU868: 868100000, 868300000, 868500000                           */
/* JP923: 923400000, 923200000                                      */
#define FREQUENCY_REGION    923400000
#define FREQ_RX2_REGION     923400000

/* TX power, Depends on your region             */
/* EU868: 16 @ 0dBi Antenna                     */
/* JP923: 13 @ 3dBi Antenna by ARIB STD-T108    */
#define RF_PWR_REGION      13  // 13dBm=20mW;

/* After setup gps module, remove comment and work */
/* Otherwise send dummy gps data                   */
//#define USE_GPS_MODULE

/* GPS data size = 11byte, Air Time <= 400ms @SF10,  */
/* MUST be larger then 40 for 1% duty rule          */
#define INTERVAL_KEEP_ALIVE 60  // Second
#define SF_         SF7         //
#define SF_RX2_       SF7 
#define RSSI_CH_FREE    -80     // dBm
#define CARRIER_SENSE_TIME 160  //uS
#define INTERVAL_RX1        1   // Second
#define INTERVAL_RX2        1   // Second
/* It may take a few minutes to get Join_acc. */
/* You need to wait patiently.                */
#define NB_TRY_JOIN_REQ_MAX  5
#define TX_PERIOD_MAX   3600

typedef uint8_t            byte;
typedef uint8_t            bit_t;
typedef uint8_t            u1_t;
typedef int8_t             s1_t;
typedef uint16_t           u2_t;
typedef int16_t            s2_t;
typedef uint32_t           u4_t;
typedef int32_t            s4_t;
typedef uint64_t           u8_t;
typedef unsigned int       uint;
typedef const char* str_t;
typedef       u1_t* xref2u1_t;
typedef const u1_t* xref2cu1_t;

#endif  
/* ENDDEVICE_H_INCLUDED */
