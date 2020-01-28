#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
  WiFi.mode(WIFI_STA);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  delay(1000);
}

void loop() {
  digitalWrite(5, HIGH);
  int n=WiFi.scanNetworks();
  digitalWrite(5, LOW);
  
  digitalWrite(7, HIGH);
  for(int i=0;i<n;i++){
      String ssid = WiFi.SSID(i);
      if(ssid.startsWith("D")&&ssid.endsWith("*")){
        Serial.printf("%d: %s, Ch:%d (%ddBm)\n\r", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i));
      }//if
    }//for
   digitalWrite(7, LOW);
  
}
