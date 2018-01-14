#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H
 
#include <Arduino.h>
#include <ESP8266WiFi.h>

class WifiHandler {
private:
     void storeMacAddress();
public:
     void startConnection(String inSSID, String inPassword);
     bool serverCheckIn();
     String getMacAddress();
     void setHost(String inHost);
     String getHost();
     void setUpnpSearchTarget(String searchTarget);
     void sendToServer(String message);
};
 
#endif
