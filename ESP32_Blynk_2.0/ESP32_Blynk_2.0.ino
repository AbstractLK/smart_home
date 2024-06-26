/**********************************************************************************
 *  TITLE: Blynk + IR + Manual Switch control 1 Fan & 4 Relays using ESP32 (Real time feedback + no WiFi control + EEPROM)
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/vBq3Ozrwj-4
 *  Related Blog : https://iotcircuithub.com/esp32-home-automation-system-with-fan-speed-control/
 *  
 *  This code is provided free for project purpose and fair use only.
 *  Please do mail us to techstudycell@gmail.com if you want to use it commercially.
 *  Copyrighted © by Tech StudyCell
 *  
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP32 : https://github.com/espressif/arduino-esp32
 *
 *  Download the libraries 
 *  Blynk Library (1.1.0):  https://github.com/blynkkk/blynk-library
 *  IRremote Library (3.6.1): https://github.com/Arduino-IRremote/Arduino-IRremote
 *  DHT Library (1.4.3): https://github.com/adafruit/DHT-sensor-library
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
// #define IR_Fan_Up     0x80BF53AC
// #define IR_Fan_Down   0x80BF4BB4
// #define IR_All_Off    0x80BF3BC4


//#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESPmDNS.h>
//#include <NetworkUdp.h>
#include <ArduinoOTA.h>
#include <Preferences.h>

// #include <IRremote.h>
// #include <DHT.h>  

Preferences pref;

// define the GPIO connected with Relays and switches
#define RelayPin1 23  //D23
#define RelayPin2 22  //D22
#define RelayPin3 21  //D21
#define RelayPin4 19  //D19
#define RelayPin5 18  //D18
#define RelayPin6 5   //D5
#define RelayPin7 25  //D25
#define RelayPin8 26  //D26

#define SwitchPin1 13  //D13
#define SwitchPin2 12  //D12
#define SwitchPin3 14  //D14
#define SwitchPin4 27  //D27
#define SwitchPin5 33  //D33
#define SwitchPin6 32  //D32
#define SwitchPin7 15  //D15
#define SwitchPin8 4   //D4

// #define FanRelay1 18  //D18
// #define FanRelay2 5   //D5
// #define FanRelay3 25  //D25

// #define FanSwitch1 33  //D33
// #define FanSwitch2 32  //D32
// #define FanSwitch3 15  //D15
// #define FanSwitch4 4   //D4

#define wifiLed   2   //D2
// #define IR_RECV_PIN   35 // D35 (IR receiver pin)
// #define DHTPIN        16 //D16  pin connected with DHT

//Change the virtual pins according the rooms
// #define VPIN_BUTTON_FAN  V0 
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2
#define VPIN_BUTTON_3    V3
#define VPIN_BUTTON_4    V4
#define VPIN_BUTTON_5    V5 
#define VPIN_BUTTON_6    V6
#define VPIN_BUTTON_7    V7 
#define VPIN_BUTTON_8    V8

#define VPIN_BUTTON_C    V9
#define VPIN_BUTTON_D    V10
// #define VPIN_TEMPERATURE V6
// #define VPIN_HUMIDITY    V7

// Uncomment whatever type you're using!
// #define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

// int currSpeed = 0;

// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW; //Define integer to remember the toggle state for relay 4
bool toggleState_5 = LOW; //Define integer to remember the toggle state for relay 5
bool toggleState_6 = LOW; //Define integer to remember the toggle state for relay 6
bool toggleState_7 = LOW; //Define integer to remember the toggle state for relay 7
bool toggleState_8 = LOW; //Define integer to remember the toggle state for relay 8

// bool fanSpeed_0 = LOW;
// bool fanSpeed_1 = LOW; //Define integer to remember the toggle state for relay 5
// bool fanSpeed_2 = LOW; //Define integer to remember the toggle state for relay 6
// bool fanSpeed_3 = LOW; //Define integer to remember the toggle state for relay 7
// bool fanSpeed_4 = LOW; //Define integer to remember the toggle state for relay 8

// Switch State
bool SwitchState_1 = LOW;
bool SwitchState_2 = LOW;
bool SwitchState_3 = LOW;
bool SwitchState_4 = LOW;
bool SwitchState_5 = LOW;
bool SwitchState_6 = LOW;
bool SwitchState_7 = LOW;
bool SwitchState_8 = LOW;

// float temperature1 = 0;
// float humidity1   = 0;
int wifiFlag = 0;
// IRrecv irrecv(IR_RECV_PIN);
// decode_results results;

// DHT dht(DHTPIN, DHTTYPE);

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

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

BLYNK_WRITE(VPIN_BUTTON_5) {
  toggleState_5 = param.asInt();
  digitalWrite(RelayPin5, !toggleState_5);
  pref.putBool("Relay5", toggleState_5);
}

BLYNK_WRITE(VPIN_BUTTON_6) {
  toggleState_6 = param.asInt();
  digitalWrite(RelayPin6, !toggleState_6);
  pref.putBool("Relay6", toggleState_6);
}

BLYNK_WRITE(VPIN_BUTTON_7) {
  toggleState_7 = param.asInt();
  digitalWrite(RelayPin7, !toggleState_7);
  pref.putBool("Relay7", toggleState_7);
}

BLYNK_WRITE(VPIN_BUTTON_8) {
  toggleState_8 = param.asInt();
  digitalWrite(RelayPin8, !toggleState_8);
  pref.putBool("Relay8", toggleState_8);
}

// BLYNK_WRITE(VPIN_BUTTON_FAN) {
//   currSpeed = param.asInt();
//   fanSpeedControl(currSpeed);
//   pref.putInt("Fan", currSpeed);
// }

BLYNK_WRITE(VPIN_BUTTON_C) {
  all_SwitchOff();
}
BLYNK_WRITE(VPIN_BUTTON_D) {
  all_SwitchOn();
}

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    digitalWrite(wifiLed, LOW);
    Serial.println("Blynk Not Connected");
  }
  if (isconnected == true) {
    wifiFlag = 0;
    digitalWrite(wifiLed, HIGH);
    //Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  // update the latest state to the server
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
  Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
  Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
  Blynk.virtualWrite(VPIN_BUTTON_5, toggleState_5);
  Blynk.virtualWrite(VPIN_BUTTON_6, toggleState_6);
  Blynk.virtualWrite(VPIN_BUTTON_6, toggleState_7);
  Blynk.virtualWrite(VPIN_BUTTON_7, toggleState_8);
  // Blynk.virtualWrite(VPIN_BUTTON_FAN, currSpeed);

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
  if (digitalRead(SwitchPin5) == LOW && SwitchState_5 == LOW) {
    digitalWrite(RelayPin5, LOW);
    toggleState_5 = HIGH;
    SwitchState_5 = HIGH;
    pref.putBool("Relay5", toggleState_5);
    Blynk.virtualWrite(VPIN_BUTTON_5, toggleState_5);
    Serial.println("Switch-5 on");
  }
  if (digitalRead(SwitchPin5) == HIGH && SwitchState_5 == HIGH) {
    digitalWrite(RelayPin5, HIGH);
    toggleState_5 = LOW;
    SwitchState_5 = LOW;
    pref.putBool("Relay5", toggleState_5);
    Blynk.virtualWrite(VPIN_BUTTON_5, toggleState_5);
    Serial.println("Switch-5 off");
  }
  if (digitalRead(SwitchPin6) == LOW && SwitchState_6 == LOW) {
    digitalWrite(RelayPin6, LOW);
    toggleState_6 = HIGH;
    SwitchState_6 = HIGH;
    pref.putBool("Relay6", toggleState_6);
    Blynk.virtualWrite(VPIN_BUTTON_6, toggleState_6);
    Serial.println("Switch-6 on");
  }
  if (digitalRead(SwitchPin6) == HIGH && SwitchState_6 == HIGH) {
    digitalWrite(RelayPin6, HIGH);
    toggleState_6 = LOW;
    SwitchState_6 = LOW;
    pref.putBool("Relay6", toggleState_6);
    Blynk.virtualWrite(VPIN_BUTTON_6, toggleState_6);
    Serial.println("Switch-6 off");
  }
  if (digitalRead(SwitchPin7) == LOW && SwitchState_7 == LOW) {
    digitalWrite(RelayPin7, LOW);
    toggleState_7 = HIGH;
    SwitchState_7 = HIGH;
    pref.putBool("Relay7", toggleState_7);
    Blynk.virtualWrite(VPIN_BUTTON_7, toggleState_7);
    Serial.println("Switch-7 on");
  }
  if (digitalRead(SwitchPin7) == HIGH && SwitchState_7 == HIGH) {
    digitalWrite(RelayPin7, HIGH);
    toggleState_7 = LOW;
    SwitchState_7 = LOW;
    pref.putBool("Relay7", toggleState_7);
    Blynk.virtualWrite(VPIN_BUTTON_7, toggleState_7);
    Serial.println("Switch-7 off");
  }
  if (digitalRead(SwitchPin8) == LOW && SwitchState_8 == LOW) {
    digitalWrite(RelayPin8, LOW);
    toggleState_8 = HIGH;
    SwitchState_8 = HIGH;
    pref.putBool("Relay8", toggleState_8);
    Blynk.virtualWrite(VPIN_BUTTON_8, toggleState_8);
    Serial.println("Switch-8 on");
  }
  if (digitalRead(SwitchPin8) == HIGH && SwitchState_8 == HIGH) {
    digitalWrite(RelayPin8, HIGH);
    toggleState_8 = LOW;
    SwitchState_8 = LOW;
    pref.putBool("Relay8", toggleState_8);
    Blynk.virtualWrite(VPIN_BUTTON_8, toggleState_8);
    Serial.println("Switch-8 off");
  }
}  

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
//           case IR_Fan_Up: 
//             if(currSpeed < 4){
//               currSpeed = currSpeed + 1;
//               fanSpeedControl(currSpeed);
//               pref.putInt("Fan", currSpeed);
//               Blynk.virtualWrite(VPIN_BUTTON_FAN, currSpeed);
//             }
//             delay(100);            
//             break;
//           case IR_Fan_Down: 
//             if(currSpeed > 0){
//               currSpeed = currSpeed - 1;
//               fanSpeedControl(currSpeed);
//               pref.putInt("Fan", currSpeed);
//               Blynk.virtualWrite(VPIN_BUTTON_FAN, currSpeed);
//             }
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

void all_SwitchOff(){
  toggleState_1 = 0; digitalWrite(RelayPin1, HIGH); pref.putBool("Relay1", toggleState_1); Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1); delay(100);
  toggleState_2 = 0; digitalWrite(RelayPin2, HIGH); pref.putBool("Relay2", toggleState_2); Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2); delay(100);
  toggleState_3 = 0; digitalWrite(RelayPin3, HIGH); pref.putBool("Relay3", toggleState_3); Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3); delay(100);
  toggleState_4 = 0; digitalWrite(RelayPin4, HIGH); pref.putBool("Relay4", toggleState_4); Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4); delay(100);
  toggleState_5 = 0; digitalWrite(RelayPin5, HIGH); pref.putBool("Relay5", toggleState_5); Blynk.virtualWrite(VPIN_BUTTON_5, toggleState_5); delay(100);
  toggleState_6 = 0; digitalWrite(RelayPin6, HIGH); pref.putBool("Relay6", toggleState_6); Blynk.virtualWrite(VPIN_BUTTON_6, toggleState_6); delay(100);
  toggleState_7 = 0; digitalWrite(RelayPin7, HIGH); pref.putBool("Relay7", toggleState_7); Blynk.virtualWrite(VPIN_BUTTON_7, toggleState_7); delay(100);
  toggleState_8 = 0; digitalWrite(RelayPin8, HIGH); pref.putBool("Relay8", toggleState_8); Blynk.virtualWrite(VPIN_BUTTON_8, toggleState_8); delay(100);
}

void all_SwitchOn(){
  
  toggleState_1 = 1; digitalWrite(RelayPin1, LOW); pref.putBool("Relay1", toggleState_1); Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1); delay(100);
  toggleState_2 = 1; digitalWrite(RelayPin2, LOW); pref.putBool("Relay2", toggleState_2); Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2); delay(100);
  toggleState_3 = 1; digitalWrite(RelayPin3, LOW); pref.putBool("Relay3", toggleState_3); Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3); delay(100);
  toggleState_4 = 1; digitalWrite(RelayPin4, LOW); pref.putBool("Relay4", toggleState_4); Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4); delay(100);
  toggleState_5 = 1; digitalWrite(RelayPin5, LOW); pref.putBool("Relay5", toggleState_5); Blynk.virtualWrite(VPIN_BUTTON_5, toggleState_5); delay(100);
  toggleState_6 = 1; digitalWrite(RelayPin6, LOW); pref.putBool("Relay6", toggleState_6); Blynk.virtualWrite(VPIN_BUTTON_6, toggleState_6); delay(100);
  toggleState_7 = 1; digitalWrite(RelayPin7, LOW); pref.putBool("Relay7", toggleState_7); Blynk.virtualWrite(VPIN_BUTTON_7, toggleState_7); delay(100);
  toggleState_8 = 1; digitalWrite(RelayPin8, LOW); pref.putBool("Relay8", toggleState_8); Blynk.virtualWrite(VPIN_BUTTON_8, toggleState_8); delay(100);
}

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
  toggleState_5 = pref.getBool("Relay5", 0);
  digitalWrite(RelayPin5, !toggleState_5); 
  Blynk.virtualWrite(VPIN_BUTTON_5, toggleState_5);
  delay(200);
  toggleState_6 = pref.getBool("Relay6", 0);
  digitalWrite(RelayPin6, !toggleState_6); 
  Blynk.virtualWrite(VPIN_BUTTON_6, toggleState_6);
  delay(200);
  toggleState_7 = pref.getBool("Relay7", 0);
  digitalWrite(RelayPin7, !toggleState_7); 
  Blynk.virtualWrite(VPIN_BUTTON_7, toggleState_7);
  delay(200);
  toggleState_8 = pref.getBool("Relay8", 0);
  digitalWrite(RelayPin8, !toggleState_8); 
  Blynk.virtualWrite(VPIN_BUTTON_8, toggleState_8);
  delay(200);
  // currSpeed = pref.getInt("Fan", 0);
  // fanSpeedControl(currSpeed); 
  // Blynk.virtualWrite(VPIN_BUTTON_4, currSpeed);
  // delay(200);  
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);

  //Open namespace in read-write mode
  pref.begin("Relay_State", false);
  
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);
  pinMode(RelayPin6, OUTPUT);
  pinMode(RelayPin7, OUTPUT);
  pinMode(RelayPin8, OUTPUT);
  // pinMode(FanRelay1, OUTPUT);
  // pinMode(FanRelay2, OUTPUT);
  // pinMode(FanRelay3, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);
  pinMode(SwitchPin5, INPUT_PULLUP);
  pinMode(SwitchPin6, INPUT_PULLUP);
  pinMode(SwitchPin7, INPUT_PULLUP);
  pinMode(SwitchPin8, INPUT_PULLUP);
  // pinMode(FanSwitch1, INPUT_PULLUP);
  // pinMode(FanSwitch2, INPUT_PULLUP);
  // pinMode(FanSwitch3, INPUT_PULLUP);
  // pinMode(FanSwitch4, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !toggleState_1);
  digitalWrite(RelayPin2, !toggleState_2);
  digitalWrite(RelayPin3, !toggleState_3);
  digitalWrite(RelayPin4, !toggleState_4);
  digitalWrite(RelayPin5, !toggleState_5);
  digitalWrite(RelayPin6, !toggleState_6);
  digitalWrite(RelayPin7, !toggleState_7);
  digitalWrite(RelayPin8, !toggleState_8);
  // digitalWrite(FanRelay1, HIGH);
  // digitalWrite(FanRelay2, HIGH);
  // digitalWrite(FanRelay3, HIGH);

  digitalWrite(wifiLed, LOW);

  // irrecv.enableIRIn(); // Enabling IR sensor
  // dht.begin();    // Enabling DHT sensor

  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  // timer.setInterval(1000L, sendSensor); // Sending Sensor Data to Blynk Cloud every 1 second
  Blynk.config(auth);
  delay(1000);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
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

  getRelayState(); //fetch data from NVS Flash Memory
//  delay(1000);
}

void loop()
{  
  Blynk.run();
  timer.run(); // Initiates SimpleTimer

  ArduinoOTA.handle();

  manual_control();
  
  // fanRegularor(); //Control Fan Speed

  // ir_remote(); //IR remote Control
}

// void fanRegularor(){
//   if (digitalRead(FanSwitch1) == HIGH && digitalRead(FanSwitch2) == HIGH && digitalRead(FanSwitch3) == HIGH && digitalRead(FanSwitch4) == HIGH  && fanSpeed_0 == LOW)
//   {
//     currSpeed = 0;
//     fanSpeedControl(currSpeed);
//     pref.putInt("Fan", currSpeed);
//     Blynk.virtualWrite(VPIN_BUTTON_FAN, 0);
//     fanSpeed_1 = LOW;
//     fanSpeed_2 = LOW;
//     fanSpeed_3 = LOW;
//     fanSpeed_4 = LOW;
//     fanSpeed_0 = HIGH;
//   }
//   if (digitalRead(FanSwitch1) == LOW && fanSpeed_1 == LOW)
//   {
//     currSpeed = 1;
//     fanSpeedControl(currSpeed);
//     pref.putInt("Fan", currSpeed);
//     Blynk.virtualWrite(VPIN_BUTTON_FAN, 1);
//     fanSpeed_1 = HIGH;
//     fanSpeed_2 = LOW;
//     fanSpeed_3 = LOW;
//     fanSpeed_4 = LOW;
//     fanSpeed_0 = LOW;
//   }
//   if (digitalRead(FanSwitch2) == LOW && digitalRead(FanSwitch3) == HIGH && fanSpeed_2 == LOW)
//   {
//     currSpeed = 2;
//     fanSpeedControl(currSpeed);
//     pref.putInt("Fan", currSpeed);
//     Blynk.virtualWrite(VPIN_BUTTON_FAN, 2);
//     fanSpeed_1 = LOW;
//     fanSpeed_2 = HIGH;
//     fanSpeed_3 = LOW;
//     fanSpeed_4 = LOW;
//     fanSpeed_0 = LOW;
//   }
//   if (digitalRead(FanSwitch2) == LOW && digitalRead(FanSwitch3) == LOW && fanSpeed_3 == LOW)
//   {
//     currSpeed = 3;
//     fanSpeedControl(currSpeed);
//     pref.putInt("Fan", currSpeed);
//     Blynk.virtualWrite(VPIN_BUTTON_FAN, 3);
//     fanSpeed_1 = LOW;
//     fanSpeed_2 = LOW;
//     fanSpeed_3 = HIGH;
//     fanSpeed_4 = LOW;
//     fanSpeed_0 = LOW;
//   }
//   if (digitalRead(FanSwitch4) == LOW  && fanSpeed_4 == LOW)
//   {
//     currSpeed = 4;
//     fanSpeedControl(currSpeed);
//     pref.putInt("Fan", currSpeed);
//     Blynk.virtualWrite(VPIN_BUTTON_FAN, 4);
//     fanSpeed_1 = LOW;
//     fanSpeed_2 = LOW;
//     fanSpeed_3 = LOW;
//     fanSpeed_4 = HIGH;
//     fanSpeed_0 = LOW;
//   }
// }

// void fanSpeedControl(int fanSpeed){
//   switch(fanSpeed){
//       case 0:
//         digitalWrite(FanRelay1, HIGH);
//         digitalWrite(FanRelay2, HIGH);
//         digitalWrite(FanRelay3, HIGH);        
//       break;
//       case 1:
//         digitalWrite(FanRelay1, HIGH);
//         digitalWrite(FanRelay2, HIGH);
//         digitalWrite(FanRelay3, HIGH);
//         delay(500);
//         digitalWrite(FanRelay1, LOW);
//       break;
//       case 2:
//         digitalWrite(FanRelay1, HIGH);
//         digitalWrite(FanRelay2, HIGH);
//         digitalWrite(FanRelay3, HIGH);
//         delay(500);
//         digitalWrite(FanRelay2, LOW);
//       break;
//       case 3:
//         digitalWrite(FanRelay1, HIGH);
//         digitalWrite(FanRelay2, HIGH);
//         digitalWrite(FanRelay3, HIGH);
//         delay(500);
//         digitalWrite(FanRelay1, LOW);
//         digitalWrite(FanRelay2, LOW);
//       break;
//       case 4:
//         digitalWrite(FanRelay1, HIGH);
//         digitalWrite(FanRelay2, HIGH);
//         digitalWrite(FanRelay3, HIGH);
//         delay(500);
//         digitalWrite(FanRelay3, LOW);
//       break;          
//       default : break;         
//     } 
// }
