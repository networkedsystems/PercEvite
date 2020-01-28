#include <WiFi.h>
#include "esp_wifi.h"
const char* ssid = "PercEvite_P";
const char* password =  "hi64nyvy";
//IP address to send TCP data to:
// either use the ip address of the server or 
// a network broadcast address
/* Put IP Address details */
IPAddress local_ip(192,168,1,100);
IPAddress gateway(192,168,1,100);
IPAddress subnet(255,255,255,0);
char incomingPacket[80];  // buffer for incoming packets

WiFiUDP udp;

int rescounter = 2E6;
void setup() {
   Serial.begin(115200);
  Serial.setTimeout(100);
  WiFi.mode(WIFI_AP);
  
   WiFi.softAP(ssid, password, 8);
   WiFi.softAPConfig(local_ip, gateway, subnet);
   delay(100);
   
  udp.begin(WiFi.localIP(),8000);
  
}

void loop() {
  int packetSize = udp.parsePacket();
    if (packetSize)
    {
      // receive incoming UDP packets
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
      int len = udp.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = 0;
      }
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      
      udp.beginPacket(udp.remoteIP().toString().c_str(),udp.remotePort());
      udp.printf("Message received\r\n");
      udp.endPacket();
      
      udp.flush();
      

     
    }
    rescounter--;
    if(rescounter <= 0){
     ESP.restart();
    }

}
