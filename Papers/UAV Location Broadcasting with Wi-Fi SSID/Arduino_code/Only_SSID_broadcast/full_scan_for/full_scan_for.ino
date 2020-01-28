#include <ESP8266WiFi.h>




void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(12, OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  WiFi.setPhyMode(WIFI_PHY_MODE_11B);
  WiFi.mode(WIFI_STA);
 

}


void loop() {
 
  
  //int n=WiFi.scanNetworks();
  digitalWrite(12, HIGH);
  //delay(5);
  //digitalWrite(13, HIGH);
  //delay(5);
  //for(int c = 1;c<15;c++){
    int n = WiFi.scanNetworks();
  /*for(int i=0;i<n;i++){
      String ssid = WiFi.SSID(i);
      if(ssid.startsWith("D") && ssid.endsWith("*")){
        Serial.printf("%s ch:%d\n\r",  WiFi.SSID(i).c_str(),WiFi.channel(i));
        
      }//if
    }//for  */
  //}
  
    
  //delay(5);
  digitalWrite(12, LOW);
  //digitalWrite(13, LOW);
  delay(50);
  /*digitalWrite(12, HIGH);
  for(int c=1;c<15;c++){
    digitalWrite(13, HIGH);
    delay(100);
    /*int n = WiFi.scanNetworks(false, true, c);
    delay(10);
     
    digitalWrite(13, LOW); 
  }
  digitalWrite(12, LOW);*****/
  
    


}
