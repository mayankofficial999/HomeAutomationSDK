#ifndef MagicHome_h
#define MagicHome_h
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UUID.h>

class MagicHome {
  private:
    UUID uuid;
    String homeid,roomid,nodeid;
    String getAddress(String id);
    int noOfRooms=0;
    //Max Homes = 10
    String rooms[10];
    String ip_addr[10];
  public:
    String message;
    MagicHome();
    void registerHome(String serverAddr);
    void process();
};
#endif
