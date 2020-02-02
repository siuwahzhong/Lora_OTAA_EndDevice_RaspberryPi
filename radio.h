/*******************************************************************************
 * Copyright (c) 2019-2020 Tsuyoshi Ohashi.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *******************************************************************************/
// radio.h
#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include "enddevice.h"
#include "frame.h"
// Remove comment when debug
//#define     Debug_Register
#define     Debug_Radio

#define     RETRY_TX 10

#define     REG_VERSION     0x42
#define     REG_FIFO        0x00
#define     REG_OPMODE     0x01
// frequency
#define     REG_FRF_MSB     0x06
#define     REG_FRF_MID     0x07
#define     REG_FRF_LSB     0x08
// RSSI
#define     REG_RSSI_VAL    0x1B
// Tx  PA Register
#define	    REG_PA_CFG	    0x09
#define     REG_PA_RAMP     0x0A
#define     REG_PA_DAC      0x4D
//
#define     REG_SYNC_WORD   0x39
// SF(Spread Factor)
#define REG_MODEM_CONFIG1           0x1D
#define REG_MODEM_CONFIG2           0x1E
#define REG_MODEM_CONFIG3           0x26
#define REG_SYMB_TIMEOUT_LSB  		0x1F
//
#define REG_PREAMBLE_LENGTH_LSB 0x21

#define REG_PAYLOAD_LENGTH      0x22
#define REG_MAX_PAYLOAD_LENGTH  0x23
#define REG_HOP_PERIOD          0x24
#define REG_FIFO_ADDR_PTR       0x0D
#define REG_FIFO_TX_BASE_AD     0x0E
#define REG_FIFO_RX_BASE_AD     0x0F
#define REG_DIO_MAPPING_1       0x40
#define REG_IRQ_FLAGS           0x12
#define REG_IRQ_FLAGS_MASK      0x11
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_RX_NB_BYTES     0x13

#define REG_LNA     0x0C
#define REG_INV_IQ  0x33

#define  REG_PKT_SNR_VALUE      0x19
#define  REG_PKT_RSSI_VALUE     0x1A
#define  REG_RSSI_VALUE         0x1B

// SX1276
#define SX1276_VerCode      0x12
// SX OP MODE
// We SELECT HIGH FREQ BAND (Above 779MHz)
#define     SX_MODE_SLEEP           0x80   // 10000000 0x80=Lora, !0x08=HF
#define     SX_MODE_STDBY           0x81   // 10000001
#define     SX_MODE_TX              0x83   // 10000011 TX
#define     SX_MODE_RX_CONTINUOS    0x85   // 10000101 RX_CONT
#define     SX_MODE_RXSINGLE        0x86   // 10000110 RX_SINGLE
// RSSI
#define     SX_RSSI_CORR        -157    // RSSI=-157+Rssi in HF mode
#define     SX_IRQ_TXDONE_MASK  0x08
// Lor Sync Word
#define     LORA_SYNC_WORD      0x34
//
#define     MAX_PAYLOAD_LENGTH  0x80
#define     PAYLOAD_LENGTH      0x40
//
#define     LNA_MAX_GAIN        0x23
#define     SX_MC3_AGCAUTO      0x04
#define     SX_INV_IQ_RX        0x40
#define     SX_INV_IQ_TX        0x01

#define     SX_IRQ_RXDONE_MASK  0x40

enum sf_t { SF7=7, SF8, SF9, SF10, SF11, SF12 };

typedef struct {
    u4_t freq;
    s1_t txpwr;
    s1_t pkt_snr;
    s1_t pkt_rssi;
    s1_t rssi;
    u1_t sf;
    u1_t RX1DRoffset;
    u1_t RX2Datarate;
    bool OptNeg;
} radio_t;

typedef struct {
    u1_t nss;
    u1_t rst;
    u1_t dio0;
} pin_map;

typedef struct {
    u4_t rx_recv;
    u4_t rx_bad;
    u4_t rx_ok;
    u4_t tx_snd;
} number;

void init_pins(void);
void rst_pin(u1_t);
void selectreceiver(void);
void unselectreceiver(void);
byte readRegister(byte);
void writeRegister(byte, byte);
void Set_Opmode(byte);
void Set_SyncWord();
byte Rst_radio(void);
void Setup_FIFO();
bool Init_radio(void);
void init_nb();

void Set_Frequency();
void Set_Power();
void Set_DataRate();
void Cfg_Modem();
void Tx_Frame(packet_t* );

int receivePkt(u1_t* );
void Rx_Packet(packet_t* );
bool is_Channel_free();
void Dump_Reg(void);
void show_lora_radio();
void Try_txFrame(packet_t*);
void start_Rx();
void start_RX1();
void start_RX2();
bool is_tx_Done();
#endif /* RADIO_H_INCLUDED */