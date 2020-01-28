#include <WiFi.h>
#include "esp_wifi.h"


esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

uint8_t channel = 0;

int r;

char buffer[28] = {'*','*','*','*',
                   '*','*','*','*',
                   '*','*','*','*',
                   '*','*','*','*',
                   '*','*','*','*',
                   '*','*','*','*',
                   '*','*','*', '*'};

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
  //digitalWrite(21, HIGH);
  int numSsid = WiFi.scanNetworks(false, true, true, Ts, ch);
  
  for (int j = 0; j < numSsid; j++) {
    String ssid = WiFi.SSID(j);
    if(ssid.startsWith("D")){
      Serial.print(ssid+"\t");
      Serial.print("rssi:");
      Serial.println(WiFi.RSSI(j));
    
    }
  
  }//for
 //digitalWrite(21, LOW);
}//scan()

void broadcastSSID(){
  
  //digitalWrite(21, HIGH);
  random_mac();
  
  
  for(int c = 1;c<15;c++){
    
    packet[80] = c;
    esp_wifi_set_channel(c,WIFI_SECOND_CHAN_NONE);      
    esp_wifi_80211_tx(WIFI_IF_AP, packet, 81, false);
    
    delay(1);  
  }//14 channels
  delay(8);
  //digitalWrite(21, LOW);
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  WiFi.mode(WIFI_AP_STA);
 
  //Set channel
  channel = random(1,14); 
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  packet[38] = 'D';
  packet[39] = 'N';
  
  //Select external antenna
  //pinMode(15,OUTPUT);
  pinMode(21,OUTPUT);
  digitalWrite(21, LOW);//ceramic/printed antenna on (default)
  //digitalWrite(15, LOW);//ceramic/printed antenna on (default)

}

void loop() {

   r = random(0,9999);
    //if(r < 3226){//20Tx, 80Rx
    //if(r < 4615){ //30Tx,70Rx
    //if(r < 5714){ //40Tx,60Rx
    if(r < 6667){ //50Tx,50Rx
    //if(r < 7500){ //60Tx,40Rx
    //if(r < 8235){ //70Tx,30Rx
    //if(r < 8889){ //80Tx,20Rx
      unsigned long start = micros();
      //digitalWrite(15, HIGH);
      Serial.println('>'); //Used to synchronize UART communication
      Serial.write(17);//XON
      int b = Serial.readBytesUntil('*',&packet[39], 26);  
      Serial.write(19);//XOFF    
      broadcastSSID();//B   
      //digitalWrite(15, LOW);
      unsigned long stop = micros();
      Serial.printf("dur: %u\r\n",stop-start);
        }
   else{
    unsigned long start = micros();
    Serial.println('S');
    scan(channel,60);//S
    unsigned long stop = micros();
    Serial.printf("s-dur: %u\r\n",stop-start);
   }
   
  
  
  
}
