#include "MagicHome.h"

WiFiClient homeServer;
AsyncWebServer magicHome(80);

MagicHome::MagicHome() {
    UUID uuid;
    // Send a POST request to <IP>/post with a form field message set to <message>
    magicHome.on("/control", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("room_id", true)) {
            roomid = request->getParam("room_id", true)->value();
        } else {
            roomid = "No message sent";
        }
        if (request->hasParam("node_id", true)) {
            nodeid = request->getParam("node_id", true)->value();
        } else {
            nodeid = "No message sent";
        }
        if (request->hasParam("message", true)) {
            message = request->getParam("message", true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
        //mainHome->connect(getAddress(roomid)+"/control?node_id="+nodeid+"&&message="+message, 80);
    });
    
    magicHome.on("/reg", HTTP_POST, [this, &uuid](AsyncWebServerRequest *request){
        uuid.generate();
        String ip;
        const char* uid = uuid.toCharArray();
        if (request->hasParam("ip_address", true)) {
            ip = request->getParam("ip_address", true)->value();
        } else {
            ip = "No message sent";
        }
        ip_addr[noOfRooms] = ip.c_str();
        rooms[noOfRooms]= uid;
        noOfRooms++;
        request->send(200, "text/plain", uid);
    });
    magicHome.begin();
}


String MagicHome::getAddress(String id) {
    for(int i = 0; i<10;i++) {
      if(id==rooms[i]) {
        return ip_addr[i];
      }
    }
    return "";
}

void MagicHome::registerHome(String serverAddr) {
    HTTPClient magicHttp;
    magicHttp.begin(homeServer,"http://"+serverAddr+"/reg");
    magicHttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = magicHttp.POST("ip_address="+WiFi.localIP().toString());
    // httpCode will be negative on error
    if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
            const String& payload = magicHttp.getString();
            homeid = payload;
        }
    } 
    else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", magicHttp.errorToString(httpCode).c_str());
    }
    homeServer.stop();
}

void MagicHome::process() {
    HTTPClient magicHttp;
    // Forward the message
    // configure traged server and url
    if(message.length()!=0) {
        String url = "http://"+getAddress(roomid)+"/control";
        bool httpInitResult = magicHttp.begin(homeServer, url); //HTTP
        Serial.println(httpInitResult);
        //http.addHeader("Content-Type", "application/json");
        magicHttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        Serial.print("[HTTP] POST...\n");
        //Serial.println("http://"+getAddress(homeid)+"/control");
        //Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
        // start connection and send HTTP header and body
        //int httpCode = http.POST("{\"room_id\":\""+roomid+"\""+"\"node_id\":\""+nodeid+"\""+"\"message\":\""+message+"\"}");
        String params = "node_id="+nodeid+"&message="+message;
        int httpCode = magicHttp.POST(params);
        //int httpCode = http.GET();
        // httpCode will be negative on error
        if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
            // file found at server
            if (httpCode == HTTP_CODE_OK) {
                const String& payload = magicHttp.getString();
            }
        } 
        else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", magicHttp.errorToString(httpCode).c_str());
        }
        magicHttp.end();
        message="";
    }
    roomid="";
    nodeid="";
    message="";
}
