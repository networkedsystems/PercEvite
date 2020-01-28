#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

int n_beacons;
int t_beacons;
int sequence = 0;
char seq_buf[7];//buffer for sequence to char[] conversion
byte channel;
int start = 0;
int go = 0;
// Beacon Packet buffer
uint8_t packet[128] = { 0x80, 0x00, 0x00, 0x00, 
                /*4*/   0xff, 0xff, 0xff, 0xff, 
                /*8*/   0xff, 0xff, 0x01, 0x02, 
                /*12*/  0x03, 0x04, 0x05, 0x06,
                /*16*/  0x01, 0x02, 0x03, 0x04, 
                /*20*/  0x05, 0x06, 0xc0, 0x6c, 
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 
                /*28*/  0x0f, 0x00, 0x00, 0x00, 
                /*32*/  0x64, 0x00, 0x01, 0x04, 
                /* SSID */
                /*36*/  0x00, 0x09, 0x72, 0x72, 
                /*40*/  0x72, 0x72, 0x72, 0x72,
                /*44*/  0x72, 0x72,0x72,  0x01, 
                /*48*/  0x08, 0x82, 0x84, 0x8b, 
                /*52*/  0x96, 0x24, 0x30, 0x48, 
                /*56*/  0x6c, 0x03, 0x01, 0x04};                       


void setup() {
   Serial.begin(115200);
   //Set channel
   channel = random(1,13); 
   wifi_set_channel(channel);
   WiFi.setPhyMode(WIFI_PHY_MODE_11B);
   WiFi.mode(WIFI_STA);
  
   //Beacon fixed bytes
   // Randomize SRC MAC
    packet[10] = packet[16] = random(256);
    packet[11] = packet[17] = random(256);
    packet[12] = packet[18] = random(256);
    packet[13] = packet[19] = random(256);
    packet[14] = packet[20] = random(256);
    packet[15] = packet[21] = random(256);
    //channel
   
    //n_beacons every t_beacons ms
    n_beacons = 14;//it was 5;//beacons 
    t_beacons = 102;//ms 
    //Beacon message
    packet[38] = 'D';
    packet[39] = 'F';
    seq_buf[1] = packet[40] = '=';
    seq_buf[2] = packet[41] = '=';
    seq_buf[3] = packet[42] = '=';
    seq_buf[4] = packet[43] = '=';
    seq_buf[5] = packet[44] = '=';
    seq_buf[6] = packet[45] = '=';
    packet[46] = '*';
    
  
}

void broadcastGPS(){
  
  wifi_promiscuous_enable(1); 
    // SSID 
    if(sequence > 100000){
      seq_buf[1] = 's';
      seq_buf[2] = 't';
      seq_buf[3] = 'o';
      seq_buf[4] = 'p';
      seq_buf[5] = '*';
      
    }
    else{
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
      
    }
   
    packet[46] = '*';
    
    
   
    for(int i=0;i<n_beacons;i++){
      channel = i+1;
      wifi_set_channel(channel);
       packet[59] = channel;
      wifi_send_pkt_freedom(packet, 60, 0);
      
      delay(170);
    }
    
   
}

void loop() {
  broadcastGPS();
  Serial.println(sequence);
  sequence+=1;
  
  
    
    
}

