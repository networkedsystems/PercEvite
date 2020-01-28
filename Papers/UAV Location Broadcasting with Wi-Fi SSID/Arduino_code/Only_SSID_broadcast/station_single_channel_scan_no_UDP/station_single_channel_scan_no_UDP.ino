#include <ESP8266WiFi.h>


uint8_t channel=8;


void setup() {
  Serial.begin(115200);
  Serial.println();
  
  
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  WiFi.mode(WIFI_STA);


    
    channel = 3;
    Serial.printf("Selected channel= %d\n\r",channel);

}


void loop() {
 
  int n = WiFi.scanNetworks(false, true, random(1,14));
    for(int i=0;i<n;i++){
      String ssid = WiFi.SSID(i);
    // if(ssid.startsWith("D")&&ssid.endsWith("*")){
        Serial.printf("%d: %s, Ch:%d \n\r", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i));
      //}//if
    }//for
}
