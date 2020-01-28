/*
License:
© Copyright 2018, Networked Systems group, ESAT-TELEMIC,KU Leuven.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by Networked Systems group,
   ESAT-TELEMIC,KU Leuven.
4. Neither the name of the Networked Systems group, ESAT-TELEMIC,KU Leuven nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ESAT-TELEMIC-KU Leuven, ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Networked Systems group, ESAT-TELEMIC,KU Leuven
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}

int n_beacons;
int t_beacons;
uint32_t sequence = 0;
char seq_buf[6];//buffer for sequence to char[] conversion
//byte channel;
byte B;
String ssid;
uint8_t encryptionType;
int32_t RSSI;
uint8_t* BSSID;
int32_t channel;
bool isHidden;
int p1 = 0;
int p2 = 0;
byte buffer[8];
// Beacon Packet buffer
uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, 
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                /*22*/  0xc0, 0x6c, 
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, 
                /*32*/  0x64, 0x00, 
                /*34*/  0x01, 0x04, 
                /* SSID */
                /*36*/  0x00, 0x09, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72,0x72, 0x72,0x72,
                        0x01, 0x08, 0x82, 0x84,
                        0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01, 
                /*59*/  0x04};                       


void random_mac(){
  // Randomize SRC MAC
    packet[10] = packet[16] = random(256);
    packet[11] = packet[17] = random(256);
    packet[12] = packet[18] = random(256);
    packet[13] = packet[19] = random(256);
    packet[14] = packet[20] = random(256);
    packet[15] = packet[21] = random(256);
}

void broadcastSSID(){
    
  itoa(sequence,seq_buf,10);
      
  int l = strlen(seq_buf);
      
  for(int i =0;i<7;i++){
    if(i<l){
      packet[40+i] = seq_buf[i];  
    }
    else{
      packet[40+i] = '*';
      }
  }
    
  //Closing character 
    packet[46] = '*';
      
     for(int i=0;i<n_beacons;i++){
        for(int j =1; j<15;j++){
          //random_mac();
          wifi_set_channel(j);
          packet[59] = j;
          wifi_send_pkt_freedom(packet, 60, 0);
          
          delay(t_beacons);
      }
      
    }
    wifi_set_channel(channel);
    
}

void scan(){
  //Scan single channel
  int n=WiFi.scanNetworks(false,true,channel);
  
    for(int i=0;i<n;i++){
      WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
      String ssid = WiFi.SSID(i);
      
      if(ssid.startsWith("D")&&ssid.endsWith("*")){
        
        Serial.printf("%s, %d\n\r",ssid.c_str(), channel);
        
      }//if
    }//for
    WiFi.scanDelete();  
}

void setup() {
  Serial.begin(115200);
   //Set channel
   channel = 8; 
   //wifi_set_channel(channel);
   WiFi.setPhyMode(WIFI_PHY_MODE_11B);
   WiFi.mode(WIFI_AP_STA);
   WiFi.setOutputPower(10.0);
   random_mac();
   //channel
    packet[59] = channel;
    //n_beacons every t_beacons ms
    n_beacons = 4;
    t_beacons = 1;
    //Beacon message
    packet[38] = 'D';
    packet[39] = 'A';
    seq_buf[0] = packet[40] = '=';
    seq_buf[1] = packet[41] = '=';
    seq_buf[2] = packet[42] = '=';
    seq_buf[3] = packet[43] = '=';
    seq_buf[4] = packet[44] = '=';
    seq_buf[5] = packet[45] = '=';
    seq_buf[6] = packet[46] = '*';
    Serial.println(".");
    //Reset sequence
    Serial.setTimeout(1000);
    wifi_promiscuous_enable(1);
}

void loop() {
  p1 = random(0,7);
  p2 = random(8,15);
  
  for(int i=0;i<16;i++){
    if(i==p1 || i==p2){
      broadcastSSID();  
    }
    else{
      scan();
    }
    
    
  }
 
  Serial.readBytes(buffer, 8);
  //Advancing sequence
  sequence+=1;
  Serial.println("test");

 
}
