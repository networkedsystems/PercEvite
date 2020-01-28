#include <ESP8266WiFi.h>

extern "C" {
  #include "user_interface.h"
}

int n_beacons;
int t_beacons;
unsigned int sequence = 0;
char seq_buf[7];//buffer for sequence to char[] conversion
byte channel;
int start = 0;
int go = 0;

int delta;
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
   channel = 8;//random(1,14); 
   packet[59] = channel;
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
    n_beacons = 1;//beacons 
    //(TRx/Nrep)-Nch*(packet_duration)
    t_beacons =133;// round((2128/n_beacons)-14.854);//ms
    Serial.printf("Nrep=%d\n",n_beacons);
    Serial.printf("DeltaTx=%d\n",t_beacons);
    //Beacon message
    packet[38] = 'D';
    seq_buf[1] = packet[39] = '=';
    seq_buf[2] = packet[40] = '=';
    seq_buf[3] = packet[41] = '=';
    seq_buf[4] = packet[42] = '=';
    seq_buf[5] = packet[43] = '=';
    seq_buf[6] = packet[44] = '=';
    packet[45] = '*';
    packet[46] = '*';
    
  
}


void random_mac(){
  // Randomize SRC MAC
    packet[10] = packet[16] = random(256);
    packet[11] = packet[17] = random(256);
    packet[12] = packet[18] = random(256);
    packet[13] = packet[19] = random(256);
    packet[14] = packet[20] = random(256);
    packet[15] = packet[21] = random(256);
}

void broadcastGPS(){
  
  wifi_promiscuous_enable(1); 

   if(n_beacons > 10){
        seq_buf[1] = packet[39] = '=';
        seq_buf[2] = packet[40] = 's';
        seq_buf[3] = packet[41] = 't';
        seq_buf[4] = packet[42] = 'o';
        seq_buf[5] = packet[43] = 'p';
        seq_buf[6] = packet[44] = '=';
        
      }
      else{
        itoa(sequence,seq_buf,10);
      
      int l = strlen(seq_buf);
      
      for(int i =0;i<7;i++){
        if(i<l){
          packet[39+i] = seq_buf[i];  
        }
        else{
          packet[39+i] = '*';
          }
        }
      }
         
      
    
   
    packet[46] = '*';
    
     // Randomize SRC MAC
    packet[10] = packet[16] = random(256);
    packet[11] = packet[17] = random(256);
    packet[12] = packet[18] = random(256);
    packet[13] = packet[19] = random(256);
    packet[14] = packet[20] = random(256);
    packet[15] = packet[21] = random(256);

    for(int i=0;i<n_beacons;i++){
      for(channel = 1;channel < 15;channel++){
        wifi_set_channel(channel);
        packet[59] = channel;
        random_mac();
        wifi_send_pkt_freedom(packet, 60, 0);
        delay(1);
      }
      delay(t_beacons);
    }
    
   
}

void loop() {
  
    broadcastGPS();

    delay(random(2124)+1);
    
    if(sequence == 10000){
        sequence = 0;
        //n_beacons += 1;
        t_beacons = 133;//round((2124/n_beacons)-14.854);
        if(t_beacons <= 0){
          t_beacons = 0;
        }
        Serial.printf("Nrep=%d\n\r",n_beacons);
        Serial.printf("DeltaTx=%d\n\r",t_beacons);
        int D = 500+random(5000);
        
        Serial.println(D);
        Serial.println("stop");
        delay(D);
               
    }
    sequence+=1;
    
}
