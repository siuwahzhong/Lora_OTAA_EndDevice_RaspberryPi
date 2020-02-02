/*******************************************************************************
 * Copyright (c) 2019-2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// frame.h

#ifndef FRAME_H_INCLUDED
#define FRAME_H_INCLUDED

#include <stdio.h>

#include "enddevice.h"

#define AES_ENC       0x00 
#define AES_DEC       0x01
#define AES_MIC       0x02
#define AES_CTR       0x04
#define AES_MICNOAUX  0x08
// MHDR
#define LORAWAN_R1       0b00
#define JOIN_REQUEST    0b000
#define JOIN_ACCEPT     0b001
#define DATA_UNCFMD_UP  0b010
#define DATA_UNCFMD_DN  0b011
#define DATA_CFMD_UP    0b100
#define DATA_CFMD_DN    0b101
#define PROPRIETARY     0b111
//
#define MHDR_MTYPE  0xe0
#define MHDR_MAJOR  0x03
//
#define FCTRL_ADR       0x80
#define FCTRL_ADRACKREQ 0x40
#define FCTRL_ACK       0x20
#define FCTRL_FPENDING  0x10
#define FCTRL_CLASSB    0x10
#define FCTRL_FOPTLEN   0x0f

#define LEN_JA      17
#define LEN_JAOPT   33

#define os_clearMem(a,b)   memset(a,0,b)
#define os_copyMem(a,b,c)  memcpy(a,b,c)

//u4_t os_aes(u1_t, xref2u1_t, u2_t);

extern u4_t AESAUX[];
extern u4_t AESKEY[];
#define AESkey ((u1_t*)AESKEY)
#define AESaux ((u1_t*)AESAUX)

#define BUF_SIZE    256
typedef struct {
    byte bytes;
    byte buf[BUF_SIZE];
} packet_t;

typedef struct {
    u1_t MType;
    u1_t Major;
    u4_t DevAdr;
    u1_t FCtrl;
        u1_t ADR;        // bit7 0x80
        u1_t ADRACKReq;  // bit6 0x40
        u1_t ACK;        // bit5 0x20
        u1_t ClassB;     // bit4 0x10 downlink
        u1_t FPending;   // bit4 0x10 uplink
    u1_t FOptLen;
    u2_t FCnt;
    u1_t FOpts[16];
    u1_t FPort;
    u1_t FRMPayload[128];
    u4_t Mic;
    u2_t Crc;
    u8_t AppEUI;
    u8_t DevEUI;
    u2_t DevNonce;
    u4_t AppNonce;
    u4_t NetID;
    u1_t DLSettings;
    u1_t OptNeg;    /* for V1.1, Not used in this ver. */
    u1_t RxDelay;
    u1_t CFList[16];
    u4_t Freq_Ch[6];
    u1_t CFListType;

} frame_t;

void os_wlsbf2(xref2u1_t , u2_t);
u4_t os_rmsbf4 (u1_t* );
void os_wlsbf4 (u1_t* , u4_t );
void initAES();
void micB0 (u4_t , u4_t , int , int );
bool aes_verifyMic(const u1_t* , u4_t , u4_t , int , u1_t* , int );
void aes_appendMic (const u1_t* , u4_t , u4_t , int , u1_t* , int );
void aes_appendMic0 (xref2u1_t pdu, int );
bool aes_verifyMic0 (xref2u1_t pdu, int );
void aes_encrypt (xref2u1_t, int);
void aes_cipher (const u1_t* , u4_t , u4_t , int , u1_t* , int);
void aes_sessKeys (u2_t , xref2cu1_t , xref2u1_t , xref2u1_t );
void aes_2_sessKeys (u1_t* );
bool Rx_JoinAccept(u1_t*, int);
bool Rx_DataMesg(u1_t*, int);
bool Decode_rxFrame(u1_t *, int);
u2_t Gen_DevNonce();

void Build_JoinRequest(packet_t*);
void Build_EmptyFrame(packet_t* );
void Build_txFrame(packet_t*, u1_t*, int );
#endif /* FRAME_H_INCLUDED */