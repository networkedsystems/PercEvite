#include <ESP8266WiFi.h>

long counter = 0;
void setup() {
  Serial.begin(115200);
  Serial.println();
  
  
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
  WiFi.mode(WIFI_STA);
 Serial.println("full scan");

}


void loop() {
 
  Serial.printf("Scan n:%d\n\r",counter);
  int n=WiFi.scanNetworks();
    for(int i=0;i<n;i++){
      String ssid = WiFi.SSID(i);
      if(ssid.startsWith("D") && ssid.endsWith("*")){
        Serial.printf("%s ch:%d\n\r",  WiFi.SSID(i).c_str(),WiFi.channel(i));
        
      }//if
    }//for    
    counter+=1;
  
}
