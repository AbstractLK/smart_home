/**********************************************************************************
 *  TITLE: Blynk + Manual Switch (latched) + IR + DHT11 control 4 Relays using NodeMCU (store last GPIO state + Real time feedback + no WiFi control)
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/WqQZpOQB1AE
 *  Related Blog : https://iotcircuithub.com/esp8266-projects/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP8266 NodeMCU : https://github.com/esp8266/Arduino
 *
 *  Download the libraries 
 *  Blynk Library (1.1.0):  https://github.com/blynkkk/blynk-library
 *  IRremote Library (3.6.1): https://github.com/Arduino-IRremote/Arduino-IRremote
 *  DHT Library (1.4.3): https://github.com/adafruit/DHT-sensor-library
 *  Preferences Library (2.0.0): https://github.com/vshymanskyy/Preferences
 **********************************************************************************/

#include "credentials.h"

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID TEMPLATE_ID
#define BLYNK_TEMPLATE_NAME TEMPLATE_NAME
#define BLYNK_AUTH_TOKEN AUTH_TOKEN

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = MY_SSID;
char pass[] = MY_PASS;

//Update the HEX code of IR Remote buttons 0x<HEX CODE>
// #define IR_Button_1   0x80BF49B6
// #define IR_Button_2   0x80BFC936
// #define IR_Button_3   0x80BF33CC
// #define IR_Button_4   0x80BF718E
// #define IR_All_Off    0x80BF3BC4

//#define BLYNK_PRINT Serial
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>        
#include <BlynkSimpleEsp8266.h>
//#include <DHT.h>  
//#include <IRremote.h>
#include <Preferences.h>

Preferences pref;

// #define DHTPIN              D4 //D4  pin connected with DHT
// #define IR_RECV_PIN         9 // SD2 (IR receiver pin)

// Uncomment whatever type you're using!
// #define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

// IRrecv irrecv(IR_RECV_PIN);
// decode_results results;
  
// define the GPIO connected with Relays and switches
#define RelayPin1 D1  //D1
#define RelayPin2 4  //D2
#define RelayPin3 14 //D5
#define RelayPin4 12 //D6

#define SwitchPin1 1  //TX
#define SwitchPin2 D3   //D3 
#define SwitchPin3 13  //D7
#define SwitchPin4 3   //RX

#define wifiLed   D4   //D0

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2
#define VPIN_BUTTON_3    V3 
#define VPIN_BUTTON_4    V4

#define VPIN_BUTTON_C    V5
// #define VPIN_TEMPERATURE V6
// #define VPIN_HUMIDITY    V7

// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW; //Define integer to remember the toggle state for relay 4

// Switch State
bool SwitchState_1 = LOW;
bool SwitchState_2 = LOW;
bool SwitchState_3 = LOW;
bool SwitchState_4 = LOW;

int wifiFlag = 0;
// float temperature1 = 0;
// float humidity1   = 0;

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;
// DHT dht(DHTPIN, DHTTYPE);

// When App button is pushed - switch the state

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, !toggleState_1);
  pref.putBool("Relay1", toggleState_1);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, !toggleState_2);
  pref.putBool("Relay2", toggleState_2);
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, !toggleState_3);
  pref.putBool("Relay3", toggleState_3);
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleState_4 = param.asInt();
  digitalWrite(RelayPin4, !toggleState_4);
  pref.putBool("Relay4", toggleState_4);
}

BLYNK_WRITE(VPIN_BUTTON_C) {
  all_SwitchOff();
}

void all_SwitchOff(){
  toggleState_1 = 0; digitalWrite(RelayPin1, HIGH); pref.putBool("Relay1", toggleState_1); Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1); delay(100);
  toggleState_2 = 0; digitalWrite(RelayPin2, HIGH); pref.putBool("Relay2", toggleState_2); Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2); delay(100);
  toggleState_3 = 0; digitalWrite(RelayPin3, HIGH); pref.putBool("Relay3", toggleState_3); Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3); delay(100);
  toggleState_4 = 0; digitalWrite(RelayPin4, HIGH); pref.putBool("Relay4", toggleState_4); Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4); delay(100);
  // Blynk.virtualWrite(VPIN_HUMIDITY, humidity1);
  // Blynk.virtualWrite(VPIN_TEMPERATURE, temperature1);
}

void checkBlynkStatus() { // called every 2 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, HIGH);
  }
  if (isconnected == true) {
    wifiFlag = 0;
    digitalWrite(wifiLed, LOW);
    //Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  // update the latest state to the server
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
  Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
  Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
  // Blynk.syncVirtual(VPIN_TEMPERATURE);
  // Blynk.syncVirtual(VPIN_HUMIDITY);
}

// void readSensor(){
  
//   float h = dht.readHumidity();
//   float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  
//   if (isnan(h) || isnan(t)) {
//     Serial.println("Failed to read from DHT sensor!");
//     return;
//   }
//   else {
//     humidity1 = h;
//     temperature1 = t;
//    // Serial.println(temperature1);
//    // Serial.println(humidity1);
//   }  
// }

// void sendSensor()
// {
//   readSensor();
//   // You can send any value at any time.
//   // Please don't send more that 10 values per second.
//   Blynk.virtualWrite(VPIN_HUMIDITY, humidity1);
//   Blynk.virtualWrite(VPIN_TEMPERATURE, temperature1);
// }

// void ir_remote(){
//   if (irrecv.decode(&results)) {
//       switch(results.value){
//           case IR_Button_1:  
//             digitalWrite(RelayPin1, toggleState_1);
//             toggleState_1 = !toggleState_1;
//             pref.putBool("Relay1", toggleState_1);
//             Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
//             delay(100);            
//             break;
//           case IR_Button_2:  
//             digitalWrite(RelayPin2, toggleState_2);
//             toggleState_2 = !toggleState_2;
//             pref.putBool("Relay2", toggleState_2);
//             Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
//             delay(100);            
//             break;
//           case IR_Button_3:  
//             digitalWrite(RelayPin3, toggleState_3);
//             toggleState_3 = !toggleState_3;
//             pref.putBool("Relay3", toggleState_3);
//             Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
//             delay(100);            
//             break;
//           case IR_Button_4:  
//             digitalWrite(RelayPin4, toggleState_4);
//             toggleState_4 = !toggleState_4;
//             pref.putBool("Relay4", toggleState_4);
//             Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
//             delay(100);            
//             break;
//           case IR_All_Off:
//             all_SwitchOff();  
//             break;
//           default : break;         
//         }   
//         //Serial.println(results.value, HEX);    
//         irrecv.resume();   
//   } 
// }

void getRelayState()
{
  //Serial.println("reading data from NVS");
  toggleState_1 = pref.getBool("Relay1", 0);
  digitalWrite(RelayPin1, !toggleState_1); 
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  delay(200);
  toggleState_2 = pref.getBool("Relay2", 0);
  digitalWrite(RelayPin2, !toggleState_2); 
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
  delay(200);
  toggleState_3 = pref.getBool("Relay3", 0);
  digitalWrite(RelayPin3, !toggleState_3); 
  Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
  delay(200);
  toggleState_4 = pref.getBool("Relay4", 0);
  digitalWrite(RelayPin4, !toggleState_4); 
  Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
  delay(200);
}

void manual_control()
{
  if (digitalRead(SwitchPin1) == LOW && SwitchState_1 == LOW) {
    digitalWrite(RelayPin1, LOW);
    toggleState_1 = HIGH;
    SwitchState_1 = HIGH;
    pref.putBool("Relay1", toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Serial.println("Switch-1 on");
  }
  if (digitalRead(SwitchPin1) == HIGH && SwitchState_1 == HIGH) {
    digitalWrite(RelayPin1, HIGH);
    toggleState_1 = LOW;
    SwitchState_1 = LOW;
    pref.putBool("Relay1", toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Serial.println("Switch-1 off");
  }
  if (digitalRead(SwitchPin2) == LOW && SwitchState_2 == LOW) {
    digitalWrite(RelayPin2, LOW);
    toggleState_2 = HIGH;
    SwitchState_2 = HIGH;
    pref.putBool("Relay2", toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Serial.println("Switch-2 on");
  }
  if (digitalRead(SwitchPin2) == HIGH && SwitchState_2 == HIGH) {
    digitalWrite(RelayPin2, HIGH);
    toggleState_2 = LOW;
    SwitchState_2 = LOW;
    pref.putBool("Relay2", toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Serial.println("Switch-2 off");
  }
  if (digitalRead(SwitchPin3) == LOW && SwitchState_3 == LOW) {
    digitalWrite(RelayPin3, LOW);
    toggleState_3 = HIGH;
    SwitchState_3 = HIGH;
    pref.putBool("Relay3", toggleState_3);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Serial.println("Switch-3 on");
  }
  if (digitalRead(SwitchPin3) == HIGH && SwitchState_3 == HIGH) {
    digitalWrite(RelayPin3, HIGH);
    toggleState_3 = LOW;
    SwitchState_3 = LOW;
    pref.putBool("Relay3", toggleState_3);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    Serial.println("Switch-3 off");
  }
  if (digitalRead(SwitchPin4) == LOW && SwitchState_4 == LOW) {
    digitalWrite(RelayPin4, LOW);
    toggleState_4 = HIGH;
    SwitchState_4 = HIGH;
    pref.putBool("Relay4", toggleState_4);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
    Serial.println("Switch-4 on");
  }
  if (digitalRead(SwitchPin4) == HIGH && SwitchState_4 == HIGH) {
    digitalWrite(RelayPin4, HIGH);
    toggleState_4 = LOW;
    SwitchState_4 = LOW;
    pref.putBool("Relay4", toggleState_4);
    Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
    Serial.println("Switch-4 off");
  }
}  

void setup()
{
  Serial.begin(9600);
  Serial.println("Booting");
  //Open namespace in read-write mode
  pref.begin("Relay_State", false);
  
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !toggleState_1);
  digitalWrite(RelayPin2, !toggleState_2);
  digitalWrite(RelayPin3, !toggleState_3);
  digitalWrite(RelayPin4, !toggleState_4);

  // irrecv.enableIRIn(); // Enabling IR sensor
  // dht.begin();    // Enabling DHT sensor

  digitalWrite(wifiLed, HIGH);

  WiFi.mode(WIFI_STA);
  //Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  //timer.setInterval(1000L, sendSensor); // Sending Sensor Data to Blynk Cloud every 1 second
  Blynk.config(auth);
  delay(1000);
  
  getRelayState(); // Get the last state of Relays
  
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
  Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
  Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{ 
  manual_control();
  //ir_remote(); //IR remote Control
  Blynk.run();
  ArduinoOTA.handle();
  timer.run();
}
