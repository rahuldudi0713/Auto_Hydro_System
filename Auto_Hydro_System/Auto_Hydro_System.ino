// Code Presented and build by Hacki Tech
//Project Name :- Auto_Hydro_System_1.0
//Developer :-    Rahul Dudi
//Presentation Video :- YT- Epic Life Shots
//Cyber Sec Channel  :- YT- Hacki Tech
//The Code Is Editable and for further help Comment on youtube video i will connect you.



// Important libraries
//Change Template ID and Template Name according to your Blynk Template
#define BLYNK_TEMPLATE_ID "TMPLATE ID"           
#define BLYNK_TEMPLATE_NAME "TEMPLATE NAME"
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>            //Esp32 Library
#include <WiFiClient.h>                  //Library To connect with wifi
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "BLYNK AUTH TOKEN";  //Enter your Blynk Auth token
char ssid[] = "SSID";                        //Enter your WIFI SSID
char pass[] = "PASSWORD";                         //Enter your WIFI Password

// LCD PINS
// SDA PIN = 21
// SCL PIN = 22
int Red   = 16;
int Green  = 4;

#define soil 34                                      //Pin-34 Soil Moisture Sensor
#define RELAY_PIN_1       17                         //16 Relay
#define RELAY_PIN_2       5                          //17 Relay
#define RELAY_PIN_3       18                         //18 Relay
#define RELAY_PIN_4       19                         //18 Relay

#define wifiLed           2
#define PUSH_BUTTON_1     26                         //26 Button
#define PUSH_BUTTON_2     25                         //25 Button
Servo servo;

#define VPIN_BUTTON_1    V5   
#define VPIN_BUTTON_2    V6 
#define VPIN_BUTTON_3    V7 

int toggleState_1 = 1;                               //Define integer to rember the toggle state for relay 1
int toggleState_2 = 1;                               //Define integer to rember the toggle state for relay 2
int toggleState_3 = 1;                               //Define integer to rember the toggle state for relay 3
int toggleState_4 = 1;                               //Define integer to rember the toggle state for relay 4


int wifiFlag = 0;


BlynkTimer timer;

void relayOnOff(int relay){
  switch(relay){
    case 1:
          if(toggleState_1 == 1){
            digitalWrite(RELAY_PIN_1, LOW); //turn on relay 1
            toggleState_1 = 0;
          }
          else{
            digitalWrite(RELAY_PIN_1, HIGH); //turn off relay 1
            toggleState_1 = 1;
          }
          delay(100);
    break;
    case 2:
          if(toggleState_2 == 1){
            digitalWrite(RELAY_PIN_2, LOW);
            toggleState_2 = 0;
          }
          else{
            digitalWrite(RELAY_PIN_2, HIGH);
            toggleState_2 = 1;
          }
          delay(100);
    break;
    case 3:
          if(toggleState_3 == 1){
            digitalWrite(RELAY_PIN_3, LOW);
            toggleState_3 = 0;
          }
          else{
            digitalWrite(RELAY_PIN_3, HIGH);
            toggleState_3 = 1;
          }
          delay(100);
    break;
    case 4:
          if(toggleState_4 == 1){
            digitalWrite(RELAY_PIN_4, LOW);
            toggleState_4 = 0;
          }
          else{
            digitalWrite(RELAY_PIN_4, HIGH);
            toggleState_4 = 1;
          }
          delay(100);
    break;
    default : break; 
  }
}

void with_internet(){
  if (digitalRead(PUSH_BUTTON_1) == LOW){
    delay(200);
    relayOnOff(3);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_1);
  }
  else if (digitalRead(PUSH_BUTTON_2) == LOW){
    delay(200);
    relayOnOff(4);
  }
}

void without_internet(){
  if (digitalRead(PUSH_BUTTON_1) == LOW){
    delay(200);
    relayOnOff(1);
  }
  if (digitalRead(PUSH_BUTTON_2) == LOW){
    delay(200);
    relayOnOff(2);
  }
}


BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
  Blynk.syncVirtual(VPIN_BUTTON_3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V3);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RELAY_PIN_1, toggleState_1);
}
BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RELAY_PIN_2, toggleState_2);
}
BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RELAY_PIN_3, toggleState_3);
}


void checkBlynkStatus(){
  bool isconnected = Blynk.connected();
  if(isconnected == false){
    wifiFlag = 1;
    digitalWrite(wifiLed, LOW);
  }
  if(isconnected == true){
    wifiFlag = 0;
    digitalWrite(wifiLed, HIGH);
  }
}


void setup() {
  Serial.begin(115200);
  lcd.init();                       // Initialize the LCD
  lcd.backlight();                  // Turn on the backlight
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("Connecting to");        
  lcd.setCursor(0, 1);  
  lcd.print("Wifi...");   
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
  delay(1000);    
  }
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("   Auto-Hydro");

  pinMode(wifiLed, OUTPUT);
  pinMode(Red,OUTPUT);
  pinMode(Green,OUTPUT);
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(RELAY_PIN_4, OUTPUT);
  
  digitalWrite(RELAY_PIN_1, toggleState_1);
  digitalWrite(RELAY_PIN_2, toggleState_2);
  digitalWrite(RELAY_PIN_3, toggleState_3);
  digitalWrite(RELAY_PIN_4, toggleState_4);

  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
  pinMode(PUSH_BUTTON_2, INPUT_PULLUP);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080);

  timer.setInterval(3000L, checkBlynkStatus);
  timer.setInterval(500L, soilMoistureSensor);
  
}

//Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soil);
  value = map(value, 4095, 0, 100, 0);
  value = (value - 100) * -1;
  Blynk.virtualWrite(V4, value);
  lcd.setCursor(0, 1);  
  lcd.print("Soil ");        
  lcd.setCursor(5, 1);  
  lcd.print(value);  
  Serial.print("Soil Moisture: ");
  Serial.print(value);
  Serial.println("%");  
  if(value > 50){
    digitalWrite(Red, LOW);   
    digitalWrite(Green, HIGH);
  }
  else if (value < 50){
    digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW);

  }
}

void loop()
{  
  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {           
    delay(1000);
   }

  }  
  else
  {
    lcd.setCursor(0, 0);  
    lcd.print("   Auto-Hydro");        
    Blynk.run();                                  //Run the Blynk library
  }
  
  timer.run();                                    //Run the Blynk timer
  if(wifiFlag == 0)
    with_internet();
  else
    without_internet();    

}
