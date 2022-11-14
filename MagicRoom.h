#ifndef MagicRoom_h
#define MagicRoom_h
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

class MagicRoom {
  private:
    UUID uuid;
    String roomid,nodeid;
    String getAddress(String id);
    int noOfNodes=0;
    //Max Homes = 10
    String nodes[10];
    String ip_addr[10];
  public:
    String message;
    MagicRoom();
    void registerRoom(String serverAddr);
    void process();
};
#endif
