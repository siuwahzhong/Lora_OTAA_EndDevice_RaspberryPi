/*******************************************************************************
 * Copyright (c) 2019-2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
/*******************************************************************************
 * Copyright (c) 2014-2015 IBM Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    IBM Zurich Research Lab - initial API, implementation and documentation
 *******************************************************************************/
// frame.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "frame.h"
#include "radio.h"
#include "aes.h"
#include "showtime.h"
#include "lora.h"

//// Add/Remove comment when debug ////
#define Debug_Frame

frame_t txFrm;
frame_t rxFrm;

extern lora_t lora;
extern radio_t radio;

u4_t os_rlsbf4 (xref2cu1_t buf) {
    return (u4_t)((u4_t)buf[0] | ((u4_t)buf[1]<<8) | ((u4_t)buf[2]<<16) | ((u4_t)buf[3]<<24));
}

u4_t os_rmsbf4 (u1_t* buf) {
    return (u4_t)(buf[3] | (buf[2]<<8) | ((u4_t)buf[1]<<16) | ((u4_t)buf[0]<<24));
}

void os_wlsbf4 (u1_t* buf, u4_t v) {
    buf[0] = v;
    buf[1] = v>>8;
    buf[2] = v>>16;
    buf[3] = v>>24;
}
void os_wmsbf4 (u1_t* buf, u4_t v) {
    buf[3] = v;
    buf[2] = v>>8;
    buf[1] = v>>16;
    buf[0] = v>>24;
}
void os_wlsbf2(xref2u1_t buf, u2_t v) {
    buf[0] = v;
    buf[1] = v>>8;
}
/*
void initAES(){
    //
    memcpy(AESkey,DEVKEY,16);
}*/

void micB0 (u4_t devaddr, u4_t seqno, int dndir, int len) {
    memset(AESaux,0x00, 16);
    AESaux[0]  = 0x49;
    AESaux[5]  = dndir?1:0;
    AESaux[15] = len;
    os_wlsbf4(AESaux+ 6,devaddr);
    os_wlsbf4(AESaux+10,seqno);
}

bool aes_verifyMic(const u1_t* key, u4_t devaddr, u4_t seqno, int dndir, u1_t* payload, int len) {
    micB0(devaddr, seqno, dndir, len);
    memcpy(AESkey,key,16);
    u4_t mic_c = os_aes(AES_MIC, payload, len);
    //uint32_t mic_f = rxFrm.Mic;         //os_rmsbf4(payload+len);
    printf("\tcal_Mic:%#x, frm_Mic:%#x\n", mic_c, rxFrm.Mic);  //os_rmsbf4(payload+len) );
    if(mic_c == rxFrm.Mic)
        return(1);
    else
        return(0);        
}

 void aes_appendMic (const u1_t* key, u4_t devaddr, u4_t seqno, int dndir, u1_t* payload, int len) {
    micB0(devaddr, seqno, dndir, len);
    memcpy(AESkey,key,16);
    // MSB because of internal structure of AES
    os_wmsbf4(payload+len, os_aes(AES_MIC, payload, len));
}

void os_getDevKey(u1_t* buf){
    memcpy(buf,APPKEY,16);
}

void aes_appendMic0 (xref2u1_t pdu, int len) {
    os_getDevKey(AESkey);
    os_wmsbf4(pdu+len, os_aes(AES_MIC|AES_MICNOAUX, pdu, len));  // MSB because of internal structure of AES
}

bool aes_verifyMic0 (xref2u1_t pdu, int len) {
    os_getDevKey(AESkey);
    u4_t mic_c = os_aes(AES_MIC|AES_MICNOAUX, pdu, len);
    rxFrm.Mic = os_rmsbf4(pdu+len);
    printf("\tcalc_Mic:%02x, frame_Mic:%02x\n", mic_c, rxFrm.Mic);  //os_rmsbf4(payload+len) );
    //printf("\tcal_Mic:%02x, frm_Mic:%02x\n", mic_c, os_rmsbf4(pdu+len));  //os_rmsbf4(payload+len) );
    if(mic_c == rxFrm.Mic)
        return(1);
    else
        return(0);
}

void aes_encrypt (xref2u1_t pdu, int len) {
    os_getDevKey(AESkey);
    os_aes(AES_ENC, pdu, len);
}

void aes_cipher (const uint8_t* key, u4_t devaddr, u4_t seqno, int dndir, uint8_t* payload, int len) {
    if( len <= 0 )
        return;
#ifdef Debug_Frame
    printf("\tEnc:");
    for(int i=0;i<len; i++){
        printf("%02x:",payload[i]);
    }
    printf("\n");
#endif
    memset(AESaux, 0x00, 16);
    AESaux[0] = AESaux[15] = 1; // mode=cipher / dir=down / block counter=1
    AESaux[5] = dndir?1:0;
    os_wlsbf4(AESaux+ 6,devaddr);
    os_wlsbf4(AESaux+10,seqno);
    memcpy(AESkey,key,16);
    os_aes(AES_CTR, payload, len);
#ifdef Debug_Frame
    printf("\tCph:");
    for( int i=0;i<len; i++){
        printf("%02x:",payload[i]);
    }
    printf("\n");
#endif
}
/*
void aes_sessKeys (u2_t devnonce, xref2cu1_t artnonce, xref2u1_t nwkkey, xref2u1_t artkey) {
    os_clearMem(nwkkey, 16);
    nwkkey[0] = 0x01;
    os_copyMem(nwkkey+1, artnonce, 3+3);
    os_wlsbf2(nwkkey+1+3+3, devnonce);
    os_copyMem(artkey, nwkkey, 16);
    artkey[0] = 0x02;

    os_getDevKey(AESkey);
    os_aes(AES_ENC, nwkkey, 16);
    os_getDevKey(AESkey);
    os_aes(AES_ENC, artkey, 16);
}*/
void aes_2_sessKeys (u1_t* mes) {
    os_clearMem(lora.NwkSKey, 16);
    lora.NwkSKey[0] = 0x01;
    os_copyMem(lora.NwkSKey+1, &mes[1], 3+3);   // Appnonce, NetID
    os_wlsbf2(lora.NwkSKey+1+3+3, lora.DevNonce);
    os_copyMem(lora.AppSKey, lora.NwkSKey, 16);
    lora.AppSKey[0] = 0x02;
    os_getDevKey(AESkey);
    os_aes(AES_ENC, lora.NwkSKey, 16);
    os_getDevKey(AESkey);
    os_aes(AES_ENC, lora.AppSKey, 16);
    
    printf("\tDevNonce: %04X.\n", lora.DevNonce);

    printf("\tNwkSKey: ");
    for(u1_t i=0; i<16; i++){
        printf("%02X:", lora.NwkSKey[i]);
    }
    printf("\n");
    printf("\tAppSKey: ");
    for(u1_t i=0; i<16; i++){
        printf("%02X:", lora.AppSKey[i]);
    }
    printf("\n");
}

bool Rx_JoinAccept(u1_t* mes, int mlen){

    if(mlen != LEN_JA && mlen != LEN_JAOPT){
        show_time2("RX JoinAccept Length Error.", mlen);
        return(0);
    }
    //aes_encrypt(mes+1, mlen-1);
    //rxFrm.Mic = os_rlsbf4(&mes[mlen-4]);

    aes_encrypt(mes+1, mlen-1);
    if( aes_verifyMic0(mes, mlen-4) == 0) {
        show_time("RX JoinAccept MIC Error.");
        return(0);
    }
    show_time("Recv JoinAccept, JOINED.");
    lora.join_st = JOINED;
    // Reset tx interval
    lora.tx_period = INTERVAL_KEEP_ALIVE;

    rxFrm.AppNonce = os_rlsbf4(mes+1) & 0xFFFFFF;    // 3bytes
    rxFrm.NetID = os_rlsbf4(mes+4) & 0xFFFFFF;
    rxFrm.DevAdr = os_rlsbf4(mes+7);
    rxFrm.DLSettings = mes[11];
    rxFrm.RxDelay = mes[12];
    radio.RX1DRoffset = rxFrm.DLSettings & 0x70 >> 4;
    radio.RX2Datarate = rxFrm.DLSettings & 0x0F;
    
    radio.OptNeg = rxFrm.DLSettings & 0x80 >> 7; /* for V1.1, Not used in this ver. */

    printf("\tAppNonce: \t%06X.\n", rxFrm.AppNonce);
    printf("\tNetID: \t%06X.\n", rxFrm.NetID);
    printf("\tDevAdr: \t%08X.\n", rxFrm.DevAdr);
    printf("\tOptNeg: %d.\n", radio.OptNeg);
    printf("\tRX1DRoffset: \t%d.\n", radio.RX1DRoffset);
    printf("\tRX2Datarate: \tDR%d.\n", radio.RX2Datarate);
    printf("\tRxDelay: \t%d.\n", rxFrm.RxDelay);
    // Check CFList
    if (mlen == LEN_JAOPT){ 
        printf("\tRecv CFList,\n");
        memcpy(rxFrm.CFList, &mes[13], 16);
        for(u1_t i=0; i<5; i++){
            u1_t j;
            j=i*3;
            rxFrm.Freq_Ch[i+2] = (u4_t)(u4_t)mes[j+13] | (u4_t)mes[j+14] << 8 | (u4_t)mes[j+15] << 16;
            //printf("\tCh%d=%02x:%02x:%02x.\n", i+2, mes[13+j+2], mes[13+j+1], mes[13+j]);
            printf("\t\tCh%d: %d00 Hz.\n", i+2, rxFrm.Freq_Ch[i+2]);
        }
        rxFrm.CFListType = mes[28];
        printf("\t\tCFListType: \t%02x.\n", rxFrm.CFListType);
    }
    fflush(stdout);
    aes_2_sessKeys( mes);
    lora.DevAddr = rxFrm.DevAdr;

    return(1);
}

bool Rx_DataMesg(u1_t* mes, int mlen){
    //rxFrm.MType = (mes[0] & MHDR_MTYPE) >> 5;    // bit 7,6,5
    rxFrm.Major = mes[0] & MHDR_MAJOR;    // bit0,1
    rxFrm.DevAdr = ( mes[4] << 24 | mes[3] << 16 | mes[2] << 8 | mes[1] );
    rxFrm.FCtrl = mes[5];
    // Frame Control
    rxFrm.ACK = (rxFrm.FCtrl & FCTRL_ACK)?1:0; // bit5

    rxFrm.FOptLen = mes[5] & FCTRL_FOPTLEN;
    rxFrm.FCnt = (uint32_t)( mes[7] << 8 | mes[6] );
    if(rxFrm.FOptLen > 0){     // FOpts present
        memcpy(rxFrm.FOpts, &mes[8], rxFrm.FOptLen);
    }
    rxFrm.FPort = mes[8+rxFrm.FOptLen];     // 0:MAC command only, 1-223(0xDF): application specific, 224:test
    memcpy(rxFrm.FRMPayload, &mes[9 + rxFrm.FOptLen], mlen - 9 - rxFrm.FOptLen - 4);

    rxFrm.Mic =  ( mes[mlen-4] << 24 | mes[mlen-3] << 16 | mes[mlen-2] << 8 | mes[mlen-1] );
    rxFrm.Crc = 0;  // not use crc in down link
    lora.FCntDown = rxFrm.FCnt;
    if(rxFrm.FPort == 0){       // TODO MUST add the case FPort absent
        lora.NFCntDown = rxFrm.FCnt;
    }else{
        lora.AFCntDown = rxFrm.FCnt;
    }

    printf("\tLen:%d, MT:%x, Mj:%x, DA:%08x, FCntrl:%x, FCnt:%d, FOptLen:%d, FPort:%d, MIC:%x\n",\
        mlen,rxFrm.MType,rxFrm.Major,rxFrm.DevAdr,rxFrm.FCtrl,rxFrm.FCnt,rxFrm.FOptLen,rxFrm.FPort,rxFrm.Mic);    
    
    printf("\tFRMPayload: ");
    for(int i=0; i< (mlen-13-rxFrm.FOptLen); i++){
        printf("%02x:",rxFrm.FRMPayload[i]);
    }
    printf("\n");
    
    if(rxFrm.DevAdr != lora.DevAddr){
        show_timeh("To Another Node, ",rxFrm.DevAdr);
        return(0);
    }

    if( aes_verifyMic( lora.NwkSKey, rxFrm.DevAdr, rxFrm.FCnt, 0x01, mes , mlen-4) == 0){
        show_time("MIC Error.");
        return(0);
    };
    // we get data! 
    packet_t phy_pld;
    phy_pld.bytes = mlen -13-rxFrm.FOptLen;
    u1_t* phypld_p = mes+9+rxFrm.FOptLen;
    aes_cipher(rxFrm.FPort == 0 ? lora.NwkSKey : lora.AppSKey, rxFrm.DevAdr, rxFrm.FCnt, /*dn*/1, phypld_p, phy_pld.bytes); //mlen-13-rxFrm.FOptLen);  //-4-8-1-rxFrm.FOptLen);
    memcpy(phy_pld.buf, phypld_p, phy_pld.bytes); //  
    // Set ACK on next tx frame   
    if(rxFrm.MType == DATA_CFMD_DN){
        txFrm.ACK |= FCTRL_ACK;   // bit5
    }else{
        txFrm.ACK &= (~FCTRL_ACK);
    }
    show_time2("Data Received. byte(s):",phy_pld.bytes);
    printf("\tPayload:");
    for( u1_t i=0;i<phy_pld.bytes; i++){
        printf("%02x:",phy_pld.buf[i]);
    }
    printf("\n");

    if(rxFrm.FPort == 0){   // ver 1.1
        show_timeh("Rcv MAC CMD: ",rxFrm.FRMPayload[0]);    // CID?
        ////process_Mac();  // TODO
        lora.NFCntDown +=1;
    }else{
        lora.AFCntDown +=1;
    }
    lora.FCntDown +=1;      // ver 1.0.3

    // FoptLen > 1 Piggybacked
    if( rxFrm.FOptLen > 1){
        // NEED to process mac command
        show_timeh("Rcv Piggied MAC CMD: ",rxFrm.FOpts[0]);    // CID?
        ////process_Mac();  // TODO
    }
    return(1);
}

bool Decode_rxFrame(u1_t * mes, int mlen){
    bool result;
    rxFrm.MType = (mes[0] & MHDR_MTYPE) >> 5; //bit7-5

    if(rxFrm.MType == JOIN_ACCEPT && lora.join_st == JOINING){
        result = Rx_JoinAccept(mes,mlen);
        return(result);
    }else if( lora.join_st == JOINED && ( rxFrm.MType == DATA_UNCFMD_DN || rxFrm.MType == DATA_CFMD_DN) ){
        result = Rx_DataMesg(mes,mlen);
        return(result);
    }else{
#ifdef Debug_Frame
        show_tm();
        printf(" ?:MType: %0x, mlen: %d, State: %0x.\n\t", rxFrm.MType, mlen, lora.join_st);
        for(u1_t i=0; i<mlen;i++){
            printf("%02x:",mes[i]);
        }
        printf("\n");
#endif
        return(0);
    }
}

u2_t Gen_DevNonce(){
    srand((unsigned)time(NULL));
    lora.DevNonce = rand()%0xffff;  // 2bytes
    printf("\tDevNonce: %04X.\n", lora.DevNonce);
    return( lora.DevNonce);
}

void Build_JoinRequest(packet_t* txbuf_p){
    // MHDR,Mac Header
    txFrm.MType = JOIN_REQUEST;
    txFrm.Major = 0b00;     //LoRaWAN R1
    txFrm.DevNonce = Gen_DevNonce();
    //printf("DevNonce=%d\n",txFrm.DevNonce);
    txbuf_p->buf[0] = (byte)((byte)txFrm.MType << 5 | (byte)txFrm.Major);
    memcpy(txbuf_p->buf+1, APPEUI, 8 );
    memcpy(txbuf_p->buf+9, DEVEUI, 8 );
    os_wlsbf2(txbuf_p->buf+17, txFrm.DevNonce);
    aes_appendMic0(txbuf_p->buf, 19);
    txbuf_p->bytes = 23;
}

void Build_EmptyFrame(packet_t* txbuf_p){
    
    // Only set the MHDR correctly
    // MHDR 1
    txFrm.MType = PROPRIETARY;
    txFrm.Major = 0b00;     //LoRaWAN R1
    txbuf_p->buf[0] = (byte)((byte)txFrm.MType << 5 | (byte)txFrm.Major);
    // FHDR // DEVADDR 4
    txFrm.DevAdr = 0;
    os_wlsbf4(txbuf_p->buf+1,  txFrm.DevAdr);
    // FCTRL 1 ,FCNT 2
    txbuf_p->buf[5] = 0;
    txbuf_p->buf[6] = 0;
    txbuf_p->buf[7] = 0;
    //  MIC 4  
    txbuf_p->buf[8] = 0;
    txbuf_p->buf[9] = 0;
    txbuf_p->buf[10] = 0;
    txbuf_p->buf[11] = 0;
    txbuf_p->bytes =  1+4+1+2+4;    // MHDR DEVADDR FCTRL FCNT MIC
    // Absent FPort,FRMPayload
}

void Build_txFrame(  packet_t* txbuf_p, u1_t* tx_mes, int tx_mlen){

    txFrm.MType = DATA_UNCFMD_UP; //0x02; // 0x02=Unconfirmed Data Up
    txFrm.Major = LORAWAN_R1;     // 0x00=LoRaWANR1
    txFrm.DevAdr = lora.DevAddr; //DEVADDR;
    txFrm.FCtrl = txFrm.ACK;   // ADR | ADRACKReq | ACK | classB | FOptLen
    lora.FCntUp +=1; 
    txFrm.FCnt = lora.FCntUp;   //16bit
    txFrm.FPort = 1;        // 0=MAC command, 1-223=application specific, 224=test

    txbuf_p->buf[0] = txFrm.MType << 5 | txFrm.Major;
    os_wlsbf4(txbuf_p->buf+1,  txFrm.DevAdr);
    txbuf_p->buf[5] = txFrm.FCtrl;
    os_wlsbf2(txbuf_p->buf+6, txFrm.FCnt);
    // Let FOpts=0
    txbuf_p->buf[8] = txFrm.FPort;
    for(int i=0; i<tx_mlen; i++){
        txbuf_p->buf[i+9] = tx_mes[i];
    }
    txbuf_p->buf[tx_mlen+9] = '\0';
   
    //aes_cipher( txFrm.FPort==0 ? DEVKEY : ARTKEY, txFrm.DevAdr, txFrm.FCnt, /*up*/0, &txbuf_p->buf[9], tx_mlen);
    //aes_appendMic(DEVKEY, txFrm.DevAdr, txFrm.FCnt, /*up*/0, txbuf_p->buf, tx_mlen+9);
    aes_cipher( txFrm.FPort==0 ? lora.NwkSKey : lora.AppSKey, txFrm.DevAdr, txFrm.FCnt, /*up*/0, &txbuf_p->buf[9], tx_mlen);
    aes_appendMic(lora.NwkSKey, txFrm.DevAdr, txFrm.FCnt, /*up*/0, txbuf_p->buf, tx_mlen+9);
    
    printf("\tMT:%02x,Mj:%x,DA:%04x,FCntrl:%x,FCnt:%d,FOptLen:%d,FPort:%d\n",\
        txFrm.MType,txFrm.Major,txFrm.DevAdr,txFrm.FCtrl,txFrm.FCnt,txFrm.FOptLen,txFrm.FPort);
    
    txbuf_p->bytes =  tx_mlen+9+4; //+1+4+1+2+1+4
    txFrm.ACK = 0;  // reset ACK
}

// end of frame.c