#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// #include "selfhttpclient.h"
#include "selfultrasonic.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "sabrina_wifi";
const char* password = "08091974";

// ESP8266WiFiMulti WiFiMulti;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;
int waterOut = 0;
int pinSoil = A0;
int valve = D8;
int pump = D7;
int height = 115;
int minSoil = 380;
int dataValve = 0;
int counter = 0;
bool counterstate = false;
String pushbackend(String getApi){
  String payload;
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      String serverPath = getApi;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
  
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
        
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  return payload;
}

int ultrasonic(){
  long duration, jarak;
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2); 
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  jarak = (duration/2) / 29.1;
  return height-jarak;
}

void lcdset(String dis, String soil){
  lcd.setCursor(0,0);
  lcd.print("Soil : "+soil);
  lcd.setCursor(0,1);
  lcd.print("Water : "+dis);
}

void valvestate(int i){
  if (i==0){
    digitalWrite(valve,HIGH);
    dataValve = 1;
  }else{
    digitalWrite(valve,LOW);
    dataValve = 0;
  }
}
void pumpstate(int i){
  if (i==0){
    digitalWrite(pump,HIGH);
  }else{
    digitalWrite(pump,LOW);
  }
}

int soilMoisture(){
  float sensorValue = analogRead(pinSoil) - float(minSoil); 
  float persentage = (sensorValue/620)*100;
  float persentageRev = 100 - persentage;
  if (persentageRev<0){
    return 0;
  }
  return persentageRev;
}


void setup() {  
  Serial.begin(115200); 
  Serial.println();
  Serial.println();
  Serial.println();


  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.init();
  lcd.backlight();
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(valve,OUTPUT);
  pinMode(pump,OUTPUT);
  pinMode(pinSoil,INPUT);
}

void loop() {
  int distance = ultrasonic();
  int soilData = soilMoisture();
  Serial.println(distance);
  if(distance<50){
    pumpstate(1);
    Serial.println("pumpOn");
  }else if(distance>90){
    pumpstate(0);
    Serial.println("pumpOff");
  }
  if (counterstate == true){
    counter++;
    Serial.println("counting : "+String(counter) );
  }else if(soilData<12){
    valvestate(1);
    Serial.println("valvepOn");
    counter=0;
    counterstate = true;
  }else if(counter > 15){
    valvestate(0);
    Serial.println("valveOF");
    counter=0;
    counterstate = false;
  }else{
    Serial.println("nothing");
  }
  pushbackend ("http://47.254.244.187:8080/api/sensor/add?soil="+String(soilData)+"&water_level="+String(distance)+"&water="+String(dataValve));
  lcdset(String(distance),String(soilData));
  Serial.println("dis : "+String(distance)+" soil : "+String(soilData)+" datavaleve : "+String(dataValve));
  delay(1000);
  lcd.clear();
}


//commect wifi
// #include <ESP8266WiFi.h>

// const char *ssid =  "sabrina_wifi";     // replace with your wifi ssid and wpa2 key
// const char *pass =  "08091974";

// WiFiClient client;
 
// void setup() 
// {
//        Serial.begin(9600);
//        delay(10);
               
//        Serial.println("Connecting to ");
//        Serial.println(ssid); 
 
//        WiFi.begin(ssid, pass); 
//        while (WiFi.status() != WL_CONNECTED) 
//           {
//             delay(500);
//             Serial.print(".");
//           }
//       Serial.println("");
//       Serial.println("WiFi connected"); 
// }
 
// void loop() 
// {      
  
// }



// wifihttpclient


// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>

// const char* ssid = "sabrina_wifi";
// const char* password = "08091974";

// //Your Domain name with URL path or IP address with path
// String serverName = "http://192.168.1.106:1880/update-sensor";

// // the following variables are unsigned longs because the time, measured in
// // milliseconds, will quickly become a bigger number than can be stored in an int.
// unsigned long lastTime = 0;
// // Timer set to 10 minutes (600000)
// //unsigned long timerDelay = 600000;
// // Set timer to 5 seconds (5000)
// unsigned long timerDelay = 5000;

// void setup() {
//   Serial.begin(115200); 

//   WiFi.begin(ssid, password);
//   Serial.println("Connecting");
//   while(WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.print("Connected to WiFi network with IP Address: ");
//   Serial.println(WiFi.localIP());
 
//   Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
// }

// void loop() {
//   // Send an HTTP POST request depending on timerDelay
//   if ((millis() - lastTime) > timerDelay) {
//     //Check WiFi connection status
//     if(WiFi.status()== WL_CONNECTED){
//       WiFiClient client;
//       HTTPClient http;
//       int soilData=1;
//       int distance=1;
//       int dataValve=1;
//       String serverPath = "http://47.254.244.187:8080/api/sensor/add?soil="+String(soilData)+"&water_level="+String(distance)+"&water="+String(dataValve);
      
//       // Your Domain name with URL path or IP address with path
//       http.begin(client, serverPath.c_str());
  
//       // If you need Node-RED/server authentication, insert user and password below "http://47.254.244.187:8080/api/sensor/add?soil="+String(soilData)+"&water_level="+String(distance)+"&water="+String(dataValve)
//       //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
        
//       // Send HTTP GET request
//       int httpResponseCode = http.GET();
      
//       if (httpResponseCode>0) {
//         Serial.print("HTTP Response code: ");
//         Serial.println(httpResponseCode);
//         String payload = http.getString();
//         Serial.println(payload);
//       }
//       else {
//         Serial.print("Error code: ");
//         Serial.println(httpResponseCode);
//       }
//       // Free resources
//       http.end();
//     }
//     else {
//       Serial.println("WiFi Disconnected");
//     }
//     lastTime = millis();
//   }
// }