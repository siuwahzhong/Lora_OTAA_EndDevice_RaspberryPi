/*******************************************************************************
 * Copyright (c) 2019-2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// radio.c
#include <stdio.h>
#include <assert.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "enddevice.h"
#include "radio.h"
#include "frame.h"
#include "showtime.h"

//raspi SPI Channel
static const int CHANNEL = 0;

// Radio parameters
radio_t radio;
number nb;
pin_map pins;

void init_nb(){
    nb.rx_recv = 0;
    nb.rx_bad = 0;
    nb.rx_ok = 0;
    nb.tx_snd = 0;
}

void SetupwiringPiSPI(){
    wiringPiSPISetup(CHANNEL, 1E6);
}

void init_pins(){
    // Pin mapping for DRAGINO LoRa Shield
    pins.nss = 6;
    pins.rst = 0;
    pins.dio0 = 7;
    pinMode(pins.nss, OUTPUT);
    pinMode(pins.dio0, INPUT);
    pinMode(pins.rst, OUTPUT);
}

// 0,1=OUTPUT, 2=INPUT
void rst_pin(u1_t val){
    if(val == 0 || val == 1){       // drive pin
        pinMode(pins.rst, OUTPUT);
        digitalWrite(pins.rst, val);
    }else{                          // keep pin floating
        pinMode(pins.rst, INPUT);
    }
}

void selectreceiver(void){
    digitalWrite(pins.nss, LOW);
}
void unselectreceiver(void){
    digitalWrite(pins.nss, HIGH);
}

byte readRegister(byte addr){
     unsigned char spibuf[2];

    selectreceiver();
    spibuf[0] = addr & 0x7F;
    spibuf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);
    unselectreceiver();
#ifdef Debug_Register
    printf("\t\tAddr: %02x Read: %02x\n", addr,spibuf[1]);
#endif
    return spibuf[1];
}

void writeRegister(byte addr, byte value){
    unsigned char spibuf[2];

    spibuf[0] = addr | 0x80;
    spibuf[1] = value;
    selectreceiver();
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);

    unselectreceiver();
#ifdef Debug_Register
    printf("\t\tAddr: %02x Write: %02x\n", addr,value);
#endif
}

void Set_Opmode(byte opmode){
    writeRegister(REG_OPMODE, opmode);
}

void Set_SyncWord(){
    writeRegister(REG_SYNC_WORD, LORA_SYNC_WORD);
}

void Setup_LNA(){
    writeRegister(REG_LNA, LNA_MAX_GAIN);  // max lna gain
}

byte Rst_radio(){
    // Manual Reset
    rst_pin(0);
    //digitalWrite(pins.rst, LOW);
    delay(1);   // keep Low while 100uS
    rst_pin(2);
    //digitalWrite(pins.rst, HIGH);
    delay(10);  // wait 5mS
    // Set opmode in Lora/sleep
    Set_Opmode(SX_MODE_SLEEP);
    byte version = readRegister(REG_VERSION);
    return(version);
}
void Setup_FIFO(){
    writeRegister(REG_MAX_PAYLOAD_LENGTH,MAX_PAYLOAD_LENGTH);
    writeRegister(REG_PAYLOAD_LENGTH,PAYLOAD_LENGTH);
    writeRegister(REG_HOP_PERIOD,0xFF);
    writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_BASE_AD));
}

bool Init_radio(void){
    wiringPiSetup();
    init_pins();
    SetupwiringPiSPI();
    // reset SX1276 and set Sleep mode
    byte version = Rst_radio();
    if(version != SX1276_VerCode){    
        return(0);
    }
    radio.freq = FREQUENCY_REGION;
    radio.txpwr = RF_PWR_REGION;
    radio.sf = SF_;
    
    // Lora+Sleep
    writeRegister(REG_OPMODE, SX_MODE_SLEEP);
    // Freq
    Set_Frequency();
    // RF Power
    Set_Power();
    // set Sync Word
    Set_SyncWord();
    Set_DataRate();
    // setup FIFO
    Setup_FIFO();
    //　Set LNA
    Setup_LNA();
    // Init frame counters
    init_nb();
    return(1);
}

void Set_Frequency(){

    u8_t frf = ((u8_t)radio.freq << 19) / 32000000;
    writeRegister(REG_FRF_MSB, (uint8_t)(frf>>16) );
    writeRegister(REG_FRF_MID, (uint8_t)(frf>> 8) );
    writeRegister(REG_FRF_LSB, (uint8_t)(frf>> 0) );
}

void Set_Power(){
//  ---SELECT PA_BOOST----
//  SX1276 RegPaConfig(0x09)    Val=1.011.1011=0xbb
//  bit 7 	PaSelect = 1		select PA_BOOST (not RFO!)
//  bit 6-4	MaxPower = 4		Pmax=10.8+0.6*4=13.2
//  bit 3-0 OutputPower     	Pout=17-(15-OutputPower)
//                              OutputPower = Pout - 2
    u1_t pwr = (u1_t)( 0xc0 | (0x0f&(radio.txpwr-2)));
	writeRegister(REG_PA_CFG, pwr);
    // Config PA_BOOST
    //writeRegister(REG_PA_DAC, readRegister(REG_PA_DAC)|0x04);
    writeRegister(REG_PA_RAMP, 0x08); // set PA ramp-up time 50 uSec
}

void Set_DataRate(){
    u1_t sf;
    sf = radio.sf;
    if (sf == SF11 || sf == SF12) {
        // LowDataRateOptimize + AGC
        writeRegister(REG_MODEM_CONFIG3,0x0C);
    } else {
        // internal AGC loop
        writeRegister(REG_MODEM_CONFIG3,0x04);
    }
    // bw=125kHz, coding rate=4/5, Explicit header
    writeRegister(REG_MODEM_CONFIG1,0x72);
    // SF rate, Enable CRC 
    writeRegister(REG_MODEM_CONFIG2,(sf<<4) | 0x04);
    
    if (sf == SF10 || sf == SF11 || sf == SF12) {
        writeRegister(REG_SYMB_TIMEOUT_LSB,0x05);
    } else {
        writeRegister(REG_SYMB_TIMEOUT_LSB,0x08);
    }
}

void Cfg_Modem(){
    // 8 Symbols 
    writeRegister(REG_PREAMBLE_LENGTH_LSB, 0x08);
    // datar
    // code rate
    // crc
    // ipol
    uint8_t mc1=0, mc2=0, mc3=0;
    mc1 |= 0x70;    //SX1276_MC1_BW_125;
    mc1 |= 0x02;    //SX1276_MC1_CR_4_5;
    writeRegister(REG_MODEM_CONFIG1, mc1);

    mc2 |= radio.sf << 4; //0x70;    //0x70
    mc2 |= 0x04;    //SX1276_MC2_RX_PAYLOAD_CRCON;
    writeRegister(REG_MODEM_CONFIG2, mc2);
    // RX AGC AUTO    
    mc3 |= SX_MC3_AGCAUTO;
    writeRegister(REG_MODEM_CONFIG3, mc3);    
    // ipol
    writeRegister(REG_INV_IQ, readRegister(REG_INV_IQ)| SX_INV_IQ_TX);
}

void Tx_Frame(packet_t* txpkt_p){
    u1_t i;
    printf("\t%d bytes PHYPayload to tx.", txpkt_p->bytes);
    for( i=0; i< txpkt_p->bytes; i++){
        printf("%02x:",txpkt_p->buf[i]);
    }
    printf("\n");
    
    nb.tx_snd++;
    Set_Opmode(SX_MODE_STDBY);
    Cfg_Modem();
    Set_Frequency();

    // set sync word already done
    Set_SyncWord();
    // set the IRQ mapping DIO0=TxDone DIO1=NOP DIO2=NOP
    // bit7-6:DIO0 00=RxDone,01=TxDone,10=CadDone
    writeRegister(REG_DIO_MAPPING_1, 0x40);
    // clear all radio IRQ flags
    writeRegister(REG_IRQ_FLAGS, 0xFF);
    // mask all IRQs but TxDone
    writeRegister(REG_IRQ_FLAGS_MASK, ~SX_IRQ_TXDONE_MASK);
    // initialize the payload size and address pointers    
    writeRegister(REG_FIFO_TX_BASE_AD, 0x00);
    writeRegister(REG_FIFO_ADDR_PTR, 0x00);
    //writeRegister(REG_FIFO_ADDR_PTR, REG_FIFO_TX_BASE_AD);
    writeRegister(REG_PAYLOAD_LENGTH, txpkt_p->bytes);
    // download buffer to the radio FIFO
    for( i = 0; i < txpkt_p->bytes; i++){
        writeRegister(REG_FIFO,txpkt_p->buf[i]);
    }
   
    // DEBUG
    /*
    for(i=1; i< 0x42; i++){
        printf(" REG%02x: %02x\n",i, readRegister(i));
    }
    */
    // now we actually start the transmission
    Set_Opmode(SX_MODE_TX);
}

int receivePkt(u1_t *payload){
    byte currentAddr;
    byte receivedCount;
    // clear rxDone
    writeRegister(REG_IRQ_FLAGS, 0x40);

    byte irqflags = readRegister(REG_IRQ_FLAGS);
    nb.rx_recv++;
    //  payload crc: 0x20
    if((irqflags & 0x20) == 0x20){
        show_time("Pkt Recved, but CRC error.");
        writeRegister(REG_IRQ_FLAGS, 0x20);
        nb.rx_bad++;
        return(0);
    } else {
        nb.rx_ok++;
        printf("\r");
        show_time("Recved radio packet");
        currentAddr = readRegister(REG_FIFO_RX_CURRENT_ADDR);
        receivedCount = readRegister(REG_RX_NB_BYTES);
        writeRegister(REG_FIFO_ADDR_PTR, currentAddr);
#ifdef Debug_Radio
        printf("\tSX FIFO DATA: ");
        for(int i = 0; i < receivedCount; i++){
            //char?
            payload[i] = (char)readRegister(REG_FIFO);
            printf("%02x:", payload[i]);
        }
        printf("\n");
#endif
    }
    return((int) receivedCount );
}
void Rx_Packet(packet_t* rxpkt_p){
    //DIO0: Map=00:RxDone
    //      Map=01:TxDone
    if(digitalRead(pins.dio0) == 1){
        rxpkt_p->bytes = receivePkt(rxpkt_p->buf);
        if(rxpkt_p->bytes > 1) {
            byte snr_val = readRegister(REG_PKT_SNR_VALUE);
            if( snr_val & 0x80 ){ // The SNR sign bit is 1
                // Invert and divide by 4
                snr_val = ( ( ~snr_val + 1 ) & 0xFF ) >> 2;
                radio.pkt_snr = -snr_val;
            }else{
                // Divide by 4
                radio.pkt_snr = ( snr_val & 0xFF ) >> 2;
            }
            radio.pkt_rssi = readRegister(REG_PKT_RSSI_VALUE) + SX_RSSI_CORR ;
            radio.rssi = readRegister(REG_RSSI_VALUE) + SX_RSSI_CORR;
            printf("\tPacket RSSI: %d, ", radio.pkt_rssi);
            printf("Packet SNR: %d, ", radio.pkt_snr);
            printf("RSSI: %d, ", radio.rssi);
            printf("Length: %d",(int)rxpkt_p->bytes);
            printf("\n");
            return;
        }else{
            return;
        } // received a message
    }else{
        rxpkt_p->bytes = 0;
        return;
    } // if dio0==1
}

// Check CH is free with period uS
bool is_Channel_free(){

    int rssi[2];
    int i=0;
    Set_Opmode(SX_MODE_RX_CONTINUOS);

    while(i<2){
        rssi[i] = (int)(readRegister(REG_RSSI_VAL));
        rssi[i] += SX_RSSI_CORR;
        i++;
        delayMicroseconds(CARRIER_SENSE_TIME);
    }

    if(rssi[0] > RSSI_CH_FREE || rssi[1] > RSSI_CH_FREE){
        show_time2("Channel is Busy. ", rssi[0]>rssi[1]?rssi[0]:rssi[1]);
        return(FALSE);    
    }else{
        show_time2("Channel is Free. ", rssi[0]>rssi[1]?rssi[0]:rssi[1]);
        return(TRUE);
    }
}

void Dump_Reg(){
    for(byte adrs=0; adrs<0x41; adrs++){
        readRegister(adrs);
    } 
}

void show_lora_radio(){
    //show_tm();
    printf("\tOperating SF%i, %.6lf Mhz.\n", radio.sf,(double)radio.freq/1000000);
}

void Try_txFrame(packet_t* txpkt_p){
    for (u1_t i=0; i< RETRY_TX; i++){
        bool rf_free = is_Channel_free();
        if(rf_free){
            Tx_Frame(txpkt_p);
            show_time("Tx Frame done.");
            break;
        }
        delay(10);      // retry wait 10mS // wiringPI function
    }
}

void start_Rx(){ 
    // clear radio IRQ flags
    writeRegister(REG_IRQ_FLAGS, 0xFF);
    //mask all radio IRQ flags but RxDone 
    writeRegister(REG_IRQ_FLAGS_MASK,  ~SX_IRQ_RXDONE_MASK);
    // Set DIO0 RxDone 
    writeRegister(REG_DIO_MAPPING_1, 0x00);
    // Set DevMode RX_Cont
    writeRegister(REG_OPMODE, SX_MODE_RX_CONTINUOS);
    //show_time("Tx Done, Start Rx.");
}

void start_RX1(){
    radio.freq = FREQUENCY_REGION;
    Set_Frequency();
    //TODO sf may be set by ADR
    radio.sf = SF_;
    Set_DataRate();
    start_Rx();
}

void start_RX2(){
    radio.freq = FREQ_RX2_REGION;
    Set_Frequency();
    radio.sf = SF_RX2_;
    Set_DataRate();
    start_Rx();
}
bool is_tx_Done(){
    bool tx_done;
    if( readRegister(REG_OPMODE) == SX_MODE_STDBY){
        tx_done = true;    
    }else{
        tx_done = false;
    }
    return(tx_done);
}

// end of radio.c