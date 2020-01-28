#include <WiFi.h>
#include "esp_wifi.h"


esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

uint8_t channel = 0;

int r;


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


void setup() {
  Serial.begin(230400);
  Serial.setTimeout(100);
  WiFi.mode(WIFI_AP_STA);
 
  //Set channel
  channel = random(1,14); 
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_max_tx_power(78);
  packet[38] = 'D';
  packet[39] = 'N';
  
  //Select external antenna
  pinMode(15,OUTPUT);
  //digitalWrite(21, HIGH);//External antenna on
  digitalWrite(15, LOW);//ceramic/printed antenna on (default)

}

void loop() {
    
    scan(channel,60);//S

}
