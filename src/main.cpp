#include <Arduino.h>
#include <ESP8266WiFi.h>        
#include <PubSubClient.h>
#include <Keypad.h>
#include <tr064.h>
#include "secrets.h"

//WiFi info
const char* ssid     = SECRET_WIFISSID;      //defined in secrets.h 
const char* password = SECRET_WIFIPASS;     
const char* deviceName = SECRET_DEVICENAME;

//MQTT info
const char* mqttServer = SECRET_MQTTSERVER; //MQTT Server
const int mqttPort = SECRET_MQTTPORT; //MQTT Port
const char* mqttUser = SECRET_MQTTUSER; //MQTT User
const char* mqttPassword = SECRET_MQTTPASS; //MQTT Password

//Keypad info
const char sendKey = '*'; //Keypress to initialize send to MQTT
const byte n_rows = 4;
const byte n_cols = 4;

//Fritzbox info
const char FB_USER[] = SECRET_FBUSER;
const char FB_PASSWORD[] = SECRET_FBPASS; 
const char FRITZBOX_IP[] = SECRET_FBIP;
const int FRITZBOX_PORT = 49000;
const char ringKey = 'A'; // Keypress to ring the doorbell

TR064 tr064_connection(FRITZBOX_PORT, FRITZBOX_IP, FB_USER, FB_PASSWORD);

char keys[n_rows][n_cols] = { //Define values of keys
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','P','D'}
};

byte colPins[n_rows] = {D1, D2, D3, D4}; //Define row pins
byte rowPins[n_cols] = {D0, D5, D6, D7}; //Define column pins

Keypad Keypad1 = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols);
String fullKey = String("");
char KeyMQTT[50];
WiFiClient espClient;
PubSubClient mqttclient(espClient);

void wificn () {               //Connect to WiFi
  WiFi.hostname(deviceName);
  if (WiFi.status() != WL_CONNECTED){
     WiFi.begin(ssid, password);             
     Serial.print("Connecting to ");
     Serial.print(ssid); Serial.println(" ...");

     int i = 0;
     while (WiFi.status() != WL_CONNECTED){ 
        delay(200);
        Serial.print(++i); Serial.print(' ');
     }
     Serial.println('\n');
     Serial.println("Connection established!");  
     Serial.print("IP address:\t");
     Serial.println(WiFi.localIP());        
    }    
}

void mqttcn () {              //Connect to MQTT
   mqttclient.setServer(mqttServer, mqttPort);
   
    while (!mqttclient.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (mqttclient.connect("Keypad Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
      mqttclient.loop();
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(mqttclient.state());
      delay(2000);
 
    }
    mqttclient.publish("keypad/LWT", "Online");
    }
}
void setup() {
  Serial.begin(115200);         
  delay(10);
  Serial.println('\n');
  wificn();
  mqttcn();
  
}

void fb_ring(){
//Ring all phones connected to FB to simulate a doorbell
   tr064_connection.init();

   String tr064_service = "urn:dslforum-org:service:X_VoIP:1";

   // **9 is Fritzbox-Round Call number.
   String call_params[][2] = {{"NewX_AVM-DE_PhoneNumber", "**9"}};
   String req[][2] ={{}};
   // tr064_connection.debug_level = 4;
   tr064_connection.action(tr064_service, "X_AVM-DE_DialNumber", call_params, 1, req, 0);
   // Warte vier Sekunden bis zum auflegen
   delay(6000);
   tr064_connection.action(tr064_service, "X_AVM-DE_DialHangup");
   
}

void loop() {
    char Key1 = Keypad1.getKey();
    if(Key1 == ringKey){
       Serial.print(Key1);
       Serial.print("Ringing the bell");
       fullKey = "";
       Key1 = char('\0'); //Don't want to add the ring key to a sequence
       mqttclient.publish("keypad/RING","1");
       fb_ring();

    }
    if(Key1 == sendKey) {
     fullKey.toCharArray(KeyMQTT,fullKey.length()+1);
     mqttclient.publish("keypad/KEY", KeyMQTT);
     Serial.print("Sent key: ");
     Serial.println(fullKey);
     fullKey = "";
    } else if (Key1 != '\0'){
     fullKey += Key1;
     Serial.print("Current keys: ");
     Serial.println(fullKey);
 
  }
  if(WiFi.status() != WL_CONNECTED) {
      wificn();
  }
  if(!mqttclient.loop()) {
      mqttcn();
  }
}
  


