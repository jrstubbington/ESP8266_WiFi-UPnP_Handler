#ifndef UPNPHANDLER_H
#define UPNPHANDLER_H
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

class UpnpHandler {
private:
        WiFiUDP UDP;  
public:
	bool beginUdpMulticast();
	void upnpProcessor();
     void broadcastSearch();
     void findServer();
     IPAddress getServerIP();
     void setSearchTarget(String inSearchTarget);
};
 
#endif
