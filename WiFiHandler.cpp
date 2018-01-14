#define useUPnP 1
#include "WifiHandler.h"
#if (useUPnP)
     #include "UpnpHandler.h"
#endif
#include <EEPROM.h>
#include <functional>

char ssid[50]     = "";
char password[50] = "";

String macAddress = "";

String host = "";
IPAddress hostIP(255,255,255,255);

#if (useUPnP)
     UpnpHandler upnpHandler;
#endif

void WifiHandler::startConnection(String inSSID, String inPassword)
{
     EEPROM.begin(512);
     storeMacAddress();
     delay(10);

     inSSID.toCharArray(ssid, inSSID.length()+1);
     inPassword.toCharArray(password, inPassword.length()+1);
     
     Serial.print("\nConnecting to " + String(ssid));
     if (ssid != "")
     {
          WiFi.begin(ssid, password);
     }
     else
     {
          Serial.print("SSID cannot be blank");
     }
     
     while (WiFi.status() != WL_CONNECTED)
     {
          delay(500);
          Serial.print(".");
     }
     Serial.println("DONE\n");
     
     if (host.equals(""))
     {
          Serial.print("Reading Previous Host IP...");

          for (int i = 0; i < 4; ++i)
          {
               hostIP[i] = int(EEPROM.read(i));
               host += String(hostIP[i]);
               if (i < 3)
               {
                   host += '.';
               }
          }
          Serial.println("DONE\n");
     }
     bool serverResponse = false;
     if (hostIP[0] != 255)
     {
          Serial.print("Attempting to connect to previous host ");
          Serial.print(host);
          Serial.print("...");
          serverResponse = serverCheckIn();
          if (serverResponse)
          {
               Serial.println("DONE\n");
          }
          else
          {
               Serial.println("FAILED\n");
               host = "";
          }
     }
     #if (useUPnP)
          if (host.equals("") && !serverResponse)
          {
               Serial.println("Searching For New Host...");
               //upnpHandler.beginUdpMulticast();
               upnpHandler.findServer();
               host="";
               hostIP = upnpHandler.getServerIP();
               Serial.println("\nFOUND NEW HOST\n");
               
               Serial.print("Writing New Host IP....");
               for (int i = 0; i < 4; i++)
               {
                    if( EEPROM.read(i) != hostIP[i] )
                    {
                         EEPROM.write(i, hostIP[i]);
                    }
                    host += String(hostIP[i]);
                    if (i < 3)
                    {
                        host += '.';
                    }
               }
               Serial.println("DONE\n");
          }
     
          EEPROM.commit();
          EEPROM.end();
     #endif

     Serial.print("HOST: ");
     Serial.println(host);
     Serial.println("\n");

     Serial.println("Begin main loop:");
}

void WifiHandler::setUpnpSearchTarget(String searchTarget)
{
     #if (useUPnP)
          upnpHandler.setSearchTarget(searchTarget);
     #endif
}

void WifiHandler::setHost(String inHost)
{
     host = inHost;
}

String WifiHandler::getHost()
{
     return host;
}

void WifiHandler::storeMacAddress()
{
     byte mac[6];
     WiFi.macAddress(mac);

     String cmac;

     for (int i = 0; i < 6; i++)
     {
          cmac += String(mac[i], HEX);
     }

     #if defined(PRINT_STATUS)
          //printIfEnabled(cmac);
     #endif

     macAddress = cmac;
}

void WifiHandler::sendToServer(String message)
{
     // Use WiFiClient class to create TCP connections
     WiFiClient client;
     const int httpPort = 8080;
     char charBuf[50];
     host.toCharArray(charBuf, 50);
     if (!client.connect(charBuf, httpPort))
     {
          Serial.println("connection failed");
          return;
     }

     //We now create a URI for the request
     //String url = "/services/climate/internal/send?&macaddress=" + wifiHandler.getMacAddress() + "&temperature=" + temp;

     client.print(String("GET ") + message + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" +"Connection: close\r\n\r\n");

     delay(500);
     // Read all the lines of the reply from server and print them to Serial
     while (client.available())
     {
          String line = client.readStringUntil('\r');
          //printIfEnabled(line);
     }

     //printIfEnabled("closing connection");
}

String WifiHandler::getMacAddress()
{
     return macAddress;
}


bool WifiHandler::serverCheckIn()
{
     WiFiClient client;
     const int httpPort = 8080;
     char charBuf[50];
     host.toCharArray(charBuf, 50);
     return client.connect(charBuf, httpPort);
}

