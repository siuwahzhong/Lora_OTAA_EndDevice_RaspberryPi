Lora OTAA End Device for RPI and Dragino GPS HAT
====================================================

This Software is Lora OTAA Eed Device sample.

Tested only RPI3B+, Dragino GPS HAT (SX1276) and TTN.  

How to build:
-----------
Step1: SPI needs to be enabled on the Raspberry Pi  
    '$ sudo raspi-config'  
Step2: Install wiringPi with a command like,  
    '$ sudo apt-get install wiringPi'  
Step3: If send GPS data, install libgps from https://github.com/tsuyoshiohashi/libgps  
    Configire serial port name if necessary.  
Step4: UPDATE enddevice.h according to your environment.    
    DEVEUI,APPEUI,APPKEY  
    frequency,txpower  
    whether to use GPS module. If not use, dummy GPS data will be used.  
Step5: $ make    
Step6: $ ./enddevice  

Feature:
-----------
This implementation is for personal experiments of a OTAA LoRaWAN enddevice  
 -Activate by OTAA  
 -GPS data is sent with cayenne-lpp foramat.    
 -TX/RX work like Class C (RX2 only)  
 -Downlink data available  
    --Display only, tested w/ Bidirectional_Single_ch_Pkt_Fwd only  

License:
-----------
The source files in this repository are made available under the EclipsePublic License v1.0, except for the aes implementation, that has been copied from the lmic_pi https://github.com/ernstdevreede/lmic_pi, 
cayenne_lpp has been copied from the cayenne_lpp https://github.com/aabadie/cayenne-lpp  
