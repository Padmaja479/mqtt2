#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "mobiistar C1 Lite";
const char* password = "mobiistar";


#include "DHT.h"
#define DHTPIN D3  
#define LDRPIN A0// what pin we're connected to
#define DHTTYPE DHT11// define type of sensor DHT 11
DHT dht (DHTPIN, DHTTYPE);

#define ORG "4x036i"
#define DEVICE_TYPE "iot"
#define DEVICE_ID "123"
#define TOKEN "12345678"
String command;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/home/fmt/String";
char pubtopic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
//Serial.println(clientID);

WiFiClient wifiClient;
void callback(char* subtopic, byte* payload, unsigned int payloadLength);
PubSubClient client(server, 1883, callback, wifiClient);

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  dht.begin();
  wifiConnect();
  mqttConnect();
}

void loop() {
  
int dutycycle = analogRead(A0);
float h = dht.readHumidity();
float t = dht.readTemperature();

if (isnan(h) || isnan(t) || isnan(dutycycle))
{
Serial.println("Failed to read from DHT sensor!");
delay(1000);
return;
}
PublishData(t,h,dutycycle);
 if (!client.loop()) {
    mqttConnect();
  }
delay(100);
  if (!client.loop()) {
    mqttConnect();
  }
delay(100);
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  if (client.subscribe(subtopic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* subtopic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for subtopic: "); Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++) {
    //Serial.println((char)payload[i]);
    command += (char)payload[i];
  }
Serial.println(command);
if(command == "lighton"){
  digitalWrite(D1,HIGH);
  Serial.println("Light is Switched ON");
}
else if(command == "lightoff"){
  digitalWrite(D1,LOW);
  //digitalWrite(D2,LOW);
  Serial.println("Light is Switched OFF");
}
if(command == "ALLON"){
  digitalWrite(D1,HIGH);
digitalWrite(D2,HIGH);
  
  Serial.println("ALL are Switched ON");
}
else if(command == "ALLOFF"){
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);
  //digitalWrite(D2,LOW);
  Serial.println("ALL are Switched OFF");
}
if(command == "fanON"){
  
digitalWrite(D2,HIGH);
  
  Serial.println("fan is Switched ON");
}
else if(command == "fanOFF"){
  
  digitalWrite(D2,LOW);
  Serial.println("fan is Switched OFF");
}
command ="";
}

void PublishData(float temp, float humid, float light){
 if (!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"temperature\":";
  payload += temp;
  payload+="," "\"humidity\":";
  payload += humid;
  payload+="," "\"light\":";
  payload += light;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(pubtopic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
