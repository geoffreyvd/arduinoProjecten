/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>

const char* ssid     = "Sitecom8D1978";
const char* password = "D7I0RK3ECNQ2";

const char* host = "192.168.0.112";

const byte getLaserStatus = 0;
const byte setLaserStatusOn = 1;
const byte setLaserStatusOff = 2;
const byte getLaserAngle = 3; // returns range between 60-130
const byte getCalibrationMinimum = 4;
const byte getCalibrationMaximum = 5;
const byte resetLaserAngle = 90;
const byte bad = 255;
const byte ack = 254;

int maximumLaserAngle = 0;
int minimumLaserAngle = 0;

void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(5000);

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 10002;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/";

  if(maximumLaserAngle == 0){
    maximumLaserAngle = sendRequest(client, url, getCalibrationMaximum);  
  }
  if(minimumLaserAngle == 0){
    minimumLaserAngle = sendRequest(client, url, getCalibrationMinimum);  
  Serial.println("maximum: " + (String) maximumLaserAngle + " minimjum : " + (String) minimumLaserAngle);
  }

  //petentiovalue
  sendRequest(client, url, 130);

  Serial.println("closing connection");
}


byte sendRequest(WiFiClient client, String url, byte messageByte){
  Serial.println("Requesting URL: " + url + " , sending message: " + messageByte);
  
  // This will send the request to the server
  client.write(messageByte);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return 0;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    byte answer = client.read();
    if(answer == ack){
      Serial.println("Succesfull request");
    }else if(answer == bad){
      Serial.println("Bad request");      
    }
    return answer;// wahct to goedkeuring
  }
}

