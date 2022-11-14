#include "Base64.h"
#include "MagicServer.h";
WiFiClient mainServer;
AsyncWebServer magicServer(80);

MagicServer::MagicServer(uint8_t* key) {
    UUID uuid;
    memcpy(key, cipher_key, 16);
    memcpy(key, cipher_iv, 16);
    // Send a POST request to <IP>/post with a form field message set to <message>
    magicServer.on("/control", HTTP_POST, [this](AsyncWebServerRequest *request){
        if (request->hasParam("home_id", true)) {
            homeid = request->getParam("home_id", true)->value();
        } else {
            homeid = "No message sent";
        }
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
        //Encrypt
        encryptedMessage = encrypt(message);
        request->send(200, "text/plain", "Hello, POST: " + encryptedMessage);
        Serial.println("Forwarding request to http://"+getAddress(homeid)+"/control");
        // Serial.print("Available: ");
        // Serial.println(mainServer.available());
        //mainServer->connect(getAddress(homeid)+"/control?room_id="+roomid+"&&node_id="+nodeid+"&&message="+encryptedMessage, 80);
    });
    
    magicServer.on("/reg", HTTP_POST, [this, &uuid](AsyncWebServerRequest *request){
        uuid.generate();
        String ip;
        const char* uid = uuid.toCharArray();
        if (request->hasParam("ip_address", true)) {
            ip = request->getParam("ip_address", true)->value();
        } else {
            ip = "No message sent";
        }
        Serial.print("IP Address received: ");
        Serial.println(ip);
        ip_addr[noOfHomes] = ip.c_str();
        homes[noOfHomes]= uid;
        noOfHomes++;
        request->send(200, "text/plain", uid);
    });
    magicServer.begin();
}

void MagicServer::process() {
    HTTPClient magicHttp;
    // Forward the message
    // configure traged server and url
    if(message.length()!=0 || encryptedMessage.length()!=0) {
        String url = "http://"+getAddress(homeid)+"/control";
        bool httpInitResult = magicHttp.begin(mainServer, url); //HTTP
        Serial.println(httpInitResult);
        //http.addHeader("Content-Type", "application/json");
        magicHttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        Serial.print("[HTTP] POST...\n");
        //Serial.println("http://"+getAddress(homeid)+"/control");
        //Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
        // start connection and send HTTP header and body
        //int httpCode = http.POST("{\"room_id\":\""+roomid+"\""+"\"node_id\":\""+nodeid+"\""+"\"message\":\""+message+"\"}");
        String params = "room_id="+roomid+"&node_id="+nodeid+"&message="+encryptedMessage;
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
        encryptedMessage="";
    }
    homeid="";
    roomid="";
    nodeid="";
    message="";
}

String MagicServer::getAddress(String id) {
    for(int i = 0; i<10;i++) {
      if(id==homes[i]) {
        return ip_addr[i];
      }
    }
    return "";
}

String MagicServer::encrypt(String plain_data){
  int i;
  // PKCS#7 Padding (Encryption), Block Size : 16
  int len = plain_data.length();
  int n_blocks = len / 16 + 1;
  uint8_t n_padding = n_blocks * 16 - len;
  uint8_t data[n_blocks*16];
  memcpy(data, plain_data.c_str(), len);
  for(i = len; i < n_blocks * 16; i++){
    data[i] = n_padding;
  }
  
  uint8_t key[16], iv[16];
  memcpy(key, cipher_key, 16);
  memcpy(iv, cipher_iv, 16);

  // encryption context
  br_aes_big_cbcenc_keys encCtx;

  // reset the encryption context and encrypt the data
  br_aes_big_cbcenc_init(&encCtx, key, 16);
  br_aes_big_cbcenc_run( &encCtx, iv, data, n_blocks*16 );

  // Base64 encode
  len = n_blocks*16;
  char encoded_data[ base64_enc_len(len) ];
  base64_encode(encoded_data, (char *)data, len);
  
  return String(encoded_data);
}

String MagicServer::decrypt(String encoded_data_str){  
  int input_len = encoded_data_str.length();
  char *encoded_data = const_cast<char*>(encoded_data_str.c_str());
  int len = base64_dec_len(encoded_data, input_len);
  uint8_t data[ len ];
  base64_decode((char *)data, encoded_data, input_len);
  
  uint8_t key[16], iv[16];
  memcpy(key, cipher_key, 16);
  memcpy(iv, cipher_iv, 16);

  int n_blocks = len / 16;

  br_aes_big_cbcdec_keys decCtx;

  br_aes_big_cbcdec_init(&decCtx, key, 16);
  br_aes_big_cbcdec_run( &decCtx, iv, data, n_blocks*16 );

  // PKCS#7 Padding (Decryption)
  uint8_t n_padding = data[n_blocks*16-1];
  len = n_blocks*16 - n_padding;
  char plain_data[len + 1];
  memcpy(plain_data, data, len);
  plain_data[len] = '\0';

  return String(plain_data);
}
