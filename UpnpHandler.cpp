#include "UpnpHandler.h"
#include <functional>
 
// Multicast declarations
IPAddress ipMulti(239, 255, 255, 250); //239,255,255,250
IPAddress serverIP(0, 0, 0, 0);
const unsigned int portMulti = 1900;
char packetBuffer[512];
String searchTarget = "urn:**";

long t_start = -20000;
long t_end = 0;


bool UpnpHandler::beginUdpMulticast()
{
	boolean state = false;

	//Serial.println("Beginning multicast...");

	if(UDP.beginMulticast(WiFi.localIP(), ipMulti, portMulti))
	{
		Serial.print("UDP multicast server started at ");
		Serial.print(ipMulti);
		Serial.print(":");
		Serial.println(portMulti);

		state = true;
	}
	else
	{
		Serial.println("Connection failed");
	}

	return state;
}

//TODO
/*void UpnpHandler::startWebServer() {
     server = new ESP8266WebServer(1900);

     server->on("/setup.xml", [&]() {
          handleSetupXml();
     });
     /*server->on("/eventservice.xml", [&]() {
          handleEventservice();
     });
     //server->onNotFound(handleNotFound);
     server->begin();

     Serial.println("WebServer started on port: ");
     Serial.println(localPort);
}*/

void UpnpHandler::findServer()
{
     beginUdpMulticast();
     while (serverIP[0] == 0)
     {
          upnpProcessor();
     }
     Serial.println("STOPPING ALL UDP PROCESSES");
     UDP.stopAll();
}

void UpnpHandler::setSearchTarget(String inSearchTarget)
{
     searchTarget = inSearchTarget;
}

IPAddress UpnpHandler::getServerIP()
{
     return serverIP;
}

void UpnpHandler::broadcastSearch(){
     String search = "M-SEARCH * HTTP/1.1\r\n"
                    "HOST: 239.255.255.250:1900\r\n"
                    "MAN: \"ssdp:discover\""
                    "ST: urn:Trullingham:device:hall\r\n"
                    "NTS: ssdp:alive\r\n"
                    "MX: 5\r\n"
                    "\r\n";

 
       for (int i=0; i < 3; i++)
       {
          UDP.beginPacket(ipMulti, 1900);
          UDP.write(search.c_str());
          UDP.endPacket();
       }
       upnpProcessor();
}

void UpnpHandler::upnpProcessor()
{

	int packetSize = UDP.parsePacket();
	if (packetSize <= 0)
     {
          return;
     }
	
	IPAddress senderIP = UDP.remoteIP();
	unsigned int senderPort = UDP.remotePort();

	// read the packet into the buffer
	UDP.read(packetBuffer, packetSize);
     
     
	// check if this is a M-SEARCH for Target Device
	String request = String((char *)packetBuffer);
     request.toLowerCase();
     if (request.indexOf("notify") >= 0)
     {
          if (request.indexOf(searchTarget) > 0)
          {
               serverIP = senderIP;
          }
     }
	
}
