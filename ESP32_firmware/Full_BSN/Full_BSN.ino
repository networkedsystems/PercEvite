#include <WiFi.h>
#include "esp_wifi.h"
#include <WiFiUdp.h>


unsigned int net_counter = 0;
const char* ssid = "PercEvite_Pi";
const char* password =  "hi64nyvy";
//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * host = "192.168.4.1";
const int port = 8000;

//The udp library class
WiFiUDP udp;
char incomingPacket[80];  // buffer for incoming packets
IPAddress local_IP(192, 168, 4, 12);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

//Broadcast parameters
uint8_t channel = 0;
int r;
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
unsigned long msg_counter = 0;

// Beacon Packet buffer
uint8_t packet[] = { 0x80, 0x00, 0x00, 0x00,
               /*Destination MAC*/
               /*4*/ 0xff, 0xff, 0xff, 0xff, 
               /*8*/ 0xff, 0xff,
               /*Source MAC*/ 
               /*10*/0x01, 0x02, 0x03, 0x04, 
               /*14*/0x05, 0x06,
               /*BSS ID*/
               /*16*/0x01, 0x02, 0x03, 0x04, 
               /*20*/0x05, 0x06, 0xc0, 0x6c,
               /*Timestamp*/ 
               /*24*/0x83, 0x51, 0xf7, 0x8f,
               /*28*/0x0f, 0x00, 0x00, 0x00, 
               /*32*/0x64, 0x00,/*Beacon interval*/ 
               /*34*/0x01, 0x04,/*Capability info*/ 
               /* SSID=ELEMENT ID> length> 28 bytes*/
               /*36*/0x00, 30, 
               /*38*/'D', 'N', '*', '*', 
               /*42*/'*', '*', '*', '*',
               /*46*/'*', '*', '*', '*',
               /*50*/'*', '*', '*', '*',
               /*54*/'*', '*', '*', '*',
               /*58*/'*', '*', '*', '*',
               /*62*/'*', '*', '*', '*',
               /*66*/'*', '*',
               /*Supported rates*/
               /*68*/0x01, 0x08, 0x82, 0x84,
               /*72*/0x8b, 0x96, 0x24, 0x30, 
               /*76*/0x48, 0x6c, 0x03, 0x01, 
               /*80*/0x04};        


void random_mac(){
  // Randomize SRC MAC
    packet[10] = packet[16] = random(256);
    packet[11] = packet[17] = random(256);
    packet[12] = packet[18] = random(256);
    packet[13] = packet[19] = random(256);
    packet[14] = packet[20] = random(256);
    packet[15] = packet[21] = random(256);
}
void scan(uint8_t ch, uint8_t Ts){

  int numSsid = WiFi.scanNetworks(false, true, true, Ts, ch);
  
  for (int j = 0; j < numSsid; j++) {
    String ssid = WiFi.SSID(j);
    if(ssid.startsWith("D")){
      Serial.println(ssid);
    
    }
    
  }//for
  WiFi.scanDelete();

}//scan()

void broadcastSSID(){
  //digitalWrite(21, HIGH);
  random_mac();
  
  msg_counter++;
    
  String msg = "2-"+String(msg_counter);
  memcpy(&packet[39],msg.c_str(),msg.length());
 
  esp_wifi_set_promiscuous(true);
  for(int c = 1;c<15;c++){
    
    packet[80] = c;
    esp_wifi_set_channel(c,WIFI_SECOND_CHAN_NONE);      
    uint32_t err = esp_wifi_80211_tx(WIFI_IF_STA, packet, 81, true);
       
    delay(2);  
  }//14 channels
  delay(3);
  //delay(10);

}

void netCom(){
   udp.begin(WiFi.localIP(),port);
   //Send a packet
   //for(net_counter = 0;net_counter <2; net_counter++){
    udp.beginPacket(host,port);
    udp.printf("*P-%u-Heap: %u*T: %u*",net_counter,ESP.getFreeHeap(),micros());
    net_counter++;
    udp.endPacket();
    delay(100);
   //}
    udp.flush();
    udp.stop();
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  WiFi.mode(WIFI_STA);
 
  //digitalWrite(21, HIGH); // external antenna on WiPy 3.0
  digitalWrite(21, LOW); // chip antenna on WiPy 3.0
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    
  }
  Serial.println(WiFi.localIP());
  
  channel = random(1,14);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  packet[38] = 'D';
  packet[39] = 'N';
}

void wifi_connect(){
  if(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);
    }
    while (WiFi.status() != WL_CONNECTED) {
      delay(2);
      Serial.print('.');
    }
    Serial.println(WiFi.localIP());
    
}
void loop() {
    
    r = random(1,10000);

    if(r < 6515){
      //Broadcast
      for(int k=0;k < 26; k++){
        packet[39+k] = '*';
      }
      
      unsigned long start = micros();
      broadcastSSID();//B   
      unsigned long stop = micros();
      Serial.printf("B-dur: %u; ssid: %s\r\n",stop-start,"2-"+String(msg_counter));
      
    }
    else if(r > 6514 && r < 6733){
      //Network
      unsigned long start = micros();
      
      netCom();
      delay(100);
      
      unsigned long stop = micros();
      Serial.printf("N-dur: %u\r\n",stop-start);

      }
     else{
      //Scan
       unsigned long start = micros();
       scan(channel,60);//S
       unsigned long stop = micros();
       Serial.printf("S-dur: %u\r\n",stop-start);
     }
    
  
   
  
}
